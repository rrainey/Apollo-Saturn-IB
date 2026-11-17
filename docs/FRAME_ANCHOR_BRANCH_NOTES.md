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

## Open questions for the next pass

1. **What anchor did the TM-X-62831 reference simulation actually
   use?**  If we could get one of their tables plotted in the TM's
   PASCs 13 position/velocity columns (already available) and
   compare to a GRR-anchored sim run (this branch), we can tell
   whether the reference is Apollo-standard or slightly-off.  The
   PASCs 13 Xe / Ve columns in TM-X-62831 Table 4 are the right
   place to look.

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
