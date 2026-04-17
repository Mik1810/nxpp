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

1. `npm run publish:npm` using `.npmrc.publish-npm`
2. `npm run publish:github` using `.npmrc.publish-github`

The split avoids lifecycle chaining issues (for example `postpublish` inheriting
unexpected registry context) and keeps the first publish on npmjs.

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
- `clear()`

Multigraph classes (`MultiGraph*`, `MultiDiGraph*`) additionally expose
edge-ID-specific methods:

- `hasEdgeId(edgeId)`
- `edgeIds()`
- `edgeIdsBetween(source, target)`
- `getEdgeEndpoints(edgeId)`
- `getEdgeWeightById(edgeId)`
- `setEdgeWeightById(edgeId, weight)`
- `removeEdgeById(edgeId)`

Current runtime type behavior is explicit by class:

- `*Int` classes accept only integer-valued JS numbers as node IDs
- `*Str` classes accept only JS strings as node IDs
- wrong node-ID types throw explicit `std::runtime_error`

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

The next implementation goal is still to finish the `graph.hpp` surface before
moving on to later semantic headers such as `attributes.hpp` or
`shortest_paths.hpp`.

That means:

- keep extending the core graph contract in focused slices
- close the `graph.hpp` gap before starting broader library-area rollout
- avoid treating the current Embind graph layer as if it were already the
  final JavaScript API

Important design direction:

- parity does not require literal C++ operator syntax in JavaScript
- the target is behavioral parity through explicit JS-facing methods, not a
  forced `Proxy`-heavy imitation of every `operator[]` chain
- the wasm layer should therefore stabilize:
  - correct graph primitives first
  - then the NetworkX-like public graph family
  - then later semantic headers on top of that base

So the current wasm direction is:

1. complete `graph.hpp` behavior in staged slices
2. keep extending the explicit typed family (`Graph*`, `DiGraph*`,
  `MultiGraph*`, `MultiDiGraph*`) by module
3. preserve distinct concrete backends by class (no runtime switching)
4. only then move outward to later semantic headers
