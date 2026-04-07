# nxpp API Reference

A navigable reference for the public `nxpp` surface, focused on signatures,
result shapes, and header-level behavior notes.

## Use this site for

- public class and function signatures
- inline API documentation taken directly from the headers
- concrete return types and result wrappers
- namespace, type, and member navigation

## Companion guides

- [`API Reference`](md_docs_2API__REFERENCE.html): public API tables and syntax notes
- [`API Architecture`](md_docs_2API__ARCHITECTURE.html): placement policy for methods and helpers
- [`Complexity Guide`](md_docs_2COMPLEXITY.html): complexity policy and wrapper cost model
- [`External Usage`](md_docs_2EXTERNAL__USAGE.html): integration paths and compile guidance
- [`Testing Guide`](md_docs_2TEST.html): verification layers and local test commands

## Main entry points

- [`include/nxpp.hpp`](nxpp_8hpp_source.html): umbrella include for the whole library
- [`include/nxpp/graph.hpp`](graph_8hpp_source.html): core graph type, aliases, and wrapper surface
- [`include/nxpp/traversal.hpp`](traversal_8hpp_source.html): BFS / DFS helpers and visitor APIs
- [`include/nxpp/shortest_paths.hpp`](shortest__paths_8hpp_source.html): shortest-path wrappers and result types
- [`include/nxpp/flow.hpp`](flow_8hpp_source.html): flow, cut, and min-cost helpers
- [`include/nxpp/centrality.hpp`](centrality_8hpp_source.html): centrality helpers such as `degree_centrality()` and `pagerank()`
- [`include/nxpp/generators.hpp`](generators_8hpp_source.html): graph generators

## Quick navigation

- [`nxpp::Graph`](classnxpp_1_1Graph.html): main wrapper type
- [`SingleSourceShortestPathResult`](structnxpp_1_1SingleSourceShortestPathResult.html): shortest-path result wrapper
- [`MaximumFlowResult`](structnxpp_1_1MaximumFlowResult.html): flow result wrapper
- [`Classes`](annotated.html): class and struct index
- [`Files`](files.html): header-oriented navigation

This generated site is intentionally focused on the public reference surface. It
is meant to complement the longer markdown guides, not replace them.
