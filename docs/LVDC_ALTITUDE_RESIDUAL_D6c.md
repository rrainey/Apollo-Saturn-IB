# Phase D.6c — Altitude residual investigation

**Branch:** `lvdc-rewrite`, build `4f06e6f`.
**Date:** 2026-11-18.
**Question:** Why does the rewrite's cutoff altitude read ~12 km
above TMX?

## Short answer: the 12 km number was a reporting error

The earlier `+12.4 km` altitude residual came from subtracting
the WGS-84 **equatorial** radius (6378.137 km) from the sim's
cutoff `|R|` (6606.81 km) and comparing against the real TMX
cutoff altitude. That subtraction uses the wrong ellipsoid
radius: at the vehicle's geocentric latitude (31.37° N) the
Fischer-1960 ellipsoid radius is 6372.35 km, not 6378.137 km.
Correcting the reference removes ~6 km of the apparent residual.

## Corrected numbers at TMX GCS (T+614.63 s)

TMX definition (page 23, Definitions & Symbols): "Altitude (ALT).
Vehicle altitude above the referenced ellipsoid measured along
the geocentric position vector."

So TMX ALT is `|R| − R_ellipsoid(lat_geocentric)`, measured
radially. The datum is Fischer-1960 (used throughout AS-205).

| Quantity | TMX Table 4 | Rewrite | Residual |
| --- | --- | --- | --- |
| `|R|` | 6600.29 km | 6606.81 km | +6.52 km |
| Altitude (Fischer-1960 geocentric-radial) | 227.94 km | 234.46 km | +6.52 km |
| Altitude (WGS-84 geodetic) | — | 234.48 km | +6.54 km vs TMX |
| Space-fixed `|V|` | 7780.67 m/s | 7770.74 m/s | −9.93 m/s |
| Geodetic latitude | 31.53° N | 31.53° N | match |

Three facts drop out:

1. **Sim's `s2_h_fischer_m` matches the TMX convention exactly**
   (both are "radius minus Fischer-1960 radius at the geocentric
   latitude"). `6606.81 − 234.46 = 6372.35 km` = Fischer radius
   at lat 31.37°. No datum mismatch.
2. **WGS-84 geodetic altitude differs from Fischer geocentric by
   only 20 m** at this geometry. Datum choice is not the issue.
3. **The residual is a real position error** — the vehicle flies
   6.5 km above the TMX nominal trajectory.

## Where the residual comes from

The +6.5 km position residual appears in BOTH the rewrite AND
the golden (both end at `|R| = 6606.81 km`). It is not a rewrite
regression. It matches the existing memory
`project_igm_pitchup_corrected_sign`: "+2 km HIGH at IGM init;
IGM still over-pitches." At cutoff the integrated altitude
offset is ~6.5 km.

Candidates for the shared source (not investigated in this
sub-phase):

- Plant-side thrust / mass flow curves off nominal by a few
  per cent — a small over-performance tilts the trajectory
  higher without changing cutoff |V|.
- Plant atmospheric drag profile (US-1976 vs PRA-63) — see
  `project_atmosphere_model_investigation`; estimated effect
  is sub-km, so most of the 6.5 km is likely thrust/mass side.
- A small bias in the IGM phi_T prediction common to both
  bodies (e.g. ROV scale, Theta_T handling); unlikely given
  both bodies reach the same |R|.

## Correction to the earlier report

The earlier "Phase D.6b" report quoted:
- Cutoff altitude (geodetic, WGS-84): 234.48 km
- "TMX cutoff altitude": ~222.1 km  (from |R| − 6378.137 km)

The second number was wrong. Real TMX cutoff altitude per
Table 4 is **227.94 km**. Correct altitude residual is
**+6.54 km**, not +12.4 km. Velocity residual (−2.69 m/s
vs V_T−ΔV_b or −9.93 m/s vs V_T) is unchanged.

## Follow-on

- Phase D.6b TMX validation script compares `|R|`
  (position magnitude) not `ALT` — it already captures the
  6.5 km residual correctly in the `R` columns. No change
  needed to the validation tool.
- Closing the 6.5 km residual is plant-side / shared-LVDC
  work, not an IGM-rewrite task.
- PASCS PDF OCR (`reference/PASCS/chandra/`) is in progress
  as a semi-permanent reference for future coordinate-system
  investigations.
