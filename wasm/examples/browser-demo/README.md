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

The demo intentionally exercises only a small smoke path:

- load the WASM module
- create a directed integer graph
- run Dijkstra from one source
- display the path and distance result

This successful browser demo should be read only as a load-and-execute smoke
check. It does not yet prove behavioral parity with the Node-oriented runtime,
TypeScript facade, or Node contract test lane.

Browser bundler support, package exports, CI coverage, and TypeScript facade
integration are out of scope for this investigation path.
