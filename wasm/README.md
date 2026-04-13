# nxpp (npm wasm package)

Experimental Node.js wasm package for `nxpp`.

## Usage

```js
import nxpp from "@mik1810/nxpp-wasm";

const g = new nxpp.DiGraph();
g.addEdge(1, 2, 1);
g.addEdge(2, 3, 2);
g.addEdge(1, 3, 5);

console.log(g.neighbors(1));
console.log(g.getEdgeWeight(1, 3));

const r = g.dijkstraShortestPaths(1);
console.log(r.distanceTo(3));
```

## Local publish dry-run

```bash
cd wasm
npm pack
```

## Current experimental surface

Today the Node-facing wasm lane still exposes only an experimental first graph
slice around `DiGraph`.

Current `DiGraph` methods:

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

This surface is useful for iteration and contract testing, but it is not yet
the long-term public API shape.

Current `DiGraph` node-ID behavior:

- the graph accepts either integer-valued JS numbers or strings
- the internal backend is selected lazily from the first observed node ID
- once selected, mixing numeric and string node IDs in the same graph is an
  error
- the public JavaScript surface keeps that distinction internal instead of
  forcing all IDs through one normalized representation

## Public API direction

The public JavaScript-facing API is now intended to move toward a smaller
NetworkX-like family of graph types:

- `Graph`
- `DiGraph`
- `MultiGraph`
- `MultiDiGraph`

Important design choices for that direction:

- JavaScript consumers should not need to care about the full C++ alias matrix
  from `graph.hpp`
- the C++ concrete graph aliases remain internal implementation details of the
  wasm binding layer
- node IDs should support both `number` and `string`
- the graph backend should be selected lazily from the first observed node ID
  type instead of normalizing everything to strings
- mixing numeric and string node IDs inside the same graph should be treated as
  an error
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
- avoid treating the current `DiGraph`-only Embind layer as if it were already
  the final JavaScript API

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
2. reshape the public API toward `Graph` / `DiGraph` / `MultiGraph` /
   `MultiDiGraph`
3. preserve distinct internal backends for numeric and string node IDs
4. only then move outward to later semantic headers
