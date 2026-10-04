#!/usr/bin/env bash
set -euo pipefail

# stage2_bootstrap.sh
# -------------------
# Sets up Stage 2 offline-first testing deps and fixes the two known test issues:
#   1) CTest fixture tests need WORKING_DIRECTORY = repo root for data/ paths.
#   2) ADT policy scan must ignore comments/strings to avoid false positives.
#
# Assumptions:
# - You already updated top-level CMakeLists.txt to use:
#     find_package(Catch2) OR third_party/Catch2-<ver>.zip OR fail w/ message.
# - Repo root dir name is "lasa_cc" (doesn't matter; script is path-agnostic).
#
# Usage:
#   ./scripts/stage2_bootstrap.sh
#   CATCH2_VERSION=3.5.4 ./scripts/stage2_bootstrap.sh
#
# Optional:
#   RUN_GET_CATCH2=1 ./scripts/stage2_bootstrap.sh   # runs third_party/get_catch2.sh if zip missing

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TP_DIR="$ROOT_DIR/third_party"
SCRIPTS_DIR="$ROOT_DIR/scripts"

CATCH2_VERSION="${CATCH2_VERSION:-3.5.4}"
RUN_GET_CATCH2="${RUN_GET_CATCH2:-0}"

echo "==> Project root: $ROOT_DIR"
echo "==> Stage 2 bootstrap (offline-first deps + test fixes)"
echo "==> Catch2 version: $CATCH2_VERSION"

mkdir -p "$TP_DIR" "$SCRIPTS_DIR"

# -----------------------------------------------------------------------------
# 1) Ensure third_party/get_catch2.sh exists (creates/overwrites)
# -----------------------------------------------------------------------------
GET_CATCH2="$TP_DIR/get_catch2.sh"
cat > "$GET_CATCH2" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TP_DIR="$ROOT_DIR/third_party"

DEFAULT_VERSION="3.5.4"
CATCH2_VERSION="${1:-${CATCH2_VERSION:-$DEFAULT_VERSION}}"

ZIP_NAME="Catch2-${CATCH2_VERSION}.zip"
ZIP_PATH="$TP_DIR/$ZIP_NAME"

URL_PRIMARY="https://github.com/catchorg/Catch2/archive/refs/tags/v${CATCH2_VERSION}.zip"
URL_FALLBACK="https://codeload.github.com/catchorg/Catch2/zip/refs/tags/v${CATCH2_VERSION}"

mkdir -p "$TP_DIR"

if [[ -f "$ZIP_PATH" ]]; then
  echo "==> Catch2 already vendored: $ZIP_PATH"
  exit 0
fi

echo "==> Vendoring Catch2 v${CATCH2_VERSION}"
echo "==> Destination: $ZIP_PATH"

download() {
  local url="$1"
  echo "==> Downloading: $url"

  if command -v curl >/dev/null 2>&1; then
    curl -fL --retry 3 --retry-delay 1 -o "$ZIP_PATH.part" "$url"
    return 0
  elif command -v wget >/dev/null 2>&1; then
    wget -O "$ZIP_PATH.part" "$url"
    return 0
  else
    echo "ERROR: need curl or wget to download Catch2."
    exit 2
  fi
}

if ! download "$URL_PRIMARY"; then
  echo "==> Primary URL failed, trying fallback..."
  rm -f "$ZIP_PATH.part"
  download "$URL_FALLBACK"
fi

mv "$ZIP_PATH.part" "$ZIP_PATH"

if [[ ! -s "$ZIP_PATH" ]]; then
  echo "ERROR: downloaded file is empty: $ZIP_PATH"
  rm -f "$ZIP_PATH"
  exit 1
fi

echo "==> Done: $ZIP_PATH"
EOF
chmod +x "$GET_CATCH2"
echo "==> Wrote: $GET_CATCH2"

# -----------------------------------------------------------------------------
# 2) Ensure scripts/check_no_stl_adt.py is comment/string-safe (creates/overwrites)
# -----------------------------------------------------------------------------
CHECK_NO_STL="$SCRIPTS_DIR/check_no_stl_adt.py"
cat > "$CHECK_NO_STL" <<'EOF'
#!/usr/bin/env python3
import sys
import re
from pathlib import Path

FORBIDDEN = [
    "std::unordered_map",
    "std::unordered_set",
    "std::map",
    "std::set",
    "std::multimap",
    "std::multiset",
    "std::list",
    "std::deque",
    "std::queue",
    "std::stack",
    "std::priority_queue",
]

ADT_DIRS = [
    Path("include/lacc/adt"),
    Path("src/adt"),
]

def strip_comments_and_strings(text: str) -> str:
    # Strip string literals
    text = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
    text = re.sub(r"'(?:\\.|[^'\\])*'", "''", text)
    # Strip block comments
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    # Strip line comments
    text = re.sub(r"//.*?$", "", text, flags=re.MULTILINE)
    return text

violations = []

for adt_dir in ADT_DIRS:
    if not adt_dir.exists():
        continue

    # scan headers and implementations
    for path in list(adt_dir.rglob("*.hpp")) + list(adt_dir.rglob("*.h")) + list(adt_dir.rglob("*.cpp")) + list(adt_dir.rglob("*.cc")):
        try:
            text = path.read_text(errors="ignore")
        except Exception:
            continue

        stripped = strip_comments_and_strings(text)

        for token in FORBIDDEN:
            if token in stripped:
                violations.append((path.as_posix(), token))

if violations:
    print("ERROR: Forbidden STL containers detected in ADT implementation:\n")
    for path, token in violations:
        print(f" - {path}: forbidden STL container usage: '{token}'")
    print("\nPolicy: ADT implementations must not use STL container types/adapters.")
    sys.exit(1)

print("ADT policy OK: no forbidden STL containers found.")
sys.exit(0)
EOF
chmod +x "$CHECK_NO_STL"
echo "==> Wrote: $CHECK_NO_STL"

# -----------------------------------------------------------------------------
# 3) Ensure fixture tests are run with WORKING_DIRECTORY = repo root
#    This is done in CMake, but we validate your CMakeLists contains it.
# -----------------------------------------------------------------------------
CMAKE_FILE="$ROOT_DIR/CMakeLists.txt"
if [[ -f "$CMAKE_FILE" ]]; then
  if grep -q "WORKING_DIRECTORY[[:space:]]*\\\${CMAKE_SOURCE_DIR}" "$CMAKE_FILE" || grep -q "WORKING_DIRECTORY[[:space:]]*\\\${CMAKE_SOURCE_DIR}" "$CMAKE_FILE"; then
    echo "==> CMakeLists.txt appears to set WORKING_DIRECTORY for fixture tests (good)."
  else
    echo "!! WARNING: Could not find WORKING_DIRECTORY \${CMAKE_SOURCE_DIR} in CMakeLists.txt."
    echo "   Ensure test_small_gw and test_small_dna run from repo root so data/ paths resolve."
  fi
else
  echo "!! WARNING: No CMakeLists.txt found at repo root? ($CMAKE_FILE)"
fi

# -----------------------------------------------------------------------------
# 4) Optionally download Catch2 zip if missing
# -----------------------------------------------------------------------------
CATCH2_ZIP="$TP_DIR/Catch2-${CATCH2_VERSION}.zip"
if [[ -f "$CATCH2_ZIP" ]]; then
  echo "==> Catch2 zip already present: $CATCH2_ZIP"
else
  echo "==> Catch2 zip missing: $CATCH2_ZIP"
  if [[ "$RUN_GET_CATCH2" == "1" ]]; then
    echo "==> Attempting to download Catch2 (may fail on clusters with bad TLS)"
    "$GET_CATCH2" "$CATCH2_VERSION" || true
  else
    echo "==> Not downloading automatically."
    echo "    To download (on a machine with working TLS):"
    echo "      $TP_DIR/get_catch2.sh $CATCH2_VERSION"
  fi
fi

echo "==> Stage 2 bootstrap complete."
echo "==> Next:"
echo "    rm -rf build && ./build_and_run.sh"

