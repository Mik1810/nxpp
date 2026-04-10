# Testing Guide

This document explains the verification layers used in `nxpp`, how to run them,
and what each one is meant to prove.

Use the repository `README.md` for the project overview and quick
navigation.
Use this file when you want the repository's testing story in one place.

Generated-reference companion:

- landing page:
  https://mik1810.github.io/nxpp/

Use the generated reference for declaration-level API lookup. Use this guide
for the verification-layer story, commands, and scope of each test path.

## Testing layers

The repository intentionally uses several different verification paths instead
of one giant test command.

The clearest mental model is:

- showcase programs are demos
- `snippet/` is the curated example-plus-parity layer
- `tests/` is the formal assertion-based regression suite
- `scripts/unix/run_external_consumer_tests.sh` is the end-to-end external-consumer integration layer
- `test_large_graph_compare.cpp` is the separate scale-oriented raw-Boost comparison path

## Quick map

| Layer | Main artifacts | Primary purpose | What it is not |
|---|---|---|---|
| Showcase programs | `main_boost.cpp`, `main_nxpp.cpp`, `main.py` | Demonstrate usage and the wrapper-vs-reference story | Not the formal suite |
| Snippet parity / regression | `snippet/`, `scripts/unix/test_single_snippet.sh`, `snippet-review.yml` | Keep curated examples aligned across implementations | Not exhaustive assertion-based testing |
| Formal assertion-based tests | `tests/test_*.cpp`, `scripts/unix/run_tests.sh`, `compatibility.yml` | Catch regressions and enforce behavior | Not the single-header or large-graph path |
| External-consumer integration tests | `tests/external_consumers/`, `scripts/unix/run_external_consumer_tests.sh`, `external-consumers.yml` | Validate real external-consumer integration modes end-to-end | Not a replacement for formal assertion tests |
| Single-header verification | `scripts/unix/build_single_header.sh`, `scripts/unix/run_single_header_tests.sh`, `single-header.yml`, `release.yml` | Validate the generated standalone header through a mix of smoke checks and deeper standalone-header suite runs | Not a replacement for the modular formal suite |
| Large-graph raw-Boost comparison | `tests/test_large_graph_compare.cpp`, `scripts/unix/run_large_graph_compare.sh`, `compatibility.yml` | Cross-check `nxpp` against raw Boost on larger deterministic graphs in a dedicated heavy CI lane | Not a benchmark or a proof of full equivalence |

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
- [`scripts/unix/test_single_snippet.sh`](../scripts/unix/test_single_snippet.sh)
- [`scripts/unix/log_snippet_folder.sh`](../scripts/unix/log_snippet_folder.sh)
- [`.github/workflows/snippet-review.yml`](../.github/workflows/snippet-review.yml)

Purpose:

- compare curated `nxpp` snippets against their companion reference snippets
- protect the example-backed core surface from obvious regressions
- keep the repository aligned with the examples it presents

How to read this layer:

- each snippet folder is primarily an example set
- the parity tooling turns those examples into a lightweight regression harness
- this is why `snippet/` sits between showcase code and the formal test suite

What it is not:

- not a full assertion-based test suite
- not a guarantee that every edge case or internal invariant is covered

### 3. Formal assertion-based test suite

Entry point:

```bash
bash scripts/unix/run_tests.sh
```

Main test files:

- [`tests/test_core.cpp`](../tests/test_core.cpp)
- [`tests/test_attributes.cpp`](../tests/test_attributes.cpp)
- [`tests/test_edge_cases.cpp`](../tests/test_edge_cases.cpp)
- [`tests/test_flow.cpp`](../tests/test_flow.cpp)
- [`tests/test_remove_node.cpp`](../tests/test_remove_node.cpp)
- [`tests/test_multigraph.cpp`](../tests/test_multigraph.cpp)
- [`.github/workflows/compatibility.yml`](../.github/workflows/compatibility.yml)

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
bash scripts/unix/build_single_header.sh
```

Test command:

```bash
bash scripts/unix/run_single_header_tests.sh
```

Relevant files:

- [`scripts/unix/build_single_header.sh`](../scripts/unix/build_single_header.sh)
- [`scripts/unix/run_single_header_tests.sh`](../scripts/unix/run_single_header_tests.sh)
- [`.github/workflows/single-header.yml`](../.github/workflows/single-header.yml)

Purpose:

- rebuild `dist/nxpp.hpp` from the modular public headers
- provide the deeper local verification path for recompiling the assertion-based
  suite against the generated standalone header
- validate the actual release artifact more deeply than a smoke check when you
  intentionally run the standalone-header suite

What the dedicated CI workflow now does:

- [`single-header.yml`](../.github/workflows/single-header.yml) rebuilds
  `dist/nxpp.hpp`
- runs `bash scripts/unix/run_single_header_tests.sh`
- compiles and runs a small smoke program against that generated header
- uploads the generated header artifact

Where the deeper standalone-header suite does run:

- locally when you run `bash scripts/unix/run_single_header_tests.sh`
- in [`release.yml`](../.github/workflows/release.yml) before the release asset
  is published
- in [`release.yml`](../.github/workflows/release.yml) the release path now
  also checks repository-hosted package metadata for version drift through
  `python3 scripts/check_release_metadata_versions.py`

What it is not:

- not a benchmark
- not a replacement for the modular formal suite

### 5. External-consumer integration path

Command:

```bash
bash scripts/unix/run_external_consumer_tests.sh
```

Relevant files:

- [`scripts/unix/run_external_consumer_tests.sh`](../scripts/unix/run_external_consumer_tests.sh)
- [`tests/external_consumers/`](../tests/external_consumers)
- [`.github/workflows/external-consumers.yml`](../.github/workflows/external-consumers.yml)

Purpose:

- exercise supported integration modes as real external consumers
- fail per integration mode so breakages are easier to localize
- keep documented consumption modes trustworthy by testing them directly

Current required coverage:

- vendored CMake consumer with `add_subdirectory(nxpp)`
- installed-package CMake consumer with `find_package(nxpp CONFIG REQUIRED)`
- standalone single-header consumer

Optional local coverage:

- a local Conan consumer mode exists in the runner behind `NXPP_EXTERNAL_CONAN=1`

What it is not:

- not a benchmark
- not a replacement for the modular formal suite

### 6. Large-graph comparison path

Command:

```bash
bash scripts/unix/run_large_graph_compare.sh
```

Relevant files:

- [`tests/test_large_graph_compare.cpp`](../tests/test_large_graph_compare.cpp)
- [`scripts/unix/run_large_graph_compare.sh`](../scripts/unix/run_large_graph_compare.sh)
- [`.github/workflows/compatibility.yml`](../.github/workflows/compatibility.yml)

Purpose:

- generate deterministic larger graph instances
- compare `nxpp` results directly against raw Boost on the same graphs
- increase confidence that wrapper bookkeeping still stays aligned at larger
  scales than the normal suite usually exercises
- keep this heavier path in a dedicated CI lane without mixing its logs into
  the normal fast compatibility test job

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
- two regressions with non-default selectors: `boost::listS` / `boost::listS` and `boost::listS` / `boost::vecS`

What it is not:

- not a proof that `nxpp` can never diverge from Boost
- not exhaustive over every supported API or graph configuration
- not meant to replace the fast suite in [`scripts/unix/run_tests.sh`](../scripts/unix/run_tests.sh)
- not a performance benchmark

## Which command to run

### Quick choice

| Situation | Command | Why use it |
|---|---|---|
| Normal development change | `bash scripts/unix/run_tests.sh` | Fast default regression path for day-to-day work |
| Verify documented external integration modes | `bash scripts/unix/run_external_consumer_tests.sh` | Confirms real external-consumer workflows still work |
| Single-header or release-path change | `bash scripts/unix/build_single_header.sh` then `bash scripts/unix/run_single_header_tests.sh` | Matches the deeper standalone-header suite now also used by `single-header.yml` and `release.yml` |
| Extra confidence against raw Boost on larger graphs | `bash scripts/unix/run_large_graph_compare.sh` | Runs the opt-in large deterministic wrapper-vs-Boost comparison path |
| Snippet/example parity check | `bash scripts/unix/test_single_snippet.sh snippet/bfs` | Checks one curated snippet folder against its companion references |

For normal development:

```bash
bash scripts/unix/run_tests.sh
```

When you changed packaging or consumer-facing integration paths:

```bash
bash scripts/unix/run_external_consumer_tests.sh
```

When you changed the single-header build or release path:

```bash
bash scripts/unix/build_single_header.sh
bash scripts/unix/run_single_header_tests.sh
```

When you want extra wrapper-vs-Boost confidence on larger deterministic graphs:

```bash
bash scripts/unix/run_large_graph_compare.sh
```

When you want to inspect snippet parity behavior:

```bash
bash scripts/unix/test_single_snippet.sh snippet/bfs
```

## Practical interpretation

The safest way to read the testing story is:

- `scripts/unix/run_tests.sh` is the default fast regression path
- `scripts/unix/run_external_consumer_tests.sh` is the dedicated end-to-end integration path for documented external consumer modes
- `single-header.yml` now provides a dedicated standalone-header workflow that runs both the standalone-header suite and a smoke test
- `scripts/unix/run_single_header_tests.sh` is the deeper local standalone-header verification path
- `release.yml` is the path that currently rebuilds the standalone header and runs the standalone-header suite before publishing the release asset
- `scripts/unix/run_large_graph_compare.sh` adds scale-oriented cross-checks against raw Boost
- snippet tooling protects the curated example/parity layer
- showcase programs stay demos and should not be treated as proof on their own

Together these paths provide much stronger evidence than any one of them alone,
but they still should be described as a practical regression strategy rather
than a mathematical proof of full equivalence to Boost.
