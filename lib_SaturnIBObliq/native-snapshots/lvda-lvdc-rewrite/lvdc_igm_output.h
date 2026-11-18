/*
 * lvdc_igm_output.h — IGM final output stage
 * (EDD §4.4.2.2 Eqs. 4.4.83 through 4.4.87).
 *
 * The output stage runs AFTER the position-correction block
 * (lvdc_igm_position_correction.h) and produces the pitch and yaw
 * commands chi_y, chi_z referred to the S-system that the minor
 * loop consumes.
 *
 * Three pieces:
 *   1. Steering Misalignment Correction (SMC) integrators
 *      SMCY, SMCZ (Eqs. 4.4.83-84). These accumulate a running
 *      estimate of the thrust-axis / inertial-axis misalignment
 *      so the next BML can compensate. The state (SMCY, SMCZ,
 *      chi_y_past, chi_z_past) persists across BMLs and is held
 *      by the caller.
 *
 *   2. Thrust-vector transform (Eq. 4.4.85). Builds the unit
 *      thrust direction in the 4-system from chi_y4, chi_z4 and
 *      rotates it to S-system via [MS4]^T.
 *
 *      Note on the OCR of Eq. 4.4.85: the EDD HTML renders the
 *      column vector as a 3x2 matrix because LaTeX `&`
 *      multiplication separators were misinterpreted as column
 *      delimiters. The physically-correct form is a 3x1 column
 *      vector of per-row products:
 *         [ cos(chi_y4) * cos(chi_z4) ]
 *         [         sin(chi_z4)        ]
 *         [-sin(chi_y4) * cos(chi_z4) ]
 *      This is the standard IGM unit-thrust direction in the
 *      4-system.
 *
 *   3. Final guidance commands in S-system (Eqs. 4.4.86-87).
 *      chi_y = Arctan(-F'_Sz / F'_Sx) + SMCY
 *      chi_z = Arctan(F'_Sy / sqrt(1 - F'^2_Sy)) + SMCZ
 *      Note: for a unit F'_S the denominator sqrt(1 - F'^2_Sy)
 *      equals sqrt(F'^2_Sx + F'^2_Sz), so atan2 is numerically
 *      preferable near F'_Sy = +/-1.
 */

#ifndef LVDC_IGM_OUTPUT_H
#define LVDC_IGM_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/* SMC state carried across BMLs by the caller. */
typedef struct {
    double SMCY;         /* integrated pitch misalignment correction (rad) */
    double SMCZ;         /* integrated yaw   misalignment correction (rad) */
    double chi_y_past;   /* chi_y from previous BML (rad) */
    double chi_z_past;   /* chi_z from previous BML (rad) */
    int    primed;       /* 0 before first update, 1 after (dispatcher) */
} lvdc_igm_smc_state_t;

/* EDD Eqs. 4.4.83-84 — SMC integrator step.
 *
 * Inputs:
 *   smc        : current SMC state (SMCY, SMCZ, chi_y_past, chi_z_past).
 *                Updated in place.
 *   SMCG       : SMC gain (preset, dimensionless).
 *   chi_y_rad  : chi_y from this BML (output of final command,
 *                i.e. passed in by dispatcher after computing it).
 *   chi_z_rad  : chi_z from this BML.
 *   xddot_m_mps2, yddot_m_mps2, zddot_m_mps2 : measured specific
 *                acceleration in body frame (post-transform).
 *   MoverF_s2pm : (M/F)_S per Eq. 4.4.6, s^2/m.
 *   dT_sec      : BML time step (ΔT in EDD).
 *
 * The /pi literal in Eqs. 4.4.83-84 is preserved verbatim (LVDC
 * half-revolution scaling). See lvdc_igm_position_correction.h
 * ANGLE-UNIT CONVENTION note. */
void lvdc_igm_smc_update(lvdc_igm_smc_state_t *smc,
                         double SMCG,
                         double chi_y_rad,
                         double chi_z_rad,
                         double xddot_m_mps2,
                         double yddot_m_mps2,
                         double zddot_m_mps2,
                         double MoverF_s2pm,
                         double dT_sec);

/* EDD Eq. 4.4.85 — thrust-vector transform.
 *
 * F_prime_S = [MS4]^T * [cos chi_y4 cos chi_z4,
 *                         sin chi_z4,
 *                        -sin chi_y4 cos chi_z4]^T
 */
void lvdc_igm_thrust_vector_S(double MS4[3][3],
                              double chi_y4_rad,
                              double chi_z4_rad,
                              double F_prime_S[3]);

/* EDD Eq. 4.4.86 — chi_y = Arctan(-F'_Sz / F'_Sx) + SMCY.
 * Uses atan2(-F'_Sz, F'_Sx) for quadrant correctness. */
double lvdc_igm_chi_y_final(const double F_prime_S[3], double SMCY_rad);

/* EDD Eq. 4.4.87 — chi_z = Arctan(F'_Sy / sqrt(1 - F'^2_Sy)) + SMCZ.
 * For a unit thrust vector this is equivalent to
 *   atan2(F'_Sy, sqrt(F'^2_Sx + F'^2_Sz))
 * which is numerically stable near F'_Sy = +/-1. */
double lvdc_igm_chi_z_final(const double F_prime_S[3], double SMCZ_rad);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_IGM_OUTPUT_H */
