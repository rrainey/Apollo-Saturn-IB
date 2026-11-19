/*
 * lvdc_quant.h — LVDA sensor and D/A quantization helpers.
 *
 * See docs/LVDC_QUANTIZATION_INVENTORY.md for every value's source.
 */

#ifndef LVDC_QUANT_H
#define LVDC_QUANT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------
 * Quantum constants
 * ------------------------------------------------------------------ */

/* PIPA pulse size. EDD §4.2.1: 0.05 m/sec per pulse (dual redundant
 * optisyn pulse trains per axis). */
#define LVDA_PIPA_MPS_PER_PULSE  (0.05)

/* Fine gimbal resolver LSB. EDD §8.2.1: 0.00279 deg per LSB,
 * 2016 counts per 5.625-deg revolution. */
#define LVDA_FINE_RESOLVER_DEG_PER_LSB  (0.00279)

/* Attitude error D/A LSB. Saturn V hardware spec
 * `spc_stnv_000130.pdf` (MSFC, UAH Digital Projects archive): 0.0575
 * deg per LSB. Saturn IB EDD §8.2.3 derives 0.06 deg from SF = 3000
 * bits/pirad; we use the measured hardware value as the as-built LSB.
 * See docs/LVDC_QUANTIZATION_INVENTORY.md for the source comparison. */
#define LVDA_DAC_DEG_PER_LSB  (0.0575)

/* ---------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------ */

/* Round x to the nearest integer multiple of q.
 * Zero-quantum returns x unchanged (caller's choice to bypass). */
double lvdc_quant_round(double x, double q);

/* Convert a quantized platform velocity in m/s (as delivered by the
 * plant Accelerometer_Quantization) to an integer PIPA pulse count.
 * EDD §4.2.3: V_m = pulses · 0.05 m/s. */
int64_t lvdc_quant_mps_to_pulses(double v_mps);

/* Inverse of the above. */
double lvdc_quant_pulses_to_mps(int64_t pulses);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_QUANT_H */
