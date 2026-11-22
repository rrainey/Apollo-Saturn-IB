/*
 * lvdc_plant_shim.c — compatibility symbols for the plant's
 * instrumentation peek-in (SaturnIBPlantObliq.c).
 *
 * The hand-edited LVDA_LVDC.c exposes a handful of file-scope
 * statics so the plant layer (which lives outside the LVDC) can
 * emit them in --igm-trace rows. The rewrite keeps its state in
 * module-local statics behind a cleaner API, so those externs
 * don't exist.
 *
 * To let batch_sim link against the rewrite without changing
 * SaturnIBPlantObliq.c, this file defines the externs and lets
 * the dispatcher update them from inside g_lvdc / g_igm. HSL
 * fields stay zero until Phase D.5 implements the loop.
 *
 * Scope: shim only. Nothing here affects guidance output.
 */

/* HSL sample ring (EDD §4.4.2.4). Not yet implemented in the
 * rewrite; keep zero so --igm-trace emits sentinel values that
 * downstream tooling already handles. */
int    s_hsl_V_samples = 0;
double s_hsl_V_curr    = 0.0;
double s_hsl_V_prev    = 0.0;
double s_hsl_V_prev2   = 0.0;

/* Nav-side cached state. The rewrite stores these in
 * g_lvdc.nav.V_Sg and g_lvdc.nav.G_S_prev; the dispatcher
 * mirrors them into these globals each BML for the plant's
 * trace hook. Default to zero for pre-liftoff. */
double s_V_Sg[3]      = { 0.0, 0.0, 0.0 };
double s_G_S_prev[3]  = { 0.0, 0.0, 0.0 };

/* IGM-side state for --igm-trace. The hand-edited legacy LVDC
 * published these via codegen signals/enable_states struct fields;
 * the rewrite keeps them in g_igm + g_lvdc.nav and mirrors them
 * here each BML. */
int    s_igm_enabled    = 0;
double s_T_3_eff_sec    = 0.0;   /* g_igm.T_3i_sec during IGM mode */
double s_V_S_mag_mps    = 0.0;   /* |V_S| from EDD Eq. 4.3.5.1 */
double s_T_HSL_bias_sec = 0.0;   /* HSL entry threshold T_HSL + dT_b */
double s_XS[3]          = { 0.0, 0.0, 0.0 };  /* g_lvdc.nav.X_S */
double s_VS[3]          = { 0.0, 0.0, 0.0 };  /* V_S from Eq. 4.3.5 */

/* Sensor-path isolation (EDD §4.3.1 Eq. 4.3.5 decomposition).
 * V_S = V_m + V_S_init + V_Sg:
 *   V_m       — PIPA-integrated velocity in S-frame, post-GRR,
 *               non-gravitational component only.
 *   V_S_init  — Eq. 3.4.4 earth-rotation seed (constant after GRR).
 *   V_Sg      — gravity-integrated velocity (already in s_V_Sg).
 *
 * The dispatcher mirrors g_lvdc.sensor.V_m_mps and V_S_init into
 * these globals each BML so the --igm-trace emission in
 * SaturnIBPlantObliq.c can publish them for LVDC-vs-plant drift
 * audits. */
double s_V_m[3]      = { 0.0, 0.0, 0.0 };
double s_V_S_init[3] = { 0.0, 0.0, 0.0 };

/* MES DCM (E-frame → S-frame) exposed for off-line frame-rotation
 * checks. Published once per BML from the plant input. */
double s_MES_DCM[3][3] = {{1,0,0},{0,1,0},{0,0,1}};

/* Plant-level bGRR discrete.  Fires at sim_t = T_L_prime_sec +
 * GRR_offset_sec (= 295 s for AS-205 per TMX).  The host adapter mirrors
 * model->signals.bGRR_latch here after each plant step; the rewrite's
 * LVDA_LVDC.c reads it for the GRR rising-edge latch.  Shim global
 * (not a formal LVDA_LVDC input port) because the generated LVDA_LVDC.h
 * takes precedence over the rewrite's for same-directory includes. */
int s_bGRR = 0;
