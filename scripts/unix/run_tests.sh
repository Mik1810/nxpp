#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
BUILD_DIR="$ROOT_DIR/.tmp/tests"
mkdir -p "$BUILD_DIR"

CXX=${CXX:-g++}
CXXFLAGS=${CXXFLAGS:-"-std=c++20 -Wall -Wextra -pedantic -O0"}
GREEN=$'\033[32m'
RED=$'\033[31m'
RESET=$'\033[0m'

total_tests=0
passed_tests=0
overall_status=0

run_test_binary() {
    local source_file="$1"
    local binary_name
    binary_name=$(basename "${source_file%.cpp}")
    local display_name
    display_name=$(basename "$source_file")
    local test_bin="$BUILD_DIR/$binary_name"
    local output_file="$BUILD_DIR/$binary_name.out"
    local status

    echo "Testing $display_name"

    $CXX $CXXFLAGS "$ROOT_DIR/$source_file" -I"$ROOT_DIR" -o "$test_bin"
    set +e
    "$test_bin" > "$output_file" 2>&1
    status=$?
    set -e

    cat "$output_file"

    total_tests=$((total_tests + $(grep -c '^\[TEST\].*| .*PASS\|^\[TEST\].*| .*FAIL' "$output_file")))
    passed_tests=$((passed_tests + $(grep -c '^\[TEST\].*| .*PASS' "$output_file")))

    if [ "$status" -ne 0 ]; then
        overall_status=1
    fi

    echo
}

run_test_binary "tests/test_core.cpp"
run_test_binary "tests/test_attributes.cpp"
run_test_binary "tests/test_edge_cases.cpp"
run_test_binary "tests/test_flow.cpp"
run_test_binary "tests/test_remove_node.cpp"
run_test_binary "tests/test_multigraph.cpp"

if [ "$overall_status" -eq 0 ]; then
    echo "[TEST] All tests passed (${passed_tests}/${total_tests}) | ${GREEN}PASS${RESET}"
    exit 0
fi

echo "[TEST] All tests passed (${passed_tests}/${total_tests}) | ${RED}FAIL${RESET}"
exit 1
