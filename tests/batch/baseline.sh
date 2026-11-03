#!/bin/bash
# Compare batch_sim CSV output with the recorded baselines.  The hashes were
# captured from the pre-migration build on x86-64 Linux with glibc; a different
# libm or architecture can legitimately change low-order digits, so a mismatch
# there means "investigate", not necessarily "broken".
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/build/batch_sim}"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cd "$ROOT/data"

fail=0
while read -r want name cfg run_time; do
    "$BIN" "$cfg" --run-time "$run_time" --final "$TMP/$name.json" --csv "$TMP/$name.csv" >/dev/null 2>&1
    got=$(sha256sum "$TMP/$name.csv" | cut -d' ' -f1)
    if [[ "$got" == "$want" ]]; then
        echo "OK:   $name"
    else
        echo "FAIL: $name: got $got, want $want"
        fail=1
    fi
done < "$ROOT/tests/baselines/batch_csv.sha256"
exit $fail
