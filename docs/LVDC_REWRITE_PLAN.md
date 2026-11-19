# LVDC rewrite plan

**Branch:** `lvdc-rewrite`
**Status:** landed. The rewrite under
`lib_SaturnIBObliq/native-snapshots/lvda-lvdc-rewrite/` is the default
LVDC implementation (`LVDC_USE_REWRITE=ON`). The legacy hand-edited
golden is retained under
`lib_SaturnIBObliq/native-snapshots/lvda-lvdc-legacy/` for A/B
comparison; link it with `-DLVDC_USE_REWRITE=OFF`. See
`docs/LVDC_PHASE_D_CLOSURE.md` for the validation record.
**Authoritative references:**
- Saturn IU LVDC Equation Defining Document (EDD), Rev A, 11 Apr 1972
  (`reference/LVDC-EDD/` — scans in seven PDFs, Chandra and Marker OCR
  under `chandra/` and `marker/`)
- NASA TM-X-62831, AS-205/CSM-101 Operational Trajectory, Rev I,
  10 Sep 1968 (`reference/NASA-TM-X-62831/` and digitized tables under
  `model/as205-reference/tm_x_62831_*.csv`)
- Project Apollo Coordinate System Standards (MSC MA-1461, 1 Jun 1965)

## Framing

The current `LVDA_LVDC.c` is obliq-2 3.8.5 codegen output that was
later hand-edited to fix the sensor-path leak (§4.3.1-5 integration,
Eqs. 4.3.6-12 gravity) and the HSL cutoff (§4.4.2.4). The code
structure retains the flattened-block form of the codegen, which
makes the EDD provenance of each equation hard to see. The rewrite
replaces that body with a module organised by EDD section, with each
equation cited at the site that implements it.

The rewrite is a hypothetical: how would an engineer of the era build
the LVDC flight program if they had been given C instead of
assembler? That framing asks us to follow the EDD on the structure
and scheduling of the computation, not just the equations.

The rewrite keeps the obliq-2 native atomic ABI (see
`~/src/obliq-2/docs/NATIVE_SUBSYSTEM_DESIGN.md` §4) and preserves the
subsystem name `LVDA_LVDC`, so the obliq-2 parent code and the JSON
model do not change during Phases A-C.

## Scope

**In scope (AS-205 boost-to-orbit):**

| EDD section | Content |
|-------------|---------|
| §2 | Reference systems and transformation matrices [MSG], [MES], [MBS], [MSV], [MSA] |
| §3 | Prepare-to-launch: targeting load, GMT sync, azimuth laying, flight-program init |
| §4.2 | Accelerometer data format, processing, velocity accumulation, F/M, M/F smoothing |
| §4.3 | Boost navigation: integration (Eqs. 4.3.1-5), gravity (Eqs. 4.3.6-12) |
| §4.4 | Boost guidance: first-stage time-tilt (§4.4.1), IGM (§4.4.2), GRF (§4.4.3), SMC (§4.4.4), Chi (§4.4.5) |
| §7.2 | Time bases T0-T4 (Saturn IB has no T5-T7) |
| §7.3 | Discrete outputs — switch-selector bits, stage events |
| §7.4 | Discrete inputs — liftoff, S-IB low level, DI12 stage separation, DI14 outboard engine out |

**Out of scope (deferred):**

- EDD §5 orbital nav / guidance (X freeze, inertial hold, track local,
  nav update) — AS-205 is single-orbit; the plant S-IVB freezes after
  cutoff.
- EDD §10 digital command system (DCS uplink word processing) — not
  exercised on AS-205 operational trajectory.
- EDD §12 preflight tests — ground-only routines.
- S-IVB restart and the accompanying time bases on Saturn V.

## obliq-2 native ABI contract (locked)

From `~/src/obliq-2/docs/NATIVE_SUBSYSTEM_DESIGN.md` §4:

| Function | Call cadence |
|----------|--------------|
| `LVDA_LVDC_init` | Once at simulation start |
| `LVDA_LVDC_compute_outputs` | Once at step start + once per RK4 stage (k1..k4). Up to 5 calls per major step. Writes to `model->states.*` from k2..k4 are discarded (parent restores `*model = rk4_major` between stages). |
| `LVDA_LVDC_update_memory_states` | Once per major step, after the RK4 loop |
| `LVDA_LVDC_evaluate_enable_states` | Co-scheduled with `compute_outputs` |
| `LVDA_LVDC_compute_derivatives` | **Not emitted** — the LVDC is a discrete-only module |

`model->sample_tick` is stable across the k1..k4 stages of one major
step and increments after the whole step completes. It is the correct
token for discrete-cycle gating inside `compute_outputs`.

**Rule for the rewrite:** any `static` or host-global write inside
`compute_outputs` must gate on a tick change. Discrete state advances
(timebase transitions, IGM mode changes, latch-sets) live in
`update_memory_states`. See the Phase A.1 probe tests for empirical
confirmation.

## Module layout (proposed)

Native source lives alongside the current snapshot, then replaces it
in Phase D. During Phases A-C the old module remains the compile
source so the system keeps running.

```
lib_SaturnIBObliq/native-snapshots/lvda-lvdc-rewrite/
    LVDA_LVDC.h              Public ABI. Identical port-struct field
                             names to the current header; internal
                             state struct is EDD-shaped (not codegen-
                             shaped).

    LVDA_LVDC.c              obliq-2 entry points. init / 
                             compute_outputs / update_memory_states.
                             Thin dispatcher into the modules below.

    lvdc_params.h            Preset data block from EDD Part II +
                             TM-X-62831 Table 3B + AS-206 EDD Part II.
                             One struct, every value cited.

    lvdc_frames.c / .h       EDD §2.3 transforms: [MSG], [MES], [MBS],
                             [MSV], [MSA]. Pure functions.

    lvdc_nav.c / .h          EDD §4.2 accel-data processing and §4.3
                             boost navigation. PIPA processing, F/M,
                             M/F smoothing, (F/M)_c, trap integration
                             (Eqs. 4.3.1-5), oblate gravity
                             (Eqs. 4.3.6-12).

    lvdc_preigm.c / .h       EDD §4.4.1 first-stage time-tilt chi.

    lvdc_igm.c / .h          EDD §4.4.2 IGM: intermediate parameters
                             (Eqs. 4.4.20-28), Chi assembly, Chi
                             steering, DeltaT_b, artificial-tau
                             blend, SMC Y/Z (§4.4.4), HSL per
                             §4.4.2.4.

    lvdc_grf.c / .h          EDD §4.4.3 Guidance Reference Failure
                             (minimal — detection + mode latch).

    lvdc_scheduler.c / .h    EDD §7.2 time bases T0-T4. Discrete
                             mode machine with entry conditions from
                             the EDD. Owns minor-loop (25 ms) and
                             major-cycle (1 s) counters and major
                             computation sync (1.6 s BML).

    lvdc_discretes.c / .h    EDD §7.3-7.4 discrete I/O mapping:
                             switch-selector output bits and DIN
                             latches. Translates the AS-205 subset
                             into the subsystem output flags
                             (bIECO, bOECO, bSIVEngineStart, etc).

    lvdc_quant.c / .h        Sensor and actuator quantization per
                             EDD §4.2.1 (PIPA count format) and
                             Part I §10 (LVDA DAC / resolver word).
                             Scope confirmed in Phase A.2.
```

## Scheduling design

The real LVDC runs multiple rates on top of a 25 ms minor loop:

| Rate | Purpose | EDD anchor |
|------|---------|------------|
| **40 ms (25 Hz)** | Minor loop — resolver read, attitude error command to FCC, DI24 interrogation | §4.1, §8.1 (boost rate; 100 ms orbit rate is out of scope) |
| 1.0 s (1 Hz) | Major cycle — time-base bookkeeping, routine dispatch | §7.2 |
| 1.6 s (nominal, variable) | Boost major loop (BML) — accelerometer read, nav integration, IGM, F/M, M/F smoothing | §4.1, §4.2.5, §4.3.1 |

In the obliq-2 host, the native block gets called at the parent
fundamental step (0.005 s per `globalSettings.simulationTimeStep`).
The native block's `sampleTimeSec` property gates the call: only
ticks where `sample_tick % round(sampleTimeSec / dt) == 0` get a call.

**Approach:** keep `LVDA_LVDC_compute_outputs` called at the parent
fundamental step (0.005 s) as it is today — see
`LVDC_RK4_REENTRY_REPORT.md` for the empirical observation that the
current host does not gate by `sampleTimeSec` on this subsystem.
Inside `compute_outputs` (guarded by the per-tick-guard idiom) and
`update_memory_states`, run the minor loop every 40 ms and the slower
rates on internal counters:

```c
/* in update_memory_states, running once per major step at 0.005 s */
if (++minor_loop_ctr >= 8) {                 /* 40 ms = 8 · 0.005 s */
    minor_loop_step(lvdc);                   /* 25 Hz during boost */
    minor_loop_ctr = 0;
}
if (++major_cycle_ctr >= 200) {              /* 1 s = 200 · 0.005 s */
    major_cycle_step(lvdc);
    major_cycle_ctr = 0;
}
if (++bml_ctr >= 320) {                      /* 1.6 s = 320 · 0.005 s */
    bml_step(lvdc);
    bml_ctr = 0;
}
```

In Phase D we can revisit adding an explicit `sampleTimeSec: 0.005`
(or coarser) on the subsystem to save host cycles, but it is not
required for correctness.

**Phase A.1 probe tests must confirm:** `sample_tick` monotonicity
across RK4 stages, update_memory_states timing, and the behaviour
of static-variable writes under RK4 re-entry.

## Quantization strategy

Per `~/src/obliq-2/CLAUDE.md`, "sensor fidelity is a key goal" and
quantization is part of the sensor path. The LVDA/LVDC rewrite
models:

| Signal | Quantum | Source | Modelled where |
|--------|---------|--------|----------------|
| PIPA pulse / velocity increment | 0.05 m/s (to be confirmed, EDD §4.2.1) | ST-124M → LVDA signal conditioner | `lvdc_quant` on input |
| Resolver gimbal angle | ADC LSB (verify in EDD Part I §10) | ST-124M resolver → LVDA ADC | `lvdc_quant` on input |
| Commanded Psi_deg to FCC | D/A LSB (verify) | LVDA D/A on output | `lvdc_quant` on output |
| Discrete timing | 25 ms minor-loop tick | LVDC interrupt timebase | Scheduler |
| LVDC word scaling | **Not modelled in v1** — doubles throughout | 26-bit two's complement scaled integer | Future work |

The plant already runs the ST-124M with an `Accelerometer_Quantization`
block. The current LVDA_LVDC reads a continuous-valued (already-
quantized once) velocity on `V_m_bar_mps`. Whether to re-quantize on
the LVDA side or treat the plant output as the final quantized word
is a Phase A.2 decision — needs the EDD §4.2.1 format inspected.

## Validation strategy

**Per-phase bar:**

- Phase A: no behaviour change (branch, docs, probe tests only).
- Phase B: new nav module produces `R_S_bar_m` and derived F/M that
  match the current module to within integration-order noise, given
  the same `V_m_bar_mps` input stream.
- Phase C: new guidance produces `Psi_deg` and the stage-event
  discretes within current-implementation tolerance against TM-X-62831
  trajectory tables.
- Phase D: full stack regression. Target residuals **no worse** than
  current implementation (per `project_rk4_fourth_order_rebase.md`,
  `project_sivb_cutoff_tailoff_gap.md`, `project_cat_from_tnd2829.md`).
  Stretch target: tighter residuals if the cleaner EDD implementation
  exposes small errors in the current flattened form.

**Non-goal:** chasing trajectory residuals by re-tuning constants.
Per `feedback_reference_backed_fixes.md`, numerical changes must come
from an EDD or TM citation, not from matching a reference trace.

## Risks

1. **RK4 re-entry surprises.** The obliq-2 doc describes the contract
   but the current `LVDA_LVDC.c` has worked around subtleties with
   `s_last_tick` guards and static state. The probe tests (Phase A.1)
   will build empirical confidence before the scheduler is written.
2. **Quantization mismatch.** Re-quantizing at the LVDA input on top
   of plant quantization may cause small step-wise residuals; dropping
   plant quantization and doing it only at the LVDA would change the
   ST-124M model.  Phase A.2 produces the spec to settle this.
3. **Hidden coupling via signals_t.** The current module reads its own
   `signals.LVDC_S_Frame_...Eqns_3_4_3_4_Mux{1,2}` for X_0 / V_0 at
   GRR. These are computed inside the subsystem from presets, so the
   rewrite can compute them locally — but auditing every such
   cross-reference inside the existing 4967-line C body will be
   tedious. Approach: walk the EDD, write the clean module, then
   diff outputs on identical input streams.
4. **Discrete-event timing jitter.** The current module sometimes
   latches on the first tick after a condition goes true; the EDD
   specifies interrupt-driven latching on the same tick the condition
   is tested. Expect small (one-tick) timing shifts in stage-event
   discretes.

## Milestones

| Phase | Deliverable |
|-------|-------------|
| **A** | Branch cut. Port provenance doc. This plan doc. RK4 probe tests. Quantization inventory. |
| **B** | `lvdc_frames`, `lvdc_nav`, `lvdc_quant` compiled into a parallel module. Shadow-run on identical `V_m_bar_mps` stream vs the current body. Nav path matches to integration-order noise. |
| **C** | `lvdc_preigm`, `lvdc_igm`, `lvdc_grf` wired in. Full guidance path produces `Psi_deg` and stage events. Residual ≤ current implementation vs TM-X-62831. |
| **D** | `lvdc_scheduler`, `lvdc_discretes` finalise T0-T4 state machine. Flag-controlled A/B swap at the obliq-2 boundary. Old snapshot removed. Nine unused boundary ports pruned from `saturn_ib_stack.json` (parent diff shown for approval). Regression blessed. |

## Commit cadence

Small reviewable commits on the `lvdc-rewrite` branch. No pushes to
origin. Every commit cites the EDD section it implements or the
decision it records.
