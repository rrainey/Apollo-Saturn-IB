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

My LVDC code (`lib_SaturnIBObliq/native-snapshots/lvda-lvdc/LVDA_LVDC.c:1343-1360`)
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
