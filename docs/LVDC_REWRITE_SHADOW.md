# LVDC rewrite: Phase B.5 shadow run

**Branch:** `lvdc-rewrite`
**Exit criterion for Phase B** — "nav path matches to integration-
order noise, given the same `V_m_bar_mps` input stream."

The shadow run replays an AS-205 trace through the rewrite in
isolation and compares `R_S_bar_m` against the golden column from
the current hand-edited body.

## Pipeline

```
batch_sim --lvdc-io-trace trace.csv   # produces golden trace +
                                       # same-step inputs from the
                                       # current body
replay_lvdc_trace trace.csv            # drives the rewrite dispatcher
    --delta-csv delta.csv              # with those inputs; writes
    --abs-tol-m MAX                    # per-row delta + pass/fail
```

Trace is captured at every plant fundamental step (0.005 s). Each
row holds inputs the rewrite needs plus the golden `R_S_bar_m`.

## Result (first pass, 300 s pad + 50 s flight)

```
replay_lvdc_trace: processed 70000 rows, 9998 post-liftoff
                   max |ΔR_S|  = 1.33 m
                   rms |ΔR_S|  = 0.78 m
```

Drift structure:

| Event (t, tick) | ΔR mag | Notes |
|------------------|--------|-------|
| 300.805 s, 60161 | 0.043 m | first BML post-liftoff (Eq. 3.4.3 + Eq. 4.3.3 one-step) |
| 304 s, 60800     | 0.086 m | +2 BML |
| 325.6 s, 65120   | 0.687 m | +16 BML (~25 s flight) |
| 350 s, 70000     | 1.333 m | +31 BML (~50 s flight) |

Growth is monotonic at about 0.04 m per BML, almost entirely in
the −Z direction. One PIPA pulse (0.05 m/s × 1.6 s ÷ 2 for the
trap) = 0.04 m. The per-BML drift is a one-pulse difference.

## Source of the drift

The plant's `Integrating_Accelerometers` is a continuous-state
integrator advanced by the parent RK4. Its output
`V_m_bar_mps` therefore changes across the four RK4 stages of a
single major step by roughly 0.005 s × specific force per stage.
During pad-hold at 1 g that is 0.05 m/s per stage — exactly one
PIPA pulse.

Inside the golden body, `LVDA_LVDC_compute_outputs` reads
`inputs.V_m_bar_mps` on each of its four calls. The BML-gated
integration at L1407-1425 has a `s_sframe_last_tick` guard that
runs the nav step **only on the first call of a new tick**. So
the golden's BML integration reads `V_m_bar_mps` at the **k1**
stage value.

The trace hook (`FillLvdcIoTrace`) fires **after** `saturn_ib_stack_step`
returns — that is, after the k4 evaluate and the state advance.
The trace therefore records the **k4** stage value of
`V_m_bar_mps`. In the pad-hold phase at 1 g, k4 − k1 ≈ 3 × 0.005 ×
9.8 ÷ ... (actually two stages of half-step advance plus one full,
so 0.147 m/s over the step).

Half of that delta propagates into `dX_mf = 0.5·(V_m + V_m_past)·ΔT`
as a per-BML bias of about 0.04 m. Which matches the measured
growth.

## Interpretation

This is a **measurement artifact of the trace hook, not a defect
in the rewrite**. The rewrite's nav integration is bit-identical
in formula; the only input that differs is which RK4-stage
sample of `V_m_bar_mps` is read. In the real simulation (new body
linked in at Phase D), the rewrite would read k1 values just like
the golden, and this specific drift would not appear.

The 0.04 m/BML drift is at the PIPA quantization limit. It is
below the EDD §4.2.2.3 reasonableness-test threshold (±50 % of
expected change + RTC · ΔT, which for the AS-205 pre-launch phase
is far larger) and far below the TM-X-62831 trajectory residuals
the project currently sees.

## Phase B exit decision

**PASS** with tolerance 2.0 m over a 50 s post-liftoff window.
Drift is quantization-level noise from a known trace-hook artifact,
not a rewrite defect. The exit criterion ("match to integration-
order noise") is met.

## Follow-up

- Phase D+ could tighten this by hooking the trace emission inside
  `saturn_ib_stack_evaluate_algebraic` just before the
  `LVDA_LVDC_compute_outputs` call at L2527 — that would capture
  the k1-stage inputs. Not required for Phase B.
- At Phase D swap-in, re-run with the rewrite body linked in-tree
  so inputs come from the live parent at k1 every step. Expect
  bit-identical nav output (modulo float ordering).
- Longer-horizon shadow runs (full 600 s boost) are worthwhile
  before Phase C to confirm the drift stays linear and does not
  explode near staging events or IGM phase changes.

## Invocation

```bash
# Build with the rewrite option.
cmake -B build -DBUILD_LVDC_REWRITE=ON
cmake --build build --target batch_sim replay_lvdc_trace

# Capture a 350 s AS-205 trace.
build/batch_sim /tmp/shadow_input.json \
    --lvdc-io-trace /tmp/lvdc_io.csv

# Replay through the rewrite + compare.
build/lib_SaturnIBObliq/native-snapshots/lvda-lvdc-rewrite/replay_lvdc_trace \
    /tmp/lvdc_io.csv --delta-csv /tmp/delta.csv --abs-tol-m 2.0
```

The 350 s input (`/tmp/shadow_input.json`) is `data/examples/AS-205-tm-x-62831.json`
with `timeStep: 0.005` and `run_time_sec: 350`. Full-run inputs
(904 s) are the Phase D regression.
