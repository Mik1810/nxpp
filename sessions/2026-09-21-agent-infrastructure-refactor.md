# Agent infrastructure refactor

- Date: 2026-09-21
- Scope: repository guidance, task-session records, reusable skills, and the
  unreleased changelog.
- Decision: keep `AGENTS.md` focused on policy and project invariants; move
  repeatable issue, C++, WASM, and release procedures into repository skills.
- History: preserve the previous monolithic log unchanged as
  `sessions/legacy-session.md`.
- Files: `AGENTS.md`, `ISSUE.md`, `CHANGELOG.md`, `SESSIONS.md`, `sessions/`,
  and `.agents/skills/`.
- Verification: all four skills passed `quick_validate.py`; release metadata
  remained aligned at `1.4.7`; the archived log matched the original byte for
  byte; Python compilation and `git diff --check` passed.
- Next: inventory the development and release tools in a separate task before
  changing the workstation or npm authentication.
