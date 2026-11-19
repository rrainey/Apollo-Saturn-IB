/*
 * test_output_pipeline_smoke.c — unit tests for the attitude-command
 * output pipeline: minor-loop rate limit (EDD §8.3.1) and DAC rate
 * + saturation + quantization (EDD §8.2.3).
 *
 * Exits 0 on success, non-zero on any failed check.
 */

#include "../lvdc_dac.h"
#include "../lvdc_minor_loop.h"
#include "../lvdc_params.h"

#include <math.h>
#include <stdio.h>

static int g_fail_count = 0;

#define CHECK(cond, label) do {                                       \
    if (!(cond)) {                                                    \
        fprintf(stderr, "FAIL [%s] %s:%d  !(%s)\n",                  \
                (label), __FILE__, __LINE__, #cond);                  \
        g_fail_count++;                                               \
    }                                                                 \
} while (0)

#define CHECK_CLOSE(got, expected, abs_tol, label) do {               \
    const double _g = (got);                                          \
    const double _e = (expected);                                     \
    const double _d = fabs(_g - _e);                                  \
    if (_d > (abs_tol)) {                                             \
        fprintf(stderr,                                               \
                "FAIL [%s] %s:%d  got %.9g, expected %.9g, |d|=%.3g > tol %.3g\n", \
                (label), __FILE__, __LINE__, _g, _e, _d, (abs_tol));  \
        g_fail_count++;                                               \
    }                                                                 \
} while (0)

int main(void) {
    const lvdc_params_t p = lvdc_params_as205();
    const double dt = 0.005;
    const double deg2rad = M_PI / 180.0;

    /* ============================================================
     * Section A: minor-loop rate limit (EDD §8.3.1)
     * ========================================================== */
    lvdc_minor_loop_state_t ml;
    lvdc_minor_loop_reset(&ml);

    /* A1. Zero input stays zero. */
    {
        double chi_cmd[3] = {0.0, 0.0, 0.0};
        double chi_pr[3];
        lvdc_minor_loop_step(&ml, &p, chi_cmd, dt, chi_pr);
        for (int i = 0; i < 3; i++) {
            CHECK_CLOSE(chi_pr[i], 0.0, 1e-15, "minor-loop zero in = zero out");
        }
    }

    /* A2. Step input: large chi_cmd should move chi' by at most
     * rate_limit · dt per step. */
    {
        lvdc_minor_loop_reset(&ml);
        double chi_cmd[3] = {
             10.0 * deg2rad,
            -20.0 * deg2rad,
              5.0 * deg2rad
        };
        double chi_pr[3];
        lvdc_minor_loop_step(&ml, &p, chi_cmd, dt, chi_pr);

        const double max_step = p.chi_rate_limit_radps[0] * dt;  /* 1 deg/sec · 0.005 s */
        CHECK_CLOSE(chi_pr[0],  max_step, 1e-15, "chi_pr[0] clamped to +rate·dt");
        CHECK_CLOSE(chi_pr[1], -max_step, 1e-15, "chi_pr[1] clamped to -rate·dt");
        CHECK_CLOSE(chi_pr[2],  max_step, 1e-15, "chi_pr[2] clamped to +rate·dt");
    }

    /* A3. After enough steps, chi' reaches chi_cmd. Target = 0.1 deg,
     * rate = 1 deg/sec, dt = 0.005 s → needs 20 steps. */
    {
        lvdc_minor_loop_reset(&ml);
        double chi_cmd[3] = {0.1 * deg2rad, 0.0, 0.0};
        double chi_pr[3];
        for (int i = 0; i < 25; i++) {
            lvdc_minor_loop_step(&ml, &p, chi_cmd, dt, chi_pr);
        }
        CHECK_CLOSE(chi_pr[0], chi_cmd[0], 1e-15,
                    "chi_pr reaches chi_cmd after enough steps");
    }

    /* A4. Settled state: small chi_cmd below the per-step limit
     * follows without truncation. */
    {
        lvdc_minor_loop_reset(&ml);
        double chi_cmd[3] = {1e-6, -2e-6, 3e-6};
        double chi_pr[3];
        lvdc_minor_loop_step(&ml, &p, chi_cmd, dt, chi_pr);
        CHECK_CLOSE(chi_pr[0], chi_cmd[0], 1e-18,
                    "small chi_cmd passes through on first step");
        CHECK_CLOSE(chi_pr[1], chi_cmd[1], 1e-18,
                    "small chi_cmd[1] passes through");
    }

    /* ============================================================
     * Section B: DAC output pipeline (EDD §8.2.3)
     * ========================================================== */
    lvdc_dac_state_t dac;

    /* B1. Zero input stays zero. */
    {
        lvdc_dac_reset(&dac);
        double psi_in[3] = {0, 0, 0};
        double psi_out[3];
        lvdc_dac_step(&dac, &p, psi_in, dt, psi_out);
        for (int i = 0; i < 3; i++) {
            CHECK_CLOSE(psi_out[i], 0.0, 1e-15, "DAC zero in = zero out");
        }
    }

    /* B2. Magnitude saturation: a large input clamps at ±psi_mag_limit,
     * then quantizes to the nearest LSB. */
    {
        lvdc_dac_reset(&dac);
        double psi_in[3] = {
             100.0 * deg2rad,
            -100.0 * deg2rad,
               0.0
        };
        double psi_out[3];
        /* Ramp to saturation over many steps. */
        for (int i = 0; i < 1000; i++) {
            lvdc_dac_step(&dac, &p, psi_in, dt, psi_out);
        }
        const double mag_lim_quantized =
            p.dac_lsb_rad * floor(p.psi_mag_limit_rad / p.dac_lsb_rad + 0.5);
        CHECK_CLOSE(psi_out[0],  mag_lim_quantized, 1e-15,
                    "DAC saturates at +mag_lim and quantizes");
        CHECK_CLOSE(psi_out[1], -mag_lim_quantized, 1e-15,
                    "DAC saturates at -mag_lim and quantizes");
    }

    /* B3. Rate limit: step input of 10 deg moves at most
     * psi_rate_limit · dt per step. */
    {
        lvdc_dac_reset(&dac);
        double psi_in[3] = {10.0 * deg2rad, 0, 0};
        double psi_out[3];
        lvdc_dac_step(&dac, &p, psi_in, dt, psi_out);
        const double rate_step = p.psi_rate_limit_radps * dt;
        /* Quantize the expected value too. */
        const double expected =
            p.dac_lsb_rad * floor(rate_step / p.dac_lsb_rad + 0.5);
        CHECK_CLOSE(psi_out[0], expected, 1e-15,
                    "DAC rate-limits a 10 deg step on first call");
    }

    /* B4. Quantization: a sub-LSB input quantizes to zero or ±LSB. */
    {
        lvdc_dac_reset(&dac);
        /* 0.03 deg > half-LSB (0.02875 deg) → rounds up to 1 LSB. */
        double psi_in[3] = {0.03 * deg2rad, 0, 0};
        double psi_out[3];
        lvdc_dac_step(&dac, &p, psi_in, dt, psi_out);
        CHECK(fabs(psi_out[0]) <= p.dac_lsb_rad,
              "0.03 deg input rounds to no more than one LSB");

        /* 0.09 deg > LSB → rounds to 2 LSB = 0.115 deg.
         * But rate limit of 12 deg/sec · 0.005 s = 0.06 deg caps
         * the first step at 0.06 deg = ~1 LSB. */
        lvdc_dac_reset(&dac);
        psi_in[0] = 0.09 * deg2rad;
        lvdc_dac_step(&dac, &p, psi_in, dt, psi_out);
        const double rate_step   = p.psi_rate_limit_radps * dt;
        const double expected_q  = p.dac_lsb_rad * floor(rate_step / p.dac_lsb_rad + 0.5);
        CHECK_CLOSE(psi_out[0], expected_q, 1e-15,
                    "0.09 deg step rate-limited then quantized");
    }

    /* B5. Quantization is deterministic: repeat calls with the same
     * settled input give the same output. */
    {
        lvdc_dac_reset(&dac);
        double psi_in[3] = {1.23 * deg2rad, 0, 0};
        double psi_out_a[3], psi_out_b[3];
        for (int i = 0; i < 2000; i++) {
            lvdc_dac_step(&dac, &p, psi_in, dt, psi_out_a);
        }
        for (int i = 0; i < 10; i++) {
            lvdc_dac_step(&dac, &p, psi_in, dt, psi_out_b);
        }
        CHECK(psi_out_a[0] == psi_out_b[0],
              "settled DAC output is deterministic");
    }

    if (g_fail_count == 0) {
        printf("test_output_pipeline_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_output_pipeline_smoke: %d CHECK(S) FAILED\n",
            g_fail_count);
    return 1;
}
