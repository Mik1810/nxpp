# nxpp - NetworkX for C++

**nxpp** is a native, *header-only* **C++20** library designed to teleport the unmistakable syntactic and semantic elegance of [NetworkX](https://networkx.org/) (Python) into the high-performance C++ ecosystem. Internally, it acts as a powerful wrapper around the battle-tested [Boost Graph Library (BGL)](https://www.boost.org/doc/libs/1_84_0/libs/graph/doc/index.html).

It was born with a single goal: offering a strictly "1-to-1" continuous interface for complex networks, allowing researchers and developers to natively prototype algorithms in Python and compile the exact mirror topology in C++ with virtually zero cognitive overhead.

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
```

---

### Critical Under-the-Hood Differences

While perfectly emulating NetworkX—such as reproducing built-in node destruction APIs (`clear()`, `remove_node(u)`, `remove_edge(u,v)`) by dynamically maneuvering BGL fragments deep in memory via a smart algorithmic *O(1) memory-swap* swap—migrating to a statically typed systems language undeniably introduces structural differences to prevent segmentation faults:

1. **Strong Type Safety Constraints**: 
   In interpreted Python, a careless engineer might insert an integer into a graph layout, and subsequently assign an arbitrary floating-point edge mixed with textual boolean comparisons (`G.add_node(1); G.add_node("A");`). 
   In **nxpp**, the node identifier type is securely locked down by the compiler Template directly on line one. If you compile an `nxpp::DiGraph` (or explicitly declare `nxpp::Graph<std::string>`), the *Hash* Map keys will strictly resolve to `std::string`. Zero structural ambiguity translates to incredible runtime stability and mathematically non-existent debug tracking.

2. **Property Dictionaries vs Native Fixed Edge Weights**:
   In NetworkX, traversing every edge pulls a large sluggish underlying dictionary housing a plethora of string-based parameters (`cap=10, speed=130`).
   Currently, **nxpp** outright rejects this memory dispersion to honor the fundamental laws of high-performance computing, offering uninterrupted bare-metal CPU throughput. Every BGL edge intrinsically supports a primary hard-coded structural property denoted as `weight` (`EdgeWeight = double`). This constraint allows proxy-based iterative access protocols to execute completely clean mathematical overwrites without ever getting bogged down in dictionary allocations (`G["A"]["B"] = 5.0`).

3. **Solid Data Structures (Vectors) vs Lazy Generators (Python Yield)**:
   In Python, commands corresponding to `edges()`, `nodes()` or `nx.bfs_edges` exclusively spit out *Lazy Generators* sequentially to compensate for horrendous dynamic execution times and stave off garbage collector memory exhaustion limits.
   Conversely, in `nxpp`, BGL iterative methods directly graze precompiled L1-L2 hardware silicon array caches. Therefore, it is literally orders of magnitude faster to return completely compact, contiguous RAW structural blocks outright (`std::vector<NodeID>`), allowing highly hyper-performant iteration blocks and calculations without risking the corruption or stalling of the precompiled processor pipeline mechanisms.
