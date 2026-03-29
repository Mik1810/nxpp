# Changelog

This project starts explicit release versioning with `0.4.1`. Older entries below remain as date-based pre-versioning history.

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
