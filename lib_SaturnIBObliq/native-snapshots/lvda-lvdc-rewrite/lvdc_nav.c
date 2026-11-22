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

void lvdc_nav_liftoff_latch(lvdc_state_t *s,
                            const lvdc_params_t *p,
                            double t_latch_sec,
                            const double V_m_bar_mps_now[3],
                            const double X_0_eq343[3],
                            const double V_0_eq344[3]) {
    /* EDD §4.2.3 — accumulator is zeroed at GRR.  Seed BOTH the sensor
     * offset and the gravity-integrator initial condition so the first
     * BML's trap step doesn't start half-weighted (G_S_prev=0 would give
     * V_Sg = 0.5·G·dT on BML 1, which fails to cancel the V_m pulse delta
     * of 1g·dT_actual).  See lvdc_internal.h sensor_state_t for details. */
    double G_S_at_latch[3];
    lvdc_nav_compute_gravity(X_0_eq343, p, /*boost_mode=*/true, G_S_at_latch);

    for (int i = 0; i < 3; i++) {
        s->sensor.V_m_mps_offset[i] = V_m_bar_mps_now[i];
        s->sensor.V_m_mps_prev[i]   = 0.0;
        s->sensor.V_m_mps[i]        = 0.0;
        s->sensor.V_m_pulses[i]     = 0;
        s->sensor.FoverM_axis[i]    = 0.0;

        s->nav.X_S[i]           = X_0_eq343[i];
        s->nav.V_Sg[i]          = 0.0;
        s->nav.G_S[i]           = G_S_at_latch[i];
        s->nav.G_S_prev[i]      = G_S_at_latch[i];
        s->nav.V_m_mps_past[i]  = 0.0;

        s->sensor.X_S_init[i] = X_0_eq343[i];
        s->sensor.V_S_init[i] = V_0_eq344[i];
    }
    s->sensor.FoverM          = 0.0;
    s->sensor.MoverF_s        = 0.0;
    s->sensor.t_latch_sec     = t_latch_sec;
    s->sensor.t_prev_BML_sec  = 0.0;
    s->sensor.dT_BML_sec      = 0.0;

    /* Initialize M/F histories to (M/F)_0 per EDD §4.2.5. */
    for (int k = 0; k < 4; k++) {
        s->sensor.MoverF_hist[k]   = 0.0; /* (M/F)_0 populated once we have thrust */
        s->sensor.MoverF_s_hist[k] = 0.0;
    }

    s->sensor.liftoff_latched = true;
}

void lvdc_nav_compute_gravity(const double X_S[3],
                              const lvdc_params_t *p,
                              bool boost_mode,
                              double G_S_out[3]) {
    /* EDD §4.3.2 Eqs. 4.3.6-12 — oblate earth gravity in plumbline
     * frame.
     *
     *   r = |X_S|
     *   R_e / r = aaa                              (Eq. 4.3.10-ish)
     *   Y_G = X_S projected on the earth spin axis (Eq. 4.3.6):
     *       Y_G = -X_S[0]·sin(phi_L)
     *             + X_S[1]·cos(phi_L)·sin(A_z)
     *             - X_S[2]·cos(phi_L)·cos(A_z)
     *   sin(geocentric latitude) = Y_G / r
     *
     * The radial scalar (central + J_2 radial):
     *   S = 1 + (3/2)·J_2·(R_e/r)²·(1 - 5·sin²_lat) + S_34
     *
     * The spin-axis projection scalar (J_2 perpendicular + J_3,4):
     *   P = 3·J_2·sin_lat + P_34
     *
     * Final gravity (Eq. 4.3.12):
     *   G_S = (-μ/r³)·X_S·S  +  (-μ/r²)·(R_e/r)²·P·pole_hat
     *
     * where pole_hat is the unit vector from the launch site toward
     * the earth spin axis in S-frame coordinates:
     *   pole_hat = [-sin(phi_L),
     *               cos(phi_L)·sin(A_z),
     *              -cos(phi_L)·cos(A_z)]
     *
     * S_34 and P_34 capture the J_3 and J_4 zonal harmonics via EDD-
     * specific derived coefficients:
     *   S_34 =  0.575e-5·aaa³·sin_lat·(3 - 7·sin²_lat)
     *         + (0.7875e-5/7)·aaa⁴·(3 - 42·sin²_lat + 63·sin⁴_lat)
     *   P_34 =  0.575e-5·aaa·(15·sin²_lat - 3)
     *         + (0.7875e-5/7)·aaa²·sin_lat·(12 - 28·sin²_lat)
     *
     * EDD §4.3.2: during boost, S_34 and P_34 are set to zero
     * ("during boost, only Eq. 4.3.6, 4.3.7, 4.3.10, and 4.3.11 are
     * used"); we honour that when boost_mode = true. The current
     * hand-edited body always computes S_34 / P_34; the drift is
     * sub-microgravity during AS-205 boost. */

    const double r2 = X_S[0]*X_S[0] + X_S[1]*X_S[1] + X_S[2]*X_S[2];
    if (r2 <= 0.0) {
        G_S_out[0] = G_S_out[1] = G_S_out[2] = 0.0;
        return;
    }
    const double r   = sqrt(r2);
    const double r3  = r2 * r;
    const double aaa = p->R_earth_equatorial_m / r;
    const double aaa2 = aaa * aaa;

    const double deg2rad = M_PI / 180.0;
    const double phi_rad = p->phi_L_deg * deg2rad;
    const double A_z_rad = p->A_z_deg   * deg2rad;
    const double sin_phi = sin(phi_rad);
    const double cos_phi = cos(phi_rad);
    const double sin_A   = sin(A_z_rad);
    const double cos_A   = cos(A_z_rad);

    /* Eq. 4.3.6: Y_G (projection onto equatorial-plane normal). */
    const double Y_G = -X_S[0]*sin_phi
                     +  X_S[1]*cos_phi*sin_A
                     -  X_S[2]*cos_phi*cos_A;
    const double sin_lat   = Y_G / r;
    const double sin_lat2  = sin_lat * sin_lat;

    /* J_2 radial scalar and (optionally) J_3/J_4 contribution.
     * Coefficient constants from the EDD §4.3.2 expansion:
     *   3/2·J_2 = 1.62345e-3
     *   3·J_2   = 0.0032469
     * See the comment block at the top for S_34 and P_34. */
    const double J2_radial = 1.62345e-3 * aaa2 * (1.0 - 5.0 * sin_lat2);

    double S_34 = 0.0;
    double P_34 = 0.0;
    if (!boost_mode) {
        const double aaa3 = aaa2 * aaa;
        const double aaa4 = aaa3 * aaa;
        const double sin_lat3 = sin_lat2 * sin_lat;
        const double sin_lat4 = sin_lat2 * sin_lat2;

        S_34 =   0.575e-5 * aaa3 * sin_lat * (3.0 - 7.0 * sin_lat2)
             + (0.7875e-5 / 7.0) * aaa4
                 * (3.0 - 42.0 * sin_lat2 + 63.0 * sin_lat4);
        P_34 =   0.575e-5 * aaa * (15.0 * sin_lat2 - 3.0)
             + (0.7875e-5 / 7.0) * aaa2 * sin_lat
                 * (12.0 - 28.0 * sin_lat2);
        (void)sin_lat3;  /* reserved for higher-order EDD expansion */
    }

    const double S = 1.0 + J2_radial + S_34;
    const double P = 0.0032469 * sin_lat + P_34;

    /* Central + radial-J_2 term: G_central = -μ·X_S·S / r³ */
    const double central_scale = -p->mu_earth_m3ps2 * S / r3;

    /* Spin-axis projection term: G_pole = -μ·aaa²·P·pole_hat / r²
     * expressed in S-frame coordinates. */
    const double pole_scale = -p->mu_earth_m3ps2 * aaa2 * P / r2;
    const double pole_hat[3] = {
        -sin_phi,
         cos_phi * sin_A,
        -cos_phi * cos_A
    };

    for (int i = 0; i < 3; i++) {
        G_S_out[i] = central_scale * X_S[i]
                   + pole_scale    * pole_hat[i];
    }
}

void lvdc_nav_bml_step(lvdc_state_t *s,
                       const lvdc_params_t *p,
                       const double V_m_bar_mps_now[3],
                       double t_now_sec) {
    if (!s->sensor.liftoff_latched) {
        return;
    }

    /* Measured BML interval. EDD §4.2.4 uses the actual measured
     * ΔT, not a nominal 1.6 s.  On the first BML after latch, use
     * the latch→first-BML elapsed time (typically < 1.6 s), so the
     * trap integration matches the pulse-delta V_m accumulated over
     * the same interval. */
    double dT;
    if (s->sensor.t_prev_BML_sec > 0.0) {
        dT = t_now_sec - s->sensor.t_prev_BML_sec;
    } else if (s->sensor.t_latch_sec > 0.0
               && t_now_sec > s->sensor.t_latch_sec) {
        dT = t_now_sec - s->sensor.t_latch_sec;
    } else {
        dT = p->delta_T_sec;
    }
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

    /* 2. Compute G_S_new from the new X_S (Eqs. 4.3.6-12). Boost
     * mode: S_34 / P_34 zero per EDD §4.3.2. */
    double G_S_new[3];
    lvdc_nav_compute_gravity(s->nav.X_S, p, /*boost_mode=*/true, G_S_new);

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

    /* 5. Commit the new X_S to the output cache. publish_outputs
     * reads from published_R_S_bar_m so that between BMLs the
     * dispatcher's output stays constant — matches the golden
     * body's Product18 publication timing (BML-gated). */
    for (int i = 0; i < 3; i++) {
        s->nav.published_R_S_bar_m[i] = s->nav.X_S[i];
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
