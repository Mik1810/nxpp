# Min-cost-flow state registry

- Date: 2026-09-21
- Scope: centralize staged min-cost-flow state ownership and synchronization.
- Decision: preserve the pointer-keyed cache and current locking semantics,
  while exposing stage, invalidate, clear, and guarded-access operations from
  one internal registry.
- Files: `include/nxpp/flow.hpp`, `CHANGELOG.md`, `SESSIONS.md`, and this record.
- Verification: focused GCC and Clang flow tests, full native tests, external
  consumer tests, Node WASM contract tests, and diff checks.
- Next: review and commit the focused refactor.
