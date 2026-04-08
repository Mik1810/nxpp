# nxpp API Reference

A navigable reference for the public `nxpp` surface, focused on signatures,
result shapes, and header-level behavior notes.

This site is the declaration-driven API reference for `nxpp`. Use it when you
need to answer concrete questions such as:

- which header owns a helper or method
- what a result wrapper contains
- which overloads are public and how they are named
- which API path is precise versus convenience-oriented in multigraph mode
- where a helper lives in the semantic-header split under `include/nxpp/`

Use the longer markdown guides in `docs/` when you want policy, rationale,
packaging guidance, or curated usage notes rather than declaration-level facts.

## Start here

If you are new to the generated reference, the fastest path is usually:

1. start with [`nxpp::Graph`](classnxpp_1_1Graph.html) and the
   [`Graph Core`](group__nxpp__graph__core.html) module
2. jump to one semantic module such as
   [`Shortest Paths`](group__nxpp__shortest__paths.html) or
   [`Flow and Cut`](group__nxpp__flow.html)
3. open the matching companion guide if you need curated tables or policy notes
4. return to the declaration pages for exact signatures and result shapes

## Use this site for

- public class and function signatures
- inline API documentation taken directly from the headers
- concrete return types and result wrappers
- namespace, type, and member navigation

## Common starting points

- learning the main wrapper type:
  [`nxpp::Graph`](classnxpp_1_1Graph.html)
- finding the broad public API companion tables:
  [`API Reference`](md_docs_2API__REFERENCE.html)
- checking where a helper belongs architecturally:
  [`API Architecture`](md_docs_2API__ARCHITECTURE.html)
- understanding the current external-consumption paths:
  [`External Usage`](md_docs_2EXTERNAL__USAGE.html)
- checking verification guidance and test layers:
  [`Testing Guide`](md_docs_2TEST.html)

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

## Frequently used result wrappers

- [`SingleSourceShortestPathResult`](structnxpp_1_1SingleSourceShortestPathResult.html):
  distances, predecessors, and path reconstruction helpers
- [`MaximumFlowResult`](structnxpp_1_1MaximumFlowResult.html):
  max-flow value plus endpoint and `edge_id` flow views
- [`MinCostMaxFlowResult`](structnxpp_1_1MinCostMaxFlowResult.html):
  max-flow plus cost and precise per-edge flow views
- [`MinimumCutResult`](structnxpp_1_1MinimumCutResult.html):
  cut value, partition, endpoint cut view, and precise cut-edge IDs

## Library modules

- [`Graph Core`](group__nxpp__graph__core.html): main graph type, aliases, and proxy surface
- [`Attributes`](group__nxpp__attributes.html): attribute-oriented helpers and wrapper methods
- [`Multigraph`](group__nxpp__multigraph.html): precise `edge_id`-based multigraph helpers
- [`Traversal`](group__nxpp__traversal.html): BFS / DFS helpers and visitors
- [`Shortest Paths`](group__nxpp__shortest__paths.html): shortest-path wrappers and result types
- [`Components`](group__nxpp__components.html): connected-component and SCC helpers
- [`Spanning and Ordering`](group__nxpp__spanning.html): MST and topological-sort helpers
- [`Flow and Cut`](group__nxpp__flow.html): flow, cut, and min-cost-flow helpers
- [`Centrality`](group__nxpp__centrality.html): degree, PageRank, and betweenness centrality
- [`Generators`](group__nxpp__generators.html): graph-construction helpers
- [`2-SAT`](group__nxpp__sat.html): 2-SAT utilities

## Practical navigation tips

- use the module pages first if you are browsing by problem area
- use the file pages if you already know the semantic header you care about
- use the class/struct pages when you want the exact shape of a result wrapper
- use the companion markdown guides when you need policy, caveats, or curated
  examples that would be too long for declaration comments

## Generated-reference scope

This generated site is intentionally focused on the public reference surface. It
is meant to complement the longer markdown guides, not replace them. The root
[`README.md`](md_README.html) remains the overview page for the repository,
while this Doxygen site is the authoritative declaration-level reference for
the current public API.
