#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
NODE_BIN=${NODE_BIN:-node}
NXPP_WASM_NODE_CONTRACT_SKIP_BUILD=${NXPP_WASM_NODE_CONTRACT_SKIP_BUILD:-0}
MODULE_PATH="$ROOT_DIR/wasm/build/nxpp_node.mjs"
OUTPUT_FILE="$ROOT_DIR/wasm/build/node_api_contract_test.out"

mkdir -p "$(dirname "$OUTPUT_FILE")"

if ! command -v "$NODE_BIN" >/dev/null 2>&1; then
    echo "[WASM-NODE] node not found. Install Node.js or set NODE_BIN to a valid runtime." >&2
    exit 1
fi

if [ "$NXPP_WASM_NODE_CONTRACT_SKIP_BUILD" != "1" ]; then
    bash "$ROOT_DIR/wasm/scripts/build_wasm_node_module.sh"
fi

if [ ! -f "$MODULE_PATH" ]; then
    echo "[WASM-NODE] expected module not found at $MODULE_PATH" >&2
    exit 1
fi

set +e
NXPP_WASM_NODE_MODULE="$MODULE_PATH" "$NODE_BIN" "$ROOT_DIR/wasm/test/node_api_contract_test.mjs" > "$OUTPUT_FILE" 2>&1
status=$?
set -e
cat "$OUTPUT_FILE"

if [ "$status" -ne 0 ]; then
    echo "[WASM-NODE] contract tests failed with exit code $status" >&2
    exit "$status"
fi

if ! grep -q "^\[WASM-NODE\] contract-tests-ok$" "$OUTPUT_FILE"; then
    echo "[WASM-NODE] contract output marker not found." >&2
    exit 1
fi

echo "[WASM-NODE] Node API contract tests passed"
