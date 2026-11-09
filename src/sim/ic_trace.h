/**
 * Initial Conditions (<S5>) trace — plant-state snapshot for matched compare.
 *
 * batch-sim writes JSON via --ic-trace PATH (typically once at t≈0).
 * See obliq-2 examples/saturn-ib/IC_MATCHED_COMPARE_PLAN.md.
 */
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IcTraceSample_tag {
  double t_sec;
  double valid; /* 1 if Xe magnitude looks earth-scale */

  /* <S5> outports */
  double theta_GMST_0_deg;
  double theta_E_deg; /* S5 Sum = GMST + λ_L */
  double ST124M_DCM[9]; /* MES column-major Fcn 11,12,13,21,22,23,31,32,33 */
  double LIO_DCM[9];    /* LVInert same layout (Az=100°) */
  double q_ECI_0[4];
  double Xe_0_m[3];
  double Vb_0_mps[3];
  double V_ECI_0_mps[3]; /* ω_E × Xe (reconstructed from Product) */

  /* Inputs used (for audit) */
  double LaunchDate[6];
  double A_z_deg;
  double CG_LLA_deg_m[3];
} IcTraceSample;

void ic_trace_clear(IcTraceSample *s);

/** Write one JSON object (no trailing newline required). Returns 0 on success. */
int ic_trace_write_json(FILE *fp, const IcTraceSample *s);

#ifdef __cplusplus
}
#endif
