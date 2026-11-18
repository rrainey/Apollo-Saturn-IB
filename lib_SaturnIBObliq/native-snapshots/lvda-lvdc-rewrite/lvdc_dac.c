/*
 * lvdc_dac.c — attitude-error command output pipeline (EDD §8.2.3).
 */

#include "lvdc_dac.h"

#include "lvdc_quant.h"

#include <string.h>

void lvdc_dac_reset(lvdc_dac_state_t *s) {
    memset(s, 0, sizeof(*s));
}

void lvdc_dac_step(lvdc_dac_state_t *s,
                   const lvdc_params_t *p,
                   const double psi_in_rad[3],
                   double dt_sec,
                   double psi_out_rad[3]) {
    for (int i = 0; i < 3; i++) {
        /* 1. Rate limit per EDD §8.2.3 ("before each attitude error
         * command is transmitted to the LVDA ladders, it is ...
         * rate limited"). The current body applies this at the
         * fundamental step. */
        const double delta_desired = psi_in_rad[i] - s->psi_rad[i];
        const double delta_max     = p->psi_rate_limit_radps * dt_sec;
        double delta = delta_desired;
        if (delta >  delta_max) delta =  delta_max;
        if (delta < -delta_max) delta = -delta_max;
        double psi = s->psi_rad[i] + delta;

        /* 2. Magnitude saturation per EDD §8.2.3 ("magnitude and
         * rate limited"). */
        const double mag_lim = p->psi_mag_limit_rad;
        if (psi >  mag_lim) psi =  mag_lim;
        if (psi < -mag_lim) psi = -mag_lim;

        /* 3. D/A quantization. The output is in radians but the
         * LSB is defined in degrees (0.0575 deg per Saturn V hw
         * spec). Round to the nearest integer multiple of
         * dac_lsb_rad. */
        psi = lvdc_quant_round(psi, p->dac_lsb_rad);

        s->psi_rad[i] = psi;
        psi_out_rad[i] = psi;
    }
}
