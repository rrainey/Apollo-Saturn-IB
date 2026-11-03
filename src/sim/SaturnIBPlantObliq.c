/**
 * Obliq-backed Saturn IB plant step (200 Hz).
 *
 * Default backend: full `saturn_ib_stack` with LVDA_LVDC segregated_atomic.
 * Maps ExternalInputs ↔ saturn_ib_stack_rtw_step ↔ ExternalOutputs.
 */
#include "SaturnIBPlantObliq.h"

#include <stdlib.h>
#include <string.h>

#include "SaturnIBStackRtwAdapter.h"
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
  saturn_ib_stack_rtw_step(&plant->model, in, out);
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
  const LVDA_LVDC_t *lv = &m->LVDA_LVDC;

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
  return 0;
}
