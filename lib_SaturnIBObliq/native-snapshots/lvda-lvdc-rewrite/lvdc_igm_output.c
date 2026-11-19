/*
 * lvdc_igm_output.c — IGM final output stage
 * (EDD §4.4.2.2 Eqs. 4.4.83 through 4.4.87).
 */

#include "lvdc_igm_output.h"

#include <math.h>
#include <stddef.h>

void lvdc_igm_smc_update(lvdc_igm_smc_state_t *smc,
                         double SMCG,
                         double chi_y_rad,
                         double chi_z_rad,
                         double xddot_m_mps2,
                         double yddot_m_mps2,
                         double zddot_m_mps2,
                         double MoverF_s2pm,
                         double dT_sec) {
    if (smc == NULL) return;

    /* On the first call after reset, seed `past` with current so
     * the trapezoidal average is well-defined. Caller sets
     * `primed = 0` on reset. */
    if (!smc->primed) {
        smc->chi_y_past = chi_y_rad;
        smc->chi_z_past = chi_z_rad;
        smc->primed = 1;
    }

    const double dT_over_pi = dT_sec / M_PI;

    /* Eq. 4.4.83 — SMCY integrator:
     *   SMCY = SMCY + SMCG *
     *     (z̈_m + ẍ_m tan[u_y])/(ẍ_m - z̈_m tan[u_y]) * (ΔT/π)
     * where u_y = (1/2)(chi_y + chi_y_past) - SMCY. */
    {
        const double u_y = 0.5 * (chi_y_rad + smc->chi_y_past)
                         - smc->SMCY;
        const double t   = tan(u_y);
        const double num = zddot_m_mps2 + xddot_m_mps2 * t;
        const double den = xddot_m_mps2 - zddot_m_mps2 * t;
        if (den != 0.0) {
            smc->SMCY += SMCG * (num / den) * dT_over_pi;
        }
    }

    /* Eq. 4.4.84 — SMCZ integrator:
     *   SMCZ = SMCZ + SMCG *
     *     (sin[u_z] - (M/F) ÿ_m) / cos[u_z] * (ΔT/π)
     * where u_z = (1/2)(chi_z + chi_z_past) - SMCZ. */
    {
        const double u_z = 0.5 * (chi_z_rad + smc->chi_z_past)
                         - smc->SMCZ;
        const double c   = cos(u_z);
        const double s   = sin(u_z);
        if (c != 0.0) {
            const double term = (s - MoverF_s2pm * yddot_m_mps2) / c;
            smc->SMCZ += SMCG * term * dT_over_pi;
        }
    }

    /* Latch this BML's chi for the next cycle. */
    smc->chi_y_past = chi_y_rad;
    smc->chi_z_past = chi_z_rad;
}

void lvdc_igm_thrust_vector_S(double MS4[3][3],
                              double chi_y4_rad,
                              double chi_z4_rad,
                              double F_prime_S[3]) {
    /* Unit thrust in 4-system: see header note on Eq. 4.4.85 OCR. */
    const double cy = cos(chi_y4_rad);
    const double sy = sin(chi_y4_rad);
    const double cz = cos(chi_z4_rad);
    const double sz = sin(chi_z4_rad);

    const double u4[3] = {
        cy * cz,
        sz,
        -sy * cz
    };

    /* F'_S = MS4^T * u4. Expressed without allocating a transpose. */
    for (int i = 0; i < 3; i++) {
        F_prime_S[i] = MS4[0][i] * u4[0]
                     + MS4[1][i] * u4[1]
                     + MS4[2][i] * u4[2];
    }
}

double lvdc_igm_chi_y_final(const double F_prime_S[3], double SMCY_rad) {
    /* Eq. 4.4.86 */
    return atan2(-F_prime_S[2], F_prime_S[0]) + SMCY_rad;
}

double lvdc_igm_chi_z_final(const double F_prime_S[3], double SMCZ_rad) {
    /* Eq. 4.4.87. For unit F'_S, 1 - F'^2_Sy = F'^2_Sx + F'^2_Sz.
     * Use the latter form via atan2 for numerical stability near
     * F'_Sy = +/-1. */
    const double xz = sqrt(F_prime_S[0] * F_prime_S[0]
                         + F_prime_S[2] * F_prime_S[2]);
    return atan2(F_prime_S[1], xz) + SMCZ_rad;
}
