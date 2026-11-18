/*
 * lvdc_igm_intermediate.c — IGM intermediate parameters
 * (EDD §4.4.2 Eqs. 4.4.20 through 4.4.28).
 */

#include "lvdc_igm_intermediate.h"

#include <math.h>

int lvdc_igm_intermediate_compute(double V_ex1_mps, double tau_1_sec,
                                  double T_1i_sec,
                                  double V_ex3_mps, double tau_3_sec,
                                  double T_3i_sec,
                                  lvdc_igm_intermediate_t *out) {
    if (!out) return -1;

    /* Guard the log arguments. `tau - T_i` is the mass remaining
     * divided by the mass flow at phase end. Values at or below
     * zero are non-physical (phase would be over). */
    const double arg1 = tau_1_sec - T_1i_sec;
    const double arg3 = tau_3_sec - T_3i_sec;
    if (tau_1_sec <= 0.0 || arg1 <= 0.0) return -1;
    if (tau_3_sec <= 0.0 || arg3 <= 0.0) return -1;

    /* First phase, EDD Eqs. 4.4.20 through 4.4.25. */
    out->L_1 = V_ex1_mps * log(tau_1_sec / arg1);
    out->J_1 = out->L_1 * tau_1_sec - V_ex1_mps * T_1i_sec;
    out->S_1 = out->L_1 * T_1i_sec - out->J_1;
    out->Q_1 = out->S_1 * tau_1_sec
             - 0.5 * V_ex1_mps * T_1i_sec * T_1i_sec;
    out->P_1 = out->J_1 * tau_1_sec
             - 0.5 * V_ex1_mps * T_1i_sec * T_1i_sec;
    out->U_1 = out->Q_1 * tau_1_sec
             - (1.0 / 6.0) * V_ex1_mps * T_1i_sec * T_1i_sec * T_1i_sec;

    /* Second-phase primes, EDD Eqs. 4.4.26 and 4.4.28. */
    out->L_prime_3 = V_ex3_mps * log(tau_3_sec / arg3);
    out->J_prime_3 = out->L_prime_3 * tau_3_sec - V_ex3_mps * T_3i_sec;

    /* Sum, EDD Eq. 4.4.27. */
    out->L_prime_y = out->L_1 + out->L_prime_3;

    return 0;
}
