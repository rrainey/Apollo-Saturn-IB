/*
 * test_abi_smoke.c — exercise LVDA_LVDC_{init,compute_outputs,
 * update_memory_states,evaluate_enable_states} as the obliq-2 host
 * would. Validates the per-tick-guard, GRR edge detection, BML gate,
 * and output stability under RK4 re-entry.
 *
 * Mirrors the call pattern documented in
 * docs/LVDC_RK4_REENTRY_REPORT.md:
 *
 *   per major step N:
 *     for stage in [k1, k2, k3, k4]:
 *         compute_outputs(model)          # sample_tick stable
 *     update_memory_states(model)         # once per step
 *     model.time       += dt
 *     model.sample_tick++
 *
 * Exits 0 on success, non-zero on any failed check.
 */

#include "../LVDA_LVDC.h"
#include "../lvdc_params.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

/* Run one major step with four RK4-stage compute_outputs calls,
 * then one update_memory_states. */
static void drive_major_step(LVDA_LVDC_t *model, double dt) {
    LVDA_LVDC_compute_outputs(model);  /* step-start / k1 */
    LVDA_LVDC_compute_outputs(model);  /* k2 */
    LVDA_LVDC_compute_outputs(model);  /* k3 */
    LVDA_LVDC_compute_outputs(model);  /* k4 */
    LVDA_LVDC_update_memory_states(model);
    model->time += dt;
    model->sample_tick++;
}

int main(void) {
    LVDA_LVDC_t model;
    const double dt = 0.005;  /* AS-205 obliq-2 fundamental step */

    LVDA_LVDC_init(&model);
    model.dt = dt;
    model.time = 0.0;
    model.sample_tick = 0ULL;

    /* ------------------------------------------------------------
     * Scenario 1: pre-GRR major step. bLiftoff is false. Outputs
     * are all zero / pass-through. R_S_bar_m stays at the init
     * value (zero).
     * ---------------------------------------------------------- */
    model.inputs.V_m_bar_mps[0] = 7.0;
    model.inputs.V_m_bar_mps[1] = -2.5;
    model.inputs.V_m_bar_mps[2] = 0.1;
    model.inputs.Theta_deg[0] = 100.0;
    model.inputs.Theta_deg[1] = 0.0;
    model.inputs.Theta_deg[2] = 0.0;
    model.inputs.bLiftoff = false;
    model.inputs.bSIBPropellantSensorDry = false;

    drive_major_step(&model, dt);

    CHECK(model.outputs.R_S_bar_m[0] == 0.0, "pre-GRR R_S_bar_m[0] == 0");
    CHECK(model.outputs.R_S_bar_m[1] == 0.0, "pre-GRR R_S_bar_m[1] == 0");
    CHECK(model.outputs.R_S_bar_m[2] == 0.0, "pre-GRR R_S_bar_m[2] == 0");
    CHECK_CLOSE(model.outputs.theta_deg[0], 100.0, 1e-12,
                "pre-GRR theta_deg pass-through [0]");
    CHECK(!model.outputs.bSIVEngineStart, "pre-GRR bSIVEngineStart false");

    /* ------------------------------------------------------------
     * Scenario 2: bLiftoff rising edge fires GRR latch. On the
     * same step, R_S_bar_m should jump to X_0 (Eq. 3.4.3).
     *
     * Important: verify output is stable across all four RK4
     * stages of this step — all four compute_outputs calls must
     * return the same R_S_bar_m.
     * ---------------------------------------------------------- */
    const lvdc_params_t p = lvdc_params_as205();
    double X_0_expected[3], V_0_expected[3];
    {
        /* Recreate the expected X_0 locally to compare. */
        const double deg2rad = M_PI / 180.0;
        const double delta_rad = (p.phi_L_deg - p.phi_L_prime_deg) * deg2rad;
        const double A_z_rad = p.A_z_deg * deg2rad;
        const double sin_d = sin(delta_rad);
        const double cos_d = cos(delta_rad);
        const double sin_A = sin(A_z_rad);
        const double cos_A = cos(A_z_rad);
        X_0_expected[0] =  p.R_L_m * cos_d;
        X_0_expected[1] =  p.R_L_m * sin_d * sin_A;
        X_0_expected[2] = -p.R_L_m * sin_d * cos_A;
        /* V_0 is computed but unused here; silence warning. */
        V_0_expected[0] = 0.0;
        V_0_expected[1] = 0.0;
        V_0_expected[2] = 0.0;
        (void)V_0_expected;
    }

    model.inputs.bLiftoff = true;

    /* Four compute calls at the GRR tick. Collect R_S_bar_m from each. */
    double R_stage[4][3];
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_stage[0], model.outputs.R_S_bar_m, sizeof(R_stage[0]));
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_stage[1], model.outputs.R_S_bar_m, sizeof(R_stage[1]));
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_stage[2], model.outputs.R_S_bar_m, sizeof(R_stage[2]));
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_stage[3], model.outputs.R_S_bar_m, sizeof(R_stage[3]));
    LVDA_LVDC_update_memory_states(&model);
    model.time += dt;
    model.sample_tick++;

    for (int stage = 0; stage < 4; stage++) {
        CHECK_CLOSE(R_stage[stage][0], X_0_expected[0], 1e-6,
                    "GRR-step R_S_bar_m[0] stable across RK4 stage");
        CHECK_CLOSE(R_stage[stage][1], X_0_expected[1], 1e-6,
                    "GRR-step R_S_bar_m[1] stable across RK4 stage");
        CHECK_CLOSE(R_stage[stage][2], X_0_expected[2], 1e-6,
                    "GRR-step R_S_bar_m[2] stable across RK4 stage");
    }

    /* And they are identical to each other. */
    for (int stage = 1; stage < 4; stage++) {
        for (int i = 0; i < 3; i++) {
            CHECK(R_stage[stage][i] == R_stage[0][i],
                  "R_S_bar_m bit-identical across RK4 stages");
        }
    }

    /* ------------------------------------------------------------
     * Scenario 3: simulate 400 major steps = 2 s (just over one
     * BML at 1.6 s). Inject a constant 12.5 m/s^2 accel in +X
     * plumbline direction. Platform V_m grows linearly after GRR.
     * Verify that a BML step actually fires once in this window —
     * the R_S_bar_m value should change after 320 sample ticks
     * (1.6 s / 0.005 s), not before.
     * ---------------------------------------------------------- */
    const double accel = 12.5;  /* m/s^2 */
    const double R_before_BML[3] = {
        model.outputs.R_S_bar_m[0],
        model.outputs.R_S_bar_m[1],
        model.outputs.R_S_bar_m[2]
    };

    bool saw_R_change = false;
    const uint64_t grr_tick = model.sample_tick; /* now 2 */
    const uint64_t ticks_per_BML = (uint64_t)llround(1.6 / dt); /* 320 */

    for (int step = 0; step < 400; step++) {
        /* Platform velocity grows as accel * (t - t_GRR). */
        const double t_since_grr = (model.sample_tick - grr_tick) * dt;
        model.inputs.V_m_bar_mps[0] = 7.0 + accel * t_since_grr;
        drive_major_step(&model, dt);

        if (!saw_R_change
            && (model.outputs.R_S_bar_m[0] != R_before_BML[0]
             || model.outputs.R_S_bar_m[1] != R_before_BML[1]
             || model.outputs.R_S_bar_m[2] != R_before_BML[2])) {
            saw_R_change = true;
            const uint64_t ticks_after_grr = model.sample_tick - grr_tick;
            /* BML gate fires at ticks where sample_tick % 320 == 0.
             * GRR tick was 2. Next BML gate is sample_tick = 320
             * (unconditional on GRR offset). So ticks_after_grr =
             * 320 - 2 = 318 is the first BML past GRR. Allow ±1 for
             * the moment we observe the change in the output. */
            const int64_t expected = (int64_t)ticks_per_BML - (int64_t)grr_tick;
            const int64_t diff =
                (int64_t)ticks_after_grr - expected;
            CHECK(diff >= 0 && diff <= 2,
                  "BML step fires close to the first post-GRR BML tick");
            (void)diff;
        }
    }
    CHECK(saw_R_change, "R_S_bar_m changed within 2 s after GRR");

    /* ------------------------------------------------------------
     * Scenario 4: output stability on a non-BML, non-GRR tick.
     * Freeze inputs, call compute_outputs 4 times in a row on the
     * same tick, confirm the outputs don't drift.
     * ---------------------------------------------------------- */
    double R_check[4][3];
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_check[0], model.outputs.R_S_bar_m, sizeof(R_check[0]));
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_check[1], model.outputs.R_S_bar_m, sizeof(R_check[1]));
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_check[2], model.outputs.R_S_bar_m, sizeof(R_check[2]));
    LVDA_LVDC_compute_outputs(&model);
    memcpy(R_check[3], model.outputs.R_S_bar_m, sizeof(R_check[3]));

    for (int stage = 1; stage < 4; stage++) {
        for (int i = 0; i < 3; i++) {
            CHECK(R_check[stage][i] == R_check[0][i],
                  "R_S_bar_m bit-identical across 4 same-tick calls");
        }
    }

    /* ------------------------------------------------------------
     * Scenario 5: re-init resets statics. Call init again,
     * check all outputs return to zero and GRR is unlatched
     * (next bLiftoff edge should re-latch).
     * ---------------------------------------------------------- */
    LVDA_LVDC_init(&model);
    model.dt = dt;
    model.inputs.bLiftoff = false;
    drive_major_step(&model, dt);
    CHECK(model.outputs.R_S_bar_m[0] == 0.0
       && model.outputs.R_S_bar_m[1] == 0.0
       && model.outputs.R_S_bar_m[2] == 0.0,
          "re-init restores R_S_bar_m to zero");

    model.inputs.bLiftoff = true;
    drive_major_step(&model, dt);
    CHECK(model.outputs.R_S_bar_m[0] != 0.0,
          "re-init still permits GRR latch on next rising edge");

    /* --- Summary --- */
    if (g_fail_count == 0) {
        printf("test_abi_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_abi_smoke: %d CHECK(S) FAILED\n", g_fail_count);
    return 1;
}
