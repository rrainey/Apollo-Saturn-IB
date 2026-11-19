/*
 * LVDA_LVDC.c — obliq-2 native atomic entry points for the LVDC
 * rewrite.
 *
 * Thin dispatcher. The real EDD-anchored work lives in the per-
 * section modules (lvdc_nav, lvdc_frames, lvdc_quant, and the Phase
 * C+ additions). This file owns only:
 *   - the public ABI contract (LVDA_LVDC_* entry points)
 *   - the mapping from obliq-2 inputs/outputs to the internal
 *     rewrite state
 *   - the per-step scheduling (per-tick guard + BML gate)
 *
 * See docs/LVDC_REWRITE_PLAN.md for scope and milestones.
 * See docs/LVDC_RK4_REENTRY_REPORT.md for the obliq-2 native
 * atomic scheduling contract.
 *
 * Phase B.4 scope: nav path (EDD §3.4, §4.2, §4.3) is wired. The
 * dispatcher publishes R_S_bar_m from the nav state and passes
 * Theta_deg through to the theta_deg output. Guidance outputs
 * (Psi_deg, stage-event discretes, T1_sec) remain zero until
 * Phase C lands lvdc_igm and Phase D lands lvdc_scheduler.
 */

#include "LVDA_LVDC.h"

#include "lvdc_chi_to_psi.h"
#include "lvdc_dac.h"
#include "lvdc_hsl.h"
#include "lvdc_igm.h"
#include "lvdc_internal.h"
#include "lvdc_minor_loop.h"
#include "lvdc_moverf.h"
#include "lvdc_nav.h"
#include "lvdc_params.h"
#include "lvdc_preigm.h"
#include "lvdc_timebase.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Debug trace file. Enabled by setting env var
 * LVDC_IGM_DEBUG_CSV to a path. Written once per BML inside the
 * IGM update block. */
static FILE *g_igm_dbg_fp;

/* The rewrite keeps its own state in a module-local singleton. The
 * obliq-2 native atomic ABI stores sub-system state in the
 * LVDA_LVDC_t struct, but the rewrite does not use its codegen-shaped
 * signals_t / states_t layout. A file-scope static works because the
 * obliq-2 host does not instantiate LVDA_LVDC more than once in the
 * Saturn IB plant.
 *
 * Init resets this cleanly, so there is no cross-run state leak.
 * Per the RK4 report, statics survive across RK4 stages (good) and
 * across LVDC restarts (hence the explicit reset in init). */
static lvdc_state_t             g_lvdc;
static lvdc_params_t            g_params;
static lvdc_minor_loop_state_t  g_minor_loop;
static lvdc_dac_state_t         g_dac;

/* IGM state (Phase C.3f-2/5). `g_igm_enabled` latches true the
 * first BML on or after T1 >= params.igm_enable_from_T1_sec (a
 * coarse surrogate for T3+25 until Phase D wires up the time
 * bases). At the latch, presets are rebuilt from g_params and
 * the state is re-initialised so IGM starts from a clean slate.
 *
 * The IGM is driven once per BML inside the BML gate; the output
 * chi_y, chi_z are held in g_chi_igm_rad for the 25 Hz minor
 * loop to consume. `g_igm_valid` is set to true the first BML
 * IGM returns a non-error result; before that the dispatcher
 * passes through preigm commands to avoid commanding chi = 0. */
static lvdc_igm_state_t         g_igm;
static lvdc_igm_presets_t       g_igm_presets;
static lvdc_moverf_state_t      g_moverf;
static bool                     g_igm_enabled;
static bool                     g_igm_valid;
static double                   g_chi_igm_rad[2];  /* [0]=pitch, [1]=yaw */

/* Time-base + event-discrete state. Advanced every tick inside
 * compute_outputs; read by publish_outputs. */
static lvdc_timebase_state_t    g_timebase;

/* HSL state (EDD §4.4.2.4). Stepped each BML once IGM is enabled;
 * fires the cutoff edge into the time-base state machine. */
static lvdc_hsl_state_t         g_hsl;
static bool                     g_hsl_cutoff_edge_pending;

/* Latch state for the liftoff edge. Lives outside g_lvdc because
 * g_lvdc.sensor.liftoff_latched is the "has it ever fired" flag;
 * this tracks the previous tick's input for rising-edge detection.
 *
 * Note on GRR vs. liftoff: on a real Saturn IB, the PIPA zero-reset
 * (EDD §4.2.3) and the X_0/V_0 seed (EDD §3.4) fire at Guidance
 * Reference Release — an LVDC-internal event scheduled by the
 * countdown program, nominally 17 s before liftoff. The obliq-2
 * parent model does not provide a separate GRR discrete, so the
 * rewrite latches on the bLiftoff rising edge instead, matching
 * the current hand-edited snapshot. The ~17 s of pad-held PIPA
 * integration is lost from V_m, but the matching integral of G_S
 * is also absent, so Eq. 4.3.5 still yields the correct inertial
 * velocity from liftoff onward. See lvdc_internal.h for the
 * balance derivation and the Phase D deferred action. */
static bool s_prev_bLiftoff;

/* ---------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------ */

/* Return the number of fundamental ticks between BML reads at the
 * host's dt. Guards against dt==0 at init. */
static unsigned long long bml_ticks_per_BML(double dt) {
    if (dt <= 0.0) return 0ULL;
    const double n = round(1.6 / dt);
    return (n > 0.0) ? (unsigned long long)n : 0ULL;
}

/* Return the number of fundamental ticks between minor loops. Per
 * EDD §8.1 the boost minor-loop period is 40 ms. */
static unsigned long long minor_loop_ticks(double dt) {
    if (dt <= 0.0) return 0ULL;
    const double n = round(0.04 / dt);
    return (n > 0.0) ? (unsigned long long)n : 0ULL;
}

/* Convert a 3-vector from radians to degrees in place. */
static void rad_to_deg_vec(const double in_rad[3], double out_deg[3]) {
    const double k = 180.0 / M_PI;
    for (int i = 0; i < 3; i++) {
        out_deg[i] = in_rad[i] * k;
    }
}

/* Convert a 3-vector from degrees to radians. */
static void deg_to_rad_vec(const double in_deg[3], double out_rad[3]) {
    const double k = M_PI / 180.0;
    for (int i = 0; i < 3; i++) {
        out_rad[i] = in_deg[i] * k;
    }
}

/* Publish nav state and the attitude command to the obliq-2 ABI
 * outputs. Called every compute_outputs invocation (4× per major
 * step); safe to re-run because all state is held in module-local
 * singletons that only update once per tick. */
static void publish_outputs(LVDA_LVDC_t *model) {
    /* EDD §4.3.3: R_S_bar_m is the plumbline-frame position. The
     * published value is updated only on BML ticks inside
     * lvdc_nav_bml_step, so between BMLs the output stays constant.
     * Matches the current hand-edited body's Product18 publication
     * timing, which gates the write by the BML sample-tick modulo. */
    for (int i = 0; i < 3; i++) {
        model->outputs.R_S_bar_m[i] = g_lvdc.nav.published_R_S_bar_m[i];
    }

    /* Pass-through of the ST-124M resolver readings. The parent FCC
     * reads this port; the current snapshot does the same memcpy at
     * the end of its compute. */
    memcpy(model->outputs.theta_deg,
           model->inputs.Theta_deg,
           sizeof(model->outputs.theta_deg));

    /* Attitude error command (EDD §8.2.3). g_dac.psi_rad holds the
     * last DAC output in radians; the obliq-2 boundary is in
     * degrees. */
    rad_to_deg_vec(g_dac.psi_rad, model->outputs.Psi_deg);

    /* T1_sec: elapsed time since liftoff, read for downstream
     * consumers. */
    model->outputs.T1_sec = g_lvdc.tb1.T1_sec;

    /* Stage-event discretes derived from the time-base state
     * machine (Phase D.1-D.2). See lvdc_timebase.h for the
     * thresholds. Refined post-cutoff behaviour arrives with
     * HSL (Eqs. 4.4.88-94) in Phase D.5. */
    lvdc_timebase_discretes_t disc;
    lvdc_timebase_discretes(&g_timebase, model->time, &disc);
    model->outputs.bSIVEngineStart     = disc.bSIVEngineStart;
    model->outputs.bSIVCutoff          = disc.bSIVCutoff;
    model->outputs.bIECO               = disc.bIECO;
    model->outputs.bOECO               = disc.bOECO;
    model->outputs.bSimulinkSIVBEnable = disc.bSimulinkSIVBEnable;
    model->outputs.bUllageMotorStart   = disc.bUllageMotorStart;
    model->outputs.bSIVBurn            = disc.bSIVBurn;
    model->outputs.bSIVCoast           = disc.bSIVCoast;
    model->outputs.bSwitchpoin5        = disc.bSwitchpoin5;
    model->outputs.bSIBStageSep        = disc.bSIBStageSep;
}

/* Run the attitude-command pipeline once. See lvdc_preigm,
 * lvdc_minor_loop, lvdc_chi_to_psi, and lvdc_dac for EDD section
 * references.
 *
 * The caller gates the invocation to the minor-loop rate (40 ms per
 * EDD §8.1 during boost). `dt_minor_sec` is the elapsed time since
 * the previous invocation, in seconds. */
static void step_attitude_pipeline(const LVDA_LVDC_t *model,
                                   double dt_minor_sec) {
    double chi_cmd_rad[3];
    double chi_prime_rad[3];
    double theta_prime_rad[3];
    double psi_body_rad[3];
    double psi_dac_rad[3];

    /* Pre-IGM time-tilt (EDD §4.4.1, TMX Appendix B).
     * Before liftoff, T1_sec is zero and lvdc_preigm_chi_cmd returns
     * the pad-hold values. */
    lvdc_preigm_chi_cmd(&g_params, g_lvdc.tb1.T1_sec, chi_cmd_rad);

    /* IGM takeover (EDD §4.4.2, Eqs. 4.4.86-87). Once g_igm_enabled
     * latches, the pitch (chi_y, index 1) and yaw (chi_z, index 2)
     * commands come from the BML-rate IGM compute held in
     * g_chi_igm_rad. Roll (chi_x, index 0) stays with preigm per
     * EDD §4.4.1 — IGM does not command roll. */
    if (g_igm_enabled && g_igm_valid) {
        chi_cmd_rad[1] = g_chi_igm_rad[0];  /* pitch */
        chi_cmd_rad[2] = g_chi_igm_rad[1];  /* yaw   */
    }

    /* Minor-loop rate-limit ramp (EDD §8.3.1). */
    lvdc_minor_loop_step(&g_minor_loop, &g_params,
                         chi_cmd_rad, dt_minor_sec, chi_prime_rad);

    /* Gimbal-to-body transform (EDD §8.3.2 + Eq. 8.2.2). */
    deg_to_rad_vec(model->inputs.Theta_deg, theta_prime_rad);
    lvdc_chi_to_psi_transform(chi_prime_rad, theta_prime_rad,
                              psi_body_rad);

    /* Rate limit, saturate, quantize (EDD §8.2.3). */
    lvdc_dac_step(&g_dac, &g_params, psi_body_rad, dt_minor_sec,
                  psi_dac_rad);
}

/* ---------------------------------------------------------------------
 * ABI entry points
 * ------------------------------------------------------------------ */

void LVDA_LVDC_init(LVDA_LVDC_t* model) {
    memset(model, 0, sizeof(*model));
    model->enabled = 1;

    lvdc_nav_reset(&g_lvdc);
    lvdc_minor_loop_reset(&g_minor_loop);
    lvdc_dac_reset(&g_dac);
    g_params = lvdc_params_as205();
    s_prev_bLiftoff = false;

    /* IGM: build presets from params and init with those. The
     * state is re-initialised at the IGM-enable latch so this
     * initial call is just for a well-defined struct. (M/F)
     * smoothing primes with (M/F)_0 and starts collecting the
     * instantaneous ring at the first BML after liftoff. */
    lvdc_igm_presets_from_params(&g_params, &g_igm_presets);
    lvdc_igm_init(&g_igm, &g_igm_presets);
    lvdc_moverf_prime(&g_moverf, g_params.MoverF_0_s2pm);
    g_igm_enabled = false;
    g_igm_valid   = false;
    g_chi_igm_rad[0] = 0.0;
    g_chi_igm_rad[1] = 0.0;

    lvdc_timebase_reset(&g_timebase);
    lvdc_hsl_reset(&g_hsl);
    g_hsl_cutoff_edge_pending = false;

    /* Open the debug trace if requested. */
    if (g_igm_dbg_fp) { fclose(g_igm_dbg_fp); g_igm_dbg_fp = NULL; }
    const char *dbg_path = getenv("LVDC_IGM_DEBUG_CSV");
    if (dbg_path && *dbg_path) {
        g_igm_dbg_fp = fopen(dbg_path, "w");
        if (g_igm_dbg_fp) {
            fprintf(g_igm_dbg_fp,
                "t_sec,mode,T_1i,T_3i,P_C,tau_1,tau_3,"
                "L_1,J_1,S_1,P_1,Q_1,U_1,L_prime_3,J_prime_3,L_prime_y,"
                "moverf_s,igm_rc,V_mps,phi_i,phi_iT,phi_T,delta_2,"
                "dL_3,L_3,dT_3_prime,"
                "chi_tilde_y,chi_tilde_z,"
                "K_1,K_2,K_3,K_4,"
                "chi_y4,chi_z4,SMCY,SMCZ,"
                "eps_2_active\n");
        }
    }
}

void LVDA_LVDC_compute_outputs(LVDA_LVDC_t* model) {
    if (!model->enabled) {
        return;
    }

    /* Per-tick guard. See docs/LVDC_RK4_REENTRY_REPORT.md: sample_tick
     * is stable across the four RK4 stages of a major step; one-shot
     * work fires on the first call of each new tick. */
    const bool first_call_this_step =
        (model->sample_tick != g_lvdc.last_compute_tick);

    if (first_call_this_step) {
        g_lvdc.last_compute_tick = model->sample_tick;

        /* --- Liftoff latch (EDD §3.4, §4.2.3 — see note above) ---
         * Fire on the rising edge of bLiftoff. The current snapshot
         * does the same at L1378. */
        if (!s_prev_bLiftoff && model->inputs.bLiftoff
                && !g_lvdc.sensor.liftoff_latched) {
            double X_0[3], V_0[3];
            lvdc_nav_compute_initial_conditions(&g_params, X_0, V_0);
            lvdc_nav_liftoff_latch(&g_lvdc,
                                   model->inputs.V_m_bar_mps,
                                   X_0, V_0);
            g_lvdc.tb1.t_liftoff_sec = model->time;
        }
        s_prev_bLiftoff = model->inputs.bLiftoff;

        /* --- Time-base state machine (EDD §4.1) -----------------
         * T1..T4 latched from plant discretes and earlier events.
         * siv_cutoff_edge is driven by the HSL (EDD §4.4.2.4);
         * captured below in the BML gate and consumed here on
         * the following tick. */
        const bool cutoff_edge_this_tick = g_hsl_cutoff_edge_pending;
        g_hsl_cutoff_edge_pending = false;
        lvdc_timebase_step(&g_timebase,
                           model->time,
                           model->inputs.bLiftoff,
                           model->inputs.bSIBPropellantSensorDry,
                           cutoff_edge_this_tick);

        /* Update T1_sec (time since liftoff). Stays at zero before
         * the liftoff edge. */
        if (g_lvdc.sensor.liftoff_latched) {
            g_lvdc.tb1.T1_sec = model->time - g_lvdc.tb1.t_liftoff_sec;
            if (g_lvdc.tb1.T1_sec < 0.0) g_lvdc.tb1.T1_sec = 0.0;
        } else {
            g_lvdc.tb1.T1_sec = 0.0;
        }

        /* --- BML gate (EDD §4.1, §4.2.4, §4.3.1) -----------------
         * Nominal 1.6 s cadence. The sample-tick modulo is the
         * same gate shape the current snapshot uses at L1395. */
        const unsigned long long ticks_per_BML =
            bml_ticks_per_BML(model->dt);
        const bool at_BML =
            (ticks_per_BML != 0ULL)
            && ((model->sample_tick % ticks_per_BML) == 0ULL);

        if (at_BML && g_lvdc.sensor.liftoff_latched) {
            lvdc_nav_bml_step(&g_lvdc,
                              &g_params,
                              model->inputs.V_m_bar_mps,
                              model->time);

            /* Mirror nav-side state into the plant shim globals
             * so SaturnIBPlantObliq's --igm-trace emission has
             * live values. See lvdc_plant_shim.c. */
            extern double s_V_Sg[3];
            extern double s_G_S_prev[3];
            for (int i = 0; i < 3; i++) {
                s_V_Sg[i]     = g_lvdc.nav.V_Sg[i];
                s_G_S_prev[i] = g_lvdc.nav.G_S_prev[i];
            }

            /* (M/F)_S smoothing per EDD §4.2.5 Eq. 4.2.11. Runs
             * every BML once liftoff has latched. */
            const double moverf_s =
                lvdc_moverf_step(&g_moverf,
                                 model->inputs.V_m_bar_mps,
                                 g_lvdc.sensor.dT_BML_sec,
                                 g_params.MFK,
                                 g_params.MoverF_s_limit_s2pm,
                                 g_params.MoverF_s_rate_limit_per_BML);

            /* IGM enable latch (Phase D fallback: T1-based). When
             * the Phase D time-base state machine lands, swap this
             * for a T3-based enable (T3 + igm_enable_from_T3_sec). */
            if (!g_igm_enabled
                && g_params.igm_enable_from_T1_sec > 0.0
                && g_lvdc.tb1.T1_sec >= g_params.igm_enable_from_T1_sec) {
                lvdc_igm_presets_from_params(&g_params, &g_igm_presets);
                lvdc_igm_init(&g_igm, &g_igm_presets);
                /* EDD §4.2.6 — re-prime the (M/F)_S filter with
                 * (M/F)_0 at IGM initiation ("initialized at T0+0").
                 * Continuous run from liftoff accumulates S-IB-phase
                 * samples through staging and the IIR filter swings
                 * negative during the (M/F) step from 0.02 to 0.125
                 * at S-IVB ignition — producing invalid tau_1 for
                 * ~37 seconds after IGM enable without a re-prime. */
                lvdc_moverf_prime(&g_moverf, g_params.MoverF_0_s2pm);
                g_igm_enabled = true;
            }

            /* IGM compute (once per BML when enabled). Output
             * chi_y, chi_z are latched into g_chi_igm_rad for the
             * minor loop. */
            if (g_igm_enabled) {
                double V_S[3];
                lvdc_nav_compute_V_S(&g_lvdc, V_S);
                const double V_mag = lvdc_nav_compute_V_S_magnitude(&g_lvdc);

                double chi_y = 0.0, chi_z = 0.0;
                const int rc = lvdc_igm_update(
                    &g_igm,
                    g_lvdc.sensor.dT_BML_sec,
                    V_mag,
                    moverf_s,
                    g_lvdc.nav.X_S,
                    V_S,
                    g_lvdc.nav.G_S,
                    g_lvdc.sensor.FoverM_axis[0],
                    g_lvdc.sensor.FoverM_axis[1],
                    g_lvdc.sensor.FoverM_axis[2],
                    &chi_y, &chi_z);
                if (rc == 0) {
                    g_chi_igm_rad[0] = chi_y;
                    g_chi_igm_rad[1] = chi_z;
                    g_igm_valid = true;
                }
                /* rc != 0 means a domain error; the pipeline below
                 * falls back to preigm until g_igm_valid latches.
                 * Once valid, we hold the last good chi on any
                 * subsequent transient error. */

                /* Debug trace of IGM internals. */
                if (g_igm_dbg_fp) {
                    fprintf(g_igm_dbg_fp,
                        "%.4f,%d,%.4f,%.4f,%.4f,"
                        "%.6g,%.6g,"
                        "%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,"
                        "%.6g,%.6g,%.6g,"
                        "%.6g,%d,%.6g,%.9g,%.9g,%.9g,%.6g,"
                        "%.6g,%.6g,%.6g,"
                        "%.9g,%.9g,"
                        "%.9g,%.9g,%.9g,%.9g,"
                        "%.9g,%.9g,%.9g,%.9g,%d\n",
                        model->time,
                        (int)g_igm.mode,
                        g_igm.T_1i_sec, g_igm.T_3i_sec, g_igm.P_C_sec,
                        g_igm.trace.tau_1, g_igm.trace.tau_3,
                        g_igm.trace.L_1, g_igm.trace.J_1, g_igm.trace.S_1,
                        g_igm.trace.P_1, g_igm.trace.Q_1, g_igm.trace.U_1,
                        g_igm.trace.L_prime_3, g_igm.trace.J_prime_3,
                        g_igm.trace.L_prime_y,
                        moverf_s, rc, V_mag,
                        g_igm.trace.phi_i, g_igm.trace.phi_iT, g_igm.trace.phi_T,
                        g_igm.trace.delta_2,
                        g_igm.trace.dL_3, g_igm.trace.L_3,
                        g_igm.trace.dT_3_prime,
                        g_igm.trace.chi_tilde_y, g_igm.trace.chi_tilde_z,
                        g_igm.trace.K_1, g_igm.trace.K_2,
                        g_igm.trace.K_3, g_igm.trace.K_4,
                        g_igm.chi_y4_rad, g_igm.chi_z4_rad,
                        g_igm.smc.SMCY, g_igm.smc.SMCZ,
                        g_igm.trace.eps_2_active);
                }

                /* HSL BML pass (EDD §4.4.2.4). Runs Eqs. 4.4.88-94
                 * once per BML to maintain the 3-sample V history,
                 * compute ΔT_b, check entry, and refine the
                 * predicted T_3i. The actual cutoff edge is
                 * detected in the minor-loop pass below. */
                lvdc_hsl_bml_step(&g_hsl,
                                  model->time,
                                  V_mag,
                                  g_igm.T_3i_sec,
                                  g_params.T_HSL_sec,
                                  g_params.V_GRD_mps,
                                  g_params.BN_1_sec,
                                  g_params.target_V_T_mps,
                                  g_params.DeltaV_b_mps);
            }
        }

        /* --- Attitude command pipeline --------------------------
         * Runs once per minor loop (40 ms per EDD §8.1 during
         * boost). Between minor loops the DAC output is latched. */
        const unsigned long long minor_ticks =
            minor_loop_ticks(model->dt);
        const bool at_minor =
            (minor_ticks != 0ULL)
            && ((model->sample_tick % minor_ticks) == 0ULL);
        if (at_minor) {
            const double dt_minor = (double)minor_ticks * model->dt;
            step_attitude_pipeline(model, dt_minor);

            /* HSL minor-loop pass (EDD §4.4.2.4 ¶7). Decrement
             * T_3i_hsl by 40 ms and fire cutoff when it drops
             * below 60 ms. */
            if (lvdc_hsl_minor_step(&g_hsl, dt_minor)) {
                g_hsl_cutoff_edge_pending = true;
                lvdc_igm_cutoff(&g_igm);
            }
        }
    }

    /* Always re-publish outputs. Safe under RK4 re-entry — the four
     * calls per major step see the same g_lvdc state and produce
     * the same outputs. */
    publish_outputs(model);
}

void LVDA_LVDC_update_memory_states(LVDA_LVDC_t* model) {
    if (!model->enabled) {
        return;
    }

    /* Called exactly once per major step, after the RK4 combine.
     * Phase B.4: nav state has already advanced inside
     * compute_outputs's per-tick-guard scope; no per-step bookkeeping
     * is required here yet. Future work (timebases, IGM mode machine)
     * lives here. */
    g_lvdc.last_memory_tick = model->sample_tick;
}

void LVDA_LVDC_evaluate_enable_states(LVDA_LVDC_t* model) {
    if (!model->enabled) {
        return;
    }

    /* The old snapshot writes dozens of nested enable flags here for
     * its codegen-shaped enable sub-system scopes. The rewrite does
     * not use those flags; it schedules by time and mode directly.
     * No-op. */
}
