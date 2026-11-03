#include "ic_trace.h"

#include <math.h>
#include <string.h>

void ic_trace_clear(IcTraceSample *s)
{
  if (!s)
    return;
  memset(s, 0, sizeof(*s));
  s->valid = 0.0;
  s->theta_GMST_0_deg = NAN;
  s->theta_E_deg = NAN;
  s->A_z_deg = NAN;
  for (int i = 0; i < 9; i++) {
    s->ST124M_DCM[i] = NAN;
    s->LIO_DCM[i] = NAN;
  }
  for (int i = 0; i < 4; i++)
    s->q_ECI_0[i] = NAN;
  for (int i = 0; i < 3; i++) {
    s->Xe_0_m[i] = NAN;
    s->Vb_0_mps[i] = NAN;
    s->V_ECI_0_mps[i] = NAN;
    s->CG_LLA_deg_m[i] = NAN;
  }
  for (int i = 0; i < 6; i++)
    s->LaunchDate[i] = NAN;
}

static void emit_arr(FILE *fp, const char *name, const double *a, int n)
{
  fprintf(fp, "  \"%s\": [", name);
  for (int i = 0; i < n; i++) {
    if (i)
      fputc(',', fp);
    fprintf(fp, "%.17g", a[i]);
  }
  fputs("]", fp);
}

int ic_trace_write_json(FILE *fp, const IcTraceSample *s)
{
  if (!fp || !s)
    return -1;
  fputs("{\n", fp);
  fprintf(fp, "  \"t_sec\": %.17g,\n", s->t_sec);
  fprintf(fp, "  \"valid\": %.17g,\n", s->valid);
  fprintf(fp, "  \"theta_GMST_0_deg\": %.17g,\n", s->theta_GMST_0_deg);
  fprintf(fp, "  \"theta_E_deg\": %.17g,\n", s->theta_E_deg);
  emit_arr(fp, "ST124M_DCM", s->ST124M_DCM, 9);
  fputs(",\n", fp);
  emit_arr(fp, "LIO_DCM", s->LIO_DCM, 9);
  fputs(",\n", fp);
  emit_arr(fp, "q_ECI_0", s->q_ECI_0, 4);
  fputs(",\n", fp);
  emit_arr(fp, "Xe_0_m", s->Xe_0_m, 3);
  fputs(",\n", fp);
  emit_arr(fp, "Vb_0_mps", s->Vb_0_mps, 3);
  fputs(",\n", fp);
  emit_arr(fp, "V_ECI_0_mps", s->V_ECI_0_mps, 3);
  fputs(",\n", fp);
  emit_arr(fp, "LaunchDate", s->LaunchDate, 6);
  fputs(",\n", fp);
  fprintf(fp, "  \"A_z_deg\": %.17g,\n", s->A_z_deg);
  emit_arr(fp, "CG_LLA_deg_m", s->CG_LLA_deg_m, 3);
  fputs("\n}\n", fp);
  return 0;
}
