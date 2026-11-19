/*
 * lvdc_hsl.h — High-Speed Loop for S-IVB cutoff timing
 * (EDD §4.4.2.4 Eqs. 4.4.88 through 4.4.94).
 *
 * Near S-IVB cutoff the LVDC switches from the 1.6 s BML cadence
 * to a faster sampling loop. The HSL:
 *   1. Captures three successive V samples (|V_S|) at the HSL
 *      rate.
 *   2. Fits a quadratic in time to extract A' (velocity rate) and
 *      A'' (quadratic coefficient).
 *   3. Predicts the remaining time to cutoff via the implicit
 *      Eq. 4.4.93 solve.
 *   4. Fires a cutoff edge when the prediction crosses zero.
 *
 * Entry condition (TMX Table 3B):
 *   T_3i <= eps_prime (= 5 s on AS-205)
 *
 * On the first HSL cycle Eq. 4.4.88 applies a one-shot ΔT_b
 * correction to T_3i; subsequent cycles decrement T_3i by ΔT
 * (Eq. 4.4.90) and refine with Eq. 4.4.93.
 *
 * This implementation runs at BML rate (1.6 s). The real LVDC
 * samples HSL faster (~10 Hz is typical), which gives better
 * cutoff timing resolution. Call-site rate is a follow-on
 * refinement.
 */

#ifndef LVDC_HSL_H
#define LVDC_HSL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool   active;          /* true once entry latched */
    bool   bias_applied;    /* Eq. 4.4.88 one-shot done */

    /* V sample ring, newest at [0]. */
    double V[3];
    double t[3];            /* model->time at each sample */
    int    n_samples;       /* 0..3 */

    /* Running T_3i prediction. Decremented each HSL cycle and
     * refined via Eq. 4.4.93 once we have three samples. */
    double T_3i_hsl_sec;

    /* Latched cutoff edge — set true once the prediction crosses
     * zero. The dispatcher consumes this and clears to false the
     * same cycle (edge semantics). */
    bool   cutoff_edge;
} lvdc_hsl_state_t;

/* Reset state at init. */
void lvdc_hsl_reset(lvdc_hsl_state_t *s);

/* Run one HSL step.
 *
 *   now_sec        : model->time at this call.
 *   V_mps          : current |V_S| magnitude.
 *   T_3i_igm_sec   : T_3i from the IGM mode machine (used only
 *                    on first entry to decide whether we should
 *                    be in HSL and to seed the HSL T_3i).
 *   eps_prime_sec  : entry threshold (TMX AS-205: 5.0 s).
 *   V_T_mps        : target terminal velocity.
 *   dV_b_mps       : velocity-bias constant (TMX Part II: 7.2381).
 *
 * Returns true if HSL fired a cutoff edge on this call (edge
 * semantics — caller latches the persistent flag). */
bool lvdc_hsl_step(lvdc_hsl_state_t *s,
                   double now_sec,
                   double V_mps,
                   double T_3i_igm_sec,
                   double eps_prime_sec,
                   double V_T_mps,
                   double dV_b_mps);

/* Read helpers for logging / trace. */
double lvdc_hsl_T_3i(const lvdc_hsl_state_t *s);
bool   lvdc_hsl_active(const lvdc_hsl_state_t *s);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_HSL_H */
