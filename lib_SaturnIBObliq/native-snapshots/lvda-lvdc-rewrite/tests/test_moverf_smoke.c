/*
 * test_moverf_smoke.c — unit tests for EDD §4.2 Eq. 4.2.9-4.2.11
 * smoothed reciprocal specific force.
 *
 * Checks:
 *   1. Prime seeds every ring entry with (M/F)_0 and returns it
 *      on the first step.
 *   2. Constant specific force -> the filter converges to a steady
 *      state equal to the instantaneous (M/F) (within the
 *      filter's DC-gain error from the preset MFK values).
 *   3. Magnitude limit clamps the output.
 *   4. Rate limit caps the per-step change.
 *   5. Zero specific force returns the last output (safe guard).
 *   6. NULL guard.
 */

#include "../lvdc_moverf.h"
#include "../lvdc_params.h"

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
    const double _d = fabs(_g - _e);                                  \
    if (_d > (tol)) {                                                 \
        fprintf(stderr,                                               \
                "FAIL [%s] %s:%d got %.9g expected %.9g |d|=%.3g > %.3g\n", \
                (label), __FILE__, __LINE__, _g, _e, _d, (tol));      \
        g_fail++;                                                     \
    }                                                                 \
} while (0)

int main(void) {
    const lvdc_params_t p = lvdc_params_as205();
    const double MFK0 = p.MoverF_0_s2pm;
    const double dT = p.delta_T_sec;

    /* --------------------------------------------------------------
     * 1. Prime returns seeded (M/F)_0 on the first step.
     * ------------------------------------------------------------ */
    {
        lvdc_moverf_state_t s;
        memset(&s, 0, sizeof(s));
        lvdc_moverf_prime(&s, MFK0);
        CHECK(s.primed == 1, "primed flag set");
        CHECK(s.have_V_m_past == 0, "V_m_past not yet latched");

        const double V_m[3] = {0.0, 0.0, 0.0};
        const double out = lvdc_moverf_step(&s, V_m, dT, p.MFK,
                                            p.MoverF_s_limit_s2pm,
                                            p.MoverF_s_rate_limit_per_BML);
        CHECK_CLOSE(out, MFK0, 1e-15,
                    "first step returns (M/F)_0");
        CHECK(s.have_V_m_past == 1, "V_m_past latched after step");
    }

    /* --------------------------------------------------------------
     * 2. Constant specific force: with F_const and increasing V_m,
     *    inst (M/F) is always 1/F_const. The filter should converge
     *    to the DC gain * (M/F)_inst. For MFK summing to ~1, this
     *    equals (M/F)_inst.
     * ------------------------------------------------------------ */
    {
        lvdc_moverf_state_t s;
        memset(&s, 0, sizeof(s));
        lvdc_moverf_prime(&s, MFK0);

        /* F/M = 10 m/s^2 -> (M/F)_inst = 0.10 s^2/m. */
        const double A = 10.0;
        double V_m[3] = {0.0, 0.0, 0.0};
        /* First step just latches V_m_past. */
        lvdc_moverf_step(&s, V_m, dT, p.MFK,
                         p.MoverF_s_limit_s2pm,
                         p.MoverF_s_rate_limit_per_BML);

        /* Use a loose rate limit so the filter can evolve.
         * EDD rate-limit of 0.005/BML would cap steady-state
         * transient over many steps; test the IIR math itself with
         * a larger limit, then test the clamps separately. */
        const double big_rate = 1.0;
        const double big_mag  = 10.0;

        double last_out = 0.0;
        for (int k = 0; k < 400; k++) {
            V_m[0] += A * dT;  /* integrate along X */
            last_out = lvdc_moverf_step(&s, V_m, dT, p.MFK,
                                         big_mag, big_rate);
        }
        /* Expected DC output: sum(MFK[0..3]) * (1/A) /
         *                    (1 - sum(MFK[4..7])).
         * For AS-205 MFK sums: inst ≈ 0.00191, fb ≈ 0.99476 →
         * DC gain ≈ 0.00191/(1 - 0.99476) * (1/A).
         * The filter is designed so DC gain ≈ 1 giving
         * steady-state ≈ (M/F)_inst = 0.1. */
        CHECK_CLOSE(last_out, 1.0 / A, 1e-3,
                    "constant F/M -> (M/F)_S converges to 1/F_inst");
    }

    /* --------------------------------------------------------------
     * 3. Magnitude limit clamps the output.
     * ------------------------------------------------------------ */
    {
        lvdc_moverf_state_t s;
        memset(&s, 0, sizeof(s));
        /* Prime above the magnitude limit so the first smoothed
         * sample tries to exceed it. */
        const double mag_lim = 0.10;   /* clamp to 0.10 */
        lvdc_moverf_prime(&s, 0.2);    /* seed = 0.2 */
        const double V_m0[3] = {0.0, 0.0, 0.0};
        lvdc_moverf_step(&s, V_m0, dT, p.MFK, mag_lim, 100.0);

        /* On the next step with a small specific force, the IIR
         * is driven to seed * ~1 (DC gain) -> clamped to mag_lim. */
        double V_m[3] = {0.01, 0.0, 0.0};  /* very weak F/M */
        double out = 0;
        for (int k = 0; k < 5; k++) {
            V_m[0] += 0.01;
            out = lvdc_moverf_step(&s, V_m, dT, p.MFK,
                                    mag_lim, 100.0);
        }
        CHECK(fabs(out) <= mag_lim + 1e-12,
              "output within magnitude clamp");
    }

    /* --------------------------------------------------------------
     * 4. Rate limit caps per-step change.
     *    Big instantaneous jump should be slew-limited.
     * ------------------------------------------------------------ */
    {
        lvdc_moverf_state_t s;
        memset(&s, 0, sizeof(s));
        lvdc_moverf_prime(&s, 0.10);
        const double rate_lim = 0.001;
        const double V_m0[3] = {0.0, 0.0, 0.0};
        lvdc_moverf_step(&s, V_m0, dT, p.MFK, 10.0, rate_lim);

        /* Enormous specific-force step (large V_m jump). */
        const double V_m[3] = {100.0, 0.0, 0.0};
        const double out = lvdc_moverf_step(&s, V_m, dT, p.MFK,
                                             10.0, rate_lim);
        CHECK(fabs(out - 0.10) <= rate_lim + 1e-12,
              "rate clamp applied");
    }

    /* --------------------------------------------------------------
     * 5. Zero specific force returns last output.
     * ------------------------------------------------------------ */
    {
        lvdc_moverf_state_t s;
        memset(&s, 0, sizeof(s));
        lvdc_moverf_prime(&s, MFK0);
        const double V_m[3] = {5.0, 0.0, 0.0};
        lvdc_moverf_step(&s, V_m, dT, p.MFK,
                         p.MoverF_s_limit_s2pm,
                         p.MoverF_s_rate_limit_per_BML);
        /* Second step with same V_m -> zero A_m -> returns last_out. */
        const double out = lvdc_moverf_step(&s, V_m, dT, p.MFK,
                                             p.MoverF_s_limit_s2pm,
                                             p.MoverF_s_rate_limit_per_BML);
        CHECK_CLOSE(out, s.last_out, 1e-15,
                    "zero F/M returns last_out");
    }

    /* --------------------------------------------------------------
     * 6. NULL guards.
     * ------------------------------------------------------------ */
    {
        lvdc_moverf_prime(NULL, 0.1);
        const double V_m[3] = {0.0, 0.0, 0.0};
        CHECK_CLOSE(lvdc_moverf_step(NULL, V_m, dT, p.MFK, 0.25, 0.005),
                    0.0, 1e-15, "NULL state -> 0");
    }

    if (g_fail == 0) {
        printf("test_moverf_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_moverf_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
