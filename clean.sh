#!/usr/bin/env bash
set -euo pipefail
rm -rf build
find . -name "*.o" -o -name "*.obj" | xargs -r rm -f
echo "Cleaned."
