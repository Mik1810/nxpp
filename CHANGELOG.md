# Changelog

## 2026-03-28

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
