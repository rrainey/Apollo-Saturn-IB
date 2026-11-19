# Frame-anchor audit for Issue #4

Started 2026-11-17 to track down why Fix B (moving the plant's
MES_Transform plumbline anchor from sim-epoch to GRR) regresses the
AS-205 tmx cutoff residual to roughly −75 m/s, even though PASCs 13
per the Apollo Coordinate System Standards (MSC MA-1461) is
explicitly a GRR-anchored frame.

## Finding 1 — MES and MS4 do not share an anchor input

**Plant `MES_Transform`** (ECI → S-frame; computed every 0.8 s so
effectively a static matrix at sim-epoch):

```
Mux = ( A_z_deg,   phi_L_deg,   lambda_L_deg + theta_GMST_0_deg )
       Constant2,  Constant8,   Constant9 + Initial_Conditions_T_to_GMST…
```

The third input is **θ_pad_ECI(sim-epoch) = λ_pad_geographic + GMST at
`LaunchDate`**.  So the matrix orientation depends on the scenario's
sim-epoch.

**LVDC `MS4_Transform_Matrix`** (S-frame → 4-frame; also computed at
1.6 s BML rate, no time dependence):

```
Mux = ( A_z_deg,       phi_L_deg,     lambda_0_deg,   i_deg )
       Constant10,     Constant11,    Constant13,     Constant12
```

No `theta_GMST` anywhere.  The matrix is purely geometric — it
depends only on scenario presets.

**Consequence.**  For the LVDC's view of the vehicle to match its
MS4 target assumption, the S-frame delivered by `MES_Transform` must
be in the same orientation that `MS4` was authored to expect.  Since
`MS4` does not take any GMST input, its implicit anchor is set by
whatever epoch convention `lambda_0_deg = 119.0` was published for in
TM-X-62831 Table 3B.

## Finding 2 — EDD §3.4 pins MS4's implicit anchor to nominal GRR

EDD §3.4.3 (`reference/LVDC-EDD/.../satinstunitibm_1.md:1851-1859`):

> The GRR time delta (T_D) from the nominal time of GRR (T_GRR0) must
> be recomputed using T_GMT in Eq. 3.4.1.  The variable longitude of
> the descending node (λ) must be computed as a function of T_D using
> Eq. 3.4.2.
> …
> The transformation matrices between the plumbline and target plane
> [MS4], and the plumbline and gravitational [MSG] coordinate systems
> must be computed as functions of the platform azimuth (A_z), the
> orbital inclination (i), the descending node (λ), and the geodetic
> latitude of the launch site (φ_L).

And the Part II Presettings glossary
(`reference/LVDC-EDD/.../satinstunitibm_7.md:1909-1920`) names:

- `λ_0` = "Longitude of descending node" (preset)
- `λ̇`  = "Time rate of change of the descending node" (preset)

So the EDD implementation computes `λ = λ_0 + λ̇ · T_D` at each GRR.
Our model loads `λ_0` as a static constant with `λ̇ = 0` implicitly
(no `lambda_dot` input is wired into the MS4 matrix), which is fine
for a nominal launch — but it pins MS4's frame anchor to nominal
GRR, because `λ_0` is "longitude of descending node **measured from
the launch meridian** at the nominal T_GRR".

## Finding 3 — the X_0 / V_0 latch has an extra inconsistency

My LVDC code (`lib_SaturnIBObliq/native-snapshots/lvda-lvdc-legacy/LVDA_LVDC.c:1343-1360`)
latches X_0 and V_0 when `bLiftoff` goes true:

```c
s_X_S[i] += model->inputs.MES_DCM[i][k] * model->inputs.PAD_Xe_m[k];
s_V_0[i] += model->inputs.MES_DCM[i][k] * model->inputs.PAD_Ve_mps[k];
```

But EDD §3.4.3 says the initial values are computed from presets at
GRR via Eq. 3.4.3 using `R_L`, `φ'_L` (geocentric, not geodetic
φ_L), `A_z`, and `λ`.  In other words the EDD's `X_0` is

```
X_0 = R_L · ( cos φ'_L,  0,  0 )    (in the S-frame at GRR)
```

**not** `MES × PAD_Xe at bLiftoff`.

The difference between these two sources of X_0 scales with:
- the time gap between GRR and liftoff (ω_E · 17 s ≈ 0.07° rotation
  of the pad in ECI, so ~7 km displacement at R_L — mostly cancelling
  itself in S-frame but with sub-m level residual in the orthogonal
  axes),
- plus whatever R_L the EDD preset uses vs the physical pad ECI
  magnitude (the Table 3B `R_L_m = 6,373,385 m` vs our pad computed
  from `CG_LLA_deg_m`).

Our current `PAD_Xe_m` for AS-205 is derived from
`CG_LLA_deg_m = (28.521963°, -80.561141°, 34.7 m)` through the plant's
`On_Pad` subsystem using WGS-84.  TM-X-62831's R_L column uses Fischer;
and our LVDC uses Fischer presettings (`R_L_m = 6,373,385`).  So the
magnitude of X_0 can differ between what the LVDC LATCH samples and
what EDD Eq. 3.4.3 would prescribe.

## Why Fix B regresses

Fix B rotated `MES_DCM` by +(T_L' − 17) · ω_E degrees around the pole,
which would re-align the S-frame delivered to the LVDC with the
PASCs-13 S-frame at GRR.  But the X_0 / V_0 latch in LVDA_LVDC.c
samples `PAD_Xe` (= pad ECI at *liftoff*, not at GRR) through the
new DCM.  The product no longer approximates `(R_L, 0, 0)` — it ends
up at `(R_L · cos(ω_E · 17), R_L · sin(ω_E · 17) · sA, …)`, which
the IGM then has to "steer away from".  Under the previous
sim-epoch anchor, the DCM-to-PAD rotation-mismatch happened to
produce an X_0 that the (equally mis-anchored) MS4 matrix compensated
against — the two mis-rotations partially cancelled in the IGM
error budget.

## Next steps (all additive, no cited preset touched)

1. **Switch the X_0 / V_0 latch to match EDD Eq. 3.4.3 exactly.**
   Use the presetting `R_L_m` and the geocentric `phi_L_prime_deg`
   (both already in `as205TmX62831Presettings.ts`) to compose X_0 and
   V_0 in the S-frame directly, without going through `PAD_Xe` or
   `MES_DCM`.  That removes the frame-anchor coupling at the latch
   entirely — X_0 is `(R_L, 0, 0)` by construction, and V_0 is
   `(0, ω_E · R_L · cos(φ'_L) · sin(A_z), ω_E · R_L · cos(φ'_L) · cos(A_z))`
   (modulo sign conventions to verify from EDD Eq. 3.4.4).

2. **Then retry Fix B** (MES_Transform anchor shifted to nominal GRR).
   With the latch independent of `MES_DCM`, the anchor shift should no
   longer regress — it should either hold or improve the residual.

3. **If (2) regresses, the remaining suspect is the LVDC's use of
   `MES_DCM` elsewhere during flight.**  The LVDC continues to use
   `MES_DCM × plant_state` on each BML to re-express accelerometer
   velocities into S-frame.  That chain inherits the plant's
   current MES anchor; changing the anchor changes those accumulated
   V_m values.  If step 2 shows this is the culprit, the remedy is to
   build an equivalent S-frame rotation **inside** the LVDC from its
   own presets (R_L, φ_L, A_z) — making the LVDC frame-independent of
   the plant.

## What NOT to do (per project standing rules)

- Do not adjust `lambda_0_deg`, `A_z_deg`, `phi_L_deg`, `i_deg`,
  `R_L_m`, or any other cited Table 3B / EDD Part II preset to
  compensate.  These values are Table 3B / EDD Part II as published
  and stay put.
- Do not re-anchor MES_Transform in isolation; it must land with a
  consistent latch (step 1) first.

## 2026-11-17 experiment — latch rewrite + Fix B (reverted)

Implemented the proposed latch rewrite: `s_X_S` and `s_V_0` now
source directly from `LVDC_S_Frame_Position_Velocity_Calculations_
Initial_Position_and_Velocity_Eqns_3_4_3_4_Mux1/Mux2`, which the
plant already evaluates from the (R_L, φ_L, φ'_L, A_z) presets per
EDD Eq. 3.4.3 / 3.4.4.  A debug probe confirmed the two forms:

- `X_0` (Eq 3.4.3):   (6,373,359.8,     17,050.3,     −5,539.9) m
- `X_0` (MES × PAD):  (6,372,123.1,     54,351.7,    111,337.3) m

The angle between the two vectors is **1.247°**, exactly ω_E · T_L'
(300 s · 7.292115e-5 rad/s / cos-projection).  That confirms the
old MES × PAD latch really was 1.25° mis-rotated in S-frame vs the
EDD-prescribed value — and that the sim-epoch MES anchor was exactly
compensating in the integrated IGM error budget.

### Measured residuals

- Baseline (both committed): V_space-fixed = 7782.9, residual +2.3 m/s
- Latch rewrite only:        V_space-fixed = 7677.1, residual −103.6 m/s
- Latch rewrite + Fix B:     V_space-fixed = 7677.1, residual −103.6 m/s
  (identical — Fix B had zero effect once the latch no longer reads
  MES_DCM; the only remaining consumers of the top-level MES_DCM are
  a diagnostic trace in `SaturnIBPlantObliq.c` and the LVDC's `MES_DCM`
  input port which is now unused.)

### Why Fix B is a no-op after the latch rewrite

`grep -rn MES_DCM src/ lib_SaturnIBObliq/native-snapshots/` shows:
- `src/sim/SaturnIBPlantObliq.c:135-136` — diagnostic
  `MES_PAD_Xe` / `MES_PAD_Ve` only
- `lib_SaturnIBObliq/native-snapshots/lvda-lvdc-legacy/LVDA_LVDC.h:79` — the
  input port (unused after the rewrite)

The plant's state integrator does not consume `MES_DCM`.  What does
consume it on the plant side is **a different MES_Transform instance
inside the `Initial_Conditions` subsystem** (line ~8486 of the
generated plant), which builds the vehicle's attitude IC `q_ECI_0` by
converting the matrix to a quaternion.  Fix B as written only
touched the top-level `MES_Transform`, not the `Initial_Conditions`
copy.  So the plant's initial attitude is still sim-epoch anchored.

### The deeper problem

The −103 m/s regression comes from a frame mismatch between the LVDC's
latched initial state (EDD-prescribed, anchor-independent) and the
ST-124M PIPA integrator in the plant (which measures specific force
projected through the plant's current attitude, with attitude IC in
sim-epoch frame).  The LVDC's X_S integration is

```
X_S(t) = X_0 + V_0·t + ∫∫V_m dt + ∫∫G_S dt
```

For the arithmetic to be consistent, X_0 must be in the same frame
V_m is accumulated in.  V_m is in the ST-124M platform frame.  The
ST-124M platform frame in our sim is "whatever frame the plant's
`q_ECI_0` attitude IC represents" — which, through the inner
`Initial_Conditions > MES_Transform`, is **S-frame-at-sim-epoch**.

So putting EDD-correct X_0 (canonical S-frame) alongside a
sim-epoch V_m is mixing frames.  The old latch was "wrong" per EDD
but internally consistent with the sim-epoch V_m, which is why the
cutoff residual was +2.3 m/s despite the 1.25° rotation we now
know exists.

### What a real Fix B+ would require (deferred)

Making the plumbline frame anchored at GRR consistently requires
simultaneously:

1. Shifting the **top-level** `MES_Transform` GMST input by
   (T_L' − 17) · ω_E  (what the original Fix B did).
2. Shifting the `Initial_Conditions > MES_Transform` GMST input the
   same way so that `q_ECI_0` (plant attitude IC) is also GRR
   anchored.
3. Shifting the `On_Pad > Initial_Conditions > MES_Transform` GMST
   input likewise so the pad-state outputs remain consistent.
4. Keeping the LVDC V_0 / X_0 latch sourced from EDD Eq. 3.4.3 /
   3.4.4 (the rewrite in this session).
5. Verifying the ST-124M PIPA integrator sees specific force
   projected through the GRR-anchored attitude.

That is a model-wide frame reanchor, not a single-block change.  It
also crosses the regen boundary (three model subsystems need
coordinated edits), and the three `Initial_Conditions_T_to_GMST_...`
signals that are consumed in other places (Earth rotation sum, etc.)
need to be checked for unwanted knock-on effects.

Deferred as a separate work item; this audit report captures why.
Current committed state (sim-epoch anchor everywhere + MES×PAD
latch + Table-3B presettings) remains the least-wrong configuration
until that larger rework is designed.

