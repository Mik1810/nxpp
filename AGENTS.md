# AGENTS.md

## Mission

Act as a coding agent for a university-level C++20 and WebAssembly project.
Prefer small, correct, readable changes with explicit verification and concise
handoffs.

## Authorization and Scope

- Before editing, state what you understood, the files you propose to inspect,
  the minimal implementation plan, and the verification plan. Wait for an
  explicit `OK`.
- Keep one focused task per session. Treat an unrelated task as a new session.
- Preserve existing style and unrelated behavior. Do not add dependencies,
  tooling, broad refactors, Docker, linters, or formatters without approval.
- Never include secrets, tokens, passwords, registry credentials, or repo-local
  authentication files.
- Do not mention coding agents or AI tools in commits, comments, documentation,
  or generated project text.
- Do not use emojis or decorative separators.

## Context and Commands

- Inspect only files relevant to the selected task. Prefer `rg`,
  `git status --short`, `git diff --stat`, and narrow `sed -n` ranges.
- Keep command output bounded, normally below 120 lines. Do not dump complete
  logs, issue collections, generated files, dependency trees, or full diffs.
- Every terminal command must use a 30-second timeout.
- If a long-running command is still active after one poll, stop and ask the
  user to run it manually.
- Do not read session history by default. Read only the specific session file
  needed to resume explicitly requested work.
- Preserve uncommitted and unrelated user work.

## Repository Workflows

Use the repository skills in `.agents/skills/` when their descriptions match:

- `nxpp-issue-workflow` for selecting, planning, recording, and closing an issue.
- `nxpp-cpp-change` for native C++ headers, algorithms, tests, and packaging.
- `nxpp-wasm-change` for Embind, Emscripten, TypeScript, npm, and browser work.
- `nxpp-release-workflow` for versions, changelog, release notes, and publishing.

When a task crosses C++ and WASM, use both change skills but keep the C++
library as the algorithmic source of truth.

## Implementation Quality

- Use clear names, simple control flow, and reasonably small functions.
- Remove dead or debugging code.
- Comment only non-obvious intent, assumptions, or tradeoffs.
- Validate inputs and preserve documented error behavior.
- Update public documentation when public behavior changes.

## Verification

- Run the smallest relevant check first.
- Run broader tests only for changes to public APIs, shared infrastructure,
  release behavior, or cross-module contracts.
- Keep verification output out of committed documentation and session files;
  record only commands and outcomes.
- Prefer asking the user to run long Emscripten, package publication, release,
  or benchmark commands.

## Session Records

- `SESSIONS.md` is the compact index of task records.
- Store one record per task under `sessions/` using
  `YYYY-MM-DD-issue-N.md` or `YYYY-MM-DD-task-slug.md`.
- A record contains scope, decisions, changed files, verification, and the next
  action. Do not store raw logs, full diffs, issue bodies, or repeated project
  documentation.
- `sessions/legacy-session.md` is a frozen historical archive. Do not append to
  it or use it as current guidance.

## Project Invariants

- `include/nxpp.hpp` is the canonical umbrella include.
- `dist/nxpp.hpp` is generated and must not be versioned.
- The stable native library is header-only C++20 and requires Boost 1.86 or
  newer.
- The experimental npm package lives under `wasm/`; its release version and
  stability contract are independent from the C++ release.
- Keep registry credentials in user-level configuration only.
- Releases are driven by `.github/workflows/release.yml`; a normal push to
  `main` must not publish a GitHub release.
- `CHANGELOG.md` is concise technical history. `RELEASE_NOTES.md` is updated
  when preparing a declared release and must not mirror every intermediate
  change automatically.
- Benchmark runs that generate result CSV files are user-driven only.
- Prefer current major versions of official GitHub Actions when editing
  workflows.
