/*
 * replay_lvdc_trace.c — shadow-run harness.
 *
 * Reads a CSV written by batch_sim --lvdc-io-trace, drives the
 * lvdc-rewrite dispatcher with the same input stream, and compares
 * R_S_bar_m[3] against the golden column.
 *
 * Trace schema (see src/sim/lvdc_io_trace.h):
 *   t_sec, sample_tick,
 *   V_m_bar_x_mps, V_m_bar_y_mps, V_m_bar_z_mps,
 *   Theta_x_deg,   Theta_y_deg,   Theta_z_deg,
 *   bLiftoff, bSIBPropellantSensorDry,
 *   R_S_x_m, R_S_y_m, R_S_z_m
 *
 * Replay semantics: trace row N (sample_tick = N, for N > 0) was
 * captured AFTER the batch_sim iteration that ran saturn_ib_stack_step
 * with pre-step sample_tick = N-1. So to replicate step N for the
 * rewrite:
 *   1. Set model->inputs.* from trace row N
 *   2. Set model->sample_tick = N - 1
 *   3. Call compute_outputs 4× (RK4 stage simulation) + update_memory_states
 *   4. Set model->sample_tick = N (parent would increment here)
 *   5. Compare model->outputs.R_S_bar_m against row N's R_S_bar_m
 *
 * Usage:
 *   replay_lvdc_trace <input.csv> [--dt SEC] [--delta-csv PATH]
 *                     [--abs-tol-m MAX] [--max-rows N]
 *
 * Exits 0 if max |ΔR| < abs-tol-m, non-zero otherwise.
 */

#include "../LVDA_LVDC.h"

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    double t_sec;
    unsigned long long sample_tick;
    double V_m_bar_mps[3];
    double Theta_deg[3];
    bool   bLiftoff;
    bool   bSIBPropellantSensorDry;
    double R_S_bar_m[3];
    double Psi_deg[3];
} TraceRow;

static bool parse_row(const char *line, TraceRow *r) {
    int b1 = 0, b2 = 0;
    const int n = sscanf(
        line,
        "%lf,%llu,"
        "%lf,%lf,%lf,"
        "%lf,%lf,%lf,"
        "%d,%d,"
        "%lf,%lf,%lf,"
        "%lf,%lf,%lf",
        &r->t_sec, &r->sample_tick,
        &r->V_m_bar_mps[0], &r->V_m_bar_mps[1], &r->V_m_bar_mps[2],
        &r->Theta_deg[0],   &r->Theta_deg[1],   &r->Theta_deg[2],
        &b1, &b2,
        &r->R_S_bar_m[0], &r->R_S_bar_m[1], &r->R_S_bar_m[2],
        &r->Psi_deg[0],   &r->Psi_deg[1],   &r->Psi_deg[2]);
    if (n != 16) return false;
    r->bLiftoff                = (b1 != 0);
    r->bSIBPropellantSensorDry = (b2 != 0);
    return true;
}

int main(int argc, char **argv) {
    const char *in_path    = NULL;
    const char *delta_path = NULL;
    double dt_sec   = 0.005;
    double abs_tol  = 10.0;   /* m, relaxed default — Phase B first-pass bar */
    long max_rows   = LONG_MAX;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--dt") == 0 && i + 1 < argc) {
            dt_sec = atof(argv[++i]);
        } else if (strcmp(argv[i], "--delta-csv") == 0 && i + 1 < argc) {
            delta_path = argv[++i];
        } else if (strcmp(argv[i], "--abs-tol-m") == 0 && i + 1 < argc) {
            abs_tol = atof(argv[++i]);
        } else if (strcmp(argv[i], "--max-rows") == 0 && i + 1 < argc) {
            max_rows = atol(argv[++i]);
        } else if (in_path == NULL && argv[i][0] != '-') {
            in_path = argv[i];
        } else {
            fprintf(stderr,
                "usage: replay_lvdc_trace <input.csv> [--dt SEC] "
                "[--delta-csv PATH] [--abs-tol-m MAX] [--max-rows N]\n");
            return 2;
        }
    }
    if (!in_path) {
        fprintf(stderr, "replay_lvdc_trace: missing input CSV\n");
        return 2;
    }

    FILE *fp = fopen(in_path, "r");
    if (!fp) {
        fprintf(stderr, "replay_lvdc_trace: cannot open %s\n", in_path);
        return 2;
    }

    FILE *delta_fp = NULL;
    if (delta_path) {
        delta_fp = fopen(delta_path, "w");
        if (!delta_fp) {
            fprintf(stderr,
                    "replay_lvdc_trace: cannot write %s\n", delta_path);
            fclose(fp);
            return 2;
        }
        fprintf(delta_fp,
                "t_sec,sample_tick,"
                "R_gold_x,R_gold_y,R_gold_z,"
                "R_new_x,R_new_y,R_new_z,"
                "dR_x,dR_y,dR_z,dR_mag,"
                "Psi_gold_r,Psi_gold_p,Psi_gold_y,"
                "Psi_new_r,Psi_new_p,Psi_new_y,"
                "dPsi_r,dPsi_p,dPsi_y,dPsi_max\n");
    }

    LVDA_LVDC_t model;
    LVDA_LVDC_init(&model);
    model.dt = dt_sec;

    char line[4096];
    /* Skip header. */
    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "replay_lvdc_trace: empty CSV\n");
        fclose(fp);
        return 2;
    }

    /* Row 0 (pre-step initial state) — just consume and verify. */
    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "replay_lvdc_trace: missing initial row\n");
        fclose(fp);
        return 2;
    }
    TraceRow row0;
    if (!parse_row(line, &row0)) {
        fprintf(stderr, "replay_lvdc_trace: bad initial row\n");
        fclose(fp);
        return 2;
    }
    /* The initial row should reflect the state BEFORE any step.
     * Golden R_S is zero at init; rewrite's is also zero. */

    long row_count = 0;
    double max_dR = 0.0;
    double sum_sq = 0.0;
    double max_dPsi = 0.0;
    double sum_sq_dPsi = 0.0;
    long post_latch_rows = 0;

    while (fgets(line, sizeof(line), fp) && row_count < max_rows) {
        TraceRow row;
        if (!parse_row(line, &row)) {
            fprintf(stderr,
                    "replay_lvdc_trace: bad row at line %ld\n", row_count + 2);
            fclose(fp);
            if (delta_fp) fclose(delta_fp);
            return 2;
        }

        /* Step N: compute_outputs ran with pre-step sample_tick = N-1
         * inside the parent. We replicate by driving 4 calls at that
         * tick, then update_memory_states, then bumping to N. */
        for (int i = 0; i < 3; i++) {
            model.inputs.V_m_bar_mps[i] = row.V_m_bar_mps[i];
            model.inputs.Theta_deg[i]   = row.Theta_deg[i];
        }
        model.inputs.bLiftoff                = row.bLiftoff;
        model.inputs.bSIBPropellantSensorDry = row.bSIBPropellantSensorDry;

        model.sample_tick = (row.sample_tick > 0) ? (row.sample_tick - 1) : 0;
        model.time        = model.sample_tick * dt_sec;

        LVDA_LVDC_compute_outputs(&model);  /* k1 */
        LVDA_LVDC_compute_outputs(&model);  /* k2 */
        LVDA_LVDC_compute_outputs(&model);  /* k3 */
        LVDA_LVDC_compute_outputs(&model);  /* k4 */
        LVDA_LVDC_update_memory_states(&model);

        model.sample_tick = row.sample_tick;
        model.time       += dt_sec;

        /* Compare R_S_bar_m. */
        const double dRx = model.outputs.R_S_bar_m[0] - row.R_S_bar_m[0];
        const double dRy = model.outputs.R_S_bar_m[1] - row.R_S_bar_m[1];
        const double dRz = model.outputs.R_S_bar_m[2] - row.R_S_bar_m[2];
        const double dRmag = sqrt(dRx*dRx + dRy*dRy + dRz*dRz);

        /* Compare Psi_deg. */
        const double dPr = model.outputs.Psi_deg[0] - row.Psi_deg[0];
        const double dPp = model.outputs.Psi_deg[1] - row.Psi_deg[1];
        const double dPy = model.outputs.Psi_deg[2] - row.Psi_deg[2];
        const double dPmax =
            fmax(fabs(dPr), fmax(fabs(dPp), fabs(dPy)));

        if (row.bLiftoff) {
            post_latch_rows++;
            sum_sq += dRmag * dRmag;
            if (dRmag > max_dR) max_dR = dRmag;
            sum_sq_dPsi += dPmax * dPmax;
            if (dPmax > max_dPsi) max_dPsi = dPmax;
        }

        if (delta_fp) {
            fprintf(delta_fp,
                    "%.12g,%llu,"
                    "%.17g,%.17g,%.17g,"
                    "%.17g,%.17g,%.17g,"
                    "%.6g,%.6g,%.6g,%.6g,"
                    "%.17g,%.17g,%.17g,"
                    "%.17g,%.17g,%.17g,"
                    "%.6g,%.6g,%.6g,%.6g\n",
                    row.t_sec, row.sample_tick,
                    row.R_S_bar_m[0], row.R_S_bar_m[1], row.R_S_bar_m[2],
                    model.outputs.R_S_bar_m[0],
                    model.outputs.R_S_bar_m[1],
                    model.outputs.R_S_bar_m[2],
                    dRx, dRy, dRz, dRmag,
                    row.Psi_deg[0], row.Psi_deg[1], row.Psi_deg[2],
                    model.outputs.Psi_deg[0],
                    model.outputs.Psi_deg[1],
                    model.outputs.Psi_deg[2],
                    dPr, dPp, dPy, dPmax);
        }

        row_count++;
    }

    fclose(fp);
    if (delta_fp) fclose(delta_fp);

    const double rms_dR =
        (post_latch_rows > 0) ? sqrt(sum_sq / (double)post_latch_rows) : 0.0;
    const double rms_dPsi =
        (post_latch_rows > 0) ? sqrt(sum_sq_dPsi / (double)post_latch_rows) : 0.0;

    printf("replay_lvdc_trace: processed %ld rows, %ld post-liftoff\n",
           row_count, post_latch_rows);
    printf("                   max |ΔR_S|   = %.6g m\n", max_dR);
    printf("                   rms |ΔR_S|   = %.6g m\n", rms_dR);
    printf("                   max |ΔPsi|   = %.6g deg\n", max_dPsi);
    printf("                   rms |ΔPsi|   = %.6g deg\n", rms_dPsi);
    printf("                   abs-tol-m    = %.6g m\n", abs_tol);
    if (delta_path) {
        printf("                   delta-csv    = %s\n", delta_path);
    }

    if (max_dR > abs_tol) {
        printf("replay_lvdc_trace: FAIL (max ΔR exceeds tolerance)\n");
        return 1;
    }
    printf("replay_lvdc_trace: PASS\n");
    return 0;
}
