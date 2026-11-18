# LVDC IGM — Integration Status and Remaining Work

## Status (Phase C.3f-2, 2026-11-18)

The IGM equation layers and the per-BML orchestrator exist and are
tested end-to-end:

- `lvdc_igm_frames.{c,h}`         — EDD §2.3.4-5, §4.4.2.2 Eqs. 4.4.18-19, 4.4.42-45
- `lvdc_igm_intermediate.{c,h}`   — EDD §4.4.2 Eqs. 4.4.20-28
- `lvdc_igm_phasing.{c,h}`        — EDD §4.4.2.3 Eqs. 4.4.7-17
- `lvdc_igm_steering.{c,h}`       — EDD §4.4.2.2 Eqs. 4.4.29-56
- `lvdc_igm_position_correction.{c,h}` — EDD §4.4.2.2 Eqs. 4.4.51, 4.4.57-82
- `lvdc_igm_output.{c,h}`         — EDD §4.4.2.2 Eqs. 4.4.83-87
- `lvdc_igm.{c,h}`                — mode state machine and
                                    per-BML orchestrator

The dispatcher (`LVDA_LVDC.c`) instantiates the IGM state and calls
`lvdc_igm_init` at reset, but DOES NOT run `lvdc_igm_update`. The
`g_igm_enabled` flag stays false. The attitude command pipeline
still uses the pre-IGM polynomial throughout boost.

## Why IGM is not yet active

Three upstream items must close before the dispatcher can run IGM:

### 1. Preset population (TMX Appendix B / EDD Part II)

`lvdc_params.c` declares every IGM input but leaves the following
AT ZERO (grep `target_`, `V_ex`, `Mdot_`, `F1_N`, `F3_N`, `M1_kg`,
`M03_kg`, `V_GRD_mps`, `T_HSL_sec`):

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

Plus the IGM-specific preset fields (T_1i_initial, T_3i_initial)
that do not yet exist in `lvdc_params_t` and need to be added. A
new helper `lvdc_igm_presets_from_params(const lvdc_params_t*)` is
the natural place to assemble `lvdc_igm_presets_t`.

### 2. (M/F)_S smoothing (EDD §4.2.5-6)

IGM consumes `MoverF_s_s2pm`, the smoothed reciprocal specific
force. The EDD defines:

- Eq. 4.2.X — instantaneous (M/F) from V_m delta:
  `(M/F)_inst = ΔT / ΔV_m` over one BML.
- §4.2.5 — four-point moving average with magnitude and rate
  limits (`MoverF_s_limit_s2pm`, `MoverF_s_rate_limit_per_BML`).
- §4.2.6 — initialisation from `MoverF_0_s2pm` before T3+0.

`lvdc_params_t` already holds the limit and seed values. The
smoothing code itself is not yet written. This is a Phase B/C
gap, not a Phase C.3 task, but IGM blocks until it is done.

### 3. IGM enable latch

The LVDC transitions from pre-IGM pitch polynomial to IGM at a
specific instant during Time Base 4 (S-IVB burn). The enable
depends on:

- T4 start time (S-IVB ignition latch).
- Chi-freeze window after staging — the LVDC holds the last
  pre-IGM command for a few seconds across separation before
  starting IGM.

EDD §4.1 and Part II Event Sequence Timeline give the authoritative
timing. The dispatcher needs:

- An S-IVB-ignition latch (plant input or time-base event).
- A chi-freeze timer.
- The IGM-enable edge-trigger that calls `lvdc_igm_init` with
  populated presets and then flips `g_igm_enabled` true.

## Suggested future sub-phases

- **C.3f-3** — Populate `lvdc_params.c` IGM fields from TMX App B
  and EDD Part II. Add `lvdc_igm_presets_from_params` helper. Add
  smoke test that round-trips an AS-205 preset struct.
- **C.3f-4** — Implement §4.2.5-6 (M/F) smoothing in the nav layer.
- **C.3f-5** — IGM enable latch and S-IVB ignition hook. Replace
  the pre-IGM polynomial with IGM output once enabled.
- **C.3g**   — Shadow replay over full boost with IGM active.

## What the current scaffold proves

- `lvdc_igm_init` compiles and links from the dispatcher.
- The IGM state struct does not conflict with existing module
  statics.
- The dispatcher continues to produce byte-identical outputs
  because `g_igm_enabled` stays false; the current body's
  behaviour is unchanged.
