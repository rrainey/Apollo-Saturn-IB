# Hard-coded constants & gains catalog

Silver model: `lib_SaturnIBObliq/model/saturn_ib_stack.json`

## Scope

In Obliq JSON, Simulink **Constant** blocks are `source` with `signalType: "constant"`,
and Simulink **Gain** blocks are `scale`. This report lists those whose
`useParameter` flag is **false / absent** (literal hard-coded values).

- Presetting profile: `tm-x-62831`
- Metadata: TM-X-62831 presettings + App B tilt + HSL model-level + Table 11 J-2 thrust
- Hard-coded entries: **266** (205 constants, 61 scales)
- Non-trivial (not 0/±1): **181**
- Trivial (0/±1): **85**
- Excluded parameterized (`useParameter=true`): **63**
- Companion CSV: [`HARDCODED_CONSTANTS_GAINS_CATALOG.csv`](./HARDCODED_CONSTANTS_GAINS_CATALOG.csv)

**Subsystem path** is the hierarchy of nested `subsystem` blocks from the root
`Saturn_IB_Stack` model down to the subsystem that owns the block.

## Contents

- [Non-trivial hard-coded values](#non-trivial-hard-coded-values)
- [Non-trivial by subsystem](#non-trivial-by-subsystem)
- [Trivial hard-coded values (0 / ±1)](#trivial-hard-coded-values-0--1)
- [Parameterized constants (excluded)](#parameterized-constants-excluded)

## Non-trivial hard-coded values

| Block type | Block name | Value | Subsystem path |
|---|---|---|---|
| `source (constant)` | `S_IVB_Gimbal_Station` | `[25.0483878,0,0]` | `Saturn_IB_Stack` |
| `scale (gain)` | `m_sec_to_pulses` | `17.094017094` | `Saturn_IB_Stack/CM_ISS` |
| `source (constant)` | `omega_e_rad_per_sec` | `7.292115e-05` | `Saturn_IB_Stack/Earth_s_Rotation` |
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` | `Saturn_IB_Stack/Initial_Conditions` |
| `source (constant)` | `WGS_84_ECC_2` | `0.00669438000426081` | `Saturn_IB_Stack/Initial_Conditions/LLA_to_ECF` |
| `source (constant)` | `WGS_84_major` | `6378137` | `Saturn_IB_Stack/Initial_Conditions/LLA_to_ECF` |
| `source (constant)` | `WGS_84_minor` | `6356752.3142` | `Saturn_IB_Stack/Initial_Conditions/LLA_to_ECF` |
| `source (constant)` | `one_day` | `86400` | `Saturn_IB_Stack/Initial_Conditions/T_to_GMST` |
| `scale (gain)` | `seconds_to_degrees` | `0.00416666666667` | `Saturn_IB_Stack/Initial_Conditions/T_to_GMST` |
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` | `Saturn_IB_Stack/On_Pad` |
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` | `Saturn_IB_Stack/On_Pad/Centrifugal_Acceleration` |
| `scale (gain)` | `Gain` | `4` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `scale (gain)` | `Gain1` | `2` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `scale (gain)` | `Gain2` | `0.333333333333333` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `scale (gain)` | `Gain3` | `2` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `scale (gain)` | `Gain4` | `0.5` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `scale (gain)` | `Gain5` | `2` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `scale (gain)` | `Gain6` | `2` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `source (constant)` | `a` | `6378136.3` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `source (constant)` | `a1` | `3` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `source (constant)` | `a2` | `0.333333333333333` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `source (constant)` | `a3` | `0.333333333333333` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `source (constant)` | `b` | `6356751.60056` | `Saturn_IB_Stack/On_Pad/ECI_to_LLA` |
| `source (constant)` | `GM` | `398600441800000` | `Saturn_IB_Stack/On_Pad/Earth_Gravity_Model` |
| `scale (gain)` | `Gain` | `5` | `Saturn_IB_Stack/On_Pad/Earth_Gravity_Model` |
| `scale (gain)` | `Gain1` | `1.5` | `Saturn_IB_Stack/On_Pad/Earth_Gravity_Model` |
| `source (constant)` | `J1` | `3` | `Saturn_IB_Stack/On_Pad/Earth_Gravity_Model` |
| `source (constant)` | `J2` | `0.00108263` | `Saturn_IB_Stack/On_Pad/Earth_Gravity_Model` |
| `source (constant)` | `Re` | `6378137` | `Saturn_IB_Stack/On_Pad/Earth_Gravity_Model` |
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` | `Saturn_IB_Stack/On_Pad/Initial_Conditions` |
| `source (constant)` | `WGS_84_ECC_2` | `0.00669438000426081` | `Saturn_IB_Stack/On_Pad/Initial_Conditions/LLA_to_ECF` |
| `source (constant)` | `WGS_84_major` | `6378137` | `Saturn_IB_Stack/On_Pad/Initial_Conditions/LLA_to_ECF` |
| `source (constant)` | `WGS_84_minor` | `6356752.3142` | `Saturn_IB_Stack/On_Pad/Initial_Conditions/LLA_to_ECF` |
| `source (constant)` | `Post_separation_CG` | `[9.3849698,0,0]` | `Saturn_IB_Stack/S_IB_Stage/Aerodynamic_Forces_and_Moments` |
| `source (constant)` | `Referece_Area_m_2` | `34.25` | `Saturn_IB_Stack/S_IB_Stage/Aerodynamic_Forces_and_Moments` |
| `scale (gain)` | `Gain` | `4` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain1` | `2` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain2` | `0.333333333333333` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain3` | `2` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain4` | `0.5` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain5` | `2` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain6` | `2` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `source (constant)` | `a` | `6378136.3` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `source (constant)` | `a1` | `3` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `source (constant)` | `a2` | `0.333333333333333` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `source (constant)` | `a3` | `0.333333333333333` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `source (constant)` | `b` | `6356751.60056` | `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA` |
| `source (constant)` | `GM` | `398600441800000` | `Saturn_IB_Stack/S_IB_Stage/Earth_Gravity_Model` |
| `scale (gain)` | `Gain` | `5` | `Saturn_IB_Stack/S_IB_Stage/Earth_Gravity_Model` |
| `scale (gain)` | `Gain1` | `1.5` | `Saturn_IB_Stack/S_IB_Stage/Earth_Gravity_Model` |
| `source (constant)` | `J1` | `3` | `Saturn_IB_Stack/S_IB_Stage/Earth_Gravity_Model` |
| `source (constant)` | `J2` | `0.00108263` | `Saturn_IB_Stack/S_IB_Stage/Earth_Gravity_Model` |
| `source (constant)` | `Re` | `6378137` | `Saturn_IB_Stack/S_IB_Stage/Earth_Gravity_Model` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.06079469861202` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `Installation_location_m` | `[0,-1.704,1.704]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,-6,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `Engine_Mount_Point_Rotation` | `[-90,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.03764737542202` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `source (constant)` | `Installation_location_m` | `[0,-1.704,-1.704]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,6,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `source (constant)` | `Engine_Mount_Point_Rotation` | `[-180,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04883783748323` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `source (constant)` | `Installation_location_m` | `[0,1.704,-1.704]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,-6,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `source (constant)` | `Engine_Mount_Point_Rotation` | `[-270,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.03285220287751` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `source (constant)` | `Installation_location_m` | `[0,1.704,1.704]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,6,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04429640517056` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_5` |
| `source (constant)` | `Installation_location_m` | `[0,0,0.81]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_5` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,-3,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_5` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_5` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.0533792697959` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_6` |
| `source (constant)` | `Installation_location_m` | `[0,-0.81,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_6` |
| `source (constant)` | `Neutral_tilt_Eulker_angles_deg` | `[0,0,-3]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_6` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_6` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04226130494378` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_7` |
| `source (constant)` | `Installation_location_m` | `[0,0,-0.81]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_7` |
| `source (constant)` | `Neutral_tilt_Eulker_angles_deg` | `[0,3,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_7` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_7` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04938674492353` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_8` |
| `source (constant)` | `Installation_location_m` | `[0,0.81,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_8` |
| `source (constant)` | `Neutral_tilt_Eulker_angles_deg` | `[0,0,3]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_8` |
| `source (constant)` | `m_dot` | `352.144735478113` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_8` |
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` | `Saturn_IB_Stack/S_IB_Stage/Relative_Wind` |
| `scale (gain)` | `Gain` | `-4` | `Saturn_IB_Stack/S_IB_Stage/Retrorocket_Motors` |
| `source (constant)` | `Retro_Motor_cant_angle` | `9.5` | `Saturn_IB_Stack/S_IB_Stage/Retrorocket_Motors` |
| `source (constant)` | `Post_separation_CG` | `[7.0989952,0,0]` | `Saturn_IB_Stack/S_IB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Post_separation_Mass_approximate` | `42000` | `Saturn_IB_Stack/S_IB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Constant` | `100000` | `Saturn_IB_Stack/S_IVB_Stage` |
| `source (constant)` | `Installation_location_m` | `[3.5,0,-3.3]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_II` |
| `source (constant)` | `Thruster_Force` | `[0,670,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_II` |
| `source (constant)` | `Installation_location_m` | `[3.5,0,-3.3]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_IV` |
| `source (constant)` | `Thruster_Force` | `[0,-670,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_IV` |
| `source (constant)` | `Installation_location_m` | `[3.5,0,-3.3]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_P` |
| `source (constant)` | `Thruster_Force` | `[0,0,670]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_P` |
| `source (constant)` | `Installation_location_m` | `[3.5,0,3.3]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_II` |
| `source (constant)` | `Thruster_Force` | `[0,670,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_II` |
| `source (constant)` | `Installation_location_m` | `[3.5,0,3.3]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_IV` |
| `source (constant)` | `Thruster_Force` | `[0,-670,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_IV` |
| `source (constant)` | `Installation_location_m` | `[3.5,0,3.3]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_P` |
| `source (constant)` | `Thruster_Force` | `[0,0,-670]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_P` |
| `source (constant)` | `Referece_Area_m_2` | `34.25` | `Saturn_IB_Stack/S_IVB_Stage/Aerodynamic_Forces_and_Moments` |
| `scale (gain)` | `Gain` | `4` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain1` | `2` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain2` | `0.333333333333333` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain3` | `2` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain4` | `0.5` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain5` | `2` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `scale (gain)` | `Gain6` | `2` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `source (constant)` | `a` | `6378136.3` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `source (constant)` | `a1` | `3` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `source (constant)` | `a2` | `0.333333333333333` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `source (constant)` | `a3` | `0.333333333333333` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `source (constant)` | `b` | `6356751.60056` | `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA` |
| `source (constant)` | `GM` | `398600441800000` | `Saturn_IB_Stack/S_IVB_Stage/Earth_Gravity_Model` |
| `scale (gain)` | `Gain` | `5` | `Saturn_IB_Stack/S_IVB_Stage/Earth_Gravity_Model` |
| `scale (gain)` | `Gain1` | `1.5` | `Saturn_IB_Stack/S_IVB_Stage/Earth_Gravity_Model` |
| `source (constant)` | `J1` | `3` | `Saturn_IB_Stack/S_IVB_Stage/Earth_Gravity_Model` |
| `source (constant)` | `J2` | `0.00108263` | `Saturn_IB_Stack/S_IVB_Stage/Earth_Gravity_Model` |
| `source (constant)` | `Re` | `6378137` | `Saturn_IB_Stack/S_IVB_Stage/Earth_Gravity_Model` |
| `source (constant)` | `Constant3` | `1010719` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model` |
| `scale (gain)` | `in_to_m` | `0.0254` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model` |
| `source (constant)` | `m_dot` | `243.687` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model` |
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` | `Saturn_IB_Stack/S_IVB_Stage/Relative_Wind` |
| `scale (gain)` | `Gain` | `3` | `Saturn_IB_Stack/S_IVB_Stage/Ullage_Rocket_Motors` |
| `source (constant)` | `Ullage_Motor_cant_angle` | `35` | `Saturn_IB_Stack/S_IVB_Stage/Ullage_Rocket_Motors` |
| `source (constant)` | `Constant1` | `-8986` | `Saturn_IB_Stack/S_IVB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Constant2` | `-214` | `Saturn_IB_Stack/S_IVB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Roll_blending_factor` | `[0.707,1,1]` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer` |
| `scale (gain)` | `Gain` | `5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer/Spatial_Attitude_Control_System` |
| `source (constant)` | `Constant2` | `-28` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC` |
| `source (constant)` | `P_final` | `-1.0352499` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `source (constant)` | `T_S1_sec` | `63.8` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `source (constant)` | `T_S1_sec1` | `10` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `source (constant)` | `T_S2_sec` | `109.8` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `source (constant)` | `T_S3_sec` | `134.3` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `scale (gain)` | `Gain1` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_to_Psi_Transformation` |
| `source (constant)` | `Constant3` | `1.8` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Constant6` | `1.51505854565e-07` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `DeltaT_N_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `XYZdot_VT_mps` | `[0,0,7780.67]` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `XYZdotdot_VT_mps2` | `[-9.15,0,0]` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `const` | `1.57079632679` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `scale (gain)` | `Gain` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Estimated_Velocity_to_be_gained` |
| `source (constant)` | `Constant` | `0.04` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/HSL_Cutoff_Timing` |
| `source (constant)` | `Constant1` | `2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/HSL_Cutoff_Timing` |
| `scale (gain)` | `Gain2` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Intermediate_Parameters` |
| `source (constant)` | `HSL_mode_ON` | `4` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete/Set_HSL_Mode` |
| `source (constant)` | `Constant` | `4` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `artifical_tau_complete` | `3` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode/Set_nIGMMode_to_3` |
| `source (constant)` | `Constant` | `4` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `Phase_2` | `2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `scale (gain)` | `Gain1` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Improved_Estimate_of_Total_time_to_go` |
| `scale (gain)` | `Gain5` | `1/V_ex3_mps` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Improved_Estimate_of_Total_time_to_go` |
| `scale (gain)` | `Gain1` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_1_K_3_K_4_Eqns_4_4_51_67` |
| `scale (gain)` | `Gain2` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_1_K_3_K_4_Eqns_4_4_51_67` |
| `scale (gain)` | `Gain` | `2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_2_K_1_K_2_Eqns_4_4_68_80` |
| `scale (gain)` | `Gain1` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_2_K_1_K_2_Eqns_4_4_68_80` |
| `scale (gain)` | `Gain2` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_2_K_1_K_2_Eqns_4_4_68_80` |
| `source (constant)` | `Constant9` | `0.509295817894` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCY` |
| `scale (gain)` | `Gain1` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCY` |
| `scale (gain)` | `SMCG_Y_Axis` | `0.03` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCY` |
| `scale (gain)` | `pirads_to_radians` | `3.14159265358979` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCY` |
| `source (constant)` | `Constant8` | `0.509295817894` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCZ` |
| `scale (gain)` | `Gain` | `0.5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCZ` |
| `scale (gain)` | `SMCG` | `0.03` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCZ` |
| `scale (gain)` | `pirads_to_radians` | `3.14159265358979` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCZ` |
| `scale (gain)` | `2J` | `0.0032469` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Estimated_Gravitational_Acceleration_Eqns_4_3_6_12` |
| `source (constant)` | `Constant` | `-398603200000000` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Estimated_Gravitational_Acceleration_Eqns_4_3_6_12` |
| `source (constant)` | `Constant1` | `-398603200000000` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Estimated_Gravitational_Acceleration_Eqns_4_3_6_12` |
| `scale (gain)` | `Gain3` | `0.625` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5` |
| `source (constant)` | `MFK` | `[0.0120948,0.0213339,-0.0261577,-0.00526684,2.80483,-2.8588,1.24933,-0.19736]` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11` |

## Non-trivial by subsystem

### `Saturn_IB_Stack`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `S_IVB_Gimbal_Station` | `[25.0483878,0,0]` |

### `Saturn_IB_Stack/CM_ISS`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `m_sec_to_pulses` | `17.094017094` |

### `Saturn_IB_Stack/Earth_s_Rotation`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `omega_e_rad_per_sec` | `7.292115e-05` |

### `Saturn_IB_Stack/Initial_Conditions`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` |

### `Saturn_IB_Stack/Initial_Conditions/LLA_to_ECF`

3 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `WGS_84_ECC_2` | `0.00669438000426081` |
| `source (constant)` | `WGS_84_major` | `6378137` |
| `source (constant)` | `WGS_84_minor` | `6356752.3142` |

### `Saturn_IB_Stack/Initial_Conditions/T_to_GMST`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `one_day` | `86400` |
| `scale (gain)` | `seconds_to_degrees` | `0.00416666666667` |

### `Saturn_IB_Stack/On_Pad`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` |

### `Saturn_IB_Stack/On_Pad/Centrifugal_Acceleration`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` |

### `Saturn_IB_Stack/On_Pad/ECI_to_LLA`

12 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `4` |
| `scale (gain)` | `Gain1` | `2` |
| `scale (gain)` | `Gain2` | `0.333333333333333` |
| `scale (gain)` | `Gain3` | `2` |
| `scale (gain)` | `Gain4` | `0.5` |
| `scale (gain)` | `Gain5` | `2` |
| `scale (gain)` | `Gain6` | `2` |
| `source (constant)` | `a` | `6378136.3` |
| `source (constant)` | `a1` | `3` |
| `source (constant)` | `a2` | `0.333333333333333` |
| `source (constant)` | `a3` | `0.333333333333333` |
| `source (constant)` | `b` | `6356751.60056` |

### `Saturn_IB_Stack/On_Pad/Earth_Gravity_Model`

6 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `GM` | `398600441800000` |
| `scale (gain)` | `Gain` | `5` |
| `scale (gain)` | `Gain1` | `1.5` |
| `source (constant)` | `J1` | `3` |
| `source (constant)` | `J2` | `0.00108263` |
| `source (constant)` | `Re` | `6378137` |

### `Saturn_IB_Stack/On_Pad/Initial_Conditions`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` |

### `Saturn_IB_Stack/On_Pad/Initial_Conditions/LLA_to_ECF`

3 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `WGS_84_ECC_2` | `0.00669438000426081` |
| `source (constant)` | `WGS_84_major` | `6378137` |
| `source (constant)` | `WGS_84_minor` | `6356752.3142` |

### `Saturn_IB_Stack/S_IB_Stage/Aerodynamic_Forces_and_Moments`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Post_separation_CG` | `[9.3849698,0,0]` |
| `source (constant)` | `Referece_Area_m_2` | `34.25` |

### `Saturn_IB_Stack/S_IB_Stage/ECI_to_LLA`

12 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `4` |
| `scale (gain)` | `Gain1` | `2` |
| `scale (gain)` | `Gain2` | `0.333333333333333` |
| `scale (gain)` | `Gain3` | `2` |
| `scale (gain)` | `Gain4` | `0.5` |
| `scale (gain)` | `Gain5` | `2` |
| `scale (gain)` | `Gain6` | `2` |
| `source (constant)` | `a` | `6378136.3` |
| `source (constant)` | `a1` | `3` |
| `source (constant)` | `a2` | `0.333333333333333` |
| `source (constant)` | `a3` | `0.333333333333333` |
| `source (constant)` | `b` | `6356751.60056` |

### `Saturn_IB_Stack/S_IB_Stage/Earth_Gravity_Model`

6 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `GM` | `398600441800000` |
| `scale (gain)` | `Gain` | `5` |
| `scale (gain)` | `Gain1` | `1.5` |
| `source (constant)` | `J1` | `3` |
| `source (constant)` | `J2` | `0.00108263` |
| `source (constant)` | `Re` | `6378137` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1`

5 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.06079469861202` |
| `source (constant)` | `Installation_location_m` | `[0,-1.704,1.704]` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,-6,0]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2`

6 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Engine_Mount_Point_Rotation` | `[-90,0,0]` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.03764737542202` |
| `source (constant)` | `Installation_location_m` | `[0,-1.704,-1.704]` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,6,0]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3`

6 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Engine_Mount_Point_Rotation` | `[-180,0,0]` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04883783748323` |
| `source (constant)` | `Installation_location_m` | `[0,1.704,-1.704]` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,-6,0]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4`

6 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Engine_Mount_Point_Rotation` | `[-270,0,0]` |
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.03285220287751` |
| `source (constant)` | `Installation_location_m` | `[0,1.704,1.704]` |
| `source (constant)` | `Neutral_Thrust_Alignment` | `[-45,0,0]` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,6,0]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_5`

4 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04429640517056` |
| `source (constant)` | `Installation_location_m` | `[0,0,0.81]` |
| `source (constant)` | `Neutral_tilt_Euler_angles_deg` | `[0,-3,0]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_6`

4 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.0533792697959` |
| `source (constant)` | `Installation_location_m` | `[0,-0.81,0]` |
| `source (constant)` | `Neutral_tilt_Eulker_angles_deg` | `[0,0,-3]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_7`

4 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04226130494378` |
| `source (constant)` | `Installation_location_m` | `[0,0,-0.81]` |
| `source (constant)` | `Neutral_tilt_Eulker_angles_deg` | `[0,3,0]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_8`

4 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Engine_Performance_Scaling_Factor` | `1.04938674492353` |
| `source (constant)` | `Installation_location_m` | `[0,0.81,0]` |
| `source (constant)` | `Neutral_tilt_Eulker_angles_deg` | `[0,0,3]` |
| `source (constant)` | `m_dot` | `352.144735478113` |

### `Saturn_IB_Stack/S_IB_Stage/Relative_Wind`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` |

### `Saturn_IB_Stack/S_IB_Stage/Retrorocket_Motors`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `-4` |
| `source (constant)` | `Retro_Motor_cant_angle` | `9.5` |

### `Saturn_IB_Stack/S_IB_Stage/Vehicle_Mass_Properties`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Post_separation_CG` | `[7.0989952,0,0]` |
| `source (constant)` | `Post_separation_Mass_approximate` | `42000` |

### `Saturn_IB_Stack/S_IVB_Stage`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant` | `100000` |

### `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_II`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Installation_location_m` | `[3.5,0,-3.3]` |
| `source (constant)` | `Thruster_Force` | `[0,670,0]` |

### `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_IV`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Installation_location_m` | `[3.5,0,-3.3]` |
| `source (constant)` | `Thruster_Force` | `[0,-670,0]` |

### `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_P`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Installation_location_m` | `[3.5,0,-3.3]` |
| `source (constant)` | `Thruster_Force` | `[0,0,670]` |

### `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_II`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Installation_location_m` | `[3.5,0,3.3]` |
| `source (constant)` | `Thruster_Force` | `[0,670,0]` |

### `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_IV`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Installation_location_m` | `[3.5,0,3.3]` |
| `source (constant)` | `Thruster_Force` | `[0,-670,0]` |

### `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_P`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Installation_location_m` | `[3.5,0,3.3]` |
| `source (constant)` | `Thruster_Force` | `[0,0,-670]` |

### `Saturn_IB_Stack/S_IVB_Stage/Aerodynamic_Forces_and_Moments`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Referece_Area_m_2` | `34.25` |

### `Saturn_IB_Stack/S_IVB_Stage/ECI_to_LLA`

12 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `4` |
| `scale (gain)` | `Gain1` | `2` |
| `scale (gain)` | `Gain2` | `0.333333333333333` |
| `scale (gain)` | `Gain3` | `2` |
| `scale (gain)` | `Gain4` | `0.5` |
| `scale (gain)` | `Gain5` | `2` |
| `scale (gain)` | `Gain6` | `2` |
| `source (constant)` | `a` | `6378136.3` |
| `source (constant)` | `a1` | `3` |
| `source (constant)` | `a2` | `0.333333333333333` |
| `source (constant)` | `a3` | `0.333333333333333` |
| `source (constant)` | `b` | `6356751.60056` |

### `Saturn_IB_Stack/S_IVB_Stage/Earth_Gravity_Model`

6 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `GM` | `398600441800000` |
| `scale (gain)` | `Gain` | `5` |
| `scale (gain)` | `Gain1` | `1.5` |
| `source (constant)` | `J1` | `3` |
| `source (constant)` | `J2` | `0.00108263` |
| `source (constant)` | `Re` | `6378137` |

### `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model`

3 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant3` | `1010719` |
| `scale (gain)` | `in_to_m` | `0.0254` |
| `source (constant)` | `m_dot` | `243.687` |

### `Saturn_IB_Stack/S_IVB_Stage/Relative_Wind`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `omega_E_rad_per_sec` | `[0,0,7.292115e-05]` |

### `Saturn_IB_Stack/S_IVB_Stage/Ullage_Rocket_Motors`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `3` |
| `source (constant)` | `Ullage_Motor_cant_angle` | `35` |

### `Saturn_IB_Stack/S_IVB_Stage/Vehicle_Mass_Properties`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant1` | `-8986` |
| `source (constant)` | `Constant2` | `-214` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Roll_blending_factor` | `[0.707,1,1]` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer/Spatial_Attitude_Control_System`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `5` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant2` | `-28` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt`

5 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `P_final` | `-1.0352499` |
| `source (constant)` | `T_S1_sec` | `63.8` |
| `source (constant)` | `T_S1_sec1` | `10` |
| `source (constant)` | `T_S2_sec` | `109.8` |
| `source (constant)` | `T_S3_sec` | `134.3` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_to_Psi_Transformation`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain1` | `0.5` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode`

6 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant3` | `1.8` |
| `source (constant)` | `Constant6` | `1.51505854565e-07` |
| `source (constant)` | `DeltaT_N_sec` | `1.6` |
| `source (constant)` | `XYZdot_VT_mps` | `[0,0,7780.67]` |
| `source (constant)` | `XYZdotdot_VT_mps2` | `[-9.15,0,0]` |
| `source (constant)` | `const` | `1.57079632679` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Estimated_Velocity_to_be_gained`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `0.5` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/HSL_Cutoff_Timing`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant` | `0.04` |
| `source (constant)` | `Constant1` | `2` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Intermediate_Parameters`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain2` | `0.5` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete/Set_HSL_Mode`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `HSL_mode_ON` | `4` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant` | `4` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode/Set_nIGMMode_to_3`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `artifical_tau_complete` | `3` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant` | `4` |
| `source (constant)` | `Phase_2` | `2` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Improved_Estimate_of_Total_time_to_go`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain1` | `0.5` |
| `scale (gain)` | `Gain5` | `1/V_ex3_mps` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_1_K_3_K_4_Eqns_4_4_51_67`

2 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain1` | `0.5` |
| `scale (gain)` | `Gain2` | `0.5` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_2_K_1_K_2_Eqns_4_4_68_80`

3 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain` | `2` |
| `scale (gain)` | `Gain1` | `0.5` |
| `scale (gain)` | `Gain2` | `0.5` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCY`

4 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant9` | `0.509295817894` |
| `scale (gain)` | `Gain1` | `0.5` |
| `scale (gain)` | `SMCG_Y_Axis` | `0.03` |
| `scale (gain)` | `pirads_to_radians` | `3.14159265358979` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/SMCZ`

4 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `Constant8` | `0.509295817894` |
| `scale (gain)` | `Gain` | `0.5` |
| `scale (gain)` | `SMCG` | `0.03` |
| `scale (gain)` | `pirads_to_radians` | `3.14159265358979` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Estimated_Gravitational_Acceleration_Eqns_4_3_6_12`

3 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `2J` | `0.0032469` |
| `source (constant)` | `Constant` | `-398603200000000` |
| `source (constant)` | `Constant1` | `-398603200000000` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `scale (gain)` | `Gain3` | `0.625` |

### `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations/Reciprocal_Acceleration_Filter_EDD_eqn_4_2_11`

1 hard-coded non-trivial constant/gain block(s).

| Block type | Block name | Value |
|---|---|---|
| `source (constant)` | `MFK` | `[0.0120948,0.0213339,-0.0261577,-0.00526684,2.80483,-2.8588,1.24933,-0.19736]` |

## Trivial hard-coded values (0 / ±1)

Ground / unity / structural wiring. Included with subsystem path for completeness.

| Block type | Block name | Value | Subsystem path |
|---|---|---|---|
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/CM_ISS` |
| `source (constant)` | `Ground1` | `0` | `Saturn_IB_Stack/CM_ISS` |
| `source (constant)` | `zero` | `0` | `Saturn_IB_Stack/Initial_Conditions` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/On_Pad` |
| `source (constant)` | `zero` | `0` | `Saturn_IB_Stack/On_Pad/Initial_Conditions` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/S_IB_Stage` |
| `source (constant)` | `Ground1` | `0` | `Saturn_IB_Stack/S_IB_Stage` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/Aerodynamic_Forces_and_Moments` |
| `source (constant)` | `Constant` | `1` | `Saturn_IB_Stack/S_IB_Stage/Custom_Variable_Mass_6DoF_Quaternion/Calculate_DCM_Euler_Angles/qdot` |
| `scale (gain)` | `High_Gain_Quaternion_Normalization` | `1` | `Saturn_IB_Stack/S_IB_Stage/Custom_Variable_Mass_6DoF_Quaternion/Calculate_DCM_Euler_Angles/qdot` |
| `source (constant)` | `Constant1` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `Engine_Mount_Point_Rotation` | `[0,0,0]` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_1/Triggered_Timer` |
| `source (constant)` | `Constant1` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_2/Triggered_Timer` |
| `source (constant)` | `Constant1` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_3/Triggered_Timer` |
| `source (constant)` | `Constant1` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_4/Triggered_Timer` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_5` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_5/Triggered_Timer` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_6` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_6/Triggered_Timer` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_7` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_7/Triggered_Timer` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_8` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/H_1_Engine_Cluster/Engine_8/Triggered_Timer` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/Relative_Wind/Incidence_Sideslip_Airspeed` |
| `source (constant)` | `zero` | `0` | `Saturn_IB_Stack/S_IB_Stage/Retrorocket_Motors` |
| `source (constant)` | `zero1` | `[0,0,0]` | `Saturn_IB_Stack/S_IB_Stage/Retrorocket_Motors` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/Retrorocket_Motors/Time_Since_Engine_Start` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/S_IVB_Stage` |
| `source (constant)` | `Ground1` | `0` | `Saturn_IB_Stack/S_IVB_Stage` |
| `source (constant)` | `Ground2` | `0` | `Saturn_IB_Stack/S_IVB_Stage` |
| `source (constant)` | `Constant2` | `[0,0,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_II` |
| `source (constant)` | `Constant2` | `[0,0,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_IV` |
| `source (constant)` | `Constant2` | `[0,0,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_III_Module/APS_Thruster_P` |
| `source (constant)` | `Constant2` | `[0,0,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_II` |
| `source (constant)` | `Constant2` | `[0,0,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_IV` |
| `source (constant)` | `Constant2` | `[0,0,0]` | `Saturn_IB_Stack/S_IVB_Stage/APS_I_Module/APS_Thruster_P` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IVB_Stage/Aerodynamic_Forces_and_Moments` |
| `source (constant)` | `Constant` | `1` | `Saturn_IB_Stack/S_IVB_Stage/Custom_Variable_Mass_6DoF_Quaternion/Calculate_DCM_Euler_Angles/qdot` |
| `scale (gain)` | `High_Gain_Quaternion_Normalization` | `1` | `Saturn_IB_Stack/S_IVB_Stage/Custom_Variable_Mass_6DoF_Quaternion/Calculate_DCM_Euler_Angles/qdot` |
| `source (constant)` | `Constant1` | `0` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model` |
| `source (constant)` | `Constant2` | `0` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model` |
| `source (constant)` | `Installation_location_in` | `[0,0,0]` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model/Time_Since_Engine_Start` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IVB_Stage/J_2_Engine/Thrust_Model/Time_Since_Shutdown` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IVB_Stage/Relative_Wind/Incidence_Sideslip_Airspeed` |
| `source (constant)` | `zero` | `0` | `Saturn_IB_Stack/S_IVB_Stage/Ullage_Rocket_Motors` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IVB_Stage/Ullage_Rocket_Motors/Time_Since_Engine_Start` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/S_IVB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/S_IVB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Constant3` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU` |
| `source (constant)` | `Constant4` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Accelerometers` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer` |
| `source (constant)` | `Ground` | `[0,0,0]` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer/S_IB_Attitude_Rate_Error` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer/S_IVB_Stage_Timer` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/Flight_Control_Computer/Spatial_Attitude_Control_System` |
| `source (constant)` | `chi_rate_limit` | `[1,1,1]` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `source (constant)` | `yaw_command` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `source (constant)` | `zero` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Chi_command_angles_Time_Tilt` |
| `source (constant)` | `Constant5` | `1` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Constant7` | `1` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/HSL_Cutoff_Timing` |
| `source (constant)` | `First_BML_of_Phase_2` | `1` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_First_Phase/Set_nIGMMode_to_1` |
| `source (constant)` | `HSL_mode_ON` | `1` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_HSL_first_pass/Set_HSL_Mode` |
| `source (constant)` | `nHSLActive_ON` | `1` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete/Set_HSL_Mode` |
| `source (constant)` | `terminal_steering_mode_ON` | `1` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete/Set_Terminal_Steering_Mode` |
| `source (constant)` | `Constant3` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `Constant3` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `Ground` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms` |
| `source (constant)` | `Constant5` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/T1_Timer` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/T2_Timer` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/T3_Timer` |
| `source (constant)` | `Constant` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/T4_Timer` |

## Parameterized constants (excluded)

These use `useParameter: true` and are **not** hard-coded; listed for contrast.

| Block type | Block name | parameterName | Fallback value | Subsystem path |
|---|---|---|---|---|
| `source (constant)` | `Constant2` | `A_z_deg` | `72` | `Saturn_IB_Stack/Initial_Conditions/MES_Transform` |
| `source (constant)` | `Constant2` | `A_z_deg` | `72` | `Saturn_IB_Stack/MES_Transform` |
| `source (constant)` | `Constant2` | `A_z_deg` | `72` | `Saturn_IB_Stack/On_Pad/Initial_Conditions/MES_Transform` |
| `source (constant)` | `Constant10` | `A_z_deg` | `72` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Az_deg` | `A_z_deg` | `72` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations` |
| `source (constant)` | `Constant6` | `C_0_sec` | `35` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `Constant6` | `C_0_sec` | `35` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `Constant1` | `DeltaV_b_mps` | `7.2381` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/DeltaT_b` |
| `source (constant)` | `Constant3` | `DeltaV_b_mps` | `7.2381` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_HSL_first_pass` |
| `source (constant)` | `M_GR` | `M02_kg` | `129761.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `M_GR` | `M02_kg` | `129761.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `Constant5` | `Mdot_1_kg_per_sec` | `239.7877` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `Constant5` | `Mdot_1_kg_per_sec` | `239.7877` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `M_GR1` | `Mdot_3_kg_per_sec` | `183.1797` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `M_GR1` | `Mdot_3_kg_per_sec` | `183.1797` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `R_L` | `R_L_m` | `6373385` | `Saturn_IB_Stack/Initial_Conditions` |
| `source (constant)` | `R_L` | `R_L_m` | `6373385` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations` |
| `source (constant)` | `Constant` | `R_T_m` | `6600405` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_2_K_1_K_2_Eqns_4_4_68_80` |
| `source (constant)` | `Constant4` | `T10_sec` | `286.3` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `Constant4` | `T10_sec` | `286.3` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `T_HSL` | `T_HSL_sec` | `5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_HSL_first_pass` |
| `source (constant)` | `T_HSL_sec` | `T_HSL_sec` | `5` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete` |
| `source (constant)` | `Constant4` | `Theta_T_deg` | `0` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Constant` | `V_T_mps` | `7780.67` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Constant1` | `V_T_mps` | `7780.67` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_HSL_first_pass` |
| `scale (gain)` | `Gain2` | `V_ex1_mps` | `4151` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_First_Phase` |
| `scale (gain)` | `Gain` | `V_ex1_mps` | `4151` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Intermediate_Parameters` |
| `scale (gain)` | `Gain1` | `V_ex1_mps` | `4151` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Intermediate_Parameters` |
| `scale (gain)` | `Gain3` | `V_ex1_mps` | `4151` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Intermediate_Parameters` |
| `source (constant)` | `Constant1` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `scale (gain)` | `Gain5` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Intermediate_Parameters` |
| `scale (gain)` | `Gain6` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Intermediate_Parameters` |
| `scale (gain)` | `Gain1` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete` |
| `scale (gain)` | `Gain` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `scale (gain)` | `Gain` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `scale (gain)` | `Gain5` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_1_K_3_K_4_Eqns_4_4_51_67` |
| `scale (gain)` | `Gain5` | `V_ex3_mps` | `4233.2` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/Position_Correction_Terms/Position_Correction_Terms_part_2_K_1_K_2_Eqns_4_4_68_80` |
| `source (constant)` | `Constant` | `delta_T_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/DeltaT_b` |
| `source (constant)` | `Constant1` | `delta_T_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_First_Phase` |
| `source (constant)` | `Constant2` | `delta_T_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_HSL_first_pass` |
| `source (constant)` | `Constant1` | `delta_T_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete` |
| `source (constant)` | `Constant1` | `delta_T_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `Constant1` | `delta_T_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |
| `source (constant)` | `Constant4` | `delta_T_sec` | `1.6` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations` |
| `source (constant)` | `epsilon_2_from_EDD` | `epsilon_2_sec` | `15` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_complete` |
| `source (constant)` | `Constant12` | `i_deg` | `31.605` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Constant13` | `lambda_0_deg` | `119` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `Constant9` | `lambda_L_deg` | `-80.561141` | `Saturn_IB_Stack/Initial_Conditions/MES_Transform` |
| `source (constant)` | `Constant9` | `lambda_L_deg` | `-80.561141` | `Saturn_IB_Stack/MES_Transform` |
| `source (constant)` | `Constant9` | `lambda_L_deg` | `-80.561141` | `Saturn_IB_Stack/On_Pad/Initial_Conditions/MES_Transform` |
| `source (constant)` | `m_liftoff_kg` | `m_liftoff_kg` | `585943` | `Saturn_IB_Stack/S_IB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `m_separation_kg` | `m_separation_kg` | `138709.90752311` | `Saturn_IB_Stack/S_IVB_Stage/Vehicle_Mass_Properties` |
| `source (constant)` | `Position_1_Azimuth_deg` | `pad_roll_L_deg` | `100` | `Saturn_IB_Stack/Initial_Conditions` |
| `source (constant)` | `Position_1_Azimuth_deg` | `pad_roll_L_deg` | `100` | `Saturn_IB_Stack/On_Pad/Initial_Conditions` |
| `source (constant)` | `Constant8` | `phi_L_deg` | `28.521963` | `Saturn_IB_Stack/Initial_Conditions/MES_Transform` |
| `source (constant)` | `Constant8` | `phi_L_deg` | `28.521963` | `Saturn_IB_Stack/MES_Transform` |
| `source (constant)` | `Constant8` | `phi_L_deg` | `28.521963` | `Saturn_IB_Stack/On_Pad/Initial_Conditions/MES_Transform` |
| `source (constant)` | `Constant11` | `phi_L_deg` | `28.521963` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode` |
| `source (constant)` | `phi_L_deg` | `phi_L_deg` | `28.521963` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations` |
| `source (constant)` | `phi_L_prime_deg` | `phi_L_prime_deg` | `28.360795` | `Saturn_IB_Stack/Initial_Conditions` |
| `source (constant)` | `phi_L_prime_deg` | `phi_L_prime_deg` | `28.360795` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_S_Frame_Position_Velocity_Calculations` |
| `source (constant)` | `Constant2` | `tau_1_0_sec` | `536` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_artificial_tau_mode` |
| `source (constant)` | `Constant2` | `tau_1_0_sec` | `536` | `Saturn_IB_Stack/Saturn_Instrument_Unit_IU/LVDA_LVDC/LVDC_Iterative_Guidance_Mode/IGM_Phase2_first_BML` |

## Review notes

- Compare non-trivial literals to TN-AP-67-158 (`AS205_presettings.m`) vs TM-X-62831 (`as205TmX62831Presettings.ts`).
- Unit-conversion scales (e.g. `deg2rad`, `in_to_m`) are usually intentional.
- Engine cant / tilt Euler constants and stage event times (`T_S*_sec`) are common TN leftovers.
- One `scale` uses string gain `1/V_ex3_mps` (symbolic); treated as hard-coded because `useParameter` is unset.

