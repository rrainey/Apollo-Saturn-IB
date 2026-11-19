/*
 * lvdc_hsl.c — High-Speed Loop for S-IVB cutoff timing
 * (EDD §4.4.2.4 Eqs. 4.4.88 through 4.4.94).
 */

#include "lvdc_hsl.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

/* EDD §4.4.2.4 ¶7 — fire when predicted T_3i drops below 60 ms. */
#define HSL_FIRE_THRESH_SEC   0.060

void lvdc_hsl_reset(lvdc_hsl_state_t *s) {
    if (s == NULL) return;
    memset(s, 0, sizeof(*s));
}

static void push_sample(lvdc_hsl_state_t *s, double V, double t) {
    s->V[2] = s->V[1]; s->t[2] = s->t[1];
    s->V[1] = s->V[0]; s->t[1] = s->t[0];
    s->V[0] = V;       s->t[0] = t;
    if (s->n_samples < 3) s->n_samples++;
}

/* Solve Eq. 4.4.93 for T_3i:
 *   T_3i = (V_T - ΔV_b - V) / (A' + A''·T_3i)
 * ⇒  A''·T_3i² + A'·T_3i - (V_T - ΔV_b - V) = 0
 *
 * A'' ≈ 0 degenerates to the linear form T_3i = ΔV / A'.
 * Returns the smaller positive root, or the signed linear root
 * if A'' is effectively zero. Non-finite inputs or a negative
 * discriminant return the previous value via the caller. */
static double solve_T_3i(double A_prime, double A_dprime,
                         double V_T, double dV_b, double V,
                         double fallback) {
    const double delta_V = V_T - dV_b - V;

    if (fabs(A_dprime) < 1e-12) {
        if (fabs(A_prime) < 1e-12) return fallback;
        return delta_V / A_prime;
    }

    const double disc = A_prime * A_prime + 4.0 * A_dprime * delta_V;
    if (!isfinite(disc) || disc < 0.0) return fallback;

    const double sq = sqrt(disc);
    const double t1 = (-A_prime + sq) / (2.0 * A_dprime);
    const double t2 = (-A_prime - sq) / (2.0 * A_dprime);

    /* Pick the root with smallest magnitude — that is the
     * near-future cutoff. A negative result is valid and means
     * "cutoff is already in the past"; the caller's minor-loop
     * firing threshold handles it. */
    const double a1 = fabs(t1);
    const double a2 = fabs(t2);
    return (a1 <= a2) ? t1 : t2;
}

void lvdc_hsl_bml_step(lvdc_hsl_state_t *s,
                       double now_sec,
                       double V_mps,
                       double T_3i_igm_sec,
                       double T_HSL_sec,
                       double V_GRD_mps,
                       double BN_1_sec,
                       double V_T_mps,
                       double dV_b_mps) {
    if (s == NULL) return;

    /* §4.4.2.4 ¶2 — Start V-history accumulation when the
     * predicted T_3i drops below BN_1. */
    if (!s->accumulating && T_3i_igm_sec <= BN_1_sec) {
        s->accumulating = true;
    }
    if (!s->accumulating) return;

    push_sample(s, V_mps, now_sec);

    /* Eq. 4.4.88 — ΔT_b from the two most recent samples. */
    if (s->n_samples >= 2) {
        const double dT = s->t[0] - s->t[1];
        const double dV = s->V[0] - s->V[1];
        if (dT > 0.0 && fabs(dV) > 0.0) {
            s->delta_T_b_sec = dV_b_mps * dT / dV;
        }
    }

    /* §4.4.2.4 ¶4 — HSL entry condition: predicted T_3i below
     * T_HSL + ΔT_b AND velocity-to-be-gained below V_GRD. */
    if (!s->active) {
        const double vigained = V_T_mps - V_mps;
        if (T_3i_igm_sec < (T_HSL_sec + s->delta_T_b_sec)
            && vigained < V_GRD_mps) {
            s->active = true;
            s->T_3i_hsl_sec = T_3i_igm_sec;  /* seed */
        }
    }

    if (!s->active) return;

    /* §4.4.2.4 ¶5 — First-pass bias correction (Eq. 4.4.89),
     * one-shot. */
    if (!s->bias_applied) {
        s->T_3i_hsl_sec -= s->delta_T_b_sec;
        s->bias_applied = true;
    }

    /* §4.4.2.4 ¶6 — refine T_3i via Eqs. 4.4.91-93 when we have
     * three samples. */
    if (s->n_samples >= 3) {
        const double dT  = s->t[0] - s->t[1];
        const double dTp = s->t[1] - s->t[2];
        const double Vm  = s->V[0];
        const double Vp  = s->V[1];
        const double Vpp = s->V[2];

        if (dT > 0.0 && dTp > 0.0) {
            /* Eq. 4.4.91 — A''. */
            const double num = (Vm - Vp) * (dTp / dT)
                             - (Vp - Vpp) * (dT / dTp);
            const double A_dp = num / (dTp + dT);

            /* Eq. 4.4.92 — A'. */
            const double A_pr = A_dp * dT + (Vm - Vp) / dT;

            /* Eq. 4.4.93 — predicted T_3i. */
            const double refined = solve_T_3i(
                A_pr, A_dp, V_T_mps, dV_b_mps, Vm,
                s->T_3i_hsl_sec);
            if (isfinite(refined)) {
                s->T_3i_hsl_sec = refined;
            }
        }
    }
}

bool lvdc_hsl_minor_step(lvdc_hsl_state_t *s, double dt_sec) {
    if (s == NULL || !s->active) return false;
    if (dt_sec <= 0.0) return false;

    /* §4.4.2.4 ¶7 — decrement T_3i every 40 ms (Eq. 4.4.90). */
    s->T_3i_hsl_sec -= dt_sec;

    /* Fire cutoff edge when T_3i_hsl_sec < 60 ms. */
    return (s->T_3i_hsl_sec < HSL_FIRE_THRESH_SEC);
}

double lvdc_hsl_T_3i(const lvdc_hsl_state_t *s) {
    return (s && s->active) ? s->T_3i_hsl_sec : 0.0;
}

bool lvdc_hsl_active(const lvdc_hsl_state_t *s) {
    return s ? s->active : false;
}

bool lvdc_hsl_accumulating(const lvdc_hsl_state_t *s) {
    return s ? s->accumulating : false;
}
