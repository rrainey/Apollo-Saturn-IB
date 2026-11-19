/*
 * lvdc_igm_frames.h — IGM reference-frame matrices and transforms
 * (EDD §2.3.4, §2.3.5, §4.4.2.2 Eqs. 4.4.18-19, 4.4.42-45).
 *
 * Three IGM frames participate in the guidance equations:
 *   S-system : inertial launch frame. The nav block publishes R_S, V_S.
 *   4-system : launch frame rotated by (lambda, phi_L, A_z, i) — the
 *              fixed-orientation frame in which the range-angle phi_i
 *              = Arctan(Z_4 / X_4) is defined (Eq. 4.4.30). [MS4] is
 *              a per-mission constant.
 *   V-system : in-plane frame aligned with the predicted terminal
 *              radius. [M4V] is a planar rotation about Y_4 by the
 *              predicted terminal range angle phi_T, so it rebuilds
 *              each major loop (EDD §2.3.4).
 *
 * The composite [MSV] = [M4V] [MS4] (Eq. 4.4.42) rotates a vector
 * directly from S-system to V-system and is used for G_V (Eq.
 * 4.4.45) where going through the 4-system gains no accuracy.
 *
 * All matrices are stored row-major as `double m[3][3]`.
 */

#ifndef LVDC_IGM_FRAMES_H
#define LVDC_IGM_FRAMES_H

#ifdef __cplusplus
extern "C" {
#endif

/* EDD Eq. 2.3.5 — build the [MS4] 3x3 rotation. Inputs are
 * per-mission constants from the preset trajectory (TMX App B
 * for AS-205):
 *   lambda_rad : descending-node-to-launch-site angle.
 *   phi_L_rad  : launch-site geodetic latitude.
 *   A_z_rad    : flight azimuth at launch.
 *   i_rad      : target orbit inclination.
 *
 * Writes nine elements to MS4[3][3]. */
void lvdc_igm_build_MS4(double lambda_rad,
                        double phi_L_rad,
                        double A_z_rad,
                        double i_rad,
                        double MS4[3][3]);

/* EDD Eq. 2.3.4 — build the [M4V] 3x3 rotation from the predicted
 * terminal range angle phi_T (Eq. 4.4.33). */
void lvdc_igm_build_M4V(double phi_T_rad, double M4V[3][3]);

/* 3x3 matrix times 3-vector. r_out = A * r_in. r_out may alias r_in.
 *
 * `A` is not modified by the function. ISO C's pointer-to-array
 * qualifier rules (fixed only in C2X) force us to drop `const` on
 * the 2D-array parameter to avoid pedantic warnings at every call
 * site. Treat `A` as logically read-only. */
void lvdc_igm_matvec3(double A[3][3],
                      const double r_in[3],
                      double r_out[3]);

/* 3x3 matrix product. C = A * B. C may alias A or B. */
void lvdc_igm_matmul3(double A[3][3],
                      double B[3][3],
                      double C[3][3]);

/* EDD Eq. 4.4.18 — R_4 = [MS4] R_S.
 * EDD Eq. 4.4.19 — V_4 = [MS4] V_S.
 * Thin wrappers. */
void lvdc_igm_r_to_4(double MS4[3][3], const double R_S[3],
                     double R_4[3]);
void lvdc_igm_v_to_4(double MS4[3][3], const double V_S[3],
                     double V_4[3]);

/* EDD Eq. 4.4.42 — [MSV] = [M4V] [MS4]. */
void lvdc_igm_build_MSV(double M4V[3][3],
                        double MS4[3][3],
                        double MSV[3][3]);

/* EDD Eq. 4.4.43-45 — R_V = [M4V] R_4, V_V = [M4V] V_4,
 *                     G_V = [MSV] G_S. */
void lvdc_igm_r_to_V(double M4V[3][3], const double R_4[3],
                     double R_V[3]);
void lvdc_igm_v_to_V(double M4V[3][3], const double V_4[3],
                     double V_V[3]);
void lvdc_igm_g_to_V(double MSV[3][3], const double G_S[3],
                     double G_V[3]);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_IGM_FRAMES_H */
