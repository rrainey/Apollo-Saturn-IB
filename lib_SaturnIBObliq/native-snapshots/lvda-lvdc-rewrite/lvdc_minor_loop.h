/*
 * lvdc_minor_loop.h — minor-loop attitude command generation
 * (EDD §8.3.1 Eq. 8.3.1).
 *
 * The LVDC produces the "chi prime" (`chi'`) minor-loop attitude
 * command at 25 Hz (40 ms minor loop) during boost. Minor loop
 * support (MLS) runs at the BML rate (1.6 s) and resamples the
 * guidance command `chi`. Between MLS cycles each minor loop adds
 * a fixed increment:
 *
 *   On the MLS edge (fresh guidance sample):
 *     Delta_chi'_i = (chi_i - chi'_i_prev) / N
 *     Delta_chi'_i = sat(Delta_chi'_i, Delta_chi'_i_max)
 *   On each minor loop (including the MLS edge):
 *     chi'_i_new = chi'_i_prev + Delta_chi'_i
 *
 * `N` is the number of minor loops per MLS cycle. EDD §15 MS25DT
 * defines N = 40 during boost (40 ms minor loop, 1.6 s BML). The
 * magnitude limit on Delta_chi' provides rate limiting; the /N
 * divisor interpolates the staircase guidance command into a smooth
 * ramp between BMLs. Without the divisor chi' is a ZOH of chi and
 * the plant chases a 1.6 s staircase, which produces a BML-rate
 * residual in Psi (EDD §8.3.1 last paragraph).
 *
 * Phase-dependent N: IGM only updates chi at MLS cadence so the
 * caller passes N = 40 and fresh_mls = at_BML. Pre-IGM time tilt
 * re-evaluates the chi_y polynomial every minor loop and EDD
 * §4.4.1.2 requires chi' to stay within ±0.1° of the polynomial;
 * the /40 lag violates that spec at AS-205 pitch rates. The caller
 * passes N = 1 with fresh_mls = true every minor during pre-IGM so
 * chi' catches up each minor, clamped by the 1 deg/s magnitude
 * limit.
 */

#ifndef LVDC_MINOR_LOOP_H
#define LVDC_MINOR_LOOP_H

#include <stdbool.h>

#include "lvdc_internal.h"
#include "lvdc_params.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double chi_prime_rad[3];       /* previous minor-loop attitude command */
    double dchi_per_minor_rad[3];  /* cached Delta_chi' from the latest MLS */
    bool   mag_limited[3];         /* true if the cached Delta_chi' hit the rate limit (EDD §8.3.1 SMC inhibit) */
} lvdc_minor_loop_state_t;

/* Clear the minor-loop state to zero. */
void lvdc_minor_loop_reset(lvdc_minor_loop_state_t *s);

/* Advance `chi'` toward `chi_cmd_rad`.
 *
 *   s              minor-loop state (chi' + cached Delta_chi').
 *   p              mission preset parameters (reads chi_rate_limit_radps).
 *   chi_cmd_rad    current guidance command vector.
 *   fresh_mls      true on the minor tick that coincides with an MLS
 *                  (BML) cycle. On that tick Delta_chi' is recomputed
 *                  from (chi - chi') / minors_per_mls and clamped.
 *   minors_per_mls number of minor loops per MLS cycle (N in EDD
 *                  Eq. 8.3.1). Nominally 40. If <=0 the step acts as
 *                  a plain rate limiter (legacy behavior).
 *   dt_sec         time since the previous call in seconds.
 *   chi_prime_out  output: new chi' values. May alias chi_cmd_rad.
 */
void lvdc_minor_loop_step(lvdc_minor_loop_state_t *s,
                          const lvdc_params_t *p,
                          const double chi_cmd_rad[3],
                          bool fresh_mls,
                          int minors_per_mls,
                          double dt_sec,
                          double chi_prime_out[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_MINOR_LOOP_H */
