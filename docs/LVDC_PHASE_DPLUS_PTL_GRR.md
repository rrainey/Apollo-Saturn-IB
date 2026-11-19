# Phase D+ — PTL controller and EDD-faithful GRR

**Status:** closed-with-known-gap — a full EDD-faithful PTL
(Prepare To Launch) controller with GRR at T−5 s requires
plant-side changes outside the LVDC rewrite scope. The
dispatcher's current latch-at-liftoff approximation produces
trajectory residuals at the project baseline for both the
hand-edited golden and the rewrite.

## What EDD §3 and TMX say

- **GRR** fires at T−5.0 s on AS-205 (per TMX, see
  `reference_as205_grr_minus_5sec`). GRR is an LVDC-internal
  countdown event — not a plant-provided discrete.
- At GRR the LVDC:
  1. Stops erecting the ST-124M (platform becomes inertial).
  2. Zeros the PIPA pulse accumulators (EDD §4.2.3).
  3. Snaps X_0 and V_0 per EDD Eqs. 3.4.3-4 (constants
     computed from launch-site presets R_L, φ_L, A_z and
     Earth rotation rate ω_E).
  4. Starts integrating V_m (from PIPA pulses) and V_Sg
     (from Eq. 4.3.4 gravity).
- Between GRR and liftoff (5 s), the vehicle is static on
  the pad in the Earth-rotating frame; in ECI, it has a
  constant velocity equal to ω_E × R_L.

## What the current rewrite does

- **No pre-liftoff LVDC run.** The dispatcher latches on
  the `bLiftoff` rising edge: PIPA zero, X_0/V_0 snap, and
  start of V_m/V_Sg integration all happen at T0.
- **S-frame orientation:** anchored at `bLiftoff` edge
  instead of T−5 s GRR edge. In ECI terms, the two
  anchorings differ by Earth rotation over 5 s.

## Residual impact — does GRR timing move the +6.5 km bias?

**Frame-orientation bound:** The S-frame rotates with Earth
when it's defined earlier. ω_E × 5 s × R = 7.29e-5 × 5 ×
6.4e6 ≈ **2.3 km** in position at Earth radius — the
maximum possible error from the 5-second anchor mismatch.

**Observed residual:** +6.5 km at cutoff. More than the
2.3 km frame-rotation bound, so **GRR timing cannot be the
primary cause**. It could contribute at most ~2 km.

**V_0 and X_0 are not time-dependent.** Both are evaluated
from static preset constants (R_L, φ_L, A_z, ω_E). The
value doesn't change whether the latch fires at T−5 s or
T0 — only the frame-orientation reference differs.

**V_m and V_Sg accumulations cancel.** During 5 s of static
pad operation, V_m accumulates +1g vertical (specific force)
and V_Sg accumulates −1g vertical (gravity). Eq. 4.3.5
`V_S = V_m + V_Sg + V_0` cancels these, leaving V_S = V_0
either way. Position accumulations ΔX_mf (from V_m) and
ΔX_Sg (from gravity) also nearly cancel.

**Golden-level parity.** `project_lvdc_frame_anchoring`
(2026-08 fix for Issue #4) notes the project-wide plumbline
frame was already anchored at GRR correctly — matching TMX
Table 4 within 5 km / 20 m/s. The rewrite's latch-at-liftoff
approximation reaches the same residual as the golden
(both end at |R| = 6606.81 km), so both bodies use
compatible frame anchoring.

## Not implemented here, and why

A full PTL controller would require:

1. **A plant-side GRR discrete or countdown signal.** The
   plant model currently emits only `bLiftoff`; no "T minus
   N seconds" indicator reaches the LVDC. Adding one is a
   plant adapter change, not an LVDC task.

2. **Pre-liftoff LVDC run mode.** The dispatcher would need
   to start the nav loop before `bLiftoff` rises, requiring
   a separate pre-launch tick source.

3. **Platform alignment / gyro-uncaging state machine.**
   EDD §3 covers this extensively; it is the pre-GRR work
   that determines S-frame initial orientation.

None of these are needed to close the +6.5 km altitude
residual, because that residual is larger than the maximum
possible GRR-timing contribution and is shared with the
golden (which uses the same anchoring).

## Where the +6.5 km residual actually comes from

Likely contributors (not addressed in this phase, each small
on its own):

- **Plant-side thrust / mass tolerances** (see
  `project_sivb_cutoff_tailoff_gap`, `project_h1_thrust_gain_provenance`,
  `project_j2_isp_two_level`). A fraction of a percent of
  over-performance integrates into kilometres of altitude
  by cutoff.
- **Atmospheric profile** US-1976 vs PRA-63
  (`project_atmosphere_model_investigation`, estimated
  ±0.5 m/s velocity — sub-km altitude).
- **S-IB pitch program fidelity** vs AS-205 flight data
  (TMX App B coefficients vs actual).

All plant-side, all common to both bodies. Not an IGM
rewrite concern.

## Conclusion

Phase D+ is closed as "known gap". A full PTL / GRR-at-T−5
implementation is deferred to a future plant+LVDC coupling
effort. The current latch-at-liftoff approximation produces
trajectory residuals at or below the hand-edited golden on
TMX Appendix C validation (position rms 4507 m, velocity
rms 25.75 m/s, slightly better than golden's 4836 m /
26.55 m/s).
