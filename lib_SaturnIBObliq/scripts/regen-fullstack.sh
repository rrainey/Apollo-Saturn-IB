#!/usr/bin/env bash
# Regenerate Silver full-stack C into plant/generated/
#
# Usage:
#   ./scripts/regen-fullstack.sh              # regen parent + LVDA into plant/generated only
#   ./scripts/regen-fullstack.sh --initialize # also seed native-snapshots/lvda-lvdc-legacy/
#
# The active LVDA_LVDC implementation lives in native-snapshots/lvda-lvdc-rewrite/
# and is used by saturnib via LVDC_USE_REWRITE=ON (default). The legacy "golden"
# hand-edited LVDA_LVDC lives in native-snapshots/lvda-lvdc-legacy/ for reference
# and A/B comparison (LVDC_USE_REWRITE=OFF).
#
# Codegen always refreshes plant/generated/LVDA_LVDC.* as a disposable reference.
# --initialize seeds the LEGACY snapshot from codegen (reserved for forensic
# work on the golden path); the rewrite snapshot is never auto-overwritten.
set -euo pipefail

LIB="$(cd "$(dirname "$0")/.." && pwd)"
OBLIQ="${OBLIQ_ROOT:-$HOME/src/obliq-2}"
OUT="${LIB}/plant/generated"
SNAP="${LIB}/native-snapshots/lvda-lvdc-legacy"
JSON="${LIB}/../model/saturn_ib_stack.json"

INITIALIZE=0
for arg in "$@"; do
  case "$arg" in
    --initialize|-i) INITIALIZE=1 ;;
    -h|--help)
      sed -n '2,12p' "$0"
      exit 0
      ;;
    *)
      echo "Unknown argument: $arg (use --initialize or --help)" >&2
      exit 2
      ;;
  esac
done

cd "$OBLIQ"
./node_modules/.bin/tsx scripts/obliq-cgen.ts \
  "$JSON" \
  --out "$OUT" \
  --profile saturn-ib-stack \
  --name saturn_ib_stack

# Host bridge lives in src/; strip cgen's copy if emitted
rm -f "$OUT/saturn_ib_stack_rtw_adapter.c" "$OUT/saturn_ib_stack_rtw_adapter.h"

# cgen's smoke_main still includes the (now-deleted) rtw adapter header.
# Point it at our host adapter, which provides the ExternalInputs/Outputs
# types and the saturn_ib_stack_rtw_step compatibility alias.
if [[ -f "$OUT/smoke_main.c" ]]; then
  sed -i 's|#include "saturn_ib_stack_rtw_adapter.h"|#include "SaturnIBStackAdapter.h"|' \
    "$OUT/smoke_main.c"
fi

# Standalone smoke CMake for generated tree (batch_sim uses lib CMakeLists).
# LVDA_LVDC sources come from the native snapshot (hand-edit root).
cat > "$OUT/CMakeLists.txt" << 'CMAKE'
cmake_minimum_required(VERSION 3.16)
project(saturn_ib_stack_cgen C)
set(LVDC_SNAP "${CMAKE_CURRENT_SOURCE_DIR}/../../native-snapshots/lvda-lvdc-legacy")
add_library(saturn_ib_stack STATIC
  saturn_ib_stack.c
  ${LVDC_SNAP}/LVDA_LVDC.c
  ../../../src/sim/SaturnIBStackAdapter.c
)
target_include_directories(saturn_ib_stack PUBLIC
  ${LVDC_SNAP}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sim
)
target_compile_options(saturn_ib_stack PRIVATE -Wall -Wextra -O2)
target_link_libraries(saturn_ib_stack PUBLIC m)
add_executable(saturn_ib_stack_smoke smoke_main.c)
target_link_libraries(saturn_ib_stack_smoke PRIVATE saturn_ib_stack)
CMAKE

mkdir -p "$SNAP"

if [[ "$INITIALIZE" -eq 1 ]]; then
  cp "$OUT/LVDA_LVDC.c" "$OUT/LVDA_LVDC.h" "$SNAP/"
  echo "Full stack regenerated under ${OUT}"
  echo "LVDA_LVDC snapshot INITIALIZED under ${SNAP}"
  echo "(subsequent regens without --initialize will not overwrite the snapshot)"
else
  echo "Full stack regenerated under ${OUT}"
  if [[ -f "$SNAP/LVDA_LVDC.c" ]]; then
    echo "Preserved hand-edit root: ${SNAP}"
    echo "(pass --initialize to re-seed LVDA_LVDC.{c,h} from codegen — destroys local edits)"
  else
    echo "WARNING: ${SNAP}/LVDA_LVDC.c missing." >&2
    echo "  Silver builds compile from the snapshot. Seed it once with:" >&2
    echo "    $0 --initialize" >&2
  fi
fi
