/*
 * lvdc_igm_position_correction.h — IGM position-correction block
 * (EDD §4.4.2.2 Eqs. 4.4.51, 4.4.57 through 4.4.82).
 *
 * After the preliminary pitch and yaw commands chi_tilde_y,
 * chi_tilde_z satisfy the velocity constraint (Eq. 4.4.55-56),
 * this block adds a linearised position-correction term so the
 * trajectory also hits the target position at insertion.
 *
 * The block consumes:
 *   - Intermediate parameters L_1..U_1, L'_3, J'_3, L'_y from
 *     lvdc_igm_intermediate.h (§4.4.2 Eqs. 4.4.20-28).
 *   - delta_L_3 (Eq. 4.4.48), L_3 (Eq. 4.4.49) from
 *     lvdc_igm_steering.h.
 *   - Phasing state tau_1, tau_3, T_1i, T_3i, V_ex3.
 *   - Current V-system position / velocity / smoothed gravity
 *     (R_V, V_V, G_V^*) from lvdc_igm_frames.h and the smoothed
 *     G*_V (Eq. 4.4.46).
 *   - Terminal V-system X_VT (Eq. 4.4.34).
 *   - Preliminary commands chi_tilde_y, chi_tilde_z (Eqs. 4.4.55-56).
 *   - Range-angle predicted terminal phi_T (Eq. 4.4.33).
 *   - Delta-T_N per-major-loop time step.
 *
 * It produces the corrected guidance commands in the 4-system:
 *   chi_y4, chi_z4 (Eqs. 4.4.81, 4.4.82). The §4.4.2.2 thrust-vector
 *   transform and SMC (Eqs. 4.4.83-87) run after this block and
 *   produce the final chi_y, chi_z in the S-system.
 *
 * -----------------------------------------------------------------
 * ANGLE-UNIT CONVENTION
 * -----------------------------------------------------------------
 * The EDD mixes two angle conventions in §4.4.2.2:
 *   - Arctan / cos / sin ops produce or consume radians.
 *   - Several equations contain a `/ pi` divisor (Eqs. 4.4.32,
 *     4.4.81, 4.4.82) or a `1/2` literal (Eq. 4.4.81). These are
 *     half-revolution scalings the LVDC used for its fixed-point
 *     word format (angles are stored as fractions of pi radians,
 *     so pi = 1.0 unit).
 *
 * This implementation transcribes every equation VERBATIM: literal
 * `/ M_PI` where the EDD has `/pi`, and literal `0.5` where the
 * EDD has `1/2`. If angles are ever added or subtracted that come
 * from different conventions, the result mirrors whatever the
 * LVDC computed. The shadow replay layer is the source of truth
 * for whether the LVDC's /pi or 1/2 constants need any
 * reinterpretation.
 */

#ifndef LVDC_IGM_POSITION_CORRECTION_H
#define LVDC_IGM_POSITION_CORRECTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* All inputs needed to run Eqs. 4.4.51, 4.4.57-82 once per BML. */
typedef struct {
    /* Phasing (§4.4.2.3). */
    double tau_1_sec;
    double tau_3_sec;
    double T_1i_sec;
    double T_3i_sec;
    double V_ex3_mps;

    /* Intermediate parameters (§4.4.2 Eqs. 4.4.20-28). */
    double L_1;
    double J_1;
    double S_1;
    double Q_1;
    double P_1;
    double U_1;
    double L_prime_3;
    double J_prime_3;
    double L_prime_y;

    /* From steering layer. */
    double dL_3;       /* Eq. 4.4.48 */
    double L_3;        /* Eq. 4.4.49 */
    double T_star_sec; /* Eq. 4.4.53 */

    /* Current V-system state (from lvdc_igm_frames.h + Eq. 4.4.46). */
    double X_V_m;
    double Y_V_m;
    double Xdot_V_mps;
    double Ydot_V_mps;
    double Xddot_Vg_star_mps2; /* smoothed gravity, X-axis */
    double Yddot_V_mps2;       /* smoothed Y accel; see note below */

    /* Terminal V-system (Eq. 4.4.34). */
    double X_VT_m;

    /* Preliminary guidance commands (Eqs. 4.4.55-56). */
    double chi_tilde_y_rad;
    double chi_tilde_z_rad;

    /* Predicted terminal range angle (Eq. 4.4.33). */
    double phi_T_rad;

    /* Per-major-loop time delta (same ΔT_N as elsewhere). */
    double dT_N_sec;
} lvdc_igm_pc_inputs_t;

/* Every intermediate equation output, in evaluation order. The
 * caller can read any sub-result for logging or test. */
typedef struct {
    /* Eq. 4.4.51 J_3, 4.4.57 S_3, 4.4.58 Q_3. */
    double J_3;
    double S_3;
    double Q_3;

    /* Eq. 4.4.59-62. */
    double L_y;
    double J_y;
    double S_y;
    double Q_y;

    /* Eq. 4.4.63-64. */
    double K_y;
    double D_y;

    /* Eq. 4.4.65-67. */
    double dY_V;
    double K_3;
    double K_4;

    /* Eq. 4.4.68-69. */
    double P_3;
    double U_3;

    /* Eq. 4.4.70-72. */
    double L_p;
    double C_2;
    double C_4;

    /* Eq. 4.4.73-77. */
    double J_p;
    double S_p;
    double Q_p;
    double K_p;
    double D_p;

    /* Eq. 4.4.78-80. */
    double dX_V;
    double K_1;
    double K_2;

    /* Eq. 4.4.81-82 — corrected commands in 4-system. */
    double chi_y4_rad;
    double chi_z4_rad;
} lvdc_igm_pc_outputs_t;

/* Run the full position-correction block (EDD §4.4.2.2 Eqs. 4.4.51,
 * 4.4.57-82). The output is unit-consistent with the EDD literals:
 * chi_y4 and chi_z4 carry the /pi and 1/2 literals that appear in
 * Eqs. 4.4.81 and 4.4.82 as written; see the ANGLE-UNIT
 * CONVENTION note above.
 *
 * Returns 0 on success, -1 on NULL args or if any divisor computed
 * from the input state is exactly zero (J_y, D_y, J_p, D_p). */
int lvdc_igm_position_correction(const lvdc_igm_pc_inputs_t *in,
                                 lvdc_igm_pc_outputs_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_IGM_POSITION_CORRECTION_H */
