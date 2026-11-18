/*
 * lvdc_igm_intermediate.h — IGM intermediate parameters
 * (EDD §4.4.2 Eqs. 4.4.20 through 4.4.28).
 *
 * The IGM scheme uses nine intermediate quantities per BML. They
 * relate the position and velocity-to-be-gained vectors to current
 * vehicle performance for each powered-flight phase.
 *
 * Inputs for each phase:
 *   V_ex    engine exhaust velocity (m/s)
 *   tau     mass-to-mass-flow-rate ratio (s)
 *   T_i     predicted time-to-go in this phase (s)
 *
 * The nine quantities are:
 *   L_i   velocity remaining to be delivered this phase (m/s)
 *   J_i   first time moment of L_i (m)
 *   S_i   second time moment             (m·s)
 *   Q_i   third time moment              (m·s)
 *   P_i   alternate second moment        (m·s)
 *   U_i   fourth time moment             (m·s²)
 *
 * The first-phase quantities use `V_ex1`, `tau_1`, `T_1i`. The
 * second-phase primes (L'_3, J'_3) use `V_ex3`, `tau_3`, `T_3i`.
 * Sum quantities L'_y come next. The improved `_y` family is
 * computed later, after the position correction stage.
 */

#ifndef LVDC_IGM_INTERMEDIATE_H
#define LVDC_IGM_INTERMEDIATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* All nine intermediate parameters for one BML. */
typedef struct {
    /* First phase (uses V_ex1, tau_1, T_1i). */
    double L_1;    /* Eq. 4.4.20 */
    double J_1;    /* Eq. 4.4.21 */
    double S_1;    /* Eq. 4.4.22 */
    double Q_1;    /* Eq. 4.4.23 */
    double P_1;    /* Eq. 4.4.24 */
    double U_1;    /* Eq. 4.4.25 */

    /* Second-phase primes (uses V_ex3, tau_3, T_3i). */
    double L_prime_3; /* Eq. 4.4.26 */
    double J_prime_3; /* Eq. 4.4.28 */

    /* Sum. */
    double L_prime_y; /* Eq. 4.4.27 */
} lvdc_igm_intermediate_t;

/* Compute EDD Eqs. 4.4.20 through 4.4.28.
 *
 *   V_ex1_mps      first-phase exhaust velocity (TMX App B).
 *   tau_1_sec      first-phase mass/mass-flow ratio.
 *   T_1i_sec       first-phase time-to-go.
 *   V_ex3_mps      second-phase exhaust velocity.
 *   tau_3_sec      second-phase mass/mass-flow ratio.
 *   T_3i_sec       second-phase time-to-go.
 *   out            all nine intermediate parameters.
 *
 * The caller must ensure `tau > T_i` so the logarithm argument is
 * positive. Returns 0 on success, -1 on any domain error. */
int lvdc_igm_intermediate_compute(double V_ex1_mps, double tau_1_sec,
                                  double T_1i_sec,
                                  double V_ex3_mps, double tau_3_sec,
                                  double T_3i_sec,
                                  lvdc_igm_intermediate_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_IGM_INTERMEDIATE_H */
