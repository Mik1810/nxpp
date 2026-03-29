# API Reference

This document holds the detailed public API reference that was previously embedded in the root `README.md`.

Use the root [`README.md`](../README.md) for:

- project overview
- quick start
- high-level caveats
- repository navigation

Use this file for:

- graph template / alias details
- API tables
- proxy syntax details
- result-wrapper reference
- traversal / shortest-path / flow / utility reference

## Complexity conventions

The tables below use:

- `V` = number of vertices
- `E` = number of edges
- `deg(u)` = degree / out-degree relevant to `u`
- `k` = number of inserted items in a bulk call

Conventions:

- unordered-map lookups are treated as average-case `O(1)`
- vector growth is treated as amortized `O(1)` per push
- materializing the returned result container counts toward the complexity
- cleanup / metadata repair done by `nxpp` counts toward the complexity

These are practical public-call notes, not formal proofs.

## Public graph template and aliases

### Primary template

| Type | Meaning |
|---|---|
| `nxpp::Graph<NodeID, EdgeWeight, false, false>` | Undirected simple weighted graph with default selectors |
| `nxpp::Graph<NodeID, EdgeWeight, true, false>` | Directed simple weighted graph with default selectors |
| `nxpp::Graph<NodeID, EdgeWeight, false, true>` | Undirected multigraph with default selectors |
| `nxpp::Graph<NodeID, EdgeWeight, true, true>` | Directed multigraph with default selectors |
| `nxpp::Graph<NodeID, EdgeWeight, Directed, Multi, false>` | Unweighted variant with default selectors |
| `nxpp::Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>` | Advanced form with explicit BGL selectors |

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
All aliases intentionally stay on the default `boost::vecS` / `boost::vecS` backend.

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
| `has_edge_id` | `(edge_id)` | `bool` | `O(E)` | Checks whether a specific wrapper-tracked edge ID still exists. This is the precise multigraph edge existence check. | `G.has_edge_id(eid)` |
| `get_edge_weight` | `(u, v)` | `EdgeWeight` | avg `O(1 + deg(u))` | Returns the built-in edge weight. In multigraphs, this resolves through one edge returned by `boost::edge(u, v, g)` and should not be treated as a stable single-parallel-edge lookup. | `auto w = G.get_edge_weight(1,2);` |
| `get_edge_weight` | `(edge_id)` | `EdgeWeight` | `O(E)` | Returns the built-in edge weight for one specific wrapper-tracked edge ID. | `auto w = G.get_edge_weight(eid);` |
| `nodes` | `()` | `std::vector<NodeID>` | `O(V)` | Materializes all node IDs. | `auto ns = G.nodes();` |
| `edges` | `()` | weighted graphs: `std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>`; unweighted graphs: `std::vector<std::pair<NodeID, NodeID>>` | `O(E)` | Materializes all edges. | `auto es = G.edges();` |
| `edge_pairs` | `()` | `std::vector<std::pair<NodeID, NodeID>>` | `O(E)` | Materializes edges without weights. Useful for wrappers that rebuild auxiliary graphs. | `auto ep = G.edge_pairs();` |
| `edge_ids` | `()` | `std::vector<size_t>` | `O(E)` | Returns every wrapper-tracked edge ID currently present in the graph. | `auto ids = G.edge_ids();` |
| `edge_ids` | `(u, v)` | `std::vector<size_t>` | approx `O(deg(u))` | Returns the tracked edge IDs between two endpoints. In multigraphs, this is the main way to enumerate parallel edges precisely. | `auto ids = G.edge_ids("A","B");` |
| `get_edge_endpoints` | `(edge_id)` | `std::pair<NodeID, NodeID>` | `O(E)` | Returns the endpoints of one specific wrapper-tracked edge ID. | `auto [u, v] = G.get_edge_endpoints(eid);` |
| `neighbors` | `(u)` | `std::vector<NodeID>` | `O(deg(u))` | Returns out-neighbors. For directed graphs this matches successor semantics. | `G.neighbors("A")` |
| `successors` | `(u)` | `std::vector<NodeID>` | `O(out_deg(u))` | Explicit directed-style successor helper. | `G.successors("A")` |
| `predecessors` | `(u)` | `std::vector<NodeID>` | directed: `O(in_deg(u))`; undirected: `O(deg(u))` | Returns predecessor IDs in directed graphs. | `G.predecessors("B")` |
| `remove_edge` | `(u, v)` | `void` | approx `O(1 + deg(u))` plus underlying removal | Removes the edge and tracked edge metadata. In multigraphs, this removes all parallel edges between `u` and `v` and cleans all tracked metadata for that pair. | `G.remove_edge(1,2);` |
| `remove_edge` | `(edge_id)` | `void` | `O(E)` | Removes one specific wrapper-tracked edge ID. This is the precise multigraph removal API. | `G.remove_edge(eid);` |
| `remove_node` | `(u)` | `void` | **`O(V + E)`** | Removes the node, clears incident metadata, erases the vertex, then repairs shifted mappings. | `G.remove_node("Rome");` |
| `clear` | `()` | `void` | `O(V + E)` | Resets graph structure, translation maps, attribute stores, and edge-ID state. | `G.clear();` |
| `node` | `(u)` | `NodeAttrBaseProxy` | avg `O(1)` | Returns node-attribute proxy access. Creates the node if absent. | `G.node("A")["x"] = 7;` |
| `operator[]` | `(u)` | `NodeProxy` | avg `O(1)` | Returns a proxy for chained access such as `G[u][v]` and `G[u][v]["key"]`. Creates `u` if absent. | `G["A"]["B"] = 2.0;` |
| `get_impl` | `()` | `const GraphType&` | `O(1)` | Exposes the internal BGL graph for wrapper implementation or advanced inspection. | `auto& impl = G.get_impl();` |
| `get_bgl_to_id_map` | `()` | `const std::vector<NodeID>&` | `O(1)` | Exposes the wrapper's maintained index-ordered node list used by result normalization. | `auto& map = G.get_bgl_to_id_map();` |
| `get_id_to_bgl_map` | `()` | `const std::unordered_map<NodeID, VertexDesc>&` | `O(1)` | Exposes ID-to-descriptor mapping. | `auto& map = G.get_id_to_bgl_map();` |
| `get_node_id` | `(vertex_descriptor)` | `const NodeID&` | `O(1)` | Returns the user-facing node ID for a descriptor. Mostly useful for advanced integrations. | `auto id = G.get_node_id(v);` |
| `get_vertex_index` | `(vertex_descriptor)` | `size_t` | `O(1)` | Returns the wrapper-maintained vertex index used by normalized result containers. | `auto i = G.get_vertex_index(v);` |

## Attribute API reference

Node and edge attributes are stored outside the BGL graph using `std::any`.

### Checked helpers

| Function | Parameters | Returns | Public-call complexity | Description | Example |
|---|---|---:|---|---|---|
| `has_node_attr` | `(u, key)` | `bool` | avg `O(1)` | Checks whether node `u` has attribute `key`. | `G.has_node_attr("A", "color")` |
| `has_edge_attr` | `(u, v, key)` | `bool` | avg `O(1 + deg(u))` | Checks whether the resolved `(u,v)` edge has attribute `key`. In multigraphs, this uses the same non-stable `(u, v)` resolution as other edge lookups. | `G.has_edge_attr("A","B","capacity")` |
| `has_edge_attr` | `(edge_id, key)` | `bool` | avg `O(1)` | Checks whether a specific wrapper-tracked edge ID has attribute `key`. | `G.has_edge_attr(eid, "capacity")` |
| `get_node_attr<T>` | `(u, key)` | `T` | avg `O(1)` | Returns node attribute `key` with checked `std::any_cast`. Throws on missing key or type mismatch. | `G.get_node_attr<int>("Rome", "population")` |
| `get_edge_attr<T>` | `(u, v, key)` | `T` | avg `O(1 + deg(u))` | Returns edge attribute `key` with checked `std::any_cast`. In multigraphs, this resolves through one edge returned by `boost::edge(u, v, g)` and is not a stable single-parallel-edge API. | `G.get_edge_attr<std::string>("A","B","company")` |
| `get_edge_attr<T>` | `(edge_id, key)` | `T` | avg `O(1)` | Returns edge attribute `key` for one specific wrapper-tracked edge ID. | `G.get_edge_attr<std::string>(eid, "company")` |
| `try_get_node_attr<T>` | `(u, key)` | `std::optional<T>` | avg `O(1)` | Safe optional-return node attribute lookup. | `G.try_get_node_attr<int>(1, "rank")` |
| `try_get_edge_attr<T>` | `(u, v, key)` | `std::optional<T>` | avg `O(1 + deg(u))` | Safe optional-return edge attribute lookup. In multigraphs, this uses the same non-stable `(u, v)` resolution as `get_edge_attr<T>`. | `G.try_get_edge_attr<long>(0,1,"capacity")` |
| `try_get_edge_attr<T>` | `(edge_id, key)` | `std::optional<T>` | avg `O(1)` | Safe optional-return edge attribute lookup for one specific wrapper-tracked edge ID. | `G.try_get_edge_attr<long>(eid, "capacity")` |
| `get_edge_numeric_attr` | `(u, v, key)` | `double` | avg `O(1 + deg(u))` | Returns a numeric edge attribute or the built-in `"weight"` as `double`. In multigraphs, this follows the same non-stable `(u, v)` resolution path as the other edge lookup helpers. | `G.get_edge_numeric_attr(0, 1, "capacity")` |
| `get_edge_numeric_attr` | `(edge_id, key)` | `double` | avg `O(1)` | Returns a numeric edge attribute or built-in `"weight"` for one specific wrapper-tracked edge ID. | `G.get_edge_numeric_attr(eid, "capacity")` |
| `set_edge_attr<T>` | `(edge_id, key, value)` | `void` | avg `O(1)` | Sets one attribute on a specific wrapper-tracked edge ID. | `G.set_edge_attr(eid, "capacity", 5L)` |
| `set_edge_weight` | `(edge_id, weight)` | `void` | `O(E)` | Sets the built-in weight on a specific wrapper-tracked edge ID. | `G.set_edge_weight(eid, 3.5)` |

### Proxy syntax

| Syntax | Meaning |
|---|---|
| `G[u][v] = w;` | set built-in edge weight; in multigraphs this is not a stable single-parallel-edge handle |
| `auto w = (EdgeWeight)G[u][v];` | read built-in edge weight through proxy conversion; in multigraphs this follows the same non-stable `(u, v)` resolution path |
| `G[u][v]["key"] = value;` | set an edge attribute; in multigraphs this is not yet a precise per-parallel-edge API |
| `auto x = (T)G[u][v]["key"];` | read an edge attribute through proxy conversion; in multigraphs this follows the same non-stable `(u, v)` resolution path |
| `G.node(u)["key"] = value;` | set a node attribute |
| `auto x = (T)G.node(u)["key"];` | read a node attribute through proxy conversion |

### Edge-ID usage

For multigraph-safe edge access:

```cpp
nxpp::MultiDiGraph G;

auto e1 = G.add_edge_with_id("A", "B", 1.0);
auto e2 = G.add_edge_with_id("A", "B", 2.0);

G.set_edge_attr(e1, "label", std::string("first"));
G.set_edge_attr(e2, "label", std::string("second"));

auto ids = G.edge_ids("A", "B");
auto w1 = G.get_edge_weight(e1);
auto label2 = G.get_edge_attr<std::string>(e2, "label");

G.remove_edge(e1);
```

### Recommendation

Proxy syntax is convenient for writes and demos.

For reads, prefer:

- `has_*_attr`
- `get_*_attr<T>`
- `try_get_*_attr<T>`

because they make type expectations and failure behavior much clearer.

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
