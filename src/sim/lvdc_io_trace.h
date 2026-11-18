/**
 * LVDA/LVDC I/O trace schema.
 *
 * Captures the boundary ports the hand-edited LVDA_LVDC native atomic
 * actually reads (inputs) and the primary nav outputs the rewrite is
 * expected to reproduce (outputs). The replay driver under
 * lib_SaturnIBObliq/native-snapshots/lvda-lvdc-rewrite/tests/ reads
 * the CSV, drives the rewrite dispatcher, and compares R_S_bar_m
 * against this trace's "golden" column.
 *
 * Cadence: one row per LVDC major step (0.005 s fundamental). Lets
 * the replay driver reconstruct the exact RK4-stage sample_tick
 * sequence the parent produced.
 *
 * See docs/LVDA_LVDC_PORT_PROVENANCE.md for why only four inputs are
 * captured.
 */
#pragma once

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LvdcIoTraceSample_tag {
    double            t_sec;
    unsigned long long sample_tick;

    /* Inputs read by the current body (see port provenance doc). */
    double V_m_bar_mps[3];
    double Theta_deg[3];
    bool   bLiftoff;
    bool   bSIBPropellantSensorDry;

    /* Golden nav output. */
    double R_S_bar_m[3];
} LvdcIoTraceSample;

const char *lvdc_io_trace_csv_header(void);
void        lvdc_io_trace_write_row(FILE *fp, const LvdcIoTraceSample *s);

#ifdef __cplusplus
}
#endif
