#include "onpad_trace.h"

#include <math.h>
#include <string.h>

void onpad_trace_clear(OnPadTraceSample *s)
{
  if (!s)
    return;
  memset(s, 0, sizeof(*s));
  s->valid = 0.0;
  s->lat_deg = s->lon_deg = s->h_m = NAN;
  s->theta_E_deg = s->theta_GMST_0_deg = s->A_z_deg = NAN;
  for (int i = 0; i < 3; i++) {
    s->Xe_m[i] = s->Ve_mps[i] = s->Vb_mps[i] = s->CG_LLA_deg_m[i] = NAN;
  }
  for (int i = 0; i < 4; i++)
    s->q_ECI[i] = NAN;
  for (int i = 0; i < 9; i++)
    s->LIO_DCM[i] = NAN;
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

int onpad_trace_write_json(FILE *fp, const OnPadTraceSample *s)
{
  if (!fp || !s)
    return -1;
  fputs("{\n", fp);
  fprintf(fp, "  \"t_sec\": %.17g,\n", s->t_sec);
  fprintf(fp, "  \"valid\": %.17g,\n", s->valid);
  fprintf(fp, "  \"lat_deg\": %.17g,\n", s->lat_deg);
  fprintf(fp, "  \"lon_deg\": %.17g,\n", s->lon_deg);
  fprintf(fp, "  \"h_m\": %.17g,\n", s->h_m);
  emit_arr(fp, "Xe_m", s->Xe_m, 3);
  fputs(",\n", fp);
  emit_arr(fp, "Ve_mps", s->Ve_mps, 3);
  fputs(",\n", fp);
  fprintf(fp, "  \"theta_E_deg\": %.17g,\n", s->theta_E_deg);
  fprintf(fp, "  \"theta_GMST_0_deg\": %.17g,\n", s->theta_GMST_0_deg);
  emit_arr(fp, "q_ECI", s->q_ECI, 4);
  fputs(",\n", fp);
  emit_arr(fp, "LIO_DCM", s->LIO_DCM, 9);
  fputs(",\n", fp);
  emit_arr(fp, "Vb_mps", s->Vb_mps, 3);
  fputs(",\n", fp);
  fprintf(fp, "  \"A_z_deg\": %.17g,\n", s->A_z_deg);
  emit_arr(fp, "CG_LLA_deg_m", s->CG_LLA_deg_m, 3);
  fputs("\n}\n", fp);
  return 0;
}
