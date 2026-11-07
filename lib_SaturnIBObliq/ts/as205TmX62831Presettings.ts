/**
 * AS-205/CSM-101 operational trajectory presettings from NASA-TM-X-62831
 * (markdown extract: reference/NASA-TM-X-62831/chandra/NASA-TM-X-62831.md).
 *
 * Variable **names** follow `lib_SaturnIB/model/AS205_presettings.m` (TN-era schema)
 * plus LVDC mask / mass symbols used by `saturn_ib_stack.mdl`.
 *
 * Values marked `source: 'tn-hold'` were not clearly cited in TM-X-62831 App B /
 * Table 3B/12; TN numbers are retained until a TM cite is found — do not invent.
 */

export type PresettingCite =
  | { table: string; note?: string }
  | { source: 'tn-hold'; note: string }

export type PresettingEntry = {
  value: number
  units: string
  cite: PresettingCite
}

const LB_TO_KG = 0.45359237

/** Table 12 S-IVB stage weight at separation (lbm) → kg */
const SIVB_SEP_LB = 305803
const SIVB_SEP_KG = SIVB_SEP_LB * LB_TO_KG

/**
 * Operational (TM-X-62831) pack. Keys are Obliq / MATLAB workspace names.
 */
export const AS205_TM_X_62831_PRESETTINGS: Record<string, PresettingEntry> = {
  // —— Site / aim (App B Table 3B AP note + mission criteria) ——
  A_z_deg: {
    value: 72.0,
    units: 'deg',
    cite: { table: 'Table 3B (AP note) / §II mission criteria', note: 'Flight azimuth E of N' }
  },
  pad_roll_L_deg: {
    value: 100.0,
    units: 'deg',
    cite: { table: '§II mission criteria', note: 'Pad-oriented launch azimuth' }
  },
  phi_L_deg: {
    value: 28.521963,
    units: 'deg',
    cite: { table: 'Table 3B (AP note)' }
  },
  phi_L_prime_deg: {
    // Geocentric launch-site latitude (AS205_presettings.m); distinct from geodetic φ_L
    value: 28.360795,
    units: 'deg',
    cite: {
      source: 'tn-hold / AS205_presettings.m',
      note: 'Geocentric φ′; AP Table 3B lists geodetic φ_L=28.521963 separately'
    }
  },
  lambda_L_deg: {
    value: -80.561141,
    units: 'deg',
    cite: { source: 'tn-hold', note: 'LC-34; confirm vs TM site listing' }
  },
  i_deg: {
    value: 31.605,
    units: 'deg',
    cite: { table: 'Table 3B (AP note)', note: 'Inclination i' }
  },
  lambda_0_deg: {
    value: 119.0,
    units: 'deg',
    cite: {
      table: 'Table 3B (AP note)',
      note: 'θ_N descending-node argument of desired orbit (was TN 102.329)'
    }
  },
  R_L_m: {
    value: 6373385.0,
    units: 'm',
    cite: { source: 'tn-hold', note: 'Platform R_L; confirm vs TM' }
  },

  // —— IGM terminal / timing (Table 3B) ——
  V_T_mps: {
    value: 7780.67,
    units: 'm/s',
    cite: { table: 'Table 3B VT / summary GCS' }
  },
  Xdotdot_VGT_mps2: {
    value: -9.15,
    units: 'm/s^2',
    cite: { table: 'Table 3B XVG (AP App B)', note: 'Was -9.1; AP Table 3B −9.15' }
  },
  /** IGM desired terminal radius component X_VT (not TN R_T geocentric cutoff). */
  R_T_m: {
    value: 6600405,
    units: 'm',
    cite: { table: 'Table 3B XVT (position)', note: 'Mapped to legacy R_T_m name used by MDL' }
  },
  Theta_T_deg: {
    value: 0.0,
    units: 'deg',
    cite: { table: 'Table 3B', note: 'YVT/ZVT position zero; terminal pitch hold 0' }
  },
  T3_IGM_sec: {
    value: 25.0,
    units: 's',
    cite: { table: 'Table 3B / §IV', note: 'TB3 → IGM initiate' }
  },
  epsilon_prime_sec: {
    value: 5.0,
    units: 's',
    cite: {
      table: 'Table 3B (cont’d)',
      note: 'T3i to freeze IGM (ε′); MDL Compare may still hardcode >3 — verify'
    }
  },
  epsilon_2_sec: {
    value: 25.0,
    units: 's',
    cite: {
      source: 'AS-206 EDD Part II Table 1-1 (satinstunitibm_7.md:597)',
      note: 'χ̃ / terminal steering start (K_1..K_4 → 0 per EDD §4.4.2.4). ' +
            'AS-205 TM Table 3B does not list this; AS-206 Table 1-1 gives 25.0. ' +
            'BN_1 and T_HSL at this same table matched our prior tn-hold values ' +
            'exactly (14.4 and 5.0), making AS-206 Part II the best available ' +
            'reference. Was 15.0 (tn-hold) — raised to 25.0 2026-11-06.'
    }
  },
  BN_1_sec: {
    value: 14.4,
    units: 's',
    cite: { source: 'tn-hold', note: 'Velocity-history / HSL related' }
  },
  T3_FM_sec: {
    value: 5.0,
    units: 's',
    cite: {
      table: 'Table 3B (cont’d)',
      note: 'Freeze IGM / ε′ band; was TN 6.08 (M/F)_S'
    }
  },
  DeltaV_b_mps: {
    value: 7.2381,
    units: 'm/s',
    cite: {
      doc: 'LVDC-EDD Part II (satinstunitibm_7)',
      table: 'Table 1-3 PRESETTINGS',
      note: 'Thrust decay velocity bias for S-IVB burn (source code C3)'
    }
  },
  V_GRD_mps: {
    value: 150.0,
    units: 'm/s',
    cite: { source: 'tn-hold', note: 'Velocity guard' }
  },
  T_HSL_sec: {
    value: 5.0,
    units: 's',
    cite: { source: 'tn-hold', note: 'Often equals ε′ band; TM freeze row is 5 s' }
  },

  // —— LVDC mask / artificial-τ (Table 3B cont’d) ——
  T10_sec: {
    value: 286.3,
    units: 's',
    cite: { table: 'Table 3B T1i', note: 'Also used as tau_1_i_preset / T10' }
  },
  tau_1_i_preset_sec: {
    value: 286.3,
    units: 's',
    cite: { table: 'Table 3B T1i' }
  },
  tau_3_i_preset_sec: {
    value: 159.8,
    units: 's',
    cite: { table: 'Table 3B T3i' }
  },
  tau_3_preset_sec: {
    value: 326.5,
    units: 's',
    cite: { table: 'Table 3B T3 (avg ṁ/m at 2nd stage IGM)' }
  },
  V_ex1_mps: {
    value: 4151.0,
    units: 'm/s',
    cite: { table: 'Table 3B Vex1' }
  },
  V_ex3_mps: {
    value: 4233.2,
    units: 'm/s',
    cite: { table: 'Table 3B Vex3' }
  },
  Tr1: {
    value: 1.8,
    units: '1',
    cite: { table: 'Table 3B TR1' }
  },
  Tr2: {
    value: 0.0,
    units: '1',
    cite: { table: 'Table 3B TR2' }
  },
  Tr3: {
    value: 0.0,
    units: '1',
    cite: { table: 'Table 3B TR3' }
  },
  T2_sec: {
    value: 0.0,
    units: 's',
    cite: { table: 'Table 3B T̃2' }
  },
  delta_T_sec: {
    value: 1.6,
    units: 's',
    cite: { source: 'tn-hold', note: 'IGM major cycle; TM ΔT_NOM row OCR unclear (R-AS7)' }
  },
  PC0_sec: {
    value: 0.0,
    units: 's',
    cite: { table: 'Table 3B PC0', note: 'Backup IGM staging time' }
  },
  tau_1_0_sec: {
    value: 536.0,
    units: 's',
    cite: { table: 'Table 3B τ10' }
  },
  C_0_sec: {
    value: 35.0,
    units: 's',
    cite: { table: 'Table 3B C0' }
  },
  A0_sec: {
    value: 0.0,
    units: 's',
    cite: { table: 'Table 3B', note: 'N/A in AP; was TN mask leftover 458.9' }
  },
  A1: {
    value: 0.0,
    units: '1',
    cite: { table: 'Table 3B', note: 'N/A in AP; was TN mask leftover −1.194' }
  },

  // —— Mass / flow (Table 3B * rows + Table 12) ——
  /** Mass at first motion (Table 3B M1). Replaces baked m_liftoff_kg 586593. */
  m_liftoff_kg: {
    value: 585943,
    units: 'kg',
    cite: { table: 'Table 3B M1', note: 'Also Table 12 liftoff ≈ 1,291,780 lbm' }
  },
  M1_kg: {
    value: 585943,
    units: 'kg',
    cite: { table: 'Table 3B M1' }
  },
  /** S-IVB stack mass at separation (Table 12 → kg). Replaces 137883. */
  m_separation_kg: {
    value: SIVB_SEP_KG,
    units: 'kg',
    cite: { table: 'Table 12 S-IVB Stage Weight at Separation', note: `${SIVB_SEP_LB} lbm` }
  },
  M02_kg: {
    value: 129761.6,
    units: 'kg',
    cite: {
      table: 'Table 3B M02',
      note: 'MDL Constant M_GR (TN M_0_2 = 124558.41) binds to this parameter'
    }
  },
  M03_kg: {
    value: 134374,
    units: 'kg',
    cite: { table: 'Table 3B M03', note: 'J-2 start cmd mass minus jettison' }
  },
  Mdot_1_kg_per_sec: {
    value: 239.7877,
    units: 'kg/s',
    cite: { table: 'Table 3B Ṁ1 (accel-failure biased row)' }
  },
  Mdot_3_kg_per_sec: {
    value: 183.1797,
    units: 'kg/s',
    cite: { table: 'Table 3B Ṁ3 (accel-failure biased row)' }
  },
  Mdot_2_sib_kg_per_sec: {
    value: 2820.8,
    units: 'kg/s',
    cite: { table: 'Table 3B Ṁ2', note: 'Avg S-IB flow first motion→TECO' }
  },
  F1_N: {
    value: 7600000,
    units: 'N',
    cite: { table: 'Table 3B F1', note: 'S-IB thrust (kg·m/s²)' }
  },
  F2_N: {
    value: 995355.2,
    units: 'N',
    cite: { table: 'Table 3B F2' }
  },
  F3_N: {
    value: 775441.26,
    units: 'N',
    cite: { table: 'Table 3B F3' }
  },

  // —— Physics (unchanged) ——
  mu_earth: {
    value: 3.986004418e14,
    units: 'm^3/s^2',
    cite: { source: 'tn-hold', note: 'Standard; Fischer Earth in TM narrative' }
  },
  R_earth_m: {
    value: 6371000.0,
    units: 'm',
    cite: { source: 'tn-hold', note: 'Crude radius helper' }
  },
  omega_E_rps: {
    value: 2.321e-5,
    units: '1/s',
    cite: { source: 'tn-hold', note: 'ω/π style mask helper' }
  }
}

/** Flat number map for mdl2obliq / emit. */
export function tmX62831WorkspaceNumbers(): Record<string, number> {
  const out: Record<string, number> = {}
  for (const [k, e] of Object.entries(AS205_TM_X_62831_PRESETTINGS)) {
    out[k] = e.value
  }
  return out
}

/** Entries still on TN pending a TM citation. */
export function tmX62831TnHoldKeys(): string[] {
  return Object.entries(AS205_TM_X_62831_PRESETTINGS)
    .filter(([, e]) => 'source' in e.cite && e.cite.source === 'tn-hold')
    .map(([k]) => k)
}
