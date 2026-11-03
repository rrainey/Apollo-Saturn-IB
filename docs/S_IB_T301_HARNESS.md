# S-IB Stage t=301 debug harness

**File:** `examples/saturn-ib/s_ib_stage_t301_harness.json`

Re-emitted after adding Obliq **`square` (x²)** (element-wise) and fixing **Sum of Elements** vector collapse (plus suppressing the spurious 1-of-2 port warning).

## Feasibility

**Yes — practical.** The exact mdl2obliq `S-IB Stage` emit already existed (`s_ib_stage_mdl2obliq.json`, ~778 nested blocks). This harness is that model with:

1. **Boundary ICs** as named `IC_*` Constant sources (former input ports)
2. **`beta_R/P/Y = 0`**
3. **Named `Scope_<outport>`** (`signal_display`) on every stage output
4. **`p_q_r` integrator** seeded with RTW ω_b @ t=301

Import into the Obliq UI and run; scopes collect the 14 stage outputs.

## What “t=301” means here

| Item | Choice |
|------|--------|
| Reference | RTW `rtw-sib-320.csv` @ mission **t=301** (`T_L=300`) |
| Harness clock | Restarts at **0 ≡ mission 301** |
| `T1_sec` | **-1** so burn elapsed ≈ 1 s at start |
| `theta_GMST_rad` | **Frozen** at GMST(301) ≈ `4.292722` rad |
| `bEngineStart` | `ones(8)` |
| `bIECO` / `bOECO` / `bStageSep` | 0 |
| Duration / dt | **20 s** / **0.01 s** (edit in model settings) |

Seeded kinematics (RTW s1 @ 301):

- `Xe_0_m` ≈ `(-5.39487e6, 1.53267e6, 3.02747e6)`
- `q_ECI_0` ≈ `(-0.273462, -0.0533877, 0.75725, -0.590712)`
- `Vb_0_m` ≈ `(1.40786, -71.4753, 402.628)`
- `ω_b` (pqr IC) ≈ `(8.28e-4, -3.20e-3, -4.43e-4)`

## Scopes (14)

`Scope_lat_deg`, `Scope_lon_deg`, `Scope_h_m`, `Scope_Xe_m`, `Scope_Ve_mps`, `Scope_alpha_deg`, `Scope_Vb_mps`, `Scope_q_ECI`, `Scope_omega_b_rps`, `Scope_Ab_mps2`, `Scope_bPropellant_sensor_dry`, `Scope_B_b`, `Scope_CG_m`, `Scope_Vb_vec_mps`

## Honest gaps (still useful for hunting NaNs)

Not a perfect full-state snapshot:

| Internal state | Harness behavior |
|----------------|------------------|
| Propellant / mass | Starts at **m_liftoff** (not mass after 1 s burn; ~0.5% light) |
| H-1 actuator TFs | Zero state (OK with β=0) |
| Engine timers | Approximated via `T1_sec=-1` + `bEngineStart` |
| Live GMST | Frozen (fine for ~20 s) |
| IU / guidance | **Removed** — betas forced 0 on purpose |

If this open-loop S-IB still NaNs within ~20 s, the bug is inside the stage (EOM / aero / mass / engines). If it stays healthy, the stack problem is upstream (enable, betas, IC handoff, OUT11 pack).

## How to use

1. Open Obliq-2 → import / open `s_ib_stage_t301_harness.json`
2. Confirm left-side `IC_*` constants (betas should be 0)
3. In **Simulation Settings**, enable **Debug math (abort on /0, NaN)** (persisted as `globalSettings.debugMath`; already on in this harness JSON)
4. Check **Force Recompile**, then run (20 s default) — on bad math you should see `OBLIQ_DEBUG_MATH … block="…"` instead of a vague remainder/NaN failure
5. Inspect `Scope_*` plots — start with `Scope_Xe_m`, `Scope_Ve_mps`, `Scope_q_ECI`, `Scope_h_m`, `Scope_alpha_deg`

### Batch equivalent

```bash
npx tsx scripts/obliq-cgen.ts examples/saturn-ib/s_ib_stage_t301_harness.json \
  --out /tmp/sib-t301-debug --debug-math --compile
```

## Regenerate

The generator was an inline script in the session that built this file from `s_ib_stage_mdl2obliq.json`. Re-run that approach after re-emitting S-IB if the stage JSON changes.
