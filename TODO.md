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

## Top-Priority Architecture Decisions

These questions should currently outrank most implementation/detail work because they shape the long-term public API and internal model.

- [ ] Decide the long-term graph API direction: clarify when functionality should live as free functions under `nxpp::` versus methods on graph objects like `G.add_edge(...)`, and document a consistent architectural rule
- [ ] Discuss whether `nxpp` should keep explicit “NetworkX-behavioral signature” wrappers such as `bfs_edges`, `dfs_edges`, `dijkstra_path`, `connected_components`, and `strongly_connected_components`, or narrow/simplify that compatibility layer
- [ ] Decide how much of the underlying Boost graph configurability should be exposed in `nxpp`
- [ ] Design a clean way to customize the underlying BGL storage/selectors when needed (for example `vecS`, `setS`, and related choices), while keeping the current defaults simple and ergonomic
- [ ] Define which graph-configuration knobs should remain advanced/optional and which should stay fixed behind the default `nxpp` graph aliases

---

## Snippet-Driven Core Surface

These are the snippet folders that define the essential public surface of `nxpp`.

- [x] `snippet/2sat/`
- [x] `snippet/bellman_ford/`
- [x] `snippet/bfs/`
- [x] `snippet/cc/`
- [x] `snippet/dag_sp/`
- [x] `snippet/dfs/`
- [x] `snippet/dijkstra/`
- [x] `snippet/flow/`
- [x] `snippet/floyd_warshall/`
- [x] `snippet/graph_example/`
- [x] `snippet/graph_weights/`
- [x] `snippet/kruskal/`
- [x] `snippet/mcmf_cc/`
- [x] `snippet/mcmf_ssp/`
- [x] `snippet/prim/`
- [x] `snippet/scc/`
- [x] `snippet/scc_named/`
- [x] `snippet/ts/`

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

- [x] Add a `LICENSE`
- [ ] Add a standard `CMakeLists.txt`
- [ ] Add a small install/usage story for external users
- [x] Add CI with at least one Linux build
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
- [ ] Re-evaluate whether some public-facing maps/lookups should move from hash-based containers with expected running time to ordered/logarithmic structures (for example `std::map`/tree-based containers) when more stable insert/delete/lookup complexity is preferable
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
- [ ] Keep `main.cpp` aligned with current wrapper names and result shapes after API renames/refactors
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
- The current manual snippet review/parity pass across the snippet folders has been completed.
- Existing features that are outside snippet scope can stay in the codebase, but they should not drive roadmap priority over the functions listed above.
- The highest-risk implemented area at the moment is multigraph behavior and edge identity semantics.
- The highest-value non-feature improvement at the moment is broader automated verification of the snippet folders.
