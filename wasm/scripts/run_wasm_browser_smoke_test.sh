#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
DEMO_PORT=${NXPP_WASM_BROWSER_PORT:-8765}
DEMO_URL=${NXPP_WASM_BROWSER_DEMO_URL:-http://127.0.0.1:$DEMO_PORT/wasm/examples/browser-demo/}
BROWSER_BIN=${BROWSER_BIN:-}

if [ -z "$BROWSER_BIN" ]; then
    for candidate in google-chrome google-chrome-stable chromium chromium-browser; do
        if command -v "$candidate" >/dev/null 2>&1; then
            BROWSER_BIN=$candidate
            break
        fi
    done
fi

if [ -z "$BROWSER_BIN" ]; then
    echo "[WASM-BROWSER] no supported headless browser found; set BROWSER_BIN." >&2
    exit 1
fi

for artifact in \
    "$ROOT_DIR/wasm/dist/runtime/browser.js" \
    "$ROOT_DIR/wasm/examples/browser-demo/nxpp_browser.mjs" \
    "$ROOT_DIR/wasm/examples/browser-demo/nxpp_browser.wasm"; do
    if [ ! -f "$artifact" ]; then
        echo "[WASM-BROWSER] expected artifact not found: $artifact" >&2
        exit 1
    fi
done

TEMP_DIR=$(mktemp -d)
SERVER_PID=
cleanup() {
    if [ -n "$SERVER_PID" ]; then
        kill "$SERVER_PID" >/dev/null 2>&1 || true
        wait "$SERVER_PID" 2>/dev/null || true
    fi
    rm -rf "$TEMP_DIR"
}
trap cleanup EXIT

python3 -m http.server "$DEMO_PORT" --bind 127.0.0.1 --directory "$ROOT_DIR" \
    >"$TEMP_DIR/server.log" 2>&1 &
SERVER_PID=$!

server_ready=0
for _ in {1..20}; do
    if timeout 2s curl --silent --fail "$DEMO_URL" >/dev/null; then
        server_ready=1
        break
    fi
    sleep 0.1
done

if [ "$server_ready" -ne 1 ]; then
    echo "[WASM-BROWSER] demo server did not become ready." >&2
    exit 1
fi

BROWSER_PROFILE="$TEMP_DIR/browser-profile"
if [[ "$BROWSER_BIN" == *.exe ]] && command -v wslpath >/dev/null 2>&1; then
    BROWSER_PROFILE=$(wslpath -w "$BROWSER_PROFILE")
fi

timeout 30s "$BROWSER_BIN" \
    --headless \
    --disable-gpu \
    --no-sandbox \
    --user-data-dir="$BROWSER_PROFILE" \
    --virtual-time-budget=10000 \
    --dump-dom \
    "$DEMO_URL" >"$TEMP_DIR/dom.html" 2>"$TEMP_DIR/browser.log"

if ! grep -q "Browser WASM module loaded\." "$TEMP_DIR/dom.html"; then
    cat "$TEMP_DIR/browser.log" >&2
    echo "[WASM-BROWSER] load marker not found in rendered demo." >&2
    exit 1
fi

if ! grep -q "dijkstra distance 1 -&gt; 4: 4" "$TEMP_DIR/dom.html"; then
    cat "$TEMP_DIR/browser.log" >&2
    echo "[WASM-BROWSER] expected graph result not found in rendered demo." >&2
    exit 1
fi

echo "[WASM-BROWSER] browser facade smoke test passed"
