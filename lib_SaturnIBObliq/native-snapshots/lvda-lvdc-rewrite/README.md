# LVDA_LVDC rewrite

**Branch:** `lvdc-rewrite`
**Status:** default LVDA_LVDC implementation. Linked into `saturnib` by default (`LVDC_USE_REWRITE=ON`). The legacy hand-edited golden lives at `../lvda-lvdc-legacy/` and is used when `-DLVDC_USE_REWRITE=OFF`.

An EDD-anchored re-implementation of the Saturn IB Instrument Unit
LVDA/LVDC flight software. Validated against TMX Appendix C: beats
the legacy golden on both velocity and position residuals
(25.75 m/s / 4507 m vs 26.55 m/s / 4836 m at AS-205 full-boost).
See `docs/LVDC_PHASE_D_CLOSURE.md` for the validation record and
`docs/LVDC_REWRITE_PLAN.md` for the original scope / module layout.

## Module layout

| File | EDD anchor | Content |
|------|------------|---------|
| `LVDA_LVDC.h` | n/a (ABI) | Public obliq-2 native atomic ABI. Field-identical to `../lvda-lvdc-legacy/LVDA_LVDC.h` so the parent model compile does not change. |
| `LVDA_LVDC.c` | Dispatcher | obliq-2 entry points. Thin coordinator — real work lives in the modules below. Also populates the plant shim globals (`s_*` in `lvdc_plant_shim.c`) for `--igm-trace`. |
| `lvdc_params.{c,h}` | Part II + TM-X-62831 | Preset parameter catalogue. Every value cited. |
| `lvdc_internal.h` | n/a | Internal types for the rewrite. Not exposed on the parent ABI. |
| `lvdc_frames.{c,h}` | §2.3 | Reference-frame transforms: `[MSG]`, `[MES]`, `[MBS]`, `[MSV]`, `[MSA]`. |
| `lvdc_quant.{c,h}` | §4.2.1, §8.2.1, §8.2.3 | PIPA, resolver and D/A quantization helpers. |
| `lvdc_nav.{c,h}` | §4.2, §4.3 | Boost navigation: accel data processing, F/M, M/F smoothing, nav integration (Eqs. 4.3.1-5), oblate gravity (Eqs. 4.3.6-12). |
| `lvdc_preigm.{c,h}` | §4.4.1 | First-stage time-tilt chi (pre-IGM polynomial pitch program). |
| `lvdc_timebase.{c,h}` | §7.2, §3.4 | Time-base state machine T1..T4 + stage-event discretes. |
| `lvdc_igm.{c,h}` | §4.4.2 | IGM orchestrator: mode state machine (PHASE1 → PHASE2_ARTIFICIAL_TAU → PHASE2_REAL_TAU → POST_CUTOFF), Eq. 4.4.52 T_3i advance, ε_2 Ẋ-steering gate. |
| `lvdc_igm_intermediate.{c,h}` | §4.4.2.2 | IGM intermediate parameters (`tau`, `L`, `J`, `S`, `P`, `Q`, `U`). |
| `lvdc_igm_phasing.{c,h}` | §4.4.2.2 | Phase-gated updates for `L'_3`, `J'_3`, `delta_2`, `dT_3'`. |
| `lvdc_igm_frames.{c,h}` | §2.3.4-5 | MS4/M4V frame matrices + Eqs. 4.4.18-19, 4.4.42-45. |
| `lvdc_igm_steering.{c,h}` | §4.4.2 | Eqs. 4.4.29-56: T*, φ_T, G*_V, ΔV_V, preliminary χ. |
| `lvdc_igm_position_correction.{c,h}` | §4.4.2.2 | Eqs. 4.4.81/82 all-radians transcription (`chi_y4`, `chi_z4` with `M_PI_2` half-rev offset and K radians-not-pirads). |
| `lvdc_igm_output.{c,h}` | §4.4.2 | SMC integrator + thrust-vector transform. Unit thrust: `(cos χ_y4·cos χ_z4, sin χ_z4, -sin χ_y4·cos χ_z4)`. |
| `lvdc_moverf.{c,h}` | §4.2.11 | 8-tap IIR (M/F)_S smoothing filter. Primed at init AND re-primed at IGM enable (EDD §4.2.6). |
| `lvdc_hsl.{c,h}` | §4.4.2.4 | Two-rate HSL: `bml_step()` (math + 40 ms test) and `minor_step()` (decrement + <60 ms fire). |
| `lvdc_chi_to_psi.{c,h}` | §4.5 | Chi → Psi transform (obliq-2 "Wave C" roll/pitch/yaw). |
| `lvdc_minor_loop.{c,h}` | §4.5, §7.2 | Minor-loop steering sample path. |
| `lvdc_dac.{c,h}` | §8.2.3 | D/A quantization + command zero. |
| `lvdc_plant_shim.c` | n/a | File-scope globals (`s_V_Sg`, `s_G_S_prev`, `s_XS`, `s_VS`, `s_V_S_mag_mps`, `s_T_HSL_bias_sec`, `s_T_3_eff_sec`, `s_igm_enabled`, `s_hsl_V_*`) that `SaturnIBPlantObliq_fill_igm_trace` reads for `--igm-trace`. The dispatcher mirrors rewrite state into these each BML. |

## Non-goals (v1)

- A/B disagreement, zero and reasonableness tests (EDD §4.2.2).
- Backup resolver, ladder channel switchover (EDD §8.2).
- 26-bit two's-complement internal word scaling (doubles throughout).
- BML pass-to-pass timing jitter.
- EDD §5 (orbital nav), §10 (DCS), §12 (preflight), S-IVB restart.
- Full PTL controller / GRR at T−5 s (requires plant-side changes; see `docs/LVDC_PHASE_DPLUS_PTL_GRR.md`).

## Build

```bash
cmake -B build                                 # LVDC_USE_REWRITE=ON by default
cmake --build build --target batch_sim
ctest --test-dir build                         # 16 lvdc_rewrite smoke tests + 4 batch tests
```

To build against the legacy golden instead:

```bash
cmake -B build-legacy -DLVDC_USE_REWRITE=OFF
cmake --build build-legacy --target batch_sim
```

Each per-module smoke test registers with ctest as `lvdc_rewrite_<module>_smoke`. The tests use `$<TARGET_FILE:...>` for the executable path so they resolve correctly under both single-config (Makefile) and multi-config (Ninja Multi-Config, Visual Studio) generators.
