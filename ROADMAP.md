# Project Roadmap: nxpp (NetworkX for C++)

## Objective
Create a C++ library that offers a syntax as identical and intuitive as possible to the Python library `networkx`, while maintaining high performance and minimizing *bloat*. The project will internally leverage efficient data structures and algorithms (such as those from the Boost Graph Library - BGL), while exposing a clean and user-friendly API.

---

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
- [ ] **Network Flow**: `maximum_flow` (e.g. Edmonds-Karp/Push-Relabel algorithm variants), `minimum_cut`.
- [ ] **Centrality Metrics**: `degree_centrality`, `betweenness_centrality`, `pagerank`.
- [ ] Comprehensive benchmarking to compare sheer processing performance (execution time, memory usage, compilation time) between `nxpp` and `networkx`.

## Future Horizons and Distribution
- [x] Refactoring to enforce a strictly "Header-only" philosophy (evaluating the inherent trade-offs on compilation times regarding deep BGL templates).
- [ ] (Optional) Development of native Python bindings (e.g., via `pybind11`) so the library can act as an ultra high-performance backend drop-in replacement module directly importable in Python projects.
