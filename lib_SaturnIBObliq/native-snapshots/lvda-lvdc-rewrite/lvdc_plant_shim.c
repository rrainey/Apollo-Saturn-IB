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
