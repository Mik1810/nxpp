# WASM facade base consolidation

- Date: 2026-09-21
- Scope: remove duplicated simple-graph and multigraph TypeScript facade logic.
- Decision: use one generic `BaseGraph` for shared validation, lifecycle,
  algorithms, and mutation tracking; retain only edge-ID operations in the
  multigraph-specific base.
- Files: `wasm/ts/core/graph.ts`, `wasm/ts/core/multigraph.ts`, generated
  `wasm/dist/core` outputs, `CHANGELOG.md`, `SESSIONS.md`, and this record.
- Verification: TypeScript build, Node WASM contract with the existing binary,
  npm-pack consumer test, and diff checks.
- Next: commit and verify repository CI.
