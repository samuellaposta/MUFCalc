#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────────────────
#  MUFCalc build script (Linux)
# ─────────────────────────────────────────────────────────────────────────
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build-linux"

echo "[1/4] Configuring CMake..."
cmake -S "${ROOT}" -B "${BUILD}" \
      -DCMAKE_BUILD_TYPE=Release \
      -DMUFCALC_BUILD_TESTS=ON

echo "[2/4] Building..."
cmake --build "${BUILD}" -j"$(nproc)"

echo "[3/4] Running tests..."
ctest --test-dir "${BUILD}" --output-on-failure

echo "[4/4] Done. Binary: ${BUILD}/MUFCalc"
