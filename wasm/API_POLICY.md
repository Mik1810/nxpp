# nxpp-wasm API Policy

This document defines the current policy for the experimental Node.js
TypeScript facade exposed by `@mik1810/nxpp-wasm`.

The C++ library remains the source of truth for graph data structures and
algorithms. TypeScript is the public facade layer: it provides typed entry
points, validation at the JavaScript boundary, predictable result shapes, and
error normalization. It must not reimplement graph algorithms.

## Stability Scope

The current compatibility target is Node.js. Browser support is an
investigation item and is not part of the supported runtime contract.

The package stays as a single npm package named `@mik1810/nxpp-wasm`. The
package should contain the compiled wasm runtime, Emscripten JS glue, compiled
TypeScript facade, and declaration files.

## Construction Model

Runtime classes stay explicit:

- `GraphInt`
- `GraphStr`
- `DiGraphInt`
- `DiGraphStr`
- `MultiGraphInt`
- `MultiGraphStr`
- `MultiDiGraphInt`
- `MultiDiGraphStr`

Generic TypeScript interfaces provide static typing:

- `Graph<T extends number | string>`
- `DiGraph<T extends number | string>`
- `MultiGraph<T extends number | string>`
- `MultiDiGraph<T extends number | string>`

Concrete runtime classes implement the corresponding generic interfaces. Users
choose the concrete class at runtime and may type it through the generic
interface:

```ts
const g: DiGraph<number> = new DiGraphInt();
const h: DiGraph<string> = new DiGraphStr();
```

TypeScript generics are compile-time only. The public API must not depend on
runtime dispatch from erased generic parameters.

## Type Policy

Node IDs are either JavaScript numbers or strings:

- `*Int` classes accept only integer-valued JavaScript numbers.
- `*Str` classes accept only JavaScript strings.
- Boolean, object, array, `null`, `undefined`, `NaN`, and non-integer numeric
  node IDs are invalid.

Weights are finite JavaScript numbers.

Multigraph edge IDs are non-negative integer-valued JavaScript numbers.

Attribute writes currently accept only:

- string
- finite number
- boolean

`null` is reserved as a missing-value sentinel for `tryGet...` attribute reads
and is not currently a storable attribute value. Supporting storable `null`
would be a public API change and must update the TypeScript types, runtime
validation, serialization rules, and contract tests together.

## Graph API Policy

Simple graph APIs are endpoint-based. They should not expose edge-ID-centric
methods unless the public behavior requires precise edge identity.

Multigraph APIs may expose edge-ID methods because parallel edges require
stable identity. Endpoint-based multigraph methods remain convenience APIs and
may select an aggregate or first matching edge according to their documented
behavior. Precise parallel-edge operations should use `*ById` methods.

Subgraph methods return the same graph family:

- `Graph<T>.subgraph(...)` returns `Graph<T>`
- `DiGraph<T>.subgraph(...)` returns `DiGraph<T>`
- `MultiGraph<T>.subgraph(...)` returns `MultiGraph<T>`
- `MultiDiGraph<T>.subgraph(...)` returns `MultiDiGraph<T>`

Facade graph instances own Embind-backed WASM objects. They expose `dispose()`
for explicit lifetime management. `dispose()` is idempotent. Operations after
disposal must throw a clear JavaScript `Error`.

## Algorithm Result Policy

Algorithms run in C++/WASM. The TypeScript facade may adapt raw Embind values
into stable JavaScript result shapes, but it must not reimplement algorithms.

Preferred result shapes are arrays and small serializable objects. Current
examples include:

- traversal edges as `{ source, target }`
- traversal trees as `{ nodes, edges }`
- sparse successor entries as `{ node, successors }`
- sparse predecessor entries as `{ node, predecessor }`
- shortest-path tables as `{ node, distance }` and `{ node, predecessor }`
- all-pairs shortest-path maps as `{ source, distances: [{ target, distance }] }`

Weighted shortest-path wrappers currently accept only the built-in edge weight
channel named `"weight"`. Additional weight channels require a documented
attribute-to-weight policy and contract tests.

## Error Policy

The facade validates common JavaScript boundary mistakes before calling into
WASM where practical.

Runtime failures from the C++/Embind layer are normalized at the TypeScript
facade boundary. Public graph operation failures should throw JavaScript
`Error` instances with the stable prefix:

```text
WASM graph operation failed:
```

Validation errors raised directly by the facade may use JavaScript `TypeError`
when the caller passed a value of the wrong JavaScript type.

## Module Exposure Policy

The C++/WASM binding layer should mirror the C++ library modules where
reasonable. The TypeScript facade should expose a smaller, JavaScript-friendly
surface over those modules.

Core graph, attributes, traversal, shortest paths, spanning tree, and component
APIs are part of the current active Node facade surface. Centrality and flow
bindings are planned active modules but should be added in small slices with
explicit result models and contract tests.

Topological sort, generators, and SAT bindings exist structurally but are not
part of the current active facade milestone unless explicitly promoted.

## Verification Policy

Changes to the public WASM facade should run:

```bash
npm --prefix wasm run build:types
bash wasm/scripts/run_wasm_node_contract_tests.sh
```

When only TypeScript declarations or documentation change, the TypeScript build
is the minimum check. Changes to C++ bindings, WASM runtime behavior, error
mapping, result serialization, or graph semantics should also run the Node API
contract tests.
