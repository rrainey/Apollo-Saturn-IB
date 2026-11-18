# LVDA/LVDC quantization inventory (Phase A.2)

**Date:** 2026-11-18
**Branch:** `lvdc-rewrite`
**Purpose:** catalogue every finite-precision effect in the LVDA/LVDC
sensor and command path, its EDD or hardware reference, where it is
modelled today, and where the rewrite should model it.

Per `~/src/obliq-2/CLAUDE.md`, sensor fidelity is a project goal and
quantization is part of the sensor path. The LVDA/LVDC chain has four
distinct finite-precision points and three distinct sample rates.

## Primary references

- EDD Part I §4.2.1 — accelerometer data format
- EDD Part I §4.2.3 — velocity accumulation (pulse to m/s conversion)
- EDD Part I §8.2.1 — gimbal resolver data format
- EDD Part I §8.2.2 — resolver disagreement and reasonableness tests
- EDD Part I §8.2.3 — attitude error commands (D/A output)
- EDD Part I §8.3.1 — minor-loop attitude increments (rate limit)
- EDD Part I §4.1 — minor-loop timing (40 ms during boost)
- EDD Part I §8.1 — minor-loop rate confirmation (25 Hz boost / 10 Hz orbit)

## Sample rates

| Rate | Period | Purpose | EDD anchor |
|------|--------|---------|------------|
| **Minor loop (boost)** | **40 ms (25 Hz)** | Resolver read, attitude error command to FCC, DI24 interrogation (twice/40 ms) | §4.1, §8.1, §7.4 DI24 |
| Minor loop (orbit) | 100 ms (10 Hz) | Same, orbital rate | §8.1 |
| **Boost major loop (BML)** | **≈ 1.6 s (variable)** | Accelerometer read, nav integration, F/M, M/F smoothing, IGM | §4.1 |
| Minor-loop support | once per BML (boost) | Attitude increments, gimbal-to-body transform coefficients | §8.1, §8.3 |

**Correction to plan doc:** the minor loop runs at **25 Hz (40 ms)**
during boost, not 40 Hz (25 ms) as the plan said. The 25 ms references
in EDD §8 are within-minor-loop sub-timing for the resolver sampling
sequence (yaw, then pitch, then roll), not the loop period itself.
The plan doc will be amended in the same commit as this inventory.

BML cadence is nominally 1.6 s but **variable from pass to pass**
(§4.1): "The length of time, ΔT, required by the computer to execute
one complete BML sequence and the repetitive minor loop functions will
be variable from pass to pass because of the varying program
requirements throughout the boost period." For simulation, use the
nominal 1.6 s; note the model does not reproduce computer load jitter.

## Quantization points

### 1. PIPA accelerometer input — pulse to m/s

**EDD §4.2.1 and §4.2.3:**
- Dual redundant optisyn pulse trains per axis, each pulse = **0.05 m/sec**
- Data word layout: `AAAAAAAAAAASSBBBBBBBBBBBBBB` — A counter, 2 spare,
  B counter. A/B disagreement tests per §4.2.2.1 (within 2 pulses =
  0.1 m/s), reasonableness per §4.2.2.3, zero test per §4.2.2.2.
- Pulses accumulated in LVDA delay lines, read at start of each BML.
- Pulse count × 0.05 → platform velocity component in m/s.
- Accumulator initialized to zero at GRR (§4.2.3).

**Modelled today where:**
`saturn_ib_stack.json` → `Saturn_Instrument_Unit_IU/ST_124M_Stabilized_Platform/Accelerometer_Quantization`
with `quantum: 0.05` (JSON L85104). Output is a quantized *velocity*
in m/s, delivered to `LVDA_LVDC.inputs.V_m_bar_mps[3]` at the BML gate
inside the current hand-edited body.

**Rewrite behaviour:**
- Keep the plant-side quantization unchanged. The output on
  `V_m_bar_mps` is already in the correct pulse-granular form for a
  BML accumulator.
- The rewrite accumulates integer pulse *deltas* inside the LVDA at
  BML granularity, re-multiplying by 0.05 only at the end. This
  preserves integer arithmetic even though the model uses doubles.
  **Formal rule:** `V_m_pulses[axis] += round((V_m_bar_mps_now[axis] - V_m_bar_mps_prev[axis]) / 0.05)`
  at each BML read. Reduces round-off drift vs. accumulating
  quantized doubles directly.
- The rewrite does **not** re-implement §4.2.2.1-3 disagreement /
  zero / reasonableness tests in v1. These are failure-mode logic
  (A vs B counter divergence) and the plant delivers one deterministic
  value. Failure injection is deferred.

### 2. Gimbal resolver input — bits to degrees

**EDD §8.2.1:**
- Two resolvers per gimbal: fine (primary) and backup.
- Dual 11-bit counters per resolver with disagreement bit. Word
  layout: `DAAAAAAAAAAASSSSBBBBBBBBBB`.
- **Fine resolver: 0.00279 deg per LSB**, 5.625 deg per revolution,
  2016 counts full-scale.
- Backup resolver: 0.0893 deg per LSB, 180 deg per revolution, 2016
  counts full-scale.
- Attitude angle = fine resolver reading + high-order angle (count of
  5.625-deg revolutions since GRR, maintained in software).
- Disagreement bit asserted by LVDA circuitry if counters disagree by
  more than +3 / less than −4 bits.

**Modelled today where:**
`saturn_ib_stack.json` → `.../ST_124M_Stabilized_Platform/Fine_Resolver_Quantization`
with `quantum: 0.00279` (JSON L85170). Output quantum is in degrees;
feeds `LVDA_LVDC.inputs.Theta_deg[3]` via `ST_124M_Stabilized_Platform_Memory`
(unit-delay latch at the subsystem boundary).

**Rewrite behaviour:**
- Keep the plant-side fine-resolver quantization unchanged.
- The rewrite uses the quantized degree value directly; high-order
  angle accumulation is not needed when the platform model does not
  wrap the resolver (the plant delivers unwrapped radians → degrees).
- Backup resolver, disagreement bit, and reasonableness tests
  (§8.2.2) are **deferred** failure-mode logic in v1.

### 3. Attitude error command D/A — degrees to ladder bits

**EDD §8.2.3:**
- Attitude error commands transmitted to dual LVDA D/A ladder
  channels (A primary, B automatic standby).
- **"The least significant D/A converter bit equals 0.06 degrees"**
  (explicit EDD statement).
- Scale factor SF = 3000 bits per pirad (§8.3.2). Check: 1 pirad =
  π rad = 180°; 180°/3000 bits = 0.06° per bit ✓.
- Transmitted in sign-and-magnitude form.
- Magnitude and rate limits per channel defined in the Event Sequence
  Timeline Table of Part II; mission-specific.

**Modelled today where:**
`LVDA_LVDC.c` L443-447: `D_A_Converter_Quantization` with
**quantum = 0.0575**, upstream clamp to ±15.3° (L402-405), rate limit
via `Rate_Limiter` block. Output wired to `outputs.Psi_deg` (L4691).

**Discrepancy flagged:** the current code uses **0.0575° per LSB**;
the EDD specifies **0.06° per LSB**. These are different by about 4%.
The origin of 0.0575 in the obliq-2 model is not documented. Possible
hypotheses:

- Derivation from a scale factor of 3130 bits/pirad instead of 3000:
  180/3130 ≈ 0.0575. No EDD source for 3130.
- Transcription from a hand-tuned trim rather than the EDD spec.
- Legacy value from an earlier Saturn V EDD revision.

**Decision for the rewrite:** use **0.06°** per EDD §8.2.3. Record
the pre-rewrite value (0.0575) for A/B comparison when Phase D lands.
If the switch to 0.06 moves the trajectory residual, document as an
EDD-cited correction, not a tune.

**Magnitude and rate limits:** EDD says these are mission-dependent
(Part II timeline table). Current code uses ±15.3°. **Follow-up for
Phase A.2 close-out:** locate the Part II table for AS-205 (and
AS-206, since parameters are shared per the memory note) and cite
the specific ±X° values per flight phase. Place in `lvdc_params.h`
with EDD citations, not literals.

### 4. BML timing — nominal vs. variable

**EDD §4.1:** BML period varies pass-to-pass with LVDC load. For
AS-205/AS-206 simulation, treat as nominal 1.6 s.

**Modelled today where:** hard-coded 1.6 s gate in `LVDA_LVDC.c`
L1395 (`sample_tick % llround(1.6 / dt) == 0ULL`).

**Rewrite behaviour:** same nominal 1.6 s gate. No load-jitter model
in v1. Noted as a known fidelity gap.

## Summary table

| Point | EDD value | Current code | Plant or LVDA | Rewrite plan |
|-------|-----------|---------------|---------------|-------------|
| PIPA pulse | 0.05 m/s | 0.05 m/s (plant) | Plant `Accelerometer_Quantization` | Keep plant; accumulate integer pulse deltas in LVDA |
| Fine resolver | 0.00279° | 0.00279° (plant) | Plant `Fine_Resolver_Quantization` | Keep plant |
| D/A output | 0.06° | **0.0575°** (LVDA, discrepancy) | LVDA `D_A_Converter_Quantization` | **Correct to 0.06° per EDD §8.2.3** |
| Minor loop rate | 40 ms (boost) | 40 ms (plant FCC path) | FCC sub-system rate | Internal scheduler gate at 40 ms |
| BML rate | ≈ 1.6 s (variable) | 1.6 s fixed (LVDA) | LVDA internal gate | Same; jitter deferred |

## Quantization NOT modelled (v1)

- Accelerometer A/B disagreement and reasonableness tests (EDD §4.2.2).
- Backup resolver fallback (EDD §8.2.1).
- D/A ladder A/B channel switchover (EDD §8.2.3).
- LVDC internal word scaling (26-bit two's complement) — doubles
  throughout, per Phase A plan. The 0.0575 → 0.06 correction is
  about *interface* quantization, not internal arithmetic.
- BML pass-to-pass timing jitter.

These are failure-mode or finer-grained models and belong in a v2
after the clean EDD-anchored rewrite is blessed.

## Action items for the rewrite

1. **Correct `D_A_Converter_Quantization` quantum from 0.0575 to 0.06°**
   when the rewrite replaces the current body (Phase D), with EDD
   §8.2.3 citation. Record pre-switch residual delta.
2. **Model the minor loop internally at 40 ms** (25 Hz during boost).
   Confirm the FCC consumer is tolerant of a 40 ms rate (the parent
   model's FCC stage uses its own rate; the LVDA output is latched).
3. **Accumulate PIPA deltas as integer pulse counts** inside the LVDA
   between BML reads. Multiply by 0.05 only at the BML boundary.
4. **Locate Part II magnitude/rate limits** for AS-205 and record in
   `lvdc_params.h` with citation. Short follow-up before Phase C.
5. **Amend the Phase A plan doc** (`docs/LVDC_REWRITE_PLAN.md`) to
   replace "25 ms minor loop" with "40 ms minor loop" per EDD §4.1.
