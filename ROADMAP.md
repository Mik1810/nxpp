# Project Roadmap: nxpp (NetworkX for C++)

## Objective
Create a C++ library that offers a syntax as identical and intuitive as possible to the Python library `networkx`, while maintaining high performance and minimizing *bloat*. The project will internally leverage efficient data structures and algorithms (such as those from the Boost Graph Library - BGL), while exposing a clean and user-friendly API.

## Priority Rule

The local reference snippets now define implementation priority:

- `snippet/` is the primary BGL algorithm source
- `py_snippet/` is the primary NetworkX behavior source
- functions represented in those snippet sets are the essential target surface
- work outside that surface should be tracked in `TODO.md` and should not outrank snippet-backed algorithms



## Phase 1: Core Data Structures
- [x] Implementation of the `Graph` class (Undirected simple graph).
- [x] Implementation of the `DiGraph` class (Directed simple graph).
- [x] Implementation of the `MultiGraph` and `MultiDiGraph` classes (Multigraphs with parallel edges).
- [x] Support for generic node identifiers (e.g., integers, strings) via templates or flexible C++ types.
- [x] Fundamental methods identical to NetworkX: `add_node`, `add_nodes_from`, `add_edge`, `add_edges_from`, `remove_node`, `remove_edge`, `clear`.
- [x] Support for custom attributes on nodes and edges (via `std::any` parity).

## Phase 2: API Wrappers and Python-like Syntax
- [x] Operator overloading to replicate dictionary access (e.g., `G[u][v]`).
- [x] C++11/C++20 views and iterators to simulate `G.nodes()`, `G.edges()`, `G.neighbors(n)`.
- [x] Intuitive error management (simulating exceptions like `NetworkXError`, `NodeNotFound`).

## Phase 3: Fundamental Algorithms (BGL Porting & Integration)
Integration and wrapping of BGL algorithms to match NetworkX signatures:
- [x] **Traversals**: `bfs_edges`, `bfs_tree`, `dfs_edges`, `dfs_tree`.
- [x] **Shortest Paths**: `shortest_path`, `dijkstra_path`, `bellman_ford_path`.
- [x] **Components**: `connected_components`, `strongly_connected_components`.
- [x] **Generators**: Basic topological generators like `complete_graph`, `path_graph`, `erdos_renyi_graph`.

## Phase 4: Comparative Test Suite (C++ vs Python)
This is a fundamental milestone to guarantee library correctness and mathematical parity.
- [x] **Dataset Generation**: Scripts to generate standardized input graphs (in edgelist, JSON or GraphML formats).
- [x] **Python Test Runner**: Script that executes specific algorithms using original `networkx` and saves results (nodes, edges, paths, metrics) to isolated text logs.
- [x] **C++ Test Runner**: Test suite for `nxpp` that natively processes the exact same graph structures and insertions.
- [x] **Matching Validator**: Automatic comparison pipeline verifying that C++ algorithm topological outputs match the Python generated outputs *exactly*.

## Phase 5: Advanced Algorithms and Flows
Integrate more complex mathematical features based on the provided BGL architectural codebase:
- [x] **Network Flow**: `maximum_flow` (e.g. Edmonds-Karp/Push-Relabel algorithm variants), `minimum_cut`.
- [ ] **Centrality Metrics**: `degree_centrality`, `betweenness_centrality`, `pagerank`.
  `degree_centrality` is now implemented in `include/nxpp.hpp`; parity verification for the rest of the centrality block is still open.
- [ ] Comprehensive benchmarking to compare sheer processing performance (execution time, memory usage, compilation time) between `nxpp` and `networkx`.

## Current Status Audit
- [x] Phases 1-4 are present in code and documented as complete.
- [x] A compile blocker caused by a stale alias (`DiGraphStr`) in `include/nxpp.hpp` was identified on March 25, 2026 and then resolved by removing the unused alias.
- [x] Additional unused alias cleanup removed `GraphStr` to keep the public surface smaller and less ambiguous.
- [x] `README.md` now documents the actual architecture and invariants of `include/nxpp.hpp`, including current caveats around descriptor-backed state and attribute handling.
- [x] The core header no longer stores edge metadata keyed by fragile `EdgeDesc`; stable internal edge IDs now back `edge_properties`, and destructive operations clean metadata explicitly.
- [x] Directed adjacency semantics are now clearer in the public API through explicit `successors()` and `predecessors()` helpers.
- [x] Common public aliases now cover both integer and string simple-graph defaults (`GraphInt`, `GraphStr`) in addition to the directed and multigraph forms.
- [x] Attribute access is now less fragile through explicit checked getters and optional-return helpers layered on top of the existing proxy syntax.
- [x] The algorithm wrapper layer now includes the documented basic names `bfs_tree`, `dfs_tree`, `shortest_path`, and `bellman_ford_path`, though the API surface remains intentionally minimal.
- [x] `shortest_path()` semantics are now closer to NetworkX: default calls are unweighted, while weighted dispatch is explicit through the `"weight"` parameter.
- [x] Distance-returning shortest-path helpers now follow the same weighted vs unweighted split as the path-returning helpers.
- [x] Project priority is now explicitly re-anchored to the local `snippet/` and `py_snippet/` reference functions, with out-of-scope work tracked in `TODO.md`.
- [x] A first major batch of snippet-backed essentials is now implemented: `topological_sort`, `kruskal_minimum_spanning_tree`, `prim_minimum_spanning_tree`, `maximum_flow`, `dag_shortest_paths`, `floyd_warshall_all_pairs_shortest_paths`, and `two_sat_satisfiable`.
- [x] The remaining snippet-backed essentials are now covered as well: min-cost max-flow wrappers and SCC representative/root-map helper.
- [x] Chained proxy access now includes explicit `const char*` overloads to reduce editor/IntelliSense ambiguity on attribute access.
- [x] GCC false-positive warnings from Boost min-cost-flow internals are now locally contained so core builds remain readable.
- [x] Each C++ snippet in `snippet/` now has a colocated Python `networkx` counterpart to support side-by-side algorithm comparison.
- [x] Each snippet folder now also has an `*_nxpp.cpp` counterpart using `include/nxpp.hpp`, and the local examples have been aligned so Boost C++, NetworkX Python, and nxpp C++ produce matching outputs.
- [x] Traversal and single-source shortest-path APIs now cover both Boost-like and NetworkX-like usage styles, including visitor-object traversal entry points for snippet parity.
- [x] Traversal snippets now build on a single generic `nxpp` visitor base class instead of ad-hoc callback lambdas or snippet-local freeform visitor conventions.
- [x] Component algorithms now expose both grouped outputs and `node -> component_id` map helpers, reducing flattening boilerplate in snippet-style code.
- [x] A first shell-based snippet harness now exists for `2sat`, `bellman_ford`, `bfs`, and `cc`, with compile/run timing and tee-style logging to file plus terminal.
- [x] The initial shell harness now suppresses warning noise from the non-nxpp reference snippets so the report stays focused on parity and nxpp-facing issues.
- [x] Manual snippet review/parity work has now been carried from `dag_sp` through `floyd_warshall`.
- [ ] The shell harness is still only complete through `cc`; extending it to the manually reviewed folders remains open.
- [ ] The next targeted snippet folder for manual review is `graph_example`.
- [ ] Phase 5 is partially started: `degree_centrality` and the network-flow block (`maximum_flow`, `minimum_cut`) exist, while `betweenness_centrality`, `pagerank`, and benchmarking are still missing.
- [ ] No benchmarking harness exists yet for the Phase 5 performance milestone.

## Future Horizons and Distribution
- [x] Refactoring to enforce a strictly "Header-only" philosophy (evaluating the inherent trade-offs on compilation times regarding deep BGL templates).
- [ ] (Optional) Development of native Python bindings (e.g., via `pybind11`) so the library can act as an ultra high-performance backend drop-in replacement module directly importable in Python projects.
