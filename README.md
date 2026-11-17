# Apollo-Saturn-IB

## What is this project?

This is an engineering-grade simulation of the Apollo-7 (AS-205) Saturn IB launch.

This simulation includes launch vehicle modeling and simulations of both the Saturn Instrument Unit and elements of the CSM Apollo Guidance Computer. The vehicle dynamics and Launch Vehicle Digital Computer guidance are C code generated from an [Obliq-2](https://github.com/rrainey/obliq-2) model.

The Saturn launch vehicle modeling is based almost entirely on Apollo-era NASA and IBM engineering references.

This repository ships the plant and the CMC bridge as two static libraries (`saturnib`, `saturnib_cmc`) plus one executable:

- **`batch_sim`** runs headless from a JSON file as fast as the CPU allows (about 150× real time), and writes a final-state JSON and a 1 Hz CSV. Runs are deterministic. No AGC in the loop.

I also constructed a man-in-the-loop simulation that adds the Apollo Guidance Computer and Command Module visuals.  That simulation adds a DSKY and FDAI panel. It has moved to the [DSKY-matic](https://github.com/rrainey/DSKY-matic) project, where it links against the libraries built here and runs as `dskymatic-studio` (desktop) or `dskymatic-headless` (Raspberry Pi hardware). See the DSKY-matic README for the interactive run and launch choreography.


## Why did I build this simulation?

I was a child of Apollo. I was an elementary school student in the era of Gemini and Apollo. I have always been fascinated by spaceflight.

In the late 90's and early 2000's, I worked as a flight and space simulation consultant.  For several years in that time, I worked full-time for a boutique DoD contractor, primarily engaged in building flight and space simulators for training and feasibility studies. That work frequently involved referring back to NASA/NACA papers posted on the NASA Technical Reports Server ([NTRS](https://ntrs.nasa.gov)).  Through that work, I was impressed with the detailed information available on-line about the Apollo program.  I set out to see if I could create an engineering-grade simulation solely from historical documents (the NTRS-kind, not the ["Galaxy Quest"](https://www.imdb.com/title/tt2570364/) variety).

The earliest versions of the simulation were Simulink-based, as I had access to that excellent application at work. That version was detailed but incomplete and buggy. I put it down for many years.

Earlier this year (2026), I resurrected my old work and used Claude Opus to convert my original model to [`obliq-2`](https://github.com/rrainey) format.  With that running, I then leveraged Claude again to do a detailed comparison with the original NASA-published simulation results prepared for Apollo 7.  Claude Opus did an excellent job and, with some guidance from me, was able to debug the model to obtain excellent agreement with the NASA launch prediction model.

The project is a curiosity mostly.  But it does demonstrate the excellent level of historical detail published by NASA and its contractors around the Apollo Program.  So, this is really about those men and women and the incredible feat that they pulled off, "for all mankind."

**Runtime assets** needed only by the interactive panel (AGC ROM `Colossus249.bin`, PAD-load text files, downlink-spec TSVs, panel scene JSON, textures, and the stylized-Earth GSHHG binaries) now live in `DSKY-matic/data/` rather than here. Only `batch_sim`'s scenario files (`data/examples/`) remain in this repo. The legacy `apolloa` target is still present under `-DBUILD_GUI=ON` for back-compat but is otherwise deprecated — it requires those data files to be provided at the runtime CWD.

## Simulation Fidelity

The simulation is validated against the pre-flight operational trajectory in NASA TM-X-62831, *AS-205/CSM-101 Launch Vehicle Operational Trajectory, Revision 1*. The LVDC guidance implementation integrates sensor data (ST-124M stabilized platform PIPAs) per EDD §4.3.1 Eqs. 4.3.1-5; no plant state vectors are read directly by the LVDC side. The plumbline navigation frame (PASCs 13 per NASA MSC MA-1461) is anchored at nominal Guidance Reference Release (T_L' − 17 s) and the LVDC X_0 / V_0 initial state is constructed from EDD Eqs. 3.4.3 / 3.4.4 directly, so the frame is consistent through the plant's state integrator, the ST-124M platform, and the LVDC's S-frame integration.

### Dominant-signal fidelity: PASCs 13 state vector at S-IVB cutoff

The sharpest fidelity check against TM-X-62831 is the Table 4 PASCs 13 position/velocity columns, which pin direction as well as magnitude. Sample taken at the first BML boundary after Table 4's flight time (614.63 s): the probe fires at sim time 915.20 s (flight time 615.20 s), i.e. 0.57 s past the reference instant — most of the residual below is pure dead-reckoning at ~7.4 km/s over that interval.

| Component (PASCs 13) | TM-X-62831 Table 4 | Simulation | Residual |
|---|---:|---:|---:|
| X_S | 6,258,278 m | 6,257,175 m | −1,103 m |
| Y_S | 144,698 m | 144,910 m | +212 m |
| Z_S | 2,092,098 m | 2,095,810 m | +3,712 m |
| DX | −2,472.80 m/s | −2,474.88 m/s | −2.08 m/s |
| DY | 412.48 m/s | 412.31 m/s | −0.17 m/s |
| DZ | 7,365.73 m/s | 7,364.41 m/s | −1.32 m/s |
| \|V\| | 7,780.67 m/s | 7,780.08 m/s | −0.59 m/s |

### Scalar cutoff summary

The scalar cutoff magnitude row is useful at a glance but does not detect frame rotation. It is retained for continuity:

| Metric at S-IVB cutoff | TM-X-62831 target | Simulation | Residual |
|---|---|---|---|
| Space-fixed velocity (plant \|V_e\| peak) | 7780.67 m/s | 7773.07 m/s | −7.60 m/s |
| Altitude (Fischer geocentric-radial) | 227.94 km | 234.42 km | +6.48 km |
| HSL cutoff decision engages (post-liftoff) | n/a | 608.80 s | — |

For context, the operational Saturn IB program expected a nominal ±3σ cutoff velocity dispersion on the order of a few m/s. The −7.60 m/s plant \|V_e\| peak residual is just outside that envelope, but note from the PASCs 13 row above that the LVDC's own \|V_S\| at Table 4's reference instant is within −0.59 m/s of V_T — the HSL cutoff decision fires at essentially the right moment internally. The remaining gap between the LVDC's \|V_S\| view (7780.08) and the plant's \|V_e\| peak (7773.07) is tracked as a separate investigation.

### Mid-flight fit

The cutoff residuals are closed-loop (driven to V_T by the Iterative Guidance Mode) and insensitive to mid-flight trajectory errors. Measured velocity residuals vs TM-X-62831 Table 5 at key events (plant \|V_e\|):

| Event | Flight time | TM \|V\| | Sim \|V\| | Residual |
|---|---|---|---|---|
| Max Q | 75.0 s | 742.13 m/s | 750.20 m/s | +8.1 m/s |
| IECO | 140.11 s | 2267.77 m/s | 2274.6 m/s | +6.9 m/s |
| OECO | 143.11 s | 2325.01 m/s | 2342.2 m/s | +17.2 m/s |
| IGM initiation | 168.25 s | 2377.35 m/s | 2388.2 m/s | +10.8 m/s |

The current mid-flight shape reflects:

- Axial-force coefficient `CA_T` rebased to Langley 1965 wind-tunnel data (NASA TN D-2829) for Mach 0–1.25; see `memory/project_cat_from_tnd2829.md`.
- Supersonic CA_T tail (Mach 1.5 – 6) currently carries an empirical bump at Mach 3.5 (×1.15) and Mach 6 (×1.30) as a working approximation — the authoritative references (NASA TM-X-53423, TMX-53657, R-AERO-AD-68-25) are not in the repo. See the open residuals section of the CA_T memory note for revisit conditions.
- J-2 engine Isp transition from 423.28 s (high thrust) to 431.67 s (low thrust) per TM-X-62831 Table 11.
- `(M/F)_S` FIR filter history and `Rate_Limiter_0_005` state are seeded to the current raw 1/|a| sample on IGM enable rising edge. EDD §4.2.11 is a pure FIR with no slew cap; the modeled rate limiter is a Simulink artifact whose cold-start lag (~5 BMLs to catch up from the S-IB→S-IVB staging coast step) was producing an 8° Chi-Y pitch-up transient on the first IGM BMLs. Seeding on IGM enable removes that transient.

The guidance loop runs on the Equation Defining Document cadences: the Boost Major Loop (BML) at 1.6 s and the HSL minor loop at 40 ms. The HSL cutoff decision uses the Eq. 4.4.91-94 velocity polynomial with a 3-sample V-history refreshed on BML boundaries; the ΔV_b = 7.2381 m/s and V_T = 7780.67 m/s presettings come from EDD Part II Table 1-3.

**Altitude convention.** The altitude rows above use Fischer-1960 geocentric-radial altitude (from the `s2_h_fischer_m` CSV column), matching TM-X-62831's own altitude definition. Native `OUT11[2]`, `OUT12[2]`, `OUT22[2]` continue to carry WGS-84 geodetic altitude for the GUI and other consumers. Fischer and WGS-84 differ by ~30 m on orbit; the +6 km altitude residual above is overwhelmingly real trajectory error, not ellipsoid-model choice. See [`docs/ALTITUDE_CONVENTIONS.md`](docs/ALTITUDE_CONVENTIONS.md).

**Frame-anchor history.** Prior to the Issue #4 resolution the plant's `MES_Transform` was anchored at the scenario sim-epoch (= liftoff − `T_L_prime_sec`) rather than at nominal GRR. That produced a 1.25° orthogonal rotation (ω_E · T_L' at R≈6,600 km → ≈115 km) in the PASCs 13 state vector, which went undetected by the scalar `|V|` cutoff summary because rotating a velocity vector preserves its magnitude. See [`docs/FRAME_ANCHOR_AUDIT.md`](docs/FRAME_ANCHOR_AUDIT.md) and [`docs/FRAME_ANCHOR_BRANCH_NOTES.md`](docs/FRAME_ANCHOR_BRANCH_NOTES.md) for the investigation trace.

## Build

You need CMake 3.16+, a C++17 compiler, git, and network access for the first configure.

```bash
cmake -S . -B build
cmake --build build -j
```

The first configure clones Virtual AGC at a pinned commit into `third_party/virtualagc/` and applies the patches in `third_party/virtualagc-patches/`.

(The legacy `apolloa` GUI target is still present under `BUILD_GUI=ON` for compatibility and requires OpenGL + GLUT: `apt install freeglut3-dev` on Ubuntu. New development happens in DSKY-matic; pass `-DBUILD_GUI=OFF` here when building just the library + `batch_sim`.)

## Run the batch simulation

```bash
cd data

# Headless: TM-X-62831 launch profile, through insertion
../build/batch_sim examples/AS-205-tm-x-62831.json --run-time 960 --final final.json --csv results.csv
```

See [docs/batch_sim.md](docs/batch_sim.md) and [docs/apolloa_gui.md](docs/apolloa_gui.md).

## Test

```bash
ctest --test-dir build --output-on-failure
```

`tests/gui/run_scenario.py` runs a scripted launch through the panel under Xvfb and takes about 8 minutes. Build its helper first with `cmake --build build --target xclick`.

## Architecture at a glance

The engine is split into two static libraries so that the launch-vehicle
simulation is usable without a CMC in the loop:

```
  ┌───────────────────────────────────────────────────────────────┐
  │                   saturnib  (no yaAGC)                         │
  │                                                                │
  │  ┌────────────────────┐   ┌───────────────────────────────┐   │
  │  │  Obliq-2 plant     │   │  LVDC guidance (hand-edited)  │   │
  │  │  (vehicle dyn,     │◄──┤  lib_SaturnIBObliq/           │   │
  │  │   S-IB & S-IVB     │   │  native-snapshots/lvda-lvdc/  │   │
  │  │   stages, aero,    │   │  LVDA_LVDC.c                  │   │
  │  │   engines)         │   └───────────────────────────────┘   │
  │  │                    │                                       │
  │  │ lib_SaturnIBObliq/ │   ┌───────────────────────────────┐   │
  │  │ plant/generated/   │   │  SaturnIBStack (plant + IMU   │   │
  │  │ saturn_ib_stack.c  │◄──┤  physics + FDAI state).       │   │
  │  └────────────────────┘   │  AGC coupling via virtual     │   │
  │                           │  IAgcBridge hooks.            │   │
  │                           │  src/sim/SaturnIBStack.cpp    │   │
  │                           └───────────────┬───────────────┘   │
  └───────────────────────────────────────────┼───────────────────┘
                                              │ hooks (null-impl default)
  ┌───────────────────────────────────────────┼───────────────────┐
  │             saturnib_cmc  (links yaAGC)   ▼                   │
  │                                                                │
  │  ┌──────────────────────────────────────────────────────────┐ │
  │  │  SaturnIBAgcBridge  (inherits SaturnIBStack, overrides   │ │
  │  │  the AGC hooks).  Owns agc_t*, uplink queue, downlink,   │ │
  │  │  PIPA/CDU injection, ISS turn-on + NO ATT sequencing,    │ │
  │  │  and the extern "C" yaAGC channel callbacks.             │ │
  │  │  src/cmc/SaturnIBAgcBridge.cpp                           │ │
  │  └──────────────────────────────┬───────────────────────────┘ │
  │                                 │                              │
  │                                 ▼                              │
  │                     ┌───────────────────────────┐              │
  │                     │  Virtual AGC (yaAGC)      │              │
  │                     │  third_party/virtualagc/  │              │
  │                     │  Colossus 249 ROM         │              │
  │                     └───────────────────────────┘              │
  └────────────────────────────────────────────────────────────────┘
                 │                                 │
                 ▼                                 ▼
    ┌────────────────────────┐          ┌────────────────────────┐
    │   batch_sim            │          │   apolloa              │
    │   src/batch/main.cpp   │          │   src/gui/apolloa_main │
    │   links: saturnib      │          │   links: saturnib +    │
    │                        │          │          saturnib_cmc  │
    │  - headless            │          │  - DSKY + FDAI panel   │
    │  - JSON scenario in    │          │  - wall-clock pace     │
    │  - final.json + CSV    │          │  - OpenGL/GLUT         │
    │  - plant-only; no AGC  │          │  - interactive run     │
    │  - deterministic       │          │                        │
    └────────────────────────┘          └────────────────────────┘
```

`batch_sim` links `saturnib` only and never instantiates a CMC — the AGC
hook points degrade to no-ops through the default `NullAgcBridge`.
`apolloa` links both libraries; `SaturnStartupHelper` constructs a
`SaturnIBAgcBridge` which attaches itself as the active `IAgcBridge` and
is the `SimModel` registered with the clock.  The vehicle and LVDC paths
in both programs are byte-for-byte identical; only the AGC layer differs.

The `saturnib` library is itself a composition: the Obliq-2 model (`model/saturn_ib_stack.json`) is cross-compiled to C by an external tool and lives under `lib_SaturnIBObliq/plant/generated/`, except for `LVDA_LVDC.c` which is hand-edited in `lib_SaturnIBObliq/native-snapshots/lvda-lvdc/` and overrides the regenerated copy. See [`lib_SaturnIBObliq/README.md`](lib_SaturnIBObliq/README.md) for the regeneration workflow.

## Project layout

| Path | Contents |
|------|----------|
| `src/sim/` | Vehicle systems: `SaturnIBStack` (plant + IMU physics + FDAI), Obliq plant adapter, `IAgcBridge` hook interface |
| `src/cmc/` | Saturn IU ↔ CSM AGC bridge: `SaturnIBAgcBridge`, `UplinkMessage`, yaAGC channel handlers, `SaturnStartupHelper` |
| `src/support/` | Vectors, quaternions, dates, the simulation clock, logging |
| `src/batch/`, `src/gui/` | The two programs |
| `lib_SaturnIBObliq/` | Generated plant and hand-edited LVDC ([details](lib_SaturnIBObliq/README.md)) |
| `model/` | Obliq-2 models and AS-205 reference tables; `provenance/` has the original Simulink model and its reference run |
| `data/` | Runtime files: ROM, pad loads, downlink tables, panel layout, textures, example inputs |
| `reference/` | [Source documents and their OCR text](reference/README.md) |
| `docs/` | [User docs and engineering notes](docs/README.md) |
| `tests/` | Batch regression scripts and the GUI scenario |
| `third_party/` | Bundled libraries, Virtual AGC patches |
| `build/` | Your build output (not tracked) |

A few things are worth calling out explicitly:

**`src/sim/SaturnIBStack.{h,cpp}`** owns the plant tick, the IMU physics (coarse/fine align, CDU accumulators) and the FDAI state.  It knows nothing about yaAGC — AGC coupling goes through the `IAgcBridge` hook interface (`src/sim/IAgcBridge.h`), whose default implementation is a no-op.

**`src/cmc/SaturnIBAgcBridge.{h,cpp}`** inherits from `SaturnIBStack` and implements the AGC bridge: PAD-load and state-vector uplink scheduling, PIPA injection, CDU drive, ISS turn-on + NO ATT lamp tracking, the GRR/liftoff discrete, and the DSKY downlink buffer.  All Apollo 7 launch choreography in the interactive run (`NO ATT` extinguish at T−90, state-vector uplink at T−135, P01→P02 transition, liftoff discrete at T+300) is driven from here.  This translation unit is the only place that includes `<agc_engine.h>` and references yaAGC symbols; the `extern "C"` yaAGC channel callbacks live alongside it.

**`lib_SaturnIBObliq/plant/generated/saturn_ib_stack.c`** is the compiled vehicle dynamics. The model covers both Saturn stages (H-1 cluster + J-2), aerodynamics, mass flow and center-of-mass migration, the ST-124M stable platform, and the LVDC major loop. The file is regenerated from `model/saturn_ib_stack.json` by an external Obliq-2 tool and is not edited by hand.

**`lib_SaturnIBObliq/native-snapshots/lvda-lvdc/LVDA_LVDC.c`** is the LVDC guidance computer. It is the one hand-edited C module in the whole stack: certain guidance details (IGM major-cycle timing, HSL cutoff prediction, Chi-freeze gating) are maintained here with EDD / TM citations in block comments. The regenerated companion at `lib_SaturnIBObliq/plant/generated/LVDA_LVDC.c` is kept for diffing but is not linked.

**`model/provenance/`** holds the pre-Obliq lineage: the original Simulink `.mdl` and a 1000-s reference CSV / final-state JSON. The current build does not read these, but they are the historical baseline every current result is measured against.

**`reference/`** contains the NASA and IBM source documents the model is built from (TM-X-62831 and the seven-volume LVDC-EDD), plus their machine-extracted markdown / HTML for grep-friendly access. See [`docs/MODEL_REFERENCES.md`](docs/MODEL_REFERENCES.md) for the full scope-and-sources tour.

`model/stable_member_control.json` is an Obliq-2 model of the IMU stable member. It is not used by the build yet: the IMU is modeled in `src/sim/SaturnIBStack.cpp`.

## License

Copyright (c) 2026, Riley Rainey. Licensed under the GNU General Public License, version 3 only (GPL-3.0-only): see [LICENSE](LICENSE). Third-party components are listed in [THIRD_PARTY.md](THIRD_PARTY.md).
