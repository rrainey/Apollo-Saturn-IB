# Chi_Y divergence — root-cause notes

Dual-harness compare @ 1.6 s (`igm-compare-1000.md`, Product15 stash).

## Confirmed

### 1. T1 / enable are healthy
First-phase `T_1_i` matches RTW sample-for-sample. Enable ~491 s (T3≥44).

### 2. Missing APᵀ on terminal Product15 (sign bug) — **fixed**
RTW `<S356>/Math Function5` **transposes** AP before `Product15 = APᵀ·v_cmd`.
Obliq used `AP·v_cmd`. After switching to `IGM_AP_DCM_T`:
- Chi_Y sign matches RTW (negative tip)
- Magnitude still off

### 3. Pre-IGM Chi
- Obliq: hold at 0 when disabled (gated)
- RTW: Time-Tilt writes Chi DSM (~−40…−61°) before IGM enable  
Known gap until Time-Tilt → Chi DSM is ported.

### 4. Remaining divergence is **upstream of Product15** (`chi_a_cmd`)

At **t≈500** (both enabled, rates still sane):

| | RTW | Obliq |
|--|-----|-------|
| `chi_a_cmd` (rad) | **0.816** | **0.087** |
| `phi_T` (rad) | −1.466 | −1.549 |
| `v15` | [0.59, 0.02, 0.81] | [−0.10, 0.40, 0.91] |
| `Chi_Y` (deg) | **−54** | **−96** |

`Chi_Y = deg(atan2(−v15_z, v15_x))` is consistent on Obliq with its `v15`.
So Product15/atan2 wiring is fine; **`chi_a_cmd` / ΔV / rate path** does not match RTW.

At **t≈550** Obliq `chi_a_cmd` explodes to **~15 rad** (rate term runaway);
RTW stays ~0.7 rad. That matches later Chi NaNs / wild swings.

## Next probes (ordered) — updated

### Primary remaining bug: raw `χ_α = atan(dVx/dVz)` (~4× small)

At **t=500** (stash + Obliq fill):

| | RTW | Obliq |
|--|-----|-------|
| `chi_a_rad` (raw atan) | **0.424** | **0.109** |
| `chi_a_cmd` | 0.816 | 0.087 |
| `sw_a` (rate) | (not logged) | **0** |
| `Chi_Y` | −54° | −96° |

cmd formula matches on Obliq when `sw=0`:  
`cmd = χ_α − φ_T − π/2` → 0.087.  
So early First Phase is **not** a rate bug; **ΔV components disagree**.

### Root cause (MDL block inventory) — **fixed in plant**

MDL `<S384>` / RTW Product10 feeds **G_S** (gravity) into ΔV and S420
quadratic terms. Obliq had wired **`A_m_bar` (XYZdotdot / specific force)**
into `IGM_Product15_Chi` as `AS_mps2`.

Gap matrix: `examples/saturn-ib/IGM_CHI_MDL_GAP_MATRIX.md`.  
Fix: rename port → `G_S_mps2`; LVDC wires `G_S_bar_mps2`; **full plant regen + 1000 s run done**.

### Post-regen @ t=500.8 — **not closed**

| | RTW | Obliq G_S |
|--|-----|-----------|
| `chi_a_rad` | **0.421** | **−0.051** |
| `Chi_Y` | **−54.5°** | **−104.8°** |
| `phi_T` | −1.466 | −1.555 |
| `Gain1_h` | sane | **−6.6e3** (enable **−2.7e4**) |

G_S was necessary but not sufficient. Bad ΔV/φ_T → Gain1_h explosion →
`T_3_i_eff_pos` clamps T3_eff→0. Next: MDL φ_T / Product9–10 / ΔV blocks.

### Secondary: rate runaway

By **~t528** Obliq `chi_a_cmd` → tens of rad; RTW stays ~0.7.  
Fed by bad ΔV/`K_p` / Gain1_h.

## Code touchpoints

- Obliq APᵀ: `igmChiAssembly.ts` (`IGM_AP_DCM_T`), `igmProduct15Obliq.ts` Product15 input
- RTW stash: `Saturn_IB_Stack.c` `#ifdef IGM_TRACE_STASH`, `igm_trace_stash.[ch]`
- Fill: `SaturnIBPlantObliq_fill_igm_trace`, `SaturnIBStack::FillIgmTrace`
