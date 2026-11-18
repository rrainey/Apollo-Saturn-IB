# obliq-2 native atomic RK4 re-entry: empirical findings

**Date:** 2026-11-18
**Branch:** `lvdc-rewrite`
**Purpose:** validate the obliq-2 native atomic ABI contract described
in `~/src/obliq-2/docs/NATIVE_SUBSYSTEM_DESIGN.md` §4 against the
actual generated host code before writing the LVDC scheduler.

**Method:** direct code inspection of `lib_SaturnIBObliq/plant/generated/saturn_ib_stack.c`
(obliq-2 3.8.5 cgen-manifest commit `67866ac2a23c`). This is stronger
evidence than a mock probe because it captures what the production
host actually does, including any behaviour the design note might
not fully describe.

## Call pattern per major step

```
saturn_ib_stack_step(model):                                [host, saturn_ib_stack.c L13359]

  # Step-start algebraic eval (publishes outputs for k1)
  saturn_ib_stack_evaluate_algebraic(model)                 [L13361]
    └── LVDA_LVDC_compute_outputs(...)                      [L2527]   # CALL #1

  saturn_ib_stack_derivatives(t0, model, states, &k1)       [L13373]
                                                            # no LVDA call inside derivatives

  rk4_major = *model                                        [L13374]  # snapshot AFTER k1 eval

  # k2 stage
  advance states to rk4_major.states + (h/2)·k1
  saturn_ib_stack_evaluate_algebraic(model)                 [L13554]
    └── LVDA_LVDC_compute_outputs(...)                                 # CALL #2
  saturn_ib_stack_derivatives(t0+h/2, model, states, &k2)   [L13555]

  # k3 stage
  advance states to rk4_major.states + (h/2)·k2
  saturn_ib_stack_evaluate_algebraic(model)                 [L13736]
    └── LVDA_LVDC_compute_outputs(...)                                 # CALL #3
  saturn_ib_stack_derivatives(t0+h/2, model, states, &k3)   [L13737]

  # k4 stage
  advance states to rk4_major.states + h·k3
  saturn_ib_stack_evaluate_algebraic(model)                 [L13918]
    └── LVDA_LVDC_compute_outputs(...)                                 # CALL #4
  saturn_ib_stack_derivatives(t0+h, model, states, &k4)     [L13919]

  # RK4 combine: states += (h/6)·(k1 + 2·k2 + 2·k3 + k4)

  saturn_ib_stack_update_memory_states(model)               [L14321]
    └── LVDA_LVDC_update_memory_states(...)                 [L13021]   # ONE CALL per step

  saturn_ib_stack_evaluate_enable_states(model)             [L14324]
  model->time    += model->dt                               [L14327]
  model->sample_tick++                                      [L14328]
```

## Findings vs. the design note

| Claim in `NATIVE_SUBSYSTEM_DESIGN.md` §4 | Observed in code | Verdict |
|------------------------------------------|-------------------|---------|
| "up to five calls per major step (step start + k1..k4)" | **Four calls** per major step. The step-start `evaluate_algebraic` is the k1 algebra; `derivatives()` does not re-call `compute_outputs`. | Design doc off by one; **actual = 4 calls**. |
| `sample_tick` stable across the four RK4 stages, increments after the whole step | `model->sample_tick++` runs once at L14328, after RK4 combine, memory update and enable evaluation. The submodule's `sample_tick` is copied from parent in every `evaluate_algebraic` (L2431), so it is the same value across all four calls. | **Confirmed**. |
| `update_memory_states` called once per major step after RK4 | Called inside `saturn_ib_stack_update_memory_states` (L13021) which is itself called once at L14321, after the RK4 combine. | **Confirmed**. |
| Writes to `model->states.*` from k2..k4 do not stick | Each stage overwrites `model->states.*` with `rk4_major.states + stage_factor·k_prev`, so any write a submodule makes to its own `model->states.*` during k2..k4 is stomped before the next evaluation. The final state update at the end combines k1..k4 derivatives into `rk4_major.states`, not into writes from `compute_outputs`. | **Confirmed**, with an important nuance: see "States under RK4" below. |
| Writes to `static` / host-global variables are not restored | `rk4_major = *model` copies the submodule struct but not file-scope statics. | **Confirmed**. |

## States under RK4 — the practical rule

In the generated host the submodule is treated as "part of the big
state vector": `rk4_major = *model` snapshots `model->Saturn_Instrument_Unit_IU_LVDA_LVDC_LVDA_LVDC`
in full. Between RK4 stages the host restores the submodule from
`rk4_major` *only for the fields it explicitly re-advances* (the
`model->states.*` members). Fields like `inputs`, `outputs`, `signals`,
`data_stores`, `enable_states`, `time`, `dt`, `sample_tick` are not
restored by the RK4 integrator — they are re-published by the next
`evaluate_algebraic` call.

**Practical implications:**

1. **Writes to `model->states.*` inside `compute_outputs` are useless
   under RK4.** Each k-stage advance reads from `rk4_major.states` and
   writes to `model->states`, discarding any write the previous
   `compute_outputs` made to `model->states`. The design note is right
   about this; the subtlety is that the *mechanism* is not a snapshot
   restore but rather a direct `model->states.*_states[i] =
   rk4_major.states.*_states[i] + factor · k_prev.*_states[i]`
   recomputation before each stage.

2. **Writes to `model->data_stores.*` or `model->outputs.*` or
   `model->signals.*` from the k1 call persist into k2 memory until the
   k2 `evaluate_algebraic` overwrites them.** Because these are not on
   the RK4 restore path, the last stage's writes stick. This matches
   the design note "writes to `model->*` are fine".

3. **The LVDC uses discrete state, not continuous state.** It has no
   `compute_derivatives`. All its per-step state advance happens in
   `update_memory_states` (which runs exactly once per step after the
   RK4 loop). This is exactly the right shape for the rewrite —
   continuous RK4 re-entry is irrelevant to the LVDC's state advance.

## Validation of the per-tick-guard idiom

The current hand-edited `LVDA_LVDC.c` uses the pattern:

```c
static unsigned long long s_last_tick = (unsigned long long)-1;
if (model->sample_tick != s_last_tick) {
    /* one-shot per major step work */
    s_last_tick = model->sample_tick;
}
```

in three places (L1407-1425 for S-frame nav, L4035-4045 for IGM
first-pass, and implicitly via the `s_grr_latched` flag at L1378-1393).
Given `sample_tick` is stable across the four `compute_outputs` calls
of one step and increments once per step, this pattern fires on the
first call of each new major step and skips the following three. For
the LVDC rewrite, **this is the canonical pattern** for any computation
inside `compute_outputs` that must happen at most once per major step
but cannot wait until `update_memory_states` (for example, because its
output must be published in the same step's outputs).

## LVDA_LVDC call site is NOT gated by `sampleTimeSec`

An independent observation: at L2527 the host calls
`LVDA_LVDC_compute_outputs` unconditionally every `evaluate_algebraic`
pass (no `if ((sample_tick % N) == 0ULL)` wrapper). This is because
the JSON model does not carry an explicit `sampleTimeSec` on the
`LVDA_LVDC` segregated subsystem; the body does its own rate gating
internally (for example the 1.6 s BML gate at L1395).

**Implication for the rewrite:** we can either (a) keep running at
every fundamental tick (0.005 s, 200 Hz) and gate every rate internally
as today, or (b) add an explicit `sampleTimeSec: 0.025` on the
subsystem in `saturn_ib_stack.json` to let the host skip calls when the
LVDC is at rest. Option (b) is more efficient but requires a parent
model edit, which we deferred to Phase D. **Decision:** stick with
option (a) for Phases A-C; revisit at swap-in.

## Rules for the rewrite (locked)

1. **`compute_outputs` is a pure algebraic function of inputs, states,
   data_stores, and host-global state.** It is called four times per
   major step with the same `sample_tick`. It must be idempotent:
   calling it four times must yield the same outputs it would yield on
   the first call, given the same inputs.

2. **Discrete state advance lives in `update_memory_states`.** Timer
   ticks, IGM mode transitions, HSL cutoff latches, GRR arming — all
   of these are state transitions and belong in the deferred update.
   They must not live in `compute_outputs` even behind a tick guard,
   because other blocks in the parent read the data_stores and signals
   during the same `evaluate_algebraic` pass that calls `compute_outputs`.

3. **Where a quantity must be published in the same step it was
   computed (and the computation is not a pure function of current
   inputs), use the per-tick-guard idiom in `compute_outputs`.** This
   covers initializations like the GRR latch, where V_m_offset must be
   written on the first call of the step that sees `bLiftoff` go true
   and then read on the same step.

4. **Static / host-global state is survivable across RK4 stages, but
   also across LVDC restarts.** `init()` must explicitly reset every
   static. The current code has a few statics whose initial values
   rely on C default-zero (`s_grr_latched = 0`) — the rewrite will
   put every static inside a reset function called from `init`.

5. **Side-effect-free algebra only in `compute_outputs`.** Any trace
   write, file write, or external-state write must gate on
   `model->sample_tick` so it fires once per major step.

## Follow-ups for Phase A.2

- The current `s_grr_latched` global is a module-scoped singleton; if
  the LVDC is ever instantiated twice (unlikely but possible under
  obliq-2 hot reload) the second instance would share the first's
  state. The rewrite will move all statics into the `LVDA_LVDC_t`
  struct (specifically a new nested `static_t` or in `states_t`).

- The code at L2515-2526 copies nine legacy inputs the body no longer
  reads (see `LVDA_LVDC_PORT_PROVENANCE.md`). Phase D will prune these
  from the JSON model and re-generate.

## No mock probe needed

Direct inspection of the production obliq-2 cgen output gives a
complete, unambiguous answer. A mock probe would only re-derive what
the code already shows. Skipping the probe saves a day of scaffolding
and moves Phase A to Phase A.2 (quantization inventory) directly.
