# LVDC IGM — Phase C.3f shadow replay

**Branch:** `lvdc-rewrite`
**Build:** `ee0282f` (C.3f-5 dispatcher wire-in)
**Date:** 2026-11-18

Shadow-replay of the AS-205 500 s dt=0.005 trace through the
rewrite with IGM active. The golden column comes from the
current hand-edited body, which runs the preigm polynomial
throughout boost (no IGM). The rewrite runs preigm until the
T1 ≥ 166.8 s latch flips, then switches to IGM for pitch and
yaw.

## Pipeline

```bash
# 500 s run, dt=0.005 so sample_tick advances by 1 per step.
./build/batch_sim /tmp/lvdc-shadow/input-fine.json \
    --lvdc-io-trace /tmp/lvdc-shadow/trace500.csv

./build/lib_SaturnIBObliq/native-snapshots/lvda-lvdc-rewrite/replay_lvdc_trace \
    /tmp/lvdc-shadow/trace500.csv \
    --delta-csv /tmp/lvdc-shadow/delta500.csv \
    --abs-tol-m 1e8
```

## Headline numbers

| Metric      | Value     | Interpretation                               |
| ----------- | --------- | -------------------------------------------- |
| `|ΔR_S|` max | 5.5 m     | In family with Phase B.5 k1/k4 trace-hook noise |
| `|ΔR_S|` rms | 3.1 m     | Grows linearly, PIPA-quantization scale      |
| `|ΔPsi|` max | 15.7°     | IGM taking over at T1 = 166.8 s              |
| `|ΔPsi|` rms | 4.3°      | Post-IGM-enable only; nav-era is near zero   |

## Psi divergence breakdown

Split at the IGM-enable boundary (T1 = 166.8 s → trace t = 466.8 s):

| Window                   | Max `|ΔPsi|` | Source                               |
| ------------------------ | ------------ | ------------------------------------ |
| 400 s ≤ t < 466.8 s      | **0.0575°**  | One DAC LSB — rewrite matches golden |
| 466.8 s ≤ t ≤ 500 s      | **15.70°**   | IGM in rewrite vs. preigm in golden  |

Pre-IGM the rewrite is bit-identical to the golden modulo one
DAC quantum. The ramp-rate difference described in Phase C.2c
has clearly been closed.

Post-IGM the divergence is expected — the two bodies are now
computing different attitude commands. The shadow replay cannot
validate IGM correctness; it only shows IGM activated on
schedule and produces attitude commands at the DAC saturation
limit.

## What this tells us

- **Nav path is unchanged.** `|ΔR_S|` at 5.5 m peak and 3.1 m rms
  matches the Phase B.5 trace-hook artifact (k1 vs k4 sampling of
  `V_m_bar_mps`) within the expected growth. No Phase C.3f regression
  on the nav pipeline.
- **Preigm path is unchanged.** 0.0575° peak Psi divergence over
  the 66 s pre-IGM post-liftoff window is a single DAC LSB. The
  rewrite tracks the golden preigm bit-perfectly.
- **IGM activates at the correct boundary.** `|ΔPsi|` steps from
  near-zero to double digits at t = 466.8 s, exactly where
  `T1 ≥ igm_enable_from_T1_sec` (166.8 s) latches.
- **IGM is commanding, but correctness is unverified.** Peak Psi
  after enable sits at the DAC magnitude limit (15.3°) plus some
  rate-ramp overshoot. That is consistent with IGM demanding a
  large pitch step at first enable; whether the commanded angle is
  physically correct for AS-205 cannot be judged from this
  comparison.

## Next validation steps

The shadow replay closes on the preigm vs preigm case. To validate
IGM correctness we need a TMX-trajectory comparison:

- **C.3g** (phase): run the rewrite body in batch_sim (not shadow
  replay) and compare the resulting trajectory against TMX
  Appendix C trajectory listing (X, Y, Z, V, altitude, velocity
  angles). Residuals should be smaller than the current body,
  since the rewrite's IGM is EDD-anchored.
- **HSL** (EDD §4.4.2.4 Eqs. 4.4.88-94): until HSL lands, S-IVB
  cutoff timing is not EDD-faithful. The shadow trace ends at
  500 s, before S-IVB cutoff, so this does not affect the
  current numbers — but C.3g will.
- **T3-based enable** (Phase D): the T1 = 166.8 s latch is a
  fallback. Once Time Bases are wired, swap to T3 + 25 s.
- **XVG = -9.10 vs -9.15** (task #9): one of the IGM preset
  values is ambiguous between TMX cites. Minor residual effect
  on terminal gravity estimate inside IGM.
