# nxpp - NetworkX for C++

**nxpp** is a native, *header-only* **C++20** library designed to teleport the unmistakable syntactic and semantic elegance of [NetworkX](https://networkx.org/) (Python) into the high-performance C++ ecosystem. Internally, it acts as a powerful wrapper around the battle-tested [Boost Graph Library (BGL)](https://www.boost.org/doc/libs/1_84_0/libs/graph/doc/index.html).

It was born with a single goal: offering a strictly "1-to-1" continuous interface for complex networks, allowing researchers and developers to natively prototype algorithms in Python and compile the exact mirror topology in C++ with virtually zero cognitive overhead.

## Build

Standard local build command:

```bash
g++ -std=c++20 -Wall -pedantic -O3 main.cpp -o main
```

For the Python parity baseline in `test_suite/`, `networkx` must also be available in the active Python environment.

## Current Status

- Phases 1-4 are implemented and verified.
- Phase 5 is now started: `degree_centrality` is implemented, while `maximum_flow`, `minimum_cut`, `betweenness_centrality`, `pagerank`, and benchmarking remain open.
- Stale and unused aliases were removed on March 25, 2026 during a repository audit to keep the public API cleaner.
- Implementation priority is now explicitly aligned to the local reference snippets in `snippet/` and `py_snippet/`.

## Priority Policy

The project now treats the local snippet sets as the primary source of priority:

- `snippet/` contains the C++ BGL reference algorithms
- `snippet/` now also contains colocated Python `networkx` translations beside each `.cpp` snippet
- `py_snippet/` contains the Python/NetworkX reference behavior
- functions present in those folders are considered the essential core surface
- functions not represented there should go into `TODO.md` and should not outrank snippet-backed essentials

The current prioritized backlog is tracked in [TODO.md](/home/mik/github/nxpp/TODO.md).

## How `nxpp.hpp` Works

The single header is organized around one central idea: expose a NetworkX-like API while storing the real graph in BGL.

`nxpp::Graph<NodeID, EdgeWeight, Directed, Multi>` is the core type. User-facing node identifiers remain `NodeID` values such as `std::string` or `int`, but the backend graph is a `boost::adjacency_list` using `boost::vecS` vertex storage.

Because BGL internally works with vertex descriptors rather than arbitrary user IDs, `nxpp` maintains two synchronized translation structures:

- `id_to_bgl`: maps a user node ID to the internal BGL vertex descriptor
- `bgl_to_id`: maps a BGL vertex descriptor back to the user node ID

This translation layer is what makes the higher-level API possible. Almost every operation in the file follows the same pattern:

1. Convert user node IDs into BGL descriptors.
2. Invoke the corresponding BGL operation.
3. Convert results back into `NodeID` values.

### Core Graph Behavior

- `add_node()` and `add_nodes_from()` route through an internal `get_or_create_vertex()` helper that creates the BGL vertex and updates both translation structures.
- `add_edge()` creates missing endpoints automatically. In simple graphs it overwrites the existing edge weight instead of duplicating the edge. In multigraphs it allows parallel edges.
- `nodes()`, `edges()`, `neighbors()`, `successors()`, `predecessors()`, and `get_edge_weight()` expose user-facing values by translating BGL descriptors back into `NodeID`.
- `clear()` resets the underlying BGL graph and descriptor maps.

### Destruction Semantics

`remove_node()` is the most delicate low-level method in the file. Because the graph uses `boost::vecS`, removing a vertex shifts all later vertex descriptors down by one. That means every stored descriptor after the removed vertex becomes stale immediately.

To keep the graph valid, `nxpp` performs a full re-synchronization of the `NodeID -> descriptor` mapping after `boost::remove_vertex()`. This is a critical invariant of the whole design.

Edge attributes are no longer keyed directly by `EdgeDesc`. The header now assigns a stable internal edge ID when an edge is created, and `edge_properties` is keyed by that stable ID instead. This avoids tying application-level metadata to a fragile BGL descriptor.

### NetworkX-like Access Syntax

The Python-like API is implemented with proxy objects rather than real nested dictionaries.

- `G[u][v] = w` works through `NodeProxy` and `EdgeProxy`
- `G[u][v]["key"] = value` works through `EdgeAttrProxy`
- `G.node(u)["key"] = value` works through `NodeAttrProxy`

Node and edge attributes are stored separately from the BGL graph:

- node attributes live in `node_properties`
- edge attributes live in `edge_properties`

Attribute values are stored as `std::any`, which gives Python-like flexibility at the cost of runtime casts on retrieval.

The proxy syntax is still available, but the header now also exposes explicit safer helpers:

- `has_node_attr()` and `has_edge_attr()`
- `get_node_attr<T>()` and `get_edge_attr<T>()`
- `try_get_node_attr<T>()` and `try_get_edge_attr<T>()`

These helpers make attribute reads clearer and avoid relying exclusively on implicit proxy conversions.

The proxy layer also provides explicit `const char*` overloads for chained `operator[]` usage, which helps editor tooling resolve expressions like `G["u"]["v"]["key"]` more reliably.

### Algorithms

The algorithms in the header are wrappers around BGL. The current structure is:

- traversal wrappers: `bfs_edges()`, `bfs_tree()`, `dfs_edges()`, and `dfs_tree()`
- shortest paths: `shortest_path()`, `shortest_path_length()`, `dijkstra_path()`, `dijkstra_path_length()`, `bellman_ford_path()`, `bellman_ford_path_length()`, `dag_shortest_paths()`, and `floyd_warshall_all_pairs_shortest_paths()`
- components: `connected_components()` and `strongly_connected_components()`
- DAG and ordering: `topological_sort()`
- spanning trees: `kruskal_minimum_spanning_tree()` and `prim_minimum_spanning_tree()`
- flow: `maximum_flow()`, `minimum_cut()`, `max_flow_min_cost_cycle_canceling()`, and `max_flow_min_cost_successive_shortest_path()`
- SCC extras: `strongly_connected_component_roots()`
- SAT helper: `two_sat_satisfiable()`
- generators: `complete_graph()`, `path_graph()`, and `erdos_renyi_graph()`
- centrality: `degree_centrality()`

Each wrapper translates IDs to descriptors, runs the BGL algorithm, and then converts the output back into `NodeID`-based containers.

`shortest_path()` is now split more explicitly by intent:

- `shortest_path(G, source, target)` uses an unweighted BFS-style path by edge count
- `shortest_path_length(G, source, target)` returns that unweighted distance
- `shortest_path(G, source, target, "weight")` routes to the weighted path implementation
- `shortest_path_length(G, source, target, "weight")` routes to the weighted distance implementation
- weighted wrappers currently support only the built-in edge weight property named `"weight"`

## Current Design Caveats

These are the main technical constraints and risks currently visible in `include/nxpp.hpp`:

- edge metadata now uses a stable internal edge ID instead of `EdgeDesc`, which removes the worst descriptor-stability issue from the previous design.
- `clear()` now resets graph structure, node attributes, edge attributes, and internal edge ID state together.
- `remove_edge()` and `remove_node()` now explicitly clean associated edge metadata before removing graph structure.
- `neighbors()` remains successor-based for directed graphs, which matches `DiGraph.neighbors()` semantics in NetworkX.
- `successors()` and `predecessors()` now exist explicitly to make directed traversal intent clearer in user code.
- The algorithm wrappers are still intentionally thin. Basic missing wrapper names have been filled in, and `shortest_path()` now distinguishes unweighted and weighted calls, but the wider NetworkX option surface is still not implemented.
- `std::any` still gives flexible attributes, but attribute access is now less fragile because explicit checked getters and optional-return helpers exist alongside the proxy syntax.
- `maximum_flow()` currently expects edge capacities to live in the edge attribute dictionary under `"capacity"` and returns both total flow value and per-edge flow on original edges.
- `minimum_cut()` uses the same `"capacity"` convention and returns the cut value, the reachable/non-reachable partition, and the cut edges in the original graph.
- min-cost max-flow wrappers currently expect capacities in `"capacity"` and costs in the built-in edge weight property.
- GCC-specific false-positive warnings from Boost min-cost-flow internals are now contained locally in the header so normal project builds stay readable.

## Internal Invariants

The current header depends on a few invariants staying true:

- `id_to_bgl` and `bgl_to_id` must always describe the same graph state
- every algorithm wrapper assumes those mappings are valid before it calls BGL
- any operation that changes vertex indices must rebuild the forward mapping immediately
- user-facing containers should always return `NodeID` values, never raw BGL descriptors


---

---


## Prerequisites & Installation

**nxpp** is a pure header-only library, which means there are absolutely no `.cpp` object files to compile or link. You simply `#include "nxpp.hpp"` in your architecture.

However, `nxpp` is a highly-optimized structural wrapper and strictly requires the **Boost Graph Library (BGL)** to function.

**Ubuntu / Debian:**
```bash
sudo apt-get install libboost-graph-dev
```

**macOS (Homebrew):**
```bash
brew install boost
```

**Windows (vcpkg):**
```bash
vcpkg install boost-graph
```

*Note: The `nxpp.hpp` header inherently utilizes a compile-time safety check (`__has_include`). If Boost is missing or incorrectly linked in your standard include directories, the C++ compiler will safely halt and boldly alert you immediately.*

---

## Graph Topologies & Theory

In mathematical graph theory, the semantics of connections dictate the solution space. `nxpp` faithfully implements and maps the 4 core architectures defining NetworkX:

| `nxpp` Class | Python Equivalent | Graph Theory Description |
| :--- | :--- | :--- |
| **`nxpp::Graph`** | `nx.Graph` | **Undirected Simple Graph**: Edges have no direction and can be traversed both ways ($A \leftrightarrow B$). There is *at most one strict edge* between any pair of nodes. |
| **`nxpp::DiGraph`** | `nx.DiGraph` | **Directed Simple Graph**: Edges act as rigid directional vectors with a semantic origin and a specific destination ($A \rightarrow B$). |
| **`nxpp::MultiGraph`** | `nx.MultiGraph` | **Undirected Multigraph**: The container mathematically permits **parallel edges** between the exact same two nodes. |
| **`nxpp::MultiDiGraph`** | `nx.MultiDiGraph` | **Directed Multigraph**: Parallel edges equipped with a rigorous directional vector. |

---

## Python vs C++: Parity and Operations

The lifeblood of **nxpp** is its **API Proxy Parity** and heavy utilization of **C++ Generic Templates**.
Structural node identifiers (`NodeID`) are not opaque RAM memory indices (as traditionally enforced by C and BGL), but can dynamically take the visible form of any software entity that is *Hashable* (e.g., `std::string`, `int`, custom Classes), mimicking precisely the behavior of Python dictionaries.

### The Same Algorithm, in Two Languages

A practical example of the intense code overlap:

#### 1. Prototyping in NetworkX (Python)
```python
import networkx as nx

# Directed Graph Instantiation (Default: string-based nodes)
G = nx.DiGraph()

# Organic edge creation
G.add_edge("Milan", "Rome", weight=5.0)
G.add_edge("Rome", "Naples", weight=2.5)

# Proxy Dictionary: O(1) constant time edge Read/Write
G["Rome"]["Naples"]["weight"] = 3.0

# Neighbor iteration
print("Neighbors of Rome:")
for node in G.neighbors("Rome"):
    print("-", node)

# Shortest Pathfinding Algorithm
path = nx.dijkstra_path(G, "Milan", "Naples", weight="weight")
```

#### 2. The Exact Network in NXPP (C++20)
```cpp
#include "nxpp.hpp"

// Directed Graph Instantiation. (Under the hood it's actually Graph<std::string, double, true>)
auto G = nxpp::DiGraph();

// Organic edge creation (nxpp automatically generates BGL mappings seamlessly)
G.add_edge("Milan", "Rome", 5.0);
G.add_edge("Rome", "Naples", 2.5);

// C++ Proxy Dictionary: Magical overload of operator[][] mimicking Python in O(1) time
G["Rome"]["Naples"] = 3.0;

// Python-like variadic logger macro printing cleanly to console
nxpp::print("Neighbors of Rome:");
for (const auto& node : G.neighbors("Rome")) {
    nxpp::print("-", node);
}

// Shortest Pathfinding Algorithm
auto path = nxpp::dijkstra_path(G, std::string("Milan"), std::string("Naples"));

// Topological Generators (Phase 3)
auto K5 = nxpp::complete_graph(5); // Returns Graph<int> by default
auto P4 = nxpp::path_graph<nxpp::DiGraph>(4); // Path 0->1->2->3 as DiGraph

// Centrality (Phase 5 kickoff)
auto centrality = nxpp::degree_centrality(K5);

// Custom Node/Edge Attributes (std::any)
G.node("Rome")["population"] = 2800000;
G["Rome"]["Naples"]["km"] = 220;
int pop = G.node("Rome")["population"];
```
---

### Public Type Aliases

To keep snippets and user code compact, `nxpp` exposes public aliases for common graph shapes:

- `GraphInt` for `Graph<int>`
- `GraphStr` for `Graph<std::string>`
- `DiGraphInt` for `Graph<int, double, true>`
- `DiGraph` for `Graph<std::string, double, true>`
- `MultiGraphInt`, `MultiDiGraphInt`, `MultiGraph`, and `MultiDiGraph` for the multigraph variants




---


### Critical Under-the-Hood Differences

While perfectly emulating NetworkX—such as reproducing built-in node destruction APIs (`clear()`, `remove_node(u)`, `remove_edge(u,v)`) by dynamically maneuvering BGL fragments deep in memory via a smart algorithmic *O(1) memory-swap* swap—migrating to a statically typed systems language undeniably introduces structural differences to prevent segmentation faults:

1. **Strong Type Safety Constraints**: 
   In interpreted Python, a careless engineer might insert an integer into a graph layout, and subsequently assign an arbitrary floating-point edge mixed with textual boolean comparisons (`G.add_node(1); G.add_node("A");`). 
   In **nxpp**, the node identifier type is securely locked down by the compiler Template directly on line one. If you compile an `nxpp::DiGraph` (or explicitly declare `nxpp::Graph<std::string>`), the *Hash* Map keys will strictly resolve to `std::string`. Zero structural ambiguity translates to incredible runtime stability and mathematically non-existent debug tracking.

2. **Custom Attributes via `std::any`**:
   NetworkX is famous for its "attribute dictionaries" on nodes and edges.
   **nxpp** implements this same flexibility using `std::any`. You can store strings, integers, or even custom structs directly inside `G.node(u)["key"]` or `G[u][v]["key"]`. Retrieval is type-safe via implicit or explicit casting, providing Python-like dynamism with C++ type integrity.

3. **Solid Data Structures (Vectors) vs Lazy Generators (Python Yield)**:
   In Python, commands corresponding to `edges()`, `nodes()` or `nx.bfs_edges` exclusively spit out *Lazy Generators* sequentially to compensate for horrendous dynamic execution times and stave off garbage collector memory exhaustion limits.
   Conversely, in `nxpp`, BGL iterative methods directly graze precompiled L1-L2 hardware silicon array caches. Therefore, it is literally orders of magnitude faster to return completely compact, contiguous RAW structural blocks outright (`std::vector<NodeID>`), allowing highly hyper-performant iteration blocks and calculations without risking the corruption or stalling of the precompiled processor pipeline mechanisms.

### Snippet Layout

The `snippet/` tree is now organized as a three-way reference set per algorithm:

- `*.cpp`: original Boost Graph Library example
- `*.py`: matching `networkx` translation
- `*_nxpp.cpp`: matching C++ version built on top of `include/nxpp.hpp`

The intent is that all three variants produce the same observable result for the same example. Python runs should use the repo virtualenv (`.venv/bin/python -B`) so `networkx` is available and no fresh `__pycache__` files are emitted during checks.

A small shell harness now lives at `scripts/test_snippets.sh`.
At the moment it covers `2sat`, `bellman_ford`, `bfs`, and `cc`, compiling the Boost and nxpp C++ snippets, running the Python translation, measuring compile/run times, and writing a live report both to the terminal and to a timestamped log file under `logs/`.
To keep the report readable, the script currently compiles the non-nxpp reference snippets with warnings silenced, while nxpp snippet builds still use the normal warning set.

For traversal and shortest-path examples, `nxpp` now supports both styles:

- Boost-like visitor-object style for event-driven snippets:
  `breadth_first_search(G, source, visitor)` and `depth_first_search(G, source, visitor)`
- NetworkX-like result style for data-driven snippets:
  `bfs_successors(G, source)`, `dfs_predecessors(G, source)`, `dfs_successors(G, source)`, `single_source_dijkstra(G, source)`, `single_source_bellman_ford(G, source)`, and `single_source_dag_shortest_paths(G, source)`

The single-source shortest-path helpers return distances, predecessors, and full reconstructed paths so snippets can stay close to the corresponding NetworkX examples without rebuilding parent maps by hand.
For visitor-style traversals, `nxpp` now provides a single generic base class:
`default_graph_visitor<GraphWrapper>`.
It exposes no-op `examine_vertex`, `tree_edge`, and `back_edge` hooks, so user code can derive once and override only the callbacks relevant to BFS or DFS.

For component algorithms, `nxpp` now supports both grouped and mapped results:

- `connected_components(G)` / `strongly_connected_components(G)` return vectors of groups
- `connected_component_map(G)` / `strongly_connected_component_map(G)` return `node -> component_id` maps when that form is more convenient for snippets or diagnostics

These mapped results use a small `nxpp` wrapper so read access works naturally with either `result[key]` or `result.at(key)`, even when the map object itself is `const`.
