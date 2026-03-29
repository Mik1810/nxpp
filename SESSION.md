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
- Added `ISSUE_11_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #11.

## 2026-03-29
- Added `ISSUE_10_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #10.

## 2026-03-29
- Added `ISSUE_9_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #9.

## 2026-03-29
- Added `ISSUE_12_CLOSE.md` at the repository root with a ready-to-post Markdown closeout message for issue #12, including the note that the min-cost reference value was verified against the current wrapper/snippet behavior and is 22.
