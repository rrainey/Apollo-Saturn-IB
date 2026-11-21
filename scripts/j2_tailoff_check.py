#!/usr/bin/env python3
"""
J-2 tailoff standalone check.

Euler-integrate the S-IVB J-2 engine thrust tailoff over 2.0 seconds
in 0.01 s steps using the plant's Thrust_Decay lookup table. Applies
the thrust along the vehicle velocity direction (simple 1-D assumption)
and depletes mass by the integrated m_dot.

Compares the resulting ΔV against:
 - EDD Part II Table 1-3 preset (AS-206)         7.2381 m/s
 - EDD Rev-C Part II Table 2-3 (AS-207)          6.5727 m/s
 - EDD Rev-K Part II Table 1-3 (SA-210 / ASTP)   6.7118 m/s
 - Apollo 7 (= AS-205) Table 9-III, predicted    5.2    m/s
 - Apollo 7 (= AS-205) Table 9-III, flight       6.75   m/s

Usage:
    j2_tailoff_check.py [--mass_kg M] [--V0 V] [--F0 F] [--mdot MDOT]

All inputs are optional. Defaults come from the plant model and Apollo 7:
    --mass_kg 30767     Apollo 7 Table 9-III cutoff mass (best estimate)
    --V0      7775.0    plant |V_e| just before cutoff command (sim)
    --F0      783274    plant J_2 Thrust_vs_Time clamped at low-thrust mode (N)
    --mdot    208.0     plant J_2 m_dot at cutoff (low-thrust mode) (kg/s)
"""

import argparse
import sys


# Plant Thrust_Decay lookup table
# model/saturn_ib_stack.json
# S_IVB_Stage/J_2_Engine/Thrust_Model/Thrust_Decay
THRUST_DECAY_T = [
    0.0, 0.1, 0.15, 0.18, 0.20, 0.22, 0.24, 0.26, 0.28, 0.30,
    0.40, 0.50, 0.75, 1.00, 1.50, 2.00,
]
THRUST_DECAY_F = [
    1.000, 1.000, 1.000, 0.980, 0.850, 0.550, 0.300, 0.160, 0.100, 0.070,
    0.045, 0.033, 0.020, 0.013, 0.007, 0.000,
]


def thrust_fraction(t_since_ECC):
    """Piecewise-linear interpolation of the Thrust_Decay lookup."""
    if t_since_ECC <= THRUST_DECAY_T[0]:
        return THRUST_DECAY_F[0]
    if t_since_ECC >= THRUST_DECAY_T[-1]:
        return THRUST_DECAY_F[-1]
    for i in range(len(THRUST_DECAY_T) - 1):
        t_lo, t_hi = THRUST_DECAY_T[i], THRUST_DECAY_T[i + 1]
        if t_lo <= t_since_ECC <= t_hi:
            f_lo, f_hi = THRUST_DECAY_F[i], THRUST_DECAY_F[i + 1]
            u = (t_since_ECC - t_lo) / (t_hi - t_lo)
            return f_lo + u * (f_hi - f_lo)
    return 0.0


def integrate(mass0_kg, V0_mps, F0_N, mdot0_kgps, dt=0.01, T=2.0):
    """Euler integrate thrust-along-velocity for T seconds."""
    rows = []
    mass = mass0_kg
    V = V0_mps
    impulse = 0.0
    propellant_used = 0.0
    t = 0.0
    while t <= T + 1e-9:
        frac = thrust_fraction(t)
        F = F0_N * frac
        mdot = mdot0_kgps * frac
        a = F / mass
        rows.append((t, F, a, mass, V, impulse))
        # Advance
        V += a * dt
        impulse += F * dt
        propellant_used += mdot * dt
        mass -= mdot * dt
        t += dt
    return rows, impulse, propellant_used


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--mass_kg", type=float, default=30767.0,
                    help="vehicle mass at ECC (kg)")
    ap.add_argument("--V0", type=float, default=7775.0,
                    help="vehicle |V| at ECC (m/s)")
    ap.add_argument("--F0", type=float, default=783274.0,
                    help="J-2 thrust at ECC, N (= plant Thrust_vs_Time low-thrust plateau)")
    ap.add_argument("--mdot", type=float, default=208.0,
                    help="J-2 mdot at ECC, kg/s (low-thrust mode)")
    ap.add_argument("--dt", type=float, default=0.01, help="integration step (s)")
    ap.add_argument("--T",  type=float, default=2.0,  help="integration horizon (s)")
    ap.add_argument("--quiet", action="store_true",
                    help="skip per-step table, print only summary")
    args = ap.parse_args()

    rows, impulse, propellant = integrate(
        args.mass_kg, args.V0, args.F0, args.mdot, args.dt, args.T)

    if not args.quiet:
        print(f"{'t_s':>6}  {'F_kN':>7}  {'a_m/s^2':>8}  {'mass_kg':>9}  "
              f"{'|V|_mps':>10}  {'impulse_kNs':>12}")
        # Thin it to every 5-step sample + inflection points for readability
        show_times = {0.0, 0.1, 0.15, 0.18, 0.20, 0.22, 0.24, 0.26, 0.28, 0.30,
                      0.40, 0.50, 0.75, 1.00, 1.50, 2.00}
        for t, F, a, m, V, I in rows:
            if any(abs(t - st) < 1e-6 for st in show_times) or \
               (int(round(t * 100)) % 10 == 0 and t < 0.4):
                print(f"{t:6.2f}  {F / 1e3:7.1f}  {a:8.3f}  {m:9.1f}  "
                      f"{V:10.3f}  {I / 1e3:12.3f}")

    t_end, F_end, a_end, m_end, V_end, I_end = rows[-1]
    dV = V_end - args.V0

    print()
    print(f"=== Standalone J-2 tailoff integrator ===")
    print(f"inputs:")
    print(f"  mass at ECC       = {args.mass_kg:>10.1f} kg")
    print(f"  |V| at ECC        = {args.V0:>10.3f} m/s")
    print(f"  F_full at ECC     = {args.F0 / 1e3:>10.2f} kN")
    print(f"  m_dot at ECC      = {args.mdot:>10.2f} kg/s")
    print(f"  Thrust_Decay      = plant lookup (model/saturn_ib_stack.json)")
    print(f"  integration step  = {args.dt} s, horizon = {args.T} s, scheme = Euler")
    print()
    print(f"results:")
    print(f"  final |V|         = {V_end:>10.3f} m/s")
    print(f"  tailoff ΔV        = {dV:>10.3f} m/s")
    print(f"  total impulse     = {I_end / 1e3:>10.2f} kN·s")
    print(f"  propellant used   = {propellant:>10.2f} kg")
    print(f"  mass at end       = {m_end:>10.1f} kg")
    print()
    print(f"comparison:")
    print(f"  Standalone          = {dV:.3f} m/s")
    print(f"  Apollo 7 predicted  = 5.200 m/s   (Table 9-III)")
    print(f"  Apollo 7 flight     = 6.750 m/s   (Table 9-III engine data)")
    print(f"  EDD Rev-C AS-207    = 6.573 m/s   (Table 2-3)")
    print(f"  EDD Rev-K ASTP      = 6.712 m/s   (Table 1-3)")
    print(f"  EDD Rev-A AS-206    = 7.238 m/s   (Table 1-3; current LVDC preset)")


if __name__ == "__main__":
    main()
