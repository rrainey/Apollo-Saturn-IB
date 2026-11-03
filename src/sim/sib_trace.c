#include "sib_trace.h"

#include <math.h>
#include <string.h>

void sib_trace_clear(SibTraceSample *s)
{
  if (!s)
    return;
  memset(s, 0, sizeof(*s));
  s->valid = 0.0;
  s->bLiftoff = NAN;
  s->lat_deg = s->lon_deg = s->h_m = s->alpha_deg = s->Vb_mps = NAN;
  for (int i = 0; i < 3; i++) {
    s->Xe_m[i] = s->Ve_mps[i] = s->omega_b_rps[i] = s->Vb_vec_mps[i] = NAN;
  }
  for (int i = 0; i < 4; i++)
    s->q_ECI[i] = NAN;
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

int sib_trace_write_json(FILE *fp, const SibTraceSample *s)
{
  if (!fp || !s)
    return -1;
  fputs("{\n", fp);
  fprintf(fp, "  \"t_sec\": %.17g,\n", s->t_sec);
  fprintf(fp, "  \"valid\": %.17g,\n", s->valid);
  fprintf(fp, "  \"bLiftoff\": %.17g,\n", s->bLiftoff);
  fprintf(fp, "  \"lat_deg\": %.17g,\n", s->lat_deg);
  fprintf(fp, "  \"lon_deg\": %.17g,\n", s->lon_deg);
  fprintf(fp, "  \"h_m\": %.17g,\n", s->h_m);
  emit_arr(fp, "Xe_m", s->Xe_m, 3);
  fputs(",\n", fp);
  emit_arr(fp, "Ve_mps", s->Ve_mps, 3);
  fputs(",\n", fp);
  fprintf(fp, "  \"alpha_deg\": %.17g,\n", s->alpha_deg);
  fprintf(fp, "  \"Vb_mps\": %.17g,\n", s->Vb_mps);
  emit_arr(fp, "q_ECI", s->q_ECI, 4);
  fputs(",\n", fp);
  emit_arr(fp, "omega_b_rps", s->omega_b_rps, 3);
  fputs(",\n", fp);
  emit_arr(fp, "Vb_vec_mps", s->Vb_vec_mps, 3);
  fputs("\n}\n", fp);
  return 0;
}
