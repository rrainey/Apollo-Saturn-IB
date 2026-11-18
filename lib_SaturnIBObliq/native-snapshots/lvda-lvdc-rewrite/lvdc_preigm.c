/*
 * lvdc_preigm.c — first-stage guidance (EDD §4.4.1).
 */

#include "lvdc_preigm.h"

#include <math.h>

/* Horner evaluation of `a0 + a1·t + a2·t² + a3·t³`. */
static double eval_cubic(const double a[4], double t) {
    return a[0] + t * (a[1] + t * (a[2] + t * a[3]));
}

double lvdc_preigm_chi_y(const lvdc_params_t *p, double T_c_sec) {
    /* Segment selection per EDD §4.4.1.2.  The polynomial argument
     * is clamped to [T_S1_sec1, T_S3] so the output stays physical
     * if the caller passes T_c outside the active window. */
    if (T_c_sec <= p->T_S1_sec1_sec) {
        return 0.0;
    }

    const double t = (T_c_sec < p->T_S1_sec1_sec) ? p->T_S1_sec1_sec
                   : (T_c_sec > p->T_S3_sec)      ? p->T_S3_sec
                                                  : T_c_sec;

    if (T_c_sec <= p->T_S1_sec) {
        return eval_cubic(p->chi_y_F1, t);
    }
    if (T_c_sec <= p->T_S2_sec) {
        return eval_cubic(p->chi_y_F2, t);
    }
    if (T_c_sec <= p->T_S3_sec) {
        return eval_cubic(p->chi_y_F3, t);
    }
    return p->P_final_rad;
}

void lvdc_preigm_chi_cmd(const lvdc_params_t *p,
                         double T_c_sec,
                         double theta_x_rad,
                         double chi_cmd_rad[3]) {
    /* chi_x (roll) — EDD §4.4.1.1.
     * Before gantry clear: hold the stored pad roll angle.
     * After gantry clear: zero.  The rewrite uses the time gate
     * only; the X_S > GANTRY primary trigger is deferred until the
     * scheduler owns the gantry-clear discrete. */
    chi_cmd_rad[0] = (T_c_sec <= p->T_S1_sec1_sec) ? theta_x_rad : 0.0;

    /* chi_y (pitch) — EDD §4.4.1.2. */
    chi_cmd_rad[1] = lvdc_preigm_chi_y(p, T_c_sec);

    /* chi_z (yaw) — EDD §4.4.1.3 table lookup.  Phase C.1 ties this
     * to zero; the full Eq. 4.4.2.1 TYAW/YAWC table lookup lands
     * with lvdc_igm. */
    chi_cmd_rad[2] = 0.0;
}
