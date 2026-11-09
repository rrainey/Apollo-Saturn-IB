# apolloa: the interactive panel

An OpenGL/GLUT DSKY and FDAI panel running the plant and yaAGC at wall-clock pace (about 60 Hz). Left-click the DSKY buttons to send keystrokes to the AGC. The panel layout is in `data/panel/*.json`.

`apolloa` links both `saturnib` (plant + IMU) and `saturnib_cmc` (the CMC
bridge).  `SaturnStartupHelper` constructs a `SaturnIBAgcBridge` which
attaches itself to the plant through `IAgcBridge` and is the `SimModel`
the clock ticks.  See the "Architecture at a glance" diagram in the top
README.

Run it from `data/` so it can find the ROM, pad load and textures:

```bash
cd data
../build/apolloa
```

**Startup sequence** (visible on the panel and in `console.log`):

| t (sim) | Event |
|---------|-------|
| 0       | ISS TURN ON asserted; IMU in warm-up. |
| ~15     | `PAD LOAD STARTED` — automatic uplink of erasable memory. |
| ~55     | `UPLINK COMPLETE`; KEY REL fires to dismiss UPLK ACTY. |
| ~90     | ISS turn-on delay complete (ch012); **then** first real NO ATT off arms SV (+45 s). Early DSPTAB clears before this are ignored. |
| ~90–135 | Pilot: enter **P00** (required for P27). |
| ~135    | Auto **state-vector** uplink (armed NO ATT off + 45 s): V37E 00E, V71E, … KEY REL. |
| after SV | Pilot: **P01** → P02 for launch (NO ATT cycles; does not re-arm SV). |
| 300     | LIFTOFF (`T_L_prime_sec` default). |
| ~448    | S-IB stage separation; S-IVB ignition. |

**IMU / PIPA / CDU debug capture** (P11 Noun-62 triage):

```bash
cd data
IMU_DEBUG=1 ../build/apolloa 2> console.log
# → writes imu_pipa_cdu.csv at 1 Hz (mode, PIPAs, CDUs, resolvers, Ve/h,
#    SM triad quality, AGC RN/VN/REFSMMAT vs plant)
```

Launch azimuth default is **72°** (TM-X-62831), not the older ~82.82°.
GSOP preferred CSM SM at LO: X along azimuth, Z down — on a vertical
Saturn that is the IGA≈±90°/MGA≈0° gimbal set (not the horizontal-launch
IGA=0/MGA=−90 recipe).

At LO expect:
- `sm_z_dot_down`≈1, `sm_x_dot_az`≈1
- after P11: `refsm_x_dot_smx`≈1, `refsm_z_dot_smz`≈1 (needs pad-load **UNITW**=(0,0,1))
- `rn_err`/`vn_err` vs OUT22 are only meaningful on pad — OUT22 is wired to
  pad ICs only (plant); after liftoff use AGC RN/VN trends vs OUT11 h/Vb

Compare `pipa_injected_cum_* × 0.0585 m/s` to plant Δ|Ve|.

**DSKY procedure for VERB 37 (Change Program):**

    V - 3 - 7 - ENTR - <MM digit> - <MM digit> - ENTR
        (do NOT press the NOUN key)

Pressing NOUN in this sequence silently loses the digits.

**Files written to the working directory:**

| File | Purpose |
|------|---------|
| `saturn_a_log.txt` | Log; truncated at each start. |
| `imu_pipa_cdu.csv` | Only with `IMU_DEBUG=1`. |

**Scripted run:** `tests/gui/run_scenario.py` drives this procedure under Xvfb and captures screenshots.
