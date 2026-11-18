/*
 * lvdc_igm_steering.h — IGM steering: time-to-go, range angle,
 * velocity-to-be-gained, and preliminary guidance commands
 * (EDD §4.4.2.2 Eqs. 4.4.29 through 4.4.56).
 *
 * The functions here operate on quantities produced by earlier IGM
 * blocks:
 *   lvdc_igm_phasing.h       : tau_1, tau_3, T_1i, T_3i, P_C
 *   lvdc_igm_intermediate.h  : L_1, J_1, S_1, Q_1, P_1, U_1,
 *                              L'_3, J'_3, L'_y
 *   lvdc_igm_frames.h        : R_4, V_4, R_V, V_V, G_V, [M4V], [MSV]
 *
 * Together they produce the preliminary pitch and yaw commands
 * chi_tilde_y, chi_tilde_z that satisfy the velocity constraint.
 * The §4.4.2.2 position-correction block (Eqs. 4.4.57-82) is not
 * covered here; it runs after this layer and refines chi_y, chi_z.
 *
 * Angles in this module follow the OCR'd EDD verbatim. The phi
 * equations include a `/ pi` factor (Eq. 4.4.32) that is a
 * half-revolution scaling the LVDC used in its fixed-point math;
 * callers must keep the convention consistent downstream.
 */

#ifndef LVDC_IGM_STEERING_H
#define LVDC_IGM_STEERING_H

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------
 * Time-to-go total: EDD Eqs. 4.4.29 and 4.4.53.
 *   T_star = T_1i + T_3i
 * ----------------------------------------------------------------- */
double lvdc_igm_T_star(double T_1i_sec, double T_3i_sec);

/* -----------------------------------------------------------------
 * Range angle in the 4-system: EDD Eq. 4.4.30.
 *   phi_i = Arctan(Z_4 / X_4)
 * Uses atan2(Z_4, X_4) so the sign and quadrant are handled.
 * ----------------------------------------------------------------- */
double lvdc_igm_phi_i(double X_4_m, double Z_4_m);

/* -----------------------------------------------------------------
 * Predicted terminal range correction: EDD Eq. 4.4.31.
 *   delta_2 = V T* - J'_3 + L'_y T_3i
 *             - (ROV / V_ex3) * [(tau_1 - T_1i) L_1
 *                              + (tau_3 - T_3i) L'_3]
 *             * (L'_y + V - V_T)
 * All inputs in SI (m, m/s, s). ROV is a dimensionless engine
 * misalignment/off-nominal factor from the preset.
 * ----------------------------------------------------------------- */
double lvdc_igm_delta_2(double V_mps,
                        double T_star_sec,
                        double J_prime_3,
                        double L_prime_y,
                        double T_3i_sec,
                        double ROV,
                        double V_ex3_mps,
                        double tau_1_sec,
                        double T_1i_sec,
                        double L_1,
                        double tau_3_sec,
                        double L_prime_3,
                        double V_T_mps);

/* -----------------------------------------------------------------
 * Predicted terminal range angle increment: EDD Eq. 4.4.32.
 *   phi_iT = (1 / R_T) * (S_1 + delta_2) * cos(Theta_T) / pi
 * The pi divisor is the EDD's half-revolution scaling.
 * ----------------------------------------------------------------- */
double lvdc_igm_phi_iT(double R_T_m, double S_1, double delta_2,
                       double Theta_T_rad);

/* -----------------------------------------------------------------
 * Predicted terminal range angle: EDD Eq. 4.4.33.
 *   phi_T = phi_i + phi_iT
 * ----------------------------------------------------------------- */
double lvdc_igm_phi_T(double phi_i_rad, double phi_iT_rad);

/* -----------------------------------------------------------------
 * Terminal position, velocity, gravity in V-system:
 * EDD Eqs. 4.4.34-40.
 *   X_VT = R_T,            Y_VT = 0,                 Z_VT = 0
 *   Xdot_VT = V_T sin Θ_T, Ydot_VT = 0, Zdot_VT = V_T cos Θ_T
 *   Xddot_VgT = G_T,       Yddot_VgT = 0,            Zddot_VgT = 0
 * ----------------------------------------------------------------- */
typedef struct {
    double R_VT[3];    /* terminal position in V-system (m)   */
    double V_TV[3];    /* terminal velocity in V-system (m/s) */
    double G_TV[3];    /* terminal gravity in V-system (m/s^2) */
} lvdc_igm_terminal_V_t;

void lvdc_igm_terminal_state_V(double R_T_m,
                               double V_T_mps,
                               double Theta_T_rad,
                               double G_T_mps2,
                               lvdc_igm_terminal_V_t *out);

/* -----------------------------------------------------------------
 * Estimated and improved velocity-to-be-gained (Eqs. 4.4.46-54).
 *
 * `V_V` is the current velocity in V-system (Eq. 4.4.44 output).
 * `G_V` is the current gravity in V-system (Eq. 4.4.45 output).
 * `term` is the terminal V-system block (Eqs. 4.4.34-40).
 * `T_star` is the current time-to-go estimate (Eq. 4.4.29).
 * Outputs are written to `G_star_V` (Eq. 4.4.46) and
 * `dV_V_prime` (Eq. 4.4.47).
 * ----------------------------------------------------------------- */
void lvdc_igm_G_star_V(const double G_V[3], const double G_TV[3],
                       double G_star_V[3]);

void lvdc_igm_dV_V_prime(const double V_TV[3], const double V_V[3],
                         double T_star_sec,
                         const double G_star_V[3],
                         double dV_V_prime[3]);

/* -----------------------------------------------------------------
 * Improved estimate of total time-to-go (Eqs. 4.4.48-50, 52-53).
 *
 * Inputs:
 *   dV_V_prime     : Eq. 4.4.47 result.
 *   L_prime_y      : Eq. 4.4.27 result.
 *   L_prime_3      : Eq. 4.4.26 result.
 *   tau_3, T_3i    : phasing state.
 *   V_ex3          : second-phase exhaust velocity.
 *
 * Outputs (all optional, pass NULL to skip):
 *   dL_3_out       : Eq. 4.4.48.
 *   L_3_out        : Eq. 4.4.49.
 *   dT_3_prime_out : Eq. 4.4.50.
 *
 * Returns 0 on success, -1 if L_prime_y <= 0 (division guard). */
int lvdc_igm_improved_tgo(const double dV_V_prime[3],
                          double L_prime_y,
                          double L_prime_3,
                          double tau_3_sec,
                          double T_3i_sec,
                          double V_ex3_mps,
                          double *dL_3_out,
                          double *L_3_out,
                          double *dT_3_prime_out);

/* -----------------------------------------------------------------
 * Improved velocity-to-be-gained: EDD Eq. 4.4.54.
 *   dV_V = dV_V_prime - dT_3_prime * G_star_V
 * ----------------------------------------------------------------- */
void lvdc_igm_dV_V_final(const double dV_V_prime[3],
                         double dT_3_prime_sec,
                         const double G_star_V[3],
                         double dV_V[3]);

/* -----------------------------------------------------------------
 * Preliminary guidance commands satisfying the velocity constraint:
 * EDD Eqs. 4.4.55-56.
 *   chi_tilde_y = Arctan(dV_Vx / dV_Vz)
 *   chi_tilde_z = Arctan(dV_Vy / sqrt(dV_Vx^2 + dV_Vz^2))
 *
 * The EDD page-38 OCR for 4.4.55-56 is corrupted ("\dot{\chi}_V"
 * appears where the Cartesian V-system components dV_Vx, dV_Vy,
 * dV_Vz should be). The forms above follow the V-system axis
 * convention from Eqs. 4.4.34-37:
 *   X_V : radial up at terminal point.
 *   Y_V : out of orbital plane.
 *   Z_V : downrange, along terminal velocity projection.
 * so chi_tilde_y rotates about Y_V (pitch) using X and Z, and
 * chi_tilde_z rotates about Z_V (yaw) using Y and sqrt(X^2+Z^2).
 *
 * Both use atan2 for correct quadrants.
 * ----------------------------------------------------------------- */
double lvdc_igm_chi_tilde_y(const double dV_V[3]);
double lvdc_igm_chi_tilde_z(const double dV_V[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_IGM_STEERING_H */
