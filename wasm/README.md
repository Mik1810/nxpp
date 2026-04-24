# nxpp (npm wasm package)

Experimental Node.js wasm package for `nxpp`.

## Usage

```js
import nxpp from "@mik1810/nxpp-wasm";

const g = new nxpp.DiGraphInt();
g.addEdge(1, 2, 1);
g.addEdge(2, 3, 2);
g.addEdge(1, 3, 5);

console.log(g.neighbors(1));
console.log(g.getEdgeWeight(1, 3));
```

## TypeScript facade

The package now ships a TypeScript-facing facade on top of the wasm runtime.

```ts
import nxpp, { type DiGraph } from "@mik1810/nxpp-wasm";

const g: DiGraph<number> = new nxpp.DiGraphInt();
g.addNode(1);
g.addEdge(1, 2, 3);

const h: DiGraph<string> = new nxpp.DiGraphStr();
h.addNode("a");
h.addEdge("a", "b", 2);
```

Facade instances own Embind-backed WASM objects. Release them explicitly when a
graph is no longer needed:

```ts
const g = new nxpp.GraphInt();
try {
  g.addEdge(1, 2, 1);
  console.log(g.neighbors(1));
} finally {
  g.dispose();
}
```

`dispose()` is safe to call more than once. Any graph operation after disposal
throws a clear error. In runtimes that expose `Symbol.dispose`, facade
instances also attach that symbol to the same disposal path.

Runtime failures from the C++/WASM layer are normalized at the TypeScript
facade boundary. Common invalid operations throw JavaScript `Error` instances
with a stable `WASM graph operation failed: ...` prefix instead of exposing raw
Embind exception objects.

Design rules for the facade:

- runtime classes stay explicit (`*Int` and `*Str`)
- TypeScript generics are static only (no generic runtime dispatch tricks)
- simple graph APIs stay endpoint-based
- multigraph APIs expose edge-id methods where needed

## Local publish dry-run

```bash
cd wasm
npm pack
```

## Publish order (npm first)

Use the package scripts to publish in a deterministic order:

```bash
cd wasm
npm run publish:all
```

This always performs:

1. `npm run publish:npm` against `https://registry.npmjs.org/`
2. `npm run publish:github` against `https://npm.pkg.github.com/`

The split avoids lifecycle chaining issues (for example `postpublish` inheriting
unexpected registry context) and keeps the first publish on npmjs. Credentials
are expected to live in the user's npm configuration, not in repository files.

## Current experimental surface

Today the Node-facing wasm lane exposes an experimental first graph
slice around explicit typed graph classes:

- `GraphInt`
- `GraphStr`
- `DiGraphInt`
- `DiGraphStr`
- `MultiGraphInt`
- `MultiGraphStr`
- `MultiDiGraphInt`
- `MultiDiGraphStr`

Current simple graph methods (`Graph*`, `DiGraph*`) are:

Core endpoint-oriented methods:

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
- `bfsTree(start)`
- `bfsSuccessors(start)`
- `dfsEdges(start)`
- `dfsTree(start)`
- `dfsPredecessors(start)`
- `dfsSuccessors(start)`
- `shortestPath(source, target)`
- `shortestPathWeighted(source, target, weightKey = "weight")`
- `shortestPathLength(source, target)`
- `shortestPathLengthWeighted(source, target, weightKey = "weight")`
- `dijkstraPath(source, target)`
- `dijkstraPathWeighted(source, target, weightKey = "weight")`
- `dijkstraShortestPaths(source)`
- `dijkstraPathLengths(source)`
- `dijkstraPathLength(source, target)`
- `dijkstraPathLengthWeighted(source, target, weightKey = "weight")`
- `bellmanFordPath(source, target)`
- `bellmanFordPathWeighted(source, target, weightKey = "weight")`
- `bellmanFordShortestPaths(source)`
- `bellmanFordPathLength(source, target)`
- `bellmanFordPathLengthWeighted(source, target, weightKey = "weight")`
- `dagShortestPaths(source)`
- `floydWarshallAllPairsShortestPaths()`
- `floydWarshallAllPairsShortestPathsMap()`
- `kruskalMinimumSpanningTree()`
- `primMinimumSpanningTree(root)`
- `clear()`

Multigraph classes (`MultiGraph*`, `MultiDiGraph*`) additionally expose
edge-ID-specific methods:

- `hasEdgeId(edgeId)`
- `edgeIds()`
- `edgeIdsBetween(source, target)`
- `getEdgeEndpoints(edgeId)`
- `getEdgeWeightById(edgeId)`
- `setEdgeWeightById(edgeId, weight)`
- `hasEdgeAttrById(edgeId, key)`
- `getEdgeAttrById(edgeId, key)`
- `tryGetEdgeAttrById(edgeId, key)`
- `setEdgeAttrById(edgeId, key, value)`
- `getEdgeNumericAttrById(edgeId, key)`
- `removeEdgeById(edgeId)`

Current attribute-value contract is intentionally narrow and explicit:

- supported attribute values are `string`, finite `number`, and `boolean`
- `tryGet...` methods return `null` when the attribute is missing or the stored
  value does not match the wasm contract
- `null` is not accepted as an attribute value in writes
- endpoint-based edge-attribute access on multigraphs remains convenience-only;
  use `*ById` methods when one concrete parallel edge matters

Current runtime type behavior is explicit by class:

- `*Int` classes accept only integer-valued JS numbers as node IDs
- `*Str` classes accept only JS strings as node IDs
- wrong node-ID types throw explicit `std::runtime_error`

Current shortest-path result behavior is explicit and JS-oriented:

- single-pair methods return node arrays or numeric distances directly
- `dijkstraShortestPaths(source)`, `bellmanFordShortestPaths(source)`, and
  `dagShortestPaths(source)` return a result object with:
  - `distance`
  - `predecessor`
  - `hasPathTo(target)`
  - `pathTo(target)`
- `floydWarshallAllPairsShortestPaths()` returns a dense `number[][]` matrix
- `floydWarshallAllPairsShortestPathsMap()` returns serializable source/target
  DTO entries instead of a JS `Map`
- weighted wrappers currently accept only the built-in `"weight"` channel
- minimum-spanning-tree wrappers return serializable `{ source, target }`
  edge entries
- facade graph instances expose explicit `dispose()` lifetime management
- facade graph operations normalize common runtime failures into predictable
  JavaScript errors

This surface is useful for iteration and contract testing, but it is not yet
the long-term public API shape.

The binding implementation is now modular and mirrors the core library modules
under `wasm/include/nxpp_wasm/` and `wasm/src/`, with a single final
`EMSCRIPTEN_BINDINGS(...)` entrypoint in `wasm/src/nxpp_wasm.cpp`.

The TypeScript facade source is organized under `wasm/ts/`, and the published
entrypoint and declarations are under `wasm/dist/`.

## Public API direction

The public JavaScript-facing API is now intended to stay aligned with explicit
typed graph classes from the core `graph.hpp` model:

- `GraphInt` / `GraphStr`
- `DiGraphInt` / `DiGraphStr`
- `MultiGraphInt` / `MultiGraphStr`
- `MultiDiGraphInt` / `MultiDiGraphStr`

Important design choices for that direction:

- JavaScript consumers should not need to care about the full C++ alias matrix
  from `graph.hpp`
- node IDs should support both `number` and `string`
- each exported wasm class should bind one concrete backend directly (no lazy
  backend selection from first use)
- mixing numeric and string node IDs is prevented by class design (`*Int` vs
  `*Str`) and explicit runtime validation at JS boundary conversion points
- the built-in `weight` channel remains part of graph behavior, but not part of
  the public type names, which stays closer to the NetworkX model

## Direction for the next wasm steps

The current wasm lane is intentionally still below full `nxpp.hpp` parity.

The graph-core-plus-attributes-plus-traversal base is now in place for the
explicit typed graph family. The next implementation goal is to continue by
semantic module, starting with `shortest_paths.hpp`.

Important design direction:

- parity does not require literal C++ operator syntax in JavaScript
- the target is behavioral parity through explicit JS-facing methods, not a
  forced `Proxy`-heavy imitation of every `operator[]` chain
- the wasm layer should therefore stabilize:
  - correct graph primitives first
  - then the NetworkX-like public graph family
  - then later semantic headers on top of that base

So the current wasm direction is:

1. keep the explicit typed family (`Graph*`, `DiGraph*`, `MultiGraph*`,
  `MultiDiGraph*`) aligned with the base C++ modules
2. preserve distinct concrete backends by class (no runtime switching)
3. close semantic headers block by block, with `attributes.hpp`,
  `traversal.hpp`, `shortest_paths.hpp`, and `spanning_tree.hpp` now covered
4. continue with the next semantic headers after `spanning_tree.hpp`
