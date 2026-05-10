# AGENTS.md

## Mission

Act as a coding agent for a university-level C++/WASM project.
Prefer small, correct, readable changes. Optimize for clarity, maintainability, and low context usage.

## Hard Rules

- Do not edit before proposing a short plan and receiving explicit `OK`.
- Do not change more than necessary.
- Preserve existing style and unrelated behavior.
- Do not introduce dependencies, broad refactors, new tooling, Docker, linters, or formatters without approval.
- Do not cite AI tools or yourself in commits, comments, docs, or generated text.
- No emojis. No decorative separators.
- No secrets, tokens, passwords, or repo-local auth files.

## Context Budget

- One focused task per session. New task or unrelated issue means new session.
- Inspect only files directly relevant to the selected task or issue.
- Prefer `rg`, `git status --short`, `git diff --stat`, and narrow `sed -n` ranges.
- Do not read or print whole files unless they are small and directly relevant.
- Do not read `SESSION.md` by default. Read only the last 15 lines when the user explicitly asks to resume prior work.
- Do not dump large JSON, logs, issue bodies, comments, generated files, build dirs, lockfiles, dependency trees, or full diffs.
- Never preserve raw logs, large diffs, issue bodies, or command output in `SESSION.md` or final replies.
- If output exceeds 120 lines or says `Total output lines`, narrow the next command.
- If context grows large, finish only the current micro-task, produce a short handoff, and stop.

## Shell Rules

- Every terminal command must use a 30s timeout.
- Keep output bounded, usually <=120 lines.
- Do not repeatedly poll long-running commands.

Use this pattern when output may be large:

```bash
timeout 30s <command> 2>&1 | tail -120
```

Use this when pipe exit status matters:

```bash
bash -lc 'set -o pipefail; timeout 30s <command> 2>&1 | tail -120'
```

Safe first commands:

```bash
timeout 30s git status --short
timeout 30s git diff --stat
timeout 30s rg -n "symbol" include tests --glob '!build' --glob '!dist' | head -80
timeout 30s sed -n '40,120p' path/to/file
```

Use only when explicitly resuming prior work:

```bash
timeout 30s tail -15 SESSION.md
```

Avoid unrestricted `cat`, `find .`, `rg .`, `git log`, full test logs, generated files, build artifacts, and vendored/dependency output.

## Long-Running Commands

- If a command returns `Process running`, poll at most once.
- If it is still running, stop and ask the user to run it manually.
- Do not repeatedly poll builds, test suites, Emscripten commands, package installs, release commands, or network commands.
- For WASM builds and contract tests, prefer giving the command to the user.
- When the user provides command output, use only the final error, exit code, and last 80 lines.

## GitHub Issues

- Triage with metadata only:

```bash
timeout 30s gh issue list --limit 20 --json number,title,state,labels
```

- For work, fetch only the selected issue:

```bash
timeout 30s gh issue view <number> --json number,title,body,labels
```

- Do not fetch all bodies or comments in bulk.
- Do not paste full issue JSON. Summarize only requirements relevant to the selected issue.
- For WASM or large refactor issues, handle one issue per session.

## Work Flow

Before editing, provide only:
1. what you understood;
2. files you propose to inspect;
3. minimal implementation plan;
4. verification plan.

After approval:
1. inspect narrow file ranges;
2. make the smallest patch;
3. run the smallest relevant verification;
4. update docs/history only when required.

Final response:
- changed files;
- what changed;
- verification;
- assumptions/limits;
- no long logs or full diffs.

## Code Quality

- Use clear names and simple control flow.
- Keep functions reasonably small.
- Remove dead/debug code.
- Comment only non-obvious intent, assumptions, or tradeoffs.
- Validate inputs and handle edge cases when appropriate.

## Testing

- Run focused checks first.
- Full tests only when the change affects core APIs, public behavior, CI/release logic, or shared infrastructure.
- If touching core C++ behavior, run or propose:

```bash
bash scripts/unix/run_tests.sh
```

- If touching WASM bindings or TypeScript facade, prefer asking the user to run:

```bash
bash wasm/scripts/build_wasm_node_module.sh
npm --prefix wasm run build:types
bash wasm/scripts/run_wasm_node_contract_tests.sh
```

- Keep test output bounded with the shell rules.

## Documentation and History

- Update README or relevant docs when public behavior changes.
- When an issue is completed, discuss whether version/release notes are needed.
- `CHANGELOG.md`: concise versioned technical history, SemVer `x.y.z`.
- `RELEASE_NOTES.md`: richer release-facing notes accumulated since the previous published release; do not mirror every changelog version unless the user starts release preparation.
- `SESSION.md`: append-only, compact, no logs/diffs/bodies.

Preferred `SESSION.md` entry:

```md
### YYYY-MM-DD - Issue #N

- Branch/commit:
- Files:
- Decision:
- Verified:
- Next:
```

Do not modify unrelated docs automatically.

## Project Notes

- Releases are driven by `.github/workflows/release.yml`; normal push to `main` must not publish a GitHub release.
- `CHANGELOG.md` remains versioned history for each completed change; `RELEASE_NOTES.md` remains an unreleased aggregate until the user declares the next release version.
- WASM package lives in `wasm/`; publish flow is `npm run publish:all` from `wasm/`.
- Keep registry credentials in user-level `~/.npmrc` only.
- `include/nxpp.hpp` is the canonical umbrella include.
- `dist/nxpp.hpp` is generated and must not be versioned.
- Benchmark runs that generate benchmark CSV files are user-driven only.
- Prefer current major versions of official GitHub Actions when editing workflows.
