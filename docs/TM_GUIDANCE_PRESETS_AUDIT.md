# TM guidance presets audit (remaining Chi / ΔV errors)

**Date:** 2026-08-26  
**Context:** After H-1/J-2 Table 11, LES/ullage Table 12, and `lambda_0_deg`→119, GCS \|V\|≈7802 vs TM 7781. Remaining: Chi_Z bias ~+8–12° vs TM CHIY≈−4°, late-looking `nIGMMode`.

**IU reference:** LVDC EDD `ApolloA/reference/satinstunitibm_*.pdf` (esp. vol.1 §2.3 matrices, §4.4 IGM/SMC/Chi).  
**TM presets:** Table 3B (+ AP note).

---

## 1. Preset inventory vs Table 3B

### Updated to TM (and used by stack)

| Symbol | TM | Pack / model |
|--------|-----|----------------|
| A_z | 72° | ✓ |
| i | 31.605° | ✓ |
| **θ_N / λ (`lambda_0_deg`)** | **119.0°** | ✓ (was TN 102.329 — fixed) |
| φ_L | 28.521963° | ✓ |
| V_T, X_VT, X_VG | 7780.67, 6600405, −9.10 | ✓ |
| T1i / T3i / T3 | 286.3 / 159.8 / 326.5 | ✓ |
| Vex1 / Vex3 | 4151 / 4233.2 | ✓ |
| τ10, PC0, ε′ | 536, 0, 5 | ✓ |
| M1, m_sep, Ṁ1, Ṁ3 | Table 3B / 12 | ✓ |

### Same as TN **but confirmed identical in TM Table 3B** (not stale)

M02=129761.6, M03=134374, F1/F2/F3, TR1=1.8, TR2=TR3=0, C0=35.

### Still `tn-hold` / not cited from Table 3B

| Name | Value | Likely relevance to remaining errors |
|------|-------|--------------------------------------|
| `phi_L_prime_deg` | 28.360795 | MSG in some docs uses φ_L′; **Table 3B AP note + our MS4 use φ_L**. Low unless MSG path elsewhere differs |
| `lambda_L_deg`, `R_L_m` | LC-34 | Pad / MES; weak for mid-IGM Chi_Z |
| `epsilon_2_sec` | 15 | Terminal freeze band; **hardcoded 15** in art-τ path too |
| `BN_1_sec` | 14.4 | HSL / velocity history |
| `DeltaV_b_mps` | 7.2381 | Thrust-decay cutoff bias |
| `delta_T_sec` | 1.6 | IGM cycle; TM OCR unclear |
| `A0_sec`, `A1` | 458.9, −1.194 | Mask leftovers — **check if still wired** |
| `T3_FM_sec`, `V_GRD`, `T_HSL` | TN | Cutoff / guard |

---

## 2. MS4 / AP geometry (yaw frame) — closed

IU EDD §2.3.1–2.3.5: `[MS4]=[MG4][MSG]` from \((A_z,\phi_L,\lambda,i)\).

Stack builds this on-the-fly (`MS4_Transform_Matrix` inputs `A_z`, `phi_L`, `lambda`, `i`).  

**Check:** with θ_N=119°, recomputed MS4 matches TM Table 3B AP1–9 to **~1e−8**.  
With old λ=102.329°, error ~**0.24** (same as stale TN `IGM_AP_DCM` in `igmChiAssembly.ts`).

So remaining Chi_Z bias is **not** another missing AP angle — geometry is TM-aligned.

**Follow-up:** companion-plant `IGM_AP_DCM` in `igmChiAssembly.ts` is still TN ConstB; harmless for full-stack cgen, stale for plant-only path.

---

## 3. What still drives the Chi_Z error

### Chi writers in the stack (two paths)

1. **Time Tilt** → DSM `Chi_Y` / `Chi_Z` (S-IB / early hold)  
2. **IGM Chi Steering** (enable ← `epsilon_prime`) → DSM `Chi_Y` / `Chi_Z` from Product15 + ±45° sat  

SMCZ is present (`SMCZ_rad` added into Chi_Z path) and **enable-gated** — not the plant-stub “bypassed” story. Worth comparing SMCZ magnitude/sign to IU EDD §4.4.4.

### `nIGMMode` ≠ “IGM off until 473 s”

Chi already tracks TM pitch from ~190 s while `nIGMMode=0`. Mode 0→1 @ ~473 s is near high-MR / T1i burn-down (T1i=286.3 from IGM start ~168 → ~454 s), i.e. **IGM phase index**, not initiate. Framing “IGM 305 s late” overstates the initiate problem; Chi steering is active earlier.

### Ranked suspects for ~+8–12° yaw bias (sim Chi_Z − TM CHIY)

| Rank | Suspect | Why |
|------|---------|-----|
| 1 | **Chi_Z / vigained / ΔV path** (IU §4.4.2–4.4.5) | AP correct; pitch OK; yaw offset ≈ constant — classic wrong Y component or sign in χ_z=atan(v_y/…) |
| 2 | **SMCZ** enable / sign / `pirads` wiring | Direct additive to Chi_Z; historically caused Chi_Z bias when Gain=`pi` broke |
| 3 | **Which Chi writer wins** when Time Tilt and IGM Steering both touch DSM | Could mix hold and IGM yaw |
| 4 | Remaining tn-hold (`epsilon_2`, `DeltaV_b`, `A0/A1` if live) | Secondary; freeze/cutoff more than mid-burn yaw |
| 5 | Hardcoded TN `IGM_AP_DCM` | Only if running companion plant, not stack cgen |

---

## 4. Recommended next probes (IU-first)

1. **Log mid-burn** (t=300, 450, 600): Product15 \(v\), raw χ_z before sat, `SMCZ_rad`, which enable writes Chi DSM.  
2. **IU EDD §4.4.5 Chi Computations** vs MDL `atan(v_y/√(1−v_y²))` + SMCZ — confirm sign of \(v_y\) channel.  
3. **IU §4.4.4 SMC** — when enabled in flight program vs our `SMCZ_enable_bool`.  
4. Confirm **`A0`/`A1`/`DeltaV_b`** still connected or dead.  
5. Optionally replace plant `IGM_AP_DCM` with TM AP1–9 for dual-path consistency.

---

## 5. Bottom line (presets)

- Preset gap that explained the **big** yaw/ΔV hole was **θ_N / `lambda_0_deg`**.  
- Other Table 3B IGM numbers that matter for mass/τ are already TM.  
- Several “inherited TN” values are **intentionally equal** to TM Table 3B.  
- Remaining Chi_Z bias → **IU §4.4 Chi/ΔV path** (below).

---

## 6. Suspect 1 — IU EDD §4.4.5 vs stack (2026-08-26)

### EDD formulas (satinstunitibm_5)

\[
\overline{F_S'} = [\mathrm{MS4}]^T
\begin{bmatrix}\cos\chi_{y4}\cos\chi_{z4}\\ \sin\chi_{z4}\\ -\sin\chi_{y4}\cos\chi_{z4}\end{bmatrix}
\quad (4.4.85)
\]

\[
\chi_y = \mathrm{Arctan}(-F'_{Sz}/F'_{Sx}) + \mathrm{SMCY}
\quad (4.4.86)
\]

\[
\chi_z = \mathrm{Arctan}\!\left[\frac{F'_{Sy}}{(1-F'_{Sy}^2)^{1/2}}\right] + \mathrm{SMCZ}
\quad (4.4.87)
\]

### Stack terminal path — **matches EDD**

| Step | Stack | EDD |
|------|-------|-----|
| Unit thrust Mux | `[c_y c_z,\ s_z,\ -s_y c_z]` from Add12/Add14 | (4.4.85) bracket ✓ |
| Product15 | `MS4^T · Mux` | (4.4.85) ✓ |
| Chi_Y | `atan2(-F'_Sz,\ F'_Sx) + SMCY` → deg | (4.4.86) ✓ |
| Chi_Z | `atan(F'_Sy/\sqrt{1-F'_Sy^2}) + SMCZ`, sat ±45° | (4.4.87) ✓ |
| χ_y4 offset | subtract `const_ = π/2` | EDD −½ pirad = −π/2 rad ✓ |

**Conclusion:** The **Product15 → Chi_Y/Z algebra is not inverted**. Pitch tracking TM after θ_N fix is consistent with that.

### Where the yaw bias must live

Upstream of (4.4.85): **velocity-constrained \(\tilde\chi_z\)** and thus **ΔV_Y**.

Stack:

- \(\tilde\chi_y = \mathrm{atan}(\Delta V_x / \Delta V_z)\) (`Divide3` / `TrigFunction4`)
- \(\tilde\chi_z = \mathrm{atan}(\Delta V_y / \sqrt{\Delta V_x^2+\Delta V_z^2})\) (`Divide2` / `TrigFunction3`)
- \(\Delta V =\) `Add10` = improved velocity-to-be-gained (`Demux1`)

For small angles, \(\mathrm{sign}(\chi_z)\approx\mathrm{sign}(\tilde\chi_z)\approx\mathrm{sign}(\Delta V_y)\).  
TM CHIY ≈ **−4°**, sim Chi_Z ≈ **+4…+8°** → **ΔV_Y likely wrong sign (or Y-channel of V/G transform)**.

Pitch uses ΔV_x/ΔV_z and tracks TM → X/Z of ΔV are comparatively healthy.

### SMCZ (not the primary mid-burn story, but active)

SMCZ enable: `T3_Timer ≥ 58.5` and not HSL — so **on** by mid S-IVB. Still secondary: bias already looks like a near-constant opposite sign vs TM, pointing at \(\tilde\chi_z\)/ΔV_Y first.

### Next probes (ordered)

1. At t≈300 and 450, log: `Demux1` (ΔV_x,y,z), `TrigFunction3` (\(\tilde\chi_z\)), `Add14` (χ_z4), `Product15[1]` (F'_Sy), raw atan, `SMCZ_rad`, final Chi_Z.  
2. Trace **Add10 / Estimated_Velocity_to_be_gained** Y vs IU (4.4.42–4.4.47): V-frame transform, G\* term, terminal \(Y_{VT}=0\).  
3. Revisit known plant gap: G_S vs A_m into ΔV (`IGM_CHI_DIVERGENCE.md`) — confirm full-stack wiring uses **G**, not specific force.  
4. Only then chase SMCZ sign (suspect 2).

---

## 7. Probe results (`igm_dvy_probe`) — Suspect 1 revised (2026-08-26)

Source: `/tmp/verify-lambda0-119-cgen/igm_dvy_probe.c` → `/tmp/igm_dvy_probe2.csv`  
(λ₀=119, Table 11/12 stack, ExtIn A_z=72).

### Headline

| t_flight | dVy | χ̃_z | χ_z4 | χ_y4 | F'Sy | atan(F'Sy) | SMCZ | Chi_Z | TM CHIY |
|---------:|----:|-----:|-----:|-----:|-----:|-----------:|-----:|------:|--------:|
| 210 | +14.3 | +0.15° | +0.23° | +61.1° | +0.053 | **+3.02°** | −0.10° | **+2.89°** | −3.45° |
| 300 | +11.5 | +0.14° | +0.28° | +49.5° | +0.059 | **+3.36°** | +1.05° | **+4.39°** | −4.00° |
| 450 | **−9.2** | **−0.18°** | −1.45° | +29.9° | +0.032 | **+1.84°** | +2.85° | **+4.68°** | −4.24° |
| 550 | +6.3 | +0.25° | −3.14° | +14.3° | +0.001 | **+0.04°** | +3.58° | **+3.61°** | −4.14° |

**Chi_Z ≈ atan(F'Sy) + SMCZ** checks to ~0.01°.

### What Suspect 1 got wrong

Audit §6 predicted `sign(Chi_Z)≈sign(ΔV_Y)`. **False for this trajectory.**

- Mid-burn **ΔV_Y and χ̃_z already have the TM-desired sign** (negative at t=450).
- χ̃_z magnitude is **≪ 0.3°** — cannot explain a ±4° Chi_Z.
- At χ_z4≈0, F'Sy is still **~+0.05…0.06** from MS4 off-diagonals:

```
F'Sy = MS4[0][1]·Mux0 + MS4[1][1]·Mux1 + MS4[2][1]·Mux2
     ≈ 0.0501·Mux0 + 0.998·Mux1 − 0.0280·Mux2
```

At t=210 (χ_z4≈0): `0.024 + 0.004 + 0.024 = +0.053` → atan **+3.0°** with almost no yaw demand in the 4-frame.

### What actually drives +Chi_Z

1. **Geometric:** MS4ᵀ·Mux maps near-in-plane 4-frame thrust (χ_z4≈0, χ_y4~15–60°) to **positive F'Sy** → atan ≈ **+2…+3.5°**.
2. **SMCZ:** same sign, grows to **~+3.6°** by t=550 (then dominates Chi_Z).
3. **ΔV_Y / χ̃_z:** small trim only.

### Sign-flip thought experiment

`Chi_Z_flip = −atan(F'Sy) − SMCZ`:

| t | flip | TM CHIY | Δ |
|--:|-----:|--------:|--:|
| 210 | −2.92° | −3.45° | +0.5° |
| 300 | −4.41° | −4.00° | −0.4° |
| 450 | −4.69° | −4.24° | −0.4° |
| 550 | −3.62° | −4.14° | +0.5° |

Matches TM yaw to **≲0.5°** across the burn. Pitch path (F'Sx/F'Sz only) would be untouched by an F'Sy / SMCZ-Y sign change.

### Implications

- Terminal path **implements EDD (4.4.85–87) as written** (Mux, MS4ᵀ, atan, +SMCZ).
- Full-stack Chi_Z also tracks **RTW** (`RTW_VS_OBLIQ` §1d) — so this is likely **MDL+RTW vs TM** yaw sense, not an Obliq-only codegen bug.
- G_S→ΔV wiring is confirmed; not the yaw bias source.
- **Revised primary suspects:**
  1. **Y-axis / CHIY sense** vs LVDC Chi_Z (TM table or platform middle-gimbal convention).
  2. **SMCZ Ÿ_m (Demux3_1 / Gain3)** sign or pirad/rad mix in the SMCZ recursion (feeds same-sign growth).
  3. Only then: MS4 Y-column vs a different TM AP publication (stack MS4[0][1]=0.0501; TN plant const had 0.0209 — TM match was claimed at 1e−8 for Table 3B AP1–9).

### Next actions

1. Confirm TM-X-62831 CHIY definition vs IU χ_z (sign of positive yaw).
2. Log SMCZ internals: `Demux3_1`, `Add16`, `SMCG`, Chi_z_last units (rad vs pirad).
3. Optional A/B: negate F'Sy into atan and/or SMCZ addend; re-run tm904 Chi vs TM (expect ~0.5° yaw if hypothesis holds).

---

## 8. CHIY vs Chi_Z / PHIY sign convention (2026-08-26)

### TM-X-62831 definitions (nomenclature)

| Symbol | Meaning | Positive sense |
|--------|---------|----------------|
| **PHIY** | Vehicle Euler yaw vs space-fixed | **nose right** |
| **CHIY** | Programmed yaw **attitude command** | Same family as PHI* (commands) |
| **PHIP / CHIP** | Pitch actual / command | PHIP = negative nose downrange |

### Digitized CHIY is **not** an OCR sign flip

NASA-TM-X-62831 **Figure 13** (*S-IVB Stage Yaw Attitude Commands vs Time*, PDF ~p.77) shows the yaw **command** curve at about **−4°** for the whole S-IVB burn (slightly more negative near the high-MR → low-MR step). That matches `tm_x_62831_chi.csv` (CHIY ≈ −3.5°…−4.2°).

### Same document, actual yaw is **positive**

| Epoch | CHIP / CHIY (cmd) | PHIP / PHIY (actual) |
|-------|-------------------|----------------------|
| Sep. ~144 s | −59.32 / 0 | −59.31 / −0.07 |
| GCS ~615 s | ≈ −106 / **≈ −4** (Fig.13 / Table 7) | −105.87 / **+3.37** |

So on the nominal OT, **CHIY ≈ −4°** while **PHIY ≈ +3.4°** — opposite signs, ~7° apart. Pitch CHIP≈PHIP (same sign, good track).

### IU Chi_Z sense (EDD + frames)

- S-frame: \(X_S\) up, \(Z_S\) downrange, \(Y_S\) RH → **+Y_S = right** facing downrange.
- (4.4.85–87): \(F'_{Sy}>0\) ⇒ \(\chi_z>0\) ⇒ commanded thrust / nose toward **+Y_S** = **nose right**.
- Therefore **IU Chi_Z positive ≡ TM PHIY positive** (nose right), and Chi_Z should be compared to **CHIY** if CHIY is the platform/yaw command published in Fig.13.

### What that means for our +4° Chi_Z

| Comparison | Our Chi_Z ~+4° | Verdict |
|------------|----------------|---------|
| vs TM **CHIY** (Fig.13, −4°) | Opposite | **Disagrees with published commands** |
| vs TM **PHIY** at GCS (+3.4°) | Same sign | Matches body Euler actual, not command |
| vs RTW MDL Chi_Z | Same | Shared with MATLAB truth model |

**CHIY OCR is fine.** The remaining issue is real: stack/RTW Chi_Z is opposite TM Figure 13 / Table 7 yaw **commands**. Pitch convention is consistent (CHIP ↔ Chi_Y ↔ PHIP).

Near ±90° pitch, gimbal χ_z vs body Euler PHIY can diverge in sign — that may explain CHIY≠PHIY in the TM book, but it does **not** excuse Chi_Z≠CHIY if both are platform yaw commands.

### Revised next step

Prefer an **A/B negate of F'Sy (+ SMCZ addend)** against TM CHIY / Fig.13, not a CSV sign flip. Also keep SMCZ `Demux3_1` instrumentation — same-sign SMCZ growth still amplifies the geometric +F'Sy bias.

---

## 9. A/B negate results (closed-loop) — 2026-08-26

Flag `OBLIQ_AB_NEGATE_CHIZ` in `/tmp/verify-lambda0-119-cgen` (separate `*_negchiz` binaries).

| Variant | t≈210 Chi_Z | Later | Trajectory |
|---------|-------------|-------|------------|
| Baseline | +2.88° | +4…+5° | Stable; GCS Ve≈7802 |
| **A:** `Sum = -(atan+SMCZ)` | −4.24° (≈TM) | ±45° sat, Chi_Y NaN | **Diverges** |
| **B:** negate `F'Sy` + SMCZ `Demux3_1` | −4.73° (≈TM) | SMCZ→−130°, sat | **Diverges** |

Open-loop “−atan−SMCZ ≈ TM CHIY ±0.5°” does **not** survive closed loop: flipping only the Chi_Z / F'Sy tail makes SMCZ and vigained fight the plant into saturation.

**Takeaway:** a terminal-only sign patch is the wrong fix. Need a **consistent** Y-sense change (MS4 Y-axis / state ΔV_Y / Product15 / SMCZ together) or acceptance that MDL/RTW Chi_Z matches the MATLAB model and the TM CHIY comparison needs a platform↔Euler story near −90° pitch.

A/B flag removed from cgen after the trial; result recorded here only.

---

## 10. Pitch-path probe — χ_y4 / Add12 / φ_T (2026-08-26)

Source: `/tmp/verify-lambda0-119-cgen/chi_y4_probe.c` → `/tmp/chi_y4_probe.csv`.

### Algebra

\[
\chi_{y4}=\tilde\chi_y-\mathrm{Add13}-\phi_T-\pi/2
\quad\text{(EDD 4.4.81; stack Add12)}
\]

| Check | Result |
|-------|--------|
| `chi_y4_check − Add12` | **0** (bit-exact) |
| `φ_T = φ_iT + atan2(R_z,R_x)` | matches `igmChiAssembly.ts` |
| `const_ = π/2` | ✓ |

### Term magnitudes @ t≈450

| Term | deg | Role |
|------|----:|------|
| χ̃_y = atan(ΔV_x/ΔV_z) | +12.1 | velocity constraint |
| Add13 ≈ (1/π)(K1−K2ΔT) | **−11.4** | pitch rate/K (minus ⇒ *adds* to χ_y4) |
| atan2(R_z,R_x) | **−105.7** | dominates φ_T |
| φ_iT | +9.4 | small bias |
| **φ_T** | **−96.3** | almost constant all burn |
| π/2 | 90 | |
| **χ_y4** | **+29.9** | |

φ_T ≈ **−96°** all mid-burn (atan2R ≈ −106…−113° + small φ_iT). That is what forces χ_y4 **positive** via `−φ_T` in Add12.

### Chi_Y vs TM CHIP

| t | Chi_Y | CHIP | Δ |
|--:|------:|-----:|--:|
| 210 | −49.1 | −56.0 | **+6.9** |
| 300 | −63.3 | −64.2 | +0.9 |
| 450 | −84.5 | −84.1 | −0.4 |
| 600 | −108.7 | −105.7 | −3.1 |

Mid-burn pitch command tracks TM well; early IGM is lofted ~+4…+7°.

### Why this matters for the “yaw” bias

`F'Sy` from pitch alone (`MS4[0][1]·Mux0 + MS4[2][1]·Mux2`) is **≈ +0.05…0.057 for every χ_y4 we actually fly (+14°…+63°)**. Nulling that geometric bleed needs χ_y4 ≲ **−60°**, which would require φ_T near 0 (or atan2(R) flipped ~π) — not a small Add13 tweak.

So under a pitch-command hypothesis, the interesting next question is not “is Add12 coded wrong?” (it isn’t) but **is atan2(R_z, R_x) / 4-frame R the right φ for this MS4?** — because that single ~−106° term sets χ_y4’s sign and locks +F'Sy.

---

## 11. φ_T / R₄ verification (EDD 4.4.30–33) — 2026-08-26

Source: `/tmp/verify-lambda0-119-cgen/phi_T_probe.c` → `/tmp/phi_T_probe.csv`.

### EDD vs stack

| Step | EDD | Stack | Status |
|------|-----|-------|--------|
| R₄ | `[MS4] R_S` | `Product6 = MS4 · Product18` | ✓ before M4V |
| φ_i | `Arctan(Z_i/X_i)` (4.4.30) | `atan2(R4z, R4x)` | ✓ full-quadrant |
| φ_iT | (4.4.31–32) | `Product5` | present |
| φ_T | `φ_i + φ_iT` (4.4.33) | `Add7` | ✓ |
| M4V | Rot_Y(−φ_T) (2.3.4) | Mux2 cos/sin/−sin | ✓ bit-pattern |
| R_V | `[M4V] R_4` (4.4.43) | `Product8` | ✓ |

### Sanity @ t≈450

| Quantity | Value |
|----------|------:|
| R₄ | (−1.78, +0.001, −6.35) Mm |
| \|R₄y\|/\|R\| (OOP) | **0.016%** |
| φ_i | −105.69° |
| φ_iT | +9.36° |
| φ_T | −96.34° |
| R_V − expected(φ_iT) | **~0 m** (dRVx, dRVz ~ 1e−9) |

Both X₄ and Z₄ negative (QIII) ⇒ `atan2(Z,X)=−105.7°` vs naive `atan(Z/X)=+74.3°`. LVDC Table 13-2 is the quadrant fix; C `atan2` matches.

### Alternate atan2 → χ_y4 (what-if, same other terms)

| φ_i formula | χ_y4 |
|-------------|-----:|
| **atan2(Z,X) EDD/stack** | **+29.9°** |
| atan2(X,Z) | +88.5° |
| atan2(−Z,X) | +178° |
| atan2(Z,−X) | −1.5° |

Only non-EDD swaps move χ_y4 near 0/−60°. **No evidence the stack disagrees with EDD on φ.**

### Conclusion

R₄ → φ_i → φ_T → M4V → R_V is **structurally and numerically consistent with IU EDD**. The −96° φ_T (and thus +χ_y4 / +F'Sy bleed) is what these equations produce for this MS4/state — not an atan2 argument-order bug.
