/*
 * lvdc_preigm.c — first-stage guidance (EDD §4.4.1).
 */

#include "lvdc_preigm.h"

#include <math.h>

/* Horner evaluation of `a0 + a1·t + a2·t² + a3·t³`. */
static double eval_cubic(const double a[4], double t) {
    return a[0] + t * (a[1] + t * (a[2] + t * a[3]));
}

double lvdc_preigm_chi_y(const lvdc_params_t *p, double T_sec) {
    /* Segment selection per EDD §4.4.1.2 Eq. 4.4.2, with AS-205
     * coefficients from TM-X-62831 Appendix B. */
    if (T_sec <= p->T_S1_sec1_sec) {
        return 0.0;
    }
    if (T_sec <= p->T_S1_sec) {
        return eval_cubic(p->chi_y_F1, T_sec);
    }
    if (T_sec <= p->T_S2_sec) {
        return eval_cubic(p->chi_y_F2, T_sec);
    }
    if (T_sec <= p->T_S3_sec) {
        return eval_cubic(p->chi_y_F3, T_sec);
    }
    return p->P_final_rad;
}

void lvdc_preigm_chi_cmd(const lvdc_params_t *p,
                         double T_sec,
                         double chi_cmd_rad[3]) {
    /* chi_x (roll) — TM-X-62831 Appendix B.
     *   T ≤ 10 s : chi_x = chi_x_pad_rad   (= −0.488692 rad)
     *   T > 10 s : chi_x = 0
     *
     * EDD §4.4.1.1 defines the primary trigger as X_S − initial_X_S
     * ≥ GANTRY (138 m per EDD Part II), with T ≥ T_S0 as the
     * backup. The rewrite uses the time gate only; the GANTRY
     * discrete lands with the scheduler. */
    chi_cmd_rad[0] = (T_sec <= p->T_S1_sec1_sec) ? p->chi_x_pad_rad : 0.0;

    /* chi_y (pitch) — EDD §4.4.1.2 Eq. 4.4.2. */
    chi_cmd_rad[1] = lvdc_preigm_chi_y(p, T_sec);

    /* chi_z (yaw) — TM-X-62831 Appendix B: zero throughout the S-IB
     * phase for AS-205. The EDD Eq. 4.4.2.1 TYAW/YAWC table lookup
     * is an SL-1 feature (EDD Part II Rev A, 1972) and does not
     * apply to AS-205. */
    chi_cmd_rad[2] = 0.0;
}
