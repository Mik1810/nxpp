# Flow-capacity refactor

- Date: 2026-09-21
- Scope: isolate flow-only capacity parsing from the core graph header.
- Decision: keep the private graph declaration in `graph.hpp` and move its
  implementation and numeric validation helpers to `flow.hpp`.
- Files: `include/nxpp/graph.hpp`, `include/nxpp/flow.hpp`, `CHANGELOG.md`,
  `SESSIONS.md`, and this record.
- Verification: focused flow tests, standalone `graph.hpp` compilation, full
  native tests, external consumer tests, and diff checks.
- Next: review and commit the focused refactor.
