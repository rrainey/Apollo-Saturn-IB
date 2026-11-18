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

    /* Chi time-tilt — EDD §4.4.1.2 Eq. 4.4.2 for AS-205.
     * Coefficients and breakpoints taken from the current body at
     * ../lvda-lvdc/LVDA_LVDC.c L564-613 and the PARAM_* defines in
     * ../lvda-lvdc/LVDA_LVDC.h. */
    p.P_final_rad   = -1.0352499;   /* ≈ −59.3° */
    p.T_S1_sec1_sec =  10.0;        /* gantry-clear */
    p.T_S1_sec      =  63.8;
    p.T_S2_sec      = 109.8;
    p.T_S3_sec      = 134.3;

    /* F_1: segment [T_S1_sec1_sec, T_S1_sec] */
    p.chi_y_F1[0] = -0.0139871;
    p.chi_y_F1[1] =  0.00320258;
    p.chi_y_F1[2] = -0.00022283;
    p.chi_y_F1[3] =  1.1473e-6;

    /* F_2: segment (T_S1_sec, T_S2_sec] */
    p.chi_y_F2[0] =  0.337135;
    p.chi_y_F2[1] = -0.0108052;
    p.chi_y_F2[2] = -3.84334e-5;
    p.chi_y_F2[3] =  3.50965e-7;

    /* F_3: segment (T_S2_sec, T_S3_sec] */
    p.chi_y_F3[0] =  0.210868;
    p.chi_y_F3[1] = -0.00978481;
    p.chi_y_F3[2] = -1.0118e-5;
    p.chi_y_F3[3] =  1.03404e-7;

    return p;
}
