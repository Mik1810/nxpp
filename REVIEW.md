# nxpp — Deep Repository Review

> **Reviewed:** 2026-04-29  
> **Version reviewed:** `v1.3.0`  
> **Repository:** https://github.com/Mik1810/nxpp

---

## Overview

`nxpp` is a header-only C++20 graph library that wraps Boost Graph Library with a NetworkX-inspired API. The codebase is well-structured and actively developed, with clear documentation and a growing feature set. This review covers the full source tree, all CI workflows, and all 13 open GitHub issue.

The findings are organized into seven sections. Each item includes the relevant file, approximate line number, a description of the problem, and a suggested fix.

---

## Table of Contents

1. [Bugs and Correctness Issues](#1-bugs-and-correctness-issues)
2. [API Design and Consistency Issues](#2-api-design-and-consistency-issues)
3. [Performance Concerns](#3-performance-concerns)
4. [Missing Test Coverage](#4-missing-test-coverage)
5. [CI and Workflow Gaps](#5-ci-and-workflow-gaps)
6. [Code Duplication and Maintainability](#6-code-duplication-and-maintainability)
7. [Other Quality Concerns](#7-other-quality-concerns)
8. [Open GitHub Issues — Status Overview](#8-open-github-issues--status-overview)
9. [Priority Summary](#9-priority-summary)

---

## 1. Bugs and Correctness Issues

### 1.1 `path_graph(0)` — unsigned underflow causes an infinite loop

**File:** `include/nxpp/generators.hpp`

```cpp
for (size_t i = 0; i < n - 1; ++i)  // when n == 0, n-1 wraps to SIZE_MAX
```

When `n == 0`, the subtraction wraps to `SIZE_MAX` (unsigned underflow), causing the loop to run approximately 18 quintillion times and hanging the process.

**Fix:** Add `if (n == 0) return G;` before the loop.

---

### 1.2 `collect_edge_ids_between` double-counts edges in undirected graphs

**File:** `include/nxpp/graph.hpp` ~line 448

For undirected graphs, the function scans `out_edges(u)` looking for target `v`, then also scans `out_edges(v)` looking for target `u`. Because BGL represents each undirected edge as two half-edges, both scans find the same logical edge, appending its ID twice. This causes `edge_ids(u, v)` to return duplicate IDs and `remove_edge(u, v)` to attempt double-deletion of attributes.

**Fix:** Deduplicate results with a `std::unordered_set<std::size_t>` inside `collect_edge_ids_between`, or remove the second scan for undirected graphs since `out_edges(u)` already sees both directions.

---

### 1.3 `node()` method has a dead branch

**File:** `include/nxpp/graph.hpp` ~line 1286

```cpp
if (!has_node(u) && !Multi) add_node(u);
else if (!has_node(u)) add_node(u);  // identical to the first branch
```

Both branches call `add_node(u)` when the node is absent. The `!Multi` condition in the first branch has no effect — the `else if` re-checks the same condition and does the same thing. This is a leftover from a refactoring.

**Fix:** Simplify to `if (!has_node(u)) add_node(u);`.

---

### 1.4 `complete_graph` generates redundant edge insertions for undirected graphs

**File:** `include/nxpp/generators.hpp` ~line 31

The double loop (`i` from `0..n`, `j` from `0..n`, `i != j`) calls `add_edge(i, j)` and `add_edge(j, i)` for every pair in an undirected graph. For `Simple=true` graphs, the second call is silently ignored (idempotent), so the result is correct but wastes O(n²) unnecessary `boost::edge` lookups. For `Multi=true` graphs, both calls succeed, incorrectly adding parallel edges.

**Fix:** Iterate `j` from `i+1` to `n` when the graph is undirected.

---

### 1.5 `erdos_renyi_graph` silently drops isolated nodes

**File:** `include/nxpp/generators.hpp` ~line 70

Nodes are only added when an edge is sampled. If all probability checks fail for a node (especially likely at low `p`), that node never appears in the graph. A request for `n=6, p=0.001` may return a graph with fewer than 6 nodes.

**Fix:** Pre-call `G.add_node(i)` for all `n` nodes before the edge-sampling loop.

---

### 1.6 `remove_edge(u, v)` on undirected multigraphs leaves stale BGL edges

**File:** `include/nxpp/graph.hpp` ~line 876

The implementation erases all parallel edge attributes via `collect_edge_ids_between`, then calls `boost::remove_edge(bu, bv, g)` — which on `vecS` storage removes only one BGL edge. The graph ends up with live BGL edges whose attribute entries were already deleted. Subsequent `has_edge_id` checks will fail for the deleted IDs while the underlying edge still exists in BGL.

**Fix:** Loop `boost::remove_edge` until `boost::edge(bu, bv, g)` returns false, or use `boost::remove_out_edge_if` to batch-remove all matching edges.

---

### 1.7 Path reconstruction loops have no cycle guard

**File:** `include/nxpp/shortest_paths.hpp` ~lines 349, 672

Both the BFS `shortest_path` and Bellman-Ford path-reconstruction use:

```cpp
for (VertexDesc curr = target; curr != source; curr = pred[get_vertex_index(curr)])
```

There is no cycle detection. An inconsistent predecessor map (e.g., from a disconnected graph, a self-loop, or floating-point edge cases in Bellman-Ford) causes an infinite loop. The `path_to()` method on `SingleSourceShortestPathResult` has a hop counter as a safety net, but these inline helpers do not.

**Fix:** Replace the hop counter with a `std::unordered_set<VertexDesc>` seen-set that breaks the loop if a vertex is revisited.

---

### 1.8 `to_2sat_vertex_id(0)` returns `-1` — undefined behavior

**File:** `include/nxpp/sat.hpp` ~line 19

```cpp
return (literal > 0) ? (literal - 1) : (-literal - 1);
// to_2sat_vertex_id(0) == -1
```

A literal of `0` produces `-1`, an invalid vertex index. `boost::add_edge` is then called with this value, causing undefined behavior. The internal loop guards against literal `0`, but the function is public and has no precondition check.

**Fix:** Add `if (literal == 0) throw std::invalid_argument("Literal 0 is not valid in 2-SAT");` at the top of the function.

---

### 1.9 `bfs_tree` / `dfs_tree` hardcode `double` as edge weight type

**File:** `include/nxpp/traversal.hpp` ~lines 371, 451

Both functions return `Graph<NodeID, double, Directed>` regardless of the source graph's `EdgeWeight` template parameter. For a graph instantiated with `EdgeWeight = int`, the returned tree is a different, incompatible type, causing silent type mismatches in generic code.

**Fix:** Use `Graph<NodeID, EdgeWeight, Directed>` as the return type, propagating the source graph's weight type.

---

### 1.10 `capacity` attributes silently truncated to `long`

**File:** `include/nxpp/flow.hpp` ~line 408

```cpp
capacity[e] = static_cast<long>(get_edge_numeric_attr(edge_id, capacity_attr));
```

`get_edge_numeric_attr` converts the stored value to `double`, which is then cast to `long`. A `long long` capacity loses precision silently; a fractional `double` is truncated without warning. This is not documented anywhere.

**Fix:** Document that capacity attributes must be integral and within `long` range, or change the internal representation to preserve the original type.

---

### 1.11 Min-cost flow cache is not thread-safe

**File:** `include/nxpp/flow.hpp` ~lines 153–165

Two function-local static `std::map`s serve as a shared cache across all graph instances. Concurrent calls to `push_relabel_maximum_flow` or `cycle_canceling` on different `Graph` objects from different threads will race on these maps with no synchronization.

**Fix:** Protect cache access with a `std::mutex`, switch to `thread_local` storage, or add a prominent comment requiring external synchronization. (Also see open issue #92 on thread-safety documentation.)

---

## 2. API Design and Consistency Issues

### 2.1 `edges()` return type changes based on the `Weighted` template parameter

**File:** `include/nxpp/graph.hpp` ~line 1114

- Weighted graph: `std::vector<std::tuple<NodeID, NodeID, EdgeWeight>>`
- Unweighted graph: `std::vector<std::pair<NodeID, NodeID>>`

Generic code that calls `g.edges()` on a templated graph type cannot be written uniformly without `if constexpr`. The separately named `edge_pairs()` is always consistent, but its co-existence alongside `edges()` adds confusion.

**Fix:** Either always return a uniform type (e.g., always a tuple, with weight defaulting to `1` or `0` for unweighted graphs), or rename the weighted variant to `weighted_edges()` and have `edges()` always return pairs.

---

### 2.2 `operator[]` silently creates nodes on read access

**File:** `include/nxpp/graph.hpp` ~line 1274

`G["missing"]` adds the node `"missing"` to the graph even in a read-only context such as `if (G["missing"]["attr"] == 3)`. The method cannot be `const`. This matches Python dict semantics but is surprising in C++ and breaks const-correctness.

**Fix:** Provide a `const`-qualified overload that throws `std::out_of_range` on a missing node, or document the write-creates behavior prominently at the declaration site. (Related: open issue #93.)

---

### 2.3 `lookup_map::operator[]` throws on missing keys — inconsistent with `std::map`

**File:** `include/nxpp/graph.hpp` ~line 190

`operator[]` calls `at()`, which throws `std::out_of_range`. Users familiar with `std::map` expect `operator[]` to default-insert on a missing key. The semantics are inverted from the standard.

**Fix:** Rename the throwing accessor to `at_or_throw` or document it as `at` semantics, removing `operator[]` from the const overload to avoid the naming conflict.

---

### 2.4 `num_vertices()` returns `int` — overflow and type inconsistency

**File:** `include/nxpp/graph.hpp` ~line 1877

```cpp
return static_cast<int>(boost::num_vertices(g));
```

Truncates `std::size_t` to `int`. Inconsistent with `nodes().size()`, which is `std::size_t`. Overflows silently for large graphs. (Also flagged in open issue #99.)

**Fix:** Return `std::size_t`.

---

### 2.5 No `num_edges()` convenience method

Users must call `edges().size()` or `edge_ids().size()`, both of which allocate and populate a `std::vector` just to count edges.

**Fix:** Add `std::size_t num_edges() const { return static_cast<std::size_t>(boost::num_edges(g)); }`.

---

### 2.6 `shortest_path(u, v, "weight")` — string parameter is actually a boolean switch

**File:** `include/nxpp/shortest_paths.hpp` ~line 359

The `weight` parameter looks like a generic attribute name selector, but only `""` (unweighted BFS) and `"weight"` (Dijkstra on built-in weight) are valid. Any other string throws. The signature is misleading.

**Fix:** Replace the `std::string` parameter with `bool use_builtin_weight = false` or an enum `WeightMode { Unweighted, BuiltIn }`.

---

### 2.7 `connected_components()` returns a label map, not a count

**File:** `include/nxpp/components.hpp` ~line 154

The docstring says "Returns the number of connected components," but the method returns an `indexed_lookup_map<NodeID, int>` mapping each node to its component label. The actual count comes from `connected_component_groups().size()`.

**Fix:** Fix the docstring. Consider renaming to `connected_component_map()` (a one-liner alias already exists) and deprecating the current name.

---

### 2.8 `prim_minimum_spanning_tree` includes a root → root self-entry

**File:** `include/nxpp/spanning_tree.hpp` ~line 101

The returned `std::map<NodeID, NodeID>` includes `root → root` (a Boost predecessor-map convention). Callers who iterate the map as tree edges will process a spurious self-edge for the root node.

**Fix:** Filter the root's self-entry before returning, or document explicitly that the root always maps to itself.

---

### 2.9 No `[[nodiscard]]` on any result-returning methods

Methods like `dijkstra_shortest_paths()`, `nodes()`, `edge_ids()`, `has_node()`, `maximum_flow()`, and `connected_components()` do not carry `[[nodiscard]]`. Silently discarded return values compile without any warning. (Open issue #95.)

**Fix:** Add `[[nodiscard]]` to every `const` method that returns a computed result.

---

### 2.10 No `noexcept` on pure-query methods

**File:** Throughout `include/nxpp/graph.hpp`

Methods such as `is_directed()`, `is_multigraph()`, `is_weighted()`, `number_of_nodes()`, and `number_of_edges()` are not `noexcept`. This prevents the compiler-generated move constructor for `Graph` from being `noexcept`, causing `std::vector<Graph>` to fall back to copying instead of moving on reallocation. (Open issue #99.)

**Fix:** Annotate all non-throwing query methods with `noexcept`.

---

### 2.11 No C++20 concepts for `NodeID` validation

**File:** `include/nxpp/graph.hpp`

`NodeID` constraints are enforced via manual `static_assert`. C++20 concepts (`nxpp::ValidNodeID`, `nxpp::NumericNodeID`) would produce dramatically better compiler diagnostics and enable concept-based overload resolution. (Open issue #85.)

**Fix:** Define and apply `concept ValidNodeID = std::copyable<T> && std::equality_comparable<T> && std::totally_ordered<T>` and constrain the `Graph` template directly.

---

## 3. Performance Concerns

### 3.1 O(E) reverse lookup for every edge-id operation

**File:** `include/nxpp/graph.hpp` ~line 431; `include/nxpp/multigraph.hpp` ~line 93

Every call to `has_edge_id`, `remove_edge(id)`, `get_edge_weight(id)`, `set_edge_weight(id)`, `set_edge_attr(id)`, and `get_edge_endpoints(id)` performs a full linear scan of all edges to map an edge ID back to a BGL descriptor. In a multigraph loop over thousands of edges, this is quietly O(E²).

**Fix:** Maintain a `std::unordered_map<std::size_t, EdgeDesc>` reverse index alongside `edge_properties`, kept consistent in `add_edge` and `remove_edge`. This reduces all edge-id lookups from O(E) to O(1).

---

### 3.2 `erase_incident_edge_properties` scans all edges, not just incident ones

**File:** `include/nxpp/graph.hpp` ~line 467

This private method called by `remove_node` iterates `boost::edges(g)` and checks each edge for `source == v || target == v`. For a graph with E edges and degree D at node v, this is O(E) per call when O(D) is achievable.

**Fix:** Use `boost::out_edges(v, g)` and (for directed graphs) `boost::in_edges(v, g)` to collect only incident edges.

---

### 3.3 No batch `remove_nodes_from` — `rebuild_vertex_maps` called once per node

Every call to `remove_node` rebuilds all vertex maps in O(V). Removing k nodes in a loop is O(k·V). There is no bulk-removal API that amortises the rebuild.

**Fix:** Add `remove_nodes_from(std::vector<NodeID>)` that calls `rebuild_vertex_maps` only once after all removals.

---

### 3.4 PageRank hardcodes 20 iterations with no convergence check

**File:** `include/nxpp/centrality.hpp` ~line 57

The implementation always runs exactly 20 iterations. For large or sparse graphs this may not converge; for small graphs it wastes iterations. The iteration count is not documented.

**Fix:** Add a `double tolerance = 1e-6` parameter and an L1-norm convergence check (`if (norm(rank - next) < tolerance) break;`), plus a maximum-iteration cap.

---

### 3.5 Floyd-Warshall sorts the node order twice

**File:** `include/nxpp/shortest_paths.hpp` ~lines 802, 819

`floyd_warshall_all_pairs_shortest_paths_map()` calls `floyd_warshall_all_pairs_shortest_paths()` internally, causing the `order` vector to be sorted once inside the callee and then sorted again by the caller.

**Fix:** Extract a private `floyd_warshall_impl()` that returns `(matrix, order)` as a pair; both public functions call it once.

---

### 3.6 Flow graph construction is duplicated four times

**File:** `include/nxpp/flow.hpp`

`edmonds_karp_maximum_flow`, `push_relabel_maximum_flow_result`, `minimum_cut`, and `max_flow_min_cost_cycle_canceling` each contain ~40 lines of identical local type aliases, vertex-addition loops, and edge-addition loops for the internal BGL flow graph.

**Fix:** Extract a `build_capacity_flow_graph(capacity_attr)` helper that returns a struct containing the flow graph and property maps; call it from all four sites.

---

## 4. Missing Test Coverage

The table below lists algorithms and APIs that have no direct assertion-based unit test in the formal test suite (`tests/*.cpp`).

| Gap | Notes |
|---|---|
| `topological_sort()` | No test for correct ordering on a DAG, or that a cyclic graph throws |
| `kruskal_minimum_spanning_tree()` | Not called in any formal test file |
| `prim_minimum_spanning_tree()` | Not called in any formal test file |
| `dag_shortest_paths()` | Only exercised in the large-graph compare; no unit assertions |
| `floyd_warshall_all_pairs_shortest_paths()` | Only exercised in the large-graph compare |
| `two_sat_satisfiable()` | Zero test coverage in the formal suite |
| `bellman_ford_path()` with a negative cycle | No test that it throws or returns the correct error |
| Unweighted graph operations | BFS, DFS, components, shortest path on `Weighted=false` graphs are untested |
| `subgraph()` on undirected/unweighted graphs | Only tested on `DiGraph` and `MultiDiGraph` |
| `add_edges_from()` / `add_nodes_from()` | No formal test coverage |
| `Graph` copy/move independence | No test verifying attribute map independence after copy |
| `remove_edge(edge_id)` on non-existent ID | No test asserting it throws |
| `write_dot()` failure path | No test for a non-writable output path |
| `path_graph(0)` crash | No regression test for the unsigned-underflow bug |
| `betweenness_centrality` on directed graphs | Only tested on undirected path graphs; the normalization formula branches on `Directed` |
| `try_get_edge_attr(edge_id, key)` | The `edge_id` overload is never called in tests |

**Structural test issue:** Every test file declares a manually maintained `constexpr int total = N` counter. If a test is added to `main()` without incrementing `total`, the runner silently reports the wrong pass/fail count. Extract a `tests/test_helpers.hpp` with shared infrastructure and auto-count tests.

---

## 5. CI and Workflow Gaps

### 5.1 No sanitizer CI run (open issue #100)

None of the workflows compile with `-fsanitize=address,undefined`. Given the use of `std::any` casts, raw `void*` pointer keys in the flow cache, and BGL descriptor arithmetic, ASan/UBSan would be a meaningful correctness gate.

**Fix:** Add a CI job (e.g., `ubuntu-gcc-asan`) that builds the test suite with `-O1 -fsanitize=address,undefined -fno-omit-frame-pointer`.

---

### 5.2 Boost version now reported from the latest archive release

**File:** `.github/workflows/compatibility.yml` ~line 62

Compatibility CI now downloads the latest Boost release from `archives.boost.io` on Linux, macOS, Windows, and the large-graph comparison lane. The resolved Boost version is also printed in the workflow summary, so each run makes the exact library version explicit.

**Status:** addressed in `.github/workflows/compatibility.yml`.

---

### 5.3 No path filters on most workflows

`compatibility.yml`, `single-header.yml`, and `external-consumers.yml` still trigger on every push, including documentation-only changes. `wasm-experimental.yml` is now restricted to `wasm/**` changes plus its own workflow file, so unrelated PRs no longer pay the cost of the experimental WASM lane.

**Fix:** Add a `paths` filter to each workflow:

```yaml
on:
  push:
    paths:
      - 'include/**'
      - 'tests/**'
      - 'CMakeLists.txt'
      - 'conanfile.py'
      - '.github/workflows/compatibility.yml'
```

---

### 5.4 WASM CI can block unrelated PRs

**File:** `.github/workflows/wasm-experimental.yml`

The experimental WASM job runs on every push and pull request using a third-party action (`mymindstorm/setup-emsdk`). A flaky or broken Emscripten SDK release can block non-WASM PRs. The job is self-described as experimental but has no `continue-on-error: true`.

**Fix:** Add `continue-on-error: true` to the WASM job, or move it to a `workflow_dispatch`-only trigger.

---

### 5.5 Docs workflow does not run on PRs

**File:** `.github/workflows/docs-pages.yml`

The Doxygen build only runs on push to `main` and `workflow_dispatch`. A malformed Doxygen comment breaks the published site post-merge, with no pre-merge signal.

**Fix:** Add `pull_request` as a trigger with a build-only step (skip the GitHub Pages deploy on PRs).

---

### 5.6 No performance regression baseline (open issue #97)

Benchmarks run in CI but there is no comparison against a stored baseline. A 3× BFS regression on a 100k-node graph would pass CI silently.

**Fix:** Upload benchmark JSON as a GitHub Actions artifact, download the `main`-branch baseline on PRs, and fail if any benchmark regresses by more than a threshold (e.g., 15%). The `benchmark-action/github-action-benchmark` action handles this automatically.

---

### 5.7 Release workflow skips the large-graph comparison test

**File:** `.github/workflows/release.yml`

The release workflow runs the formal unit tests and single-header tests, but not `test_large_graph_compare`. A correctness regression in large-graph algorithms that passes unit tests could be published in a release.

**Fix:** Add a step running `bash scripts/unix/run_large_graph_compare.sh` before the "Create GitHub release" step.

---

### 5.8 No `ctest` timeout configured

**File:** `CMakeLists.txt`

There is no timeout on any `ctest` target. A hang (for example, the `path_graph(0)` infinite loop) would block CI indefinitely.

**Fix:** Add `set_tests_properties(... PROPERTIES TIMEOUT 30)` for each test, or set a global default with `set(CTEST_TEST_TIMEOUT 30)`.

---

## 6. Code Duplication and Maintainability

### 6.1 Test helper infrastructure is copy-pasted across all 7 test files

**Files:** `tests/test_core.cpp`, `tests/test_attributes.cpp`, `tests/test_centrality.cpp`, `tests/test_edge_cases.cpp`, `tests/test_flow.cpp`, `tests/test_multigraph.cpp`, `tests/test_remove_node.cpp`

The functions `expect()`, `expect_throws()`, `expect_runtime_error_message()`, `run_test()`, and ANSI colour constants (`green`, `red`, `reset`) are defined identically in every test file. A change to the output format must be applied in seven places.

**Fix:** Extract all shared helpers into `tests/test_helpers.hpp` and `#include` it from each test file.

---

### 6.2 Flow graph construction boilerplate is duplicated four times

**File:** `include/nxpp/flow.hpp`

Approximately 40 lines of identical local type aliases (`FlowGraph`, `CapacityMap`, `ResidualMap`, `ReverseMap`, `FlowEdgeDesc`), vertex-addition loop, and forward+reverse edge-addition loop appear in four separate functions.

**Fix:** Extract a `build_capacity_flow_graph(capacity_attr)` helper returning a struct with all constructed artifacts; call it from each flow function.

---

### 6.3 `get_edge_numeric_attr` numeric dispatch chain is duplicated

**Files:** `include/nxpp/attributes.hpp` ~line 219; `include/nxpp/multigraph.hpp` ~line 138

The chain of `std::any_cast` checks for `int`, `long`, `long long`, `float`, `double` is copy-pasted in both files. Adding support for `unsigned long long` or `std::size_t` (currently missing — see §1) requires editing two places.

**Fix:** Extract a free function `nxpp::detail::numeric_any_cast(const std::any&) -> double` used by both overloads.

---

### 6.4 `bfs_tree` and `dfs_tree` are structurally identical

**File:** `include/nxpp/traversal.hpp` ~lines 371, 451

Both functions: call the corresponding `_edges()` method, add the root node to a new graph, and add each tree edge. The bodies differ only in the traversal call.

**Fix:** Extract `build_tree_from_edges(root, edges)` and call it from both.

---

### 6.5 Deprecated free-function wrappers are intentionally explicit

**Files:** `include/nxpp/components.hpp`, `traversal.hpp`, `shortest_paths.hpp`, `spanning_tree.hpp`, `topological_sort.hpp`, `flow.hpp`, `centrality.hpp`

Each deprecated free function is intentionally written out as a small, readable forwarding shim. That repetition keeps the compatibility surface obvious and avoids hiding the deprecation text or target method behind a macro layer.

**Status:** accepted as intentional compatibility boilerplate.

---

## 7. Other Quality Concerns

### 7.1 `graph.hpp` pulls in heavyweight standard headers unconditionally

**File:** `include/nxpp/graph.hpp` ~lines 24–44

The core header no longer needs to pull in the heaviest incidental headers. The include cleanup for `#139` moved `<random>` to `generators.hpp`, `<queue>` to `flow.hpp`, `centrality.hpp`, and `shortest_paths.hpp`, and removed the unused `<boost/functional/hash.hpp>` dependency from the core header.

**Status:** addressed in `include/nxpp/graph.hpp` and the affected semantic sub-headers.

---

### 7.2 `boost::vertex_wrapper_index_t` pollutes the `boost` namespace

**File:** `include/nxpp/graph.hpp` ~line 71

```cpp
namespace boost {
    enum vertex_wrapper_index_t { vertex_wrapper_index };
    BOOST_INSTALL_PROPERTY(vertex, wrapper_index);
}
```

Declaring a user-defined enum and calling `BOOST_INSTALL_PROPERTY` in `namespace boost` is a known but fragile technique. If a future Boost release defines a symbol with the same name or enum value, there will be a silent ODR violation or a hard compile error.

**Fix:** Document this as a known global side effect in the header. Add a `static_assert` or preprocessor guard checking for name conflicts, and investigate whether a scoped BGL extension mechanism is available.

---

### 7.3 `conanfile.py` does not export the `cmake/` directory

**File:** `conanfile.py` ~line 26

`exports_sources` includes `cmake/*`, but the `package()` method only copies `LICENSE` and `*.hpp`. The `cmake/nxppConfig.cmake.in` file required by `find_package(nxpp CONFIG REQUIRED)` is never installed into the Conan package, breaking the CMake integration advertised in the documentation.

**Fix:**

```python
def package(self):
    copy(self, "LICENSE", ...)
    copy(self, "*.hpp", ...)
    copy(self, "cmake/*.cmake.in", src=self.source_folder, dst=os.path.join(self.package_folder, "cmake"))
```

---

### 7.4 `to_dot` ignores all user-defined edge and node attributes

**File:** `include/nxpp/viz/dot.hpp`

`to_dot` only emits `weight=` and `edge_id=`. Attributes set via `set_edge_attr` / `set_node_attr` are silently dropped. There is also no `DotOptions` field for graph-level Graphviz attributes (`rankdir`, `splines`, `bgcolor`).

**Fix:** Add optional output of user-defined attributes behind a `DotOptions::show_user_attrs = false` flag for backward compatibility. Add `std::map<std::string, std::string> graph_attrs` to `DotOptions`.

---

### 7.5 `DotLayout` enum is defined but never used

**File:** `include/nxpp/viz/dot.hpp` ~line 23

```cpp
enum class DotLayout { Dot, Neato, Fdp, Sfdp, Circo };
```

This enum has no field in `DotOptions` and is referenced nowhere in the library, tests, or examples. It is dead API surface.

**Fix:** Either remove it or wire it into `DotOptions` and the `to_dot` output.

---

### 7.6 `is_dot_number` incorrectly accepts `+` as a leading character

**File:** `include/nxpp/viz/dot.hpp` ~line 71

The implementation allows `+` at position 0. DOT format permits leading `-` on numbers but not `+`. A value like `"+3"` passes `is_dot_number` but would need to be quoted to be a valid Graphviz attribute.

**Fix:** Remove `+` from the valid leading-character set in `is_dot_number`.

---

### 7.7 No `CONTRIBUTING.md` (open issue #98)

Contributors must reverse-engineer the build system from CI workflows and `CMakeLists.txt`. There are no documented requirements (CMake version, Boost version, C++ standard), code style rules, or PR guidelines.

**Fix:** Add `CONTRIBUTING.md` covering: prerequisites, how to build and run tests locally, how to add a new algorithm (tests + snippet), and PR rules (one change per PR, update CHANGELOG, CI must pass).

---

### 7.8 No SemVer policy document (open issue #96)

The project went from `1.0.38` to `1.3.0` within days, suggesting patch numbers are used as commit counters rather than strict bugfix indicators. Without a documented policy, consumers cannot make reliable version-pinning decisions.

**Fix:** Add `VERSIONING.md` defining when PATCH/MINOR/MAJOR increments apply per SemVer 2.0.0, and link it from `CHANGELOG.md` and `README.md`.

---

### 7.9 Thread safety is completely undocumented (open issue #92)

The library maintains mutable internal structures with no documented thread safety model. The flow algorithm cache is explicitly not thread-safe (see §1.11). Users coming from NetworkX (which uses the GIL) may not expect this.

**Fix:** Add a "Thread Safety" section to `README.md` and `docs/API_REFERENCE.md`: concurrent reads on an unmodified graph are safe; any write must be externally synchronized; the flow cache makes sharing a graph across threads during flow computation unsafe.

---

### 7.10 `get_edge_numeric_attr` does not handle unsigned integer types

**Files:** `include/nxpp/attributes.hpp` ~line 219; `include/nxpp/multigraph.hpp` ~line 138

The numeric dispatch tries `int`, `long`, `long long`, `float`, `double` — but not `unsigned int`, `unsigned long`, `unsigned long long`, or `std::size_t`. Storing a capacity as `std::size_t` (a common choice) and reading it via `get_edge_numeric_attr` will throw `"stored value is not numeric"`.

**Fix:** Add `unsigned int`, `unsigned long`, `unsigned long long`, and `std::size_t` to the dispatch chain, or widen to `double` via a `std::visit` on a variant.

---

## 8. Open GitHub Issues — Status Overview

All 13 open issues were filed by the repository owner (Mik1810) on 2026-04-24. None have external comments, assignees, or milestones. The project has no external contributors at this time.

| # | Title | Labels | Relation to this review |
|---|---|---|---|
| #100 | CI: add ASan/UBSan sanitizer run | `enhancement`, `correctness` | §5.1 |
| #99 | Code quality: annotate methods with `noexcept` | `enhancement`, `dx` | §2.10 |
| #98 | Docs: add `CONTRIBUTING.md` | `documentation`, `dx` | §7.7 |
| #97 | CI: add performance regression tracking | `enhancement` | §5.6 |
| #96 | Docs: define and commit to a SemVer policy | `documentation`, `dx` | §7.8 |
| #95 | Code quality: add `[[nodiscard]]` to query methods | `enhancement`, `dx` | §2.9 |
| #93 | Correctness: audit Graph copy/move semantics | `correctness`, `api-design` | §1.11, §4 (copy/move tests) |
| #92 | Docs: document thread safety guarantees | `documentation`, `dx` | §7.9 |
| #88 | DX: introduce `nxpp::storage::` aliases for BGL selectors | `enhancement`, `api-design`, `dx` | New ergonomics improvement |
| #87 | Enhancement: add lazy `bfs_edges_view` / `dfs_edges_view` | `enhancement`, `performance`, `api-design` | §3 (eager allocation) |
| #85 | API: replace `NodeID` static_assert with C++20 concepts | `enhancement`, `api-design` | §2.11 |
| #84 | WASM: add minimal browser-demo investigation | `documentation`, `experimental`, `wasm` | Out of scope for C++ review |
| #80 | WASM: add native-vs-WASM overhead benchmarks | `experimental`, `wasm`, `performance` | Out of scope for C++ review |

**Recently closed issues relevant to this review:**

- **#93** (copy/move semantics): PR #103 added explicit copy/move constructors but was merged directly to `main` as commit `f32c89c` rather than through the PR. Issue remains open. No regression test for copy independence was added.
- **#86** (O(V+E) warning on `remove_node`): Closed — a `@warning` Doxygen comment was added. However, no batch `remove_nodes_from` API was added to address the root performance concern (§3.3).

---

## 9. Priority Summary

| Priority | # | Finding |
|---|---|---|
| **Critical** | 1.1 | `path_graph(0)` unsigned underflow — hangs the process |
| **Critical** | 3.1 | O(E) edge-id reverse lookup — quadratic in multigraph loops |
| **Critical** | 7.3 | `conanfile.py` not exporting `cmake/` — breaks published Conan integration |
| **High** | 1.2 | Undirected graph edge double-counting in `collect_edge_ids_between` |
| **High** | 1.6 | `remove_edge(u,v)` on undirected multigraphs leaves stale BGL edges |
| **High** | 1.7 | Path reconstruction loops with no cycle guard — infinite loop risk |
| **High** | 1.11 | Min-cost flow cache is not thread-safe |
| **High** | 3.6 | Flow graph construction boilerplate duplicated 4× |
| **High** | 5.1 | No sanitizer CI run (also open issue #100) |
| **Medium** | 1.4 | `complete_graph` generates redundant/wrong edges for undirected multigraphs |
| **Medium** | 1.5 | `erdos_renyi_graph` silently drops isolated nodes |
| **Medium** | 1.9 | `bfs_tree` / `dfs_tree` hardcode `double` edge weight type |
| **Medium** | 2.1 | `edges()` return type differs between weighted and unweighted graphs |
| **Medium** | 2.9 | No `[[nodiscard]]` on any result-returning methods (open issue #95) |
| **Medium** | 3.3 | No batch `remove_nodes_from` — O(V) rebuild per node in a loop |
| **Medium** | 3.4 | PageRank fixed 20-iteration cap with no convergence check |
| **Medium** | 4 | No formal tests for: topological sort, spanning trees, 2-SAT, Floyd-Warshall, DAG paths |
| **Medium** | 6.1 | Test helper boilerplate duplicated across all 7 test files |
| **Medium** | 7.10 | `get_edge_numeric_attr` does not handle unsigned integer types |
| **Low** | 1.3 | Dead branch in `node()` method |
| **Low** | 2.4 | `num_vertices()` returns `int` instead of `std::size_t` (open issue #99) |
| **Low** | 2.5 | No `num_edges()` convenience method |
| **Low** | 5.3 | No path filters on CI workflows — doc changes trigger full suite |
| **Low** | 5.4 | WASM CI can block unrelated PRs — missing `continue-on-error: true` |
| **Low** | 5.5 | Doxygen build doesn't run on PRs — breakage discovered post-merge |
| **Low** | 7.1 | `graph.hpp` heavyweight include cleanup addressed (#139) |
| **Low** | 7.4 | `to_dot` ignores user-defined node/edge attributes |
| **Low** | 7.5 | `DotLayout` enum defined but never used |
| **Low** | 7.7 | No `CONTRIBUTING.md` (open issue #98) |
| **Low** | 7.8 | No SemVer policy document (open issue #96) |

---

*Report generated by automated deep review on 2026-04-29.*
