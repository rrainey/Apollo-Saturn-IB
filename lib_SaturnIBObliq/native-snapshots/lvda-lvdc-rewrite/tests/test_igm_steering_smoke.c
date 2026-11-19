/*
 * test_igm_steering_smoke.c — unit tests for EDD §4.4.2.2
 * Eqs. 4.4.29 through 4.4.56.
 *
 * Checks:
 *   1. T* = T_1i + T_3i (Eqs. 4.4.29, 4.4.53).
 *   2. phi_i uses atan2 so sign and quadrant are correct
 *      (Eq. 4.4.30).
 *   3. delta_2 reduces to the easy terms when ROV = 0
 *      (Eq. 4.4.31).
 *   4. phi_iT scales inversely with R_T and matches formula
 *      (Eq. 4.4.32).
 *   5. Terminal V-system state has the expected axis pattern
 *      (Eqs. 4.4.34-40).
 *   6. G*_V is the straight average (Eq. 4.4.46).
 *   7. dV_V_prime = V_TV - V_V - T* G*_V (Eq. 4.4.47).
 *   8. improved_tgo dL_3 formula (Eq. 4.4.48) and dT_3_prime
 *      sign consistency (Eq. 4.4.50).
 *   9. dV_V_final = dV_V_prime - dT_3' G*_V (Eq. 4.4.54).
 *  10. chi_tilde_y and chi_tilde_z have expected limits.
 *  11. Degenerate guards: improved_tgo returns -1 when L'_y <= 0.
 */

#include "../lvdc_igm_steering.h"

#include <math.h>
#include <stdio.h>

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
    /* --------------------------------------------------------------
     * 1. T* = T_1i + T_3i.
     * ------------------------------------------------------------ */
    CHECK_CLOSE(lvdc_igm_T_star(60.0, 150.0), 210.0, 1e-12,
                "Eq. 4.4.29 T* sum");

    /* --------------------------------------------------------------
     * 2. phi_i quadrant handling.
     * ------------------------------------------------------------ */
    CHECK_CLOSE(lvdc_igm_phi_i(1.0, 0.0),   0.0,          1e-15,
                "phi_i (1, 0) = 0");
    CHECK_CLOSE(lvdc_igm_phi_i(0.0, 1.0),   M_PI_2,       1e-15,
                "phi_i (0, 1) = pi/2");
    CHECK_CLOSE(lvdc_igm_phi_i(-1.0, 0.0),  M_PI,         1e-15,
                "phi_i (-1, 0) = pi");
    CHECK_CLOSE(lvdc_igm_phi_i(1.0, 1.0),   M_PI / 4.0,   1e-15,
                "phi_i (1, 1) = pi/4");

    /* --------------------------------------------------------------
     * 3. delta_2 with ROV = 0 collapses to V T* - J'_3 + L'_y T_3i.
     * ------------------------------------------------------------ */
    {
        const double V = 3000.0, T_star = 200.0, J_prime_3 = 1.0e5;
        const double L_prime_y = 500.0, T_3i = 80.0;
        const double got = lvdc_igm_delta_2(V, T_star, J_prime_3,
                                            L_prime_y, T_3i,
                                            /*ROV*/ 0.0,
                                            /*V_ex3*/ 4233.2,
                                            286.9, 50.0, 2200.0,
                                            326.5, 1800.0, 7800.0);
        const double expected = V * T_star - J_prime_3
                              + L_prime_y * T_3i;
        CHECK_CLOSE(got, expected, 1e-6,
                    "Eq. 4.4.31 delta_2 ROV=0 reduces");
    }

    /* --------------------------------------------------------------
     * 4. phi_iT inverse scaling with R_T (radian form; EDD /pi is
     *    an LVDC half-rev conversion that we drop in the radian
     *    pipeline).
     * ------------------------------------------------------------ */
    {
        const double R_T = 6.571e6, S_1 = 1.0e5, d2 = 5.0e4;
        const double Theta_T = 0.03;
        const double got = lvdc_igm_phi_iT(R_T, S_1, d2, Theta_T);
        const double expected = (S_1 + d2) * cos(Theta_T) / R_T;
        CHECK_CLOSE(got, expected, 1e-12,
                    "Eq. 4.4.32 phi_iT formula (radians)");
        CHECK_CLOSE(lvdc_igm_phi_iT(0.0, S_1, d2, Theta_T),
                    0.0, 1e-15,
                    "Eq. 4.4.32 R_T=0 guard");
    }

    /* --------------------------------------------------------------
     * 5. Terminal V-system state.
     * ------------------------------------------------------------ */
    {
        const double R_T = 6.571e6, V_T = 7804.0;
        const double Theta_T = 0.0245;
        const double G_T = -9.10;
        lvdc_igm_terminal_V_t t;
        lvdc_igm_terminal_state_V(R_T, V_T, Theta_T, G_T, &t);

        CHECK_CLOSE(t.R_VT[0], R_T, 1e-9, "X_VT = R_T");
        CHECK_CLOSE(t.R_VT[1], 0.0, 1e-15, "Y_VT = 0");
        CHECK_CLOSE(t.R_VT[2], 0.0, 1e-15, "Z_VT = 0");
        CHECK_CLOSE(t.V_TV[0], V_T * sin(Theta_T), 1e-9,
                    "Xdot_VT = V_T sin Theta_T");
        CHECK_CLOSE(t.V_TV[1], 0.0, 1e-15, "Ydot_VT = 0");
        CHECK_CLOSE(t.V_TV[2], V_T * cos(Theta_T), 1e-9,
                    "Zdot_VT = V_T cos Theta_T");
        CHECK_CLOSE(t.G_TV[0], G_T, 1e-15, "Xddot_VgT = G_T");
        CHECK_CLOSE(t.G_TV[1], 0.0, 1e-15, "Yddot_VgT = 0");
        CHECK_CLOSE(t.G_TV[2], 0.0, 1e-15, "Zddot_VgT = 0");
    }

    /* --------------------------------------------------------------
     * 6. G*_V = 1/2 (G_TV + G_V).
     * ------------------------------------------------------------ */
    {
        const double G_V[3]  = {-7.0, 0.3, -1.2};
        const double G_TV[3] = {-9.1, 0.0,  0.0};
        double G_star[3];
        lvdc_igm_G_star_V(G_V, G_TV, G_star);
        for (int i = 0; i < 3; i++) {
            CHECK_CLOSE(G_star[i], 0.5 * (G_V[i] + G_TV[i]), 1e-15,
                        "Eq. 4.4.46 G*_V average");
        }
    }

    /* --------------------------------------------------------------
     * 7. dV_V_prime = V_TV - V_V - T* G*_V.
     * ------------------------------------------------------------ */
    {
        const double V_TV[3] = { 190.0, 0.0, 7800.0 };
        const double V_V[3]  = { 100.0, 5.0, 7000.0 };
        const double T_star = 120.0;
        const double G_star[3] = { -8.5, 0.0, 0.0 };
        double dV_V_prime[3];
        lvdc_igm_dV_V_prime(V_TV, V_V, T_star, G_star, dV_V_prime);
        for (int i = 0; i < 3; i++) {
            const double expected = V_TV[i] - V_V[i]
                                  - T_star * G_star[i];
            CHECK_CLOSE(dV_V_prime[i], expected, 1e-9,
                        "Eq. 4.4.47 dV_V_prime");
        }
    }

    /* --------------------------------------------------------------
     * 8. improved_tgo analytic match.
     * ------------------------------------------------------------ */
    {
        const double dV[3] = {90.0, 0.0, 1500.0};
        const double L_prime_y = 1800.0;
        const double L_prime_3 = 1400.0;
        const double tau_3 = 326.5;
        const double T_3i = 80.0;
        const double V_ex3 = 4233.2;

        double dL_3 = 0, L_3 = 0, dT_3_prime = 0;
        int rc = lvdc_igm_improved_tgo(dV, L_prime_y, L_prime_3,
                                       tau_3, T_3i, V_ex3,
                                       &dL_3, &L_3, &dT_3_prime);
        CHECK(rc == 0, "improved_tgo success");

        const double dVsq = 90.0 * 90.0 + 1500.0 * 1500.0;
        const double dL_3_expected =
            0.5 * (dVsq / L_prime_y - L_prime_y);
        const double L_3_expected = L_prime_3 + dL_3_expected;
        const double dT_3_prime_expected =
            dL_3_expected * (tau_3 - T_3i) / V_ex3;

        CHECK_CLOSE(dL_3, dL_3_expected, 1e-9, "Eq. 4.4.48 ΔL_3");
        CHECK_CLOSE(L_3,  L_3_expected,  1e-9, "Eq. 4.4.49 L_3");
        CHECK_CLOSE(dT_3_prime, dT_3_prime_expected, 1e-12,
                    "Eq. 4.4.50 ΔT'_3");
    }

    /* --------------------------------------------------------------
     * 9. dV_V_final = dV_V_prime - dT_3' G*_V.
     * ------------------------------------------------------------ */
    {
        const double dVp[3]  = { 50.0, -1.2, 400.0 };
        const double G_s[3]  = { -8.5, 0.0,   0.1 };
        const double dT      = 0.8;
        double dV[3];
        lvdc_igm_dV_V_final(dVp, dT, G_s, dV);
        for (int i = 0; i < 3; i++) {
            CHECK_CLOSE(dV[i], dVp[i] - dT * G_s[i], 1e-12,
                        "Eq. 4.4.54 dV_V");
        }
    }

    /* --------------------------------------------------------------
     * 10. chi_tilde_y, chi_tilde_z limits.
     *   dV_V = (0, 0, 1) -> chi_y = 0  (pure downrange),
     *                       chi_z = 0  (no cross-range).
     *   dV_V = (1, 0, 0) -> chi_y = pi/2 (straight up).
     *   dV_V = (0, 1, 0) -> chi_z = pi/2 (all cross-range).
     * ------------------------------------------------------------ */
    {
        const double a[3] = {0.0, 0.0, 1.0};
        CHECK_CLOSE(lvdc_igm_chi_tilde_y(a), 0.0, 1e-15,
                    "chi_tilde_y downrange = 0");
        CHECK_CLOSE(lvdc_igm_chi_tilde_z(a), 0.0, 1e-15,
                    "chi_tilde_z downrange = 0");

        const double b[3] = {1.0, 0.0, 0.0};
        CHECK_CLOSE(lvdc_igm_chi_tilde_y(b), M_PI_2, 1e-15,
                    "chi_tilde_y radial = pi/2");

        const double c[3] = {0.0, 1.0, 0.0};
        CHECK_CLOSE(lvdc_igm_chi_tilde_z(c), M_PI_2, 1e-15,
                    "chi_tilde_z cross = pi/2");
    }

    /* --------------------------------------------------------------
     * 11. Degenerate guard: L'_y <= 0 -> -1.
     * ------------------------------------------------------------ */
    {
        const double dV[3] = {1.0, 0.0, 1.0};
        int rc = lvdc_igm_improved_tgo(dV, 0.0, 10.0, 326.5, 80.0,
                                       4233.2,
                                       NULL, NULL, NULL);
        CHECK(rc == -1, "improved_tgo L'_y=0 returns -1");

        rc = lvdc_igm_improved_tgo(dV, 100.0, 10.0, 326.5, 80.0,
                                   0.0,
                                   NULL, NULL, NULL);
        CHECK(rc == -1, "improved_tgo V_ex3=0 returns -1");
    }

    if (g_fail == 0) {
        printf("test_igm_steering_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_igm_steering_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
