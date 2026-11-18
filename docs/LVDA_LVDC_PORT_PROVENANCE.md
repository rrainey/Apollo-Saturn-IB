# LVDA/LVDC boundary-port provenance

**Scope:** every input port on the `LVDA_LVDC` native atomic subsystem as
exposed by `model/saturn_ib_stack.json` and consumed in
`lib_SaturnIBObliq/native-snapshots/lvda-lvdc/LVDA_LVDC.c`.

**Purpose:** before the `lvdc-rewrite` effort, pin down which ports are
real sensor / discrete inputs to the LVDA/LVDC chain, and which are
legacy wiring left over from an earlier codegen topology. The rewrite
must not take any dependency on a "plant ground truth" port.

**Audit date:** 2026-11-18.

## Summary

Four ports carry real data into the current body. Nine ports are wired
on the boundary but have zero reads in the body; they are codegen
artifacts from the pre-sensor-path-leak-fix era (see
`memory/project_lvdc_sensor_path_leak.md` and
`memory/project_lvdc_sframe_eqns_4_3_1_5.md`).

No remaining input is classed as *questionable*.

## Inputs actually used

| Port | EDD reference | Role | Read sites (LVDA_LVDC.c) |
|------|---------------|------|---------------------------|
| `V_m_bar_mps[3]` | §4.2.1-8 | ST-124M PIPA integrated velocity in plumbline frame | L785, L1389, L1402-04, L4750 |
| `Theta_deg[3]` | §2.2.8, §4.2 | ST-124M resolver gimbal angles (platform-to-body) | L285, L332, L4703 (pass-through) |
| `bLiftoff` | §3.4, §7 | Umbilical disconnect DIN; starts T1 timebase, arms GRR latch | L1378, L4632 |
| `bSIBPropellantSensorDry` | §4.4.1 end, §7 | S-IB low-level sensor DIN; starts T2 timebase | L4645 |

**Internal (not boundary) imports:** the GRR latch reads
`signals.LVDC_S_Frame_Position_Velocity_Calculations_Initial_Position_and_Velocity_Eqns_3_4_3_4_Mux{1,2}`
(L1380-84). These are EDD Eqs. 3.4.3 / 3.4.4 computed inside the
subsystem from preset parameters `PARAM_R_L_m`, `PARAM_A_z_deg`,
`PARAM_phi_L_deg`, `PARAM_phi_L_prime_deg`. They are not a plant
dependency.

## Inputs on the boundary but unused in the body

| Port | Origin | Why it was on the boundary |
|------|--------|-----------------------------|
| `MES_DCM[3][3]` | plant `MES_Transform` | Former plumbline / body transform used by the pre-sensor-path-leak-fix Switch1/2/3/5 nav path. Removed at the Product18 site (see L1342-48 and L2035-36 comments). |
| `PAD_Ve_mps[3]` | plant `Initial_Conditions` | Pad-relative velocity, former nav path. Removed. |
| `PAD_Xe_m[3]` | plant `Initial_Conditions` | Pad-relative position, former nav path. Removed. |
| `S_IVB_Ve_mps[3]` | plant `S_IVB_Stage` | S-IVB plant velocity, former nav switch path. Removed. |
| `S_IVB_Xe_m[3]` | plant `S_IVB_Stage` | S-IVB plant position, former nav switch path. Removed. |
| `Ve_mps[3]` | plant root `v_b` / `Ve` | Plant ground-truth inertial velocity. Removed. |
| `Xe_m[3]` | plant root `r_i` / `Xe` | Plant ground-truth inertial position. Removed. |
| `bLiftoff__tag` | crossing-tag auto-promotion | Duplicate of `bLiftoff` lifted to a crossing tag by obliq-2 codegen; never read. |
| `bStageSep` | self-feedback from this subsystem | Output discrete `bSIBStageSep` fed back as an input. Per `cgen-manifest.json` warning the codegen relinked it to the local driver. Not read in the body. |

The seven plant-ground-truth ports (`MES_DCM`, `PAD_Ve_mps`, `PAD_Xe_m`,
`S_IVB_Ve_mps`, `S_IVB_Xe_m`, `Ve_mps`, `Xe_m`) are the exact set that
the sensor-path leak audit excised. Keeping them wired on the boundary
costs one input copy per tick but has no behavioural effect.

## Outputs for reference (not audited in depth here)

- Sensor path: `R_S_bar_m[3]`, `T1_sec`
- Commanded attitude: `Psi_deg[3]`, `theta_deg[3]` (pass-through of
  `Theta_deg`)
- Discrete events: `bSIVEngineStart`, `bSIVCutoff`, `bIECO`, `bOECO`,
  `bSimulinkSIVBEnable`, `bUllageMotorStart`, `bSIVBurn`, `bSIVCoast`,
  `bSwitchpoin5`, `bSIBStageSep`

## Implication for the rewrite

The rewrite target is a clean ABI with these legitimate inputs only:

| Port | Quantization in LVDA | Notes |
|------|----------------------|-------|
| `V_m_bar_mps[3]` | PIPA pulse size 0.05 m/s (per L1348 comment; confirm vs EDD §4.2.1) | Accumulated in the LVDA signal-conditioner from PIPA pulse train. The parent plant currently emits a quantized continuous value, not a pulse count. Quantization inventory (task #3) must reconcile this. |
| `Theta_deg[3]` | ADC resolution of the resolver chain (verify in EDD Part I §10 or equivalent hardware doc) | Also delivered on a discrete sample. |
| `bLiftoff` | 1-bit DIN | |
| `bSIBPropellantSensorDry` | 1-bit DIN | |

**Phase A/B/C action:** keep the current nine-port boundary on
`saturn_ib_stack.json` unchanged. The new C body consumes only the four
legitimate ports. Parent JSON stays unchanged; obliq-2 regen stays
unchanged; no risk of breaking shadow comparisons during development.

**Phase D action:** when the new module replaces the old at the
obliq-2 boundary, prune the nine unused ports from
`saturn_ib_stack.json`. Expected side effect: the eight
"auto-promoted crossing tags" warnings in
`lib_SaturnIBObliq/plant/generated/cgen-manifest.json` disappear.
Parent-model diff gets shown for approval before it is applied.

## Follow-ups

- Confirm PIPA pulse size against EDD §4.2.1 `{N_x_i, N_y_i, N_z_i}`
  format and scaling tables (Phase A.2 quantization spec).
- Confirm resolver ADC quantum for `Theta_deg` against EDD Part I §10
  or the LVDA hardware spec.
- Decide whether the rewrite re-exposes `theta_deg` (pass-through of
  `Theta_deg`) as an output or lets the parent read `Theta_deg`
  directly from the ST-124M upstream of the LVDC.
