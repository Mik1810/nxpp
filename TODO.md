# TODO: Issue Index and Prioritization Guide

`TODO.md` is no longer the full backlog.

The canonical source of truth for open work is now the GitHub issue tracker:

- https://github.com/Mik1810/nxpp/issues

This file stays in the repo only as a compact planning index so the current priorities are visible locally without duplicating every task description.

## Priority Rule

Implementation priority is still driven by the local snippet sets:

- `snippet/` is the primary C++ algorithm reference
- colocated Python snippets are the primary behavior/parity reference
- functions represented in those snippet sets define the essential target surface
- work outside that surface can exist, but should not outrank snippet-backed algorithms and correctness fixes

## GitHub Priority Labels

Issue priority is tracked with GitHub labels:

- `priority:critical`: highest-priority correctness and architectural work
- `priority:high`: important follow-up work that should happen soon
- `priority:medium`: useful work that should not outrank the items above
- `priority:low`: future-facing or exploratory work

## Current Priority Bands

### Critical

These items currently outrank most other open work:

- multigraph edge identity and semantics
- long-term public API direction
- BGL configurability / graph-configuration surface

Relevant issues:

- `#1` Redesign multigraph edge identity
- `#2` Specify multigraph behavior for existing public calls
- `#3` Audit `remove_edge()` metadata cleanup in multigraph cases
- `#23` Define the long-term public API shape and compatibility policy
- `#24` Define the BGL configurability and graph-configuration surface

### High

These items materially improve correctness, test confidence, docs, and maintainability:

- assertion-based tests and targeted regression coverage
- internal-state encapsulation
- public-call complexity clarity
- attribute-system direction
- generated docs and markdown source-of-truth cleanup
- clearer separation of examples, snippets, smoke checks, and formal tests

Relevant issues:

- `#4` Add regression tests for `remove_node()` descriptor remapping
- `#5` Clarify which APIs implicitly create missing nodes
- `#8` Add a real assertion-based test suite
- `#9` Add edge-case tests for empty / singleton / disconnected graphs
- `#10` Add attribute failure tests
- `#11` Add multigraph-specific tests
- `#12` Add flow-wrapper tests
- `#25` Audit and reduce exposure of internal mutable state
- `#26` Clarify and strengthen public-call complexity guarantees
- `#27` Re-evaluate the long-term attribute system design
- `#28` Add inline public API docs and a generated-docs scaffold
- `#29` Define markdown source-of-truth rules and reduce documentation drift
- `#30` Separate examples, smoke checks, snippet parity, and formal tests more clearly

### Medium

Useful work that should follow once the higher-risk items are in better shape:

- exception/error-message polish
- compile-time constraints for `NodeID`
- docs/examples improvements
- build and packaging basics
- CI expansion
- compiler/platform support matrix

Relevant issues:

- `#6` Standardize exception wording
- `#7` Add compile-time constraints for `NodeID`
- `#13` Document all thin aliases explicitly
- `#14` Add examples for richer result wrappers
- `#15` Add a "utility wrappers beyond NetworkX/BGL" docs section
- `#16` Clarify weighted-overload semantics
- `#17` Add `CMakeLists.txt`
- `#18` Document external-usage story
- `#19` Define versioning and release policy
- `#20` Extend CI beyond the current Linux snippet workflow
- `#31` Add a minimal compiler and platform support matrix to the README

### Low

These items are valid, but should not outrank current C++ API, correctness, tests, and docs work:

- investigation of older GitHub Actions warnings
- portability exploration
- distribution-channel evaluation after packaging stabilizes
- features outside the current snippet-driven core
- benchmarking
- optional Python bindings

Relevant issues:

- `#21` Inspect GitHub Actions summary for deprecated Node/runtime warnings
- `#22` Evaluate portability of the `nxpp` design to other compiled ecosystems
- `#32` Evaluate distribution channels after the packaging story stabilizes
- `#33` Add `betweenness_centrality`
- `#34` Add `pagerank`
- `#35` Add a benchmarking harness
- `#36` Explore optional Python bindings

## Snippet-Driven Core Surface

The current snippet-backed core surface has already been reviewed manually across:

- `snippet/2sat/`
- `snippet/bellman_ford/`
- `snippet/bfs/`
- `snippet/cc/`
- `snippet/dag_sp/`
- `snippet/dfs/`
- `snippet/dijkstra/`
- `snippet/flow/`
- `snippet/floyd_warshall/`
- `snippet/graph_example/`
- `snippet/graph_weights/`
- `snippet/kruskal/`
- `snippet/mcmf_cc/`
- `snippet/mcmf_ssp/`
- `snippet/prim/`
- `snippet/scc/`
- `snippet/scc_named/`
- `snippet/ts/`

This list is useful as local context, but open work should be tracked and updated in GitHub issues rather than duplicated here.

## Notes

- `graph_example` and `graph_weights` are informational reference snippets rather than standalone algorithm targets.
- The highest-risk implemented area remains multigraph behavior and edge identity semantics.
- The highest-value non-feature improvement remains broader automated verification beyond snippet output comparison.
