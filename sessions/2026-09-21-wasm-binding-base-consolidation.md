# WASM binding base consolidation

- Date: 2026-09-21
- Scope: remove duplicated simple-graph and multigraph Embind adapter logic.
- Decision: use one `GraphBindingBase<NodeT, Directed, Multi>` implementation,
  constrain edge-ID operations to multigraph instantiations, and retain the
  previous internal names as aliases.
- Files: `wasm/include/nxpp_wasm/common/binding_utils.hpp`, `CHANGELOG.md`,
  `SESSIONS.md`, and this record.
- Verification: focused Emscripten translation units, full Node WASM contract,
  TypeScript build, npm-pack consumer, and diff checks.
- Next: review and commit after the full WASM verification.
