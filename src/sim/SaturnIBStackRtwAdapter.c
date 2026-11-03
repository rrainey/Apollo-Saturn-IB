/**
 * Apply ExtIn → step → collect ExtOut for saturn_ib_stack (LVDA_LVDC segregated).
 * OUT11 packing matches SaturnIBStack.cpp / batch_sim RTW layout:
 *   [0..2] lat,lon,h  [3..5] Xe  [6..8] Ve  [9..11] IMU stub
 *   [12] alpha  [13] |Vb|  [14..17] q  [18..20] omega_b  [21..23] Vb  [24] mass
 */
#include "SaturnIBStackRtwAdapter.h"

#include <math.h>
#include <string.h>

#ifndef SATURNIB_RTW_A_M
#define SATURNIB_RTW_A_M 6378136.3
#endif
#ifndef SATURNIB_WGS84_E2
#define SATURNIB_WGS84_E2 6.6943800042608068e-3
#endif

static int saturn_ib_stack_ics_seeded = 0;

enum {
  OUT11_lat_deg = 0,
  OUT11_lon_deg = 1,
  OUT11_h_m = 2,
  OUT11_Xe_x_m = 3,
  OUT11_Xe_y_m = 4,
  OUT11_Xe_z_m = 5,
  OUT11_Ve_x_mps = 6,
  OUT11_Ve_y_mps = 7,
  OUT11_Ve_z_mps = 8,
  OUT11_IMU_theta_x_deg = 9,
  OUT11_IMU_theta_y_deg = 10,
  OUT11_IMU_theta_z_deg = 11,
  OUT11_alpha_deg = 12,
  OUT11_Vb_mps = 13,
  OUT11_q_ECI_q0 = 14,
  OUT11_q_ECI_q1 = 15,
  OUT11_q_ECI_q2 = 16,
  OUT11_q_ECI_q3 = 17,
  OUT11_omega_b_x_rps = 18,
  OUT11_omega_b_y_rps = 19,
  OUT11_omega_b_z_rps = 20,
  OUT11_Vb_x_mps = 21,
  OUT11_Vb_y_mps = 22,
  OUT11_Vb_z_mps = 23,
  OUT11_compare_c3 = 24
};

static double n3(const double *v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

/**
 * ECI → geodetic lat/lon/h (WGS-84 e2, RTW a).
 * lon = atan2(y,x) − θ_GMST(t) with live sidereal angle at the sample
 * (matches plant ECI_to_LLA: −Earth_s_Rotation_Sum + atan2).
 * Passing gmst_rad=0 yields ECI right ascension — do not use that for OUT11.
 */
static void eci_to_lla(
  double x, double y, double z, double gmst_rad,
  double *lat_deg, double *lon_deg, double *h_m
) {
  const double a = SATURNIB_RTW_A_M;
  const double e2 = SATURNIB_WGS84_E2;
  /* Rotate ECI → ECEF-like for lat/h: RotZ(+GMST)·Xe */
  const double c = cos(gmst_rad);
  const double s = sin(gmst_rad);
  const double x_e = c * x - s * y;
  const double y_e = s * x + c * y;
  const double z_e = z;
  double lon = atan2(y, x) - gmst_rad;
  /* Wrap to (−π, π] */
  if (lon > M_PI) lon -= 2.0 * M_PI;
  if (lon <= -M_PI) lon += 2.0 * M_PI;
  const double p = sqrt(x_e * x_e + y_e * y_e);
  double lat = atan2(z_e, p * (1.0 - e2));
  for (int k = 0; k < 8; k++) {
    const double sin_lat = sin(lat);
    const double N = a / sqrt(1.0 - e2 * sin_lat * sin_lat);
    lat = atan2(z_e + e2 * N * sin_lat, p);
  }
  {
    const double sin_lat = sin(lat);
    const double N = a / sqrt(1.0 - e2 * sin_lat * sin_lat);
    *h_m = p / cos(lat) - N;
  }
  *lat_deg = lat * (180.0 / M_PI);
  *lon_deg = lon * (180.0 / M_PI);
}

void saturn_ib_stack_apply_external_inputs(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in
) {
  if (!model || !in) return;
  for (int i = 0; i < 6; i++) model->inputs.LaunchDate[i] = in->LaunchDate[i];
  model->inputs.A_z_deg = in->A_z_deg;
  for (int i = 0; i < 3; i++) model->inputs.CG_LLA_deg_m[i] = in->CG_LLA_deg_m[i];
  for (int i = 0; i < 4; i++) model->inputs.q_ECItoSM[i][0] = in->q_ECItoSM[i];
  model->inputs.T_L_prime_sec = in->T_L_prime_sec;

  /* One-time IC reseed after inputs are live (not every step). */
  if (!saturn_ib_stack_ics_seeded) {
    saturn_ib_stack_evaluate_algebraic(model);
    saturn_ib_stack_reseed_integrator_ics(model);
    saturn_ib_stack_evaluate_algebraic(model);
    saturn_ib_stack_ics_seeded = 1;
  }
}

void saturn_ib_stack_collect_external_outputs(
  const saturn_ib_stack_t *model,
  ExternalOutputs_saturn_ib_stack *out
) {
  if (!model || !out) return;
  memset(out, 0, sizeof(*out));

  out->FDAI_Roll_deg = model->outputs.FDAI_Roll_deg;
  out->FDAI_Pitch_deg = model->outputs.FDAI_Pitch_deg;
  out->FDAI_Yaw_deg = model->outputs.FDAI_Yaw_deg;
  out->FDAI_RollDot_dps = model->outputs.FDAI_RollDot_dps;
  out->FDAI_PitchDot_dps = model->outputs.FDAI_PitchDot_dps;
  out->FDAI_YawDot_dps = model->outputs.FDAI_YawDot_dps;
  for (int i = 0; i < 3; i++) out->CM_IMU_PIPA_Pulses[i] = model->outputs.CM_IMU_PIPA_Pulses[i];
  out->CM_IMU_IGA_rad = model->outputs.CM_IMU_IGA_rad;
  out->CM_IMU_MGA_Rad = model->outputs.CM_IMU_MGA_Rad;
  out->CM_IMU_OGA_rad = model->outputs.CM_IMU_OGA_rad;

  out->bStageSep = model->outputs.bStageSep ? 1 : 0;
  out->bIECO = model->outputs.bIECO ? 1 : 0;
  out->bOECO = model->outputs.bOECO ? 1 : 0;
  out->bS_IVB_EngineStart = model->outputs.bS_IVB_EngineStart ? 1 : 0;
  out->bLiftoff = (model->outputs.bLiftoff != 0.0) ? 1 : 0;
  out->BodyToSM_Phi_deg = model->outputs.BodyToSM_Phi_deg;
  out->BodyToSM_Theta_deg = model->outputs.BodyToSM_Theta_deg;
  out->BodyToSM_Psi_deg = model->outputs.BodyToSM_Psi_deg;

  for (int i = 0; i < 9; i++) out->OUT22[i] = model->outputs.Out22[i];
  for (int i = 0; i < 4; i++) out->veh_q_ECI[i] = model->outputs.veh_q_ECI[i][0];

  {
    const double *xe_ivb = model->signals.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze;
    const double *ve_ivb =
      model->signals.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_transform_to_Inertial_axes_Product;
    const double *xe_ib = model->signals.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_xe_ye_ze;
    const double *ve_ib =
      model->signals.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_transform_to_Inertial_axes_Product;
    const double *vb_ib = model->signals.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb;
    const double *vb_ivb = model->signals.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb;
    const double *om_ib = model->signals.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r;
    const double *om_ivb = model->signals.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_p_q_r;
    const double *xe_ic = model->signals.Initial_Conditions_Product;

    double rib = n3(xe_ib);
    double rivb = n3(xe_ivb);
    int use_ivb = out->bStageSep && rivb > 1.0;
    const double *xe = use_ivb ? xe_ivb : (rib > 1.0 ? xe_ib : xe_ic);
    const double *ve = use_ivb ? ve_ivb : ve_ib;
    const double *vb = use_ivb ? vb_ivb : vb_ib;
    const double *om = use_ivb ? om_ivb : om_ib;

    /*
     * LLA via live sidereal angle at the sample:
     *   Earth_s_Rotation_Sum = θ_GMST(t)  (same signal plant ECI_to_LLA uses)
     *   lon = atan2(y,x) − θ_GMST(t), wrapped to (−180, 180]
     *
     * Do NOT use plant Angle_Conversion4 for OUT packing: its wrap Bias2 is
     * codegen'd as `Add1+0` (should be +2π), so lon escapes to ~−400° once
     * RA−GMST ≤ −π. Prefer plant lat/h when the stage ECI_to_LLA is live.
     */
    double lat = 0.0, lon = 0.0, h_geo = 0.0;
    const double gmst_rad = model->signals.Earth_s_Rotation_Sum;
    eci_to_lla(xe[0], xe[1], xe[2], gmst_rad, &lat, &lon, &h_geo);

    double lat_plant = use_ivb ? model->signals.S_IVB_Stage_Angle_Conversion5
                               : model->signals.S_IB_Stage_Angle_Conversion5;
    double h_plant = use_ivb ? model->signals.S_IVB_Stage_ECI_to_LLA_Add17
                             : model->signals.S_IB_Stage_ECI_to_LLA_Add17;

    int plant_lla_live = use_ivb
      ? (model->enable_states.S_IVB_Stage_enabled && isfinite(lat_plant))
      : (rib > 1.0 && model->enable_states.S_IB_Stage_enabled && isfinite(lat_plant));

    if (plant_lla_live) {
      lat = lat_plant;
    }
    double h = (plant_lla_live && isfinite(h_plant)) ? h_plant : h_geo;
    if (!isfinite(h)) h = h_geo;

    out->OUT11[OUT11_lat_deg] = lat;
    out->OUT11[OUT11_lon_deg] = lon;
    out->OUT11[OUT11_h_m] = h;
    out->OUT11[OUT11_Xe_x_m] = xe[0];
    out->OUT11[OUT11_Xe_y_m] = xe[1];
    out->OUT11[OUT11_Xe_z_m] = xe[2];
    out->OUT11[OUT11_Ve_x_mps] = ve[0];
    out->OUT11[OUT11_Ve_y_mps] = ve[1];
    out->OUT11[OUT11_Ve_z_mps] = ve[2];
    out->OUT11[OUT11_IMU_theta_x_deg] = 0.0;
    out->OUT11[OUT11_IMU_theta_y_deg] = 0.0;
    out->OUT11[OUT11_IMU_theta_z_deg] = 0.0;
    out->OUT11[OUT11_alpha_deg] = 0.0;
    out->OUT11[OUT11_Vb_mps] = n3(vb);
    out->OUT11[OUT11_omega_b_x_rps] = om[0];
    out->OUT11[OUT11_omega_b_y_rps] = om[1];
    out->OUT11[OUT11_omega_b_z_rps] = om[2];
    out->OUT11[OUT11_Vb_x_mps] = vb[0];
    out->OUT11[OUT11_Vb_y_mps] = vb[1];
    out->OUT11[OUT11_Vb_z_mps] = vb[2];

    {
      double qn = fabs(out->veh_q_ECI[0]) + fabs(out->veh_q_ECI[1]) +
                  fabs(out->veh_q_ECI[2]) + fabs(out->veh_q_ECI[3]);
      if (qn < 1e-12) {
        const double(*q)[1] =
          use_ivb
            ? model->signals.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3
            : model->signals.S_IB_Stage_Custom_Variable_Mass_6DoF_Quaternion_Calculate_DCM_Euler_Angles_q0_q1_q2_q3;
        out->veh_q_ECI[0] = q[0][0];
        out->veh_q_ECI[1] = q[1][0];
        out->veh_q_ECI[2] = q[2][0];
        out->veh_q_ECI[3] = q[3][0];
      }
      out->OUT11[OUT11_q_ECI_q0] = out->veh_q_ECI[0];
      out->OUT11[OUT11_q_ECI_q1] = out->veh_q_ECI[1];
      out->OUT11[OUT11_q_ECI_q2] = out->veh_q_ECI[2];
      out->OUT11[OUT11_q_ECI_q3] = out->veh_q_ECI[3];
    }

    out->OUT11[OUT11_compare_c3] =
      use_ivb ? model->signals.S_IVB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg
              : model->signals.S_IB_Stage_Vehicle_Mass_Properties_vehicle_mass_kg;
  }

  /* OUT12: mirror active stage into s2_* after sep (host expects non-zero post-sep). */
  if (out->bStageSep) {
    for (int i = 0; i < 24; i++) out->OUT12[i] = out->OUT11[i];
  }
}

void saturn_ib_stack_rtw_step(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
) {
  saturn_ib_stack_apply_external_inputs(model, in);
  saturn_ib_stack_step(model);
  saturn_ib_stack_collect_external_outputs(model, out);
}
