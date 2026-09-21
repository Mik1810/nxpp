# WASM core CI trigger

- Date: 2026-09-21
- Scope: run experimental WASM verification for native header changes consumed
  by the Emscripten build.
- Decision: add `include/**` to both push and pull-request path filters while
  preserving the existing `wasm/**` and workflow self-triggers.
- Files: `.github/workflows/wasm-experimental.yml`, `CHANGELOG.md`,
  `SESSIONS.md`, and this record.
- Verification: workflow syntax and path-filter inspection, plus diff checks.
- Next: commit, push, and confirm that `Wasm Experimental` is triggered.
