# TM-X-62831 full-stack compare (first cut)

**Date:** 2026-08-26  
**Model:** `model/saturn_ib_stack.json` (mdl2obliq, presetting profile **`tm-x-62831`**)  
**Run:** `tm904_probe` — ExtIn `A_z=72°`, `T_L'=300` s, `t_end=920` s  
**Artifacts:** `artifacts/tm904_obliq.csv`, `artifacts/tm904_obliq.err`  
**Reference events:** `model/as205-reference/tm_x_62831_events.csv`

## Epoch

| Quantity | Definition |
|----------|------------|
| Sim \(t=0\) | Pad / GRR-era hold |
| First motion / liftoff | \(t = T_L' = 300\,\mathrm{s}\) (`bLiftoff`) |
| Flight time | \(t_\mathrm{flight} = t_\mathrm{sim} - 300\) |
| TM table time | Seconds after **first motion** (GRR = −5 s) |

Run length 920 s covers GCS (~614.6 s flight → ~914.6 s sim).

## What changed vs TN-shaped stack

- Model parameters from NASA-TM-X-62831 (IGM, mass, \(A_z=72\), …)
- App B S-IB Time Tilt polys / breaks / `P_final`
- Constants bound with `useParameter` (mask no longer freezes TN literals)

## Event compare

| Event | TM-X-62831 | Obliq (this run) | Δ |
|-------|------------|------------------|---|
| Liftoff | 0 | ~0.01 s flight | OK |
| Physical separation | **144.49 s**, h=62.0 km, \(V_S\)=2325.8 m/s | **~145.15 s** (flag, after H-1 Table 11); at \(t_f=145\): h≈**61.07 km**, \(V\)≈**2324.7** m/s | +0.7 s; h −0.9 km; \(V\) −1.1 m/s |
| χ pitch hold (pre-IGM) | App B \(P_\mathrm{final}\) −59.315° | **χ_Y ≈ −59.32°** by \(t_f\approx135\) | Match |
| IGM mode→1 | (TB3+25 ≈ 168 s flight for IGM start) | mode 0→1 @ **476 s** flight | See note |
| Mode→4 / HSL arming | GCS **614.63 s** | mode 3→4 @ **615.2 s** flight | **+0.6 s** |
| Near-GCS state (\(t_f\approx612\)) | h=227.94 km, \(V_S\)=7780.67 | h≈**228.2 km**, \(V\)≈**7804** | h +0.3 km; \(V\) **+23 m/s (~0.3%)** |
| Post mode-4 coast proxy | — | \(V\) climbs to ~**7930** before thr drops | Overshoot / cutoff arming |

**IGM start note:** TM initiates IGM at OECO+25 s (~168 s flight). Obliq `nIGMMode` 0→1 is much later (~476 s) because mode-1 is the first-phase *mode flag* after T1 burn-down, not the “IGM equations enabled” discrete. Enabling / Chi activity earlier is not fully captured by `nIGMMode` alone in this CSV.

## Mass spot checks

| Epoch | TM | Obliq |
|-------|-----|-------|
| Liftoff \(M1\) | 585943 kg | ~572079 kg at \(t_f=5\) (already burned ~14 t — plausible under boost) |
| S-IVB @ sep (Table 12) | ~138710 kg | **138707 kg** at \(t_f=150\) | Excellent |

## Open-loop pitch (App B)

`P_final` and early χ_Y track App B (−59.315°) before S-IVB / IGM dominate. That confirms Time Tilt replacement took effect.

## Known gaps (expected residuals)

1. **S-IB sep ~2.6 s late** / slightly low h & V — engine/thrust tables, PU, aero, or discrete sep logic still TN-shaped in places.
2. **Cutoff / HSL** — mode→4 time matches GCS well, but \(|V|\) continues to rise (~7930) → HSL/`nHSLActive` / thr shutdown still not TM-tight (Phase A gate1000-style issues).
3. **χ_Z → 45°** late — saturating; IGM yaw / SMC path not yet TM-closed.
4. **Table 5/6 full time history** not yet digitized for dense residual; this report is **event + spot** only.
5. Full **batch_sim** still defaults to plant stub; this run is **full-stack cgen probe**, not ApolloA host.

## HSL follow-up (model-level, no codegen name patches)

After this report, HSL was moved off codegen heuristics:

- Explicit `sampleTimeSec: 0.04` on `HSL_Cutoff_Timing` + children (`applyHslModelLevel`)
- `nHSLActive=1` written in the same ActionPort as `nIGMMode=4`
- Removed `isRtwHslCutoffSubsystem` from mapper / ModelFlattener
- Same-step enable no longer keys on the string `Set_HSL_Mode`

Re-run (`artifacts/tm904_hsl_model.*`): mode→4 @ 615.2 s now shows **hsl=1 on the same sample** (previously hsl lagged). Coast \(|V|\) still ~7930 — remaining issue is likely **bCutoff → engine shutdown** coupling / ungated `nHSLActive→2` vs ΔT_b, not sample-rate codegen.

## J-2 thrust → TM Table 11

`Thrust_vs_Time` low plateau was ~11% hot vs NASA-TM-X-62831 Table 11. Corrected via `applyJ2ThrustTable11` (mdl2obliq emit hook) + patched `saturn_ib_stack.json`:

| Plateau | Table 11 | Model (post-fix) |
|---------|----------|-------------------|
| High (≈159–455 s flight) | **226019 lbf** | **1.005e6 N** (= 226019 lbf) |
| Low (≈480+ s flight) | **176087 lbf** | **7.833e5 N** (= 176087 lbf) |

Re-run: `artifacts/tm904_j2_table11.*` (cgen `/tmp/verify-j2-table11-cgen`).

| Quantity | Pre (hsl_model, hot low) | Post (Table 11) | TM |
|----------|--------------------------|-----------------|-----|
| thr @ \(t_f\approx560\) | ~8.48e5 N (~190.6 klbf) | **7.833e5 N (176.1 klbf)** | 176087 lbf |
| Near-GCS \(t_f\approx612\) \(V\) | 7804 m/s | **7407 m/s** | **7780.67** |
| Near-GCS \(h\) | 228.2 km | **226.7 km** | 227.94 km |
| mode→4 | @ 615.2 s | **none by 920 s** (still mode 3) | GCS 614.63 s |
| Final @ 920 s | mode 4, \(V\)≈7932 | mode 3, \(V\)≈7589 | — |

Lower low-MR thrust slows ΔV / T3 burn-down, so HSL/mode→4 no longer fires inside the 920 s window. Cutoff story shifted: previously slightly hot \(V\) with early mode→4; now under-speed at TM GCS time and still in mode 3. Indirect cutoff effect only — plant LUT, not guidance logic.

## H-1 thrust / \(\dot{W}\) → TM Table 11 (SL)

Table 11 lists **average longitudinal sea-level** thrust (H-1 + turbine) per engine and stage \(\dot{W}=6210.77\,\mathrm{lb/s}\). Model kept the shared altitude-ramped `Thrust_vs_Time` LUT; per-engine `Engine_Performance_Scaling_Factor` was retargeted so \(\mathrm{LUT}(t=0)\times\mathrm{scale}=\) Table 11 SL, and `m_dot` → \(6210.77/8\) lbm/s.

| Eng | Table 11 SL (lbf) | New scale |
|-----|-------------------|-----------|
| 1–8 | 204851 … 202648 | 1.06079 … 1.04939 |
| Stage \(\dot{m}\) | 2817.16 kg/s | **352.145 kg/s × 8** |

Re-run: `artifacts/tm904_h1_table11.*` (cgen `/tmp/verify-h1-table11-cgen`).

| Quantity | Pre (J-2-only Table 11) | **Post (H-1+J-2 Table 11)** | TM |
|----------|-------------------------|-----------------------------|-----|
| \(\dot{m}\) (boost) | 2778.4 kg/s (−1.4%) | **2817.2 kg/s** | 2817.2 |
| Sep flag | 147.07 s | **145.15 s** | 144.49 |
| Near-sep \(t_f\approx145\) \(h\) | 58.16 km | **61.07 km** | 62.00 |
| Near-sep \(V\) | 2303 m/s | **2324.7 m/s** | 2325.8 |
| Near-GCS \(t_f\approx612\) \(V\) | 7407 m/s | ~7663 @920 still mode 3 | 7780.67 |

S-IB altitude/velocity shortfall largely closed (+2.9 km, +22 m/s at sep). Remaining ~0.9 km / ~1 m/s near sep and late mode→4/cutoff still open.

## Full trajectory residuals (Tables 5/6/7)

Digitized SI listings → `model/as205-reference/tm_x_62831_trajectory.csv`; compared to dense tm904 in **`docs/TM_X_62831_TRAJECTORY_COMPARE.md`** (`artifacts/tm904_vs_tm_x_62831_residuals.csv`). Headline: S-IB V near-match after H-1 Table 11; S-IVB still ~−240 m/s / heavy mass at GCS.

## Next steps

1. Residual CLI like `as205:compare` wired to `tm_x_62831_trajectory.csv` (manual compare done).
2. Finish cutoff so coast \(|V|\) holds near **7780.7** (thr kill on `bCutoff`; review HSL Write1 vs Compare). With Table 11 H-1+J-2, near-GCS \(V\)≈7483 still low and mode→4 absent by 920 s — chase T3 / IGM energy vs plant ΔV; optional extend past 920 s.
3. Wire stack cgen into `batch_sim` (`SATURNIB_OBLIQ_MODEL=stack`) for host+AGC path.
4. Optional: parameterize Time Tilt coefs (not only emit-time patch).
5. Optional: revisit J-2 propellant flow / Isp so mass + ΔV stay consistent with the corrected thrust levels.

## Reproduce

```bash
# From current saturn_ib_stack.json (tm-x-62831 + App B tilt):
npx tsx obliq-2/scripts/obliq-cgen.ts \
  model/saturn_ib_stack.json \
  --out /tmp/verify-tm-tilt-cgen --name saturn_ib_stack \
  --profile saturn-ib-stack --compile

# Build probe (see /tmp/verify-tm-tilt-cgen/tm904_probe.c) then:
./build/tm904_probe > /tmp/tm904_obliq.csv 2> /tmp/tm904_obliq.err
```
