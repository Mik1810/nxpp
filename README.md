# nxpp — NetworkX-inspired graph utilities for modern C++

Current release: `v0.8.6` (April 5, 2026)

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

`nxpp` is now released as `v0.8.6`.

> [!WARNING]
> The public API is still settling.
> The biggest open work is no longer the critical multigraph block or the first
> formal-test block, but the next round of documentation cleanup, generated-docs
> scaffolding, packaging polish, and broader CI / usage clarity.

Today, the project is strongest as:

- a **header-only wrapper over BGL**
- a **NetworkX-inspired convenience layer**
- a library that often returns **materialized C++ results**
- a project with a **snippet-based parity / regression harness**
- a codebase that now also has a **real assertion-based test suite with CI**

The most important open issue groups right now are:

- API safety / attribute-system follow-up: `#25`, `#27`
- documentation-source-of-truth cleanup: `#29`
- packaging / CI follow-up: none immediately blocking

The generated API reference now lives in the Doxygen site published from the
public headers:

- `https://mik1810.github.io/nxpp/`

[`docs/API_REFERENCE.md`](docs/API_REFERENCE.md) remains as a curated markdown
companion for alias tables, proxy syntax, result shapes, and selected API
notes rather than a second full declaration dump.
Runtime validation errors now use a more consistent `X failed: ...` wording
across the public surface.
The repo now also has a minimal Doxygen scaffold for generated API reference
under [`Doxyfile`](Doxyfile) and [`docs/DOXYGEN_MAINPAGE.md`](docs/DOXYGEN_MAINPAGE.md).
A dedicated GitHub Pages workflow can also publish that generated reference as
a static site when the repository Pages source is set to `GitHub Actions`.

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

Where `nxpp` now materializes ordered map-like public results, those notes also
count the real `O(log n)` insertion/lookup behavior of the underlying
`std::map` storage instead of treating hash-table behavior as expected `O(1)`.
Some other results now use indexed wrappers that keep linear materialization
cost while still giving `O(log n)` key lookup. The detailed API reference calls
out which helpers use ordered maps and which use indexed wrappers.

See [`docs/COMPLEXITY.md`](docs/COMPLEXITY.md) for the fuller distinction
between Boost's algorithmic complexity and `nxpp`'s full public-call cost.

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
- Doxygen site / [`Doxyfile`](Doxyfile): declaration-driven API reference generated from the public headers
- [`docs/README.md`](docs/README.md): index for longer-form and generated documentation
- [`docs/API_REFERENCE.md`](docs/API_REFERENCE.md): curated markdown companion for aliases, proxies, result shapes, and selected API notes
- [`docs/API_ARCHITECTURE.md`](docs/API_ARCHITECTURE.md): public API placement policy for graph methods and namespace-scope helpers
- [`docs/GRAPH_CONFIGURATION.md`](docs/GRAPH_CONFIGURATION.md): supported BGL configurability surface and advanced-knob policy
- [`docs/COMPLEXITY.md`](docs/COMPLEXITY.md): complexity policy and Boost-vs-`nxpp` cost model
- [`docs/EXTERNAL_USAGE.md`](docs/EXTERNAL_USAGE.md): how to consume `nxpp` from another project today
- [`docs/TEST.md`](docs/TEST.md): testing layers, test commands, and verification scope

The external-consumption story now also includes a minimal root
[`CMakeLists.txt`](CMakeLists.txt) for vendored `add_subdirectory(...)` usage,
in addition to the plain compiler examples documented in
[`docs/EXTERNAL_USAGE.md`](docs/EXTERNAL_USAGE.md).

If these files disagree:

- the generated Doxygen site wins on declaration-level API facts
- `README.md` describes the current user-facing reality and document navigation
- `TODO.md` describes what is still open

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

`NodeID` now has an explicit compile-time contract too:

- it must be copy-constructible
- it must support equality comparison
- it must be orderable through `std::less`

That is the contract used by the wrapper-owned translation maps, ordered result
wrappers, and on-demand shortest-path reconstruction.

The free graph generators (`complete_graph`, `path_graph`, and
`erdos_renyi_graph`) have one extra requirement on top of that:

- `NodeID` must be constructible from `std::size_t`, because those helpers
  synthesize node IDs `0..n-1`

So `Graph<std::string>` remains a valid graph type, but the numeric generator
helpers are intentionally aimed at integer-like node IDs unless you provide a
custom generator path yourself.

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

The canonical umbrella header is `include/nxpp.hpp`.

The repo now also has a semantic include layout under `include/nxpp/`:

- `nxpp/graph.hpp`
- `nxpp/attributes.hpp`
- `nxpp/traversal.hpp`
- `nxpp/shortest_paths.hpp`
- `nxpp/components.hpp`
- `nxpp/spanning_tree.hpp`
- `nxpp/flow.hpp`
- `nxpp/generators.hpp`
- `nxpp/multigraph.hpp`
- `nxpp/sat.hpp`

That split is now real for the main public surface:

- `nxpp/graph.hpp` owns the core `Graph` type, its base structural operations,
  and the public graph aliases
- `nxpp/attributes.hpp` owns the attribute-aware overloads and checked
  attribute accessors
- `nxpp/multigraph.hpp` owns the precise `edge_id`-based multigraph API
- `nxpp/traversal.hpp` owns the traversal visitors, deprecated BFS/DFS wrappers,
  and the `Graph` traversal method definitions
- `nxpp/shortest_paths.hpp` owns the shortest-path helpers, deprecated shortest-path wrappers,
  and the `Graph` shortest-path method definitions
- `nxpp/components.hpp` owns the component helpers, deprecated component wrappers,
  and the `Graph` component method definitions
- `nxpp/spanning_tree.hpp` owns topological sort, minimum-spanning-tree helpers,
  deprecated wrappers, and the matching `Graph` method definitions
- `nxpp/flow.hpp` owns flow/cut/min-cost helpers, deprecated wrappers,
  supporting result types, and the matching `Graph` method definitions
- `nxpp/generators.hpp` owns `complete_graph`, `path_graph`, and
  `erdos_renyi_graph`
- `nxpp/sat.hpp` owns the 2-SAT helpers

`include/nxpp.hpp` is the real umbrella include for the modular public surface.

That means both of these forms currently work:

```cpp
#include "include/nxpp/flow.hpp"
```

```cpp
#include "include/nxpp.hpp"
```

The repository also now has a small helper script for rebuilding a distributable
single header from the umbrella rooted at `include/nxpp.hpp`:

```bash
bash scripts/unix/build_single_header.sh
```

That command now generates a standalone `dist/nxpp.hpp` by expanding local
`nxpp` headers recursively while leaving standard-library and Boost includes
alone. The modular headers remain the source of truth; the generated file is a
distribution artifact and is intentionally not versioned in git.

The repo now keeps three aligned showcase entry points:

- `main_boost.cpp` for the raw Boost version
- `main_nxpp.cpp` for the `nxpp` wrapper version
- `main.py` as the NetworkX/Python companion to the two C++ demos

The assertion-based test suite now has a dedicated runner:

```bash
bash scripts/unix/run_tests.sh
```

Use [`docs/TEST.md`](docs/TEST.md) as the source of truth for the testing
story. It now separates:

- showcase programs
- snippet parity / regression
- formal assertion-based tests
- single-header validation
- large-graph raw-Boost comparison

There is also an opt-in large-graph comparison runner:

```bash
bash scripts/unix/run_large_graph_compare.sh
```

That path compiles `tests/test_large_graph_compare.cpp`, generates
deterministic larger graph instances, and compares `nxpp` against raw Boost for:

- BFS depth results
- DFS tree edges
- connected-component partitions
- strongly connected-component partitions
- Dijkstra distance maps
- Bellman-Ford distance maps
- DAG shortest-path distance maps
- reachable negative-cycle detection behavior
- post-`remove_node()` graph state on large graphs
- large multigraph mutation and cleanup behavior
- large attribute preservation and cleanup after repeated mutations
- large combined weighted-graph mutation sequences with raw-Boost comparison
- large Floyd-Warshall all-pairs agreement with a raw-Boost all-pairs baseline
- large maximum-flow / minimum-cut agreement with Boost
- large successive-shortest-path min-cost-flow agreement with Boost

It is intentionally kept outside `bash scripts/unix/run_tests.sh` so the default
formal suite stays fast while the larger comparison path remains available when
you want extra confidence on wrapper behavior at scale.

The large-graph driver now also re-runs representative BFS, connected-component,
strongly-connected-component, Dijkstra, and `remove_node()` comparisons across
multiple fixed seeds instead of relying on one seed per scenario, and it
includes a non-default `boost::listS` / `boost::listS` selector regression so
the comparison path exercises more of the supported wrapper configuration
surface than the default aliases alone.

and a dedicated GitHub Actions workflow:

- `.github/workflows/compatibility.yml`

There is also a dedicated workflow for the generated standalone header:

- `.github/workflows/single-header.yml`
- `.github/workflows/large-graph-compare.yml`
- `.github/workflows/release.yml`

The formal-test workflow is intentionally narrow: it installs Boost, runs the
formal test suite, and publishes the test output as a Markdown job summary.

The standalone-header workflow is equally narrow in scope:

- runs `bash scripts/unix/build_single_header.sh`
- smoke-tests the generated `dist/nxpp.hpp`
- uploads the generated header as a workflow artifact

The large-graph comparison workflow is dedicated to the new scale-oriented
verification path:

- runs `bash scripts/unix/run_large_graph_compare.sh` against the modular headers
- builds `dist/nxpp.hpp`
- reruns the same large-graph comparison against the generated single header
- publishes both outputs as a separate Markdown job summary

The release workflow is intentionally separate from normal pushes to `main`.

It can start in two ways:

- a pushed `vX.Y.Z` tag
- `workflow_dispatch`

In both cases it:

- reads the top version from `RELEASE_NOTES.md` and `CHANGELOG.md`
- fails if those top versions do not match
- builds `dist/nxpp.hpp`
- runs `bash scripts/unix/run_tests.sh`
- runs `bash scripts/unix/run_single_header_tests.sh` against `dist/nxpp.hpp`
- creates the GitHub release from `RELEASE_NOTES.md`
- uploads the tested file as `nxpp.hpp`

When started by `workflow_dispatch`, the workflow is self-contained:

- it creates and pushes the matching `vX.Y.Z` tag only after the release checks pass
- then continues in the same run to publish the release

When started by a pushed `vX.Y.Z` tag, it uses that tag directly and verifies
that it matches the top documented version.

So the top version in `RELEASE_NOTES.md` and `CHANGELOG.md` should be treated as
the concrete next release candidate, but a normal push to `main` alone should
not publish a release.

These are showcase demos, not formal tests or parity harnesses.

The `*_nxpp.cpp` snippets now also follow the semantic-header split instead of
pulling in the full umbrella header by default. For example:

- traversal snippets include `nxpp/traversal.hpp`
- shortest-path snippets include `nxpp/shortest_paths.hpp`
- component snippets include `nxpp/components.hpp`
- flow snippets include `nxpp/flow.hpp`
- the topological-sort snippet includes `nxpp/topological_sort.hpp`

This keeps the snippet layer closer to the real modular include story and makes
compile-time comparisons more meaningful.

The intended split is:

- `main_*` files are showcase demos
- `snippet/` is the curated example-plus-parity layer
- `tests/` is the formal regression suite
- `test_large_graph_compare.cpp` is the separate large deterministic raw-Boost comparison path

There is now also a minimal assertion-based test entry point:

- `tests/test_core.cpp`
- `tests/test_attributes.cpp`
- `tests/test_edge_cases.cpp`
- `tests/test_flow.cpp`
- `tests/test_remove_node.cpp`
- `tests/test_multigraph.cpp`
- `tests/test_large_graph_compare.cpp`
- `scripts/unix/run_tests.sh`
- `scripts/unix/run_large_graph_compare.sh`

Where it stays readable, the test files now mirror the semantic-header split too:

- `tests/test_attributes.cpp` includes `nxpp/attributes.hpp`
- `tests/test_multigraph.cpp` includes `nxpp/multigraph.hpp`
- `tests/test_flow.cpp` includes `nxpp/flow.hpp`
- `tests/test_edge_cases.cpp` includes only the graph/traversal/shortest-path/components headers it actually exercises

That test binary is intentionally small for now, but it gives the project a real
formal test path in addition to snippets, showcases, and benchmarks.

The large-graph comparison path is intentionally separate:

- it reuses deterministic generated graphs instead of committed fixtures
- it compares `nxpp` against raw Boost implementations directly
- it is meant as scale-oriented verification, not as a benchmark claim
- it stays opt-in so the normal test path remains quick to run locally and in CI

For benchmark runs that should produce both CSV data and a ready-to-read report,
you can use:

```bash
python3 scripts/run_benchmark_report.py --all --iterations 3 --jobs "$(nproc)"
```

That command:

- runs the serial benchmark first and the parallel benchmark second
- moves any previous benchmark CSVs, `benchmark/BENCHMARK.md`, and `benchmark/imgs/` into `backups/benchmark/<timestamp>/`
- generates exactly two timestamped CSV files: one serial and one parallel
- writes the report to `benchmark/BENCHMARK.md` and the plots to `benchmark/imgs/`

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

`nxpp` is a modular library in the repository, and releases also ship a generated
single-header distribution asset. It depends on **Boost Graph Library**.

### Minimal support matrix

This matrix is intentionally conservative and reflects what the repository
currently verifies directly:

| Area | Current status |
|---|---|
| Language level | C++20 required |
| Standard library / toolchain used in repo scripts | local scripts are still primarily `g++`-oriented |
| Continuous integration | GitHub Actions on `ubuntu-latest`, `macos-latest`, and `windows-latest` |
| Boost dependency | `libboost-graph-dev` on Ubuntu CI, Homebrew Boost on macOS CI, official Boost archive on Windows CI |
| Standalone single header | Generated in CI and tested on Ubuntu |
| macOS | Covered by the formal test workflow with Homebrew Boost |
| Windows | Covered by the formal test workflow with native MSVC (`cl`) and the repository's PowerShell runners |
| Clang | Covered by the formal test workflow on Ubuntu and macOS |

In practice, the repository now has a CI-backed compatibility story across
Linux, macOS, and Windows. Linux covers both `g++` and `clang++`, macOS covers
`clang++`, and Windows uses a native MSVC (`cl`) path through the repository's
PowerShell runners.

### Minimal local build

```bash
g++ -std=c++20 -Wall -Wextra -pedantic -O3 main_boost.cpp -o main_boost
g++ -std=c++20 -Wall -Wextra -pedantic -O3 main_nxpp.cpp -o main_nxpp
python3 main.py
bash scripts/unix/run_tests.sh
```

## Script layout

Repository automation scripts are now split by platform:

- `scripts/unix/` contains the `.sh` runners used by the existing Unix/macOS CI paths
- `scripts/windows/` contains the PowerShell counterparts for Windows-oriented flows

The top-level `scripts/` directory now keeps only the shared Python helpers,
while platform-specific runners live under the explicit `unix/` and `windows/`
subdirectories.

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

### External usage

Today there are two supported ways to consume `nxpp` from another project:

1. the modular header tree under `include/`
2. the tested single-header release asset `nxpp.hpp`

For the modular layout, add the repo's `include/` directory to your compiler
include path and write includes like:

```cpp
#include <nxpp.hpp>
```

or, when you only need one area:

```cpp
#include <nxpp/shortest_paths.hpp>
```

Minimal modular compile example:

```bash
g++ -std=c++20 -I/path/to/nxpp/include app.cpp -o app
```

For the single-header path, use the tested `nxpp.hpp` asset attached to a GitHub
release, place it in a vendor/include directory, and compile with that
directory on your include path:

```cpp
#include <nxpp.hpp>
```

```bash
g++ -std=c++20 -I/path/to/vendor/include app.cpp -o app
```

In both cases, `nxpp` is still header-only and still depends on **Boost Graph
Library** headers being available to the compiler.

See [`docs/EXTERNAL_USAGE.md`](docs/EXTERNAL_USAGE.md) for the fuller external
consumer story, including the difference between the repo-local generated
`dist/nxpp.hpp` artifact and the tested release asset.

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
    auto path_to_naples = routes.path_to("Naples");

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
- richer result wrappers such as `dijkstra_shortest_paths()` with on-demand path reconstruction and indexed component views

---

## Internal model in one minute

`nxpp` keeps these synchronized structures:

- `id_to_bgl`: `NodeID -> vertex_descriptor`
- `bgl_to_id`: index-ordered `NodeID` storage used for normalized wrapper results
- a maintained wrapper index property on each vertex, exposed as `vertex_index_map` for BGL algorithms

This lets the public API use `std::string`, `int`, or other orderable node IDs while running algorithms on a normal BGL graph internally.

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
