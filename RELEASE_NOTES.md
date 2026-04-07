# Release Notes

These notes are written for GitHub releases and can be more narrative than the
version entries in `CHANGELOG.md`.

## [0.8.10]

### Highlights

- Closed `#14` by adding direct usage examples for the richer result wrappers instead of only listing their fields and names.
- The docs now show practical examples for:
  - `SingleSourceShortestPathResult`
  - `MaximumFlowResult`
  - `MinimumCutResult`
- `MinCostMaxFlowResult`
- `indexed_lookup_map`
- The new examples live in `README.md` and `docs/API_REFERENCE.md`, so the public docs now explain not just what those wrappers are, but how callers are expected to use them.

### Verification

- `timeout 30s git diff --check`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version CHANGELOG.md`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version RELEASE_NOTES.md`

### Assets

- This change does not add a new binary asset, but it makes the richer wrapper-returning APIs easier to learn from the repository docs and the future release page.

## [0.8.9]

### Highlights

- Closed `#13` by making the thin-alias story explicit instead of leaving it scattered across headers and guide text.
- The docs now distinguish between:
  - explicit graph type presets such as `WeightedDiGraph`
  - shorter compatibility-friendly synonym aliases such as `DiGraph`
  - convenience method aliases such as `single_source_dijkstra(...)` and `minimum_spanning_tree(...)`
  - deprecated namespace-scope compatibility wrappers such as `nxpp::bfs_edges(G, ...)`
- This makes the canonical entry points easier to identify without removing the migration-friendly alias layer.
- Closed `#16` by making the weighted-overload semantics explicit:
  - shortest-path overloads that accept `"weight"` are now documented as compatibility-shaped wrappers around the built-in edge-weight property
  - flow/min-cost APIs now say more clearly that their default `weight_attr = "weight"` still refers to the built-in edge-weight property
  - the docs no longer imply that arbitrary custom attribute names are broadly supported as interchangeable weight backends unless a specific API says so
- Closed `#27` at the design level by making the attribute-system direction explicit instead of leaving it implicit in the code and tests.
- Documented that:
  - `std::any` remains the pragmatic attribute-storage model for now
  - proxy syntax is kept for ergonomic writes and demos
  - checked accessors are the recommended read path
  - `edge_id` is the precise attribute path for multigraph usage
- Closed `#25` by removing `Graph::node_properties` and `Graph::edge_properties` from the public mutable surface, so callers can no longer bypass wrapper invariants by mutating those stores directly.
- Kept the advanced read-only escape hatches such as `get_impl()`, `get_bgl_to_id_map()`, and `get_id_to_bgl_map()` documented as intentional `const` integration hooks rather than treating them as normal day-to-day API entry points.
- Clarified the source-of-truth split between:
  - the generated Doxygen site for declaration-level API facts
  - `README.md` for overview and navigation
  - the markdown files under `docs/` for policy, rationale, and curated guides
- Repositioned `docs/API_REFERENCE.md` as a curated companion guide instead of a second full declaration dump.
- Refined the generated Doxygen site into a cleaner public reference instead of a mostly stock Doxygen dump.
- Narrowed the generated input surface so the published site now focuses on:
  - the public semantic headers
  - the generated landing page
  - the curated API / complexity / testing / external-usage guides
- Removed the noisier release-log style pages from the reference navigation, so the site no longer treats `CHANGELOG.md`, `RELEASE_NOTES.md`, `SESSION.md`, or `TODO.md` as part of the main API docs surface.
- Rebuilt the landing page into a more editorial entry point with working links to generated class, file, and guide pages instead of the earlier raw repository-relative file links.
- Reworked the custom stylesheet so the site now overrides the stock Doxygen chrome more decisively, with a calmer neutral palette, a cleaner top navigation, better spacing, and more deliberate treatment of cards, member sections, and code blocks.
- Updated `.github/workflows/docs-pages.yml` so changes under `assets/doxygen/` also trigger a Pages rebuild, which keeps the published theme in sync with the checked-in CSS.

### Verification

- `timeout 30s rm -rf build/docs/doxygen && mkdir -p build/docs/doxygen && doxygen Doxyfile`
- `timeout 30s rg -n "custom.css|md_docs_2API__REFERENCE|classnxpp_1_1Graph|graph_8hpp_source" build/docs/doxygen/html/index.html`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version CHANGELOG.md`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version RELEASE_NOTES.md`

### Assets

- This change does not add a new binary release asset, but it substantially improves the presentation and focus of the hosted generated API reference site.

## [0.8.8]

### Highlights

- Closed `#28` by turning the generated-docs path into a real repository feature instead of leaving public API documentation entirely in handwritten markdown.
- Added Doxygen-style inline comments to the most user-facing public API areas, including:
  - the main `Graph<...>` wrapper surface
  - key traversal entry points
  - single-source shortest-path result helpers
  - flow / minimum-cut result types
  - the built-in graph generators
- Extended that same pass to the remaining public semantic-header surface, including:
  - component helpers
  - minimum-spanning-tree and topological-sort wrappers
  - 2-SAT helpers
  - the deprecated free-function aliases that still remain part of the public reference for migration purposes
- Added a minimal root `Doxyfile` and a dedicated `docs/DOXYGEN_MAINPAGE.md` so the repository now has a concrete local scaffold for a future Javadoc-/Boost-style navigable API reference site.
- Updated the docs index and `README.md` so the generated-reference path is now part of the documented repository structure instead of only being an idea in the backlog.
- Polished the most frequently used `Graph` entry points again with richer multi-line comments, so the IDE hover text is closer to real API guidance than to a terse signature label, especially around edge insertion, removal, attribute access, explicit edge-ID usage, traversal results, shortest-path result shapes, flow wrappers, and minimum-spanning-tree helpers.
- Tightened the Doxygen configuration itself so local generation now runs cleanly with the expected toolchain, and added a dedicated GitHub Pages workflow that can publish the generated HTML reference site directly from `main`.
- The generated reference is now also live-publishable through GitHub Pages at the repository level, without introducing a committed `CNAME` or coupling the docs deploy to an external personal domain.
- Added a small repo-local Doxygen stylesheet under `assets/doxygen/`, giving the generated site a more polished and formal visual presentation without depending on an external theme or custom post-processing step.
- Tightened `.github/workflows/snippet-review.yml` so the expensive Boost-backed snippet parity workflow now runs only when snippet-related files, public headers, the snippet runner, or the showcase mains change, instead of firing on every generic documentation or release-note edit.

### Verification

- `timeout 30s git diff --check`
- `timeout 30s rm -rf build/docs/doxygen && mkdir -p build/docs/doxygen && doxygen Doxyfile`
- `timeout 30s rg -n "custom.css" build/docs/doxygen/html/index.html`
- `timeout 30s bash scripts/unix/run_tests.sh`
- `timeout 30s bash scripts/unix/build_single_header.sh`
- `timeout 30s g++ -std=c++20 -Wall -Wextra -pedantic -O0 -I/workspaces/nxpp/include main_nxpp.cpp -o /tmp/main_nxpp_check`

### Assets

- This change does not add a new release asset, but it introduces the first real generated-docs scaffold, improves IDE hover / header-driven API discovery substantially, and adds a GitHub Pages publication path for the generated reference site.

## [0.8.7]

### Highlights

- Finalized `#20` by extending the formal-test workflow beyond the old Linux-only `g++` path into a full cross-platform matrix.
- `.github/workflows/compatibility.yml` now runs a readable matrix covering:
  - `ubuntu-latest` with `g++`
  - `ubuntu-latest` with `clang++`
  - `macos-latest` with `clang++`
- `windows-latest` is now covered too through a native MSVC (`cl`) path, using the repository's PowerShell runners plus a cached Boost archive/extract setup instead of the earlier Linux-centric shell assumptions.
- Kept the snippet-review workflow unchanged so the example/parity layer remains intact and easy to read.
- Updated the support matrix in `README.md` so it now matches the real CI-backed story instead of the older Ubuntu-only wording.
- Cleaned the compatibility summaries so optional environment values are handled more safely and the Windows job strips ANSI escapes before publishing its Markdown output.

### Verification

- `timeout 30s git diff --check`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version CHANGELOG.md`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version RELEASE_NOTES.md`
- `timeout 30s bash scripts/unix/run_tests.sh`
- `timeout 30s bash scripts/unix/run_single_header_tests.sh`
- `timeout 30s bash scripts/unix/run_large_graph_compare.sh`

### Assets

- This change does not add a new release asset, but it broadens the CI-backed support story for the formal suite and makes that support matrix more honest in the docs.

## [0.8.6]

### Highlights

- Closed `#20` by extending the formal-test workflow beyond the old Linux-only `g++` path.
- `.github/workflows/compatibility.yml` now runs a small, readable matrix covering:
  - `ubuntu-latest` with `g++`
  - `ubuntu-latest` with `clang++`
  - `macos-latest` with `clang++`
- Kept the snippet-review workflow unchanged so the example/parity layer remains intact and easy to read.
- Updated the support matrix in `README.md` so it now matches the real CI-backed story instead of the older Ubuntu-only wording.

### Verification

- `timeout 30s git diff --check`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version CHANGELOG.md`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version RELEASE_NOTES.md`

### Assets

- This change does not add a new release asset, but it broadens the CI-backed support story for the formal suite and makes that support matrix more honest in the docs.

## [0.8.5]

### Highlights

- Closed `#17` by adding a minimal root `CMakeLists.txt`.
- The repo now exposes `nxpp` as a header-only `INTERFACE` target named `nxpp`, intended for the simplest vendored `add_subdirectory(...)` use case.
- Updated the external-usage docs with a matching minimal CMake example.
- Kept the scope deliberately small:
  - no install rules yet
  - no exported package config yet
  - no CMake-driven test matrix yet

### Verification

- `timeout 30s cmake -S /tmp/nxpp-cmake-consumer -B /tmp/nxpp-cmake-consumer/build`
- `timeout 30s cmake --build /tmp/nxpp-cmake-consumer/build`

### Assets

- This change does not add a new release asset, but it gives external users a minimal CMake entry point in addition to the plain compiler-based integration path.

## [0.8.4]

### Highlights

- Closed `#30` by clarifying how the repository's verification layers should be read.
- The docs now distinguish more explicitly between:
  - showcase programs
  - the curated snippet parity / regression layer
  - the formal assertion-based suite
  - the generated single-header validation path
  - the opt-in large-graph raw-Boost comparison path
- Tightened the wording around `snippet/` specifically so it is now described as an example-backed parity layer rather than as a substitute for the formal suite.

### Verification

- `timeout 30s rg -n "snippet|example|showcase|formal test|large-graph|parity|TEST.md|testing" README.md docs tests scripts snippet .github/workflows`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version CHANGELOG.md`
- `timeout 30s python3 scripts/extract_release_notes.py --latest-version RELEASE_NOTES.md`

### Assets

- This change does not add a new release asset, but it makes the repository's verification story easier to read and less ambiguous for contributors.

## [0.8.3]

### Highlights

- Closed `#6` by standardizing the main runtime-error wording across `nxpp`'s public surface.
- Replaced the older mix of generic messages, legacy `NetworkXError` text, and inconsistent punctuation with a simpler family of messages such as:
  - `Node lookup failed: ...`
  - `Traversal failed: ...`
  - `Shortest-path lookup failed: ...`
  - `Path reconstruction failed: ...`
  - `Minimum spanning tree failed: ...`
  - `Flow setup failed: ...`
- Kept the change intentionally conservative: behavior and exception types stay the same, while the wording becomes easier to read and easier to test consistently.

### Verification

- `timeout 30s bash scripts/run_tests.sh`
- `timeout 30s bash scripts/build_single_header.sh`
- `timeout 30s bash scripts/run_single_header_tests.sh`
- `timeout 30s bash scripts/run_large_graph_compare.sh`

### Assets

- This change does not add a new release asset, but it makes runtime failures more consistent for users and easier to assert in focused tests.

## [0.8.2]

### Highlights

- Closed `#18` by documenting the external consumer story directly: the repo now explains how to consume `nxpp` either from the modular `include/` tree or from the tested single-header release asset.
- Added a dedicated external-usage guide with:
  - the expected include-path layout for modular headers
  - the intended use of the released `nxpp.hpp` asset
  - the plain Boost Graph dependency story
  - minimal `g++` compile examples for both integration styles
- Clarified that `dist/nxpp.hpp` inside the repo is a generated validation artifact, while external consumers should prefer the tested `nxpp.hpp` release asset.

### Verification

- `timeout 30s bash scripts/build_single_header.sh`
- `timeout 30s g++ -std=c++20 -I/workspaces/nxpp/include /tmp/nxpp_external_modular.cpp -o /tmp/nxpp_external_modular`
- `timeout 30s g++ -std=c++20 -I/workspaces/nxpp/dist /tmp/nxpp_external_single.cpp -o /tmp/nxpp_external_single`

### Assets

- This change does not add a new release asset, but it makes the current external-consumption story much easier to follow from the repository docs.

## [0.8.1]

### Highlights

- Closed `#7` by documenting and enforcing the real `NodeID` contract at compile time: the main `Graph<NodeID, ...>` template now requires copyability, equality comparison, and `std::less` ordering, while the numeric generators keep a separate `std::size_t`-constructibility requirement because they synthesize node IDs `0..n-1`.

### Verification

- `timeout 30s bash scripts/run_tests.sh`
- `timeout 30s bash scripts/build_single_header.sh`
- `timeout 30s bash scripts/run_single_header_tests.sh`
- `timeout 30s bash scripts/run_large_graph_compare.sh`

### Assets

- This change does not add a new release asset, but it makes the `NodeID` contract explicit in both compile-time diagnostics and the user-facing docs.

## [0.8.0]

### Highlights

- Switched the release workflow to a fully automatic main-branch path: a push to `main` now treats the top version in `CHANGELOG.md` and `RELEASE_NOTES.md` as the concrete next release, skips if that tag already exists, otherwise builds the standalone header, runs the modular and single-header suites, creates `vX.Y.Z`, and publishes the GitHub release in the same run.
- Started the first `#26` complexity-hardening pass by switching several public result wrappers from hash-backed storage to `std::map`, including `lookup_map`, `SingleSourceShortestPathResult`, Floyd-Warshall's map view, and the rooted Prim parent map.
- Continued `#26` by switching the core `NodeID -> vertex_descriptor` translation map plus the external node/edge attribute stores to `std::map`, keeping the dominant Boost algorithmic phases unchanged while giving wrapper-managed lookups real tree-based bounds.
- Updated the public complexity notes so the ordered map costs now cover both the earlier result wrappers and the wrapper-managed node / attribute lookup paths instead of relying on expected hash-table costs.
- Added a second result-wrapper path for `#26`: `indexed_lookup_map`, which lets `connected_components()`, `strong_component_map()`, and `degree_centrality()` keep linear materialization and the same dominant Boost-side asymptotic order while still offering `O(log V)` key lookup without hash-table assumptions.
- Extended `indexed_lookup_map` to `strong_components()` too, so the SCC representative/root map now follows the same real-bound, no-hash, linear-materialization story as the other component helpers.
- Extended the same wrapper to `bfs_successors()`, `dfs_predecessors()`, and `dfs_successors()`, so the traversal tree-view helpers now keep the same dominant BFS/DFS complexity as Boost while exposing real `O(log V)` key lookup instead of hash-backed results.
- Replaced the old external `VertexDesc -> index` hash map inside `Graph` with an internal wrapper-index vertex property, which removes the last big hash-backed dependency from the core BGL algorithm path without narrowing the supported selector combinations.
- Removed the remaining local `NodeID -> index` hash maps from the flow helpers too, reusing the wrapper's maintained vertex indices directly so the library implementation under `include/nxpp` no longer depends on `std::unordered_map`.
- Removed eager all-path storage from `SingleSourceShortestPathResult`, replacing it with on-demand `path_to(target)` reconstruction so the single-source Dijkstra, Bellman-Ford, and DAG shortest-path wrappers now stay aligned with the dominant complexity of their Boost counterparts.

### Verification

- `timeout 30s bash scripts/build_single_header.sh`
- `timeout 30s bash scripts/run_tests.sh`
- `timeout 30s bash scripts/run_single_header_tests.sh`
- `timeout 30s bash scripts/run_large_graph_compare.sh`

### Assets

- This change does not add a new release asset, but it significantly strengthens the public complexity guarantees and aligns the single-source shortest-path wrappers with the dominant complexity of the delegated Boost algorithms.

## [0.7.13]

### Highlights

- Strengthened the opt-in large-graph comparison path so several of the most valuable wrapper-vs-Boost checks now run across multiple fixed seeds instead of trusting a single deterministic graph per scenario.
- The multi-seed pass currently covers representative BFS, connected-components, strongly-connected-components, Dijkstra, and post-`remove_node()` alignment checks.
- Added a non-default selector regression using `nxpp::Graph<int, int, true, false, true, boost::listS, boost::listS>` so the large-graph path now validates one of the advanced supported graph configurations against raw Boost as well.

### Verification

- `timeout 30s bash scripts/run_tests.sh`
- `timeout 30s bash scripts/run_large_graph_compare.sh`

### Assets

- This change does not add a new release asset, but it makes the large-graph correctness story materially stronger for both default and non-default supported graph configurations.

## [0.7.12]

### Highlights

- Added an opt-in large-graph comparison path through `tests/test_large_graph_compare.cpp` and `scripts/run_large_graph_compare.sh`.
- The new driver generates deterministic larger graph instances and compares `nxpp` directly against raw Boost for BFS depth, DFS tree edges, connected components, strongly connected components, Dijkstra, Bellman-Ford, DAG shortest paths, reachable negative-cycle detection, post-`remove_node()` state, large multigraph mutation behavior, attribute preservation/cleanup after repeated mutations, combined weighted-graph mutation sequences, Floyd-Warshall all-pairs shortest paths, large maximum-flow/minimum-cut results, and large successive-shortest-path min-cost flow.
- Added `.github/workflows/large-graph-compare.yml` so GitHub Actions can run the same comparison path separately against both the modular headers and the generated standalone `dist/nxpp.hpp`.
- Kept this verification path separate from `bash scripts/run_tests.sh`, so the default formal suite remains fast while larger scale checks stay available when needed.

### Verification

- `timeout 30s bash scripts/run_tests.sh`
- `timeout 30s bash scripts/run_large_graph_compare.sh`

### Assets

- This change does not add a new release asset, but it adds a new scale-oriented correctness check for the modular and single-header library surfaces.

## [0.7.11]

### Highlights

- Added a minimal compiler and platform support matrix to `README.md` so the repository now states its current support story more explicitly and conservatively.
- Documented Linux with Boost Graph and a C++20-capable `g++` toolchain as the main verified path based on the current GitHub Actions workflows and local repo scripts.
- Clarified that macOS and Windows are expected follow-up environments rather than current CI-backed guarantees, and that Clang is not yet claimed as a supported CI-verified toolchain.

### Verification

- `timeout 30s bash scripts/run_tests.sh`

### Assets

- This change does not add a new release asset, but it improves the public support and build expectations documented for external users.

## [0.7.10]

### Highlights

- Added `scripts/run_benchmark_report.py` so snippet compile benchmarks can now be run through one Python entry point that executes the serial pass first, then the parallel pass, and finishes by generating a report-ready analysis.
- Kept the benchmark output intentionally small: the workflow now produces exactly two benchmark CSVs, writes the narrative report to `benchmark/BENCHMARK.md`, and stores all plots under `benchmark/imgs/`.
- Added automatic benchmark backup behavior so previous CSVs, the generated benchmark report, and the `benchmark/imgs/` directory are moved into `backups/benchmark/<timestamp>/` before a new run starts.

### Verification

- `python3 scripts/run_benchmark_report.py --snippet bfs --iterations 1 --jobs 1`

### Assets

- This change does not add a new release asset, but it improves the local benchmark/report tooling used to evaluate compile-time costs after the semantic-header split.

## [0.7.9]

### Highlights

- Fixed `.github/workflows/release.yml` so the `workflow_dispatch` path no longer assumes that a tag pushed with `GITHUB_TOKEN` will trigger a second release run.
- The manual release path now creates and pushes the tag, then continues in the same workflow run to extract notes, build `dist/nxpp.hpp`, run `bash scripts/run_single_header_tests.sh`, and publish the GitHub release.
- Kept the pushed-tag path working too, so the same workflow still supports both manual kickoff and direct tag-driven release publication.

### Verification

- `python3 scripts/extract_release_notes.py --latest-version RELEASE_NOTES.md`
- `python3 scripts/extract_release_notes.py --latest-version CHANGELOG.md`
- `bash scripts/build_single_header.sh && bash scripts/run_single_header_tests.sh`

### Assets

- `nxpp.hpp` will be attached from the tested `dist/nxpp.hpp` build output.

## [0.7.8]

### Highlights

- Extended `.github/workflows/release.yml` with `workflow_dispatch`, so release kickoff no longer requires a manually created git tag.
- The dispatch path now reads the top version from `RELEASE_NOTES.md` and `CHANGELOG.md`, fails if they disagree, creates and pushes the corresponding `vX.Y.Z` tag, and lets the tag-triggered path create the actual GitHub release.
- Extended `scripts/extract_release_notes.py` with a `--latest-version` mode so the workflow can read the current top version from the versioned markdown files without duplicating the parsing logic.

### Verification

- `python3 scripts/extract_release_notes.py --latest-version RELEASE_NOTES.md`
- `python3 scripts/extract_release_notes.py --latest-version CHANGELOG.md`
- `bash scripts/build_single_header.sh && bash scripts/run_single_header_tests.sh`

### Assets

- `nxpp.hpp` will still be attached from the tested `dist/nxpp.hpp` build output once the tag-triggered release run completes.

## [0.7.7]

### Highlights

- Added a versioned `RELEASE_NOTES.md` file so GitHub releases can use richer release-specific prose without forcing the same level of detail into `CHANGELOG.md`.
- Switched `scripts/extract_release_notes.py` and `.github/workflows/release.yml` to read from `RELEASE_NOTES.md` instead of `CHANGELOG.md`, keeping the changelog concise while improving release presentation quality.
- Stopped ignoring `RELEASE_NOTES.md` in `.gitignore`, so the release-note source of truth now lives in the repository alongside the rest of the release automation.

### Verification

- `python3 scripts/extract_release_notes.py v0.7.7`
- `bash scripts/build_single_header.sh`
- `bash scripts/run_single_header_tests.sh`

### Assets

- `nxpp.hpp` will be attached from the tested `dist/nxpp.hpp` build output.

## [0.7.6]

### Highlights

- Added automated GitHub release creation from pushed `v*` tags through `.github/workflows/release.yml`.
- Moved release-note extraction into `scripts/extract_release_notes.py`, so the release workflow now pulls a versioned release body from this file instead of relying on ad-hoc local notes.
- Kept `.github/workflows/single-header.yml` focused on CI validation of the generated standalone header, while the dedicated release workflow now owns release publication.
- Updated the repository workflows to `actions/checkout@v6` and `actions/setup-python@v6`, removing the Node 20 deprecation path and aligning the repo with GitHub's Node 24 transition.

### Verification

- `bash scripts/build_single_header.sh`
- `bash scripts/run_single_header_tests.sh`
- `python3 scripts/extract_release_notes.py v0.7.6`

### Assets

- `nxpp.hpp` is attached from the tested `dist/nxpp.hpp` build output.

## [0.7.5]

### Highlights

- Added `scripts/run_single_header_tests.sh` so the full formal suite can be recompiled directly against `dist/nxpp.hpp`.
- Tightened the standalone-header release path so the generated asset is validated by the dedicated single-header suite before publication.
- Kept the small smoke test in place as a fast extra check on the generated file.

### Verification

- `bash scripts/run_tests.sh`
- `bash scripts/build_single_header.sh && bash scripts/run_single_header_tests.sh`

### Assets

- `nxpp.hpp` remains the standalone header asset built from `dist/nxpp.hpp`.

## [0.7.4]

### Highlights

- Extended the standalone-header automation so published releases can receive the tested CI-built `nxpp.hpp` asset instead of relying on a purely manual local upload path.
- Kept `dist/nxpp.hpp` available as a workflow artifact for normal CI runs while preparing the release flow for tighter automation.

### Verification

- `bash scripts/build_single_header.sh`
- `bash scripts/run_tests.sh`

### Assets

- `nxpp.hpp`
