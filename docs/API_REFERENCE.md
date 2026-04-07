# API Reference

This document is the curated markdown companion to the generated API reference.

Use the repository `README.md` for:

- project overview
- quick start
- high-level caveats
- repository navigation

Use the generated Doxygen site for:

- declaration-driven API reference
- full member lists and signatures
- declaration-local comments taken directly from the public headers

Use this file for:

- graph template / alias details
- selected API tables that are useful to scan in markdown form
- proxy syntax details
- result-wrapper reference
- return-shape and behavior notes that are easier to summarize at guide level

For complexity guarantees and the side-by-side `Boost` vs `nxpp` discussion, use
[`COMPLEXITY.md`](COMPLEXITY.md). This file intentionally focuses on API shape,
return types, and behavioral notes.

It should not try to become a second full copy of the generated declaration
reference. When this file and the generated docs overlap, the generated docs
win on declaration-level detail.

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

### `NodeID` requirements

`nxpp::Graph<NodeID, ...>` currently expects `NodeID` to be:

- copy-constructible
- equality comparable
- orderable through `std::less`

Those requirements come from the wrapper's ordered translation maps, ordered
result wrappers, and shortest-path predecessor/path reconstruction helpers.

The free numeric graph generators in `nxpp/generators.hpp` have one additional
requirement:

- `NodeID` must be constructible from `std::size_t`

That extra generator constraint is not a global `Graph` requirement; it only
applies to helpers that synthesize node IDs `0..n-1` themselves.

## Core graph API reference

### Construction, mutation, inspection

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `Graph()` | — | graph object | Creates an empty wrapper and initializes internal state. | `nxpp::GraphInt G;` |
| `add_node` | `(const NodeID& id)` | `void` | Inserts the node if absent. | `G.add_node(42);` |
| `add_nodes_from` | `(const std::vector<NodeID>& nodes)` | `void` | Inserts `k` nodes by repeated `add_node`. | `G.add_nodes_from({1,2,3});` |
| `has_node` | `(const NodeID& u)` | `bool` | Checks whether a node exists. | `G.has_node(1)` |
| `add_edge` | `(u, v, w = 1.0)` | `void` | Creates missing endpoints automatically. In simple graphs, a repeated `(u, v)` overwrites weight. | `G.add_edge(1, 2, 3.5);` |
| `add_edge` | `(u, v)` on unweighted graphs | `void` | Inserts an unweighted edge. | `UG.add_edge(1, 2);` |
| `add_edge` | `(u, v, {"key", value})` | `void` | Adds one edge attribute with default built-in weight where applicable. Here `A` is the attribute count stored on the resolved edge. | `G.add_edge(0, 1, {"capacity", 5L});` |
| `add_edge` | `(u, v, {{"key", value}, ...})` | `void` | Adds multiple edge attributes with default built-in weight where applicable. | `G.add_edge(0, 1, {{"capacity", 5L}});` |
| `add_edge` | `(u, v, w, {"key", value})` | `void` | Adds / updates weight and one edge attribute. | `G.add_edge(0, 1, 3.5, {"capacity", 5L});` |
| `add_edge` | `(u, v, w, {{"key", value}, ...})` | `void` | Adds / updates weight and multiple edge attributes. | `G.add_edge(0, 1, 3.5, {{"capacity", 5L}});` |
| `add_edges_from` | `vector<tuple<u,v,w>>` | `void` | Bulk weighted insertion. | `G.add_edges_from({{1,2,2},{2,3,4}});` |
| `add_edges_from` | `vector<pair<u,v>>` | `void` | Bulk insertion with default weight or unweighted insertion depending on graph type. | `G.add_edges_from({{1,2},{2,3}});` |
| `has_edge` | `(u, v)` | `bool` | Checks whether an edge exists. In multigraphs, this means "at least one edge exists". | `G.has_edge("A","B")` |
| `has_edge_id` | `(edge_id)` | `bool` | Checks whether a specific wrapper-tracked edge ID still exists. This is the precise multigraph edge existence check. | `G.has_edge_id(eid)` |
| `get_edge_weight` | `(u, v)` | `EdgeWeight` | Returns the built-in edge weight. In multigraphs, this resolves through one edge returned by `boost::edge(u, v, g)` and should not be treated as a stable single-parallel-edge lookup. | `auto w = G.get_edge_weight(1,2);` |
| `get_edge_weight` | `(edge_id)` | `EdgeWeight` | Returns the built-in edge weight for one specific wrapper-tracked edge ID. | `auto w = G.get_edge_weight(eid);` |
| `nodes` | `()` | `std::vector<NodeID>` | Materializes all node IDs. | `auto ns = G.nodes();` |
| `edges` | `()` | weighted graphs: `std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>`; unweighted graphs: `std::vector<std::pair<NodeID, NodeID>>` | Materializes all edges. | `auto es = G.edges();` |
| `edge_pairs` | `()` | `std::vector<std::pair<NodeID, NodeID>>` | Materializes edges without weights. Useful for wrappers that rebuild auxiliary graphs. | `auto ep = G.edge_pairs();` |
| `edge_ids` | `()` | `std::vector<size_t>` | Returns every wrapper-tracked edge ID currently present in the graph. | `auto ids = G.edge_ids();` |
| `edge_ids` | `(u, v)` | `std::vector<size_t>` | Returns the tracked edge IDs between two endpoints. In multigraphs, this is the main way to enumerate parallel edges precisely. | `auto ids = G.edge_ids("A","B");` |
| `get_edge_endpoints` | `(edge_id)` | `std::pair<NodeID, NodeID>` | Returns the endpoints of one specific wrapper-tracked edge ID. | `auto [u, v] = G.get_edge_endpoints(eid);` |
| `neighbors` | `(u)` | `std::vector<NodeID>` | Returns out-neighbors. For directed graphs this matches successor semantics. | `G.neighbors("A")` |
| `successors` | `(u)` | `std::vector<NodeID>` | Explicit directed-style successor helper. | `G.successors("A")` |
| `predecessors` | `(u)` | `std::vector<NodeID>` | Returns predecessor IDs in directed graphs. | `G.predecessors("B")` |
| `remove_edge` | `(u, v)` | `void` | Removes the edge and tracked edge metadata. In multigraphs, this removes all parallel edges between `u` and `v` and cleans all tracked metadata for that pair. | `G.remove_edge(1,2);` |
| `remove_edge` | `(edge_id)` | `void` | Removes one specific wrapper-tracked edge ID. This is the precise multigraph removal API. | `G.remove_edge(eid);` |
| `remove_node` | `(u)` | `void` | Removes the node, clears incident metadata, erases the vertex, then repairs shifted mappings. | `G.remove_node("Rome");` |
| `clear` | `()` | `void` | Resets graph structure, translation maps, attribute stores, and edge-ID state. | `G.clear();` |
| `node` | `(u)` | `NodeAttrBaseProxy` | Returns node-attribute proxy access. Creates the node if absent. | `G.node("A")["x"] = 7;` |
| `operator[]` | `(u)` | `NodeProxy` | Returns a proxy for chained access such as `G[u][v]` and `G[u][v]["key"]`. Creates `u` if absent. | `G["A"]["B"] = 2.0;` |
| `get_impl` | `()` | `const GraphType&` | Exposes the internal BGL graph for wrapper implementation or advanced inspection. | `auto& impl = G.get_impl();` |
| `get_bgl_to_id_map` | `()` | `const std::vector<NodeID>&` | Exposes the wrapper's maintained index-ordered node list used by result normalization. | `auto& map = G.get_bgl_to_id_map();` |
| `get_id_to_bgl_map` | `()` | `const std::map<NodeID, VertexDesc>&` | Exposes ID-to-descriptor mapping. | `auto& map = G.get_id_to_bgl_map();` |
| `get_node_id` | `(vertex_descriptor)` | `const NodeID&` | Returns the user-facing node ID for a descriptor. Mostly useful for advanced integrations. | `auto id = G.get_node_id(v);` |
| `get_vertex_index` | `(vertex_descriptor)` | `size_t` | Returns the wrapper-maintained vertex index used by normalized result containers. | `auto i = G.get_vertex_index(v);` |

## Attribute API reference

Node and edge attributes are stored outside the BGL graph using `std::any` in
ordered `std::map`-backed stores.

### Checked helpers

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `has_node_attr` | `(u, key)` | `bool` | Checks whether node `u` has attribute `key`. Here `A` is the attribute count stored on node `u`. | `G.has_node_attr("A", "color")` |
| `has_edge_attr` | `(u, v, key)` | `bool` | Checks whether the resolved `(u,v)` edge has attribute `key`. In multigraphs, this uses the same non-stable `(u, v)` resolution as other edge lookups. | `G.has_edge_attr("A","B","capacity")` |
| `has_edge_attr` | `(edge_id, key)` | `bool` | Checks whether a specific wrapper-tracked edge ID has attribute `key`. | `G.has_edge_attr(eid, "capacity")` |
| `get_node_attr<T>` | `(u, key)` | `T` | Returns node attribute `key` with checked `std::any_cast`. Throws on missing key or type mismatch. | `G.get_node_attr<int>("Rome", "population")` |
| `get_edge_attr<T>` | `(u, v, key)` | `T` | Returns edge attribute `key` with checked `std::any_cast`. In multigraphs, this resolves through one edge returned by `boost::edge(u, v, g)` and is not a stable single-parallel-edge API. | `G.get_edge_attr<std::string>("A","B","company")` |
| `get_edge_attr<T>` | `(edge_id, key)` | `T` | Returns edge attribute `key` for one specific wrapper-tracked edge ID. | `G.get_edge_attr<std::string>(eid, "company")` |
| `try_get_node_attr<T>` | `(u, key)` | `std::optional<T>` | Safe optional-return node attribute lookup. | `G.try_get_node_attr<int>(1, "rank")` |
| `try_get_edge_attr<T>` | `(u, v, key)` | `std::optional<T>` | Safe optional-return edge attribute lookup. In multigraphs, this uses the same non-stable `(u, v)` resolution as `get_edge_attr<T>`. | `G.try_get_edge_attr<long>(0,1,"capacity")` |
| `try_get_edge_attr<T>` | `(edge_id, key)` | `std::optional<T>` | Safe optional-return edge attribute lookup for one specific wrapper-tracked edge ID. | `G.try_get_edge_attr<long>(eid, "capacity")` |
| `get_edge_numeric_attr` | `(u, v, key)` | `double` | Returns a numeric edge attribute or the built-in `"weight"` as `double`. In multigraphs, this follows the same non-stable `(u, v)` resolution path as the other edge lookup helpers. | `G.get_edge_numeric_attr(0, 1, "capacity")` |
| `get_edge_numeric_attr` | `(edge_id, key)` | `double` | Returns a numeric edge attribute or built-in `"weight"` for one specific wrapper-tracked edge ID. | `G.get_edge_numeric_attr(eid, "capacity")` |
| `set_edge_attr<T>` | `(edge_id, key, value)` | `void` | Sets one attribute on a specific wrapper-tracked edge ID after validating that the edge ID still exists. | `G.set_edge_attr(eid, "capacity", 5L)` |
| `set_edge_weight` | `(edge_id, weight)` | `void` | Sets the built-in weight on a specific wrapper-tracked edge ID. | `G.set_edge_weight(eid, 3.5)` |

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
| `SingleSourceShortestPathResult<NodeID, Distance>` | ordered `distance`, `predecessor`, plus `has_path_to(target)` / `path_to(target)` | single-source shortest-path results in a C++-friendly shape with tree-based map bounds and on-demand path reconstruction |
| `lookup_map<Key, Value>` | `operator[]`, `at`, iterators over ordered storage | const-friendly ordered lookup wrapper returned by some component helpers |
| `indexed_lookup_map<Key, Value>` | `at`, `operator[]`, `contains`, iterators over key-sorted storage | const-friendly indexed result wrapper that preserves linear materialization while keeping `O(log n)` key lookup |
| `visitor` | no-op hooks `examine_vertex`, `tree_edge`, `back_edge` | small visitor base for traversal entry points |

This is one of the design directions that makes `nxpp` more than a thin parity layer:
some wrappers return results that are easier to work with directly in C++ than raw Boost primitives.

## Traversal API reference

For operations on an existing graph, the canonical form is method-based: `G.foo(...)`.

### Edge/tree style helpers

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `bfs_edges` | `(start)` | `std::vector<std::pair<NodeID, NodeID>>` | Runs BFS and returns discovered tree edges. | `auto es = G.bfs_edges(0);` |
| `bfs_tree` | `(start)` | `Graph<NodeID, double, Directed>` | Builds a new graph containing the BFS tree rooted at `start`. | `auto T = G.bfs_tree(0);` |
| `bfs_successors` | `(start)` | `indexed_lookup_map<NodeID, std::vector<NodeID>>` | Groups BFS tree edges by parent with linear materialization and `O(log n)` key lookup. | `auto s = G.bfs_successors(0);` |
| `dfs_edges` | `(start)` | `std::vector<std::pair<NodeID, NodeID>>` | Runs DFS and returns DFS tree edges. | `auto es = G.dfs_edges(0);` |
| `dfs_tree` | `(start)` | `Graph<NodeID, double, Directed>` | Builds a new graph containing the DFS tree rooted at `start`. | `auto T = G.dfs_tree(0);` |
| `dfs_predecessors` | `(start)` | `indexed_lookup_map<NodeID, NodeID>` | Returns DFS predecessor map with linear materialization and `O(log n)` key lookup. | `auto p = G.dfs_predecessors(0);` |
| `dfs_successors` | `(start)` | `indexed_lookup_map<NodeID, std::vector<NodeID>>` | Groups DFS tree edges by parent with linear materialization and `O(log n)` key lookup. | `auto s = G.dfs_successors(0);` |

### Visitor-style helpers

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `breadth_first_search` | `(start, visitor)` | `void` | Visitor-object BFS entry point. | `G.breadth_first_search(0, vis);` |
| `depth_first_search` | `(start, visitor)` | `void` | Visitor-object DFS entry point. | `G.depth_first_search(0, vis);` |
| `bfs_visit` | `(start, on_vertex, on_tree_edge)` | `void` | Callback-style BFS adapter around the visitor layer. | `G.bfs_visit(0, on_v, on_e);` |
| `dfs_visit` | `(start, on_tree_edge, on_back_edge)` | `void` | Callback-style DFS adapter around the visitor layer. | `G.dfs_visit(0, on_t, on_b);` |

## Shortest-path API reference

### Source-target helpers

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `shortest_path` | `(source, target)` | `std::vector<NodeID>` | Unweighted shortest path by edge count. | `auto p = G.shortest_path(0, 3);` |
| `shortest_path_length` | `(source, target)` | `double` | Unweighted shortest-path length in edge count. | `auto d = G.shortest_path_length(0, 3);` |
| `shortest_path` | `(source, target, "weight")` | `std::vector<NodeID>` | Weighted shortest path through the built-in edge weight. | `auto p = G.shortest_path(0, 3, "weight");` |
| `shortest_path_length` | `(source, target, "weight")` | `double` | Weighted shortest-path length through the built-in edge weight. | `auto d = G.shortest_path_length(0, 3, "weight");` |
| `dijkstra_path` | `(source, target)` | `std::vector<NodeID>` | Direct Dijkstra source-target path wrapper. | `auto p = G.dijkstra_path(0, 3);` |
| `dijkstra_path` | `(source, target, "weight")` | `std::vector<NodeID>` | Same as above; explicit `"weight"` overload for compatibility-shaped usage. | `auto p = G.dijkstra_path(0, 3, "weight");` |
| `dijkstra_path_length` | `(source, target)` | `Distance` | Dijkstra distance to one target. | `auto d = G.dijkstra_path_length(0, 3);` |
| `dijkstra_path_length` | `(source, target, "weight")` | `Distance` | Same as above with explicit `"weight"` overload. | `auto d = G.dijkstra_path_length(0, 3, "weight");` |
| `bellman_ford_path` | `(source, target)` | `std::vector<NodeID>` | Bellman-Ford path wrapper. Throws on negative cycle. | `auto p = G.bellman_ford_path(0, 3);` |
| `bellman_ford_path` | `(source, target, "weight")` | `std::vector<NodeID>` | Same as above with explicit `"weight"` overload. | `auto p = G.bellman_ford_path(0, 3, "weight");` |
| `bellman_ford_path_length` | `(source, target)` | `Distance` | Bellman-Ford distance wrapper with a final accumulation over the reconstructed path. | `auto d = G.bellman_ford_path_length(0, 3);` |
| `bellman_ford_path_length` | `(source, target, "weight")` | `Distance` | Same as above with explicit `"weight"` overload. | `auto d = G.bellman_ford_path_length(0, 3, "weight");` |

### Single-source result helpers

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `dijkstra_shortest_paths` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | Returns distances and predecessors, with on-demand path reconstruction through the result wrapper. | `auto r = G.dijkstra_shortest_paths(0);` |
| `single_source_dijkstra` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | Thin alias to `dijkstra_shortest_paths`. | `auto r = G.single_source_dijkstra(0);` |
| `bellman_ford_shortest_paths` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | Returns distances and predecessors, with on-demand path reconstruction through the result wrapper. | `auto r = G.bellman_ford_shortest_paths(0);` |
| `single_source_bellman_ford` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | Thin alias to `bellman_ford_shortest_paths`. | `auto r = G.single_source_bellman_ford(0);` |
| `dag_shortest_paths` | `(source)` | `SingleSourceShortestPathResult<NodeID, Distance>` | DAG shortest-path helper returning distances and predecessors, with on-demand path reconstruction through the result wrapper. | `auto r = G.dag_shortest_paths(0);` |
| `floyd_warshall_all_pairs_shortest_paths` | `()` | `std::vector<std::vector<Distance>>` | Returns an all-pairs distance matrix. | `auto fw = G.floyd_warshall_all_pairs_shortest_paths();` |
| `floyd_warshall_all_pairs_shortest_paths_map` | `()` | `std::map<NodeID, std::map<NodeID, Distance>>` | Convenience map wrapper around the Floyd-Warshall matrix. | `auto fw = G.floyd_warshall_all_pairs_shortest_paths_map();` |

## Components, ordering, and spanning

### Components

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `connected_component_groups` | `()` | `std::vector<std::vector<NodeID>>` | Groups vertices by connected component. | `auto cc = G.connected_component_groups();` |
| `connected_components` | `()` | `indexed_lookup_map<NodeID, int>` | Returns `node -> component_id` with linear materialization and `O(log n)` key lookup. | `auto map = G.connected_components();` |
| `connected_component_map` | `()` | `indexed_lookup_map<NodeID, int>` | Thin alias to `connected_components`. | `auto map = G.connected_component_map();` |
| `strongly_connected_component_groups` | `()` | `std::vector<std::vector<NodeID>>` | Groups vertices by SCC. | `auto scc = G.strongly_connected_component_groups();` |
| `strongly_connected_components` | `()` | `std::vector<std::vector<NodeID>>` | Thin alias to grouped SCC output. | `auto scc = G.strongly_connected_components();` |
| `strong_component_map` | `()` | `indexed_lookup_map<NodeID, int>` | Returns `node -> component_id` for SCCs with linear materialization and `O(log n)` key lookup. | `auto map = G.strong_component_map();` |
| `strongly_connected_component_map` | `()` | `indexed_lookup_map<NodeID, int>` | Thin alias to `strong_component_map`. | `auto map = G.strongly_connected_component_map();` |
| `strong_components` | `()` | `indexed_lookup_map<NodeID, NodeID>` | Returns a representative/root per SCC with linear materialization and `O(log n)` key lookup. | `auto roots = G.strong_components();` |
| `strongly_connected_component_roots` | `()` | `indexed_lookup_map<NodeID, NodeID>` | Thin alias to SCC root map. | `auto roots = G.strongly_connected_component_roots();` |

### Ordering and spanning

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `topological_sort` | `()` | `std::vector<NodeID>` | Returns a topological ordering. | `auto order = G.topological_sort();` |
| `kruskal_minimum_spanning_tree` | `()` | `std::vector<std::pair<NodeID, NodeID>>` | Returns MST edges as pairs. | `auto mst = G.kruskal_minimum_spanning_tree();` |
| `prim_minimum_spanning_tree` | `(root)` | `std::map<NodeID, NodeID>` | Returns a `node -> parent` map rooted at `root`. | `auto p = G.prim_minimum_spanning_tree(0);` |
| `minimum_spanning_tree` | `()` | `std::vector<std::pair<NodeID, NodeID>>` | Thin default wrapper delegating to Kruskal. | `auto mst = G.minimum_spanning_tree();` |
| `minimum_spanning_tree` | `(root)` | `std::map<NodeID, NodeID>` | Thin rooted wrapper delegating to Prim. | `auto p = G.minimum_spanning_tree(0);` |

## Flow and cut API reference

### Maximum flow / minimum cut

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `edmonds_karp_maximum_flow` | `(source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | Max-flow wrapper returning value and per-edge flow map. | `auto f = G.edmonds_karp_maximum_flow(0, 5);` |
| `maximum_flow` | `(source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | Backward-compatible default max-flow wrapper. | `auto f = G.maximum_flow(0, 5);` |
| `push_relabel_maximum_flow_result` | `(source, sink, capacity_attr = "capacity")` | `MaximumFlowResult<NodeID>` | Push-Relabel wrapper returning value and per-edge flow map. | `auto f = G.push_relabel_maximum_flow_result(0, 5);` |
| `minimum_cut` | `(source, sink, capacity_attr = "capacity")` | `MinimumCutResult<NodeID>` | Returns cut value, partition, and cut edges. | `auto c = G.minimum_cut(0, 5);` |

### Staged min-cost-flow path

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `push_relabel_maximum_flow` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `long` | Computes max flow and caches staged state for a later `cycle_canceling()`. | `long f = G.push_relabel_maximum_flow(0, 5);` |
| `cycle_canceling` | `(weight_attr = "weight")` | deduced cost type | Runs cycle-canceling over staged state prepared by `push_relabel_maximum_flow`. | `long c = G.cycle_canceling();` |

### One-shot min-cost max-flow wrappers

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `max_flow_min_cost_cycle_canceling` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | One-shot min-cost max-flow wrapper using cycle canceling. | `auto r = G.max_flow_min_cost_cycle_canceling(0, 5);` |
| `successive_shortest_path_nonnegative_weights` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | One-shot min-cost max-flow wrapper using SSP. | `auto r = G.successive_shortest_path_nonnegative_weights(0, 5);` |
| `max_flow_min_cost_successive_shortest_path` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | Thin alias to the SSP wrapper. | `auto r = G.max_flow_min_cost_successive_shortest_path(0, 5);` |
| `max_flow_min_cost` | `(source, sink, capacity_attr = "capacity", weight_attr = "weight")` | `MinCostMaxFlowResult<NodeID>` | Default min-cost max-flow wrapper; currently delegates to cycle canceling. | `auto r = G.max_flow_min_cost(0, 5);` |

## Generators and extra utilities

These are good examples of public helpers that are useful in real C++ code even when they are not exact one-to-one ports of a single NetworkX or BGL entry point.

| Function | Parameters | Returns | Description | Example |
|---|---|---:|---|---|
| `complete_graph` | `(n)` | `GraphType` | Generates a complete graph for the chosen graph type template. | `auto K5 = nxpp::complete_graph(5);` |
| `path_graph` | `(n)` | `GraphType` | Generates a path graph. | `auto P4 = nxpp::path_graph(4);` |
| `erdos_renyi_graph` | `(n, p, seed = 42)` | `GraphType` | Generates an Erdős–Rényi random graph. | `auto G = nxpp::erdos_renyi_graph(100, 0.05);` |
| `num_vertices` | `()` | `int` | Convenience wrapper over `boost::num_vertices`. | `auto n = G.num_vertices();` |
| `degree_centrality` | `()` | `indexed_lookup_map<NodeID, double>` | Returns degree centrality with NetworkX-like normalization by `n - 1`, using linear materialization plus `O(log n)` key lookup. | `auto c = G.degree_centrality();` |
| `to_2sat_vertex_id` | `(literal)` | `int` | Internal/public helper mapping a literal to its implication-graph vertex index. | `auto id = nxpp::to_2sat_vertex_id(-2);` |
| `two_sat_satisfiable` | `(num_variables, clauses)` | `bool` | 2-SAT satisfiability helper built on SCC computation. | `bool ok = nxpp::two_sat_satisfiable(2, {{1,2},{-1,2}});` |
