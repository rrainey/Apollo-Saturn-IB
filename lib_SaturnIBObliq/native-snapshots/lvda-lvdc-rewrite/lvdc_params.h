/*
 * lvdc_params.h — mission preset catalogue for the LVDC rewrite.
 *
 * One struct, every field cited. The values here are the rewrite's
 * canonical source. The current snapshot's LVDA_LVDC.h has a parallel
 * set of `#define PARAM_*` values with some unit-suffix bugs
 * (notably PARAM_omega_E_rps stores ω/π in pirad/s, not rad/s);
 * the rewrite does not depend on those, to avoid propagating the
 * naming errors.
 *
 * Provenance keys:
 *   EDD   = Saturn IB LVDC Equation Defining Document, IBM
 *           70-207-0001 Rev A, 11 April 1972. Section numbers refer
 *           to Part I; Table / Presetting references to Part II.
 *   TM    = NASA TM-X-62831, AS-205/CSM-101 Launch Vehicle
 *           Operational Trajectory Rev I, 10 Sep 1968. Appendix B
 *           gives the AS-205 pitch, yaw, and roll programs in
 *           authoritative form (coefficients and breakpoints).
 *   PASC  = Project Apollo Coordinate System Standards
 *           (MSC MA-1461, 1 Jun 1965).
 *   HW    = Saturn V LVDA hardware specification
 *           (MSFC spc_stnv_000130.pdf, UAH Digital Projects archive).
 *   NESP  = Natural Environment and Physical Standards for the
 *           Apollo Program, NASA M-DE8020.008B, April 1965
 *           (the EDD §4.3.2 footnote reference).
 *
 * Scope: Phase B populates the nav / sensor / earth / launch-site
 * fields. Guidance fields (IGM targets, mass flow, exhaust velocities,
 * HSL thresholds) are declared but left zero-initialised until Phase C.
 */

#ifndef LVDC_PARAMS_H
#define LVDC_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* -----------------------------------------------------------------
     * Earth model (EDD §4.3.2, NESP M-DE8020.008B)
     * -------------------------------------------------------------- */

    /* μ = GM_earth, m^3 / s^2. The current obliq-2 model uses
     * 3.986032e14 (see LVDA_LVDC.c L79-80 gravity block constants).
     * Modern value is 3.986004e14 (WGS84/EGM); the 2.8e7 m^3/s^2 delta
     * is ~7 ppm and comes from the 1960s Fischer-ellipsoid era
     * reference the EDD cites (NESP M-DE8020.008B). Keep the EDD-era
     * value for fidelity to the flight software. */
    double mu_earth_m3ps2;

    /* Earth rotation rate, rad / s. EDD-era value is ω = π · 2.321e-5
     * = 7.292115e-5 rad/s (sidereal). Old snapshot stores this as
     * 0.00002321 pirad/s under the misleading name PARAM_omega_E_rps;
     * see ts/as205InitialPosition.ts for the conversion note. */
    double omega_E_radps;

    /* Earth equatorial radius, m. Used by EDD §4.3.2 Eqs. 4.3.6-12
     * oblate gravity expansion. Fischer-1960 ellipsoid per PASC. */
    double R_earth_equatorial_m;

    /* Second zonal harmonic J_2 of the earth potential. EDD §4.3.2
     * references NESP. During boost only Eqs. 4.3.6, 4.3.7, 4.3.10,
     * 4.3.11 are used; J_3 and J_4 (S_34, P_34) are set to zero. */
    double J2;
    double J3;  /* not used during boost */
    double J4;  /* not used during boost */

    /* -----------------------------------------------------------------
     * Launch site (EDD §3.4 Eq. 3.4.3-4; TM-X-62831 Table 3B)
     * -------------------------------------------------------------- */

    /* Geodetic latitude, geocentric latitude, longitude. EDD §3.4. */
    double phi_L_deg;
    double phi_L_prime_deg;
    double lambda_L_deg;

    /* Radius from earth centre to launch point, m. EDD §3.4. */
    double R_L_m;

    /* Launch azimuth (angle from north, clockwise), deg. */
    double A_z_deg;

    /* Pad roll alignment, deg. */
    double pad_roll_L_deg;

    /* -----------------------------------------------------------------
     * Boost timing (EDD §4.1, §4.2.4-5; Part II Event Sequence)
     * -------------------------------------------------------------- */

    /* Nominal BML period, s. EDD §4.1 — actual BML period varies with
     * LVDC load; the simulation uses this nominal value. */
    double delta_T_sec;

    /* Boost navigation-1 timer, s. EDD §7.2 T1 start logic. */
    double BN_1_sec;

    /* T3 + T3_FM_sec = time to start M/F smoothing (§4.2.5). */
    double T3_FM_sec;

    /* -----------------------------------------------------------------
     * Accelerometer / M/F smoothing (EDD §4.2.5, §4.2.6)
     * -------------------------------------------------------------- */

    /* Initial smoothed reciprocal acceleration (M/F)_0, s^2/m. EDD
     * §4.2.5 says this is from the Presetting Table of Part II.
     * Seeds the four historical values of M/F and (M/F)_s at or
     * before T3+0. TMX Table 1-3: 0.135 s^2/m. */
    double MoverF_0_s2pm;

    /* Smoothed M/F magnitude limit, s^2/m. EDD §4.2.5: 0.25 s^2/m. */
    double MoverF_s_limit_s2pm;

    /* Smoothed M/F rate-of-change limit, (s^2/m) per BML.
     * EDD §4.2.5: 0.005 s^2/m per BML. */
    double MoverF_s_rate_limit_per_BML;

    /* (M/F)_S filter coefficients for S-IVB stage (EDD Eq. 4.2.11,
     * TMX Table 1-3). The LVDC uses an 8-tap IIR filter on the
     * four most recent instantaneous (M/F)_i and four most recent
     * smoothed (M/F)_Si samples:
     *   (M/F)_S = MFK[0](M/F)_1 + MFK[1](M/F)_2
     *           + MFK[2](M/F)_3 + MFK[3](M/F)_4
     *           + MFK[4](M/F)_S1 + MFK[5](M/F)_S2
     *           + MFK[6](M/F)_S3 + MFK[7](M/F)_S4
     */
    double MFK[8];

    /* -----------------------------------------------------------------
     * Guidance (EDD §4.4; TM-X-62831 Table 3B)
     *
     * AS-205 numeric values live in lvdc_params.c as `AS205_*`
     * #defines co-located with the function body, so swapping
     * missions is a matter of editing one file while keeping the
     * same struct shape.
     * -------------------------------------------------------------- */

    /* Target orbit (EDD §4.4.2; TM-X-62831 Table 3B). */
    double target_i_deg;          /* inclination */
    double target_lambda_0_deg;   /* descending node longitude */
    double target_V_T_mps;        /* terminal velocity magnitude */
    double target_R_T_m;          /* terminal radius */
    double target_Theta_T_deg;    /* downrange angle */
    double target_Xdotdot_VGT_mps2; /* target vertical accel */

    /* Stage performance (TM-X-62831 Table 3B). */
    double V_ex1_mps;             /* S-IVB first-phase exhaust velocity */
    double V_ex3_mps;             /* S-IVB second-phase exhaust velocity */
    double Mdot_1_kgps;           /* S-IB mass flow */
    double Mdot_3_kgps;           /* S-IVB second-phase mass flow */
    double F1_N;                  /* S-IB thrust */
    double F3_N;                  /* S-IVB thrust */
    double M1_kg;                 /* S-IB initial mass */
    double M03_kg;                /* S-IVB initial mass */

    /* IGM phasing presets (TM-X-62831 Table 3B; EDD §4.4.2.3). */
    double igm_T_1i_initial_sec;  /* T_1i initial value */
    double igm_T_3i_initial_sec;  /* T_3i initial value */
    double igm_tau_3_initial_sec; /* tau_3 at Phase 2 initiation */
    double igm_tau_10_sec;        /* tau_1 at EMRC (Eq. 4.4.10) */
    double igm_T_10_sec;          /* T_1 at EMRC (Eqs. 4.4.10-11) */
    double igm_P_CMR_sec;         /* artificial-tau cutover (Eq. 4.4.14) */
    double igm_eps_2_sec;         /* T_3i to start Ẋ-steering (§4.4.2.4 ¶1) */
    double igm_M_GR_kg;           /* mass remaining at EMRC (Eq. 4.4.11) */
    double igm_Mdot_2_kgps;       /* S-IVB first-phase mass flow */
    double igm_SMCG;              /* SMC gain (Eqs. 4.4.83-84) */
    double igm_ROV;               /* engine misalignment factor (Eq. 4.4.31) */

    /* IGM frame constants (TM-X-62831 Table 3B; EDD §2.3.5).
     * phi_L and A_z are shared with the launch-site block above;
     * lambda_N_deg (TMX phi_N) and i_orb_deg are IGM-specific. */
    double igm_lambda_N_deg;      /* descending-node angle */
    double igm_i_orb_deg;         /* target orbit inclination */

    /* IGM enable timing (TM-X-62831 Table 3B). The LVDC starts IGM
     * at T3 + igm_enable_from_T3_sec. Full T3 bookkeeping is a
     * Phase D deliverable; the dispatcher currently uses a T1-based
     * fallback (see igm_enable_from_T1_sec). */
    double igm_enable_from_T3_sec;
    double igm_enable_from_T1_sec;

    /* HSL cutoff (EDD §4.4.2.4; Part II). */
    double DeltaV_b_mps;          /* velocity cutoff bias, Part II T1-3 */
    double V_GRD_mps;             /* vigained threshold for HSL entry */
    double T_HSL_sec;             /* HSL time */

    /* -----------------------------------------------------------------
     * Chi time-tilt — EDD §4.4.1.2 Eq. 4.4.2, with AS-205 values from
     * TM-X-62831 Appendix B (S-IB Stage Unbiased Pitch Program).
     *
     * The pitch command `chi_y(T)` is a piecewise cubic polynomial
     * in `T`. The segments are:
     *
     *   T ≤ T_S1_sec1_sec              : chi_y = 0   (vertical hold)
     *   T_S1_sec1_sec < T ≤ T_S1_sec   : F_1(T)
     *   T_S1_sec      < T ≤ T_S2_sec   : F_2(T)
     *   T_S2_sec      < T ≤ T_S3_sec   : F_3(T)
     *   T > T_S3_sec                   : P_final_rad   (freeze)
     *
     * Each F_i is `a0 + a1·T + a2·T² + a3·T³` in radians.
     *
     * Time variable: `T` is "time from Time Base 1" per TM-X-62831
     * Appendix B (unbiased). The EDD §4.4.1 Eq. 4.4.1 defines
     * `T_c = T1 − T_d`, where `T_d ≈ 1.24 s` is the compute-to-
     * command delay. The TM-X-62831 coefficients are calibrated for
     * the unbiased time variable, matching the trajectory analysis
     * assumption of instantaneous command application. The rewrite
     * follows TM-X-62831 literally and does not apply `T_d`.
     *
     * Roll (chi_x) per TM-X-62831 Appendix B:
     *   T ≤ 10 s : chi_x = chi_x_pad_rad (= −0.488692 rad = −28°)
     *   T > 10 s : chi_x = 0
     *
     * Yaw (chi_z) per TM-X-62831 Appendix B: chi_z = 0 throughout
     * S-IB phase.
     * -------------------------------------------------------------- */
    double P_final_rad;           /* chi_y after the final segment */
    double T_S1_sec1_sec;         /* gantry-clear time (chi_y held at 0) */
    double T_S1_sec;
    double T_S2_sec;
    double T_S3_sec;
    double chi_y_F1[4];           /* F_1 cubic coefficients, a0..a3 */
    double chi_y_F2[4];           /* F_2 cubic coefficients, a0..a3 */
    double chi_y_F3[4];           /* F_3 cubic coefficients, a0..a3 */
    double chi_x_pad_rad;         /* roll command during pad hold */

    /* -----------------------------------------------------------------
     * Minor-loop attitude increment limits — EDD §8.3.1 Eq. 8.3.1
     *
     * The LVDC ramps the guidance commands `chi_i` into the minor
     * loop `chi'_i` with a fixed increment per minor loop. The
     * magnitude of each increment must not exceed its mission-
     * dependent limit.
     *
     * TM-X-62831 (p. 2) states the AS-205 boost rate limits:
     *   "Guidance command angle rate limit of 1 deg/sec in pitch
     *    and yaw". The current body uses the same value in roll.
     * -------------------------------------------------------------- */
    double chi_rate_limit_radps[3]; /* rate limits in {roll, pitch, yaw} */

    /* -----------------------------------------------------------------
     * Attitude-error command output pipeline — EDD §8.2.3
     *
     * After the gimbal-to-body transform (§8.3.2), each attitude-
     * error signal is magnitude-limited, rate-limited, and sent to
     * the LVDA D/A ladder. The LSB is 0.0575 deg per the Saturn V
     * hardware specification (see docs/LVDC_QUANTIZATION_INVENTORY.md
     * for the EDD-vs-hw discrepancy).
     *
     * The current body uses:
     *   magnitude limit = 15.3 deg per axis
     *   rate limit      = 12  deg/sec per axis
     * These are mission-dependent per EDD Part II Event Sequence
     * Timeline Table; values pulled from the current body pending
     * Part II verification.
     * -------------------------------------------------------------- */
    double psi_mag_limit_rad;     /* per-axis saturation on Psi */
    double psi_rate_limit_radps;  /* per-axis rate limit on Psi */
    double dac_lsb_rad;           /* D/A quantization LSB (0.0575 deg) */

} lvdc_params_t;

/* Return the AS-205 preset. Values anchored to the citations above. */
lvdc_params_t lvdc_params_as205(void);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_PARAMS_H */
