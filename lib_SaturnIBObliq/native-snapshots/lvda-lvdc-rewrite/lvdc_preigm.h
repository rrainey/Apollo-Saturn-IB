/*
 * lvdc_preigm.h — first-stage guidance (EDD §4.4.1).
 *
 * Generates the pitch, yaw, and roll `chi_cmd_rad[3]` attitude
 * commands from liftoff to IGM initiation. Values are authoritative
 * for AS-205 per TM-X-62831 Appendix B.
 *
 * Scope (Phase C.1):
 *   §4.4.1.1  Roll maneuver. Hold `chi_x` at `chi_x_pad_rad` before
 *             gantry clear, then set to zero.
 *   §4.4.1.2  Pitch time-tilt. Piecewise cubic polynomial in `T`,
 *             held at zero before gantry clear, held at
 *             `P_final_rad` after the final segment.
 *   §4.4.1.3  Yaw time-tilt. Zero throughout S-IB phase per TMX
 *             Appendix B. (EDD Eq. 4.4.2.1 TYAW/YAWC table is an
 *             SL-1 feature and does not apply to AS-205.)
 *
 * Not yet implemented in v1:
 *   §4.4.1.4  Engine-out guidance modifications (DI14, DI15).
 *             Nominal operation is assumed.
 *
 * Time variable: `T_sec` is time from Time Base 1 (first motion)
 * per TM-X-62831 Appendix B. The rewrite does not apply the
 * compute-to-command delay bias `T_d` from EDD Eq. 4.4.1; the TMX
 * coefficients are calibrated for the unbiased time variable.
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
 *   p             mission preset parameters.
 *   T_sec         time from Time Base 1 (first motion) in seconds.
 *                 Zero before liftoff.
 *   chi_cmd_rad[] output: {chi_x, chi_y, chi_z} in radians.
 */
void lvdc_preigm_chi_cmd(const lvdc_params_t *p,
                         double T_sec,
                         double chi_cmd_rad[3]);

/* Evaluate the pitch time-tilt polynomial at `T_sec`. Public for
 * unit tests; the main entry point is lvdc_preigm_chi_cmd. */
double lvdc_preigm_chi_y(const lvdc_params_t *p, double T_sec);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_PREIGM_H */
