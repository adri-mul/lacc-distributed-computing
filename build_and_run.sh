#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

echo "==> Project root: $ROOT_DIR"

RUN_MPI_TESTS="${RUN_MPI_TESTS:-0}"

# ---- Catch2 vendoring (offline-first, best-effort) ----
CATCH2_VERSION="${CATCH2_VERSION:-3.5.4}"
CATCH2_ZIP="$ROOT_DIR/third_party/Catch2-${CATCH2_VERSION}.zip"

# IMPORTANT: script must be named exactly this (or change the path below)
GET_CATCH2="$ROOT_DIR/third_party/get_catch2.sh"

if [[ ! -f "$CATCH2_ZIP" ]]; then
  echo "==> Catch2 zip not found: $CATCH2_ZIP"
  if [[ -x "$GET_CATCH2" ]]; then
    echo "==> Running: $GET_CATCH2 $CATCH2_VERSION"
    set +e
    "$GET_CATCH2" "$CATCH2_VERSION"
    rc=$?
    set -e
    if [[ $rc -ne 0 ]]; then
      echo "==> WARNING: get_catch2.sh failed (rc=$rc)."
      echo "    Download Catch2 on a machine with working TLS and copy it to:"
      echo "      $CATCH2_ZIP"
    fi
  else
    echo "==> ERROR: missing or non-executable: $GET_CATCH2"
    echo "    Fix: chmod +x $GET_CATCH2"
    echo "    Or place Catch2 zip at: $CATCH2_ZIP"
  fi
else
  echo "==> Found vendored Catch2: $CATCH2_ZIP"
fi

# ---- Detect moved/renamed source directory and clean stale caches ----
if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
  cached_home_dir="$(
    grep -E '^CMAKE_HOME_DIRECTORY:INTERNAL=' "$BUILD_DIR/CMakeCache.txt" \
      | cut -d= -f2- \
      || true
  )"
  if [[ -n "$cached_home_dir" && "$cached_home_dir" != "$ROOT_DIR" ]]; then
    echo "==> Detected moved source dir:"
    echo "    cached: $cached_home_dir"
    echo "    now:    $ROOT_DIR"
    echo "==> Cleaning build directory to avoid stale caches"
    rm -rf "$BUILD_DIR"
  fi
fi

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build "$BUILD_DIR" -j

echo "==> Running tests"
if [[ "$RUN_MPI_TESTS" == "1" ]]; then
  ctest --test-dir "$BUILD_DIR" --output-on-failure
else
  ctest --test-dir "$BUILD_DIR" --output-on-failure -LE mpi
fi

echo "==> Running GW and DNA demos"
mkdir -p "$BUILD_DIR/demo_out"

# Optional: set LACC_PLOTS=1 to generate SVG plot artifacts for the DNA demo.
# Example:
#   LACC_PLOTS=1 ./build_and_run.sh

# NOTE: lacc_analyze expects an *output prefix*, not an output directory.
# To keep demo artifacts grouped, we place the prefix inside build/demo_out/.
"$BUILD_DIR/lacc_analyze" gw  "$ROOT_DIR/data/gw_sample.csv"     "$BUILD_DIR/demo_out/demo"
"$BUILD_DIR/lacc_analyze" dna "$ROOT_DIR/data/dna_sample.fasta"  "$BUILD_DIR/demo_out/demo"

echo "==> Demo outputs written to build/demo_out"
ls -1 "$BUILD_DIR/demo_out" || true

echo "==> Done."

