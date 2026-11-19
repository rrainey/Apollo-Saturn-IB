/*
 * lvdc_moverf.c — M/F computation and smoothing (EDD §4.2).
 */

#include "lvdc_moverf.h"

#include <math.h>
#include <stddef.h>

static void shift_ring(double ring[4], double newest) {
    ring[3] = ring[2];
    ring[2] = ring[1];
    ring[1] = ring[0];
    ring[0] = newest;
}

void lvdc_moverf_prime(lvdc_moverf_state_t *s, double moverf_0_s2pm) {
    if (s == NULL) return;
    for (int i = 0; i < 4; i++) {
        s->inst[i]   = moverf_0_s2pm;
        s->smooth[i] = moverf_0_s2pm;
    }
    s->V_m_past[0] = s->V_m_past[1] = s->V_m_past[2] = 0.0;
    s->last_out       = moverf_0_s2pm;
    s->primed         = 1;
    s->have_V_m_past  = 0;
}

double lvdc_moverf_step(lvdc_moverf_state_t *s,
                        const double V_m_mps[3],
                        double dT_sec,
                        const double MFK[8],
                        double mag_limit_s2pm,
                        double rate_limit_s2pm) {
    if (s == NULL || V_m_mps == NULL || MFK == NULL) return 0.0;

    /* Prime must have been called; otherwise no filter state. */
    if (!s->primed) {
        return 0.0;
    }

    /* First step after prime: we have no V_m_past. Latch it and
     * return the seeded value. */
    if (!s->have_V_m_past) {
        s->V_m_past[0] = V_m_mps[0];
        s->V_m_past[1] = V_m_mps[1];
        s->V_m_past[2] = V_m_mps[2];
        s->have_V_m_past = 1;
        return s->last_out;
    }

    /* Eq. 4.2.9 — A_m = (V_m - V_m_past) / ΔT. */
    double A_m[3];
    if (dT_sec <= 0.0) return s->last_out;
    for (int i = 0; i < 3; i++) {
        A_m[i] = (V_m_mps[i] - s->V_m_past[i]) / dT_sec;
    }

    /* Eq. 4.2.10 — F/M = |A_m|. */
    const double FoverM = sqrt(A_m[0]*A_m[0]
                             + A_m[1]*A_m[1]
                             + A_m[2]*A_m[2]);
    if (FoverM <= 0.0) {
        /* No thrust detected. Latch V_m_past and return last. */
        for (int i = 0; i < 3; i++) s->V_m_past[i] = V_m_mps[i];
        return s->last_out;
    }

    /* (M/F)_inst = 1 / (F/M). */
    const double MoverF_inst = 1.0 / FoverM;

    /* Shift the instantaneous ring so [0] is current, then build
     * (M/F)_S from Eq. 4.2.11. */
    shift_ring(s->inst, MoverF_inst);

    double out = MFK[0] * s->inst[0]
               + MFK[1] * s->inst[1]
               + MFK[2] * s->inst[2]
               + MFK[3] * s->inst[3]
               + MFK[4] * s->smooth[0]
               + MFK[5] * s->smooth[1]
               + MFK[6] * s->smooth[2]
               + MFK[7] * s->smooth[3];

    /* Magnitude clamp (EDD §4.2.5). */
    if (mag_limit_s2pm > 0.0) {
        if (out >  mag_limit_s2pm) out =  mag_limit_s2pm;
        if (out < -mag_limit_s2pm) out = -mag_limit_s2pm;
    }

    /* Rate clamp (EDD §4.2.5). */
    if (rate_limit_s2pm > 0.0) {
        const double max_delta = rate_limit_s2pm;
        const double delta = out - s->last_out;
        if (delta >  max_delta) out = s->last_out + max_delta;
        if (delta < -max_delta) out = s->last_out - max_delta;
    }

    /* Shift the smoothed ring and latch state. */
    shift_ring(s->smooth, out);
    s->last_out = out;

    for (int i = 0; i < 3; i++) s->V_m_past[i] = V_m_mps[i];

    return out;
}
