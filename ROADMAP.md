# nxpp Roadmap

This roadmap tracks the main stabilization and development lanes for nxpp. It is
intended to describe direction, not to replace GitHub issues or release notes.
Completed work is recorded in `CHANGELOG.md`; release-facing summaries live in
`RELEASE_NOTES.md`.

## Stabilization Targets

### Stable C++ Core

The C++ public API remains the primary stable surface.

- Preserve the NetworkX-inspired graph API while keeping Boost Graph Library
  interoperability explicit.
- Keep `include/nxpp.hpp` as the canonical umbrella include.
- Maintain focused documentation for graph configuration, complexity, external
  consumption, and API caveats.
- Treat breaking public C++ API changes as major-version work under the
  repository versioning policy.

### Experimental WASM Lane

The WebAssembly and TypeScript layer remains experimental until the facade is
documented, covered by contract tests, and separated from incremental binding
experiments.

- Define a stable JavaScript and TypeScript API policy.
- Separate Embind bindings from the TypeScript facade.
- Document error mapping, attribute serialization, and multigraph edge-ID
  behavior.
- Add contract tests before treating exported APIs as stable.

### Packaging and Support Matrix

Packaging remains stable only where documented in the support matrix.

- Keep first-class CMake and header-only consumption paths aligned with
  `docs/EXTERNAL_USAGE.md`.
- Define and test the official Boost support matrix.
- Keep secondary channels such as Conan, vcpkg overlays, AUR packaging, and
  generated reference docs scoped by `docs/STABILITY.md`.

## Milestones

### Core C++

- Audit SemVer compliance across released versions.
- Define a release checklist for API compatibility review.
- Add API compatibility snapshot coverage for public aliases, method
  signatures, CMake targets, and WASM exports.
- Expand targeted stress coverage for staged flow and cache invalidation paths.
- Document thread-safety guarantees for read-only access, mutation, staged flow
  operations, and internal cache behavior.

### WASM and TypeScript

- Publish `wasm/API_POLICY.md` and `wasm/ARCHITECTURE.md`.
- Rebuild the facade around predictable JavaScript semantics for graph creation,
  mutation, queries, attributes, and multigraph edge IDs.
- Expose traversal algorithms, shortest paths, components, SCC, centrality, and
  flow algorithms in staged modules after the core graph facade is stable.
- Keep browser support as an investigation track until the Node-facing package
  has clearer API boundaries and contract coverage.

### CI, Compatibility, and Performance

- Define the official minimum supported Boost version and test it in CI.
- Keep latest stable Boost coverage in CI where practical.
- Track compile-time growth for the header-only API.
- Maintain focused performance checks without committing generated benchmark
  output.
- Add native-vs-WASM benchmark coverage after the WASM facade settles.

### Documentation

- Keep `docs/STABILITY.md` as the source of truth for stable and experimental
  surfaces.
- Document weighted-edge attribute policy and unsupported custom weight-key
  behavior.
- Keep roadmap items linked to focused GitHub issues instead of embedding large
  issue bodies in repository docs.

## Experimental Areas

These areas may change without the same compatibility guarantees as the stable
C++ public API:

- WebAssembly bindings and the TypeScript facade.
- Browser support and browser demos.
- Native-vs-WASM benchmark harnesses and results.
- Secondary packaging channels that are not documented as first-class support
  paths.
- Long-term API ideas such as lazy traversal views.

## Release Discipline

- `CHANGELOG.md` records concise versioned technical history.
- `RELEASE_NOTES.md` records release-facing notes when preparing a release.
- Public behavior changes should identify whether they affect stable C++ APIs,
  experimental WASM APIs, packaging channels, or documentation only.
- Breaking changes to stable C++ APIs should be planned as major-version work
  with migration notes.
