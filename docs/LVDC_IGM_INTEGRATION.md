# LVDC IGM — Integration Status

## Status (Phase C.3f-5, 2026-11-18)

**IGM is now live in the dispatcher.** The attitude command
pipeline uses IGM pitch and yaw once `g_igm_enabled` latches on
the first BML at or after T1 ≥ `igm_enable_from_T1_sec` (166.8 s
for AS-205 per TMX T3+25). Pre-IGM pitch poly covers the pad
hold and S-IB phase; IGM takes over for S-IVB.

The IGM equation layers and the per-BML orchestrator:

- `lvdc_igm_frames.{c,h}`         — EDD §2.3.4-5, §4.4.2.2 Eqs. 4.4.18-19, 4.4.42-45
- `lvdc_igm_intermediate.{c,h}`   — EDD §4.4.2 Eqs. 4.4.20-28
- `lvdc_igm_phasing.{c,h}`        — EDD §4.4.2.3 Eqs. 4.4.7-17
- `lvdc_igm_steering.{c,h}`       — EDD §4.4.2.2 Eqs. 4.4.29-56
- `lvdc_igm_position_correction.{c,h}` — EDD §4.4.2.2 Eqs. 4.4.51, 4.4.57-82
- `lvdc_igm_output.{c,h}`         — EDD §4.4.2.2 Eqs. 4.4.83-87
- `lvdc_igm.{c,h}`                — mode state machine and
                                    per-BML orchestrator

The dispatcher (`LVDA_LVDC.c`) builds the IGM preset struct from
`g_params` via `lvdc_igm_presets_from_params`, primes the
(M/F)_S smoothing with `(M/F)_0`, and runs the orchestrator once
per BML inside the existing BML gate. The output chi_y, chi_z
land in `g_chi_igm_rad` and replace the preigm pitch/yaw in
`step_attitude_pipeline` once the enable latch flips.

## Three upstream items (now closed)

The three items that blocked integration in Phase C.3f-2 are
done:

### 1. Preset population (TMX Appendix B / EDD Part II) — **DONE** (C.3f-3)

`lvdc_params.c` now carries every AS-205 IGM preset as an
`AS205_*` #define. Populated from TMX Table 3B:

| Field                            | Authoritative source            |
| -------------------------------- | ------------------------------- |
| `target_i_deg`                   | TMX Table 3B                    |
| `target_lambda_0_deg`            | TMX Table 3B                    |
| `target_V_T_mps`                 | TMX Table 3B                    |
| `target_R_T_m`                   | TMX Table 3B                    |
| `target_Theta_T_deg`             | TMX Table 3B                    |
| `target_Xdotdot_VGT_mps2`        | EDD §4.4.2 / TMX                |
| `V_ex1_mps`, `V_ex3_mps`         | TMX App B (used in unit tests)  |
| `Mdot_1_kgps`, `Mdot_3_kgps`     | TMX Table 3B                    |
| `F1_N`, `F3_N`                   | TMX Table 3B                    |
| `M1_kg`, `M03_kg`                | TMX Table 3B                    |
| `V_GRD_mps`, `T_HSL_sec`         | EDD Part II                     |
| `SMCG`, `ROV`, `P_CMR_sec`       | EDD Part II / §4.4.2            |
| `tau_10_sec`, `T_10_sec`         | EDD §4.4.2 / TMX                |
| `M_GR_kg`, `Mdot_2_kgps`         | EDD §4.4.2                      |

The helper `lvdc_igm_presets_from_params(const lvdc_params_t*,
lvdc_igm_presets_t*)` builds the IGM preset struct from
`lvdc_params_t`. `test_igm_presets_smoke` pins every TMX value.

### 2. (M/F)_S smoothing (EDD §4.2 Eq. 4.2.9-11) — **DONE** (C.3f-4)

`lvdc_moverf.{c,h}` implements:

- Eq. 4.2.9  — A_m from V_m delta.
- Eq. 4.2.10 — F/M magnitude.
- Eq. 4.2.11 — 8-tap IIR filter (4 inst + 4 smoothed past).
- §4.2.5    — magnitude + rate clamps.

MFK1..8 and (M/F)_0 are populated from TMX Table 1-3 in
`lvdc_params_as205`. The filter is primed with (M/F)_0 at reset
and stepped each BML by the dispatcher.

### 3. IGM enable latch — **DONE** (C.3f-5, T1-based fallback)

The dispatcher latches `g_igm_enabled` on the first BML at or
after T1 ≥ `params.igm_enable_from_T1_sec` (166.8 s for AS-205,
derived from TMX T3 + 25 s and the nominal T1→T3 offset). At
latch, presets are rebuilt from `g_params` and `lvdc_igm_init`
runs so IGM starts from a clean mode state.

This is a T1-based fallback. When the Phase D time-base state
machine lands (task #6), swap it for a T3-based enable using
`params.igm_enable_from_T3_sec` directly.

## Remaining work for a full "IGM live" baseline

- **Phase D**  — Time bases T1..T7. Replace T1-based enable with
  T3-based. Also enables the chi-freeze window across staging
  (currently preigm runs right up to the IGM enable, no explicit
  freeze).
- **Phase D+** — PTL controller with EDD-faithful GRR at T-5 s
  per TMX (see `reference_as205_grr_minus_5sec`).
- **C.3g**     — Shadow replay over full boost with IGM active;
  verify residuals against TM-X-62831 trajectory tables.
- **HSL** (EDD §4.4.2.4, Eqs. 4.4.88-94) — last-10-s cutoff
  timing. Separate module, not covered by C.3f.
- **Task #9** — Resolve XVG = -9.10 vs -9.15 discrepancy.

## What the current wiring proves

- `lvdc_igm` compiles, links, and runs from the dispatcher once
  per BML during AS-205 S-IVB burn.
- Pre-enable behaviour is byte-identical to the previous
  dispatcher output (preigm pitch poly throughout).
- At T1 ≥ 166.8 s the IGM output replaces the preigm pitch/yaw
  inputs to the minor-loop ramp.
