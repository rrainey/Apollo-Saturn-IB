/**
 * Saturn IB stack ExternalInputs / ExternalOutputs — RTW-layout compatible,
 * no MATLAB / Simulink headers required.
 *
 * Layout must match Saturn_IB_Stack_grt_malloc_rtw/Saturn_IB_Stack.h so
 * batch_sim / ApolloA field access stays identical across backends.
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
  real_T OUT11[25];
  real_T OUT12[24];
  boolean_T bStageSep;
  boolean_T bIECO;
  boolean_T bOECO;
  boolean_T bS_IVB_EngineStart;
  real_T veh_q_ECI[4];
  boolean_T bLiftoff;
  real_T BodyToSM_Phi_deg;
  real_T BodyToSM_Theta_deg;
  real_T BodyToSM_Psi_deg;
  real_T OUT22[9];
} ExternalOutputs_saturn_ib_stack;

#ifdef __cplusplus
}
#endif
