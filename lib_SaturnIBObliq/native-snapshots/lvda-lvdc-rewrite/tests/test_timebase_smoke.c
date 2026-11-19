/*
 * test_timebase_smoke.c — unit tests for the time-base state
 * machine and event discretes (lvdc_timebase.{c,h}).
 *
 * Simulates the AS-205 event sequence step by step and verifies:
 *   - T1 latches on bLiftoff rising edge.
 *   - T2 latches on bPropDry rising edge.
 *   - T3 latches 6.2 s after T2.
 *   - T4 latches 2.7 s after T3.
 *   - Each discrete fires at the correct threshold.
 *   - SIV cutoff is persistent once latched.
 *   - NULL guards don't crash.
 */

#include "../lvdc_timebase.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int g_fail = 0;

#define CHECK(cond, label) do {                                       \
    if (!(cond)) {                                                    \
        fprintf(stderr, "FAIL [%s] %s:%d  !(%s)\n",                  \
                (label), __FILE__, __LINE__, #cond);                  \
        g_fail++;                                                     \
    }                                                                 \
} while (0)

#define CHECK_CLOSE(got, expected, tol, label) do {                   \
    const double _g = (got);                                          \
    const double _e = (expected);                                     \
    if (fabs(_g - _e) > (tol)) {                                      \
        fprintf(stderr,                                               \
                "FAIL [%s] %s:%d got %.9g expected %.9g\n",           \
                (label), __FILE__, __LINE__, _g, _e);                 \
        g_fail++;                                                     \
    }                                                                 \
} while (0)

int main(void) {
    lvdc_timebase_state_t s;
    lvdc_timebase_reset(&s);

    /* Pre-liftoff: no bases active, no discretes. */
    lvdc_timebase_step(&s, 10.0, false, false, false);
    lvdc_timebase_discretes_t d;
    lvdc_timebase_discretes(&s, 10.0, &d);
    CHECK(!d.bIECO && !d.bOECO && !d.bSIVEngineStart,
          "pre-liftoff: no discretes");
    CHECK_CLOSE(lvdc_timebase_T1_sec(&s, 10.0), 0.0, 1e-15,
                "pre-liftoff: T1 = 0");

    /* --------------------------------------------------------------
     * Liftoff: bLiftoff rising edge at t = 100.
     * ------------------------------------------------------------ */
    lvdc_timebase_step(&s, 100.0, true, false, false);
    CHECK(s.started[0], "T1 started on liftoff");
    CHECK_CLOSE(s.t_start[0], 100.0, 1e-15, "T1 start = 100");

    /* 30 s later: T1 = 30. */
    lvdc_timebase_step(&s, 130.0, true, false, false);
    CHECK_CLOSE(lvdc_timebase_T1_sec(&s, 130.0), 30.0, 1e-15,
                "T1 at t=130 is 30 s");

    /* --------------------------------------------------------------
     * Propellant dry: at t = 230 (T1 = 130 s).
     * ------------------------------------------------------------ */
    lvdc_timebase_step(&s, 230.0, true, true, false);
    CHECK(s.started[1], "T2 started on prop-dry");
    CHECK_CLOSE(s.t_start[1], 230.0, 1e-15, "T2 start = 230");

    lvdc_timebase_discretes(&s, 230.0, &d);
    CHECK(!d.bIECO, "T2=0: no IECO yet");
    CHECK(!d.bOECO, "T2=0: no OECO yet");

    /* Step to T2 > 3.2 (just past threshold): IECO fires. */
    lvdc_timebase_step(&s, 233.3, true, true, false);
    lvdc_timebase_discretes(&s, 233.3, &d);
    CHECK(d.bIECO, "IECO fires past T2 = 3.2");
    CHECK(!d.bOECO, "OECO not yet");

    /* Step to T2 > 6.2: OECO fires, T3 starts. */
    lvdc_timebase_step(&s, 236.3, true, true, false);
    CHECK(s.started[2], "T3 started at OECO threshold");
    lvdc_timebase_discretes(&s, 236.3, &d);
    CHECK(d.bIECO, "IECO still true past 6.2");
    CHECK(d.bOECO, "OECO fires past T2 = 6.2");

    /* T3 > 1.1: ullage. */
    lvdc_timebase_step(&s, 237.5, true, true, false);
    lvdc_timebase_discretes(&s, 237.5, &d);
    CHECK(d.bUllageMotorStart, "ullage fires past T3 = 1.1");

    /* T3 > 1.38: stage sep + SIVB enable. */
    lvdc_timebase_step(&s, 237.8, true, true, false);
    lvdc_timebase_discretes(&s, 237.8, &d);
    CHECK(d.bSIBStageSep, "stage sep past T3 = 1.38");
    CHECK(d.bSimulinkSIVBEnable, "SIVB enable past T3 = 1.38");

    /* T3 > 2.7: SIVEngineStart, T4 starts. */
    lvdc_timebase_step(&s, 239.1, true, true, false);
    CHECK(s.started[3], "T4 started at SIV start threshold");
    lvdc_timebase_discretes(&s, 239.1, &d);
    CHECK(d.bSIVEngineStart, "SIVB engine start past T3 = 2.7");
    CHECK(d.bSIVBurn, "SIVBurn active");
    CHECK(!d.bSIVCoast, "coast is false during burn");

    /* --------------------------------------------------------------
     * SIV cutoff latch.
     * ------------------------------------------------------------ */
    CHECK(!d.bSIVCutoff, "cutoff not yet latched");

    lvdc_timebase_step(&s, 500.0, true, true, true);
    lvdc_timebase_discretes(&s, 500.0, &d);
    CHECK(d.bSIVCutoff, "cutoff latched on edge");
    CHECK(!d.bSIVBurn, "SIVBurn false after cutoff");

    /* Edge goes away — cutoff STAYS latched. */
    lvdc_timebase_step(&s, 600.0, true, true, false);
    lvdc_timebase_discretes(&s, 600.0, &d);
    CHECK(d.bSIVCutoff, "cutoff persists after edge");

    /* --------------------------------------------------------------
     * NULL guards.
     * ------------------------------------------------------------ */
    lvdc_timebase_reset(NULL);
    lvdc_timebase_step(NULL, 0.0, false, false, false);
    lvdc_timebase_discretes(NULL, 0.0, NULL);
    CHECK_CLOSE(lvdc_timebase_T1_sec(NULL, 0.0), 0.0, 1e-15,
                "NULL T1");

    if (g_fail == 0) {
        printf("test_timebase_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_timebase_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
