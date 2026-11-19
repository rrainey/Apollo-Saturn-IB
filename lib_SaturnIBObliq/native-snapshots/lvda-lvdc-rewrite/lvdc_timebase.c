/*
 * lvdc_timebase.c — time-base state machine and event discretes.
 */

#include "lvdc_timebase.h"

#include <stddef.h>
#include <string.h>

/* Threshold constants from the legacy LVDA_LVDC.c Condition blocks. */
#define IECO_THRESH_SEC           3.2
#define OECO_THRESH_SEC           6.2
#define SWITCHPOINT5_THRESH_SEC   147.2
#define ULLAGE_THRESH_SEC         1.1
#define STAGESEP_THRESH_SEC       1.38
#define SIV_START_THRESH_SEC      2.7

enum { TB1 = 0, TB2 = 1, TB3 = 2, TB4 = 3 };

void lvdc_timebase_reset(lvdc_timebase_state_t *s) {
    if (s == NULL) return;
    memset(s, 0, sizeof(*s));
}

static void start_base(lvdc_timebase_state_t *s, int k, double now) {
    if (!s->started[k]) {
        s->started[k] = true;
        s->t_start[k] = now;
    }
}

static double elapsed(const lvdc_timebase_state_t *s, int k,
                      double now) {
    if (!s->started[k]) return 0.0;
    const double dt = now - s->t_start[k];
    return (dt < 0.0) ? 0.0 : dt;
}

void lvdc_timebase_step(lvdc_timebase_state_t *s,
                        double now_sec,
                        bool bLiftoff,
                        bool bPropDry,
                        bool siv_cutoff_edge) {
    if (s == NULL) return;

    /* T1 : bLiftoff rising edge. */
    if (!s->prev_bLiftoff && bLiftoff) {
        start_base(s, TB1, now_sec);
    }
    s->prev_bLiftoff = bLiftoff;

    /* T2 : bSIBPropellantSensorDry rising edge. */
    if (!s->prev_bPropDry && bPropDry) {
        start_base(s, TB2, now_sec);
    }
    s->prev_bPropDry = bPropDry;

    /* T3 : OECO threshold on T2. */
    if (s->started[TB2]
        && elapsed(s, TB2, now_sec) >= OECO_THRESH_SEC) {
        start_base(s, TB3, now_sec);
    }

    /* T4 : S-IVB engine start threshold on T3. */
    if (s->started[TB3]
        && elapsed(s, TB3, now_sec) >= SIV_START_THRESH_SEC) {
        start_base(s, TB4, now_sec);
    }

    /* SIV cutoff latch. */
    if (siv_cutoff_edge) {
        s->siv_cutoff_latched = true;
    }
}

double lvdc_timebase_T1_sec(const lvdc_timebase_state_t *s,
                            double now_sec) {
    return s ? elapsed(s, TB1, now_sec) : 0.0;
}
double lvdc_timebase_T2_sec(const lvdc_timebase_state_t *s,
                            double now_sec) {
    return s ? elapsed(s, TB2, now_sec) : 0.0;
}
double lvdc_timebase_T3_sec(const lvdc_timebase_state_t *s,
                            double now_sec) {
    return s ? elapsed(s, TB3, now_sec) : 0.0;
}
double lvdc_timebase_T4_sec(const lvdc_timebase_state_t *s,
                            double now_sec) {
    return s ? elapsed(s, TB4, now_sec) : 0.0;
}

void lvdc_timebase_discretes(const lvdc_timebase_state_t *s,
                             double now_sec,
                             lvdc_timebase_discretes_t *out) {
    if (s == NULL || out == NULL) return;

    const double T2 = elapsed(s, TB2, now_sec);
    const double T3 = elapsed(s, TB3, now_sec);

    const bool t2_active = s->started[TB2];
    const bool t3_active = s->started[TB3];

    out->bIECO             = t2_active && (T2 >= IECO_THRESH_SEC);
    out->bOECO             = t2_active && (T2 >= OECO_THRESH_SEC);
    out->bSwitchpoin5      = t2_active && (T2 >= SWITCHPOINT5_THRESH_SEC);
    out->bUllageMotorStart = t3_active && (T3 >= ULLAGE_THRESH_SEC);
    out->bSIBStageSep      = t3_active && (T3 >= STAGESEP_THRESH_SEC);
    out->bSimulinkSIVBEnable = out->bSIBStageSep;
    out->bSIVEngineStart   = t3_active && (T3 >= SIV_START_THRESH_SEC);
    out->bSIVCutoff        = s->siv_cutoff_latched;
    out->bSIVBurn          = out->bSIVEngineStart && !out->bSIVCutoff;
    out->bSIVCoast         = out->bSIBStageSep && !out->bSIVEngineStart;
}
