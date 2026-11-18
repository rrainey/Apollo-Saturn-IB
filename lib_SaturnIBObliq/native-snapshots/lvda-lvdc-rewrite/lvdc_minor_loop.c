/*
 * lvdc_minor_loop.c — minor-loop attitude command generation
 * (EDD §8.3.1 Eq. 8.3.1).
 *
 * The rewrite uses a continuous-rate approximation of EDD Eq. 8.3.1.
 * The current body does the same; see the module header for the
 * EDD-faithful two-rate form.
 */

#include "lvdc_minor_loop.h"

#include <string.h>

void lvdc_minor_loop_reset(lvdc_minor_loop_state_t *s) {
    memset(s, 0, sizeof(*s));
}

void lvdc_minor_loop_step(lvdc_minor_loop_state_t *s,
                          const lvdc_params_t *p,
                          const double chi_cmd_rad[3],
                          double dt_sec,
                          double chi_prime_out[3]) {
    for (int i = 0; i < 3; i++) {
        /* Desired increment this step. */
        const double delta_desired = chi_cmd_rad[i] - s->chi_prime_rad[i];

        /* Saturate at ±(rate_limit · dt). The sign-preserving clamp
         * is EDD §8.3.1 Eq. 8.3.1 restriction. */
        const double delta_max = p->chi_rate_limit_radps[i] * dt_sec;
        double delta = delta_desired;
        if (delta >  delta_max) delta =  delta_max;
        if (delta < -delta_max) delta = -delta_max;

        s->chi_prime_rad[i] += delta;
        chi_prime_out[i] = s->chi_prime_rad[i];
    }
}
