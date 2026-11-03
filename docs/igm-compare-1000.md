# IGM trace compare (Layer-1)

- RTW: `/tmp/igm-rtw-p15.csv`
- Obliq: `/tmp/obliq-gs-fix/igm-obliq.csv`
- Paired rows: **351** (join tol ±0.8 s)
- Gates: rel=0.001, abs=1e-06

| Field | n | n_nan | max\|Δ\| | max rel | fail | Status |
|-------|---|-------|---------|---------|------|--------|
| igm_enable | 351 | 0 | 0 | 0 | 0 | ok |
| T_1_i_sec | 351 | 0 | 286.9 | 1 | 1 | FAIL |
| T_3_i_sec | 351 | 0 | 116 | 1 | 44 | FAIL |
| T_3_eff_sec | 351 | 0 | 118.7 | 1 | 43 | FAIL |
| nIGMMode | 351 | 0 | 0 | 0 | 0 | ok |
| nHSLActive | 351 | 0 | 0 | 0 | 0 | ok |
| bCutoff | 351 | 0 | 0 | 0 | 0 | ok |
| Chi_Y_deg | 351 | 0 | 234.6 | 1.947 | 157 | FAIL |
| Chi_Z_deg | 351 | 0 | 43.48 | 1 | 44 | FAIL |
| Chi_cmd_0 | 351 | 0 | 0 | 0 | 0 | ok |
| Chi_cmd_1 | 351 | 0 | 234.6 | 1.947 | 157 | FAIL |
| Chi_cmd_2 | 351 | 0 | 43.48 | 1 | 44 | FAIL |
| SMCY_rad | 351 | 0 | 0.09236 | 1 | 34 | FAIL |
| SMCZ_rad | 351 | 0 | 0.01016 | 1 | 34 | FAIL |
| Psi_P_deg | 0 | 351 | 0 | 0 | 0 | skip |
| Psi_Y_deg | 0 | 351 | 0 | 0 | 0 | skip |
| XS_x | 351 | 0 | 6.667e+04 | 0.01025 | 72 | FAIL |
| XS_y | 351 | 0 | 3.506e+05 | 1.996 | 337 | FAIL |
| XS_z | 351 | 0 | 4.662e+05 | 1.972 | 345 | FAIL |
| VS_x | 351 | 0 | 1006 | 1.952 | 350 | FAIL |
| VS_y | 351 | 0 | 2893 | 1.945 | 341 | FAIL |
| VS_z | 351 | 0 | 3985 | 1.588 | 168 | FAIL |
| A_x | 351 | 0 | 5819 | 1 | 350 | FAIL |
| A_y | 351 | 0 | 62.81 | 1.989 | 350 | FAIL |
| A_z | 351 | 0 | 2498 | 1.002 | 345 | FAIL |
| elev_cmd_deg | 0 | 351 | 0 | 0 | 0 | skip |
| phi_T_rad | 43 | 308 | 0.1365 | 0.08534 | 43 | FAIL |
| chi_a_cmd_rad | 43 | 308 | 171.3 | 1.131 | 43 | FAIL |
| chi_b_cmd_rad | 43 | 308 | 2.855 | 1.003 | 43 | FAIL |
| v15_x | 43 | 308 | 1.283 | 1.952 | 43 | FAIL |
| v15_y | 43 | 308 | 0.977 | 0.9771 | 43 | FAIL |
| v15_z | 43 | 308 | 1.83 | 1.925 | 43 | FAIL |
| Gain1_h | 43 | 308 | 1.066e+04 | 0.9952 | 43 | FAIL |
| chi_a_rad | 43 | 308 | 0.5162 | 1.301 | 43 | FAIL |
| sw_a | 0 | 351 | 0 | 0 | 0 | skip |

**Result: FAIL**
