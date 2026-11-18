/*
 * lvdc_nav.c — boost-phase navigation (EDD §4.2 and §4.3).
 *
 * Implements:
 *   §3.4  Eq. 3.4.3-4   Flight-program initialization at GRR
 *   §4.2.3 Eq. 4.2.7-8  Velocity accumulation (pulse → m/s)
 *   §4.2.4 Eq. 4.2.8-10 F/M calculation
 *   §4.2.5 Eq. 4.2.11   M/F smoothing (4th-order recursive filter,
 *                       with 0.25 s^2/m magnitude limit and 0.005
 *                       per BML rate limit)
 *   §4.3.1 Eq. 4.3.1-5  Trapezoidal integration of position and
 *                       velocity in the plumbline frame
 *   §4.3.2 Eq. 4.3.6-12 Gravitational acceleration (point-mass
 *                       stub in Phase B.2; full J2 oblate in B.3)
 *
 * Simplifications in v1 (documented in docs/LVDC_REWRITE_PLAN.md):
 *   - A/B disagreement, zero, reasonableness tests (EDD §4.2.2) are
 *     skipped. The plant delivers one deterministic quantized value.
 *   - (F/M)_c backup acceleration (EDD §4.2.6) is skipped — not
 *     triggered unless an accel reading fails the §4.2.2 tests.
 *   - BML cadence is treated as nominal 1.6 s; pass-to-pass jitter
 *     (EDD §4.1) is not modelled.
 */

#include "lvdc_nav.h"

#include "lvdc_quant.h"

#include <math.h>
#include <string.h>

/* M/F smoothing filter coefficients, EDD §4.2.5 Eq. 4.2.11. The EDD
 * states the filter is a 4th-order difference equation using four
 * past M/F and four past (M/F)_s values. The coefficient table is in
 * EDD Part II; placeholder until Phase C wires in the full filter.
 * For Phase B we use a simple rolling 4-point average of M/F as a
 * conservative stand-in — enough to keep the nav integration path
 * exercised without depending on the full filter. */
static double lvdc_nav_smooth_MoverF(lvdc_sensor_state_t *sens,
                                     const lvdc_params_t *p,
                                     double MoverF_now) {
    /* Shift history: hist[0] = current, hist[1..3] = past */
    sens->MoverF_hist[3] = sens->MoverF_hist[2];
    sens->MoverF_hist[2] = sens->MoverF_hist[1];
    sens->MoverF_hist[1] = sens->MoverF_hist[0];
    sens->MoverF_hist[0] = MoverF_now;

    /* Phase B stand-in: 4-point average. Replace with EDD Eq. 4.2.11
     * when Part II coefficients are wired. */
    double smoothed = 0.25 * (sens->MoverF_hist[0] +
                              sens->MoverF_hist[1] +
                              sens->MoverF_hist[2] +
                              sens->MoverF_hist[3]);

    /* Magnitude limit (EDD §4.2.5). */
    const double mag_lim = p->MoverF_s_limit_s2pm;
    if (mag_lim > 0.0) {
        if (smoothed >  mag_lim) smoothed =  mag_lim;
        if (smoothed < -mag_lim) smoothed = -mag_lim;
    }

    /* Rate-of-change limit (EDD §4.2.5). */
    const double rate_lim = p->MoverF_s_rate_limit_per_BML;
    if (rate_lim > 0.0) {
        const double prev = sens->MoverF_s_hist[0];
        const double delta = smoothed - prev;
        if (delta >  rate_lim) smoothed = prev + rate_lim;
        if (delta < -rate_lim) smoothed = prev - rate_lim;
    }

    /* Shift (M/F)_s history. */
    sens->MoverF_s_hist[3] = sens->MoverF_s_hist[2];
    sens->MoverF_s_hist[2] = sens->MoverF_s_hist[1];
    sens->MoverF_s_hist[1] = sens->MoverF_s_hist[0];
    sens->MoverF_s_hist[0] = smoothed;

    return smoothed;
}

/* --------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------ */

void lvdc_nav_reset(lvdc_state_t *s) {
    memset(s, 0, sizeof(*s));
    s->last_compute_tick = (uint64_t)-1;
    s->last_memory_tick  = (uint64_t)-1;
}

void lvdc_nav_compute_initial_conditions(const lvdc_params_t *p,
                                         double X_0_eq343[3],
                                         double V_0_eq344[3]) {
    /* EDD Eq. 3.4.3 — position at GRR in plumbline frame:
     *   X_0 = (R_L·cos δ,  R_L·sin δ·sin A_z,  -R_L·sin δ·cos A_z)
     *   where δ = φ_L − φ'_L (geodetic − geocentric latitude)
     *
     * EDD Eq. 3.4.4 — earth-rotation velocity seed:
     *   V_0 = (0,  ω_E·R_L·cos φ'_L·cos A_z,  ω_E·R_L·cos φ'_L·sin A_z)
     */
    const double deg2rad = M_PI / 180.0;
    const double delta_rad   = (p->phi_L_deg - p->phi_L_prime_deg) * deg2rad;
    const double A_z_rad     = p->A_z_deg * deg2rad;
    const double phi_pr_rad  = p->phi_L_prime_deg * deg2rad;
    const double sin_d = sin(delta_rad);
    const double cos_d = cos(delta_rad);
    const double sin_A = sin(A_z_rad);
    const double cos_A = cos(A_z_rad);
    const double cos_pr = cos(phi_pr_rad);

    X_0_eq343[0] =  p->R_L_m * cos_d;
    X_0_eq343[1] =  p->R_L_m * sin_d * sin_A;
    X_0_eq343[2] = -p->R_L_m * sin_d * cos_A;

    const double omega_R_cospr = p->omega_E_radps * p->R_L_m * cos_pr;
    V_0_eq344[0] = 0.0;
    V_0_eq344[1] = omega_R_cospr * cos_A;
    V_0_eq344[2] = omega_R_cospr * sin_A;
}

void lvdc_nav_grr_latch(lvdc_state_t *s,
                        const double V_m_bar_mps_now[3],
                        const double X_0_eq343[3],
                        const double V_0_eq344[3]) {
    /* EDD §4.2.3 — accumulator initialized to zero at GRR, which we
     * realise by snapshotting the current platform velocity as an
     * offset and subtracting it on every subsequent read. */
    for (int i = 0; i < 3; i++) {
        s->sensor.V_m_mps_offset[i] = V_m_bar_mps_now[i];
        s->sensor.V_m_mps_prev[i]   = 0.0;
        s->sensor.V_m_mps[i]        = 0.0;
        s->sensor.V_m_pulses[i]     = 0;
        s->sensor.FoverM_axis[i]    = 0.0;

        s->nav.X_S[i]           = X_0_eq343[i];
        s->nav.V_Sg[i]          = 0.0;
        s->nav.G_S[i]           = 0.0;
        s->nav.G_S_prev[i]      = 0.0;
        s->nav.V_m_mps_past[i]  = 0.0;

        s->sensor.X_S_init[i] = X_0_eq343[i];
        s->sensor.V_S_init[i] = V_0_eq344[i];
    }
    s->sensor.FoverM          = 0.0;
    s->sensor.MoverF_s        = 0.0;
    s->sensor.t_prev_BML_sec  = 0.0;
    s->sensor.dT_BML_sec      = 0.0;

    /* Initialize M/F histories to (M/F)_0 per EDD §4.2.5. */
    for (int k = 0; k < 4; k++) {
        s->sensor.MoverF_hist[k]   = 0.0; /* (M/F)_0 populated once we have thrust */
        s->sensor.MoverF_s_hist[k] = 0.0;
    }

    s->sensor.GRR_latched = true;
}

void lvdc_nav_compute_gravity(const double X_S[3],
                              const lvdc_params_t *p,
                              double G_S_out[3]) {
    /* Phase B.2 stub: point-mass central gravity.
     *   G_S = -μ · X_S / r^3,   r = |X_S|
     * Phase B.3 will replace this with the EDD §4.3.2 J2 oblate
     * expansion (Eqs. 4.3.6-12) without changing the signature. */
    const double r2 = X_S[0]*X_S[0] + X_S[1]*X_S[1] + X_S[2]*X_S[2];
    if (r2 <= 0.0) {
        G_S_out[0] = G_S_out[1] = G_S_out[2] = 0.0;
        return;
    }
    const double r = sqrt(r2);
    const double gfac = -p->mu_earth_m3ps2 / (r2 * r);
    G_S_out[0] = gfac * X_S[0];
    G_S_out[1] = gfac * X_S[1];
    G_S_out[2] = gfac * X_S[2];
}

void lvdc_nav_bml_step(lvdc_state_t *s,
                       const lvdc_params_t *p,
                       const double V_m_bar_mps_now[3],
                       double t_now_sec) {
    if (!s->sensor.GRR_latched) {
        return;
    }

    /* Measured BML interval. EDD §4.2.4 uses the actual measured
     * ΔT, not a nominal 1.6 s. On the first BML after GRR, fall
     * back to the preset nominal. */
    double dT = (s->sensor.t_prev_BML_sec > 0.0)
                    ? (t_now_sec - s->sensor.t_prev_BML_sec)
                    : p->delta_T_sec;
    if (dT <= 0.0) {
        dT = p->delta_T_sec;
    }
    s->sensor.dT_BML_sec     = dT;
    s->sensor.t_prev_BML_sec = t_now_sec;

    /* -----------------------------------------------------------------
     * EDD §4.2.3 — Velocity accumulation.
     *
     * The plant delivers V_m_bar_mps already quantized at 0.05 m/s
     * per the ST-124M Accelerometer_Quantization block. We subtract
     * the GRR offset and accumulate integer pulse deltas to avoid
     * round-off drift (see docs/LVDC_QUANTIZATION_INVENTORY.md).
     * -------------------------------------------------------------- */
    double V_m_now[3];      /* platform velocity this BML, m/s */
    double V_m_past[3];     /* platform velocity previous BML, m/s */
    for (int i = 0; i < 3; i++) {
        V_m_past[i] = s->sensor.V_m_mps[i];

        const double raw = V_m_bar_mps_now[i] - s->sensor.V_m_mps_offset[i];
        const int64_t total_pulses = lvdc_quant_mps_to_pulses(raw);
        const int64_t delta_pulses = total_pulses - s->sensor.V_m_pulses[i];
        s->sensor.V_m_pulses[i] = total_pulses;

        V_m_now[i]            = lvdc_quant_pulses_to_mps(total_pulses);
        s->sensor.V_m_mps[i]  = V_m_now[i];

        (void)delta_pulses; /* reserved for Phase C disagreement test */
    }

    /* -----------------------------------------------------------------
     * EDD §4.2.4 — F/M calculation (Eqs. 4.2.8-10).
     *   X_m_dot = (V_m_now − V_m_past) / ΔT  (and Y, Z)
     *   F/M = sqrt(X_m_dot^2 + Y_m_dot^2 + Z_m_dot^2)
     * -------------------------------------------------------------- */
    double FoverM_sq = 0.0;
    for (int i = 0; i < 3; i++) {
        s->sensor.FoverM_axis[i] = (V_m_now[i] - V_m_past[i]) / dT;
        FoverM_sq += s->sensor.FoverM_axis[i] * s->sensor.FoverM_axis[i];
    }
    s->sensor.FoverM = sqrt(FoverM_sq);

    /* -----------------------------------------------------------------
     * EDD §4.2.5 — M/F smoothing (Eq. 4.2.11, with magnitude and
     * rate-of-change limits). The present M/F input is 1 / (F/M) when
     * F/M is non-zero; otherwise fall through to the previous
     * smoothed value.
     * -------------------------------------------------------------- */
    if (s->sensor.FoverM > 0.0) {
        const double MoverF_now = 1.0 / s->sensor.FoverM;
        s->sensor.MoverF_s = lvdc_nav_smooth_MoverF(&s->sensor, p, MoverF_now);
    }

    /* -----------------------------------------------------------------
     * EDD §4.3.1 — Trapezoidal integration of position and velocity
     * in the plumbline frame.
     *
     *   Eq. 4.3.1  ΔX_Sg = (V_Sg + ½·G_S_prev·ΔT)·ΔT
     *   Eq. 4.3.2  ΔX_mf = ½·(V_m + V_m_past)·ΔT
     *   Eq. 4.3.3  X_S   += ΔX_Sg + ΔX_mf + V_0·ΔT
     *   Eq. 4.3.4  V_Sg  += ½·(G_S_new + G_S_prev)·ΔT   (needs G_S_new
     *                                                    from updated X_S)
     *   Eq. 4.3.5  V_S    = V_m + V_0 + V_Sg
     *
     * The predictor-corrector flow: position first uses G_S_prev
     * (available from the previous BML), then we compute G_S_new
     * from the updated position, then V_Sg is integrated with the
     * trapezoidal average of G_S_prev and G_S_new.
     * -------------------------------------------------------------- */

    /* 1. Update X_S (Eqs. 4.3.1-3) using G_S_prev and V_0. */
    for (int i = 0; i < 3; i++) {
        const double dX_Sg = (s->nav.V_Sg[i] + 0.5 * s->nav.G_S_prev[i] * dT) * dT;
        const double dX_mf = 0.5 * (V_m_now[i] + s->nav.V_m_mps_past[i]) * dT;
        s->nav.X_S[i] += dX_Sg + dX_mf + s->sensor.V_S_init[i] * dT;
    }

    /* 2. Compute G_S_new from the new X_S (Eqs. 4.3.6-12). */
    double G_S_new[3];
    lvdc_nav_compute_gravity(s->nav.X_S, p, G_S_new);

    /* 3. Update V_Sg with trapezoidal gravity (Eq. 4.3.4). */
    for (int i = 0; i < 3; i++) {
        s->nav.V_Sg[i] += 0.5 * (G_S_new[i] + s->nav.G_S_prev[i]) * dT;
        s->nav.G_S_prev[i] = G_S_new[i];
        s->nav.G_S[i]      = G_S_new[i];
    }

    /* 4. Carry the measured velocity forward for the next BML. */
    for (int i = 0; i < 3; i++) {
        s->nav.V_m_mps_past[i] = V_m_now[i];
    }
}

void lvdc_nav_compute_V_S(const lvdc_state_t *s, double V_S_out[3]) {
    /* EDD Eq. 4.3.5. */
    for (int i = 0; i < 3; i++) {
        V_S_out[i] = s->sensor.V_m_mps[i]
                   + s->sensor.V_S_init[i]
                   + s->nav.V_Sg[i];
    }
}

double lvdc_nav_compute_V_S_magnitude(const lvdc_state_t *s) {
    /* EDD Eq. 4.3.5.1. */
    double V_S[3];
    lvdc_nav_compute_V_S(s, V_S);
    return sqrt(V_S[0]*V_S[0] + V_S[1]*V_S[1] + V_S[2]*V_S[2]);
}
