# Changelog

This project starts explicit release versioning with `0.4.1`. Older entries below remain as date-based pre-versioning history.

## [1.0.38] - 2026-04-24

- Added explicit lifetime management to the wasm TypeScript facade: all public graph facade classes now expose idempotent `dispose()` methods, attach `Symbol.dispose` when the JavaScript runtime provides it, and throw a clear error when graph operations are attempted after disposal.
- Extended the wasm contract suite and TypeScript compile checks for facade disposal behavior, and advanced the experimental wasm package candidate from `0.4.0` to `0.5.0`.
- Added the wasm `spanning_tree.hpp` block for the explicit typed graph family: `kruskalMinimumSpanningTree()` and `primMinimumSpanningTree(root)` now return serializable `{ source, target }` edge DTO arrays through the Node/TS facade.
- Extended the wasm TypeScript facade, compile checks, and Node contract suite for the spanning-tree surface, and advanced the experimental wasm package candidate from `0.3.0` to `0.4.0`.
- Updated the experimental wasm JS and TypeScript examples to use a richer weighted directed graph and demonstrate `dijkstraShortestPaths(1)` with path reconstruction and the single-source distance table.
- Kept the TypeScript example aligned with the public facade types by annotating the graph and Dijkstra result with exported package interfaces.
- Moved the wasm/TS continuation plan from the repository root to `wasm/TODO.md`, keeping the near-term planning next to the experimental package work.
- Simplified wasm publish routing by removing the token-free per-target `.npmrc.publish-*` files and passing the npmjs / GitHub Packages registries directly in the publish scripts.
- Updated the wasm troubleshooting docs to use a generic `emsdk_env.sh` activation path, with the current WSL-local path documented as an example.

## [1.0.37] - 2026-04-21

- Closed the wasm `shortest_paths.hpp` block for the explicit typed graph family: the Node/TS lane now covers single-pair shortest paths, single-source wrappers (`dijkstra*`, `bellmanFord*`, `dagShortestPaths(...)`), and the all-pairs Floyd-Warshall surface.
- Added JS/TS-facing all-pairs result shapes to the wasm facade: `floydWarshallAllPairsShortestPaths()` returns a dense `number[][]` matrix in stable node order, and `floydWarshallAllPairsShortestPathsMap()` returns serializable `{ source, distances: [{ target, distance }] }` DTO entries rather than a JS `Map`.
- Extended the modular wasm Node contract suite and TS compile checks to cover the full shortest-path surface, including weighted-path behavior, single-source path reconstruction, all-pairs matrix/map views, and multigraph parallel-edge Floyd-Warshall handling.
- Updated `wasm/README.md`, `wasm/WASM.md`, and `SESSION.md` to reflect that `shortest_paths.hpp` is now covered in full for the current wasm lane.
- Advanced the experimental wasm package metadata from `0.2.0` to `0.3.0` in `wasm/package.json` (and synced `wasm/package-lock.json`) to mark the completed shortest-path surface as the next npm publish candidate.

## [1.0.36] - 2026-04-17

- Published `@mik1810/nxpp-wasm@0.2.0` to both npmjs and GitHub Packages after the deterministic publish-flow hardening, and verified registry visibility on both channels.
- Added a repository continuation plan in `TODO.md` for the next wasm/ts development slices, explicitly excluding items 6 (`topological_sort`), 11 (`generators`), and 12 (`sat`) from the active near-term scope.

## [1.0.35] - 2026-04-17

- Advanced the experimental wasm npm package metadata from `0.1.3` to `0.2.0` in `wasm/package.json` (and synced `wasm/package-lock.json`) to mark the new typed-runtime-plus-TypeScript-facade lane as the next publish candidate.
- Finalized deterministic publish routing for the wasm package: npmjs publish runs first, GitHub Packages publish runs second, and both targets are now driven by token-free repository config files (`wasm/.npmrc.publish-npm`, `wasm/.npmrc.publish-github`) to avoid host-level scope registry overrides.

## [1.0.34] - 2026-04-17

- Hardened the experimental wasm publish flow so npmjs is always published first: `wasm/package.json` no longer uses `postpublish`, `publish:all` now runs explicit sequential steps (`publish:npm && publish:github`), and each step now uses dedicated user config files to avoid scope-registry bleed-through from host npm configuration.
- Added `wasm/.npmrc.publish-npm` and `wasm/.npmrc.publish-github` as token-free publish-routing configs, and updated `wasm/README.md` with the deterministic publish command/order.

## [1.0.33] - 2026-04-17

- Added a TypeScript-facing facade layer for the experimental wasm package under `wasm/dist/` and `wasm/ts/`, with generic public interfaces (`Graph<T>`, `DiGraph<T>`, `MultiGraph<T>`, `MultiDiGraph<T>`) mapped to explicit runtime classes (`GraphInt`/`GraphStr`, `DiGraphInt`/`DiGraphStr`, `MultiGraphInt`/`MultiGraphStr`, `MultiDiGraphInt`/`MultiDiGraphStr`).
- Switched `wasm/package.json` public entrypoints to the facade artifacts (`dist/index.js` + `dist/index.d.ts`), added TS metadata/build wiring (`types`, `tsconfig.json`, `build:types`, and TypeScript dev dependency), and bumped wasm package version from `0.1.2` to `0.1.3`.
- Kept `wasm/nxpp.mjs` as a compatibility shim that re-exports the new facade entrypoint.
- Updated wasm and root docs to reflect the two-layer runtime/facade architecture and the new TypeScript layout.
- Updated `.gitignore` to keep `wasm/dist/` versioned so the facade publish artifacts are tracked in the repository.

## [1.0.32] - 2026-04-17

- Refactored the wasm Node binding layer from the old monolithic `wasm/node/nxpp_bindings.cpp` layout to a modular structure under `wasm/include/nxpp_wasm/` and `wasm/src/`, with a single final `EMSCRIPTEN_BINDINGS(...)` entrypoint in `wasm/src/nxpp_wasm.cpp`.
- Introduced a module-aligned registration architecture (`register_*_bindings()` + `register_all_bindings()`), plus shared common utilities in `wasm/include/nxpp_wasm/common/` and `wasm/src/common/`.
- Reworked graph-core exports around explicit typed classes (`GraphInt`, `GraphStr`, `DiGraphInt`, `DiGraphStr`, `MultiGraphInt`, `MultiGraphStr`, `MultiDiGraphInt`, `MultiDiGraphStr`) with no runtime int/string backend switching.
- Updated wasm build wiring so `wasm/scripts/build_wasm_node_module.sh` compiles all `wasm/src/**/*.cpp` sources and includes `wasm/include/`.
- Updated the Node contract surface/tests and wasm example to the new graph-core API, and revalidated build + contract tests on the modular layout.

## [1.0.31] - 2026-04-13

- Closed `#74` incremental wasm-node stabilization by fixing embind portability/build diagnostics in `wasm/node/nxpp_bindings.cpp`: finite checks now use `::isfinite(...)` and the affected `std::variant` wrappers now construct explicit alternatives through `std::in_place_type<...>`.
- Revalidated the experimental Node wasm lane end-to-end with emsdk active: `wasm/scripts/build_wasm_node_module.sh` passes, `wasm/scripts/run_wasm_node_contract_tests.sh` passes (`contract-tests-ok`), and `node wasm/nxpp_example.js` still returns the expected shortest-path smoke output.
- Advanced the experimental npm wasm package metadata by one patch step from `0.1.1` to `0.1.2` in `wasm/package.json` for the next publish.

## [1.0.30] - 2026-04-12

- Added a first npm-publish scaffold for the experimental wasm lane under `wasm/`: introduced/standardized `wasm/package.json`, `wasm/nxpp.mjs`, and `wasm/README.md` with a package-facing loader shape (`default`, `createNxpp`, and `nxpp` alias) over the generated Emscripten module.
- Added a minimal local usage example in `wasm/nxpp_example.js` that loads the package wrapper and runs a shortest-path call.
- Updated `.gitignore` so `wasm/build/nxpp_node.mjs` and `wasm/build/nxpp_node.wasm` are no longer ignored, enabling explicit npm package assembly from tracked artifacts, and finalized package metadata for the published experimental npm scope `@mik1810/nxpp-wasm`.

## [1.0.29] - 2026-04-12

- Realigned the experimental wasm lane after the folder refactor: Node bindings now live in `wasm/node/nxpp_bindings.cpp`, the Node API contract test now lives in `wasm/test/node_api_contract_test.mjs`, wasm Node artifacts now emit into `wasm/build/`, and the wasm Node build/contract scripts plus testing docs were updated to match the new layout.

## [1.0.28] - 2026-04-12

- Closed `#73` by converting the remaining distribution-channel discussion into explicit repository policy: AUR is now documented as an active secondary release-based channel, FetchContent / CPM.cmake guidance is now adopted as a near-term CMake-consumer path, Homebrew tap and Spack are documented as later-stage candidates, and Meson WrapDB plus Debian / Ubuntu packaging are documented as deferred for now; related strategy/status text is now aligned across `README.md`, `docs/README.md`, `docs/EXTERNAL_USAGE.md`, and `packaging/aur/README.md`.

## [1.0.27] - 2026-04-10

- Closed `#72` by expanding non-default selector coverage in the large-graph raw-Boost comparison path: `tests/test_large_graph_compare.cpp` now adds a second selector-focused regression using `boost::listS` / `boost::vecS` (alongside the existing `boost::listS` / `boost::listS` case) and validates `remove_node()` alignment, weighted edge-state alignment, shortest-path agreement, and attribute cleanup/preservation behavior against the raw Boost baseline.
- Continued `#73` with a concrete AUR implementation step instead of a pure roadmap note: the repository now carries `packaging/aur/` with a release-based `PKGBUILD`, `.SRCINFO`, and an Arch-oriented Docker smoke fixture, the README/external-usage docs now state more precisely that ConanCenter is still in progress while AUR has become a real repository-hosted distribution path, and the first `nxpp` package was pushed live to AUR as `1.0.21-1`.

## [1.0.26] - 2026-04-10

- Closed `#70` by adding end-to-end external-consumer integration tests for the main supported consumption modes: new fixture projects under `tests/external_consumers/` now validate vendored CMake (`add_subdirectory(nxpp)`), installed-package CMake (`find_package(nxpp CONFIG REQUIRED)`), and standalone single-header consumption; `scripts/unix/run_external_consumer_tests.sh` now runs those modes with mode-level failure visibility; and `.github/workflows/external-consumers.yml` runs that external-consumer path automatically in CI with a dedicated summary.

## [1.0.25] - 2026-04-10

- Folded the large-graph raw-Boost comparison lane into `compatibility.yml` as a dedicated `large-graph-compare` job, preserving the heavy-vs-fast split while removing the separate standalone workflow; the testing docs and README now describe the large-graph path as part of the main compatibility workflow family instead of as an orphan CI workflow.

## [1.0.24] - 2026-04-10

- Closed `#71` by adding dedicated formal assertion-based centrality tests: the repository now includes `tests/test_centrality.cpp` with direct deterministic checks for `degree_centrality()`, `pagerank()`, and `betweenness_centrality()`, and the Unix runner, Windows runner, and CMake formal-test list now all execute that new file.

## [1.0.23] - 2026-04-10

- Closed `#69` by adding a dedicated `minimum_cut()` multigraph regression test with heterogeneous parallel capacities: `tests/test_flow.cpp` now includes a small deterministic case that verifies both the aggregate cut value and the precise `cut_edge_ids` view when two parallel edges between the same endpoints carry different capacities.

## [1.0.22] - 2026-04-10

- Closed `#68` by fixing the multigraph capacity path inside `minimum_cut()`: the flow-graph builder now reads capacities through precise `edge_id` lookup instead of the ambiguous endpoint-based `(u, v)` lookup, so heterogeneous parallel capacities are modeled per concrete edge instance in multigraph mode.

## [1.0.21] - 2026-04-10

- Closed `#67` by adding an automated release-metadata drift guard: the new `scripts/check_release_metadata_versions.py` script now verifies that the top documented release version matches the repository-hosted package metadata in `CMakeLists.txt`, `conanfile.py`, and `packaging/vcpkg/ports/nxpp/vcpkg.json`, `release.yml` now runs that check before release extraction and publication, and the repository-hosted package metadata now tracks `1.0.21` so the guard passes against the current release candidate.

## [1.0.20] - 2026-04-10

- Closed `#66` by realigning the repository-hosted package metadata to the documented release version: `CMakeLists.txt`, `conanfile.py`, and `packaging/vcpkg/ports/nxpp/vcpkg.json` were brought back into sync with the active documented release line instead of the stale `1.0.1` version, and the README now states explicitly that the repository-hosted package metadata is aligned to the latest documented release line.

## [1.0.19] - 2026-04-09

- Closed `#51` by strengthening the dedicated single-header CI: `single-header.yml` now runs the real standalone-header test suite after rebuilding `dist/nxpp.hpp`, instead of stopping at a smoke check, while still keeping the smoke program and artifact upload path.

## [1.0.18] - 2026-04-09

- Closed `#50` by aligning the documented single-header testing story with the CI that actually exists: the testing guide now distinguishes clearly between the dedicated `single-header.yml` smoke-check workflow, the deeper local `run_single_header_tests.sh` path, and the release workflow that rebuilds and runs the standalone-header suite before publishing the release asset.

## [1.0.17] - 2026-04-09

- Closed `#49` by moving the narrow `"weight"` semantics higher in the public docs: the README now explains earlier that `"weight"` refers to the built-in edge-weight property rather than an arbitrary edge-attribute key, and the API guide now repeats that rule prominently ahead of the shortest-path and min-cost-flow tables instead of leaving it buried only in per-row notes and lower-level declaration comments.

## [1.0.16] - 2026-04-09

- Closed `#48` by making the include-path and external-consumption story more explicit: the README now separates repository-local examples from normal external includes, the external-usage guide now distinguishes more clearly between clone-local, vendored, installed-package, and single-header paths, and the docs index now points to `docs/EXTERNAL_USAGE.md` as the source of truth for include-path / consumption-mode guidance.

## [1.0.15] - 2026-04-09

- Closed `#47` by making the `NodeID` requirements much more visible in the public docs: the README now explains the practical constraints up front, the API companion guide now states explicitly that hash support is not a core requirement, the numeric-generator-only `std::size_t` construction rule is separated more clearly from the general `Graph` rule, and the main `Graph` declaration docs now call out the same assumptions directly.

## [1.0.14] - 2026-04-09

- Closed `#46` by clarifying the public policy for implicit node creation: the README, docs index, API architecture guide, and core proxy comments now state much more directly that write-style graph access may create missing nodes or edges, while read-style accessors and algorithmic reads do not create implicitly.

## [1.0.13] - 2026-04-08

- Closed `#56` by refining the Doxygen / GitHub Pages presentation layer: the generated site now uses the tree/sidebar navigation again, the local theme styles tables, code blocks, wrapper pages, and the landing page more deliberately, and the Doxygen main page now gives clearer reader-path guidance for module browsing, wrapper lookup, and guide-first navigation.
- Followed up on the `#56` presentation pass by tightening the remaining class/group-page rough edges: member tables now use the light theme consistently, long signatures wrap more safely inside the declaration box, `Parameters` / `Template Parameters` / `Returns` blocks have more padding and cleaner left alignment, and the duplicated `requires` lines are visually de-emphasized rather than dominating the method header.

## [1.0.12] - 2026-04-08

- Closed `#58` by adding generated-doc-friendly examples for the richer public result wrappers: the declaration pages for `SingleSourceShortestPathResult`, `MaximumFlowResult`, `MinCostMaxFlowResult`, and `MinimumCutResult` now carry short embedded examples, the curated API guide now points readers directly at those generated struct pages, and the landing/overview docs now note that those result-wrapper pages are now useful example entry points as well as shape references.

## [1.0.11] - 2026-04-08

- Closed `#57` by connecting the narrative docs and generated API docs more cleanly: the README and companion guides now point readers toward the most relevant published Doxygen entry pages and module pages, the docs index now states the cross-navigation rule explicitly, and the Doxygen landing page now tells GitHub Pages readers when to jump back into the longer narrative guides instead of descending only through symbol indexes.

## [1.0.10] - 2026-04-08

- Closed `#55` by turning the Doxygen main page into a stronger GitHub Pages landing page: `docs/DOXYGEN_MAINPAGE.md` now provides start-here guidance, common entry paths, result-wrapper links, practical module/file/class navigation cues, and clearer links back to the companion markdown guides instead of behaving like a thin reference placeholder.

## [1.0.9] - 2026-04-08

- Closed `#54` by organizing the generated docs around the real library modules: the semantic headers now define lightweight Doxygen groups for graph core, attributes, multigraph helpers, traversal, shortest paths, components, spanning/order helpers, flow, centrality, generators, and 2-SAT, and the generated main page now links directly to those module groups instead of relying only on flat file/class navigation.

## [1.0.8] - 2026-04-08

- Closed `#53` by filling Doxygen gaps for deprecated aliases and thin wrappers: the namespace-scope compatibility aliases across traversal, shortest paths, components, spanning-tree helpers, topological sort, and flow now carry real `@param` / `@return` coverage instead of only skeletal one-line deprecation stubs, so the generated reference is more useful when these entries appear in search or symbol navigation.

## [1.0.7] - 2026-04-08

- Closed `#52` by auditing and tightening Doxygen coverage across the full modular public-header surface: the semantic headers under `include/nxpp/` now carry explicit `@file` briefs for generated-reference navigation, the public graph alias/type presets at the end of `graph.hpp` now have clearer alias-level comments in the generated docs, and the docs index now records that this file-level/header-level coverage audit has been completed.

## [1.0.6] - 2026-04-08

- Closed `#45` by hardening the staged min-cost-flow path against hidden stale-state reuse: the internal residual-network cache is now invalidated on graph mutations such as node/edge insertion, edge removal, node removal, `clear()`, `set_edge_weight(...)`, and `set_edge_attr(...)`, `cycle_canceling()` now throws a specific error if the graph changed after `push_relabel_maximum_flow(...)`, and the formal flow tests now cover that invalidation path explicitly.

## [1.0.5] - 2026-04-08

- Closed `#44` by making the flow-wrapper result types edge-aware in multigraph cases: `MaximumFlowResult` and `MinCostMaxFlowResult` now expose both endpoint-keyed aggregate views and precise `edge_id`-keyed flow maps, `MinimumCutResult` now exposes precise `cut_edge_ids`, the flow builders now preserve every original edge instance instead of collapsing parallel edges by `(u, v)`, direct `flow.hpp` inclusion now pulls in the multigraph layer so the edge-ID helpers remain link-complete, and the formal flow tests now cover multigraph max-flow, minimum-cut, and min-cost result precision explicitly.
- Added a new `snippet/centrality/` parity folder with `nxpp`, Boost, and NetworkX implementations that print degree centrality, PageRank, and betweenness centrality on the same small directed graph; the snippet-review workflow summary now names the new case explicitly, and the printed precision was aligned so `bash scripts/unix/test_single_snippet.sh centrality` passes under the current exact-output harness.

## [1.0.4] - 2026-04-08

- Closed `#42` by explicitly restricting ambiguous multigraph insertion forms: endpoint-based `add_edge(..., attrs)` overloads now throw in multigraph mode instead of pretending to target one precise parallel edge, tests now cover the new failure mode, and the docs point users to `add_edge_with_id(...)` plus `set_edge_attr(edge_id, ...)` as the precise alternative.

## [1.0.3] - 2026-04-08

- Closed `#41` by documenting endpoint-based multigraph semantics more rigorously: `docs/API_REFERENCE.md` now carries a strict endpoint-based multigraph semantics table with a precise alternative for each ambiguous API, the README calls out the especially important `remove_edge(u, v)` behavior, and the public comments now spell out that endpoint-based numeric/optional edge reads remain convenience lookups in multigraph mode.

## [1.0.2] - 2026-04-08

- Closed `#43` at the policy/documentation level by defining the public multigraph split between precise APIs and convenience APIs: the docs now state explicitly that `edge_id` is the precise path for one concrete edge instance, while endpoint-based `(u, v)` forms remain convenience-oriented in multigraph mode unless a function documents a stronger guarantee.

## [1.0.1] - 2026-04-07

- Closed `#59` by defining the packaging and distribution strategy explicitly in the repository docs: the current supported consumer paths remain direct source inclusion and vendored `add_subdirectory(nxpp)` usage, the next first-class packaging target is proper CMake install/export/package-config support, Conan comes after the CMake package story is stable, vcpkg is a later ecosystem follow-up, and Debian / Ubuntu packaging is now documented as a later optional target rather than the default first distribution path.
- Closed `#60` by adding proper CMake install/export/package-config support: the repo now installs headers plus an exported `nxpp::nxpp` target, generates `nxppConfig.cmake` / `nxppConfigVersion.cmake`, and supports external `find_package(nxpp CONFIG REQUIRED)` consumption once installed into a CMake prefix.
- Closed `#61` by making the external-consumer story explicit across the docs: the repository now distinguishes clearly between repository-local header usage, vendored `add_subdirectory(...)` usage, installed-package `find_package(nxpp CONFIG REQUIRED)` usage, and the tested single-header release asset, with consistent include and Boost-dependency guidance across `README.md` and `docs/EXTERNAL_USAGE.md`.
- Closed `#62` by adding a first local Conan recipe: the repository now includes `conanfile.py` that models `nxpp` as a header-only package, declares Boost as a Conan dependency in header-only mode, and documents a `conan create . --profile:build=default --profile:host=default -s:h compiler.cppstd=20` validation path that now works locally with Conan 2.
- Closed `#63` by delivering a real supported vcpkg path through the repository-hosted overlay port under `packaging/vcpkg/ports/nxpp`: the repository now includes `portfile.cmake` and `vcpkg.json` for overlay-port consumption on top of the installed CMake package path, the port removes the empty `lib/` / `debug/` directories that vcpkg flags in post-build validation, both `vcpkg install nxpp --overlay-ports=...` and a small `find_package(nxpp CONFIG REQUIRED)` consumer path were validated locally, and a curated-registry submission was attempted upstream but deferred under vcpkg's current project-maturity policy.
- Closed `#64` by evaluating Debian / Ubuntu packaging and deferring it for now: the repository docs now state explicitly that a Debian source package / PPA / apt-facing maintenance story would add more ongoing packaging overhead than the project currently needs, while the supported consumption paths remain direct source usage, installed-package CMake, Conan, and the repository-hosted vcpkg overlay port.
- Closed `#65` by defining versioning and release expectations for distributed packages: the docs now state explicitly that tagged repository releases are the source of truth for repository-hosted package metadata, and that `CMakeLists.txt`, `conanfile.py`, and the repository-hosted vcpkg overlay port should track the same `X.Y.Z` release version even when third-party or policy-gated channels lag behind.

## [1.0.0] - 2026-04-07

- Declared the first stable `1.0.0` release after the `0.8.x` hardening cycle.
- Refined the root documentation for faster onboarding: `README.md` is now quick-start-first and includes CI, Boost, and C++20 badges.
- Consolidated release messaging for `1.0.0` in `RELEASE_NOTES.md` with a summary of API scope, CI coverage, and documentation maturity.

## [0.8.14] - 2026-04-07

- Closed `#40` by adding `docs/ATTRIBUTE_DESIGN.md`, a dedicated evaluation document that documents the explicit tradeoffs of the current `std::any` storage model, compares it against two plausible future alternatives (bounded `std::variant` and a typed attribute schema), and describes a conservative three-phase migration path from `std::any` to `std::variant`; the current decision (keep `std::any`) is justified explicitly in terms of the project's educational and prototyping scope; `docs/API_REFERENCE.md` now links to the new design doc at the end of the Attribute API section.

## [0.8.13] - 2026-04-07

- Closed `#5` by formalizing the implicit-node-and-edge-creation policy: the docs now carry a dedicated "Implicit node and edge creation" section in `docs/API_REFERENCE.md` with explicit write-creates / read-does-not-create tables; `NodeAttrProxy::operator=`, `EdgeAttrProxy::operator=`, and `EdgeProxy::operator=` in `graph.hpp` now carry docstrings documenting that they create the target node or edge if absent; a new formal test `test_implicit_creation_policy` in `test_edge_cases.cpp` verifies each creation case and confirms that read-only accessors (`has_node`, `has_edge`, `neighbors`) do not create on access.

## [0.8.12] - 2026-04-07

- Closed `#33` by adding `betweenness_centrality()` as a graph method in `include/nxpp/centrality.hpp`, returning `indexed_lookup_map<NodeID, double>` with normalization matching NetworkX `betweenness_centrality(G, normalized=True)` semantics; the implementation uses the Brandes BFS algorithm directly over the wrapper's internal BGL graph and vertex-index layer without requiring BGL property-map setup; a deprecated free-function alias is included for consistency with the rest of the centrality API migration; the formal test suite, `docs/API_REFERENCE.md`, and `docs/COMPLEXITY.md` are updated accordingly.

## [0.8.11] - 2026-04-07

- Closed `#34` by adding a first `pagerank()` wrapper on `Graph`, returning `indexed_lookup_map<NodeID, double>` plus a deprecated free-function alias for consistency with the rest of the graph-local API migration; `degree_centrality()` and `pagerank()` now also live under a dedicated `include/nxpp/centrality.hpp` semantic header instead of keeping their definitions inside `graph.hpp`.
- Closed `#19` by making the versioning and release policy explicit in the repository docs: `README.md` and `docs/README.md` now document the roles of `CHANGELOG.md`, `RELEASE_NOTES.md`, and `SESSION.md`, the fact that normal pushes to `main` do not publish releases, and the rule that the top documented version is the concrete next release candidate.
- Closed `#15` by adding an explicit docs section on the utility wrappers that go beyond direct NetworkX/BGL parity, explaining why wrappers such as `SingleSourceShortestPathResult`, `MaximumFlowResult`, `MinimumCutResult`, `MinCostMaxFlowResult`, and `indexed_lookup_map` are intentional parts of the public C++ surface rather than accidental extras.
- Closed `#14` by adding concrete examples for the richer public result wrappers in the main docs: `README.md` and `docs/API_REFERENCE.md` now show direct usage of `SingleSourceShortestPathResult`, flow/cut wrappers, and `indexed_lookup_map` instead of only naming those types abstractly.

## [0.8.9] - 2026-04-07

- Closed `#13` by documenting the thin alias story explicitly: the docs now distinguish between explicit graph type presets, short compatibility-friendly synonym aliases, convenience method aliases, and the remaining deprecated free-function compatibility wrappers.
- Closed `#16` by clarifying the weighted-overload story in the docs and public comments: the string `"weight"` is now documented explicitly as the built-in edge-weight property name in the compatibility-shaped overloads, rather than as a promise that arbitrary user-defined attribute names are supported everywhere as interchangeable weight backends.
- Closed `#27` at the design/policy level by documenting the intended direction of the current attribute system: `std::any` remains the pragmatic storage model for now, proxy syntax stays as the ergonomic write-friendly surface, checked accessors are the recommended read path, and `edge_id` remains the precise multigraph attribute path.
- Closed `#25` by reducing one of the clearest invariant-bypass paths in the public surface: `Graph::node_properties` and `Graph::edge_properties` are no longer public mutable members, while the existing `const` advanced getters remain as explicit escape hatches for read-only integrations.
- Clarified the documentation source-of-truth split: the generated Doxygen site is now documented as the declaration-driven API reference, `README.md` as the overview/navigation layer, and the markdown files under `docs/` as companion guides and policy documents rather than a second full declaration dump.
- Refined the generated Doxygen site into a cleaner API reference surface: the docs input now focuses on the public headers plus the curated guide pages instead of exposing changelog/session/todo material as first-class reference content, the landing page was rewritten into a more editorial homepage with working generated-page links, and the custom stylesheet now overrides the stock Doxygen chrome more decisively for a calmer, more formal presentation.
- Updated `.github/workflows/docs-pages.yml` so style-only changes under `assets/doxygen/` also trigger a Pages rebuild, preventing the published site from drifting behind the checked-in theme assets.

## [0.8.8] - 2026-04-05

- Closed `#28` by turning the generated-docs idea into a real documentation path: the semantic public headers now carry Doxygen-style inline API comments across the core graph wrapper, traversal helpers, shortest-path and flow result types, generators, topological sort / spanning-tree / component wrappers, 2-SAT helpers, and the deprecated free-function aliases, while the repository also gains a root `Doxyfile` plus `docs/DOXYGEN_MAINPAGE.md` as the generated-reference scaffold.
- Follow-up polish made the IDE hover text materially richer by moving weighted/template-gated declarations into hover-friendlier documentation positions and by expanding `@brief` / `@param` / `@return` guidance for traversal, shortest paths, spanning tree, flow, and result-wrapper APIs so the public surface now explains concrete return shapes instead of only naming the delegated algorithm.
- Validated the Doxygen generation path locally, tightened the remaining Doxygen references and markdown inputs so the generated HTML builds cleanly, and added `.github/workflows/docs-pages.yml` so the generated reference can now be published automatically to GitHub Pages when the repository Pages source is set to `GitHub Actions`.
- Added a lightweight custom Doxygen stylesheet under `assets/doxygen/` so the generated site looks more formal and less stock, while keeping the presentation layer entirely repo-local and easy to maintain.
- Tightened `.github/workflows/snippet-review.yml` so the heavy snippet parity workflow no longer runs on every generic push or pull request and instead triggers only when snippet-related sources, headers, showcase mains, or the workflow itself change.

## [0.8.7] - 2026-04-05

- Finalized `#20` as a real cross-platform formal-suite matrix: the repository now verifies the full formal test suite on `ubuntu-latest` with both `g++` and `clang++`, on `macos-latest` with `clang++`, and on `windows-latest` with native MSVC (`cl`) plus Boost from the official archive cache, while keeping the snippet-review workflow separate.

## [0.8.6] - 2026-04-05

- Closed `#20` by extending `.github/workflows/compatibility.yml` beyond the old Ubuntu-only `g++` path: the formal suite now runs in a small readable matrix covering Ubuntu with both `g++` and `clang++`, plus `macos-latest` with `clang++`, while leaving the snippet-review workflow unchanged.

## [0.8.5] - 2026-04-05

- Closed `#17` by adding a minimal root `CMakeLists.txt` that exposes `nxpp` as a header-only `INTERFACE` target for vendored `add_subdirectory(...)` consumption, while keeping packaging/install/export logic explicitly out of scope for now.

## [0.8.4] - 2026-04-05

- Closed `#30` by clarifying the repository testing story: the docs now distinguish more explicitly between showcase programs, the snippet parity/regression layer, the formal assertion-based suite, the single-header validation path, and the opt-in large-graph raw-Boost comparison path.

## [0.8.3] - 2026-04-05

- Closed `#6` by standardizing the main runtime-error wording across the public surface: node, edge, traversal, shortest-path, spanning-tree, and flow failures now use a consistent `X failed: ...` style instead of the earlier mix of generic messages, legacy `NetworkXError` text, and inconsistent punctuation.

## [0.8.2] - 2026-04-05

- Closed `#18` by documenting the external-usage story explicitly: the docs now explain the modular include path, the tested single-header release asset, the Boost dependency, and minimal consumer-side compile commands for both integration styles.

## [0.8.1] - 2026-04-05

- Closed `#7` by making the `NodeID` contract explicit: `Graph<NodeID, ...>` now fails early unless `NodeID` is copy-constructible, equality-comparable, and orderable via `std::less`, while the numeric generator helpers separately require `NodeID` to be constructible from `std::size_t`.

## [0.8.0] - 2026-04-05

- Switched `.github/workflows/release.yml` to a fully automatic main-branch publication path: when the top versions in `CHANGELOG.md` and `RELEASE_NOTES.md` match and the corresponding tag does not exist yet, the workflow now builds `dist/nxpp.hpp`, runs the modular and single-header suites, creates and pushes `vX.Y.Z`, and publishes the GitHub release in the same run.
- Began the first implementation pass for `#26` by moving `lookup_map`, `SingleSourceShortestPathResult`, `floyd_warshall_all_pairs_shortest_paths_map()`, and the rooted Prim parent-map result onto `std::map`, so those public result paths now rely on real tree-based bounds instead of expected hash-table behavior.
- Continued `#26` by moving the wrapper-owned `NodeID -> vertex_descriptor` translation map and external node/edge attribute stores onto `std::map`, so the core wrapper lookup path now has real tree-based bounds too.
- Updated the complexity documentation to reflect the new ordered-map costs explicitly, including the extra `V log V` materialization work for the affected shortest-path and component-map helpers and the `log` terms now paid by wrapper-managed node/attribute lookups.
- Added an `indexed_lookup_map` result wrapper and switched `connected_components()`, `strong_component_map()`, and `degree_centrality()` onto it, so those helpers keep linear materialization and their Boost-dominant asymptotic order while still exposing `O(log V)` key lookup with no hash-table assumptions.
- Extended the same indexed-wrapper approach to `strong_components()` / `strongly_connected_component_roots()`, so the SCC representative map also keeps linear materialization while exposing real `O(log V)` key lookup.
- Extended `indexed_lookup_map` to `bfs_successors()`, `dfs_predecessors()`, and `dfs_successors()`, so the traversal tree-view helpers also keep linear materialization while dropping the earlier hash-table-based result containers.
- Replaced the old external `VertexDesc -> index` hash map with an internal per-vertex wrapper-index property in `Graph`, so the main BGL algorithm path no longer depends on `std::unordered_map` for vertex-index normalization.
- Removed the local `NodeID -> flow-graph-index` hash maps from the flow helpers and switched those auxiliary builders to the existing wrapper index, so the library implementation no longer relies on `std::unordered_map` in `include/nxpp`.
- Removed eager all-path storage from `SingleSourceShortestPathResult`, so `dijkstra_shortest_paths()`, `bellman_ford_shortest_paths()`, and `dag_shortest_paths()` now return ordered `distance` / `predecessor` maps plus on-demand `path_to(target)` reconstruction while preserving the same dominant complexity as the underlying Boost algorithms.

## [0.7.13] - 2026-04-03

- Strengthened the opt-in large-graph comparison driver so representative BFS, connected-components, strongly-connected-components, Dijkstra, and post-`remove_node()` checks now run across multiple fixed deterministic seeds instead of relying on one seed per scenario.
- Added a large-graph regression that builds `nxpp::Graph<int, int, true, false, true, boost::listS, boost::listS>` and verifies that non-default selector usage still matches raw Boost after large `remove_node()` mutations and Dijkstra recomputation.
- Kept the expanded verification path inside `tests/test_large_graph_compare.cpp` and `scripts/run_large_graph_compare.sh`, so the extra confidence stays opt-in without slowing down the default fast suite in `scripts/run_tests.sh`.

## [0.7.12] - 2026-03-30

- Added `tests/test_large_graph_compare.cpp` plus `scripts/run_large_graph_compare.sh` as an opt-in large-graph verification path that generates deterministic graph instances and compares `nxpp` results against raw Boost on BFS, DFS, connected components, strongly connected components, Dijkstra, Bellman-Ford, DAG shortest paths, reachable negative-cycle detection, post-`remove_node()` graph state, large multigraph mutation behavior, attribute preservation/cleanup after repeated mutations, combined weighted-graph mutation sequences, Floyd-Warshall all-pairs shortest paths, large maximum-flow/minimum-cut results, and large successive-shortest-path min-cost flow.
- Added `.github/workflows/large-graph-compare.yml` as a dedicated workflow that runs the new large-graph comparison path against both the modular headers and the generated `dist/nxpp.hpp`, then publishes a separate Markdown job summary.
- Kept the large-graph path outside `scripts/run_tests.sh` so the normal formal suite stays fast while the repository still gains a scale-oriented correctness check beyond the smaller snippet and assertion-based cases.

## [0.7.11] - 2026-03-30

- Added a minimal compiler/platform support matrix to `README.md` that states the currently verified support story conservatively instead of implying broader CI coverage than the repository actually has.
- Documented Linux plus `g++`/C++20 with Boost Graph as the main tested path, while marking macOS, Windows, and Clang as expected or possible follow-up environments rather than current CI-backed guarantees.

## [0.7.10] - 2026-03-29

- Added `scripts/run_benchmark_report.py` as a benchmark orchestrator that runs the serial snippet-compile benchmark first, then the parallel one, and produces a single Markdown report with embedded SVG plots.
- Kept the benchmark output intentionally compact: one serial CSV, one parallel CSV, `benchmark/BENCHMARK.md`, and plots under `benchmark/imgs/`, with older benchmark outputs moved into `backups/benchmark/<timestamp>/`.
- Updated the benchmark documentation and repository rules to reflect the new report workflow and the separation between concise technical history (`CHANGELOG.md`) and richer operational conventions (`AGENTS.md` / `RELEASE_NOTES.md`).

## [0.7.9] - 2026-03-29

- Fixed `.github/workflows/release.yml` so the `workflow_dispatch` path no longer assumes that a tag pushed with `GITHUB_TOKEN` will trigger a second release run.
- The manual release path now creates and pushes the tag, then continues in the same workflow run to extract notes, build `dist/nxpp.hpp`, run `bash scripts/run_single_header_tests.sh`, and publish the GitHub release.
- Kept the pushed-tag path working too, so the same workflow still supports both manual kickoff and direct tag-driven release publication.

## [0.7.8] - 2026-03-29

- Extended `.github/workflows/release.yml` with `workflow_dispatch`, so release kickoff no longer requires a manually created git tag.
- The dispatch path now reads the top version from `RELEASE_NOTES.md` and `CHANGELOG.md`, fails if they disagree, creates and pushes the corresponding `vX.Y.Z` tag, and lets the tag-triggered path build, test, and publish the actual release.
- Extended `scripts/extract_release_notes.py` with a `--latest-version` mode so the release workflow can read the current top version from the versioned markdown files without duplicating parsing logic.

## [0.7.7] - 2026-03-29

- Added a versioned `RELEASE_NOTES.md` file so GitHub releases can use richer release-specific prose without forcing the same level of detail into `CHANGELOG.md`.
- Switched `scripts/extract_release_notes.py` and `.github/workflows/release.yml` to read from `RELEASE_NOTES.md` instead of `CHANGELOG.md`, keeping the changelog concise while improving release presentation quality.
- Stopped ignoring `RELEASE_NOTES.md` in `.gitignore` so the release-note source of truth now lives in the repository alongside the rest of the release automation.

## [0.7.6] - 2026-03-29

- Added `scripts/extract_release_notes.py` plus `.github/workflows/release.yml` so pushed `v*` tags now create GitHub releases automatically using the matching `CHANGELOG.md` section as the release notes.
- Moved the standalone-header asset publication responsibility out of `.github/workflows/single-header.yml` and into the dedicated release workflow, which builds `dist/nxpp.hpp`, runs `bash scripts/run_single_header_tests.sh`, and only then creates the release with `nxpp.hpp` attached.
- Kept `.github/workflows/single-header.yml` focused on push/pull-request validation of the generated header and artifact upload, instead of mixing CI verification with release publication.
- Updated the repository workflows to `actions/checkout@v6` and `actions/setup-python@v6` so they align with GitHub's Node 24 migration instead of emitting the Node 20 deprecation warning.

## [0.7.5] - 2026-03-29

- Added `scripts/run_single_header_tests.sh` so the formal test suite can be recompiled directly against `dist/nxpp.hpp` instead of the modular include tree.
- Tightened `.github/workflows/single-header.yml` so the release-upload path now builds `dist/nxpp.hpp`, runs the dedicated single-header test suite against that generated file, and only then publishes the standalone `nxpp.hpp` asset.
- Push and pull-request runs still expose `dist/nxpp.hpp` as a workflow artifact without adding the full single-header test-suite gate to every standalone-header check.

## [0.7.4] - 2026-03-29

- Extended `.github/workflows/single-header.yml` so published GitHub releases receive the tested CI-built `nxpp.hpp` asset instead of relying on a locally generated upload.
- The standalone-header workflow still uploads `dist/nxpp.hpp` as a workflow artifact, and now also pushes the same tested file into the release assets on `release.published`.

## [0.7.3] - 2026-03-29

- Added a dedicated GitHub Actions workflow for the generated standalone header under `.github/workflows/single-header.yml`.
- The workflow now builds `dist/nxpp.hpp`, smoke-tests the generated header by compiling and running a tiny program against it, and uploads the file as a workflow artifact.

## [0.7.2] - 2026-03-29

- Narrowed the test-suite include surface where it stayed readable, moving `test_attributes.cpp`, `test_multigraph.cpp`, and `test_flow.cpp` onto their matching semantic headers and replacing the blanket umbrella include in `test_edge_cases.cpp` with the specific graph/traversal/shortest-path/components headers it actually uses.
- Re-verified the refactor by rebuilding the touched test binaries explicitly and rerunning the full formal suite at `31/31`.

## [0.7.1] - 2026-03-29

- Tightened the semantic-header dependency graph so the public modular headers no longer form the earlier artificial include chain.
- Moved `topological_sort()` into a dedicated `include/nxpp/topological_sort.hpp` header instead of keeping it bundled with the minimum-spanning-tree helpers.
- Updated the `*_nxpp.cpp` snippet set to include the narrowest semantic header that matches each example domain instead of defaulting to `include/nxpp.hpp`, with `2sat_helper_nxpp.cpp` using `sat.hpp` where it really needs the direct 2-SAT helper.
- Re-verified the snippet include refactor by compiling the touched snippets in batches, keeping the formal test suite green at `31/31`, and rechecking the generated standalone `dist/nxpp.hpp`.

## [0.7.0] - 2026-03-29

- Split the library into a real semantic public-header layout under `include/nxpp/`, with dedicated homes for the core graph surface, attributes, multigraph edge-identity helpers, traversal, shortest paths, components, spanning-tree helpers, flow/cut/min-cost helpers, generators, and 2-SAT helpers.
- Kept `include/nxpp.hpp` as the canonical umbrella include while moving the implementation source of truth into the semantic headers.
- Added `include/nxpp/sat.hpp` so the 2-SAT helpers no longer live as an odd tail inside the umbrella header.
- Added a real standalone single-header build path via `scripts/build_single_header.py`, with `scripts/build_single_header.sh` as a thin wrapper, generating `dist/nxpp.hpp` by recursively expanding local `nxpp` headers while leaving standard-library and Boost includes intact.
- Updated the repository documentation to explain the modular include story, the umbrella header role, and the generated single-header distribution artifact.

## [0.6.0] - 2026-03-29

- Completed the first formal test-suite block by closing `#8`, `#9`, `#10`, `#11`, `#12`, and `#4`.
- Expanded the assertion-based suite to cover the core wrapper surface, attribute failure paths, empty/singleton/disconnected graph cases, multigraph-specific behavior, flow wrappers, and `remove_node()` descriptor-remapping behavior.
- Extended `scripts/run_tests.sh` into the canonical local test entry point, with grouped output and a single aggregate summary line.
- Added a dedicated GitHub Actions workflow for the formal test suite under `.github/workflows/tests.yml`, including a Markdown job summary for each run.
- Brought the suite to 31 passing checks across `test_core`, `test_attributes`, `test_edge_cases`, `test_flow`, `test_remove_node`, and `test_multigraph`.

## [0.5.6] - 2026-03-29

- Added `tests/test_remove_node.cpp` and wired it into `scripts/run_tests.sh` to cover `remove_node()` descriptor-remapping and cleanup behavior in the formal test suite.
- Added executable checks for post-removal node/edge views, attribute cleanup, traversal and shortest-path behavior after remapping, component views, and multigraph incident-edge cleanup during node removal.

## [0.5.5] - 2026-03-29

- Added `tests/test_flow.cpp` and wired it into `scripts/run_tests.sh` to cover the flow-wrapper surface in the formal test suite.
- Added executable checks for `maximum_flow`, `minimum_cut`, `push_relabel_maximum_flow`, `cycle_canceling`, `successive_shortest_path_nonnegative_weights`, and the min-cost alias wrappers, including the failure path where `cycle_canceling()` is called without cached flow state.

## [0.5.4] - 2026-03-29

- Added `tests/test_edge_cases.cpp` and wired it into `scripts/run_tests.sh` to cover empty, singleton, missing-node, and disconnected-graph cases in the formal test suite.
- Added executable checks for empty graph views, singleton traversal behavior, failure on missing-node operations, unreachable shortest-path state, and disconnected component grouping.

## [0.5.3] - 2026-03-29

- Added `tests/test_attributes.cpp` and wired it into `scripts/run_tests.sh` to cover attribute failure paths in the formal test suite.
- Added executable checks for missing attribute lookups, attribute type mismatches, `try_get_*_attr(...)` empty-return behavior, and rejection of non-numeric values in numeric edge-attribute lookup.

## [0.5.2] - 2026-03-29

- Added a dedicated multigraph regression binary under `tests/test_multigraph.cpp` and wired it into `scripts/run_tests.sh`.
- Covered multigraph-specific behavior beyond the initial test-suite foundation: distinct `edge_id` values for parallel edges, per-edge attribute separation, precise `remove_edge(edge_id)` semantics, all-edge `remove_edge(u, v)` semantics, and stable endpoint lookup after partial removal.

## [0.5.1] - 2026-03-29

- Added a first assertion-based test binary under `tests/test_core.cpp` and a matching `scripts/run_tests.sh` runner, establishing a real test-suite entry point beyond snippet parity and showcase demos.
- Covered the most recently stabilized core behavior with executable assertions: typed node/edge attributes, Dijkstra result wrappers, explicit multigraph `edge_id` operations, multigraph removal cleanup, and C-string attribute normalization.
- Added a dedicated GitHub Actions workflow for the formal test suite under `.github/workflows/tests.yml`, with a Markdown job summary that records the test command and output.

## [0.5.0] - 2026-03-29

- Closed the previous critical issue block around graph configuration and multigraph behavior: `#24`, `#3`, `#2`, and `#1`.
- Promoted explicit multigraph edge identity into the public API through `edge_id`-based creation, lookup, mutation, and removal helpers.
- Finished the configuration-surface work so `Graph<...>` now supports direct selector customization while the standard aliases remain on the default `boost::vecS` / `boost::vecS` backend.
- Fixed multigraph edge-metadata cleanup when removing all parallel edges between two endpoints.
- Clarified the remaining `(u, v)` multigraph semantics in the public docs so the README now matches the current implementation behavior.
- Split the showcase layer into aligned demos: `main_boost.cpp` for raw Boost, `main_nxpp.cpp` for `nxpp`, and `main.py` as the NetworkX/Python companion.
- Removed the unused `nxpp::print` helper from the public header/reference docs.
- Normalized C-string attributes to `std::string` internally so calls like `set_edge_attr(..., "fast")`, proxy assignment, and initializer-list attrs work correctly with typed string reads.
- Slimmed the root `README.md` into a clearer overview/navigation document and moved detailed reference material into `docs/API_REFERENCE.md`.
- Improved benchmark tooling with clearer timing resolution, a parallel compile benchmark driver, and quieter default output.
- Prepared the repository for release consumption by keeping the single-header entry point at `include/nxpp.hpp` and shipping it as a dedicated release asset alongside the standard source archives.

## [0.4.1] - 2026-03-29

- Extended `nxpp::Graph` so advanced users can customize both the BGL out-edge selector and vertex selector directly, while all existing aliases continue to use the default `boost::vecS` / `boost::vecS` backend.
- Removed the remaining internal assumption that vertex descriptors behave like dense indices by maintaining wrapper-side vertex index bookkeeping compatible with non-`vecS` vertex selectors.
- Added compile-time validation for unsupported selector combinations, including `Multi=true` with `boost::setS`.
- Fixed `remove_edge(u, v)` metadata cleanup in multigraph cases so tracked edge properties are removed consistently when all parallel edges between two endpoints are erased.
- Clarified the current multigraph semantics of `has_edge`, `get_edge_weight`, `get_edge_attr`, proxy edge access (`G[u][v]`), and `remove_edge` so the README now matches the actual `(u, v)`-based implementation behavior.
- Added an explicit `edge_id`-based API for precise multigraph work, including `add_edge_with_id`, `edge_ids`, `has_edge_id`, `get_edge_endpoints`, edge-id-based attribute/weight accessors, and single-edge removal by `edge_id`.
- Slimmed the root `README.md` back toward an overview/navigation role and moved the detailed API reference into `docs/API_REFERENCE.md` to reduce documentation drift risk.
- Split the showcase demos into aligned entry points: `main_boost.cpp` now demonstrates raw Boost, `main_nxpp.cpp` demonstrates `nxpp`, and `main.py` follows the same story from the NetworkX side.
- Removed the unused `nxpp::print` helper from the public header/reference docs.
- Normalized C-string attributes to `std::string` internally so calls like `set_edge_attr(..., "fast")` and initializer-list attrs behave correctly with typed string reads.
- Updated `main.cpp` to the current method-based API and removed reliance on deprecated free-function entry points in the demo/smoke path.
- Updated `README.md` and `docs/GRAPH_CONFIGURATION.md` to describe the actual configuration surface, current support matrix, testing story, and open issue areas.

## 2026-03-28

- Added `docs/GRAPH_CONFIGURATION.md` to define the supported BGL graph-configuration surface and the current policy for advanced selector-level customization.
- Extended `nxpp::Graph` so advanced users can override the BGL out-edge selector directly while keeping existing aliases on the default `boost::vecS` / `boost::vecS` backend.
- Added a MIT `LICENSE` file at the repository root.
- Completed the manual snippet parity/review pass across the folders under `snippet/` and backed it with a GitHub Actions snippet-review workflow summary.
- Renamed and aligned several Boost-style wrappers and snippet examples, including `dijkstra_shortest_paths()`, `dag_shortest_paths()`, `successive_shortest_path_nonnegative_weights()`, and the SCC representative/map helpers.
- Fixed weighted shortest-path parity for integer-weight graph aliases, including the `dag_sp` and `floyd_warshall` snippet paths.
- Updated project documentation (`README.md`, `TODO.md`) to reflect the current snippet-review status, architecture open questions, and repository metadata.

## 2026-03-27

- Extended the flow API in `include/nxpp.hpp` with explicit `edmonds_karp_maximum_flow()` and `push_relabel_maximum_flow()` entry points while keeping `maximum_flow()` as the backward-compatible default.
- Added inline edge-attribute overloads for `add_edge()`, including the single-attribute form used by the flow snippets.
- Updated Floyd-Warshall to return a matrix-oriented result directly, with a companion `floyd_warshall_all_pairs_shortest_paths_map()` helper for NodeID-keyed access.
- Continued manual snippet review/parity work through `floyd_warshall`; the next planned snippet pass is `graph_example`.

## 2026-03-25

- Added snippet-oriented API improvements in `include/nxpp.hpp`, including visitor-based traversal entry points, single-source shortest-path helpers, common graph aliases, component map helpers, and a const-friendly lookup wrapper.
- Added `scripts/test_snippet_batch.sh` to compile and compare the `2sat`, `bellman_ford`, `bfs`, and `cc` snippet triplets while logging timings to both terminal and file.
- Reached snippet cleanup and parity work through `cc`; the next planned snippet pass is `dag_sp`.
