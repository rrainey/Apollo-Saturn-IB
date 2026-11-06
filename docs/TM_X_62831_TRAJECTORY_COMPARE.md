# TM-X-62831 Tables 5/6/7 vs Obliq tm904

> **Status (2026-11-06):** the residual tables and "Interpretation" section
> below describe the pre-fix archival sim `tm904_h1_table11_dense.csv`.  Since
> then, the LVDC Chi path has been corrected in two places (gravity averaging
> for ΔX_V/ΔY_V per EDD 4.4.78/4.4.65; SMCY/SMCZ all-radians convention per
> EDD 4.4.83/4.4.84 — see `LVDA_LVDC.c` block comments at `:67`, `:2616`,
> `:2929`, `:3510`, `:3631`).  Current GCS residuals on `AS-205-tm-x-62831`:
> **ΔV = −12.2 m/s** (was −239 m/s), **Δh = +5.9 km** (was −8 km, sign flipped).
> The velocity residual is nearly closed.  The **altitude bias (vehicle
> cuts off ~6 km high)** is the open item — likely upstream in the S-IVB
> mass/thrust modelling described in the "S-IVB mass deep-dive" section
> below, which remains relevant.  The Chi-freeze symptom that previously
> masked these numbers is now gated by `tests/batch/chi_freeze.sh`.

**TM source:** `reference/NASA-TM-X-62831/chandra/NASA-TM-X-62831.md` (SI HTML tables)
**TM CSV:** `model/as205-reference/tm_x_62831_trajectory.csv`
**Sim:** `artifacts/tm904_h1_table11_dense.csv` (H-1+J-2 Table 11 scales/flow; dense 1 s S-IB / 5 s S-IVB)
**Residuals:** `artifacts/tm904_vs_tm_x_62831_residuals.csv` (sim − TM, interpolated to TM times)

Epoch: flight time after first motion. TM \(V\) is space-fixed (includes ~409 m/s Earth-rate at t=0).

## Digitized TM coverage

| Stage | Table | Samples | t range |
|-------|-------|---------|---------|
| S-IB | 5 | 34 | 0–144.41 s |
| Ullage / J-2 build-up | 6 | 3–4 | 144.49–149.41 s |
| S-IVB | 7 | 31 | 149.41–624.63 s |

## Residual summary (sim − TM)

Excluded t≈0 pad sample (sim mass/thrust still settling). Ullage mass not compared (TM already S-IVB stack mass; sim sep at 145.15 s).

### S-IB (1–140 s)

| Quantity | n | mean | RMS | max\|d\| | mean % |
|----------|---|------|-----|---------|--------|
| Altitude [m] | 29 | -331.2 | 602.8 | -1488.3 | +0.84% |
| Vs [m/s] | 29 | -12.0 | 16.8 | -28.4 | -0.90% |
| Mass [kg] | 29 | +559.2 | 799.1 | +1698.6 | +0.20% |
| Thrust [N] | 29 | +196326.6 | 197815.8 | +245645.0 | +2.51% |

### Near TM sep / ullage (h, V)

| Quantity | n | mean | RMS | max\|d\| | mean % |
|----------|---|------|-----|---------|--------|
| Altitude [m] | 3 | -1404.7 | 1408.2 | -1490.2 | -2.21% |
| Vs [m/s] | 3 | -0.0 | 0.8 | +1.0 | +0.00% |

### S-IVB high mixture (~150–455 s)

| Quantity | n | mean | RMS | max\|d\| | mean % |
|----------|---|------|-----|---------|--------|
| Altitude [m] | 19 | -5374.9 | 5969.2 | -8327.0 | -3.22% |
| Vs [m/s] | 19 | -47.8 | 57.3 | -113.3 | -1.30% |
| Mass [kg] | 18 | +4356.8 | 4827.7 | +9471.6 | +4.92% |
| Thrust [N] | 18 | -26984.9 | 76054.4 | -312344.0 | -2.93% |

### S-IVB low mixture / to GCS (~455–615 s)

| Quantity | n | mean | RMS | max\|d\| | mean % |
|----------|---|------|-----|---------|--------|
| Altitude [m] | 9 | -4279.5 | 4973.4 | -8000.0 | -1.90% |
| Vs [m/s] | 9 | -159.7 | 166.6 | -238.7 | -2.42% |
| Mass [kg] | 9 | +3775.8 | 3783.9 | +4216.1 | +9.08% |
| Thrust [N] | 9 | +14172.3 | 28797.6 | +84998.1 | +1.82% |

## Spot checks

| t (s) | stage | h TM | h sim | Δh | V TM | V sim | ΔV | m TM | m sim | Δm | F TM | F sim | ΔF |
|------:|-------|-----:|------:|---:|-----:|------:|---:|-----:|------:|---:|-----:|------:|---:|
| 5.00 | S-IB | 65 | 67 | 2 | 409.1 | 409.1 | -0.0 | 572029 | 571899 | -130 | 7150397 | 7265925 | 115528 |
| 50.00 | S-IB | 4693 | 4790 | 97 | 514.4 | 514.7 | 0.3 | 445055 | 445127 | 72 | 7677844 | 7865935 | 188091 |
| 100.00 | S-IB | 24697 | 24259 | -438 | 1145.1 | 1119.0 | -26.1 | 303108 | 304269 | 1162 | 8131797 | 8345975 | 214178 |
| 140.00 | S-IB | 57394 | 55906 | -1488 | 2263.7 | 2244.1 | -19.6 | 191420 | 191583 | 163 | 7949385 | 8195030 | 245645 |
| 144.49 | ULLAGE | 61995 | 60536 | -1459 | 2325.8 | 2326.9 | 1.0 | 138710 | 184884 | 46174 | 41321 | 88528 | 47208 |
| 149.41 | S-IVB | 67020 | 65513 | -1507 | 2314.1 | 2308.9 | -5.2 | 138403 | 138622 | 219 | 833448 | 413761 | -419687 |
| 170.00 | S-IVB | 86603 | 84720 | -1883 | 2384.4 | 2370.3 | -14.2 | 124483 | 133954 | 9472 | 1007592 | 1005000 | -2592 |
| 290.00 | S-IVB | 168802 | 162901 | -5901 | 3091.7 | 3047.6 | -44.1 | 100186 | 104866 | 4680 | 1006414 | 1005000 | -1414 |
| 454.78 | S-IVB | 219165 | 210951 | -8214 | 4973.8 | 4860.5 | -113.3 | — | — | — | — | — | — |
| 610.00 | S-IVB | 227949 | 226876 | -1073 | 7665.4 | 7437.6 | -227.7 | 31585 | 35058 | 3473 | 776228 | 783300 | 7072 |
| 614.63 | S-IVB | 227941 | 227079 | -862 | 7780.7 | 7541.9 | -238.7 | 30734 | 34184 | 3450 | 775758 | 783300 | 7542 |

## Interpretation

1. **S-IB:** After Table 11 H-1 SL retarget, velocity tracks TM closely through mid-boost (ΔV ~0–few m/s early; ~−20 m/s by IECO). Altitude runs slightly high early then ~1.5 km low by sep. Mass within ~0.1–0.4%. Stage thrust is ~+2–3% hot vs TM Table 5 history (LUT altitude rise on top of SL ratings).
2. **Separation:** Flag at 145.15 s vs TM 144.49 s. Near-sep V essentially matched (−/+1 m/s); h still ~1.5 km low.
3. **S-IVB:** High-MR thrust plateau matches Table 11 (~1.005e6 N). Trajectory still under-performs: midcoast Δh ~−5 to −8 km, ΔV growing to **−239 m/s at GCS** (7482 vs 7781). Sim mass ~3–9 t heavy → lower acceleration. Mode→4 / GCS not reached by 920 s.
4. **Thrust caveat:** S-IVB `thr_N` is the J-2 `Thrust_vs_Time` LUT output (not necessarily gated net force during buildup/cutoff). Large ΔF at 149.41 s is buildup-timing, not cruise error.

## Reproduce

```bash
# TM CSV already at model/as205-reference/tm_x_62831_trajectory.csv
/tmp/verify-h1-table11-cgen/build/tm904_probe > artifacts/tm904_h1_table11_dense.csv \
  2> artifacts/tm904_h1_table11_dense.err
```

## S-IVB mass deep-dive

### Anchors (TM Table 12 / Table 7) — model matches sep

| Epoch | Table 12 | Table 7 | Sim |
|-------|----------|---------|-----|
| S-IVB separation | 305803 lbm = **138709.9 kg** | 138709.92 @ 144.49 | **138709.91** @ 146 |
| 90% J-2 thrust | 305127 lbm = 138403.3 kg | 138403.47 @ 149.41 | 138665 (buildup) |
| GCS | 67757 lbm = **30734.1 kg** | 30734.06 @ 614.63 | **34113** (integrator) |
| Propellant consumed | 228164 lbm = 103493 kg | — | ~104.5 t (integrator, no LES) |

### Propellant flow is not the main story

| Regime | TM \(\dot m\) (Table 7 FD) | Sim | Notes |
|--------|----------------------------|-----|-------|
| High-MR cruise | ~242 kg/s | **242.4** | \(243.687\times T/1.010719\times10^6\) |
| Low-MR cruise | ~184 kg/s | **188.9** | ~+2.6% hot |

Isp ~423 s both regimes in sim; TM low-MR ~431 s (slightly lower \(\dot m\)).

### The +3.5–4.7 t “overweight” is LES (mostly)

Table 1 sequence:

| Event | TM \(t\) | Mass effect (Table 12) |
|-------|----------|------------------------|
| Ullage motor case jettison | **156.41 s** | 214 lbm ≈ 97 kg |
| **LES / tower jettison** | **163.11 s** | **8986 lbm ≈ 4076 kg** |

That is exactly the Table 7 mass step 163.11 → 170 (after propellant burn):  
\(135084 - 242\times6.89 - 4076 \approx 129337\) (matches first t=170 row).

**Model behavior:**

| Item | Model | TM |
|------|-------|-----|
| LES mass drop | **−8450 lbm (−3833 kg)** | −8986 lbm (−4076 kg) |
| Ullage cases | −220 lbm (−100 kg) | −214 lbm (−97 kg) |
| LES gate | `T1_Timer ≥ 186.88` (~**24 s late**) | 163.11 s |
| Ullage gate | `T1_Timer ≥ 159.18` | 156.41 s |
| Applied to EOM? | Yes — `Subtract = m_int + ΔLES + Δullage` used in \(a=F/m\) | — |
| Logged in tm904 probe? | **No** — probe prints integrator `vehicle_mass_kg` only | — |

So the residual CSV overstates mass error after ~163–187 s: dynamics may already be using the lighter `Subtract` mass once the late LES flag trips, while the compare still reads the heavy integrator.

If sim GCS integrator 34113 kg had LES+ullage removed: \(34113-3833-100\approx30180\) kg vs TM 30734 (**~−0.55 t**, slightly light). Remaining gap is low-MR \(\dot m\) / timing / LES mass constant (8450 vs 8986 lbm).

### Digitization note

Table 7 SI listing had two rows labeled `170.00`; the second (mass 124482.83, range 142568) is **t=190** by continuity (mdot≈243 to neighbors). Corrected in `tm_x_62831_trajectory.csv`.

### Fixes applied (mass path)

Implemented via `applySivbJettisonTable12` + patched `saturn_ib_stack.json` / MDL; probe logs EOM `Subtract` mass.

| Item | Before | After (TM) |
|------|--------|------------|
| Ullage gate | T1 ≥ 159.18 | **T1 ≥ 156.41** |
| LES gate | T1 ≥ 186.88 | **T1 ≥ 163.11** |
| Ullage Δm | −220 lbm | **−214 lbm** |
| LES Δm | −8450 lbm | **−8986 lbm** |
| tm904 mass column | integrator | **`Subtract` (EOM)** |

Re-run: `artifacts/tm904_les_table12.*` (cgen `/tmp/verify-les-table12-cgen`).

| Event | Sim | TM |
|-------|-----|-----|
| Ullage jettison | **156.43 s**, m=137132 | 156.41 s |
| LES jettison | **163.13 s**, m=131445 | 163.11 s |

Post-fix mass residuals (sim − TM, EOM mass):

| t (s) | Δm | notes |
|------:|---:|-------|
| 170 | **+443 kg (+0.34%)** | was ~+4.6 t on integrator |
| 290 | **+506 kg (+0.51%)** | |
| 450 | **+520 kg (+0.85%)** | |
| 614.63 | **−794 kg (−2.6%)** | slightly light; low-MR \(\dot m\) still ~189 vs TM ~184 |

GCS \(V\): 7581 vs TM 7781 (was 7550) — small gain from earlier LES; remaining ΔV still open.

Optional follow-up: impulse the mass integrator on jettison so integrator and `Subtract` stay consistent; trim low-MR \(\dot m\) toward Table 7 ~184 kg/s.

## S-IVB Chi steering (CHIP / CHIY / CHIR)

**TM CSV:** `model/as205-reference/tm_x_62831_chi.csv` (Tables 6/7)  
**Residuals:** `artifacts/tm904_vs_tm_chi_residuals.csv`  
**Mapping:** sim `Chi_Y` ≈ TM **CHIP** (pitch), sim `Chi_Z` ≈ TM **CHIY** (yaw). `CHIR`≈0 in TM; not logged in probe.

IGM initiates at TB3+25 ≈ **168.11 s**. TM CHIP/CHIY leave the App-B hold (−59.32°, 0) by **t=170**.

### Headline

| Phase | Pitch Δ (Chi_Y−CHIP) | Yaw Δ (Chi_Z−CHIY) |
|-------|----------------------|--------------------|
| Pre-IGM hold | Match (~0°) | Match (~0°) |
| TM IGM era (170–455) | Sim still held −59.3 → large lag | Sim χ_Z≈0 vs TM ~−4° |
| Late / GCS | Pitch converges toward TM ~−106° | **Sim yaw ≫ TM** (sim ~+40° vs TM ~−4°) |

Sim `nIGMMode` 0→1 only at **~473 s** — ~305 s late vs TM IGM start. Chi still moves earlier (~190 s), but formal mode lag remains.

### Stale preset: `lambda_0_deg` / θ_N (fixed)

Your suspicion was right. MS4 / AP is built on-the-fly from `(A_z, φ_L, λ_0, i)`:

| Angle | Was (TN hold) | TM Table 3B AP note |
|-------|---------------|---------------------|
| A_z | 72 ✓ | 72 |
| φ_L | 28.521963 ✓ | 28.521963 |
| **θ_N / `lambda_0_deg`** | **102.329** | **119.0** |
| i | 31.605 ✓ | 31.605 |

Updated packs + `saturn_ib_stack.json`; re-run `artifacts/tm904_lambda0_119.*`.

| Metric | Before λ₀ fix | After λ₀=119 | TM |
|--------|---------------|--------------|-----|
| Chi_Z @ GCS | **+44°** | **+8.3°** | −4.0° |
| Chi_Y @ GCS | −137° | **−111°** | −106° |
| Yaw RMS low-MR | ~34° | **~9°** | — |
| \|V\| @ ~615 | 7581 | **7802** | 7781 |

Yaw still has a ~**+8–12°** bias (sign opposite TM CHIY); runaway / ΔV hole largely closed. Remaining: yaw polarity / SMCZ, and late `nIGMMode`.
