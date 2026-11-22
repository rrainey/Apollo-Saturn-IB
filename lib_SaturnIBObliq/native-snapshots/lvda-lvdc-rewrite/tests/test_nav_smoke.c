/*
 * test_nav_smoke.c — smoke-test the lvdc_nav module against known
 * analytical cases.
 *
 * Runs three scenarios:
 *   1. Pre-latch: nav step is a no-op.
 *   2. Liftoff latch: X_S equals X_0 (Eq. 3.4.3); V_Sg = 0; V_m = 0.
 *      (On a real IB the latch fires at GRR, T_0 − 17 s; the obliq-2
 *      parent model does not expose a GRR discrete, so we latch on
 *      bLiftoff. See lvdc_internal.h for the balance analysis.)
 *   3. Constant-thrust burn: 10 BMLs at 20 m/s per BML sensed in
 *      the +X plumbline direction. Checks that F/M, V_m
 *      accumulation and X_S integration produce the right
 *      magnitudes.
 *
 * Point-mass gravity is active, so checks on V_Sg allow for
 * ~g·T drop from the vertical component of the central-force
 * acceleration.
 *
 * Exits 0 on success, non-zero on any failed check.
 */

#include "../lvdc_nav.h"
#include "../lvdc_params.h"
#include "../lvdc_quant.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
    lvdc_params_t p = lvdc_params_as205();
    lvdc_state_t  s;
    lvdc_nav_reset(&s);

    /* ------------------------------------------------------------
     * Scenario 1: pre-latch BML step is a no-op.
     * ---------------------------------------------------------- */
    {
        const double V_m_initial[3] = {100.0, 20.0, -5.0};
        lvdc_nav_bml_step(&s, &p, V_m_initial, 0.0);
        CHECK(s.nav.X_S[0] == 0.0 && s.nav.X_S[1] == 0.0 && s.nav.X_S[2] == 0.0,
              "pre-latch nav step is no-op");
    }

    /* ------------------------------------------------------------
     * Scenario 2: liftoff latch at t = 0. Platform velocity on the pad
     * is non-zero (earth-rotation contribution). X_S must equal
     * EDD Eq. 3.4.3 output; V_m accumulator must be zero.
     * ---------------------------------------------------------- */
    double X_0[3], V_0[3];
    lvdc_nav_compute_initial_conditions(&p, X_0, V_0);

    /* Sanity on the initial conditions themselves. */
    const double r_from_X0 = sqrt(X_0[0]*X_0[0] + X_0[1]*X_0[1] + X_0[2]*X_0[2]);
    CHECK_CLOSE(r_from_X0, p.R_L_m, 1e-3, "Eq. 3.4.3 |X_0| == R_L");
    /* V_0 has zero X component per Eq. 3.4.4. */
    CHECK_CLOSE(V_0[0], 0.0, 1e-12, "Eq. 3.4.4 V_0 X = 0");
    /* |V_0| must equal omega_E · R_L · cos(phi_L'). */
    const double expected_V0_mag =
        p.omega_E_radps * p.R_L_m * cos(p.phi_L_prime_deg * M_PI / 180.0);
    const double got_V0_mag = sqrt(V_0[0]*V_0[0] + V_0[1]*V_0[1] + V_0[2]*V_0[2]);
    CHECK_CLOSE(got_V0_mag, expected_V0_mag, 1e-6,
                "Eq. 3.4.4 |V_0| == omega_E * R_L * cos(phi'_L)");

    /* Perform the liftoff latch. */
    const double V_m_at_latch[3] = {7.1234, -2.5, 0.123};
    lvdc_nav_liftoff_latch(&s, &p, /*t_latch=*/0.0, V_m_at_latch, X_0, V_0);
    CHECK(s.sensor.liftoff_latched, "liftoff latched flag set");
    CHECK_CLOSE(s.nav.X_S[0], X_0[0], 1e-9, "X_S seeded from Eq. 3.4.3 [0]");
    CHECK_CLOSE(s.nav.X_S[1], X_0[1], 1e-9, "X_S seeded from Eq. 3.4.3 [1]");
    CHECK_CLOSE(s.nav.X_S[2], X_0[2], 1e-9, "X_S seeded from Eq. 3.4.3 [2]");
    CHECK_CLOSE(s.sensor.V_S_init[1], V_0[1], 1e-12, "V_S_init Y stored");
    CHECK_CLOSE(s.sensor.V_m_mps[0], 0.0, 1e-12, "V_m accumulator zero post-latch");
    CHECK(s.sensor.V_m_pulses[0] == 0, "V_m_pulses zero post-latch");

    /* ------------------------------------------------------------
     * Scenario 3: constant thrust burn in the +X plumbline
     * direction. Simulate 10 BMLs at dT = 1.6 s with a steady
     * 12.5 m/s^2 axial thrust, so sensed velocity grows by
     * 20 m/s per BML. Starting from the pad state.
     *
     * Expected at BML N (counting from latch+1):
     *   V_m[0] = 20 * N
     *   F/M   = 12.5 m/s^2  (once steady)
     *   X_S[0] growth: initial-value-free analytical answer is
     *      X_S[0] - X_0[0] = (N·dT)² / 2 · a_x + small gravity term
     *   V_Sg: accumulates gravity over the elapsed interval, mag
     *      ~ g · (N·dT) where g ≈ μ/R_L² ≈ 9.81 m/s². We bound-
     *      check rather than exact-check because the gravity
     *      acceleration is not aligned with any axis (depends on X_S).
     * ---------------------------------------------------------- */
    const double accel_x_mps2 = 12.5;
    const double dT = p.delta_T_sec;
    const double dV_per_BML = accel_x_mps2 * dT;  /* 20 m/s */
    double V_m_bar[3] = { V_m_at_latch[0], V_m_at_latch[1], V_m_at_latch[2] };

    for (int n = 1; n <= 10; n++) {
        V_m_bar[0] += dV_per_BML;
        const double t_now = n * dT;
        lvdc_nav_bml_step(&s, &p, V_m_bar, t_now);

        /* V_m accumulator should be N * dV_per_BML after N BMLs. */
        const double expected_Vm_x = n * dV_per_BML;
        CHECK_CLOSE(s.sensor.V_m_mps[0], expected_Vm_x, 0.05,
                    "V_m accumulator vs synthetic input (X)");

        /* F/M axial = 12.5 m/s^2 nominally. Allow 0.1% for one-BML
         * finite-difference quantization at the pulse scale. */
        CHECK_CLOSE(s.sensor.FoverM_axis[0], accel_x_mps2,
                    accel_x_mps2 * 0.001,
                    "F/M axis X vs expected axial thrust");
    }

    /* After 10 BMLs = 16 s:
     *   displacement from accel alone: ½·a·t² = ½·12.5·16² = 1600 m
     *   Plus V_0·t ~ 400 m/s · 16 s = 6400 m in the Y/Z plane.
     *   Minus gravity-induced position drop ~ ½·g·t² = ½·9.81·256 = 1256 m
     *     projected on the −X axis (since X_0 is along +X in the S frame).
     * Net X_S[0] − X_0[0] ≈ 1600 − 1256 ≈ 344 m.  We check only
     * magnitudes and signs, not exact numerics, since the gravity
     * contribution depends on the point-mass stub. */
    const double dX_x = s.nav.X_S[0] - X_0[0];
    CHECK(dX_x > -2000.0 && dX_x <  3000.0,
          "X_S[0] drift within reasonable bounds after 10 BMLs");

    /* V_Sg should have accumulated roughly 0.5·(−g)·t · 2 ≈ −g·t ≈
     *   −9.81 · 16 ≈ −157 m/s in the -X direction (pointing to
     *   earth centre). Bound-check. */
    CHECK(s.nav.V_Sg[0] < 0.0 && s.nav.V_Sg[0] > -250.0,
          "V_Sg[0] is negative and bounded (gravity drag)");

    /* Total V_S from Eq. 4.3.5 = V_m + V_0 + V_Sg. */
    double V_S[3];
    lvdc_nav_compute_V_S(&s, V_S);
    const double expected_Vm10 = 10 * dV_per_BML; /* 200 m/s */
    const double expected_VS_x = expected_Vm10 + V_0[0] + s.nav.V_Sg[0];
    CHECK_CLOSE(V_S[0], expected_VS_x, 0.1,
                "Eq. 4.3.5 V_S[0] = V_m + V_0 + V_Sg");

    /* ------------------------------------------------------------
     * Scenario 4: gravity magnitude at the launch site.
     *
     * EDD §4.3.2 oblate gravity at r = R_L, launch site. The scalar
     * gravity magnitude must be close to 9.80 m/s² and is well-
     * bounded by the central term with the J_2 correction:
     *   g ≈ μ/R_L² · (1 + ~0.001)
     *
     * We also check that boost_mode toggling (S_34/P_34 off vs on)
     * changes the result by ~1e-6 m/s² — i.e. that the extra terms
     * are negligible during boost (as the EDD asserts). */
    {
        double G_boost[3], G_orbit[3];
        lvdc_nav_compute_gravity(X_0, &p, /*boost_mode=*/true,  G_boost);
        lvdc_nav_compute_gravity(X_0, &p, /*boost_mode=*/false, G_orbit);

        const double g_boost_mag = sqrt(G_boost[0]*G_boost[0]
                                      + G_boost[1]*G_boost[1]
                                      + G_boost[2]*G_boost[2]);
        const double g_point     = p.mu_earth_m3ps2 / (p.R_L_m * p.R_L_m);

        /* Magnitude bracket: 9.78 .. 9.82 m/s² at Cape latitude. */
        CHECK(g_boost_mag > 9.78 && g_boost_mag < 9.82,
              "|G_S| at launch site is in [9.78, 9.82] m/s^2");

        /* Within ~0.5% of central-term point-mass gravity (J_2
         * correction at the pole is ~0.5%). */
        CHECK_CLOSE(g_boost_mag, g_point, g_point * 0.01,
                    "|G_S| within 1% of central-term point-mass");

        /* S_34 + P_34 are sub-microgravity at boost altitudes. */
        const double diff = sqrt(
            (G_orbit[0]-G_boost[0])*(G_orbit[0]-G_boost[0]) +
            (G_orbit[1]-G_boost[1])*(G_orbit[1]-G_boost[1]) +
            (G_orbit[2]-G_boost[2])*(G_orbit[2]-G_boost[2]));
        CHECK(diff < 1e-3,
              "S_34/P_34 contribution is sub-mg at boost altitudes");
    }

    /* --- Summary --- */
    if (g_fail_count == 0) {
        printf("test_nav_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_nav_smoke: %d CHECK(S) FAILED\n", g_fail_count);
    return 1;
}
