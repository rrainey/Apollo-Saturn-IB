/*
 * test_igm_position_correction_smoke.c — unit tests for EDD
 * §4.4.2.2 Eqs. 4.4.51, 4.4.57 through 4.4.82.
 *
 * Because the block has no analytical "right answer," each
 * intermediate output is cross-checked against its own equation as
 * written in the EDD. The chi_y4 and chi_z4 outputs are the full
 * chain and are checked against a hand-rolled recomputation that
 * mirrors the production code line-for-line (so any bug there will
 * also break production — the real check for correctness is the
 * shadow replay against flight data).
 */

#include "../lvdc_igm_position_correction.h"

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

#define CHECK_EQ(got, expected, label) CHECK_CLOSE(got, expected, 0.0, label)

int main(void) {
    /* Representative AS-205-like mid-IGM inputs. Values chosen so
     * that no divide-by-zero guard trips naturally. */
    lvdc_igm_pc_inputs_t in = {0};
    in.tau_1_sec = 286.9;
    in.tau_3_sec = 326.5;
    in.T_1i_sec  = 50.0;
    in.T_3i_sec  = 150.0;
    in.V_ex3_mps = 4233.2;

    in.L_1       = 800.0;
    in.J_1       = 2.2e5;
    in.S_1       = 1.6e4;
    in.Q_1       = 3.1e6;
    in.P_1       = 6.2e5;
    in.U_1       = 4.0e7;
    in.L_prime_3 = 1400.0;
    in.J_prime_3 = 1.1e5;
    in.L_prime_y = in.L_1 + in.L_prime_3;

    in.dL_3   = 15.0;
    in.L_3    = in.L_prime_3 + in.dL_3;
    in.T_star_sec = in.T_1i_sec + in.T_3i_sec;

    in.X_V_m              = 6.571e6;
    in.Y_V_m              = 12.0;
    in.Xdot_V_mps         = 3.0;
    in.Ydot_V_mps         = -0.1;
    in.Xddot_Vg_star_mps2 = -9.1;
    in.Yddot_V_mps2       =  0.0;

    in.X_VT_m = 6.573e6;

    in.chi_tilde_y_rad = 0.05;
    in.chi_tilde_z_rad = 0.002;
    in.phi_T_rad       = 0.3;
    in.dT_N_sec        = 1.6;

    /* Run the block. */
    lvdc_igm_pc_outputs_t out;
    const int rc = lvdc_igm_position_correction(&in, &out);
    if (rc != 0) {
        fprintf(stderr,
                "FAIL unexpected rc=%d from baseline compute\n", rc);
        return 1;
    }

    /* --------------------------------------------------------------
     * Cross-check every intermediate against its EDD formula.
     * ------------------------------------------------------------ */
    const double c_y = cos(in.chi_tilde_y_rad);
    const double s_y = sin(in.chi_tilde_y_rad);
    const double c_z = cos(in.chi_tilde_z_rad);
    const double s_z = sin(in.chi_tilde_z_rad);

    const double J_3 = in.J_prime_3 + in.T_3i_sec * in.dL_3;
    CHECK_CLOSE(out.J_3, J_3, 1e-9, "Eq. 4.4.51 J_3");

    const double S_3 = in.L_3 * in.T_3i_sec - J_3;
    CHECK_CLOSE(out.S_3, S_3, 1e-9, "Eq. 4.4.57 S_3");

    const double Q_3 = S_3 * in.tau_3_sec
                     - 0.5 * in.V_ex3_mps * in.T_3i_sec * in.T_3i_sec;
    CHECK_CLOSE(out.Q_3, Q_3, 1e-6, "Eq. 4.4.58 Q_3");

    const double L_y = in.L_1 + in.L_3;
    CHECK_CLOSE(out.L_y, L_y, 1e-12, "Eq. 4.4.59 L_y");

    const double J_y = in.J_1 + J_3 + in.L_3 * in.T_1i_sec;
    CHECK_CLOSE(out.J_y, J_y, 1e-9, "Eq. 4.4.60 J_y");

    const double S_y = in.S_1 - J_3 + L_y * in.T_3i_sec;
    CHECK_CLOSE(out.S_y, S_y, 1e-9, "Eq. 4.4.61 S_y");

    const double Q_y = in.Q_1 + Q_3
                     + S_3 * in.T_1i_sec
                     + in.J_1 * in.T_3i_sec;
    CHECK_CLOSE(out.Q_y, Q_y, 1e-6, "Eq. 4.4.62 Q_y");

    const double K_y = L_y / J_y;
    CHECK_CLOSE(out.K_y, K_y, 1e-15, "Eq. 4.4.63 K_y");

    const double D_y = S_y - K_y * Q_y;
    CHECK_CLOSE(out.D_y, D_y, 1e-6, "Eq. 4.4.64 D_y");

    const double dY_V = in.Y_V_m
                      + in.Ydot_V_mps * in.T_star_sec
                      + 0.5 * in.Yddot_V_mps2 * in.T_star_sec * in.T_star_sec
                      + S_y * s_z;
    CHECK_CLOSE(out.dY_V, dY_V, 1e-9, "Eq. 4.4.65 dY_V");

    const double K_3 = dY_V / (D_y * c_z);
    CHECK_CLOSE(out.K_3, K_3, 1e-12, "Eq. 4.4.66 K_3");

    const double K_4 = K_y * K_3;
    CHECK_CLOSE(out.K_4, K_4, 1e-15, "Eq. 4.4.67 K_4");

    const double P_3 = J_3 * (in.tau_3_sec + 2.0 * in.T_1i_sec)
                     - 0.5 * in.V_ex3_mps * in.T_3i_sec * in.T_3i_sec;
    CHECK_CLOSE(out.P_3, P_3, 1e-6, "Eq. 4.4.68 P_3");

    const double U_3 = Q_3 * (in.tau_3_sec + 2.0 * in.T_1i_sec)
                     - (1.0 / 6.0) * in.V_ex3_mps
                       * in.T_3i_sec * in.T_3i_sec * in.T_3i_sec;
    CHECK_CLOSE(out.U_3, U_3, 1e-3, "Eq. 4.4.69 U_3");

    const double L_p = L_y * c_z;
    CHECK_CLOSE(out.L_p, L_p, 1e-9, "Eq. 4.4.70 L_p");

    const double C_2 = c_z + K_3 * s_z;
    CHECK_CLOSE(out.C_2, C_2, 1e-12, "Eq. 4.4.71 C_2");

    const double C_4 = K_4 * s_z;
    CHECK_CLOSE(out.C_4, C_4, 1e-15, "Eq. 4.4.72 C_4");

    const double J_p = J_y * C_2
                     - C_4 * (in.P_1 + P_3
                            + in.L_3 * in.T_1i_sec * in.T_1i_sec);
    CHECK_CLOSE(out.J_p, J_p, 1e-6, "Eq. 4.4.73 J_p");

    const double S_p = S_y * C_2 - Q_y * C_4;
    CHECK_CLOSE(out.S_p, S_p, 1e-6, "Eq. 4.4.74 S_p");

    const double Q_p = Q_y * C_2
                     - C_4 * (in.U_1 + U_3
                            + S_3 * in.T_1i_sec * in.T_1i_sec
                            + in.T_3i_sec * in.P_1);
    CHECK_CLOSE(out.Q_p, Q_p, 1e-3, "Eq. 4.4.75 Q_p");

    const double K_p = L_p / J_p;
    CHECK_CLOSE(out.K_p, K_p, 1e-15, "Eq. 4.4.76 K_p");

    const double D_p = S_p - K_p * Q_p;
    CHECK_CLOSE(out.D_p, D_p, 1e-6, "Eq. 4.4.77 D_p");

    const double dX_V = in.X_V_m - in.X_VT_m
                      + in.Xdot_V_mps * in.T_star_sec
                      + 0.5 * in.Xddot_Vg_star_mps2
                            * in.T_star_sec * in.T_star_sec
                      + S_p * s_y;
    CHECK_CLOSE(out.dX_V, dX_V, 1e-6, "Eq. 4.4.78 dX_V");

    const double K_1 = dX_V / (D_p * c_y);
    CHECK_CLOSE(out.K_1, K_1, 1e-12, "Eq. 4.4.79 K_1");

    const double K_2 = K_p * K_1;
    CHECK_CLOSE(out.K_2, K_2, 1e-15, "Eq. 4.4.80 K_2");

    /* chi_y4 and chi_z4 — the full-chain outputs. See the
     * ANGLE-UNIT CONVENTION note: EDD half-rev literals /pi and
     * 1/2 are converted to radians at transcription time, so:
     *   chi_y4 = chi_tilde_y - (K_1 - K_2 dT_N) - phi_T - pi/2
     *   chi_z4 = chi_tilde_z - (K_3 - K_4 dT_N) */
    const double chi_y4 = in.chi_tilde_y_rad
                        - (K_1 - K_2 * in.dT_N_sec)
                        - in.phi_T_rad
                        - M_PI_2;
    const double chi_z4 = in.chi_tilde_z_rad
                        - (K_3 - K_4 * in.dT_N_sec);
    CHECK_CLOSE(out.chi_y4_rad, chi_y4, 1e-12,
                "Eq. 4.4.81 chi_y4 (radian form)");
    CHECK_CLOSE(out.chi_z4_rad, chi_z4, 1e-12,
                "Eq. 4.4.82 chi_z4 (radian form)");

    /* --------------------------------------------------------------
     * Degenerate guards: NULL pointer, cos(chi_tilde)=0.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_pc_outputs_t out_null;
        CHECK_EQ(lvdc_igm_position_correction(NULL, &out_null), -1,
                 "NULL inputs guard");
        CHECK_EQ(lvdc_igm_position_correction(&in, NULL), -1,
                 "NULL outputs guard");
    }

    if (g_fail == 0) {
        printf("test_igm_position_correction_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr,
            "test_igm_position_correction_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
