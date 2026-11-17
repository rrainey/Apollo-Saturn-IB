/**
 * Apply ExtIn → step → collect ExtOut for saturn_ib_stack (LVDA_LVDC segregated).
 *
 * OUT11 and OUT12 come from the plant's native Mux / Mux1 packs (per MDL
 * design intent): OUT11 = S-IB stage, OUT12 = S-IVB stage.  Both are
 * independent across the whole flight — pre-sep, OUT12 carries the S-IVB
 * integrator state (which the plant holds in a disabled state until
 * `Stage_Sep_3`); post-sep, OUT11 continues to track the spent S-IB on
 * its ballistic arc.  Pre-liftoff, the plant's stage ECI_to_LLA chain
 * returns NaN — consumers should use OUT22 until bLiftoff goes true.
 *
 * Model fixes that landed alongside this simplification:
 *  - rrainey/Apollo-Saturn-IB#1: S-IB xe_ye_ze IC now sourced from
 *    On_Pad.Xe_m (live rotating pad ECI), eliminating the ω_E·T_L' = 1.25°
 *    longitude offset in the pre-/post-liftoff trajectory.
 *  - ECI_to_LLA.Bias2 blocks in On_Pad, S_IB_Stage and S_IVB_Stage were
 *    emitting lon wraps of `+0` instead of `+2π`.  Fixed in the model.
 */
#include "SaturnIBStackAdapter.h"

#include <math.h>
#include <string.h>

/* Fischer 1960 ellipsoid (Apollo-program standard per NASA MSC MA-1461).
 * See docs/ALTITUDE_CONVENTIONS.md. */
#define SATIB_FISCHER_A_M 6378165.0
#define SATIB_FISCHER_B_M 6356783.0

/* Altitude "along the geocentric position vector", per TM-X-62831 symbol
 * glossary: subtract the Fischer ellipsoid's radius at the position's
 * geocentric latitude from |Xe|.  Pure postprocessing; no state. */
static double saturn_ib_stack_fischer_alt_m(double x, double y, double z) {
    const double a = SATIB_FISCHER_A_M, b = SATIB_FISCHER_B_M;
    const double r2 = x*x + y*y + z*z;
    if (!(r2 > 0.0)) return 0.0;
    const double r  = sqrt(r2);
    const double sin_phi_c = z / r;
    const double cos_phi_c = sqrt(x*x + y*y) / r;
    const double denom = sqrt((a*sin_phi_c)*(a*sin_phi_c) +
                              (b*cos_phi_c)*(b*cos_phi_c));
    if (!(denom > 0.0)) return 0.0;
    return r - (a * b) / denom;
}

static int saturn_ib_stack_ics_seeded = 0;

void saturn_ib_stack_set_ics_seeded(int seeded) {
    saturn_ib_stack_ics_seeded = seeded ? 1 : 0;
}
int saturn_ib_stack_get_ics_seeded(void) {
    return saturn_ib_stack_ics_seeded;
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

  /* Native packs from the plant: OUT11 = S-IB stage, OUT12 = S-IVB stage.
   * Plant's Out11 has 25 elements matching the OUT11 layout exactly.
   * Plant's Out12 has 22 elements (layout matches OUT12[0..20] plus one
   * tail element); the S-IVB body-velocity vector [21..23] is not yet
   * packed by the plant's Mux1 — fill it from the integrator signal here
   * pending a model-side Mux1 extension. */
  for (int i = 0; i < 25; i++) out->OUT11[i] = model->outputs.Out11[i];
  for (int i = 0; i < 21; i++) out->OUT12[i] = model->outputs.Out12[i];
  {
    const double *vb_ivb =
      model->signals.S_IVB_Stage_Custom_Variable_Mass_6DoF_Quaternion_ub_vb_wb;
    out->OUT12[21] = vb_ivb[0];
    out->OUT12[22] = vb_ivb[1];
    out->OUT12[23] = vb_ivb[2];
  }

  /* Fischer-1960 geocentric-radial altitude, trailing slot of each pack.
   * For OUT22 (pad), use the pad Xe at indices [3..5].  For OUT11/OUT12
   * (stages) use the integrator Xe at the same indices; pre-liftoff these
   * are zero/NaN and the Fischer altitude is correspondingly meaningless
   * — consumers should respect the same bLiftoff gate they already use
   * for the stage lat/lon/h. */
  out->OUT22[9]  = saturn_ib_stack_fischer_alt_m(out->OUT22[3],
                                                 out->OUT22[4],
                                                 out->OUT22[5]);
  out->OUT11[25] = saturn_ib_stack_fischer_alt_m(out->OUT11[3],
                                                 out->OUT11[4],
                                                 out->OUT11[5]);
  out->OUT12[24] = saturn_ib_stack_fischer_alt_m(out->OUT12[3],
                                                 out->OUT12[4],
                                                 out->OUT12[5]);
}

void saturn_ib_stack_io_step(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
) {
  saturn_ib_stack_apply_external_inputs(model, in);
  saturn_ib_stack_step(model);
  saturn_ib_stack_collect_external_outputs(model, out);
}

/*
 * Compatibility alias: obliq-2 cgen emits smoke_main.c calling the historical
 * name. Our host build never links smoke_main, but the generated tree's
 * standalone smoke target expects this symbol.
 */
void saturn_ib_stack_rtw_step(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
) {
  saturn_ib_stack_io_step(model, in, out);
}
