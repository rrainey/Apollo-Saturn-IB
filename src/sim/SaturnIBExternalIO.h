/**
 * Saturn IB stack ExternalInputs / ExternalOutputs — plain C structs. These are the consumer-facing I/O
 * types for batch_sim and apolloa.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef double  saturnib_real_T;
typedef int     saturnib_boolean_T;

#ifndef real_T
typedef saturnib_real_T real_T;
#endif
#ifndef boolean_T
typedef saturnib_boolean_T boolean_T;
#endif

typedef struct _ExternalInputs_saturn_ib_stack_tag {
  real_T LaunchDate[6];
  real_T A_z_deg;
  real_T CG_LLA_deg_m[3];
  real_T q_ECItoSM[4];
  real_T T_L_prime_sec;
} ExternalInputs_saturn_ib_stack;

typedef struct _ExternalOutputs_saturn_ib_stack_tag {
  real_T FDAI_Roll_deg;
  real_T FDAI_Pitch_deg;
  real_T FDAI_Yaw_deg;
  real_T FDAI_RollDot_dps;
  real_T FDAI_PitchDot_dps;
  real_T FDAI_YawDot_dps;
  real_T CM_IMU_PIPA_Pulses[3];
  real_T CM_IMU_IGA_rad;
  real_T CM_IMU_MGA_Rad;
  real_T CM_IMU_OGA_rad;
  /* OUT11 — S-IB stage state (per MDL design intent): pre-sep tracks the
   * mated stack's position/velocity/attitude; post-sep tracks the spent
   * S-IB on its ballistic arc.  OUT12 — S-IVB stage state: zero while the
   * S-IVB integrator is disabled pre-sep; post-sep carries the active
   * S-IVB trajectory.  Pre-liftoff both channels report 0 or NaN in the
   * lat/lon/h fields because the stage integrators are disabled —
   * consumers should use OUT22 for the vehicle's geographic location
   * until bLiftoff goes true.
   *
   * Native plant fields occupy OUT11[0..24] and OUT12[0..23] (OUT12[21..23]
   * are patched in by the adapter from the S-IVB body-velocity signal).
   * Trailing slot OUT11[25] / OUT12[24] carries a Fischer-1960
   * geocentric-radial altitude computed in the adapter from the stage's
   * Xe vector; see docs/ALTITUDE_CONVENTIONS.md.  The native OUTxx[2]
   * altitude remains WGS-84 geodetic. */
  real_T OUT11[26];
  real_T OUT12[25];
  boolean_T bStageSep;
  boolean_T bIECO;
  boolean_T bOECO;
  boolean_T bS_IVB_EngineStart;
  real_T veh_q_ECI[4];
  boolean_T bLiftoff;
  real_T BodyToSM_Phi_deg;
  real_T BodyToSM_Theta_deg;
  real_T BodyToSM_Psi_deg;
  /* OUT22 — S-IB pad reference state.  OUT22[0..8] are the plant's native
   * pad lat/lon/h + Xe + Ve pack; OUT22[9] is a Fischer-1960
   * geocentric-radial altitude computed by the adapter (see note above). */
  real_T OUT22[10];
} ExternalOutputs_saturn_ib_stack;

#ifdef __cplusplus
}
#endif
