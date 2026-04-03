# Complexity Guide

This document explains how to read complexity claims in `nxpp`.

The key distinction is:

- **Boost algorithm complexity**: the complexity of the underlying Boost Graph
  Library algorithm or primitive
- **`nxpp` public-call complexity**: the cost of calling the `nxpp` API,
  including wrapper-side translation, bookkeeping, and result materialization

Use the root [`README.md`](../README.md) for the short user-facing explanation.
Use [`API_REFERENCE.md`](API_REFERENCE.md) for per-function tables.
Use this file for the policy that connects those two views.

Notation used in this file:

- `n = |V|`, number of vertices
- `m = |E|`, number of edges
- `F_maxflow(n, m)`, dominant complexity of the delegated Boost max-flow core
- `F_mincost_cc(n, m)`, dominant complexity of the delegated Boost
  cycle-canceling min-cost-flow core
- `F_ssp(n, m)`, dominant complexity of the delegated Boost successive
  shortest-path min-cost-flow core

## Core rule

`nxpp` does not document only the algorithmic core.
It documents the cost of the **whole public function call**.

That means the following wrapper work counts:

- `NodeID <-> vertex_descriptor` translation
- attribute lookup through external maps
- result materialization into C++ containers
- cleanup of wrapper-owned metadata
- descriptor/index repair after destructive graph updates

So a Boost algorithm and the corresponding `nxpp` method can have:

- the **same algorithmic core complexity**
- but a **different full public-call complexity**

## Reading the guarantees

There are three useful levels of complexity statements in this project.

### 1. Same algorithmic order as Boost

This means the wrapper keeps the same dominant asymptotic order as the Boost
algorithm it delegates to.

Typical examples:

- BFS / DFS edge collection
- shortest-path source-target helpers
- Bellman-Ford negative-cycle detection
- Kruskal minimum spanning tree
- max-flow / min-cost-flow wrappers where the wrapper build step is lower-order

In these cases, `nxpp` may still do extra constant-factor or lower-order work,
but the dominant asymptotic term stays the same as Boost.

### 2. Same Boost core plus wrapper materialization

This means the underlying algorithmic phase still has the same order as Boost,
but `nxpp` adds a separate cost to build a user-facing result.

Typical examples:

- `dijkstra_shortest_paths()`
- `bellman_ford_shortest_paths()`
- `dag_shortest_paths()`
- `floyd_warshall_all_pairs_shortest_paths_map()`
- component-map style helpers

For these APIs, the most honest statement is usually:

- Boost core: `...`
- `nxpp` public call: `Boost core + result materialization`

That is still compatible with the design goal of preserving Boost's algorithmic
order while being honest about wrapper work.

### 3. Wrapper-managed operations

These are operations whose cost is driven mainly by `nxpp`'s own bookkeeping
rather than by one delegated Boost algorithm.

Typical examples:

- `remove_node()`
- attribute accessors
- multigraph edge-id lookup/removal by id
- proxy-driven attribute writes

For these functions, the relevant guarantee is the `nxpp` public-call
complexity directly, not a Boost comparison.

## Current container policy

The repository is moving toward stronger, real complexity bounds where that can
be done without needlessly changing the dominant order of Boost algorithms.

Today, the public surface is mixed:

- some public results now use ordered `std::map`-backed storage
- some public results now use indexed wrappers built directly in sorted key order
- the core `NodeID -> vertex_descriptor` translation layer and external
  attribute stores are now ordered
- the wrapper index used for BGL normalization is now stored as a per-vertex property

So the right way to read the docs today is:

- if a function uses an ordered wrapper-owned map, count real `O(log n)`
  lookup/insert costs
- if a function uses an indexed wrapper materialized directly in key order,
  count `O(log n)` key lookup but keep the construction cost linear unless the
  wrapped algorithm itself dominates differently

## What “preserve Boost complexity” means here

For this project, preserving Boost complexity should be interpreted as:

- do not add a new multiplicative factor to the **algorithmic core** unless the
  wrapper truly needs it
- accept additive wrapper terms when the API intentionally materializes a richer
  result
- document both honestly

So these two statements are different:

- “same dominant order as Boost”
- “identical full-call complexity to raw Boost”

`nxpp` aims for the first whenever practical, not automatically the second.

## Practical examples

### Example: source-target Dijkstra path

- Boost core: Dijkstra
- `nxpp` wrapper work: source/target translation plus path reconstruction
- practical reading: same dominant order as Boost

### Example: single-source shortest-path result wrapper

- Boost core: Dijkstra or Bellman-Ford
- `nxpp` wrapper work: build `distance`, `predecessor`, and `paths`
- practical reading: same algorithmic core as Boost, plus explicit result
  materialization cost

### Example: component / centrality indexed wrapper

- Boost core: connected-components, strong-components, or degree accumulation
- `nxpp` wrapper work: build a key-sorted indexed result without hashing
- practical reading: same dominant order as Boost, with `O(log n)` key lookup
  on the returned wrapper

### Example: traversal predecessor / successor wrapper

- Boost core: BFS or DFS tree discovery
- `nxpp` wrapper work: group the discovered tree edges into sparse indexed
  predecessor / successor views
- practical reading: same dominant order as Boost traversal, with `O(log n)`
  lookup on the returned wrapper

### Example: internal wrapper index normalization

- Boost core: algorithms that require a vertex index map
- `nxpp` wrapper work: use the maintained per-vertex wrapper index property
- practical reading: no external hash map is needed on the core algorithm path

### Example: Floyd-Warshall map view

- Boost-style core: all-pairs shortest-path matrix work
- `nxpp` wrapper work: convert matrix output into nested maps
- practical reading: matrix complexity plus map-building overhead

### Example: `remove_node()`

- Boost core: vertex removal
- `nxpp` wrapper work: clear metadata, rebuild id/descriptor maps, rebuild
  maintained vertex indices
- practical reading: this is a wrapper-managed `O(n + m)` public operation

## Comparison tables

The tables below make the side-by-side comparison explicit:

- **Boost counterpart** names the closest raw Boost algorithm or primitive
- **Boost core complexity** is the complexity usually associated with that
  underlying Boost step
- **`nxpp` public-call complexity** is the documented end-to-end cost of the
  `nxpp` API call
- **Wrapper delta** explains where `nxpp` adds work or deliberately preserves
  the same dominant order

These tables are intentionally practical rather than exhaustive proofs. For the
complete `nxpp` API list, see [`API_REFERENCE.md`](API_REFERENCE.md).

When a lower-order wrapper term is fixed and clearly dominated, the table keeps
only the dominant term in the `nxpp` column and moves the lower-order detail
into the `Note` column.

The final column classifies the comparison like this:

- `yes`: same dominant asymptotic order as the Boost counterpart
- `no`: the `nxpp` public call can have a different dominant order
- `wrapper-managed`: there is no clean one-to-one Boost comparison, or the
  cost is driven mainly by `nxpp` bookkeeping

### Traversal

| `nxpp` function | Boost counterpart | Boost complexity | `nxpp` complexity | Boost-order status | Note |
|---|---|---|---|---|---|
| `bfs_edges(start)` | `boost::breadth_first_search` with tree-edge collection | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` materializes the final vector of discovered tree edges. |
| `bfs_tree(start)` | `boost::breadth_first_search` + tree construction | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` builds a new wrapper graph for the BFS tree. |
| `bfs_successors(start)` | `boost::breadth_first_search` with tree-edge discovery | `O(n + m)` | `O(n + m)` | `yes` | The result is indexed: linear construction, `O(log n)` key lookup. |
| `dfs_edges(start)` | `boost::depth_first_search` with tree-edge collection | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` materializes the final vector of discovered tree edges. |
| `dfs_tree(start)` | `boost::depth_first_search` + tree construction | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` builds a new wrapper graph for the DFS tree. |
| `dfs_predecessors(start)` | `boost::depth_first_search` with predecessor discovery | `O(n + m)` | `O(n + m)` | `yes` | The result is indexed: linear construction, `O(log n)` key lookup. |
| `dfs_successors(start)` | `boost::depth_first_search` with tree-edge discovery | `O(n + m)` | `O(n + m)` | `yes` | The result is indexed: linear construction, `O(log n)` key lookup. |
| `breadth_first_search(start, visitor)` | `boost::breadth_first_search` | `O(n + m)` | `O(n + m)` | `yes` | The wrapper only adds the initial `NodeID -> descriptor` translation. |
| `depth_first_search(start, visitor)` | `boost::depth_first_search` | `O(n + m)` | `O(n + m)` | `yes` | The wrapper only adds the initial `NodeID -> descriptor` translation. |

### Shortest paths

| `nxpp` function | Boost counterpart | Boost complexity | `nxpp` complexity | Boost-order status | Note |
|---|---|---|---|---|---|
| `shortest_path(source, target)` | unweighted Boost shortest-path primitives based on BFS | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` adds endpoint translation and path reconstruction. |
| `shortest_path_length(source, target)` | unweighted Boost shortest-path primitives based on BFS | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` adds endpoint translation. |
| `dijkstra_path(...)` | `boost::dijkstra_shortest_paths` | `O((n + m) log n)` | `O((n + m) log n)` | `yes` | Path reconstruction is bounded by `O(n)` and stays lower-order than the Dijkstra core. |
| `dijkstra_path_length(...)` | `boost::dijkstra_shortest_paths` | `O((n + m) log n)` | `O((n + m) log n)` | `yes` | `nxpp` adds endpoint translation only. |
| `dijkstra_shortest_paths(source)` | `boost::dijkstra_shortest_paths` | `O((n + m) log n)` | `O(max((n + m) log n, n^2))` | `no` | Ordered `distance` / `predecessor` materialization is `O(n log n)`. Reconstructing all stored paths costs `O(P)`, where `P` is the sum of reconstructed path lengths and can reach `O(n^2)` in the worst case. |
| `bellman_ford_path(...)` | `boost::bellman_ford_shortest_paths` | `O(nm)` | `O(nm)` | `yes` | Path reconstruction is bounded by `O(n)` and does not change the dominant term. |
| `bellman_ford_path_length(...)` | `boost::bellman_ford_shortest_paths` | `O(nm)` | `O(nm)` | `yes` | The final accumulation pass is `O(L)` with `L <= n - 1`, so it is lower-order and kept only in this note. |
| `bellman_ford_shortest_paths(source)` | `boost::bellman_ford_shortest_paths` | `O(nm)` | `O(max(nm, n^2))` | `no` | Ordered `distance` / `predecessor` materialization is `O(n log n)`. Reconstructing all stored paths costs `O(P)`, where `P` is the sum of reconstructed path lengths and can reach `O(n^2)` in the worst case. |
| `dag_shortest_paths(source)` | `boost::dag_shortest_paths` | `O(n + m)` | `O(max(n + m, n^2))` | `no` | Ordered `distance` / `predecessor` materialization is `O(n log n)`. Reconstructing all stored paths costs `O(P)`, where `P` is the sum of reconstructed path lengths and can reach `O(n^2)` in the worst case. |
| `floyd_warshall_all_pairs_shortest_paths()` | `boost::floyd_warshall_all_pairs_shortest_paths` | `O(n^3)` | `O(n^3)` | `yes` | The matrix view preserves the same dominant order. |
| `floyd_warshall_all_pairs_shortest_paths_map()` | `boost::floyd_warshall_all_pairs_shortest_paths` | `O(n^3)` | `O(n^3)` | `yes` | Converting the matrix into nested ordered maps adds `O(n^2 log n)`, which is lower-order than `O(n^3)`. |

### Components and centrality

| `nxpp` function | Boost counterpart | Boost complexity | `nxpp` complexity | Boost-order status | Note |
|---|---|---|---|---|---|
| `connected_components()` | `boost::connected_components` | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` builds an indexed `NodeID -> component_id` result in key order. |
| `strongly_connected_component_groups()` | `boost::strong_components` | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` groups SCCs into vectors after the Boost run without changing the dominant term. |
| `strong_components()` | `boost::strong_components` | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` builds an indexed `NodeID -> SCC root` result without adding `n log n`. |
| `degree_centrality()` | degree accumulation through BGL adjacency iteration | `O(n + m)` | `O(n + m)` | `yes` | `nxpp` materializes a normalized indexed result while keeping construction linear. |

### Spanning trees

| `nxpp` function | Boost counterpart | Boost complexity | `nxpp` complexity | Boost-order status | Note |
|---|---|---|---|---|---|
| `kruskal_minimum_spanning_tree()` | `boost::kruskal_minimum_spanning_tree` | `O(m log m)` | `O(m log m)` | `yes` | `nxpp` materializes MST edges as `NodeID` pairs. The table uses the standard Kruskal bound documented for the underlying Boost algorithm path. |
| `prim_minimum_spanning_tree(root)` | `boost::prim_minimum_spanning_tree` | `O((n + m) log n)` | `O((n + m) log n)` | `yes` | The ordered parent map adds `O(n log n)`, which stays lower-order than Prim's dominant term. The table uses the standard Prim bound documented for the underlying Boost algorithm path. |

### Flow

| `nxpp` function | Boost counterpart | Boost complexity | `nxpp` complexity | Boost-order status | Note |
|---|---|---|---|---|---|
| `maximum_flow(source, sink, ...)` | `boost::edmonds_karp_max_flow` | `F_maxflow(n, m)` | `O(n + m + F_maxflow(n, m))` | `yes` | `nxpp` adds auxiliary-graph construction and result materialization around the delegated Boost max-flow core. |
| `minimum_cut(source, sink, ...)` | max-flow + residual BFS | `O(n + m + F_maxflow(n, m))` | `O(n + m + F_maxflow(n, m))` | `yes` | Both sides include the residual BFS on top of the max-flow core. `nxpp` also materializes the cut partition and cut-edge list. |
| `max_flow_min_cost_cycle_canceling(...)` | min-cost-flow pipeline based on `boost::cycle_canceling` | `F_mincost_cc(n, m)` | `O(n + m + F_mincost_cc(n, m))` | `yes` | `nxpp` adds auxiliary-graph construction and result shaping. |
| `successive_shortest_path_nonnegative_weights(...)` | `boost::successive_shortest_path_nonnegative_weights` | `F_ssp(n, m)` | `O(n + m + F_ssp(n, m))` | `yes` | `nxpp` adds auxiliary-graph construction and result shaping. |
| `max_flow_min_cost(...)` | current default min-cost-flow wrapper path | `F_mincost_cc(n, m)` | `O(n + m + F_mincost_cc(n, m))` | `yes` | The current default still delegates to the cycle-canceling path, with linear auxiliary construction and result shaping on top. |

### Wrapper-managed operations

| `nxpp` function | Boost counterpart | Boost complexity | `nxpp` complexity | Boost-order status | Note |
|---|---|---|---|---|---|
| `add_node(id)` | `boost::add_vertex` | `O(1)` | `O(log n)` | `wrapper-managed` | The Boost-side figure is the default `vecS` insertion bound; `nxpp` maintains ordered `NodeID -> descriptor` translation and wrapper index metadata. |
| `has_node(id)` | no standalone Boost primitive beyond wrapper-owned lookup | `—` | `O(log n)` | `wrapper-managed` | Pure lookup in the ordered translation map. |
| `add_edge(u, v, ...)` | `boost::add_edge` | `—` | `O(log n + deg(u))` | `wrapper-managed` | Raw BGL insertion cost depends on graph configuration. For multigraph insertion, the public-call bound drops to `O(log n)`. `nxpp` adds endpoint translation and edge-id / attribute bookkeeping. |
| `remove_node(u)` | `boost::clear_vertex` + `boost::remove_vertex` | `—` | `O(n + m)` | `wrapper-managed` | Raw BGL removal cost depends on graph configuration and erase path. `nxpp` also clears wrapper metadata, translation maps, and maintained indices. |
| `clear()` | graph clear/reset primitives | `—` | `O(n + m)` | `wrapper-managed` | Raw BGL clear/reset cost is not treated here as a standalone contract. `nxpp` also resets attribute stores and tracked edge IDs. |
| `has_node_attr(u, key)` | no direct Boost counterpart; attributes are wrapper-owned | `—` | `O(log n + log A)` | `wrapper-managed` | Looks up attributes in ordered stores external to the BGL graph. |
| `get_edge_attr(...)` | no direct Boost counterpart; attributes are wrapper-owned | `—` | `O(log n + deg(u) + log m + log A)` | `wrapper-managed` | Includes edge resolution and ordered lookup in wrapper-owned attribute stores. The edge-id overload is `O(log m + log A)`. |

## Recommended wording style

When documenting or reviewing complexity for `nxpp`, prefer this style:

- first identify the Boost core when one exists
- then state the extra wrapper work explicitly
- finally give the full `nxpp` public-call complexity

That keeps the docs honest without hiding where `nxpp` intentionally trades
extra work for a more convenient C++ API.
