# Codegen Saturn heuristics (name-keyed debt)

Inventory of **model-specific** forks in mdl2obliq / Obliq codegen that match
Saturn IB block or subsystem **names** (or tight structural allowlists) to
change emitted C. These exist to recover gate1000 RTW fidelity; they are
**not** the long-term API.

Companion narrative (symptoms, surgical C history):
[`RTW_VS_OBLIQ_CODEGEN_DISPARITY.md`](./RTW_VS_OBLIQ_CODEGEN_DISPARITY.md).

**Policy going forward:** prefer intrinsic **block/subsystem/model properties**
(and general semantics) over name allowlists. New name matches need an entry
here and a retirement path.

---

## How Simulink controls sample time (properties, not workspace params)

Simulink treats timing as **block/system properties**, edited in the UI / MDL:

| Property | Where | Role |
|----------|--------|------|
| **SampleTime** | Most blocks | Discrete period, `0` continuous, `inf` constant, **`-1` inherited** |
| **SystemSampleTime** | Atomic / some SubSystems | Period for the subsystem as a unit |
| **Fixed-step size** | Model config | Fundamental solver step (ApolloA/Saturn: **0.005 s**) |
| Rate IDs / TID table | Compiled model | After **sample-time propagation**, every block gets a concrete rate |

Important behaviors ([MathWorks: sample time in systems](https://www.mathworks.com/help/simulink/ug/managing-sample-times-in-systems.html)):

1. **`-1` / inherited** is resolved at **compile** from context (drivers, atomic
   parent, multirate peers)—not left as “every fundamental step.”
2. **Atomic subsystem** with an explicit Sample time forces contents to
   `Inf`, `-1`, or that same discrete period; with inherited Sample time,
   Simulink **derives** the subsystem rate from its contents (like virtual /
   enabled subsystems).
3. **Enabled** subsystems (e.g. HSL Cutoff Timing) often show
   `SystemSampleTime -1` on the parent while **some children** carry explicit
   `0.04`; compile-time propagation assigns the rest. Obliq does not yet
   reimplement that propagator, so naïve “inherit parent IGM 1.6” is wrong for
   HSL.

Workspace numbers like `delta_T_sec = 1.6` are **data** used in equations /
compares—they are **not** the SampleTime property (though they often match
the major IGM period numerically).

---

## How Obliq represents timing today

| Layer | Mechanism | User-editable? |
|-------|-----------|----------------|
| Model fundamental step | `globalSettings.simulationTimeStep` (Saturn JSON: **0.005**) | Yes (model settings) |
| Per-block discrete period | `parameters.sampleTimeSec` on mapped blocks | In JSON; UI surfacing uneven |
| Subsystem inherit | `ModelFlattener.resolveSampleScope`: explicit `sampleTimeSec` else parent | Indirect |
| Codegen hit gate | `sample_tick % round(period/dt) == 0` | Automatic |

**Gap:** when MDL says inherit (`-1`) but RTW compiles to 0.04 / 1.6, we either
need **propagation** or we currently **name-match** and force a period. The
latter is the debt below.

**Desired end state (properties):**

1. Every discrete scope has an explicit **`sampleTimeSec` property** on the
   subsystem (or leaf) after convert—or the user sets it in the UI.
2. Flattener only inherits that property; **no** `/HSL_Cutoff_Timing/` regex.
3. Optional later: a real inherit resolver (Simulink-like) so convert can leave
   `-1` and still emit correct hits without names.
4. Keep `delta_T_sec` / `T_HSL_sec` as **model parameters** (equation inputs);
   do not conflate them with SampleTime properties unless a block literally
   reads them as Ts.

---

## Inventory (name / allowlist keyed)

### A. Sample-time defaults by subsystem name

| Match | Forced period | Location | Why |
|-------|---------------|----------|-----|
| `Iterative Guidance Mode`, `S-Frame Position…`, `Reciprocal Acceleration Filter` | **1.6 s** | `mdl2obliq/src/mapper.ts` → `isRtwGuidanceMajorCycleSubsystem` | MDL parent often `-1`; RTW tid 4 |
| ~~`HSL Cutoff Timing` → 0.04~~ | — | **Removed** | Stamped as explicit `sampleTimeSec` via `applyHslModelLevel` |

**Retirement (HSL done):** `applyHslModelLevel` stamps `sampleTimeSec: 0.04` on
`HSL_Cutoff_Timing` + children and adds `nHSLActive=1` beside mode→4. Name
heuristics for HSL removed from mapper/ModelFlattener.

**Still open:** IGM / S-frame **1.6 s** name default — prefer explicit
`sampleTimeSec` on those subsystems at convert, then delete
`isRtwGuidanceMajorCycleSubsystem`.

### B. Same-step ActionPort enable allowlist

| Match (subsystem name) | Location | Why |
|------------------------|----------|-----|
| Same-step ActionPort for mode/Chi/terminal DSWs | `AlgebraicEvaluator` — **store-name / structural** (not `Set_HSL_Mode` string) | Mode→4 ActionPort may still be named Set_HSL_Mode but is selected because it writes `nIGMMode`. **nHSLActive arm** is model-level (`applyHslModelLevel`), not codegen. |

Related (less name-specific): enable-wire sources ordered before controlled
blocks; SwitchCase `case_*` **excluded** from early pull so live DSM sees
mode writes.

**Retirement:** General **If / ActionPort same-step** semantics (condition
update → enable → action algebra in one pass) for all ActionPorts, with
correct ordering—then delete the allowlist.

### C. Live DSM read for SwitchCase selectors

| Match | Location | Why |
|-------|----------|-----|
| Enable-source evaluate whose input is `data_store_read` | `AlgebraicEvaluator.getBlockInputExpressions` | Case used stale Read signal (evaluated before mode DSW); live `data_stores.*` removes extra mode lag |

**Retirement:** Keep as **general** rule (any SwitchCase / enable selector fed
by a Data Store Read), or re-read DSM at end of algebra for those selectors.
Not Saturn-named today—OK to keep if documented as general.

### D. Terminal Chi Add12 / Add14 latch

| Match | Location | Why |
|-------|----------|-----|
| Flattened `…Iterative_Guidance_Mode_Add12` / `Add14` (not nested Position_Correction) | `AlgebraicEvaluator.wrapIgmTerminalChiLatch` | After Add8≤15, atan(vigained) still slews Chi; latch after first terminal major (gate1000 §1h) |

**Retirement:** Prefer correct terminal / memory / chi-dot hold semantics (or
MDL Unit Delay behavior) so vigained collapse cannot rewrite held angles—
then remove latch.

---

## What is *not* a name heuristic

- Fundamental **0.005 s** plant step (`simulationTimeStep` / ApolloA
  `MODEL_TIME_STEP_SEC`)—model property.
- Explicit MDL `SampleTime "1.6"` / `"0.04"` → `sampleTimeSec` via
  `withSampleTime`—property preservation.
- `delta_T_sec`, `T_HSL_sec` workspace → Obliq **parameters**—equation data.
- TM-X-62831 / TN presetting **value packs** and Constant `useParameter` emit
  (`mdl2obliq/src/workspaceConsts.ts`) — mission data, not timing heuristics.
  Mass ICs (`m_liftoff_kg`, `m_separation_kg`, Table 3B \(\dot m\) / \(M1\))
  belong in that pack; see `ts/as205TmX62831Presettings.ts`.
- saturn-ib-stack **rtw_adapter** OUT11/22 packing—integration profile, not
  block-name codegen forks (still Saturn-shaped; track separately if needed).

---

## Gate1000 snapshot (stock cgen after heuristics)

Approximate vs `/tmp/igm-rtw-1000.csv` (Phase A “close enough” bar):

| Check | Stock cgen (heuristics) | RTW |
|-------|-------------------------|-----|
| Mode ladder | ~777.6→1 … 892.8→4 (~1.6–3.2 s early) | 779.2 / 780.8 / 814.4 / 894.4 |
| Freeze Chi_Z | ~8.3° | ~9.42° |
| Coast \|Ve\| | ~7772 | ~7780 |

Surgical debug28 remains the golden bit-match; heuristics aim to make **stock**
JSON→cgen usable for further model work without hand-edited `.c`.

---

## Checklist before adding another name match

1. Can this be an explicit **`sampleTimeSec` / enable / hold property** on the
   block or subsystem instead?
2. Is there a **general** Simulink semantic (ActionPort, inherit resolve, DSM
   ordering) that fixes a class of models?
3. If a name match is unavoidable short-term: add a row to this doc + comment
   with retirement criterion + link to disparity §
