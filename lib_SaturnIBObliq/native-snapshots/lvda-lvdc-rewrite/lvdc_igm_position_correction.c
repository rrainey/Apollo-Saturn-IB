/*
 * lvdc_igm_position_correction.c — IGM position-correction block
 * (EDD §4.4.2.2 Eqs. 4.4.51, 4.4.57 through 4.4.82).
 *
 * Every equation is transcribed verbatim from the EDD; the
 * section-header comments below reference the EDD equation number
 * and the English sub-heading as it appears on pages 38-40 of
 * IBM 70-207-0001 Rev A.
 */

#include "lvdc_igm_position_correction.h"

#include <math.h>
#include <stddef.h>

int lvdc_igm_position_correction(const lvdc_igm_pc_inputs_t *in,
                                 lvdc_igm_pc_outputs_t *out) {
    if (in == NULL || out == NULL) return -1;

    const double T_1i = in->T_1i_sec;
    const double T_3i = in->T_3i_sec;
    const double tau_3 = in->tau_3_sec;
    const double V_ex3 = in->V_ex3_mps;
    const double T_star = in->T_star_sec;
    const double dT_N = in->dT_N_sec;

    const double c_y = cos(in->chi_tilde_y_rad);
    const double s_y = sin(in->chi_tilde_y_rad);
    const double c_z = cos(in->chi_tilde_z_rad);
    const double s_z = sin(in->chi_tilde_z_rad);

    /* --------------------------------------------------------------
     * Position Correction Terms (EDD §4.4.2.2 page 38-39).
     * ------------------------------------------------------------ */

    /* Eq. 4.4.51 — J_3 = J'_3 + T_3i ΔL_3. */
    const double J_3 = in->J_prime_3 + T_3i * in->dL_3;

    /* Eq. 4.4.57 — S_3 = L_3 T_3i - J_3. */
    const double S_3 = in->L_3 * T_3i - J_3;

    /* Eq. 4.4.58 — Q_3 = S_3 τ_3 - 1/2 V_ex3 T_3i^2. */
    const double Q_3 = S_3 * tau_3 - 0.5 * V_ex3 * T_3i * T_3i;

    /* Eq. 4.4.59 — L_y = L_1 + L_3. */
    const double L_y = in->L_1 + in->L_3;

    /* Eq. 4.4.60 — J_y = J_1 + J_3 + L_3 T_1i. */
    const double J_y = in->J_1 + J_3 + in->L_3 * T_1i;

    /* Eq. 4.4.61 — S_y = S_1 - J_3 + L_y T_3i. */
    const double S_y = in->S_1 - J_3 + L_y * T_3i;

    /* Eq. 4.4.62 — Q_y = Q_1 + Q_3 + S_3 T_1i + J_1 T_3i. */
    const double Q_y = in->Q_1 + Q_3 + S_3 * T_1i + in->J_1 * T_3i;

    /* Eq. 4.4.63 — K_y = L_y / J_y. Guard division. */
    if (J_y == 0.0) return -1;
    const double K_y = L_y / J_y;

    /* Eq. 4.4.64 — D_y = S_y - K_y Q_y. */
    const double D_y = S_y - K_y * Q_y;
    if (D_y == 0.0) return -1;

    /* Eq. 4.4.65 — ΔY_V = Y_V + Ẏ_V T* + 1/2 Ÿ_V^* T*^2 + S_y sin χ̃_z. */
    const double dY_V = in->Y_V_m
                      + in->Ydot_V_mps * T_star
                      + 0.5 * in->Yddot_V_mps2 * T_star * T_star
                      + S_y * s_z;

    /* Eq. 4.4.66 — K_3 = ΔY_V / (D_y cos χ̃_z). */
    const double K_3 = dY_V / (D_y * c_z);

    /* Eq. 4.4.67 — K_4 = K_y K_3. */
    const double K_4 = K_y * K_3;

    /* Eq. 4.4.68 — P_3 = J_3 (τ_3 + 2 T_1i) - 1/2 V_ex3 T_3i^2. */
    const double P_3 = J_3 * (tau_3 + 2.0 * T_1i)
                     - 0.5 * V_ex3 * T_3i * T_3i;

    /* Eq. 4.4.69 — U_3 = Q_3 (τ_3 + 2 T_1i) - 1/6 V_ex3 T_3i^3. */
    const double U_3 = Q_3 * (tau_3 + 2.0 * T_1i)
                     - (1.0 / 6.0) * V_ex3 * T_3i * T_3i * T_3i;

    /* --------------------------------------------------------------
     * Position Correction Terms (Continued) (EDD page 40).
     * ------------------------------------------------------------ */

    /* Eq. 4.4.70 — L_p = L_y cos χ̃_z. */
    const double L_p = L_y * c_z;

    /* Eq. 4.4.71 — C_2 = cos χ̃_z + K_3 sin χ̃_z. */
    const double C_2 = c_z + K_3 * s_z;

    /* Eq. 4.4.72 — C_4 = K_4 sin χ̃_z. */
    const double C_4 = K_4 * s_z;

    /* Eq. 4.4.73 — J_p = J_y C_2 - C_4 (P_1 + P_3 + L_3 T_1i^2). */
    const double J_p = J_y * C_2
                     - C_4 * (in->P_1 + P_3 + in->L_3 * T_1i * T_1i);

    /* Eq. 4.4.74 — S_p = S_y C_2 - Q_y C_4. */
    const double S_p = S_y * C_2 - Q_y * C_4;

    /* Eq. 4.4.75 — Q_p = Q_y C_2 - C_4 (U_1 + U_3 + S_3 T_1i^2 + T_3i P_1). */
    const double Q_p = Q_y * C_2
                     - C_4 * (in->U_1 + U_3
                            + S_3 * T_1i * T_1i
                            + T_3i * in->P_1);

    /* Eq. 4.4.76 — K_p = L_p / J_p. */
    if (J_p == 0.0) return -1;
    const double K_p = L_p / J_p;

    /* Eq. 4.4.77 — D_p = S_p - K_p Q_p. */
    const double D_p = S_p - K_p * Q_p;
    if (D_p == 0.0) return -1;

    /* Eq. 4.4.78 — ΔX_V = X_V - X_VT + Ẋ_V T* + 1/2 Ẍ_Vg^* T*^2 + S_p sin χ̃_y. */
    const double dX_V = in->X_V_m - in->X_VT_m
                      + in->Xdot_V_mps * T_star
                      + 0.5 * in->Xddot_Vg_star_mps2 * T_star * T_star
                      + S_p * s_y;

    /* Eq. 4.4.79 — K_1 = ΔX_V / (D_p cos χ̃_y). */
    const double K_1 = dX_V / (D_p * c_y);

    /* Eq. 4.4.80 — K_2 = K_p K_1. */
    const double K_2 = K_p * K_1;

    /* --------------------------------------------------------------
     * Corrected Guidance Commands in 4-System (EDD page 40).
     *
     * The EDD writes these equations in LVDC half-revolution angle
     * units (angle / pi radians). The `/ pi` divisors on the K-
     * correction terms and the trailing `1/2` in Eq. 4.4.81 are
     * half-rev literals. Converted to radians (our internal unit):
     *   `/ pi` is dropped on each K correction term.
     *   `1/2` becomes `pi/2` (= M_PI_2).
     * The physics is unchanged — see the ANGLE-UNIT CONVENTION
     * note in lvdc_igm_position_correction.h.
     * ------------------------------------------------------------ */

    /* Eq. 4.4.81 — χ_y4 = χ̃_y - (K_1 - K_2 ΔT_N) - φ_T - π/2. */
    const double chi_y4 = in->chi_tilde_y_rad
                        - (K_1 - K_2 * dT_N)
                        - in->phi_T_rad
                        - M_PI_2;

    /* Eq. 4.4.82 — χ_z4 = χ̃_z - (K_3 - K_4 ΔT_N). */
    const double chi_z4 = in->chi_tilde_z_rad
                        - (K_3 - K_4 * dT_N);

    /* Write outputs. */
    out->J_3 = J_3;
    out->S_3 = S_3;
    out->Q_3 = Q_3;
    out->L_y = L_y;
    out->J_y = J_y;
    out->S_y = S_y;
    out->Q_y = Q_y;
    out->K_y = K_y;
    out->D_y = D_y;
    out->dY_V = dY_V;
    out->K_3 = K_3;
    out->K_4 = K_4;
    out->P_3 = P_3;
    out->U_3 = U_3;
    out->L_p = L_p;
    out->C_2 = C_2;
    out->C_4 = C_4;
    out->J_p = J_p;
    out->S_p = S_p;
    out->Q_p = Q_p;
    out->K_p = K_p;
    out->D_p = D_p;
    out->dX_V = dX_V;
    out->K_1 = K_1;
    out->K_2 = K_2;
    out->chi_y4_rad = chi_y4;
    out->chi_z4_rad = chi_z4;
    return 0;
}
