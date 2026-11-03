#!/bin/bash
# Run the nominal config twice and confirm results.csv is byte-identical
# and final.json differs only in wall_sec / realtime_ratio.
#
# Exit 0 = deterministic, 1 = drift detected.
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/build/batch_sim}"
CFG="examples/AS-205-nominal.json"

if [[ ! -x "$BIN" ]]; then
    echo "determinism.sh: build batch_sim first" >&2
    exit 2
fi

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

# The ROM, pad load and example paths are relative to data/.
cd "$ROOT/data"

for i in 1 2; do
    "$BIN" "$CFG" --run-time 120 --final "$TMP/f$i.json" --csv "$TMP/c$i.csv" >/dev/null 2>&1
done

if ! diff -q "$TMP/c1.csv" "$TMP/c2.csv" >/dev/null; then
    echo "FAIL: CSV differs between runs"
    diff "$TMP/c1.csv" "$TMP/c2.csv" | head -20
    exit 1
fi

# JSON should differ only in wall_sec and realtime_ratio.
diff_out=$(diff "$TMP/f1.json" "$TMP/f2.json" || true)
unexpected=$(echo "$diff_out" | grep -E '^[<>] ' | grep -Ev 'wall_sec|realtime_ratio' || true)
if [[ -n "$unexpected" ]]; then
    echo "FAIL: final.json has non-wall-time drift:"
    echo "$unexpected"
    exit 1
fi

echo "OK: results.csv byte-identical; final.json differs only in wall_sec/realtime_ratio"
