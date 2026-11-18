/*
 * test_preigm_smoke.c — unit tests for EDD §4.4.1 first-stage
 * guidance.
 *
 * Scenarios:
 *   1. Before gantry clear: chi_y = 0, chi_x = pad roll.
 *   2. After gantry clear: chi_x = 0.
 *   3. Each polynomial segment at its boundary matches the current
 *      body's expression for that T_c.
 *   4. After T_S3: chi_y = P_final.
 *   5. Continuity across segment boundaries (step < 1 deg).
 *
 * Exits 0 on success, non-zero on any failed check.
 */

#include "../lvdc_params.h"
#include "../lvdc_preigm.h"

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

/* Reference cubic evaluator — matches the current body's
 * LVDC_Chi_command_angles_Time_Tilt_F_i expressions exactly. */
static double eval_cubic(const double a[4], double t) {
    return a[0] + t * (a[1] + t * (a[2] + t * a[3]));
}

int main(void) {
    const lvdc_params_t p = lvdc_params_as205();
    const double pad_roll_rad = 100.0 * M_PI / 180.0;  /* 100° */
    double chi[3];

    /* ------------------------------------------------------------
     * Scenario 1: before gantry clear.
     * ---------------------------------------------------------- */
    lvdc_preigm_chi_cmd(&p, 0.0, pad_roll_rad, chi);
    CHECK_CLOSE(chi[0], pad_roll_rad, 1e-12,
                "pre-gantry chi_x holds pad roll");
    CHECK_CLOSE(chi[1], 0.0, 1e-12, "pre-gantry chi_y = 0");
    CHECK_CLOSE(chi[2], 0.0, 1e-12, "pre-gantry chi_z = 0");

    lvdc_preigm_chi_cmd(&p, 9.9, pad_roll_rad, chi);
    CHECK_CLOSE(chi[0], pad_roll_rad, 1e-12,
                "still-pre-gantry chi_x holds pad roll at T_c=9.9");
    CHECK_CLOSE(chi[1], 0.0, 1e-12, "still-pre-gantry chi_y = 0");

    /* ------------------------------------------------------------
     * Scenario 2: after gantry clear, chi_x goes to zero.
     * ---------------------------------------------------------- */
    lvdc_preigm_chi_cmd(&p, 10.1, pad_roll_rad, chi);
    CHECK_CLOSE(chi[0], 0.0, 1e-12, "post-gantry chi_x = 0");

    /* ------------------------------------------------------------
     * Scenario 3: polynomial segment outputs.
     * Check that chi_y at mid-segment matches the reference cubic
     * exactly.  Each call must select the right segment.
     * ---------------------------------------------------------- */
    const double t_mid_1 = 0.5 * (p.T_S1_sec1_sec + p.T_S1_sec);
    const double t_mid_2 = 0.5 * (p.T_S1_sec       + p.T_S2_sec);
    const double t_mid_3 = 0.5 * (p.T_S2_sec       + p.T_S3_sec);

    CHECK_CLOSE(lvdc_preigm_chi_y(&p, t_mid_1),
                eval_cubic(p.chi_y_F1, t_mid_1), 1e-15,
                "chi_y segment 1 matches F_1 at mid-segment");
    CHECK_CLOSE(lvdc_preigm_chi_y(&p, t_mid_2),
                eval_cubic(p.chi_y_F2, t_mid_2), 1e-15,
                "chi_y segment 2 matches F_2 at mid-segment");
    CHECK_CLOSE(lvdc_preigm_chi_y(&p, t_mid_3),
                eval_cubic(p.chi_y_F3, t_mid_3), 1e-15,
                "chi_y segment 3 matches F_3 at mid-segment");

    /* ------------------------------------------------------------
     * Scenario 4: after T_S3, chi_y freezes at P_final.
     * ---------------------------------------------------------- */
    CHECK_CLOSE(lvdc_preigm_chi_y(&p, p.T_S3_sec + 0.01),
                p.P_final_rad, 1e-12,
                "chi_y freezes at P_final after T_S3");
    CHECK_CLOSE(lvdc_preigm_chi_y(&p, 1000.0),
                p.P_final_rad, 1e-12,
                "chi_y stays at P_final far past T_S3");

    /* ------------------------------------------------------------
     * Scenario 5: continuity at segment boundaries.
     * The EDD Part II coefficients produce near-continuous output
     * across segment joins.  Allow at most 1° step so a bad
     * coefficient transcription is caught, but tolerate the EDD's
     * own small discontinuity.
     * ---------------------------------------------------------- */
    const double eps = 1e-9;
    const double deg_1 = (180.0 / M_PI);

    const double step_12 = fabs(
        lvdc_preigm_chi_y(&p, p.T_S1_sec + eps) -
        lvdc_preigm_chi_y(&p, p.T_S1_sec - eps)) * deg_1;
    CHECK(step_12 < 1.0,
          "chi_y continuous across segment-1/2 boundary (< 1 deg)");

    const double step_23 = fabs(
        lvdc_preigm_chi_y(&p, p.T_S2_sec + eps) -
        lvdc_preigm_chi_y(&p, p.T_S2_sec - eps)) * deg_1;
    CHECK(step_23 < 1.0,
          "chi_y continuous across segment-2/3 boundary (< 1 deg)");

    const double step_3f = fabs(
        lvdc_preigm_chi_y(&p, p.T_S3_sec + eps) -
        lvdc_preigm_chi_y(&p, p.T_S3_sec - eps)) * deg_1;
    CHECK(step_3f < 1.0,
          "chi_y continuous across segment-3/P_final (< 1 deg)");

    /* ------------------------------------------------------------
     * Scenario 6: pitch-over reaches ~ −60° by T_S3.
     * The AS-205 reference trajectory has chi_c ≈ −60° at S-IB
     * cutoff.  Check the AS-205 preset produces a value in the
     * right ballpark at T_c = T_S3.
     * ---------------------------------------------------------- */
    const double chi_at_T_S3_deg = lvdc_preigm_chi_y(&p, p.T_S3_sec) * deg_1;
    CHECK(chi_at_T_S3_deg > -70.0 && chi_at_T_S3_deg < -50.0,
          "chi_y at T_S3 is between -70 and -50 deg (pitch-over)");

    if (g_fail_count == 0) {
        printf("test_preigm_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_preigm_smoke: %d CHECK(S) FAILED\n", g_fail_count);
    return 1;
}
