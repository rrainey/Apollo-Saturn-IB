# RTW vs Obliq codegen disparity catalog

Focus: **what differs in the two generated models / C**, not flight trajectory diagnosis.
Artifacts compared:

| Side | Path |
|------|------|
| RTW | `Saturn_IB_Stack_grt_malloc_rtw/Saturn_IB_Stack.c` (RTW output, not in this repository) (~13k LOC) |
| Obliq | `/tmp/mdl2obliq/cgen-fs3/saturn_ib_stack_translated.c` (~15k LOC) |
| Manifest | `/tmp/mdl2obliq/cgen-fs3/cgen-manifest.json` (198 warnings) |
| MDL | `model/provenance/saturn_ib_stack.mdl` |

**Bottom line:** 6DoF plant algebra (ω̇, q̄ on S-IB, quat normalize) is largely aligned. The large structural gaps are in **control/actuator TransferFcn parameter emission**, **wiring (multi-input / Goto)**, and **enable gating**. Those alone are enough for the stacks to diverge even when the EOM RHS matches.

---

## Matches (should be similar — and are)

| Area | RTW | Obliq | Notes |
|------|-----|-------|-------|
| S-IB ω̇ | `I` diagonal → `rt_MatDivRR_Dbl` of net torque | `inertia_diag_pack` → element-wise `1/I_i` | Equivalent for principal-axis I |
| Idot | zeros concatenated then selected | `Ground` → Idot slots | Same |
| Body accel Sum | `v×ω + F/m` | same Sum shape | Same |
| S-IB q̄ | `0.5 * V² * SFunction_o4(ρ)` | `0.5 * ρ * V * V` | Same |
| Quat normalize | `1.0 - \|q\|²` then Fcn qdot | Constant=1 − DotProduct | Same shape |
| H-1 mount clocking | — | `0/−90/−180/−270` after `mpr_deg` fix | Aligned with mask intent |
| Isolated EOM C↔TS | — | max\|Δ\| ~1e−19 | Plant core OK |

---

## Critical mismatches (code / model structure)

### 1. TransferFcn parameters dropped → wrong dynamics (highest impact)

**MDL** (`A Actuator`):

```text
BlockType    TransferFcn
Name         "A Actuator"
Denominator  "[0.00001942 0.0007963 0.05576 1]"
```

**Obliq JSON:** `parameters: {}`  
**cgen-manifest:** 64× `INVALID_TF_NUMERATOR` / `INVALID_TF_DENOMINATOR` across **32** TF blocks (all H-1 A/B actuators, FCC attitude/rate/accel filters, body-rate gyros, rate modulators, J-2 actuators).

**Generated C:**

| | RTW | Obliq |
|---|-----|-------|
| A Actuator order | **3** states (`AActuator_CSTATE[3]`) | **1** state |
| Poles / gain | e.g. `5.15e4`, `-41`, `-2.87e3`, … | fallback `ẋ = 1·u − 1·x` (τ≈1 s) |
| FCC Transfer Fcn3 | pole **6.313…**, out = `6.313·x` | same 1/(s+1) fallback |

**Root cause in translator:** `TransferFcn` is marked `MAPPED` in `coverage.ts` but has **no `case` in `mapper.ts`**. The default branch emits:

```ts
return { type: e.obliqType, name, parameters: {} }  // drops Numerator/Denominator
```

So Obliq C cannot resemble RTW for any path through actuators or FCC filters until TF params are mapped.

---

### 1e. Simulink `If` → Obliq ternary `if` — **fixed** (IGM mode handoff)

MDL `If` / If Action Subsystem (e.g. First Phase `If2`: `u1 <= delta_T_sec`) was coverage-mapped to Obliq ternary `if` (needs 3 data ports). Codegen emitted `// Error: If block requires 3 inputs` and never wrote the condition, so `Set nIGMMode to 1` never enabled → `nIGMMode` stuck at 0, T1 ran negative through 800s. Fix: map `If` → bool `evaluate` (`u1`→`in(0)`, workspace `delta_T_sec`→`1.6`); ActionPort `ifaction` already wires enable. After patch, mode 0→1→2 near t≈779–782 (RTW 779/781).

**Follow-on (also fixed surgically on debug28):** `Multiport Switch` on `(τ1,T1,τ3,T3)` used live `nIGMMode` DSM while ActionPort phase enables lag one major cycle → one hit selected a disabled phase `Mux2` (zeros) → `Ly`/`Add8`/`T3`/`Chi_Z` NaN. Select by which phase subsystem `enable_states` is active this step (same mux that was just computed). After both fixes: no NaNs through 800s; `T3@800≈106.8` vs RTW `106.7`.

**Also patched (debug28 C):** remaining IGM `If` actions for mode 2→3 (`P_C >= 33.4`), mode 3 terminal/`T3<=15`, mode 3→4 (`T3 <= T_HSL+ΔT_b`), and HSL-first-pass.

### 1g. T3 bias / late mode ladder — **fixed**

**Symptom:** Through mode 2, `|ΔT3|≲0.5 s`. At mode→3 entry Obliq was **+3.2 s late** (817.6 vs RTW 814.4) and then double-fast-decremented T3 for ~2 cycles → persistent **`ΔT3≈−3 s`**, so mode→4 fired early (892.8 vs 894.4).

**Root causes:**
1. `Switch_Case_case_*` used stale `Data_Store_Read2` (nIGMMode read **before** mode DSWs) → **extra** ActionPort enable lag on every mode change (mode→2 at 782.4 vs RTW 780.8).
2. Mode→3 / mode→4 `If` ActionPorts added another +1.6 s before the mode DSW.

**Fixes (debug28 C):**
1. `Switch_Case` evaluates against **live** `data_stores.nIGMMode` (and refreshes Read2).
2. Same-step `nIGMMode=3` when `P_C >= 33.4`; same-step `nIGMMode=4` (+ HSL arm schedule) when `T3 <= T_HSL+ΔT_b`.

**Result (gate1000_39):** mode ladder **779.2→1, 780.8→2, 814.4→3, 894.4→4** (matches RTW). Mode-3 `|ΔT3|≲0.25 s` after settle. 0 NAN lines.

### 1f. Late HSL cutoff arming — **fixed / fine-tuned** (Ve overshoot)

Mode→4 wrote `nIGMMode=4` only; `HSL_Cutoff_Timing` / engine shutdown needs `nHSLActive≠0`, which waited on lagged HSL-first-pass → **~6 s extra burn** → coast **~7893** vs RTW **~7781**.

**Fixes (debug28 C surgical):**
1. Schedule `nHSLActive=1` on mode→4 (same-step with §1g).
2. `HSL_Cutoff_Timing` Sum/Compare/Switch/DSW sample time **1.6→0.04 s**.
3. Gate `nHSLActive→2` on Compare (`Sum<=0.04`) so `ΔT_b` keeps subtracting each 0.04 s tick until cutoff (ungated write dropped `ΔT_b` after one tick → slow T3 countdown).
4. Defer arm **~0.25 s** after mode→4 (with §1g timing aligned).

**Probe42:** mode→4@894.4 → arm@894.7 → cut@895.0–895.1 → plateau **`|Ve|≈7781`** (RTW **7780.3**).

### 1h. Chi_Z freeze value — **fixed / fine-tuned**

**Symptom:** After cutoff Obliq held `Chi_Z≈−0.99°` (earlier runs ~7.7°) vs RTW freeze **`Chi_Z≈9.42°`**.

**Root causes:**
1. `IGM_Chi_Steering` enable (`epsilon_prime = Demux4_3>3`) lags one major cycle → the T3-cross-below-3 hit still **wrote** Chi from a collapsing `v_cmd` (`vigained_y`→negative) → snap to **−0.99°**.
2. In terminal (`Add8≤15`) Obliq zeroed chi-**dot** corrections but still recomputed `Add12/Add14 = atan(vigained)…`, so as `vigained_y` collapsed Chi_Z dove **9.5→5.7°** before epsilon freeze. RTW keeps Chi near **9.4–10.5°** through that window.

**Fixes (debug28 C):**
1. Same-step gate Chi_Y/Z DSM writes on live `Demux4_3>3` (prevents −0.99 snap).
2. After the **first** terminal major (allows the Chi step-up at `T3≲15`), **latch `Add12`/`Add14`** for subsequent terminal hits so atan(vigained) cannot slew the command attitude.

**Result (gate1000_42 / chi_freeze_42):** freeze **`Chi_Z≈8.89°`** (RTW **9.42°**, **Δ≈−0.5°**). Was −10° with the snap. Residual ~0.5° from SMCZ/AP drift on held angles + remaining vigained/XS/VS divergence. Coast `|Ve|` unchanged ~7781.

### 1d. Gain `"pi"` (SMCY/SMCZ `pirads to radians`) — **fixed** (Chi_Z bias)

MDL Gain blocks `pirads to radians` use `Gain = "pi"`. Mapper treated unresolved `Number("pi")` as NaN → **default gain 1**, so DSM `SMCY_rad` / `SMCZ_rad` were **π× too small**. Chi_Z = deg(atan(v_y/…) + SMCZ) therefore ran ~0.3–0.6° low by t=700 while raw atan tracked. Fix: `resolveWorkspaceNumber` on Gain (same as Saturate/`pi/4`). After fix, |ΔChi_Z| ≲ 0.1° through 700s.

### 1b. Quantizer / Saturate `pi/4` — **fixed** (post–J-2 yaw)

Same empty-generic-`MAPPED` failure mode as TransferFcn:

| Block | MDL | Was (Obliq) | Now |
|-------|-----|-------------|-----|
| Fine Resolver Quantization | `QuantizationInterval 0.00279` (~10″) | `quantum=1` (°!) | `0.00279` |
| D/A Converter Quantization | `0.0575` | `quantum=1` | `0.0575` |
| Accelerometer Quantization | `0.05` | `quantum=1` | `0.05` |
| ST-124 `+/- 45.0 deg` | `±pi/4.0` (rad) | `±1` (fallback) | `±π/4` |

**Flight impact:** 1° Fine Resolver stairs put ~1° yaw into Memory with `Chi_Z=0` → FCC err≈1 → gimY≈0.6° at J-2 light → huge Mz. After fix (`/tmp/mdl2obliq-stack-debug25`), FineRes Z≈0.05–0.14°, initial gimY≈0.03°, |pqr| at light ≪ prior runaway. Residual under-thrust yaw oscillation still under investigation (β_Y / Mz polarity or damping).

**Fix:** `case 'Quantizer'` in `mapper.ts`; `numParamAllowInf` + `resolveWorkspaceNumber` evaluate `pi/4.0`. Tests: `__tests__/mdl2obliq-quantizer.test.ts`.

---

### 1c. TransferFcn controllable form used only `numerator[0]` — **fixed** (rate damping)

Even with Numerator/Denominator mapped, codegen realized

```text
ẋ_{n-1} = (num[0]/den[0])·u − Σ (a_k/den[0]) x_k ,   y = x[0]
```

That is correct only for **constant** numerators. S-IVB pitch/yaw **rate** filters have

```text
num = [1.4e-4, 4e-4, 1]   →  broken DC = num[0] ≈ 1.4e-4
```

so `rateY ≈ 0` while body `R` was nonzero → undamped attitude loop under J-2 thrust (growing yaw oscillation after quantizer fix).

**Fix:** controllable canonical form with `B=[0…1]`, `y = C·x` (`C_k = b_k / a_n`). Tests: `__tests__/transfer-function-block.test.ts`. Probe `/tmp/j2_att_probe26.out`: `|pqr|~1e-3`, `thZ` bounded ~0.3°.

---

### 2. MULTIPLE_INPUT_CONNECTIONS (wiring fan-in) — **fixed**

**Was:** 52× fan-in errors (timers, Chi, IGM, …).

**Cause:** MDL `DstPort "trigger"` / `"enable"` was parsed as `Number(...)→NaN→1`, colliding with data Inport 1 (e.g. both `bStart→trigger` and `Clock→1` on Timer Initialization).

**Fix:** `parseMdl` keeps `dstSpecial`; `emitObliq` routes those wires to enable pin `targetPortIndex: -1`. `EnableEvaluator` uses `flattenedName` for enable source exprs.

**Now (cgen-fs5):** `MULTIPLE_INPUT` = **0**. Tests: `__tests__/mdl2obliq-trigger-enable.test.ts`.

---

### 3. Goto/From (sheet_label) broken

**22** connections reference non-existent `sheet_label_source_*` / `sheet_label_sink_*` (cross-sheet tags between S-IB, S-IVB, IU/LVDC). Those wires are missing or dangling in Obliq vs RTW’s resolved Goto/From network.

---

### 4. Enabled subsystems: enable pin unwired

**27** unique enabled subsystems report *“has enable input but no enable wire connected”* (listed twice in manifest → 54). Includes:

- `S_IB_Stage`, `S_IVB_Stage`  
- All H-1 Triggered Timer inits, retrorockets  
- IGM, IGM Chi Steering, SMCY/SMCZ, Chi Time-Tilt store  
- T1–T4 Timer Initialization  

Obliq init sets `enable_states.* = 1` and leaves them on. RTW gates derivatives to zero when a stage/subsystem is disabled (`/* zero derivatives to prevent integration while disabled */`). Structural difference: both stages / IGM / timer inits can run concurrently in Obliq.

---

### 5. FCC gain lookup tables not strictly increasing

**6** `LOOKUP_NOT_MONOTONIC` warnings: `a0_deg_deg`, `a0_roll`, `a1_*`, S-IV variants. RTW interpolates the same breakpoints; Obliq codegen rejects or mishandles non-monotonic axes → gain schedule path differs.

---

### 6. Multi-rate vs single-rate — **addressed (2026-08-25)**

RTW sample hits: **0.005 s**, **0.04 s**, **0.8 s**, **1.6 s** (`MdlInitializeSampleTimes`).  
Fundamental step remains **0.005 s** (ApolloA `MODEL_TIME_STEP_SEC`, Obliq `simulationTimeStep` / `--dt`).

Obliq now:
- Parses MDL `SampleTime` / `SystemSampleTime` → `parameters.sampleTimeSec`
- Flattener inherits subsystem sample scopes (child override wins)
- Algebra gates with `sample_tick % round(period/dt) == 0` (AND enable)
- Defaults IGM / S-frame / Reciprocal Acceleration parents to **1.6 s** when MDL only stamped ports (RTW tid 4)

Probe (`/tmp/mdl2obliq-stack-debug15`): after IGM enable, `T_1_i` holds then `−1.6` on the next absolute 1.6 s tick (not every 0.005 s). First major-cycle `T_3_i` write can still NaN (Product13 / S-frame inputs — separate from cadence).

---

## Explicitly *not* the main plant gap

Earlier suspicion that RTW `rt_MatDivRR_Dbl` vs Obliq `1/I_i` was a plant bug is **not** a material mismatch here: RTW builds a **diagonal** I (`Ixx,0,0 / 0,Iyy,0 / 0,0,Izz`) then MatDiv; Obliq packs the same diagonals. Isolated EOM residual already PASSed.

S-IVB dynamic pressure → `Ground2` (0) in Obliq matches RTW’s S-IVB `urhoV2 = V² * 0.0 * 0.5` during S-IB flight.

---

## Priority fix order (translator / model graph)

1. **`TransferFcn` Num/Den mapping** — **Done**.  
2. **`MULTIPLE_INPUT` / DstPort `trigger`\|`enable`** — **Done**.  
3. **UI validation parity (2026-08-20)** — **Done for the reported 25**:
   - Hierarchical sheet labels (nested global Goto)
   - `RelationalOperator`/`Logic` → `bool`
   - Mux vector→scalar demux expand (Out11 → `double[25]`)
   - Enable-pin bool cast on trigger lines
   - Empty subsystem `outputPorts` (no phantom `y`)
   - `mdl:emit` runs `validateModelTypeCompatibilityMultiSheet` and **exits 1** on errors (`--skip-validate` escape)
4. **Remaining Obliq validation (~40)** — Demux1–5 scalar inputs + Product/Sum type prop inside engines/FCC (not the original From/Mux/enable set).  
5. **FCC lookup monotonicity** — 6× `LOOKUP_NOT_MONOTONIC` (codegen warnings).  
6. **Optional:** multi-rate sample hits.

Import latest: `/tmp/mdl2obliq/saturn_ib_stack_translated.json`
