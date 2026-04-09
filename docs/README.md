# docs/

This directory is for longer-form documentation and the source markdown that
feeds the generated reference site.

The repository now has three distinct documentation roles:

- the root [`README.md`](../README.md) is the overview and navigation entry point
- the generated Doxygen site is the declaration-driven API reference
- the markdown files in `docs/` are companion guides, policies, and deeper notes

Source-of-truth rule:

- use the generated Doxygen reference for function signatures, member lists,
  result-wrapper fields, and declaration-local API behavior
- use the markdown guides in `docs/` for design rules, complexity policy,
  testing/process guidance, and curated usage notes
- use the root [`README.md`](../README.md) for the current user-facing project
  story, caveats, and where to go next

When these layers disagree, prefer:

1. generated Doxygen for declaration-level API facts
2. `README.md` for the current public overview and navigation story
3. the companion markdown guides for rationale, policy, and longer explanations

Cross-navigation rule:

- each narrative guide should point readers toward the most relevant generated
  API/module pages where practical
- the Doxygen landing page should point readers back toward the companion
  guides when policy, rationale, or curated usage notes matter more than raw
  declaration detail
- prefer stable published-reference URLs in the markdown guides when that keeps
  the same link useful both on GitHub and in the published docs

What currently lives here:

- [`API_REFERENCE.md`](API_REFERENCE.md): curated API companion guide for aliases,
  result-wrapper shapes, proxy syntax, high-value reference tables, and the
  write-creates / read-does-not-create implicit-creation policy
- [`API_ARCHITECTURE.md`](API_ARCHITECTURE.md): the public API placement policy for graph methods and namespace-scope helpers
- [`GRAPH_CONFIGURATION.md`](GRAPH_CONFIGURATION.md): graph selector/configuration policy and supported BGL customization surface
- [`COMPLEXITY.md`](COMPLEXITY.md): complexity policy, Boost-vs-`nxpp` cost model, and documentation rules
- [`EXTERNAL_USAGE.md`](EXTERNAL_USAGE.md): current external-consumption story for modular headers and the release single-header asset
- [`TEST.md`](TEST.md): distinction between showcase programs, snippet parity, formal tests, single-header validation, and the large-graph comparison path
- [`DOXYGEN_MAINPAGE.md`](DOXYGEN_MAINPAGE.md): landing page for the generated Doxygen reference scaffold

The repo now has a real semantic-header split under `include/nxpp/`:

- `graph.hpp` owns the core graph type and public aliases
- `attributes.hpp` owns the attribute-oriented overloads and accessors
- `multigraph.hpp` owns the precise `edge_id`-based multigraph API
- `traversal.hpp` owns the BFS/DFS visitor helpers, deprecated wrappers, and
  graph traversal method definitions
- `shortest_paths.hpp` owns the shortest-path helpers, deprecated wrappers,
  and graph shortest-path method definitions
- `components.hpp` owns the component helpers, deprecated wrappers, and graph
  component method definitions
- `spanning_tree.hpp` owns topological sort, minimum-spanning-tree helpers,
  deprecated wrappers, and the matching graph method definitions
- `flow.hpp` owns flow/cut/min-cost helpers, deprecated wrappers, supporting
  result types, and the matching graph method definitions
- `centrality.hpp` owns centrality helpers such as `degree_centrality()` and
  `pagerank()`, plus the matching graph method definitions
- `generators.hpp` owns `complete_graph`, `path_graph`, and
  `erdos_renyi_graph`
- `sat.hpp` owns the 2-SAT helpers

`include/nxpp.hpp` is now the real umbrella include for the modular layout.

The single-header rebuild script now produces a standalone `dist/nxpp.hpp`
by recursively expanding local `nxpp` headers while leaving external includes
alone. The generated file is a distribution artifact rather than another source
of truth in the repository.

Generated API reference:

- a minimal [`Doxyfile`](../Doxyfile) now exists at the repo root
- local generation command:
  - `doxygen Doxyfile`
- HTML output path:
  - `build/docs/doxygen/html/`
- published site:
  - `https://mik1810.github.io/nxpp/`
- optional GitHub Pages publication:
  - set the repository Pages source to `GitHub Actions`
  - let [docs-pages.yml](../.github/workflows/docs-pages.yml) publish the generated HTML

This is now a real generated reference path, even if it is still stylistically
lighter than a fully custom docs site. The goal is to keep `nxpp` on a
header-driven reference model that can continue growing toward a more
Javadoc-/Boost-style navigable documentation site.

Notes for custom domains:

- the GitHub Pages workflow publishes only the generated HTML artifact
- no `CNAME` file is committed in the repository for now
- if a custom domain is desired, configure it from the repository Pages settings
- this keeps the workflow simpler and makes it easier to fall back to the default `github.io` URL if the custom domain is ever removed or allowed to expire

Current status:

- the generated Doxygen site is now the primary API reference path
- this directory should not try to duplicate every declaration-level detail from the headers
- the root README is intentionally the overview / navigation entry point
- the files under `docs/` should stay focused on policy, rationale, examples,
  and curated technical guidance rather than becoming a second full API dump
- the current Doxygen coverage audit now also includes explicit file-level
  briefs across the semantic headers and clearer generated-reference coverage
  for the public graph alias/type surface
- the generated reference is now also grouped around the real semantic modules
  of the library instead of relying only on file and symbol indexes
- the generated landing page now also works more clearly as a real entry point
  for GitHub Pages readers, with start-here guidance, result-wrapper links,
  module navigation, and stronger connections back to the companion markdown
  guides
- the narrative guides and the generated reference now also cross-link more
  directly, so readers can move more easily between policy/guide material and
  the exact declaration pages for the same module area
- the implicit-creation rule is now treated as one of the high-value public
  policy topics rather than a buried proxy/accessor detail
- the Doxygen / GitHub Pages presentation is now also more deliberate: the
  published site enables the tree/sidebar navigation again and uses the local
  theme more assertively for tables, code blocks, wrapper pages, and the main
  landing page

Packaging / distribution strategy:

- today, the supported external-consumption paths are:
  - direct repository-local header usage
  - vendored CMake usage through `add_subdirectory(nxpp)`
  - installed-package CMake usage through `find_package(nxpp CONFIG REQUIRED)`
- the repository now also carries a first local `conanfile.py`, intended for
  local recipe validation before any remote publication story
- the repository now also carries a first local vcpkg overlay port under
  `packaging/vcpkg/ports/nxpp`, which is now the supported vcpkg path in the
  repository while curated-registry publication remains deferred by project
  maturity
- Debian / Ubuntu packaging has now been evaluated and explicitly deferred for
  now because the extra packaging/distribution maintenance cost would be higher
  than the current project needs
- the next packaging step is not Debian packaging and not package-manager fan-out
- the preferred order is:
  1. keep the CMake install / export / package-config path as the foundation
  2. Conan support
  3. vcpkg evaluation and support
  4. only later, if still worth the maintenance cost, Debian / Ubuntu packaging
- package-manager distribution should follow the installed CMake package story,
  not bypass it
- repository-hosted package definitions are expected to track the same release
  version as the tagged repository release:
  - `CMakeLists.txt`
  - `conanfile.py`
  - `packaging/vcpkg/ports/nxpp/vcpkg.json`
- if an external channel lags behind or is blocked by third-party policy, the
  docs should say that explicitly instead of implying simultaneous publication

Versioning / release policy:

- `CHANGELOG.md` is the concise technical history
- `RELEASE_NOTES.md` is the richer release-facing narrative
- `SESSION.md` is the append-only chronological context log
- the top version in `CHANGELOG.md` and `RELEASE_NOTES.md` is the concrete next release candidate
- new work should move to the next version once the current one is already tagged/published
- normal pushes to `main` do not publish releases by themselves
- releases are driven by [`release.yml`](../.github/workflows/release.yml) from a pushed `vX.Y.Z` tag or from `workflow_dispatch`
