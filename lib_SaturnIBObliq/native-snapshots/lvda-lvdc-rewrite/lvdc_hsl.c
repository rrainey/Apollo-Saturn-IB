/*
 * lvdc_hsl.c — High-Speed Loop for S-IVB cutoff timing
 * (EDD §4.4.2.4 Eqs. 4.4.88 through 4.4.94).
 */

#include "lvdc_hsl.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

void lvdc_hsl_reset(lvdc_hsl_state_t *s) {
    if (s == NULL) return;
    memset(s, 0, sizeof(*s));
}

/* Shift the 3-sample ring so the newest value lands at index 0. */
static void push_sample(lvdc_hsl_state_t *s, double V, double t) {
    s->V[2] = s->V[1]; s->t[2] = s->t[1];
    s->V[1] = s->V[0]; s->t[1] = s->t[0];
    s->V[0] = V;       s->t[0] = t;
    if (s->n_samples < 3) s->n_samples++;
}

/* Solve Eq. 4.4.93 for T_3i:
 *   T_3i = (Vt - dV_b - V) / (A' + A'' * T_3i)
 * rearranges to A'' * T_3i^2 + A' * T_3i - (Vt - dV_b - V) = 0.
 *
 * If A'' is effectively zero the solve degenerates to the linear
 * form T_3i = (Vt - dV_b - V) / A'. Returns the positive root
 * (the physically meaningful one — time-to-go must be positive). */
static double solve_T_3i(double A_prime, double A_dprime,
                         double V_T, double dV_b, double V) {
    const double delta_V = V_T - dV_b - V;

    /* Linear case. */
    if (fabs(A_dprime) < 1e-12) {
        if (A_prime <= 0.0) return 0.0;
        return delta_V / A_prime;
    }

    /* Quadratic: A'' T^2 + A' T - delta_V = 0. */
    const double disc = A_prime * A_prime + 4.0 * A_dprime * delta_V;
    if (disc < 0.0) return 0.0;

    const double sq = sqrt(disc);
    const double t1 = (-A_prime + sq) / (2.0 * A_dprime);
    const double t2 = (-A_prime - sq) / (2.0 * A_dprime);

    /* Pick the smaller positive root — the near-future cutoff. */
    double best = -1.0;
    if (t1 > 0.0 && (best < 0.0 || t1 < best)) best = t1;
    if (t2 > 0.0 && (best < 0.0 || t2 < best)) best = t2;
    return (best > 0.0) ? best : 0.0;
}

bool lvdc_hsl_step(lvdc_hsl_state_t *s,
                   double now_sec,
                   double V_mps,
                   double T_3i_igm_sec,
                   double eps_prime_sec,
                   double V_T_mps,
                   double dV_b_mps) {
    if (s == NULL) return false;

    /* HSL entry latch. */
    if (!s->active) {
        if (T_3i_igm_sec > 0.0 && T_3i_igm_sec > eps_prime_sec) {
            /* Not yet — stay idle. */
            return false;
        }
        s->active = true;
        s->T_3i_hsl_sec = T_3i_igm_sec;  /* seed from IGM */
    }

    /* Capture this sample. */
    push_sample(s, V_mps, now_sec);

    /* First HSL cycle after entry: apply Eq. 4.4.88 bias once we
     * have two samples. Needs (V - V') / ΔT. */
    if (!s->bias_applied && s->n_samples >= 2) {
        const double dT = s->t[0] - s->t[1];
        const double dV = s->V[0] - s->V[1];
        if (dT > 0.0 && dV != 0.0) {
            /* ΔT_b = ΔV_b · ΔT / (V - V'). */
            const double dT_b = dV_b_mps * dT / dV;
            s->T_3i_hsl_sec -= dT_b;  /* Eq. 4.4.89 */
            s->bias_applied = true;
        }
    }

    /* Each HSL cycle: decrement T_3i by ΔT (Eq. 4.4.90). */
    if (s->n_samples >= 2) {
        const double dT = s->t[0] - s->t[1];
        s->T_3i_hsl_sec -= dT;       /* Eq. 4.4.90 */
    }

    /* With three samples, refine T_3i via the quadratic predictor
     * (Eqs. 4.4.91-93). */
    if (s->n_samples >= 3) {
        const double dT    = s->t[0] - s->t[1];
        const double dTp   = s->t[1] - s->t[2];  /* ΔT' */
        const double Vm    = s->V[0];
        const double Vp    = s->V[1];
        const double Vpp   = s->V[2];

        if (dT > 0.0 && dTp > 0.0) {
            /* Eq. 4.4.91 — A''. The EDD denominator has ΔT'/ΔT
             * and ΔT/ΔT'; transcribed directly. */
            const double num = (Vm - Vp) * (dTp / dT)
                             - (Vp - Vpp) * (dT / dTp);
            const double A_dp = num / (dTp + dT);

            /* Eq. 4.4.92 — A'. */
            const double A_pr = A_dp * dT + (Vm - Vp) / dT;

            /* Eq. 4.4.93 — predict T_3i. */
            const double refined =
                solve_T_3i(A_pr, A_dp, V_T_mps, dV_b_mps, Vm);
            if (isfinite(refined) && refined >= 0.0) {
                s->T_3i_hsl_sec = refined;
            }
        }
    }

    /* Cutoff edge: when the prediction crosses zero (we've
     * reached the velocity target), fire. Edge is one-shot. */
    s->cutoff_edge = false;
    if (s->T_3i_hsl_sec <= 0.0) {
        s->cutoff_edge = true;
    }
    return s->cutoff_edge;
}

double lvdc_hsl_T_3i(const lvdc_hsl_state_t *s) {
    return (s && s->active) ? s->T_3i_hsl_sec : 0.0;
}

bool lvdc_hsl_active(const lvdc_hsl_state_t *s) {
    return s ? s->active : false;
}
