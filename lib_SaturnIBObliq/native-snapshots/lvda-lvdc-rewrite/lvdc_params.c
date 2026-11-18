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

    /* Earth model (NESP via EDD §4.3.2). */
    p.mu_earth_m3ps2       = 3.986032e14;   /* matches LVDA_LVDC.c gravity block */
    p.omega_E_radps        = M_PI * 2.321e-5; /* 7.2921e-5 rad/s sidereal */
    p.R_earth_equatorial_m = 6378166.0;      /* Fischer 1960 equatorial, PASC */
    p.J2                   = 1.08248e-3;     /* zonal 2 per NESP */
    p.J3                   = 0.0;            /* boost: Eq. 4.3.8 set to 0 */
    p.J4                   = 0.0;            /* boost: Eq. 4.3.9 set to 0 */

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

    return p;
}
