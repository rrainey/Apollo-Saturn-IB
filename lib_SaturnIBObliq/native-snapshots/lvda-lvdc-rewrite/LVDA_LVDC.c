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

#include "lvdc_internal.h"
#include "lvdc_nav.h"
#include "lvdc_params.h"

#include <math.h>
#include <string.h>

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
static lvdc_state_t  g_lvdc;
static lvdc_params_t g_params;

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

/* Publish nav state to the obliq-2 ABI outputs. Called every
 * compute_outputs invocation (4× per major step); safe to re-run. */
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

    /* Phase C+ outputs. Zero until lvdc_igm (guidance) and
     * lvdc_scheduler (T0..T4) land. */
    model->outputs.Psi_deg[0] = 0.0;
    model->outputs.Psi_deg[1] = 0.0;
    model->outputs.Psi_deg[2] = 0.0;

    model->outputs.T1_sec = 0.0;

    model->outputs.bSIVEngineStart     = false;
    model->outputs.bSIVCutoff          = false;
    model->outputs.bIECO               = false;
    model->outputs.bOECO               = false;
    model->outputs.bSimulinkSIVBEnable = false;
    model->outputs.bUllageMotorStart   = false;
    model->outputs.bSIVBurn            = false;
    model->outputs.bSIVCoast           = false;
    model->outputs.bSwitchpoin5        = false;
    model->outputs.bSIBStageSep        = false;
}

/* ---------------------------------------------------------------------
 * ABI entry points
 * ------------------------------------------------------------------ */

void LVDA_LVDC_init(LVDA_LVDC_t* model) {
    memset(model, 0, sizeof(*model));
    model->enabled = 1;

    lvdc_nav_reset(&g_lvdc);
    g_params = lvdc_params_as205();
    s_prev_bLiftoff = false;
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
        }
        s_prev_bLiftoff = model->inputs.bLiftoff;

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
