/*
 * lvdc_moverf.h — M/F computation and smoothing (EDD §4.2).
 *
 * The IGM needs the smoothed reciprocal specific force (M/F)_S
 * (Eqs. 4.4.6-7). The LVDC derives it from the per-BML change in
 * the sensed-velocity accumulator and runs it through an 8-tap
 * IIR filter (Eq. 4.2.11) that uses the four most recent
 * instantaneous values and the four most recent smoothed values:
 *
 *   A_m        = (V_m - V_m_past) / ΔT                (Eq. 4.2.9)
 *   F/M        = |A_m|                                 (Eq. 4.2.10)
 *   (M/F)_1    = 1 / (F/M)                             (per-BML inst)
 *   (M/F)_S    = Σ MFK[i] * samples[i], i = 0..7       (Eq. 4.2.11)
 *
 * After the sum the output is clamped to
 *   |(M/F)_S| ≤ MoverF_s_limit_s2pm                    (EDD §4.2.5)
 * and rate-limited to
 *   |(M/F)_S - last| ≤ MoverF_s_rate_limit_per_BML     (EDD §4.2.5)
 *
 * Pre-T3+0 the LVDC uses (M/F)_0 instead of the filter. The
 * dispatcher calls `lvdc_moverf_prime(..., (M/F)_0)` once before
 * the first live BML to seed every ring entry with (M/F)_0.
 *
 * The AS-205 TMX Table 1-3 S-IVB MFK coefficients are
 *   MFK1..4 : 0.0120948, 0.0213339, -0.0261577, -0.00526684
 *   MFK5..8 : 2.80483, -2.85880, 1.24933, -0.197360
 * (M/F)_0  : 0.135 s^2/m.
 */

#ifndef LVDC_MOVERF_H
#define LVDC_MOVERF_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* Four most recent instantaneous (M/F)_1..4, newest at [0]. */
    double inst[4];
    /* Four most recent smoothed    (M/F)_S1..4, newest at [0]. */
    double smooth[4];
    /* V_m from the previous BML (m/s), used to compute A_m. */
    double V_m_past[3];
    /* Last output, for rate limiting. */
    double last_out;
    /* 1 after the first prime, 0 before. */
    int    primed;
    /* 1 after at least one step past prime, so V_m_past is valid. */
    int    have_V_m_past;
} lvdc_moverf_state_t;

/* Seed the filter rings with `moverf_0_s2pm` and reset V_m_past.
 * Called once at T3+0 before the first live step. */
void lvdc_moverf_prime(lvdc_moverf_state_t *s, double moverf_0_s2pm);

/* Run one BML step. Returns (M/F)_S in s^2/m.
 *
 *   V_m_mps           : current V_m vector (m/s).
 *   dT_sec            : BML period (s).
 *   MFK[8]            : filter coefficients.
 *   mag_limit_s2pm    : magnitude clamp, EDD §4.2.5.
 *   rate_limit_s2pm   : rate clamp per BML, EDD §4.2.5.
 *
 * On the first step after priming `V_m_past` is unknown, so the
 * step reuses the seeded ring contents and simply sets V_m_past
 * from the current input. It returns `moverf_0_s2pm` so the
 * dispatcher gets a sane value on the first tick. */
double lvdc_moverf_step(lvdc_moverf_state_t *s,
                        const double V_m_mps[3],
                        double dT_sec,
                        const double MFK[8],
                        double mag_limit_s2pm,
                        double rate_limit_s2pm);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_MOVERF_H */
