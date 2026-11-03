/**
 * S-IB Stage pad / early-flight trace — OUT11 + liftoff flag.
 *
 * First matched gate: pre-liftoff OUT11 ≈ On Pad OUT22 (Xe/Ve/LLA).
 * batch-sim: --sib-trace PATH
 */
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SibTraceSample_tag {
  double t_sec;
  double valid; /* 1 if |Xe| earth-scale */
  double bLiftoff;

  /* OUT11 pad-relevant slice */
  double lat_deg;
  double lon_deg;
  double h_m;
  double Xe_m[3];
  double Ve_mps[3];
  double alpha_deg;
  double Vb_mps; /* airspeed scalar OUT11[13] */
  double q_ECI[4];
  double omega_b_rps[3];
  double Vb_vec_mps[3]; /* OUT11[21..23] */
} SibTraceSample;

void sib_trace_clear(SibTraceSample *s);
int sib_trace_write_json(FILE *fp, const SibTraceSample *s);

#ifdef __cplusplus
}
#endif
