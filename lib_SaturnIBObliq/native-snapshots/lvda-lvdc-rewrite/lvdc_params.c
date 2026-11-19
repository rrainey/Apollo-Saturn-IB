/*
 * lvdc_params.c — AS-205 preset values for the LVDC rewrite.
 *
 * Every field cited. Mission-specific scalars live in the
 * AS205_* #defines below so that a mission swap only touches
 * this one translation unit. See lvdc_params.h for the
 * provenance key.
 *
 * Primary citations: NASA TM-X-62831 Table 3B (AS-205
 * Operational Flight Trajectory, IGM Presettings) and
 * TM-X-62831 Appendix B (S-IB Stage Unbiased Pitch Program).
 * Per feedback_tmx_precedence_over_edd.md, TMX wins over EDD
 * on AS-205-specific numbers.
 */

#include "lvdc_params.h"

#include <math.h>

/* =====================================================================
 * AS-205 mission presets (TM-X-62831).
 *
 * Grouped by physical meaning. Rules:
 *   - One #define per scalar.
 *   - Comment cites the exact TMX table / page where applicable.
 *   - No naked magic numbers in the function body below.
 * ================================================================== */

/* --- Earth model (NESP via EDD §4.3.2) ------------------------------ */
#define AS205_MU_EARTH_M3PS2            3.986032e14   /* current gravity block */
#define AS205_R_EARTH_EQ_M              6378165.0     /* Fischer-1960 */
#define AS205_J2                        1.0823e-3     /* NESP, from current (3/2)*J2 */
#define AS205_J3                        0.0           /* boost: EDD §4.3.2 */
#define AS205_J4                        0.0           /* boost: EDD §4.3.2 */

/* --- Launch site (TM-X-62831 Table 3B; EDD §3.4) ------------------- */
#define AS205_PHI_L_DEG                 28.521963     /* LC-34 geodetic lat */
#define AS205_PHI_L_PRIME_DEG           28.360795
#define AS205_LAMBDA_L_DEG             (-80.561141)
#define AS205_R_L_M                     6373385.0
#define AS205_A_Z_DEG                   72.0          /* TMX Table 3B */
#define AS205_PAD_ROLL_DEG              100.0

/* --- Boost timing (EDD §4.1, §7.2; TMX Part II) -------------------- */
#define AS205_DELTA_T_SEC               1.6
#define AS205_BN_1_SEC                  14.4
#define AS205_T3_FM_SEC                 5.0

/* --- M/F smoothing (EDD §4.2.5-6, Eq. 4.2.11; TMX Table 1-3) ------- */
#define AS205_MOVERF_0_S2PM             0.135         /* TMX Table 1-3 (M/F)_0 */
#define AS205_MOVERF_S_LIMIT_S2PM       0.25          /* EDD §4.2.5 */
#define AS205_MOVERF_S_RATE_LIMIT       0.005         /* EDD §4.2.5 per BML */

/* (M/F)_S filter coefficients — S-IVB stage. TMX Table 1-3. */
#define AS205_MFK1                      0.0120948
#define AS205_MFK2                      0.0213339
#define AS205_MFK3                     (-0.0261577)
#define AS205_MFK4                     (-0.00526684)
#define AS205_MFK5                      2.80483
#define AS205_MFK6                     (-2.85880)
#define AS205_MFK7                      1.24933
#define AS205_MFK8                     (-0.197360)

/* --- Target orbit (TM-X-62831 Table 3B) ---------------------------- */
#define AS205_TARGET_I_DEG              31.605
#define AS205_TARGET_LAMBDA_0_DEG       119.0         /* TMX phi_N */
#define AS205_TARGET_V_T_MPS            7780.67
#define AS205_TARGET_R_T_M              6600405.0
#define AS205_TARGET_THETA_T_DEG        0.0           /* X_dot_VT = 0, Z_dot_VT = V_T */
#define AS205_TARGET_XDDOT_VGT_MPS2    (-9.15)        /* TMX Table 3B "XVG" (EDD Part II shows -9.35 for other profiles) */

/* --- Stage performance (TM-X-62831 Table 3B) ----------------------- */
#define AS205_V_EX1_MPS                 4151.0        /* S-IVB phase 1 avg exhaust */
#define AS205_V_EX3_MPS                 4233.2        /* S-IVB phase 2 avg exhaust */
#define AS205_MDOT_1_KGPS               2820.8        /* S-IB flow from first motion to IECO */
#define AS205_MDOT_3_KGPS               183.1797      /* S-IVB phase 2 flow */
#define AS205_F1_N                      7600000.0     /* S-IB thrust */
#define AS205_F3_N                      775441.26     /* S-IVB phase 2 avg thrust */
#define AS205_M1_KG                     585943.0      /* mass at first motion */
#define AS205_M03_KG                    134374.0      /* mass at J-2 start minus jettison */

/* --- IGM phasing (TM-X-62831 Table 3B) ----------------------------- */
#define AS205_IGM_T_1I_INITIAL_SEC      286.3
#define AS205_IGM_T_3I_INITIAL_SEC      159.8
#define AS205_IGM_TAU_3_INITIAL_SEC     326.5
#define AS205_IGM_TAU_10_SEC            536.0
#define AS205_IGM_T_10_SEC              286.3
#define AS205_IGM_P_CMR_SEC             35.0          /* TMX "CO" row */
#define AS205_IGM_EPS_2_SEC             25.0          /* TMX Table 3B, §4.4.2.4 */
#define AS205_IGM_M_GR_KG               129761.6      /* TMX "M03" row in artificial-tau group */
#define AS205_IGM_MDOT_2_KGPS           239.7877      /* TMX "MI" row in artificial-tau group */
#define AS205_IGM_SMCG                  0.03          /* EDD Part II Table 1-3 (rad/sec) */
#define AS205_IGM_ROV                   1.8           /* TMX Table 3B "TR1" */

/* --- IGM frame constants (TM-X-62831 Table 3B cont'd) -------------- */
#define AS205_IGM_LAMBDA_N_DEG          119.0         /* TMX phi_N */
#define AS205_IGM_I_ORB_DEG             31.605        /* same as target_i */

/* --- IGM enable timing (TM-X-62831 Table 3B) ----------------------- */
#define AS205_IGM_ENABLE_FROM_T3_SEC    25.0          /* "Time from T3 to initiate IGM" */
/* T1-based fallback until Phase D time bases land. T3 starts at
 * S-IB cutoff (~141.8 s post-liftoff per TMX App B trajectory
 * tables); T3 + 25 s = ~166.8 s T1. */
#define AS205_IGM_ENABLE_FROM_T1_SEC    166.8

/* --- HSL cutoff (EDD §4.4.2.4; Part II) ---------------------------- */
#define AS205_DELTAV_B_MPS              7.2381        /* EDD Part II Table 1-3 */
#define AS205_V_GRD_MPS                 150.0         /* legacy PARAM, Part II */
#define AS205_T_HSL_SEC                 5.0           /* legacy PARAM, Part II */

/* --- Chi time-tilt — TMX Appendix B unbiased pitch poly ------------ */
#define AS205_P_FINAL_RAD              (-1.0352499)   /* ~-59.3 deg */
#define AS205_T_S1_SEC1_SEC             10.0
#define AS205_T_S1_SEC                  63.8
#define AS205_T_S2_SEC                  109.8
#define AS205_T_S3_SEC                  134.3

#define AS205_F1_A0                    (-0.139871e-1)
#define AS205_F1_A1                     0.320258e-2
#define AS205_F1_A2                    (-0.222830e-3)
#define AS205_F1_A3                     0.114730e-5

#define AS205_F2_B0                     0.337135e+0
#define AS205_F2_B1                    (-0.108052e-1)
#define AS205_F2_B2                    (-0.384334e-4)
#define AS205_F2_B3                     0.350965e-6

#define AS205_F3_C0                     0.210868e+0
#define AS205_F3_C1                    (-0.978481e-2)
#define AS205_F3_C2                    (-0.101180e-4)
#define AS205_F3_C3                     0.103404e-6

#define AS205_CHI_X_PAD_RAD            (-0.488692)    /* -28 deg */

/* --- Minor-loop + DAC limits --------------------------------------- */
#define AS205_CHI_RATE_LIMIT_DEG_PER_SEC   1.0        /* TMX p.2 */
#define AS205_PSI_MAG_LIMIT_DEG            15.3       /* pending Part II */
#define AS205_PSI_RATE_LIMIT_DEG_PER_SEC   12.0       /* pending Part II */
#define AS205_DAC_LSB_DEG                  0.0575     /* MSFC hw spec */

/* =====================================================================
 * Struct population.
 * ================================================================== */

lvdc_params_t lvdc_params_as205(void) {
    lvdc_params_t p = {0};

    /* Earth model. */
    p.mu_earth_m3ps2       = AS205_MU_EARTH_M3PS2;
    p.omega_E_radps        = M_PI * 2.321e-5;   /* 7.2921e-5 rad/s sidereal */
    p.R_earth_equatorial_m = AS205_R_EARTH_EQ_M;
    p.J2                   = AS205_J2;
    p.J3                   = AS205_J3;
    p.J4                   = AS205_J4;

    /* Launch site. */
    p.phi_L_deg        = AS205_PHI_L_DEG;
    p.phi_L_prime_deg  = AS205_PHI_L_PRIME_DEG;
    p.lambda_L_deg     = AS205_LAMBDA_L_DEG;
    p.R_L_m            = AS205_R_L_M;
    p.A_z_deg          = AS205_A_Z_DEG;
    p.pad_roll_L_deg   = AS205_PAD_ROLL_DEG;

    /* Boost timing. */
    p.delta_T_sec = AS205_DELTA_T_SEC;
    p.BN_1_sec    = AS205_BN_1_SEC;
    p.T3_FM_sec   = AS205_T3_FM_SEC;

    /* M/F smoothing. */
    p.MoverF_0_s2pm               = AS205_MOVERF_0_S2PM;
    p.MoverF_s_limit_s2pm         = AS205_MOVERF_S_LIMIT_S2PM;
    p.MoverF_s_rate_limit_per_BML = AS205_MOVERF_S_RATE_LIMIT;
    p.MFK[0] = AS205_MFK1;
    p.MFK[1] = AS205_MFK2;
    p.MFK[2] = AS205_MFK3;
    p.MFK[3] = AS205_MFK4;
    p.MFK[4] = AS205_MFK5;
    p.MFK[5] = AS205_MFK6;
    p.MFK[6] = AS205_MFK7;
    p.MFK[7] = AS205_MFK8;

    /* Target orbit. */
    p.target_i_deg            = AS205_TARGET_I_DEG;
    p.target_lambda_0_deg     = AS205_TARGET_LAMBDA_0_DEG;
    p.target_V_T_mps          = AS205_TARGET_V_T_MPS;
    p.target_R_T_m            = AS205_TARGET_R_T_M;
    p.target_Theta_T_deg      = AS205_TARGET_THETA_T_DEG;
    p.target_Xdotdot_VGT_mps2 = AS205_TARGET_XDDOT_VGT_MPS2;

    /* Stage performance. */
    p.V_ex1_mps   = AS205_V_EX1_MPS;
    p.V_ex3_mps   = AS205_V_EX3_MPS;
    p.Mdot_1_kgps = AS205_MDOT_1_KGPS;
    p.Mdot_3_kgps = AS205_MDOT_3_KGPS;
    p.F1_N        = AS205_F1_N;
    p.F3_N        = AS205_F3_N;
    p.M1_kg       = AS205_M1_KG;
    p.M03_kg      = AS205_M03_KG;

    /* IGM phasing. */
    p.igm_T_1i_initial_sec  = AS205_IGM_T_1I_INITIAL_SEC;
    p.igm_T_3i_initial_sec  = AS205_IGM_T_3I_INITIAL_SEC;
    p.igm_tau_3_initial_sec = AS205_IGM_TAU_3_INITIAL_SEC;
    p.igm_tau_10_sec        = AS205_IGM_TAU_10_SEC;
    p.igm_T_10_sec          = AS205_IGM_T_10_SEC;
    p.igm_P_CMR_sec         = AS205_IGM_P_CMR_SEC;
    p.igm_eps_2_sec         = AS205_IGM_EPS_2_SEC;
    p.igm_M_GR_kg           = AS205_IGM_M_GR_KG;
    p.igm_Mdot_2_kgps       = AS205_IGM_MDOT_2_KGPS;
    p.igm_SMCG              = AS205_IGM_SMCG;
    p.igm_ROV               = AS205_IGM_ROV;

    /* IGM frame constants. */
    p.igm_lambda_N_deg = AS205_IGM_LAMBDA_N_DEG;
    p.igm_i_orb_deg    = AS205_IGM_I_ORB_DEG;

    /* IGM enable timing. */
    p.igm_enable_from_T3_sec = AS205_IGM_ENABLE_FROM_T3_SEC;
    p.igm_enable_from_T1_sec = AS205_IGM_ENABLE_FROM_T1_SEC;

    /* HSL cutoff. */
    p.DeltaV_b_mps = AS205_DELTAV_B_MPS;
    p.V_GRD_mps    = AS205_V_GRD_MPS;
    p.T_HSL_sec    = AS205_T_HSL_SEC;

    /* Chi time-tilt. */
    p.P_final_rad   = AS205_P_FINAL_RAD;
    p.T_S1_sec1_sec = AS205_T_S1_SEC1_SEC;
    p.T_S1_sec      = AS205_T_S1_SEC;
    p.T_S2_sec      = AS205_T_S2_SEC;
    p.T_S3_sec      = AS205_T_S3_SEC;
    p.chi_y_F1[0] = AS205_F1_A0;
    p.chi_y_F1[1] = AS205_F1_A1;
    p.chi_y_F1[2] = AS205_F1_A2;
    p.chi_y_F1[3] = AS205_F1_A3;
    p.chi_y_F2[0] = AS205_F2_B0;
    p.chi_y_F2[1] = AS205_F2_B1;
    p.chi_y_F2[2] = AS205_F2_B2;
    p.chi_y_F2[3] = AS205_F2_B3;
    p.chi_y_F3[0] = AS205_F3_C0;
    p.chi_y_F3[1] = AS205_F3_C1;
    p.chi_y_F3[2] = AS205_F3_C2;
    p.chi_y_F3[3] = AS205_F3_C3;
    p.chi_x_pad_rad = AS205_CHI_X_PAD_RAD;

    /* Minor-loop + DAC. */
    const double d2r = M_PI / 180.0;
    const double chi_rate = AS205_CHI_RATE_LIMIT_DEG_PER_SEC * d2r;
    p.chi_rate_limit_radps[0] = chi_rate;
    p.chi_rate_limit_radps[1] = chi_rate;
    p.chi_rate_limit_radps[2] = chi_rate;
    p.psi_mag_limit_rad    = AS205_PSI_MAG_LIMIT_DEG * d2r;
    p.psi_rate_limit_radps = AS205_PSI_RATE_LIMIT_DEG_PER_SEC * d2r;
    p.dac_lsb_rad          = AS205_DAC_LSB_DEG * d2r;

    return p;
}
