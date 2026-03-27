# nxpp — NetworkX-inspired graph utilities for modern C++

<p align="center">
  <img src="imgs/logo.svg" alt="nxpp logo" width="220">
</p>

`nxpp` is a **header-only C++20** library built on top of the **Boost Graph Library (BGL)**.
Its goal is not to clone all of NetworkX, but to offer a **small, practical, NetworkX-inspired API** that is easier to use directly from C++ than raw BGL in many common cases.

The project centers on a single generic graph wrapper:

```cpp
nxpp::Graph<NodeID, EdgeWeight, Directed, Multi>
```

with public aliases such as:

- `nxpp::WeightedGraphInt`
- `nxpp::WeightedGraphStr`
- `nxpp::WeightedDiGraph`
- `nxpp::WeightedDiGraphInt`
- `nxpp::UnweightedGraphInt`
- `nxpp::UnweightedDiGraphInt`

Internally, `nxpp` stores a BGL graph and maintains a translation layer between user-facing node IDs and BGL vertex descriptors.
That translation layer is what allows code such as:

```cpp
G.add_edge("Rome", "Milan", 5.0);
auto path = nxpp::dijkstra_path(G, std::string("Rome"), std::string("Milan"));
G.node("Rome")["population"] = 2800000;
G["Rome"]["Milan"]["company"] = std::string("Trenitalia");
```

---

## What `nxpp` is and what it is not

`nxpp` is best understood as:

- a **BGL-backed convenience layer**
- a **partial NetworkX-inspired API**, not full parity
- a library that prefers **materialized C++ containers** over Python-style lazy iterators
- a project that also includes a few **quality-of-life utilities** not meant as strict NetworkX or BGL ports

It is **not**:

- a full drop-in replacement for NetworkX
- a complete abstraction over all BGL concepts
- a zero-cost wrapper in every public function

Several public calls do extra work beyond the underlying algorithm itself:

- user-ID ↔ descriptor translation
- node/edge attribute lookups through `std::any`
- result materialization into `std::vector`, `std::unordered_map`, `std::map`, or custom wrappers
- cleanup/reindex work after destructive operations such as `remove_node()`

For this reason, the complexity notes below describe the cost of the **public `nxpp` function call**, not only the theoretical complexity of the wrapped graph algorithm.

---

## Current scope

The current public surface covers four main areas:

1. **Graph construction and mutation**
2. **Proxy-style node/edge attribute access**
3. **Common graph algorithms wrapped over BGL**
4. **Utility helpers returning easier-to-consume C++ results**

Highlights currently present in `include/nxpp.hpp` include:

- graph types: simple graphs, digraphs, multigraphs
- core methods: add/remove/clear, neighbors, successors, predecessors
- attribute helpers: checked getters and optional-return variants
- traversals: BFS / DFS edges, trees, visitor-style entry points
- shortest paths: unweighted, Dijkstra, Bellman-Ford, DAG shortest paths, Floyd-Warshall
- components: connected and strongly connected components, plus component maps
- ordering / spanning: topological sort, Kruskal, Prim
- flows: max flow, min cut, min-cost max-flow variants
- generators: complete graph, path graph, Erdős–Rényi graph
- extras: degree centrality, 2-SAT helper, SCC root helper

`include/nxpp.hpp` remains the canonical public header.
If you want the same API split into themed headers, run:

```bash
python3 scripts/sync_modular_headers.py
```

That command regenerates `include_modular/` from `include/nxpp.hpp`, so the split headers stay aligned with the single source of truth under `include/`.

Current status, aligned with `TODO.md`, `ROADMAP.md`, and `CHANGELOG.md`:

- snippet-backed algorithm coverage in the header is largely in place
- the shell harness currently checks `2sat`, `bellman_ford`, `bfs`, and `cc`
- the current manual snippet review pass has reached `floyd_warshall`
- the next snippet-harness expansion item is still `dag_sp`, followed by the other remaining reviewed folders
- the next manual snippet target is `graph_example`
- `betweenness_centrality`, `pagerank`, and a benchmarking harness are still not implemented
- repository hygiene work such as `LICENSE`, `CMakeLists.txt`, and CI is still open

---

## Build and requirements

`nxpp` is header-only, but it depends on **Boost Graph Library**.

### Minimal local build

```bash
g++ -std=c++20 -Wall -pedantic -O3 main.cpp -o main
```

### Install Boost Graph Library

**Ubuntu / Debian**

```bash
sudo apt-get install libboost-graph-dev
```

**macOS (Homebrew)**

```bash
brew install boost
```

**Windows (vcpkg)**

```bash
vcpkg install boost-graph
```

The header performs a compile-time include check and fails early if BGL headers are missing.

---

## Quick start

```cpp
#include "include/nxpp.hpp"
#include <string>

int main() {
    auto G = nxpp::DiGraph();

    G.add_edge("Milan", "Rome", 5.0);
    G.add_edge("Rome", "Naples", 2.5);

    G.node("Rome")["population"] = 2800000;
    G["Rome"]["Naples"]["company"] = std::string("Trenitalia");

    auto path = nxpp::dijkstra_path(G, std::string("Milan"), std::string("Naples"));
    auto preds = G.predecessors("Naples");
    auto company = G.get_edge_attr<std::string>("Rome", "Naples", "company");

    return 0;
}
```

---

## Internal model in one minute

`nxpp` keeps two synchronized structures:

- `id_to_bgl`: `NodeID -> vertex_descriptor`
- `bgl_to_id`: `vertex_descriptor -> NodeID`

This gives the library a user-facing API based on `std::string`, `int`, or other hashable node IDs while still running algorithms on a normal BGL graph internally.

### Important consequence

The current backend uses `boost::adjacency_list` with `boost::vecS` vertex storage.
That choice is fast and convenient for many operations, but it has a crucial side effect:

> when a vertex is removed, later vertex descriptors shift.

So `remove_node()` is not a trivial wrapper. In `nxpp`, it also performs:

- removal of incident edge metadata
- node property cleanup
- `bgl_to_id` erase/shift
- full rebuild of the `id_to_bgl` map for shifted vertices

This is one of the biggest reasons why the complexity of a public `nxpp` function can differ from the “pure algorithm” you might expect from a textbook description.

---

## Complexity model used in this README

The tables below use these conventions:

- `V` = number of vertices
- `E` = number of edges
- `deg(u)` = degree / out-degree relevant to `u`
- unordered-map lookups are treated as **average-case O(1)**
- vector growth is treated as **amortized O(1)** per push
- materializing a returned container is counted in the complexity
- destructive cleanup work done by `nxpp` is counted

These are **practical complexity notes**, not formal proofs. Some BGL primitives depend on storage selectors and can vary in constant factors.

---

## Public graph types

| Type | Meaning |
|---|---|
| `nxpp::Graph<NodeID, EdgeWeight, false, false>` | Undirected simple graph |
| `nxpp::Graph<NodeID, EdgeWeight, true, false>` | Directed simple graph |
| `nxpp::Graph<NodeID, EdgeWeight, false, true>` | Undirected multigraph |
| `nxpp::Graph<NodeID, EdgeWeight, true, true>` | Directed multigraph |

### Common aliases

| Alias | Expands to |
|---|---|
| `WeightedGraphInt` | `Graph<int>` |
| `WeightedGraphStr` | `Graph<std::string>` |
| `WeightedDiGraphInt` | `Graph<int, double, true>` |
| `WeightedDiGraph` | `Graph<std::string, double, true>` |
| `WeightedMultiGraphInt` | `Graph<int, double, false, true>` |
| `WeightedMultiDiGraphInt` | `Graph<int, double, true, true>` |
| `WeightedMultiGraph` | `Graph<std::string, double, false, true>` |
| `WeightedMultiDiGraph` | `Graph<std::string, double, true, true>` |
| `GraphInt` | `Graph<int>` |
| `GraphStr` | `Graph<std::string>` |
| `DiGraphInt` | `Graph<int, double, true>` |
| `DiGraph` | `Graph<std::string, double, true>` |
| `MultiGraphInt` | `Graph<int, double, false, true>` |
| `MultiDiGraphInt` | `Graph<int, double, true, true>` |
| `MultiGraph` | `Graph<std::string, double, false, true>` |
| `MultiDiGraph` | `Graph<std::string, double, true, true>` |

The `Weighted*` aliases are the preferred explicit names for graphs with built-in edge weights.
The shorter aliases such as `GraphInt` and `DiGraphInt` are kept as backward-compatible synonyms.

---

## Core graph API reference

### Construction, mutation, inspection

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `Graph()` | — | graph object | `O(1)` | Creates an empty graph wrapper and initializes internal property maps. | `nxpp::GraphInt G;` |
| `add_node` | `(const NodeID& id)` | `void` | avg `O(1)` | Inserts the node if absent and updates the ID/descriptor translation layer. | `G.add_node(42);` |
| `add_nodes_from` | `(const std::vector<NodeID>& nodes)` | `void` | avg `O(k)` | Inserts `k` nodes by repeatedly calling `add_node`. | `G.add_nodes_from({1,2,3});` |
| `has_node` | `(const NodeID& u)` | `bool` | avg `O(1)` | Checks if a node ID is present in the graph. | `if (G.has_node(1)) {}` |
| `add_edge` | `(u, v, w = 1.0)` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` | Creates missing endpoints automatically. In simple graphs it overwrites the existing edge weight if `(u,v)` already exists. In multigraphs it allows parallel edges. | `G.add_edge(1, 2, 3.5);` |
| `add_edge` | `(u, v, {"key", value})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute insert | Creates an edge with default built-in weight `1.0` and applies one edge attribute during insertion. | `G.add_edge(0, 1, {"capacity", 5L});` |
| `add_edge` | `(u, v, {{"key", value}, ...})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute inserts | Creates an edge with default built-in weight `1.0` and applies edge attributes during insertion. | `G.add_edge(0, 1, {{"capacity", 5L}});` |
| `add_edge` | `(u, v, w, {"key", value})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute insert | Creates or updates an edge weight and applies one edge attribute during insertion. | `G.add_edge(0, 1, 3.5, {"capacity", 5L});` |
| `add_edge` | `(u, v, w, {{"key", value}, ...})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute inserts | Creates or updates an edge weight and applies edge attributes during insertion. | `G.add_edge(0, 1, 3.5, {{"capacity", 5L}});` |
| `add_edges_from` | `vector<tuple<u,v,w>>` | `void` | sum of `add_edge` costs | Bulk edge insertion with explicit weights. | `G.add_edges_from({{1,2,2.0},{2,3,4.0}});` |
| `add_edges_from` | `vector<pair<u,v>>` | `void` | sum of `add_edge` costs | Bulk edge insertion with default weight `1.0`. | `G.add_edges_from({{1,2},{2,3}});` |
| `has_edge` | `(u, v)` | `bool` | avg `O(1 + deg(u))` | Checks whether an edge between `u` and `v` exists. | `if (G.has_edge("A","B")) {}` |
| `get_edge_weight` | `(u, v)` | `EdgeWeight` | avg `O(1 + deg(u))` | Returns the built-in edge weight stored in the BGL edge property. | `auto w = G.get_edge_weight(1,2);` |
| `nodes` | `()` | `std::vector<NodeID>` | `O(V)` | Materializes all node IDs into a vector. | `auto ns = G.nodes();` |
| `edges` | `()` | `std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>` | `O(E)` | Materializes all edges into a vector of `(u,v,weight)`. | `auto es = G.edges();` |
| `neighbors` | `(u)` | `std::vector<NodeID>` | `O(deg(u))` | Returns the out-neighbors of `u`; in directed graphs this matches successor semantics. | `auto ns = G.neighbors("A");` |
| `successors` | `(u)` | `std::vector<NodeID>` | `O(out_deg(u))` | Explicit directed-style successor helper. For undirected graphs it behaves like `neighbors`. | `auto succ = G.successors("A");` |
| `predecessors` | `(u)` | `std::vector<NodeID>` | directed: `O(in_deg(u))`; undirected: `O(deg(u))` | Returns predecessor IDs in directed graphs. | `auto pred = G.predecessors("B");` |
| `remove_edge` | `(u, v)` | `void` | approx `O(1 + deg(u))` plus underlying edge removal | Removes the edge and clears associated edge metadata tracked by `nxpp`. | `G.remove_edge(1,2);` |
| `remove_node` | `(u)` | `void` | **`O(V + E)`** in practice | Removes the node, clears incident metadata, erases the vertex from the `vecS` graph, then rebuilds shifted descriptor mappings. | `G.remove_node("Rome");` |
| `clear` | `()` | `void` | `O(V + E)` | Resets graph structure, translation maps, node attributes, edge attributes, and edge-ID state. | `G.clear();` |
| `node` | `(u)` | `NodeAttrBaseProxy` | avg `O(1)` | Returns a proxy for `G.node(u)["key"]` attribute access; creates the node if absent. | `G.node("A")["x"] = 7;` |
| `operator[]` | `(u)` | `NodeProxy` | avg `O(1)` | Returns a proxy for chained access such as `G[u][v]` and `G[u][v]["key"]`; creates `u` if absent. | `G["A"]["B"] = 2.0;` |

---

## Attribute API reference

Node and edge attributes are stored separately from the BGL graph using `std::any`.

### Checked helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `has_node_attr` | `(u, key)` | `bool` | avg `O(1)` | Checks whether node `u` has attribute `key`. | `G.has_node_attr("A", "color")` |
| `has_edge_attr` | `(u, v, key)` | `bool` | avg `O(1 + deg(u))` | Checks whether edge `(u,v)` has attribute `key`. | `G.has_edge_attr("A","B","capacity")` |
| `get_node_attr<T>` | `(u, key)` | `T` | avg `O(1)` | Returns node attribute `key` with checked `std::any_cast`. Throws if missing or wrong type. | `int pop = G.get_node_attr<int>("Rome", "population");` |
| `get_edge_attr<T>` | `(u, v, key)` | `T` | avg `O(1 + deg(u))` | Returns edge attribute `key` with checked `std::any_cast`. Throws if missing or wrong type. | `auto c = G.get_edge_attr<std::string>("A","B","company");` |
| `try_get_node_attr<T>` | `(u, key)` | `std::optional<T>` | avg `O(1)` | Safer optional-return node attribute lookup. | `auto x = G.try_get_node_attr<int>(1, "rank");` |
| `try_get_edge_attr<T>` | `(u, v, key)` | `std::optional<T>` | avg `O(1 + deg(u))` | Safer optional-return edge attribute lookup. | `auto cap = G.try_get_edge_attr<long>(0,1,"capacity");` |

### Proxy syntax

| Syntax | Meaning |
|---|---|
| `G[u][v] = w;` | set built-in edge weight |
| `auto w = (double)G[u][v];` | read built-in edge weight through proxy conversion |
| `G[u][v]["key"] = value;` | set an edge attribute |
| `auto x = (T)G[u][v]["key"];` | read an edge attribute through proxy conversion |
| `G.node(u)["key"] = value;` | set a node attribute |
| `auto x = (T)G.node(u)["key"];` | read a node attribute through proxy conversion |

### Recommendation

Proxy syntax is convenient for write-heavy code and demos.
For production-style reads, prefer:

- `has_*_attr`
- `get_*_attr<T>`
- `try_get_*_attr<T>`

because they make missing keys and type expectations much clearer.

---

## Traversal API reference

### Edge/tree style helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `bfs_edges` | `(G, start)` | `std::vector<std::pair<NodeID, NodeID>>` | `O(V + E)` | Runs BFS and returns discovered tree edges as `(u,v)` pairs. | `auto es = nxpp::bfs_edges(G, 0);` |
| `bfs_tree` | `(G, start)` | `Graph<NodeID, double, Directed>` | `O(V + E)` | Builds a new graph containing the BFS tree rooted at `start`. | `auto T = nxpp::bfs_tree(G, 0);` |
| `dfs_edges` | `(G, start)` | `std::vector<std::pair<NodeID, NodeID>>` | `O(V + E)` | Runs DFS and returns DFS tree edges. | `auto es = nxpp::dfs_edges(G, 0);` |
| `dfs_tree` | `(G, start)` | `Graph<NodeID, double, Directed>` | `O(V + E)` | Builds a new graph containing the DFS tree rooted at `start`. | `auto T = nxpp::dfs_tree(G, 0);` |
| `bfs_successors` | `(G, start)` | `std::unordered_map<NodeID, std::vector<NodeID>>` | `O(V + E)` | Groups BFS tree edges by parent node. | `auto s = nxpp::bfs_successors(G, 0);` |
| `dfs_predecessors` | `(G, start)` | `std::unordered_map<NodeID, NodeID>` | `O(V + E)` | Returns DFS predecessor map. | `auto p = nxpp::dfs_predecessors(G, 0);` |
| `dfs_successors` | `(G, start)` | `std::unordered_map<NodeID, std::vector<NodeID>>` | `O(V + E)` | Groups DFS tree edges by parent node. | `auto s = nxpp::dfs_successors(G, 0);` |

### Visitor-style helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `breadth_first_search` | `(G, start, visitor)` | `void` | `O(V + E)` | Visitor-object BFS entry point closer to a BGL usage style while still exposing user-facing node IDs. | `nxpp::breadth_first_search(G, 0, vis);` |
| `depth_first_search` | `(G, start, visitor)` | `void` | `O(V + E)` | Visitor-object DFS entry point closer to a BGL usage style. | `nxpp::depth_first_search(G, 0, vis);` |
| `bfs_visit` | `(G, start, on_vertex, on_tree_edge)` | `void` | `O(V + E)` | Convenience callback adapter around visitor-style BFS. | `nxpp::bfs_visit(G, 0, on_v, on_e);` |
| `dfs_visit` | `(G, start, on_tree_edge, on_back_edge)` | `void` | `O(V + E)` | Convenience callback adapter around visitor-style DFS. | `nxpp::dfs_visit(G, 0, on_t, on_b);` |
| `visitor` | base class | visitor base | — | Minimal no-op visitor base exposing hooks such as `examine_vertex`, `tree_edge`, and `back_edge`. | `struct V : nxpp::visitor {};` |

---

## Shortest-path API reference

### Source-target path helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `shortest_path` | `(G, source, target)` | `std::vector<NodeID>` | `O(V + E)` | Unweighted shortest path by edge count. | `auto p = nxpp::shortest_path(G, 0, 3);` |
| `shortest_path_length` | `(G, source, target)` | `double` | `O(V + E)` | Unweighted shortest-path distance in edge count. | `auto d = nxpp::shortest_path_length(G, 0, 3);` |
| `shortest_path` | `(G, source, target, "weight")` | `std::vector<NodeID>` | `O((V + E) log V)` | Weighted shortest path via Dijkstra using the built-in edge weight. | `auto p = nxpp::shortest_path(G, 0, 3, "weight");` |
| `shortest_path_length` | `(G, source, target, "weight")` | `double` | `O((V + E) log V)` | Weighted shortest-path distance using the built-in edge weight. | `auto d = nxpp::shortest_path_length(G, 0, 3, "weight");` |
| `dijkstra_path` | `(G, source, target)` | `std::vector<NodeID>` | `O((V + E) log V)` | Direct Dijkstra path wrapper. | `auto p = nxpp::dijkstra_path(G, 0, 3);` |
| `dijkstra_path_length` | `(G, source, target)` | `double` | `O((V + E) log V)` | Direct Dijkstra distance wrapper. | `auto d = nxpp::dijkstra_path_length(G, 0, 3);` |
| `bellman_ford_path` | `(G, source, target)` | `std::vector<NodeID>` | `O(VE)` | Bellman-Ford path wrapper. Throws on negative cycle. | `auto p = nxpp::bellman_ford_path(G, 0, 3);` |
| `bellman_ford_path_length` | `(G, source, target)` | `double` | `O(VE + L)` | Bellman-Ford distance wrapper; `L` is reconstructed path length for the final accumulation pass. | `auto d = nxpp::bellman_ford_path_length(G, 0, 3);` |

### Single-source helpers returning richer results

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `single_source_dijkstra` | `(G, source)` | `SingleSourceShortestPathResult<NodeID>` | `O((V + E) log V + V)` | Returns distances, predecessors, and fully reconstructed paths. | `auto r = nxpp::single_source_dijkstra(G, 0);` |
| `bellman_ford_shortest_paths` | `(G, source)` | `SingleSourceShortestPathResult<NodeID>` | `O(VE + V + total_path_materialization)` | Bellman-Ford wrapper returning distances, predecessors, and materialized paths. | `auto r = nxpp::bellman_ford_shortest_paths(G, 0);` |
| `dag_shortest_paths` | `(G, source)` | `std::unordered_map<NodeID, double>` | `O(V + E)` | DAG shortest-path distances. | `auto d = nxpp::dag_shortest_paths(G, 0);` |
| `single_source_dag_shortest_paths` | `(G, source)` | `SingleSourceShortestPathResult<NodeID>` | `O(V + E + total_path_materialization)` | DAG shortest-path helper returning distances, predecessors, and paths. | `auto r = nxpp::single_source_dag_shortest_paths(G, 0);` |
| `floyd_warshall_all_pairs_shortest_paths` | `(G)` | `std::vector<std::vector<double>>` | `O(V^3)` | All-pairs shortest-path distance matrix in internal vertex order. For integer snippet-style graphs this lines up with node IDs directly. | `auto fw = nxpp::floyd_warshall_all_pairs_shortest_paths(G);` |
| `floyd_warshall_all_pairs_shortest_paths_map` | `(G)` | `unordered_map<NodeID, unordered_map<NodeID, double>>` | `O(V^3 + V^2)` | Convenience wrapper converting the Floyd-Warshall matrix into NodeID-keyed maps. | `auto fw = nxpp::floyd_warshall_all_pairs_shortest_paths_map(G);` |

---

## Components, ordering, spanning, flows, and extras

### Components and ordering

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `connected_components` | `(G)` | `lookup_map<NodeID, int>` | `O(V + E)` | Boost-like wrapper returning `node -> component_id`. | `auto m = nxpp::connected_components(G);` |
| `connected_component_groups` | `(G)` | `std::vector<std::vector<NodeID>>` | `O(V + E)` | Convenience helper that groups vertices by connected component. | `auto cc = nxpp::connected_component_groups(G);` |
| `strong_components` | `(G)` | `lookup_map<NodeID, int>` | `O(V + E)` | Boost-like wrapper returning `node -> SCC id`. | `auto m = nxpp::strong_components(G);` |
| `strongly_connected_component_groups` | `(G)` | `std::vector<std::vector<NodeID>>` | `O(V + E)` | Convenience helper that groups vertices by SCC. | `auto scc = nxpp::strongly_connected_component_groups(G);` |
| `strong_component_roots` | `(G)` | `std::unordered_map<NodeID, NodeID>` | `O(V + E)` | Convenience helper returning a representative/root node for each vertex's SCC. | `auto r = nxpp::strong_component_roots(G);` |
| `topological_sort` | `(G)` | `std::vector<NodeID>` | `O(V + E)` | Returns a topological ordering. | `auto order = nxpp::topological_sort(G);` |

### Spanning structures

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `minimum_spanning_tree` | `(G)` | `std::vector<std::pair<NodeID, NodeID>>` | typically `O(E log E)` | Default MST wrapper; currently delegates to `kruskal_minimum_spanning_tree`. | `auto mst = nxpp::minimum_spanning_tree(G);` |
| `minimum_spanning_tree` | `(G, root)` | `std::unordered_map<NodeID, NodeID>` | typically `O((V + E) log V)` | Rooted overload that delegates to `prim_minimum_spanning_tree`. | `auto p = nxpp::minimum_spanning_tree(G, 0);` |
| `kruskal_minimum_spanning_tree` | `(G)` | `std::vector<std::pair<NodeID, NodeID>>` | typically `O(E log E)` | Returns MST edges as `(u,v)` pairs. | `auto mst = nxpp::kruskal_minimum_spanning_tree(G);` |
| `prim_minimum_spanning_tree` | `(G, root)` | `std::unordered_map<NodeID, NodeID>` | typically `O((V + E) log V)` | Returns a `node -> parent` map for the Prim tree rooted at `root`. | `auto p = nxpp::prim_minimum_spanning_tree(G, 0);` |

### Flow and cut helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `maximum_flow` | `(G, source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | build wrapper graph `O(V + E)` + Edmonds-Karp cost | Backward-compatible default max-flow wrapper; currently delegates to `edmonds_karp_maximum_flow`. | `auto f = nxpp::maximum_flow(G, 0, 5);` |
| `edmonds_karp_maximum_flow` | `(G, source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | build wrapper graph `O(V + E)` + Edmonds-Karp cost | Max-flow wrapper using Boost Edmonds-Karp. | `auto f = nxpp::edmonds_karp_maximum_flow(G, 0, 5);` |
| `push_relabel_maximum_flow` | `(G, source, sink, capacity_attr = "capacity")` | `long` | build wrapper graph `O(V + E)` + Push-Relabel cost | Push-Relabel max-flow value. When used before `cycle_canceling(G)`, nxpp keeps the internal staged state needed for the next step. | `long f = nxpp::push_relabel_maximum_flow(G, 0, 5);` |
| `push_relabel_maximum_flow_result` | `(G, source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | build wrapper graph `O(V + E)` + Push-Relabel cost | Push-Relabel wrapper returning both the total value and the per-edge flow map. | `auto f = nxpp::push_relabel_maximum_flow_result(G, 0, 5);` |
| `minimum_cut` | `(G, source, sink, capacity_attr = "capacity")` | `MinimumCutResult<NodeID>` | build wrapper graph `O(V + E)` + max-flow cost + residual BFS `O(V + E)` | Returns cut value, reachable/non-reachable partition, and crossing edges. | `auto c = nxpp::minimum_cut(G, 0, 5);` |
| `max_flow_min_cost` | `(G, source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build wrapper graph `O(V + E)` + max-flow / cycle-canceling cost | Default min-cost max-flow wrapper; currently delegates to `max_flow_min_cost_cycle_canceling`. | `auto r = nxpp::max_flow_min_cost(G, 0, 5);` |
| `max_flow_min_cost_cycle_canceling` | `(G, source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build wrapper graph `O(V + E)` + max-flow / cycle-canceling cost | Returns total flow in `result.flow`, flow cost in `result.cost`, and per-edge flows in `result.edge_flows`. | `auto r = nxpp::max_flow_min_cost_cycle_canceling(G, 0, 5);` |
| `max_flow_min_cost_successive_shortest_path` | `(G, source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build wrapper graph `O(V + E)` + SSP min-cost-flow cost | Same result shape using successive shortest path. | `auto r = nxpp::max_flow_min_cost_successive_shortest_path(G, 0, 5);` |
| `cycle_canceling` | `(G, weight_attr = "weight")` | deduced cost type | uses internal staged state from `push_relabel_maximum_flow` + cycle-canceling cost | Runs cycle-canceling over the internally cached staged flow graph prepared by `push_relabel_maximum_flow`. | `long c = nxpp::cycle_canceling(G);` |

Native-style staged helpers for the cycle-canceling path are also available while keeping the state internal:

- `long flow = nxpp::push_relabel_maximum_flow(G, source, sink, "capacity");`
- `long cost = nxpp::cycle_canceling(G);`

### Generators, centrality, SAT

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `complete_graph` | `(n)` | `GraphType` | directed current implementation: `O(n^2)`; undirected current implementation: `O(n^2)` edge insertion attempts | Generates a complete graph using the chosen graph type template. | `auto K5 = nxpp::complete_graph(5);` |
| `path_graph` | `(n)` | `GraphType` | `O(n)` | Generates a path graph `0-1-...-(n-1)` (or directed path if `GraphType` is directed). | `auto P4 = nxpp::path_graph(4);` |
| `erdos_renyi_graph` | `(n, p, seed = 42)` | `GraphType` | `O(n^2)` | Generates an Erdős–Rényi random graph. | `auto G = nxpp::erdos_renyi_graph(100, 0.05);` |
| `degree_centrality` | `(G)` | `std::unordered_map<NodeID, double>` | `O(V + E)` | Returns degree centrality with NetworkX-like normalization by `n - 1`. | `auto c = nxpp::degree_centrality(G);` |
| `two_sat_satisfiable` | `(num_variables, clauses)` | `bool` | `O(V + E)` on the implication graph | Convenience 2-SAT satisfiability helper built on SCC computation. | `bool ok = nxpp::two_sat_satisfiable(2, {{1,2},{-1,2}});` |

---

## `nxpp` utilities that are not just straight NetworkX/BGL ports

One of the useful directions of the project is that it already contains some helpers whose purpose is not strict parity, but **better ergonomics in C++**.

### Notable examples

| Utility | Why it is useful |
|---|---|
| `successors()` / `predecessors()` | Makes directed traversal intent explicit, even if `neighbors()` already exists. |
| `has_*_attr`, `get_*_attr<T>`, `try_get_*_attr<T>` | Safer than relying only on proxy conversions backed by `std::any`. |
| `connected_component_groups()` / `strongly_connected_component_groups()` | Often easier to consume in C++ than flat component-id maps. |
| `single_source_dijkstra()` / `bellman_ford_shortest_paths()` / `single_source_dag_shortest_paths()` | Return distances, predecessors, and full paths together instead of making users rebuild them manually. |
| `MaximumFlowResult`, `MinimumCutResult`, `MinCostMaxFlowResult` | Wrap raw algorithm results into structures that are easier to inspect and use directly. |
| `visitor` | Gives a project-owned visitor base instead of forcing raw BGL visitor plumbing on every example. |
| `lookup_map<Key, Value>` | Keeps a convenient `result[key]` style even in const-read contexts for some mapped results. |

This is a good direction for the library: use BGL internally when it makes sense, but expose **results that are easier to work with in real C++ code**.

---

## Differences from NetworkX and from raw BGL

### Compared with NetworkX

- `nxpp` is **statically typed**: node ID type is fixed by the graph type.
- returned values are usually **materialized containers**, not lazy iterators
- weighted path dispatch is currently intentionally narrow: the weighted form is tied to the built-in `weight` edge property
- node and edge attributes are stored with `std::any`, so reads are runtime-checked rather than dynamically duck-typed
- some helper APIs intentionally go beyond NetworkX because they are more convenient from C++

### Compared with raw BGL

- `nxpp` hides descriptors behind user-facing node IDs
- `nxpp` adds attribute storage outside the base BGL graph
- `nxpp` returns higher-level containers rather than forcing users to wire property maps manually each time
- destructive operations pay extra bookkeeping cost because `nxpp` must keep its translation structures valid

---

## Current caveats and design trade-offs

The current design is already useful, but a good README should be explicit about the trade-offs.

### 1. Multigraph semantics are currently limited

For simple graphs, APIs such as `get_edge_weight(u, v)`, `get_edge_attr(u, v, key)`, `G[u][v]`, and `remove_edge(u, v)` are straightforward.
For multigraphs, the current implementation is more limited and should be treated carefully:

- `has_edge(u, v)` means "there exists at least one parallel edge between `u` and `v`"
- `get_edge_weight(u, v)`, `get_edge_attr(u, v, key)`, and proxy reads/writes such as `G[u][v]["key"]` operate on one matching `(u, v)` edge, not on a stable public edge identity
- `remove_edge(u, v)` removes the `(u, v)` connection at the BGL level and is not yet documented as a precise "remove one specific parallel edge" API
- because of that ambiguity, multigraph edge reads and destructive operations should currently be treated as a known caveat rather than a polished parity surface

This is the highest-risk part of the public API today and is explicitly tracked in `TODO.md` for redesign/documentation follow-up.

### 2. `remove_node()` is intentionally not cheap

Because of the `vecS` backend and the ID/descriptor mapping, `remove_node()` should be treated as an **O(V + E)** operation in practice, not as a cheap local edit.
That is fine, but it must be clearly documented.

### 3. `std::any` is ergonomic, but not free

The attribute system is flexible and Python-like, but it comes with trade-offs:

- runtime casts
- runtime failures on mismatched types
- more fragile large-scale usage if attribute keys/types are not disciplined

### 4. The single-header approach keeps usage easy, but increases maintenance pressure

A single `include/nxpp.hpp` is convenient for users and examples.
But as the project grows, it also means:

- larger compile units
- harder navigation
- higher risk of documentation drift if status files and README are not updated together

---

## Snippets and local verification

The repository also includes a snippet-based workflow and a small harness script:

- `snippet/` contains local reference examples
- each algorithm folder now includes Boost-style C++, NetworkX Python, and `nxpp` C++ counterparts
- `scripts/test_snippet_batch.sh` currently compiles/runs `2sat`, `bellman_ford`, `bfs`, and `cc`, logging timings and diff-based parity results
- `scripts/test_single_snippet.sh <folder>` runs one snippet triplet at a time, compiling the two C++ variants, running the Python version, saving all logs, and producing all three pairwise diffs; if the file basename differs from the folder name, the script auto-detects it
- snippet harness coverage for `dag_sp`, `flow`, `floyd_warshall`, `kruskal`, `prim`, `scc`, `scc_named`, `ts`, and min-cost max-flow is still open
- the project markdown files (`TODO.md`, `ROADMAP.md`, `CHANGELOG.md`, `SESSION.md`) are useful to track scope, status, and cleanup direction

Example single-snippet runs:

```bash
scripts/test_single_snippet.sh bfs
scripts/test_single_snippet.sh 2sat
scripts/test_single_snippet.sh 2sat /path/to/input.txt
```

Each run writes artifacts under `logs/snippet/<folder>_<timestamp>/`, including:

- the combined run log
- dedicated compile stderr logs for the Boost and `nxpp` C++ builds
- captured `stdout` / `stderr` for all three implementations
- `cpp vs py`, `cpp vs nxpp`, and `py vs nxpp` diff files

That is a healthy sign for the project: there is a visible effort not just to write wrappers, but to compare behavior, keep examples aligned, and make regressions easier to notice.

---

## Status snapshot

The project currently looks strongest as:

- a header-only C++20 graph utility layer with a practical subset of NetworkX-style ergonomics
- a snippet-driven algorithm wrapper project whose implemented surface is broader than the automated regression harness
- a codebase where the biggest remaining correctness/documentation risk is multigraph edge identity semantics

If you use `nxpp` today, the safest mental model is:

- simple graphs and the snippet-backed algorithm family are the best-covered parts
- richer path/flow/component helpers are available and useful
- multigraph mutation/lookup semantics still need a more explicit, stable public contract
