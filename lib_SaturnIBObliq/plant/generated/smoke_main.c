#include "saturn_ib_stack_rtw_adapter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static void write_final_json(const char *path, double t,
                             const ExternalOutputs_saturn_ib_stack *o) {
  FILE *f = fopen(path, "w");
  if (!f) { perror(path); return; }
  fprintf(f, "{\n");
  fprintf(f, "  \"elapsed_sim_sec\": %.17g,\n", t);
  fprintf(f, "  \"s2_Xe_x_m\": %.17g,\n", o->OUT11[0]);
  fprintf(f, "  \"s2_Xe_y_m\": %.17g,\n", o->OUT11[1]);
  fprintf(f, "  \"s2_Xe_z_m\": %.17g,\n", o->OUT11[2]);
  fprintf(f, "  \"s2_Ve_x_mps\": %.17g,\n", o->OUT11[3]);
  fprintf(f, "  \"s2_Ve_y_mps\": %.17g,\n", o->OUT11[4]);
  fprintf(f, "  \"s2_Ve_z_mps\": %.17g,\n", o->OUT11[5]);
  fprintf(f, "  \"s2_h_m\": %.17g,\n", o->OUT11[6]);
  fprintf(f, "  \"veh_q_ECI_q0\": %.17g,\n", o->veh_q_ECI[0]);
  fprintf(f, "  \"veh_q_ECI_q1\": %.17g,\n", o->veh_q_ECI[1]);
  fprintf(f, "  \"veh_q_ECI_q2\": %.17g,\n", o->veh_q_ECI[2]);
  fprintf(f, "  \"veh_q_ECI_q3\": %.17g,\n", o->veh_q_ECI[3]);
  fprintf(f, "  \"BodyToSM_Phi_deg\": %.17g,\n", o->BodyToSM_Phi_deg);
  fprintf(f, "  \"BodyToSM_Theta_deg\": %.17g,\n", o->BodyToSM_Theta_deg);
  fprintf(f, "  \"BodyToSM_Psi_deg\": %.17g,\n", o->BodyToSM_Psi_deg);
  fprintf(f, "  \"pad_lat_deg\": %.17g,\n", o->OUT22[0]);
  fprintf(f, "  \"pad_lon_deg\": %.17g,\n", o->OUT22[1]);
  fprintf(f, "  \"pad_h_m\": %.17g,\n", o->OUT22[2]);
  fprintf(f, "  \"pad_Xe_x_m\": %.17g,\n", o->OUT22[3]);
  fprintf(f, "  \"pad_Xe_y_m\": %.17g,\n", o->OUT22[4]);
  fprintf(f, "  \"pad_Xe_z_m\": %.17g,\n", o->OUT22[5]);
  fprintf(f, "  \"pad_Ve_x_mps\": %.17g,\n", o->OUT22[6]);
  fprintf(f, "  \"pad_Ve_y_mps\": %.17g,\n", o->OUT22[7]);
  fprintf(f, "  \"pad_Ve_z_mps\": %.17g,\n", o->OUT22[8]);
  fprintf(f, "  \"bLiftoff\": %s,\n", o->bLiftoff ? "true" : "false");
  fprintf(f, "  \"bStageSep\": %s,\n", o->bStageSep ? "true" : "false");
  fprintf(f, "  \"bIECO\": %s,\n", o->bIECO ? "true" : "false");
  fprintf(f, "  \"bOECO\": %s,\n", o->bOECO ? "true" : "false");
  fprintf(f, "  \"bS_IVB_EngineStart\": %s\n", o->bS_IVB_EngineStart ? "true" : "false");
  fprintf(f, "}\n");
  fclose(f);
}

int main(int argc, char **argv) {
  double duration = 1.0;
  const char *outPath = "final.json";
  for (int a = 1; a < argc; a++) {
    if (!strcmp(argv[a], "--duration") && a + 1 < argc) duration = atof(argv[++a]);
    else if (!strcmp(argv[a], "--out") && a + 1 < argc) outPath = argv[++a];
  }

  saturn_ib_stack_t model;
  ExternalInputs_saturn_ib_stack in;
  ExternalOutputs_saturn_ib_stack out;
  memset(&in, 0, sizeof(in));
  in.A_z_deg = 82.82;
  in.CG_LLA_deg_m[0] = 28.521963;
  in.CG_LLA_deg_m[1] = -80.561141;
  in.CG_LLA_deg_m[2] = 34.7;
  in.T_L_prime_sec = 300.0;
  in.q_ECItoSM[0] = 1.0;
  in.LaunchDate[0] = 1968; in.LaunchDate[1] = 10; in.LaunchDate[2] = 11;
  in.LaunchDate[3] = 14; in.LaunchDate[4] = 57; in.LaunchDate[5] = 45;

  saturn_ib_stack_init(&model, 0.005);
  saturn_ib_stack_apply_external_inputs(&model, &in);
  /* Algebra/enables/ICs need live ExtU (CG_LLA, LaunchDate, …) */
  saturn_ib_stack_seed_from_inputs(&model);
  /* IC snapshot after seed (before stepping) */
  saturn_ib_stack_collect_external_outputs(&model, &out);
  write_final_json("final-ic.json", model.time, &out);

  const int nsteps = (int)(duration / 0.005 + 0.5);
  for (int i = 0; i < nsteps; i++) {
    saturn_ib_stack_rtw_step(&model, &in, &out);
  }
  write_final_json(outPath, model.time, &out);
  printf("batch ok steps=%d time=%g out=%s bLiftoff=%d Xe=(%.3g,%.3g,%.3g)\n",
         nsteps, model.time, outPath, (int)out.bLiftoff,
         out.OUT11[0], out.OUT11[1], out.OUT11[2]);
  printf("  OUT22 pad lat=%.6f lon=%.6f h=%.3f |Xe|=%.3f\n",
         out.OUT22[0], out.OUT22[1], out.OUT22[2],
         sqrt(out.OUT22[3]*out.OUT22[3]+out.OUT22[4]*out.OUT22[4]+out.OUT22[5]*out.OUT22[5]));
  return 0;
}
