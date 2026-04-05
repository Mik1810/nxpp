# docs/

This directory is for longer-form and eventually generated documentation.

Today, the main user-facing source of truth is still the root [`README.md`](../README.md).

What currently lives here:

- [`API_REFERENCE.md`](API_REFERENCE.md): detailed public API tables, alias reference, proxy syntax, and algorithm/helper reference
- [`API_ARCHITECTURE.md`](API_ARCHITECTURE.md): the public API placement policy for graph methods and namespace-scope helpers
- [`GRAPH_CONFIGURATION.md`](GRAPH_CONFIGURATION.md): graph selector/configuration policy and supported BGL customization surface
- [`COMPLEXITY.md`](COMPLEXITY.md): complexity policy, Boost-vs-`nxpp` cost model, and documentation rules
- [`EXTERNAL_USAGE.md`](EXTERNAL_USAGE.md): current external-consumption story for modular headers and the release single-header asset
- [`TEST.md`](TEST.md): distinction between showcase programs, snippet parity, formal tests, single-header validation, and the large-graph comparison path

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
- `generators.hpp` owns `complete_graph`, `path_graph`, and
  `erdos_renyi_graph`
- `sat.hpp` owns the 2-SAT helpers

`include/nxpp.hpp` is now the real umbrella include for the modular layout.

The single-header rebuild script now produces a standalone `dist/nxpp.hpp`
by recursively expanding local `nxpp` headers while leaving external includes
alone. The generated file is a distribution artifact rather than another source
of truth in the repository.

What should eventually live here:

- longer guides that would make the root README too heavy
- docs-generation instructions once the tooling is committed

Current status:

- there is not yet a full generated docs site
- this directory should not pretend otherwise
- the root README is now intentionally smaller and should act as the overview / navigation entry point
- when root docs and `docs/` disagree, the root README describes the current user-facing reality while the files under `docs/` hold the deeper technical detail
