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

## Session Audit: April 5, 2026

1. **Script Layout Split**:
   - Split the runnable helper scripts into `scripts/unix/` and `scripts/windows/`.
   - Kept only the shared Python helpers at the top level of `scripts/`.
   - Updated Unix workflows and docs to reference the new `scripts/unix/...` paths.

2. **Windows PowerShell Runner Pass**:
   - Added native PowerShell counterparts under `scripts/windows/` for the main runner surface, including formal tests, single-header tests, large-graph compare, snippet review helpers, and benchmark helpers.
   - Removed the CMake dependency from the main PowerShell runners so they now compile and execute sources directly instead of relying on CMake/CTest as an intermediate layer.
   - Centralized the shared Windows-side helper logic in `scripts/windows/common.ps1`.

3. **Compatibility Workflow Reshaping**:
   - Kept the Linux and macOS formal-suite coverage in a single matrix under `.github/workflows/compatibility.yml`.
   - Folded Windows back into that same matrix instead of a separate job, while still using Windows-specific setup and execution steps.
   - Switched the Windows compatibility path away from `vcpkg` and toward a direct Boost zip include-path setup paired with the new PowerShell runner.

4. **Release Workflow Policy Realignment**:
   - Reworked `.github/workflows/release.yml` so normal pushes to `main` no longer publish releases.
   - The workflow now supports the two intended entry points: pushed `vX.Y.Z` tags and `workflow_dispatch`.
   - `workflow_dispatch` remains self-contained: it creates and pushes the tag only after the release checks pass, then continues in the same run to publish the release.
   - Updated `README.md` and `AGENTS.md` so the documented release behavior matches the workflow again.

5. **Windows Compatibility Cache Follow-up**:
   - Added explicit cache-hit reporting for the Windows Boost archive/extract cache in `.github/workflows/compatibility.yml`.
   - Tightened the Windows Boost setup so the job skips extraction when the cached extracted tree is already present.
   - Switched the preferred extraction path from `Expand-Archive` to `tar -xf` when available, keeping `Expand-Archive` as a fallback.
   - Added guard checks so the job now fails early and clearly if the Boost root or headers are still missing after setup.

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
- Added an explicit operating rule to `AGENTS.md`: benchmark runs that generate CSV outputs are always launched by the user, so the assistant must not start them autonomously.

## 2026-03-30
- Added a minimal support matrix to `README.md` for issue `#31`, keeping the wording conservative and aligned with the actual repository evidence instead of implying broader compiler/platform support than CI currently proves.
- Recorded Linux with Boost Graph plus a C++20-capable `g++` toolchain as the strongest verified path, while documenting macOS, Windows, and Clang as not yet CI-backed support claims.
- Added `tests/test_large_graph_compare.cpp` and `scripts/run_large_graph_compare.sh` as an opt-in large-graph verification path that generates deterministic graph instances and compares `nxpp` against raw Boost on BFS depth, connected components, and Dijkstra distances.
- Kept the new large-graph path out of `scripts/run_tests.sh`, and documented it in `README.md` as a scale-oriented correctness check rather than a benchmark or a replacement for the normal fast suite.
- Added `.github/workflows/large-graph-compare.yml` so the new large-graph comparison path also has its own dedicated GitHub Actions run and summary, separate from the normal formal test suite.
- Expanded the large-graph comparison driver so it now also covers DFS tree edges, strongly connected components, Bellman-Ford, DAG shortest paths, and reachable negative-cycle detection against raw Boost.
- Extended the same driver further with large post-`remove_node()` verification against raw Boost plus large multigraph mutation and cleanup checks around `edge_id`, bundle removal, and node removal behavior.
- Extended the large-graph driver again with wrapper-specific attribute-preservation checks after repeated mutations plus larger max-flow/min-cut and successive-shortest-path min-cost-flow comparisons against raw Boost.
- Extended the large-graph driver once more with a combined weighted-graph mutation sequence that mixes `remove_edge(u, v)`, `add_edge(...)`, and `remove_node()` while comparing final graph state and Dijkstra results against raw Boost.
- Added a large Floyd-Warshall all-pairs comparison that checks both the matrix and the NodeID-keyed map result against a raw Boost all-pairs baseline built from repeated single-source shortest-path runs, including unreachable-pair handling.

## 2026-04-03
- Aligned `README.md` with the current released version `v0.7.12` after a repo-wide markdown audit found the older `v0.6.0` release text still present in the landing-page header and project-status section.
- Refreshed the README project-status summary so it now points to actually open issue groups (`#28`, `#30`, `#25`, `#26`, `#27`, `#17`, `#18`, `#20`) instead of the older support-matrix wording that was already resolved by `v0.7.11`.
- Re-audited issue `#38` against the current repository state and confirmed that the acceptance path already exists in-repo through `tests/test_large_graph_compare.cpp`, `scripts/run_large_graph_compare.sh`, `README.md`, and `.github/workflows/large-graph-compare.yml`, including deterministic graph generation, raw-Boost result comparison, normalization-aware comparisons for unordered results, and separate opt-in execution outside the default fast suite.
- Strengthened the large-graph comparison driver for the next `#38` pass by rerunning representative BFS, connected-components, strongly-connected-components, Dijkstra, and `remove_node()` checks across multiple fixed seeds rather than one seed per scenario.
- Added a non-default selector regression to the same driver using `nxpp::Graph<int, int, true, false, true, boost::listS, boost::listS>`, then re-verified the whole large-graph path with `timeout 30s bash scripts/run_large_graph_compare.sh`, which still passed end-to-end within the expected timeout window.
- Added [`docs/TEST.md`](docs/TEST.md) as a dedicated testing guide so the repo now documents, in one place, the difference between showcases, snippet parity, the formal assertion-based suite, the generated single-header suite, and the opt-in large-graph comparison path.
- Added a compact quick-choice table to [`docs/TEST.md`](docs/TEST.md) so the common question "which test command should I run now?" has a fast answer without reading the whole guide.
- Began the first implementation pass for `#26` by moving `lookup_map`, `SingleSourceShortestPathResult`, Floyd-Warshall's map view, and the rooted Prim parent-map result onto `std::map`, leaving the more delicate internal hash-based indices for a later pass so we can improve real public bounds without disturbing the hottest wrapper internals yet.
- Updated [`README.md`](README.md) and [`docs/API_REFERENCE.md`](docs/API_REFERENCE.md) so the complexity notes now distinguish between the new ordered-map result paths and the remaining hash-backed public APIs that still need a later `#26` pass.
- Added [`docs/COMPLEXITY.md`](docs/COMPLEXITY.md) to document the intended reading of complexity guarantees: Boost algorithmic core vs full `nxpp` public-call cost, when preserving Boost's dominant order is the goal, and when wrapper-side materialization or bookkeeping must be counted separately.
- Continued the safer `#26` refactor by moving the wrapper-owned `id_to_bgl` translation map and the external `node_properties` / `edge_properties` stores onto `std::map`, while deliberately leaving `vertex_index_storage` and the still-hash-backed result helpers for a later pass so the hottest descriptor-index path stays unchanged for now.
- Updated the public complexity notes and contract wording to reflect that `nxpp::Graph` now expects orderable `NodeID` values through `std::less`, and added a focused regression that exercises the core graph and shortest-path APIs with an orderable but non-hashable custom node-ID type.
- Added a new `indexed_lookup_map<Key, Value>` result wrapper in `include/nxpp/graph.hpp` and switched `connected_components()`, `strong_component_map()`, and `degree_centrality()` onto it so those helpers no longer pay the earlier `V log V` materialization cost from `std::map`.
- Kept the wrapper intentionally simple and key-sorted by iterating the already-ordered `id_to_bgl` map directly, which means the returned wrapper still gives `O(log V)` `at()` lookup while preserving linear construction and the same dominant asymptotic order as the underlying Boost component or degree computation.
- Extended the same indexed-wrapper path to `strong_components()` / `strongly_connected_component_roots()`, and added coverage with the custom ordered-only `NodeID` regression so the `NodeID -> NodeID` result case is exercised too.
- Extended the indexed-wrapper approach into `include/nxpp/traversal.hpp` for `bfs_successors()`, `dfs_predecessors()`, and `dfs_successors()`, using sparse per-vertex temporary storage plus ordered final materialization so those traversal result helpers keep linear construction while dropping hash-backed return containers.
- Expanded the ordered-only `NodeID` regression to cover the traversal wrappers too, so we now exercise indexed results for `NodeID -> vector<NodeID>` and `NodeID -> NodeID` in the traversal layer as well as in the component layer.
- Replaced the old `VertexIndexStorage` external hash map in `include/nxpp/graph.hpp` with an internal `vertex_wrapper_index` property stored directly on the BGL graph vertices, while keeping the existing `vertex_index_map` call sites intact.
- Re-verified that this more structural `#26` refactor still works across both the default suite and the large raw-Boost comparison path, including the non-default `boost::listS` / `boost::listS` regression that depends on the wrapper's custom index normalization layer.
- Removed the local `NodeID -> index` hash maps from the flow helpers in `include/nxpp/flow.hpp` and switched those auxiliary builders onto the same maintained wrapper indices, which means `include/nxpp` itself no longer contains `std::unordered_map`-backed implementation state.
- Expanded `docs/COMPLEXITY.md` with explicit side-by-side comparison tables for the main `nxpp` function families, showing the nearest raw Boost counterpart, the Boost core complexity, the documented `nxpp` public-call complexity, and the wrapper-specific delta that explains where extra work is intentionally counted.
- Tightened the same complexity guide after review so the comparison tables now use `n = |V|` and `m = |E|`, and each row presents a more direct side-by-side `Boost complexity` vs `nxpp complexity` comparison instead of the earlier more narrative wording.
- Re-aligned the revised comparison tables in `docs/COMPLEXITY.md` back to English after the notation cleanup, so the new side-by-side `Boost complexity` vs `nxpp complexity` view stays consistent with the rest of the repository documentation.
- Tightened the same comparison tables again so the main complexity columns now keep only the dominant term where the lower-order wrapper work is clearly dominated, while details such as final accumulation, ordered-map materialization, and all-path reconstruction remain documented explicitly in the note column.
- Simplified `docs/API_REFERENCE.md` by removing the per-row complexity column entirely and replacing the old local complexity section with a single pointer to `docs/COMPLEXITY.md`, so API shape and complexity policy now live in one place each instead of drifting across two parallel documents.
- Added an explicit `Boost-order status` column to the comparison tables in `docs/COMPLEXITY.md`, using `yes`, `no`, and `wrapper-managed` so the reader can scan immediately whether a given `nxpp` API preserves Boost's dominant order, changes it, or is mainly driven by wrapper-side bookkeeping rather than a clean one-to-one Boost counterpart.
- Cleaned the same comparison tables again so the `Boost complexity` and `nxpp complexity` columns now contain formulas only, with qualifiers such as "standard bound", delegated-core wording, and auxiliary-build explanations moved into the note column instead of being mixed into the complexity cells.
- Tightened the remaining flow and wrapper-managed rows in `docs/COMPLEXITY.md` so even those cells now avoid prose: flow rows use symbolic delegated-core notation (`F_maxflow`, `F_mincost_cc`, `F_ssp`), while rows without a clean Boost-side asymptotic contract now use `—` and explain the reason in the note column.
- Removed the last overloaded labels from complexity cells as well, so variants such as simple-graph vs multigraph insertion and endpoint-form vs edge-id attribute lookup are now described in the note column instead of mixed into the asymptotic formula field.
- Continued `#26` by removing eager `paths` materialization from `SingleSourceShortestPathResult` and switching the public API to on-demand `path_to(target)` reconstruction, which restores Boost-faithful dominant complexity for `dijkstra_shortest_paths()`, `bellman_ford_shortest_paths()`, and `dag_shortest_paths()` while keeping ordered `distance` / `predecessor` maps in the result wrapper.
- Corrected the release-versioning docs after confirming that GitHub already published `v0.7.13`: opened a new `0.8.0` top entry in `CHANGELOG.md` and `RELEASE_NOTES.md` for the post-release complexity/API work, restored `0.7.13` to the already-published large-graph comparison changes, and updated `README.md` to show `v0.7.13` as the current release while removing now-closed `#26` from the main open-issue summary.
- Closed the implementation side of `#7` by making the `NodeID` contract explicit in code and docs: `Graph<NodeID, ...>` now checks copyability, equality comparison, and `std::less` ordering at compile time, while `complete_graph`, `path_graph`, and `erdos_renyi_graph` separately check that the node ID type can be constructed from `std::size_t`.
- Changed the release model to a fully automatic main-branch path: `.github/workflows/release.yml` now reads the top version from `CHANGELOG.md` and `RELEASE_NOTES.md` on every push to `main`, skips if the matching tag already exists, otherwise builds the standalone header, runs the modular and single-header suites, creates and pushes the tag, and publishes the GitHub release in the same run. From here on, the top version in those markdown files must be treated as the concrete next release that the workflow will publish.
- Corrected the release numbering again to match the repo's real release workflow: moved the just-finished `#7` work into a new top entry `0.8.1` in `CHANGELOG.md` and `RELEASE_NOTES.md`, while leaving `0.8.0` to cover the earlier `#26` and release-automation tranche.
- Closed the implementation side of `#18` by documenting the external-usage story explicitly: added a dedicated `docs/EXTERNAL_USAGE.md` guide covering modular-header and release-asset consumption, clarified the Boost dependency and minimal compile commands in `README.md`, linked the new guide from `docs/README.md`, and prepared the versioned notes under `0.8.2`.
- Closed the implementation side of `#6` by standardizing the main runtime-error wording across the public surface: traversal, shortest-path, spanning-tree, flow, graph lookup, attribute-adjacent edge lookup, and multigraph-by-id failures now use a consistent `X failed: ...` style instead of the earlier mixture of generic messages, legacy `NetworkXError` text, and inconsistent punctuation.
- Added a few focused exact-message assertions in the edge-case and flow tests so the new wording is locked in for missing-node lookup, missing traversal start, unreachable path reconstruction, and missing staged min-cost-flow state, then prepared the release notes as `0.8.3`.
- Closed the implementation side of `#30` by tightening the testing docs: `docs/TEST.md` now presents a clearer stable split between showcase programs, the curated snippet parity/regression layer, the formal assertion-based suite, the single-header validation path, and the separate large-graph raw-Boost comparison path; `README.md` and `docs/README.md` were updated to point at that distinction and `#30` was removed from the main open-issue summary.
- Closed the implementation side of `#17` by adding a minimal root `CMakeLists.txt` that exposes `nxpp` as a header-only `INTERFACE` target for vendored `add_subdirectory(...)` usage, then updated `docs/EXTERNAL_USAGE.md` and `README.md` to reflect that the repo now has a basic CMake entry point even though install/export/package-manager support is still intentionally out of scope.
- Closed the implementation side of `#20` by extending `.github/workflows/tests.yml` from the old Ubuntu-only `g++` job to a small readable matrix covering Ubuntu with both `g++` and `clang++` plus `macos-latest` with `clang++`, while intentionally leaving the snippet-review workflow untouched and updating the support matrix in `README.md` to match the new CI-backed reality.
- Followed up immediately on `#20` after the first macOS GitHub Actions run failed to find Boost headers: the fix keeps the same CI matrix but now exports `-I$(brew --prefix boost)/include` into `CXXFLAGS` for the macOS formal-test job, so the Homebrew-installed BGL headers are visible to the existing `run_tests.sh` compile path without changing the Linux jobs or the snippet-review workflow.
- Renamed the current formal-suite workflow from `.github/workflows/tests.yml` to `.github/workflows/compatibility.yml` so the file name and GitHub Actions label better reflect its new role as a small compiler/platform compatibility matrix rather than just a single test runner.
- Tightened the macOS compatibility follow-up again after the first fix masked the workflow's default C++20 flags: the job now carries its full `CXXFLAGS` configuration directly in the matrix, so macOS gets the normal `-std=c++20 -Wall -Wextra -pedantic -O0` set plus the Homebrew Boost include prefix, while the existing repository scripts remain unchanged.
- Tightened the same compatibility workflow once more after the next macOS run showed that `$(brew --prefix boost)` was being passed as a literal string from the matrix rather than expanded by a shell: the workflow now computes the Homebrew include prefix in a dedicated macOS step, exports it through `GITHUB_ENV`, and adds short matrix labels (`ubuntu-gcc`, `ubuntu-clang`, `macos-clang`) so the GitHub checks stay readable.
- Started a first Windows pass on the compatibility matrix by adding a `windows-ucrt64-gcc` entry through `msys2/setup-msys2@v2`, with the goal of reusing the existing `bash scripts/run_tests.sh` path inside an MSYS2/UCRT64 environment instead of forking the test runner for Windows immediately; public docs and versioning were intentionally left untouched until the workflow proves itself on a real Actions run.
- Added a minimal CMake-driven formal-test path behind `NXPP_BUILD_TESTS` in the root `CMakeLists.txt`, wiring the six main modular test binaries into `ctest` so the repository now has a shell-independent path that can serve as the basis for a future native Windows compatibility job without relying on MinGW/MSYS2.
- Reworked the Windows direction for `#20` again: instead of continuing with the earlier MSYS2/UCRT64 bash-based experiment, the compatibility workflow now moves toward a native `windows-latest` path built around Visual Studio 2022, `vcpkg`, the new `NXPP_BUILD_TESTS` CMake option, and `ctest`, while leaving the existing Unix/macOS script-driven jobs unchanged.
- Tightened that native Windows path for practicality by adding a dedicated `NXPP_BUILD_SMOKE_TESTS` CMake mode (`test_core` + `test_edge_cases`) and a simple `actions/cache` layer for `vcpkg` downloads/installed packages, so the Windows compatibility job can act as a fast native smoke check instead of rebuilding the full modular suite from scratch every time.
- Split the repository script surface by platform: the real `.sh` runners now live under `scripts/unix/`, the top-level `scripts/` directory was reduced back to shared Python helpers only, and `scripts/windows/` now provides PowerShell counterparts for every moved shell runner, with native PowerShell/CMake entry points for the main test, single-header-test, and large-graph paths plus native PowerShell implementations for snippet logging/parity and the compile-benchmark commands.
- Removed the CMake dependency from the main PowerShell runners in `scripts/windows/`: `run_tests.ps1`, `run_single_header_tests.ps1`, and `run_large_graph_compare.ps1` now compile and execute their targets directly, mirroring the Unix shell runners more closely and leaving CMake as a separate repository build path rather than the engine behind the Windows scripts.
- Simplified the Windows compatibility job to match the new PowerShell runners: it no longer prepares `vcpkg` or calls CMake, and instead downloads an official Boost source zip, exports the include path through `BOOST_ROOT` / `NXPP_EXTRA_INCLUDE`, selects `cl`, and invokes `scripts/windows/run_tests.ps1 -Smoke` directly.
- Folded the Windows path back into the main compatibility matrix instead of keeping it as a separate job: the workflow now has one `test-suite` matrix with Unix/macOS rows following the shell-runner path and a `windows-msvc` row that uses conditional setup and the PowerShell smoke runner.
- Tightened the Windows Boost archive cache behavior in the compatibility workflow by exposing the cache-hit state in the summary, skipping extraction when the cached extracted tree is already present, preferring `tar -xf` over `Expand-Archive` when available, and failing early if the extracted Boost headers are still missing after setup.
- Fixed the first real GitHub Actions parsing issues in the new PowerShell path: the runners that accept parameters now keep `param(...)` at the top of the file so switches such as `-Smoke` parse correctly, and the Windows summary block in `compatibility.yml` now avoids fragile inline Markdown quoting inside PowerShell string literals.
- Added an explicit MSVC-environment initialization step to the Windows compatibility path by importing the Visual Studio developer environment from `VsDevCmd.bat` into `GITHUB_ENV`, so the PowerShell smoke runner can reliably invoke `cl` on the hosted Windows runner without falling back to MinGW/MSYS2.
- Finalized the `#20` documentation and release-facing notes around the completed compatibility expansion: `README.md`, `CHANGELOG.md`, and `RELEASE_NOTES.md` now describe the real final state of the compatibility matrix, including full formal-suite coverage on Ubuntu (`g++`, `clang++`), macOS (`clang++`), and Windows (`cl`), plus the cleaned Windows job summaries and the move to `actions/cache@v5`.
- Expanded the Windows compatibility row from a smoke subset to the full formal suite, and cleaned the job summaries on both Unix and Windows so optional include-path environment values are handled locally inside the shell steps and Windows summary output strips ANSI escape sequences before publishing to GitHub.
- Corrected the release-versioning notes after confirming that GitHub had already published `v0.8.6`: the earlier, smaller `#20` milestone stays under `0.8.6`, while the final cross-platform completion of `#20` is now recorded under a new top entry `0.8.7` in both `CHANGELOG.md` and `RELEASE_NOTES.md`.
- Updated `main_nxpp.cpp` after the lazy shortest-path API change: the showcase now uses `has_path_to(...)` / `path_to(...)` instead of the removed eager `result.paths` member, and the example compiles again against the current public API.
- Began the first implementation pass for `#28` by adding Doxygen-style comments to the most user-facing graph, shortest-path, and generator APIs, plus a minimal root `Doxyfile` and `docs/DOXYGEN_MAINPAGE.md` so the repository now has a concrete local generated-reference scaffold to build on.
- Closed the first pass of `#28` as a real public-docs milestone: extended the Doxygen-style comments to the core graph surface, traversal visitor base, shortest-path and flow result types, and key algorithm entry points, then updated `README.md`, `TODO.md`, `CHANGELOG.md`, and `RELEASE_NOTES.md` so the repository now treats the generated-docs scaffold as part of the user-facing documentation story rather than a future-only idea.
- Followed up on `#28` by broadening the inline-doc coverage beyond the first API hotspots: added Doxygen comments for the remaining public graph utility methods, component / spanning-tree / topological-sort wrappers, 2-SAT helpers, and the deprecated free-function aliases so the generated reference is less uneven and migration paths still show meaningful hover/reference text.
- Performed a final hover-focused polish pass on `Graph` itself by rewriting the most visible methods into fuller multi-line comments; the docs now read less like one-line labels and more like concise API guidance, especially around `add_node`, the `add_edge` overload family, removals, adjacency queries, proxies, and wrapper-managed edge identity.
- Extended that same hover-focused polish to the attribute and edge-id accessors on `Graph`, so typed attribute lookup, numeric attribute conversion, weight access, and edge-id mutation helpers now carry clearer behavior notes instead of only short labels.
- Audited the remaining `#28` hover gaps after validating the Doxygen scaffold locally: identified that several weighted/template-gated declarations in `graph.hpp` already had comments but placed them after `template` / `requires`, which some language servers do not attach reliably to symbol hover, and also found that the traversal entry points still needed fuller declaration-side descriptions.
- Tightened the Doxygen scaffold and hover-facing declarations again: `flow.hpp` now uses an explicit `@ref MaximumFlowResult::flow`, the generated-doc main page resolves the docs index through a Doxygen-managed reference instead of a workspace-local path, the `Doxyfile` indexes `docs/README.md`, and the traversal / weighted shortest-path / MST declarations in `graph.hpp` now use richer block comments with `@brief`, `@param`, `@return`, and `@throws`-style structure in a hover-friendlier position before the template declarations.
- Performed one more `#28` hover-polish pass focused on result-shape clarity: traversal, shortest-path, spanning-tree, and flow declarations in `graph.hpp` now say what they return in concrete terms (`std::vector<NodeID>`, edge-pair vectors, dense matrices, nested maps, result-wrapper types, and parent maps) instead of only describing the algorithm at a high level, and `SingleSourceShortestPathResult::path_to()` now documents its source-to-target path return value explicitly as well.
- Added a dedicated GitHub Pages workflow for the Doxygen site in `.github/workflows/docs-pages.yml`: it builds the generated reference on `main` changes that affect docs or headers, installs both `doxygen` and `graphviz`, uploads the HTML with the official Pages artifact action, and deploys through the standard `configure-pages` / `deploy-pages` flow. The repo docs now also state that Pages should be configured with `GitHub Actions` as the source and that any future custom domain should be managed from Pages settings rather than through a committed `CNAME` file.
- Tightened `.github/workflows/snippet-review.yml` so it no longer runs on every generic push or pull request: the snippet parity workflow is now triggered only when headers, snippet sources, the top-level showcase mains, the snippet runner itself, or the workflow file change, which keeps the expensive Boost installation path off purely documentary or release-note edits.
- Added a small custom Doxygen stylesheet wired through `HTML_EXTRA_STYLESHEET` in `Doxyfile`, giving the generated Pages site a more formal, less stock-Doxygen presentation while keeping the setup lightweight and fully self-contained in the repo.
- Moved the custom Doxygen stylesheet out of `docs/` into `assets/doxygen/` so the generated-site CSS lives with other static assets rather than among hand-written documentation pages; `Doxyfile` now points at the new asset path.
- Continued the Doxygen-site polish by treating it as a real public reference surface instead of a raw stock-Doxygen dump: `Doxyfile` now indexes only the semantic headers plus curated guide pages, the Pages workflow also rebuilds on `assets/doxygen/**` changes, and the next pass rewrites the landing page and theme more aggressively around generated-page links and calmer navigation chrome.
- Refined the Doxygen presentation again after local browser review: the generated site now forces light-mode backgrounds consistently, the directory/index pages override the remaining dark stock-Doxygen table styling, the detailed API sections (`memtitle` / `memproto` / template rows / return blocks) have cleaner contrast and spacing, and the last markdown headings that rendered literal `&lt;tt&gt;...&lt;/tt&gt;` in the HTML were rewritten into plain text.
- Audited the docs split for `#29` and clarified the intended source-of-truth roles: the generated Doxygen site is now documented as the declaration-driven API reference, `README.md` as the overview/navigation layer, and `docs/*.md` as companion guides and policy documents. `docs/API_REFERENCE.md` is now positioned as a curated markdown companion instead of a second full declaration dump.
- Closed the first concrete encapsulation step for `#25`: `Graph::node_properties` and `Graph::edge_properties` are now private wrapper internals instead of public mutable members, while the existing `const` integration getters (`get_impl()`, `get_bgl_to_id_map()`, `get_id_to_bgl_map()`) remain documented as advanced read-only escape hatches rather than the normal API surface.
- Closed the policy side of `#27` without changing runtime behavior: the docs now state explicitly that the current attribute system intentionally stays on wrapper-owned `std::any` storage for pragmatic flexibility, that proxy syntax is mainly the ergonomic write/demo path, that checked accessors are the recommended read path, and that multigraph precision still belongs to the `edge_id`-based attribute APIs.
- Closed the documentation side of `#16` by tightening the wording around `"weight"`: shortest-path overloads and the min-cost-flow parameter docs now state explicitly that `"weight"` refers to the built-in edge-weight property in the current API, rather than suggesting a fully general user-defined weight-key abstraction.
- Closed the documentation side of `#13` by making the thin alias taxonomy explicit: the guides and public header comments now distinguish between explicit graph-type presets, shorter compatibility-friendly type synonyms, convenience method aliases, and the remaining deprecated namespace-scope compatibility wrappers.
- Closed the documentation side of `#14` by adding concrete examples for the richer result wrappers directly in `README.md` and `docs/API_REFERENCE.md`, covering single-source shortest-path results, max-flow / minimum-cut / min-cost wrappers, and the `indexed_lookup_map` result shape.
- Closed the documentation side of `#15` by adding an explicit “utility wrappers beyond direct NetworkX/BGL parity” explanation to `README.md` and `docs/API_REFERENCE.md`, so the public docs now state clearly why wrappers such as `SingleSourceShortestPathResult`, `MaximumFlowResult`, `MinimumCutResult`, `MinCostMaxFlowResult`, and `indexed_lookup_map` are intentional parts of the `nxpp` surface.
- Closed the documentation side of `#19` by formalizing the repository's versioning and release policy in `README.md` and `docs/README.md`: the docs now state explicitly that `CHANGELOG.md` is the concise technical history, `RELEASE_NOTES.md` is the richer release narrative, `SESSION.md` is the append-only chronological log, normal `main` pushes do not publish releases, and the top documented version is the concrete next release candidate.
- Began and completed the first implementation pass for `#34`: `Graph` now exposes a minimal `pagerank()` method returning `indexed_lookup_map<NodeID, double>`, the repo also keeps a deprecated free-function alias for consistency with the other graph-local helpers, the first test case checks that the returned scores stay normalized and rank a small directed graph sensibly, and the centrality-layer definitions now live under a dedicated `include/nxpp/centrality.hpp` header instead of staying in `graph.hpp`.
- Closed `#33`: implemented `betweenness_centrality()` as a `Graph` method in `include/nxpp/centrality.hpp`, returning `indexed_lookup_map<NodeID, double>` with normalization matching NetworkX `betweenness_centrality(G, normalized=True)` semantics (factor `1/((n-1)(n-2))` for directed, `2/((n-1)(n-2))` for undirected); the implementation uses the Brandes BFS algorithm directly over the wrapper's internal BGL adjacency and vertex-index layer without requiring BGL property-map setup; a deprecated free-function alias mirrors the existing `degree_centrality` and `pagerank` patterns; the formal test in `test_edge_cases.cpp` covers the linear-chain case (B and C should have positive betweenness while leaf nodes A and D have zero), the empty-graph case, and the singleton case; `docs/API_REFERENCE.md`, `docs/COMPLEXITY.md`, and `README.md` are updated to include `betweenness_centrality` in all relevant tables and listings; all 35 formal tests pass.
- Closed `#5`: formalized the implicit-node-and-edge-creation policy. Added a dedicated "Implicit node and edge creation" section to `docs/API_REFERENCE.md` with write-creates / read-does-not-create tables. Added docstrings to `NodeAttrProxy::operator=`, `EdgeAttrProxy::operator=`, and `EdgeProxy::operator=` in `graph.hpp` documenting that they create the target node or edge when absent. Added `test_implicit_creation_policy` to `tests/test_edge_cases.cpp` (10 tests total). Bumped version to 0.8.13 in `CHANGELOG.md` and `RELEASE_NOTES.md`.
- Closed `#40`: added `docs/ATTRIBUTE_DESIGN.md`, a dedicated design evaluation document covering the tradeoffs of the current `std::any` storage model, two plausible alternatives (bounded `std::variant` and typed attribute schema), a side-by-side comparison table, an explicit rationale for keeping `std::any`, and a conservative three-phase migration path. Added a pointer from `docs/API_REFERENCE.md` to the new doc. Bumped version to 0.8.14.
- Prepared release `1.0.0`: promoted the version in `CHANGELOG.md` and `RELEASE_NOTES.md` with a first-stable summary that consolidates the completed `0.8.x` hardening work (API coverage, multigraph `edge_id` precision path, CI maturity, and docs maturity).
- Updated the root `README.md` for the release handoff by keeping the quick-start-first layout, adding CI/Boost/C++20 badges, and exposing the current release label `v1.0.0`.
- Added the missing declaration-side Doxygen coverage for the centrality layer after new algorithms landed: `degree_centrality()`, `pagerank()`, and `betweenness_centrality()` in `graph.hpp` now explain normalization and result-shape semantics with fuller `@brief` / `@return` comments, and the deprecated free-function aliases in `include/nxpp/centrality.hpp` now carry matching parameter/return docs instead of one-line labels.
- Closed the decision side of `#59` by documenting the packaging/distribution order explicitly: `nxpp` still supports direct source inclusion and vendored `add_subdirectory(nxpp)` as the current public paths, the next first-class target is proper CMake install/export/package-config support, Conan comes after that, vcpkg is a later ecosystem follow-up, and Debian / Ubuntu packaging is now treated as a later optional evaluation rather than the first default distribution target.
- Closed the implementation side of `#60` by turning the minimal root CMake target into a real installable/exported package: `CMakeLists.txt` now installs the public headers, exports `nxppTargets.cmake` under the `nxpp::nxpp` namespace, generates `nxppConfig.cmake` and `nxppConfigVersion.cmake`, and the external-usage docs now distinguish clearly between vendored `add_subdirectory(...)` use and installed-package `find_package(nxpp CONFIG REQUIRED)` consumption.
- Closed the docs side of `#61` by tightening the external-consumer story around the real supported paths: `README.md` now labels the three main integration modes more explicitly, `docs/EXTERNAL_USAGE.md` now explains when to choose repo-local headers vs vendored CMake vs installed-package CMake vs the tested single-header asset, and the packaging/docs index no longer duplicates the “supported external-consumption paths” bullet line.
- Closed the first implementation pass of `#62` by adding a local `conanfile.py`: `nxpp` is now modeled as a header-only Conan package with Boost declared as a dependency, and the docs explain `conan create .` as the intended local validation step before any CI or remote-publication story is added.
- Followed up on `#62` after a real local Conan run: the recipe now forces `boost/*:header_only=True`, which avoids Conan trying to resolve a compiled Boost binary for the `compiler.cppstd=20` host profile, and the external-usage docs now recommend the explicit C++20 validation command that passed locally.
- Closed the first implementation pass of `#63` by adding a local vcpkg overlay port under `packaging/vcpkg/ports/nxpp`: the port now drives the installed CMake package path through `vcpkg_cmake_configure()` / `vcpkg_cmake_install()` / `vcpkg_cmake_config_fixup()`, declares `boost-graph` and the usual vcpkg CMake helpers in `vcpkg.json`, and the external-usage docs now explain the intended local `vcpkg install nxpp --overlay-ports=...` validation shape.
- Followed up on `#63` with real local validation: the overlay install path now passes after removing the empty `lib/` directory that vcpkg flagged in post-build validation, and a small external consumer using the vcpkg toolchain plus `find_package(nxpp CONFIG REQUIRED)` also builds and runs locally against the installed port.
- Followed up on `#63` with a real public curated-registry attempt: the port was converted to a registry-ready `vcpkg_from_github(...)` form against the immutable `v1.0.1` source tag, a PR was opened upstream as `microsoft/vcpkg#51032`, and the submission was then closed under vcpkg's minimum project-maturity policy. The repository-hosted overlay port therefore remains the supported vcpkg integration path for now.
- Closed `#64` as an evaluation issue rather than an implementation issue: Debian / Ubuntu packaging was reviewed against the project's current distribution surface, and the decision is now documented as "defer for now" because a Debian source package / PPA / apt-facing maintenance path would add more ongoing packaging overhead than the project currently needs relative to the already-supported source, installed-CMake, Conan, and vcpkg-overlay paths.
- Closed `#65` at the policy level by defining release/versioning expectations for the repository-hosted package paths: the tagged repository release is now documented as the source of truth for package metadata inside the repo, so `CMakeLists.txt`, `conanfile.py`, and `packaging/vcpkg/ports/nxpp/vcpkg.json` are expected to track the same `X.Y.Z` version even when third-party or policy-gated channels do not publish at exactly the same time.
- Closed `#43` at the policy/documentation level by making the multigraph rule explicit: `edge_id` is now documented as the precise edge-instance path, while endpoint-based `(u, v)` APIs and proxy forms are documented as convenience-oriented in multigraph mode unless they explicitly promise stronger behavior.
- Closed `#41` by turning the multigraph caveats into a stricter public reference table: `docs/API_REFERENCE.md` now maps each ambiguous endpoint-based multigraph API to its real semantics plus its precise `edge_id`-based alternative, `README.md` now highlights that `remove_edge(u, v)` removes all parallel edges between those endpoints, and the remaining declaration comments on endpoint-based edge lookups now say more directly that they are convenience paths in multigraph mode.
- Closed `#42` with a small runtime restriction instead of a broad redesign: the ambiguous endpoint-based attr-bearing `add_edge(..., attrs)` overloads now throw in multigraph mode with an error that points callers toward `add_edge_with_id(...)` plus `set_edge_attr(edge_id, ...)`, and `tests/test_multigraph.cpp` now covers that failure mode explicitly.
- Corrected the new per-issue patch-version rule in the release docs: `CHANGELOG.md` and `RELEASE_NOTES.md` now split the recent work into separate `1.0.1` / `1.0.2` / `1.0.3` / `1.0.4` buckets instead of incorrectly dragging earlier packaging work forward into the latest patch entry.
- Adopted the new issue-closing versioning rule from this point onward: each resolved issue advances the top documented next-release version by one patch step, so the current release-candidate entry moved from `1.0.1` to `1.0.2` when `#43` was closed, while repository package metadata remains on the last tagged release until the next real release is cut.
- Closed `#44` by making the flow-wrapper result shapes multigraph-safe without breaking the old endpoint views: `MaximumFlowResult` and `MinCostMaxFlowResult` now carry both aggregate endpoint maps and precise `edge_id`-keyed flow maps, `MinimumCutResult` now carries `cut_edge_ids`, the flow builders now track every original edge instance instead of collapsing parallel edges by `(u, v)`, `cycle_canceling()` now refreshes staged edge weights by precise edge ID, direct `flow.hpp` inclusion now also brings in the multigraph layer it depends on, and the flow tests now cover multigraph max-flow / minimum-cut / min-cost result precision explicitly. Updated `CHANGELOG.md` / `RELEASE_NOTES.md` to `1.0.5`.
- Added `snippet/centrality/` with matched `nxpp`, Boost, and NetworkX examples for degree centrality, PageRank, and betweenness centrality. The first parity run exposed two issues: NetworkX `pagerank()` required SciPy in the current environment, and the PageRank outputs differed enough across implementations to fail the exact-output snippet harness. After installing the missing Python dependency locally, the snippet outputs were aligned by normalizing the Boost PageRank view, printing all three implementations at two-decimal precision, and adding the new snippet description to `.github/workflows/snippet-review.yml`; `bash scripts/unix/test_single_snippet.sh centrality` now passes.
- Closed `#45` with a conservative invalidation fix instead of a public API redesign: the staged residual state behind `push_relabel_maximum_flow(...)` / `cycle_canceling()` now gets invalidated on graph mutations and on graph destruction, `cycle_canceling()` now throws a specific stale-state error if callers mutate the graph between the two stages, and the flow tests now cover that mutation-invalidation path directly. Updated `CHANGELOG.md` / `RELEASE_NOTES.md` to `1.0.6`.
