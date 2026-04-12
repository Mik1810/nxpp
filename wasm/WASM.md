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

## Objectives

Primary objectives:

- compile and run `nxpp` algorithms under Emscripten
- provide a stable, consumer-friendly Node-facing API layer over wasm
- avoid requiring Boost/Emscripten installation on consumer machines

Longer-term objective:

- extend coverage from a minimal API subset to broad library support

## Current scope

Current wasm lane includes:

- Node-compatible bindings in `wasm/node/nxpp_node_bindings.cpp`
- Node-oriented wasm module build script in `wasm/scripts/build_wasm_node_module.sh`
- Node API contract tests in `wasm/node/node_api_contract_test.mjs`
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

4. Packaging
- Publish prebuilt runtime artifacts.
- Avoid install-time compilation for users.

## Node-compatible API surface (experimental)

Current exported class: `DiGraph`

Current methods:

- `addNode(id)`
- `addEdge(source, target, weight)`
- `hasNode(id)`
- `hasEdge(source, target)`
- `nodes()`
- `dijkstraDistance(source, target)`
- `dijkstraPath(source, target)`
- `clear()`

This is intentionally a narrow first slice.

## API contract v0 (Node)

The following contract is the current stability baseline for the exported
`DiGraph` surface.

### Method signatures

- `addNode(id: number): void`
- `addEdge(source: number, target: number, weight: number): void`
- `hasNode(id: number): boolean`
- `hasEdge(source: number, target: number): boolean`
- `nodes(): number[]`
- `dijkstraDistance(source: number, target: number): number`
- `dijkstraPath(source: number, target: number): number[]`
- `clear(): void`

### Return-shape guarantees (v0)

- `nodes()` returns an array-like list of numeric node IDs
- `dijkstraDistance()` returns a numeric scalar distance
- `dijkstraPath()` returns an array-like ordered list of node IDs
- mutation methods return no value

### Error contract (v0)

- methods throw JS exceptions when the wrapped C++ operation fails
- at minimum, invalid shortest-path queries on missing/unreachable targets must
  throw
- exact message text is not yet guaranteed in v0; exception presence is
  guaranteed by contract tests

### Compatibility rules (v0)

The following are breaking changes and require versioned migration notes:

- method rename/removal
- parameter list or parameter-type changes
- return-type/shape changes
- removing currently-thrown failure behavior for invalid path queries

The following are non-breaking for v0:

- adding new methods
- adding stricter validation that still throws on invalid input without
  changing existing method signatures
- improving exception message text without removing exception behavior

## Feature matrix (tracking)

| Area | Status | Current coverage | Next step |
|---|---|---|---|
| Toolchain and build | Active | Emscripten + Node module build script | Keep CI stable on Node LTS matrix |
| Core graph lifecycle | Partial | `DiGraph` creation and `clear()` | Add explicit lifecycle/error contract tests |
| Graph mutation APIs | Partial | `addNode`, `addEdge` | Add remove/update operations with deterministic semantics |
| Query APIs | Partial | `hasNode`, `hasEdge`, `nodes` | Add shape guarantees and edge-case validation |
| Shortest paths | Partial | `dijkstraDistance`, `dijkstraPath` | Expand to additional shortest-path helpers |
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
