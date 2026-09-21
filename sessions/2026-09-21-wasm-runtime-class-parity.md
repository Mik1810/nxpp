# WASM runtime class parity

- Date: 2026-09-21
- Scope: freeze the shared and graph-family-specific contract of all eight
  Node/WASM facade classes before binding consolidation.
- Decision: use one data-driven contract for method capabilities, component
  directionality, edge orientation, multigraph edge IDs, and concrete
  `subgraph()` return types.
- Files: `wasm/test/node_api_contract/class_parity.test.mjs`, the Node contract
  entrypoint, `CHANGELOG.md`, `SESSIONS.md`, and this record.
- Verification: TypeScript build, Node WASM contract tests, npm-pack consumer,
  and diff checks.
- Next: commit the contract baseline before refactoring shared binding code.
