/*
 * lvdc_params.c — AS-205 preset values for the LVDC rewrite.
 *
 * Every value cited in lvdc_params.h; see that file for the
 * provenance key.
 */

#include "lvdc_params.h"

#include <math.h>

lvdc_params_t lvdc_params_as205(void) {
    lvdc_params_t p = {0};

    /* Earth model (NESP via EDD §4.3.2). Values match the LVDA_LVDC.c
     * gravity block constants (verified 2026-11-18). */
    p.mu_earth_m3ps2       = 3.986032e14;   /* current gravity block */
    p.omega_E_radps        = M_PI * 2.321e-5; /* 7.2921e-5 rad/s sidereal */
    p.R_earth_equatorial_m = 6378165.0;      /* R_e per current gravity block */
    /* J2 is encoded in the current code as (3/2)·J2 = 1.62345e-3, so
     * J2 = 1.0823e-3 (Fischer-1960 era value, matches NESP). */
    p.J2                   = 1.0823e-3;
    p.J3                   = 0.0;            /* boost: EDD §4.3.2 sets S_34, P_34 to 0 */
    p.J4                   = 0.0;            /* boost: EDD §4.3.2 sets S_34, P_34 to 0 */

    /* Launch site (TM-X-62831 Table 3B; EDD §3.4). AS-205 LC-34. */
    p.phi_L_deg        = 28.521963;
    p.phi_L_prime_deg  = 28.360795;
    p.lambda_L_deg     = -80.561141;
    p.R_L_m            = 6373385.0;
    p.A_z_deg          = 72.0;
    p.pad_roll_L_deg   = 100.0;

    /* Boost timing (EDD §4.1, §7.2; Part II). */
    p.delta_T_sec = 1.6;
    p.BN_1_sec    = 14.4;
    p.T3_FM_sec   = 5.0;

    /* M/F smoothing (EDD §4.2.5). The initial (M/F)_0 value depends
     * on S-IVB thrust and mass at T3+0; placeholder here, refined in
     * Phase C when IGM-feeding m/F is wired up. */
    p.MoverF_0_s2pm              = 0.0;      /* TODO Phase C: from Part II Preset Table */
    p.MoverF_s_limit_s2pm        = 0.25;     /* EDD §4.2.5 */
    p.MoverF_s_rate_limit_per_BML = 0.005;   /* EDD §4.2.5 */

    /* Guidance / targets / stage performance / HSL — Phase C. All
     * zero-initialised, populated when lvdc_igm is written. */

    /* Chi time-tilt — TM-X-62831 Appendix B (S-IB Stage Unbiased
     * Pitch Program). AS-205 values. The EDD §4.4.1.2 defines the
     * polynomial form; TM-X-62831 provides the mission-specific
     * coefficients. The current body at ../lvda-lvdc/LVDA_LVDC.c
     * L564-613 uses the same values. */
    p.P_final_rad   = -1.0352499;   /* ≈ −59.3° */
    p.T_S1_sec1_sec =  10.0;        /* gantry-clear */
    p.T_S1_sec      =  63.8;
    p.T_S2_sec      = 109.8;
    p.T_S3_sec      = 134.3;

    /* F_1 (A coefficients in TMX): segment 10 < T ≤ 63.8 */
    p.chi_y_F1[0] = -0.139871e-1;   /* A_0 rad */
    p.chi_y_F1[1] =  0.320258e-2;   /* A_1 rad/sec */
    p.chi_y_F1[2] = -0.222830e-3;   /* A_2 rad/sec^2 */
    p.chi_y_F1[3] =  0.114730e-5;   /* A_3 rad/sec^3 */

    /* F_2 (B coefficients in TMX): segment 63.8 < T ≤ 109.8 */
    p.chi_y_F2[0] =  0.337135e+0;   /* B_0 */
    p.chi_y_F2[1] = -0.108052e-1;   /* B_1 */
    p.chi_y_F2[2] = -0.384334e-4;   /* B_2 */
    p.chi_y_F2[3] =  0.350965e-6;   /* B_3 */

    /* F_3 (C coefficients in TMX): segment 109.8 < T ≤ 134.3 */
    p.chi_y_F3[0] =  0.210868e+0;   /* C_0 */
    p.chi_y_F3[1] = -0.978481e-2;   /* C_1 */
    p.chi_y_F3[2] = -0.101180e-4;   /* C_2 */
    p.chi_y_F3[3] =  0.103404e-6;   /* C_3 */

    p.chi_x_pad_rad = -0.488692;    /* TMX Appendix B, T ≤ 10 */

    /* Minor-loop attitude increment limits — EDD §8.3.1, AS-205
     * values from TM-X-62831 p. 2 (1 deg/sec pitch and yaw). Roll
     * carries the same value per the current body. */
    const double one_deg_per_sec_in_radps = M_PI / 180.0;
    p.chi_rate_limit_radps[0] = one_deg_per_sec_in_radps;  /* roll */
    p.chi_rate_limit_radps[1] = one_deg_per_sec_in_radps;  /* pitch */
    p.chi_rate_limit_radps[2] = one_deg_per_sec_in_radps;  /* yaw */

    /* Attitude-error command pipeline — EDD §8.2.3, Saturn V
     * hardware spec for the D/A LSB. Mag + rate limits from the
     * current body pending EDD Part II verification. */
    p.psi_mag_limit_rad    =  15.3 * M_PI / 180.0;
    p.psi_rate_limit_radps =  12.0 * M_PI / 180.0;
    p.dac_lsb_rad          = 0.0575 * M_PI / 180.0;

    return p;
}
