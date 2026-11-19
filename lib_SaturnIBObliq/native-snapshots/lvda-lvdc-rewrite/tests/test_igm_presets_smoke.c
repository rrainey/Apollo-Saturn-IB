/*
 * test_igm_presets_smoke.c — unit tests for the AS-205 preset
 * population (lvdc_params_as205) and the lvdc_igm_presets_from_params
 * mapping (lvdc_igm.c).
 *
 * The tests pin the authoritative TMX Table 3B values so a future
 * accidental edit shows up as a test failure.
 */

#include "../lvdc_igm.h"
#include "../lvdc_params.h"

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

int main(void) {
    const lvdc_params_t p = lvdc_params_as205();

    /* --------------------------------------------------------------
     * Pin TMX Table 3B values on lvdc_params directly.
     * ------------------------------------------------------------ */
    CHECK_CLOSE(p.V_ex1_mps,                4151.0,     1e-9, "TMX Vex1");
    CHECK_CLOSE(p.V_ex3_mps,                4233.2,     1e-9, "TMX Vex3");
    CHECK_CLOSE(p.igm_T_1i_initial_sec,     286.3,      1e-9, "TMX T1i");
    CHECK_CLOSE(p.igm_T_3i_initial_sec,     159.8,      1e-9, "TMX T3i");
    CHECK_CLOSE(p.igm_tau_3_initial_sec,    326.5,      1e-9, "TMX tau_3");
    CHECK_CLOSE(p.igm_tau_10_sec,           536.0,      1e-9, "TMX tau_10");
    CHECK_CLOSE(p.igm_T_10_sec,             286.3,      1e-9, "TMX T_10");
    CHECK_CLOSE(p.igm_P_CMR_sec,            35.0,       1e-9, "TMX CO (P_CMR)");
    CHECK_CLOSE(p.igm_M_GR_kg,              129761.6,   1e-3, "TMX M_GR");
    CHECK_CLOSE(p.igm_Mdot_2_kgps,          239.7877,   1e-6, "TMX Mdot_2");
    CHECK_CLOSE(p.Mdot_3_kgps,              183.1797,   1e-6, "TMX Mdot_3");
    CHECK_CLOSE(p.target_V_T_mps,           7780.67,    1e-9, "TMX V_T");
    CHECK_CLOSE(p.target_R_T_m,             6600405.0,  1e-3, "TMX R_T");
    CHECK_CLOSE(p.target_Theta_T_deg,       0.0,        1e-15, "TMX Theta_T");
    CHECK_CLOSE(p.target_Xdotdot_VGT_mps2, -9.15,       1e-9, "TMX XVG");
    CHECK_CLOSE(p.A_z_deg,                  72.0,       1e-9, "TMX A_z");
    CHECK_CLOSE(p.phi_L_deg,                28.521963,  1e-9, "TMX phi_L");
    CHECK_CLOSE(p.igm_lambda_N_deg,         119.0,      1e-9, "TMX phi_N");
    CHECK_CLOSE(p.target_i_deg,             31.605,     1e-9, "TMX inclination");
    CHECK_CLOSE(p.igm_i_orb_deg,            31.605,     1e-9, "TMX orb i");
    CHECK_CLOSE(p.igm_enable_from_T3_sec,   25.0,       1e-9, "TMX T3+25");

    /* --------------------------------------------------------------
     * Preset-to-IGM-preset mapping.
     * ------------------------------------------------------------ */
    lvdc_igm_presets_t ig;
    lvdc_igm_presets_from_params(&p, &ig);

    const double d2r = M_PI / 180.0;

    CHECK_CLOSE(ig.V_ex1_mps,     p.V_ex1_mps,     1e-15, "map V_ex1");
    CHECK_CLOSE(ig.V_ex3_mps,     p.V_ex3_mps,     1e-15, "map V_ex3");
    CHECK_CLOSE(ig.T_1i_initial_sec,
                p.igm_T_1i_initial_sec, 1e-15, "map T_1i");
    CHECK_CLOSE(ig.T_3i_initial_sec,
                p.igm_T_3i_initial_sec, 1e-15, "map T_3i");
    CHECK_CLOSE(ig.tau_10_sec,    p.igm_tau_10_sec, 1e-15, "map tau_10");
    CHECK_CLOSE(ig.T_10_sec,      p.igm_T_10_sec,   1e-15, "map T_10");
    CHECK_CLOSE(ig.M_GR_kg,       p.igm_M_GR_kg,    1e-15, "map M_GR");
    CHECK_CLOSE(ig.Mdot_2_kgps,   p.igm_Mdot_2_kgps, 1e-15, "map Mdot_2");
    CHECK_CLOSE(ig.Mdot_3_kgps,   p.Mdot_3_kgps,    1e-15, "map Mdot_3");
    CHECK_CLOSE(ig.P_CMR_sec,     p.igm_P_CMR_sec,  1e-15, "map P_CMR");
    CHECK_CLOSE(ig.R_T_m,         p.target_R_T_m,   1e-15, "map R_T");
    CHECK_CLOSE(ig.V_T_mps,       p.target_V_T_mps, 1e-15, "map V_T");
    CHECK_CLOSE(ig.Theta_T_rad,
                p.target_Theta_T_deg * d2r,         1e-15, "map Theta_T");
    CHECK_CLOSE(ig.G_T_mps2,
                p.target_Xdotdot_VGT_mps2,          1e-15, "map G_T");
    CHECK_CLOSE(ig.ROV,  p.igm_ROV,  1e-15, "map ROV");
    CHECK_CLOSE(ig.SMCG, p.igm_SMCG, 1e-15, "map SMCG");
    CHECK_CLOSE(ig.lambda_rad, p.igm_lambda_N_deg * d2r, 1e-15, "map lambda");
    CHECK_CLOSE(ig.phi_L_rad,  p.phi_L_deg * d2r,        1e-15, "map phi_L");
    CHECK_CLOSE(ig.A_z_rad,    p.A_z_deg * d2r,          1e-15, "map A_z");
    CHECK_CLOSE(ig.i_orb_rad,  p.igm_i_orb_deg * d2r,    1e-15, "map i_orb");

    /* --------------------------------------------------------------
     * End-to-end: init IGM with mapped presets and run one BML
     * cycle. Verifies the AS-205 presets produce finite commands.
     * ------------------------------------------------------------ */
    lvdc_igm_state_t s;
    lvdc_igm_init(&s, &ig);

    const double R_S[3] = { 6.571e6, 0.0, 0.0 };
    const double V_S[3] = { 100.0, 0.0, 7000.0 };
    const double G_S[3] = { -9.1, 0.0, 0.0 };
    /* (M/F)_S chosen so tau = V_ex * MoverF > T_1i and T_3i. */
    const double MoverF = 0.15;

    double chi_y = 0, chi_z = 0;
    const int rc = lvdc_igm_update(&s, 1.6, 7000.0, MoverF,
                                    R_S, V_S, G_S,
                                    25.0, 0.0, 0.0,
                                    &chi_y, &chi_z);
    if (rc != 0) {
        fprintf(stderr, "FAIL AS-205 IGM update rc=%d\n", rc);
        g_fail++;
    }
    if (!isfinite(chi_y) || !isfinite(chi_z)) {
        fprintf(stderr, "FAIL AS-205 IGM chi not finite\n");
        g_fail++;
    }

    /* NULL guards. */
    lvdc_igm_presets_from_params(NULL, &ig);
    lvdc_igm_presets_from_params(&p, NULL);

    if (g_fail == 0) {
        printf("test_igm_presets_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_igm_presets_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
