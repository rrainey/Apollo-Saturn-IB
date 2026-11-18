/*
 * lvdc_preigm.h — first-stage guidance (EDD §4.4.1).
 *
 * Generates the pitch, yaw and roll `chi_cmd_rad[3]` attitude commands
 * from GRR to T3 + 0.0, before IGM takes over.
 *
 * Scope (Phase C.1):
 *   §4.4.1.1  Roll maneuver.  Hold `chi_x` at the stored pad roll,
 *             then set to zero after gantry clear.
 *   §4.4.1.2  Pitch time-tilt.  Piecewise cubic polynomial in `T_c`,
 *             held at zero before gantry clear, held at `P_final`
 *             after the final segment.
 *   §4.4.1.3  Yaw time-tilt.  Zero for Phase C.1; the EDD Eq. 4.4.2.1
 *             table lookup lands with lvdc_igm.
 *
 * Not yet implemented in v1:
 *   §4.4.1.4  Engine-out guidance modifications (DI14, DI15).  All
 *             engines are assumed nominal.
 *
 * `T_c` is `T1 − T_d` per EDD Eq. 4.4.1.  `T_d` is the compute-to-
 * command delay bias.  The current body uses `T1_Timer_Switch` as
 * the independent variable; the bias term is absorbed into the
 * mission preset tables.  The rewrite exposes `T_c` directly so the
 * scheduler owns the bias.
 */

#ifndef LVDC_PREIGM_H
#define LVDC_PREIGM_H

#include "lvdc_internal.h"
#include "lvdc_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compute `chi_cmd_rad[3]` from the first-stage time-tilt profile.
 *
 *   T_c_sec       time since liftoff, biased for compute delay
 *                 (EDD Eq. 4.4.1).  Zero before liftoff.
 *   theta_x_rad   stored pad roll angle; used as `chi_x` before
 *                 gantry clear per EDD §4.4.1.1.
 *   p             mission preset parameters.
 *   chi_cmd_rad[] output: {chi_x, chi_y, chi_z} in radians.
 */
void lvdc_preigm_chi_cmd(const lvdc_params_t *p,
                         double T_c_sec,
                         double theta_x_rad,
                         double chi_cmd_rad[3]);

/* Evaluate the pitch time-tilt polynomial at `T_c_sec` only.  Public
 * for unit tests; the main entry point is lvdc_preigm_chi_cmd. */
double lvdc_preigm_chi_y(const lvdc_params_t *p, double T_c_sec);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_PREIGM_H */
