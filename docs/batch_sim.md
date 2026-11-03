# batch_sim

Headless command-line driver for the Saturn-IB / Colossus249 simulation.
Reads inputs from a JSON file, runs the simulation as fast as the CPU
allows, and writes a final-state JSON plus a 1 Hz CSV time-series.

## Run

Run from `data/`, or supply absolute paths in the JSON `agc` section. `batch_sim` is in your build directory.

```
../build/batch_sim examples/AS-205-nominal.json
../build/batch_sim examples/AS-205-quiet.json --run-time 120
../build/batch_sim inputs.json --final out/final.json --csv out/trace.csv
```

### CLI

| Flag                 | Effect                                             |
|----------------------|----------------------------------------------------|
| `<inputs.json>`      | Positional; required.                              |
| `--run-time SEC`     | Overrides `sim.run_time_sec` from the JSON.        |
| `--final PATH`       | Overrides `sim.final_output`.                      |
| `--csv PATH`         | Overrides `sim.csv_output`.                        |
| `--igm-trace PATH`   | Write Layer-1 IGM interface CSV.                   |
| `--igm-trace-dt SEC` | IGM trace sample period (default **1.6** s).       |

## IGM trace

`--igm-trace` writes the IGM subsystem interfaces to CSV (schema: `src/sim/igm_trace.h`), filled from the plant's LVDC signals by `SaturnIBStack::FillIgmTrace`. Missing fields are written as `nan`. `scripts/compare_igm_trace.py` compares two such traces.

## inputs.json schema

All sections and fields are optional.  Anything omitted keeps the
default that the `apolloa` GUI uses.

```json
{
  "sim": {
    "run_time_sec": 800.0,            // total sim seconds (default 800)
    "timeStep":     0.01,             // outer SaturnIBStack::TimeStep delta
    "final_output": "final.json",
    "csv_output":   "results.csv"
  },
  "launch_date": {
    "year":   1968,  "month":  10,  "day":    11,
    "hour":   14,    "minute": 57,  "second": 45.0
  },
  "model": {
    "A_z_deg":        82.8200,                       // launch azimuth
    "CG_LLA_deg_m":   [28.521963, -80.561141, 34.7], // pad lat, lon, alt
    "T_L_prime_sec":  300.0,                         // T-minus at sim start
    "q_ECItoSM":      [1, 0, 0, 0]                   // (optional) initial IMU quaternion
  },
  "agc": {
    "rom_path":            "Colossus249.bin",
    "pad_load_file":       "TM-X-62831_PAD_LOAD.txt",    // "-" disables
    "pad_load_start_sec":  15.0
  }
}
```

## Output files

`final.json` — snapshot of every scalar plant output at the end of the
run, plus `elapsed_sim_sec`, `wall_sec`, `realtime_ratio`.

`results.csv` — one row per second of simulation time (plus t=0
initial state).  83 columns.

**Column naming conventions**

The plant emits three per-stage state vectors that partially
overlap.  In the CSV / JSON they are broken out with prefixes:

- `pad_*`  — OUT22, S-IB pad reference frame (mostly static, `pad_h_m`
  stays at the launch pad altitude for the entire run).
- `s1_*`   — OUT11.  Joined stack pre-staging (drives forward from
  liftoff at t=300 s), continues to track the dropped S-IB stage
  post-separation (t≈448 s).
- `s2_*`   — OUT12.  All zeros pre-staging, activates at separation to
  track the S-IVB + CSM (the primary payload trajectory).  Post-staging
  altitude climbs faster than `s1_*` due to S-IVB thrust.

Within each stage prefix the suffix names mirror the model block
names: `lat_deg`, `lon_deg`, `h_m`, `Xe_x_m` / `Xe_y_m` / `Xe_z_m`
(ECI position), `Ve_x_mps` / `Ve_y_mps` / `Ve_z_mps` (ECI velocity),
`q_ECI_q0..q3` (attitude quaternion), `omega_b_x_rps` / `_y` / `_z`
(body angular rates), `Vb_x_mps` / `_y` / `_z` (body-frame velocity),
`IMU_theta_x_deg` / `_y` / `_z`, `alpha_deg`, `Vb_mps`.

`s1_compare_c3` is a discrete flag whose semantic identification is TBD
(matches the model block name "Compare_c3", index 24 of OUT11).

Scalar (non-per-stage) columns: `FDAI_Roll_deg` / `Pitch` / `Yaw`,
`FDAI_RollDot_dps` / `Pitch` / `Yaw`, `CM_IMU_PIPA_x` / `y` / `z`,
`CM_IMU_OGA_rad` / `IGA` / `MGA`, staging booleans (`bLiftoff`,
`bStageSep`, `bIECO`, `bOECO`, `bS_IVB_EngineStart`),
`veh_q_ECI_q0..q3`, `BodyToSM_Phi_deg` / `Theta` / `Psi`.

## Verification

`ctest` in the build directory runs three scripts from `tests/batch/`:

- `determinism.sh` runs the nominal config twice and confirms the CSV is byte-identical and `final.json` differs only in `wall_sec` and `realtime_ratio`.
- `staging_events.sh` runs the full 800 s nominal config and checks that `bLiftoff`, `bIECO`, `bOECO`, `bStageSep` and `bS_IVB_EngineStart` fire in order at plausible times.
- `baseline.sh` compares CSV hashes with `tests/baselines/`.

## Exit codes

- `0` — success
- `2` — bad CLI / config JSON
- `3` — the model reported an error status during the run

## Design notes

- The clock is advanced with `SimClock::FixedTimeUpdate()`, so runs are independent of wall time.
- The log goes to `/dev/null`, so batch runs do not write `saturn_a_log.txt`.
- The AGC is always present and stepped alongside the plant. Set `agc.pad_load_file` to `"-"` to skip the automatic uplink.
- `main()` ends with `std::_Exit(0)` to skip a destructor-ordering crash at teardown. CSV and JSON files are flushed first.
