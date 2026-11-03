#!/usr/bin/env python3
"""Summarize imu_pipa_cdu.csv around liftoff (SM + AGC RN/VN/REFSMMAT)."""
from __future__ import annotations

import csv
import math
import sys
from pathlib import Path


def near(rows, t0: float):
    return min(rows, key=lambda r: abs(float(r["t_sec"]) - t0))


def main() -> int:
    path = Path(sys.argv[1] if len(sys.argv) > 1 else "imu_pipa_cdu.csv")
    rows = list(csv.DictReader(path.open()))
    if not rows:
        print("empty", path)
        return 1
    if "agc_rn_x" not in rows[0]:
        print("CSV missing agc_rn_* columns — rebuild/re-run with latest IMU_DEBUG")
        return 1

    print(f"{path}: {len(rows)} rows  A_z={rows[0].get('A_z_deg')}")
    for t0 in (90.0, 250.0, 300.0, 301.0, 360.0):
        r = near(rows, t0)
        keys = [
            "t_sec",
            "liftoff",
            "imu_mode",
            "sm_z_dot_down",
            "sm_x_dot_az",
            "rn_err_m",
            "vn_err_mps",
            "refsm_x_dot_smx",
            "refsm_z_dot_smz",
            "agc_piptime_sec",
        ]
        bits = " ".join(f"{k}={r.get(k, '?')}" for k in keys)
        print(bits)
        if "agc_rn_x" in r:
            print(
                f"  RN=({r['agc_rn_x']},{r['agc_rn_y']},{r['agc_rn_z']}) "
                f"plant Xe=({r['out22_xe_x']},{r['out22_xe_y']},{r['out22_xe_z']})"
            )
            print(
                f"  VN=({r['agc_vn_x']},{r['agc_vn_y']},{r['agc_vn_z']}) "
                f"plant Ve=({r['out22_ve_x']},{r['out22_ve_y']},{r['out22_ve_z']})"
            )
            for label, a in (
                ("X", ("refsm_xx", "refsm_xy", "refsm_xz")),
                ("Z", ("refsm_zx", "refsm_zy", "refsm_zz")),
            ):
                v = [float(r[k]) for k in a]
                n = math.sqrt(sum(x * x for x in v))
                print(f"  REFSMMAT {label}={v} ||= {n:.4f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
