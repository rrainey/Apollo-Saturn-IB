/**
 * Obliq-backed Saturn IB plant step (200 Hz).
 *
 * Default: full saturn_ib_stack with LVDA_LVDC segregated_atomic.
 * SaturnIBStack.cpp stays RTW-free; ExtIn/Out packing is in
 * SaturnIBStackRtwAdapter.
 */
#pragma once

#include "SaturnIBExternalIO.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SaturnIBPlantObliq_tag SaturnIBPlantObliq;

SaturnIBPlantObliq * SaturnIBPlantObliq_create(double dt_sec);
void SaturnIBPlantObliq_destroy(SaturnIBPlantObliq *plant);

/** One plant step. Reads *in, writes *out (may also mutate plant state). */
void SaturnIBPlantObliq_step(
  SaturnIBPlantObliq *plant,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
);

const char * SaturnIBPlantObliq_error(const SaturnIBPlantObliq *plant);

/**
 * Fill IGM interface sample from live plant signals (Layer-1 allowlist).
 * Requires igm_trace.h (batch-sim). Returns 0 on success, -1 if unavailable.
 */
struct IgmTraceSample_tag;
int SaturnIBPlantObliq_fill_igm_trace(
  const SaturnIBPlantObliq *plant,
  double t_sec,
  struct IgmTraceSample_tag *out
);

#ifdef __cplusplus
}
#endif
