# AS-205 validation baseline

## Primary reference (current)

**Document:** NASA-TM-X-62831 — *AS-205/CSM-101 Launch Vehicle Operational Trajectory, Revision I* (Solmon, Sept 1968)  
**Markdown extract:** `reference/NASA-TM-X-62831/chandra/NASA-TM-X-62831.md`

This is the **flown Apollo 7 / AS-205 operational** reference. Full-stack Obliq work targets this document for guidance presettings, mass properties, and trajectory residual.

| Layer | Source of truth |
|-------|-----------------|
| **Trajectory residual (h, \(V\), γ, events, …)** | **TM-X-62831** tables / App C |
| **Site / guidance / IGM / mass presettings** | **TM-X-62831** App B Table 3B + Table 12 (via Obliq **model parameters**) |
| **`LaunchDate` / epoch → GMST / `[MES]`** | **Actual Apollo 7 liftoff** (1968-10-11) |

Typed inventory + cites: [`../ts/as205TmX62831Presettings.ts`](../ts/as205TmX62831Presettings.ts).  
mdl2obliq default profile: **`tm-x-62831`** (`mdl2obliq/src/workspaceConsts.ts`).

### Historical: TN-AP-67-158 (never-flown)

**Document:** Chrysler TN-AP-67-158 — *AS-205 Revised Launch Vehicle Reference Trajectory* (1967)  
**Public PDF (example):** [ibiblio](https://www.ibiblio.org/apollo/Documents/CHRYSLER-TN-AP-67-158%20-%20AS-205%20-%20Revised%20Launch%20Vehicle%20Reference%20Trajectory.pdf)

| Role | Notes |
|------|--------|
| **Variable name list** | `lib_SaturnIB/model/AS205_presettings.m` — schema only; values are TN-era |
| **Simulink / RTW stack as built** | Loaded TN `.m` before compile |
| **Plant Tier-A residual** | May still use TN tables until retargeted |
| **mdl2obliq profile** | `tn-ap-67-158` for dual compares |

Do **not** “fix” the Obliq full stack to match TN-shaped RTW once TM parameters are active. When TM and Simulink (TN) disagree, prefer **TM-X-62831** for ops fidelity.

## Secondary references (use carefully)

| Source | Role |
|--------|------|
| TM-X-62831 tables / App B / Table 12 | **Primary** — trajectory, IGM, mass breakdown |
| `AS205_presettings.m` | **Names** for presettings (not numeric truth) |
| `saturn_ib_stack.mdl` | Structure; Constant Values often TN literals until retranslate |
| COESA atmosphere in obliq | Table approximation; TM cites 1963 Patrick atmosphere |

**Mass / propellant:** TM Table 12 (lbm breakdown) and Table 3B (`M1`, `M03`, \(\dot m\), …) differ from baked Obliq `m_liftoff_kg` / `m_separation_kg`. Prefer model parameters (`m_liftoff_kg`, `m_separation_kg`, `Mdot_*`, …) over sheet literals — see presettings TS + convert `useParameter` path.

## Frame correspondence (EDD ↔ TN) — working assumption

| TN wording (typical) | EDD / IU system | Notes |
|----------------------|-----------------|--------|
| **Space frame** / space-fixed trajectory state | **S (plumbline)** | Space-fixed at \(T_{\mathrm{GRR}}\) ⇒ non-rotating ⇒ **inertial** in EDD language; site-defined axes (up + downrange) |
| (Classical) celestial inertial / equinox | **E** | Apollo Std 4; Simulink ECI world — **not** current identity for TN “Space frame” |
| Vehicle body | **B** | \(X_B\) forward/thrust, pitch about \(Y_B\) |

No explicit TN↔EDD call-out found yet; this is a **belief/working map**. Revisit if TN text equates Space frame to equinox/E.

**ECI→S:** Fixed after launch epoch / \(T_{\mathrm{GRR}}\) (defines \(X_S,Z_S\) in E). Needed for S-component residuals and SM outputs; not required for scalar validation below.

Full discussion: [`APOLLO_COORDINATE_FRAMES.md`](./APOLLO_COORDINATE_FRAMES.md).

## Comparable quantities

### Use **now** (do not require S-frame components)

| Quantity | TN role | Model signal | Notes |
|----------|---------|--------------|--------|
| Time from liftoff | Independent var | logger time − \(t_{\mathrm{liftoff}}\) | |
| Altitude | Trajectory table | `altitude_m` / \(\|r\|-R_{\mathrm{pad}}\) | Scalar |
| Mass | Propellant table | `mass_kg` | Scalar |
| Dynamic pressure \(q̄\) | Loads | `qbar_Pa` | Model-aero dependent |
| Thrust / \(a_x\) (if digitized) | Performance | `thrust_N` | Scalar |

### Defer until ECI→S / SM path matches Simulink

| Quantity | Why deferred |
|----------|----------------|
| Space-fixed velocity magnitude & path angle (TN “space-fixed”) | Expressed in **Space/S** frame |
| Position components \(X,Y,Z\) (plumbline / space listing) | S-basis components |
| Full \(\mathbf{v}\) residual in E or S | Needs consistent transform |

**Units:** Prefer SI in CSV (m, m/s, kg, Pa, rad). Convert English units from the TN once when building the reference CSV and record the conversion in the file header comment.

**Simulink:** Continue translating the original model (ECI primary, Body/SM as in EDD). Prefer TN over Simulink when they disagree; do not retune plant to Simulink without a decision.

## Reference CSV layout

Place extracted tables under:

```text
docs/sample-models/saturn/as205-reference/
```

See that folder’s `README.md` and `as205_trajectory_template.csv`.

### Column contract (v1)

| Column | Unit | Required |
|--------|------|----------|
| `t_s` | s from liftoff | yes |
| `h_m` | m MSL (or note geometric) | yes for alt compare |
| `v_mps` | m/s | recommended |
| `gamma_rad` | rad | optional |
| `mass_kg` | kg | optional |
| `qbar_Pa` | Pa | optional |
| `source_note` | text | optional (table ID / page) |

## Comparison workflow

1. Run **9.4** (or 9.3) with Signal Loggers on altitude, mass, \(q̄\) (and optionally \(V\)).  
2. In the app, export **all logged data** CSV (multi-column: `time`, `log_altitude`, …).  
   Single-logger `time,value` files are not enough for multi-field residuals.  
3. Align sim time to **liftoff**: 9.x models use a liftoff step at \(t\approx 1\,\mathrm{s}\); residual CLI defaults `--offset 1`.  
4. Run the residual CLI:

```bash
# Dry-run with synthetic logger-shaped CSV (not flight data):
npm run as205:compare -- \
  --model docs/sample-models/saturn/as205-reference/example_model_logger_export.csv \
  --offset 1 --tmin 0 --tmax 150 --tol 2 \
  --fields h_m,mass_kg,qbar_Pa \
  --out /tmp/as205-residual.md

# Real 9.4 export:
npm run as205:compare -- \
  --model path/to/saturn-9.4_data.csv \
  --offset 1 --out residual-report.md
```

Programmatic API: `examples/saturn-ib/as205Compare.ts`  
(`loadTrajectoryCsv`, `compareTrajectories`, `compareCsvTexts`, `formatCompareReport`).

5. Interpret residuals: max |Δh|, |Δm|, RMS over a stated window (e.g. 0–150 s S-IB).  
   Soft flags in the report are **diagnostic only** — not pass/fail gates for 9.x.

## Acceptance policy (current phase)

| Phase | Expectation |
|-------|-------------|
| 9.1 / 9.2 plant / rate loop | **Qualitative** agreement only (shape of h(t), mass drop). |
| 9.3 + Table 5 CSV | Residual **reports** enabled; still no hard pass/fail gates. Prefer h, mass first. |
| After aero tables + mass props + χ program | Quantitative windows vs TN (to be defined). |
| Full IGM stack | Guidance presettings from TN + trajectory tables. |

**Frame policy (current):** Treat TN **Space frame** ≈ EDD **S**. Validate with **\(h\), mass, \(q̄\)** only until ECI→S (or native S outputs from the Simulink translation) exist. Do not treat space-fixed \(V\)/γ/XYZ residuals as plant failures yet.

## Digitized tables (repo)

| File | Content |
|------|---------|
| `as205_trajectory_reference.csv` | **Table 5 S-IB** (t=0…147.26 s), SI, ~5 s steps + event times |
| `as205_trajectory_template.csv` | Column contract |
| `as205_trajectory_smoke.csv` | Synthetic unit-test series only |

S-IVB Table 6 and Appendix C English listings are **not** fully digitized yet.

### Dynamic pressure unit note

TN Table 5 column header is **DYNAMIC PRESSURE (KG/M2)**. Values at max-q (~3254) are treated as **kgf/m²** and converted with \(q_{\mathrm{Pa}} = q_{\mathrm{TN}}\times 9.80665\) (~31.9 kPa ≈ 666 psf). If a later audit shows the TN intended N/m² already, re-scale the CSV and re-run residuals — do not retune the plant to a wrong unit.

## Extracting more tables from the PDF

The TN is multi-page with trajectory listings. Recommended process:

1. Download TN-AP-67-158 from a trusted host (ibiblio link above or NASA archive mirrors).  
2. Identify the **revised reference trajectory** time history tables (not only summary plots).  
3. Digitize selected rows (or full table) into `as205_trajectory_reference.csv`.  
4. Record **page numbers** and **column definitions** in `source_note` / README.  
5. Never commit OCR garbage without a human spot-check against the PDF (mass continuity \(\dot m\sim 2.7\,\mathrm{t/s}\) is a good check).

## Related models

| Model | Use for |
|-------|---------|
| `saturn-9.1-open-loop-6dof-ascent` | h, mass, q̄, \|r\| vs time (plant, no aero force) |
| `saturn-9.2-closed-loop-pitch-rate-damp` | rate loop only — **not** TN trajectory match |
| `saturn-9.3-open-loop-6dof-ascent-aero` | plant + constant-CdA drag (no pitch program) |
| `saturn-9.4-open-loop-chi-6dof-ascent` | TN-class plant + simplified χ elev program |
| `saturn-9.5-open-loop-chi-table2b-ascent` | Table 2B elev + rate-only My |
| `saturn-9.6-chi-table2b-attitude-pd` | **primary residual candidate**: Table 2B elev + body-pitch PD |
| `as205-reference/as205_table2b_chi.csv` | Digitized Table 2B (χ_c and elev) |
| `saturn-8.8-chi-time-tilt` | χ program shape alone (no plant) |
| Future 9.7+ | Platform/quat attitude (explicit frame decision); residual pass/fail gates |

### Table 2B pitch convention (practical)

- TN **χ_c**: deg from **inertial vertical**, **negative downrange** (0 at pad vertical).  
- Plant elev used for open-loop \(Q_{\mathrm{cmd}}\): **elev = 90 + χ_c**.  
- Full platform / IGM frame transforms are **not** implemented here.

### Frames (user stack vs 9.x plant)

| Frame | User / standard | Apollo IU (satinstunitibm §2) | 9.x plant today |
|-------|-----------------|------------------------------|-----------------|
| **Body** | AIAA: \(+X\) thrust, \(+Y\) pitch up, \(+Z\) yaw | **B-system** \(X_B\) forward, pitch about \(Y_B\) | \(F_b=[T,0,0]\), \(M_y\), \(Q=\omega_y\) |
| **TN Space frame** | Space-fixed state / path angle | **S (plumbline)** — **working assumption** | Partial pad S-like IC (`as205PadFrames.ts`) |
| **Classical ECI** | Equinox / north | **E** (Apollo Std 4); Simulink ECI | Not primary for TN Space listings |
| **Body** | AIAA +X thrust | **B** | \(F_b=[T,0,0]\), \(M_y\) |
| **ECI → S** | Fixed after \(T_{\mathrm{GRR}}\) | `[MES]` / Simulink ECItoSM | **In plant** (`MES_E_to_S`, `log_X/Y/Z_S`); dynamics in E |

Full write-up: [`APOLLO_COORDINATE_FRAMES.md`](./APOLLO_COORDINATE_FRAMES.md).
