#ifndef SATURN_IB_STACK_H
#define SATURN_IB_STACK_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Segregated subsystem headers */
#include "LVDA_LVDC.h"

/*
 * Model Parameters
 */
#define i_deg 31.605
#define PARAM_i_deg i_deg
#define lambda_0_deg 119
#define PARAM_lambda_0_deg lambda_0_deg
#define V_T_mps 7780.67
#define PARAM_V_T_mps V_T_mps
#define Xdotdot_VGT_mps2 -9.15
#define PARAM_Xdotdot_VGT_mps2 Xdotdot_VGT_mps2
#define R_T_m 6600405
#define PARAM_R_T_m R_T_m
#define Theta_T_deg 0
#define PARAM_Theta_T_deg Theta_T_deg
/* Parameter A_z_deg conflicts with a signal name; use PARAM_A_z_deg */
#define PARAM_A_z_deg 72
#define phi_L_deg 28.521963
#define PARAM_phi_L_deg phi_L_deg
#define phi_L_prime_deg 28.360795
#define PARAM_phi_L_prime_deg phi_L_prime_deg
#define lambda_L_deg -80.561141
#define PARAM_lambda_L_deg lambda_L_deg
#define R_L_m 6373385
#define PARAM_R_L_m R_L_m
#define pad_roll_L_deg 100
#define PARAM_pad_roll_L_deg pad_roll_L_deg
#define epsilon_2_sec 15
#define PARAM_epsilon_2_sec epsilon_2_sec
#define epsilon_prime_sec 5
#define PARAM_epsilon_prime_sec epsilon_prime_sec
#define BN_1_sec 14.4
#define PARAM_BN_1_sec BN_1_sec
#define T3_FM_sec 5
#define PARAM_T3_FM_sec T3_FM_sec
#define T3_IGM_sec 25
#define PARAM_T3_IGM_sec T3_IGM_sec
#define DeltaV_b_mps 7.2381
#define PARAM_DeltaV_b_mps DeltaV_b_mps
#define V_GRD_mps 150
#define PARAM_V_GRD_mps V_GRD_mps
#define T_HSL_sec 5
#define PARAM_T_HSL_sec T_HSL_sec
#define T10_sec 286.3
#define PARAM_T10_sec T10_sec
#define A0_sec 0
#define PARAM_A0_sec A0_sec
#define A1 0
#define PARAM_A1 A1
#define Tr1 1.8
#define PARAM_Tr1 Tr1
#define Tr2 0
#define PARAM_Tr2 Tr2
#define Tr3 0
#define PARAM_Tr3 Tr3
#define T2_sec 0
#define PARAM_T2_sec T2_sec
#define delta_T_sec 1.6
#define PARAM_delta_T_sec delta_T_sec
#define tau_1_i_preset_sec 286.3
#define PARAM_tau_1_i_preset_sec tau_1_i_preset_sec
#define tau_3_i_preset_sec 159.8
#define PARAM_tau_3_i_preset_sec tau_3_i_preset_sec
#define tau_3_preset_sec 326.5
#define PARAM_tau_3_preset_sec tau_3_preset_sec
#define V_ex1_mps 4151
#define PARAM_V_ex1_mps V_ex1_mps
#define V_ex3_mps 4233.2
#define PARAM_V_ex3_mps V_ex3_mps
#define PC0_sec 0
#define PARAM_PC0_sec PC0_sec
#define tau_1_0_sec 536
#define PARAM_tau_1_0_sec tau_1_0_sec
#define C_0_sec 35
#define PARAM_C_0_sec C_0_sec
#define Mdot_1_kg_per_sec 239.7877
#define PARAM_Mdot_1_kg_per_sec Mdot_1_kg_per_sec
#define Mdot_3_kg_per_sec 183.1797
#define PARAM_Mdot_3_kg_per_sec Mdot_3_kg_per_sec
#define m_liftoff_kg 585943
#define PARAM_m_liftoff_kg m_liftoff_kg
#define m_separation_kg 138709.90752311
#define PARAM_m_separation_kg m_separation_kg
#define M1_kg 585943
#define PARAM_M1_kg M1_kg
#define M02_kg 129761.6
#define PARAM_M02_kg M02_kg
#define M03_kg 134374
#define PARAM_M03_kg M03_kg
#define Mdot_2_sib_kg_per_sec 2820.8
#define PARAM_Mdot_2_sib_kg_per_sec Mdot_2_sib_kg_per_sec
#define F1_N 7600000
#define PARAM_F1_N F1_N
#define F2_N 995355.2
#define PARAM_F2_N F2_N
#define F3_N 775441.26
#define PARAM_F3_N F3_N
#define mu_earth 398600441800000
#define PARAM_mu_earth mu_earth
#define R_earth_m 6371000
#define PARAM_R_earth_m R_earth_m
#define omega_E_rps 0.00002321
#define PARAM_omega_E_rps omega_E_rps

#ifdef __cplusplus
extern "C" {
#endif

/* Model input signals */
typedef struct {
    double LaunchDate[6]; /* Input port: LaunchDate */
    double A_z_deg; /* Input port: A_z_deg */
    double CG_LLA_deg_m[3]; /* Input port: CG_LLA_deg_m */
    double q_ECItoSM[4][1]; /* Input port: q_ECItoSM */
    double T_L_prime_sec; /* Input port: T_L_prime_sec */
} saturn_ib_stack_inputs_t;

/* Model output signals */
typedef struct {
    double FDAI_Roll_deg; /* Output port: FDAI_Roll_deg */
    double FDAI_Pitch_deg; /* Output port: FDAI_Pitch_deg */
    double FDAI_Yaw_deg; /* Output port: FDAI_Yaw_deg */
    double FDAI_RollDot_dps; /* Output port: FDAI_RollDot_dps */
    double FDAI_PitchDot_dps; /* Output port: FDAI_PitchDot_dps */
    double FDAI_YawDot_dps; /* Output port: FDAI_YawDot_dps */
    double CM_IMU_PIPA_Pulses[3]; /* Output port: CM_IMU_PIPA_Pulses */
    double CM_IMU_IGA_rad; /* Output port: CM_IMU_IGA_rad */
    double CM_IMU_MGA_Rad; /* Output port: CM_IMU_MGA_Rad */
    double CM_IMU_OGA_rad; /* Output port: CM_IMU_OGA_rad */
    double Out11[25]; /* Output port: Out11 */
    double Out12[22]; /* Output port: Out12 */
    bool bStageSep; /* Output port: bStageSep */
    bool bIECO; /* Output port: bIECO */
    bool bOECO; /* Output port: bOECO */
    bool bS_IVB_EngineStart; /* Output port: bS_IVB_EngineStart */
    double veh_q_ECI[4][1]; /* Output port: veh_q_ECI */
    bool bLiftoff; /* Output port: bLiftoff */
    double BodyToSM_Phi_deg; /* Output port: BodyToSM_Phi_deg */
    double BodyToSM_Theta_deg; /* Output port: BodyToSM_Theta_deg */
    double BodyToSM_Psi_deg; /* Output port: BodyToSM_Psi_deg */
    double Out22[9]; /* Output port: Out22 */
} saturn_ib_stack_outputs_t;

/* Internal signal values */
typedef struct {
    double LaunchDate[6]; /* Signal from input port: LaunchDate */
    double A_z_deg; /* Signal from input port: A_z_deg */
    double CG_LLA_deg_m[3]; /* Signal from input port: CG_LLA_deg_m */
    double q_ECItoSM[4][1]; /* Signal from input port: q_ECItoSM */
    double T_L_prime_sec; /* Signal from input port: T_L_prime_sec */
    double Body_to_ECI_Math_Function[3][3];
    double Body_to_ECI_Product[3];
    double Body_to_ECI_Sum[3];
    double Body_to_ECI_Sum1[3];
    double CM_ISS_Angle_Conversion[3];
    double CM_ISS_Angle_Conversion1;
    double CM_ISS_Angle_Conversion2;
    double CM_ISS_Angle_Conversion3;
    double CM_ISS_Angle_Conversion4;
    double CM_ISS_Angle_Conversion5;
    double CM_ISS_Angle_Conversion6;
    double CM_ISS_BODYtoECI[3][3];
    double CM_ISS_Demux_0;
    double CM_ISS_Demux_1;
    double CM_ISS_Demux_2;
    double CM_ISS_Demux1_0;
    double CM_ISS_Demux1_1;
    double CM_ISS_Demux1_2;
    double CM_ISS_Demux2_0;
    double CM_ISS_Demux2_1;
    double CM_ISS_Demux2_2;
    double CM_ISS_Demux3_0;
    double CM_ISS_Demux3_1;
    double CM_ISS_Demux3_2;
    double CM_ISS_Demux4_0;
    double CM_ISS_Demux4_1;
    double CM_ISS_Demux4_2;
    double CM_ISS_Direction_Cosine_Matrix_to_Euler_Angles[3];
    double CM_ISS_ECItoBODY[3][3];
    double CM_ISS_ECItoSM[3][3];
    double CM_ISS_Ground;
    double CM_ISS_Ground1;
    double CM_ISS_Mux[3];
    double CM_ISS_Product2[3];
    double CM_ISS_Product3[3][3];
    double CM_ISS_Rounding_Function;
    double CM_ISS_Rounding_Function1;
    double CM_ISS_Rounding_Function2;
    double CM_ISS_Stable_Member_Gimbal_Angles2_Cos_MGA_Sin_OGA;
    double CM_ISS_Stable_Member_Gimbal_Angles2_sin_IGA_cos_MGA;
    double CM_ISS_Stable_Member_Gimbal_Angles2_Cos_MGA_Cos_OGA;
    double CM_ISS_Stable_Member_Gimbal_Angles2_Mux2[3];
    double CM_ISS_Stable_Member_Gimbal_Angles2_Trigonometric_Function1;
    double CM_ISS_Stable_Member_Gimbal_Angles2_Trigonometric_Function3;
    double CM_ISS_Stable_Member_Gimbal_Angles2_Trigonometric_Function4;
    double CM_ISS_Stable_Member_Gimbal_Angles2_cos_IGA_cos_MGA;
    double CM_ISS_Stable_Member_Gimbal_Angles2_sin_MGA;
    double CM_ISS_m_sec_to_pulses[3];
    double CM_ISS_velocity_in_SM_frame[3];
    double Demux1_0;
    double Demux1_1;
    double Demux1_2;
    double Demux3_0;
    double Demux3_1;
    double Demux3_2;
    double Earth_s_Rotation_Angle_Conversion3;
    double Earth_s_Rotation_Product2;
    double Earth_s_Rotation_Sum;
    double Earth_s_Rotation_Time_sec;
    double Earth_s_Rotation_omega_e_rad_per_sec;
    double Ground;
    double Initial_Conditions__3x3_Cross_Product[3];
    double Initial_Conditions_Angle_Conversion;
    double Initial_Conditions_Angle_Conversion1;
    double Initial_Conditions_Angle_Conversion2;
    double Initial_Conditions_Date_to_JD_Add;
    double Initial_Conditions_Date_to_JD_B;
    double Initial_Conditions_Date_to_JD_Bias;
    double Initial_Conditions_Date_to_JD_Bias1;
    double Initial_Conditions_Date_to_JD_Bias2;
    double Initial_Conditions_Date_to_JD_C;
    double Initial_Conditions_Date_to_JD_C1;
    bool Initial_Conditions_Date_to_JD_Compare_To_Constant;
    double Initial_Conditions_Date_to_JD_Demux_0;
    double Initial_Conditions_Date_to_JD_Demux_1;
    double Initial_Conditions_Date_to_JD_Demux_2;
    double Initial_Conditions_Date_to_JD_Demux_3;
    double Initial_Conditions_Date_to_JD_Demux_4;
    double Initial_Conditions_Date_to_JD_Demux_5;
    double Initial_Conditions_Date_to_JD_Mux[6];
    double Initial_Conditions_Date_to_JD_Switch;
    double Initial_Conditions_Date_to_JD_Switch1;
    double Initial_Conditions_Demux_0;
    double Initial_Conditions_Demux_1;
    double Initial_Conditions_Demux_2;
    double Initial_Conditions_Direction_Cosine_Matrix_to_Quaternion[4][1];
    double Initial_Conditions_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Add;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Angle_Conversion;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Angle_Conversion1;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Angle_Conversion2;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Fcn;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Fcn1;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Fcn2;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Fcn3;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Fcn4;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Fcn5;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Mux[9];
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Mux1[3];
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_Mux2[3];
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos1_sin;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos1_cos;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos2_sin;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos2_cos;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos3_sin;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos3_cos;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos4_sin;
    double Initial_Conditions_Initial_Position_and_Velocity_Eqns_3_4_3_4_SinCos4_cos;
    double Initial_Conditions_L_V_Inertial_Orientation_Angle_Conversion2[3];
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__11;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__12;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__13;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__21;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__22;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__23;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__31;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__32;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__33;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux[6];
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux2[3][3];
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_SinCos3_sin[3];
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_SinCos3_cos[3];
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900047_0;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900047_1;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900047_2;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900052_0;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900052_1;
    double Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900052_2;
    double Initial_Conditions_L_V_Inertial_Orientation_Mux[3];
    double Initial_Conditions_LLA_to_ECF_Add;
    double Initial_Conditions_LLA_to_ECF_Add1;
    double Initial_Conditions_LLA_to_ECF_Bias;
    double Initial_Conditions_LLA_to_ECF_Divide;
    double Initial_Conditions_LLA_to_ECF_Divide1;
    double Initial_Conditions_LLA_to_ECF_Math_Function;
    double Initial_Conditions_LLA_to_ECF_Math_Function1;
    double Initial_Conditions_LLA_to_ECF_Math_Function2;
    double Initial_Conditions_LLA_to_ECF_Math_Function3;
    double Initial_Conditions_LLA_to_ECF_Mux[3];
    double Initial_Conditions_LLA_to_ECF_Product;
    double Initial_Conditions_LLA_to_ECF_Product1;
    double Initial_Conditions_LLA_to_ECF_Product2;
    double Initial_Conditions_LLA_to_ECF_Product3;
    double Initial_Conditions_LLA_to_ECF_Product4;
    double Initial_Conditions_LLA_to_ECF_SinCos_sin;
    double Initial_Conditions_LLA_to_ECF_SinCos_cos;
    double Initial_Conditions_LLA_to_ECF_SinCos1_sin;
    double Initial_Conditions_LLA_to_ECF_SinCos1_cos;
    double Initial_Conditions_LLA_to_ECF_Unary_Minus;
    double Initial_Conditions_LLA_to_ECF_WGS_84_ECC_2;
    double Initial_Conditions_LLA_to_ECF_WGS_84_major;
    double Initial_Conditions_LLA_to_ECF_WGS_84_minor;
    double Initial_Conditions_MES_Transform_Constant2;
    double Initial_Conditions_MES_Transform_Constant8;
    double Initial_Conditions_MES_Transform_Constant9;
    double Initial_Conditions_MES_Transform_MES_Transform_Angle_Conversion2[3];
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__11;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__12;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__13;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__21;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__22;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__23;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__31;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__32;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__33;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux[6];
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux2[3][3];
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_SinCos3_sin[3];
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_SinCos3_cos[3];
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900057_0;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900057_1;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900057_2;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900062_0;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900062_1;
    double Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900062_2;
    double Initial_Conditions_MES_Transform_MES_Transform_Mux[3];
    double Initial_Conditions_MES_Transform_Sum2;
    double Initial_Conditions_Math_Function[3][3];
    double Initial_Conditions_Mux[3];
    double Initial_Conditions_Position_1_Azimuth_deg;
    double Initial_Conditions_Product[3];
    double Initial_Conditions_Product1[3];
    double Initial_Conditions_Product2[3];
    double Initial_Conditions_Product3[3];
    double Initial_Conditions_R_L;
    double Initial_Conditions_Sum;
    double Initial_Conditions_Sum1[3];
    double Initial_Conditions_Sum2[3];
    double Initial_Conditions_T_to_GMST_Math_Function;
    double Initial_Conditions_T_to_GMST_eqn;
    double Initial_Conditions_T_to_GMST_one_day;
    double Initial_Conditions_T_to_GMST_seconds_to_degrees;
    double Initial_Conditions_T_UT1;
    double Initial_Conditions_Unary_Minus;
    double Initial_Conditions_omega_E_rad_per_sec[3];
    double Initial_Conditions_phi_L_prime_deg;
    double Initial_Conditions_zero;
    bool LES_Jettison;
    bool Logical_Operator;
    double MES_Transform_Constant2;
    double MES_Transform_Constant8;
    double MES_Transform_Constant9;
    double MES_Transform_MES_Transform_Angle_Conversion2[3];
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__11;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__12;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__13;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__21;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__22;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__23;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__31;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__32;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__33;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux[6];
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux2[3][3];
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_SinCos3_sin[3];
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_SinCos3_cos[3];
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900067_0;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900067_1;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900067_2;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900072_0;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900072_1;
    double MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900072_2;
    double MES_Transform_MES_Transform_Mux[3];
    double MES_Transform_Sum2;
    double Memory1[3];
    double Memory2[3];
    bool Memory3;
    double Memory4[3];
    bool Memory5;
    double Memory6[3];
    double Mux[25];
    double Mux1[22];
    double On_Pad_Add2[3];
    double On_Pad_Angle_Conversion4;
    double On_Pad_Angle_Conversion5;
    double On_Pad_Centrifugal_Acceleration__3x3_Cross_Product[3];
    double On_Pad_Centrifugal_Acceleration__3x3_Cross_Product1[3];
    double On_Pad_Centrifugal_Acceleration_Product1[3];
    double On_Pad_Centrifugal_Acceleration_omega_E_rad_per_sec[3];
    double On_Pad_ECI_to_LLA_Add1;
    double On_Pad_ECI_to_LLA_Add10;
    double On_Pad_ECI_to_LLA_Add11;
    double On_Pad_ECI_to_LLA_Add12;
    double On_Pad_ECI_to_LLA_Add13;
    double On_Pad_ECI_to_LLA_Add14;
    double On_Pad_ECI_to_LLA_Add15;
    double On_Pad_ECI_to_LLA_Add16;
    double On_Pad_ECI_to_LLA_Add17;
    double On_Pad_ECI_to_LLA_Add18;
    double On_Pad_ECI_to_LLA_Add19;
    double On_Pad_ECI_to_LLA_Add2;
    double On_Pad_ECI_to_LLA_Add3;
    double On_Pad_ECI_to_LLA_Add4;
    double On_Pad_ECI_to_LLA_Add5;
    double On_Pad_ECI_to_LLA_Add6;
    double On_Pad_ECI_to_LLA_Add7;
    double On_Pad_ECI_to_LLA_Add8;
    double On_Pad_ECI_to_LLA_Add9;
    double On_Pad_ECI_to_LLA_Bias;
    double On_Pad_ECI_to_LLA_Bias1;
    double On_Pad_ECI_to_LLA_Bias2;
    bool On_Pad_ECI_to_LLA_Compare_To_Constant;
    bool On_Pad_ECI_to_LLA_Compare_To_Zero;
    double On_Pad_ECI_to_LLA_Demux_0;
    double On_Pad_ECI_to_LLA_Demux_1;
    double On_Pad_ECI_to_LLA_Demux_2;
    double On_Pad_ECI_to_LLA_Divide2;
    double On_Pad_ECI_to_LLA_Divide4;
    double On_Pad_ECI_to_LLA_Divide5;
    double On_Pad_ECI_to_LLA_Divide6;
    double On_Pad_ECI_to_LLA_Divide7;
    double On_Pad_ECI_to_LLA_Divide8;
    double On_Pad_ECI_to_LLA_Gain;
    double On_Pad_ECI_to_LLA_Gain1;
    double On_Pad_ECI_to_LLA_Gain2;
    double On_Pad_ECI_to_LLA_Gain3;
    double On_Pad_ECI_to_LLA_Gain4;
    double On_Pad_ECI_to_LLA_Gain5;
    double On_Pad_ECI_to_LLA_Gain6;
    double On_Pad_ECI_to_LLA_Math_Function1;
    double On_Pad_ECI_to_LLA_Math_Function10;
    double On_Pad_ECI_to_LLA_Math_Function11;
    double On_Pad_ECI_to_LLA_Math_Function12;
    double On_Pad_ECI_to_LLA_Math_Function13;
    double On_Pad_ECI_to_LLA_Math_Function14;
    double On_Pad_ECI_to_LLA_Math_Function15;
    double On_Pad_ECI_to_LLA_Math_Function16;
    double On_Pad_ECI_to_LLA_Math_Function17;
    double On_Pad_ECI_to_LLA_Math_Function18;
    double On_Pad_ECI_to_LLA_Math_Function2;
    double On_Pad_ECI_to_LLA_Math_Function3;
    double On_Pad_ECI_to_LLA_Math_Function4;
    double On_Pad_ECI_to_LLA_Math_Function5;
    double On_Pad_ECI_to_LLA_Math_Function6;
    double On_Pad_ECI_to_LLA_Math_Function7;
    double On_Pad_ECI_to_LLA_Math_Function8;
    double On_Pad_ECI_to_LLA_Math_Function9;
    double On_Pad_ECI_to_LLA_Product1;
    double On_Pad_ECI_to_LLA_Product10;
    double On_Pad_ECI_to_LLA_Product11;
    double On_Pad_ECI_to_LLA_Product12;
    double On_Pad_ECI_to_LLA_Product2;
    double On_Pad_ECI_to_LLA_Product3;
    double On_Pad_ECI_to_LLA_Product4;
    double On_Pad_ECI_to_LLA_Product5;
    double On_Pad_ECI_to_LLA_Product6;
    double On_Pad_ECI_to_LLA_Product7;
    double On_Pad_ECI_to_LLA_Product8;
    double On_Pad_ECI_to_LLA_Product9;
    double On_Pad_ECI_to_LLA_Sign;
    double On_Pad_ECI_to_LLA_SinCos1_sin;
    double On_Pad_ECI_to_LLA_SinCos1_cos;
    double On_Pad_ECI_to_LLA_Switch;
    double On_Pad_ECI_to_LLA_Switch1;
    double On_Pad_ECI_to_LLA_Trigonometric_Function;
    double On_Pad_ECI_to_LLA_Trigonometric_Function1;
    double On_Pad_ECI_to_LLA_Trigonometric_Function2;
    double On_Pad_ECI_to_LLA_Trigonometric_Function3;
    double On_Pad_ECI_to_LLA_Unary_Minus;
    double On_Pad_ECI_to_LLA_Unary_Minus1;
    double On_Pad_ECI_to_LLA_a;
    double On_Pad_ECI_to_LLA_a1;
    double On_Pad_ECI_to_LLA_a2;
    double On_Pad_ECI_to_LLA_a3;
    double On_Pad_ECI_to_LLA_b;
    double On_Pad_Earth_Gravity_Model_Add;
    double On_Pad_Earth_Gravity_Model_Bias;
    double On_Pad_Earth_Gravity_Model_Bias1;
    double On_Pad_Earth_Gravity_Model_Bias2;
    double On_Pad_Earth_Gravity_Model_Demux_0;
    double On_Pad_Earth_Gravity_Model_Demux_1;
    double On_Pad_Earth_Gravity_Model_Demux_2;
    double On_Pad_Earth_Gravity_Model_Divide[3];
    double On_Pad_Earth_Gravity_Model_Divide1;
    double On_Pad_Earth_Gravity_Model_Divide2;
    double On_Pad_Earth_Gravity_Model_GM;
    double On_Pad_Earth_Gravity_Model_Gain;
    double On_Pad_Earth_Gravity_Model_Gain1;
    double On_Pad_Earth_Gravity_Model_J1;
    double On_Pad_Earth_Gravity_Model_J2;
    double On_Pad_Earth_Gravity_Model_Math_Function;
    double On_Pad_Earth_Gravity_Model_Math_Function1;
    double On_Pad_Earth_Gravity_Model_Math_Function2;
    double On_Pad_Earth_Gravity_Model_Math_Function3;
    double On_Pad_Earth_Gravity_Model_Math_Function5;
    double On_Pad_Earth_Gravity_Model_Math_Function6;
    double On_Pad_Earth_Gravity_Model_Mux[3];
    double On_Pad_Earth_Gravity_Model_Product1[3];
    double On_Pad_Earth_Gravity_Model_Product3[3];
    double On_Pad_Earth_Gravity_Model_Re;
    double On_Pad_Earth_Gravity_Model_Trigonometric_Function;
    double On_Pad_Earth_Gravity_Model_Unary_Minus[3];
    double On_Pad_Earth_Gravity_Model_Unary_Minus1;
    double On_Pad_Ground;
    double On_Pad_Initial_Conditions__3x3_Cross_Product[3];
    double On_Pad_Initial_Conditions_Angle_Conversion;
    double On_Pad_Initial_Conditions_Angle_Conversion1;
    double On_Pad_Initial_Conditions_Angle_Conversion2;
    double On_Pad_Initial_Conditions_Angle_Conversion3;
    double On_Pad_Initial_Conditions_Demux_0;
    double On_Pad_Initial_Conditions_Demux_1;
    double On_Pad_Initial_Conditions_Demux_2;
    double On_Pad_Initial_Conditions_Direction_Cosine_Matrix_to_Quaternion[4][1];
    double On_Pad_Initial_Conditions_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_Angle_Conversion2[3];
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__11;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__12;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__13;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__21;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__22;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__23;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__31;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__32;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix__33;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux[6];
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux2[3][3];
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_SinCos3_sin[3];
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_SinCos3_cos[3];
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900077_0;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900077_1;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900077_2;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900082_0;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900082_1;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900082_2;
    double On_Pad_Initial_Conditions_L_V_Inertial_Orientation_Mux[3];
    double On_Pad_Initial_Conditions_LLA_to_ECF_Add;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Add1;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Bias;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Divide;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Divide1;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Math_Function;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Math_Function1;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Math_Function2;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Math_Function3;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Mux[3];
    double On_Pad_Initial_Conditions_LLA_to_ECF_Product;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Product1;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Product2;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Product3;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Product4;
    double On_Pad_Initial_Conditions_LLA_to_ECF_SinCos_sin;
    double On_Pad_Initial_Conditions_LLA_to_ECF_SinCos_cos;
    double On_Pad_Initial_Conditions_LLA_to_ECF_SinCos1_sin;
    double On_Pad_Initial_Conditions_LLA_to_ECF_SinCos1_cos;
    double On_Pad_Initial_Conditions_LLA_to_ECF_Unary_Minus;
    double On_Pad_Initial_Conditions_LLA_to_ECF_WGS_84_ECC_2;
    double On_Pad_Initial_Conditions_LLA_to_ECF_WGS_84_major;
    double On_Pad_Initial_Conditions_LLA_to_ECF_WGS_84_minor;
    double On_Pad_Initial_Conditions_MES_Transform_Constant2;
    double On_Pad_Initial_Conditions_MES_Transform_Constant8;
    double On_Pad_Initial_Conditions_MES_Transform_Constant9;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_Angle_Conversion2[3];
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__11;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__12;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__13;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__21;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__22;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__23;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__31;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__32;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix__33;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux[6];
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux2[3][3];
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_SinCos3_sin[3];
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_SinCos3_cos[3];
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900087_0;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900087_1;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_0_demux_900087_2;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900092_0;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900092_1;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_E_Frame_to_s_Frame_MES_matrix_Mux_expand_1_demux_900092_2;
    double On_Pad_Initial_Conditions_MES_Transform_MES_Transform_Mux[3];
    double On_Pad_Initial_Conditions_MES_Transform_Sum2;
    double On_Pad_Initial_Conditions_Mux[3];
    double On_Pad_Initial_Conditions_Position_1_Azimuth_deg;
    double On_Pad_Initial_Conditions_Product[3];
    double On_Pad_Initial_Conditions_Product1[3];
    double On_Pad_Initial_Conditions_Sum;
    double On_Pad_Initial_Conditions_Unary_Minus;
    double On_Pad_Initial_Conditions_omega_E_rad_per_sec[3];
    double On_Pad_Initial_Conditions_zero;
    double On_Pad_Product[3];
    double On_Pad_Product1[3];
    double On_Pad_Quaternions_to_Direction_Cosine_Matrix[3][3];
    double On_Pad_omega_E_rad_per_sec[3];
    double S_IB_Stage_Add[3];
    double S_IB_Stage_Add1[3];
    double S_IB_Stage_Add2[3];
    double S_IB_Stage_Add3[3];
    double S_IB_Stage_Add4[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments__3x3_Cross_Product[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Add[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Angle_Conversion;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Angle_Conversion1;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_CA_T;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_CN_alpha;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_CN_beta;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_CP_m;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Constant;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Divide;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Dynamic_Pressure;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Mux[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Mux1[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Post_separation_CG[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Product2[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Referece_Area_m_2;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Switch[3];
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Unary_Minus;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Unary_Minus1;
    double S_IB_Stage_Aerodynamic_Forces_and_Moments_Unary_Minus2;
    double S_IB_Stage_Angle_Conversion10;
    double S_IB_Stage_Angle_Conversion4;
    double S_IB_Stage_Angle_Conversion5;
    double S_IB_Stage_Angle_Conversion9;
    double S_IB_Stage_COESA_Atmosphere_Model_temperature_K;
    double S_IB_Stage_COESA_Atmosphere_Model_speed_of_sound_mps;
    double S_IB_Stage_COESA_Atmosphere_Model_pressure_Pa;
    double S_IB_Stage_COESA_Atmosphere_Model_density_kgpm3;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_Quaternions_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_Quaternions_to_Euler_Angles[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3[4][1];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Constant;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Dot_Product;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_High_Gain_Quaternion_Normalization;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1[8];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux2[4][1];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Sum;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q0dot;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q1dot;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q2dot;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q3dot;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_0_demux_900097_0;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_0_demux_900097_1;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_0_demux_900097_2;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900102_0;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900102_1;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900102_2;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900102_3;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_I_x_w_Product[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_I_x_w1_Product[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Product2[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Selector[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Selector1[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Selector2[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Sum2[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_wx_Iw[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Product[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Sum[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Transpose[3][3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Vbxw[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_transform_to_Inertial_axes_Product[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze[3];
    double S_IB_Stage_ECI_to_LLA_Add1;
    double S_IB_Stage_ECI_to_LLA_Add10;
    double S_IB_Stage_ECI_to_LLA_Add11;
    double S_IB_Stage_ECI_to_LLA_Add12;
    double S_IB_Stage_ECI_to_LLA_Add13;
    double S_IB_Stage_ECI_to_LLA_Add14;
    double S_IB_Stage_ECI_to_LLA_Add15;
    double S_IB_Stage_ECI_to_LLA_Add16;
    double S_IB_Stage_ECI_to_LLA_Add17;
    double S_IB_Stage_ECI_to_LLA_Add18;
    double S_IB_Stage_ECI_to_LLA_Add19;
    double S_IB_Stage_ECI_to_LLA_Add2;
    double S_IB_Stage_ECI_to_LLA_Add3;
    double S_IB_Stage_ECI_to_LLA_Add4;
    double S_IB_Stage_ECI_to_LLA_Add5;
    double S_IB_Stage_ECI_to_LLA_Add6;
    double S_IB_Stage_ECI_to_LLA_Add7;
    double S_IB_Stage_ECI_to_LLA_Add8;
    double S_IB_Stage_ECI_to_LLA_Add9;
    double S_IB_Stage_ECI_to_LLA_Bias;
    double S_IB_Stage_ECI_to_LLA_Bias1;
    double S_IB_Stage_ECI_to_LLA_Bias2;
    bool S_IB_Stage_ECI_to_LLA_Compare_To_Constant;
    bool S_IB_Stage_ECI_to_LLA_Compare_To_Zero;
    double S_IB_Stage_ECI_to_LLA_Demux_0;
    double S_IB_Stage_ECI_to_LLA_Demux_1;
    double S_IB_Stage_ECI_to_LLA_Demux_2;
    double S_IB_Stage_ECI_to_LLA_Divide2;
    double S_IB_Stage_ECI_to_LLA_Divide4;
    double S_IB_Stage_ECI_to_LLA_Divide5;
    double S_IB_Stage_ECI_to_LLA_Divide6;
    double S_IB_Stage_ECI_to_LLA_Divide7;
    double S_IB_Stage_ECI_to_LLA_Divide8;
    double S_IB_Stage_ECI_to_LLA_Gain;
    double S_IB_Stage_ECI_to_LLA_Gain1;
    double S_IB_Stage_ECI_to_LLA_Gain2;
    double S_IB_Stage_ECI_to_LLA_Gain3;
    double S_IB_Stage_ECI_to_LLA_Gain4;
    double S_IB_Stage_ECI_to_LLA_Gain5;
    double S_IB_Stage_ECI_to_LLA_Gain6;
    double S_IB_Stage_ECI_to_LLA_Math_Function1;
    double S_IB_Stage_ECI_to_LLA_Math_Function10;
    double S_IB_Stage_ECI_to_LLA_Math_Function11;
    double S_IB_Stage_ECI_to_LLA_Math_Function12;
    double S_IB_Stage_ECI_to_LLA_Math_Function13;
    double S_IB_Stage_ECI_to_LLA_Math_Function14;
    double S_IB_Stage_ECI_to_LLA_Math_Function15;
    double S_IB_Stage_ECI_to_LLA_Math_Function16;
    double S_IB_Stage_ECI_to_LLA_Math_Function17;
    double S_IB_Stage_ECI_to_LLA_Math_Function18;
    double S_IB_Stage_ECI_to_LLA_Math_Function2;
    double S_IB_Stage_ECI_to_LLA_Math_Function3;
    double S_IB_Stage_ECI_to_LLA_Math_Function4;
    double S_IB_Stage_ECI_to_LLA_Math_Function5;
    double S_IB_Stage_ECI_to_LLA_Math_Function6;
    double S_IB_Stage_ECI_to_LLA_Math_Function7;
    double S_IB_Stage_ECI_to_LLA_Math_Function8;
    double S_IB_Stage_ECI_to_LLA_Math_Function9;
    double S_IB_Stage_ECI_to_LLA_Product1;
    double S_IB_Stage_ECI_to_LLA_Product10;
    double S_IB_Stage_ECI_to_LLA_Product11;
    double S_IB_Stage_ECI_to_LLA_Product12;
    double S_IB_Stage_ECI_to_LLA_Product2;
    double S_IB_Stage_ECI_to_LLA_Product3;
    double S_IB_Stage_ECI_to_LLA_Product4;
    double S_IB_Stage_ECI_to_LLA_Product5;
    double S_IB_Stage_ECI_to_LLA_Product6;
    double S_IB_Stage_ECI_to_LLA_Product7;
    double S_IB_Stage_ECI_to_LLA_Product8;
    double S_IB_Stage_ECI_to_LLA_Product9;
    double S_IB_Stage_ECI_to_LLA_Sign;
    double S_IB_Stage_ECI_to_LLA_SinCos1_sin;
    double S_IB_Stage_ECI_to_LLA_SinCos1_cos;
    double S_IB_Stage_ECI_to_LLA_Switch;
    double S_IB_Stage_ECI_to_LLA_Switch1;
    double S_IB_Stage_ECI_to_LLA_Trigonometric_Function;
    double S_IB_Stage_ECI_to_LLA_Trigonometric_Function1;
    double S_IB_Stage_ECI_to_LLA_Trigonometric_Function2;
    double S_IB_Stage_ECI_to_LLA_Trigonometric_Function3;
    double S_IB_Stage_ECI_to_LLA_Unary_Minus;
    double S_IB_Stage_ECI_to_LLA_Unary_Minus1;
    double S_IB_Stage_ECI_to_LLA_a;
    double S_IB_Stage_ECI_to_LLA_a1;
    double S_IB_Stage_ECI_to_LLA_a2;
    double S_IB_Stage_ECI_to_LLA_a3;
    double S_IB_Stage_ECI_to_LLA_b;
    double S_IB_Stage_Earth_Gravity_Model_Add;
    double S_IB_Stage_Earth_Gravity_Model_Bias;
    double S_IB_Stage_Earth_Gravity_Model_Bias1;
    double S_IB_Stage_Earth_Gravity_Model_Bias2;
    double S_IB_Stage_Earth_Gravity_Model_Demux_0;
    double S_IB_Stage_Earth_Gravity_Model_Demux_1;
    double S_IB_Stage_Earth_Gravity_Model_Demux_2;
    double S_IB_Stage_Earth_Gravity_Model_Divide[3];
    double S_IB_Stage_Earth_Gravity_Model_Divide1;
    double S_IB_Stage_Earth_Gravity_Model_Divide2;
    double S_IB_Stage_Earth_Gravity_Model_GM;
    double S_IB_Stage_Earth_Gravity_Model_Gain;
    double S_IB_Stage_Earth_Gravity_Model_Gain1;
    double S_IB_Stage_Earth_Gravity_Model_J1;
    double S_IB_Stage_Earth_Gravity_Model_J2;
    double S_IB_Stage_Earth_Gravity_Model_Math_Function;
    double S_IB_Stage_Earth_Gravity_Model_Math_Function1;
    double S_IB_Stage_Earth_Gravity_Model_Math_Function2;
    double S_IB_Stage_Earth_Gravity_Model_Math_Function3;
    double S_IB_Stage_Earth_Gravity_Model_Math_Function5;
    double S_IB_Stage_Earth_Gravity_Model_Math_Function6;
    double S_IB_Stage_Earth_Gravity_Model_Mux[3];
    double S_IB_Stage_Earth_Gravity_Model_Product1[3];
    double S_IB_Stage_Earth_Gravity_Model_Product3[3];
    double S_IB_Stage_Earth_Gravity_Model_Re;
    double S_IB_Stage_Earth_Gravity_Model_Trigonometric_Function;
    double S_IB_Stage_Earth_Gravity_Model_Unary_Minus[3];
    double S_IB_Stage_Earth_Gravity_Model_Unary_Minus1;
    double S_IB_Stage_Ground;
    double S_IB_Stage_Ground1;
    double S_IB_Stage_H_1_Engine_Cluster_Add2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Add3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Add4;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_0;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_1;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_2;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_3;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_4;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_5;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_6;
    double S_IB_Stage_H_1_Engine_Cluster_Demux_7;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_A_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Angle_Conversion1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Angle_Conversion2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Angle_Conversion3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_B_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Constant1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Engine_Mount_Point_Rotation[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Euler_Angles_to_Direction_Cosine_Matrix1[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Euler_Angles_to_Direction_Cosine_Matrix2[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Euler_Angles_to_Direction_Cosine_Matrix3[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Gimbal_Limits;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Gimbal_Limits1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Mux1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Neutral_Thrust_Alignment[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Neutral_tilt_Euler_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Product1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Product2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Product3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Product4;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Product5;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_A_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Angle_Conversion1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Angle_Conversion2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Angle_Conversion3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_B_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Constant1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Engine_Mount_Point_Rotation[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Euler_Angles_to_Direction_Cosine_Matrix1[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Euler_Angles_to_Direction_Cosine_Matrix2[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Euler_Angles_to_Direction_Cosine_Matrix3[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Gimbal_Limits;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Gimbal_Limits1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Mux1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Neutral_Thrust_Alignment[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Neutral_tilt_Euler_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Product1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Product2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Product3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Product4;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Product5;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_A_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Angle_Conversion1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Angle_Conversion2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Angle_Conversion3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_B_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Constant1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Engine_Mount_Point_Rotation[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Euler_Angles_to_Direction_Cosine_Matrix1[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Euler_Angles_to_Direction_Cosine_Matrix2[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Euler_Angles_to_Direction_Cosine_Matrix3[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Gimbal_Limits;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Gimbal_Limits1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Mux1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Neutral_Thrust_Alignment[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Neutral_tilt_Euler_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Product1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Product2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Product3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Product4;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Product5;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_A_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Angle_Conversion1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Angle_Conversion2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Angle_Conversion3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_B_Actuator;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Constant1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Engine_Mount_Point_Rotation[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Euler_Angles_to_Direction_Cosine_Matrix1[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Euler_Angles_to_Direction_Cosine_Matrix2[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Euler_Angles_to_Direction_Cosine_Matrix3[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Gimbal_Limits;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Gimbal_Limits1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Mux1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Neutral_Thrust_Alignment[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Neutral_tilt_Euler_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Product1[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Product2[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Product3[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Product4;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Product5;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Neutral_tilt_Euler_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Product1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Product2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Neutral_tilt_Eulker_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Product1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Product2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Neutral_tilt_Eulker_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Product1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Product2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8__3x3_Cross_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Add[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Angle_Conversion[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Constant2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Engine_Performace_Scaling_Factor;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Euler_Angles_to_Direction_Cosine_Matrix[3][3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Installation_location_m[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Mux[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Neutral_tilt_Eulker_angles_deg[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Product[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Product1;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Product2;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Thrust_Decay;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Thrust_vs_Time;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Clock;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Constant;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Data_Store_Read;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Sum;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Switch;
    bool S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization_enable_bool;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_m_dot;
    double S_IB_Stage_H_1_Engine_Cluster_Sum10;
    double S_IB_Stage_H_1_Engine_Cluster_Sum11;
    double S_IB_Stage_H_1_Engine_Cluster_Sum12;
    double S_IB_Stage_H_1_Engine_Cluster_Sum13;
    double S_IB_Stage_H_1_Engine_Cluster_Sum6;
    double S_IB_Stage_H_1_Engine_Cluster_Sum7;
    double S_IB_Stage_H_1_Engine_Cluster_Sum8;
    double S_IB_Stage_H_1_Engine_Cluster_Sum9;
    double S_IB_Stage_H_1_Engine_Cluster_Unary_Minus;
    double S_IB_Stage_Math_Function[3];
    double S_IB_Stage_Math_Function1;
    double S_IB_Stage_Product[3];
    double S_IB_Stage_Product1[3];
    double S_IB_Stage_Relative_Wind__3x3_Cross_Product[3];
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Airspeed;
    bool S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Compare_To_Zero;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Constant;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Demux_0;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Demux_1;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Dot_Product;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Incidence;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Product;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Selector;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Selector1[2];
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Sideslip;
    double S_IB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Switch;
    double S_IB_Stage_Relative_Wind_Product9[3];
    double S_IB_Stage_Relative_Wind_Sum6[3];
    double S_IB_Stage_Relative_Wind_omega_E_rad_per_sec[3];
    double S_IB_Stage_Retrorocket_Motors_Angle_Conversion;
    double S_IB_Stage_Retrorocket_Motors_Gain;
    double S_IB_Stage_Retrorocket_Motors_Mux[3];
    double S_IB_Stage_Retrorocket_Motors_Mux1[3];
    double S_IB_Stage_Retrorocket_Motors_Product;
    double S_IB_Stage_Retrorocket_Motors_Retro_Motor_cant_angle;
    double S_IB_Stage_Retrorocket_Motors_S_IB_Retrorocket_Thrust;
    double S_IB_Stage_Retrorocket_Motors_Switch[3];
    double S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Clock;
    double S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Constant;
    double S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Data_Store_Read;
    double S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Sum;
    double S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Switch;
    bool S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enable_bool;
    double S_IB_Stage_Retrorocket_Motors_Trigonometric_Function;
    double S_IB_Stage_Retrorocket_Motors_zero;
    double S_IB_Stage_Retrorocket_Motors_zero1[3];
    double S_IB_Stage_Sum_of_Elements;
    double S_IB_Stage_Vehicle_Mass_Properties_CG_X;
    double S_IB_Stage_Vehicle_Mass_Properties_CG_Y;
    double S_IB_Stage_Vehicle_Mass_Properties_CG_Z;
    double S_IB_Stage_Vehicle_Mass_Properties_Constant;
    double S_IB_Stage_Vehicle_Mass_Properties_Create_3x3_Matrix[3][3];
    double S_IB_Stage_Vehicle_Mass_Properties_Ixx;
    double S_IB_Stage_Vehicle_Mass_Properties_Iyy;
    double S_IB_Stage_Vehicle_Mass_Properties_Izz;
    double S_IB_Stage_Vehicle_Mass_Properties_Mux3[3];
    double S_IB_Stage_Vehicle_Mass_Properties_Post_separation_CG[3];
    double S_IB_Stage_Vehicle_Mass_Properties_Post_separation_Mass_approximate;
    bool S_IB_Stage_Vehicle_Mass_Properties_Propellant_Sensor_Dry;
    double S_IB_Stage_Vehicle_Mass_Properties_Switch[3];
    double S_IB_Stage_Vehicle_Mass_Properties_Switch1;
    double S_IB_Stage_Vehicle_Mass_Properties_m_liftoff_kg;
    double S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg;
    double S_IVB_Gimbal_Station[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_II__3x3_Cross_Product[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_II_Add[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_II_Constant2[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_II_Installation_location_m[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_II_Switch[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_II_Thruster_Force[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_IV__3x3_Cross_Product[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_IV_Add[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_IV_Constant2[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_IV_Installation_location_m[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_IV_Switch[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_IV_Thruster_Force[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_P__3x3_Cross_Product[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_P_Add[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_P_Constant2[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_P_Installation_location_m[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_P_Switch[3];
    double S_IVB_Stage_APS_I_Module_APS_Thruster_P_Thruster_Force[3];
    double S_IVB_Stage_APS_I_Module_Demux_0;
    double S_IVB_Stage_APS_I_Module_Demux_1;
    double S_IVB_Stage_APS_I_Module_Demux_2;
    double S_IVB_Stage_APS_I_Module_Sum[3];
    double S_IVB_Stage_APS_I_Module_Sum1[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_II__3x3_Cross_Product[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_II_Add[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_II_Constant2[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_II_Installation_location_m[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_II_Switch[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_II_Thruster_Force[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_IV__3x3_Cross_Product[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_IV_Add[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_IV_Constant2[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_IV_Installation_location_m[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_IV_Switch[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_IV_Thruster_Force[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_P__3x3_Cross_Product[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_P_Add[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_P_Constant2[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_P_Installation_location_m[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_P_Switch[3];
    double S_IVB_Stage_APS_III_Module_APS_Thruster_P_Thruster_Force[3];
    double S_IVB_Stage_APS_III_Module_Demux_0;
    double S_IVB_Stage_APS_III_Module_Demux_1;
    double S_IVB_Stage_APS_III_Module_Demux_2;
    double S_IVB_Stage_APS_III_Module_Sum[3];
    double S_IVB_Stage_APS_III_Module_Sum1[3];
    double S_IVB_Stage_Add[3];
    double S_IVB_Stage_Add1[3];
    double S_IVB_Stage_Add2[3];
    double S_IVB_Stage_Add3[3];
    double S_IVB_Stage_Add4[3];
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments__3x3_Cross_Product[3];
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Add[3];
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Angle_Conversion;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Angle_Conversion1;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_CA_T;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_CN_alpha;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_CN_beta;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_CP_m;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Constant;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Divide;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Dynamic_Pressure;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Mux[3];
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Mux1[3];
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Product2[3];
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Referece_Area_m_2;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Unary_Minus;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Unary_Minus1;
    double S_IVB_Stage_Aerodynamic_Forces_and_Moments_Unary_Minus2;
    double S_IVB_Stage_Angle_Conversion10;
    double S_IVB_Stage_Angle_Conversion4;
    double S_IVB_Stage_Angle_Conversion5;
    double S_IVB_Stage_Angle_Conversion9;
    double S_IVB_Stage_COESA_Atmosphere_Model_temperature_K;
    double S_IVB_Stage_COESA_Atmosphere_Model_speed_of_sound_mps;
    double S_IVB_Stage_COESA_Atmosphere_Model_pressure_Pa;
    double S_IVB_Stage_COESA_Atmosphere_Model_density_kgpm3;
    double S_IVB_Stage_Constant;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_Quaternions_to_Direction_Cosine_Matrix[3][3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_Quaternions_to_Euler_Angles[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3[4][1];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Constant;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Dot_Product;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_High_Gain_Quaternion_Normalization;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1[8];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux2[4][1];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Sum;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q0dot;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q1dot;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q2dot;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_q3dot;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_0_demux_900108_0;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_0_demux_900108_1;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_0_demux_900108_2;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900113_0;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900113_1;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900113_2;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_qdot_Mux1_expand_1_demux_900113_3;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_I_x_w_Product[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_I_x_w1_Product[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Product2[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Selector[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Selector1[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Selector2[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_Sum2[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_omega_dot_wx_Iw[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Product[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Sum[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Transpose[3][3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Vbxw[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_transform_to_Inertial_axes_Product[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb[3];
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze[3];
    double S_IVB_Stage_ECI_to_LLA_Add1;
    double S_IVB_Stage_ECI_to_LLA_Add10;
    double S_IVB_Stage_ECI_to_LLA_Add11;
    double S_IVB_Stage_ECI_to_LLA_Add12;
    double S_IVB_Stage_ECI_to_LLA_Add13;
    double S_IVB_Stage_ECI_to_LLA_Add14;
    double S_IVB_Stage_ECI_to_LLA_Add15;
    double S_IVB_Stage_ECI_to_LLA_Add16;
    double S_IVB_Stage_ECI_to_LLA_Add17;
    double S_IVB_Stage_ECI_to_LLA_Add18;
    double S_IVB_Stage_ECI_to_LLA_Add19;
    double S_IVB_Stage_ECI_to_LLA_Add2;
    double S_IVB_Stage_ECI_to_LLA_Add3;
    double S_IVB_Stage_ECI_to_LLA_Add4;
    double S_IVB_Stage_ECI_to_LLA_Add5;
    double S_IVB_Stage_ECI_to_LLA_Add6;
    double S_IVB_Stage_ECI_to_LLA_Add7;
    double S_IVB_Stage_ECI_to_LLA_Add8;
    double S_IVB_Stage_ECI_to_LLA_Add9;
    double S_IVB_Stage_ECI_to_LLA_Bias;
    double S_IVB_Stage_ECI_to_LLA_Bias1;
    double S_IVB_Stage_ECI_to_LLA_Bias2;
    bool S_IVB_Stage_ECI_to_LLA_Compare_To_Constant;
    bool S_IVB_Stage_ECI_to_LLA_Compare_To_Zero;
    double S_IVB_Stage_ECI_to_LLA_Demux_0;
    double S_IVB_Stage_ECI_to_LLA_Demux_1;
    double S_IVB_Stage_ECI_to_LLA_Demux_2;
    double S_IVB_Stage_ECI_to_LLA_Divide2;
    double S_IVB_Stage_ECI_to_LLA_Divide4;
    double S_IVB_Stage_ECI_to_LLA_Divide5;
    double S_IVB_Stage_ECI_to_LLA_Divide6;
    double S_IVB_Stage_ECI_to_LLA_Divide7;
    double S_IVB_Stage_ECI_to_LLA_Divide8;
    double S_IVB_Stage_ECI_to_LLA_Gain;
    double S_IVB_Stage_ECI_to_LLA_Gain1;
    double S_IVB_Stage_ECI_to_LLA_Gain2;
    double S_IVB_Stage_ECI_to_LLA_Gain3;
    double S_IVB_Stage_ECI_to_LLA_Gain4;
    double S_IVB_Stage_ECI_to_LLA_Gain5;
    double S_IVB_Stage_ECI_to_LLA_Gain6;
    double S_IVB_Stage_ECI_to_LLA_Math_Function1;
    double S_IVB_Stage_ECI_to_LLA_Math_Function10;
    double S_IVB_Stage_ECI_to_LLA_Math_Function11;
    double S_IVB_Stage_ECI_to_LLA_Math_Function12;
    double S_IVB_Stage_ECI_to_LLA_Math_Function13;
    double S_IVB_Stage_ECI_to_LLA_Math_Function14;
    double S_IVB_Stage_ECI_to_LLA_Math_Function15;
    double S_IVB_Stage_ECI_to_LLA_Math_Function16;
    double S_IVB_Stage_ECI_to_LLA_Math_Function17;
    double S_IVB_Stage_ECI_to_LLA_Math_Function18;
    double S_IVB_Stage_ECI_to_LLA_Math_Function2;
    double S_IVB_Stage_ECI_to_LLA_Math_Function3;
    double S_IVB_Stage_ECI_to_LLA_Math_Function4;
    double S_IVB_Stage_ECI_to_LLA_Math_Function5;
    double S_IVB_Stage_ECI_to_LLA_Math_Function6;
    double S_IVB_Stage_ECI_to_LLA_Math_Function7;
    double S_IVB_Stage_ECI_to_LLA_Math_Function8;
    double S_IVB_Stage_ECI_to_LLA_Math_Function9;
    double S_IVB_Stage_ECI_to_LLA_Product1;
    double S_IVB_Stage_ECI_to_LLA_Product10;
    double S_IVB_Stage_ECI_to_LLA_Product11;
    double S_IVB_Stage_ECI_to_LLA_Product12;
    double S_IVB_Stage_ECI_to_LLA_Product2;
    double S_IVB_Stage_ECI_to_LLA_Product3;
    double S_IVB_Stage_ECI_to_LLA_Product4;
    double S_IVB_Stage_ECI_to_LLA_Product5;
    double S_IVB_Stage_ECI_to_LLA_Product6;
    double S_IVB_Stage_ECI_to_LLA_Product7;
    double S_IVB_Stage_ECI_to_LLA_Product8;
    double S_IVB_Stage_ECI_to_LLA_Product9;
    double S_IVB_Stage_ECI_to_LLA_Sign;
    double S_IVB_Stage_ECI_to_LLA_SinCos1_sin;
    double S_IVB_Stage_ECI_to_LLA_SinCos1_cos;
    double S_IVB_Stage_ECI_to_LLA_Switch;
    double S_IVB_Stage_ECI_to_LLA_Switch1;
    double S_IVB_Stage_ECI_to_LLA_Trigonometric_Function;
    double S_IVB_Stage_ECI_to_LLA_Trigonometric_Function1;
    double S_IVB_Stage_ECI_to_LLA_Trigonometric_Function2;
    double S_IVB_Stage_ECI_to_LLA_Trigonometric_Function3;
    double S_IVB_Stage_ECI_to_LLA_Unary_Minus;
    double S_IVB_Stage_ECI_to_LLA_Unary_Minus1;
    double S_IVB_Stage_ECI_to_LLA_a;
    double S_IVB_Stage_ECI_to_LLA_a1;
    double S_IVB_Stage_ECI_to_LLA_a2;
    double S_IVB_Stage_ECI_to_LLA_a3;
    double S_IVB_Stage_ECI_to_LLA_b;
    double S_IVB_Stage_Earth_Gravity_Model_Add;
    double S_IVB_Stage_Earth_Gravity_Model_Bias;
    double S_IVB_Stage_Earth_Gravity_Model_Bias1;
    double S_IVB_Stage_Earth_Gravity_Model_Bias2;
    double S_IVB_Stage_Earth_Gravity_Model_Demux_0;
    double S_IVB_Stage_Earth_Gravity_Model_Demux_1;
    double S_IVB_Stage_Earth_Gravity_Model_Demux_2;
    double S_IVB_Stage_Earth_Gravity_Model_Divide[3];
    double S_IVB_Stage_Earth_Gravity_Model_Divide1;
    double S_IVB_Stage_Earth_Gravity_Model_Divide2;
    double S_IVB_Stage_Earth_Gravity_Model_GM;
    double S_IVB_Stage_Earth_Gravity_Model_Gain;
    double S_IVB_Stage_Earth_Gravity_Model_Gain1;
    double S_IVB_Stage_Earth_Gravity_Model_J1;
    double S_IVB_Stage_Earth_Gravity_Model_J2;
    double S_IVB_Stage_Earth_Gravity_Model_Math_Function;
    double S_IVB_Stage_Earth_Gravity_Model_Math_Function1;
    double S_IVB_Stage_Earth_Gravity_Model_Math_Function2;
    double S_IVB_Stage_Earth_Gravity_Model_Math_Function3;
    double S_IVB_Stage_Earth_Gravity_Model_Math_Function5;
    double S_IVB_Stage_Earth_Gravity_Model_Math_Function6;
    double S_IVB_Stage_Earth_Gravity_Model_Mux[3];
    double S_IVB_Stage_Earth_Gravity_Model_Product1[3];
    double S_IVB_Stage_Earth_Gravity_Model_Product3[3];
    double S_IVB_Stage_Earth_Gravity_Model_Re;
    double S_IVB_Stage_Earth_Gravity_Model_Trigonometric_Function;
    double S_IVB_Stage_Earth_Gravity_Model_Unary_Minus[3];
    double S_IVB_Stage_Earth_Gravity_Model_Unary_Minus1;
    double S_IVB_Stage_Ground;
    double S_IVB_Stage_Ground1;
    double S_IVB_Stage_Ground2;
    double S_IVB_Stage_J_2_Engine_Thrust_Model__3x3_Cross_Product[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Add[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Angle_Conversion3[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Bias;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Constant1;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Constant2;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Constant3;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Divide;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Euler_Angles_to_Direction_Cosine_Matrix3[3][3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Gimbal_Limits;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Gimbal_Limits1;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Gimbal_Limits2;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Gimbal_Limits3;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Installation_location_in[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Mux[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Mux1[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Pitch_Actuator;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Product;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Product3[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Thrust_Decay;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Thrust_vs_Time;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Clock;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Constant;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Data_Store_Read;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Sum;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Switch;
    bool S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization_enable_bool;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Clock;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Constant;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Data_Store_Read;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Sum;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Switch;
    bool S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization_enable_bool;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Unary_Minus2[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Yaw_Actuator;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_in_to_m[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_m_dot;
    double S_IVB_Stage_J_2_Engine_Unary_Minus;
    double S_IVB_Stage_Math_Function[3];
    double S_IVB_Stage_Math_Function1;
    double S_IVB_Stage_Product[3];
    double S_IVB_Stage_Product1[3];
    double S_IVB_Stage_Relative_Wind__3x3_Cross_Product[3];
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Airspeed;
    bool S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Compare_To_Zero;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Constant;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Demux_0;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Demux_1;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Dot_Product;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Incidence;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Product;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Selector;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Selector1[2];
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Sideslip;
    double S_IVB_Stage_Relative_Wind_Incidence_Sideslip_Airspeed_Switch;
    double S_IVB_Stage_Relative_Wind_Product9[3];
    double S_IVB_Stage_Relative_Wind_Sum6[3];
    double S_IVB_Stage_Relative_Wind_omega_E_rad_per_sec[3];
    double S_IVB_Stage_Sum_of_Elements;
    double S_IVB_Stage_Ullage_Rocket_Motors_Angle_Conversion;
    double S_IVB_Stage_Ullage_Rocket_Motors_Gain;
    double S_IVB_Stage_Ullage_Rocket_Motors_Mux[3];
    double S_IVB_Stage_Ullage_Rocket_Motors_Mux1[3];
    double S_IVB_Stage_Ullage_Rocket_Motors_Product;
    double S_IVB_Stage_Ullage_Rocket_Motors_S_IVB_Ullage_Motor_Thrust;
    double S_IVB_Stage_Ullage_Rocket_Motors_Switch;
    double S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Clock;
    double S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Constant;
    double S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Data_Store_Read;
    double S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Sum;
    double S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Switch;
    bool S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enable_bool;
    double S_IVB_Stage_Ullage_Rocket_Motors_Trigonometric_Function;
    double S_IVB_Stage_Ullage_Rocket_Motors_Ullage_Motor_cant_angle;
    double S_IVB_Stage_Ullage_Rocket_Motors_zero;
    double S_IVB_Stage_Vehicle_Mass_Properties_CG_X;
    double S_IVB_Stage_Vehicle_Mass_Properties_CG_Y;
    double S_IVB_Stage_Vehicle_Mass_Properties_CG_Z;
    double S_IVB_Stage_Vehicle_Mass_Properties_Constant;
    double S_IVB_Stage_Vehicle_Mass_Properties_Constant1;
    double S_IVB_Stage_Vehicle_Mass_Properties_Constant2;
    double S_IVB_Stage_Vehicle_Mass_Properties_Create_3x3_Matrix[3][3];
    double S_IVB_Stage_Vehicle_Mass_Properties_Ground;
    double S_IVB_Stage_Vehicle_Mass_Properties_Ixx;
    double S_IVB_Stage_Vehicle_Mass_Properties_Iyy;
    double S_IVB_Stage_Vehicle_Mass_Properties_Izz;
    double S_IVB_Stage_Vehicle_Mass_Properties_Mass_Conversion;
    double S_IVB_Stage_Vehicle_Mass_Properties_Mass_Conversion1;
    double S_IVB_Stage_Vehicle_Mass_Properties_Mux3[3];
    bool S_IVB_Stage_Vehicle_Mass_Properties_Propellant_Depleted;
    double S_IVB_Stage_Vehicle_Mass_Properties_Subtract;
    double S_IVB_Stage_Vehicle_Mass_Properties_Switch;
    double S_IVB_Stage_Vehicle_Mass_Properties_Switch1;
    double S_IVB_Stage_Vehicle_Mass_Properties_m_separation_kg;
    double S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg;
    double Saturn_Instrument_Unit_IU_Accelerometers__10[3];
    double Saturn_Instrument_Unit_IU_Accelerometers_Constant;
    double Saturn_Instrument_Unit_IU_Accelerometers_Demux2_0;
    double Saturn_Instrument_Unit_IU_Accelerometers_Demux2_1;
    double Saturn_Instrument_Unit_IU_Accelerometers_Mux1[3];
    double Saturn_Instrument_Unit_IU_Accelerometers_Selector[2];
    double Saturn_Instrument_Unit_IU_Accelerometers_Unary_Minus;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Angle_Conversion[3];
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Demux_0;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Demux_1;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Demux_2;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Mux[3];
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn1;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn2;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_sensor_saturation[3];
    double Saturn_Instrument_Unit_IU_Constant3;
    double Saturn_Instrument_Unit_IU_Constant4;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Constant;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Mux[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Mux1[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Mux5[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Mux6[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Mux7[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Mux8[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Product[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Product1[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Product11[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Product12[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Product2[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Product9[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Roll_blending_factor[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Demux1_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Demux1_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Demux1_2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Mux3[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn3;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn4;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn5;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Demux_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Demux_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Demux_2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Ground[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Mux2[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Demux2_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Demux2_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Demux2_2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Mux4[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn6;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn7;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Demux1_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Demux1_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Demux1_2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Demux4_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Demux4_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Demux4_2;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Logical_Operator;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Mux1[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Mux10[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Switch[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn10;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn11;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Demux1_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Demux1_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Demux1_2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Demux3_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Demux3_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Demux3_2;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Logical_Operator;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Mux1[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Mux9[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Switch[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn8;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn9;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Clock;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Constant;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Data_Store_Read;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Sum;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Switch;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization_enable_bool;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Constant;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Demux_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Demux_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Demux_2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Demux1_0;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Demux1_1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Demux1_2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon_2;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Compare_To_Zero;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Compare_To_Zero1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Sum;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Sum5;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Transfer_Fcn;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Unary_Minus;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon_2;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Compare_To_Zero;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Compare_To_Zero1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Sum;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Sum5;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Transfer_Fcn;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Unary_Minus;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon_2;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Compare_To_Zero;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Compare_To_Zero1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Sum;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Sum5;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Transfer_Fcn;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Unary_Minus;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Gain[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Saturation;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Saturation1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Saturation2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Sum1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Sum14;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Sum2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Sum3;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Sum4;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Switch;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Switch1;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Switch2;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Switch3;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Sum1[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Sum14[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_a0_deg_deg;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_a0_deg_deg_s_IV;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_a0_roll;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_a1_deg_deg_per_sec;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_a1_deg_deg_per_sec_s_IV;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_a1_roll;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_g2_deg_m_per_sec_2;
    double Saturn_Instrument_Unit_IU_Ground;
    double Saturn_Instrument_Unit_IU_Mux2[8];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform__45_0_deg;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Accelerometer_Quantization[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Angle_Conversion8[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Body_to_ECI[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Demux5_0;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Demux5_1;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Demux5_2;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_ECI_to_Platform[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Fine_Resolver_Quantization[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Integrating_Accelerometers[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Math_Function[3][3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Math_Function1[3][3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Math_Function2[3][3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Memory[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Mux[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_sin_theta_x_cos_theta_z;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_sin_theta_y_cos_theta_z;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_Mux2[3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_Trigonometric_Function1;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_Trigonometric_Function3;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_Trigonometric_Function4;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_cos_theta_x_cos_theta_z;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_cos_theta_y_cos_theta_z;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Platform_Gimbal_Angles_sin_theta_z;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Product9[3][3];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Quaternions_to_Direction_Cosine_Matrix1[3][3];
    bool Saturn_Instrument_Unit_IU_Switchpoint_1;
    bool Saturn_Instrument_Unit_IU_Switchpoint_2;
    bool Saturn_Instrument_Unit_IU_Switchpoint_3;
    double Switch1[3];
    double Switch2[4][1];
    double Switch3[3];
    double Switch4[3];
    double Switch5[4][1];
    double Switch6[3];
    double Switch7[3];
    bool T_L_prime;
    double Time_sec3;
    bool Ullage_Jettison;
    bool S_IVB_Stage_enable_bool;
    double Mux_expand_3_demux_900000_0;
    double Mux_expand_3_demux_900000_1;
    double Mux_expand_3_demux_900000_2;
    double Mux_expand_4_demux_900005_0;
    double Mux_expand_4_demux_900005_1;
    double Mux_expand_4_demux_900005_2;
    double Mux_expand_8_demux_900010_0;
    double Mux_expand_8_demux_900010_1;
    double Mux_expand_8_demux_900010_2;
    double Mux_expand_8_demux_900010_3;
    double Mux_expand_9_demux_900016_0;
    double Mux_expand_9_demux_900016_1;
    double Mux_expand_9_demux_900016_2;
    double Mux_expand_10_demux_900021_0;
    double Mux_expand_10_demux_900021_1;
    double Mux_expand_10_demux_900021_2;
    double Mux1_expand_3_demux_900026_0;
    double Mux1_expand_3_demux_900026_1;
    double Mux1_expand_3_demux_900026_2;
    double Mux1_expand_4_demux_900031_0;
    double Mux1_expand_4_demux_900031_1;
    double Mux1_expand_4_demux_900031_2;
    double Mux1_expand_8_demux_900036_0;
    double Mux1_expand_8_demux_900036_1;
    double Mux1_expand_8_demux_900036_2;
    double Mux1_expand_8_demux_900036_3;
    double Mux1_expand_9_demux_900042_0;
    double Mux1_expand_9_demux_900042_1;
    double Mux1_expand_9_demux_900042_2;
    double Mux_expand_9_demux_900129_0;
    double Mux_expand_9_demux_900129_1;
    double Mux_expand_9_demux_900129_2;
    double Mux1_expand_9_demux_900134_0;
    double Mux1_expand_9_demux_900134_1;
    double Mux1_expand_9_demux_900134_2;
    double Out22_Xe_demux_0;
    double Out22_Xe_demux_1;
    double Out22_Xe_demux_2;
    double Out22_Ve_demux_0;
    double Out22_Ve_demux_1;
    double Out22_Ve_demux_2;
    double Out22_pack[9];
} saturn_ib_stack_signals_t;

/* Model-scoped named data stores (shared across hierarchy) */
typedef struct {
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_A */
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_A */
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_A */
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_A */
    double S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_A */
    double S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_A */
    double S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_A */
    double S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_A; /* data store: S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_A */
    double S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_A; /* data store: S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_A */
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_A; /* data store: S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_A */
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_A; /* data store: S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_A */
    double S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_A; /* data store: S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_A */
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_A; /* data store: Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_A */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_X_deg; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_X_deg */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Y_deg; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Y_deg */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Z_deg; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Z_deg */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_1_i_sec; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_1_i_sec */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_N_sec; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_N_sec */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_3_i_sec; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_3_i_sec */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_P_C_sec; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_P_C_sec */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_DeltaT_b_sec; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_DeltaT_b_sec */
    long Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nIGMMode; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nIGMMode */
    long Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nHSLActive; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nHSLActive */
    long Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nTerminalSteeringMode; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nTerminalSteeringMode */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_alpha_f; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_alpha_f */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_1_sec; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_1_sec */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_sec; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_sec */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCY_rad; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCY_rad */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCZ_rad; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCZ_rad */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZdot_Sg_mps[3]; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZdot_Sg_mps */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZ_S_m[3]; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZ_S_m */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_T1_Timer_A; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_T1_Timer_A */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_T2_Timer_A; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_T2_Timer_A */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_T3_Timer_A; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_T3_Timer_A */
    double Saturn_Instrument_Unit_IU_LVDA_LVDC_T4_Timer_A; /* data store: Saturn_Instrument_Unit_IU_LVDA_LVDC_T4_Timer_A */
} saturn_ib_stack_data_stores_t;

/* State variables for dynamic blocks */
typedef struct {
    double CM_ISS_velocity_in_SM_frame_states[3];
    int CM_ISS_velocity_in_SM_frame_ic_needs_loading;
    double Memory1_state[3];
    double Memory2_state[3];
    double Memory3_state;
    double Memory4_state[3];
    double Memory5_state;
    double Memory6_state[3];
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_states[4][1];
    int S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_ic_needs_loading;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_states[3];
    int S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_ic_needs_loading;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_states[3];
    int S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_ic_needs_loading;
    double S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_states[3];
    int S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_ic_needs_loading;
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_A_Actuator_states[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_1_B_Actuator_states[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_A_Actuator_states[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_2_B_Actuator_states[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_A_Actuator_states[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_3_B_Actuator_states[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_A_Actuator_states[3];
    double S_IB_Stage_H_1_Engine_Cluster_Engine_4_B_Actuator_states[3];
    double S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_states[1];
    int S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_ic_needs_loading;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_states[4][1];
    int S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_ic_needs_loading;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_states[3];
    int S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_ic_needs_loading;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_states[3];
    int S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_ic_needs_loading;
    double S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_states[3];
    int S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_ic_needs_loading;
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Pitch_Actuator_states[3];
    double S_IVB_Stage_J_2_Engine_Thrust_Model_Yaw_Actuator_states[3];
    double S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_states[1];
    int S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_ic_needs_loading;
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn_states[2];
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn1_states[2];
    double Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn2_states[2];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn3_states[1];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn4_states[1];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn5_states[1];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn_states[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn1_states[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn2_states[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn6_states[2];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn7_states[2];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn1_states[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn10_states[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn11_states[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn2_states[3];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn1_states[1];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn2_states[1];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn8_states[1];
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn9_states[1];
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon_is_on;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon_2_is_on;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Transfer_Fcn_states[1];
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon_is_on;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon_2_is_on;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Transfer_Fcn_states[1];
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon_is_on;
    bool Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon_2_is_on;
    double Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Transfer_Fcn_states[1];
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Integrating_Accelerometers_states[3];
    int Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Integrating_Accelerometers_ic_needs_loading;
    double Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Memory_state[3];
} saturn_ib_stack_states_t;

/* Enable states for all subsystems */
typedef struct {
    int S_IB_Stage_enabled; /* Enable state for S_IB_Stage */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization */
    int S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enabled; /* Enable state for S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization */
    int S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization */
    int S_IVB_Stage_enabled; /* Enable state for S_IVB_Stage */
    int S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization_enabled; /* Enable state for S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization */
    int S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization */
    int S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization_enabled; /* Enable state for S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization */
    int S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization */
    int S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enabled; /* Enable state for S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization */
    int S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization */
    int Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization_enabled; /* Enable state for Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization */
    int Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization_trig_prev; /* Previous trigger level for rising-edge Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization */
} enable_states_t;


/* Main model structure containing all signals and states */
typedef struct {
    saturn_ib_stack_inputs_t inputs;
    saturn_ib_stack_outputs_t outputs;
    saturn_ib_stack_signals_t signals;
    saturn_ib_stack_states_t states;
    saturn_ib_stack_data_stores_t data_stores;
    enable_states_t enable_states;
    LVDA_LVDC_t LVDA_LVDC; /* Segregated subsystem */
    double time;
    double dt; /* Time step */
    unsigned long long sample_tick; /* Fundamental steps since t=0 (multi-rate hits) */
    int use_rk4; /* Integration method: 1=RK4, 0=Euler */
} saturn_ib_stack_t;

/*
 * Function prototypes
 */
/* Initialize model with given time step */
void saturn_ib_stack_init(saturn_ib_stack_t* model, double dt);
/* Evaluate algebraic relationships (pure function, no state changes) */
void saturn_ib_stack_evaluate_algebraic(saturn_ib_stack_t* model);
/* Re-apply integrator ICs from x(0) after algebraic IC signals are live */
void saturn_ib_stack_reseed_integrator_ics(saturn_ib_stack_t* model);
/* After apply ExtU: algebra → enables → integrator ICs → algebra */
void saturn_ib_stack_seed_from_inputs(saturn_ib_stack_t* model);
/* Execute one simulation step */
void saturn_ib_stack_step(saturn_ib_stack_t* model);
/* Calculate state derivatives for RK4 integration */
void saturn_ib_stack_derivatives(double t, const saturn_ib_stack_t* model, const saturn_ib_stack_states_t* current_states, saturn_ib_stack_states_t* state_derivatives);
/* Update enable states based on enable inputs */
void saturn_ib_stack_evaluate_enable_states(saturn_ib_stack_t* model);

#ifdef __cplusplus
}
#endif

#endif /* SATURN_IB_STACK_H */
