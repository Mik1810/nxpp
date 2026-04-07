# nxpp API Reference

This generated reference complements the hand-written markdown guides in
[`docs/README.md`](README.md).

Use the generated pages for:

- public class and function signatures
- inline API documentation taken directly from the headers
- namespace, type, and member navigation

Use the markdown guides for:

- overview material
- design notes
- complexity policy
- testing and release process details

The main public entry points are:

- [`include/nxpp.hpp`](../include/nxpp.hpp): umbrella include for the whole library
- [`include/nxpp/graph.hpp`](../include/nxpp/graph.hpp): core graph type and aliases
- [`include/nxpp/traversal.hpp`](../include/nxpp/traversal.hpp): traversal helpers
- [`include/nxpp/shortest_paths.hpp`](../include/nxpp/shortest_paths.hpp): shortest-path helpers and result types
- [`include/nxpp/flow.hpp`](../include/nxpp/flow.hpp): flow, cut, and min-cost helpers
- [`include/nxpp/generators.hpp`](../include/nxpp/generators.hpp): graph generators

The generated site is intentionally minimal for now. The first goal is to make
the public headers readable in IDE hovers and to provide a navigable local
reference that can later grow into a richer hosted docs site.
