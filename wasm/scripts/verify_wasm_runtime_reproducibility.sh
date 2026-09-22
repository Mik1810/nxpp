#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
RUNTIME_DIR="$ROOT_DIR/wasm/runtime"
CHECKSUM_FILE=$(mktemp)
trap 'rm -f "$CHECKSUM_FILE"' EXIT

(
    cd "$RUNTIME_DIR"
    sha256sum node.mjs node.wasm > "$CHECKSUM_FILE"
)

NXPP_WASM_EMIT_TSD=1 bash "$ROOT_DIR/wasm/scripts/build_wasm_node_module.sh"

(
    cd "$RUNTIME_DIR"
    sha256sum --check --status "$CHECKSUM_FILE"
)

echo "[WASM-NODE] runtime artifacts are reproducible with the current toolchain"
