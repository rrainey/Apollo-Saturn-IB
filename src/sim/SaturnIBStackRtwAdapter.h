/**
 * RTW-shaped ExtIn/ExtOut bridge for the segregated_atomic full stack.
 * Uses host ExternalIO layout from SaturnIBExternalIO.h (batch_sim / ApolloA).
 */
#ifndef SATURNIB_STACK_RTW_ADAPTER_H
#define SATURNIB_STACK_RTW_ADAPTER_H

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

void saturn_ib_stack_rtw_step(
  saturn_ib_stack_t *model,
  const ExternalInputs_saturn_ib_stack *in,
  ExternalOutputs_saturn_ib_stack *out
);

#ifdef __cplusplus
}
#endif

#endif /* SATURNIB_STACK_RTW_ADAPTER_H */
