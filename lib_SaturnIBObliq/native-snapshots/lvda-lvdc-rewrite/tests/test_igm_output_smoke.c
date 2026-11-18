/*
 * test_igm_output_smoke.c — unit tests for EDD §4.4.2.2
 * Eqs. 4.4.83 through 4.4.87.
 *
 * Checks:
 *   1. Thrust-vector transform at MS4 = I and (chi_y4, chi_z4) = 0
 *      gives F'_S = (1, 0, 0) (pure +X direction).
 *   2. Thrust-vector transform gives a unit vector |F'_S| = 1.
 *   3. MS4 = I, chi_y4 = 0, chi_z4 = +pi/4 -> F'_S = (cos45, sin45, 0).
 *   4. chi_y_final inverts the forward thrust map for SMCY = 0:
 *      if we build F'_S from (chi_y4, 0) at MS4 = I, then
 *      lvdc_igm_chi_y_final(F'_S, 0) returns chi_y4.
 *   5. chi_z_final inverts the forward thrust map for SMCZ = 0.
 *   6. SMC additive: lvdc_igm_chi_*_final with SMC_x != 0 is the
 *      SMC_x = 0 result plus SMC_x.
 *   7. SMC update:
 *      a. On first call (primed = 0), chi_*_past is seeded to
 *         current so u = -SMC_*.
 *      b. Zero ẍ_m, z̈_m guard: den=0 skips SMCY update.
 *      c. SMCG = 0 leaves SMCY, SMCZ unchanged.
 *      d. SMC update latches current chi_* into _past for the
 *         next cycle.
 *   8. NULL SMC guard: silent no-op.
 */

#include "../lvdc_igm_output.h"
#include "../lvdc_igm_frames.h"

#include <math.h>
#include <stdio.h>

static int g_fail = 0;

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

#define CHECK(cond, label) do {                                       \
    if (!(cond)) {                                                    \
        fprintf(stderr, "FAIL [%s] %s:%d  !(%s)\n",                  \
                (label), __FILE__, __LINE__, #cond);                  \
        g_fail++;                                                     \
    }                                                                 \
} while (0)

static void identity_MS4(double MS4[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) MS4[i][j] = (i == j) ? 1.0 : 0.0;
}

int main(void) {
    /* --------------------------------------------------------------
     * 1. MS4 = I, chi_y4 = chi_z4 = 0: F'_S = (1, 0, 0).
     * ------------------------------------------------------------ */
    {
        double MS4[3][3];
        identity_MS4(MS4);
        double F[3];
        lvdc_igm_thrust_vector_S(MS4, 0.0, 0.0, F);
        CHECK_CLOSE(F[0], 1.0, 1e-15, "F'_S x at zero cmds");
        CHECK_CLOSE(F[1], 0.0, 1e-15, "F'_S y at zero cmds");
        CHECK_CLOSE(F[2], 0.0, 1e-15, "F'_S z at zero cmds");
    }

    /* --------------------------------------------------------------
     * 2. Thrust vector is always unit length.
     * ------------------------------------------------------------ */
    {
        double MS4[3][3];
        lvdc_igm_build_MS4(0.1, 0.49, 1.5708, 0.5236, MS4);
        const double chi_pairs[][2] = {
            { 0.0, 0.0 }, { 0.3, 0.0 }, { 0.0, 0.3 }, { 0.3, -0.2 },
            { 1.5, 0.05 }, { -0.5, 0.3 }
        };
        for (unsigned k = 0; k < sizeof(chi_pairs)/sizeof(chi_pairs[0]); k++) {
            double F[3];
            lvdc_igm_thrust_vector_S(MS4, chi_pairs[k][0],
                                     chi_pairs[k][1], F);
            const double mag = sqrt(F[0]*F[0] + F[1]*F[1] + F[2]*F[2]);
            CHECK_CLOSE(mag, 1.0, 1e-12, "F'_S is unit");
        }
    }

    /* --------------------------------------------------------------
     * 3. MS4 = I, chi_y4 = 0, chi_z4 = +pi/4.
     *    u_4 = (cos0 * cos45, sin45, -sin0 * cos45)
     *        = (cos45, sin45, 0)
     * ------------------------------------------------------------ */
    {
        double MS4[3][3];
        identity_MS4(MS4);
        double F[3];
        lvdc_igm_thrust_vector_S(MS4, 0.0, M_PI_4, F);
        CHECK_CLOSE(F[0], cos(M_PI_4), 1e-15, "F'_S x at chi_z=pi/4");
        CHECK_CLOSE(F[1], sin(M_PI_4), 1e-15, "F'_S y at chi_z=pi/4");
        CHECK_CLOSE(F[2], 0.0,         1e-15, "F'_S z at chi_z=pi/4");
    }

    /* --------------------------------------------------------------
     * 4. Round-trip: chi_y_final inverts chi_y4 at MS4 = I.
     *
     * With MS4 = I and chi_z4 = 0, F'_S = (cos chi_y4, 0, -sin chi_y4).
     * Then chi_y = atan2(sin chi_y4, cos chi_y4) + 0 = chi_y4.
     * ------------------------------------------------------------ */
    {
        double MS4[3][3];
        identity_MS4(MS4);
        for (double chi_y4 = -1.0; chi_y4 <= 1.0; chi_y4 += 0.1) {
            double F[3];
            lvdc_igm_thrust_vector_S(MS4, chi_y4, 0.0, F);
            const double chi_y = lvdc_igm_chi_y_final(F, 0.0);
            CHECK_CLOSE(chi_y, chi_y4, 1e-12,
                        "chi_y round-trip at chi_z4=0");
        }
    }

    /* --------------------------------------------------------------
     * 5. Round-trip: chi_z_final inverts chi_z4 at MS4 = I.
     *
     * With MS4 = I and chi_y4 = 0, F'_S = (cos chi_z4, sin chi_z4, 0).
     * Then chi_z = atan2(sin chi_z4, cos chi_z4) = chi_z4.
     * ------------------------------------------------------------ */
    {
        double MS4[3][3];
        identity_MS4(MS4);
        for (double chi_z4 = -1.0; chi_z4 <= 1.0; chi_z4 += 0.1) {
            double F[3];
            lvdc_igm_thrust_vector_S(MS4, 0.0, chi_z4, F);
            const double chi_z = lvdc_igm_chi_z_final(F, 0.0);
            CHECK_CLOSE(chi_z, chi_z4, 1e-12,
                        "chi_z round-trip at chi_y4=0");
        }
    }

    /* --------------------------------------------------------------
     * 6. SMCY / SMCZ are additive bias terms.
     * ------------------------------------------------------------ */
    {
        const double F[3] = { 0.7, 0.2, -0.1 };
        const double y0 = lvdc_igm_chi_y_final(F, 0.0);
        const double y1 = lvdc_igm_chi_y_final(F, 0.03);
        CHECK_CLOSE(y1 - y0, 0.03, 1e-15, "SMCY adds as bias");

        const double z0 = lvdc_igm_chi_z_final(F, 0.0);
        const double z1 = lvdc_igm_chi_z_final(F, -0.01);
        CHECK_CLOSE(z1 - z0, -0.01, 1e-15, "SMCZ adds as bias");
    }

    /* --------------------------------------------------------------
     * 7a. SMC update with SMCG = 0 leaves state unchanged (apart
     *     from past-latch and primed flag).
     * ------------------------------------------------------------ */
    {
        lvdc_igm_smc_state_t smc = {0};
        const double chi_y = 0.1, chi_z = 0.02;
        lvdc_igm_smc_update(&smc, /*SMCG*/ 0.0,
                            chi_y, chi_z,
                            25.0, -0.1, 0.05,
                            /*MoverF*/ 0.015,
                            /*dT*/ 1.6);
        CHECK_CLOSE(smc.SMCY, 0.0, 1e-15, "SMCG=0 leaves SMCY");
        CHECK_CLOSE(smc.SMCZ, 0.0, 1e-15, "SMCG=0 leaves SMCZ");
        CHECK(smc.primed == 1, "SMC primed after first call");
        CHECK_CLOSE(smc.chi_y_past, chi_y, 1e-15, "chi_y_past latched");
        CHECK_CLOSE(smc.chi_z_past, chi_z, 1e-15, "chi_z_past latched");
    }

    /* --------------------------------------------------------------
     * 7b. First-call priming: with primed=0, chi_past is seeded to
     *     current, so u_y = chi_y - SMCY. With chi_y = 0 and SMCY = 0,
     *     u_y = 0 => tan 0 = 0, leaving
     *       SMCY_update = SMCG * (z̈_m / ẍ_m) * ΔT/π.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_smc_state_t smc = {0};
        const double SMCG = 1.0, dT = 1.6;
        const double x_dd = 25.0, z_dd = 0.5;
        lvdc_igm_smc_update(&smc, SMCG, /*chi_y*/ 0.0, /*chi_z*/ 0.0,
                            x_dd, 0.0, z_dd,
                            0.0, dT);
        const double expected = SMCG * (z_dd / x_dd) * (dT / M_PI);
        CHECK_CLOSE(smc.SMCY, expected, 1e-12,
                    "SMCY first-call formula");
    }

    /* --------------------------------------------------------------
     * 7c. SMCZ first-call with MoverF = 0, SMCZ = 0:
     *     u_z = 0, sin 0 = 0, cos 0 = 1
     *     So SMCZ_update = SMCG * (0 - 0) / 1 * ΔT/π = 0.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_smc_state_t smc = {0};
        lvdc_igm_smc_update(&smc, /*SMCG*/ 2.0,
                            /*chi_y*/ 0.0, /*chi_z*/ 0.0,
                            25.0, 0.0, 0.0, /*MoverF*/ 0.0,
                            /*dT*/ 1.6);
        CHECK_CLOSE(smc.SMCZ, 0.0, 1e-15, "SMCZ zero-input result");
    }

    /* --------------------------------------------------------------
     * 7d. SMCZ first-call with nonzero MoverF * yddot:
     *     u_z = 0 - 0 = 0, so sin = 0, cos = 1.
     *     SMCZ += SMCG * (0 - (M/F) ÿ_m) / 1 * ΔT/π
     *           = -SMCG * (M/F) * ÿ_m * ΔT/π.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_smc_state_t smc = {0};
        const double SMCG = 0.5, dT = 1.6;
        const double MoverF = 0.015, y_dd = 0.2;
        lvdc_igm_smc_update(&smc, SMCG,
                            /*chi_y*/ 0.0, /*chi_z*/ 0.0,
                            25.0, y_dd, 0.0, MoverF, dT);
        const double expected = -SMCG * MoverF * y_dd * (dT / M_PI);
        CHECK_CLOSE(smc.SMCZ, expected, 1e-12,
                    "SMCZ nonzero-yddot formula");
    }

    /* --------------------------------------------------------------
     * 8. NULL state guard is a silent no-op (doesn't crash).
     * ------------------------------------------------------------ */
    lvdc_igm_smc_update(NULL, 1.0, 0.0, 0.0,
                        25.0, 0.0, 0.0, 0.0, 1.6);

    if (g_fail == 0) {
        printf("test_igm_output_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_igm_output_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
