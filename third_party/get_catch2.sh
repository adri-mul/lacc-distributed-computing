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
