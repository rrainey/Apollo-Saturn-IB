/*
 * lvdc_chi_to_psi.h — gimbal-to-body attitude-error transform
 * (EDD §8.3.2 + Eq. 8.2.2).
 *
 * The LVDC computes the attitude error in the platform (gimbal)
 * frame and transforms it into the vehicle body frame. The
 * resulting body-frame error is what drives the Flight Control
 * Computer.
 *
 * The EDD defines six transform coefficients `A_1..A_6` that depend
 * on the predicted average roll and yaw angles for the next
 * computation cycle (Eqs. 8.3.2-8.3.10). The transform itself
 * (Eq. 8.2.2) is a linear combination of the per-axis attitude
 * errors:
 *
 *   Delta_X = theta'_x - chi'_x
 *   Delta_Y = theta'_y - chi'_y
 *   Delta_Z = theta'_z - chi'_z
 *
 *   ROLL  = A_6 * Delta_X  + A_3 * Delta_Y
 *   PITCH = A_1 * Delta_Y  + A_2 * Delta_Z
 *   YAW   = A_5 * Delta_Z  - A_4 * Delta_Y
 *
 * where
 *
 *   theta_xa = 1/2 * (chi'_xp + theta'_x)      (Eq. 8.3.3)
 *   theta_za = 1/2 * (chi'_zp + theta'_z)      (Eq. 8.3.4)
 *
 *   A_1 = SF * cos(theta_xa) * cos(theta_za)   (Eq. 8.3.5)
 *   A_2 = SF * sin(theta_xa)                   (Eq. 8.3.6)
 *   A_3 = SF * sin(theta_za)                   (Eq. 8.3.7)
 *   A_4 = SF * sin(theta_xa) * cos(theta_za)   (Eq. 8.3.8)
 *   A_5 = SF * cos(theta_xa)                   (Eq. 8.3.9)
 *   A_6 = SF                                   (Eq. 8.3.10)
 *
 * The EDD defines `chi'_xp` as the predicted value of `chi'_x` at
 * the end of the next minor-loop support cycle (Eq. 8.3.2). The EDD
 * computes this prediction from the increment Δχ' and the number
 * of minor loops per support cycle.
 *
 * The rewrite uses the continuous-rate approximation (same shape as
 * the current hand-edited body): it reads `chi'` at every
 * fundamental step. For the average-angle computation of Eqs.
 * 8.3.3-8.3.4 we therefore substitute `chi'_x` (current rate-limited
 * value) for `chi'_xp`. Under the no-saturation limit this converges
 * to the EDD form. During saturation it is a first-order
 * approximation that stays closer to the physical attitude than the
 * EDD two-rate form predicts.
 *
 * Scale factor: EDD §8.3.2 uses `SF = 3000 bits/pirad` to produce
 * ladder-bit output. The rewrite keeps everything in radians
 * (effective `SF = 1`) and applies the Saturn V D/A LSB
 * quantization in the next pipeline stage (`lvdc_dac`).
 *
 * Index convention: `[0, 1, 2] = (roll, pitch, yaw)`.
 */

#ifndef LVDC_CHI_TO_PSI_H
#define LVDC_CHI_TO_PSI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Transform platform-frame attitude error into body-frame error.
 *
 *   chi_prime_rad  current rate-limited minor-loop command. Used
 *                  both in the Eq. 8.2.2 error (`theta' - chi'`)
 *                  and in the Eq. 8.3.3-8.3.4 average-angle
 *                  computation. Index convention (roll, pitch, yaw).
 *   theta_prime_rad current gimbal angle from the ST-124M resolver.
 *   psi_rad_out    body-frame attitude error. Output in radians,
 *                  roll-pitch-yaw indexing. The caller applies the
 *                  rate limit, magnitude saturation and D/A LSB
 *                  quantization through `lvdc_dac`.
 */
void lvdc_chi_to_psi_transform(const double chi_prime_rad[3],
                               const double theta_prime_rad[3],
                               double psi_rad_out[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_CHI_TO_PSI_H */
