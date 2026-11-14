#include "plant_serialize.h"

#include "saturn_ib_stack.h"
#include "SaturnIBStackAdapter.h"

#include <cstdio>
#include <cstring>

// Private handle definition, duplicated from SaturnIBPlantObliq.c.
// This coupling is why the serializer lives in src/sim/ rather than
// support/: it needs to see the saturn_ib_stack_t and reach through
// the opaque handle to get at it.
struct SaturnIBPlantObliq_tag {
    double            dt_sec;
    double            time_sec;
    const char       *error;
    saturn_ib_stack_t model;
    int               model_inited;
};

namespace dskymatic {

// -----------------------------------------------------------------------
// Small helpers.  Prefer these to raw w.write_f64() calls so array /
// flat-field lines stay one-liners below.
// -----------------------------------------------------------------------

static void w1(BinaryWriter &w, double  v) { w.write_f64 (v); }
static void w1(BinaryWriter &w, int     v) { w.write_i32 (v); }
static void w1(BinaryWriter &w, long    v) { w.write_i64 (v); }
static void w1(BinaryWriter &w, bool    v) { w.write_bool(v); }

static double r_f64  (BinaryReader &r) { return r.read_f64 (); }
static int    r_i32  (BinaryReader &r) { return r.read_i32 (); }
static long   r_i64  (BinaryReader &r) { return (long)r.read_i64(); }
static bool   r_bool (BinaryReader &r) { return r.read_bool(); }

template <size_t N>
static void w_d_arr(BinaryWriter &w, const double (&a)[N]) {
    for (size_t i = 0; i < N; ++i) w.write_f64(a[i]);
}
template <size_t N>
static void r_d_arr(BinaryReader &r, double (&a)[N]) {
    for (size_t i = 0; i < N; ++i) a[i] = r.read_f64();
}
template <size_t N, size_t M>
static void w_d_mat(BinaryWriter &w, const double (&a)[N][M]) {
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < M; ++j)
            w.write_f64(a[i][j]);
}
template <size_t N, size_t M>
static void r_d_mat(BinaryReader &r, double (&a)[N][M]) {
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < M; ++j)
            a[i][j] = r.read_f64();
}

// -----------------------------------------------------------------------
// saturn_ib_stack_states_t
// -----------------------------------------------------------------------

static void write_states(BinaryWriter &w, const saturn_ib_stack_states_t &s) {
    w_d_arr(w, s.CM_ISS_velocity_in_SM_frame_states);
    w1(w, s.CM_ISS_velocity_in_SM_frame_ic_needs_loading);
    w_d_arr(w, s.Memory1_state);
    w_d_arr(w, s.Memory2_state);
    w1(w, s.Memory3_state);
    w_d_arr(w, s.Memory4_state);
    w1(w, s.Memory5_state);
    w_d_arr(w, s.Memory6_state);
    w_d_mat(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_states);
    w1(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_ic_needs_loading);
    w_d_arr(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_states);
    w1(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_ic_needs_loading);
    w_d_arr(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_states);
    w1(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_ic_needs_loading);
    w_d_arr(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_states);
    w1(w, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_ic_needs_loading);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_A_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_B_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_A_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_B_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_A_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_B_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_A_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_B_Actuator_states);
    w_d_arr(w, s.S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_states);
    w1(w, s.S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_ic_needs_loading);
    w_d_mat(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_states);
    w1(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_ic_needs_loading);
    w_d_arr(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_states);
    w1(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_ic_needs_loading);
    w_d_arr(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_states);
    w1(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_ic_needs_loading);
    w_d_arr(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_states);
    w1(w, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_ic_needs_loading);
    w_d_arr(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Pitch_Actuator_states);
    w_d_arr(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Yaw_Actuator_states);
    w_d_arr(w, s.S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_states);
    w1(w, s.S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_ic_needs_loading);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn1_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn2_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn3_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn4_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn5_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn1_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn2_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn6_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn7_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn1_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn10_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn11_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn2_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn1_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn2_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn8_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn9_states);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon_is_on);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon_2_is_on);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Transfer_Fcn_states);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon_is_on);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon_2_is_on);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Transfer_Fcn_states);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon_is_on);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon_2_is_on);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Transfer_Fcn_states);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Integrating_Accelerometers_states);
    w1(w, s.Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Integrating_Accelerometers_ic_needs_loading);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Memory_state);
}

static void read_states(BinaryReader &r, saturn_ib_stack_states_t &s) {
    r_d_arr(r, s.CM_ISS_velocity_in_SM_frame_states);
    s.CM_ISS_velocity_in_SM_frame_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.Memory1_state);
    r_d_arr(r, s.Memory2_state);
    s.Memory3_state = r_f64(r);
    r_d_arr(r, s.Memory4_state);
    s.Memory5_state = r_f64(r);
    r_d_arr(r, s.Memory6_state);
    r_d_mat(r, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_states);
    s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_states);
    s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_states);
    s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_states);
    s.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_A_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_B_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_A_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_B_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_A_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_B_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_A_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_B_Actuator_states);
    r_d_arr(r, s.S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_states);
    s.S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_ic_needs_loading = r_i32(r);
    r_d_mat(r, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_states);
    s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_states);
    s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_states);
    s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_states);
    s.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Pitch_Actuator_states);
    r_d_arr(r, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Yaw_Actuator_states);
    r_d_arr(r, s.S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_states);
    s.S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn1_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Body_Rate_Gyros_Transfer_Fcn2_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn3_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn4_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Error_Filters_Transfer_Fcn5_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn1_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Attitude_Rate_Error_Transfer_Fcn2_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn6_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IB_Y_Z_Acceleration_Filters_Transfer_Fcn7_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn1_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn10_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn11_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Rate_Filters_Transfer_Fcn2_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn1_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn2_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn8_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Pitch_Yaw_Attitude_Error_Filters_Transfer_Fcn9_states);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon_is_on   = r_bool(r);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Uoff_Uon_2_is_on = r_bool(r);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Transfer_Fcn_states);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon_is_on   = r_bool(r);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Uoff_Uon_2_is_on = r_bool(r);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator_Pitch_Transfer_Fcn_states);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon_is_on   = r_bool(r);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Uoff_Uon_2_is_on = r_bool(r);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_Spatial_Attitude_Control_System_Derived_Rate_Modulator1_Transfer_Fcn_states);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Integrating_Accelerometers_states);
    s.Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Integrating_Accelerometers_ic_needs_loading = r_i32(r);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_ST_124M_Stabilized_Platform_Memory_state);
}

// -----------------------------------------------------------------------
// saturn_ib_stack_data_stores_t
// -----------------------------------------------------------------------

static void write_data_stores(BinaryWriter &w, const saturn_ib_stack_data_stores_t &s) {
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_A);
    w1(w, s.S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_A);
    w1(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_A);
    w1(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_A);
    w1(w, s.S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_A);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_A);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_X_deg);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Y_deg);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Z_deg);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_1_i_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_N_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_3_i_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_P_C_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_DeltaT_b_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nIGMMode);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nHSLActive);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nTerminalSteeringMode);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_alpha_f);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_1_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCY_rad);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCZ_rad);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZdot_Sg_mps);
    w_d_arr(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZ_S_m);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T1_Timer_A);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T2_Timer_A);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T3_Timer_A);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T4_Timer_A);
}

static void read_data_stores(BinaryReader &r, saturn_ib_stack_data_stores_t &s) {
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_A = r_f64(r);
    s.S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_A  = r_f64(r);
    s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_A = r_f64(r);
    s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_A     = r_f64(r);
    s.S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_A    = r_f64(r);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_A = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_X_deg = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Y_deg = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Z_deg = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_1_i_sec    = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_N_sec  = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_3_i_sec    = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_P_C_sec      = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_DeltaT_b_sec = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nIGMMode              = r_i64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nHSLActive            = r_i64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nTerminalSteeringMode = r_i64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_alpha_f    = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_1_sec  = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_sec  = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCY_rad   = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCZ_rad   = r_f64(r);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZdot_Sg_mps);
    r_d_arr(r, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_XYZ_S_m);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T1_Timer_A = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T2_Timer_A = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T3_Timer_A = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T4_Timer_A = r_f64(r);
}

// -----------------------------------------------------------------------
// saturn_ib_stack_enable_states_t
// -----------------------------------------------------------------------

static void write_enable_states(BinaryWriter &w, const saturn_ib_stack_enable_states_t &s) {
    w1(w, s.S_IB_Stage_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization_trig_prev);
    w1(w, s.S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enabled);
    w1(w, s.S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization_trig_prev);
    w1(w, s.S_IVB_Stage_enabled);
    w1(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization_enabled);
    w1(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization_trig_prev);
    w1(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization_enabled);
    w1(w, s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization_trig_prev);
    w1(w, s.S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enabled);
    w1(w, s.S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization_trig_prev);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization_enabled);
    w1(w, s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization_trig_prev);
}

static void read_enable_states(BinaryReader &r, saturn_ib_stack_enable_states_t &s) {
    s.S_IB_Stage_enabled = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_1_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_2_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_3_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_4_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_5_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_6_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_7_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.S_IB_Stage_H_1_Engine_Cluster_Engine_8_Triggered_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enabled    = r_i32(r);
    s.S_IB_Stage_Retrorocket_Motors_Time_Since_Engine_Start_Timer_Initialization_trig_prev  = r_i32(r);
    s.S_IVB_Stage_enabled = r_i32(r);
    s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization_enabled   = r_i32(r);
    s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Engine_Start_Timer_Initialization_trig_prev = r_i32(r);
    s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization_enabled       = r_i32(r);
    s.S_IVB_Stage_J_2_Engine_Thrust_Model_Time_Since_Shutdown_Timer_Initialization_trig_prev     = r_i32(r);
    s.S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization_enabled      = r_i32(r);
    s.S_IVB_Stage_Ullage_Rocket_Motors_Time_Since_Engine_Start_Timer_Initialization_trig_prev    = r_i32(r);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.Saturn_Instrument_Unit_IU_Flight_Control_Computer_S_IVB_Stage_Timer_Timer_Initialization_trig_prev = r_i32(r);
}

// -----------------------------------------------------------------------
// LVDA_LVDC_t sub-structs
// -----------------------------------------------------------------------

static void write_lvdc_states(BinaryWriter &w, const LVDA_LVDC_states_t &s) {
    w_d_arr(w, s.LVDC_Chi_command_angles_Time_Tilt_Rate_Limiter_Dynamic_Delay_Input2_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_Memory1_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_Memory2_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCY_Chi_y_last_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCY_Chi_y_last2_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCY_SMCY_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCZ_Chi_z_last_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCZ_Chi_z_last2_state);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCZ_SMCZ_state);
    w_d_arr(w, s.LVDC_Iterative_Guidance_Mode_loop_state);
    w_d_arr(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_Memory2_state);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Rate_Limiter_0_005_last_output);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MF2_state);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MF3_state);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MF4_state);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS1_state);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS2_state);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS3_state);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS4_state);
    w1(w, s.Memory_state);
    w_d_arr(w, s.Rate_Limiter_last_output);
}

static void read_lvdc_states(BinaryReader &r, LVDA_LVDC_states_t &s) {
    r_d_arr(r, s.LVDC_Chi_command_angles_Time_Tilt_Rate_Limiter_Dynamic_Delay_Input2_state);
    s.LVDC_Iterative_Guidance_Mode_Memory1_state       = r_f64(r);
    s.LVDC_Iterative_Guidance_Mode_Memory2_state       = r_f64(r);
    s.LVDC_Iterative_Guidance_Mode_SMCY_Chi_y_last_state  = r_f64(r);
    s.LVDC_Iterative_Guidance_Mode_SMCY_Chi_y_last2_state = r_f64(r);
    s.LVDC_Iterative_Guidance_Mode_SMCY_SMCY_state        = r_f64(r);
    s.LVDC_Iterative_Guidance_Mode_SMCZ_Chi_z_last_state  = r_f64(r);
    s.LVDC_Iterative_Guidance_Mode_SMCZ_Chi_z_last2_state = r_f64(r);
    s.LVDC_Iterative_Guidance_Mode_SMCZ_SMCZ_state        = r_f64(r);
    r_d_arr(r, s.LVDC_Iterative_Guidance_Mode_loop_state);
    r_d_arr(r, s.LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_Memory2_state);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Rate_Limiter_0_005_last_output                  = r_f64(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MF2_state  = r_f64(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MF3_state  = r_f64(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MF4_state  = r_f64(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS1_state = r_f64(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS2_state = r_f64(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS3_state = r_f64(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_MFS4_state = r_f64(r);
    s.Memory_state = r_f64(r);
    r_d_arr(r, s.Rate_Limiter_last_output);
}

static void write_lvdc_data_stores(BinaryWriter &w, const LVDA_LVDC_data_stores_t &s) {
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_X_deg);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Y_deg);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Z_deg);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCY_rad);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nHSLActive);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nIGMMode);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCZ_rad);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_3_i_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_DeltaT_b_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_1_i_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_1_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nTerminalSteeringMode);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_P_C_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_alpha_f);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_N_sec);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T1_Timer_A);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T2_Timer_A);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T3_Timer_A);
    w1(w, s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T4_Timer_A);
}

static void read_lvdc_data_stores(BinaryReader &r, LVDA_LVDC_data_stores_t &s) {
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_X_deg = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Y_deg = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Z_deg = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCY_rad               = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nHSLActive             = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nIGMMode               = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCZ_rad               = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_3_i_sec              = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_DeltaT_b_sec           = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_1_i_sec              = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_sec              = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_1_sec              = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nTerminalSteeringMode  = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_P_C_sec                = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_alpha_f                = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_tau_3_N_sec            = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T1_Timer_A = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T2_Timer_A = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T3_Timer_A = r_f64(r);
    s.Saturn_Instrument_Unit_IU_LVDA_LVDC_T4_Timer_A = r_f64(r);
}

static void write_lvdc_enable_states(BinaryWriter &w, const LVDA_LVDC_enable_states_t &s) {
    w1(w, s.LVDC_Chi_command_angles_Time_Tilt_Store_Chi_Command_Angles_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_DeltaT_b_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_HSL_Cutoff_Timing_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Chi_Steering_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_First_Phase_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_First_Phase_Set_nIGMMode_to_1_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_HSL_first_pass_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_HSL_first_pass_Set_HSL_Mode_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_complete_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_complete_Set_HSL_Mode_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_complete_Set_Terminal_Steering_Mode_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_mode_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_mode_Set_nIGMMode_to_3_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_first_BML_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCY_enabled);
    w1(w, s.LVDC_Iterative_Guidance_Mode_SMCZ_enabled);
    w1(w, s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_enabled);
    w1(w, s.T1_Timer_Timer_Initialization_enabled);
    w1(w, s.T1_Timer_Timer_Initialization_trig_prev);
    w1(w, s.T2_Timer_Timer_Initialization_enabled);
    w1(w, s.T2_Timer_Timer_Initialization_trig_prev);
    w1(w, s.T3_Timer_Timer_Initialization_enabled);
    w1(w, s.T3_Timer_Timer_Initialization_trig_prev);
    w1(w, s.T4_Timer_Timer_Initialization_enabled);
    w1(w, s.T4_Timer_Timer_Initialization_trig_prev);
}

static void read_lvdc_enable_states(BinaryReader &r, LVDA_LVDC_enable_states_t &s) {
    s.LVDC_Chi_command_angles_Time_Tilt_Store_Chi_Command_Angles_enabled = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_enabled                               = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_DeltaT_b_enabled                      = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_HSL_Cutoff_Timing_enabled             = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Chi_Steering_enabled              = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_First_Phase_enabled               = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_First_Phase_Set_nIGMMode_to_1_enabled = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_HSL_first_pass_enabled                 = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_HSL_first_pass_Set_HSL_Mode_enabled    = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_complete_enabled        = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_complete_Set_HSL_Mode_enabled                = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_complete_Set_Terminal_Steering_Mode_enabled  = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_mode_enabled            = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_mode_Set_nIGMMode_to_3_enabled                = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_IGM_Phase2_first_BML_enabled                       = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_SMCY_enabled                                       = r_i32(r);
    s.LVDC_Iterative_Guidance_Mode_SMCZ_enabled                                       = r_i32(r);
    s.LVDC_S_Frame_Position_Velocity_Calculations_Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11_enabled = r_i32(r);
    s.T1_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.T1_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.T2_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.T2_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.T3_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.T3_Timer_Timer_Initialization_trig_prev = r_i32(r);
    s.T4_Timer_Timer_Initialization_enabled   = r_i32(r);
    s.T4_Timer_Timer_Initialization_trig_prev = r_i32(r);
}

// -----------------------------------------------------------------------
// Top-level entry points
// -----------------------------------------------------------------------

void plant_serialize(BinaryWriter &w, const SaturnIBPlantObliq *plant) {
    w.write_u32(kPlantFormatVersion);
    if (!plant || !plant->model_inited) {
        // Empty payload (version only).  A valid load will reject this
        // because the deserialize path reads substantially more.
        w.write_bool(false);
        return;
    }
    w.write_bool(true);

    const saturn_ib_stack_t &m = plant->model;
    w.write_f64(m.time);
    w.write_f64(m.dt);
    w.write_u64(m.sample_tick);
    w.write_i32(m.use_rk4);

    write_states       (w, m.states);
    write_data_stores  (w, m.data_stores);
    write_enable_states(w, m.enable_states);

    const LVDA_LVDC_t &lv = m.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDA_LVDC;
    w.write_f64(lv.time);
    w.write_f64(lv.dt);
    w.write_u64(lv.sample_tick);
    w.write_i32(lv.enabled);
    write_lvdc_states       (w, lv.states);
    write_lvdc_data_stores  (w, lv.data_stores);
    write_lvdc_enable_states(w, lv.enable_states);

    // Plant-level scalars that live on the opaque handle too.
    w.write_f64(plant->dt_sec);
    w.write_f64(plant->time_sec);

    // Top-level inputs.  Small, field-by-field portable.
    for (int i = 0; i < 6; ++i) w.write_f64(m.inputs.LaunchDate[i]);
    w.write_f64(m.inputs.A_z_deg);
    for (int i = 0; i < 3; ++i) w.write_f64(m.inputs.CG_LLA_deg_m[i]);
    for (int i = 0; i < 4; ++i) w.write_f64(m.inputs.q_ECItoSM[i][0]);
    w.write_f64(m.inputs.T_L_prime_sec);

    // Signals + outputs (both top-level and LVDC).  These are the
    // ~1600-field intermediate signal structs; a clean portable
    // serialization would hand-code each field or require a generator.
    // For v1 we dump them as raw bytes with an explicit u64 size prefix;
    // the reader rejects the file if the host endianness or struct
    // sizeof differs.  Still cross-build on the same ABI, which
    // covers all realistic DSKY-matic targets (x86_64/ARM64 LE).
    // Rationale documented at:
    //   The alternative — calling saturn_ib_stack_evaluate_algebraic
    //   post-load — overwrites several Memory*_state and other state
    //   fields from freshly-computed signals (see the deferred updates
    //   at the bottom of evaluate_algebraic), which corrupts the exact
    //   state we just restored.  Preserving signals verbatim avoids
    //   that whole class of edge cases.
    w.write_u64((uint64_t)sizeof(m.signals));
    w.write_bytes(&m.signals, sizeof(m.signals));
    w.write_u64((uint64_t)sizeof(m.outputs));
    w.write_bytes(&m.outputs, sizeof(m.outputs));

    const LVDA_LVDC_t &lv2 = m.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDA_LVDC;
    w.write_u64((uint64_t)sizeof(lv2.signals));
    w.write_bytes(&lv2.signals, sizeof(lv2.signals));
    w.write_u64((uint64_t)sizeof(lv2.outputs));
    w.write_bytes(&lv2.outputs, sizeof(lv2.outputs));
    w.write_u64((uint64_t)sizeof(lv2.inputs));
    w.write_bytes(&lv2.inputs,  sizeof(lv2.inputs));
}

bool plant_deserialize(BinaryReader &r, SaturnIBPlantObliq *plant) {
    uint32_t v = r.read_u32();
    if (v != kPlantFormatVersion) {
        std::fprintf(stderr,
            "plant_deserialize: version mismatch (file=%u, expected=%u)\n",
            v, kPlantFormatVersion);
        return false;
    }
    bool has_body = r.read_bool();
    if (!has_body) {
        std::fprintf(stderr, "plant_deserialize: empty payload\n");
        return false;
    }
    if (!plant) return false;

    saturn_ib_stack_t &m = plant->model;
    m.time        = r.read_f64();
    m.dt          = r.read_f64();
    m.sample_tick = r.read_u64();
    m.use_rk4     = r.read_i32();

    // signals + outputs will be rebuilt by evaluate_algebraic at the
    // bottom of this function, so zero them to a known starting point.
    std::memset(&m.signals, 0, sizeof m.signals);
    std::memset(&m.outputs, 0, sizeof m.outputs);
    std::memset(&m.inputs,  0, sizeof m.inputs);

    read_states       (r, m.states);
    read_data_stores  (r, m.data_stores);
    read_enable_states(r, m.enable_states);

    LVDA_LVDC_t &lv = m.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDA_LVDC;
    lv.time        = r.read_f64();
    lv.dt          = r.read_f64();
    lv.sample_tick = r.read_u64();
    lv.enabled     = r.read_i32();
    std::memset(&lv.signals, 0, sizeof lv.signals);
    std::memset(&lv.outputs, 0, sizeof lv.outputs);
    std::memset(&lv.inputs,  0, sizeof lv.inputs);
    read_lvdc_states       (r, lv.states);
    read_lvdc_data_stores  (r, lv.data_stores);
    read_lvdc_enable_states(r, lv.enable_states);

    plant->dt_sec   = r.read_f64();
    plant->time_sec = r.read_f64();
    plant->error    = nullptr;
    plant->model_inited = 1;

    // Prevent SaturnIBStackAdapter's one-shot integrator-IC reseed from
    // clobbering the restored states on the first post-resume tick.
    // That flag is module-static in SaturnIBStackAdapter.c; by the time
    // we're writing a save we've already run >=1 tick, so the real value
    // is always 1 — safe to force unconditionally here.
    saturn_ib_stack_set_ics_seeded(1);

    // Top-level inputs (see write side).
    for (int i = 0; i < 6; ++i) m.inputs.LaunchDate[i] = r.read_f64();
    m.inputs.A_z_deg = r.read_f64();
    for (int i = 0; i < 3; ++i) m.inputs.CG_LLA_deg_m[i] = r.read_f64();
    for (int i = 0; i < 4; ++i) m.inputs.q_ECItoSM[i][0] = r.read_f64();
    m.inputs.T_L_prime_sec = r.read_f64();

    // Signals + outputs (raw bytes, see write side comment).  The
    // size prefix is a sanity check against ABI drift / wrong plant
    // version: if sizeof(signals) differs, we've got mismatched code
    // and the file, so refuse rather than read garbage.
    auto check_raw = [&](void *dst, size_t want, const char *what) -> bool {
        uint64_t sz = r.read_u64();
        if (sz != want) {
            std::fprintf(stderr,
                "plant_deserialize: %s size mismatch (file=%llu code=%zu). "
                "File was built against a different plant/ABI.\n",
                what, (unsigned long long)sz, want);
            return false;
        }
        r.read_bytes(dst, want);
        return true;
    };
    LVDA_LVDC_t &lv2 = m.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDA_LVDC;
    if (!check_raw(&m.signals,    sizeof(m.signals),    "signals"))       return false;
    if (!check_raw(&m.outputs,    sizeof(m.outputs),    "outputs"))       return false;
    if (!check_raw(&lv2.signals,  sizeof(lv2.signals),  "lvdc.signals"))  return false;
    if (!check_raw(&lv2.outputs,  sizeof(lv2.outputs),  "lvdc.outputs"))  return false;
    if (!check_raw(&lv2.inputs,   sizeof(lv2.inputs),   "lvdc.inputs"))   return false;

    return r.ok();
}

} // namespace dskymatic
