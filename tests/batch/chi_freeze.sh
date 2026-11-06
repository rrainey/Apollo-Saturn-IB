#!/bin/bash
# Verify the LVDC Chi Freeze behaves per EDD §4.4 at the terminal-phase
# switch (T_3_eff <= epsilon_2 = 15 s, which zeros K_1..K_4 in Eq. 4.4.81
# and 4.4.82).
#
# The symptom this guards against: Chi_Z (yaw command) taking a large
# step at the instant the position-correction terms switch off, which
# indicates ΔY_V was still nonzero at terminal — i.e. SMCZ drifted and
# the vehicle did not trim onto the orbital plane.  History: the AS-205
# TM-X-62831 scenario showed a ~6.7° step before the SMCZ unit fix.
#
# Checks:
#   1. T_3_eff crosses below 15 somewhere in the trace (freeze engaged).
#   2. |ΔChi_Z| across the two adjacent IGM samples spanning the crossing
#      is small — i.e. the freeze does not introduce a discontinuity.
#   3. Chi_Z settles (doesn't drift) across the samples after freeze.
#
# Thresholds are deliberately loose so a numeric wobble in low-order
# digits can't flap the test, but any reintroduction of the pre-fix
# multi-degree step would fail loudly.

set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/build/batch_sim}"
CFG="examples/AS-205-tm-x-62831.json"

if [[ ! -x "$BIN" ]]; then
    echo "chi_freeze.sh: build batch_sim first" >&2
    exit 2
fi

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

cd "$ROOT/data"
"$BIN" "$CFG" --run-time 960 \
    --final "$TMP/final.json" --csv "$TMP/results.csv" \
    --igm-trace "$TMP/igm.csv" --igm-trace-dt 1.0 \
    >/dev/null 2>&1

IGM="$TMP/igm.csv"

# Extract the two Chi_Z samples (deg) and their T_3_eff values on either
# side of the T_3_eff = 15 crossing.  Column indices come from
# src/sim/igm_trace.c header order:
#   1=t_sec, 5=T_3_eff_sec, 10=Chi_Z_deg
awk -F, '
    NR==1 { next }
    $5 == "" || $10 == "" { next }
    prev_T != "" && prev_T > 15 && $5 <= 15 {
        printf "%s %s %s %s\n", prev_t, prev_T, prev_Z, $1" "$5" "$10
        exit
    }
    { prev_t=$1; prev_T=$5; prev_Z=$10 }
' "$IGM" > "$TMP/cross.txt"

read -r t_pre T_pre Z_pre t_post T_post Z_post < "$TMP/cross.txt" || true

if [[ -z "${Z_pre:-}" || -z "${Z_post:-}" ]]; then
    echo "FAIL: T_3_eff did not cross below 15 s within the run"
    exit 1
fi

dZ=$(awk -v a="$Z_pre" -v b="$Z_post" 'BEGIN { d=b-a; if (d<0) d=-d; print d }')

printf "pre-freeze : t=%7.3f  T_3_eff=%6.3f  Chi_Z=%+8.4f deg\n" "$t_pre" "$T_pre" "$Z_pre"
printf "post-freeze: t=%7.3f  T_3_eff=%6.3f  Chi_Z=%+8.4f deg\n" "$t_post" "$T_post" "$Z_post"
printf "|dChi_Z|   = %.4f deg\n" "$dZ"

fail=0

# Threshold: the pre-fix step was +6.73°.  A clean freeze should be
# well under 1°.  Use 1.5° as the regression gate.
if awk -v d="$dZ" 'BEGIN { exit !(d < 1.5) }'; then
    echo "OK: Chi_Z step at freeze within 1.5 deg"
else
    echo "FAIL: Chi_Z stepped $dZ deg at freeze (limit 1.5 deg)"
    fail=1
fi

# Post-freeze drift check: Chi_Z samples at +5, +10, +20 s after the
# crossing should not stray far from Z_post (the latch holds it).
drift() {
    local dt="$1"
    awk -F, -v t0="$t_post" -v dt="$dt" '
        NR==1 { next }
        $5 == "" || $10 == "" { next }
        $1 >= t0+dt { print $10; exit }
    ' "$IGM"
}

for off in 5 10 20; do
    Z_later=$(drift "$off")
    if [[ -z "$Z_later" ]]; then
        continue   # run ended before this offset — not a failure
    fi
    d=$(awk -v a="$Z_post" -v b="$Z_later" 'BEGIN { d=b-a; if (d<0) d=-d; print d }')
    if awk -v d="$d" 'BEGIN { exit !(d < 1.0) }'; then
        printf "OK: Chi_Z drift at +%2ds = %.4f deg (< 1.0)\n" "$off" "$d"
    else
        printf "FAIL: Chi_Z drift at +%2ds = %.4f deg (limit 1.0)\n" "$off" "$d"
        fail=1
    fi
done

exit $fail
