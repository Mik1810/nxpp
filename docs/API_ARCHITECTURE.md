# API Architecture Policy

This document defines the intended long-term shape of the public `nxpp` API.

It exists to answer two recurring questions:

1. when functionality should live as methods on `nxpp::Graph`
2. when functionality should live as free functions under `nxpp::`

## Core Position

`nxpp` is a BGL-backed graph wrapper with a NetworkX-inspired surface.

The primary public shape is:

- a stateful graph object for graph ownership, mutation, and local access
- methods on that graph object for most user-facing operations on an existing graph
- free functions only where namespace-scope factory behavior is the cleaner fit

`nxpp` should not try to force every capability into methods, and it should not try to mirror all of NetworkX one-to-one.

## Rule 1: Use graph methods for graph-local operations

Operations belong on `Graph` when they primarily:

- create, mutate, or clear graph state
- read graph-local structural facts
- read or write node/edge attributes
- expose graph-local proxy ergonomics such as `G[u][v]` or `G.node(u)[key]`
- depend on the wrapper's maintained internal state rather than a standalone algorithmic pass

Examples:

- `add_node`
- `add_edge`
- `remove_edge`
- `remove_node`
- `clear`
- `neighbors`
- `successors`
- `predecessors`
- `has_node`
- `has_edge`
- `get_edge_weight`
- `get_node_attr<T>`
- `get_edge_attr<T>`

Attribute-specific note:

- proxy syntax such as `G[u][v]["key"]` and `G.node(u)["key"]` is acceptable
  as part of the graph-local ergonomic surface
- checked accessors such as `get_*_attr<T>` and `try_get_*_attr<T>` should still
  be treated as the primary read-oriented API for robust user code

## Rule 2: Prefer graph methods for algorithms on an existing graph

Operations should prefer methods on `Graph` when they primarily:

- run an algorithm over an already-constructed graph
- compute a derived result rather than mutate graph identity
- are part of the normal user workflow on `G`
- make the API feel closer to the object-oriented usage style users expect from NetworkX-like code

Examples:

- `bfs_edges`
- `dfs_edges`
- `breadth_first_search`
- `depth_first_search`
- `shortest_path`
- `dijkstra_path`
- `dijkstra_shortest_paths`
- `bellman_ford_shortest_paths`
- `connected_components`
- `strongly_connected_components`
- `topological_sort`
- `maximum_flow`
- `minimum_cut`
- `max_flow_min_cost`
- `degree_centrality`

For this project, user-facing coherence now outranks preserving these as namespace-level calls.
That means the long-term primary entry points should be `G.algorithm(...)` for most existing-graph operations.

## Rule 3: Keep generators as free functions

Graph generators should remain free functions that return graph values.

Examples:

- `complete_graph`
- `path_graph`
- `erdos_renyi_graph`

They are better treated as constructors/factories at namespace scope than as methods on an existing graph instance.

## Rule 4: NetworkX-shaped naming is allowed, but it is not the whole design

NetworkX-inspired entry points are still useful, especially when they:

- match the snippet-backed essential surface
- improve discoverability for users coming from Python
- stay thin and unsurprising
- do not force the internals into a Python-first design that hurts the C++ API

But they should be treated as compatibility-friendly wrappers, not as a promise of full NetworkX parity.

That means:

- some names can remain NetworkX-shaped
- some wrappers can forward to a more direct canonical implementation
- aliases should be documented explicitly when they are thin synonyms
- new additions should not be accepted only because NetworkX has them

## Rule 5: nxpp namespace free functions are now narrow in scope

The `nxpp::` namespace should now keep only functions that do not belong to the context of one existing graph object.

Examples:

- `complete_graph`
- `path_graph`
- `erdos_renyi_graph`

The old `nxpp::foo(G, ...)` forms for existing-graph operations are deprecated.

## Rule 6: Additions should pass a placement check

Before adding a new public API surface, ask:

1. Is this primarily graph ownership/mutation/local access?
2. Is this primarily an operation on an already-constructed graph that should therefore be a method?
3. Is it actually a namespace-scope factory/generator instead?
4. Is it a real capability or only an alternate spelling of an existing one?

If the answer is unclear, prefer the narrower surface.

## Practical consequence for the current codebase

The current codebase is already broadly aligned with this policy:

- `Graph` owns mutation, attributes, proxies, and local lookups
- generators already fit naturally as free functions
- existing-graph algorithms now have method-based primary entry points
- remaining namespace-level API should stay focused on non-graph-context functions

So the immediate goal is:

- document the rule clearly
- keep method-based entry points as the primary API
- keep `nxpp::` focused on functions that do not belong to one graph instance
- reduce ambiguity in the public docs about which API surface is primary
