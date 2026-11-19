/*
 * test_hsl_smoke.c — unit tests for the HSL (EDD §4.4.2.4
 * Eqs. 4.4.88-94).
 *
 * The HSL is a quadratic predictor driven by three V samples.
 * We feed it known analytic velocity profiles and check:
 *   1. Entry latches only when T_3i <= eps_prime.
 *   2. Constant acceleration (V = V_0 + A*t) produces the
 *      correct linear T_3i prediction via Eq. 4.4.93.
 *   3. Cutoff edge fires when the prediction crosses zero.
 *   4. Degenerate inputs don't produce NaN.
 */

#include "../lvdc_hsl.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int g_fail = 0;

#define CHECK(cond, label) do {                                       \
    if (!(cond)) {                                                    \
        fprintf(stderr, "FAIL [%s] %s:%d  !(%s)\n",                  \
                (label), __FILE__, __LINE__, #cond);                  \
        g_fail++;                                                     \
    }                                                                 \
} while (0)

#define CHECK_CLOSE(got, expected, tol, label) do {                   \
    const double _g = (got);                                          \
    const double _e = (expected);                                     \
    if (fabs(_g - _e) > (tol)) {                                      \
        fprintf(stderr,                                               \
                "FAIL [%s] %s:%d got %.9g expected %.9g\n",           \
                (label), __FILE__, __LINE__, _g, _e);                 \
        g_fail++;                                                     \
    }                                                                 \
} while (0)

int main(void) {
    const double eps_prime = 5.0;
    const double V_T       = 7780.67;
    const double dV_b      = 7.2381;
    const double A         = 5.0;        /* m/s^2 — constant S-IVB accel */
    const double dT        = 1.6;        /* BML period */

    /* --------------------------------------------------------------
     * 1. Entry only when T_3i <= eps_prime.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);

        /* IGM says T_3i = 20 s — no entry. */
        bool edge = lvdc_hsl_step(&s, 100.0, V_T - 100.0,
                                   /*T_3i*/ 20.0,
                                   eps_prime, V_T, dV_b);
        CHECK(!s.active, "T_3i > eps: HSL stays idle");
        CHECK(!edge, "no edge yet");

        /* T_3i drops below eps — entry latches. */
        edge = lvdc_hsl_step(&s, 101.6, V_T - 95.0,
                             /*T_3i*/ 4.0,
                             eps_prime, V_T, dV_b);
        CHECK(s.active, "T_3i < eps: HSL enters");
    }

    /* --------------------------------------------------------------
     * 2. Constant-accel analytic: with V = V_T - delta_V and
     *    A' = A, A'' = 0, Eq. 4.4.93 gives
     *      T_3i = (V_T - dV_b - V) / A.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);

        /* Three samples at constant accel A.
         * Choose so T_3i_igm = 4.0 at start. */
        double t = 100.0;
        double V = V_T - 20.0;  /* 20 m/s remaining */
        /* Sample 1 */
        lvdc_hsl_step(&s, t, V, /*T_3i*/ 4.0,
                      eps_prime, V_T, dV_b);
        /* Sample 2 */
        t += dT; V += A * dT;
        lvdc_hsl_step(&s, t, V, /*T_3i*/ 4.0 - dT,
                      eps_prime, V_T, dV_b);
        /* Sample 3 — now Eq. 4.4.93 kicks in. */
        t += dT; V += A * dT;
        lvdc_hsl_step(&s, t, V, /*T_3i*/ 4.0 - 2*dT,
                      eps_prime, V_T, dV_b);

        /* Expected T_3i: (V_T - dV_b - V) / A. */
        const double expected = (V_T - dV_b - V) / A;
        CHECK_CLOSE(lvdc_hsl_T_3i(&s), expected, 0.05,
                    "quadratic predictor matches linear case");
    }

    /* --------------------------------------------------------------
     * 3. Cutoff edge fires when prediction crosses zero.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);

        /* Drive V up to and past V_T - dV_b. */
        double t = 100.0;
        double V = V_T - 50.0;
        bool fired = false;
        for (int i = 0; i < 20 && !fired; i++) {
            fired = lvdc_hsl_step(&s, t, V, /*T_3i*/ 4.0 - i*dT,
                                   eps_prime, V_T, dV_b);
            t += dT;
            V += A * dT;
        }
        CHECK(fired, "cutoff edge fired");
    }

    /* --------------------------------------------------------------
     * 4. Degenerate — identical samples give zero V-delta.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);
        for (int i = 0; i < 4; i++) {
            lvdc_hsl_step(&s, 100.0 + i*dT, V_T - 5.0,
                          /*T_3i*/ 1.0,
                          eps_prime, V_T, dV_b);
        }
        /* No NaN expected; prediction may be zero or negative. */
        const double T = lvdc_hsl_T_3i(&s);
        CHECK(isfinite(T), "degenerate: T_3i finite");
    }

    /* --------------------------------------------------------------
     * 5. NULL guards.
     * ------------------------------------------------------------ */
    lvdc_hsl_reset(NULL);
    CHECK(!lvdc_hsl_step(NULL, 0, 0, 0, 0, 0, 0),
          "NULL state step returns false");
    CHECK_CLOSE(lvdc_hsl_T_3i(NULL), 0.0, 1e-15, "NULL T_3i");
    CHECK(!lvdc_hsl_active(NULL), "NULL active");

    if (g_fail == 0) {
        printf("test_hsl_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_hsl_smoke: %d CHECK(S) FAILED\n", g_fail);
    return 1;
}
