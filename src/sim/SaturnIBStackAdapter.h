/**
 * Host-side ExtIn/ExtOut bridge for the obliq-2 saturn_ib_stack plant.
 * Uses the ExternalIO layout from SaturnIBExternalIO.h (shared by batch_sim
 * and apolloa).
 */
#ifndef SATURNIB_STACK_ADAPTER_H
#define SATURNIB_STACK_ADAPTER_H

#include "saturn_ib_stack.h"
#include "SaturnIBExternalIO.h"

#ifdef __cplusplus
extern "C" {
#endif

void saturn_ib_stack_apply_external_inputs(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in
);

void saturn_ib_stack_collect_external_outputs(
  const saturn_ib_stack_t *model,
  ExternalOutputs_saturn_ib_stack *out
);

/*
 * One-shot convenience: apply ExternalInputs → advance the model one tick →
 * collect ExternalOutputs. Prefer this over calling the three functions
 * above by hand unless you need to inspect model state mid-tick.
 */
void saturn_ib_stack_io_step(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
);

/*
 * Compatibility alias. obliq-2 cgen emits a standalone `smoke_main.c`
 * (under lib_SaturnIBObliq/plant/generated/) that calls this name; the
 * cgen contract is not ours to change. Our build never links smoke_main,
 * but this alias keeps the generated tree's standalone smoke target buildable.
 */
void saturn_ib_stack_rtw_step(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
);

/*
 * Tell the adapter whether the one-time integrator-IC reseed has
 * already run.  dskymatic-studio calls this with (1) right after
 * loading a save file so the next apply_external_inputs doesn't
 * clobber the restored integrator states with fresh pre-launch ICs.
 */
void saturn_ib_stack_set_ics_seeded(int seeded);
int  saturn_ib_stack_get_ics_seeded(void);

#ifdef __cplusplus
}
#endif

#endif /* SATURNIB_STACK_ADAPTER_H */
