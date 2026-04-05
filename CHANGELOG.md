# Changelog

This project starts explicit release versioning with `0.4.1`. Older entries below remain as date-based pre-versioning history.

## [0.7.13] - 2026-04-03

- Strengthened the opt-in large-graph comparison driver so representative BFS, connected-components, strongly-connected-components, Dijkstra, and post-`remove_node()` checks now run across multiple fixed deterministic seeds instead of relying on one seed per scenario.
- Added a large-graph regression that builds `nxpp::Graph<int, int, true, false, true, boost::listS, boost::listS>` and verifies that non-default selector usage still matches raw Boost after large `remove_node()` mutations and Dijkstra recomputation.
- Kept the expanded verification path inside `tests/test_large_graph_compare.cpp` and `scripts/run_large_graph_compare.sh`, so the extra confidence stays opt-in without slowing down the default fast suite in `scripts/run_tests.sh`.
- Began the first implementation pass for `#26` by moving `lookup_map`, `SingleSourceShortestPathResult`, `floyd_warshall_all_pairs_shortest_paths_map()`, and the rooted Prim parent-map result onto `std::map`, so those public result paths now rely on real tree-based bounds instead of expected hash-table behavior.
- Continued `#26` by moving the wrapper-owned `NodeID -> vertex_descriptor` translation map and external node/edge attribute stores onto `std::map`, so the core wrapper lookup path now has real tree-based bounds too.
- Updated the complexity documentation to reflect the new ordered-map costs explicitly, including the extra `V log V` materialization work for the affected shortest-path and component-map helpers and the `log` terms now paid by wrapper-managed node/attribute lookups.
- Added an `indexed_lookup_map` result wrapper and switched `connected_components()`, `strong_component_map()`, and `degree_centrality()` onto it, so those helpers keep linear materialization and their Boost-dominant asymptotic order while still exposing `O(log V)` key lookup with no hash-table assumptions.
- Extended the same indexed-wrapper approach to `strong_components()` / `strongly_connected_component_roots()`, so the SCC representative map also keeps linear materialization while exposing real `O(log V)` key lookup.
- Extended `indexed_lookup_map` to `bfs_successors()`, `dfs_predecessors()`, and `dfs_successors()`, so the traversal tree-view helpers also keep linear materialization while dropping the earlier hash-table-based result containers.
- Replaced the old external `VertexDesc -> index` hash map with an internal per-vertex wrapper-index property in `Graph`, so the main BGL algorithm path no longer depends on `std::unordered_map` for vertex-index normalization.
- Removed the local `NodeID -> flow-graph-index` hash maps from the flow helpers and switched those auxiliary builders to the existing wrapper index, so the library implementation no longer relies on `std::unordered_map` in `include/nxpp`.
- Removed eager all-path storage from `SingleSourceShortestPathResult`, so `dijkstra_shortest_paths()`, `bellman_ford_shortest_paths()`, and `dag_shortest_paths()` now return ordered `distance` / `predecessor` maps plus on-demand `path_to(target)` reconstruction while preserving the same dominant complexity as the underlying Boost algorithms.

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
