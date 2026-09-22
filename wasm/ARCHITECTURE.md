# nxpp-wasm Architecture

This document describes the current implementation of the experimental WASM
package. The `1.0.0` completion gates are recorded in
[`ARCHITECTURE_1_0.md`](ARCHITECTURE_1_0.md). The package version remains
`0.6.0` until those gates are met; its root API already uses explicit runtime
contexts.

`@mik1810/nxpp-wasm` is a single npm package with three internal layers:

1. C++/WASM binding layer
2. Node runtime loader
3. TypeScript facade layer

The C++ library remains the source of truth. The WASM package exposes selected
C++ graph behavior to Node.js through Emscripten/Embind and wraps that runtime
with a TypeScript-facing API.

## Layer Responsibilities

The C++/WASM binding layer is responsible for:

- compiling the native `nxpp` implementation to WebAssembly
- exposing concrete runtime graph classes through Embind
- keeping bindings close to the native module structure
- converting native graph values into bridge-safe runtime values

The TypeScript facade layer is responsible for:

- exporting the public npm entrypoint
- exposing generic TypeScript interfaces
- wrapping concrete WASM runtime classes
- validating JavaScript inputs where practical
- normalizing raw runtime errors
- adapting raw Embind values into stable JavaScript result shapes
- owning explicit graph lifetime management through `dispose()`

The TypeScript facade must not reimplement graph algorithms or mirror native
semantic state. Algorithmic work and staged algorithm lifecycles belong in the
C++ library and the WASM binding layer.

## Current Layout

The current implementation is organized around module-level bindings and a
separate TypeScript facade:

```text
wasm/
  include/
    nxpp_wasm.hpp
    nxpp_wasm/
      common/
      graph.hpp
      multigraph.hpp
      attributes.hpp
      traversal.hpp
      shortest_paths.hpp
      spanning_tree.hpp
      components.hpp
      centrality.hpp
      flow.hpp
      generators.hpp
      sat.hpp
      topological_sort.hpp
      register_all.hpp
  src/
    common/
    graph.cpp
    multigraph.cpp
    attributes.cpp
    traversal.cpp
    shortest_paths.cpp
    spanning_tree.cpp
    components.cpp
    centrality.cpp
    flow.cpp
    generators.cpp
    sat.cpp
    topological_sort.cpp
    nxpp_wasm.cpp
  ts/
    algorithms/
    core/
    internal/
    legacy/
      core/
      index.ts
      load.ts
    runtime/
    index.ts
    types.ts
  dist/
  runtime/
    node.mjs
    node.wasm
  build/
```

`wasm/src/nxpp_wasm.cpp` defines the Embind module and calls
`nxpp_wasm::register_all_bindings()`. `register_all_bindings()` is the single
registration entrypoint for module binding registration.

## Runtime Classes and Facade Types

The raw runtime exposes explicit classes for concrete node-ID families:

- `GraphInt`
- `GraphStr`
- `DiGraphInt`
- `DiGraphStr`
- `MultiGraphInt`
- `MultiGraphStr`
- `MultiDiGraphInt`
- `MultiDiGraphStr`

The TypeScript facade exports those concrete classes and also exports generic
interfaces:

- `Graph<T extends number | string>`
- `DiGraph<T extends number | string>`
- `MultiGraph<T extends number | string>`
- `MultiDiGraph<T extends number | string>`

This is intentionally a split model. Runtime construction is explicit, while
generic interfaces provide static TypeScript typing.

## Loading Flow

1. `wasm/ts/runtime/node.ts` initializes one raw Node module per call.
2. `wasm/ts/runtime/context.ts` validates it and returns a frozen
   `NxppRuntime` containing module-bound constructors.
3. Graphs and subgraphs created by one context remain bound to that context.

The compiled facade in `dist/index.js` is the only supported package
entrypoint. It exports `createNxpp()` and public TypeScript contracts. The raw
runtime under `runtime/` is an internal package asset used by the Node loader,
not a public export.

The `legacy` source directory is retained only as an internal migration
reference and is excluded from the package. It is not a supported subpath.

## Binding Modules

Graph class registration is split across graph families:

- `graph.cpp` registers simple graph and directed graph runtime classes.
- `multigraph.cpp` registers multigraph runtime classes and edge endpoint
  wrappers.

Shared capability modules attach methods to those runtime classes:

- `attributes.cpp`
- `traversal.cpp`
- `shortest_paths.cpp`
- `spanning_tree.cpp`
- `components.cpp`
- `centrality.cpp`
- `flow.cpp`

## Error Boundary

The raw C++/Embind layer may throw native or Embind-specific errors. The facade
wraps raw graph objects and normalizes runtime failures to JavaScript `Error`
instances with the prefix:

```text
WASM graph operation failed:
```

Optimized Emscripten builds export the runtime exception-message and reference-
count helpers. The facade uses them to preserve `std::exception::what()` and
release caught native exceptions without reproducing the failed semantic check.

Facade-side validation errors may use `TypeError` when the caller passed an
invalid JavaScript type before crossing into WASM.

## Data Boundary

Public methods should prefer JavaScript arrays and small object shapes over
leaking low-level Embind implementation details.

Current result examples include:

- traversal edge arrays
- traversal tree objects
- shortest-path result wrappers
- all-pairs shortest-path matrices and map entries
- spanning-tree edge arrays
- component group arrays
- centrality score arrays
- max-flow, min-cut, and min-cost-flow result objects

Multigraph edge endpoint wrappers are still used for precise edge-ID lookup.
Future APIs should choose serializable DTOs unless an Embind wrapper is needed
for lifetime or identity reasons.

## Runtime Support Boundary

Node.js is the current supported experimental runtime target. The contract
tests and package validation are Node-oriented.

Browser support is intentionally separate from Node stabilization. Browser
loading strategy, bundler assumptions, and demo scope should be investigated in
a separate path before any browser compatibility promise is made.

## Verification

TypeScript facade and declaration changes should pass:

```bash
npm --prefix wasm run build:types
```

C++ binding, runtime behavior, serialization, and error mapping changes should
also pass:

```bash
bash wasm/scripts/run_wasm_node_contract_tests.sh
```

The contract suite is the current behavioral baseline for the Node-facing WASM
facade.
