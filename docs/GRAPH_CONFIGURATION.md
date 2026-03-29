# Graph Configuration Policy

This document defines the supported Boost Graph Library (BGL) configuration surface for `nxpp`.

It exists to answer:

1. which graph-configuration knobs are officially supported today
2. which advanced BGL selectors are intentionally fixed
3. how future configurability may be added without breaking the current API

## Current backend model

`nxpp::Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>` currently builds on:

```cpp
boost::adjacency_list<OutEdgeSelector, VertexSelector, DirectedSelector, ...>
```

with:

- vertex storage selector defaulting to `boost::vecS`
- out-edge storage selector defaulting to `boost::vecS`
- directedness selected via `Directed`, using `boost::bidirectionalS` for directed graphs and `boost::undirectedS` for undirected graphs
- built-in edge weight property enabled/disabled via `Weighted`
- edge index property enabled for internal edge-ID tracking

## Supported public configuration surface

The stable, user-facing configuration knobs are:

1. `NodeID`
2. `EdgeWeight`
3. `Directed`
4. `Multi`
5. `Weighted`
6. `OutEdgeSelector`
7. `VertexSelector`

The existing alias types remain the default presets and still resolve to the standard `boost::vecS` / `boost::vecS` backend.

Example:

```cpp
nxpp::WeightedDiGraphInt default_graph;

nxpp::Graph<int, int, true, false, true, boost::listS> custom_out_edge_graph;
nxpp::Graph<int, int, true, false, true, boost::listS, boost::listS> custom_vertex_graph;
```

## Currently supported selector policy

Selector customization is now broader than the default aliases:

- `OutEdgeSelector` may be customized on `Graph<...>`
- `VertexSelector` may also be customized on `Graph<...>`
- `Multi=true` is intentionally rejected with `boost::setS`

This means the default aliases stay simple, while advanced users can opt into alternate storage selectors directly on the primary template.

## Still not exposed as public knobs

The following BGL-level choices are currently out of scope for direct public customization:

- custom internal property chains for vertex/edge bundles in the core wrapper type
- alternate descriptor-stability models at the public API level
- global `EdgeListS` customization
- arbitrary selector combinations beyond the currently documented constraints

This keeps the wrapper behavior predictable and avoids exposing unstable low-level choices before invariants are documented and tested.

## Why this now works beyond `vecS`

The wrapper no longer relies on "descriptor == index" assumptions.

Instead, `nxpp` now maintains:

- `NodeID <-> vertex_descriptor` translation maps
- a wrapper-owned vertex index map for algorithm result normalization
- rebuilt index/descriptor state after destructive operations such as `remove_node()`

That is what makes non-`vecS` vertex selectors usable while preserving the current public API shape.

## Future extension path (scoped, not implemented)

Future configurability may be added through an explicit advanced configuration layer, for example:

- a dedicated `GraphConfig` type
- an opt-in advanced graph template variant
- clearly documented constraints for each supported selector combination

Any such extension should remain additive and preserve the current default `Graph<...>` behavior.

## Practical policy

- The alias types remain the recommended default entry points.
- Direct `Graph<...>` instantiation is the advanced path when custom selectors are needed.
- The default aliases intentionally stay on `boost::vecS` / `boost::vecS`.
- Any expansion beyond the current selector constraints must ship with explicit invariants and targeted tests.
