# Release Notes

These notes are written for GitHub releases and can be more narrative than the
version entries in `CHANGELOG.md`.

## [1.0.27]

### Highlights

- Closed `#72` by expanding non-default selector coverage in the heavy
  large-graph raw-Boost comparison suite.
- `tests/test_large_graph_compare.cpp` now includes a second selector-focused
  regression based on `boost::listS` / `boost::vecS`, in addition to the
  existing `boost::listS` / `boost::listS` case.
- The new regression cross-checks node/edge state after `remove_node()`,
  Dijkstra shortest-path distances, and attribute cleanup/preservation behavior
  against the raw Boost baseline on deterministic large inputs.

## [1.0.26]

### Highlights

- Closed `#70` by adding an end-to-end external-consumer integration lane.
- The repository now includes dedicated fixture consumers under
  `tests/external_consumers/` for:
  - vendored CMake (`add_subdirectory(nxpp)`)
  - installed-package CMake (`find_package(nxpp CONFIG REQUIRED)`)
  - standalone single-header consumption
- A new Unix runner script, `scripts/unix/run_external_consumer_tests.sh`, now
  executes those integration modes as real consumers and reports failures by
  mode.
- A new CI workflow, `.github/workflows/external-consumers.yml`, runs that lane
  automatically and publishes a dedicated summary so consumer-path breakages are
  easier to diagnose quickly.

## [1.0.25]

### Highlights

- Folded the large-graph raw-Boost comparison lane into the main
  `compatibility.yml` workflow.
- The heavy large-graph path still runs as a separate dedicated job, so it does
  not get mixed into the normal fast test job logs or mental model.
- The old standalone `large-graph-compare.yml` workflow was removed, and the
  docs now describe the large-graph path as part of the main compatibility
  workflow family.

## [1.0.24]

### Highlights

- Closed `#71` by adding a dedicated formal centrality test file.
- `tests/test_centrality.cpp` now provides direct deterministic checks for:
  - `degree_centrality()`
  - `pagerank()`
  - `betweenness_centrality()`
- the Unix runner, Windows runner, and CMake formal-test source list now all
  include this new focused centrality test path.

## [1.0.23]

### Highlights

- Closed `#69` by adding a focused multigraph `minimum_cut()` regression test.
- `tests/test_flow.cpp` now includes a small deterministic case with
  heterogeneous parallel capacities and checks both:
  - the aggregate cut value
  - the precise `cut_edge_ids` view
- This locks in the `#68` fix so future endpoint-based regressions are much
  easier to catch.

## [1.0.22]

### Highlights

- Closed `#68` by correcting `minimum_cut()` in multigraph mode.
- The internal flow-graph builder now reads capacities through precise
  `edge_id` lookup instead of the old endpoint-based `(u, v)` convenience path.
- This means heterogeneous parallel capacities are now modeled per concrete
  edge instance instead of being flattened through the ambiguous endpoint view.

## [1.0.21]

### Highlights

- Closed `#67` by adding an explicit CI guard for release-facing version drift.
- The new `scripts/check_release_metadata_versions.py` script checks that the
  documented release version matches the repository-hosted package metadata in:
  - `CMakeLists.txt`
  - `conanfile.py`
  - `packaging/vcpkg/ports/nxpp/vcpkg.json`
- `release.yml` now runs that guard before extracting notes and publishing a
  release, so future version drift fails fast with a file-by-file mismatch
  message.
- The repository-hosted package metadata now also tracks `1.0.21`, so the new
  guard passes against the current release candidate instead of failing
  immediately on already-known drift.

## [1.0.20]

### Highlights

- Closed `#66` by aligning the repository-hosted package metadata with the documented release version.
- The shipped version metadata in:
  - `CMakeLists.txt`
  - `conanfile.py`
  - `packaging/vcpkg/ports/nxpp/vcpkg.json`
  was brought back into sync with the documented release line instead of the
  stale `1.0.1` version.
- This removes one of the clearest remaining sources of packaging drift between the release-facing docs and the package-manager entry points that live inside the repository.

## [1.0.19]

### Highlights

- Closed `#51` by strengthening the dedicated single-header CI itself.
- `single-header.yml` now:
  - rebuilds `dist/nxpp.hpp`
  - runs `bash scripts/unix/run_single_header_tests.sh`
  - still runs the small smoke program
  - still uploads the generated standalone header artifact
- This means the dedicated single-header workflow no longer stops at a smoke check; it now runs the real standalone-header suite as well.

## [1.0.18]

### Highlights

- Closed `#50` by correcting the public single-header testing story so it matches the CI and scripts that actually exist.
- The docs now distinguish clearly between:
  - the dedicated `single-header.yml` workflow, which builds `dist/nxpp.hpp`, smoke-tests it, and uploads the artifact
  - the deeper local `bash scripts/unix/run_single_header_tests.sh` path, which recompiles the assertion-based suite against `dist/nxpp.hpp`
  - the release workflow, which rebuilds the standalone header and runs the standalone-header suite before publishing the tested release asset
- This was a docs/process correction, not a CI behavior change.

## [1.0.17]

### Highlights

- Closed `#49` by moving the narrow `"weight"` semantics higher in the public documentation.
- The docs now state much more clearly, earlier in the reading path, that `"weight"` in the weighted shortest-path and min-cost-flow APIs refers to the built-in edge-weight property.
- The docs also now say more explicitly what `"weight"` does **not** mean in the current API:
  - it is not a generic user-defined edge-attribute key selector
  - it is not a promise that arbitrary custom numeric edge attributes can replace the built-in weighted path/cost channel
- This stayed documentation-only; the runtime behavior was already narrow and already enforced.

## [1.0.16]

### Highlights

- Closed `#48` by tightening the include-path story and the external-consumption guidance.
- The docs now distinguish more clearly between:
  - repository-local examples run from a clone of `nxpp`
  - modular external consumption with `#include <nxpp.hpp>`
  - vendored CMake usage
  - installed-package CMake usage
  - the tested single-header release asset
- This removes the old mixed signal where a repo-local include form such as `#include "include/nxpp.hpp"` could look like the normal external-consumer path even though it is only appropriate inside a local clone or repository-root example.

## [1.0.15]

### Highlights

- Closed `#47` by making the `NodeID` requirements much easier to discover from the normal public documentation path.
- The docs now state the practical rule more directly:
  - `NodeID` must be copyable
  - `NodeID` must support equality comparison
  - `NodeID` must be orderable through `std::less`
- The docs now also state more clearly what is *not* required for the core graph wrapper:
  - no public hash-table requirement
  - no global `NodeID(std::size_t)` requirement unless the user is calling the numeric generators that synthesize IDs `0..n-1`
- This stayed documentation-only; the compile-time constraints were already present in the implementation.

## [1.0.14]

### Highlights

- Closed `#46` by making the implicit-creation policy much more visible in the public docs.
- The docs now state the rule more directly:
  - write-style graph access may create missing nodes or edges
  - read-style accessors and algorithmic reads do not create implicitly
- The policy is no longer buried only in the API companion guide; it is now reflected in the README, the docs index, the API architecture guide, and the core proxy comments that back `G.node(u)[key]`, `G[u][v]`, and `G[u][v][key]`.

## [1.0.13]

### Highlights

- Closed `#56` by refining the Doxygen / GitHub Pages presentation layer.
- The published site now restores the tree/sidebar navigation, which makes the
  semantic module structure easier to browse without relying only on flat page
  links.
- The local theme now leans more deliberately into a formal reference look for:
  - landing-page navigation blocks
  - module/result-wrapper discovery
  - tables
  - code fragments
  - declaration pages
- A follow-up polish pass also cleaned up several remaining rough edges on class
  and group pages:
  - light-themed member tables now stay visually consistent
  - long signatures wrap more safely inside their boxes
  - `Parameters` and `Returns` blocks have cleaner padding and left alignment
  - repeated internal `requires` lines are still present when Doxygen emits
    them, but they are now visually toned down instead of dominating the page
- This pass stayed presentation-focused: it improves how the generated docs are
  read and navigated, not the underlying API content model.

## [1.0.12]

### Highlights

- Closed `#58` by adding short generated-doc examples for the richer result wrappers.
- The declaration pages for:
  - `SingleSourceShortestPathResult`
  - `MaximumFlowResult`
  - `MinCostMaxFlowResult`
  - `MinimumCutResult`
  now include short embedded usage examples rather than only field descriptions.
- The companion API guide now points readers directly at those generated struct pages, so the richer wrapper types are easier to discover from both the narrative docs and the generated site.

## [1.0.11]

### Highlights

- Closed `#57` by tightening the navigation between the narrative docs and the generated API reference.
- The README and companion guides now point more directly at the published Doxygen entry pages and semantic module pages for the same subject area.
- The docs index now makes the cross-navigation rule explicit:
  - narrative guides should point toward the most relevant generated pages
  - the generated landing page should point readers back toward the longer guides when policy or curated explanation matters more than raw declaration detail
- This makes the docs stack feel less like separate islands and more like one connected documentation system.

## [1.0.10]

### Highlights

- Closed `#55` by making the Doxygen main page a more useful landing page for GitHub Pages readers.
- The generated landing page now gives a clearer start-here flow:
  - begin with `nxpp::Graph`
  - jump into a semantic module such as shortest paths or flow
  - use the companion guides when policy or curated explanation matters
- It now also highlights the most common result wrappers directly, so readers can discover `SingleSourceShortestPathResult`, `MaximumFlowResult`, `MinCostMaxFlowResult`, and `MinimumCutResult` from the landing page instead of only through search or symbol indexes.
- The goal of this pass was not visual restyling; it was to make the generated site feel like a real API front door rather than a thin index page.

## [1.0.9]

### Highlights

- Closed `#54` by organizing the generated docs around the real semantic modules of the library.
- The semantic headers now define lightweight Doxygen groups for:
  - graph core
  - attributes
  - multigraph helpers
  - traversal
  - shortest paths
  - components
  - spanning/order helpers
  - flow
  - centrality
  - generators
  - 2-SAT
- The generated main page now links directly to those module groups, so the site no longer depends only on flat file/class navigation for orientation.

## [1.0.8]

### Highlights

- Closed `#53` by improving the generated-documentation coverage for deprecated aliases and thin namespace-scope wrappers.
- The compatibility aliases across traversal, shortest paths, components, spanning-tree helpers, topological sort, and flow now carry short but real parameter/return documentation instead of appearing as mostly skeletal deprecation stubs in the generated site.
- This keeps the migration surface easier to read in Doxygen search and symbol navigation without changing runtime behavior.

## [1.0.7]

### Highlights

- Closed `#52` by auditing the generated-doc coverage across the full modular public API surface.
- The semantic headers under `include/nxpp/` now carry explicit Doxygen `@file` briefs, so the generated site has a clearer module/file story instead of a set of mostly anonymous header pages.
- The public alias/type surface in `graph.hpp` now has clearer alias-level comments, which makes the weighted, compatibility, and unweighted graph presets easier to navigate from the generated reference.
- The docs index now also records that this file-level/header-level Doxygen coverage pass has been completed.

## [1.0.6]

### Highlights

- Closed `#45` by making the staged min-cost-flow path safer without changing its public shape.
- `push_relabel_maximum_flow(...)` still prepares the staged residual state for `cycle_canceling()`, but that staged state is no longer silently reusable after the graph changes.
- Graph mutations such as:
  - adding or removing nodes or edges
  - clearing the graph
  - changing built-in edge weights
  - changing edge attributes
  now invalidate the staged min-cost-flow state explicitly.
- `cycle_canceling()` now throws a clear error if callers try to reuse stale staged state after a graph mutation, instead of continuing with a hidden residual network that no longer matches the graph.
- The formal flow tests now cover that invalidation path directly.

## [1.0.5]

### Highlights

- Closed `#44` by making the flow-result wrappers precise again in multigraph cases.
- `MaximumFlowResult` and `MinCostMaxFlowResult` now expose both:
  - endpoint-keyed aggregate convenience views
  - precise `edge_id`-keyed flow views
- `MinimumCutResult` now exposes both:
  - endpoint-keyed `cut_edges`
  - precise `cut_edge_ids`
- Internally, the flow builders no longer collapse parallel edges into one `(u, v)` slot before materializing result wrappers, so multigraph callers can now inspect each original edge instance separately.
- The direct `flow.hpp` include path is now self-contained for these edge-aware flow helpers because it also pulls in the multigraph layer it now depends on.
- Added a new `centrality` snippet triplet under `snippet/`:
  - `centrality_nxpp.cpp`
  - `centrality.cpp`
  - `centrality.py`
- The snippet-review summary now describes that new case explicitly, and the snippet outputs were aligned to the current exact-match harness by printing to two decimal places.

## [1.0.4]

### Highlights

- Closed `#42` by turning one especially risky multigraph ambiguity into an explicit restriction.
- Endpoint-based attr-bearing `add_edge(..., attrs)` overloads now throw in multigraph mode instead of pretending to identify one concrete parallel edge.
- The precise supported alternative is now documented as:
  - `add_edge_with_id(...)`
  - followed by `set_edge_attr(edge_id, ...)`
- The reason for the restriction is now also documented explicitly:
  - `(u, v)` is not enough to identify one concrete parallel edge
  - silently choosing one endpoint-resolved edge would be misleading and unstable

## [1.0.3]

### Highlights

- Closed `#41` by making the endpoint-based multigraph semantics explicit API by API.
- The docs now include a strict table that maps each ambiguous endpoint-based multigraph API to:
  - its actual meaning in multigraph mode
  - the precise `edge_id`-based alternative to prefer when one concrete parallel edge matters
- This also makes one especially easy-to-misread rule more visible:
  - `remove_edge(u, v)` removes all parallel edges between those endpoints
  - `remove_edge(edge_id)` is the precise single-edge removal path

## [1.0.2]

### Highlights

- Closed `#43` by defining the public multigraph policy for precise APIs vs convenience APIs.
- The multigraph policy now states explicitly that:
  - `edge_id` is the precise path when one concrete parallel edge matters
  - endpoint-based `(u, v)` edge APIs remain convenience-oriented in multigraph mode unless a function documents a stronger guarantee
  - proxy access such as `G[u][v]` and endpoint-based edge-attribute reads should not be treated as precise single-parallel-edge handles

## [1.0.1]

### Highlights

- Closed `#59` by making the packaging and distribution strategy explicit before starting the implementation-heavy packaging work.
- The docs now state clearly that the currently supported consumption paths are still:
  - direct repository-local header usage
  - vendored CMake usage via `add_subdirectory(nxpp)`
- The preferred distribution order is now documented as:
  1. proper CMake install / export / package-config support
  2. Conan support
  3. vcpkg evaluation and support
  4. Debian / Ubuntu packaging only as a later optional target if the maintenance tradeoff still makes sense
- This also makes one important boundary explicit: `nxpp` is not yet documented as a `find_package(nxpp CONFIG REQUIRED)` package today, and that workflow remains a follow-up target rather than an implied public promise.
- Closed `#60` by implementing that next packaging step for real:
  - the root CMake project now installs headers and exports an `nxpp::nxpp` target
  - the build generates and installs `nxppConfig.cmake` and `nxppConfigVersion.cmake`
  - external consumers can now use `find_package(nxpp CONFIG REQUIRED)` once the package is installed into a visible prefix
  - the external-usage docs now distinguish vendored `add_subdirectory(...)` consumption from installed-package consumption explicitly
- Closed `#61` by tightening the external-consumer docs around the now-supported paths:
  - repository-local header usage inside a clone of the repo
  - vendored `add_subdirectory(...)` usage
  - installed-package `find_package(nxpp CONFIG REQUIRED)` usage
  - tested single-header release-asset usage
- The README and external-usage guide now use the same story for include style, Boost expectations, and when each path is appropriate, so users no longer need to infer whether they should follow the repo-local include model or the installed-package model.
- Closed `#62` by adding a first Conan recipe to the repository.
- The new `conanfile.py`:
  - models `nxpp` as a header-only package
  - declares Boost as a Conan dependency in header-only mode
  - is intended first for local validation with `conan create .` or an explicit C++20 host-profile override
  - does not yet introduce CI publication or remote package-upload workflow
- Closed `#63` by delivering a supported repository-hosted vcpkg overlay-port path.
- The vcpkg work now includes:
  - `packaging/vcpkg/ports/nxpp/portfile.cmake`
  - `packaging/vcpkg/ports/nxpp/vcpkg.json`
  - docs for the intended local validation command shape:
    - `vcpkg install nxpp --overlay-ports=/path/to/nxpp/packaging/vcpkg/ports`
- Follow-up validation and submission work then tightened that port further:
  - the port now removes the empty `lib/` and `debug/` directories that vcpkg warns about in post-build checks
  - the overlay install path was validated locally
  - a small external CMake consumer using the vcpkg toolchain and `find_package(nxpp CONFIG REQUIRED)` was also validated locally
  - a curated-registry submission was opened upstream and then closed under vcpkg's current minimum project-maturity policy
- The practical outcome is that `nxpp` now supports a real tested vcpkg overlay path in-repo, while curated-registry publication remains a future follow-up once the project is considered mature enough.
- Closed `#64` by evaluating Debian / Ubuntu packaging and choosing to defer it for now.
- The Debian / Ubuntu evaluation concluded that:
  - a Debian source package or Ubuntu PPA path would add a noticeably heavier maintenance burden than the project currently needs
  - the already-supported consumption paths remain sufficient for now:
    - direct source inclusion
    - installed-package CMake usage
    - Conan
    - the repository-hosted vcpkg overlay port
  - Debian / Ubuntu packaging should therefore remain a later optional revisit, not an immediate distribution target
- Closed `#65` by defining versioning and release expectations for distributed packages.
- The package-versioning policy now states explicitly that:
  - the tagged repository release is the source of truth for repository-hosted package metadata
  - `CMakeLists.txt`, `conanfile.py`, and `packaging/vcpkg/ports/nxpp/vcpkg.json` should track the same `X.Y.Z` release version
  - repository-hosted package paths are expected to move with the tagged release
  - third-party or policy-gated channels may lag behind, but that lag must be documented explicitly instead of being hidden behind ambiguous release language

### Notes

- This release candidate now covers the packaging strategy, the first concrete packaging implementation steps, and the release/versioning expectations for the repository-hosted package paths.
- The follow-up implementation path is now clearer:
  - `#62` Conan
  - `#63` vcpkg
  - `#64` Debian / Ubuntu packaging evaluation
  - `#65` package-distribution release expectations

## [1.0.0]

### Summary

`nxpp` reaches `1.0.0` as the first stable release milestone.

This version consolidates the work completed across the `0.8.x` line into a
clearer and more consumable public release:

- Stable header-only C++20 graph API on top of Boost Graph Library
- NetworkX-inspired ergonomics with graph-method-first usage
- Full algorithm coverage already present in the wrapper surface:
  - traversal, shortest paths, components, spanning tree/topological sort
  - flow/min-cut/min-cost helpers
  - centrality helpers (`degree_centrality`, `pagerank`, `betweenness_centrality`)
- Explicit multigraph precision path based on `edge_id`
- Cross-platform CI coverage for the formal suite (Linux/macOS/Windows)
- Stronger documentation and onboarding, including a quick-start-first root README
  plus the generated API reference and companion docs

### Notes

- This release marks maturity and stabilization of the current public surface.
- No new architectural direction is introduced in `1.0.0`; it formalizes the
  already-completed hardening and documentation work.

## [0.8.14]

### Highlights

- Closed `#40` by adding `docs/ATTRIBUTE_DESIGN.md`, a standalone design evaluation document for the attribute storage model.
- The document covers:
  - Explicit pros/cons of the current `std::any` model, including where it works well and where it falls short
  - Alternative A: bounded `std::variant<bool, int, long, double, std::string>` — same storage shape, bounded type set, `std::visit`-compatible, but cannot store custom object types
  - Alternative B: typed attribute schema — full compile-time type safety, but a major breaking change incompatible with the NetworkX dynamic style
  - A side-by-side comparison table across six criteria
  - A clear rationale for keeping `std::any` for now (educational/prototyping scope, no concrete blocking use case yet)
  - A conservative three-phase migration path (parallel alias → deprecation → removal) if a future version decides to switch to `std::variant`
- `docs/API_REFERENCE.md` now links to `ATTRIBUTE_DESIGN.md` at the end of the Attribute API section.

## [0.8.13]

### Highlights

- Closed `#5` by formalizing the implicit node and edge creation policy across the public API.
- A new "Implicit node and edge creation" section in `docs/API_REFERENCE.md` documents the write-creates / read-does-not-create rule with explicit tables for both sides.
- `NodeAttrProxy::operator=`, `EdgeAttrProxy::operator=`, and `EdgeProxy::operator=` in `graph.hpp` now carry docstrings confirming that they create the target node or edge when absent — matching the NetworkX convention of implicit creation on write-style access.
- A new formal test `test_implicit_creation_policy` verifies each proxy creation case and confirms that `has_node`, `has_edge`, and `neighbors` never create on read.

## [0.8.12]

### Highlights

- Closed `#33` by adding `betweenness_centrality()` as a first-class graph method.
- The new surface:
  - returns `indexed_lookup_map<NodeID, double>` keyed by public node IDs
  - normalizes results matching NetworkX `betweenness_centrality(G, normalized=True)` semantics
  - includes a deprecated free-function alias for consistency with the rest of the centrality API migration
- Implementation approach:
  - Brandes BFS algorithm implemented directly over the wrapper's internal BGL graph and vertex-index layer
  - no BGL property-map setup required from the caller
  - handles directed and undirected normalization transparently via the `Directed` template parameter
- The test suite covers the linear-chain case (interior nodes get non-zero betweenness, leaf nodes stay at zero), empty graphs, and singleton graphs.

## [0.8.11]

### Highlights

- Closed `#34` by adding `pagerank()` as a first class graph method.
- The new surface:
  - returns `indexed_lookup_map<NodeID, double>`
  - keeps the result keyed by public node IDs
  - includes a deprecated free-function alias for consistency with the rest of the method-first API migration
- Follow-up cleanup keeps the semantic header split consistent too:
  - `degree_centrality()` and `pagerank()` now live under `include/nxpp/centrality.hpp`
  - `include/nxpp.hpp` includes that new semantic header
  - the docs now describe centrality as its own algorithm layer instead of leaving it in `graph.hpp`
- The implementation stays conservative:
  - no extra configuration overloads yet
  - no new dependencies
  - explicit handling for dangling nodes in the fixed-iteration wrapper implementation
- Closed `#19` by turning the already-emerging versioning/release practice into an explicit documented policy.
- The docs now state clearly that:
  - `CHANGELOG.md` is the concise technical history
  - `RELEASE_NOTES.md` is the richer release-facing narrative
  - `SESSION.md` is the append-only chronological context log
  - normal pushes to `main` do not publish releases by themselves
  - the top version in `CHANGELOG.md` / `RELEASE_NOTES.md` is the concrete next release candidate
  - once a version is already tagged/published, new work must move to the next version
- Closed `#15` by adding an explicit docs section for the utility wrappers that are intentionally beyond direct one-to-one NetworkX/BGL parity.
- The docs now explain why the public surface includes wrappers such as:
  - `SingleSourceShortestPathResult`
  - `MaximumFlowResult`
  - `MinimumCutResult`
  - `MinCostMaxFlowResult`
  - `indexed_lookup_map`
  - small utility surfaces such as `degree_centrality()` and `two_sat_satisfiable(...)`
- This makes it clearer that `nxpp` is not only a naming/parity layer: it also adds a few focused C++-oriented result shapes where raw lower-level output would be awkward to consume directly.
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
