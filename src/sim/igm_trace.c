#include "igm_trace.h"

#include <math.h>

static void emit(FILE *f, double v, int last)
{
  if (isnan(v)) {
    fputs(last ? "nan" : "nan,", f);
  } else {
    fprintf(f, last ? "%.12g" : "%.12g,", v);
  }
}

const char *igm_trace_csv_header(void)
{
  return "t_sec,"
         "igm_enable,"
         "T_1_i_sec,T_3_i_sec,T_3_eff_sec,"
         "nIGMMode,nHSLActive,bCutoff,"
         "Chi_Y_deg,Chi_Z_deg,"
         "Chi_cmd_0,Chi_cmd_1,Chi_cmd_2,"
         "SMCY_rad,SMCZ_rad,"
         "Psi_P_deg,Psi_Y_deg,"
         "XS_x,XS_y,XS_z,"
         "VS_x,VS_y,VS_z,"
         "A_x,A_y,A_z,"
         "elev_cmd_deg,"
         "phi_T_rad,chi_a_cmd_rad,chi_b_cmd_rad,"
         "v15_x,v15_y,v15_z,Gain1_h,"
         "chi_a_rad,sw_a,"
         "phi_iT_rad,"
         "GS_x,GS_y,GS_z,"
         "Vphi_x,Vphi_y,Vphi_z,"
         "Gphi_x,Gphi_y,Gphi_z,"
         "dV0_x,dV0_y,dV0_z,"
         "dV_x,dV_y,dV_z,"
         "T_star_l,L_y,"
         "rate_lim_mf,tau1_mp,T1_mp,tau3_mp,T3_mp,"
         "V_S_mag_mps,vigained_mps,T_HSL_bias_sec,"
         "V_m_raw_x,V_m_raw_y,V_m_raw_z,"
         "V_Sg_x,V_Sg_y,V_Sg_z,"
         "G_S_prev_x,G_S_prev_y,G_S_prev_z,"
         "hsl_V_samples,hsl_V_curr,hsl_V_prev,hsl_V_prev2,"
         "PAD_Ve_x,PAD_Ve_y,PAD_Ve_z,"
         "PAD_Xe_x,PAD_Xe_y,PAD_Xe_z,"
         "MES_PAD_Ve_x,MES_PAD_Ve_y,MES_PAD_Ve_z,"
         "MES_PAD_Xe_x,MES_PAD_Xe_y,MES_PAD_Xe_z";
}

void igm_trace_clear(IgmTraceSample *s)
{
  const double nan = NAN;
  double t = s->t_sec;
  IgmTraceSample z;
  z.t_sec = t;
  z.igm_enable = nan;
  z.T_1_i_sec = nan;
  z.T_3_i_sec = nan;
  z.T_3_eff_sec = nan;
  z.nIGMMode = nan;
  z.nHSLActive = nan;
  z.bCutoff = nan;
  z.Chi_Y_deg = nan;
  z.Chi_Z_deg = nan;
  z.Chi_cmd_0 = nan;
  z.Chi_cmd_1 = nan;
  z.Chi_cmd_2 = nan;
  z.SMCY_rad = nan;
  z.SMCZ_rad = nan;
  z.Psi_P_deg = nan;
  z.Psi_Y_deg = nan;
  z.XS_x = nan;
  z.XS_y = nan;
  z.XS_z = nan;
  z.VS_x = nan;
  z.VS_y = nan;
  z.VS_z = nan;
  z.A_x = nan;
  z.A_y = nan;
  z.A_z = nan;
  z.elev_cmd_deg = nan;
  z.phi_T_rad = nan;
  z.chi_a_cmd_rad = nan;
  z.chi_b_cmd_rad = nan;
  z.v15_x = nan;
  z.v15_y = nan;
  z.v15_z = nan;
  z.Gain1_h = nan;
  z.chi_a_rad = nan;
  z.sw_a = nan;
  z.phi_iT_rad = nan;
  z.GS_x = nan;
  z.GS_y = nan;
  z.GS_z = nan;
  z.Vphi_x = nan;
  z.Vphi_y = nan;
  z.Vphi_z = nan;
  z.Gphi_x = nan;
  z.Gphi_y = nan;
  z.Gphi_z = nan;
  z.dV0_x = nan;
  z.dV0_y = nan;
  z.dV0_z = nan;
  z.dV_x = nan;
  z.dV_y = nan;
  z.dV_z = nan;
  z.T_star_l = nan;
  z.L_y = nan;
  z.rate_lim_mf = nan;
  z.tau1_mp = nan;
  z.T1_mp = nan;
  z.tau3_mp = nan;
  z.T3_mp = nan;
  z.V_S_mag_mps = nan;
  z.vigained_mps = nan;
  z.T_HSL_bias_sec = nan;
  z.V_m_raw_x = nan;
  z.V_m_raw_y = nan;
  z.V_m_raw_z = nan;
  z.hsl_V_samples = nan;
  z.hsl_V_curr = nan;
  z.hsl_V_prev = nan;
  z.hsl_V_prev2 = nan;
  z.V_Sg_x = nan; z.V_Sg_y = nan; z.V_Sg_z = nan;
  z.G_S_prev_x = nan; z.G_S_prev_y = nan; z.G_S_prev_z = nan;
  z.PAD_Ve_x = nan; z.PAD_Ve_y = nan; z.PAD_Ve_z = nan;
  z.PAD_Xe_x = nan; z.PAD_Xe_y = nan; z.PAD_Xe_z = nan;
  z.MES_PAD_Ve_x = nan; z.MES_PAD_Ve_y = nan; z.MES_PAD_Ve_z = nan;
  z.MES_PAD_Xe_x = nan; z.MES_PAD_Xe_y = nan; z.MES_PAD_Xe_z = nan;
  *s = z;
}

int igm_trace_write_row(FILE *f, const IgmTraceSample *s)
{
  if (!f || !s)
    return -1;
  emit(f, s->t_sec, 0);
  emit(f, s->igm_enable, 0);
  emit(f, s->T_1_i_sec, 0);
  emit(f, s->T_3_i_sec, 0);
  emit(f, s->T_3_eff_sec, 0);
  emit(f, s->nIGMMode, 0);
  emit(f, s->nHSLActive, 0);
  emit(f, s->bCutoff, 0);
  emit(f, s->Chi_Y_deg, 0);
  emit(f, s->Chi_Z_deg, 0);
  emit(f, s->Chi_cmd_0, 0);
  emit(f, s->Chi_cmd_1, 0);
  emit(f, s->Chi_cmd_2, 0);
  emit(f, s->SMCY_rad, 0);
  emit(f, s->SMCZ_rad, 0);
  emit(f, s->Psi_P_deg, 0);
  emit(f, s->Psi_Y_deg, 0);
  emit(f, s->XS_x, 0);
  emit(f, s->XS_y, 0);
  emit(f, s->XS_z, 0);
  emit(f, s->VS_x, 0);
  emit(f, s->VS_y, 0);
  emit(f, s->VS_z, 0);
  emit(f, s->A_x, 0);
  emit(f, s->A_y, 0);
  emit(f, s->A_z, 0);
  emit(f, s->elev_cmd_deg, 0);
  emit(f, s->phi_T_rad, 0);
  emit(f, s->chi_a_cmd_rad, 0);
  emit(f, s->chi_b_cmd_rad, 0);
  emit(f, s->v15_x, 0);
  emit(f, s->v15_y, 0);
  emit(f, s->v15_z, 0);
  emit(f, s->Gain1_h, 0);
  emit(f, s->chi_a_rad, 0);
  emit(f, s->sw_a, 0);
  emit(f, s->phi_iT_rad, 0);
  emit(f, s->GS_x, 0);
  emit(f, s->GS_y, 0);
  emit(f, s->GS_z, 0);
  emit(f, s->Vphi_x, 0);
  emit(f, s->Vphi_y, 0);
  emit(f, s->Vphi_z, 0);
  emit(f, s->Gphi_x, 0);
  emit(f, s->Gphi_y, 0);
  emit(f, s->Gphi_z, 0);
  emit(f, s->dV0_x, 0);
  emit(f, s->dV0_y, 0);
  emit(f, s->dV0_z, 0);
  emit(f, s->dV_x, 0);
  emit(f, s->dV_y, 0);
  emit(f, s->dV_z, 0);
  emit(f, s->T_star_l, 0);
  emit(f, s->L_y, 0);
  emit(f, s->rate_lim_mf, 0);
  emit(f, s->tau1_mp, 0);
  emit(f, s->T1_mp, 0);
  emit(f, s->tau3_mp, 0);
  emit(f, s->T3_mp, 0);
  emit(f, s->V_S_mag_mps, 0);
  emit(f, s->vigained_mps, 0);
  emit(f, s->T_HSL_bias_sec, 0);
  emit(f, s->V_m_raw_x, 0);
  emit(f, s->V_m_raw_y, 0);
  emit(f, s->V_m_raw_z, 0);
  emit(f, s->V_Sg_x, 0); emit(f, s->V_Sg_y, 0); emit(f, s->V_Sg_z, 0);
  emit(f, s->G_S_prev_x, 0); emit(f, s->G_S_prev_y, 0); emit(f, s->G_S_prev_z, 0);
  emit(f, s->hsl_V_samples, 0);
  emit(f, s->hsl_V_curr, 0); emit(f, s->hsl_V_prev, 0); emit(f, s->hsl_V_prev2, 0);
  emit(f, s->PAD_Ve_x, 0); emit(f, s->PAD_Ve_y, 0); emit(f, s->PAD_Ve_z, 0);
  emit(f, s->PAD_Xe_x, 0); emit(f, s->PAD_Xe_y, 0); emit(f, s->PAD_Xe_z, 0);
  emit(f, s->MES_PAD_Ve_x, 0); emit(f, s->MES_PAD_Ve_y, 0); emit(f, s->MES_PAD_Ve_z, 0);
  emit(f, s->MES_PAD_Xe_x, 0); emit(f, s->MES_PAD_Xe_y, 0); emit(f, s->MES_PAD_Xe_z, 1);
  fputc('\n', f);
  return 0;
}
