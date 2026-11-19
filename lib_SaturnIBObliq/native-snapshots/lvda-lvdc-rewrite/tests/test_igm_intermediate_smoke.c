/*
 * test_igm_intermediate_smoke.c — unit tests for EDD §4.4.2 Eqs.
 * 4.4.20 through 4.4.28.
 *
 * The equations have no analytical "right answer" we can look up.
 * We verify them by:
 *   1. Dimensional consistency (sanity of ratios).
 *   2. Known limiting behaviour: T_i -> 0 drives L_i -> 0.
 *   3. AS-205 preset inputs give finite, non-negative L values.
 *   4. L'_y is the sum of L_1 and L'_3 (Eq. 4.4.27).
 *   5. Domain errors (tau < T_i) return -1.
 *
 * Exits 0 on success, non-zero on any failed check.
 */

#include "../lvdc_igm_intermediate.h"

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
    /* AS-205 preset values from TMX App B. */
    const double V_ex1 = 4151.0;
    const double V_ex3 = 4233.2;
    const double tau_1 = 286.9;   /* nominal first-phase ratio */
    const double tau_3 = 326.5;   /* nominal second-phase ratio */

    lvdc_igm_intermediate_t out;

    /* ------------------------------------------------------------
     * Scenario 1: T_i = 0 drives L_i to zero.
     * ln(tau / (tau - 0)) = ln(1) = 0, so L = 0.
     * ---------------------------------------------------------- */
    {
        int rc = lvdc_igm_intermediate_compute(V_ex1, tau_1, 0.0,
                                               V_ex3, tau_3, 0.0,
                                               &out);
        CHECK(rc == 0, "zero T_i returns success");
        CHECK_CLOSE(out.L_1,       0.0, 1e-12, "T_1i=0: L_1=0");
        CHECK_CLOSE(out.J_1,       0.0, 1e-12, "T_1i=0: J_1=0");
        CHECK_CLOSE(out.S_1,       0.0, 1e-12, "T_1i=0: S_1=0");
        CHECK_CLOSE(out.Q_1,       0.0, 1e-12, "T_1i=0: Q_1=0");
        CHECK_CLOSE(out.P_1,       0.0, 1e-12, "T_1i=0: P_1=0");
        CHECK_CLOSE(out.U_1,       0.0, 1e-12, "T_1i=0: U_1=0");
        CHECK_CLOSE(out.L_prime_3, 0.0, 1e-12, "T_3i=0: L'_3=0");
        CHECK_CLOSE(out.J_prime_3, 0.0, 1e-12, "T_3i=0: J'_3=0");
        CHECK_CLOSE(out.L_prime_y, 0.0, 1e-12, "T_i=0: L'_y=0");
    }

    /* ------------------------------------------------------------
     * Scenario 2: AS-205 mid-flight values give finite outputs.
     * Use T_1i = 50 s (some way into first phase) and T_3i = 150 s.
     * ---------------------------------------------------------- */
    {
        const double T_1i = 50.0;
        const double T_3i = 150.0;
        int rc = lvdc_igm_intermediate_compute(V_ex1, tau_1, T_1i,
                                               V_ex3, tau_3, T_3i,
                                               &out);
        CHECK(rc == 0, "AS-205 mid-flight values compute");

        /* L_1 = V_ex1 * ln(tau_1 / (tau_1 - T_1i)) > 0 */
        CHECK(out.L_1 > 0.0, "L_1 positive during burn");
        /* L should be smaller than V_ex (less than one time constant). */
        CHECK(out.L_1 < V_ex1, "L_1 < V_ex1 for T_i < tau");

        /* L'_y = L_1 + L'_3 */
        CHECK_CLOSE(out.L_prime_y, out.L_1 + out.L_prime_3, 1e-9,
                    "Eq. 4.4.27 L'_y = L_1 + L'_3");

        /* J_1 at this point: L_1*tau_1 - V_ex1*T_1i. */
        const double expected_J_1 = out.L_1 * tau_1 - V_ex1 * T_1i;
        CHECK_CLOSE(out.J_1, expected_J_1, 1e-6,
                    "Eq. 4.4.21 direct check");

        /* S_1 = L_1*T_1i - J_1 */
        const double expected_S_1 = out.L_1 * T_1i - out.J_1;
        CHECK_CLOSE(out.S_1, expected_S_1, 1e-6,
                    "Eq. 4.4.22 direct check");
    }

    /* ------------------------------------------------------------
     * Scenario 3: domain error when tau <= T_i.
     * ---------------------------------------------------------- */
    {
        int rc = lvdc_igm_intermediate_compute(V_ex1, tau_1, tau_1 + 1.0,
                                               V_ex3, tau_3, 0.0,
                                               &out);
        CHECK(rc == -1, "T_1i > tau_1 returns domain error");

        rc = lvdc_igm_intermediate_compute(V_ex1, tau_1, tau_1,
                                           V_ex3, tau_3, 0.0,
                                           &out);
        CHECK(rc == -1, "T_1i = tau_1 returns domain error");

        rc = lvdc_igm_intermediate_compute(V_ex1, tau_1, 0.0,
                                           V_ex3, tau_3, tau_3 + 0.1,
                                           &out);
        CHECK(rc == -1, "T_3i > tau_3 returns domain error");
    }

    /* ------------------------------------------------------------
     * Scenario 4: monotonic L growth with T_i.
     * As T_i grows from 0 to near tau, L grows monotonically.
     * ---------------------------------------------------------- */
    {
        double L_prev = -1.0;
        for (int step = 0; step <= 10; step++) {
            const double T_1i = (step / 10.0) * (tau_1 * 0.95);
            int rc = lvdc_igm_intermediate_compute(V_ex1, tau_1, T_1i,
                                                   V_ex3, tau_3, 0.0,
                                                   &out);
            CHECK(rc == 0, "monotonic L: compute succeeds");
            CHECK(out.L_1 >= L_prev,
                  "monotonic L: L_1 grows with T_1i");
            L_prev = out.L_1;
        }
    }

    if (g_fail_count == 0) {
        printf("test_igm_intermediate_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_igm_intermediate_smoke: %d CHECK(S) FAILED\n",
            g_fail_count);
    return 1;
}
