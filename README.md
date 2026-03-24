# nxpp - NetworkX for C++

**nxpp** is a native, *header-only* **C++20** library designed to teleport the unmistakable syntactic and semantic elegance of [NetworkX](https://networkx.org/) (Python) into the high-performance C++ ecosystem. Internally, it acts as a powerful wrapper around the battle-tested [Boost Graph Library (BGL)](https://www.boost.org/doc/libs/1_84_0/libs/graph/doc/index.html).

It was born with a single goal: offering a strictly "1-to-1" continuous interface for complex networks, allowing researchers and developers to natively prototype algorithms in Python and compile the exact mirror topology in C++ with virtually zero cognitive overhead.

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
// Shortest Pathfinding Algorithm
auto path = nxpp::dijkstra_path(G, std::string("Milan"), std::string("Naples"));

// ---------------------------------------------------------
// NEW: Topological Generators (Phase 3)
// ---------------------------------------------------------
auto K5 = nxpp::complete_graph(5); // Returns Graph<int> by default
auto P4 = nxpp::path_graph<nxpp::DiGraph>(4); // Path 0->1->2->3 as DiGraph

// ---------------------------------------------------------
// NEW: Custom Node/Edge Attributes (std::any)
// ---------------------------------------------------------
G.node("Rome")["population"] = 2800000;
G["Rome"]["Naples"]["km"] = 220;
int pop = G.node("Rome")["population"];
```

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
