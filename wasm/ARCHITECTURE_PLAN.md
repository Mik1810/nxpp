# nxpp-wasm TypeScript/WASM Refactor Prompt

I want to redesign and refactor my existing `nxpp-wasm` package so that it has a clean TypeScript-facing API on top of a C++/WASM runtime core.

## Important context

- The package already exists and is named `nxpp-wasm`.
- I want to keep a single npm package.
- I do **not** want a second package such as `nxpp-js`.
- The runtime source of truth must remain my C++ library compiled to WASM.
- I do **not** want to rewrite the graph algorithms in TypeScript.
- TypeScript should be used as the public API/facade layer on top of the WASM runtime.
- The current WASM binding code is too monolithic and should be modularized.

## Core architectural direction

I want a 2-layer architecture inside the same `nxpp-wasm` package:

1. **C++/WASM binding layer**
   - close to the structure of the C++ library
   - responsible for exposing the runtime classes/functions to JS via Emscripten/Embind
   - should stay honest and explicit about runtime graph classes

2. **TypeScript facade/public API layer**
   - responsible for public ergonomics
   - provides generic interfaces/types
   - wraps the WASM runtime classes
   - adds static typing and, where useful, minimal runtime validation
   - must **not** reimplement graph algorithms

## Very important constraints

- Keep the package name `nxpp-wasm`.
- Do not create a second public npm package.
- Do not propose a custom TypeScript transform, AST rewrite, macro system, or compiler plugin.
- Do not rely on TypeScript generic parameters for runtime dispatch.
- Do not try to make `new DiGraph<number>()` instantiate `DiGraphInt`.
- Do not try to make `new DiGraph<string>()` instantiate `DiGraphStr`.
- I understand that TypeScript generics are erased at compile time, so I do not want fake runtime magic.
- I do **not** want a factory-based public API as the main construction mechanism.
- I want explicit runtime classes and generic TypeScript interfaces/types.

This means:

- runtime classes remain concrete and explicit
- TypeScript uses generic interfaces/types for static typing
- users can write things like:
  - `const g: DiGraph<number> = new DiGraphInt();`
  - `const h: DiGraph<string> = new DiGraphStr();`

This is the intended design direction.

## Runtime class naming policy

Keep explicit runtime/WASM classes such as:

- `GraphInt`
- `GraphStr`
- `DiGraphInt`
- `DiGraphStr`
- `MultiGraphInt`
- `MultiGraphStr`
- `MultiDiGraphInt`
- `MultiDiGraphStr`

## TypeScript API policy

Use generic interfaces/types such as:

- `Graph<T extends number | string>`
- `DiGraph<T extends number | string>`
- `MultiGraph<T extends number | string>`
- `MultiDiGraph<T extends number | string>`

Then make the concrete runtime classes implement the corresponding interfaces:

- `GraphInt implements Graph<number>`
- `GraphStr implements Graph<string>`
- `DiGraphInt implements DiGraph<number>`
- `DiGraphStr implements DiGraph<string>`
- `MultiGraphInt implements MultiGraph<number>`
- `MultiGraphStr implements MultiGraph<string>`
- `MultiDiGraphInt implements MultiDiGraph<number>`
- `MultiDiGraphStr implements MultiDiGraph<string>`

## Type policy

- `*Int` runtime classes must accept only integer-valued JS numbers as node IDs.
- `*Str` runtime classes must accept only JS strings as node IDs.
- Wrong runtime types must throw clear runtime errors.
- Use `string`, not `str`, at the TypeScript level.

## Graph API policy

- Simple graph APIs should be endpoint-based.
- Multigraph APIs may expose edge-id-based methods.
- Do not expose edge-id-centric methods on simple graphs unless strictly necessary.
- Edge IDs may remain an internal implementation detail for simple graphs if the C++ core uses them internally.

## About TypeScript generics

- I know that TypeScript generics are compile-time only.
- Therefore, I do not want impossible runtime dispatch tricks based on `DiGraph<number>` vs `DiGraph<string>`.
- Instead, I want a clean separation:
  - generic interfaces/types for static typing
  - explicit concrete classes for runtime behavior

## About construction

- The primary public construction mechanism should remain explicit constructors of concrete runtime classes, not factories.
- So prefer usage such as:
  - `new DiGraphInt()`
  - `new DiGraphStr()`
  - `new MultiDiGraphInt()`
  - `new MultiDiGraphStr()`
- And allow users to type them against generic interfaces if they want:
  - `const g: DiGraph<number> = new DiGraphInt();`

## Package/module organization goal

I want the internal structure of `nxpp-wasm` to be reorganized so that:

- the C++/WASM binding layer mirrors the module structure of my core C++ library
- the TypeScript facade is cleanly separated from the binding layer
- the final published package contains:
  - compiled WASM
  - generated JS glue/module
  - compiled TypeScript facade
  - `.d.ts` files

My C++ library structure is conceptually like:

- `graph.hpp`
- `multigraph.hpp`
- `attributes.hpp`
- `traversal.hpp`
- `shortest_paths.hpp`
- `topological_sort.hpp`
- `spanning_tree.hpp`
- `components.hpp`
- `centrality.hpp`
- `flow.hpp`
- `generators.hpp`
- `sat.hpp`
- plus umbrella header `nxpp.hpp`

I want the WASM layer to mirror that structure as much as reasonably possible.

## Target internal layout

Please refactor toward a structure conceptually like this:

```text
wasm/
├── include/
│   ├── nxpp_wasm.hpp
│   └── nxpp_wasm/
│       ├── common/
│       │   ├── errors.hpp
│       │   ├── js_convert.hpp
│       │   └── binding_utils.hpp
│       ├── graph.hpp
│       ├── multigraph.hpp
│       ├── attributes.hpp
│       ├── traversal.hpp
│       ├── shortest_paths.hpp
│       ├── topological_sort.hpp
│       ├── spanning_tree.hpp
│       ├── components.hpp
│       ├── centrality.hpp
│       ├── flow.hpp
│       ├── generators.hpp
│       ├── sat.hpp
│       └── register_all.hpp
│
├── src/
│   ├── common/
│   │   ├── errors.cpp
│   │   └── js_convert.cpp
│   ├── graph.cpp
│   ├── multigraph.cpp
│   ├── attributes.cpp
│   ├── traversal.cpp
│   ├── shortest_paths.cpp
│   ├── topological_sort.cpp
│   ├── spanning_tree.cpp
│   ├── components.cpp
│   ├── centrality.cpp
│   ├── flow.cpp
│   ├── generators.cpp
│   ├── sat.cpp
│   └── nxpp_wasm.cpp
│
├── ts/
│   ├── index.ts
│   ├── types.ts
│   ├── load.ts
│   ├── core/
│   │   ├── graph.ts
│   │   ├── multigraph.ts
│   │   └── attributes.ts
│   ├── algorithms/
│   │   ├── traversal.ts
│   │   ├── shortest_paths.ts
│   │   ├── topological_sort.ts
│   │   ├── spanning_tree.ts
│   │   ├── components.ts
│   │   ├── centrality.ts
│   │   ├── flow.ts
│   │   ├── generators.ts
│   │   └── sat.ts
│   └── internal/
│       ├── assert.ts
│       ├── wasm_types.ts
│       └── wrap.ts
│
├── dist/
├── package.json
├── tsconfig.json
└── README.md
```

You may adjust minor details if needed, but stay close to this idea.

## Responsibilities by layer

### 1. C++/WASM binding layer

- exposes runtime classes and functions via Embind
- should be modularized by module, not kept in one monolithic `.cpp`
- should not be shaped by temporary test-oriented code
- should keep `emscripten::val` only at the JS boundary where possible
- should convert immediately to typed C++ values and then use typed internals

### 2. TypeScript facade layer

- defines generic interfaces/types
- re-exports or wraps the concrete runtime classes
- provides a natural TS-facing API
- does not pretend generics exist at runtime
- improves ergonomics without hiding the real runtime classes behind fragile magic

## About algorithms

- Dijkstra and related wrappers were originally introduced for testing.
- I do not want test-oriented binding code to dictate the architecture of the package.
- The graph core should be clean first.
- Algorithm-related bindings and TS wrappers should live in their corresponding modules.
- TypeScript wrappers for algorithms should call into the WASM backend, not reimplement the algorithms.

## Development workflow I want to support

The intended workflow is:

1. Start from one C++ core module, e.g. `graph.hpp`
2. Decide the public API for that module
3. Implement the corresponding WASM binding module
4. Test the raw WASM-level API
5. Build the TypeScript facade for that module
6. Test the final TS/JS-facing API
7. Move to the next module

So the real pipeline is:

**C++ headers/core -> WASM bindings -> generated WASM/JS glue -> TypeScript facade -> published JS/TS package**

This means:

- TypeScript does not come directly from `.hpp`
- the C++ headers are the conceptual source of truth
- the TS layer is designed on top of the WASM layer

## API examples I want to support conceptually

```ts
const g: DiGraph<number> = new DiGraphInt();
g.addNode(1);
g.addEdge(1, 2, 3);

const h: DiGraph<string> = new DiGraphStr();
h.addNode("a");
h.addEdge("a", "b", 2);
```

And similarly for:

- `Graph<number>` / `Graph<string>`
- `MultiGraph<number>` / `MultiGraph<string>`
- `MultiDiGraph<number>` / `MultiDiGraph<string>`

## What I want from you

1. Propose the cleanest final architecture for this design.
2. Explain why this approach is the best tradeoff between WASM reality and TypeScript ergonomics.
3. Show the TypeScript declarations for:
   - `Graph<T>`
   - `DiGraph<T>`
   - `MultiGraph<T>`
   - `MultiDiGraph<T>`
4. Show how the concrete runtime classes should implement those interfaces.
5. Propose the module/file layout for both the WASM binding layer and the TS facade layer.
6. Show the concrete refactor steps from the current monolithic design to this architecture.
7. Keep the design realistic and maintainable.
8. Do not propose impossible generic-runtime dispatch tricks.
9. Do not make factories the primary public pattern.
10. Do not introduce unrelated style-only changes.

## Output format

1. First explain the design choice and why it is the cleanest.
2. Then show the runtime-vs-TypeScript type relationship.
3. Then show the recommended TypeScript interfaces/types.
4. Then show the mapping of concrete runtime classes to those interfaces.
5. Then show the suggested folder/module layout.
6. Then show the concrete refactor plan.
7. Keep comments minimal and human-like.

## Extra preferences

- The public TypeScript API should feel natural for both TS and JS users.
- The runtime API should remain explicit and honest about the actual concrete classes.
- The TS layer should improve ergonomics, not hide reality with brittle abstractions.
- Prefer maintainable modularity over preserving the current monolithic shape.
