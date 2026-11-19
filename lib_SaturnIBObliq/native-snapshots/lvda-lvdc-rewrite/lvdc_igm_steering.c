/*
 * lvdc_igm_steering.c — IGM steering layer
 * (EDD §4.4.2.2 Eqs. 4.4.29 through 4.4.56).
 */

#include "lvdc_igm_steering.h"

#include <math.h>

double lvdc_igm_T_star(double T_1i_sec, double T_3i_sec) {
    /* Eqs. 4.4.29 and 4.4.53. */
    return T_1i_sec + T_3i_sec;
}

double lvdc_igm_phi_i(double X_4_m, double Z_4_m) {
    /* Eq. 4.4.30. atan2 handles sign and X_4 near zero. */
    return atan2(Z_4_m, X_4_m);
}

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
                        double V_T_mps) {
    /* Eq. 4.4.31 — bracket-by-bracket. */
    const double term_a = V_mps * T_star_sec
                        - J_prime_3
                        + L_prime_y * T_3i_sec;

    if (V_ex3_mps == 0.0) {
        /* Guard the ratio; caller should never hit this. */
        return term_a;
    }

    const double rov_over_vex = ROV / V_ex3_mps;
    const double bracket = (tau_1_sec - T_1i_sec) * L_1
                         + (tau_3_sec - T_3i_sec) * L_prime_3;
    const double factor  = L_prime_y + V_mps - V_T_mps;
    return term_a - rov_over_vex * bracket * factor;
}

double lvdc_igm_phi_iT(double R_T_m, double S_1, double delta_2,
                       double Theta_T_rad) {
    /* Eq. 4.4.32 — the EDD form is
     *   phi_iT = (S_1 + delta_2) cos(Theta_T) / (R_T * pi)
     * where the `/ pi` converts the arc-length-over-radius
     * radian quantity to the LVDC's half-revolution angle unit.
     * We stay in radians everywhere, so the `/ pi` is dropped.
     * See lvdc_igm_position_correction.h ANGLE-UNIT CONVENTION. */
    if (R_T_m == 0.0) return 0.0;
    const double num = (S_1 + delta_2) * cos(Theta_T_rad);
    return num / R_T_m;
}

double lvdc_igm_phi_T(double phi_i_rad, double phi_iT_rad) {
    /* Eq. 4.4.33. */
    return phi_i_rad + phi_iT_rad;
}

void lvdc_igm_terminal_state_V(double R_T_m,
                               double V_T_mps,
                               double Theta_T_rad,
                               double G_T_mps2,
                               lvdc_igm_terminal_V_t *out) {
    /* Eqs. 4.4.34-40. X_V is radial, Y_V out-of-plane, Z_V downrange.
     * By construction of the V-system the terminal point has Y, Z
     * components zero for R; Y zero for V; Y, Z zero for G. */
    const double s = sin(Theta_T_rad), c = cos(Theta_T_rad);

    out->R_VT[0] = R_T_m;           /* Eq. 4.4.34 */
    out->R_VT[1] = 0.0;
    out->R_VT[2] = 0.0;

    out->V_TV[0] = V_T_mps * s;     /* Eq. 4.4.35 */
    out->V_TV[1] = 0.0;             /* Eq. 4.4.36 */
    out->V_TV[2] = V_T_mps * c;     /* Eq. 4.4.37 */

    out->G_TV[0] = G_T_mps2;        /* Eq. 4.4.38 */
    out->G_TV[1] = 0.0;             /* Eq. 4.4.39 */
    out->G_TV[2] = 0.0;             /* Eq. 4.4.40 */
}

void lvdc_igm_G_star_V(const double G_V[3], const double G_TV[3],
                       double G_star_V[3]) {
    /* Eq. 4.4.46 — G*_V = 1/2 (G_TV + G_V). */
    for (int i = 0; i < 3; i++) {
        G_star_V[i] = 0.5 * (G_TV[i] + G_V[i]);
    }
}

void lvdc_igm_dV_V_prime(const double V_TV[3], const double V_V[3],
                         double T_star_sec,
                         const double G_star_V[3],
                         double dV_V_prime[3]) {
    /* Eq. 4.4.47 — ΔV'_V = V_TV - V_V - T* G*_V. */
    for (int i = 0; i < 3; i++) {
        dV_V_prime[i] = V_TV[i] - V_V[i] - T_star_sec * G_star_V[i];
    }
}

int lvdc_igm_improved_tgo(const double dV_V_prime[3],
                          double L_prime_y,
                          double L_prime_3,
                          double tau_3_sec,
                          double T_3i_sec,
                          double V_ex3_mps,
                          double *dL_3_out,
                          double *L_3_out,
                          double *dT_3_prime_out) {
    if (L_prime_y <= 0.0 || V_ex3_mps == 0.0) return -1;

    /* Eq. 4.4.48 — ΔL_3 = 0.5 * ((ΔV'_V)^2 / L'_y - L'_y).
     * (ΔV'_V)^2 is the squared magnitude of the vector. */
    const double dVsq = dV_V_prime[0] * dV_V_prime[0]
                      + dV_V_prime[1] * dV_V_prime[1]
                      + dV_V_prime[2] * dV_V_prime[2];
    const double dL_3 = 0.5 * (dVsq / L_prime_y - L_prime_y);

    /* Eq. 4.4.49 — L_3 = L'_3 + ΔL_3. */
    const double L_3 = L_prime_3 + dL_3;

    /* Eq. 4.4.50 — ΔT'_3 = ΔL_3 * (tau_3 - T_3i) / V_ex3. */
    const double dT_3_prime = dL_3 * (tau_3_sec - T_3i_sec) / V_ex3_mps;

    if (dL_3_out)       *dL_3_out       = dL_3;
    if (L_3_out)        *L_3_out        = L_3;
    if (dT_3_prime_out) *dT_3_prime_out = dT_3_prime;
    return 0;
}

void lvdc_igm_dV_V_final(const double dV_V_prime[3],
                         double dT_3_prime_sec,
                         const double G_star_V[3],
                         double dV_V[3]) {
    /* Eq. 4.4.54 — ΔV_V = ΔV'_V - ΔT'_3 G*_V. */
    for (int i = 0; i < 3; i++) {
        dV_V[i] = dV_V_prime[i] - dT_3_prime_sec * G_star_V[i];
    }
}

double lvdc_igm_chi_tilde_y(const double dV_V[3]) {
    /* Eq. 4.4.55 — reconstructed from physics (see header note). */
    return atan2(dV_V[0], dV_V[2]);
}

double lvdc_igm_chi_tilde_z(const double dV_V[3]) {
    /* Eq. 4.4.56 — reconstructed from physics (see header note). */
    const double xz = sqrt(dV_V[0] * dV_V[0] + dV_V[2] * dV_V[2]);
    return atan2(dV_V[1], xz);
}
