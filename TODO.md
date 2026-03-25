# TODO: Snippet-Aligned Priorities

This file tracks two things:

1. Missing functionality that is explicitly present in the local C++/Python snippet sets and should therefore be treated as core priority work.
2. Secondary work that currently exists or is planned in `nxpp`, but is not directly driven by the snippet sets.

## Priority: Functions Present in `snippet`

These are the functions and algorithm families that should drive implementation order.

- [x] `bfs_edges`
- [x] `dfs_edges`
- [x] `connected_components`
- [x] `strongly_connected_components`
- [x] `dijkstra_path`
- [x] `bellman_ford_path`
- [x] `shortest_path`

## Missing Snippet-Backed Essentials

- [x] `topological_sort`
  Reference: `snippet/ts/ts.cpp`, `py_snippet/graph_operations/ts.py`
- [x] `kruskal_minimum_spanning_tree`
  Reference: `snippet/kruskal/kruskal.cpp`, `py_snippet/graph_operations/kruskal.py`
- [x] `prim_minimum_spanning_tree`
  Reference: `snippet/prim/prim.cpp`
- [x] `maximum_flow`
  Reference: `snippet/flow/flow.cpp`, `py_snippet/flow/flow.py`
- [x] `dag_shortest_paths`
  Reference: `snippet/dag_sp/dag_sp.cpp`
- [x] `floyd_warshall_all_pairs_shortest_paths`
  Reference: `snippet/floyd_warshall/floyd_warshall.cpp`
- [ ] `max_flow_min_cost` / min-cost max-flow wrappers
  Reference: `snippet/mcmf_cc/mcmf_cc.cpp`, `snippet/mcmf_spp/mcmf_ssp.cpp`, `py_snippet/flow/mcmf_cc.py`, `py_snippet/flow/mcmf_ssp.py`
- [x] `two_sat_satisfiable` or equivalent 2-SAT helper
  Reference: `snippet/2sat/2sat.cpp`
- [ ] SCC representative/root-map style helper
  Reference: `snippet/scc_named/scc_named.cpp`

## Outside Current Snippet Scope

These items are not currently driven by the snippet sets, so they should not outrank the essentials above.

- [ ] `degree_centrality`
- [ ] `betweenness_centrality`
- [ ] `pagerank`
- [ ] Additional parity polish for attribute-heavy APIs
- [ ] Benchmarking harness
- [ ] Optional Python bindings

## Notes

- `graph_example` and `graph_weights` are informational reference snippets rather than standalone algorithm targets.
- Existing features that are outside snippet scope can stay in the codebase, but they should not drive roadmap priority over the functions listed above.
