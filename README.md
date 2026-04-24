# nxpp

[![Compatibility CI](https://github.com/Mik1810/nxpp/actions/workflows/compatibility.yml/badge.svg?branch=main)](https://github.com/Mik1810/nxpp/actions/workflows/compatibility.yml) ![Boost](https://img.shields.io/badge/Boost-required-F7901E?logo=boost&logoColor=white) ![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=c%2B%2B)

Current release: `v1.0.46`

`nxpp` is a modern C++20 graph library with a NetworkX-inspired API on top of
Boost Graph Library. The goal is to keep graph code concise and readable while
preserving Boost-level performance and ecosystem compatibility.

## Quick Start

Create `quick_start.cpp` in the repository root:

```cpp
#include <iomanip>
#include <iostream>

#include "include/nxpp.hpp"

int main() {
    nxpp::DiGraph g;
    g.add_edge("A", "B", 1.0);
    g.add_edge("B", "C", 2.0);
    g.add_edge("A", "C", 5.0);

    const auto result = g.dijkstra_shortest_paths("A");
    std::cout << std::fixed << std::setprecision(1)
              << "Distance A -> C: " << result.distance.at("C") << "\n";
}
```

Compile and run:

```bash
g++ -std=c++20 -O2 -I. quick_start.cpp -o quick_start
./quick_start
```

## Installation / Consumption

`nxpp` is header-only and requires:
- C++20 compiler
- Boost headers

Supported consumption paths:

1. **Repository-local examples**
   - `#include "include/nxpp.hpp"`
2. **External modular headers**
   - `#include <nxpp.hpp>` or semantic headers such as `#include <nxpp/graph.hpp>`
3. **Vendored CMake**
   - `add_subdirectory(nxpp)` and `target_link_libraries(... PRIVATE nxpp::nxpp)`
4. **Installed CMake package**
   - `find_package(nxpp CONFIG REQUIRED)` and `target_link_libraries(... PRIVATE nxpp::nxpp)`
5. **Single-header release asset**
   - `#include <nxpp.hpp>` from the release artifact

For detailed install/packaging guidance (Conan, vcpkg overlay, AUR, channel policy),
see [docs/EXTERNAL_USAGE.md](docs/EXTERNAL_USAGE.md).

## Feature Overview

- Header-only C++20 API over Boost Graph Library
- Directed/undirected/weighted/unweighted/multigraph variants
- Attribute APIs (proxy writes + checked typed reads)
- Traversal, shortest paths, components, flow/min-cut/min-cost, centrality
- Graph generators (`complete_graph`, `path_graph`, `erdos_renyi_graph`)
- Precise multigraph edge handling via `edge_id` APIs
- Graphviz DOT export via `to_dot` / `to_dot_string` (see [docs/API_REFERENCE.md](docs/API_REFERENCE.md))

## Support and API stability

Not every install path and wrapper has the same upgrade contract as the C++
public headers. The full matrix of **stable vs experimental** surfaces (CMake,
Conan, vcpkg overlay, AUR, wasm, browser scope) is maintained in
[docs/STABILITY.md](docs/STABILITY.md). First-class C++ paths are still described
in [docs/EXTERNAL_USAGE.md](docs/EXTERNAL_USAGE.md).

## Documentation Map

- Docs index: [docs/README.md](docs/README.md)
- Public support and API stability: [docs/STABILITY.md](docs/STABILITY.md)
- Curated API guide and caveats: [docs/API_REFERENCE.md](docs/API_REFERENCE.md)
- External usage and packaging policy: [docs/EXTERNAL_USAGE.md](docs/EXTERNAL_USAGE.md)
- Testing and CI map: [docs/TEST.md](docs/TEST.md)
- Complexity model: [docs/COMPLEXITY.md](docs/COMPLEXITY.md)
- Generated reference site: https://mik1810.github.io/nxpp/

## WASM (Concise Status)

The experimental npm package lives under `wasm/` and currently targets Node.js.
For coverage, runtime boundaries, release checklist, and usage:

- [wasm/README.md](wasm/README.md)
- [wasm/WASM.md](wasm/WASM.md)
- [wasm/TODO.md](wasm/TODO.md)

## License

MIT License. See [LICENSE](LICENSE).
