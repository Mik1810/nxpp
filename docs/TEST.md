# Testing Guide

This document explains the verification layers used in `nxpp`, how to run them,
and what each one is meant to prove.

Use the root [`README.md`](../README.md) for the project overview and quick
navigation.
Use this file when you want the repository's testing story in one place.

## Testing layers

The repository intentionally uses several different verification paths instead
of one giant test command.

### 1. Showcase programs

Files:

- [`main_boost.cpp`](../main_boost.cpp)
- [`main_nxpp.cpp`](../main_nxpp.cpp)
- [`main.py`](../main.py)

Purpose:

- demonstrate the user-facing API and the wrapper-vs-raw-Boost story
- act as lightweight smoke/demo entry points

What they are not:

- not the formal regression suite
- not the snippet parity harness
- not the large-graph comparison path

### 2. Snippet parity / regression harness

Relevant files:

- [`snippet/`](../snippet)
- [`scripts/test_single_snippet.sh`](../scripts/test_single_snippet.sh)
- [`scripts/log_snippet_folder.sh`](../scripts/log_snippet_folder.sh)
- [`.github/workflows/snippet-review.yml`](../.github/workflows/snippet-review.yml)

Purpose:

- compare curated `nxpp` snippets against their companion reference snippets
- protect the snippet-backed core surface from obvious regressions
- keep the repository aligned with the examples it presents

What it is not:

- not a full assertion-based test suite
- not a guarantee that every edge case or internal invariant is covered

### 3. Formal assertion-based test suite

Entry point:

```bash
bash scripts/run_tests.sh
```

Main test files:

- [`tests/test_core.cpp`](../tests/test_core.cpp)
- [`tests/test_attributes.cpp`](../tests/test_attributes.cpp)
- [`tests/test_edge_cases.cpp`](../tests/test_edge_cases.cpp)
- [`tests/test_flow.cpp`](../tests/test_flow.cpp)
- [`tests/test_remove_node.cpp`](../tests/test_remove_node.cpp)
- [`tests/test_multigraph.cpp`](../tests/test_multigraph.cpp)
- [`.github/workflows/tests.yml`](../.github/workflows/tests.yml)

Purpose:

- run executable assertion-based regression tests
- cover core behavior, attribute failure paths, edge cases, multigraph behavior,
  flow wrappers, and `remove_node()` invariants
- provide the main fast local and CI regression path

What it is not:

- not the standalone single-header verification path
- not the large-graph wrapper-vs-Boost comparison path

### 4. Single-header verification path

Build command:

```bash
bash scripts/build_single_header.sh
```

Test command:

```bash
bash scripts/run_single_header_tests.sh
```

Relevant files:

- [`scripts/build_single_header.sh`](../scripts/build_single_header.sh)
- [`scripts/run_single_header_tests.sh`](../scripts/run_single_header_tests.sh)
- [`.github/workflows/single-header.yml`](../.github/workflows/single-header.yml)

Purpose:

- rebuild `dist/nxpp.hpp` from the modular public headers
- recompile the formal suite against the generated standalone header
- validate the actual release artifact instead of trusting the modular build

What it is not:

- not a benchmark
- not a replacement for the modular formal suite

### 5. Large-graph comparison path

Command:

```bash
bash scripts/run_large_graph_compare.sh
```

Relevant files:

- [`tests/test_large_graph_compare.cpp`](../tests/test_large_graph_compare.cpp)
- [`scripts/run_large_graph_compare.sh`](../scripts/run_large_graph_compare.sh)
- [`.github/workflows/large-graph-compare.yml`](../.github/workflows/large-graph-compare.yml)

Purpose:

- generate deterministic larger graph instances
- compare `nxpp` results directly against raw Boost on the same graphs
- increase confidence that wrapper bookkeeping still stays aligned at larger
  scales than the normal suite usually exercises

Current coverage includes:

- BFS
- DFS tree edges
- connected components
- strongly connected components
- Dijkstra
- Bellman-Ford
- DAG shortest paths
- reachable negative-cycle detection behavior
- large `remove_node()` state
- large multigraph mutation behavior
- large attribute preservation and cleanup after repeated mutations
- large combined weighted-graph mutation sequences
- Floyd-Warshall all-pairs results
- maximum-flow / minimum-cut agreement
- successive-shortest-path min-cost-flow agreement
- representative multi-seed rechecks for several graph families
- one regression with non-default `boost::listS` / `boost::listS` selectors

What it is not:

- not a proof that `nxpp` can never diverge from Boost
- not exhaustive over every supported API or graph configuration
- not meant to replace the fast suite in [`scripts/run_tests.sh`](../scripts/run_tests.sh)
- not a performance benchmark

## Which command to run

### Quick choice

| Situation | Command | Why use it |
|---|---|---|
| Normal development change | `bash scripts/run_tests.sh` | Fast default regression path for day-to-day work |
| Single-header or release-path change | `bash scripts/build_single_header.sh` then `bash scripts/run_single_header_tests.sh` | Verifies the generated `dist/nxpp.hpp` artifact, not just the modular headers |
| Extra confidence against raw Boost on larger graphs | `bash scripts/run_large_graph_compare.sh` | Runs the opt-in large deterministic wrapper-vs-Boost comparison path |
| Snippet/example parity check | `bash scripts/test_single_snippet.sh snippet/bfs` | Checks one curated snippet folder against its companion references |

For normal development:

```bash
bash scripts/run_tests.sh
```

When you changed the single-header build or release path:

```bash
bash scripts/build_single_header.sh
bash scripts/run_single_header_tests.sh
```

When you want extra wrapper-vs-Boost confidence on larger deterministic graphs:

```bash
bash scripts/run_large_graph_compare.sh
```

When you want to inspect snippet parity behavior:

```bash
bash scripts/test_single_snippet.sh snippet/bfs
```

## Practical interpretation

The safest way to read the testing story is:

- `run_tests.sh` is the default fast regression path
- `run_single_header_tests.sh` validates the generated release artifact
- `run_large_graph_compare.sh` adds scale-oriented cross-checks against raw Boost
- snippet tooling protects the curated example/parity layer

Together these paths provide much stronger evidence than any one of them alone,
but they still should be described as a practical regression strategy rather
than a mathematical proof of full equivalence to Boost.
