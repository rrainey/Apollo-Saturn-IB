/*
 * lvdc_minor_loop.c — minor-loop attitude command generation
 * (EDD §8.3.1 Eq. 8.3.1).
 */

#include "lvdc_minor_loop.h"

#include <string.h>

void lvdc_minor_loop_reset(lvdc_minor_loop_state_t *s) {
    memset(s, 0, sizeof(*s));
}

void lvdc_minor_loop_step(lvdc_minor_loop_state_t *s,
                          const lvdc_params_t *p,
                          const double chi_cmd_rad[3],
                          bool fresh_mls,
                          int minors_per_mls,
                          double dt_sec,
                          double chi_prime_out[3]) {
    const double delta_max_axis[3] = {
        p->chi_rate_limit_radps[0] * dt_sec,
        p->chi_rate_limit_radps[1] * dt_sec,
        p->chi_rate_limit_radps[2] * dt_sec,
    };

    /* On the MLS edge, resample the guidance command and compute
     * a per-minor increment. EDD §8.3.1: Δχ' = (χ − χ') / N, with
     * magnitude limit. The limit flag feeds the SMC-inhibit rule
     * from the same section. */
    if (fresh_mls) {
        for (int i = 0; i < 3; i++) {
            double dchi;
            if (minors_per_mls > 0) {
                dchi = (chi_cmd_rad[i] - s->chi_prime_rad[i])
                     / (double)minors_per_mls;
            } else {
                /* Legacy fallback: catch up as fast as the rate
                 * limit allows (no interpolation). */
                dchi = chi_cmd_rad[i] - s->chi_prime_rad[i];
            }
            const double dmax = delta_max_axis[i];
            s->mag_limited[i] = (dchi >  dmax) || (dchi < -dmax);
            if (dchi >  dmax) dchi =  dmax;
            if (dchi < -dmax) dchi = -dmax;
            s->dchi_per_minor_rad[i] = dchi;
        }
    }

    /* Each minor loop (including the MLS edge) applies the cached
     * increment. We also re-clamp here in case the rate limit
     * changes mid-cycle. */
    for (int i = 0; i < 3; i++) {
        double dchi = s->dchi_per_minor_rad[i];
        const double dmax = delta_max_axis[i];
        if (dchi >  dmax) dchi =  dmax;
        if (dchi < -dmax) dchi = -dmax;
        s->chi_prime_rad[i] += dchi;
        chi_prime_out[i] = s->chi_prime_rad[i];
    }
}
