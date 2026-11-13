/**
 * IGM interface trace schema.
 *
 * batch-sim owns the CSV contract. SaturnIBStack fills IgmTraceSample via
 * FillIgmTrace() from the Obliq plant; missing fields use NAN.
 *
 * Cadence: typically every IGM major cycle (1.6 s) or every outer step.
 *
 * See README section "IGM trace" and scripts/compare_igm_trace.py.
 */
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** IGM major-cycle sample time (s). */
#ifndef IGM_TRACE_DT_SEC
#define IGM_TRACE_DT_SEC 1.6
#endif

typedef struct IgmTraceSample_tag {
  double t_sec;

  /* Discrete / mode */
  double igm_enable;     /* 1 when IGM enable path active (best-effort) */
  double T_1_i_sec;
  double T_3_i_sec;
  double T_3_eff_sec;    /* Add8 / T3_eff */
  double nIGMMode;
  double nHSLActive;
  double bCutoff;

  /* Chi / SMC */
  double Chi_Y_deg;
  double Chi_Z_deg;
  double Chi_cmd_0;
  double Chi_cmd_1;
  double Chi_cmd_2;
  double SMCY_rad;
  double SMCZ_rad;

  /* Chi→Ψ (Obliq Wave C). */
  double Psi_P_deg;
  double Psi_Y_deg;

  /* S-frame / accel inputs to IGM (when available). XS/VS come from the
     LVDC's S_Frame_Eqns_4_3_1_5 Product18 / Product6 vectors. */
  double XS_x;
  double XS_y;
  double XS_z;
  double VS_x;
  double VS_y;
  double VS_z;
  double A_x;
  double A_y;
  double A_z;

  /* LVDC-visible HSL entry inputs (sensor-path). vigained = V_T − |V|_S;
     entry gate is (T_3_i ≤ T_HSL_bias) && (vigained < V_GRD). */
  double V_S_mag_mps;
  double vigained_mps;
  double T_HSL_bias_sec;

  /* Diagnostic: raw V_m_bar_mps from plant (pre-GRR-offset). */
  double V_m_raw_x;
  double V_m_raw_y;
  double V_m_raw_z;

  /* Diagnostic: EDD integration state (static locals in LVDA_LVDC.c). */
  double V_Sg_x;
  double V_Sg_y;
  double V_Sg_z;
  double G_S_prev_x;
  double G_S_prev_y;
  double G_S_prev_z;

  /* Diagnostic: HSL polynomial state (does it activate?) */
  double hsl_V_samples;
  double hsl_V_curr;
  double hsl_V_prev;
  double hsl_V_prev2;

  /* Diagnostic: snapshot inputs for V_0/X_0 timing audit. */
  double PAD_Ve_x;
  double PAD_Ve_y;
  double PAD_Ve_z;
  double PAD_Xe_x;
  double PAD_Xe_y;
  double PAD_Xe_z;
  double MES_PAD_Ve_x;  /* MES × PAD_Ve_mps (= V_0 if snapshotted now) */
  double MES_PAD_Ve_y;
  double MES_PAD_Ve_z;
  double MES_PAD_Xe_x;  /* MES × PAD_Xe_m (= X_0 if snapshotted now) */
  double MES_PAD_Xe_y;
  double MES_PAD_Xe_z;

  /* Plant diagnostic */
  double elev_cmd_deg;

  /* Product15 internals (Obliq; or legacy stash via -DIGM_TRACE_STASH). */
  double phi_T_rad;
  double chi_a_cmd_rad;
  double chi_b_cmd_rad;
  double v15_x;
  double v15_y;
  double v15_z;
  double Gain1_h;
  double chi_a_rad; /* raw atan(dVx/dVz) before rate/φ/π/2 */
  double sw_a;      /* χ̇_α switch (rate term into chi_a_cmd) */

  /* Matched-input ΔV path (legacy stash; NAN on Obliq until filled). */
  double phi_iT_rad;
  double GS_x;
  double GS_y;
  double GS_z;
  double Vphi_x;
  double Vphi_y;
  double Vphi_z;
  double Gphi_x;
  double Gphi_y;
  double Gphi_z;
  double dV0_x;
  double dV0_y;
  double dV0_z;
  double dV_x;
  double dV_y;
  double dV_z;
  double T_star_l;
  double L_y;

  double rate_lim_mf;
  double tau1_mp;
  double T1_mp;
  double tau3_mp;
  double T3_mp;
} IgmTraceSample;

/** CSV column order (must match IgmTraceSample field order after t_sec). */
const char *igm_trace_csv_header(void);

/** Write one data row (no header). Returns 0 on success. */
int igm_trace_write_row(FILE *f, const IgmTraceSample *s);

/** Clear sample to NAN (except t_sec left unchanged if already set). */
void igm_trace_clear(IgmTraceSample *s);

#ifdef __cplusplus
}
#endif
