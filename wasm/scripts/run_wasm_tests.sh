#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
BUILD_DIR="$ROOT_DIR/.tmp/wasm-tests"
mkdir -p "$BUILD_DIR"

EMXX=${EMXX:-em++}
NODE_BIN=${NODE_BIN:-node}
EMXXFLAGS=${EMXXFLAGS:-"-std=c++20 -Wall -Wextra -pedantic -O1 -fexceptions -sDISABLE_EXCEPTION_CATCHING=0 -sENVIRONMENT=node -sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1"}
BOOST_INCLUDE=${BOOST_INCLUDE:-/usr/include}
NXPP_WASM_INCLUDE_LARGE=${NXPP_WASM_INCLUDE_LARGE:-0}
NXPP_WASM_INCLUDE_NODE_CONTRACT=${NXPP_WASM_INCLUDE_NODE_CONTRACT:-1}
NXPP_WASM_NODE_CONTRACT_SKIP_BUILD=${NXPP_WASM_NODE_CONTRACT_SKIP_BUILD:-0}

GREEN=$'\033[32m'
RED=$'\033[31m'
RESET=$'\033[0m'

total_tests=0
passed_tests=0
overall_status=0

if ! command -v "$EMXX" >/dev/null 2>&1; then
    echo "[WASM] em++ not found. Install Emscripten or set EMXX to a valid compiler." >&2
    exit 1
fi

if ! command -v "$NODE_BIN" >/dev/null 2>&1; then
    echo "[WASM] node not found. Install Node.js or set NODE_BIN to a valid runtime." >&2
    exit 1
fi

run_test_js() {
    local source_file="$1"
    local binary_name
    binary_name=$(basename "${source_file%.cpp}")
    local display_name
    display_name=$(basename "$source_file")
    local output_js="$BUILD_DIR/$binary_name.js"
    local output_file="$BUILD_DIR/$binary_name.out"
    local status

    echo "Testing wasm $display_name"

    $EMXX $EMXXFLAGS "$ROOT_DIR/$source_file" -I"$ROOT_DIR" -idirafter "$BOOST_INCLUDE" -o "$output_js"

    set +e
    "$NODE_BIN" "$output_js" > "$output_file" 2>&1
    status=$?
    set -e

    cat "$output_file"

    total_tests=$((total_tests + $(grep -c '^\[TEST\].*| .*PASS\|^\[TEST\].*| .*FAIL' "$output_file" || true)))
    passed_tests=$((passed_tests + $(grep -c '^\[TEST\].*| .*PASS' "$output_file" || true)))

    if [ "$status" -ne 0 ]; then
        overall_status=1
    fi

    echo
}

if [ "$NXPP_WASM_INCLUDE_NODE_CONTRACT" = "1" ]; then
    NXPP_WASM_NODE_CONTRACT_SKIP_BUILD="$NXPP_WASM_NODE_CONTRACT_SKIP_BUILD" \
        bash "$ROOT_DIR/wasm/scripts/run_wasm_node_contract_tests.sh"
fi

run_test_js "tests/test_core.cpp"
run_test_js "tests/test_attributes.cpp"
run_test_js "tests/test_centrality.cpp"
run_test_js "tests/test_edge_cases.cpp"
run_test_js "tests/test_flow.cpp"
run_test_js "tests/test_remove_node.cpp"
run_test_js "tests/test_multigraph.cpp"

if [ "$NXPP_WASM_INCLUDE_LARGE" = "1" ]; then
    run_test_js "tests/test_large_graph_compare.cpp"
fi

if [ "$overall_status" -eq 0 ]; then
    echo "[WASM] All tests passed (${passed_tests}/${total_tests}) | ${GREEN}PASS${RESET}"
    exit 0
fi

echo "[WASM] All tests passed (${passed_tests}/${total_tests}) | ${RED}FAIL${RESET}"
exit 1
