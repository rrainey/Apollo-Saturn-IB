/*
 * test_igm_orchestrator_smoke.c — unit tests for the IGM mode
 * state machine and per-BML orchestrator (lvdc_igm.{c,h}).
 *
 * These tests verify integration: the orchestrator calls the
 * §4.4.2 layers in the correct order with the correct inputs,
 * respects mode transitions, and keeps SMC state between calls.
 *
 * Numerical correctness of the individual equation blocks is
 * covered by the earlier test_igm_*_smoke executables; this file
 * does NOT re-check equation math.
 *
 * Checks:
 *   1. lvdc_igm_init populates MS4, resets T_1i/T_3i/P_C/SMC,
 *      and starts in PHASE1.
 *   2. One PHASE1 update produces finite chi_y, chi_z and
 *      decrements T_1i by exactly ΔT. T_3i is unchanged.
 *   3. EMRC transitions PHASE1 -> PHASE2_ARTIFICIAL_TAU and
 *      sets T_1i = 0, computes tau_3N, resets P_C = 0.
 *   4. In PHASE2_ARTIFICIAL_TAU, each update increments P_C by
 *      ΔT and decrements T_3i by ΔT.
 *   5. When P_C crosses P_CMR, mode auto-promotes to
 *      PHASE2_REAL_TAU on the next update.
 *   6. cutoff() -> POST_CUTOFF: subsequent updates hold the
 *      previous chi_y, chi_z unchanged.
 *   7. NULL guards don't crash.
 */

#include "../lvdc_igm.h"

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
    const double _d = fabs(_g - _e);                                  \
    if (_d > (tol)) {                                                 \
        fprintf(stderr,                                               \
                "FAIL [%s] %s:%d got %.9g expected %.9g |d|=%.3g > %.3g\n", \
                (label), __FILE__, __LINE__, _g, _e, _d, (tol));      \
        g_fail++;                                                     \
    }                                                                 \
} while (0)

/* Representative AS-205-style presets. Values are plausible but
 * NOT the authoritative TMX App B set — those will be plumbed in
 * from lvdc_params.c by the dispatcher in Phase C.3f-2. */
static void make_presets(lvdc_igm_presets_t *p) {
    memset(p, 0, sizeof(*p));
    p->V_ex1_mps = 4151.0;
    p->V_ex3_mps = 4233.2;
    p->T_1i_initial_sec = 150.0;
    p->T_3i_initial_sec = 350.0;
    p->tau_10_sec = 300.0;
    p->T_10_sec   = 150.0;
    p->M_GR_kg    = 120000.0;
    p->Mdot_2_kgps = 220.0;
    p->Mdot_3_kgps = 200.0;
    p->P_CMR_sec  = 20.0;
    p->R_T_m      = 6.571e6;
    p->V_T_mps    = 7804.0;
    p->Theta_T_rad = 0.0245;
    p->G_T_mps2   = -9.10;
    p->ROV        = 0.0;
    p->SMCG       = 0.0;    /* keep SMC inert for these tests */
    p->lambda_rad = 0.1;
    p->phi_L_rad  = 0.49;
    p->A_z_rad    = 1.5708;
    p->i_orb_rad  = 0.5236;
}

static const double R_S0[3] = { 6.571e6, 0.0, 0.0 };
static const double V_S0[3] = { 100.0, 0.0, 7600.0 };
static const double G_S0[3] = { -9.1, 0.0, 0.0 };

int main(void) {
    lvdc_igm_presets_t p;
    make_presets(&p);

    /* --------------------------------------------------------------
     * 1. Init.
     * ------------------------------------------------------------ */
    lvdc_igm_state_t s;
    lvdc_igm_init(&s, &p);
    CHECK(s.mode == LVDC_IGM_MODE_PHASE1, "init: mode PHASE1");
    CHECK_CLOSE(s.T_1i_sec, p.T_1i_initial_sec, 1e-15, "init: T_1i");
    CHECK_CLOSE(s.T_3i_sec, p.T_3i_initial_sec, 1e-15, "init: T_3i");
    CHECK_CLOSE(s.P_C_sec, 0.0, 1e-15, "init: P_C=0");
    CHECK_CLOSE(s.smc.SMCY, 0.0, 1e-15, "init: SMCY=0");
    CHECK_CLOSE(s.smc.SMCZ, 0.0, 1e-15, "init: SMCZ=0");
    CHECK(s.smc.primed == 0, "init: SMC not primed");
    /* MS4 must have been built: it is orthonormal so
     * MS4 * MS4^T = I. Spot-check (0,0) and (1,1) via dot of row 0. */
    const double row0sq = s.MS4[0][0]*s.MS4[0][0]
                        + s.MS4[0][1]*s.MS4[0][1]
                        + s.MS4[0][2]*s.MS4[0][2];
    CHECK_CLOSE(row0sq, 1.0, 1e-12, "init: MS4 row 0 unit");

    /* --------------------------------------------------------------
     * 2. One PHASE1 update: T_1i decrements by ΔT, T_3i unchanged.
     * ------------------------------------------------------------ */
    const double dT = 1.6;
    /* M/F chosen so tau = V_ex * (M/F) exceeds T_1i and T_3i (the
     * intermediate block requires tau > T_i for the ln argument). */
    const double MoverF = 0.14;
    double chi_y = 0, chi_z = 0;
    int rc = lvdc_igm_update(&s, dT, 7700.0, MoverF,
                             R_S0, V_S0, G_S0,
                             25.0, 0.0, 0.5,
                             &chi_y, &chi_z);
    CHECK(rc == 0, "PHASE1 update rc=0");
    CHECK(isfinite(chi_y) && isfinite(chi_z), "PHASE1 chi finite");
    CHECK_CLOSE(s.T_1i_sec, p.T_1i_initial_sec - dT, 1e-12,
                "PHASE1: T_1i decrements by ΔT");
    CHECK_CLOSE(s.T_3i_sec, p.T_3i_initial_sec, 1e-15,
                "PHASE1: T_3i unchanged");
    CHECK_CLOSE(s.P_C_sec, 0.0, 1e-15, "PHASE1: P_C stays 0");

    /* --------------------------------------------------------------
     * 3. EMRC transition.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_state_t s2;
        lvdc_igm_init(&s2, &p);
        const double tau_1_at_emrc = 250.0;
        lvdc_igm_emrc(&s2, tau_1_at_emrc);
        CHECK(s2.mode == LVDC_IGM_MODE_PHASE2_ARTIFICIAL_TAU,
              "EMRC: mode becomes ART_TAU");
        CHECK_CLOSE(s2.T_1i_sec, 0.0, 1e-15, "EMRC: T_1i = 0");
        CHECK_CLOSE(s2.P_C_sec,  0.0, 1e-15, "EMRC: P_C = 0");

        /* Hand-check tau_3N:
         *   alpha_f = tau_10 / (T_10 + tau_1)
         *   tau_3N  = (M_GR - alpha_f * Mdot_2 * T_10) / Mdot_3
         */
        const double alpha = p.tau_10_sec
                           / (p.T_10_sec + tau_1_at_emrc);
        const double expected_tau_3N =
            (p.M_GR_kg - alpha * p.Mdot_2_kgps * p.T_10_sec)
            / p.Mdot_3_kgps;
        CHECK_CLOSE(s2.tau_3N_sec, expected_tau_3N, 1e-9,
                    "EMRC: tau_3N matches Eqs. 4.4.10-11");
    }

    /* --------------------------------------------------------------
     * 4. ART_TAU update increments P_C and decrements T_3i.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_state_t s2;
        lvdc_igm_init(&s2, &p);
        lvdc_igm_emrc(&s2, 250.0);

        const double T_3i_before = s2.T_3i_sec;
        rc = lvdc_igm_update(&s2, dT, 7800.0, MoverF,
                             R_S0, V_S0, G_S0,
                             25.0, 0.0, 0.5,
                             &chi_y, &chi_z);
        CHECK(rc == 0, "ART_TAU update rc=0");
        CHECK_CLOSE(s2.P_C_sec, dT, 1e-12, "ART_TAU: P_C += ΔT");
        CHECK_CLOSE(s2.T_3i_sec, T_3i_before - dT, 1e-12,
                    "ART_TAU: T_3i -= ΔT");
    }

    /* --------------------------------------------------------------
     * 5. P_C >= P_CMR promotes to REAL_TAU on the FOLLOWING
     *    update's mode advance.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_state_t s2;
        lvdc_igm_init(&s2, &p);
        lvdc_igm_emrc(&s2, 250.0);

        /* Drive P_C just over P_CMR with a single big ΔT. The
         * mode promotion runs at the end of the update. */
        const double big_dT = p.P_CMR_sec + 1.0;
        rc = lvdc_igm_update(&s2, big_dT, 7800.0, MoverF,
                             R_S0, V_S0, G_S0,
                             25.0, 0.0, 0.5,
                             &chi_y, &chi_z);
        CHECK(rc == 0, "promotion update rc=0");
        CHECK(s2.mode == LVDC_IGM_MODE_PHASE2_REAL_TAU,
              "P_C >= P_CMR promotes to REAL_TAU");
    }

    /* --------------------------------------------------------------
     * 6. Cutoff holds last chi.
     * ------------------------------------------------------------ */
    {
        lvdc_igm_state_t s2;
        lvdc_igm_init(&s2, &p);
        double cy = 0, cz = 0;
        lvdc_igm_update(&s2, dT, 7700.0, MoverF,
                        R_S0, V_S0, G_S0,
                        25.0, 0.0, 0.5, &cy, &cz);
        lvdc_igm_cutoff(&s2);
        CHECK(s2.mode == LVDC_IGM_MODE_POST_CUTOFF,
              "cutoff: mode becomes POST_CUTOFF");

        double cy2 = 0, cz2 = 0;
        rc = lvdc_igm_update(&s2, dT, 0.0, 0.0,
                             R_S0, V_S0, G_S0,
                             0.0, 0.0, 0.0, &cy2, &cz2);
        CHECK(rc == 0, "POST_CUTOFF update rc=0");
        CHECK_CLOSE(cy2, cy, 1e-15, "POST_CUTOFF: chi_y held");
        CHECK_CLOSE(cz2, cz, 1e-15, "POST_CUTOFF: chi_z held");
    }

    /* --------------------------------------------------------------
     * 7. NULL guards.
     * ------------------------------------------------------------ */
    lvdc_igm_init(NULL, &p);
    lvdc_igm_init(&s, NULL);
    lvdc_igm_emrc(NULL, 0.0);
    lvdc_igm_cutoff(NULL);
    CHECK(lvdc_igm_update(NULL, dT, 0, 0, R_S0, V_S0, G_S0,
                          0, 0, 0, NULL, NULL) == -1,
          "update NULL state -> -1");

    if (g_fail == 0) {
        printf("test_igm_orchestrator_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr,
            "test_igm_orchestrator_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
