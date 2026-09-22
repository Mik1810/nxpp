# nxpp WASM package

`@mik1810/nxpp-wasm` exposes selected native nxpp graph operations through a
TypeScript facade over WebAssembly. The package is experimental. This README
describes the current repository revision; a published npm version may differ
until the next package release.

## Runtime support

- Node.js 22, 24, and 26 are tested with an installed npm tarball in CI.
- Browsers have a separate smoke-tested demo, not a supported package API.
- Consumers use prebuilt JavaScript and WASM assets; Boost and Emscripten are
  needed only to build the package.

The package exports only its root entrypoint. `runtime/`, `dist/`, the raw
Embind module, and the browser adapter are internal, not importable subpaths.

Install a published version with `npm install @mik1810/nxpp-wasm`. To validate
the current repository revision before its next npm release, build and test a
local tarball as described in the
[build guide](https://github.com/Mik1810/nxpp/blob/main/wasm/WASM.md).

## Start with a graph

Initialize a context before constructing graphs. Each `createNxpp()` call
creates an independent runtime context.

```js
import { createNxpp } from "@mik1810/nxpp-wasm";

const nxpp = await createNxpp();
const graph = new nxpp.DiGraphInt();

try {
  graph.addEdge(1, 2, 1);
  graph.addEdge(2, 3, 2);
  graph.addEdge(1, 3, 5);

  console.log(graph.dijkstraPath(1, 3)); // [1, 2, 3]
  console.log(graph.dijkstraPathLength(1, 3)); // 3
} finally {
  graph.dispose();
}
```

The eight concrete constructors are `GraphInt`, `GraphStr`, `DiGraphInt`,
`DiGraphStr`, `MultiGraphInt`, `MultiGraphStr`, `MultiDiGraphInt`, and
`MultiDiGraphStr`. `*Int` node IDs are integer-valued JavaScript numbers;
`*Str` node IDs are strings. Constructors are properties of the returned
context, not global package exports.

## Other common operations

Attributes, traversal, and components use ordinary JavaScript values:

```js
import { createNxpp } from "@mik1810/nxpp-wasm";

const nxpp = await createNxpp();
const graph = new nxpp.GraphStr();
try {
  graph.addEdge("a", "b", 1);
  graph.addEdge("b", "c", 1);
  graph.setNodeAttr("a", "label", "start");

  console.log(graph.getNodeAttr("a", "label")); // "start"
  console.log(graph.bfsTree("a")); // { nodes: [...], edges: [...] }
  console.log(graph.connectedComponents()); // [["a", "b", "c"]]
} finally {
  graph.dispose();
}
```

For parallel edges, use an edge ID when changing one particular edge:

```js
import { createNxpp } from "@mik1810/nxpp-wasm";

const nxpp = await createNxpp();
const graph = new nxpp.MultiDiGraphInt();
try {
  graph.addEdge(1, 2, 4);
  graph.addEdge(1, 2, 7);

  const [edgeId] = graph.edgeIdsBetween(1, 2);
  graph.setEdgeAttrById(edgeId, "capacity", 10);
  console.log(graph.getEdgeAttrById(edgeId, "capacity")); // 10
  console.log(graph.getEdgeEndpoints(edgeId).source()); // 1
} finally {
  graph.dispose();
}
```

Flow results are plain data; only graph handles need disposal:

```js
import { createNxpp } from "@mik1810/nxpp-wasm";

const nxpp = await createNxpp();
const graph = new nxpp.DiGraphInt();
try {
  graph.addEdge(0, 1, 1);
  graph.setEdgeAttr(0, 1, "capacity", 3);
  const result = graph.maximumFlow(0, 1);
  console.log(result.value); // 3
  console.log(result.edgeFlows); // [{ source: 0, target: 1, flow: 3 }]
} finally {
  graph.dispose();
}
```

Attribute values are limited to strings, finite numbers, and booleans.
`tryGet...` methods return `null` for missing or unsupported values. Weighted
shortest-path wrappers currently use the built-in `"weight"` channel.

## TypeScript and lifetime

Generic interfaces are for static typing; the runtime constructors stay
explicit:

```ts
import { createNxpp, type DiGraph } from "@mik1810/nxpp-wasm";

const nxpp = await createNxpp();
const graph: DiGraph<number> = new nxpp.DiGraphInt();
try {
  graph.addEdge(1, 2, 3);
  console.log(graph.neighbors(1));
} finally {
  graph.dispose();
}
```

`dispose()` is idempotent. Operations after disposal throw a JavaScript error.
In runtimes with `Symbol.dispose`, facade instances expose the same disposal
path. Raw C++/WASM failures are normalized with a
`WASM graph operation failed: ...` prefix.

## API coverage

| Area | Current package surface |
|---|---|
| Graphs and attributes | Eight typed graph families, node/edge attributes, multigraph edge IDs |
| Traversal and paths | BFS/DFS, single-pair and single-source shortest paths, Floyd-Warshall |
| Other algorithms | Spanning trees, components, centrality, and flow |
| Not exposed | Topological sort, generators, and SAT |

For exact method signatures and result types, consult the public declarations
in the installed package or the [TypeScript source](https://github.com/Mik1810/nxpp/blob/main/wasm/ts/types.ts).
The [API policy](https://github.com/Mik1810/nxpp/blob/main/wasm/API_POLICY.md)
defines validation and support boundaries; the
[architecture](https://github.com/Mik1810/nxpp/blob/main/wasm/ARCHITECTURE.md)
explains layer ownership and the 1.0 criteria.

## Migration from the former 0.6 singleton

The default singleton export, global graph constructors,
`loadNxppRuntime()`, and the `@mik1810/nxpp-wasm/runtime` shim have been
removed from this repository revision. Replace `new nxpp.GraphInt()` on a
default import with asynchronous context creation:

```js
import { createNxpp } from "@mik1810/nxpp-wasm";

const nxpp = await createNxpp();
const graph = new nxpp.GraphInt();
// Use the graph, then call graph.dispose().
```

The raw Embind module has no public replacement. See the
[WASM build and release guide](https://github.com/Mik1810/nxpp/blob/main/wasm/WASM.md)
for maintainer commands and the
[issue roadmap](https://github.com/Mik1810/nxpp/issues/177) for remaining
release-readiness decisions.
