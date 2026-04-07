# nxpp

[![Compatibility CI](https://github.com/Mik1810/nxpp/actions/workflows/compatibility.yml/badge.svg?branch=main)](https://github.com/Mik1810/nxpp/actions/workflows/compatibility.yml) ![Boost](https://img.shields.io/badge/Boost-required-F7901E?logo=boost&logoColor=white) ![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=c%2B%2B)

Current release: `v1.0.0`

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

## Installation

nxpp is header-only.

Requirements:
- C++20 compiler
- Boost headers

Use nxpp in one of these ways:

1. Repository-local headers

```cpp
#include "include/nxpp.hpp"
```

Use this when you are compiling directly inside a clone of the repository.

2. Vendored CMake subdirectory

```cmake
add_subdirectory(nxpp)
target_link_libraries(your_target PRIVATE nxpp::nxpp)
```

Use this when `nxpp` lives inside your source tree as a vendored dependency.

3. Installed CMake package

```cmake
find_package(nxpp CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE nxpp::nxpp)
```

Use this when `nxpp` has been installed into a prefix and you want a normal
`find_package(...)` workflow.

For a fuller external-consumer guide, including Boost expectations and a
minimal installed-package example, see [docs/EXTERNAL_USAGE.md](docs/EXTERNAL_USAGE.md).

## Packaging Status

The current first-class consumption paths are:

- direct repository-local header usage
- vendored CMake usage through `add_subdirectory(nxpp)`
- installed-package CMake usage through `find_package(nxpp CONFIG REQUIRED)`

The packaging and distribution strategy is now explicit:

1. keep the current direct-source and vendored-CMake paths working well
2. treat the installable/exported CMake package as the main distribution foundation
3. add Conan support after the CMake package story is stable
4. evaluate vcpkg after that
5. treat Debian / Ubuntu packaging as a later optional target rather than the first distribution priority

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

## License

MIT License. See [LICENSE](LICENSE).
