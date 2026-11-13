# Apollo-Saturn-IB

This is a simulation of the Apollo-7 (AS-205) Saturn IB launch. This simulation includes launch vehicle modeling and simulations of both the Saturn Instrument Unit and the CSM Apollo Guidance Computer. The vehicle dynamics and Launch Vehicle Digital Computer guidance are C code generated from an [Obliq-2](https://github.com/rrainey/obliq-2) model. The AGC is [Virtual AGC](https://github.com/virtualagc/virtualagc)'s `yaAGC`, running the Colossus 249 flight software.

The Saturn launch vehicle modeling is based almost entirely on Apollo-era NASA and IBM engineering references.

There are two ways to run it:

- **`apolloa`** is an interactive DSKY and FDAI panel running at wall-clock pace.

- **`batch_sim`** runs headless from a JSON file as fast as the CPU allows (about 150× real time), and writes a final-state JSON and a 1 Hz CSV. Runs are deterministic.

## Simulation Fidelity

The simulation is validated against the pre-flight operational trajectory in NASA TM-X-62831, *AS-205/CSM-101 Launch Vehicle Operational Trajectory, Revision 1*. The LVDC guidance implementation integrates sensor data (ST-124M stabilized platform PIPAs) per EDD §4.3.1 Eqs. 4.3.1-5; no plant state vectors are read directly by the LVDC side.

Measured residuals at S-IVB guidance cutoff on the `AS-205-tm-x-62831` scenario (`data/examples/AS-205-tm-x-62831.json`, `--run-time 960`):

| Metric at S-IVB cutoff | TM-X-62831 target | Simulation | Residual |
|---|---|---|---|
| Space-fixed velocity | 7780.7 m/s | 7779.2 m/s | −1.5 m/s |
| Altitude | 227.94 km | 227.60 km | −0.34 km |
| Flight time (post-liftoff) | 614.63 s | 613.00 s | −1.6 s |

For context, the operational Saturn IB program expected a nominal ±3σ cutoff velocity dispersion on the order of a few m/s. The residuals above are well inside that envelope.

The guidance loop runs on the EDD cadences: the Boost Major Loop (BML) at 1.6 s and the HSL minor loop at 40 ms. The HSL cutoff decision uses the Eq. 4.4.91-94 velocity polynomial with a 3-sample V-history refreshed on BML boundaries; the ΔV_b = 7.2381 m/s and V_T = 7780.67 m/s presettings come from EDD Part II Table 1-3.

## Build

You need CMake 3.16+, a C++17 compiler, git and network access for the first configure. The GUI also needs OpenGL and GLUT (`apt install freeglut3-dev` on Ubuntu). Pass `-DBUILD_GUI=OFF` to skip it.

```bash
cmake -S . -B build
cmake --build build -j
```

The first configure clones Virtual AGC at a pinned commit into `third_party/virtualagc/` and applies the patches in `third_party/virtualagc-patches/`.

## Run the Interactive Simulation

The application is currently designed to run from the  `./data/` directory.

```
$ cd data
$ ../build/apolloa
```

1. Starting the simulation automatically initiates a 90-second power-up sequence for the ISS (the inertial platform for the AGC). The "NO ATT" lamp will illuminate. In the terminal window where you started the application, you will see these messages:

```
scene: 1362x800, 65 top-level children, 3 textures
Opening ddd-77773-Colossus249.tsv
Records read from ddd-77773-Colossus249.tsv:  126
Opening ddd-77774-Colossus249.tsv
Records read from ddd-77774-Colossus249.tsv:  125
Opening ddd-77775-Colossus249.tsv
Records read from ddd-77775-Colossus249.tsv:  130
Opening ddd-77776-Colossus249.tsv
Records read from ddd-77776-Colossus249.tsv:  136
Opening ddd-77777-Colossus249.tsv
Records read from ddd-77777-Colossus249.tsv:  133
AGC downlink: configured for Colossus249 (CmOrLm=1)
sim: launch vehicle = 0x5fdec440e070
sim: Colossus249.bin OK (73728 bytes)
draw_sphere name=AttitudeBall tex=1 radii=(214.732727,214.732727,214.732544) loc=(249.160477,250.419769,-260.000000)
  scene.textures.size=2, tex_ok=1
  tex[1]: textures/sphere-rotated.png gl_id=5
sim FDAI nodes: ball=0x5fdec36c9530 indexer=0x5fdec36d4050 rates=(0x5fdec36d3b10,0x5fdec36d3870,0x5fdec36d35a0) errs=(0x5fdec36d2d90,0x5fdec36ca390,0x5fdec36caa60)
  sim ch030 = 17377  [TEMP_OK=1 TURN_ON_REQ=1 IMU_FAIL=0 ICDU_FAIL=0 IMU_CAGE=0 SC_CTL=1 IMU_OPER=1 GRR=0 LIFTOFF=0 SIVB_ABORT=0]
  sim ch031 = 37777  [IMU_PWR=1 FREE=1 HOLD=1 THC/RHC=7777]
  sim ch032 = 77777  [PROCEED=0]
  sim ch033 = 77777  [PIPA_FAIL=0 DNTM_FAST=0 UPTM_FAST=0 AGC_WARN=0 OSCILLATOR_FAIL=0]
AGC downlink: first decoded list "Coast and Align"
```

2. Immediately after the simulation starts, press the **RSET** push button. This works around a bug in the current start up sequence.  If you fail to press **RESET** you may see a flashing "02" on the DSKY display -- just press **RSET** at that point and the startup sequence will proceed.

3. After 15 seconds, a simulated data uplink will begin.  The "UPLINK" lamp will illuminate to confirm this.  This is a transfer of the Apollo 7 PAD LOAD parameters to the AGC.

```
  PAD LOAD STARTED at elapsed=15.010 sec (file=TM-X-62831_PAD_LOAD.txt)
  UPLINK COMPLETE at elapsed=56.451 sec
```

4. At start+90 seconds, the "NO ATT" lamp will extinguish.  Keep monitoring the display.

```
  ISS turn-on delay complete at elapsed=90.627 sec
  NO ATT OFF (SV arm) at elapsed=90.627 sec — state-vector uplink in +45 s (enter P00 before then; then P01→P02 after SV)
  sim ch030 = 37377  [TEMP_OK=1 TURN_ON_REQ=0 IMU_FAIL=0 ICDU_FAIL=0 IMU_CAGE=0 SC_CTL=1 IMU_OPER=1 GRR=0 LIFTOFF=0 SIVB_ABORT=0]
```

5. At start+135 seconds, the vehicle position and velocity AGC State Vector will be uplinked to the AGC:

```
  STATE VECTOR UPLINK STARTED at elapsed=135.637 sec
    R_ECI=[-5409656.7, 1479638.0, 3027472.3] m   |R|=6373327.5 m
    V_ECI=[-107.8969, -394.4784, 0.0000] m/s |V|=408.9681 m/s
    T_clock=135.637 s (from CMC zero)  id=CSM/Earth  ECADR=01501
    preamble: V37E 00E then V71E (need P00 or P02)
    gate: NO ATT off at 90.627 + 45 s
  UPLINK COMPLETE at elapsed=169.654 sec
```

6. Once the state vector uplink is complete, you must prepare for launch monitoring by putting the AGC into program **P01**, "Prelaunch Initialization and Alignment"; to initiate **P01**, key in:

  **VERB** **3** **7** **ENTR** **0** **1** **ENTR**

7. **_P01_** will orchestrate ISS alignment for launch.  The FDAI will erect onto the correct launch pad alignment.  Once correctly aligned, the AGC will then automatically switch to program **P02**, "Gyro Compassing Program", and await the **liftoff discrete** from the Saturn Instrument Unit.

8. start+297 seconds -- **Ignition**

9. start+300 seconds -- **Liftoff**; the AGC will automatically switch to **P11**, "Earth Orbit Insertion Monitor Program".

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

Copyright (c) Riley Rainey. Licensed under the GNU General Public License, version 3 only (GPL-3.0-only): see [LICENSE](LICENSE). Third-party components are listed in [THIRD_PARTY.md](THIRD_PARTY.md).
