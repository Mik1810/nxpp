# TODO - Continuation Plan (WASM + TS Facade)

This plan continues the current wasm/ts architecture work with a module-first approach.

## Scope decision

The following items are intentionally excluded for now:

- 6) topological_sort
- 11) generators
- 12) sat

## Current target modules

- 1) graph
- 2) multigraph
- 3) attributes
- 4) traversal
- 5) shortest_paths
- 7) spanning_tree
- 8) components
- 9) centrality
- 10) flow

## Phase 1 - Stabilize graph core contract

- [ ] Freeze API contract for graph + multigraph + attributes at wasm runtime level.
- [ ] Ensure error messages are consistent and explicit for wrong node-id/value types.
- [ ] Add missing node/edge mutation edge-case tests in node contract suite.
- [ ] Revalidate typed runtime classes parity:
  - GraphInt / GraphStr
  - DiGraphInt / DiGraphStr
  - MultiGraphInt / MultiGraphStr
  - MultiDiGraphInt / MultiDiGraphStr

## Phase 2 - Complete TS facade for graph core

- [ ] Ensure all runtime classes are surfaced in facade entrypoint.
- [ ] Ensure interface coverage matches runtime methods for graph core.
- [ ] Add TS compile checks for all graph core classes and generic interfaces.
- [ ] Add one end-to-end TS example for number node IDs and one for string node IDs.

## Phase 3 - Module expansion (remaining active modules)

For each active module (traversal, shortest_paths, spanning_tree, components, centrality, flow):

- [ ] Define minimal public API slice.
- [ ] Implement wasm binding module implementation and registration.
- [ ] Add/extend node contract tests for that slice.
- [ ] Expose typed TS facade methods for the same slice.
- [ ] Add TS usage example and smoke verification.

## Phase 4 - Packaging and release discipline

- [ ] Keep dist artifacts and types aligned after every facade change.
- [ ] Validate npmjs publish path first, then GitHub Packages.
- [ ] Keep README + wasm/README + wasm/WASM synchronized with exported surface.
- [ ] Keep CHANGELOG + RELEASE_NOTES + SESSION synchronized for each closed slice.

## Definition of done for next milestone

- [ ] Graph core contract stable and documented.
- [ ] TS facade stable for graph core with passing checks.
- [ ] At least one additional active module completed end-to-end (runtime + facade + tests + docs).
