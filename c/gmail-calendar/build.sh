#!/usr/bin/env bash
# Convenience wrapper: configure + build into ./build/.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
cmake -S "$HERE" -B "$HERE/build" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$HERE/build" -j
echo
echo "Built: $HERE/build/gcal_c_sample"
