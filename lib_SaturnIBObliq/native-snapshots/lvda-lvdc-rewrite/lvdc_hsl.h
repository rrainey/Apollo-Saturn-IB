/*
 * lvdc_hsl.h — High-Speed Loop for S-IVB cutoff timing
 * (EDD §4.4.2.4 Eqs. 4.4.88 through 4.4.94).
 *
 * The EDD narrative (§4.4.2.4) prescribes a two-rate structure:
 *
 *   1. V-history accumulation and Eqs. 4.4.88-94 run at BML
 *      rate once the predicted T_3i drops below BN_1.
 *   2. HSL entry latches when predicted T_3i < (T_HSL + ΔT_b)
 *      AND the velocity-to-be-gained (V_T - |V_S|) is less than
 *      the velocity guard V_GRD.
 *   3. After entry, T_3i is decremented and tested every 40 ms
 *      (minor-loop rate). Cutoff fires when T_3i < 60 ms.
 *
 * Call lvdc_hsl_bml_step once per BML — it maintains the
 * 3-sample V history, computes ΔT_b (Eq. 4.4.88), checks entry,
 * applies the first-pass Eq. 4.4.89 bias, and refines T_3i via
 * Eqs. 4.4.91-93.
 *
 * Call lvdc_hsl_minor_step once per minor loop (nominally every
 * 40 ms). Once HSL is active, it decrements T_3i_hsl_sec by
 * `dt_sec` and returns true when T_3i_hsl_sec < 60 ms — the
 * caller consumes this as the one-shot cutoff edge for the
 * time-base state machine.
 */

#ifndef LVDC_HSL_H
#define LVDC_HSL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool   accumulating;     /* V-history accumulation started */
    bool   active;           /* HSL entry latched */
    bool   bias_applied;     /* Eq. 4.4.89 one-shot done */

    /* V sample ring at BML rate, newest at [0]. */
    double V[3];
    double t[3];
    int    n_samples;

    /* Last ΔT_b from Eq. 4.4.88. */
    double delta_T_b_sec;

    /* Running HSL prediction of T_3i. BML pass refines via
     * Eqs. 4.4.91-93; minor-loop pass decrements by dt. */
    double T_3i_hsl_sec;
} lvdc_hsl_state_t;

/* Reset state at init. */
void lvdc_hsl_reset(lvdc_hsl_state_t *s);

/* BML-rate step. Runs the §4.4.2.4 equations:
 *
 *   now_sec       : model->time at this BML.
 *   V_mps         : current |V_S| magnitude (Eq. 4.3.5.1).
 *   T_3i_igm_sec  : T_3i from the IGM mode machine. Drives both
 *                   accumulation (T_3i <= BN_1) and entry
 *                   (T_3i < T_HSL + ΔT_b) tests.
 *   T_HSL_sec     : preset entry window (TMX: 5 s).
 *   V_GRD_mps     : velocity-guard for entry (TMX: 150 m/s).
 *   BN_1_sec      : accumulation threshold (TMX: 14.4 s).
 *   V_T_mps       : target terminal velocity.
 *   dV_b_mps      : ΔV_b bias (TMX Part II Table 1-3: 7.2381 m/s).
 */
void lvdc_hsl_bml_step(lvdc_hsl_state_t *s,
                       double now_sec,
                       double V_mps,
                       double T_3i_igm_sec,
                       double T_HSL_sec,
                       double V_GRD_mps,
                       double BN_1_sec,
                       double V_T_mps,
                       double dV_b_mps);

/* Minor-loop-rate step (nominally 40 ms per EDD §4.4.2.4). While
 * HSL is active, decrement T_3i_hsl_sec by dt_sec and return
 * true when T_3i_hsl_sec < 60 ms — the dispatcher latches that
 * as the S-IVB cutoff edge. No-op when HSL is not yet active. */
bool lvdc_hsl_minor_step(lvdc_hsl_state_t *s, double dt_sec);

/* Read helpers. */
double lvdc_hsl_T_3i(const lvdc_hsl_state_t *s);
bool   lvdc_hsl_active(const lvdc_hsl_state_t *s);
bool   lvdc_hsl_accumulating(const lvdc_hsl_state_t *s);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_HSL_H */
