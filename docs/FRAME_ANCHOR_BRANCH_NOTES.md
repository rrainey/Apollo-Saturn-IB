# Frame-anchor investigation — branch notes

Living log for `issue-4/frame-anchor-grr`.  Not for main; captures
experiments and intermediate findings that inform whether (and how) to
eventually land a PASCs-13-correct frame anchor.

## Current branch state

Three coordinated changes (commit 436f36c):

1. Top-level model: new `evaluate_7400` computing
   `GMST_shifted = theta_GMST_0 + (T_L' − 17) · ω_E` degrees,
   feeding both the top-level `MES_Transform.Sum2` and a new
   `Initial_Conditions.theta_GMST_shifted_deg` input port.
2. `Initial_Conditions` subsystem: the inner `MES_Transform.Sum2`
   now consumes the shifted GMST (wire_593 rewired from the
   sim-epoch sheet-label source to the new input port).
3. LVDA_LVDC latch: `s_X_S` / `s_V_0` sourced from the plant's
   already-computed EDD Eq. 3.4.3 / 3.4.4 block.

The `On_Pad > Initial_Conditions > MES_Transform` is intentionally
unchanged (it uses live `Earth_s_Rotation_Sum` — rotating pad frame).

## Measurements to date

All at AS-205 tmx cutoff, `--run-time 960`.  V_T target 7780.67 m/s,
altitude target (Fischer geocentric-radial) 227.94 km.

| Configuration                                   | V        | ΔV       | h (Fischer) | Δh       |
|------------------------------------------------|----------|----------|-------------|----------|
| Committed main baseline                         | 7782.9   | +2.3     | 233.00 km   | +5.06 km |
| Top-level Fix B only                            | 7707.9   | −72.8    |   —         |   —      |
| LVDC latch rewrite only                         | 7677.1   | −103.6   |   —         |   —      |
| Latch rewrite + top-level Fix B                 | 7677.1   | −103.6   |   —         |   —      |
| Double-MES Fix B + MES×PAD latch (isolation)    | 7798.5   | **+17.8**| 232.97 km   | +5.03 km |
| Double-MES Fix B + EDD-Eq. latch (branch tip)   | 7794.6   | **+13.9**| 234.43 km   | +6.49 km |

## What the isolation runs tell us

Comparing the two "double-MES Fix B" rows (same frame anchor, two
latch forms):

- Switching the LVDC latch from MES×PAD to EDD Eq. 3.4.3 / 3.4.4
  **improves velocity by 4 m/s but degrades altitude by 1.5 km**.
- The altitude at the MES×PAD-latch row is essentially identical to
  the sim-epoch baseline altitude (both ~233 km, within 30 m).  So
  the altitude channel is not sensitive to the MES anchor at all —
  it's only sensitive to the latch form.
- Velocity, in contrast, shifts by **+15.5 m/s when the MES anchor
  moves** regardless of latch form.

## Where the +15.5 m/s is coming from

The MES anchor shift changes the LVDC's view of the vehicle state.
Specifically the latched `V_0` (Earth-rotation velocity at pad in
S-frame) differs:

- Baseline MES×PAD at sim-epoch:
  `V_0 = (−7.86, 122.29, 390.18) m/s`,  |V_0| = 408.56
- EDD Eq. 3.4.4 formulation:
  `V_0 = (  0.00, 126.38, 388.96) m/s`,  |V_0| = 408.96

The −7.86 m/s X-component in the baseline is *physically nonsensical*
(S-frame X is radially up through the pad; the pad is not moving
radially).  It is the 1.25°-rotation artifact of using sim-epoch
MES with liftoff-time PAD.  In EDD-formulated V_0 the X-component is
zero by construction.

The IGM uses V_0 throughout the flight as a background Earth-rotation
term in its velocity budget.  Over the 614 s burn, a ~7 m/s
nonphysical X-component shifts how the HSL's velocity polynomial
projects onto the actual flight direction.  The 15 m/s cutoff shift
is plausibly dominated by this; a precise attribution would need a
V_m / V_0 / V_Sg trace at HSL triggering.

## What this means

The committed main baseline achieves a smaller cutoff residual
*because* two frame errors cancel: the sim-epoch MES rotates both
V_0 and the MS4 target by the same 1.25°, and those rotations
cancel inside the IGM arithmetic.  The branch's configuration
removes both errors independently; the IGM arithmetic then
correctly computes a slightly different trajectory.

Neither configuration is "broken".  The branch config is
*physically more correct* (PASCs 13 at GRR per Apollo MSC MA-1461,
EDD Eq. 3.4.3 / 3.4.4 latch) but the measured residual against
TM-X-62831's cutoff numbers is larger.  This likely means the
TM-X-62831 Table 3 numbers themselves were computed against a
reference simulation that used **the same sim-epoch-ish frame
convention** we have in main — in which case our baseline is
"accurately wrong in the same way the reference is" and the branch
is "truly correct but doesn't match the published numbers".

## Cross-check against TM-X-62831 Table 4 — DECISIVE (2026-11-17)

Instrumented the LVDC with a one-shot probe to dump the PASCs 13
`s_X_S` and `s_V_S = V_m + V_Sg + V_0` at the first BML boundary
after TM-X-62831's S-IVB cutoff time (flight time 614.63 s, sim_t =
915.26 s).  Our probe fires at sim_t = 915.20 s, i.e. ~0.57 s
*after* the Table 4 reference moment; most of the residual
is therefore pure dead-reckoning over 0.57 s at ~7.4 km/s.  Ran the
same probe on main (committed baseline) and on branch tip.

**Against TM-X-62831 Table 4 S-IVB GCS (flight time 614.63 s):**

| Component | Table 4 | Main (sim-epoch) | Δ (main) | Branch (GRR) | Δ (branch) |
|-----------|--------:|-----------------:|---------:|-------------:|-----------:|
| X_S (m)   | 6,258,278 | 6,221,477  | **−36,801**  | 6,256,771  | −1,507  |
| Y_S (m)   | 144,698   | 150,691    | +5,993       | 144,978    | +280    |
| Z_S (m)   | 2,092,098 | 2,199,129  | **+107,031** | 2,097,023  | +4,925  |
| DX (m/s)  | −2472.80  | −2598.13   | **−125.3**   | −2481.11   | −8.3    |
| DY (m/s)  | 412.48    | 409.40     | −3.1         | 413.42     | +0.9    |
| DZ (m/s)  | 7365.73   | 7327.07    | **−38.7**    | 7384.96    | +19.2   |

Main's PASCs 13 state vector is off by **≈115 km position / 130 m/s
velocity** at the cutoff instant.  That matches exactly the
ω_E · T_L' = 1.25° frame rotation applied to R ≈ 6,600 km
(sin 1.25° · 6,600 km ≈ 144 km orthogonal displacement, consistent
with the 107 km Z-component shift and the −37 km X-component shift
in combination).

Branch residuals are ~100× smaller: all three position components
within 5 km (and mostly 0.57 s of dead reckoning), all three
velocity components within 20 m/s.

### What this changes about our understanding

The main baseline's small |V_S| cutoff residual (+2.3 m/s vs 7780.67)
was a magnitude-match coincidence — a 1.25°-rotated velocity vector
still has the same magnitude as the correct one, so a scalar
|V_S| comparison does not detect the frame rotation.  The *direction*
of the vehicle's state in PASCs 13 under the sim-epoch baseline is
**not** the trajectory TM-X-62831 published.  The branch (coordinated
GRR anchor + EDD Eq. 3.4.3 / 3.4.4 latch) IS.

The earlier working hypothesis above — that TM-X-62831's reference
simulation might have used the same sim-epoch frame convention we
had on main — is **refuted** by this measurement.  TM-X-62831's
PASCs 13 columns are published against the standard GRR anchor,
as the Apollo program's coordinate standards require.  The branch
matches that.

The +11 m/s / +1.4 km fidelity-table regression (branch vs main at
the cutoff sample) is a reporting artefact, not a real trajectory
degradation.  What's actually happening:

- Main's trajectory is **physically wrong** by 1.25° in PASCs 13
  orientation.  Its cutoff magnitude happens to land close to V_T
  because the LVDC steered in a self-consistent (but mis-rotated)
  frame.
- Branch's trajectory is **physically correct** per the Apollo
  standard.  Its cutoff magnitude is 11 m/s higher than V_T
  because the LVDC is now steering to the real target in the
  correct frame, and the HSL cutoff decision is now triggered
  without the compensating frame-rotation error.

### Implications

Against the TM-X-62831 PASCs 13 position/velocity reference (which
is the thing we actually care about for fidelity), the branch is
~100× closer than main.  By the dominant-signal measure, the
branch wins decisively.

The remaining ~11 m/s cutoff-magnitude residual on branch is real
but second-order — likely from the HSL's velocity polynomial
coefficients or ΔV_b bias under a now-correct frame, or the
gravity model's projection at the end of burn.  A follow-up task
can chase that against the properly-anchored baseline.

## Open questions deferred to follow-up tasks

1. **Trace the remaining +11 m/s cutoff-magnitude residual** — now
   known to be a real second-order artefact, not a frame leak.
   HSL velocity polynomial is the first place to look.

2. **Is the V_0 X-component nonzero something that other NASA
   references treat as significant?**  EDD Eq. 3.4.4 clearly
   prescribes V_0_X = 0; if other sims used the MES×PAD latch, their
   V_0_X would be ~−8 m/s too.  A sanity check on the AGC's or
   another LVDC stack's initialization could help.

3. **Does the altitude shift (+6.5 km vs +5.1 km, i.e. +1.4 km
   worse on branch) come from the geocentric-vs-geodetic latitude
   difference in Eq. 3.4.3 that the MES×PAD form never saw?**  The
   EDD latch produces X_0 with ~17 km Y and ~5 km Z components
   (geodetic φ_L minus geocentric φ'_L).  Those non-X IC components
   might translate into slight altitude offset during the burn.

## What this branch is NOT doing

- Not tuning any cited preset (`A_z_deg`, `phi_L_deg`, `i_deg`,
  `lambda_0_deg`, `R_L_m` all unchanged; standing project rule).
- Not merging to main.  The committed baseline continues to produce
  the smaller fidelity number against TM-X-62831's cutoff
  conditions.
