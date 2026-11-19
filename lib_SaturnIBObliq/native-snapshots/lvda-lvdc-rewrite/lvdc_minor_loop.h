/*
 * lvdc_minor_loop.h — minor-loop attitude command generation
 * (EDD §8.3.1 Eq. 8.3.1).
 *
 * The LVDC produces the "chi prime" (`chi'`) minor-loop attitude
 * command at 25 Hz (40 ms minor loop) during boost by ramping toward
 * the guidance command `chi` at a limited rate. The ramp prevents
 * actuator slew from exceeding vehicle control authority when the
 * guidance command jumps between BMLs.
 *
 * Authoritative form (EDD §8.3.1 Eq. 8.3.1):
 *
 *   Delta_chi'_i = (chi_i - chi'_i_prev) / N
 *   Delta_chi'_i = sat(Delta_chi'_i, Delta_chi'_i_max)
 *   chi'_i_new   = chi'_i_prev + Delta_chi'_i
 *
 * where `N` is the number of minor loops between minor-loop-support
 * cycles (nominally 40 at a 40 ms minor loop and a 1.6 s BML).
 *
 * The current hand-edited body uses a simpler continuous-rate
 * limiter at the fundamental step. For AS-205 the two forms converge
 * to the same steady-state value and differ only in the first-tick
 * transient after a BML update. The rewrite follows the simpler
 * continuous-rate form (see Rev B note in lvdc_minor_loop.c).
 */

#ifndef LVDC_MINOR_LOOP_H
#define LVDC_MINOR_LOOP_H

#include "lvdc_internal.h"
#include "lvdc_params.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double chi_prime_rad[3]; /* previous minor-loop attitude command */
} lvdc_minor_loop_state_t;

/* Clear the minor-loop state to zero. */
void lvdc_minor_loop_reset(lvdc_minor_loop_state_t *s);

/* Advance `chi'` toward `chi_cmd_rad` with per-axis rate limiting.
 *
 *   s              minor-loop state (chi' from the previous call).
 *   p              mission preset parameters (reads chi_rate_limit_radps).
 *   chi_cmd_rad    current guidance command vector.
 *   dt_sec         time since the previous call in seconds.
 *   chi_prime_out  output: new chi' values. May alias chi_cmd_rad.
 */
void lvdc_minor_loop_step(lvdc_minor_loop_state_t *s,
                          const lvdc_params_t *p,
                          const double chi_cmd_rad[3],
                          double dt_sec,
                          double chi_prime_out[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_MINOR_LOOP_H */
