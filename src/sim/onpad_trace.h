/**
 * On Pad (<S8>) trace — OUT22 + BlockIO for matched compare.
 *
 * batch-sim: --onpad-trace PATH
 */
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OnPadTraceSample_tag {
  double t_sec;
  double valid;

  /* OUT22 pad reference */
  double lat_deg;
  double lon_deg;
  double h_m;
  double Xe_m[3];
  double Ve_mps[3]; /* ω×Xe into OUT22 */

  /* Nested <S74> Initial Conditions (BlockIO) */
  double theta_E_deg; /* S74 Sum */
  double theta_GMST_0_deg; /* Sum − λ */
  double q_ECI[4]; /* Merge_e */
  double LIO_DCM[9]; /* LVInertialOrientation_e Fcn layout */
  double Vb_mps[3]; /* Product1_l when available */

  double CG_LLA_deg_m[3];
  double A_z_deg;
} OnPadTraceSample;

void onpad_trace_clear(OnPadTraceSample *s);
int onpad_trace_write_json(FILE *fp, const OnPadTraceSample *s);

#ifdef __cplusplus
}
#endif
