#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
WASM_DIR="$ROOT_DIR/wasm"
NODE_BIN=${NODE_BIN:-node}
NPM_BIN=${NPM_BIN:-npm}
NXPP_WASM_NPM_PACK_SKIP_BUILD=${NXPP_WASM_NPM_PACK_SKIP_BUILD:-0}
NXPP_WASM_NPM_PACK_TARBALL=${NXPP_WASM_NPM_PACK_TARBALL:-}
MODULE_PATH="$ROOT_DIR/wasm/runtime/node.mjs"
WORK_DIR="$ROOT_DIR/.tmp/wasm-npm-pack-consumer"
OUTPUT_FILE="$WORK_DIR/npm_pack_consumer_test.out"

if ! command -v "$NODE_BIN" >/dev/null 2>&1; then
    echo "[WASM-NPM-PACK] node not found. Install Node.js or set NODE_BIN to a valid runtime." >&2
    exit 1
fi

if ! command -v "$NPM_BIN" >/dev/null 2>&1; then
    echo "[WASM-NPM-PACK] npm not found. Install npm or set NPM_BIN to a valid executable." >&2
    exit 1
fi

if [ "$NXPP_WASM_NPM_PACK_SKIP_BUILD" != "1" ]; then
    bash "$ROOT_DIR/wasm/scripts/build_wasm_node_module.sh"
fi

if [ ! -f "$MODULE_PATH" ]; then
    echo "[WASM-NPM-PACK] expected module not found at $MODULE_PATH" >&2
    exit 1
fi

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

if [ -n "$NXPP_WASM_NPM_PACK_TARBALL" ]; then
    PACK_TGZ_PATH="$NXPP_WASM_NPM_PACK_TARBALL"
else
    PACK_TGZ_NAME=$(
        cd "$WASM_DIR"
        "$NPM_BIN" pack --silent --pack-destination "$WORK_DIR" | tail -n 1
    )
    PACK_TGZ_PATH="$WORK_DIR/$PACK_TGZ_NAME"
fi

if [ ! -f "$PACK_TGZ_PATH" ]; then
    echo "[WASM-NPM-PACK] expected tarball not found at $PACK_TGZ_PATH" >&2
    exit 1
fi

expected_files=(
    package/README.md
    package/dist/algorithms/centrality.d.ts
    package/dist/algorithms/centrality.js
    package/dist/algorithms/components.d.ts
    package/dist/algorithms/components.js
    package/dist/algorithms/shortest_paths.d.ts
    package/dist/algorithms/shortest_paths.js
    package/dist/core/graph.d.ts
    package/dist/core/graph.js
    package/dist/core/multigraph.d.ts
    package/dist/core/multigraph.js
    package/dist/index.d.ts
    package/dist/index.js
    package/dist/internal/assert.d.ts
    package/dist/internal/assert.js
    package/dist/internal/errors.d.ts
    package/dist/internal/errors.js
    package/dist/internal/wasm_types.d.ts
    package/dist/internal/wrap.d.ts
    package/dist/internal/wrap.js
    package/dist/runtime/context.d.ts
    package/dist/runtime/context.js
    package/dist/runtime/node.d.ts
    package/dist/runtime/node.js
    package/dist/types.d.ts
    package/package.json
    package/runtime/node.mjs
    package/runtime/node.wasm
)
printf '%s\n' "${expected_files[@]}" | sort > "$WORK_DIR/expected-files.txt"
tar -tzf "$PACK_TGZ_PATH" | sort > "$WORK_DIR/actual-files.txt"
if ! diff -u "$WORK_DIR/expected-files.txt" "$WORK_DIR/actual-files.txt"; then
    echo "[WASM-NPM-PACK] tarball contents differ from the intentional package layout." >&2
    exit 1
fi

CONSUMER_DIR="$WORK_DIR/consumer"
mkdir -p "$CONSUMER_DIR"
cp "$WASM_DIR/test/npm_pack_consumer/package.json" "$CONSUMER_DIR/package.json"
cp "$WASM_DIR/test/npm_pack_consumer/smoke_test.mjs" "$CONSUMER_DIR/smoke_test.mjs"
cp "$WASM_DIR/test/npm_pack_consumer/type_check.ts" "$CONSUMER_DIR/type_check.ts"

(
    cd "$CONSUMER_DIR"
    "$NPM_BIN" install --silent "$PACK_TGZ_PATH"
)

"$WASM_DIR/node_modules/.bin/tsc" \
    --noEmit \
    --strict \
    --target ES2022 \
    --module NodeNext \
    --moduleResolution NodeNext \
    "$CONSUMER_DIR/type_check.ts"

set +e
(
    cd "$CONSUMER_DIR"
    "$NODE_BIN" smoke_test.mjs
) > "$OUTPUT_FILE" 2>&1
status=$?
set -e

cat "$OUTPUT_FILE"

if [ "$status" -ne 0 ]; then
    echo "[WASM-NPM-PACK] consumer smoke test failed with exit code $status" >&2
    exit "$status"
fi

if ! grep -q "^\[WASM-NPM-PACK\] consumer-smoke-ok$" "$OUTPUT_FILE"; then
    echo "[WASM-NPM-PACK] smoke output marker not found." >&2
    exit 1
fi

echo "[WASM-NPM-PACK] npm pack consumer test passed"
