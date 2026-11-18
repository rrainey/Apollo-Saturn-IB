/*
 * lvdc_igm_phasing.c — IGM phasing equations
 * (EDD §4.4.2.3, Eqs. 4.4.7 through 4.4.17).
 */

#include "lvdc_igm_phasing.h"

double lvdc_igm_tau_1(double V_ex1_mps, double MoverF_s_s2pm) {
    /* Eq. 4.4.7 */
    return V_ex1_mps * MoverF_s_s2pm;
}

double lvdc_igm_decrement_T_1i(double T_1i_sec, double DeltaT_sec) {
    /* Eq. 4.4.8 */
    return T_1i_sec - DeltaT_sec;
}

double lvdc_igm_alpha_f(double tau_10_sec, double T_10_sec,
                       double tau_1_sec) {
    /* Eq. 4.4.10 */
    const double denom = T_10_sec + tau_1_sec;
    if (denom == 0.0) return 0.0;
    return tau_10_sec / denom;
}

double lvdc_igm_tau_3N(double M_GR_kg,
                      double alpha_f,
                      double Mdot_2_kgps,
                      double T_10_sec,
                      double Mdot_3_kgps) {
    /* Eq. 4.4.11 */
    if (Mdot_3_kgps == 0.0) return 0.0;
    return (M_GR_kg - alpha_f * Mdot_2_kgps * T_10_sec) / Mdot_3_kgps;
}

double lvdc_igm_tau_3_real(double V_ex3_mps, double MoverF_s_s2pm) {
    /* Eq. 4.4.12 / Eq. 4.4.16 */
    return V_ex3_mps * MoverF_s_s2pm;
}

double lvdc_igm_tau_3K(double tau_3N_sec, double P_C_sec) {
    /* Eq. 4.4.13 */
    return tau_3N_sec - P_C_sec;
}

double lvdc_igm_tau_3_blend(double tau_3K_sec,
                           double tau_3M_sec,
                           double P_C_sec,
                           double P_CMR_sec) {
    /* Eq. 4.4.14 — blend with (P_C / P_CMR)^4 weight. */
    if (P_CMR_sec <= 0.0) return tau_3M_sec;
    const double w = P_C_sec / P_CMR_sec;
    const double w4 = w * w * w * w;
    return tau_3K_sec + (tau_3M_sec - tau_3K_sec) * w4;
}

double lvdc_igm_increment_P_C(double P_C_sec, double DeltaT_sec) {
    /* Eq. 4.4.15 */
    return P_C_sec + DeltaT_sec;
}

double lvdc_igm_decrement_T_3i(double T_3i_sec, double DeltaT_sec) {
    /* Eq. 4.4.17 */
    return T_3i_sec - DeltaT_sec;
}
