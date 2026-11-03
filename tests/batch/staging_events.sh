#!/bin/bash
# Run the nominal 800 s config and confirm the expected staging events
# fire in the expected order at plausible times.  This is the primary
# "physics didn't break" regression check.
#
# Expected sequence (from historical Saturn IB AS-205 and current model):
#   bLiftoff        : first true near t = T_L_prime_sec (300)
#   bIECO           : first true late in S-IB burn (~t=440-445)
#   bOECO           : first true within ~5 s after IECO
#   bStageSep       : first true within ~5 s after OECO
#   bS_IVB_EngineStart: first true within ~2 s after StageSep
#
# Exit 0 = all events at plausible times, 1 = mismatch.
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/build/batch_sim}"
CFG="examples/AS-205-nominal.json"

if [[ ! -x "$BIN" ]]; then
    echo "staging_events.sh: build batch_sim first" >&2
    exit 2
fi

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

cd "$ROOT/data"
"$BIN" "$CFG" --final "$TMP/final.json" --csv "$TMP/results.csv" >/dev/null 2>&1

# Find first row where each event bit == 1.
CSV="$TMP/results.csv"

first_true() {
    local col="$1"
    awk -F, -v col="$col" '
        NR==1 { for (i=1;i<=NF;i++) if ($i==col) c=i; next }
        $c=="1" { print $1; exit }
    ' "$CSV"
}

t_liftoff=$(first_true bLiftoff)
t_ieco=$(first_true bIECO)
t_oeco=$(first_true bOECO)
t_sep=$(first_true bStageSep)
t_sivb=$(first_true bS_IVB_EngineStart)

echo "bLiftoff            first true at t = $t_liftoff"
echo "bIECO               first true at t = $t_ieco"
echo "bOECO               first true at t = $t_oeco"
echo "bStageSep           first true at t = $t_sep"
echo "bS_IVB_EngineStart  first true at t = $t_sivb"

fail=0

in_range() {
    local val="$1" lo="$2" hi="$3" name="$4"
    if [[ -z "$val" ]]; then
        echo "FAIL: $name never triggered"
        return 1
    fi
    awk -v v="$val" -v lo="$lo" -v hi="$hi" 'BEGIN { exit !(v>=lo && v<=hi) }' \
        && return 0 \
        || { echo "FAIL: $name = $val, expected [$lo..$hi]"; return 1; }
}

in_range "$t_liftoff" 295 305 "liftoff"    || fail=1
in_range "$t_ieco"    430 460 "IECO"       || fail=1
in_range "$t_oeco"    430 460 "OECO"       || fail=1
in_range "$t_sep"     430 460 "stage sep"  || fail=1
in_range "$t_sivb"    430 465 "S-IVB start"|| fail=1

# Order: liftoff < ieco <= oeco <= sep <= sivb
if awk -v a="$t_liftoff" -v b="$t_ieco" -v c="$t_oeco" -v d="$t_sep" -v e="$t_sivb" \
    'BEGIN { exit !(a<b && b<=c && c<=d && d<=e) }'; then
    :
else
    echo "FAIL: staging event order wrong"
    fail=1
fi

exit $fail
