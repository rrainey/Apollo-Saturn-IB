#!/usr/bin/env python3
"""Compare two IGM Layer-1 trace CSVs (RTW vs Obliq).

Usage:
  python3 scripts/compare_igm_trace.py \\
    --rtw /tmp/igm-rtw.csv --obliq /tmp/igm-obliq.csv \\
    [--rel 1e-3] [--abs 1e-6] [--out report.md]

Joins on nearest t_sec (within half of median dt). Prints / writes a markdown
table of max |Δ| and relative error per column.
"""

from __future__ import annotations

import argparse
import csv
import math
import sys
from pathlib import Path


def load(path: Path) -> list[dict[str, float]]:
    rows: list[dict[str, float]] = []
    with path.open() as f:
        r = csv.DictReader(f)
        for row in r:
            d: dict[str, float] = {}
            for k, v in row.items():
                if v is None or v == "" or v.lower() == "nan":
                    d[k] = float("nan")
                else:
                    d[k] = float(v)
            rows.append(d)
    return rows


def nearest(rows: list[dict[str, float]], t: float, half_dt: float):
    best = None
    best_d = 1e99
    for r in rows:
        d = abs(r["t_sec"] - t)
        if d < best_d:
            best_d = d
            best = r
    if best is None or best_d > half_dt:
        return None
    return best


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--rtw", required=True, type=Path)
    ap.add_argument("--obliq", required=True, type=Path)
    ap.add_argument("--rel", type=float, default=1e-3)
    ap.add_argument("--abs", type=float, default=1e-6)
    ap.add_argument("--out", type=Path, default=None)
    args = ap.parse_args()

    rtw = load(args.rtw)
    obliq = load(args.obliq)
    if not rtw or not obliq:
        print("empty trace", file=sys.stderr)
        return 2

    # Sample cadence from RTW
    dts = [
        rtw[i]["t_sec"] - rtw[i - 1]["t_sec"]
        for i in range(1, min(20, len(rtw)))
    ]
    dt = sorted(dts)[len(dts) // 2] if dts else 1.6
    half = 0.5 * dt + 1e-9

    skip = {"t_sec"}
    cols = [c for c in rtw[0].keys() if c not in skip]

    stats: dict[str, dict[str, float]] = {
        c: {"n": 0, "n_nan": 0, "max_abs": 0.0, "max_rel": 0.0, "fail": 0}
        for c in cols
    }

    paired = 0
    for rr in rtw:
        oo = nearest(obliq, rr["t_sec"], half)
        if oo is None:
            continue
        paired += 1
        for c in cols:
            a, b = rr.get(c, float("nan")), oo.get(c, float("nan"))
            if math.isnan(a) or math.isnan(b):
                stats[c]["n_nan"] += 1
                continue
            stats[c]["n"] += 1
            d = abs(a - b)
            denom = max(abs(a), abs(b), 1e-30)
            rel = d / denom
            stats[c]["max_abs"] = max(stats[c]["max_abs"], d)
            stats[c]["max_rel"] = max(stats[c]["max_rel"], rel)
            if d > args.abs and rel > args.rel:
                stats[c]["fail"] += 1

    lines = [
        "# IGM trace compare (Layer-1)",
        "",
        f"- RTW: `{args.rtw}`",
        f"- Obliq: `{args.obliq}`",
        f"- Paired rows: **{paired}** (join tol ±{half:.4g} s)",
        f"- Gates: rel={args.rel:g}, abs={args.abs:g}",
        "",
        "| Field | n | n_nan | max\\|Δ\\| | max rel | fail | Status |",
        "|-------|---|-------|---------|---------|------|--------|",
    ]
    any_fail = False
    for c in cols:
        st = stats[c]
        if st["n"] == 0:
            status = "skip"
        elif st["fail"] > 0:
            status = "FAIL"
            any_fail = True
        else:
            status = "ok"
        lines.append(
            f"| {c} | {int(st['n'])} | {int(st['n_nan'])} | "
            f"{st['max_abs']:.4g} | {st['max_rel']:.4g} | {int(st['fail'])} | {status} |"
        )
    lines.append("")
    lines.append(f"**Result: {'FAIL' if any_fail else 'PASS'}**")
    text = "\n".join(lines) + "\n"
    print(text)
    if args.out:
        args.out.write_text(text)
        print(f"Wrote {args.out}", file=sys.stderr)
    return 1 if any_fail else 0


if __name__ == "__main__":
    sys.exit(main())
