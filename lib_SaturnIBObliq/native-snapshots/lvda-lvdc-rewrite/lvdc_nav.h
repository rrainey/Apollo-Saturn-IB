/*
 * lvdc_nav.h — boost-phase navigation (EDD §4.2 and §4.3).
 *
 * Reads the stabilized-platform PIPA integrated velocity on each
 * boost major loop (BML, nominal 1.6 s), converts to platform velocity
 * in m/s, computes F/M and smoothed M/F, integrates position and
 * velocity in the plumbline frame via trapezoidal integration
 * (Eqs. 4.3.1-5), and computes gravitational acceleration from the
 * oblate earth model (Eqs. 4.3.6-12).
 *
 * All state lives in lvdc_state_t.nav and .sensor (see lvdc_internal.h).
 * No file-scope statics in this module.
 */

#ifndef LVDC_NAV_H
#define LVDC_NAV_H

#include "lvdc_internal.h"
#include "lvdc_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Reset nav + sensor state to the pre-GRR condition. Called from
 * LVDA_LVDC_init. */
void lvdc_nav_reset(lvdc_state_t *s);

/* Execute one GRR event: snapshot the current platform velocity as
 * the zero-reset offset per EDD §4.2.3, and seed X_S, V_Sg from the
 * EDD Eqs. 3.4.3 and 3.4.4 position / velocity initialisation.
 *
 * X_0_eq343 and V_0_eq344 are the 3-vectors computed from the preset
 * launch-site parameters (R_L, A_z, phi_L, phi_L') by the caller.
 * See lvdc_nav_compute_initial_conditions. */
void lvdc_nav_grr_latch(lvdc_state_t *s,
                        const double V_m_bar_mps_now[3],
                        const double X_0_eq343[3],
                        const double V_0_eq344[3]);

/* Compute X_0 (EDD Eq. 3.4.3) and V_0 (EDD Eq. 3.4.4) from the
 * launch-site presets. */
void lvdc_nav_compute_initial_conditions(const lvdc_params_t *p,
                                         double X_0_eq343[3],
                                         double V_0_eq344[3]);

/* Execute one boost major loop (BML) read-and-integrate pass.
 *
 * V_m_bar_mps_now: platform velocity accumulator value read now from
 *   the ST-124M PIPAs. Already quantized at 0.05 m/s per the plant
 *   Accelerometer_Quantization block.
 *
 * t_now_sec: simulation time at the read, used to compute the
 *   actual ΔT between reads (EDD §4.2.4 requires the measured
 *   interval, not a nominal).
 *
 * p: mission preset parameters (earth model, timing limits).
 *
 * On return:
 *   s->nav.X_S, s->nav.V_Sg, s->nav.G_S updated per EDD Eqs. 4.3.1-12
 *   s->sensor.V_m_mps, FoverM, MoverF_s updated per EDD Eqs. 4.2.8-11
 */
void lvdc_nav_bml_step(lvdc_state_t *s,
                       const lvdc_params_t *p,
                       const double V_m_bar_mps_now[3],
                       double t_now_sec);

/* Compute total space-fixed vehicle velocity V_S (EDD Eq. 4.3.5).
 *   V_S = V_m + V_0 + V_Sg. Result is a 3-vector in the plumbline
 *   frame. */
void lvdc_nav_compute_V_S(const lvdc_state_t *s, double V_S_out[3]);

/* Magnitude of V_S (EDD Eq. 4.3.5.1). */
double lvdc_nav_compute_V_S_magnitude(const lvdc_state_t *s);

/* Compute gravitational acceleration G_S in the plumbline frame from
 * the vehicle position X_S. EDD §4.3.2 Eqs. 4.3.6-12.
 *
 * Phase B.2 implementation is a point-mass gravity (G_S = -μ·X_S/|X_S|^3).
 * Phase B.3 replaces this with the full J2 oblate expansion from the
 * EDD. The callers (lvdc_nav_bml_step) do not need to change.
 */
void lvdc_nav_compute_gravity(const double X_S[3],
                              const lvdc_params_t *p,
                              double G_S_out[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_NAV_H */
