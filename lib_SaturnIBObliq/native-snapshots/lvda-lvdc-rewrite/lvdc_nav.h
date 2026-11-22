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

/* Initialize the PIPA zero-reset and plumbline-frame position /
 * velocity seed per EDD §3.4 and §4.2.3.
 *
 * On a real Saturn IB, this fires at Guidance Reference Release (GRR)
 * — nominally T-5 s per TM-X-62831 for AS-205.  The plant now exposes
 * a bGRR discrete at that instant; the caller may fire latch on either
 * bGRR edge or bLiftoff edge.  Firing at GRR requires the first-BML
 * trap-startup fixes below (t_latch_sec + gravity seed) so the pad-hold
 * integration of V_m (+1g normal force) and V_Sg (-1g gravity) cancels
 * cleanly.
 *
 * p         : mission preset parameters, used to compute G_S_prev at X_0
 *             so Eq. 4.3.4 trap has a correct initial condition.
 * t_latch_sec: simulation time at the latch instant.  Stored in sensor
 *             state so the first bml_step can compute dT = t_now - t_latch
 *             instead of the nominal 1.6 s.
 * V_m_bar_mps_now: current platform-velocity accumulator value.
 * X_0_eq343, V_0_eq344: 3-vectors from EDD Eqs. 3.4.3-4
 *             (see lvdc_nav_compute_initial_conditions).
 */
void lvdc_nav_liftoff_latch(lvdc_state_t *s,
                            const lvdc_params_t *p,
                            double t_latch_sec,
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

/* Compute gravitational acceleration G_S in the plumbline (S) frame
 * from the vehicle position X_S. EDD §4.3.2 Eqs. 4.3.6-12 — oblate
 * earth gravity with the second zonal harmonic (J_2).
 *
 * During boost the EDD mandates zero S_34 and P_34 (the third and
 * fourth zonal harmonics); set boost_mode = true. During orbital
 * navigation set boost_mode = false to include S_34 / P_34.
 *
 * The plumbline-frame projection of the earth spin axis depends on
 * the launch-site latitude and azimuth, which the function reads
 * from p. */
void lvdc_nav_compute_gravity(const double X_S[3],
                              const lvdc_params_t *p,
                              bool boost_mode,
                              double G_S_out[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_NAV_H */
