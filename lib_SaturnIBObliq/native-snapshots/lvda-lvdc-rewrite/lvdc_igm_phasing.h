/*
 * lvdc_igm_phasing.h — IGM phasing equations
 * (EDD §4.4.2.3, Eqs. 4.4.7 through 4.4.17).
 *
 * These pure functions compute the mass-to-mass-flow-rate ratios
 * `tau_1` and `tau_3` and their predicted times-to-go `T_1i`,
 * `T_3i` as the IGM scheme transitions between phases.
 *
 * The AS-205 flight profile has two IGM phases:
 *   Phase 1 : S-IVB burn, exhaust velocity V_ex1.
 *   Phase 2 : S-IVB burn after mixture ratio change (EMRC), exhaust
 *             velocity V_ex3.
 *
 * Phase 2 is split into:
 *   Artificial tau period : smoothed transition at EMRC using the
 *                            tau_3N / tau_3K / tau_3M blend
 *                            (Eqs. 4.4.11 through 4.4.14).
 *   Real tau period       : pure (M/F)_s based tau_3 (Eq. 4.4.16).
 *
 * The caller holds the mode state and calls these functions in the
 * order the mode machine dictates (see lvdc_igm_phasing.c for the
 * full narrative and the mode state machine in Phase C.3c).
 */

#ifndef LVDC_IGM_PHASING_H
#define LVDC_IGM_PHASING_H

#ifdef __cplusplus
extern "C" {
#endif

/* EDD Eq. 4.4.7 — first-phase mass/mass-flow ratio:
 *   tau_1 = V_ex1 * (M/F)_S
 */
double lvdc_igm_tau_1(double V_ex1_mps, double MoverF_s_s2pm);

/* EDD Eq. 4.4.8 — decrement first-phase time-to-go once per BML:
 *   T_1i = T_1i - DeltaT
 */
double lvdc_igm_decrement_T_1i(double T_1i_sec, double DeltaT_sec);

/* EDD Eq. 4.4.10 — thrust/mass-flow perturbation factor at EMRC:
 *   alpha_f = tau_10 / (T_10 + tau_1)
 *
 * Returns 0 and leaves the caller to handle zero denominator. */
double lvdc_igm_alpha_f(double tau_10_sec, double T_10_sec, double tau_1_sec);

/* EDD Eq. 4.4.11 — first guess to tau_3K (artificial tau_3):
 *   tau_3N = (M_GR - alpha_f * Mdot_2 * T_10) / Mdot_3
 */
double lvdc_igm_tau_3N(double M_GR_kg,
                       double alpha_f,
                       double Mdot_2_kgps,
                       double T_10_sec,
                       double Mdot_3_kgps);

/* EDD Eq. 4.4.12 — real tau_3 from smoothed M/F:
 *   tau_3M = V_ex3 * (M/F)_S
 *
 * Also used by Eq. 4.4.16 after the artificial tau period ends. */
double lvdc_igm_tau_3_real(double V_ex3_mps, double MoverF_s_s2pm);

/* EDD Eq. 4.4.13 — artificial tau_3:
 *   tau_3K = tau_3N - P_C
 */
double lvdc_igm_tau_3K(double tau_3N_sec, double P_C_sec);

/* EDD Eq. 4.4.14 — tau_3 during artificial-tau period, weighted
 * blend between tau_3K and tau_3M:
 *   tau_3 = tau_3K + (tau_3M - tau_3K) * (P_C / P_CMR)^4
 *
 * The exponent 4 provides a smooth transition. At P_C = 0 tau_3 =
 * tau_3K; at P_C = P_CMR tau_3 = tau_3M. Caller must ensure
 * `P_CMR > 0`. */
double lvdc_igm_tau_3_blend(double tau_3K_sec,
                            double tau_3M_sec,
                            double P_C_sec,
                            double P_CMR_sec);

/* EDD Eq. 4.4.15 — increment elapsed time since EMRC once per BML:
 *   P_C = P_C + DeltaT
 */
double lvdc_igm_increment_P_C(double P_C_sec, double DeltaT_sec);

/* EDD Eq. 4.4.17 — decrement second-phase time-to-go once per BML:
 *   T_3i = T_3i - DeltaT
 */
double lvdc_igm_decrement_T_3i(double T_3i_sec, double DeltaT_sec);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_IGM_PHASING_H */
