/*
 * lvdc_igm.h — IGM mode state machine and per-BML orchestrator.
 *
 * This layer wraps the §4.4.2 equation blocks (phasing,
 * intermediate parameters, steering, position correction, output)
 * behind one API that the dispatcher calls once per BML. It owns
 * the mode state, the SMC integrator state, and the slow-moving
 * quantities (T_1i, T_3i, P_C) that persist across cycles.
 *
 * EDD narrative (§4.4.2 pages 33-37) defines four IGM phases:
 *
 *   PHASE1                : S-IVB first-phase burn, pre-EMRC.
 *                            tau_1 from Eq. 4.4.7, T_1i decrements
 *                            per Eq. 4.4.8. Phase-3 equations run
 *                            with their presets but T_3i is the
 *                            fixed terminal remaining time.
 *   PHASE2_ARTIFICIAL_TAU : after EMRC, during the smoothed-tau
 *                            transition. tau_3 is the Eq. 4.4.14
 *                            blend between tau_3K and tau_3M.
 *                            P_C accumulates per Eq. 4.4.15.
 *   PHASE2_REAL_TAU       : after P_C >= P_CMR, tau_3 is the
 *                            pure Eq. 4.4.16 form. T_3i decrements
 *                            per Eq. 4.4.17.
 *   POST_CUTOFF           : S-IVB cutoff latched. Last chi held.
 *
 * Mode transitions are triggered by external events:
 *   lvdc_igm_emrc()        : PHASE1 -> PHASE2_ARTIFICIAL_TAU.
 *   automatic              : PHASE2_ARTIFICIAL_TAU -> PHASE2_REAL_TAU
 *                            when P_C >= P_CMR.
 *   lvdc_igm_cutoff()      : any -> POST_CUTOFF.
 *
 * The caller is responsible for the external triggers (EMRC,
 * cutoff). The dispatcher ties them to time-base events or to
 * S-IVB engine state.
 *
 * The HSL (high-speed loop, Eqs. 4.4.88-94) for S-IVB cutoff
 * timing is NOT implemented here. It is a separate layer that
 * runs during the last ~10 s of boost.
 *
 * See lvdc_igm_position_correction.h for the ANGLE-UNIT CONVENTION
 * note that covers the LVDC half-revolution scaling.
 */

#ifndef LVDC_IGM_H
#define LVDC_IGM_H

#include "lvdc_igm_output.h"  /* lvdc_igm_smc_state_t */
#include "lvdc_params.h"     /* lvdc_params_t */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LVDC_IGM_MODE_PHASE1 = 0,
    LVDC_IGM_MODE_PHASE2_ARTIFICIAL_TAU,
    LVDC_IGM_MODE_PHASE2_REAL_TAU,
    LVDC_IGM_MODE_POST_CUTOFF
} lvdc_igm_mode_t;

/* All one-shot and cycle-constant quantities the IGM needs. Loaded
 * from TMX App B / EDD §14 presets at IGM enable. */
typedef struct {
    /* Exhaust velocities (m/s), TMX App B. */
    double V_ex1_mps;
    double V_ex3_mps;

    /* Time-to-go initial values (s). */
    double T_1i_initial_sec;
    double T_3i_initial_sec;

    /* Phase-1 tau values latched at EMRC (s). Eqs. 4.4.10-11. */
    double tau_10_sec;    /* tau_1 at EMRC */
    double T_10_sec;      /* T_1 at EMRC */

    /* Phase-2 initial mass / mass-flow (kg, kg/s). Eq. 4.4.11. */
    double M_GR_kg;
    double Mdot_2_kgps;
    double Mdot_3_kgps;

    /* Artificial-tau transition cutover (s). Eqs. 4.4.13-14. */
    double P_CMR_sec;

    /* Terminal constraints. Eqs. 4.4.34-40. */
    double R_T_m;
    double V_T_mps;
    double Theta_T_rad;
    double G_T_mps2;

    /* Engine misalignment factor. Eq. 4.4.31. */
    double ROV;

    /* SMC gain. Eqs. 4.4.83-84. */
    double SMCG;

    /* [MS4] inputs. EDD §2.3.5. */
    double lambda_rad;
    double phi_L_rad;
    double A_z_rad;
    double i_orb_rad;
} lvdc_igm_presets_t;

/* Persistent state carried across BMLs. The caller owns storage;
 * `lvdc_igm_init` resets every field. */
typedef struct {
    lvdc_igm_mode_t mode;

    /* Copy of presets so the state is self-contained. */
    lvdc_igm_presets_t p;

    /* Constant after init. */
    double MS4[3][3];

    /* Decrementing time-to-go estimates (s). */
    double T_1i_sec;
    double T_3i_sec;

    /* Post-EMRC artificial-tau progress (s). */
    double P_C_sec;

    /* tau_3N latched at EMRC (s). Eq. 4.4.11. */
    double tau_3N_sec;

    /* Last chi_y4, chi_z4 computed (rad). For logging. */
    double chi_y4_rad;
    double chi_z4_rad;

    /* SMC integrator (Eqs. 4.4.83-84). */
    lvdc_igm_smc_state_t smc;
} lvdc_igm_state_t;

/* Reset `s` to IGM-enable initial conditions using `p`. Starts in
 * LVDC_IGM_MODE_PHASE1. Computes [MS4] once from the preset
 * lambda/phi_L/A_z/i. Zeros SMC state. */
void lvdc_igm_init(lvdc_igm_state_t *s,
                   const lvdc_igm_presets_t *p);

/* Build `out` from the AS-205 (or other mission) `params` struct.
 * The field map is:
 *
 *   V_ex1_mps        <- params.V_ex1_mps                 (TMX Vex1)
 *   V_ex3_mps        <- params.V_ex3_mps                 (TMX Vex3)
 *   T_1i_initial_sec <- params.igm_T_1i_initial_sec      (TMX T1i)
 *   T_3i_initial_sec <- params.igm_T_3i_initial_sec      (TMX T3i)
 *   tau_10_sec       <- params.igm_tau_10_sec            (TMX tau10)
 *   T_10_sec         <- params.igm_T_10_sec              (TMX T10)
 *   M_GR_kg          <- params.igm_M_GR_kg               (TMX M03 artif.)
 *   Mdot_2_kgps      <- params.igm_Mdot_2_kgps           (TMX MI artif.)
 *   Mdot_3_kgps      <- params.Mdot_3_kgps               (TMX M3)
 *   P_CMR_sec        <- params.igm_P_CMR_sec             (TMX CO)
 *   R_T_m            <- params.target_R_T_m              (TMX TVX)
 *   V_T_mps          <- params.target_V_T_mps            (TMX VT)
 *   Theta_T_rad      <- params.target_Theta_T_deg (deg->rad)
 *   G_T_mps2         <- params.target_Xdotdot_VGT_mps2   (TMX XVG)
 *   ROV              <- params.igm_ROV
 *   SMCG             <- params.igm_SMCG
 *   lambda_rad       <- params.igm_lambda_N_deg (deg->rad)
 *   phi_L_rad        <- params.phi_L_deg        (deg->rad)
 *   A_z_rad          <- params.A_z_deg          (deg->rad)
 *   i_orb_rad        <- params.igm_i_orb_deg    (deg->rad)
 */
void lvdc_igm_presets_from_params(const lvdc_params_t *params,
                                  lvdc_igm_presets_t *out);

/* Transition from PHASE1 to PHASE2_ARTIFICIAL_TAU at the EMRC event.
 *
 * Eq. 4.4.9 : T_1i <- 0
 * Eq. 4.4.10 (alpha_f) and Eq. 4.4.11 (tau_3N) run here once with
 * tau_1 captured from the last PHASE1 cycle.
 *
 * `tau_1_at_emrc_sec` is tau_1 from Eq. 4.4.7 at the last BML of
 * PHASE1 (caller preserves). No-op if `s->mode != PHASE1`. */
void lvdc_igm_emrc(lvdc_igm_state_t *s, double tau_1_at_emrc_sec);

/* Latch cutoff. Freezes mode at POST_CUTOFF so subsequent updates
 * are no-ops that keep the last chi_y4, chi_z4. */
void lvdc_igm_cutoff(lvdc_igm_state_t *s);

/* One IGM BML tick. Orchestrates:
 *   1. Advance T_1i, T_3i, P_C per the current mode.
 *   2. Compute tau_1, tau_3 per the current mode.
 *   3. Run EDD §2.3 and §4.4.2.2 equation chain (frames,
 *      intermediates, steering, position correction, output).
 *   4. Promote PHASE2_ARTIFICIAL_TAU -> PHASE2_REAL_TAU when
 *      P_C >= P_CMR.
 *   5. Update SMC (Eqs. 4.4.83-84) with the new chi_y, chi_z.
 *
 * Inputs:
 *   dT_sec              BML time step (same ΔT used in phasing).
 *   V_mps               current speed |V_S| (m/s).
 *   MoverF_s_s2pm       (M/F)_S per Eq. 4.4.6 (s^2/m).
 *   R_S[3]              current position in S-system (m).
 *   V_S[3]              current velocity in S-system (m/s).
 *   G_S[3]              current gravity in S-system (m/s^2).
 *   xddot_m, yddot_m, zddot_m :
 *                       measured body-frame specific accel used
 *                       by SMC (m/s^2).
 *
 * Outputs:
 *   *chi_y_out, *chi_z_out : final pitch and yaw commands in
 *                            S-system (rad). Pass NULL to skip.
 *
 * Returns 0 on success, -1 on internal domain error (e.g. tau <=
 * T_i in the intermediate block). POST_CUTOFF always succeeds and
 * returns the latched chi. */
int lvdc_igm_update(lvdc_igm_state_t *s,
                    double dT_sec,
                    double V_mps,
                    double MoverF_s_s2pm,
                    const double R_S[3],
                    const double V_S[3],
                    const double G_S[3],
                    double xddot_m_mps2,
                    double yddot_m_mps2,
                    double zddot_m_mps2,
                    double *chi_y_out,
                    double *chi_z_out);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_IGM_H */
