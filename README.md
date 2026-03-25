# nxpp — NetworkX-inspired graph utilities for modern C++

`nxpp` is a **header-only C++20** library built on top of the **Boost Graph Library (BGL)**.
Its goal is not to clone all of NetworkX, but to offer a **small, practical, NetworkX-inspired API** that is easier to use directly from C++ than raw BGL in many common cases.

The project centers on a single generic graph wrapper:

```cpp
nxpp::Graph<NodeID, EdgeWeight, Directed, Multi>
```

with public aliases such as:

- `nxpp::GraphInt`
- `nxpp::GraphStr`
- `nxpp::DiGraph`
- `nxpp::DiGraphInt`
- `nxpp::MultiGraph`
- `nxpp::MultiDiGraph`

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
| `GraphInt` | `Graph<int>` |
| `GraphStr` | `Graph<std::string>` |
| `DiGraphInt` | `Graph<int, double, true>` |
| `DiGraph` | `Graph<std::string, double, true>` |
| `MultiGraphInt` | `Graph<int, double, false, true>` |
| `MultiDiGraphInt` | `Graph<int, double, true, true>` |
| `MultiGraph` | `Graph<std::string, double, false, true>` |
| `MultiDiGraph` | `Graph<std::string, double, true, true>` |

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
| `default_graph_visitor<GraphWrapper>` | template base class | visitor base | — | No-op base visitor exposing hooks such as `examine_vertex`, `tree_edge`, and `back_edge`. | `struct V : nxpp::default_graph_visitor<nxpp::GraphInt> {};` |

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
| `single_source_bellman_ford` | `(G, source)` | `SingleSourceShortestPathResult<NodeID>` | `O(VE + V + total_path_materialization)` | Bellman-Ford variant returning the richer result bundle. | `auto r = nxpp::single_source_bellman_ford(G, 0);` |
| `dag_shortest_paths` | `(G, source)` | `std::unordered_map<NodeID, double>` | `O(V + E)` | DAG shortest-path distances. | `auto d = nxpp::dag_shortest_paths(G, 0);` |
| `single_source_dag_shortest_paths` | `(G, source)` | `SingleSourceShortestPathResult<NodeID>` | `O(V + E + total_path_materialization)` | DAG shortest-path helper returning distances, predecessors, and paths. | `auto r = nxpp::single_source_dag_shortest_paths(G, 0);` |
| `floyd_warshall_all_pairs_shortest_paths` | `(G)` | `unordered_map<NodeID, unordered_map<NodeID, double>>` | `O(V^3 + V^2)` | All-pairs shortest paths plus conversion from matrix form to NodeID-keyed maps. | `auto fw = nxpp::floyd_warshall_all_pairs_shortest_paths(G);` |

---

## Components, ordering, spanning, flows, and extras

### Components and ordering

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `connected_components` | `(G)` | `std::vector<std::vector<NodeID>>` | `O(V + E)` | Returns connected components as grouped node vectors. | `auto cc = nxpp::connected_components(G);` |
| `connected_component_map` | `(G)` | `lookup_map<NodeID, int>` | `O(V + E)` | Returns `node -> component_id`, often easier than flattening grouped results. | `auto m = nxpp::connected_component_map(G);` |
| `strongly_connected_components` | `(G)` | `std::vector<std::vector<NodeID>>` | `O(V + E)` | Returns strongly connected components as grouped node vectors. | `auto scc = nxpp::strongly_connected_components(G);` |
| `strongly_connected_component_map` | `(G)` | `lookup_map<NodeID, int>` | `O(V + E)` | Returns `node -> SCC id`. | `auto m = nxpp::strongly_connected_component_map(G);` |
| `strongly_connected_component_roots` | `(G)` | `std::unordered_map<NodeID, NodeID>` | `O(V + E)` | Returns a representative/root node for each vertex's SCC. | `auto r = nxpp::strongly_connected_component_roots(G);` |
| `topological_sort` | `(G)` | `std::vector<NodeID>` | `O(V + E)` | Returns a topological ordering. | `auto order = nxpp::topological_sort(G);` |

### Spanning structures

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `kruskal_minimum_spanning_tree` | `(G)` | `std::vector<std::pair<NodeID, NodeID>>` | typically `O(E log E)` | Returns MST edges as `(u,v)` pairs. | `auto mst = nxpp::kruskal_minimum_spanning_tree(G);` |
| `prim_minimum_spanning_tree` | `(G, root)` | `std::unordered_map<NodeID, NodeID>` | typically `O((V + E) log V)` | Returns a `node -> parent` map for the Prim tree rooted at `root`. | `auto p = nxpp::prim_minimum_spanning_tree(G, 0);` |

### Flow and cut helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `maximum_flow` | `(G, source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | build wrapper graph `O(V + E)` + Edmonds-Karp cost | Builds an internal flow graph, reads capacities from edge attributes, and returns total flow plus per-edge flows. | `auto f = nxpp::maximum_flow(G, 0, 5);` |
| `minimum_cut` | `(G, source, sink, capacity_attr = "capacity")` | `MinimumCutResult<NodeID>` | build wrapper graph `O(V + E)` + max-flow cost + residual BFS `O(V + E)` | Returns cut value, reachable/non-reachable partition, and crossing edges. | `auto c = nxpp::minimum_cut(G, 0, 5);` |
| `max_flow_min_cost_cycle_canceling` | `(G, source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build wrapper graph `O(V + E)` + max-flow / cycle-canceling cost | Returns flow value, flow cost, and per-edge flows. | `auto r = nxpp::max_flow_min_cost_cycle_canceling(G, 0, 5);` |
| `max_flow_min_cost_successive_shortest_path` | `(G, source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build wrapper graph `O(V + E)` + SSP min-cost-flow cost | Same result shape using successive shortest path. | `auto r = nxpp::max_flow_min_cost_successive_shortest_path(G, 0, 5);` |

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
| `connected_component_map()` / `strongly_connected_component_map()` | Often easier to consume in C++ than grouped vectors. |
| `single_source_dijkstra()` / `single_source_bellman_ford()` / `single_source_dag_shortest_paths()` | Return distances, predecessors, and full paths together instead of making users rebuild them manually. |
| `MaximumFlowResult`, `MinimumCutResult`, `MinCostMaxFlowResult` | Wrap raw algorithm results into structures that are easier to inspect and use directly. |
| `default_graph_visitor<GraphWrapper>` | Gives a project-owned visitor base instead of forcing raw BGL visitor plumbing on every example. |
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

### 1. Multigraph semantics need careful documentation

The API shape `get_edge_weight(u, v)`, `get_edge_attr(u, v, key)`, `G[u][v]`, and `remove_edge(u, v)` is naturally clear for simple graphs.
For multigraphs, however, `(u, v)` no longer uniquely identifies one edge.

That means the README should explicitly explain what the intended semantics are for:

- reading the “edge weight” of parallel edges
- reading an attribute from one of many parallel edges
- removing one parallel edge vs all parallel edges

At the moment, this is the area that most deserves precise documentation and possibly future API refinement.

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
- `scripts/test_snippets.sh` compiles/runs selected cases and logs timings
- the project markdown files (`TODO.md`, `ROADMAP.md`, `CHANGELOG.md`, `SESSION.md`) are useful to track scope, status, and cleanup direction

That is a healthy sign for the project: there is a visible effort not just to write wrappers, but to compare behavior, keep examples aligned, and make regressions easier to notice.

---

## Suggested roadmap for documentation quality

A stronger README should keep the following order:

1. one-paragraph description
2. installation and 30-second quick start
3. honest scope statement: “NetworkX-inspired, not full parity”
4. graph types and core API
5. complexity table for **public functions**
6. algorithm and utility reference tables
7. explicit caveats
8. snippet/testing notes

That structure makes the project easier to trust because users can immediately see:

- what exists
- how to compile it
- how expensive public operations are
- where the API intentionally differs from NetworkX or BGL
- which helpers are extra ergonomics rather than strict ports

---

## Short assessment of the codebase beyond the README

Leaving documentation aside, the project has several genuinely good signs.

### Strong points

- The **ID translation layer** is the right architectural move if the goal is “user IDs outside, BGL inside”.
- The switch to **stable internal edge IDs** for edge metadata is a meaningful improvement; it avoids binding user-level attributes to fragile edge descriptors.
- The library is starting to develop **its own useful C++ ergonomics** rather than only copying names from NetworkX.
- The richer result types for paths, flows, and component maps are a good direction.
- The snippet/test-harness mindset is healthy: it suggests the code is being shaped against concrete examples rather than only abstract plans.

### Main risks

- Documentation/status drift is currently too easy.
- Multigraph semantics are the biggest conceptual weak point in the public API.
- The single-header design will become harder to evolve if the surface keeps growing quickly.
- Error handling is still mostly string-based `std::runtime_error`, which is acceptable for now but not very expressive.
