# nxpp-wasm 1.0 Architecture

This document is the decision record for the `1.0.0` architecture of
`@mik1810/nxpp-wasm`. The package-root context API and artifact layout are
implemented; the remaining release gates are tracked below.

The package remains on the current `0.x` version while the migration is in
progress. The version advances to `1.0.0` only after the completion gates in
this document are satisfied.

## Decision Summary

- Native C++ remains the only source of graph algorithms and semantic state.
- Embind remains an internal implementation detail of the WASM bridge.
- `createNxpp()` is the asynchronous public initialization boundary and
  returns an explicit runtime context.
- Public graph constructors are bound to that context rather than to a global
  module singleton.
- The normal public API exposes facade objects and plain JavaScript values, not
  raw Embind objects.
- Node.js remains the supported experimental runtime target during the
  migration. Browser loading remains separate and experimental.
- The root facade is the supported package surface. Raw runtime access is
  internal in `1.0.0` unless a later decision assigns it a supported contract.

## Starting `0.6.0` Baseline

The migration started from a root entrypoint that exported generic TypeScript interfaces, eight
concrete facade classes, `createNxpp()`, and `loadNxppRuntime()`.

The eight concrete graph families are:

- `GraphInt` and `GraphStr`
- `DiGraphInt` and `DiGraphStr`
- `MultiGraphInt` and `MultiGraphStr`
- `MultiDiGraphInt` and `MultiDiGraphStr`

The legacy loader imported the Node-specific generated module, cached one global
runtime promise, and initializes that singleton through top-level `await`.
`createNxpp()` can also create a separate raw module, but the directly exported
facade constructors remain bound to the singleton.

The package root resolved to `dist/index.js`. The former `./runtime` export was
a re-export shim over the same facade. Raw runtime values are reachable through
the loading functions rather than through a separately defined and supported
raw API.

The baseline tarball included compiled facade output, TypeScript
sources and configuration, the facade shim, and generated Node runtime files
under `build/`. These facts form the migration baseline, not the `1.0.0`
target layout.

## Target Layers

The target dependency direction is strictly top to bottom:

```text
public TypeScript facade
          |
          v
explicit NxppRuntime context
          |
          v
environment loader (Node or browser)
          |
          v
internal Embind bridge
          |
          v
native nxpp C++ library
```

### Native C++ library

The header-only C++ library owns graph storage, algorithms, validation of graph
semantics, and stateful algorithm lifecycles. WASM work must not introduce a
second algorithm implementation or semantic state machine in TypeScript.

### Internal Embind bridge

The bridge instantiates the supported C++ graph templates, registers methods,
and converts values crossing the JavaScript boundary. It should expose
primitives, arrays, and plain data-transfer objects where practical. Only
long-lived graph handles require explicit lifetime management.

The bridge is versioned with the package and is not a stable public ABI.
Replacing Embind with a C ABI requires separate measured evidence; it is not a
goal of the `1.0.0` refactor.

### Environment loaders

An environment loader creates the raw module and resolves its assets. Node and
browser loaders may use different Emscripten output and asset resolution, but
they must not fork graph bindings or facade behavior.

The Node loader is required for `1.0.0`. A browser loader may remain an
experimental smoke path and does not gain support status without its own CI,
contract coverage, and policy decision.

### Explicit runtime context

`createNxpp()` returns one `NxppRuntime` context bound to one initialized raw
module. Constructors supplied by the context create facade instances backed by
that module:

```ts
const nxpp = await createNxpp();
const graph = new nxpp.DiGraphInt();
```

Multiple contexts may coexist and must not share mutable graph or lifecycle
state. Runtime initialization options belong to the context creation call, not
to module-global mutable configuration.

### Public TypeScript facade

The facade owns JavaScript input validation, public typing, error
normalization, value conversion, and explicit graph disposal. It does not own
graph semantics or reproduce native mutation and algorithm state.

Generic interfaces remain compile-time contracts. Concrete number and string
constructors remain explicit at runtime because TypeScript generic parameters
do not exist at runtime.

## Surface Classification

### Supported public `1.0.0` surface

- the package root entrypoint
- `createNxpp()` and the returned `NxppRuntime`
- runtime-bound concrete graph constructors
- generic graph interfaces and documented result DTOs
- explicit `dispose()` and supported `Symbol.dispose` integration
- the documented Node loader and packaged Node runtime assets

### Experimental surface

- browser loader and browser build artifacts
- browser demos and smoke tests
- capabilities explicitly marked experimental in the API policy

### Internal surface

- raw Embind module and constructors
- binding adapters and registration helpers
- raw runtime TypeScript declarations
- conversion and loader implementation modules
- generated Emscripten glue details

Internal modules must not become supported through accidental deep imports.

### Generated surface

- compiled facade JavaScript and declarations
- normalized or verified raw declarations
- Node Emscripten JavaScript and `.wasm` assets

Generated outputs must be deterministic and checked against their sources in
CI. They are consumer artifacts, not independent sources of truth.

## State, Ownership, and Errors

Native code is the sole owner of graph and staged algorithm state. In
particular, the TypeScript facade must not mirror min-cost-flow availability or
mutation versions.

Each facade graph owns one raw graph handle. `dispose()` is idempotent, and any
operation after disposal throws a clear JavaScript error. Plain return values
and DTOs do not require manual deletion.

TypeScript rejects invalid JavaScript value types before crossing the boundary
where practical. Native graph and algorithm failures cross the bridge and are
normalized at the public facade. Error mapping must not depend on a second
facade-side implementation of the failed semantic check.

## Contract Ownership

The public facade remains deliberately written and reviewed TypeScript. It is
not generated from the C++ API.

The pinned Emscripten declaration output is the machine-verifiable source for
raw constructors, method names, arity, and the eight-class capability matrix.
It is tracked at `wasm/generated/nxpp_node.raw.d.ts`, regenerated with
`NXPP_WASM_EMIT_TSD=1`, and checked for drift in CI.

Emscripten emits `any` for values crossing through `emscripten::val`, so this
declaration is not the source for DTO field types. `wasm_types.ts` remains the
reviewed refinement of the JavaScript-native shapes defined by the bridge
contract. A mechanical check requires its constructor assignments, methods,
and arities to match the generated declaration.

## Migration and Compatibility

The package is still experimental and the `0.6` singleton surface is not a
compatibility constraint for the `1.0` refactor. The approved package-layout
cutover intentionally removes the default singleton export,
`loadNxppRuntime()`, global constructors, and the `./runtime` shim rather than
shipping parallel compatibility layers.

The explicit `NxppRuntime` implementation now lives under `wasm/ts/runtime/`,
with runtime-neutral facade factories under `wasm/ts/core/`. It supports
multiple independent Node contexts and is now the package-root API. The
unpublished legacy singleton source and generated output have been removed.

Runtime initialization now crosses the shared `NxppRuntimeLoader` contract.
The Node adapter owns the packaged Emscripten module import, while the separate
browser adapter owns browser WASM URL resolution and receives the experimental
browser module factory. Both construct the same facade context. The browser
adapter and demo remain outside the supported package export map.

The package-layout cutover removes direct constructors bound to an implicit
global runtime, `loadNxppRuntime()`, the raw return meaning of `createNxpp()`,
the `./runtime` shim, and development files previously shipped in the tarball.

The old and new initialization forms are documented in `wasm/README.md`.
The unpublished legacy source and generated output were removed in #180.

## Implementation Roadmap

- [#166](https://github.com/Mik1810/nxpp/issues/166): define native flow-state
  invalidation semantics.
- [#178](https://github.com/Mik1810/nxpp/issues/178): isolate the current
  facade behind the internal legacy compatibility boundary.
- [#171](https://github.com/Mik1810/nxpp/issues/171): introduce the explicit
  runtime context internally before the package-root cutover.
- [#172](https://github.com/Mik1810/nxpp/issues/172): remove semantic state
  duplication from the facade.
- [#173](https://github.com/Mik1810/nxpp/issues/173): standardize bridge DTO and
  ownership boundaries.
- [#174](https://github.com/Mik1810/nxpp/issues/174): establish one verifiable
  raw runtime contract.
- [#175](https://github.com/Mik1810/nxpp/issues/175): separate environment
  loaders from the runtime-neutral facade.
- [#176](https://github.com/Mik1810/nxpp/issues/176): finalize package layout
  and supported exports.
- [#168](https://github.com/Mik1810/nxpp/issues/168): retire the stale local
  checklist after the issue-backed roadmap is authoritative.
- [#179](https://github.com/Mik1810/nxpp/issues/179): verify the Node versions
  declared by the npm package before the `1.0.0` release.
- [#180](https://github.com/Mik1810/nxpp/issues/180): remove the unused
  singleton implementation and its generated output.

The ordered umbrella roadmap is
[#177](https://github.com/Mik1810/nxpp/issues/177).

## `1.0.0` Completion Gates

The package version changes to `1.0.0` only when:

- every required roadmap issue is complete or explicitly rejected with a
  recorded rationale
- all eight graph families pass public class-parity and behavior contracts
- native C++ remains the algorithmic and semantic source of truth
- multiple runtime contexts are isolated and correctly dispose graph handles
- Node contract, TypeScript compile, and npm-pack consumer checks pass
- tested Node versions match the range declared in `wasm/package.json`
- raw declarations and generated facade artifacts are reproducible in CI
- the final tarball contains only intentional consumer files and exports
- migration notes cover every intentional breaking change from `0.6.0`
- Node and browser support claims match their actual verification coverage

Publication and registry credentials are outside the architectural refactor;
release preparation remains a separate, explicitly authorized task.
