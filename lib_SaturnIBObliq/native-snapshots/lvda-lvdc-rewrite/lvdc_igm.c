/*
 * lvdc_igm.c — IGM mode state machine and per-BML orchestrator.
 *
 * Wraps the EDD §4.4.2 equation blocks behind one API. See
 * lvdc_igm.h for the mode narrative.
 */

#include "lvdc_igm.h"
#include "lvdc_igm_frames.h"
#include "lvdc_igm_intermediate.h"
#include "lvdc_igm_phasing.h"
#include "lvdc_igm_steering.h"
#include "lvdc_igm_position_correction.h"
#include "lvdc_igm_output.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

void lvdc_igm_init(lvdc_igm_state_t *s,
                   const lvdc_igm_presets_t *p) {
    if (s == NULL || p == NULL) return;

    memset(s, 0, sizeof(*s));
    s->p = *p;
    s->mode = LVDC_IGM_MODE_PHASE1;

    /* EDD §2.3.5 — one-time [MS4]. */
    lvdc_igm_build_MS4(p->lambda_rad, p->phi_L_rad,
                       p->A_z_rad, p->i_orb_rad,
                       s->MS4);

    s->T_1i_sec = p->T_1i_initial_sec;
    s->T_3i_sec = p->T_3i_initial_sec;
    s->P_C_sec  = 0.0;
    s->tau_3N_sec = 0.0;

    /* SMC resets to all zeros; primed flag already 0 via memset. */
}

void lvdc_igm_emrc(lvdc_igm_state_t *s, double tau_1_at_emrc_sec) {
    if (s == NULL) return;
    if (s->mode != LVDC_IGM_MODE_PHASE1) return;

    /* Eq. 4.4.9 — T_1i <- 0. */
    s->T_1i_sec = 0.0;

    /* Eq. 4.4.10 — alpha_f. */
    const double alpha_f = lvdc_igm_alpha_f(s->p.tau_10_sec,
                                            s->p.T_10_sec,
                                            tau_1_at_emrc_sec);

    /* Eq. 4.4.11 — tau_3N latched. */
    s->tau_3N_sec = lvdc_igm_tau_3N(s->p.M_GR_kg,
                                     alpha_f,
                                     s->p.Mdot_2_kgps,
                                     s->p.T_10_sec,
                                     s->p.Mdot_3_kgps);

    s->P_C_sec = 0.0;
    s->mode = LVDC_IGM_MODE_PHASE2_ARTIFICIAL_TAU;
}

void lvdc_igm_cutoff(lvdc_igm_state_t *s) {
    if (s == NULL) return;
    s->mode = LVDC_IGM_MODE_POST_CUTOFF;
}

/* Build the input struct for the position-correction block. */
static void pc_fill_inputs(const lvdc_igm_state_t *s,
                           double dT_sec,
                           double tau_1,
                           double tau_3,
                           const lvdc_igm_intermediate_t *im,
                           double dL_3,
                           double L_3,
                           double T_star,
                           double chi_tilde_y,
                           double chi_tilde_z,
                           double phi_T,
                           const double R_V[3],
                           const double V_V[3],
                           const double G_star_V[3],
                           double X_VT,
                           lvdc_igm_pc_inputs_t *pc_in) {
    pc_in->tau_1_sec = tau_1;
    pc_in->tau_3_sec = tau_3;
    pc_in->T_1i_sec  = s->T_1i_sec;
    pc_in->T_3i_sec  = s->T_3i_sec;
    pc_in->V_ex3_mps = s->p.V_ex3_mps;

    pc_in->L_1       = im->L_1;
    pc_in->J_1       = im->J_1;
    pc_in->S_1       = im->S_1;
    pc_in->Q_1       = im->Q_1;
    pc_in->P_1       = im->P_1;
    pc_in->U_1       = im->U_1;
    pc_in->L_prime_3 = im->L_prime_3;
    pc_in->J_prime_3 = im->J_prime_3;
    pc_in->L_prime_y = im->L_prime_y;

    pc_in->dL_3        = dL_3;
    pc_in->L_3         = L_3;
    pc_in->T_star_sec  = T_star;

    pc_in->X_V_m              = R_V[0];
    pc_in->Y_V_m              = R_V[1];
    pc_in->Xdot_V_mps         = V_V[0];
    pc_in->Ydot_V_mps         = V_V[1];
    pc_in->Xddot_Vg_star_mps2 = G_star_V[0];
    pc_in->Yddot_V_mps2       = G_star_V[1];

    pc_in->X_VT_m = X_VT;

    pc_in->chi_tilde_y_rad = chi_tilde_y;
    pc_in->chi_tilde_z_rad = chi_tilde_z;
    pc_in->phi_T_rad       = phi_T;

    pc_in->dT_N_sec = dT_sec;
}

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
                    double *chi_z_out) {
    if (s == NULL) return -1;

    if (s->mode == LVDC_IGM_MODE_POST_CUTOFF) {
        /* Hold last output. Don't advance state. */
        if (chi_y_out) *chi_y_out = s->smc.chi_y_past;
        if (chi_z_out) *chi_z_out = s->smc.chi_z_past;
        return 0;
    }

    /* ----------------------------------------------------------
     * Phasing (§4.4.2.3). Decrement time-to-go. Order matters:
     * compute tau_1 / tau_3 BEFORE decrementing T_1i / T_3i so
     * this cycle uses the pre-decrement state.
     * -------------------------------------------------------- */
    double tau_1 = 0.0, tau_3 = 0.0;

    switch (s->mode) {
    case LVDC_IGM_MODE_PHASE1:
        /* Eq. 4.4.7 — tau_1 = V_ex1 * (M/F)_S. */
        tau_1 = lvdc_igm_tau_1(s->p.V_ex1_mps, MoverF_s_s2pm);
        /* Phase 2 tau_3 unused in Phase 1 for chi, but Eq. 4.4.17
         * requires it to be well-defined for intermediate math.
         * Use Eq. 4.4.16 real-tau form. */
        tau_3 = lvdc_igm_tau_3_real(s->p.V_ex3_mps, MoverF_s_s2pm);
        break;

    case LVDC_IGM_MODE_PHASE2_ARTIFICIAL_TAU: {
        /* In the artificial-tau period, tau_1 is not advanced
         * (T_1i already 0). tau_3 is blended per Eq. 4.4.14. */
        tau_1 = 0.0;
        const double tau_3M = lvdc_igm_tau_3_real(s->p.V_ex3_mps,
                                                   MoverF_s_s2pm);
        const double tau_3K = lvdc_igm_tau_3K(s->tau_3N_sec,
                                               s->P_C_sec);
        tau_3 = lvdc_igm_tau_3_blend(tau_3K, tau_3M,
                                      s->P_C_sec, s->p.P_CMR_sec);
        break;
    }

    case LVDC_IGM_MODE_PHASE2_REAL_TAU:
        tau_1 = 0.0;
        tau_3 = lvdc_igm_tau_3_real(s->p.V_ex3_mps, MoverF_s_s2pm);
        break;

    default:
        return -1;
    }

    /* ----------------------------------------------------------
     * Intermediate parameters (§4.4.2 Eqs. 4.4.20-28).
     * -------------------------------------------------------- */
    lvdc_igm_intermediate_t im;
    if (lvdc_igm_intermediate_compute(s->p.V_ex1_mps, tau_1,
                                      s->T_1i_sec,
                                      s->p.V_ex3_mps, tau_3,
                                      s->T_3i_sec,
                                      &im) != 0) {
        return -1;
    }

    /* ----------------------------------------------------------
     * Frame transforms (§4.4.2.2 Eqs. 4.4.18-19, 4.4.42-45).
     * -------------------------------------------------------- */
    double R_4[3], V_4[3];
    lvdc_igm_r_to_4(s->MS4, R_S, R_4);
    lvdc_igm_v_to_4(s->MS4, V_S, V_4);

    /* ----------------------------------------------------------
     * Steering: T*, phi_T, terminal state, G*_V, dV'_V,
     * improved T-to-go, dV_V, preliminary chi (Eqs. 4.4.29-56).
     * -------------------------------------------------------- */
    const double T_star = lvdc_igm_T_star(s->T_1i_sec, s->T_3i_sec);

    const double phi_i = lvdc_igm_phi_i(R_4[0], R_4[2]);
    const double d2 = lvdc_igm_delta_2(V_mps, T_star,
                                        im.J_prime_3, im.L_prime_y,
                                        s->T_3i_sec, s->p.ROV,
                                        s->p.V_ex3_mps,
                                        tau_1, s->T_1i_sec, im.L_1,
                                        tau_3, im.L_prime_3,
                                        s->p.V_T_mps);
    const double phi_iT = lvdc_igm_phi_iT(s->p.R_T_m, im.S_1, d2,
                                           s->p.Theta_T_rad);
    const double phi_T = lvdc_igm_phi_T(phi_i, phi_iT);

    lvdc_igm_terminal_V_t term;
    lvdc_igm_terminal_state_V(s->p.R_T_m, s->p.V_T_mps,
                              s->p.Theta_T_rad, s->p.G_T_mps2,
                              &term);

    double M4V[3][3], MSV[3][3];
    lvdc_igm_build_M4V(phi_T, M4V);
    lvdc_igm_build_MSV(M4V, s->MS4, MSV);

    double R_V[3], V_V[3], G_V[3];
    lvdc_igm_r_to_V(M4V, R_4, R_V);
    lvdc_igm_v_to_V(M4V, V_4, V_V);
    lvdc_igm_g_to_V(MSV, G_S, G_V);

    double G_star_V[3];
    lvdc_igm_G_star_V(G_V, term.G_TV, G_star_V);

    double dV_V_prime[3];
    lvdc_igm_dV_V_prime(term.V_TV, V_V, T_star, G_star_V,
                        dV_V_prime);

    double dL_3 = 0.0, L_3 = im.L_prime_3, dT_3_prime = 0.0;
    if (lvdc_igm_improved_tgo(dV_V_prime, im.L_prime_y, im.L_prime_3,
                              tau_3, s->T_3i_sec, s->p.V_ex3_mps,
                              &dL_3, &L_3, &dT_3_prime) != 0) {
        return -1;
    }

    /* Eq. 4.4.52-53 — update T_3i and T*. */
    const double T_3i_improved = s->T_3i_sec + dT_3_prime;
    const double T_star_improved = s->T_1i_sec + T_3i_improved;

    double dV_V[3];
    lvdc_igm_dV_V_final(dV_V_prime, dT_3_prime, G_star_V, dV_V);

    const double chi_tilde_y = lvdc_igm_chi_tilde_y(dV_V);
    const double chi_tilde_z = lvdc_igm_chi_tilde_z(dV_V);

    /* ----------------------------------------------------------
     * Position correction (§4.4.2.2 Eqs. 4.4.51, 4.4.57-82).
     * Use the IMPROVED T_3i and T* per Eqs. 4.4.52-53.
     * -------------------------------------------------------- */
    lvdc_igm_pc_inputs_t pc_in;
    lvdc_igm_pc_outputs_t pc_out;

    pc_fill_inputs(s, dT_sec, tau_1, tau_3, &im, dL_3, L_3,
                   T_star_improved, chi_tilde_y, chi_tilde_z,
                   phi_T, R_V, V_V, G_star_V, term.R_VT[0],
                   &pc_in);
    pc_in.T_3i_sec = T_3i_improved;  /* use improved T_3i */

    if (lvdc_igm_position_correction(&pc_in, &pc_out) != 0) {
        return -1;
    }

    s->chi_y4_rad = pc_out.chi_y4_rad;
    s->chi_z4_rad = pc_out.chi_z4_rad;

    /* ----------------------------------------------------------
     * Output stage (§4.4.2.2 Eqs. 4.4.85-87).
     * -------------------------------------------------------- */
    double F_prime_S[3];
    lvdc_igm_thrust_vector_S(s->MS4, pc_out.chi_y4_rad,
                              pc_out.chi_z4_rad, F_prime_S);

    const double chi_y = lvdc_igm_chi_y_final(F_prime_S, s->smc.SMCY);
    const double chi_z = lvdc_igm_chi_z_final(F_prime_S, s->smc.SMCZ);

    /* SMC update (Eqs. 4.4.83-84) AFTER chi is computed. */
    lvdc_igm_smc_update(&s->smc, s->p.SMCG,
                        chi_y, chi_z,
                        xddot_m_mps2, yddot_m_mps2, zddot_m_mps2,
                        MoverF_s_s2pm, dT_sec);

    /* ----------------------------------------------------------
     * Advance mode / decrement state.
     * Done LAST so the next cycle sees fresh values.
     * -------------------------------------------------------- */
    switch (s->mode) {
    case LVDC_IGM_MODE_PHASE1:
        /* Eq. 4.4.8 — T_1i -= ΔT. */
        s->T_1i_sec = lvdc_igm_decrement_T_1i(s->T_1i_sec, dT_sec);
        /* Eq. 4.4.17 — T_3i -= ΔT. Phase 1 may or may not advance
         * T_3i; the EDD places Eq. 4.4.17 under "Second Phase
         * Time-to-go Calculations" so it is only the pre-EMRC
         * terminal-time window that we set from presets.
         * We do NOT decrement T_3i in Phase 1. */
        break;

    case LVDC_IGM_MODE_PHASE2_ARTIFICIAL_TAU:
        /* Eq. 4.4.15 — P_C += ΔT. */
        s->P_C_sec = lvdc_igm_increment_P_C(s->P_C_sec, dT_sec);
        /* Eq. 4.4.17 — T_3i -= ΔT. */
        s->T_3i_sec = lvdc_igm_decrement_T_3i(s->T_3i_sec, dT_sec);
        /* Promote to real-tau when P_C >= P_CMR. */
        if (s->p.P_CMR_sec > 0.0 &&
            s->P_C_sec >= s->p.P_CMR_sec) {
            s->mode = LVDC_IGM_MODE_PHASE2_REAL_TAU;
        }
        break;

    case LVDC_IGM_MODE_PHASE2_REAL_TAU:
        /* Eq. 4.4.17 — T_3i -= ΔT. */
        s->T_3i_sec = lvdc_igm_decrement_T_3i(s->T_3i_sec, dT_sec);
        break;

    default:
        break;
    }

    if (chi_y_out) *chi_y_out = chi_y;
    if (chi_z_out) *chi_z_out = chi_z;
    return 0;
}
