# nxpp-wasm Architecture

`@mik1810/nxpp-wasm` is a single npm package with two internal layers:

1. C++/WASM binding layer
2. TypeScript facade layer

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

The TypeScript facade must not reimplement graph algorithms. Algorithmic work
belongs in the C++ library and the WASM binding layer.

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
    core/
    algorithms/
    internal/
    index.ts
    load.ts
    types.ts
  dist/
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

The runtime loading path is:

1. `wasm/nxpp.mjs` loads the generated Emscripten module.
2. `wasm/ts/load.ts` creates or exposes the raw runtime module.
3. `wasm/ts/core/*.ts` wraps raw graph instances.
4. `wasm/ts/index.ts` exports the public package facade.

The published package entrypoint is the compiled facade in `dist/index.js`.
The raw runtime remains available as the package runtime artifact, but the
facade is the public API boundary for normal consumers.

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

The current centrality and flow modules are structural placeholders for planned
API slices. They should not be treated as complete public facade coverage until
their binding implementations, TypeScript result models, and contract tests are
added.

## Error Boundary

The raw C++/Embind layer may throw native or Embind-specific errors. The facade
wraps raw graph objects and normalizes runtime failures to JavaScript `Error`
instances with the prefix:

```text
WASM graph operation failed:
```

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
