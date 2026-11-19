/*
 * test_chi_to_psi_smoke.c — unit tests for the gimbal-to-body
 * transform (EDD §8.3.2 + Eq. 8.2.2).
 *
 * Exits 0 on success, non-zero on any failed check.
 */

#include "../lvdc_chi_to_psi.h"

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
    const double deg2rad = M_PI / 180.0;
    double chi_cmd[3];
    double chi_pr[3];
    double theta_pr[3];
    double psi[3];

    /* ------------------------------------------------------------
     * Scenario 1: zero error.
     * If `theta' == chi'`, the attitude error is zero in all axes.
     * ---------------------------------------------------------- */
    chi_cmd[0]  = chi_cmd[1]  = chi_cmd[2]  = 0.1;
    chi_pr[0]   = chi_pr[1]   = chi_pr[2]   = 0.1;
    theta_pr[0] = theta_pr[1] = theta_pr[2] = 0.1;
    lvdc_chi_to_psi_transform(chi_pr, theta_pr, psi);
    CHECK_CLOSE(psi[0], 0.0, 1e-15, "zero error: ROLL = 0");
    CHECK_CLOSE(psi[1], 0.0, 1e-15, "zero error: PITCH = 0");
    CHECK_CLOSE(psi[2], 0.0, 1e-15, "zero error: YAW = 0");

    /* ------------------------------------------------------------
     * Scenario 2: nominal pad attitude, pure pitch error.
     * At liftoff with chi_cmd = chi_pr = theta_pr = 0 in pitch and
     * yaw, and a 1 deg pitch error, the transform must reduce to:
     *   ROLL  = A_3 * dY  = sin(theta_za) * dY  = 0 * dY  = 0
     *   PITCH = A_1 * dY  = cos(0)*cos(0) * dY  = dY
     *   YAW   = -A_4 * dY = -sin(0)*cos(0) * dY = 0
     * ---------------------------------------------------------- */
    chi_cmd[0]  = chi_cmd[1]  = chi_cmd[2]  = 0.0;
    chi_pr[0]   = chi_pr[1]   = chi_pr[2]   = 0.0;
    theta_pr[0] = 0.0;
    theta_pr[1] = 1.0 * deg2rad;  /* pitch error */
    theta_pr[2] = 0.0;
    lvdc_chi_to_psi_transform(chi_pr, theta_pr, psi);
    CHECK_CLOSE(psi[0], 0.0, 1e-15, "pure pitch: ROLL = 0 at zero average");
    CHECK_CLOSE(psi[1], 1.0 * deg2rad, 1e-15,
                "pure pitch: PITCH = dY at zero average");
    CHECK_CLOSE(psi[2], 0.0, 1e-15, "pure pitch: YAW = 0 at zero average");

    /* ------------------------------------------------------------
     * Scenario 3: pure yaw error at zero average.
     * dZ = 1 deg, theta_xa = theta_za = 0:
     *   ROLL  = A_6 * dX + A_3 * dY = 0
     *   PITCH = A_2 * dZ = sin(0) * dZ = 0
     *   YAW   = A_5 * dZ = cos(0) * dZ = dZ
     * ---------------------------------------------------------- */
    chi_cmd[0]  = chi_cmd[1]  = chi_cmd[2]  = 0.0;
    chi_pr[0]   = chi_pr[1]   = chi_pr[2]   = 0.0;
    theta_pr[0] = 0.0;
    theta_pr[1] = 0.0;
    theta_pr[2] = 1.0 * deg2rad;  /* yaw error */
    lvdc_chi_to_psi_transform(chi_pr, theta_pr, psi);
    CHECK_CLOSE(psi[0], 0.0, 1e-15, "pure yaw: ROLL = 0");
    CHECK_CLOSE(psi[1], 0.0, 1e-15,
                "pure yaw: PITCH = 0 at zero average");
    CHECK_CLOSE(psi[2], 1.0 * deg2rad, 1e-15,
                "pure yaw: YAW = dZ at zero average");

    /* ------------------------------------------------------------
     * Scenario 4: pure roll error at zero average.
     * dX = 1 deg, theta_xa = theta_za = 0, dY = dZ = 0:
     *   ROLL  = A_6 * dX + A_3 * dY = dX + 0 = dX
     *   PITCH = A_1 * dY + A_2 * dZ = 0
     *   YAW   = A_5 * dZ - A_4 * dY = 0
     * ---------------------------------------------------------- */
    chi_cmd[0]  = chi_cmd[1]  = chi_cmd[2]  = 0.0;
    chi_pr[0]   = chi_pr[1]   = chi_pr[2]   = 0.0;
    theta_pr[0] = 1.0 * deg2rad;  /* roll error */
    theta_pr[1] = 0.0;
    theta_pr[2] = 0.0;
    lvdc_chi_to_psi_transform(chi_pr, theta_pr, psi);
    CHECK_CLOSE(psi[0], 1.0 * deg2rad, 1e-15, "pure roll: ROLL = dX");
    CHECK_CLOSE(psi[1], 0.0, 1e-15, "pure roll: PITCH = 0");
    CHECK_CLOSE(psi[2], 0.0, 1e-15, "pure roll: YAW = 0");

    /* ------------------------------------------------------------
     * Scenario 5: pitch during pitched-over flight.
     * Vehicle is 60 deg pitched over (chi_cmd = theta = -60 deg
     * pitch), so theta_xa = theta_za = 0 (pitch does not appear in
     * the average angles). A 0.5 deg pitch error must still map
     * directly to PITCH, because the transform is only coupled
     * through roll and yaw.
     * ---------------------------------------------------------- */
    chi_cmd[0]  = 0.0;
    chi_cmd[1]  = -60.0 * deg2rad;
    chi_cmd[2]  = 0.0;
    chi_pr[0]   = 0.0;
    chi_pr[1]   = -60.0 * deg2rad;
    chi_pr[2]   = 0.0;
    theta_pr[0] = 0.0;
    theta_pr[1] = -60.0 * deg2rad + 0.5 * deg2rad;  /* 0.5 deg error */
    theta_pr[2] = 0.0;
    lvdc_chi_to_psi_transform(chi_pr, theta_pr, psi);
    CHECK_CLOSE(psi[1], 0.5 * deg2rad, 1e-12,
                "pitched-over pitch error: PITCH = dY");

    /* ------------------------------------------------------------
     * Scenario 6: roll and yaw coupling through A_3 and A_4.
     * With theta_za = 45 deg (large yaw average), a pitch error
     * (dY) feeds into ROLL via A_3 = sin(45) and into YAW via
     * -A_4 = -sin(theta_xa)*cos(45). Choose theta_xa = 0 so A_4=0:
     *   ROLL  = A_6*dX + A_3*dY = 0 + sin(45)*dY
     * ---------------------------------------------------------- */
    chi_cmd[0]  = 0.0;
    chi_cmd[1]  = 0.0;
    chi_cmd[2]  = 90.0 * deg2rad;  /* so theta_za = 45 deg */
    chi_pr[0]   = 0.0;
    chi_pr[1]   = 0.0;
    chi_pr[2]   = 90.0 * deg2rad;
    theta_pr[0] = 0.0;
    theta_pr[1] = 1.0 * deg2rad;
    theta_pr[2] = 0.0;              /* dZ = 0 - 90 deg; irrelevant to ROLL */
    lvdc_chi_to_psi_transform(chi_pr, theta_pr, psi);
    const double expect_roll = sin(45.0 * deg2rad) * (1.0 * deg2rad);
    CHECK_CLOSE(psi[0], expect_roll, 1e-15,
                "pitch-to-roll coupling via A_3 = sin(theta_za)");

    if (g_fail_count == 0) {
        printf("test_chi_to_psi_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_chi_to_psi_smoke: %d CHECK(S) FAILED\n",
            g_fail_count);
    return 1;
}
