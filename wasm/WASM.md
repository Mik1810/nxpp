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
- not yet a standalone package-distribution channel

The current binding still exposes only a narrow experimental `DiGraph`-first
slice. That should be read as an implementation stepping stone, not as the
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

- Node-compatible bindings in `wasm/node/nxpp_bindings.cpp`
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
   - `attributes.hpp`
   - broader `shortest_paths.hpp`
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

Current exported class: `DiGraph`

Current methods:

- `addNode(id)`
- `addEdge(source, target, weight)`
- `hasNode(id)`
- `hasEdge(source, target)`
- `hasEdgeId(edgeId)`
- `nodes()`
- `edgeIds()`
- `edgeIdsBetween(source, target)`
- `neighbors(id)`
- `removeNode(id)`
- `removeEdge(source, target)`
- `getEdgeWeight(source, target)`
- `getEdgeEndpoints(edgeId)`
- `getEdgeWeightById(edgeId)`
- `setEdgeWeightById(edgeId, weight)`
- `dijkstraShortestPaths(source)`
- `clear()`

Current shortest-path result object: `DijkstraResult`

- `hasPathTo(target)`
- `distanceTo(target)`
- `pathTo(target)`
- `reachableNodes()`

This is intentionally a narrow first slice and should not be treated as the
final taxonomy of wasm graph types.

Current `DiGraph` runtime behavior:

- accepts integer-valued JS numbers or strings as node IDs
- chooses its internal backend lazily from the first observed node ID
- rejects mixed numeric/string node IDs after that first choice
- preserves the chosen node-ID shape in arrays, result objects, and endpoint
  wrappers returned to JavaScript

## Planned public graph taxonomy

The intended public JavaScript graph family is now:

- `Graph`
- `DiGraph`
- `MultiGraph`
- `MultiDiGraph`

Design rules for that family:

- public JS graph names should be simple and NetworkX-like
- weighted behavior remains available, but not encoded into the public type
  names
- the full C++ alias list from `graph.hpp` remains an internal binding concern
- node IDs should support both `number` and `string`
- backend selection should happen lazily from the first observed node ID type
- once selected, a graph should reject mixed numeric/string node IDs

This means the current experimental `DiGraph` export is a staging shape, not
the final public type story.

## API contract v0 (Node)

The following contract is the current stability baseline for the exported
experimental `DiGraph` surface only.

### Method signatures

- `addNode(id: number | string): void`
- `addEdge(source: number | string, target: number | string, weight: number): void`
- `hasNode(id: number | string): boolean`
- `hasEdge(source: number | string, target: number | string): boolean`
- `hasEdgeId(edgeId: number): boolean`
- `nodes(): Array<number | string>`
- `edgeIds(): number[]`
- `edgeIdsBetween(source: number | string, target: number | string): number[]`
- `neighbors(id: number | string): Array<number | string>`
- `removeNode(id: number | string): void`
- `removeEdge(source: number | string, target: number | string): void`
- `getEdgeWeight(source: number | string, target: number | string): number`
- `getEdgeEndpoints(edgeId: number): EdgeEndpoints`
- `getEdgeWeightById(edgeId: number): number`
- `setEdgeWeightById(edgeId: number, weight: number): void`
- `dijkstraShortestPaths(source: number | string): DijkstraResult`
- `clear(): void`

`DijkstraResult` contract:

- `hasPathTo(target: number | string): boolean`
- `distanceTo(target: number | string): number`
- `pathTo(target: number | string): Array<number | string>`
- `reachableNodes(): Array<number | string>`

`EdgeEndpoints` contract:

- `source(): number | string`
- `target(): number | string`

### Return-shape guarantees (v0)

- `nodes()` returns an array-like list of node IDs using the graph's chosen
  runtime node-ID kind
- `edgeIds()` returns an array-like list of numeric wrapper-managed edge IDs
- `edgeIdsBetween()` returns an array-like list of numeric edge IDs matching
  the endpoint pair
- `neighbors()` returns an array-like list of node IDs using the graph's chosen
  runtime node-ID kind
- `dijkstraShortestPaths()` returns a `DijkstraResult` object
- `getEdgeWeight()` returns a numeric scalar weight
- `getEdgeEndpoints()` returns an `EdgeEndpoints` object
- `getEdgeWeightById()` returns a numeric scalar weight
- `distanceTo()` returns a numeric scalar distance
- `pathTo()` returns an array-like ordered list of node IDs using the graph's
  chosen runtime node-ID kind
- `reachableNodes()` returns an array-like list of node IDs present in the
  result using the graph's chosen runtime node-ID kind
- mutation methods return no value

### Error contract (v0)

- methods throw JS exceptions when the wrapped C++ operation fails
- at minimum, the following invalid operations must throw:
  - adding or querying with unsupported node ID shapes
  - mixing numeric and string node IDs inside the same graph
  - invalid shortest-path source queries
  - invalid `DijkstraResult` target queries
  - `neighbors()` on a missing node
  - `removeNode()` on a missing node
  - `removeEdge()` on a missing edge or missing endpoint
  - `getEdgeWeight()` on a missing edge
  - `getEdgeWeightById()` on a missing edge ID
  - `setEdgeWeightById()` on a missing edge ID
  - `getEdgeEndpoints()` on a missing edge ID
- exact message text is not yet guaranteed in v0; exception presence is
  guaranteed by contract tests

### Compatibility rules (v0)

The following are breaking changes and require versioned migration notes:

- method rename/removal
- parameter list or parameter-type changes
- return-type/shape changes
- removing currently-thrown failure behavior for invalid shortest-path queries

The following are non-breaking for v0:

- adding new methods
- adding stricter validation that still throws on invalid input without
  changing existing method signatures
- improving exception message text without removing exception behavior

## Feature matrix (tracking)

| Area | Status | Current coverage | Next step |
|---|---|---|---|
| Toolchain and build | Active | Emscripten + Node module build script | Keep CI stable on Node LTS matrix |
| Core graph lifecycle | Partial | experimental `DiGraph` creation, `clear()`, and lazy numeric/string backend selection | Introduce the broader `Graph` / `DiGraph` / `MultiGraph` / `MultiDiGraph` family on top of the current core slice |
| Graph mutation APIs | Partial | `addNode`, `addEdge`, `removeNode`, `removeEdge`, `setEdgeWeightById` | Add remaining `graph.hpp` mutation surface and then generalize it across the planned public graph family |
| Query APIs | Partial | `hasNode`, `hasEdge`, `hasEdgeId`, `nodes`, `edgeIds`, `edgeIdsBetween`, `neighbors`, `getEdgeWeight`, `getEdgeWeightById`, `getEdgeEndpoints` with numeric/string runtime preservation | Add remaining `graph.hpp` query surface and extend the same node-type-aware model to the rest of the public graph family |
| Graph parity layer | In design | explicit methods only | Keep behavior close to native `graph.hpp` while avoiding a misleading one-to-one operator-syntax imitation |
| Shortest paths | Partial | `dijkstraShortestPaths` + `DijkstraResult` (`distanceTo`, `pathTo`) | Expand to additional shortest-path helpers after `graph.hpp` coverage is stronger |
| Components/topology | Not started | none | Add first exported component/topology slice |
| Spanning/centrality | Not started | none | Add centrality subset after topology |
| Flow/multigraph precision | Not started | none | Add edge-id-safe flow/multigraph exports |
| TypeScript surface | Not started | none | Add `.d.ts` contract for current API |
| NPM packaging | Not started | local scripts only | Build installable runtime package |

Keep this table updated whenever a new exported API family is merged.

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
source /workspaces/emsdk/emsdk_env.sh
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
