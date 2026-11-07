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

## IGM-enable gate: hardcoded 44 s vs TM 25 s (fixed)

**Finding (2026-11-06).**  TM-X-62831 §V-A p.796: *"The IGM equations and
logic which provide guidance in pitch and yaw are initiated at TB3 + 25
seconds."*  Table 1 row at flight 168.11 s: `(25.0)_3 Command IGM
initiation.` — the subscript denotes time-in-TB3, so the TM-preset value is
**T3_IGM = 25.0 s**.

The source Simulink model (`model/provenance/saturn_ib_stack.mdl:40417`,
block "IGM Enable (3)") carries a hardcoded `Compare To Constant
const "44.0"`, which propagated through codegen to
`lib_SaturnIBObliq/native-snapshots/lvda-lvdc/LVDA_LVDC.c:444` and the
regenerated `plant/generated/` copy.  Sibling Compare-To-Constant blocks in
the same MDL sheet match TM Table 1 (IECO at 3.2, J-2 Engine Start at 2.7);
only this one diverged.  The repo's own preset carries the correct value at
`lib_SaturnIBObliq/ts/as205TmX62831Presettings.ts:103`
(`T3_IGM_sec = 25.0`, cited Table 3B / §IV).

**Numerical match.**  44 − 25 = 19 s, matching the observed ~20 s late IGM
engagement (sim `igm_enable` flipping at flight ~188 s vs TM 168.11 s).

**Fix applied (point-fix).**  Changed the literal 44.0 → 25.0 in
`lib_SaturnIBObliq/native-snapshots/lvda-lvdc/LVDA_LVDC.c:444` only — the
obliq-2 native snapshot is what `batch_sim` links.  The legacy
`model/provenance/saturn_ib_stack.mdl` and the regenerated
`plant/generated/LVDA_LVDC.c` are deliberately left untouched (we're
focused on obliq-2 forward; the MDL is kept for historical provenance).

**Measured impact on `AS-205-tm-x-62831`:**

| Quantity | Before | After | TM |
|----------|-------:|------:|---:|
| IGM engage (flight t) | 188 s | **169 s** | 168.11 s |
| Mode→4 (flight t) | 612 s | 610 s | — |
| Engine cutoff (flight t) | 617 s | **613 s** | 614.63 s |
| h at sim cutoff | 228,113 m | 228,102 m | 227,941 m |
| V at sim cutoff | 7,768.5 m/s | **7,753.2 m/s** | 7,780.67 m/s |
| Δh at TM GCS time (614.63) | +162 m | +160 m | — |
| ΔV at TM GCS time | −12.2 m/s | **−19.0 m/s** | — |

The fix is reference-correct and should stand.  It exposed a secondary issue:
with IGM running on time, the HSL / Mode-4 cutoff now fires **~1.6 s early**
at V ~27 m/s short of `V_T_mps = 7780.67`.  Previously the late IGM was
effectively racing to catch up and cutoff landed ~2.4 s late at a V closer to
TM.  The ΔV residual is now attributable to the HSL/Mode-4 arming logic
(`epsilon_1`, `epsilon_2`, or `LVDC_Iterative_Guidance_Mode_HSL_Cutoff_Timing_*`),
not to the plant or IGM entry.

## Chi_Z yaw bias: reporting convention, not a steering error

**Finding (2026-11-06).**  The long-standing ~8° Chi_Z residual (sim ≈ +4°
vs TM CHIY ≈ −4° through the S-IVB burn) is **not** steering the vehicle
off-plane.  Ground-track comparison closes it cleanly.

**What was ruled out (audit summary):**

1. **MS4 matrix:** every element at `LVDA_LVDC.c:1835–1860` matches EDD
   Eq. (2.3.5) exactly (9/9 terms verified, A_z/φ_L/λ/i expansion with raw
   `PARAM_A_z_deg = 72°` is correct — the EDD pre-expanded the "A_z − 90°"
   rotation algebra).  [Equation extracted from
   `reference/LVDC-EDD/chandra/satinstunitibm_5/satinstunitibm_5.md:457`.]
2. **Mux assembly** `[cos χ_y cos χ_z, sin χ_z, −sin χ_y cos χ_z]` matches
   Eq. (4.4.85) `F'_S = [MS4]ᵀ · Mux`.
3. **Transpose** at `:3235–3240` (`Math_Function5 = MS4ᵀ`) correctly gives
   `Product15 = MS4ᵀ·Mux = F'_S` (S-frame thrust unit vector).
4. **Writer formula** `Chi_Z_deg = asin(Product15[1]) + SMCZ` is the
   S-frame yaw angle of the commanded thrust — a well-defined quantity.
5. **TM CHIY sign convention** is the same as sim Chi_Z: pre-IGM hold
   matches to 2 decimal places (both 0.00°); TM CHIP tracks sim Chi_Y with
   same sign throughout burn (both reach ≈ −106° at GCS).  If yaw used
   opposite convention, pitch would too — it doesn't.

**Probe data at mid-burn (t_fl=500, Mux pitch-dominated):** Mux[1] (raw IGM
yaw command) ≈ +0.005 — essentially zero.  Product15[1] ≈ +0.062 is
generated almost entirely by `MS4[0][1]·Mux[0] + MS4[2][1]·Mux[2]`,
dominated by `sin(λ)·cos(A_z)·cos(χ_y_cmd)`.  That is EDD-correct
geometry — not an algebra bug.

**Ground-track decides it.**  Comparing sim latitude to TM `LATT` (both
earth-fixed — no frame-conversion ambiguity):

| t_fl (s) | sim lat | TM LATT | Δ (°) | cross-track (km) |
|---------:|--------:|--------:|------:|-----------------:|
| 144 | 28.686 | 28.69 | −0.004 | −0.4 |
| 250 | 29.213 | 29.23 | −0.017 | −1.9 |
| 450 | 30.467 | 30.49 | −0.023 | −2.6 |
| 600 | 31.443 | 31.40 | +0.043 | +4.8 |
| 614 | 31.516 | 31.53 | −0.014 | −1.6 |

**Max deviation ±0.05° (≈ ±5 km) across the entire S-IVB burn**, with no
systematic drift.  An actual +4° yaw command applied for 470 s at ~5 km/s
would displace the vehicle by hundreds of km off-plane; instead we're
within ~5 km.  **The vehicle is yawing per TM's intent.**

**Interpretation:** the `Chi_Z_deg` data-store value we read via the IGM
trace carries one sign convention; the downstream FCC/TVC yaw loop
compensates with the opposite-sign gain polarity already documented in
`docs/H1_YAW_POLARITY_PROBE.md` (`β_Y = +Kp_yaw · Ψ_Y`, not the pitch-style
`−Kp · Ψ_Y`).  The two sign conventions cancel through the control loop,
which is why the vehicle flies the correct track despite the reported
Chi_Z sitting opposite to TM CHIY.

**Decision:** do not "fix" the Chi_Z writer.  Any change would ripple
through the FCC sign compensation and break the closed loop that already
flies correctly.  Document the artifact, move on to the real open items
(HSL cutoff timing).

**Separate observation:** sim `s1_lon_deg` reports ~1.25° west of pad at
`t_fl=0` and the longitude drift against TM `LONG` reaches +2.4° by GCS
after sidereal-rate correction.  Looks like the ECI vehicle state is not
being carried through the 300 s pad hold (the ECI_to_LLA then has GMST
advancing while Xe stays at the T_GRR value, producing a westward drift of
earth-fixed lon during pad time).  Doesn't affect the trajectory-fidelity
story but worth noting separately as a pad-hold frame-handling question.

## HSL polynomial cutoff: implementing EDD §4.4.2.4 Eqs. 4.4.91–4.4.94

**Finding (2026-11-06).**  After the IGM-enable gate fix brought engine
cutoff into the right ballpark (flight 613 s vs TM 614.63 s), the velocity
residual at GCS widened to **−19.0 m/s** rather than closed.  Investigation
revealed that the HSL cutoff gate was using a **simple linear T_3i
countdown** (subtract 0.04 s per 40 ms tick) with no reference to the actual
velocity history — effectively ignoring the EDD's polynomial cutoff
prediction.

**EDD §4.4.2.4 (p. I-4-29) specifies:**

> *"The prediction of the S-IVB cutoff time is performed with Eq. 4.4.91
> through 4.4.94.  This cutoff prediction uses a second-degree polynomial
> which expresses velocity as a function of time.  This polynomial is
> constructed using the current and two previous BML durations and
> velocity values.  The polynomial is then evaluated to give the time at
> which the cutoff velocity will be achieved.  The cutoff velocity is
> biased by the preset ΔV_b to compensate for thrust tailoff and system
> delays which cause velocity gain after the S-IVB cutoff command is
> issued."*

**Equations (from `reference/LVDC-EDD/chandra/satinstunitibm_5/satinstunitibm_5.md:897`):**

```
ΔT_b  = ΔV_b · ΔT / (V − V')                                 (4.4.88)   [already implemented]
T_3i  = T_3i − ΔT_b     (first HSL pass)                     (4.4.89)   [already implemented]
T_3i  = T_3i − ΔT       (subsequent passes)                  (4.4.90)   [already implemented]
A''   = [(V−V')·ΔT'/ΔT − (V'−V'')·ΔT/ΔT'] / (ΔT' + ΔT)      (4.4.91)   [ADDED]
A'    = A''·ΔT + (V−V')/ΔT                                    (4.4.92)   [ADDED]
T_3i  = (V_T − ΔV_b − V) / (A' + A''·T_3i)                   (4.4.93)   [ADDED]
T_CO  = T_3i + T_as                                           (4.4.94)   [implicit in fire gate]
```

**Implementation** (`lib_SaturnIBObliq/native-snapshots/lvda-lvdc/LVDA_LVDC.c`):

- Added module-scope V-history pipeline (`s_hsl_V_curr`, `s_hsl_V_prev`,
  `s_hsl_V_prev2`, `s_hsl_dT_curr`, `s_hsl_dT_prev`, `s_hsl_V_samples`)
  refreshed on each IGM major cycle right after `Math_Function3` is
  updated.
- Replaced the pure linear-countdown gate in `HSL_Cutoff_Timing_Compare_To_Constant`
  (`:3744`) with: fall back to linear `tgo` when nHSLActive=1 (first pass)
  or history incomplete; otherwise apply Eq. 4.4.91/4.4.92/4.4.93 to
  refine `tgo` using the current linear countdown as the fixed-point seed
  for `T_3i` on the RHS of Eq. 4.4.93.  Cut when refined `tgo ≤ 40 ms`.
- Sanity-clamped: polynomial refinement only trusted if positive and < 60 s.

**Measured impact on `AS-205-tm-x-62831` (chain of today's fixes):**

| Quantity | Pre-fixes | After IGM gate 44→25 | After ε_2 15→25 | After HSL polynomial | **After Fig 9-3 tailoff** | TM |
|----------|----------:|---------------------:|----------------:|---------------------:|--------------------------:|---:|
| IGM engage (fl) | 188 s | 169 s | 169 s | 169 s | 169 s | 168.11 s |
| Mode→4 (fl) | 612 s | 610 s | 608 s | 608 s | 608 s | — |
| Cutoff (fl) | 617 s | 613 s | 613 s | ~614.5 s | **~614.5 s** | 614.63 s |
| h at cutoff | 228,113 m | 228,103 m | 228,103 m | 228,104 m | 228,104 m | 227,941 m |
| V at cutoff | 7,768.5 m/s | 7,753.2 m/s | 7,753.2 m/s | 7,773.0 m/s | **7,776.72 m/s** | 7,780.67 m/s |
| Δh at cutoff | +172 m | +162 m | +162 m | +163 m | +163 m | — |
| ΔV at cutoff | −12.2 m/s | −19.0 m/s | −19.0 m/s | −7.6 m/s | **−3.95 m/s** | — |

The HSL polynomial solves for the time at which V reaches `V_T − ΔV_b`,
intentionally cutting the engine early so that real J-2 thrust tailoff
(plus command-latency / shutdown transients) adds the remaining ΔV_b m/s
to bring V up to V_T. `PARAM_DeltaV_b_mps = 7.2381` is cited to LVDC-EDD
Part II, Table 1-3 "PRESETTINGS" ("Thrust decay velocity bias for S-IVB
burn") and is not to be re-tuned.

On 2026-11-07 the J-2 `Thrust_Decay` lookup in `model/saturn_ib_stack.json`
was replaced (4-point stub → 16-point table read from AS-205 post-flight
Fig 9-3 in `apollo_07_saturn_ib.pdf` §9.2.4). The new tail delivers
~3.7 m/s of impulse, closing the ΔV gap from −7.6 to −3.95 m/s. The
remaining ~4 m/s discrepancy source remains to be identified.

**Regression:** `tests/batch/chi_freeze.sh` still passes — Chi_Z step at
T_3_eff = 15 crossing is 0.15° (gate: 1.5°).

**Residual integer code tidying:** `HSL_Cutoff_Timing_Compare_To_Constant`
still retains the redundant `(tgo < 0.02)` clause for parity with EDD text
("immediate cut if predicted t-to-go < 20 ms").  At the 40 ms gate
resolution it collapses into `(tgo <= 0.04)`.  Kept explicit for
documentation.
