/*
 * lvdc_dac.h — attitude-error command output pipeline (EDD §8.2.3).
 *
 * The LVDC sends attitude-error commands to the Flight Control
 * Computer through the LVDA D/A ladder. Per EDD §8.2.3, each
 * command is:
 *
 *   1. magnitude limited  (per-axis saturation);
 *   2. rate limited       (per-axis dchi/dt saturation);
 *   3. quantized          (0.0575 deg per LSB, Saturn V hw spec).
 *
 * The output is in radians. The caller converts to degrees for the
 * obliq-2 `outputs.Psi_deg` boundary port.
 *
 * Call rate: once per minor loop (40 ms during boost). The current
 * hand-edited body applies the rate limit at the fundamental step
 * (0.005 s) instead; the rewrite follows the same convention until
 * the scheduler owns the minor-loop cadence (Phase D).
 */

#ifndef LVDC_DAC_H
#define LVDC_DAC_H

#include "lvdc_internal.h"
#include "lvdc_params.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double psi_rad[3]; /* previous DAC output value */
} lvdc_dac_state_t;

/* Clear the DAC state to zero. */
void lvdc_dac_reset(lvdc_dac_state_t *s);

/* Apply rate limit, magnitude saturation, and D/A quantization.
 *
 *   s              DAC state (Psi from the previous call).
 *   p              mission preset parameters.
 *   psi_in_rad     raw attitude-error input, 3-vector in radians.
 *   dt_sec         time since the previous call.
 *   psi_out_rad    output: rate-limited + saturated + quantized.
 */
void lvdc_dac_step(lvdc_dac_state_t *s,
                   const lvdc_params_t *p,
                   const double psi_in_rad[3],
                   double dt_sec,
                   double psi_out_rad[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_DAC_H */
