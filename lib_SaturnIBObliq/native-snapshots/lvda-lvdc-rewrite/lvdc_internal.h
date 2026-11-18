/*
 * lvdc_internal.h — internal types for the LVDC rewrite.
 *
 * None of these types cross the obliq-2 native atomic ABI boundary.
 * The public ABI lives in LVDA_LVDC.h and matches the current hand-
 * edited snapshot field-for-field. The rewrite keeps its own internal
 * state in the structs below, reached from the dispatcher through
 * module-local statics reset in LVDA_LVDC_init().
 *
 * Shape follows EDD sections, not obliq-2 generator output.
 */

#ifndef LVDC_INTERNAL_H
#define LVDC_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------
 * EDD §4.2 accelerometer / sensor state
 * ------------------------------------------------------------------ */
typedef struct {
    /* Accumulated PIPA pulse count per axis, X-Y-Z (EDD §4.2.3 Eq. 4.2.7).
     * Integer for round-off-free accumulation; multiplied by 0.05 only
     * at BML read to yield platform velocity in m/s. */
    int64_t V_m_pulses[3];

    /* Last V_m_bar_mps sample used to compute the per-BML pulse delta.
     * Needed because the parent delivers quantized m/s, not raw pulses. */
    double  V_m_mps_prev[3];

    /* Platform velocity in m/s per axis (EDD §4.2.3 Eq. 4.2.8),
     * = V_m_pulses * 0.05. */
    double  V_m_mps[3];

    /* Measured acceleration F/M per axis (EDD §4.2.4 Eqs. 4.2.8-10).
     * X_m_dot, Y_m_dot, Z_m_dot in the EDD notation, from
     * (V_m_now - V_m_prev) / ΔT. */
    double  FoverM_axis[3];
    double  FoverM;         /* √(sum of squares), EDD Eq. 4.2.10 */

    /* M/F smoothed per EDD §4.2.5 Eq. 4.2.11.
     * Four historical values of M/F and of (M/F)_s. */
    double  MoverF_hist[4]; /* (M/F)_1 .. (M/F)_4 (present .. past-3) */
    double  MoverF_s_hist[4];
    double  MoverF_s;       /* current smoothed value */

    /* Timing: previous BML timestamp, used to compute ΔT between reads.
     * EDD §4.2.4 requires ΔT from the actual read interval, not a
     * nominal 1.6 s. */
    double  t_prev_BML_sec;
    double  dT_BML_sec;

    /* Zero-reset latch for the PIPA accumulator (EDD §4.2.3) and
     * seed for the plumbline-frame position / earth-rotation
     * velocity (EDD §3.4 Eq. 3.4.3-4).
     *
     * On a real Saturn IB, this latch fires at Guidance Reference
     * Release (GRR, nominally T_0 − 17 s), which is an LVDC-internal
     * event in the countdown-program timing: the moment the computer
     * stops driving the ST-124M erection loop and lets the platform
     * track inertial motion. The pulse accumulators are zeroed at
     * that moment per EDD §4.2.3. There is no external GRR discrete
     * — the LVDC owns the timing.
     *
     * Phase B rewrite behaviour: the obliq-2 parent model does not
     * provide a separate GRR discrete or a pre-liftoff LVDC run
     * period. The only timing input is bLiftoff (DI24 rising edge).
     * We therefore latch at bLiftoff, matching the current hand-
     * edited snapshot (../lvda-lvdc/LVDA_LVDC.c L1378). The ~17 s
     * of pad-held specific-force integration from GRR to liftoff
     * is lost from V_m, but the equal and opposite ∫G_S dt is
     * also absent, so Eq. 4.3.5 (V_S = V_m + V_0 + V_Sg) still
     * produces the correct inertial velocity from liftoff onward.
     *
     * A faithful GRR implementation (Phase D or later) would need
     * either a GRR discrete from the parent or a pre-liftoff LVDC
     * run period triggered by the prelaunch countdown. */
    bool    liftoff_latched;
    double  V_m_mps_offset[3];  /* platform velocity at latch time */
    double  X_S_init[3];        /* Eq. 3.4.3 position at latch */
    double  V_S_init[3];        /* Eq. 3.4.4 earth-rotation seed */
} lvdc_sensor_state_t;

/* ---------------------------------------------------------------------
 * EDD §4.3 boost navigation state
 * ------------------------------------------------------------------ */
typedef struct {
    /* Position X_S (m) and velocity V_Sg (m/s) in plumbline (S) frame.
     * EDD Eq. 4.3.3 and 4.3.4. */
    double  X_S[3];
    double  V_Sg[3];

    /* Gravitational acceleration G_S (m/s^2) from EDD Eqs. 4.3.6-12.
     * Kept from the previous BML for the Eq. 4.3.1 trap step. */
    double  G_S[3];
    double  G_S_prev[3];

    /* V_m_mps from the previous BML, for the Eq. 4.3.2 trap step. */
    double  V_m_mps_past[3];

    /* Last R_S value committed to the output at a BML tick. The
     * obliq-2 parent consumes R_S_bar_m from the segregated-
     * subsystem boundary only at BML cadence (its gravity block and
     * downstream IGM consumers are BML-gated). To match the current
     * hand-edited body's publish behaviour — Product18 is only
     * written inside the BML gate at L1434 — the dispatcher's
     * publish_outputs reads from this cached value, which the BML
     * step updates. Between BMLs the output stays constant. */
    double  published_R_S_bar_m[3];
} lvdc_nav_state_t;

/* Time Base 1 tracking for the dispatcher (see LVDA_LVDC.c).
 * `T1_sec` is the elapsed time since the liftoff tick; the pre-IGM
 * time-tilt uses it as the polynomial argument per TM-X-62831
 * Appendix B. */
typedef struct {
    double t_liftoff_sec;        /* `time` captured on the liftoff edge */
    double T1_sec;               /* time - t_liftoff_sec, clamped >= 0 */
} lvdc_tb1_state_t;

/* ---------------------------------------------------------------------
 * Combined rewrite state singleton
 * ------------------------------------------------------------------ */
typedef struct {
    lvdc_sensor_state_t sensor;
    lvdc_nav_state_t    nav;
    lvdc_tb1_state_t    tb1;

    /* The sample tick at which the last once-per-step work ran.
     * For the per-tick-guard idiom in compute_outputs. */
    uint64_t last_compute_tick;
    uint64_t last_memory_tick;
} lvdc_state_t;

#ifdef __cplusplus
}
#endif

#endif /* LVDC_INTERNAL_H */
