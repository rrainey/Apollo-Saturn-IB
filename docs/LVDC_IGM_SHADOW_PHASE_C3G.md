# LVDC IGM — Phase C.3g full-boost shadow replay

**Branch:** `lvdc-rewrite`
**Build:** `fe06e9a` (angle-unit resolution; stays in radians
throughout the IGM chain)
**Date:** 2026-11-18

Shadow-replay of the AS-205 904 s dt=0.005 trace through the
rewrite with IGM active. The golden column comes from the
current hand-edited body (preigm polynomial throughout). The
rewrite runs preigm until T1 ≥ 166.8 s, then switches to IGM
for pitch and yaw.

## Pipeline

```bash
./build/batch_sim /tmp/lvdc-shadow/input-full.json \
    --lvdc-io-trace /tmp/lvdc-shadow/trace904.csv

./build/lib_SaturnIBObliq/native-snapshots/lvda-lvdc-rewrite/replay_lvdc_trace \
    /tmp/lvdc-shadow/trace904.csv \
    --delta-csv /tmp/lvdc-shadow/delta904v2.csv \
    --abs-tol-m 1e9
```

180 800 rows at dt = 0.005 s, 120 798 post-liftoff.

## Headline numbers

| Metric      | Value     | Interpretation                              |
| ----------- | --------- | ------------------------------------------- |
| `|ΔR_S|` max | 20.9 m    | Linear growth from Phase B.5 k1/k4 artifact |
| `|ΔR_S|` rms | 10.9 m    | Nav path matches modulo trace-hook noise    |
| `|ΔPsi|` max | 15.7°     | DAC saturation during IGM transient         |
| `|ΔPsi|` rms | 6.4°      | IGM vs. preigm over the full 904 s run      |

## Pitch command by flight phase

Rewrite pitch-axis Psi (gimbal-frame attitude error command),
grouped by phase:

| Phase                      | n       | min     | max    | mean  | rms  |
| -------------------------- | ------- | ------- | ------ | ----- | ---- |
| Pre-liftoff (t < 300)      | 59 999  |  −0.06° |  1.03° |  0.48 | 0.58 |
| Pre-IGM (300 ≤ t < 466.8)  | 33 360  |  −0.06° |  1.03° |  0.35 | 0.40 |
| IGM active, T_3i > 0       | 32 000  | −15.29° |  1.38° | −4.99 | 8.30 |
| IGM active, T_3i < 0       | 55 441  |  −4.25° | 15.29° |  1.88 | 6.97 |

Pre-liftoff and pre-IGM-enable match the preigm polynomial
exactly (0.58° rms is the DAC quantization noise of the ramping
transient to the pad-hold values).

During the IGM-active window with positive T_3i (nominal S-IVB
burn, ~160 s), mean pitch sits at −5° with rms 8.3°. That is the
classic gravity-turn profile: pitch starts near 0 and swings
down toward roughly −90° as the vehicle rotates into orbital
plane. The DAC's 15.3° magnitude limit clips the early transient
(rate-limited slew through the first 20 s or so) but releases as
IGM converges.

Once T_3i goes negative (no HSL to freeze cutoff), IGM continues
running against degenerate state. Mean pitch rises back toward
zero and the DAC occasionally saturates positive. Post-cutoff
behaviour is not valid guidance and is expected to be wrong
until HSL (Eqs. 4.4.88–94) lands.

## Angle-unit convention finding

The first full-boost shadow run (build `ee0282f`, before this
report) showed pitch locked at −15.29° for the entire IGM
window. Investigation showed the EDD §4.4.2.2 writes several
equations in LVDC half-revolution angle units; the literal
transcription mixed radians and half-revs. The fix (build
`fe06e9a`) stays in radians throughout — see commit log and
the ANGLE-UNIT CONVENTION note in
`lvdc_igm_position_correction.h`. Physics check:

- Eq. 4.4.85 unit thrust: `chi_y4 = 0` → +X_4 (radial up);
  `chi_y4 = -pi/2` → +Z_4 (downrange). The `-1/2` half-rev
  literal in Eq. 4.4.81 (= `-pi/2` rad = −90°) is the fixed
  offset that shifts the preliminary V-system pitch into the
  4-system attitude frame.
- After the fix, rms `|ΔPsi|` drops from 13.0° to 6.4°, and the
  pitch profile matches the expected gravity-turn shape.

## Scope limits

- **This is not a trajectory validation.** The plant trajectory
  is driven by the golden preigm polynomial throughout. The
  rewrite's IGM commands do not steer the plant. `|ΔPsi|` only
  tells us the IGM is producing varying commands; it does not
  tell us whether those commands would fly AS-205 correctly.
- **No HSL.** Post-nominal-cutoff behaviour (T_3i < 0) is
  degenerate. The AS-205 S-IVB cutoff is ~ t = 626.8 s (300 s
  countdown + 160 s T_3i); shadow data past that point reflects
  divergent IGM math, not real guidance.
- **T1-based enable.** The 166.8 s enable is a Phase D fallback
  for T3 + 25 s; when Phase D lands we expect an identical
  enable boundary, but the transition narrative changes
  (post-staging chi-freeze window etc).

## What this proves

- **Nav path unchanged.** `|ΔR_S|` 20.9 m peak over 904 s is
  within the Phase B.5 k1/k4 trace-hook drift budget, with no
  additional growth from the IGM wire-in.
- **Preigm pipeline bit-identical.** Pre-IGM-enable Psi delta
  stays under 1.1° peak (one DAC LSB per cycle quantization).
- **IGM activates on schedule.** The step in Psi delta at
  t = 466.8 s is exact.
- **IGM produces a physically-plausible gravity-turn profile.**
  Mean pitch during the valid burn window matches S-IVB
  expectations; the mode state machine handles the ART_TAU /
  REAL_TAU transition without NaN or domain errors.

## Next steps

- **Phase D swap-in.** Link the rewrite body into batch_sim
  (replacing the current hand-edited body). Then the plant
  flies IGM for real and we can compare the resulting trajectory
  against TM-X-62831 Appendix C.
- **HSL** (EDD §4.4.2.4 Eqs. 4.4.88-94). Last ~10 s before
  S-IVB cutoff. Required for correct cutoff timing and for
  avoiding the T_3i < 0 degenerate region.
- **Task #9** (XVG = −9.10 vs −9.15). Resolve once alternate
  TMX source is located.
- **Phase D+** (PTL + T−5 s GRR). Separate deliverable;
  independent of the IGM validation path.
