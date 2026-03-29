#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BUILD_DIR="$ROOT_DIR/.tmp/tests"
mkdir -p "$BUILD_DIR"

CXX=${CXX:-g++}
CXXFLAGS=${CXXFLAGS:-"-std=c++20 -Wall -Wextra -pedantic -O0"}
TEST_BIN="$BUILD_DIR/test_core"

$CXX $CXXFLAGS "$ROOT_DIR/tests/test_core.cpp" -I"$ROOT_DIR" -o "$TEST_BIN"
"$TEST_BIN"
