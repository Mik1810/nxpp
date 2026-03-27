# TODO: Snippet-Aligned Priorities and Core Engineering Improvements

This file tracks three things:

1. Missing functionality that is explicitly present in the local C++/Python snippet sets and should therefore be treated as core priority work.
2. Secondary work that currently exists or is planned in `nxpp`, but is not directly driven by the snippet sets.
3. Cross-cutting engineering improvements that should not outrank snippet-backed essentials, but are important for correctness, maintainability, and external usability.

## Priority Rule

Implementation priority is still driven by the local snippet sets:

- `snippet/` is the primary C++ algorithm reference
- colocated Python snippets are the primary behavior/parity reference
- functions represented in those snippet sets are the essential target surface
- work outside that surface can exist, but should not outrank snippet-backed algorithms

---

## Priority: Functions Present in `snippet`

These are the functions and algorithm families that should drive implementation order.

- [x] `bfs_edges`
- [x] `dfs_edges`
- [x] `connected_components`
- [x] `strongly_connected_components`
- [x] `dijkstra_path`
- [x] `bellman_ford_path`
- [x] `shortest_path`

---

## Snippet-Backed Essentials

- [x] `topological_sort`
  Reference: `snippet/ts/ts.cpp`, `snippet/ts/ts.py`
- [x] `kruskal_minimum_spanning_tree`
  Reference: `snippet/kruskal/kruskal.cpp`, `snippet/kruskal/kruskal.py`
- [x] `prim_minimum_spanning_tree`
  Reference: `snippet/prim/prim.cpp`
- [x] `maximum_flow`
  Reference: `snippet/flow/flow.cpp`, `snippet/flow/flow.py`
- [x] `dag_shortest_paths`
  Reference: `snippet/dag_sp/dag_sp.cpp`
- [x] `floyd_warshall_all_pairs_shortest_paths`
  Reference: `snippet/floyd_warshall/floyd_warshall.cpp`
- [x] `max_flow_min_cost` / min-cost max-flow wrappers
  Reference: `snippet/mcmf_cc/mcmf_cc.cpp`, `snippet/mcmf_spp/mcmf_ssp.cpp`, `snippet/mcmf_cc/mcmf_cc.py`, `snippet/mcmf_spp/mcmf_ssp.py`
- [x] `two_sat_satisfiable` or equivalent 2-SAT helper
  Reference: `snippet/2sat/2sat.cpp`
- [x] SCC representative/root-map style helper
  Reference: `snippet/scc_named/scc_named.cpp`

---

## Remaining Snippet Verification / Harness Work

These items are still snippet-driven, but concern parity coverage and verification rather than missing API surface.

- [x] Carry the current manual snippet review pass through `dag_sp`
- [x] Carry the current manual snippet review pass through `dfs`
- [x] Carry the current manual snippet review pass through `dijkstra`
- [x] Carry the current manual snippet review pass through `flow`
- [x] Carry the current manual snippet review pass through `floyd_warshall`
- [ ] Continue the current manual snippet review pass with `graph_example`
- [ ] Extend `scripts/test_snippet_batch.sh` beyond the current first batch
- [ ] Add `dag_sp` to the shell harness
- [ ] Add `flow` to the shell harness
- [ ] Add `floyd_warshall` to the shell harness
- [ ] Add `kruskal` to the shell harness
- [ ] Add `prim` to the shell harness
- [ ] Add `scc` / `scc_named` to the shell harness
- [ ] Add `ts` to the shell harness
- [ ] Add min-cost max-flow snippet coverage to the shell harness
- [ ] Add summary output for pass/fail counts across all snippet folders
- [ ] Add a simple regression workflow so snippet parity can be rerun quickly after header changes

---

## High-Priority Correctness and API Risks

These items are not missing snippet-backed algorithms, but they are the highest-priority design/correctness issues in the current implementation.

- [ ] Redesign multigraph edge semantics so parallel edges are not ambiguously addressed by `(u, v)` alone
- [ ] Define explicit behavior for multigraph reads:
  - `get_edge_weight(u, v)`
  - `get_edge_attr(u, v, key)`
  - `has_edge(u, v)`
  - `remove_edge(u, v)`
- [ ] Introduce a stable public/internal way to refer to one specific parallel edge
- [ ] Audit `remove_edge()` behavior against multigraph metadata cleanup
- [ ] Document multigraph semantics clearly in `README.md`
- [ ] Audit all algorithms that currently assume a unique `(u, v)` edge in simple-graph style
- [ ] Revisit whether proxy syntax like `G[u][v]` should be limited or clarified for multigraph usage

---

## Engineering Improvements (Important, but Not Above Snippet Scope)

These items improve maintainability, packaging, and project quality, but should not outrank snippet-backed algorithm work.

### Repository and Build

- [ ] Add a `LICENSE`
- [ ] Add a standard `CMakeLists.txt`
- [ ] Add a small install/usage story for external users
- [ ] Add CI with at least one Linux build
- [ ] Extend CI to macOS and Windows if practical
- [ ] Add a minimal compiler/version support matrix to the README
- [ ] Define a GitHub release process (tags, release notes, artifacts, and release cadence)
- [ ] Define a versioning strategy for the library (e.g. SemVer and compatibility expectations)
- [ ] Once the packaging/install story is stable, evaluate Linux distribution channels such as `apt` and `snap`

### Testing

- [ ] Add a real test suite with assertions in addition to snippet output comparisons
- [ ] Add edge-case tests for:
  - empty graphs
  - single-node graphs
  - disconnected graphs
  - missing nodes / missing edges
  - unreachable shortest paths
  - attribute lookup failures
  - destructive operations after node removal
  - multigraph-specific behavior
- [ ] Add regression tests for descriptor remapping after `remove_node()`
- [ ] Add dedicated tests for flow wrappers and capacity/weight attribute handling

### Header Architecture and Encapsulation

- [ ] Split `include/nxpp.hpp` into logical subheaders once the public surface stabilizes
- [ ] Keep a single umbrella include if desired, but move implementation into internal modules
- [ ] Reduce direct exposure of internal mutable state
- [ ] Consider making `node_properties` and `edge_properties` non-public
- [ ] Re-check public API for invariants that can currently be bypassed

### Error Handling and API Clarity

- [ ] Introduce more expressive exception types instead of relying mostly on `std::runtime_error`
- [ ] Standardize error message style across graph operations and algorithm wrappers
- [ ] Clarify which functions create missing nodes implicitly and which should fail
- [ ] Document complexity and caveats for destructive operations more explicitly in the API reference
- [ ] Revisit complexity documentation so it does not lean too casually on expected `unordered_map` running time where a stricter effective bound can be stated without overstating performance
- [ ] Where algorithmic complexity does not worsen, prefer documenting a stronger/worst-case-style public-call running time instead of only average-case hash-table assumptions
- [ ] Add compile-time constraints or concepts for `NodeID` expectations

### Attribute System

- [ ] Continue parity polish for attribute-heavy APIs
- [ ] Re-evaluate long-term use of `std::any` for node/edge attributes
- [ ] Keep proxy syntax for ergonomics, but push safer checked accessors in docs/examples
- [ ] Add more tests for wrong-type attribute reads and missing-key behavior

### Documentation and Project Hygiene

- [ ] Reduce drift risk between `README.md`, `ROADMAP.md`, `TODO.md`, `SESSION.md`, and `CHANGELOG.md`
- [ ] Decide which file is the source of truth for current status
- [ ] Keep `README.md` focused on user-facing usage and caveats
- [ ] Keep `TODO.md` focused on open work only
- [ ] Keep `SESSION.md` as historical development log rather than current status source
- [ ] Separate examples, smoke tests, and formal tests more clearly in repo structure
- [ ] Study and define a proper documentation strategy (scope, structure, tooling, hosting, and API/reference split)
- [ ] Add inline API documentation comments for public functions so editor hover/tooling can explain purpose, parameters, and return type directly from the code

---

## Outside Current Snippet Scope

These items are not currently driven by the snippet sets, so they should not outrank the essentials above.

- [x] `degree_centrality`
- [ ] `betweenness_centrality`
- [ ] `pagerank`
- [ ] Benchmarking harness
- [ ] Optional Python bindings

---

## Notes

- `graph_example` and `graph_weights` are informational reference snippets rather than standalone algorithm targets.
- The current manual snippet review/parity pass has reached `floyd_warshall`; the next folder queued for inspection is `graph_example`.
- Existing features that are outside snippet scope can stay in the codebase, but they should not drive roadmap priority over the functions listed above.
- The highest-risk implemented area at the moment is multigraph behavior and edge identity semantics.
- The highest-value non-feature improvement at the moment is broader automated verification of the snippet folders.
