# nxpp — NetworkX-inspired graph utilities for modern C++

<p align="center">
  <img src="imgs/logo.svg" alt="nxpp logo" width="220">
</p>

`nxpp` is a **header-only C++20** library built on top of the **Boost Graph Library (BGL)**.

It does **not** try to reimplement all of NetworkX.  
It aims to provide a **small, practical, C++-friendly graph API** that feels closer to NetworkX in day-to-day use while still delegating most graph work to Boost.

The core abstraction is:

```cpp
nxpp::Graph<NodeID, EdgeWeight, Directed, Multi, Weighted>
```

with public aliases such as:

- `nxpp::WeightedGraphInt`
- `nxpp::WeightedGraphStr`
- `nxpp::WeightedDiGraph`
- `nxpp::WeightedDiGraphInt`
- `nxpp::GraphInt`
- `nxpp::GraphStr`
- `nxpp::DiGraph`
- `nxpp::DiGraphInt`
- `nxpp::MultiGraph`
- `nxpp::MultiDiGraph`
- `nxpp::UnweightedGraphInt`
- `nxpp::UnweightedDiGraphInt`

---

## Project status

> [!WARNING]
> The public API is still settling.
> The biggest open design questions are:
>
> - how much of the API should remain NetworkX-shaped
> - how much Boost configurability should be exposed publicly
> - how multigraph edge identity should be modeled

Today, the project is strongest as:

- a **header-only wrapper over BGL**
- a **NetworkX-inspired convenience layer**
- a library that often returns **materialized C++ results**
- a project with a **snippet-based parity / regression harness**
- a codebase whose main remaining correctness/documentation risk is **multigraph edge semantics**

---

## Read this before using `nxpp`

### 1. Complexity in this README means **public-call complexity**

This README documents the cost of calling an `nxpp` function, not only the underlying graph algorithm.

That matters because `nxpp` often does extra work around the Boost call itself, for example:

- NodeID ↔ descriptor translation
- `std::any`-based attribute lookups
- result materialization into containers
- cleanup of metadata on destructive operations
- descriptor remapping after `remove_node()`

So the complexity tables below describe the cost of the **full `nxpp` function call**.

### 2. Multigraph APIs are still the weakest part of the public surface

For simple graphs, calls such as `has_edge(u, v)`, `get_edge_weight(u, v)`, `get_edge_attr(u, v, key)`, `G[u][v]`, and `remove_edge(u, v)` are straightforward.

For multigraphs, the current implementation still addresses edges through `(u, v)` in several places, which is not a stable public way to identify one specific parallel edge.

Treat multigraph mutation / lookup as a **known caveat**, not as a fully polished API.

### 3. `remove_node()` is intentionally not cheap

The backend uses `boost::adjacency_list` with `boost::vecS` vertex storage.

That makes many operations convenient, but removing a vertex shifts later descriptors.
So `remove_node()` must also:

- clear incident edge metadata
- erase node metadata
- shift internal descriptor-to-ID state
- rebuild `id_to_bgl` mappings

In practice, this is an **`O(V + E)` public operation**.

---

## What `nxpp` is and what it is not

`nxpp` is best understood as:

- a **BGL-backed convenience layer**
- a **partial NetworkX-inspired API**, not full parity
- a library that prefers **materialized C++ containers** over lazy iterator-heavy public APIs
- a place for a few **C++-oriented utility wrappers** that are easier to consume than raw Boost output

It is **not**:

- a drop-in replacement for NetworkX
- a full abstraction over all BGL concepts
- a zero-cost wrapper in every public function
- a fully stabilized graph framework yet

---

## Project documents

These repository files should have clearly separated roles:

- [`README.md`](README.md): user-facing overview, caveats, usage, and API reference
- [`TODO.md`](TODO.md): compact local priority index; GitHub Issues are the full backlog source of truth
- [`CHANGELOG.md`](CHANGELOG.md): dated change history
- [`SESSION.md`](SESSION.md): historical development log
- [`docs/README.md`](docs/README.md): index for longer-form and future generated documentation
- [`docs/API_ARCHITECTURE.md`](docs/API_ARCHITECTURE.md): public API placement policy for graph methods and namespace-scope helpers

If these files disagree, `README.md` should describe the **current user-facing reality**, while `TODO.md` should describe what is still open.

---

## API architecture policy

The intended public shape of `nxpp` is:

- **graph methods** as the primary API for graph ownership, mutation, local lookups, traversals, algorithms, and attribute/proxy access on an existing graph
- **free functions under `nxpp::`** only where namespace-scope factories/generators are the cleaner fit

In other words:

- `G.add_edge(...)`, `G.remove_node(...)`, `G.neighbors(...)`, `G.get_edge_attr<T>(...)`, `G.bfs_edges(...)`, and `G.dijkstra_shortest_paths(...)` belong on the graph object
- calls such as `nxpp::complete_graph(...)`, `nxpp::path_graph(...)`, and `nxpp::erdos_renyi_graph(...)` remain free functions because they construct and return graphs
- old free-function forms for existing-graph operations are deprecated and should not be treated as canonical entry points

See [`docs/API_ARCHITECTURE.md`](docs/API_ARCHITECTURE.md) for the fuller rule and future-addition placement policy.

---

## Current scope

The current public surface covers these areas:

1. **Graph construction and mutation**
2. **Node and edge attributes through proxies and checked accessors**
3. **Traversal wrappers**
4. **Shortest paths**
5. **Connected / strongly connected components**
6. **Topological sort and minimum spanning tree helpers**
7. **Maximum flow, minimum cut, and min-cost flow wrappers**
8. **Graph generators and extra utilities**

The most important implemented functionality currently includes:

- graph aliases for directed / undirected / multi / unweighted variants
- `add_node`, `add_edge`, `remove_edge`, `remove_node`, `clear`
- `neighbors`, `successors`, `predecessors`
- `has_*_attr`, `get_*_attr<T>`, `try_get_*_attr<T>`
- `bfs_edges`, `dfs_edges`, `bfs_tree`, `dfs_tree`
- `breadth_first_search`, `depth_first_search`, `bfs_visit`, `dfs_visit`
- `shortest_path`, `dijkstra_path`, `bellman_ford_path`, `dag_shortest_paths`
- `dijkstra_shortest_paths`, `bellman_ford_shortest_paths`
- `floyd_warshall_all_pairs_shortest_paths`
- `connected_components`, `connected_component_groups`
- `strongly_connected_components`, `strongly_connected_component_map`, `strongly_connected_component_roots`
- `topological_sort`, `kruskal_minimum_spanning_tree`, `prim_minimum_spanning_tree`
- `maximum_flow`, `minimum_cut`, `max_flow_min_cost`, `cycle_canceling`
- `complete_graph`, `path_graph`, `erdos_renyi_graph`
- `degree_centrality`, `two_sat_satisfiable`

`include/nxpp.hpp` is the canonical public header.

---

## Build and requirements

`nxpp` is header-only, but it depends on **Boost Graph Library**.

### Minimal local build

```bash
g++ -std=c++20 -Wall -Wextra -pedantic -O3 main.cpp -o main
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

The old graph-weights / direct-path style example is no longer representative enough.
A more up-to-date "small but real" example is:

```cpp
#include "include/nxpp.hpp"
#include <iostream>
#include <string>

int main() {
    auto G = nxpp::DiGraph();

    G.add_edge("Milan", "Rome", 5.0, {
        {"capacity", 8L},
        {"company", std::string("Trenitalia")}
    });
    G.add_edge("Rome", "Naples", 2.5);
    G.add_edge("Milan", "Florence", 2.0);
    G.add_edge("Florence", "Naples", 4.0);

    G.node("Rome")["population"] = 2800000;

    auto routes = G.dijkstra_shortest_paths(std::string("Milan"));
    auto dist_to_naples = routes.distance.at("Naples");
    auto path_to_naples = routes.paths.at("Naples");

    auto operator_name =
        G.get_edge_attr<std::string>("Milan", "Rome", "company");

    std::cout << "Distance Milan -> Naples: " << dist_to_naples << "\n";
    std::cout << "Rail operator on Milan -> Rome: " << operator_name << "\n";
    std::cout << "Path:";
    for (const auto& node : path_to_naples) {
        std::cout << " " << node;
    }
    std::cout << "\n";

    return 0;
}
```

This shows the three main ideas of the library:

- graph mutation through a small wrapper API
- checked node / edge attribute access
- richer result wrappers such as `dijkstra_shortest_paths()`

---

## Internal model in one minute

`nxpp` keeps two synchronized structures:

- `id_to_bgl`: `NodeID -> vertex_descriptor`
- `bgl_to_id`: `vertex_descriptor -> NodeID`

This lets the public API use `std::string`, `int`, or other hashable node IDs while running algorithms on a normal BGL graph internally.

### Consequence

The current backend uses:

```cpp
boost::adjacency_list<boost::vecS, boost::vecS, ...>
```

That is convenient, but vertex removal shifts later descriptors.

So operations such as `remove_node()` must also repair `nxpp`'s own translation layer.
That is why some public-call costs differ from the textbook complexity of the wrapped algorithm alone.

---

## Complexity conventions used below

The tables use these symbols:

- `V` = number of vertices
- `E` = number of edges
- `deg(u)` = degree / out-degree relevant to `u`
- `k` = number of inserted items in a bulk call

Conventions:

- unordered-map lookups are treated as **average-case `O(1)`**
- vector growth is treated as **amortized `O(1)` per push**
- materializing the returned result container counts toward the complexity
- cleanup / metadata repair done by `nxpp` counts toward the complexity

These are **practical public-call notes**, not formal proofs.

---

## Public graph template and aliases

### Primary template

| Type | Meaning |
|---|---|
| `nxpp::Graph<NodeID, EdgeWeight, false, false>` | Undirected simple weighted graph |
| `nxpp::Graph<NodeID, EdgeWeight, true, false>` | Directed simple weighted graph |
| `nxpp::Graph<NodeID, EdgeWeight, false, true>` | Undirected multigraph |
| `nxpp::Graph<NodeID, EdgeWeight, true, true>` | Directed multigraph |
| `nxpp::Graph<NodeID, EdgeWeight, Directed, Multi, false>` | Unweighted variant |

### Common aliases

| Alias | Expands to |
|---|---|
| `WeightedGraphInt` | `Graph<int, int>` |
| `WeightedGraphStr` | `Graph<std::string>` |
| `WeightedDiGraphInt` | `Graph<int, int, true>` |
| `WeightedDiGraph` | `Graph<std::string, double, true>` |
| `WeightedMultiGraphInt` | `Graph<int, int, false, true>` |
| `WeightedMultiDiGraphInt` | `Graph<int, int, true, true>` |
| `WeightedMultiGraph` | `Graph<std::string, double, false, true>` |
| `WeightedMultiDiGraph` | `Graph<std::string, double, true, true>` |
| `GraphInt` | `Graph<int, int>` |
| `GraphStr` | `Graph<std::string>` |
| `DiGraphInt` | `Graph<int, int, true>` |
| `DiGraph` | `Graph<std::string, double, true>` |
| `MultiGraphInt` | `Graph<int, int, false, true>` |
| `MultiDiGraphInt` | `Graph<int, int, true, true>` |
| `MultiGraph` | `Graph<std::string, double, false, true>` |
| `MultiDiGraph` | `Graph<std::string, double, true, true>` |
| `UnweightedGraphInt` | `Graph<int, double, false, false, false>` |
| `UnweightedDiGraphInt` | `Graph<int, double, true, false, false>` |
| `UnweightedGraphStr` | `Graph<std::string, double, false, false, false>` |
| `UnweightedDiGraph` | `Graph<std::string, double, true, false, false>` |
| `UnweightedMultiGraphInt` | `Graph<int, double, false, true, false>` |
| `UnweightedMultiDiGraphInt` | `Graph<int, double, true, true, false>` |
| `UnweightedMultiGraph` | `Graph<std::string, double, false, true, false>` |
| `UnweightedMultiDiGraph` | `Graph<std::string, double, true, true, false>` |

The `Weighted*` aliases are the clearest explicit names.
The shorter aliases such as `GraphInt` and `DiGraph` are kept as compatibility-friendly synonyms.

---

## Core graph API reference

### Construction, mutation, inspection

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `Graph()` | — | graph object | `O(1)` | Creates an empty wrapper and initializes internal state. | `nxpp::GraphInt G;` |
| `add_node` | `(const NodeID& id)` | `void` | avg `O(1)` | Inserts the node if absent. | `G.add_node(42);` |
| `add_nodes_from` | `(const std::vector<NodeID>& nodes)` | `void` | avg `O(k)` | Inserts `k` nodes by repeated `add_node`. | `G.add_nodes_from({1,2,3});` |
| `has_node` | `(const NodeID& u)` | `bool` | avg `O(1)` | Checks whether a node exists. | `G.has_node(1)` |
| `add_edge` | `(u, v, w = 1.0)` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` | Creates missing endpoints automatically. In simple graphs, a repeated `(u, v)` overwrites weight. | `G.add_edge(1, 2, 3.5);` |
| `add_edge` | `(u, v)` on unweighted graphs | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` | Inserts an unweighted edge. | `UG.add_edge(1, 2);` |
| `add_edge` | `(u, v, {"key", value})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute insert | Adds one edge attribute with default built-in weight where applicable. | `G.add_edge(0, 1, {"capacity", 5L});` |
| `add_edge` | `(u, v, {{"key", value}, ...})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute inserts | Adds multiple edge attributes with default built-in weight where applicable. | `G.add_edge(0, 1, {{"capacity", 5L}});` |
| `add_edge` | `(u, v, w, {"key", value})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute insert | Adds / updates weight and one edge attribute. | `G.add_edge(0, 1, 3.5, {"capacity", 5L});` |
| `add_edge` | `(u, v, w, {{"key", value}, ...})` | `void` | simple graph: avg `O(1 + deg(u))`; multigraph: avg `O(1)` + attribute inserts | Adds / updates weight and multiple edge attributes. | `G.add_edge(0, 1, 3.5, {{"capacity", 5L}});` |
| `add_edges_from` | `vector<tuple<u,v,w>>` | `void` | sum of `add_edge` costs | Bulk weighted insertion. | `G.add_edges_from({{1,2,2},{2,3,4}});` |
| `add_edges_from` | `vector<pair<u,v>>` | `void` | sum of `add_edge` costs | Bulk insertion with default weight or unweighted insertion depending on graph type. | `G.add_edges_from({{1,2},{2,3}});` |
| `has_edge` | `(u, v)` | `bool` | avg `O(1 + deg(u))` | Checks whether an edge exists. In multigraphs, this means "at least one edge exists". | `G.has_edge("A","B")` |
| `get_edge_weight` | `(u, v)` | `EdgeWeight` | avg `O(1 + deg(u))` | Returns the built-in edge weight. | `auto w = G.get_edge_weight(1,2);` |
| `nodes` | `()` | `std::vector<NodeID>` | `O(V)` | Materializes all node IDs. | `auto ns = G.nodes();` |
| `edges` | `()` | weighted graphs: `std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>`; unweighted graphs: `std::vector<std::pair<NodeID, NodeID>>` | `O(E)` | Materializes all edges. | `auto es = G.edges();` |
| `edge_pairs` | `()` | `std::vector<std::pair<NodeID, NodeID>>` | `O(E)` | Materializes edges without weights. Useful for wrappers that rebuild auxiliary graphs. | `auto ep = G.edge_pairs();` |
| `neighbors` | `(u)` | `std::vector<NodeID>` | `O(deg(u))` | Returns out-neighbors. For directed graphs this matches successor semantics. | `G.neighbors("A")` |
| `successors` | `(u)` | `std::vector<NodeID>` | `O(out_deg(u))` | Explicit directed-style successor helper. | `G.successors("A")` |
| `predecessors` | `(u)` | `std::vector<NodeID>` | directed: `O(in_deg(u))`; undirected: `O(deg(u))` | Returns predecessor IDs in directed graphs. | `G.predecessors("B")` |
| `remove_edge` | `(u, v)` | `void` | approx `O(1 + deg(u))` plus underlying removal | Removes the edge and tracked edge metadata. In multigraphs, semantics are not yet precise enough. | `G.remove_edge(1,2);` |
| `remove_node` | `(u)` | `void` | **`O(V + E)`** | Removes the node, clears incident metadata, erases the vertex, then repairs shifted mappings. | `G.remove_node("Rome");` |
| `clear` | `()` | `void` | `O(V + E)` | Resets graph structure, translation maps, attribute stores, and edge-ID state. | `G.clear();` |
| `node` | `(u)` | `NodeAttrBaseProxy` | avg `O(1)` | Returns node-attribute proxy access. Creates the node if absent. | `G.node("A")["x"] = 7;` |
| `operator[]` | `(u)` | `NodeProxy` | avg `O(1)` | Returns a proxy for chained access such as `G[u][v]` and `G[u][v]["key"]`. Creates `u` if absent. | `G["A"]["B"] = 2.0;` |
| `get_impl` | `()` | `const GraphType&` | `O(1)` | Exposes the internal BGL graph for wrapper implementation or advanced inspection. | `auto& impl = G.get_impl();` |
| `get_bgl_to_id_map` | `()` | `const std::vector<NodeID>&` | `O(1)` | Exposes descriptor-to-ID mapping. | `auto& map = G.get_bgl_to_id_map();` |
| `get_id_to_bgl_map` | `()` | `const std::unordered_map<NodeID, VertexDesc>&` | `O(1)` | Exposes ID-to-descriptor mapping. | `auto& map = G.get_id_to_bgl_map();` |

---

## Attribute API reference

Node and edge attributes are stored outside the BGL graph using `std::any`.

### Checked helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `has_node_attr` | `(u, key)` | `bool` | avg `O(1)` | Checks whether node `u` has attribute `key`. | `G.has_node_attr("A", "color")` |
| `has_edge_attr` | `(u, v, key)` | `bool` | avg `O(1 + deg(u))` | Checks whether edge `(u,v)` has attribute `key`. | `G.has_edge_attr("A","B","capacity")` |
| `get_node_attr<T>` | `(u, key)` | `T` | avg `O(1)` | Returns node attribute `key` with checked `std::any_cast`. Throws on missing key or type mismatch. | `G.get_node_attr<int>("Rome", "population")` |
| `get_edge_attr<T>` | `(u, v, key)` | `T` | avg `O(1 + deg(u))` | Returns edge attribute `key` with checked `std::any_cast`. | `G.get_edge_attr<std::string>("A","B","company")` |
| `try_get_node_attr<T>` | `(u, key)` | `std::optional<T>` | avg `O(1)` | Safe optional-return node attribute lookup. | `G.try_get_node_attr<int>(1, "rank")` |
| `try_get_edge_attr<T>` | `(u, v, key)` | `std::optional<T>` | avg `O(1 + deg(u))` | Safe optional-return edge attribute lookup. | `G.try_get_edge_attr<long>(0,1,"capacity")` |
| `get_edge_numeric_attr` | `(u, v, key)` | `double` | avg `O(1 + deg(u))` | Returns a numeric edge attribute or the built-in `"weight"` as `double`. Used internally by flow wrappers. | `G.get_edge_numeric_attr(0, 1, "capacity")` |

### Proxy syntax

| Syntax | Meaning |
|---|---|
| `G[u][v] = w;` | set built-in edge weight |
| `auto w = (EdgeWeight)G[u][v];` | read built-in edge weight through proxy conversion |
| `G[u][v]["key"] = value;` | set an edge attribute |
| `auto x = (T)G[u][v]["key"];` | read an edge attribute through proxy conversion |
| `G.node(u)["key"] = value;` | set a node attribute |
| `auto x = (T)G.node(u)["key"];` | read a node attribute through proxy conversion |

### Recommendation

Proxy syntax is convenient for writes and demos.

For reads, prefer:

- `has_*_attr`
- `get_*_attr<T>`
- `try_get_*_attr<T>`

because they make type expectations and failure behavior much clearer.

---

## Result-wrapper and helper types

These types are part of the public API and are worth knowing because they make some results easier to consume than raw BGL output.

| Type | Main fields / behavior | What it is for |
|---|---|---|
| `MaximumFlowResult<NodeID>` | `value`, `flow` | max-flow total plus per-edge flow map |
| `MinCostMaxFlowResult<NodeID>` | `flow`, `cost`, `edge_flows` | min-cost max-flow total flow, cost, and per-edge flows |
| `MinimumCutResult<NodeID>` | `value`, `reachable`, `non_reachable`, `cut_edges` | cut value and partition information |
| `SingleSourceShortestPathResult<NodeID, Distance>` | `distance`, `predecessor`, `paths` | single-source shortest-path results in a C++-friendly shape |
| `lookup_map<Key, Value>` | `operator[]`, `at`, iterators | const-friendly lookup wrapper returned by some component helpers |
| `visitor` | no-op hooks `examine_vertex`, `tree_edge`, `back_edge` | small visitor base for traversal entry points |

This is one of the design directions that makes `nxpp` more than a thin parity layer:
some wrappers return results that are easier to work with directly in C++ than raw Boost primitives.

---

## Traversal API reference

For operations on an existing graph, the canonical form is method-based: `G.foo(...)`.

### Edge/tree style helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `bfs_edges` | `(start)` | `std::vector<std::pair<NodeID, NodeID>>` | `O(V + E)` | Runs BFS and returns discovered tree edges. | `auto es = G.bfs_edges(0);` |
| `bfs_tree` | `(start)` | `Graph<NodeID, double, Directed>` | `O(V + E)` | Builds a new graph containing the BFS tree rooted at `start`. | `auto T = G.bfs_tree(0);` |
| `bfs_successors` | `(start)` | `std::unordered_map<NodeID, std::vector<NodeID>>` | `O(V + E)` | Groups BFS tree edges by parent. | `auto s = G.bfs_successors(0);` |
| `dfs_edges` | `(start)` | `std::vector<std::pair<NodeID, NodeID>>` | `O(V + E)` | Runs DFS and returns DFS tree edges. | `auto es = G.dfs_edges(0);` |
| `dfs_tree` | `(start)` | `Graph<NodeID, double, Directed>` | `O(V + E)` | Builds a new graph containing the DFS tree rooted at `start`. | `auto T = G.dfs_tree(0);` |
| `dfs_predecessors` | `(start)` | `std::unordered_map<NodeID, NodeID>` | `O(V + E)` | Returns DFS predecessor map. | `auto p = G.dfs_predecessors(0);` |
| `dfs_successors` | `(start)` | `std::unordered_map<NodeID, std::vector<NodeID>>` | `O(V + E)` | Groups DFS tree edges by parent. | `auto s = G.dfs_successors(0);` |

### Visitor-style helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `breadth_first_search` | `(start, visitor)` | `void` | `O(V + E)` | Visitor-object BFS entry point. | `G.breadth_first_search(0, vis);` |
| `depth_first_search` | `(start, visitor)` | `void` | `O(V + E)` | Visitor-object DFS entry point. | `G.depth_first_search(0, vis);` |
| `bfs_visit` | `(start, on_vertex, on_tree_edge)` | `void` | `O(V + E)` | Callback-style BFS adapter around the visitor layer. | `G.bfs_visit(0, on_v, on_e);` |
| `dfs_visit` | `(start, on_tree_edge, on_back_edge)` | `void` | `O(V + E)` | Callback-style DFS adapter around the visitor layer. | `G.dfs_visit(0, on_t, on_b);` |

---

## Shortest-path API reference

### Source-target helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `shortest_path` | `(source, target)` | `std::vector<NodeID>` | `O(V + E)` | Unweighted shortest path by edge count. | `auto p = G.shortest_path(0, 3);` |
| `shortest_path_length` | `(source, target)` | `double` | `O(V + E)` | Unweighted shortest-path length in edge count. | `auto d = G.shortest_path_length(0, 3);` |
| `shortest_path` | `(source, target, "weight")` | `std::vector<NodeID>` | `O((V + E) log V)` | Weighted shortest path through the built-in edge weight. | `auto p = G.shortest_path(0, 3, "weight");` |
| `shortest_path_length` | `(source, target, "weight")` | `double` | `O((V + E) log V)` | Weighted shortest-path length through the built-in edge weight. | `auto d = G.shortest_path_length(0, 3, "weight");` |
| `dijkstra_path` | `(source, target)` | `std::vector<NodeID>` | `O((V + E) log V)` | Direct Dijkstra source-target path wrapper. | `auto p = G.dijkstra_path(0, 3);` |
| `dijkstra_path` | `(source, target, "weight")` | `std::vector<NodeID>` | `O((V + E) log V)` | Same as above; explicit `"weight"` overload for compatibility-shaped usage. | `auto p = G.dijkstra_path(0, 3, "weight");` |
| `dijkstra_path_length` | `(source, target)` | `Distance` | `O((V + E) log V)` | Dijkstra distance to one target. | `auto d = G.dijkstra_path_length(0, 3);` |
| `dijkstra_path_length` | `(source, target, "weight")` | `Distance` | `O((V + E) log V)` | Same as above with explicit `"weight"` overload. | `auto d = G.dijkstra_path_length(0, 3, "weight");` |
| `bellman_ford_path` | `(source, target)` | `std::vector<NodeID>` | `O(VE)` | Bellman-Ford path wrapper. Throws on negative cycle. | `auto p = G.bellman_ford_path(0, 3);` |
| `bellman_ford_path` | `(source, target, "weight")` | `std::vector<NodeID>` | `O(VE)` | Same as above with explicit `"weight"` overload. | `auto p = G.bellman_ford_path(0, 3, "weight");` |
| `bellman_ford_path_length` | `(source, target)` | `Distance` | `O(VE + L)` | Bellman-Ford distance wrapper; `L` is reconstructed path length used for the final accumulation pass. | `auto d = G.bellman_ford_path_length(0, 3);` |
| `bellman_ford_path_length` | `(source, target, "weight")` | `Distance` | `O(VE + L)` | Same as above with explicit `"weight"` overload. | `auto d = G.bellman_ford_path_length(0, 3, "weight");` |

### Single-source result helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `dijkstra_shortest_paths` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | `O((V + E) log V + V + total_path_materialization)` | Returns distances, predecessors, and reconstructed paths. | `auto r = G.dijkstra_shortest_paths(0);` |
| `single_source_dijkstra` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | same as `dijkstra_shortest_paths` | Thin alias to `dijkstra_shortest_paths`. | `auto r = G.single_source_dijkstra(0);` |
| `bellman_ford_shortest_paths` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | `O(VE + V + total_path_materialization)` | Returns distances, predecessors, and reconstructed paths. | `auto r = G.bellman_ford_shortest_paths(0);` |
| `single_source_bellman_ford` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | same as `bellman_ford_shortest_paths` | Thin alias to `bellman_ford_shortest_paths`. | `auto r = G.single_source_bellman_ford(0);` |
| `dag_shortest_paths` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | `O(V + E + total_path_materialization)` | DAG shortest-path helper returning distances, predecessors, and paths. | `auto r = G.dag_shortest_paths(0);` |
| `floyd_warshall_all_pairs_shortest_paths` | `()` | `std::vector<std::vector<Distance>>` | `O(V^3)` | Returns an all-pairs distance matrix. | `auto fw = G.floyd_warshall_all_pairs_shortest_paths();` |
| `floyd_warshall_all_pairs_shortest_paths_map` | `()` | `std::unordered_map<NodeID, std::unordered_map<NodeID, Distance>>` | `O(V^3 + V^2)` | Convenience map wrapper around the Floyd-Warshall matrix. | `auto fw = G.floyd_warshall_all_pairs_shortest_paths_map();` |

---

## Components, ordering, and spanning

### Components

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `connected_component_groups` | `()` | `std::vector<std::vector<NodeID>>` | `O(V + E)` | Groups vertices by connected component. | `auto cc = G.connected_component_groups();` |
| `connected_components` | `()` | `lookup_map<NodeID, int>` | `O(V + E)` | Returns `node -> component_id`. | `auto map = G.connected_components();` |
| `connected_component_map` | `()` | `lookup_map<NodeID, int>` | same as `connected_components` | Thin alias to `connected_components`. | `auto map = G.connected_component_map();` |
| `strongly_connected_component_groups` | `()` | `std::vector<std::vector<NodeID>>` | `O(V + E)` | Groups vertices by SCC. | `auto scc = G.strongly_connected_component_groups();` |
| `strongly_connected_components` | `()` | `std::vector<std::vector<NodeID>>` | same as `strongly_connected_component_groups` | Thin alias to grouped SCC output. | `auto scc = G.strongly_connected_components();` |
| `strong_component_map` | `()` | `lookup_map<NodeID, int>` | `O(V + E)` | Returns `node -> component_id` for SCCs. | `auto map = G.strong_component_map();` |
| `strongly_connected_component_map` | `()` | `lookup_map<NodeID, int>` | same as `strong_component_map` | Thin alias to `strong_component_map`. | `auto map = G.strongly_connected_component_map();` |
| `strong_components` | `()` | `std::unordered_map<NodeID, NodeID>` | `O(V + E)` | Returns a representative/root per SCC. | `auto roots = G.strong_components();` |
| `strongly_connected_component_roots` | `()` | `std::unordered_map<NodeID, NodeID>` | same as `strong_components` | Thin alias to SCC root map. | `auto roots = G.strongly_connected_component_roots();` |

### Ordering and spanning

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `topological_sort` | `()` | `std::vector<NodeID>` | `O(V + E)` | Returns a topological ordering. | `auto order = G.topological_sort();` |
| `kruskal_minimum_spanning_tree` | `()` | `std::vector<std::pair<NodeID, NodeID>>` | typically `O(E log E)` | Returns MST edges as pairs. | `auto mst = G.kruskal_minimum_spanning_tree();` |
| `prim_minimum_spanning_tree` | `(root)` | `std::unordered_map<NodeID, NodeID>` | typically `O((V + E) log V)` | Returns a `node -> parent` map rooted at `root`. | `auto p = G.prim_minimum_spanning_tree(0);` |
| `minimum_spanning_tree` | `()` | `std::vector<std::pair<NodeID, NodeID>>` | typically `O(E log E)` | Thin default wrapper delegating to Kruskal. | `auto mst = G.minimum_spanning_tree();` |
| `minimum_spanning_tree` | `(root)` | `std::unordered_map<NodeID, NodeID>` | typically `O((V + E) log V)` | Thin rooted wrapper delegating to Prim. | `auto p = G.minimum_spanning_tree(0);` |

---

## Flow and cut API reference

### Maximum flow / minimum cut

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `edmonds_karp_maximum_flow` | `(source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | build auxiliary graph `O(V + E)` + Edmonds-Karp cost | Max-flow wrapper returning value and per-edge flow map. | `auto f = G.edmonds_karp_maximum_flow(0, 5);` |
| `maximum_flow` | `(source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | build auxiliary graph `O(V + E)` + Edmonds-Karp cost | Backward-compatible default max-flow wrapper. | `auto f = G.maximum_flow(0, 5);` |
| `push_relabel_maximum_flow_result` | `(source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | build auxiliary graph `O(V + E)` + Push-Relabel cost | Push-Relabel wrapper returning value and per-edge flow map. | `auto f = G.push_relabel_maximum_flow_result(0, 5);` |
| `minimum_cut` | `(source, sink, capacity_attr = "capacity")` | `MinimumCutResult<NodeID>` | build auxiliary graph `O(V + E)` + max-flow cost + residual BFS `O(V + E)` | Returns cut value, partition, and cut edges. | `auto c = G.minimum_cut(0, 5);` |

### Staged min-cost-flow path

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `push_relabel_maximum_flow` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `long` | build staged auxiliary graph `O(V + E)` + Push-Relabel cost | Computes max flow and caches staged state for a later `cycle_canceling()`. | `long f = G.push_relabel_maximum_flow(0, 5);` |
| `cycle_canceling` | `(weight_attr = "weight")` | deduced cost type | uses cached staged state + cycle-canceling cost | Runs cycle-canceling over staged state prepared by `push_relabel_maximum_flow`. | `long c = G.cycle_canceling();` |

### One-shot min-cost max-flow wrappers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `max_flow_min_cost_cycle_canceling` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build auxiliary graph `O(V + E)` + max-flow / cycle-canceling cost | One-shot min-cost max-flow wrapper using cycle canceling. | `auto r = G.max_flow_min_cost_cycle_canceling(0, 5);` |
| `successive_shortest_path_nonnegative_weights` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build auxiliary graph `O(V + E)` + SSP min-cost-flow cost | One-shot min-cost max-flow wrapper using SSP. | `auto r = G.successive_shortest_path_nonnegative_weights(0, 5);` |
| `max_flow_min_cost_successive_shortest_path` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | same as `successive_shortest_path_nonnegative_weights` | Thin alias to the SSP wrapper. | `auto r = G.max_flow_min_cost_successive_shortest_path(0, 5);` |
| `max_flow_min_cost` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | build auxiliary graph `O(V + E)` + max-flow / cycle-canceling cost | Default min-cost max-flow wrapper; currently delegates to cycle canceling. | `auto r = G.max_flow_min_cost(0, 5);` |

---

## Generators and extra utilities

These are good examples of public helpers that are useful in real C++ code even when they are not exact one-to-one ports of a single NetworkX or BGL entry point.

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `complete_graph` | `(n)` | `GraphType` | current implementation: `O(n^2)` edge insertion attempts | Generates a complete graph for the chosen graph type template. | `auto K5 = nxpp::complete_graph(5);` |
| `path_graph` | `(n)` | `GraphType` | `O(n)` | Generates a path graph. | `auto P4 = nxpp::path_graph(4);` |
| `erdos_renyi_graph` | `(n, p, seed = 42)` | `GraphType` | `O(n^2)` | Generates an Erdős–Rényi random graph. | `auto G = nxpp::erdos_renyi_graph(100, 0.05);` |
| `num_vertices` | `()` | `int` | `O(1)` | Convenience wrapper over `boost::num_vertices`. | `auto n = G.num_vertices();` |
| `degree_centrality` | `()` | `std::unordered_map<NodeID, double>` | `O(V + E)` | Returns degree centrality with NetworkX-like normalization by `n - 1`. | `auto c = G.degree_centrality();` |
| `to_2sat_vertex_id` | `(literal)` | `int` | `O(1)` | Internal/public helper mapping a literal to its implication-graph vertex index. | `auto id = nxpp::to_2sat_vertex_id(-2);` |
| `two_sat_satisfiable` | `(num_variables, clauses)` | `bool` | `O(V + E)` on the implication graph | 2-SAT satisfiability helper built on SCC computation. | `bool ok = nxpp::two_sat_satisfiable(2, {{1,2},{-1,2}});` |
| `print` | variadic | `void` | proportional to printed output | Small Python-style convenience print helper used by examples. | `nxpp::print("Node", 3);` |

---

## Repository layout

| Path | Role |
|---|---|
| `include/nxpp.hpp` | canonical public header |
| `snippet/` | local reference snippets and parity-oriented cases |
| `scripts/test_single_snippet.sh` | compiles/runs implementations in one snippet folder and compares outputs pairwise |
| `scripts/log_snippet_folder.sh` | runs all implementations in one snippet folder and writes a combined log |
| `.github/workflows/snippet-review.yml` | CI snippet review workflow |
| `main.cpp` | smoke/demo-style local entry point |
| `main.py` | older Python-side demo / comparison script |
| `TODO.md` | open work |
| `CHANGELOG.md` | dated changes |
| `SESSION.md` | historical development notes |
| `docs/README.md` | placeholder for fuller docs |

---

## Snippet parity and testing status

The repository includes a visible snippet-based verification workflow:

- `snippet/` contains Boost-style C++, `nxpp` C++, and Python `networkx` variants where relevant
- `scripts/test_single_snippet.sh <folder>` compiles/runs every implementation present in that snippet folder, stores artifacts under `logs/snippet/`, and compares outputs pairwise
- `scripts/log_snippet_folder.sh <folder>` runs all implementations in a folder and writes a combined log
- the GitHub Actions `snippet-review` workflow iterates across all folders under `snippet/`

This is a useful regression / parity harness.

It is **not yet a substitute** for a dedicated test suite with assertion-based unit tests and focused edge-case coverage.

---

## Differences from NetworkX and from raw BGL

### Compared with NetworkX

- `nxpp` is **statically typed**
- return values are usually **materialized C++ containers**
- weighted path dispatch is intentionally narrow: the weighted overloads currently support the built-in edge weight property `"weight"`
- node and edge attributes are runtime-checked through `std::any`
- some helpers intentionally go beyond NetworkX because they are easier to consume from C++

### Compared with raw BGL

- `nxpp` hides descriptors behind user-facing node IDs
- `nxpp` stores node / edge attributes outside the BGL graph
- `nxpp` often returns higher-level result containers
- destructive operations pay extra bookkeeping cost to keep internal translation state valid

---

## `nxpp` utilities that are not just strict NetworkX/BGL ports

A useful direction of the project is that some public helpers exist because they are better ergonomically in C++, not because one upstream API required them.

Examples:

- `successors()` / `predecessors()`
- `has_*_attr`, `get_*_attr<T>`, `try_get_*_attr<T>`
- `connected_component_groups()` and `strongly_connected_component_groups()`
- `dijkstra_shortest_paths()`, `bellman_ford_shortest_paths()`, `dag_shortest_paths()`
- `MaximumFlowResult`, `MinimumCutResult`, `MinCostMaxFlowResult`
- `lookup_map`
- `visitor`

This is one of the project’s strongest ideas and should stay explicit in the documentation:
use Boost internally when it helps, but expose results that are easier to work with in real C++.

---

## Current caveats and trade-offs

### 1. Multigraph edge identity is still under-specified

This is the highest-risk public area today.

### 2. `std::any` keeps the API flexible, but shifts some failures to runtime

That is useful for ergonomics, but it means:

- runtime casts
- runtime type mismatches
- more discipline needed around attribute keys and value types

### 3. Single-header distribution keeps usage easy, but increases maintenance pressure

A single `include/nxpp.hpp` is convenient, but it also means:

- larger compile units
- harder navigation
- higher risk of README / code drift if repo metadata is not maintained carefully

### 4. The weighted overload story is intentionally narrow

The `"weight"` string overloads currently refer to the built-in edge weight property.
They are not a generic "use any named edge attribute as a weight" abstraction yet.

---

## What is still clearly open

The most important open work visible from the repository today is:

- multigraph edge identity redesign / clarification
- a real assertion-based test suite
- stronger edge-case coverage
- packaging / install story beyond manual header inclusion
- clearer documentation generation and source-of-truth discipline
- eventual API stabilization

See [`TODO.md`](TODO.md) for the open list.

---

## License

This project is licensed under the MIT License. See [`LICENSE`](LICENSE).
