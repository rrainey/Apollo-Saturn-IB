/*
 * lvdc_igm_frames.c — IGM reference-frame matrices and transforms
 * (EDD §2.3.4, §2.3.5, §4.4.2.2 Eqs. 4.4.18-19, 4.4.42-45).
 */

#include "lvdc_igm_frames.h"

#include <math.h>

void lvdc_igm_matvec3(double A[3][3],
                      const double r_in[3],
                      double r_out[3]) {
    const double x = r_in[0], y = r_in[1], z = r_in[2];
    r_out[0] = A[0][0] * x + A[0][1] * y + A[0][2] * z;
    r_out[1] = A[1][0] * x + A[1][1] * y + A[1][2] * z;
    r_out[2] = A[2][0] * x + A[2][1] * y + A[2][2] * z;
}

void lvdc_igm_matmul3(double A[3][3],
                      double B[3][3],
                      double C[3][3]) {
    double tmp[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            tmp[i][j] = A[i][0] * B[0][j]
                      + A[i][1] * B[1][j]
                      + A[i][2] * B[2][j];
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            C[i][j] = tmp[i][j];
        }
    }
}

void lvdc_igm_build_MS4(double lambda_rad,
                        double phi_L_rad,
                        double A_z_rad,
                        double i_rad,
                        double MS4[3][3]) {
    /* EDD Eq. 2.3.5 — verbatim. */
    const double cl = cos(lambda_rad), sl = sin(lambda_rad);
    const double cL = cos(phi_L_rad),  sL = sin(phi_L_rad);
    const double cA = cos(A_z_rad),    sA = sin(A_z_rad);
    const double ci = cos(i_rad),      si = sin(i_rad);

    MS4[0][0] =  cl * cL;
    MS4[0][1] =  cl * sL * sA + sl * cA;
    MS4[0][2] = -cl * sL * cA + sl * sA;

    MS4[1][0] =  si * sl * cL - ci * sL;
    MS4[1][1] =  si * sl * sL * sA + ci * cL * sA - si * cl * cA;
    MS4[1][2] = -si * sl * sL * cA - ci * cL * cA - si * cl * sA;

    MS4[2][0] = -ci * sl * cL - si * sL;
    MS4[2][1] = -ci * sl * sL * sA + si * cL * sA + ci * cl * cA;
    MS4[2][2] =  ci * sl * sL * cA - si * cL * cA + ci * cl * sA;
}

void lvdc_igm_build_M4V(double phi_T_rad, double M4V[3][3]) {
    /* EDD Eq. 2.3.4 — planar rotation about Y_4 by phi_T. */
    const double c = cos(phi_T_rad), s = sin(phi_T_rad);
    M4V[0][0] =  c; M4V[0][1] = 0.0; M4V[0][2] =  s;
    M4V[1][0] = 0.0; M4V[1][1] = 1.0; M4V[1][2] = 0.0;
    M4V[2][0] = -s; M4V[2][1] = 0.0; M4V[2][2] =  c;
}

void lvdc_igm_r_to_4(double MS4[3][3], const double R_S[3],
                     double R_4[3]) {
    /* Eq. 4.4.18 */
    lvdc_igm_matvec3(MS4, R_S, R_4);
}

void lvdc_igm_v_to_4(double MS4[3][3], const double V_S[3],
                     double V_4[3]) {
    /* Eq. 4.4.19 */
    lvdc_igm_matvec3(MS4, V_S, V_4);
}

void lvdc_igm_build_MSV(double M4V[3][3],
                        double MS4[3][3],
                        double MSV[3][3]) {
    /* Eq. 4.4.42 */
    lvdc_igm_matmul3(M4V, MS4, MSV);
}

void lvdc_igm_r_to_V(double M4V[3][3], const double R_4[3],
                     double R_V[3]) {
    /* Eq. 4.4.43 */
    lvdc_igm_matvec3(M4V, R_4, R_V);
}

void lvdc_igm_v_to_V(double M4V[3][3], const double V_4[3],
                     double V_V[3]) {
    /* Eq. 4.4.44 */
    lvdc_igm_matvec3(M4V, V_4, V_V);
}

void lvdc_igm_g_to_V(double MSV[3][3], const double G_S[3],
                     double G_V[3]) {
    /* Eq. 4.4.45 */
    lvdc_igm_matvec3(MSV, G_S, G_V);
}
