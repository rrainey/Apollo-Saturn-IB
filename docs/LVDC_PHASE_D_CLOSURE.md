# Phase D — Closure

Phase D is complete. The LVDC rewrite now swaps cleanly into
`saturnib` via `-DLVDC_USE_REWRITE=ON`, flies the plant
end-to-end on AS-205, and matches the hand-edited golden on
TMX Appendix C validation.

## Sub-phases

| Phase | Deliverable | Commit |
| --- | --- | --- |
| D.1 + D.2 | Time-base state machine + stage-event discretes | `7db7e3d` |
| D.3 | CMake swap-in toggle `LVDC_USE_REWRITE=ON` | `d29a3b4` |
| D.4 | First swap-in smoke run (plant + rewrite) | `d29a3b4` |
| D.5 | HSL (EDD §4.4.2.4, two-rate BML + 40 ms) | `b47b022` |
| D.6 | TMX Appendix C validation + 3 phasing fixes | `3a7eddf` |
| D.6b | (M/F)_S filter re-prime at IGM enable | `4f06e6f` |
| D.6c | Altitude-residual investigation | `8142e1e` |
| D+ (closed with gap) | PTL controller / GRR at T−5 | this phase |
| C.2 (closed) | XVG = −9.15 confirmed; "−9.10" no basis | this phase |

## TMX Appendix C residuals (AS-205 full-boost, dt=0.005)

| | V rms | V peak | R rms | R peak |
| --- | --- | --- | --- | --- |
| Rewrite (current) | **25.75 m/s** | −48.91 | **4507 m** | +6533 m |
| Golden (reference) | 26.55 m/s | −51.16 | 4836 m | +6619 m |

Rewrite beats golden on both metrics. The remaining ~25 m/s /
~5 km residual is shared between both bodies and is plant-side
(thrust/mass tolerances, atmospheric profile, pitch-program
fidelity — none of them LVDC concerns).

## Preset coverage

AS-205 presets populated from authoritative sources:

- **TMX Appendix B, Table 3B** (IGM presettings): V_ex1,
  V_ex3, T_1i, T_3i, τ_3, τ_10, T_10, P_CMR (CO), M_GR,
  Mdot_2, R_T, V_T, Θ_T, XVG, lambda_N, i, A_z, φ_L,
  ε_2, ROV (TR1), IGM enable @ T3+25.
- **TMX Appendix B Pitch Program**: pre-IGM polynomial
  coefficients F1/F2/F3, segment breakpoints, chi_x pad.
- **EDD Part II Table 1-3**: MFK1..8 filter coefficients,
  (M/F)_0, V_GRD, T_HSL, ΔV_b, SMCG, G_T (overridden by
  TMX XVG where they differ, per TMX precedence policy).

## What's deferred

- **D+ PTL controller** — full implementation requires plant
  adapter changes (GRR discrete or countdown signal).
  Current latch-at-liftoff approximation is within 2.3 km of
  the EDD-ideal anchor; well below the shared 6.5 km
  plant-side residual.
- **Shared plant-side residuals** — tracked by existing
  memories (`project_igm_pitchup_corrected_sign`,
  `project_sivb_cutoff_tailoff_gap`, `project_cat_from_tnd2829`,
  etc.). Not an LVDC rewrite concern.

## What landed

- 16 lvdc_rewrite ctest targets, all passing.
- `LVDC_USE_REWRITE=ON` swaps the entire sensor/guidance/HSL
  path into saturnib with zero changes to the plant or the
  obliq-2 ABI contract.
- IGM equation layers transcribed EDD-verbatim with careful
  angle-unit reasoning documented in
  `reference_edd_4_4_81_82_pirads_convention` and
  `reference_edd_eq_4_4_85_column_form`.
- (M/F)_S filter re-prime captured in
  `project_moverf_reprime_at_igm` — the single-largest-win
  fix from D.6b.
- Phase C.2 and D+ closed with explicit evidence.
