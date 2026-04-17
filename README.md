# nxpp

[![Compatibility CI](https://github.com/Mik1810/nxpp/actions/workflows/compatibility.yml/badge.svg?branch=main)](https://github.com/Mik1810/nxpp/actions/workflows/compatibility.yml) ![Boost](https://img.shields.io/badge/Boost-required-F7901E?logo=boost&logoColor=white) ![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=c%2B%2B)

Current release: `v1.0.21`

nxpp is a modern C++20 graph library that gives you a NetworkX-inspired API on top of Boost Graph Library.
It helps you write graph code faster with less BGL boilerplate while keeping Boost performance and compatibility.

## Quick Start

Create a file named `quick_start.cpp` in the repository root:

```cpp
#include <iomanip>
#include <iostream>
#include <string>

#include "include/nxpp.hpp"

int main() {
    nxpp::DiGraph g;
    g.add_edge("A", "B", 1.0);
    g.add_edge("B", "C", 2.0);
    g.add_edge("A", "C", 5.0);

    const auto result = g.dijkstra_shortest_paths("A");

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Distance A -> C: " << result.distance.at("C") << "\n";

    std::cout << "Path A -> C:";
    for (const auto& node : result.path_to("C")) {
        std::cout << " " << node;
    }
    std::cout << "\n";
}
```

Compile and run:

```bash
g++ -std=c++20 -O2 -I. quick_start.cpp -o quick_start
./quick_start
```

Expected output:

```text
Distance A -> C: 3.0
Path A -> C: A B C
```

## Installation / Consumption Modes

nxpp is header-only.

Requirements:
- C++20 compiler
- Boost headers

The current compatibility baseline is now explicitly tracked against
**Boost 1.74.0**.

Use nxpp in one of these ways:

1. Repository-local examples inside a clone of `nxpp`

```cpp
#include "include/nxpp.hpp"
```

Use this only when you are compiling directly inside a clone of the repository,
for example from the repository root with `-I.` as in the quick-start example.

2. External modular-header consumption

```cpp
#include <nxpp.hpp>
```

or narrower semantic headers such as:

```cpp
#include <nxpp/graph.hpp>
#include <nxpp/shortest_paths.hpp>
```

Use this when your project points its include path at `nxpp/include`.

3. Vendored CMake subdirectory

```cmake
add_subdirectory(nxpp)
target_link_libraries(your_target PRIVATE nxpp::nxpp)
```

Use this when `nxpp` lives inside your source tree as a vendored dependency.

4. Installed CMake package

```cmake
find_package(nxpp CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE nxpp::nxpp)
```

Use this when `nxpp` has been installed into a prefix and you want a normal
`find_package(...)` workflow.

5. Tested single-header release asset

```cpp
#include <nxpp.hpp>
```

Use this when you want the narrowest single-file integration path from a
GitHub release asset rather than the modular `include/` tree.

For the full source of truth on include paths, header forms, and external
consumption modes, see [docs/EXTERNAL_USAGE.md](docs/EXTERNAL_USAGE.md).

## Packaging Status

The current first-class consumption paths are:

- direct repository-local header usage
- vendored CMake usage through `add_subdirectory(nxpp)`
- installed-package CMake usage through `find_package(nxpp CONFIG REQUIRED)`

The packaging and distribution strategy is now explicit:

1. keep the current direct-source and vendored-CMake paths working well
2. treat the installable/exported CMake package as the main distribution foundation
3. keep Conan support active while the ConanCenter submission track is still in progress
4. keep the repository-hosted vcpkg overlay path active
5. keep AUR as an active secondary release-based distribution channel
6. adopt FetchContent / CPM.cmake guidance as a near-term CMake convenience path
7. treat Homebrew tap and Spack as later-stage candidates
8. treat Meson WrapDB and Debian / Ubuntu packaging as deferred for now

The repository now also includes a local Conan recipe in
[conanfile.py](conanfile.py) for header-only packaging work. Today this is the
actively supported Conan path inside the repository. See
[docs/EXTERNAL_USAGE.md](docs/EXTERNAL_USAGE.md) for the current scope and how
to test it locally in an environment where Conan is installed.

The repository also carries a ConanCenter-style submission recipe under
[packaging/conan-center-index/](packaging/conan-center-index).

Important status clarification:

- `nxpp` is not yet published on ConanCenter
- the repository currently supports the local Conan recipe directly
- the ConanCenter path is being pursued through an upstream
  `conan-center-index` pull request, but it should still be treated as
  in-progress until that submission is accepted and published

The repository now also includes a supported repository-hosted vcpkg overlay
port under [packaging/vcpkg/](packaging/vcpkg). That overlay path has been
validated locally with `vcpkg install nxpp --overlay-ports=...` plus a small
external CMake consumer using `find_package(nxpp CONFIG REQUIRED)`. A curated
registry submission was also attempted in `microsoft/vcpkg`, but it was closed
under their current project-maturity policy, so the supported vcpkg path today
remains the repository-hosted overlay port.

The repository now also carries a repository-hosted AUR packaging path under
[packaging/aur/](packaging/aur). This tracks the release-based `nxpp` package
shape and is already a real active secondary channel.

Additional channel decisions from the ongoing distribution evaluation are now
explicit too:

- near-term: FetchContent / CPM.cmake documentation guidance for pinned-tag
  CMake consumption
- later-stage candidates: Homebrew tap, Spack
- deferred for now: Meson WrapDB, Debian / Ubuntu packaging

Debian / Ubuntu packaging has also been evaluated as a later distribution
option. The current conclusion is to defer that path for now: a Debian source
package, PPA flow, or broader apt-facing maintenance story would add more
ongoing packaging overhead than the project currently needs, while the existing
source, installed-CMake, Conan, and vcpkg overlay paths already cover the main
consumer scenarios.

Distributed-package versioning follows the repository release version directly:

- the top version in `CHANGELOG.md` and `RELEASE_NOTES.md` is the concrete next
  release candidate
- when a release tag `vX.Y.Z` is published, the repository should keep the
  shipped package metadata aligned to that same `X.Y.Z`
- in practice that means keeping the release version coherent across:
  - `CMakeLists.txt`
  - `conanfile.py`
  - `packaging/vcpkg/ports/nxpp/vcpkg.json`
- the repository-hosted package metadata is currently aligned to the latest
  documented release line
- repository-hosted package paths such as the Conan recipe and the vcpkg
  overlay port are expected to track the published repository release directly
- third-party or policy-gated channels can lag behind, but the docs should say
  so explicitly rather than implying simultaneous publication everywhere

## Features

- **Header-only** C++20 API over **Boost Graph Library**
- **NetworkX-inspired** graph interface and ergonomics
- Directed, undirected, weighted, unweighted, and multigraph variants
- Dynamic node and edge attributes with proxy syntax
- Checked attribute accessors for robust typed reads
- Traversal helpers: BFS and DFS
- Shortest paths: Dijkstra, Bellman-Ford, DAG shortest paths, Floyd-Warshall
- Components: connected and strongly connected
- Topological sort and minimum spanning tree helpers (Kruskal, Prim)
- Flow helpers: maximum flow, minimum cut, min-cost flow routines
- Centrality helpers: degree centrality, PageRank, betweenness centrality
- Graph generators: complete graph, path graph, Erdos-Renyi graph
- Precise multigraph edge handling through `edge_id` APIs

## `"weight"` Semantics

In the current public API, the string `"weight"` has a **narrow** meaning:

- it refers to the built-in edge-weight property
- it does **not** mean “use any arbitrary custom edge attribute as the weight”

This matters in weighted shortest-path and min-cost-flow calls such as:

- `shortest_path(..., "weight")`
- `dijkstra_path(..., "weight")`
- `bellman_ford_path(..., "weight")`
- `max_flow_min_cost(..., capacity_attr, "weight")`

If you are reading or mutating arbitrary custom edge attributes, use the
attribute APIs directly. If you are asking the weighted algorithm layer for
cost/weight semantics, `"weight"` still means the wrapper's built-in edge
weight channel.

## `NodeID` Requirements

For the main `nxpp::Graph<NodeID, ...>` template, `NodeID` currently needs to be:

- copy-constructible
- equality comparable
- orderable through `std::less`

That means common types such as `std::string`, `int`, and many ordered custom
types work naturally.

Important clarifications:

- the core wrapper does **not** require a public hash-table interface
- the core wrapper does **not** require `NodeID` to be constructible from
  `std::size_t`

That extra `std::size_t` rule applies only to the numeric graph generators in
[`nxpp/generators.hpp`](include/nxpp/generators.hpp), because those helpers
synthesize IDs `0..n-1` themselves.

For the fuller policy wording, see:

- [docs/API_REFERENCE.md](docs/API_REFERENCE.md)
- [docs/GRAPH_CONFIGURATION.md](docs/GRAPH_CONFIGURATION.md)

## Implicit Creation Policy

`nxpp` follows a simple public rule:

- write-style graph access may create missing nodes or edges
- read-style accessors and algorithmic reads do not create implicitly

In practice that means:

- `add_node(u)` creates `u`
- `add_edge(u, v, ...)` creates missing endpoints
- `G.node(u)[key] = value` creates `u` if absent
- `G[u][v] = weight` creates `(u, v)` and any missing endpoints
- `G[u][v][key] = value` creates `(u, v)` if absent

But pure reads do not create:

- `has_node(...)`
- `has_edge(...)`
- checked `get_*` / `try_get_*` accessors
- traversal reads such as BFS/DFS entry points
- shortest-path and other algorithmic reads

The fuller policy table lives in [docs/API_REFERENCE.md](docs/API_REFERENCE.md).

For multigraphs, the public rule is:

- use `edge_id` APIs when one specific parallel edge matters
- treat endpoint-based `(u, v)` APIs as convenience-oriented unless a function
  explicitly documents a stronger guarantee
- note one especially important case: `remove_edge(u, v)` removes all parallel
  edges between those endpoints; use `remove_edge(edge_id)` for one edge
- attr-bearing endpoint `add_edge(..., attrs)` forms are now rejected in
  multigraphs; use `add_edge_with_id(...)` then `set_edge_attr(edge_id, ...)`
  because `(u, v)` is not enough to identify one concrete parallel edge
- flow wrappers now follow the same rule: endpoint-keyed flow or cut-edge
  views are aggregate convenience views in multigraphs, while the new
  `edge_id`-keyed result fields are the precise path
- `minimum_cut()` now also follows the precise multigraph rule internally:
  heterogeneous parallel capacities are modeled per concrete `edge_id`, not
  through an endpoint-based convenience lookup
- the staged `push_relabel_maximum_flow(...)` then `cycle_canceling()` path
  now invalidates itself on any later graph mutation; rerun the push-relabel
  stage after mutations, or prefer the one-shot min-cost wrappers

## More Examples

### Attributes with safe reads

```cpp
nxpp::DiGraph g;
g.add_edge("Milan", "Rome", 5.0, {{"capacity", 8}});
g.node("Milan")["region"] = "Lombardy";

auto region = g.get_node_attr<std::string>("Milan", "region");
double cap = g.get_edge_numeric_attr("Milan", "Rome", "capacity");
```

### Precise parallel-edge handling

```cpp
nxpp::MultiDiGraph g;

auto e1 = g.add_edge_with_id("A", "B", 1.0);
auto e2 = g.add_edge_with_id("A", "B", 2.0);

g.set_edge_attr(e1, "label", "first");
g.set_edge_attr(e2, "label", "second");
```

## Documentation

- API reference: https://mik1810.github.io/nxpp/
- Companion docs index: [docs/README.md](docs/README.md)
- Practical API guide: [docs/API_REFERENCE.md](docs/API_REFERENCE.md)
- Complexity model: [docs/COMPLEXITY.md](docs/COMPLEXITY.md)
- Testing guide: [docs/TEST.md](docs/TEST.md)
- Experimental wasm + Node plan/guide: [wasm/WASM.md](wasm/WASM.md)
- Experimental wasm npm package guide: [wasm/README.md](wasm/README.md)

Current experimental wasm layout:

- Node bindings: [wasm/node/nxpp_bindings.cpp](wasm/node/nxpp_bindings.cpp)
- Node contract test: [wasm/test/node_api_contract_test.mjs](wasm/test/node_api_contract_test.mjs)
- Node build artifacts: [wasm/build/](wasm/build/)

Current wasm architecture direction:

- finish `graph.hpp` coverage before moving to later semantic headers
- move the public JS API toward a smaller NetworkX-like family:
  - `Graph`
  - `DiGraph`
  - `MultiGraph`
  - `MultiDiGraph`
- keep concrete C++ graph aliases as internal wasm binding details
- support both numeric and string node IDs without collapsing them into a
  single normalized representation internally
- choose the internal node-ID backend lazily from first use and reject mixed
  numeric/string IDs inside the same graph

Useful generated-reference entry points:

- Main wrapper type: https://mik1810.github.io/nxpp/classnxpp_1_1Graph.html
- Module index: https://mik1810.github.io/nxpp/group__nxpp__graph__core.html
- Flow and cut module: https://mik1810.github.io/nxpp/group__nxpp__flow.html
- Shortest-path module: https://mik1810.github.io/nxpp/group__nxpp__shortest__paths.html
- Centrality module: https://mik1810.github.io/nxpp/group__nxpp__centrality.html

The generated Doxygen reference now has explicit file-level coverage across the
semantic headers under `include/nxpp/`, plus clearer alias/type coverage for
the public graph presets.

Deprecated aliases and thin namespace-scope wrappers now also carry more
complete Doxygen text, so the generated reference is less likely to show bare
one-line stubs without migration or return-shape context.

The generated reference is now also organized around real library modules such
as graph core, traversal, shortest paths, flow, centrality, and generators,
instead of relying only on flat file/class navigation.

The generated Doxygen landing page is also now stronger as an entry point for
GitHub Pages: it points readers toward the main wrapper type, the most common
result wrappers, the semantic module groups, and the companion markdown guides
instead of behaving like a thin placeholder page.

The companion markdown guides and the generated site are now linked more
directly in both directions: use the markdown docs for policy and curated
explanation, then jump into the generated pages for the exact declaration-level
surface behind the same area.

The richer generated result-wrapper pages now also carry short embedded usage
examples for the main shortest-path and flow result types, so the published
reference is more useful even when readers land directly on those struct pages.

The GitHub Pages presentation has now also been refined a bit further on the
Doxygen side: the published site uses a stronger navigation-oriented layout and
slightly more deliberate styling so the module groups, wrapper pages, tables,
and code examples read more like a small reference portal than a stock dump.

## License

MIT License. See [LICENSE](LICENSE).
