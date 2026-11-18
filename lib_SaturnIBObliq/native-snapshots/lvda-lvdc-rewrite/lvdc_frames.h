/*
 * lvdc_frames.h — reference-frame transforms (EDD §2.3).
 *
 * Pure functions. No module state. The callers (nav, igm) feed in the
 * angles and get the rotation back by reference.
 *
 * EDD §2.1 defines eight reference systems; this module implements
 * the subset the AS-205 boost-to-orbit path needs:
 *
 *   S  = plumbline (X_S, Y_S, Z_S) — primary inertial nav frame
 *   G  = gravitational (X_G, Y_G, Z_G)
 *   B  = body (X_B, Y_B, Z_B)
 *   V  = injection plane (X_V, Y_V, Z_V)
 *   P  = inertial platform gimbal (X_P, Y_P, Z_P)
 *   E  = ephemerid (X_E, Y_E, Z_E)
 *
 * Transform notation [M_FT] means "rotate from F to T".
 */

#ifndef LVDC_FRAMES_H
#define LVDC_FRAMES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Phase B scaffolding; full implementation lands with lvdc_nav.
 * Placeholder prototype so the CMake target compiles. */
void lvdc_frames_unused(void);

#ifdef __cplusplus
}
#endif

#endif /* LVDC_FRAMES_H */
