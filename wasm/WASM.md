# WebAssembly and Node Integration (Emscripten)

This document is the single source of truth for the `nxpp` wasm path.

It combines:

- current experimental scope
- architecture direction
- build and CI behavior
- troubleshooting notes
- phased development plan to reach broad library coverage

## Status

- experimental portability/build target
- focused on Node runtime first
- not yet a full JavaScript/TypeScript API parity layer
- now shipped as the experimental npm package `@mik1810/nxpp-wasm`

The current binding exposes an experimental typed graph surface centered on the
explicit runtime classes (`GraphInt`, `GraphStr`, `DiGraphInt`, `DiGraphStr`,
`MultiGraphInt`, `MultiGraphStr`, `MultiDiGraphInt`, `MultiDiGraphStr`) plus
the completed `attributes.hpp` and `traversal.hpp` blocks plus the
full `shortest_paths.hpp` block, including Floyd-Warshall all-pairs results,
plus the current `spanning_tree.hpp` minimum-spanning-tree block.
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

## Prerequisites

- Emscripten toolchain (`em++` on `PATH`)
- Node.js runtime (`node` on `PATH`)
- Boost Graph headers available to the compiler

If Boost headers are in a non-default location, export:

- `BOOST_INCLUDE=/path/to/boost/include`

before running the scripts.

## Commands

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

## CI behavior

The experimental wasm workflow runs:

- `bash wasm/scripts/build_wasm_node_module.sh`
- `NXPP_WASM_NODE_CONTRACT_SKIP_BUILD=1 bash wasm/scripts/run_wasm_node_contract_tests.sh`
- `NXPP_WASM_INCLUDE_NODE_CONTRACT=0 NXPP_WASM_INCLUDE_LARGE=1 bash wasm/scripts/run_wasm_tests.sh`

The workflow publishes a summary with:

- module-build exit code
- node-contract exit code
- suite exit code
- cleaned command outputs

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
- Current path uses Embind for rapid iteration and validation.
- Mid-term target is a narrower stable ABI-oriented bridge.

3. Node SDK wrapper
- JS/TS wrapper should expose idiomatic APIs and consistent errors.
- Wrapper should hide wasm memory and low-level bridge details.
- Wrapper layer should present a smaller NetworkX-like public type family
  instead of exposing the full C++ alias matrix directly.
- Behavioral parity matters more than literal C++ syntax replication.

4. Packaging
- Publish prebuilt runtime artifacts.
- Avoid install-time compilation for users.

## Current implementation direction

Near-term implementation order is now explicit:

1. finish `graph.hpp` coverage first
2. reshape the public graph API toward:
   - `Graph`
   - `DiGraph`
   - `MultiGraph`
   - `MultiDiGraph`
3. keep internal numeric and string graph backends distinct instead of
   normalizing all node IDs to one representation
4. only after that move to later semantic headers such as:
   - `components.hpp`
   - `flow.hpp`
5. treat any later JS convenience layer as behavior-focused and explicit,
   rather than trying to reproduce every native `operator[]` form literally

Why this direction:

- `graph.hpp` is the core public surface that the other semantic headers build
  on top of
- the C++ alias list is too detailed and partly redundant for a good JS API
- JavaScript users benefit more from a small graph-type family, similar to the
  NetworkX model
- supporting both numeric and string node IDs should not force the project to
  give up the faster integer-backed algorithms internally
- so the wasm plan should not jump to later headers before the graph core and
  the public graph-type model are nailed down

## Node-compatible API surface (experimental)

Current exported classes:

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

## Planned public graph taxonomy

The intended public API uses a split model:

- explicit runtime classes:
  - `GraphInt` / `GraphStr`
  - `DiGraphInt` / `DiGraphStr`
  - `MultiGraphInt` / `MultiGraphStr`
  - `MultiDiGraphInt` / `MultiDiGraphStr`
- generic TypeScript interfaces:
  - `Graph<T>`
  - `DiGraph<T>`
  - `MultiGraph<T>`
  - `MultiDiGraph<T>`

Design rules for that family:

- runtime class names stay explicit and honest about node-ID kind
- TypeScript generics remain static typing only (no runtime generic dispatch)
- weighted behavior remains available, but not encoded into type names
- the full C++ alias list from `graph.hpp` remains an internal binding concern
- node IDs support both `number` and `string` through explicit class choice

This means the current experimental typed graph family is already aligned with
the `graph.hpp` node-type and multigraph distinctions.

## API contract v0 (Node)

The following contract is the current stability baseline for the exported
experimental `Graph`, `DiGraph`, `MultiGraph`, and `MultiDiGraph` surface.

### Method signatures

- Simple graph methods (`Graph*`, `DiGraph*`):
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
  - `clear()`

- Multigraph-only additions (`MultiGraph*`, `MultiDiGraph*`):
  - `hasEdgeId(edgeId)`
  - `edgeIds()`
  - `edgeIdsBetween(source, target)`
  - `getEdgeEndpoints(edgeId)`
  - `getEdgeWeightById(edgeId)`
  - `setEdgeWeightById(edgeId, weight)`
  - `removeEdgeById(edgeId)`

`EdgeEndpoints` contract:

- `source(): number | string`
- `target(): number | string`

### Return-shape guarantees (v0)

- `nodes()` returns an array-like list of node IDs using the graph class node-ID kind
- `edgeIds()` returns an array-like list of numeric wrapper-managed edge IDs
- `edgeIdsBetween()` returns an array-like list of numeric edge IDs matching
  the endpoint pair
- `neighbors()` returns an array-like list of node IDs using the graph class node-ID kind
- `getEdgeWeight()` returns a numeric scalar weight
- `getEdgeEndpoints()` returns an `EdgeEndpoints` object
- `getEdgeWeightById()` returns a numeric scalar weight
- mutation methods return no value

### Error contract (v0)

- methods throw JS exceptions when the wrapped C++ operation fails
- at minimum, the following invalid operations must throw:
  - adding or querying with unsupported node ID shapes
  - `neighbors()` on a missing node
  - `removeNode()` on a missing node
  - `removeEdge()` on a missing edge or missing endpoint
  - `getEdgeWeight()` on a missing edge
  - `getEdgeWeightById()` on a missing edge ID
  - `setEdgeWeightById()` on a missing edge ID
  - `getEdgeEndpoints()` on a missing edge ID
  - `removeEdgeById()` on a missing edge ID
- exact message text is not yet guaranteed in v0; exception presence is
  guaranteed by contract tests

### Compatibility rules (v0)

The following are breaking changes and require versioned migration notes:

- method rename/removal
- parameter list or parameter-type changes
- return-type/shape changes
- removing currently-thrown failure behavior for invalid graph-core queries

The following are non-breaking for v0:

- adding new methods
- adding stricter validation that still throws on invalid input without
  changing existing method signatures
- improving exception message text without removing exception behavior

## Feature matrix (tracking)

| Area | Status | Current coverage | Next step |
|---|---|---|---|
| Toolchain and build | Active | Emscripten + Node module build script | Keep CI stable on Node LTS matrix |
| Core graph lifecycle | Active | explicit typed runtime constructors for `Graph*`, `DiGraph*`, `MultiGraph*`, `MultiDiGraph*` plus `clear()` | Expand lifecycle-oriented helpers only when they map cleanly to `graph.hpp` |
| Graph mutation APIs | Partial | endpoint-based mutation on simple/multi graphs plus precise `removeEdgeById` on multigraphs | Add next `graph.hpp` mutation slices while preserving simple-vs-multigraph API policy |
| Query APIs | Partial | endpoint-based queries on simple graphs plus edge-id queries on multigraphs | Expand query coverage module-by-module without exposing unstable aliases |
| Graph parity layer | In design | explicit methods only | Keep behavior close to native `graph.hpp` while avoiding a misleading one-to-one operator-syntax imitation |
| Shortest paths | Covered | single-pair, single-source Dijkstra/Bellman-Ford/DAG, and Floyd-Warshall all-pairs wrappers | Keep contract tests aligned with future facade changes |
| Spanning tree | Covered | Kruskal and rooted Prim MST edge-list wrappers | Continue with components / centrality / flow blocks |
| Components/topology | Not started | none | Add first exported component/topology slice |
| Spanning/centrality | Not started | none | Add centrality subset after topology |
| Flow/multigraph precision | Partial | `MultiGraph*` and `MultiDiGraph*` edge-id API with precise `removeEdgeById` and endpoint lookup wrappers | Add flow-oriented exports that preserve edge-id precision |
| TypeScript surface | Active | generic TS interfaces + explicit typed runtime class declarations (`wasm/dist/index.d.ts`) and facade source tree under `wasm/ts/` | Expand algorithm module typings and wrappers as new wasm exports land |
| NPM packaging | Partial | installable package now exposes `dist/index.js` + `dist/index.d.ts` over prebuilt wasm artifacts | Keep release automation aligned with facade build/publish flow |

Keep this table updated whenever a new exported API family is merged.

For the consumer-facing parity/stability snapshot used in package docs, keep
the companion matrix in `wasm/README.md` aligned with this internal tracking
table.

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

## Development plan

## Phase A: Toolchain and baseline stability

- keep Emscripten setup reliable in local and CI contexts
- keep include-path configurability (`BOOST_INCLUDE`)
- keep module build + wasm suite green

Exit criteria:

- baseline commands pass in CI and clean dev container

## Phase B: Stable minimal Node contract

Scope:

- graph creation/reset
- node/edge insertion and existence checks
- Dijkstra distance/path

Tasks:

- harden input validation and deterministic errors
- define explicit return-shape contracts
- add TypeScript declarations
- introduce functional Node contract tests

Exit criteria:

- Node contract is documented and verified on Node LTS matrix

## Phase C: ABI hardening

Tasks:

- introduce a stable ABI-oriented bridge layer
- route wrapper calls through that bridge
- add ownership/lifetime regression tests

Exit criteria:

- public wrapper no longer depends on fragile binding details

## Phase D: Coverage expansion toward full library

Recommended rollout order:

1. Traversal + shortest paths
2. Components + topological utilities
3. Spanning tree + centrality
4. Flow algorithms
5. Multigraph precision APIs

For each family:

- implement export wiring
- implement wrapper + typing
- add parity tests against native suite behavior
- add edge-case and error-path tests

Exit criteria:

- documented feature matrix reaches agreed full-coverage target

## Phase E: Packaging and release readiness

Tasks:

- define package layout for wasm/node artifacts
- configure exports map for ESM (and optional CJS strategy)
- include version metadata and release notes linkage
- run clean-environment install/use validation in CI

Exit criteria:

- consumers can install and use Node wasm package without native toolchain

## Testing strategy

- keep C++ suite as correctness backbone
- add wasm/node tests in layers:
  - module build verification
  - API contract tests
  - parity tests against selected native scenarios
  - negative/error-path tests
- enforce CI gates for API-shape regressions

## Risks and mitigations

- API drift between C++ and wrapper
  - mitigation: centralized API-contract mapping and contract tests
- lifetime/memory bugs at JS/wasm boundary
  - mitigation: explicit ownership rules + lifecycle tests
- artifact-size growth
  - mitigation: track artifact sizes and add budget checks

## Definition of done (final layer)

- Node consumers use the wasm layer without local Boost/Emscripten
- core algorithm families are exposed through stable Node APIs
- CI validates build, runtime behavior, and installability
- docs clearly define scope, guarantees, and versioning policy
- package published on npm with clear usage instructions and release notes
