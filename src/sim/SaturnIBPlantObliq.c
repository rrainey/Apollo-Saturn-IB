/**
 * Obliq-backed Saturn IB plant step (200 Hz).
 *
 * Default backend: full `saturn_ib_stack` with LVDA_LVDC segregated_atomic.
 * Maps ExternalInputs ↔ saturn_ib_stack_io_step ↔ ExternalOutputs.
 */
#include "SaturnIBPlantObliq.h"

#include <stdlib.h>
#include <string.h>

#include "SaturnIBStackAdapter.h"
#include "saturn_ib_stack.h"
#include "igm_trace.h"

struct SaturnIBPlantObliq_tag {
  double dt_sec;
  double time_sec;
  const char *error;
  saturn_ib_stack_t model;
  int model_inited;
};

SaturnIBPlantObliq * SaturnIBPlantObliq_create(double dt_sec)
{
  SaturnIBPlantObliq *p = (SaturnIBPlantObliq *)calloc(1, sizeof(*p));
  if (!p) return NULL;
  p->dt_sec = dt_sec > 0.0 ? dt_sec : 0.005;
  p->time_sec = 0.0;
  p->error = NULL;
  saturn_ib_stack_init(&p->model, p->dt_sec);
  p->model_inited = 1;
  return p;
}

void SaturnIBPlantObliq_destroy(SaturnIBPlantObliq *plant)
{
  if (!plant) return;
  free(plant);
}

const char * SaturnIBPlantObliq_error(const SaturnIBPlantObliq *plant)
{
  return plant ? plant->error : "null plant";
}

void SaturnIBPlantObliq_step(
  SaturnIBPlantObliq *plant,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
)
{
  if (!plant || !in || !out) return;
  saturn_ib_stack_io_step(&plant->model, in, out);
  plant->time_sec = plant->model.time;
}

int SaturnIBPlantObliq_fill_igm_trace(
  const SaturnIBPlantObliq *plant,
  double t_sec,
  struct IgmTraceSample_tag *out
)
{
  /* Silver: map LVDA_LVDC data stores / cutoff discrete for HSL forensics. */
  if (!plant || !out || !plant->model_inited) return -1;
  igm_trace_clear(out);
  out->t_sec = t_sec;

  const saturn_ib_stack_t *m = &plant->model;
  /* obliq-2 3.8.3+ qualifies segregated-subsystem instance fields with the
     parent path. The LVDA_LVDC sits under IU/LVDA_LVDC in the model. */
  const LVDA_LVDC_t *lv = &m->Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDA_LVDC;

  out->igm_enable = lv->enable_states.LVDC_Iterative_Guidance_Mode_enabled ? 1.0 : 0.0;
  out->T_1_i_sec =
    lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_1_i_sec;
  out->T_3_i_sec =
    lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_T_3_i_sec;
  out->T_3_eff_sec = lv->signals.LVDC_Iterative_Guidance_Mode_Add8;
  out->nIGMMode =
    (double)lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nIGMMode;
  out->nHSLActive =
    lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_nHSLActive;
  out->bCutoff = lv->outputs.bSIVCutoff ? 1.0 : 0.0;
  out->Chi_Y_deg = lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Y_deg;
  out->Chi_Z_deg = lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_Chi_Z_deg;
  out->SMCY_rad =
    lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCY_rad;
  out->SMCZ_rad =
    lv->data_stores.Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDC_Iterative_Guidance_Mode_SMCZ_rad;

  /* LVDC S-frame state vectors (sensor-path view, Eqns 4.3.1-5).
     Product18 = X_S, Product6 = V_S. */
  {
    const double *xs = lv->signals.LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_Product18;
    const double *vs = lv->signals.LVDC_S_Frame_Position_Velocity_Calculations_Position_and_Velocity_Calculation_S_Frame_Eqns_4_3_1_5_Product6;
    out->XS_x = xs[0]; out->XS_y = xs[1]; out->XS_z = xs[2];
    out->VS_x = vs[0]; out->VS_y = vs[1]; out->VS_z = vs[2];
  }

  /* HSL entry inputs (EDD §4.4.2.4). vigained = V_T − |V|_S. */
  out->V_S_mag_mps = lv->signals.LVDC_Iterative_Guidance_Mode_Math_Function3;
  out->vigained_mps = PARAM_V_T_mps - out->V_S_mag_mps;
  out->T_HSL_bias_sec =
    lv->signals.LVDC_Iterative_Guidance_Mode_IGM_Phase2_artificial_tau_complete_Sum;

  out->V_m_raw_x = lv->inputs.V_m_bar_mps[0];
  out->V_m_raw_y = lv->inputs.V_m_bar_mps[1];
  out->V_m_raw_z = lv->inputs.V_m_bar_mps[2];

  {
    extern int    s_hsl_V_samples;
    extern double s_hsl_V_curr, s_hsl_V_prev, s_hsl_V_prev2;
    out->hsl_V_samples = (double)s_hsl_V_samples;
    out->hsl_V_curr = s_hsl_V_curr;
    out->hsl_V_prev = s_hsl_V_prev;
    out->hsl_V_prev2 = s_hsl_V_prev2;
  }

  {
    extern double s_V_Sg[3], s_G_S_prev[3];
    out->V_Sg_x = s_V_Sg[0]; out->V_Sg_y = s_V_Sg[1]; out->V_Sg_z = s_V_Sg[2];
    out->G_S_prev_x = s_G_S_prev[0]; out->G_S_prev_y = s_G_S_prev[1]; out->G_S_prev_z = s_G_S_prev[2];
  }

  out->PAD_Ve_x = lv->inputs.PAD_Ve_mps[0];
  out->PAD_Ve_y = lv->inputs.PAD_Ve_mps[1];
  out->PAD_Ve_z = lv->inputs.PAD_Ve_mps[2];
  out->PAD_Xe_x = lv->inputs.PAD_Xe_m[0];
  out->PAD_Xe_y = lv->inputs.PAD_Xe_m[1];
  out->PAD_Xe_z = lv->inputs.PAD_Xe_m[2];
  for (int i = 0; i < 3; i++) {
    double v = 0.0, x = 0.0;
    for (int k = 0; k < 3; k++) {
      v += lv->inputs.MES_DCM[i][k] * lv->inputs.PAD_Ve_mps[k];
      x += lv->inputs.MES_DCM[i][k] * lv->inputs.PAD_Xe_m[k];
    }
    (&out->MES_PAD_Ve_x)[i] = v;
    (&out->MES_PAD_Xe_x)[i] = x;
  }
  return 0;
}
