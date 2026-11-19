/*
 * test_hsl_smoke.c — unit tests for the two-rate HSL
 * (EDD §4.4.2.4 Eqs. 4.4.88-94).
 *
 * The EDD prescribes BML-rate math (Eqs. 4.4.88-94) separate
 * from a minor-loop (40 ms) decrement-and-test. We exercise
 * both and verify:
 *   1. V-history accumulation starts when T_3i <= BN_1.
 *   2. HSL entry requires BOTH T_3i < T_HSL + ΔT_b AND
 *      vigained < V_GRD.
 *   3. Constant-accel analytic (V = V_0 + A*t): Eq. 4.4.93
 *      reduces to T_3i = (V_T - ΔV_b - V) / A.
 *   4. Minor-loop decrement fires cutoff edge when T_3i_hsl
 *      drops below 60 ms (EDD §4.4.2.4 ¶7).
 *   5. Edge does NOT fire while HSL is inactive.
 *   6. NULL guards.
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
    const double T_HSL  = 5.0;
    const double V_GRD  = 150.0;
    const double BN_1   = 14.4;
    const double V_T    = 7780.67;
    const double dV_b   = 7.2381;
    const double dT_BML = 1.6;
    const double dt_min = 0.040;
    const double A      = 20.0;   /* m/s^2, constant S-IVB-ish accel */

    /* --------------------------------------------------------------
     * 1. Accumulation starts at T_3i <= BN_1.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);

        /* T_3i = 20 > BN_1 = 14.4 — no accumulation. */
        lvdc_hsl_bml_step(&s, 100.0, V_T - 500.0,
                          /*T_3i*/ 20.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        CHECK(!lvdc_hsl_accumulating(&s),
              "T_3i > BN_1: not accumulating");

        /* T_3i = 10 < BN_1 — accumulation starts. */
        lvdc_hsl_bml_step(&s, 101.6, V_T - 480.0,
                          /*T_3i*/ 10.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        CHECK(lvdc_hsl_accumulating(&s),
              "T_3i <= BN_1: accumulating");
        CHECK(!lvdc_hsl_active(&s),
              "still too early to be active");
    }

    /* --------------------------------------------------------------
     * 2. Entry requires BOTH conditions.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);

        /* Accumulate. */
        lvdc_hsl_bml_step(&s, 100.0, V_T - 500.0, 10.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        lvdc_hsl_bml_step(&s, 101.6, V_T - 470.0, 8.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        CHECK(!lvdc_hsl_active(&s),
              "T_3i < T_HSL window but vigained > V_GRD: no entry");

        /* Drop T_3i below T_HSL but keep vigained large. */
        lvdc_hsl_bml_step(&s, 103.2, V_T - 300.0, 3.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        CHECK(!lvdc_hsl_active(&s),
              "T_3i small but vigained > V_GRD: no entry");

        /* Now drop vigained. */
        lvdc_hsl_bml_step(&s, 104.8, V_T - 100.0, 3.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        CHECK(lvdc_hsl_active(&s),
              "both conditions met: HSL entered");
    }

    /* --------------------------------------------------------------
     * 3. Constant-accel analytic (post-entry, 3+ samples).
     *    Build a stream with V_i = V_0 + A*(t_i - t_0) and verify
     *    Eq. 4.4.93 converges to T_3i = (V_T - dV_b - V) / A.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);

        double t = 100.0;
        double V = V_T - 60.0;  /* just inside V_GRD = 150 */

        /* Pre-entry sample so accumulation is on. */
        lvdc_hsl_bml_step(&s, t, V, 10.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);

        /* Walk toward entry across three BMLs so the ring fills
         * BEFORE entry — ensures the first refinement pass has
         * a full history. */
        for (int k = 0; k < 3; k++) {
            t += dT_BML;
            V += A * dT_BML;
            lvdc_hsl_bml_step(&s, t, V, /*T_3i*/ 8.0 - k*dT_BML,
                              T_HSL, V_GRD, BN_1, V_T, dV_b);
        }
        /* Now trigger entry with a sample that satisfies both. */
        t += dT_BML;
        V += A * dT_BML;
        lvdc_hsl_bml_step(&s, t, V, /*T_3i*/ 3.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        CHECK(lvdc_hsl_active(&s), "entered after walk");

        const double expected = (V_T - dV_b - V) / A;
        CHECK_CLOSE(lvdc_hsl_T_3i(&s), expected, 0.3,
                    "constant-accel: Eq. 4.4.93 matches analytic");
    }

    /* --------------------------------------------------------------
     * 4. Minor-loop decrement fires cutoff when T_3i < 60 ms.
     *    Use a slow-accel scenario so T_3i is positive at entry.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);
        const double A_slow = 5.0;   /* m/s^2 — slower than #3 */
        double t = 100.0, V = V_T - 140.0;  /* vigained=140 < V_GRD */
        /* Pre-entry walk to fill the ring. */
        lvdc_hsl_bml_step(&s, t, V, 10.0, T_HSL, V_GRD, BN_1, V_T, dV_b);
        for (int k = 0; k < 3; k++) {
            t += dT_BML; V += A_slow * dT_BML;
            lvdc_hsl_bml_step(&s, t, V, 8.0 - k*dT_BML,
                              T_HSL, V_GRD, BN_1, V_T, dV_b);
        }
        /* Entry trigger. V = V_T - 140 + 24 = V_T - 116. Still
         * well short of V_T (positive T_3i_hsl). */
        t += dT_BML; V += A_slow * dT_BML;
        lvdc_hsl_bml_step(&s, t, V, 3.0,
                          T_HSL, V_GRD, BN_1, V_T, dV_b);
        CHECK(lvdc_hsl_active(&s), "active");

        const double initial_T3i = lvdc_hsl_T_3i(&s);
        /* At V = V_T - 108, A = 5: T_3i ≈ (108 - 7.24) / 5 ≈ 20.2 s.
         * Minor-loop ticks to fire: ~ 20.2 / 0.04 = 505. */
        int steps = 0;
        bool fired = false;
        while (steps < 100000 && !fired) {
            fired = lvdc_hsl_minor_step(&s, dt_min);
            steps++;
        }
        CHECK(fired, "cutoff edge fires");
        CHECK(lvdc_hsl_T_3i(&s) < 0.060 + 1e-9,
              "T_3i_hsl below 60 ms at fire time");

        const double expected_steps = initial_T3i / dt_min;
        if (initial_T3i > 0.0
            && fabs(steps - expected_steps) >= 10) {
            fprintf(stderr,
                    "FAIL [fired step count] steps=%d expected=%.1f initial_T3i=%.3f\n",
                    steps, expected_steps, initial_T3i);
            g_fail++;
        }
    }

    /* --------------------------------------------------------------
     * 5. Inactive HSL doesn't fire from minor step.
     * ------------------------------------------------------------ */
    {
        lvdc_hsl_state_t s;
        lvdc_hsl_reset(&s);
        CHECK(!lvdc_hsl_minor_step(&s, dt_min),
              "inactive HSL: minor step doesn't fire");
    }

    /* --------------------------------------------------------------
     * 6. NULL guards.
     * ------------------------------------------------------------ */
    lvdc_hsl_reset(NULL);
    lvdc_hsl_bml_step(NULL, 0, 0, 0, 0, 0, 0, 0, 0);
    CHECK(!lvdc_hsl_minor_step(NULL, dt_min), "NULL minor step");
    CHECK_CLOSE(lvdc_hsl_T_3i(NULL), 0.0, 1e-15, "NULL T_3i");
    CHECK(!lvdc_hsl_active(NULL), "NULL active");
    CHECK(!lvdc_hsl_accumulating(NULL), "NULL accumulating");

    if (g_fail == 0) {
        printf("test_hsl_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_hsl_smoke: %d CHECK(S) FAILED\n", g_fail);
    return 1;
}
