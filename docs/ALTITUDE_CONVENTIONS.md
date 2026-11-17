# Altitude Conventions

This repository exposes vehicle altitude through several channels that use
*different* Earth reference shapes and altitude definitions.  The numbers
are internally consistent in each place, but mixing them in a single plot
or readout will show up as a latitude-dependent bias of up to ~22 km.  This
page is the one-stop reference for which channel means what.

## Short answer

| Channel                              | Earth reference                                       | Altitude definition                                                              |
|--------------------------------------|-------------------------------------------------------|----------------------------------------------------------------------------------|
| `OUT11[2]`, `OUT12[2]`, `OUT22[2]`   | WGS-84 ellipsoid (a=6378137, b=6356752.3)             | Geodetic — along the local ellipsoid normal                                      |
| AGC P11 "H" readout (N62)            | Scalar sphere EARTHR ≈ 6,373,338 m                    | `|R_vehicle| − EARTHR` (no shape correction)                                     |
| TM-X-62831 altitude columns          | Fischer 1960 ellipsoid (a=6378165, b=6356783)         | Along the geocentric position vector to the ellipsoid                            |

The Apollo-program standard, per *NASA MSC MA-1461, Project Apollo
Coordinate System Standards* (Apollo Program Director, 1 Jun 1965), is the
1960 Fischer ellipsoid:

> **REFERENCE ELLIPSOID** is an ellipsoid chosen to best fit the geoid or
> some portion of it.  For the Earth, an oblate spheroid is normally used.
> **The Apollo standard is the 1960 Fischer ellipsoid.** *(Glossary, p.18)*

The LVDC presettings in `reference/LVDC-EDD/.../satinstunitibm_7` confirm
the same numbers: `a_e = 6,378,165.0 m`, `b = 6,356,783.0 m`.

## Why the numbers differ

Fischer 1960 and WGS-84 agree in shape to within ~30 m anywhere on Earth:

| Parameter       | Fischer 1960   | WGS-84           | Δ         |
|-----------------|----------------|------------------|-----------|
| Equatorial a    | 6,378,165 m    | 6,378,137 m      | 28 m      |
| Polar b         | 6,356,783 m    | 6,356,752.3 m    | 31 m      |
| Flattening 1/f  | 298.3          | 298.257223563    | ~0.04     |

So "WGS-84 vs Fischer" is at the tens-of-meters scale.  But the plant's
geodetic altitude vs TM-X-62831's geocentric-radial altitude differs by
another ~100–300 m at mid-latitudes for a 200–300 km orbit, because at
~32° N the geocentric and geodetic latitudes differ by ~11 arcmin and
"altitude along the local normal" and "altitude along the geocentric
position vector" fall at slightly different places on the ellipsoid.
Combined, plant WGS-84-geodetic vs TM-X-62831 Fischer-geocentric-radial is
typically a few hundred metres apart.

The AGC's single-scalar `|R| − EARTHR` form is the big offender: it uses
no shape correction at all.  At the equator it reads ~4.8 km low vs the
plant; near the northern/southern orbital extremes it can read ~22 km
high.  This is purely a display convention — the AGC's state vector is
Cartesian ECI and is not biased by its own altitude-readout formula.

## Picking the right one when comparing

- **Comparing against TM-X-62831 Tables 3/4 altitude columns:** use the
  Fischer-geocentric-radial altitude.  Compute as
  `h_fischer = |X_eci| − R_Fischer(φ_geocentric)` where
  `R_Fischer(φ_c) = a·b / sqrt((a·sin φ_c)² + (b·cos φ_c)²)`
  with `a = 6,378,165`, `b = 6,356,783`.

- **Comparing against the AGC's P11 "H" display:** subtract the same
  EARTHR constant from the plant's `|X_eci|`.  Do *not* subtract
  `R_Fischer(φ)` — the AGC doesn't.

- **Everyday lat/lon/altitude readout for the GUI:** the existing WGS-84
  geodetic channels are fine; this is what every modern tool
  (Google Earth, GPS receivers, etc.) uses.

## Project Apollo Standard Coordinate Systems (PASCs) relevant here

From MSC MA-1461:

- **PASCs 4 — Geocentric Inertial.**  Origin at Earth center, Z along
  Earth's mean rotational axis, X toward mean vernal equinox.  Epoch
  "generally the nearest beginning of a Besselian year," but other epochs
  are allowed "if clearly stated."  Our `LaunchDate` scenario field plays
  this role; the epoch is documented via that field.

- **PASCs 10 — Earth-Fixed Launch Site.**  Origin at intersection of
  Fischer ellipsoid and the normal through the launch site.  Z axis
  parallel to the Earth-fixed aiming azimuth, defined at GRR.

- **PASCs 13 — Launch Vehicle Navigation.**  The frame TM-X-62831's
  plumbline columns live in.  Translatable from the Platform-Accelerometer
  system (PASCs 12) **at guidance reference release.**  See
  [the frame-anchoring known limitation in README.md](../README.md#simulation-fidelity)
  for why the current plant anchors this frame at sim-epoch instead of GRR
  and the open audit that follows from it.

## References

- NASA MSC MA-1461, *Project Apollo Coordinate System Standards*,
  Apollo Program Director, 1 Jun 1965.  Downloaded copy in this repo is
  not checked in; see the README for how to obtain it.
- NASA TM-X-62831, *AS-205/CSM-101 Launch Vehicle Operational Trajectory,
  Revision 1* — see glossary (Table 2) for its use of PASCs 13 and
  Fischer reference.
- LVDC Equation Defining Document (`reference/LVDC-EDD/`), appendix in
  `satinstunitibm_7` — presettings table citing `a_e = 6,378,165`,
  `b = 6,356,783`.
