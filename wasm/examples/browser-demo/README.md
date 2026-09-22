# Experimental browser demo

This directory is a manual investigation path for browser loading. It is not a
supported runtime target and is not part of the `@mik1810/nxpp-wasm` Node
package contract.

Build the browser demo artifacts with:

```bash
bash wasm/scripts/build_wasm_browser_demo.sh
```

This produces:

- `wasm/examples/browser-demo/nxpp_browser.mjs`
- `wasm/examples/browser-demo/nxpp_browser.wasm`

The build is separate from `wasm/build/nxpp_node.mjs`. The Node package uses
Node-specific Emscripten settings and remains the only currently supported
experimental runtime target.

The browser build direction to investigate is:

- `-sENVIRONMENT=web`
- `-sMODULARIZE=1`
- `-sEXPORT_ES6=1`
- `-sALLOW_MEMORY_GROWTH=1`
- Embind enabled with `--bind`

After building the browser artifacts, serve this directory over HTTP:

```bash
cd wasm/examples/browser-demo
python3 -m http.server 8080
```

Then open `http://localhost:8080/`.

The demo imports the shared runtime-neutral facade constructor from
`wasm/dist/runtime/` and supplies a browser adapter that owns the browser WASM
URL. It does not maintain a second graph facade or binding layer.

CI exercises the same page in a headless browser. After building both the
TypeScript output and browser module, the bounded check can be run with:

```bash
bash wasm/scripts/run_wasm_browser_smoke_test.sh
```

Set `BROWSER_BIN` when Chrome or Chromium is not available under a standard
executable name.

The demo intentionally exercises only a small smoke path:

- load the WASM module
- create a directed integer graph
- run Dijkstra from one source
- display the path and distance result

This successful browser demo should be read only as a load-and-execute smoke
check. It does not yet prove behavioral parity with the Node-oriented runtime,
TypeScript facade, or Node contract test lane.

Browser bundler support and package exports remain out of scope for this
investigation path. The smoke test covers only direct browser initialization
through the shared facade and the small graph operation above.
