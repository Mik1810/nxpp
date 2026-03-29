# Changelog

This project starts explicit release versioning with `0.4.1`. Older entries below remain as date-based pre-versioning history.

## [0.4.1] - 2026-03-29

- Extended `nxpp::Graph` so advanced users can customize both the BGL out-edge selector and vertex selector directly, while all existing aliases continue to use the default `boost::vecS` / `boost::vecS` backend.
- Removed the remaining internal assumption that vertex descriptors behave like dense indices by maintaining wrapper-side vertex index bookkeeping compatible with non-`vecS` vertex selectors.
- Added compile-time validation for unsupported selector combinations, including `Multi=true` with `boost::setS`.
- Fixed `remove_edge(u, v)` metadata cleanup in multigraph cases so tracked edge properties are removed consistently when all parallel edges between two endpoints are erased.
- Clarified the current multigraph semantics of `has_edge`, `get_edge_weight`, `get_edge_attr`, proxy edge access (`G[u][v]`), and `remove_edge` so the README now matches the actual `(u, v)`-based implementation behavior.
- Added an explicit `edge_id`-based API for precise multigraph work, including `add_edge_with_id`, `edge_ids`, `has_edge_id`, `get_edge_endpoints`, edge-id-based attribute/weight accessors, and single-edge removal by `edge_id`.
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
