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
 * X_0_eq343 and V_0_eq344 are 3-vectors computed from presets
 * (R_L, A_z, phi_L, phi_L_prime) by the caller; see lvdc_params.h. */
void lvdc_nav_grr_latch(lvdc_state_t *s,
                        const double V_m_bar_mps_now[3],
                        const double X_0_eq343[3],
                        const double V_0_eq344[3]);

/* Execute one boost major loop (BML) read-and-integrate pass.
 *
 * V_m_bar_mps_now: platform velocity accumulator value read now from
 *   the ST-124M PIPAs. Already quantized at 0.05 m/s per the
 *   Accelerometer_Quantization plant block (see Quantization doc).
 *
 * t_now_sec: simulation time at the read.
 *
 * On return:
 *   s->nav.X_S, s->nav.V_Sg, s->nav.G_S updated per EDD Eqs. 4.3.1-12
 *   s->sensor.FoverM, s->sensor.MoverF_s updated per EDD Eqs. 4.2.8-11
 */
void lvdc_nav_bml_step(lvdc_state_t *s,
                       const double V_m_bar_mps_now[3],
                       double t_now_sec);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_NAV_H */
