# nxpp WASM architecture

This document describes the current implementation and the decisions for the
WASM package's `1.0.0` architecture. The completion gates below have been
audited for the `1.0.0` release candidate; publication remains separate. The
[package README](README.md) covers usage, the [build guide](WASM.md) covers
verification and release procedure, and the [API policy](API_POLICY.md)
defines the public boundary. The [issue-backed roadmap](https://github.com/Mik1810/nxpp/issues/177)
tracks implementation work.

## Contract and dependency direction

The native C++ library is the only source of graph algorithms and semantic
state. The package exports an asynchronous `createNxpp()` initializer from
its root entrypoint. It returns an `NxppRuntime` context with eight concrete
graph constructors. There are no global graph constructors or supported deep
imports into the runtime and facade implementation.

```text
public TypeScript facade
          |
          v
explicit NxppRuntime context
          |
          v
environment loader (Node; browser experimental)
          |
          v
internal Embind bridge
          |
          v
native nxpp C++ library
```

The arrows show dependencies, not separate npm packages. The package remains
one distribution of prebuilt JavaScript, declarations, and WASM assets.

## Layer ownership

### Native C++ and Embind

The header-only C++ library owns graph storage, algorithms, semantic
validation, and staged algorithm lifecycles. `wasm/include/nxpp_wasm/` and
`wasm/src/` instantiate selected graph templates, register their methods,
and convert boundary values. They must not duplicate algorithmic logic.

`wasm/src/nxpp_wasm.cpp` supplies the single `EMSCRIPTEN_BINDINGS` entrypoint
through `nxpp_wasm::register_all_bindings()`. Registration is split into graph
and multigraph families, with shared capability modules for attributes,
traversal, shortest paths, spanning trees, components, centrality, and flow.
Topological sort, generators, and SAT are not currently exported.

Embind is internal and versioned with the package, not a stable public ABI.
Moving to a C ABI would require separate measured evidence; it is not a
condition of the 1.0 refactor. Emscripten directives stay out of the native
public `include/` headers.

### Environment loaders and runtime context

`wasm/ts/runtime/node.ts` creates a raw Node module per call. The runtime-
neutral factory in `wasm/ts/runtime/context.ts` validates that module and
returns a frozen `NxppRuntime` with constructors bound to it. Multiple
contexts can coexist without shared graph or staged-algorithm state.

The browser adapter receives a separate browser module factory and asset URL.
It uses the same facade construction but remains outside the supported package
export map. A headless demo smoke test proves only a narrow loading path, not
browser API parity or general bundler support.

### Public TypeScript facade

`wasm/ts/` owns input-type checks, reviewed public declarations, error
normalization, result adaptation, and graph-handle disposal. It does not
reimplement graph algorithms or mirror native semantic state such as staged
min-cost-flow availability or mutation versions.

The package root exports `createNxpp()` and public TypeScript types. The
returned context provides `GraphInt`, `GraphStr`, `DiGraphInt`, `DiGraphStr`,
`MultiGraphInt`, `MultiGraphStr`, `MultiDiGraphInt`, and `MultiDiGraphStr`.
Generic `Graph<T>`, `DiGraph<T>`, `MultiGraph<T>`, and `MultiDiGraph<T>`
interfaces are compile-time contracts; generic parameters cannot select a
runtime class.

## Layout and package surface

```text
wasm/
  include/nxpp_wasm/       Embind declarations and conversion helpers
  src/                     binding registrations and bridge implementation
  ts/core/                 runtime-neutral facade graph classes
  ts/runtime/              context, Node loader, experimental browser loader
  ts/internal/             validation, errors, and raw type refinements
  ts/index.ts, ts/types.ts  package root and public types
  dist/                    generated facade JavaScript and declarations
  generated/               pinned raw Embind declaration
  runtime/node.mjs         packaged Emscripten Node glue
  runtime/node.wasm        packaged WASM binary
  test/                    contract and consumer fixtures
```

The published tarball contains only the files allowlisted in
`wasm/package.json`. `dist/index.js` is the supported import target. The raw
module, generated declarations, internal facade files, and browser artifacts
are not supported subpaths. The former singleton source and generated legacy
output have been removed.

## Values, lifetime, and errors

The bridge returns JavaScript primitives, arrays, and plain DTOs for ordinary
results, including traversal edges, shortest-path distances, component groups,
centrality scores, and flow assignments. Only graph and subgraph handles
require explicit Embind lifetime management. The facade owns each handle;
`dispose()` is idempotent and operations after disposal fail clearly.

The raw multigraph edge-endpoint lookup crosses the bridge as a plain
`{ source, target }` DTO. The facade retains the documented `source()` and
`target()` methods through an unowned adapter; it is not a second native
object requiring deletion. Use edge-ID methods when one specific parallel
edge matters.

The facade validates invalid JavaScript value types before crossing into WASM
where practical. Native graph failures cross the bridge and are normalized to
JavaScript `Error` objects with the `WASM graph operation failed: ...` prefix.
Optimized builds preserve `std::exception::what()` and release caught native
exceptions. Error mapping must not reproduce the failed semantic check in
TypeScript. See [BRIDGE_CONTRACT.md](BRIDGE_CONTRACT.md) for the detailed value
and ownership rules.

## Contract ownership and verification

The public facade is deliberately reviewed TypeScript; it is not generated
from the C++ API. Emscripten emits the pinned declaration at
`wasm/generated/nxpp_node.raw.d.ts`. That declaration is the mechanical source
for raw constructor, method, arity, and class-capability checks. Since values
crossing `emscripten::val` may be declared as `any`, the reviewed
`wasm_types.ts` refines DTO fields against the bridge contract.

CI checks TypeScript output against its source, the raw declaration against
the bindings, and packaged Node runtime reproducibility. It runs the Node API
contract and installs one tarball on Node 22, 24, and 26 for a consumer smoke
test and public declaration check. The browser demo receives a separate,
narrow smoke check. These checks do not promote browser support.

## Migration decisions

The starting `0.6.0` root exported a default singleton, global graph
constructors, `loadNxppRuntime()`, and a `./runtime` shim. The singleton
loader cached one raw module promise and initialized it with top-level
`await`. The former tarball also contained development sources and generated
assets under `build/`.

The migration deliberately removed those exports and the internal legacy
implementation. Callers now initialize a context with `createNxpp()` and
construct graphs from that context. The Node loader owns packaged asset
resolution; the browser loader is an experimental separate path. This is an
intentional breaking change within the experimental line, not a compatibility
layer to be maintained in parallel. The [package README](README.md) gives a
short migration example.

## 1.0.0 completion gates

The `1.0.0` candidate follows a final audit of these conditions:

- required roadmap issues are closed or explicitly rejected with rationale;
- all eight graph families pass public parity and behavior contracts;
- native C++ remains the sole owner of algorithms and semantic state;
- independent contexts and graph-handle disposal behave correctly;
- Node contract, TypeScript build, and packed external consumer tests pass;
- tested Node majors match the package engine range;
- raw declarations, facade output, and packaged runtime assets are reproducible;
- the tarball contains only intentional files and supported exports;
- migration notes cover intentional breaking changes from the old 0.6 API;
- Node and browser support claims match their actual verification coverage.

The roadmap and this audit are separate from release preparation. Bumping the
version, tagging, staging npm publication, and registry approval require
their own explicit decisions. No C ABI rewrite or broad browser support
promise is implied by these gates.
