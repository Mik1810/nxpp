# nxpp — NetworkX-inspired graph utilities for modern C++

Current release: `v0.5.0` (March 29, 2026)

<p align="center">
  <img src="imgs/logo.svg" alt="nxpp logo" width="220">
</p>

`nxpp` is a **header-only C++20** library built on top of the **Boost Graph Library (BGL)**.

It does **not** try to reimplement all of NetworkX.  
It aims to provide a **small, practical, C++-friendly graph API** that feels closer to NetworkX in day-to-day use while still delegating most graph work to Boost.

The core abstraction is:

```cpp
nxpp::Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>
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

`nxpp` is now released as `v0.5.0`.

> [!WARNING]
> The public API is still settling.
> The biggest open work is no longer the critical multigraph block, but the
> next round of formal testing, packaging cleanup, and support-matrix clarity.

Today, the project is strongest as:

- a **header-only wrapper over BGL**
- a **NetworkX-inspired convenience layer**
- a library that often returns **materialized C++ results**
- a project with a **snippet-based parity / regression harness**
- a codebase whose next big quality step is a **real assertion-based test suite**

The most important open issue groups right now are:

- documentation/source-of-truth/testing-story cleanup: `#29`, `#30`, `#31`
- broader formal verification beyond snippet parity: `#8`, `#9`, `#10`, `#11`, `#12`
- packaging / release ergonomics after the first versioned releases

Detailed API tables now live in [`docs/API_REFERENCE.md`](docs/API_REFERENCE.md).

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
In particular:

- `has_edge(u, v)` means "at least one parallel edge exists"
- `remove_edge(u, v)` removes all parallel edges between `u` and `v`
- `get_edge_weight(u, v)`, `get_edge_attr(u, v, key)`, and `G[u][v]` resolve through a single edge returned by `boost::edge(u, v, g)`, so they do not identify a specific parallel edge in a stable public way
- `add_edge(..., attrs)` on multigraphs attaches attributes through that same `(u, v)` resolution path, so attribute assignment is also not yet a precise per-parallel-edge API

Treat multigraph mutation / lookup as a **known caveat**, not as a fully polished API.

For precise multigraph work, prefer the explicit `edge_id` API:

- create a specific edge with `add_edge_with_id(...)`
- enumerate parallel edges with `edge_ids(u, v)`
- inspect a specific edge with `get_edge_endpoints(edge_id)`, `get_edge_weight(edge_id)`, `get_edge_attr(edge_id, key)`
- mutate a specific edge with `set_edge_weight(edge_id, ...)`, `set_edge_attr(edge_id, ...)`, and `remove_edge(edge_id)`

### 3. `remove_node()` is intentionally not cheap

`nxpp` now supports custom `OutEdgeSelector` and `VertexSelector` values on `Graph<...>`,
but destructive updates still require wrapper-side bookkeeping.

In particular, `remove_node()` must also:

- clear incident edge metadata
- erase node metadata
- rebuild the NodeID <-> descriptor map
- rebuild the maintained vertex-index map used by wrapper algorithms

In practice, this is still an **`O(V + E)` public operation**.

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

- [`README.md`](README.md): user-facing overview, quick start, caveats, and document navigation
- [`TODO.md`](TODO.md): compact local priority index; GitHub Issues are the full backlog source of truth
- [`CHANGELOG.md`](CHANGELOG.md): dated change history
- [`SESSION.md`](SESSION.md): historical development log
- [`docs/README.md`](docs/README.md): index for longer-form and future generated documentation
- [`docs/API_REFERENCE.md`](docs/API_REFERENCE.md): detailed public API tables and technical reference
- [`docs/API_ARCHITECTURE.md`](docs/API_ARCHITECTURE.md): public API placement policy for graph methods and namespace-scope helpers
- [`docs/GRAPH_CONFIGURATION.md`](docs/GRAPH_CONFIGURATION.md): supported BGL configurability surface and advanced-knob policy

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

## Graph configuration policy

`nxpp` intentionally exposes a narrow graph-configuration surface.

Today, the supported public knobs are:

- `NodeID`
- `EdgeWeight`
- `Directed`
- `Multi`
- `Weighted`
- `OutEdgeSelector`
- `VertexSelector`

The existing aliases such as `WeightedDiGraphInt`, `Graph`, and `MultiGraph` still use the standard `boost::vecS` / `boost::vecS` backend.
Advanced users can override selectors by instantiating `Graph<...>` directly.

Today, selector customization is broader than the default aliases, but it is still not "anything goes":

- `OutEdgeSelector` can be changed for advanced use cases such as `boost::listS` or `boost::setS`
- `VertexSelector` can also be changed for advanced use cases such as `boost::listS` or `boost::setS`
- `Multi=true` is rejected with `boost::setS` because `setS` suppresses parallel edges by design

For example:

```cpp
nxpp::WeightedDiGraphInt G_default;
nxpp::Graph<int, int, true, false, true, boost::listS, boost::listS> G_custom;
```

See [`docs/GRAPH_CONFIGURATION.md`](docs/GRAPH_CONFIGURATION.md) for the full policy, rationale, and scoped future extension path.

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

The repo now keeps three aligned showcase entry points:

- `main_boost.cpp` for the raw Boost version
- `main_nxpp.cpp` for the `nxpp` wrapper version
- `main.py` as the NetworkX/Python companion to the two C++ demos

The assertion-based test suite now has a dedicated runner:

```bash
bash scripts/run_tests.sh
```

and a dedicated GitHub Actions workflow:

- `.github/workflows/tests.yml`

That workflow is intentionally narrow: it installs Boost, runs the formal test suite, and publishes the test output as a Markdown job summary.

These are showcase demos, not formal tests or parity harnesses.

There is now also a minimal assertion-based test entry point:

- `tests/test_core.cpp`
- `tests/test_attributes.cpp`
- `tests/test_edge_cases.cpp`
- `tests/test_flow.cpp`
- `tests/test_remove_node.cpp`
- `tests/test_multigraph.cpp`
- `scripts/run_tests.sh`

That test binary is intentionally small for now, but it gives the project a real
formal test path in addition to snippets, showcases, and benchmarks.

The multigraph-specific part of the suite now checks the exact behavior that the
recent critical issue work stabilized:

- distinct `edge_id` values for parallel edges
- precise single-edge removal with `remove_edge(edge_id)`
- all-edge removal with `remove_edge(u, v)`
- per-edge attribute separation across parallel edges
- consistent endpoint lookup after partial multigraph removal

The attribute-specific part of the suite now also checks failure behavior:

- missing node/edge attribute lookups that should throw
- type mismatches that should throw
- `try_get_*_attr(...)` returning empty on missing keys or wrong types
- numeric edge-attribute lookup rejecting non-numeric stored values

The edge-case part of the suite now checks common boundary conditions:

- empty graphs returning empty collections and component views
- singleton graphs producing empty neighbor/traversal results
- missing-node operations throwing in a defined way
- disconnected shortest paths preserving unreachable distances and missing paths
- disconnected component grouping staying stable across separate subgraphs

The flow-specific part of the suite now checks the wrapper layer around the
small reference flow networks already used in the snippets:

- `maximum_flow(...)`
- `minimum_cut(...)`
- `push_relabel_maximum_flow(...)`
- `cycle_canceling(...)`
- `successive_shortest_path_nonnegative_weights(...)`
- `max_flow_min_cost(...)` and `max_flow_min_cost_successive_shortest_path(...)`

The `remove_node()` regression part of the suite now checks the descriptor-remap
area directly:

- node and edge views after removing a middle vertex
- cleanup of node and incident edge attributes
- traversal and shortest-path behavior after internal remapping
- component views after node removal
- multigraph cleanup of incident `edge_id` state

String attributes passed as `"..."` are normalized internally, so typed reads like
`get_node_attr<std::string>(...)` and `get_edge_attr<std::string>(...)` work without
having to spell `std::string(...)` at the call site.

---

## Build and requirements

`nxpp` is header-only, but it depends on **Boost Graph Library**.

### Minimal local build

```bash
g++ -std=c++20 -Wall -Wextra -pedantic -O3 main_boost.cpp -o main_boost
g++ -std=c++20 -Wall -Wextra -pedantic -O3 main_nxpp.cpp -o main_nxpp
python3 main.py
bash scripts/run_tests.sh
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
        {"capacity", 8},
        {"company", "Trenitalia"}
    });
    G.add_edge("Rome", "Naples", 2.5);
    G.add_edge("Milan", "Florence", 2.0);
    G.add_edge("Florence", "Naples", 4.0);

    G.node("Rome")["population"] = 2800000;

    auto routes = G.dijkstra_shortest_paths("Milan");
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
}
```

This shows the three main ideas of the library:

- graph mutation through a small wrapper API
- checked node / edge attribute access
- richer result wrappers such as `dijkstra_shortest_paths()`

---

## Internal model in one minute

`nxpp` keeps these synchronized structures:

- `id_to_bgl`: `NodeID -> vertex_descriptor`
- `bgl_to_id`: index-ordered `NodeID` storage used for normalized wrapper results
- a maintained `vertex_index_map`: `vertex_descriptor -> stable wrapper index`

This lets the public API use `std::string`, `int`, or other hashable node IDs while running algorithms on a normal BGL graph internally.

### Consequence

The backend now uses:

```cpp
boost::adjacency_list<OutEdgeSelector, VertexSelector, ...>
```

The default aliases still resolve to `boost::vecS` / `boost::vecS`,
but direct `Graph<...>` instantiation can now use non-default selectors.

That flexibility means `nxpp` cannot rely on descriptor-as-index assumptions anymore,
so the wrapper maintains its own vertex index normalization layer.
That is why some public-call costs differ from the textbook complexity of the wrapped algorithm alone.

---

## Further documentation

The root README is intentionally kept focused on overview, quick start, caveats, and repository navigation.

For deeper technical detail, use:

- [`docs/API_REFERENCE.md`](docs/API_REFERENCE.md): detailed API tables, aliases, proxy syntax, result wrappers, and algorithm reference
- [`docs/API_ARCHITECTURE.md`](docs/API_ARCHITECTURE.md): policy for graph methods vs namespace-scope helpers
- [`docs/GRAPH_CONFIGURATION.md`](docs/GRAPH_CONFIGURATION.md): graph selector/configuration policy
- [`docs/README.md`](docs/README.md): docs index

The most important topics moved out of the README are:

- public API tables and complexity notes
- alias/reference detail
- traversal / shortest-path / flow reference tables
- proxy syntax detail
- result-wrapper reference
- repository layout / parity harness detail
- deeper caveat and trade-off sections

---

## What is still clearly open

The most important open work visible from the repository today is:

- a real assertion-based test suite
- stronger edge-case coverage
- packaging / install story beyond manual header inclusion
- clearer documentation generation and source-of-truth discipline
- a minimal compiler/platform support matrix in the README
- eventual API stabilization

See [`TODO.md`](TODO.md) for the open list.

---

## License

This project is licensed under the MIT License. See [`LICENSE`](LICENSE).
