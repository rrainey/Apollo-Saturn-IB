/*
 * test_igm_frames_smoke.c — unit tests for EDD §2.3.4, §2.3.5, and
 * §4.4.2.2 Eqs. 4.4.18-19, 4.4.42-45.
 *
 * Checks:
 *   1. [M4V] is an exact planar rotation about Y_4 (orthogonal,
 *      det = 1).
 *   2. [MS4] is orthogonal (M M^T = I) for generic input angles.
 *   3. [MSV] = [M4V] [MS4] matches the explicit matmul.
 *   4. R_S -> R_4 -> R_V has the same magnitude as R_S (rotations
 *      preserve length).
 *   5. phi_T = 0 collapses [M4V] to identity so R_V = R_4.
 *   6. Aliased calls (out == in) give the same answer as unaliased.
 */

#include "../lvdc_igm_frames.h"

#include <math.h>
#include <stdio.h>

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

static double vec_norm3(const double v[3]) {
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static void check_orthonormal(double M[3][3], const char *label) {
    double MT[3][3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) MT[i][j] = M[j][i];
    double P[3][3];
    lvdc_igm_matmul3(M, MT, P);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            const double exp = (i == j) ? 1.0 : 0.0;
            CHECK_CLOSE(P[i][j], exp, 1e-12, label);
        }
    }
}

int main(void) {
    /* --------------------------------------------------------------
     * 1. [M4V] is a planar rotation about Y_4.
     * ------------------------------------------------------------ */
    {
        const double phi_T = 0.27;
        double M4V[3][3];
        lvdc_igm_build_M4V(phi_T, M4V);

        /* Y-axis column must be (0, 1, 0). */
        CHECK_CLOSE(M4V[0][1], 0.0, 1e-15, "M4V row 0 col 1");
        CHECK_CLOSE(M4V[1][1], 1.0, 1e-15, "M4V[1][1]=1");
        CHECK_CLOSE(M4V[2][1], 0.0, 1e-15, "M4V row 2 col 1");
        CHECK_CLOSE(M4V[1][0], 0.0, 1e-15, "M4V row 1 col 0");
        CHECK_CLOSE(M4V[1][2], 0.0, 1e-15, "M4V row 1 col 2");

        /* (0,0) and (2,2) are cos, (0,2) and (-2,0) are sin / -sin. */
        CHECK_CLOSE(M4V[0][0], cos(phi_T), 1e-15, "M4V cos");
        CHECK_CLOSE(M4V[2][2], cos(phi_T), 1e-15, "M4V cos");
        CHECK_CLOSE(M4V[0][2],  sin(phi_T), 1e-15, "M4V +sin");
        CHECK_CLOSE(M4V[2][0], -sin(phi_T), 1e-15, "M4V -sin");

        check_orthonormal(M4V, "M4V orthonormal");
    }

    /* --------------------------------------------------------------
     * 2. phi_T = 0 collapses [M4V] to identity.
     * ------------------------------------------------------------ */
    {
        double M4V[3][3];
        lvdc_igm_build_M4V(0.0, M4V);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                const double exp = (i == j) ? 1.0 : 0.0;
                CHECK_CLOSE(M4V[i][j], exp, 1e-15, "M4V(0)=I");
            }
        }
        /* R_V = R_4 when phi_T = 0. */
        const double R_4[3] = {7.0e6, 0.0, -1.0e6};
        double R_V[3];
        lvdc_igm_r_to_V(M4V, R_4, R_V);
        CHECK_CLOSE(R_V[0], R_4[0], 1e-9, "phi_T=0: R_V = R_4");
        CHECK_CLOSE(R_V[1], R_4[1], 1e-9, "phi_T=0: R_V = R_4");
        CHECK_CLOSE(R_V[2], R_4[2], 1e-9, "phi_T=0: R_V = R_4");
    }

    /* --------------------------------------------------------------
     * 3. [MS4] is orthogonal for a representative AS-205 set.
     *    Use plausible angles — exact values are not critical for
     *    orthonormality checks.
     * ------------------------------------------------------------ */
    double MS4[3][3];
    {
        const double lambda = 0.1;    /* rad */
        const double phi_L  = 0.491;  /* KSC geodetic latitude ~28.5 deg */
        const double A_z    = 1.5708; /* ~90 deg azimuth */
        const double i_orb  = 0.5236; /* ~30 deg inclination */
        lvdc_igm_build_MS4(lambda, phi_L, A_z, i_orb, MS4);
        check_orthonormal(MS4, "MS4 orthonormal");
    }

    /* --------------------------------------------------------------
     * 4. [MSV] = [M4V] [MS4] matches explicit matmul.
     * ------------------------------------------------------------ */
    {
        const double phi_T = 0.19;
        double M4V[3][3], MSV[3][3], MSV_ref[3][3];
        lvdc_igm_build_M4V(phi_T, M4V);
        lvdc_igm_build_MSV(M4V, MS4, MSV);
        lvdc_igm_matmul3(M4V, MS4, MSV_ref);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                CHECK_CLOSE(MSV[i][j], MSV_ref[i][j], 1e-15,
                            "MSV = M4V * MS4");
            }
        }
    }

    /* --------------------------------------------------------------
     * 5. Rotations preserve length: |R_V| = |R_S|.
     * ------------------------------------------------------------ */
    {
        const double phi_T = 0.33;
        double M4V[3][3], MSV[3][3];
        lvdc_igm_build_M4V(phi_T, M4V);
        lvdc_igm_build_MSV(M4V, MS4, MSV);

        const double R_S[3] = {5.8e6, 1.2e6, -4.1e6};
        double R_V[3];
        lvdc_igm_matvec3(MSV, R_S, R_V);
        CHECK_CLOSE(vec_norm3(R_V), vec_norm3(R_S), 1e-6,
                    "|R_V| == |R_S|");
    }

    /* --------------------------------------------------------------
     * 6. Aliased in/out works.
     * ------------------------------------------------------------ */
    {
        const double R_S[3] = {1.0, 2.0, -3.0};
        double R_a[3] = { R_S[0], R_S[1], R_S[2] };
        double R_b[3];
        lvdc_igm_matvec3(MS4, R_a, R_a);   /* alias */
        lvdc_igm_matvec3(MS4, R_S, R_b);   /* no alias */
        CHECK_CLOSE(R_a[0], R_b[0], 1e-15, "matvec3 alias");
        CHECK_CLOSE(R_a[1], R_b[1], 1e-15, "matvec3 alias");
        CHECK_CLOSE(R_a[2], R_b[2], 1e-15, "matvec3 alias");

        /* matmul alias: C = A * B with C aliasing A. */
        double A[3][3], B[3][3], C_ref[3][3];
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) {
                A[i][j] = 0.1 * (i + 1) + 0.03 * (j + 1);
                B[i][j] = 0.2 * (i + 1) - 0.07 * (j + 1);
            }
        lvdc_igm_matmul3(A, B, C_ref);
        lvdc_igm_matmul3(A, B, A);   /* alias A */
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                CHECK_CLOSE(A[i][j], C_ref[i][j], 1e-12,
                            "matmul3 alias A");
    }

    if (g_fail == 0) {
        printf("test_igm_frames_smoke: ALL CHECKS PASSED\n");
        return 0;
    }
    fprintf(stderr, "test_igm_frames_smoke: %d CHECK(S) FAILED\n",
            g_fail);
    return 1;
}
