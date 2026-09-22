# WebAssembly and Node Integration (Emscripten)

This document records the current WASM implementation, build, and CI behavior.
The [issue-backed roadmap](https://github.com/Mik1810/nxpp/issues/177) tracks
actionable work; [ARCHITECTURE.md](ARCHITECTURE.md) and
[API_POLICY.md](API_POLICY.md) define the current design and public policy.

## Status

- experimental portability/build target
- focused on Node.js
- Node runtime is the only currently supported experimental runtime target
- browser runtimes are not supported by the npm package
- not yet a full JavaScript/TypeScript API parity layer
- now shipped as the experimental npm package `@mik1810/nxpp-wasm`

The current binding exposes an experimental typed graph surface centered on the
explicit runtime classes (`GraphInt`, `GraphStr`, `DiGraphInt`, `DiGraphStr`,
`MultiGraphInt`, `MultiGraphStr`, `MultiDiGraphInt`, `MultiDiGraphStr`) plus
the completed `attributes.hpp` and `traversal.hpp` blocks plus the
full `shortest_paths.hpp` block, including Floyd-Warshall all-pairs results,
plus the current `spanning_tree.hpp` minimum-spanning-tree block and the first
`components.hpp` connected/SCC block plus the current `centrality.hpp` degree,
PageRank, and betweenness block, plus the current `flow.hpp` max-flow,
min-cut, and min-cost-flow block.
This should still be read as an implementation stepping stone, not as the
final public JavaScript API shape.

## Objectives

Primary objectives:

- compile and run `nxpp` algorithms under Emscripten
- provide a stable, consumer-friendly Node-facing API layer over wasm
- avoid requiring Boost/Emscripten installation on consumer machines

Longer-term objective:

- extend coverage from a minimal API subset to broad library support

## Current scope

Current wasm lane includes:

- Node-compatible modular bindings in `wasm/include/nxpp_wasm/` and `wasm/src/`
- Node-oriented wasm module build script in `wasm/scripts/build_wasm_node_module.sh`
- Node API contract tests in `wasm/test/node_api_contract_test.mjs`
- Node contract runner in `wasm/scripts/run_wasm_node_contract_tests.sh`
- wasm formal assertion suite execution in `wasm/scripts/run_wasm_tests.sh`
- optional large-graph wasm execution via `NXPP_WASM_INCLUDE_LARGE=1`
- dedicated CI lane in `.github/workflows/wasm-experimental.yml`

Runtime boundary for the current scope:

- Node.js runtime behavior is actively tested and maintained
- CI runs a browser facade smoke check, but this does not establish browser API
  parity or package support

## Browser investigation path

Browser usage is an experimental investigation path separate from the Node
package. CI builds the demo under `wasm/examples/browser-demo/` and checks in a
headless browser that the shared facade can load a browser-specific module,
create a graph, and run one algorithm. This narrow check does not establish
browser API parity or a browser package contract.

This path is intentionally not part of the published package contract:

- it should use a browser-specific build artifact, not `runtime/node.mjs`
- the current package entrypoint and TypeScript facade remain Node-oriented
- browser bundler integration and general asset serving remain unverified
- any browser demo should stay small and should not require redesigning the C++
  API

The expected investigation build direction is an Emscripten module with
browser-compatible settings such as `-sENVIRONMENT=web`, `-sMODULARIZE=1`, and
`-sEXPORT_ES6=1`. Keep the output separate from the Node build, for example as
`wasm/examples/browser-demo/nxpp_browser.mjs` plus its matching `.wasm` file.
The repository includes an experimental build helper and smoke check:
`bash wasm/scripts/build_wasm_browser_demo.sh` and
`bash wasm/scripts/run_wasm_browser_smoke_test.sh`.
Serve the demo directory over HTTP; opening the HTML file directly is not a
valid browser module-loading test.

## Prerequisites

- Emscripten toolchain (`em++` on `PATH`)
- Node.js runtime (`node` on `PATH`)
- Boost Graph headers available to the compiler

If Boost headers are in a non-default location, export:

- `BOOST_INCLUDE=/path/to/boost/include`

before running the scripts.

## Commands

For a full release-oriented step list (metadata, validation, publish order,
and post-publish checks), use the WASM package release checklist in
`wasm/README.md`.

Build Node-compatible wasm module:

```bash
bash wasm/scripts/build_wasm_node_module.sh
```

Run wasm formal suite:

```bash
bash wasm/scripts/run_wasm_tests.sh
```

Include large-graph comparison path:

```bash
NXPP_WASM_INCLUDE_LARGE=1 bash wasm/scripts/run_wasm_tests.sh
```

Run Node API contract tests:

```bash
bash wasm/scripts/run_wasm_node_contract_tests.sh
```

Run local native-vs-WASM overhead benchmarks:

```bash
bash wasm/scripts/run_overhead_benchmarks.sh
```

This benchmark path is intentionally local and experimental. It compares native
C++, raw WASM runtime calls, and the TypeScript facade for representative graph
construction, traversal, shortest-path, all-pairs, attribute, and multigraph
edge-ID workloads. Treat its output as reproducible diagnostics only; do not use
ad hoc local numbers as marketing or release claims.

## CI behavior

The experimental wasm workflow runs:

- `npm --prefix wasm run build:types`
- `bash wasm/scripts/build_wasm_node_module.sh`
- `NXPP_WASM_NODE_CONTRACT_SKIP_BUILD=1 bash wasm/scripts/run_wasm_node_contract_tests.sh`
- `NXPP_WASM_INCLUDE_NODE_CONTRACT=0 NXPP_WASM_INCLUDE_LARGE=1 bash wasm/scripts/run_wasm_tests.sh`
- `NXPP_WASM_NPM_PACK_SKIP_BUILD=1 bash wasm/scripts/run_npm_pack_consumer_test.sh`
- `npm --prefix wasm run check:raw-contract`
- `bash wasm/scripts/verify_wasm_runtime_reproducibility.sh`
- the browser demo build and headless smoke check

The workflow publishes a summary with:

- module-build exit code
- node-contract exit code
- suite exit code
- cleaned command outputs

Passing `wasm-experimental.yml` verifies the experimental Node package contract
and one browser demo path. It does not guarantee browser package support.

## Dependency model

Build-time dependencies (maintainer/CI side):

- emsdk / em++
- nxpp C++ sources
- Boost headers

Runtime dependencies (consumer side target model):

- prebuilt JS + wasm artifacts only
- no local Boost installation required
- no local Emscripten installation required

## Architecture direction

1. C++ core remains the source of truth
- Algorithms and data structures stay in native `nxpp` code.

2. Export layer
- The current bridge uses Embind and returns JavaScript-native values where
  practical; long-lived graph handles retain explicit ownership.

3. Node SDK wrapper
- JS/TS wrapper should expose idiomatic APIs and consistent errors.
- Wrapper should hide wasm memory and low-level bridge details.
- Wrapper layer should present a smaller NetworkX-like public type family
  instead of exposing the full C++ alias matrix directly.
- Behavioral parity matters more than literal C++ syntax replication.

4. Packaging
- Publish prebuilt runtime artifacts.
- Avoid install-time compilation for users.

## Node-compatible API surface (experimental)

The `NxppRuntime` returned by `createNxpp()` provides these constructors:

- `GraphInt`
- `GraphStr`
- `DiGraphInt`
- `DiGraphStr`
- `MultiGraphInt`
- `MultiGraphStr`
- `MultiDiGraphInt`
- `MultiDiGraphStr`

Current methods are grouped as follows.

Simple graph endpoint-oriented methods (`Graph*`, `DiGraph*`):

- `addNode(id)`
- `addEdge(source, target, weight)`
- `hasNode(id)`
- `hasEdge(source, target)`
- `nodes()`
- `neighbors(id)`
- `removeNode(id)`
- `removeEdge(source, target)`
- `getEdgeWeight(source, target)`
- `setEdgeWeight(source, target, weight)`
- `subgraph(nodes)`
- `hasNodeAttr(id, key)`
- `getNodeAttr(id, key)`
- `tryGetNodeAttr(id, key)`
- `setNodeAttr(id, key, value)`
- `hasEdgeAttr(source, target, key)`
- `getEdgeAttr(source, target, key)`
- `tryGetEdgeAttr(source, target, key)`
- `setEdgeAttr(source, target, key, value)`
- `getEdgeNumericAttr(source, target, key)`
- `bfsEdges(start)`
- `bfsTree(start)` returning `{ nodes, edges }`
- `bfsSuccessors(start)` returning sparse `{ node, successors }` entries
- `dfsEdges(start)`
- `dfsTree(start)` returning `{ nodes, edges }`
- `dfsPredecessors(start)` returning sparse `{ node, predecessor }` entries
- `dfsSuccessors(start)` returning sparse `{ node, successors }` entries
- `shortestPath(source, target)`
- `shortestPathWeighted(source, target, weightKey = "weight")`
- `shortestPathLength(source, target)`
- `shortestPathLengthWeighted(source, target, weightKey = "weight")`
- `dijkstraPath(source, target)`
- `dijkstraPathWeighted(source, target, weightKey = "weight")`
- `dijkstraShortestPaths(source)` returning a result wrapper with
  `distance`, `predecessor`, `hasPathTo(target)`, and `pathTo(target)`
- `dijkstraPathLengths(source)` returning sparse `{ node, distance }` entries
- `dijkstraPathLength(source, target)`
- `dijkstraPathLengthWeighted(source, target, weightKey = "weight")`
- `bellmanFordPath(source, target)`
- `bellmanFordPathWeighted(source, target, weightKey = "weight")`
- `bellmanFordShortestPaths(source)` returning a result wrapper with
  `distance`, `predecessor`, `hasPathTo(target)`, and `pathTo(target)`
- `bellmanFordPathLength(source, target)`
- `bellmanFordPathLengthWeighted(source, target, weightKey = "weight")`
- `dagShortestPaths(source)` returning a result wrapper with
  `distance`, `predecessor`, `hasPathTo(target)`, and `pathTo(target)`
- `floydWarshallAllPairsShortestPaths()` returning a dense `number[][]`
  weighted matrix in stable node order
- `floydWarshallAllPairsShortestPathsMap()` returning serializable
  `{ source, distances: [{ target, distance }] }` entries
- `kruskalMinimumSpanningTree()` returning `{ source, target }` edge entries
- `primMinimumSpanningTree(root)` returning `{ source, target }` edge entries
- `clear()`
- `dispose()` for explicit facade-side lifetime management

Components methods are exposed only on graph families where the native
algorithm applies:

- `connectedComponents()` on `Graph*` and `MultiGraph*`, returning `T[][]`
- `stronglyConnectedComponents()` on `DiGraph*` and `MultiDiGraph*`, returning
  `T[][]`

Centrality methods are exposed on all graph families:

- `degreeCentrality()` returning `{ node, score }` entries
- `pagerank(tolerance = 1e-6, maxIterations = 100)` returning `{ node, score }`
  entries
- `betweennessCentrality()` returning `{ node, score }` entries

Flow methods are exposed on all graph families:

- `maximumFlow(source, target, capacityKey = "capacity")` returning
  `{ value, edgeFlows, edgeFlowsById }`
- `minimumCut(source, target, capacityKey = "capacity")` returning
  `{ value, reachable, nonReachable, cutEdges, cutEdgeIds }`
- `maxFlowMinCost(source, target, capacityKey = "capacity", weightKey = "weight")`
  returning `{ flow, cost, edgeFlows, edgeFlowsById }`
- `maxFlowMinCostSuccessiveShortestPath(source, target, capacityKey = "capacity", weightKey = "weight")`
  returning `{ flow, cost, edgeFlows, edgeFlowsById }`
- `pushRelabelMaximumFlow(source, target, capacityKey = "capacity", weightKey = "weight")`
  returning the staged max-flow value
- `cycleCanceling(weightKey = "weight")` returning the staged min-cost value

Multigraph methods (`MultiGraph*`, `MultiDiGraph*`) include all simple methods
and additionally expose edge-ID-specific APIs:

- `hasEdgeId(edgeId)`
- `edgeIds()`
- `edgeIdsBetween(source, target)`
- `getEdgeEndpoints(edgeId)` returning `EdgeEndpointsInt` or `EdgeEndpointsStr`
- `getEdgeWeightById(edgeId)`
- `setEdgeWeightById(edgeId, weight)`
- `hasEdgeAttrById(edgeId, key)`
- `getEdgeAttrById(edgeId, key)`
- `tryGetEdgeAttrById(edgeId, key)`
- `setEdgeAttrById(edgeId, key, value)`
- `getEdgeNumericAttrById(edgeId, key)`
- `removeEdgeById(edgeId)`

Weighted shortest-path wrappers currently accept only the built-in `"weight"`
channel.

Minimum-spanning-tree wrappers return plain edge DTO arrays. The Prim wrapper
uses the native parent map internally and omits the root self-parent from the
JS-facing edge list.

Facade graph instances own Embind-backed WASM objects and expose explicit
`dispose()` lifetime management. Disposal is idempotent, operations after
disposal throw a clear facade error, and runtimes with `Symbol.dispose` get the
same disposal path attached to that symbol.

Runtime failures from the raw C++/Embind layer are normalized at the TypeScript
facade boundary. Common invalid graph operations throw JavaScript `Error`
instances with a stable `WASM graph operation failed: ...` prefix, while
successful behavior remains unchanged.

This is intentionally a narrow first slice and should not be treated as the
final taxonomy of wasm graph types.

Current runtime behavior:

- each exported class owns one concrete backend (no runtime backend switching)
- `*Int` bindings accept only integer-valued JS numbers as node IDs
- `*Str` bindings accept only JS strings as node IDs
- wrong node-ID types throw `std::runtime_error` with explicit messages
- wasm attribute writes accept only `string`, finite `number`, and `boolean`
- wasm `tryGet...` attribute reads return `null` when the value is missing or
  unsupported by the wasm attribute contract
- endpoint-based multigraph attribute reads/writes remain convenience-oriented;
  the `*ById` methods are the precise path for one concrete parallel edge
- traversal tree DTOs are built as explicit JS data (`{ nodes, edges }`) rather
  than as nested graph wrapper instances, which keeps the wasm bridge stable
  while preserving the traversal result content

## Public contract and coverage

The package root exposes `createNxpp()` and TypeScript contracts. Its runtime
context provides eight concrete graph constructors; the raw Embind module is
an internal asset. [API_POLICY.md](API_POLICY.md) defines validation, result,
and lifetime rules. [README.md](README.md) contains the current feature matrix
and migration example. Node behavior is verified by the contract and packed
consumer tests. Work beyond the current surface is tracked in GitHub issues.

## Strategy note: Emscripten in include headers vs adapter layer

Proposed idea:

- put Emscripten-specific directives directly in public `include/` headers to
  reduce wrapper rewrite effort.

Recommendation for this repository:

- avoid putting Emscripten bindings/macros in canonical public headers
- keep `include/` platform-agnostic and native-first
- keep wasm-specific glue inside `wasm/` (binding/adapter layer)

Why this is safer against drift:

- avoids polluting core headers with runtime-specific macros
- reduces risk of native API behavior changes caused by wasm concerns
- keeps a single C++ source of truth while isolating bridge logic
- makes future runtime targets (Node/browser/other) easier to evolve

Practical anti-drift pattern:

1. keep algorithmic logic in core `include/` + tests
2. expose minimal stable exported surface in `wasm/` bindings
3. validate parity with focused wasm contract tests against native behavior
4. only promote API families when behavior and shapes are pinned by tests

## Troubleshooting

### Emscripten not found

Symptom:

- `em++ not found`

Fix:

```bash
source /path/to/emsdk/emsdk_env.sh
```

On this repository the WSL-local path used during development is:

```bash
source "$HOME/.local/opt/emsdk/emsdk_env.sh"
```

### Boost headers not found during wasm build

Symptom:

- `boost/... file not found`

Fix:

```bash
BOOST_INCLUDE=/path/to/boost/include bash wasm/scripts/build_wasm_node_module.sh
```

## Roadmap

The [issue-backed WASM roadmap](https://github.com/Mik1810/nxpp/issues/177)
records the remaining `1.0.0` work and completion gates. The
[WASM issue list](https://github.com/Mik1810/nxpp/issues?q=is%3Aissue+label%3Awasm)
tracks later feature proposals. Release preparation and publication are
separate from the implementation roadmap.
