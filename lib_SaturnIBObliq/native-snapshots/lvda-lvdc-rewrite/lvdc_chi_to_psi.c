/*
 * lvdc_chi_to_psi.c — gimbal-to-body attitude-error transform
 * (EDD §8.3.2 + Eq. 8.2.2).
 */

#include "lvdc_chi_to_psi.h"

#include <math.h>

void lvdc_chi_to_psi_transform(const double chi_prime_rad[3],
                               const double theta_prime_rad[3],
                               double psi_rad_out[3]) {
    /* Platform-frame attitude error (EDD Eq. 8.2.2, Delta terms). */
    const double dX = theta_prime_rad[0] - chi_prime_rad[0];
    const double dY = theta_prime_rad[1] - chi_prime_rad[1];
    const double dZ = theta_prime_rad[2] - chi_prime_rad[2];

    /* Predicted average roll and yaw angles (EDD Eqs. 8.3.3, 8.3.4).
     * The EDD defines `chi'_xp` as the predicted end-of-cycle value
     * (Eq. 8.3.2). The rewrite uses the continuous-rate equivalent:
     * substitute `chi'_x` for `chi'_xp`. The current hand-edited
     * body uses the same convention. */
    const double theta_xa = 0.5 * (chi_prime_rad[0] + theta_prime_rad[0]);
    const double theta_za = 0.5 * (chi_prime_rad[2] + theta_prime_rad[2]);

    const double sin_xa = sin(theta_xa);
    const double cos_xa = cos(theta_xa);
    const double sin_za = sin(theta_za);
    const double cos_za = cos(theta_za);

    /* Transform coefficients (EDD Eqs. 8.3.5-8.3.10, with SF = 1
     * so the output stays in radians). The `lvdc_dac` stage applies
     * the Saturn V D/A LSB (0.0575 deg per spc_stnv_000130). */
    const double A1 = cos_xa * cos_za;  /* Eq. 8.3.5 */
    const double A2 = sin_xa;           /* Eq. 8.3.6 */
    const double A3 = sin_za;           /* Eq. 8.3.7 */
    const double A4 = sin_xa * cos_za;  /* Eq. 8.3.8 */
    const double A5 = cos_xa;           /* Eq. 8.3.9 */
    const double A6 = 1.0;              /* Eq. 8.3.10 */

    /* Body-frame attitude error (EDD Eq. 8.2.2 ROLL, PITCH, YAW). */
    psi_rad_out[0] = A6 * dX + A3 * dY;   /* ROLL  */
    psi_rad_out[1] = A1 * dY + A2 * dZ;   /* PITCH */
    psi_rad_out[2] = A5 * dZ - A4 * dY;   /* YAW   */
}
