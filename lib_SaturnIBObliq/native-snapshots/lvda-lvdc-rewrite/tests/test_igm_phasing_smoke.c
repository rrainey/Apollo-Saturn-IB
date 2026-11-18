/*
 * test_igm_phasing_smoke.c — unit tests for EDD §4.4.2.3 phasing
 * equations (Eqs. 4.4.7 through 4.4.17).
 */

#include "../lvdc_igm_phasing.h"

#include <math.h>
#include <stdio.h>

static int g_fail_count = 0;

#define CHECK(cond, label) do {                                       \
    if (!(cond)) {                                                    \
        fprintf(stderr, "FAIL [%s] %s:%d  !(%s)\n",                  \
                (label), __FILE__, __LINE__, #cond);                  \
        g_fail_count++;                                               \
    }                                                                 \
} while (0)

#define CHECK_CLOSE(got, expected, abs_tol, label) do {               \
    const double _g = (got);                                          \
    const double _e = (expected);                                     \
    const double _d = fabs(_g - _e);                                  \
    if (_d > (abs_tol)) {                                             \
        fprintf(stderr,                                               \
                "FAIL [%s] %s:%d  got %.9g, expected %.9g, |d|=%.3g > tol %.3g\n", \
                (label), __FILE__, __LINE__, _g, _e, _d, (abs_tol));  \
        g_fail_count++;                                               \
    }                                                                 \
} while (0)

int main(void) {
    const double V_ex1 = 4151.0;
    const double V_ex3 = 4233.2;

    /* ------------------------------------------------------------
     * Eq. 4.4.7: tau_1 = V_ex1 * (M/F)_S
     * For (M/F)_s = 1/70, tau_1 = 4151/70 ≈ 59.3 s.
     * ---------------------------------------------------------- */
    CHECK_CLOSE(lvdc_igm_tau_1(V_ex1, 1.0 / 70.0),
                V_ex1 / 70.0, 1e-9,
                "Eq. 4.4.7 tau_1 scales linearly with (M/F)_s");
    CHECK_CLOSE(lvdc_igm_tau_1(V_ex1, 0.0), 0.0, 1e-15,
                "Eq. 4.4.7 tau_1 = 0 at zero (M/F)_s");

    /* ------------------------------------------------------------
     * Eq. 4.4.8: T_1i = T_1i - DeltaT
     * ---------------------------------------------------------- */
    CHECK_CLOSE(lvdc_igm_decrement_T_1i(100.0, 1.6), 98.4, 1e-12,
                "Eq. 4.4.8 decrement by DeltaT");

    /* ------------------------------------------------------------
     * Eq. 4.4.10: alpha_f = tau_10 / (T_10 + tau_1)
     * Sanity: tau_10 = tau_1 => alpha_f = tau_10 / (T_10 + tau_10).
     * ---------------------------------------------------------- */
    {
        const double tau_10 = 60.0;
        const double T_10 = 150.0;
        const double a = lvdc_igm_alpha_f(tau_10, T_10, tau_10);
        CHECK_CLOSE(a, tau_10 / (T_10 + tau_10), 1e-12,
                    "Eq. 4.4.10 alpha_f sanity");
        CHECK_CLOSE(lvdc_igm_alpha_f(60.0, 0.0, 0.0), 0.0, 0.0,
                    "Eq. 4.4.10 zero denominator returns 0");
    }

    /* ------------------------------------------------------------
     * Eq. 4.4.11: tau_3N = (M_GR - alpha_f * Mdot_2 * T_10) / Mdot_3
     * Analytical check with contrived inputs.
     * ---------------------------------------------------------- */
    {
        const double M_GR = 130000.0, alpha_f = 0.5;
        const double Mdot_2 = 200.0, T_10 = 100.0, Mdot_3 = 180.0;
        const double got = lvdc_igm_tau_3N(M_GR, alpha_f, Mdot_2,
                                           T_10, Mdot_3);
        const double expected =
            (M_GR - alpha_f * Mdot_2 * T_10) / Mdot_3;
        CHECK_CLOSE(got, expected, 1e-9, "Eq. 4.4.11 tau_3N");
    }

    /* ------------------------------------------------------------
     * Eq. 4.4.12 / 4.4.16: tau_3 = V_ex3 * (M/F)_s.
     * ---------------------------------------------------------- */
    CHECK_CLOSE(lvdc_igm_tau_3_real(V_ex3, 1.0 / 80.0),
                V_ex3 / 80.0, 1e-9,
                "Eq. 4.4.12 tau_3 real from (M/F)_s");

    /* ------------------------------------------------------------
     * Eq. 4.4.13: tau_3K = tau_3N - P_C
     * ---------------------------------------------------------- */
    CHECK_CLOSE(lvdc_igm_tau_3K(300.0, 10.0), 290.0, 1e-12,
                "Eq. 4.4.13 tau_3K");

    /* ------------------------------------------------------------
     * Eq. 4.4.14 blend endpoints:
     *   P_C = 0        -> tau_3 = tau_3K
     *   P_C = P_CMR    -> tau_3 = tau_3M
     *   P_C = P_CMR/2  -> blend weight = (0.5)^4 = 1/16
     * ---------------------------------------------------------- */
    {
        const double tau_3K = 290.0, tau_3M = 340.0, P_CMR = 35.0;
        CHECK_CLOSE(lvdc_igm_tau_3_blend(tau_3K, tau_3M, 0.0, P_CMR),
                    tau_3K, 1e-12,
                    "Eq. 4.4.14 blend at P_C=0 gives tau_3K");
        CHECK_CLOSE(lvdc_igm_tau_3_blend(tau_3K, tau_3M, P_CMR, P_CMR),
                    tau_3M, 1e-12,
                    "Eq. 4.4.14 blend at P_C=P_CMR gives tau_3M");
        const double mid = lvdc_igm_tau_3_blend(tau_3K, tau_3M,
                                                P_CMR / 2.0, P_CMR);
        const double expected = tau_3K + (tau_3M - tau_3K) * (1.0 / 16.0);
        CHECK_CLOSE(mid, expected, 1e-12,
                    "Eq. 4.4.14 blend at P_C=P_CMR/2 uses (1/2)^4 weight");

        /* P_CMR <= 0 degenerate case returns tau_3M immediately. */
        CHECK_CLOSE(lvdc_igm_tau_3_blend(tau_3K, tau_3M, 1.0, 0.0),
                    tau_3M, 1e-12,
                    "Eq. 4.4.14 P_CMR=0 returns tau_3M (degenerate)");
    }

    /* ------------------------------------------------------------
     * Eq. 4.4.15: P_C = P_C + DeltaT.
     * Eq. 4.4.17: T_3i = T_3i - DeltaT.
     * ---------------------------------------------------------- */
    CHECK_CLOSE(lvdc_igm_increment_P_C(10.0, 1.6), 11.6, 1e-12,
                "Eq. 4.4.15 P_C increments");
    CHECK_CLOSE(lvdc_igm_decrement_T_3i(100.0, 1.6), 98.4, 1e-12,
                "Eq. 4.4.17 T_3i decrements");

    if (g_fail_count == 0) {
        printf("test_igm_phasing_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_igm_phasing_smoke: %d CHECK(S) FAILED\n",
            g_fail_count);
    return 1;
}
