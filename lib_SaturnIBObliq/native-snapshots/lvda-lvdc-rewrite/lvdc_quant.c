/*
 * lvdc_quant.c — LVDA sensor and D/A quantization helpers.
 */

#include "lvdc_quant.h"

#include <math.h>

double lvdc_quant_round(double x, double q) {
    if (q <= 0.0) {
        return x;
    }
    return q * floor(x / q + 0.5);
}

int64_t lvdc_quant_mps_to_pulses(double v_mps) {
    return (int64_t)llround(v_mps / LVDA_PIPA_MPS_PER_PULSE);
}

double lvdc_quant_pulses_to_mps(int64_t pulses) {
    return (double)pulses * LVDA_PIPA_MPS_PER_PULSE;
}
