/**
 * Optional RTW Product15 / ΔV stash — filled under -DIGM_TRACE_STASH in
 * Saturn_IB_Stack.c at IGM major-cycle sites (before rtb temps are reused).
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IgmTraceStash_tag {
  double valid; /* 1 if written this major cycle */

  /* Terminal Chi / Product15 */
  double phi_T_rad;
  double phi_iT_rad;
  double chi_a_rad;     /* raw atan(dVx/dVz) from Add10 ΔV */
  double chi_a_cmd_rad;
  double chi_b_cmd_rad;
  double v15_x;
  double v15_y;
  double v15_z;
  double Chi_Y_deg_raw; /* before DSM write */
  double Chi_Z_deg_raw;

  /* S-frame gravity into Product11/10 (rtb_Add1_p) — NOT accel quant */
  double GS_x;
  double GS_y;
  double GS_z;

  /* φ-frame after Rot_Y(φ_T)·(AP·…) */
  double Vphi_x;
  double Vphi_y;
  double Vphi_z;
  double Gphi_x;
  double Gphi_y;
  double Gphi_z;

  /* <S384> Add9 ΔV′ and outer Add10 ΔV used for χ_α */
  double dV0_x;
  double dV0_y;
  double dV0_z;
  double dV_x;
  double dV_y;
  double dV_z;

  double T_star_l; /* T1+T3 used in Add9 (pre-Add8) */
  double L_y;      /* rtb_Add7 = L1+L′3 seed for Gain1_h */
  double Gain1_h;  /* true Gain1_h (not Chi_Y) */

  /* First Phase Multiport / τ1_eff (Gain2 = RL·V_ex1) */
  double rate_lim_mf; /* <S357> RateLimiter output (s²/m) */
  double tau1_mp;     /* MultiportSwitch[0] */
  double T1_mp;       /* MultiportSwitch[1] pre-decrement */
  double tau3_mp;     /* MultiportSwitch[2] */
  double T3_mp;       /* MultiportSwitch[3] */
} IgmTraceStash;

extern IgmTraceStash g_igm_trace_stash;

#ifdef __cplusplus
}
#endif
