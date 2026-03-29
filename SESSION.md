# Session Log: nxpp Project MVP & Test Suite

**Date:** March 24, 2026

## Objectives Reached in this Session

1. **Roadmap Definition and Planning**: 
   - Analysis of the initially provided isolated snippets regarding the Boost Graph Library engine configurations.
   - Creation of the comprehensive `ROADMAP.md` file to divide the C++ continuous integration of a "NetworkX-like" framework into 6 incremental phases.

2. **Base BGL Wrappers Integration (Phase 3)**:
   - Modified the base structure in previously deprecated headers to comfortably expose the encapsulated BGL core variables (`get_impl()`).
   - Created core pure C++ interfaces that perfectly simulate NetworkX behavioral signatures: `bfs_edges`, `dfs_edges`, `dijkstra_path`, `connected_components`, `strongly_connected_components`.

3. **C++ vs Python Testing Infrastructure (Phase 4)**:
   - Setup of a local virtual environment (`.venv`) for isolated installation of the `networkx` original Python module.
   - Initialization of a continuous comparative workflow generating network baselines using strictly sequential operations.
   - Creation of a heavily BGL-typed C++ test runner to load mathematically identical graphs, invoke compiled wrappers, and isolate outputs natively to `out_cpp.txt`.
   - Final terminal comparative processing via diff logs guaranteed a perfect **100% Topographical Match**.

4. **Python-Equivalent Algorithmic Prototypes**:
   - Examined internal snippets iteratively to map C++ output vectors cleanly, achieving 1-to-1 alignment with python outputs.

## Milestone 2: Generics & Header-Only Refactoring
1. **Library Architecture Consolidation (Single-Header)**: 
   - Fully eliminated the obsolete subdirectory tree architecture and successfully incorporated all active templates, algorithm modules, and utility helpers into a single overarching `include/nxpp.hpp` file.
2. **C++ Generics Architecture (NodeID)**:
   - Replaced fixed integer BGL Graph layouts with the infinitely adjustable template class `nxpp::Graph<NodeID>`. 
   - Implemented automatic memory-safe Hash Maps translating between Custom Generic IDs (textual inputs) and opaque Boost array allocation integer indices seamlessly. 
   - Added convenient User-Aliases like `nxpp::DiGraph` (which defaults directly to `std::string` nodes) for native rapid memory instantiation mirroring Python dynamics.
3. **Dictionary-like API System**:
   - Engineered the *EdgeProxy* to cleanly allow overriding sequential `G[Source][Target] = weight` mechanics simulating the robust continuous Python accessor dictionary lookups at O(1) speeds.
4. **I/O Display Simulator and Macro Parity**:
   - Brought forward a global variadic template `nxpp::print()` removing visually disruptive syntax elements across standard output mechanisms.
   - Reached unified topological code alignments linking `main.cpp` syntactically closer to `main.py` structural sequences than ever before.

## Milestone 3: Destruction Algorithms, MultiGraph, and Parity
1. **Node and Edge Destruction Algorithms (Phase 3)**:
   - Carefully maneuvered and securely integrated `clear()`, `remove_edge()`, `remove_node()` natively via internal proxy pointers.
   - Enforced standard C++ `vecS` array pointer translations leveraging standard O(1) *Memory Swap* logics natively built into BGL. This actively neutralized the critical topological array corruption invalidating downstream algorithms upon arbitrary deletion.
2. **Parallel Sub-architectures (Phase 1.5)**:
   - Resolved a massive implicit topological duplication glitch on the regular `nxpp::Graph` by automatically blocking repetitive shadow edge insertions. This mimics native generic python override behavior logic on identical key re-writes.
   - Finalized compilation sequences activating the new backend generic template parameter `bool Multi` to instantly unseal `MultiGraph` and `MultiDiGraph` multigraph structures enabling perfectly scaled parallel edges on demand.
3. **Comprehensive Documentation Refactoring**:
   - Rewrote `README.md` entirely emphasizing objective tone and deep architectural analyses.
   - Meticulously translated and maintained updates to both `ROADMAP.md` and `SESSION.md` fully recording Phase 1, Phase 2, and partial Phase 3 validations natively.
4. **Zero-Diff Parity Verification**:
   - Resolved non-deterministic SCC/CC ordering issues via lexicographical sorting in both languages.
   - Fixed `remove_node` vertex shifting bugs to ensure 100% BFS path consistency.
   - Achieved a perfect 0-diff output comparison between `nxpp` and `networkx`.
5. **Topological Generators (Phase 3)**:
   - Implemented `nxpp::complete_graph`, `nxpp::path_graph`, and `nxpp::erdos_renyi_graph`.
   - Optimized for generic `NodeID` via template overrides.
   - Integrated deterministic sorting in tests to maintain parity in complex graph structures.

## Session Audit: March 25, 2026

1. **Roadmap and Repository Audit**:
   - Reviewed `ROADMAP.md`, `SESSION.md`, `README.md`, and the active code in `include/nxpp.hpp`.
   - Confirmed that the only planned work still open is the full Phase 5 block: flows, centrality metrics, and benchmarking.
   - Confirmed that no partial implementation for `maximum_flow`, `minimum_cut`, `betweenness_centrality`, or `pagerank` currently exists in the codebase.

2. **Dangling Build Issue Resolved**:
   - Ran the standard build command `g++ -std=c++20 -Wall -pedantic -O3 main.cpp -o main`.
   - Found a stale alias declaration: `using DiGraphStr = DiGraph<std::string, double>;`.
   - Removed the stale alias after confirming it had no meaningful purpose in the current codebase, restoring compile compatibility and simplifying the public alias surface.

3. **Documentation Alignment**:
   - Updated `ROADMAP.md` with an explicit current-status audit.
   - Updated `README.md` to record the standard build command and current project status.

5. **Alias Cleanup**:
   - Removed `DiGraphStr` from `include/nxpp.hpp` after confirming it was unused and unnecessary.
   - Removed `GraphStr` from `include/nxpp.hpp` after confirming it was also unused in the current repository.
   - Kept the build green after the cleanup to avoid preserving dead API surface.

6. **Header Architecture Documentation**:
   - Re-read `include/nxpp.hpp` end-to-end and documented its real operational model in `README.md`.
   - Added a technical explanation of the `NodeID <-> BGL descriptor` translation layer, proxy-based API emulation, algorithm wrapper flow, and current internal invariants.
   - Recorded known caveats explicitly so the README now describes both strengths and current risk areas of the header.

7. **Stable Edge Attribute Refactor**:
   - Replaced `edge_properties` keyed by `EdgeDesc` with a stable internal edge-ID mechanism.
   - Updated edge creation to assign persistent edge IDs at insertion time.
   - Updated `EdgeAttrProxy` to resolve attributes through stable edge IDs instead of raw BGL descriptors.
   - Updated `clear()`, `remove_edge()`, and `remove_node()` so structural deletion also cleans node and edge metadata consistently.
   - Recompiled `main.cpp` successfully after the refactor.

8. **Directed Neighbor API Clarification**:
   - Kept `neighbors()` aligned with directed successor semantics.
   - Added explicit `successors()` and `predecessors()` methods to `Graph`.
   - Added a small usage example in `main.cpp` and documented the semantics in `README.md`.

9. **Safer Attribute Access Layer**:
   - Kept `std::any`-based proxy syntax for parity-oriented ergonomics.
   - Added explicit `has_*_attr`, `get_*_attr<T>`, and `try_get_*_attr<T>` APIs for safer reads.
   - Routed proxy reads through the checked getter layer so type mismatches and missing keys now produce clearer runtime failures.
   - Updated `main.cpp` and `README.md` to demonstrate the safer attribute access pattern.

10. **Algorithm Wrapper Coverage Alignment**:
   - Audited the actual contents of `include/nxpp.hpp` against `ROADMAP.md`.
   - Identified that some algorithms marked as completed in documentation were not actually present as callable wrappers.
   - Added basic `bfs_tree()`, `dfs_tree()`, `shortest_path()`, and `bellman_ford_path()` wrappers to reduce the mismatch between code and documented API surface.
   - Added simple coverage usage in `main.cpp` and updated `README.md` wording to describe the wrapper layer more accurately.

11. **Shortest Path Semantics Improvement**:
   - Reworked `shortest_path()` so the default call now behaves as an unweighted shortest path by edge count instead of aliasing directly to Dijkstra.
   - Added overloads that accept a `weight` string for `shortest_path()`, `dijkstra_path()`, and `bellman_ford_path()`.
   - Kept the supported weighted case intentionally narrow: only the built-in `"weight"` property is accepted for now.
   - Updated `main.cpp` and `README.md` to reflect the clearer split between weighted and unweighted path semantics.

12. **Shortest Path Length Alignment**:
   - Added `shortest_path_length()` for source-target queries with the same weighted vs unweighted dispatch model as `shortest_path()`.
   - Added source-target overloads for `dijkstra_path_length()` and `bellman_ford_path_length()`.
   - Updated `main.cpp` and `README.md` so path-returning and distance-returning APIs are documented together.

13. **Snippet-Driven Priority Reset**:
   - Audited the real local reference folders and confirmed that the active C++ algorithm references live in `snippet/`, while Python behavior references live in `py_snippet/`.
   - Re-centered project priority around functions that actually appear in those snippet sets.
   - Added `TODO.md` to separate snippet-backed essentials from secondary work that is not currently represented in the snippet folders.
   - Updated `README.md` and `ROADMAP.md` so future work stays anchored to snippet-backed algorithms rather than drifting into lower-priority API expansion.

14. **First Snippet-Backed Essentials Batch**:
   - Added `topological_sort()`, `kruskal_minimum_spanning_tree()`, `prim_minimum_spanning_tree()`, `maximum_flow()`, `dag_shortest_paths()`, `floyd_warshall_all_pairs_shortest_paths()`, and `two_sat_satisfiable()` to `include/nxpp.hpp`.
   - Added a `MaximumFlowResult` return type and numeric edge-attribute helper to support snippet-style flow use cases.
   - Added a usage block in `main.cpp` that exercises the newly added snippet-backed APIs.
   - Updated `TODO.md`, `README.md`, and `ROADMAP.md` so the missing essentials list now reflects the newly implemented functions.

15. **Remaining Snippet Essentials Closed**:
   - Added min-cost max-flow wrappers for both cycle-canceling and successive-shortest-path variants.
   - Added `strongly_connected_component_roots()` to expose the SCC representative/root-map behavior shown in the named SCC snippet.
   - Extended `main.cpp` with a small runtime exercise for the new flow and SCC-root helpers.
   - Updated `TODO.md`, `README.md`, and `ROADMAP.md` so the snippet-backed essentials list is now fully checked off.

16. **Proxy API IntelliSense Fix**:
   - Added explicit `const char*` overloads for proxy `operator[]` lookups on node and edge attribute access.
   - This reduces editor-side ambiguity on chained expressions such as `G["u"]["v"]["key"]` and `G.node("u")["key"]`.
   - Recompiled `main.cpp` successfully after the change.

17. **Boost Warning Containment**:
   - Isolated the large GCC `-Wmaybe-uninitialized` warning burst to Boost min-cost-flow headers, specifically the `cycle_canceling` include path.
   - Added a narrow GCC-only diagnostic guard around the affected Boost flow includes in `include/nxpp.hpp`.
   - Kept the suppression local to third-party headers instead of weakening warnings across project code.

18. **Minimum Cut Added**:
   - Implemented `minimum_cut()` on top of the same directed flow construction used by `maximum_flow()`.
   - The wrapper computes max flow, explores the residual graph from the source, and returns the cut value, partition, and crossing edges.
   - Added a small `main.cpp` exercise and updated `README.md` and `ROADMAP.md` so the network-flow block is documented as present.

19. **Colocated Python Snippets Added**:
   - Created Python `networkx` translations directly beside every `.cpp` file under `snippet/`.
   - Matched each translation to the intent and output style of the local C++ snippet as closely as practical.
   - Verified syntax for all newly created Python files with `python3 -m py_compile`.
   - Updated `README.md` and `ROADMAP.md` to record that `snippet/` now carries side-by-side C++ and Python references.

20. **Colocated nxpp Snippets Added**:
   - Added `*_nxpp.cpp` variants beside the existing Boost and NetworkX snippets throughout `snippet/`.
   - Built each nxpp example on top of `include/nxpp.hpp`, using direct nxpp wrappers where possible and snippet-local logic where exact BGL-style output required extra formatting or traversal bookkeeping.
   - Reconciled the remaining output mismatches in the Python companions (`dfs`, `floyd_warshall`, `kruskal`, `ts`) so the three versions now agree on observable output.
   - Cleaned out generated `__pycache__` directories and added ignore rules so future snippet verification runs stay cleaner.

21. **Boost-Style and NetworkX-Style APIs Expanded**:
   - Added `breadth_first_search()` and `depth_first_search()` visitor-object entry points so nxpp can express traversal snippets in a style much closer to Boost while still hiding BGL descriptor types from the user.
   - Added `default_graph_visitor<GraphWrapper>` as a generic no-op visitor base class shared by both BFS and DFS examples, so user visitors can derive from one nxpp-owned abstraction instead of raw snippet-local structs.
   - Kept the lighter callback wrappers (`bfs_visit()` / `dfs_visit()`) as thin adapters on top of the visitor-object traversal layer.
   - Added `bfs_successors()`, `dfs_predecessors()`, and `dfs_successors()` for more result-oriented traversal usage closer to NetworkX conventions.
   - Extended `single_source_dijkstra()`, `single_source_bellman_ford()`, and `single_source_dag_shortest_paths()` so they now expose distances, predecessors, and full reconstructed paths together.
   - Simplified the nxpp BFS/DFS/shortest-path snippets to use these higher-level APIs while preserving output parity with the original Boost examples.

22. **Common Graph Aliases Restored**:
   - Added `GraphInt` and `GraphStr` as public aliases for the common simple-graph defaults with integer and string node IDs.
   - Kept the existing directed and multigraph aliases alongside them so snippets can prefer readable public type names over local `using` declarations or raw template spellings.

23. **Component Map Helpers Added**:
   - Added `connected_component_map()` and `strongly_connected_component_map()` so users can obtain `node -> component_id` mappings directly without flattening grouped component results by hand.
   - Updated the `cc` and `scc` nxpp snippets to use these helpers, which removes repetitive indexing boilerplate and makes the snippets read more like library usage examples.

24. **Initial Shell Snippet Harness Added**:
   - Fixed the `cc` / `scc` nxpp snippets to use `.at(...)` on const component maps, resolving the editor/compiler complaint caused by `unordered_map::operator[]` on const objects.
   - Added `scripts/test_snippet_batch.sh` as a first shell harness covering `2sat`, `bellman_ford`, `bfs`, and `cc`.
   - The script compiles the Boost and nxpp C++ snippets, runs the Python snippet with the repo virtualenv, measures compile and execution times, and writes the report both to the terminal and to a timestamped log under `logs/`.
   - The reference Boost snippet compilations are now run with warnings silenced inside the harness so known third-party warning noise does not drown out nxpp-related signal in the test report.

25. **Const-Friendly Lookup Wrapper Added**:
   - Added a small `lookup_map<Key, Value>` wrapper in `nxpp` so mapped algorithm results can support `operator[]` in both mutable and const-read contexts.
   - Updated `connected_component_map()` and `strongly_connected_component_map()` to return this wrapper instead of a raw `std::unordered_map`.
   - Restored `comp_index[i]` syntax in the `cc` and `scc` nxpp snippets without requiring `.at(...)`.

26. **Snippet Progress Checkpoint**:
   - Snippet cleanup, API shaping, and parity work have now been reviewed through `2sat`, `bellman_ford`, `bfs`, and `cc`.
   - The shell harness coverage currently matches that checkpoint and stops at `cc`.
   - The next snippet queued for the same pass is `dag_sp`.

## Session Audit: March 27-28, 2026

1. **Snippet Review Completed**:
   - Continued and completed the manual parity pass across the remaining snippet folders under `snippet/`.
   - Verified the reviewed cases against the local shell harness and then mirrored that coverage in the GitHub Actions snippet-review workflow summary.
   - Closed the last parity blockers around `dag_sp` and `floyd_warshall`.

2. **Shortest-Path Wrapper Alignment**:
   - Renamed and reshaped several wrappers to stay closer to the Boost naming used by the reference snippets.
   - Promoted `dijkstra_shortest_paths()` as the main single-source Dijkstra result helper.
   - Simplified DAG shortest paths so `dag_shortest_paths()` now exposes distances, predecessors, and paths directly.
   - Renamed the successive-shortest-path min-cost-flow wrapper to `successive_shortest_path_nonnegative_weights()`.

3. **Weighted Integer Alias Fixes**:
   - Corrected the weighted `*Int` graph aliases so they actually use integer edge weights instead of silently defaulting to `double`.
   - This resolved parity issues in integer-weight snippet outputs, especially around unreachable-distance handling and integer-style output formatting.

4. **Snippet Harness and CI Improvements**:
   - Expanded `test_single_snippet.sh` to compare all implementations present in a snippet folder pairwise.
   - Added `log_snippet_folder.sh` for whole-folder execution/logging.
   - Added and refined the GitHub Actions snippet-review workflow, including clearer failure logging and a summary table with English descriptions.

5. **Documentation and Repo Metadata Updates**:
   - Added a MIT `LICENSE` file at the repository root.
   - Updated `README.md`, `TODO.md`, and `CHANGELOG.md` to reflect the completed snippet review pass and the remaining top-priority architecture questions.
   - Elevated the API-architecture decision (`nxpp::` free functions vs. graph methods) and Boost configurability exposure (`vecS`, `setS`, etc.) as top-priority design work.

4. **Phase 5 Kickoff: Degree Centrality**:
   - Implemented `nxpp::degree_centrality()` in `include/nxpp.hpp`.
   - Matched the standard NetworkX normalization rule using degree divided by `n - 1`, with zeroed output for graphs with fewer than 2 nodes.

## Session Audit: March 29, 2026

1. **Graph Configuration Surface Completed (`#24`)**:
   - Extended `nxpp::Graph` so advanced users can customize both `OutEdgeSelector` and `VertexSelector` directly while preserving the existing alias defaults on `boost::vecS` / `boost::vecS`.
   - Removed the remaining internal assumption that vertex descriptors behave like dense indices.
   - Added wrapper-side vertex index bookkeeping so selector combinations such as `boost::listS` and `boost::setS` can be used in direct `Graph<...>` instantiations.
   - Added compile-time validation for unsupported combinations such as `Multi=true` with `boost::setS`.

2. **Method-Based Demo Refresh**:
   - Updated `main.cpp` to the current method-first API direction.
   - Removed the remaining dependence on deprecated free-function wrappers in the demo/smoke path.

3. **Documentation Realignment**:
   - Updated `README.md` to reflect the current configuration surface, project status, issue-driven risk areas, and testing story.
   - Updated `docs/GRAPH_CONFIGURATION.md` so the selector policy matches the actual implementation instead of the earlier partial state.
   - Started explicit release versioning with `v0.4.1` and recorded the release entry in `CHANGELOG.md`.

4. **Verification**:
   - Recompiled `main.cpp` successfully after the selector refactor and demo cleanup.
   - Verified smoke builds using non-default selector combinations, including `boost::listS` and `boost::setS` as custom vertex selectors.
   - Rechecked `docs/GRAPH_CONFIGURATION.md` against the current implementation and tightened the backend wording so the directed selector policy now explicitly states `boost::bidirectionalS` vs `boost::undirectedS`.
   - Added a repo-local VS Code C/C++ configuration under `.vscode/c_cpp_properties.json` aligned with `/usr/bin/g++`, `linux-gcc-x64`, and `c++20` to reduce IntelliSense/parser drift against the actual build setup.
   - Prepared `RELEASE_NOTES.md` for the first explicit GitHub release associated with tag `v0.4.1`.
   - Audited `remove_edge(u, v)` in multigraph cases and fixed metadata cleanup so the wrapper now erases tracked properties for all parallel edges removed by the underlying Boost call.
   - Fixed the snippet timing scripts to stop hardcoding `/usr/bin/time`; they now resolve a usable GNU `time` binary (`/usr/bin/time`, `/bin/time`, or `gtime`) and fail clearly if timing capture is unavailable.
   - Added `scripts/benchmark_snippet_compile_parallel.sh` as a separate throughput-oriented benchmark driver: it keeps iterations serial within each snippet but can distribute different snippet folders across multiple jobs via `xargs -P`.
   - Added `--verbose` to both compile benchmark scripts so the default output is now compact (`snippet run/iterations`), while the detailed headers/timings/summaries remain available on demand; also removed `bash -l` from the parallel worker launcher to avoid NVS startup noise in child jobs.
   - Refined the compact benchmark progress format to `SNIPPET: <name> | RUN: <i>/<n>` so parallel output remains readable even when multiple snippet jobs interleave.
   - Audited the current multigraph semantics for `has_edge`, `get_edge_weight`, `get_edge_attr`, `has_edge_attr`, `try_get_edge_attr`, `get_edge_numeric_attr`, `G[u][v]`, and `add_edge(..., attrs)`: all of these still resolve through `boost::edge(u, v, g)` and therefore do not identify one specific parallel edge in a stable public way.
   - Updated `README.md` to make that current behavior explicit while keeping the actual redesign of multigraph edge identity deferred to issue `#1`.
   - Introduced a public `edge_id` path for precise multigraph work in `include/nxpp.hpp`: edge creation can now return an id via `add_edge_with_id(...)`, parallel edges can be enumerated with `edge_ids(...)`, and specific edges can be inspected/mutated/removed through edge-id-based accessors.
   - Added `ISSUE_1_CLOSE.md` at the repository root so the final closure message for issue `#1` is available as a plain markdown file in the repo, independent of chat rendering quirks.
   - Began the documentation cleanup linked to issue `#29`: moved the heavy API reference content out of the root `README.md` into `docs/API_REFERENCE.md`, tightened `docs/README.md` as a clearer docs index, and restored the root README to a lighter overview/navigation role.
   - Finished the README/doc-role alignment for `#29` by explicitly documenting `docs/API_REFERENCE.md` as the detailed technical reference and narrowing the stated role of the root README to overview/quick-start/navigation.
   - Reworked `main.cpp` into a cleaner showcase demo centered on the library's practical differences from raw Boost: string node IDs, attribute access, materialized shortest-path results, generators, flow helpers, and precise multigraph edge IDs.
   - Removed the unused `nxpp::print` helper from `include/nxpp.hpp` and from `docs/API_REFERENCE.md` so the public surface matches the intended direction of the demo/documentation.
   - Split the showcase layer into three aligned entry points: `main_boost.cpp` now presents the raw Boost version, `main_nxpp.cpp` carries the wrapper-focused demo, and `main.py` mirrors the same sections in NetworkX/Python.
   - Tightened the demos to avoid explicit `std::string(...)` construction and dropped the old `long` literal suffixes in the showcased attribute payloads so the examples read more naturally.
   - Removed the generic template-style print helpers from the showcase demos and replaced them with more direct section-local output plus short human-like comments explaining the point of each block.
   - Normalized attribute values stored through `std::any` so C-string inputs now become `std::string` internally; this fixes typed string reads after calls such as `set_edge_attr(..., "fast")` and initializer-list attribute insertion with plain string literals.
   - Tightened `main.py` so it reads as a true companion to `main_boost.cpp` and `main_nxpp.cpp`, and replaced `single_source_dijkstra(...)` with the more explicit path-length/path helpers to avoid the Pylance union-type warning on `distances[...]`.
   - Prepared the `0.5.0` release after closing the previous critical issue block, updated the versioned docs (`README.md`, `CHANGELOG.md`, `RELEASE_NOTES.md`), and framed the next project phase around formal tests and packaging polish rather than multigraph correctness triage.
   - Packaged the current showcase/docs state for release, including the aligned `main_boost.cpp` / `main_nxpp.cpp` / `main.py` trio and the plan to ship `include/nxpp.hpp` as a dedicated release asset alongside the standard source archives.
   - Added `ISSUE_HEADER_SPLIT.md` at the repository root with a ready-to-file issue proposal for splitting the current single-header public surface into semantic headers while preserving `include/nxpp.hpp` as the umbrella include.

## 2026-03-29
- Tightened test output formatting in tests/test_core.cpp and scripts/run_tests.sh to print only one colored [TEST] ... | PASS/FAIL line per test plus a colored final summary.
- Added a dedicated GitHub Actions workflow for the formal test suite in `.github/workflows/tests.yml`; it installs Boost, runs `bash scripts/run_tests.sh`, and publishes the captured output as a Markdown job summary.

## 2026-03-29
- Added `.tmp/` to `.gitignore` so local test/build scratch output stays out of git status.

## 2026-03-29
- Added a blank line before the final test-suite summary in tests/test_core.cpp so the aggregate result is visually separated from the per-test lines.

## 2026-03-29
- Refined the GitHub Actions test summary: renamed the heading, reduced status text to SUCCESS/FAILURE only, and stripped ANSI color codes before embedding test output into the Markdown job summary.

## 2026-03-29
- Added `tests/test_multigraph.cpp` for issue `#11` and extended `scripts/run_tests.sh` to compile and execute both the core suite and the new multigraph-specific regression checks.

## 2026-03-29
- Added `ISSUE_8_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #8.

## 2026-03-29
- Simplified the local test runner output: removed per-binary summary lines from `tests/test_core.cpp` and `tests/test_multigraph.cpp`, and moved the aggregate count to a single final `[TEST] All tests passed (x/y) | PASS/FAIL` line in `scripts/run_tests.sh`.

## 2026-03-29
- Updated `scripts/run_tests.sh` to group test output by source file with `Testing <file>` headings and a blank separator between test binaries.

## 2026-03-29
- Added `tests/test_attributes.cpp` for issue `#10` and extended `scripts/run_tests.sh` plus the README/changelog coverage notes to include attribute failure-path regression tests.

## 2026-03-29
- Added `tests/test_edge_cases.cpp` for issue `#9` and extended the runner/docs to cover empty graphs, singleton graphs, missing-node operations, unreachable shortest paths, and disconnected components.

## 2026-03-29
- Added `tests/test_flow.cpp` for issue `#12` and extended the runner/docs to cover max-flow, min-cut, min-cost flow wrappers, and the cached-state failure path for `cycle_canceling()`.

## 2026-03-29
- Added `tests/test_remove_node.cpp` for issue `#4` and extended the runner/docs to cover `remove_node()` remapping, attribute cleanup, component views, and multigraph incident-edge cleanup.

## 2026-03-29
- Added `ISSUE_11_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #11.

## 2026-03-29
- Added `ISSUE_10_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #10.

## 2026-03-29
- Added `ISSUE_9_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #9.

## 2026-03-29
- Added `ISSUE_12_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #12, including the note that the min-cost reference value was verified against the current wrapper/snippet behavior and is 22.

## 2026-03-29
- Added `ISSUE_4_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #4.

## 2026-03-29
- Prepared the `0.6.0` release after closing the first formal test-suite block, updated the versioned docs (`README.md`, `CHANGELOG.md`, `RELEASE_NOTES.md`), and framed the release around the new 31-test formal suite plus its dedicated GitHub Actions workflow.

## 2026-03-29
- Saved a safety copy of the pre-split umbrella header to `backups/nxpp.hpp.backup-2026-03-29` before starting header modularization work.
- Added the first-stage semantic public include layout under `include/nxpp/` as bridge headers (`graph.hpp`, `attributes.hpp`, `traversal.hpp`, `shortest_paths.hpp`, `components.hpp`, `spanning_tree.hpp`, `flow.hpp`, `generators.hpp`, `multigraph.hpp`) while keeping `include/nxpp.hpp` as the canonical source of truth.
- Added `scripts/build_single_header.sh` as the initial helper for rebuilding a distributable single header from the current umbrella header.
- Promoted the first real split step by moving the core `Graph` type, base structural methods, and public aliases into `include/nxpp/graph.hpp`, and by moving attribute-aware overloads/accessors into `include/nxpp/attributes.hpp`.
- Reworked `include/nxpp.hpp` into an umbrella header that includes the new `graph.hpp` and `attributes.hpp` sources of truth while keeping the remaining implementation in place for later split stages.
- Re-verified the split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, and compiling a direct include smoke test against `include/nxpp/attributes.hpp`.
- Promoted `include/nxpp/multigraph.hpp` from a bridge header to the real home of the `edge_id`-based multigraph API, including `add_edge_with_id(...)`, `edge_ids(...)`, `remove_edge(edge_id)`, and the `edge_id` attribute/weight accessors.
- Re-verified the multigraph split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, and compiling a direct include smoke test against `include/nxpp/multigraph.hpp`.
- Promoted `include/nxpp/traversal.hpp` from a bridge header to the real home of the BFS/DFS visitor helpers, deprecated traversal wrappers, and the `Graph` traversal method definitions.
- Re-verified the traversal split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, and compiling a direct include smoke test against `include/nxpp/traversal.hpp`.
- Promoted `include/nxpp/shortest_paths.hpp` from a bridge header to the real home of the shortest-path helpers, deprecated shortest-path wrappers, and the `Graph` shortest-path method definitions, including the `SingleSourceShortestPathResult` type needed for direct semantic-header use.
- Re-verified the shortest-path split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, compiling a direct include smoke test against `include/nxpp/shortest_paths.hpp`, and rebuilding the generated single-header output with `scripts/build_single_header.sh`.
- Promoted `include/nxpp/components.hpp` from a bridge header to the real home of the component helpers, deprecated component wrappers, and the `Graph` component method definitions.
- Promoted `lookup_map` into `include/nxpp/graph.hpp` so direct semantic-header includes can use component-return helpers without depending on the umbrella header.
- Re-verified the component split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, and compiling a direct include smoke test against `include/nxpp/components.hpp`.
- Promoted `include/nxpp/spanning_tree.hpp` from a bridge header to the real home of topological sort, minimum-spanning-tree helpers, deprecated wrappers, and the matching `Graph` method definitions.
- Re-verified the spanning-tree split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, and compiling a direct include smoke test against `include/nxpp/spanning_tree.hpp`.
- Promoted `include/nxpp/flow.hpp` from a bridge header to the real home of the flow/cut/min-cost helpers, deprecated wrappers, supporting result types, and the matching `Graph` method definitions.
- Moved the flow-specific result structs and cached-state helpers (`MaximumFlowResult`, `MinimumCutResult`, `MinCostMaxFlowResult`, and `detail::MinCostFlowState`) into `include/nxpp/flow.hpp` so direct semantic-header includes no longer depend on the umbrella header for those types.
- Re-verified the flow split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, and compiling a direct include smoke test against `include/nxpp/flow.hpp`.

## 2026-03-29
- Finished the semantic-header split by moving the remaining `num_vertices()` / `degree_centrality()` core definitions into `include/nxpp/graph.hpp` and verifying that `include/nxpp/generators.hpp` now works as a true direct include instead of accidentally depending on the umbrella header for those core methods.
- Re-verified the completed split by running `bash scripts/run_tests.sh` (`31/31` passing), rebuilding `main_nxpp.cpp`, compiling a direct include smoke test against `include/nxpp/generators.hpp`, and rebuilding the generated single-header output with `scripts/build_single_header.sh`.
- Updated `README.md` and `docs/README.md` to describe the semantic-header layout as a real split, while keeping `include/nxpp.hpp` documented as the umbrella include and home of the small leftover utility tail such as the 2-SAT helper.

## 2026-03-29
- Split the leftover 2-SAT helpers out of the umbrella header into `include/nxpp/sat.hpp`, so the semantic-header layout no longer needs an odd utility tail inside the root include.
- Introduced `include/nxpp/nxpp.hpp` as the real umbrella header and turned `include/nxpp.hpp` into a compatibility shim that forwards to the new path.
- Updated the public docs to describe `sat.hpp`, the new umbrella path, and the fact that the rebuild script currently republishes the umbrella header rather than producing a true standalone amalgamated file.
- Re-verified the new include layout by running `bash scripts/run_tests.sh` (`31/31` passing), compiling direct smoke tests against `include/nxpp/nxpp.hpp`, `include/nxpp.hpp`, and `include/nxpp/sat.hpp`, and rebuilding the current single-header output helper.

## 2026-03-29
- Simplified the umbrella layout again by restoring `include/nxpp.hpp` as the real umbrella header, deleting the redundant `include/nxpp/nxpp.hpp`, and keeping the semantic headers under `include/nxpp/` as the modular source of truth.
- Updated the rebuild helper to point back at `include/nxpp.hpp`, and re-verified the layout by running `bash scripts/run_tests.sh` (`31/31` passing) plus direct smoke tests against `include/nxpp.hpp`, `include/nxpp/flow.hpp`, and `include/nxpp/sat.hpp`.

## 2026-03-29
- Added `scripts/build_single_header.py` as the real single-header builder and turned `scripts/build_single_header.sh` into a thin wrapper that calls it.
- The builder now expands local `nxpp` headers recursively from `include/nxpp.hpp`, leaves external Boost/std includes intact, skips duplicate local files, and writes a standalone `dist/nxpp.hpp` guarded by `NXPP_SINGLE_HEADER_HPP`.
- Added `dist/` to `.gitignore`, updated the docs to describe `dist/nxpp.hpp` as a generated distribution artifact, and verified the generated file by compiling a direct smoke test that includes `/workspaces/nxpp/dist/nxpp.hpp` and runs successfully.

## 2026-03-29
- Marked the semantic-header and standalone single-header work as release-worthy by adding a `0.7.0` entry to `CHANGELOG.md` that records the new modular include layout, `sat.hpp`, the restored `include/nxpp.hpp` umbrella, and the new standalone `dist/nxpp.hpp` builder.

## 2026-03-29
- Added `include/nxpp/topological_sort.hpp` so the DAG ordering helper no longer shares a header with the MST helpers, and updated `include/nxpp.hpp` plus `include/nxpp/spanning_tree.hpp` accordingly.
- Tightened the semantic-header dependency graph further by cutting the old include chain and keeping `flow.hpp` explicitly above `attributes.hpp` while the other algorithm headers now depend directly on `graph.hpp` or only on the narrow headers they truly need.
- Refactored the `*_nxpp.cpp` snippets to include their narrow semantic headers instead of defaulting to `include/nxpp.hpp`; the only 2-SAT exception is `2sat_helper_nxpp.cpp`, which now uses `sat.hpp` because it calls `two_sat_satisfiable(...)` directly.
- Verified the snippet include refactor by compiling the touched snippets in batches, rerunning `bash scripts/run_tests.sh` (`31/31` passing), and rebuilding the standalone `dist/nxpp.hpp` output.

## 2026-03-29
- Narrowed the include surface of the formal tests where it still stayed readable: `test_attributes.cpp` now uses `attributes.hpp`, `test_multigraph.cpp` uses `multigraph.hpp`, `test_flow.cpp` uses `flow.hpp`, and `test_edge_cases.cpp` now includes only the graph/traversal/shortest-path/components headers it actually exercises.
- Left `test_core.cpp` and `test_remove_node.cpp` on the umbrella include intentionally because those tests span multiple semantic areas and would become less readable if forced onto a larger manual include list.
- Re-verified the test include refactor by rebuilding the touched test binaries directly and rerunning `bash scripts/run_tests.sh` (`31/31` passing).

## 2026-03-29
- Added a dedicated GitHub Actions workflow at `.github/workflows/single-header.yml` to build `dist/nxpp.hpp`, smoke-test the generated header by compiling and running a tiny C++ program against it, upload the generated file as an artifact, and publish a short Markdown job summary.
- Clarified the README wording so the formal-test workflow and the standalone-header workflow are described separately instead of sharing one ambiguous paragraph.
- Re-verified the standalone-header path locally by running `bash scripts/build_single_header.sh` and compiling a smoke test against `/workspaces/nxpp/dist/nxpp.hpp`, which printed `2`.

## 2026-03-29
- Extended `.github/workflows/single-header.yml` to also run on `release.published`, so the CI-built `dist/nxpp.hpp` that passes the smoke test can be uploaded directly into the GitHub release assets as `nxpp.hpp`.
- Kept the workflow artifact upload in place for push and pull-request runs, and updated the README plus `CHANGELOG.md` (`0.7.4`) to explain that releases now receive the tested header built in CI rather than relying on a local upload.

## 2026-03-29
- Added `scripts/run_single_header_tests.sh` and made the test sources configurable via `NXPP_HEADER_UNDER_TEST`, so the same formal suite can be recompiled directly against `dist/nxpp.hpp` without hardcoding any workspace-local absolute path.
- Tightened the release path in `.github/workflows/single-header.yml` so published releases now build `dist/nxpp.hpp`, run the dedicated single-header suite against that generated file, and only then upload `nxpp.hpp` into the release assets.
- Re-verified both paths locally by running `bash scripts/run_tests.sh` (`31/31` passing), then `bash scripts/build_single_header.sh && bash scripts/run_single_header_tests.sh` (`31/31` passing against `dist/nxpp.hpp`).

## 2026-03-29
- Added `scripts/extract_release_notes.py` so the GitHub release process can read the matching version section directly from `CHANGELOG.md` instead of relying on a separate handwritten notes file.
- Added `.github/workflows/release.yml` to automate release creation from pushed `v*` tags: it extracts notes from the changelog, builds `dist/nxpp.hpp`, runs `bash scripts/run_single_header_tests.sh`, and only then creates the GitHub release with `nxpp.hpp` attached.
- Simplified `.github/workflows/single-header.yml` back down to CI validation of the generated header plus artifact upload, leaving actual GitHub release publication to the dedicated release workflow.
- Updated `.github/workflows/tests.yml`, `.github/workflows/snippet-review.yml`, `.github/workflows/single-header.yml`, and `.github/workflows/release.yml` to use `actions/checkout@v6` and `actions/setup-python@v6`, removing the Node 20 runtime deprecation warning path.

## 2026-03-29
- Added a versioned `RELEASE_NOTES.md` file to hold richer GitHub release text separately from the terser `CHANGELOG.md` entries, and removed `RELEASE_NOTES.md` from `.gitignore` so it now lives in the repo as part of the release process.
- Switched `scripts/extract_release_notes.py` plus `.github/workflows/release.yml` over to `RELEASE_NOTES.md`, so automated releases now read their body from the richer release-note source rather than from the changelog.

## 2026-03-29
- Extended `.github/workflows/release.yml` with a `workflow_dispatch` path so the same workflow can now kick off a release without requiring a locally created git tag.
- Added a `--latest-version` mode to `scripts/extract_release_notes.py`, and used it in the workflow to compare the top versions in `RELEASE_NOTES.md` and `CHANGELOG.md` before creating and pushing the matching `vX.Y.Z` tag.
- Kept actual release creation on the tag-triggered path, so the dispatch run only creates the tag while the tag-push run still owns the tested-header build, standalone suite, and GitHub release publication.

## 2026-03-29
- Fixed `.github/workflows/release.yml` after observing that tags pushed from `workflow_dispatch` with `GITHUB_TOKEN` do not reliably trigger a second workflow run in this repo.
- The dispatch path now stays self-contained: after creating and pushing the matching `vX.Y.Z` tag, it continues in the same run to extract notes from `RELEASE_NOTES.md`, build `dist/nxpp.hpp`, run `bash scripts/run_single_header_tests.sh`, and create the GitHub release with the tested `nxpp.hpp` asset.

## 2026-03-29
- Updated `AGENTS.md` with the release-process conventions that emerged from this session: `CHANGELOG.md` stays compact, `RELEASE_NOTES.md` is the richer release-body source, normal pushes must not auto-publish releases, the release workflow is the source of truth for release automation, and the generated single-header asset must come from the tested `dist/nxpp.hpp` output.

## 2026-03-29
- Added `scripts/run_benchmark_report.py` as a benchmark orchestrator that reuses the existing serial and parallel compile benchmark scripts, backs up the previous contents of `benchmark/` into `backups/benchmark/<timestamp>/`, writes short timestamped CSV names, and generates a Markdown report with SVG plots.
- Documented the new benchmark entry point in `README.md` and started ignoring generated `benchmark/*.csv`, `benchmark/*.svg`, and `benchmark/*.md` outputs so repeated local benchmark runs do not keep polluting `git status`.
- Simplified the benchmark orchestrator so it now runs serial first and parallel second, produces exactly two CSV outputs, writes the report to `benchmark/BENCHMARK.md`, writes plots under `benchmark/imgs/`, and backs up the previous benchmark CSVs plus `benchmark/imgs/` into `backups/benchmark/<timestamp>/`.
