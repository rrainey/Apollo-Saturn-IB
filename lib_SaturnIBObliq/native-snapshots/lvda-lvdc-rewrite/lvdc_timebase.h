/*
 * lvdc_timebase.h — LVDC time-base state machine and event
 * discretes (EDD §4.1 + TM-X-62831 Event Sequence).
 *
 * Saturn-IB flight sequencing runs on four LVDC-internal time
 * bases plus their derived event thresholds. Each time base
 * starts on an external edge or an earlier event, and timers
 * run from that start.
 *
 *   T1 start : bLiftoff rising edge (plant input).
 *   T2 start : bSIBPropellantSensorDry rising edge
 *              (plant input, S-IB propellant depletion).
 *   T3 start : OECO event (T2 >= 6.2 s).
 *   T4 start : S-IVB engine start event (T3 >= 2.7 s).
 *
 * Discrete events (threshold form `bFoo = (Tn >= X)`), with
 * X values pulled from the current hand-edited LVDA_LVDC's
 * Condition blocks. Thresholds match the AS-205 event sequence
 * as implemented; TMX Appendix B table 1C confirms the end-state
 * times but the LVDC-internal threshold constants are what the
 * flight program used.
 *
 *   bIECO              = (T2 >= 3.2  s)      // S-IB inboard cutoff
 *   bOECO              = (T2 >= 6.2  s)      // S-IB outboard cutoff
 *   bSwitchpoin5       = (T2 >= 147.2 s)     // safety timeout
 *   bUllageMotorStart  = (T3 >= 1.1  s)
 *   bSIBStageSep       = (T3 >= 1.38 s)      // physical separation
 *   bSimulinkSIVBEnable= (T3 >= 1.38 s)      // obliq-2 enable gate
 *   bSIVEngineStart    = (T3 >= 2.7  s)
 *   bSIVBurn           = bSIVEngineStart AND NOT bSIVCutoff
 *   bSIVCoast          = bSIBStageSep AND NOT bSIVEngineStart
 *   bSIVCutoff         = latched from HSL (Eqs. 4.4.88-94) OR,
 *                        pending HSL, a conservative time cap.
 *
 * All time bases are advanced by the dispatcher each tick. The
 * state is a plain struct; the dispatcher owns storage.
 */

#ifndef LVDC_TIMEBASE_H
#define LVDC_TIMEBASE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* `started[k]` is true once T(k+1) latched. `t_start[k]` is
     * model->time at the latch. Elapsed in each time base is
     * (model->time - t_start[k]) while active. */
    bool   started[4];     /* [0]=T1, [1]=T2, [2]=T3, [3]=T4 */
    double t_start[4];

    /* Previous-tick inputs for rising-edge detection. */
    bool   prev_bLiftoff;
    bool   prev_bPropDry;

    /* SIV cutoff latch (persistent). Once set, stays set. */
    bool   siv_cutoff_latched;
} lvdc_timebase_state_t;

/* Reset all time-base state. Call from LVDA_LVDC_init. */
void lvdc_timebase_reset(lvdc_timebase_state_t *s);

/* Advance the state machine one tick.
 *
 *   now_sec         : model->time
 *   bLiftoff        : plant input, rising-edge triggers T1.
 *   bPropDry        : plant input, rising-edge triggers T2.
 *   siv_cutoff_edge : true on the BML where the IGM mode
 *                      machine (or HSL) latches cutoff; sets
 *                      the persistent cutoff flag.
 */
void lvdc_timebase_step(lvdc_timebase_state_t *s,
                        double now_sec,
                        bool bLiftoff,
                        bool bPropDry,
                        bool siv_cutoff_edge);

/* Read helpers. Each returns elapsed seconds in the given time
 * base, or 0 if the base hasn't started. */
double lvdc_timebase_T1_sec(const lvdc_timebase_state_t *s,
                            double now_sec);
double lvdc_timebase_T2_sec(const lvdc_timebase_state_t *s,
                            double now_sec);
double lvdc_timebase_T3_sec(const lvdc_timebase_state_t *s,
                            double now_sec);
double lvdc_timebase_T4_sec(const lvdc_timebase_state_t *s,
                            double now_sec);

/* All discrete outputs derived from the time bases. Builds them
 * in one pass so there is no state inconsistency. */
typedef struct {
    bool bIECO;
    bool bOECO;
    bool bSwitchpoin5;        /* OECO + 141 s safety */
    bool bUllageMotorStart;
    bool bSIBStageSep;
    bool bSimulinkSIVBEnable;
    bool bSIVEngineStart;
    bool bSIVBurn;
    bool bSIVCoast;
    bool bSIVCutoff;
} lvdc_timebase_discretes_t;

/* Build the discrete outputs from the current time-base state. */
void lvdc_timebase_discretes(const lvdc_timebase_state_t *s,
                             double now_sec,
                             lvdc_timebase_discretes_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_TIMEBASE_H */
