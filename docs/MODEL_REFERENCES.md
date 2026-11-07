# Model scope and reference sources

This document is the big-picture tour of what the Apollo-Saturn-IB simulation models, which NASA and IBM engineering references it was built from, and how the Saturn Instrument Unit couples to the Virtual AGC running Colossus 249. It is written for the technically curious reader who wants to understand what we are simulating and, where it matters, see the exact table or equation that supplied a number.

For the low-level workflow and build details, see the top-level [`README.md`](../README.md) and [`lib_SaturnIBObliq/README.md`](../lib_SaturnIBObliq/README.md). For the running tally of residuals and open items on the AS-205 scenario specifically, see [`AS205_REFERENCE.md`](AS205_REFERENCE.md) and [`TM_X_62831_TRAJECTORY_COMPARE.md`](TM_X_62831_TRAJECTORY_COMPARE.md).

---

## 1. What is modeled

The simulation flies the AS-205 vehicle (Apollo 7, launched 1968-10-11) from pre-launch through orbital insertion. The modeled scope:

### Vehicle dynamics

- Two-stage Saturn IB: eight-engine H-1 S-IB first stage and a single-engine J-2 S-IVB second stage.
- 6-DoF variable-mass equations of motion for each stage, with mass flow, center-of-mass migration, and moment-of-inertia tensor updates.
- Aerodynamics from AS-205 wind-tunnel data: longitudinal and lateral coefficients as functions of Mach and angle of attack.
- H-1 cluster thrust-and-mass-flow lookup tables with per-engine scaling and gimbal.
- J-2 start transient (90% thrust build-up), high mixture-ratio (EMR) and programmed-mixture-ratio (PMR) phases, and nominal GCS cutoff.
- Stage separation at TB3 + 1.38 s, LES/ullage mass jettison.

### Launch Vehicle Digital Computer (LVDC)

- Pre-IGM Time-Tilt pitch program (polynomial attitude schedule) from liftoff to IGM initiate.
- Iterative Guidance Mode (IGM) through S-IVB cutoff, including:
  - Chi-steering commands in pitch (Chi_Y) and yaw (Chi_Z).
  - Position-correction terms (K_1..K_4) and terminal χ̃-steering.
  - Steering Misalignment Correction (SMCY, SMCZ).
  - The MS4 plumbline-to-target-plane transformation.
- High Speed Loop (HSL) cutoff prediction with the EDD polynomial fit (Eq. 4.4.91–4.4.94).
- Time Bases (TB0..TB4) and the event-sequence discrete outputs that drive stage separation, J-2 ignition, and IGM initiation.
- Mode codes MC25/MC27 bit flags, nIGMMode state machine.

### Saturn Instrument Unit — CSM interface

- ST-124M inertial platform three-gimbal attitude and PIPA pulse-integrating accelerometers.
- FDAI (Flight Director Attitude Indicator) ball and error needles driven from the IMU gimbal angles.
- PAD load uplink to the CMC (AS-205 launch parameters).
- State-vector uplink at T−135 s (P27/V71 sequence to Colossus 249).
- Liftoff and SIVB-abort discretes to AGC input channels; ISS turn-on and NO ATT handshake on output channels.

### Command Module AGC

- Virtual AGC `yaAGC` running the Colossus 249 flight program (Apollo 7 CM software).
- DSKY (keyboard + 3 numeric register display) rendered in the interactive panel.
- Minimal CM IMU emulation sufficient to accept PIPA pulses and report CDU angles.

### What is NOT modeled

- The LM, DAP, or RCS — this is a Block II CSM-only configuration.
- S-IVB restart and TLI — the simulation ends at orbital insertion (~GCS + coast).
- J-2 thrust tailoff / shutdown transient — the engine stops instantly on cutoff command. This is the known reason the final velocity sits ~7 m/s short of `V_T` at engine-off in the current build; see [`TM_X_62831_TRAJECTORY_COMPARE.md`](TM_X_62831_TRAJECTORY_COMPARE.md).
- Ground-system interfaces beyond the pre-launch PAD and state-vector uplinks (no live DSS, no RTCC, no Mission Control loop).
- GRF (Guidance Reference Failure) branches in the LVDC — the code is present per EDD but not exercised in the nominal scenario.

---

## 2. Primary reference documents

Every numeric constant and algorithm in the model traces back to one of these four sources. All are checked into the repository under `reference/` with machine-OCR'd markdown next to each PDF for grep-friendly access.

### 2.1 NASA TM-X-62831 — AS-205 Operational Trajectory

**Citation:** Solmon, G. P., *AS-205/CSM-101 Launch Vehicle Operational Trajectory, Revision I*, NASA MSFC, 10 September 1968.

- `reference/NASA-TM-X-62831/NASA-TM-X-62831.pdf` — the document.
- `reference/NASA-TM-X-62831/chandra/NASA-TM-X-62831.md` — Chandra OCR.
- `model/as205-reference/tm_x_62831_*.csv` — digitized tables for direct comparison.

This is the authoritative reference for the AS-205 mission-specific presettings: launch azimuth, target orbit elements, IGM preset times (T3_IGM, T1i, T3i), average exhaust velocities (V_ex1, V_ex3), terminal velocity (V_T), mass properties, and the predicted trajectory itself. It is also the direct reference for the event-sequence timeline (IECO, OECO, TB3, J-2 start, IGM initiate, GCS).

Key tables used directly:

| Table | Role |
|-------|------|
| **Table 1** | Flight-time event sequence (IECO at 140.11 s, OECO/TB3 start at 143.11 s, physical separation at 144.49 s, IGM initiate at 168.11 s = TB3 + 25.0 s, ...) |
| **Table 3B** | IGM presettings — V_T, R_T, V_ex1/V_ex3, T1i/T3i/T3, AP1..AP9 (MS4 matrix elements), TR1..TR3 |
| **Table 3B (cont'd)** | LVDC mask / artificial-τ constants (T10, τ10, C0, M02, M03, F1/F2/F3) |
| **Tables 5–7** | Predicted trajectory (h, V, γ, mass, thrust) over S-IB, ullage/J-2 build-up, and S-IVB phases |
| **Table 12** | Mass sequence: stage weights at each jettison event |

### 2.2 LVDC Equation Defining Document (EDD)

**Citation:** IBM Federal Systems Division, *Saturn Instrument Unit: LVDC Equation Defining Document for the Saturn IB Flight Program, Parts I and II, Revision A*, MSFC-DRL-008A / IBM 70-207-0001, 11 April 1972.

Split across seven PDFs in `reference/LVDC-EDD/`:

| Part | Contents | Used for |
|------|----------|---------|
| **Part I §2** (`satinstunitibm_1`) | Reference coordinate systems and transformations | S-, G-, 4-, V-system definitions; [MSG], [MG4], [MS4], [MBS], [M4V] matrices |
| **Part I §4** (`satinstunitibm_2`) | Boost navigation and guidance | Accelerometer processing, IGM phase structure, HSL cutoff |
| **Part I §7** (`satinstunitibm_3`) | Time bases, discretes and interrupts | TB0–TB4 definitions, event switch selectors |
| **Part I §10** (`satinstunitibm_4`) | Digital Command System | MC25/MC27 bit maps, mode codes |
| **Part I §12–14** (`satinstunitibm_5`) | Preflight tests, *equations catalog* | **The authoritative numbered equations** — Eq. 2.3.1 through Eq. 5.4.x live here in catalog form |
| **Part I §15** (`satinstunitibm_6`) | Definitions and variable dictionary | Symbol lookup, parameter definitions |
| **Part II** (`satinstunitibm_7`) | AS-206 data and requirements | Saturn-IB program constants not re-tuned per mission (ε_2, BN_1, T_HSL in Table 1-1) |

For any equation implemented in `LVDA_LVDC.c`, the equation number cited in a block comment resolves against `reference/LVDC-EDD/chandra/satinstunitibm_5/satinstunitibm_5.md` — that volume is the catalog. Example: `Eq. 4.4.91–4.4.94` for the HSL polynomial cutoff is at line ~897 of that file.

### 2.3 Virtual AGC and the Colossus 249 flight program

**Citation:** [Virtual AGC project](https://github.com/virtualagc/virtualagc), MIT Instrumentation Laboratory source for *Colossus 249* (Apollo 7 Command Module program).

The AGC flight software is the real one that flew on Apollo 7. We assemble the sources at build time (via Virtual AGC's build chain) and the resulting ROM image is at `data/Colossus249.bin`. Downlink-list decoding tables are at `data/ddd-7777[3-7]-Colossus249.tsv`. The `yaAGC` core emulator is built as a static library and linked into `saturnib`.

### 2.4 AS-205 Flight PAD

The actual Apollo 7 launch PAD load values are captured in `data/TM-X-62831_PAD_LOAD.txt` (TM-derived) and `data/AS-205_PAD_LOAD.txt`. These are the AGC verb/noun sequences sent to the CMC during the pre-launch uplink window.

### 2.5 Supplementary references inherited from the original Simulink model

The pre-Obliq Simulink model (`model/provenance/saturn_ib_stack.mdl`) carries embedded Annotation blocks naming the documents that supplied specific subsystem data. These are the "where did that number come from?" trail for parts of the vehicle that are not covered by TM-X-62831 or the LVDC EDD — primarily aero coefficients, S-IB engine performance curves, and the pre-GRR control-system gains. The citations are preserved in the Obliq-2 port even though the MDL is no longer compiled.

| Reference | Subsystem | MDL annotation location |
|-----------|-----------|------------------------|
| **NASA-TM-X-53243** — *Static Aerodynamic Characteristics of the Aborted Saturn IB Vehicle* | Saturn IB aero coefficients (lift, drag, moment vs Mach and α) | `saturn_ib_stack.mdl:12837`, `:28638` — "Use data from NASA-TM-X-53243" |
| **NASA-CR-95967** | Individual H-1 engine performance estimates (per-engine thrust ratings, mass flow) | `saturn_ib_stack.mdl:23955`, `:34125` — "Individual engine performance estimates have been integrated into this model. See NASA-CR-95967 for the original values." |
| **NASA-CR-71607** — *Astrionics System Handbook*, Figure 3.4-3 and §3-1 "S-IVB Polarity Table" | S-IVB TVC polarity and attitude-error sign convention | `saturn_ib_stack.mdl:39501` — "Based on Figure 3.4-3 from Astrionics System Handbook and S-IVB Polarity Table from section 3-1" |
| **NASA-CR-92837** — *SA-204/LM-1 Launch Vehicle Dynamics Analyses* | S-IVB control-system gain schedule; ullage motor timing (p. 114) | `saturn_ib_stack.mdl:40099`, `:59857` |
| **Chrysler TN-AP-67-158** — *AS-205 Revised Launch Vehicle Reference Trajectory* (1967, never-flown pre-flight trajectory; referred to as "[1]" in MDL annotations) | S-IB mass characteristics, liftoff weight, separation weight (Table 3A); cutoff-mass estimate **These values were ultimately superseded by values from NASA TM-X-62831 when we transitions to `obliq-2` for modeling**| `saturn_ib_stack.mdl:25460`, `:25466`, `:35730` |
| **AS-203 vehicle document** | Rate-gyro model (same gyro package was flown on AS-203 and AS-205) | `saturn_ib_stack.mdl:36988` — "Gyro model from AS-203 document" |
| **SLV, AS-205, Rev A** | Attitude-error limits | `saturn_ib_stack.mdl:~39501` (adjacent annotation) |
| **LVDC EDD §2.3 / §4.2.5** | ST-124M Euler-angle rotation sequence (Eq. 2.3.3); geocentric-latitude convention per EDD §4.2.5 | `saturn_ib_stack.mdl:1653`, `:58385`, `:60042` |

A separate annotation at `saturn_ib_stack.mdl:60984` records a deliberate deviation from AS-205-era document convention:

> *"Early Apollo technical documents use different axes assignments for the Body, Gimbal, and S-Frames. Documents for the AS-205 flight use the original standard. This model uses the assignments used later in the project as those match up with the 1992 AIAA standard and what's used in the 1972 S-IB Equation Defining Document."*

This is why the sim's body/gimbal/S-frame handedness follows the LVDC EDD (1972) rather than the AS-205-era MSFC convention. It's a minor but important fact when correlating model signals against AS-205-era paper data.

Two of the inherited references cover subsystems that are not yet documented in `as205TmX62831Presettings.ts`:

- The aero data (NASA-TM-X-53243) populates the thrust-and-aero lookup tables inside `saturn_ib_stack.json` itself; the TS presettings file does not mirror these.
- The S-IVB control-system gains (NASA-CR-92837) populate the inner control loop in the plant (`saturn_ib_stack.json`) rather than the LVDC presettings; the H1_YAW_POLARITY_PROBE audit touches this layer.

The NASA reports themselves are not currently in the repo as full PDFs. Finding and archiving them under `reference/` would be a useful supplement — the MDL annotations are enough to identify them, and all four NASA documents (TM-X-53243, CR-95967, CR-71607, CR-92837) are generally findable through [NASA Technical Reports Server (NTRS)](https://ntrs.nasa.gov/) and ibiblio's Apollo archive.

---

## 3. Where key values come from

Every numerical preset in the model is mirrored, with its citation, in the TypeScript provenance file at [`lib_SaturnIBObliq/ts/as205TmX62831Presettings.ts`](../lib_SaturnIBObliq/ts/as205TmX62831Presettings.ts). The authoritative compile-time value lives in [`model/saturn_ib_stack.json`](../model/saturn_ib_stack.json). The TS file is not compiled — it exists solely so that for each number in the JSON there is a human-readable cite answering "where did this come from?"

A representative sample (the complete set has ~60 entries):

### Launch site and aim

| Parameter | Value | Units | Source |
|-----------|-------|-------|--------|
| `A_z_deg` | 72.0 | deg | TM-X-62831 Table 3B (AP note) / §II mission criteria — flight azimuth E of N |
| `pad_roll_L_deg` | 100.0 | deg | TM-X-62831 §II mission criteria — pad-oriented azimuth |
| `phi_L_deg` | 28.521963 | deg | TM-X-62831 Table 3B (AP note) — geodetic launch latitude |
| `lambda_L_deg` | −80.561141 | deg | LC-34 launch complex longitude (tn-hold) |
| `i_deg` | 31.605 | deg | TM-X-62831 Table 3B (AP note) — orbital inclination |
| `lambda_0_deg` | 119.0 | deg | TM-X-62831 Table 3B (AP note) — descending-node argument of desired orbit |

### Target orbit

| Parameter | Value | Units | Source |
|-----------|-------|-------|--------|
| `V_T_mps` | 7780.67 | m/s | TM-X-62831 Table 3B VT / summary GCS — terminal velocity |
| `R_T_m` | 6,600,405 | m | TM-X-62831 Table 3B XVT — terminal position magnitude |
| `Xdotdot_VGT_mps2` | −9.15 | m/s² | TM-X-62831 Table 3B XVG — terminal gravity vector |

### IGM presettings

| Parameter | Value | Units | Source |
|-----------|-------|-------|--------|
| `T3_IGM_sec` | 25.0 | s | TM-X-62831 Table 3B / §V-A — TB3 → IGM initiate |
| `epsilon_prime_sec` | 5.0 | s | TM-X-62831 Table 3B (cont'd) — T3i to freeze IGM (ε′) |
| `epsilon_2_sec` | 25.0 | s | AS-206 EDD Part II Table 1-1 — χ̃ terminal-steering start |
| `T_HSL_sec` | 5.0 | s | AS-206 EDD Part II Table 1-1 — HSL entry threshold |
| `BN_1_sec` | 14.4 | s | AS-206 EDD Part II Table 1-1 — velocity-history start |
| `V_ex1_mps` | 4151.0 | m/s | TM-X-62831 Table 3B Vex1 — S-IB avg exhaust velocity |
| `V_ex3_mps` | 4233.2 | m/s | TM-X-62831 Table 3B Vex3 — S-IVB avg exhaust velocity |
| `tau_1_i_preset_sec` | 286.3 | s | TM-X-62831 Table 3B T1i — first-stage time-to-go |
| `tau_3_i_preset_sec` | 159.8 | s | TM-X-62831 Table 3B T3i — second-stage time-to-go |

### Still-unverified "tn-hold" values

A handful of presets carry a `cite: { source: 'tn-hold' }` annotation. These are values inherited from the older Chrysler TN-AP-67-158 reference pack that never flew, retained unchanged because no TM-X-62831 citation has been located. They are audit targets — fixing them means finding the TM table they should have come from, not tuning them to make numerics match.

Current `tn-hold` entries at time of writing: `phi_L_prime_deg`, `lambda_L_deg`, `R_L_m`, `DeltaV_b_mps`, `V_GRD_mps`, `BN_1_sec`, and a few Earth-model helpers (`R_earth_m`, `omega_E_rps`).

---

## 4. The Saturn IU ↔ Virtual AGC interface

The host-side bridge between the Saturn Instrument Unit and the CMC lives in `src/sim/SaturnIBStack.cpp` (~2600 lines). This is where every signal that crosses from the launch vehicle into the Command Module — and vice versa — is mediated.

### 4.1 PAD load uplink (T−285 s to T−244 s, in sim time)

On entry to the interactive scenario the CMC is already powered up and in the standby P00 state. At simulated elapsed time ≈ 15 s, `SaturnIBStack` begins streaming keyboard keycodes to the AGC input DSKY channel to upload the AS-205 launch PAD:

- File: `data/TM-X-62831_PAD_LOAD.txt` (or the equivalent chosen in the scenario JSON).
- Format: one AGC keyboard keystroke per line (`V`, `N`, digits, `E`, `R`, etc.), with annotations.
- Delivered via `UplinkMessage` (`src/sim/UplinkMessage.{h,cpp}`), one keycode every ~40 ms.
- Completes with the UPLINK lamp lit on the DSKY.

### 4.2 ISS turn-on handshake (T−210 s)

The AGC's ISS ("Inertial Subsystem") normally requires a 90-second power-up cycle during which the NO ATT lamp is held on. Our bridge:

- Monitors the AGC output `ch030` TURN_ON_REQUEST bit.
- At 90 s after platform power-up, sets the `ch030` IMU_OPERATE discrete to signal the ISS is online, extinguishing NO ATT.

See `SaturnIBStack.h:286` ("ISS turn-on delay") for the specific channel bits.

### 4.3 State-vector uplink (T−165 s)

45 s after NO ATT extinguishes, the bridge composes a P27/V71 contiguous-update packet carrying the vehicle's R⃗_ECI and V⃗_ECI (meters / m/s), scaled to the AGC's B-29 / B-7 / B-28 fixed-point conventions, and sends it as a keycode stream:

- `V71E 21E` preamble, UPSVFLAG ECADR, state-vector ID, R_xyz, V_xyz, CMC-clock time.
- Coded in `UplinkMessage::Compose...()`.
- Reference: Colossus 249 UPDATE_PROGRAM.s / GSOP R-577 §2.1.

### 4.4 Liftoff discrete and continuous telemetry

Once the countdown reaches T+0 (simulated elapsed = `T_L_prime_sec`, nominally 300 s for the AS-205 scenario), the Saturn IU asserts the LIFTOFF discrete on `ch030`. The AGC's Colossus 249 program responds by automatically advancing from P02 (prelaunch) to P11 (Earth orbit insertion monitor) and begins integrating PIPA pulses for real.

From liftoff forward:

- The vehicle plant integrates 6-DoF dynamics at 50 Hz.
- The IMU gimbal angles and PIPA accumulations are produced each plant step.
- `SaturnIBStack::sim_tick()` samples the PIPA deltas every AGC minor cycle (~40 ms) and calls into `yaAGC` to inject them as pulse counts (`CounterCommand` pulses into the host counters), driving PIPA integration inside Colossus 249.
- CDU angles (the resolved IMU gimbal readouts) are tracked similarly.
- The FDAI ball attitude and rate needles in the interactive panel are driven from the same IMU gimbal angles the AGC sees, so what P11 reports and what the panel shows always agree.

### 4.5 Channel map (host side)

Selected AGC I/O channels the bridge touches. Full details and bit-level documentation live in `src/sim/SaturnIBStack.h`.

| Channel | Direction | Role |
|---------|-----------|------|
| `ch010` | AGC→host | DSKY display register (DSPTAB) — decoded for NO ATT lamp edges |
| `ch011` | AGC→host | Discrete outputs — ENGINE_ON, S-IVB_IGN control (not acted on in sim) |
| `ch012` | AGC→host | More discrete outputs — ISS turn-on complete, coarse-align/zero |
| `ch030` | host→AGC | ISS status / liftoff / cage / abort discretes |
| `ch031` | host→AGC | Translation/rotation hand controller (unused in flight) |
| `ch032` | host→AGC | PROCEED pushbutton |
| `ch033` | host→AGC | PIPA_FAIL, downlink-TM, oscillator-fail |

---

## 5. Fidelity status and known gaps

At the time of writing, the AS-205-tm-x-62831 scenario reaches S-IVB engine cutoff with:

- **Altitude at cutoff:** sim 228,104 m vs TM 227,941 m → Δh = +163 m.
- **Velocity at cutoff:** sim 7773.0 m/s vs TM 7780.67 m/s → ΔV = −7.6 m/s.
- **Latitude ground-track:** matches TM LATT within ±0.05° (±5 km) throughout the S-IVB burn.
- **Cutoff flight time:** sim ~614.5 s vs TM 614.63 s.

The remaining −7.6 m/s ΔV equals `PARAM_DeltaV_b_mps = 7.2381` exactly, by design. The HSL polynomial cutoff targets `V = V_T − ΔV_b` so that real J-2 thrust tailoff adds the remaining ΔV_b m/s after the cutoff command. Our plant does not model that tailoff — the engine stops instantly on cutoff command — so V stays at `V_T − ΔV_b`. Closing this last residual is a plant-model item.

The full before/after chain across today's fixes and a running log of open items are tracked in [`TM_X_62831_TRAJECTORY_COMPARE.md`](TM_X_62831_TRAJECTORY_COMPARE.md).

### Known gaps and audit targets

- **J-2 thrust tailoff model** — plant limitation; drives the remaining −7.6 m/s at cutoff (see above).
- **tn-hold presets** — `phi_L_prime_deg`, `lambda_L_deg`, `R_L_m`, `DeltaV_b_mps`, `V_GRD_mps` and a few others still carry values inherited from TN-AP-67-158 rather than cited to TM-X-62831. Finding the authoritative TM source for each is the audit task.
- **Pad-hold ECI frame handling** — during the 300 s on-pad hold, the sim reports longitude in an inertial-at-GRR frame that drifts ~1.25° west of the Earth-fixed pad position by first motion. Does not affect in-flight trajectory but shows up in lat/lon comparisons.
- **Chi_Z yaw sign convention** — sim Chi_Z and TM CHIY sit at opposite signs through S-IVB burn. Characterized as a reporting-convention artifact (not a steering error): the FCC yaw gain polarity is flipped vs pitch, exactly compensating the opposite-sign Chi_Z convention. See the "Chi_Z yaw bias" section of [`TM_X_62831_TRAJECTORY_COMPARE.md`](TM_X_62831_TRAJECTORY_COMPARE.md).

### What's authoritative

- The LVDC MS4 matrix has been audited against EDD Eq. 2.3.5 element-by-element and against TM-X-62831 AP1..AP9 numerically — all 9 elements match to 5 digits for the AS-205 angle set.
- The IGM-enable gate has been fixed to the EDD-specified `T3_Timer ≥ 25 s` (was a hardcoded Simulink literal of 44 s).
- The HSL cutoff now implements EDD Eqs. 4.4.91–4.4.94 (2nd-degree V(t) polynomial over 3 BML samples, solved for the time at which V reaches V_T − ΔV_b).
- The chi-freeze regression gate (`tests/batch/chi_freeze.sh`) verifies that Chi_Z does not step more than 1.5° at the ε_2 terminal-steering transition.

---

## 6. Further reading inside the repo

- [`AS205_REFERENCE.md`](AS205_REFERENCE.md) — concrete source-of-truth mapping between model layers and TM tables.
- [`TM_X_62831_TRAJECTORY_COMPARE.md`](TM_X_62831_TRAJECTORY_COMPARE.md) — running residual report with before/after sections for every fix.
- [`APOLLO_COORDINATE_FRAMES.md`](APOLLO_COORDINATE_FRAMES.md) — the coordinate systems used (ECI, S-frame, 4-frame, pad frame).
- [`HARDCODED_CONSTANTS_GAINS_CATALOG.md`](HARDCODED_CONSTANTS_GAINS_CATALOG.md) — exhaustive catalog of constants and gains in the model with provenance.
- [`lib_SaturnIBObliq/README.md`](../lib_SaturnIBObliq/README.md) — how `saturn_ib_stack.json` is translated to C and how the hand-edited LVDC fits in.
- [`reference/README.md`](../reference/README.md) — brief guide to the reference PDFs and their OCR outputs.
