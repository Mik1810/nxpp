#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BUILD_DIR="$ROOT_DIR/.tmp/large-graph-compare"
mkdir -p "$BUILD_DIR"

CXX=${CXX:-g++}
CXXFLAGS=${CXXFLAGS:-"-std=c++20 -Wall -Wextra -pedantic -O2"}
HEADER_UNDER_TEST=${NXPP_HEADER_UNDER_TEST:-}
GREEN=$'\033[32m'
RED=$'\033[31m'
RESET=$'\033[0m'

test_bin="$BUILD_DIR/test_large_graph_compare"
output_file="$BUILD_DIR/test_large_graph_compare.out"

read -r -a cxxflags_array <<< "$CXXFLAGS"
compile_cmd=("$CXX" "${cxxflags_array[@]}")

if [[ -n "$HEADER_UNDER_TEST" ]]; then
    echo "Testing test_large_graph_compare.cpp against $HEADER_UNDER_TEST"
    compile_cmd+=("-DNXPP_HEADER_UNDER_TEST=\"$HEADER_UNDER_TEST\"")
else
    echo "Testing test_large_graph_compare.cpp"
fi

compile_cmd+=("$ROOT_DIR/tests/test_large_graph_compare.cpp" "-I$ROOT_DIR" "-o" "$test_bin")
"${compile_cmd[@]}"

set +e
"$test_bin" > "$output_file" 2>&1
status=$?
set -e

cat "$output_file"
echo

if [[ "$status" -eq 0 ]]; then
    echo "[TEST] Large-graph comparison passed | ${GREEN}PASS${RESET}"
    exit 0
fi

echo "[TEST] Large-graph comparison passed | ${RED}FAIL${RESET}"
exit 1
