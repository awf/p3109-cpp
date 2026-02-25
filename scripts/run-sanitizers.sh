#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-sanitize"

if [[ -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
  rm -rf "${BUILD_DIR}"
fi

echo "[sanitizers] Configuring in ${BUILD_DIR}"
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

echo "[sanitizers] Building"
cmake --build "${BUILD_DIR}"

echo "[sanitizers] Running tests"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

echo "[sanitizers] OK"
