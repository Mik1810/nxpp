# AGENTS.md

## Objective

Act as a coding agent inside a university-level computer science project.
Prefer solutions that are simple, readable, correct, and easy to explain to an instructor.

The agent must work with strong context discipline: avoid loading, printing, or preserving unnecessary information. A correct small change with limited context is preferred over a broad exploratory pass that consumes excessive tokens.

## General Rules

- Do not change more than necessary.
- Follow the existing coding style in the repository.
- Avoid unnecessary refactoring.
- Do not introduce new dependencies without clear justification.
- If you make assumptions, state them explicitly.
- If requirements are ambiguous, choose the most conservative option.
- Before starting work on any issue, the assistant must first propose in chat what it plans to do.
- The assistant and user discuss or adjust that proposal first.
- The assistant may start editing files or running implementation steps only after the user explicitly approves with `OK`.
- Before making significant changes, inspect only the files that are directly relevant to the task. Do not read broad parts of the repository just to “understand the structure”.
- Prefer targeted inspection commands such as `rg`, `git diff --stat`, `git status --short`, and narrow `sed -n` ranges.
- Do not cite yourself in commit messages or comments; write as if you are a human developer contributing to the project.

## Token and Context Discipline

- Treat context as a limited resource.
- One session should correspond to one focused task or issue.
- Do not turn a session into a long-running project log covering unrelated issues, release work, merge conflicts, documentation rewrites, and feature work together.
- If the task changes substantially, recommend starting a new session instead of continuing with accumulated context.
- Do not load entire files unless they are small and directly relevant.
- Do not print entire files with `cat` unless the file is tiny, approximately below 120 lines.
- Do not run broad repository searches without output limits.
- Do not dump large JSON objects, GitHub issue bodies, comments, logs, generated files, build directories, or dependency lockfiles into the conversation unless strictly necessary.
- Do not preserve large command outputs in summaries, notes, or session documentation.
- Prefer summaries over raw output.
- Prefer file paths, line numbers, and concise excerpts over full file contents.
- If a command produces more than about 120 lines, rerun or filter it with a narrower query.
- If a command output says `Total output lines` with a large number, treat that as a context failure and narrow the next command.
- Avoid repeatedly reading the same file ranges. Reuse the already known facts unless the file has changed.
- Do not include large diffs in the final response. Summarize the diff and list modified files.
- If context becomes large or the session has been compacted, avoid continuing with unrelated work in the same session.

## Command Rules

- Every terminal command must have a timeout of 30 seconds.
- If a command does not finish within 30 seconds, stop and notify the user of the timeout.
- Prefer commands of this form when output may be non-trivial:

```bash
timeout 30s <command> 2>&1 | tail -120
```

- When preserving exit status through pipes matters, use:

```bash
bash -lc 'set -o pipefail; timeout 30s <command> 2>&1 | tail -120'
```

- Avoid commands that can produce huge output, such as unrestricted `cat`, `rg .`, `find .`, `git log`, `npm test`, `ctest`, or build commands without filtering.
- Prefer bounded commands, for example:

```bash
timeout 30s git status --short
timeout 30s git diff --stat
timeout 30s rg -n "symbol_name" include tests --glob '!build' --glob '!dist' | head -80
timeout 30s sed -n '40,120p' path/to/file
timeout 30s ctest --output-on-failure 2>&1 | tail -120
```

- Do not print generated files, vendored dependencies, package lockfiles, build artifacts, coverage output, or compiled assets unless explicitly requested.

## GitHub and Issue Handling

- Do not fetch all GitHub issue bodies or comments in bulk.
- For issue triage, use compact metadata first:

```bash
timeout 30s gh issue list --limit 20 --json number,title,state,labels
```

- Fetch the body only for a specific issue that is being worked on:

```bash
timeout 30s gh issue view <number> --json number,title,body,labels
```

- Fetch comments only when they are necessary for the current task.
- Do not paste full issue JSON into the conversation.
- Summarize only the relevant issue requirements and cite the issue number in notes or commit messages when useful.

## Style Constraints

- Do not use emojis anywhere in code, comments, commit-style messages, or generated text.
- Do not use separator comments such as `-----`, `=====`, or similar visual dividers.
- Write comments in a natural human style, as if written by a real developer for other developers.
- Keep comments concise, useful, and context-aware.
- Avoid redundant comments that merely restate the code.
- Prefer comments that explain intent, reasoning, assumptions, or non-obvious decisions.

## Code Quality

- Write clean, readable code and comment only where truly needed.
- Use clear and descriptive names for variables, functions, and classes.
- Avoid overly long functions.
- Reduce duplication and unnecessary complexity.
- Do not leave dead code, useless TODOs, or temporary debug code.
- When modifying existing functions, preserve unrelated behavior.
- Start with a minimal correct solution.
- Improve only what is necessary for the task.
- If a change is risky, propose a safer alternative.

## Safety and Robustness

- Validate inputs when appropriate.
- Handle errors and edge cases.
- Do not include secrets, tokens, passwords, or keys in the code.
- Do not modify sensitive configuration files unless strictly necessary.
- Keep npm, GitHub, and registry credentials in user-level configuration files only. Never commit tokens or repository-local auth files.

## Testing and Verification

- After a change, run the smallest relevant verification first.
- Do not run the full test suite automatically if a focused test or compile check is sufficient for the change.
- Run the full relevant test suite when the change affects public behavior, shared infrastructure, release logic, or core APIs.
- If no tests exist, perform minimal verification consistent with the project.
- If adding non-trivial logic, propose or add tests if the project already uses a testing framework.
- Do not claim something works without verifying it.
- Keep test output bounded. Show only the relevant failure, summary, and at most the last 120 lines of output.

## Expected Output

When completing a task:

1. briefly explain what changed;
2. list the modified files;
3. mention any assumptions or limitations;
4. describe how the result was verified;
5. avoid long logs, full diffs, or repeated context.

## Documentation

- If public behavior changes, update the README or the relevant documentation.
- If you add a command, explain how to run it.
- Every time an issue is completed, discuss whether a version bump and release are appropriate.
- `CHANGELOG.md` must be updated every time an issue is completed.
- `CHANGELOG.md` must report versions using semantic versioning in the form `x.y.z`.
- `CHANGELOG.md` is the concise technical history of the project; keep it versioned and compact.
- `RELEASE_NOTES.md` is the richer release-facing document; GitHub release bodies should be derived from it instead of from `CHANGELOG.md`.
- `SESSION.md` must be updated in append-only mode when important work is done.
- `SESSION.md` must stay compact. Do not paste logs, command outputs, large diffs, issue bodies, or file contents into it.
- Each `SESSION.md` update should normally be a short chronological entry with:
  - task or issue number;
  - files touched;
  - main decision;
  - verification performed;
  - next step, if any.
- Prefer concise entries over preserving enough raw context to reconstruct the whole conversation.
- When an issue is finished and the related work is pushed, remember to review whether `README.md`, `CHANGELOG.md`, `RELEASE_NOTES.md`, and `SESSION.md` need updates. Do not modify unrelated documentation automatically.

## Release Process

- A normal push to `main` must not create a GitHub release automatically.
- Releases are driven by `.github/workflows/release.yml`.
- The release workflow may be started from a pushed `vX.Y.Z` tag or from `workflow_dispatch`.
- The top version in `CHANGELOG.md` and `RELEASE_NOTES.md` should be treated as the concrete next release candidate, not as an open-ended future bucket.
- When `workflow_dispatch` is used, the workflow must treat itself as self-contained: it may create and push the tag, but it must also continue in the same run to build, test, and publish the release.
- When the workflow is started from a pushed `vX.Y.Z` tag, it must verify that the pushed tag matches the top documented version before publishing the release.
- The workflow must skip publication when the matching GitHub release already exists.
- Before a release is created, the workflow must verify that the top version in `CHANGELOG.md` matches the top version in `RELEASE_NOTES.md`.

## WASM Package Publish

- The experimental wasm npm package is published from `wasm/`.
- Standard publish flow is `npm run publish:all` from `wasm/`.
- `publish:all` first publishes to npmjs with `publish:npm`, then `postpublish` publishes to GitHub Packages with `publish:github`.
- Keep npm and GitHub registry credentials in user-level `~/.npmrc` only; never commit tokens or repository-local auth files.
- Recommended version bump before publish: `npm version patch --no-git-tag-version` in `wasm/`.

## WASM Development Workflow

- Use this mental model: core C++ in `include/nxpp/` and `tests/`, WASM binding layer in `wasm/include/nxpp_wasm/` and `wasm/src/`, TypeScript facade in `wasm/ts/` to `wasm/dist/`.
- If you touch core C++ behavior, run:

```bash
bash scripts/unix/run_tests.sh
```

- If you touch WASM bindings or the TypeScript facade, run in order:

```bash
bash wasm/scripts/build_wasm_node_module.sh
npm --prefix wasm run build:types
bash wasm/scripts/run_wasm_node_contract_tests.sh
```

- Keep command output bounded with the command rules above.
- Keep smoke examples aligned with the current public surface:
  - `wasm/nxpp_example.js`
  - `wasm/nxpp_example.ts`
- For wasm public API changes, treat these as mandatory checkpoints:
  - runtime build passes;
  - TypeScript facade build passes;
  - Node contract tests pass;
  - docs are updated when relevant: `wasm/README.md` and `wasm/WASM.md`;
  - release/history docs are updated when release-facing: `CHANGELOG.md`, `SESSION.md`, and `RELEASE_NOTES.md`.

## Single Header

- `include/nxpp.hpp` is the canonical umbrella include in the repository.
- `dist/nxpp.hpp` is a generated artifact and must not be versioned in git.
- The generated single-header release asset must come from the tested `dist/nxpp.hpp` output, not from a manually edited or unverified file.
- Validation of the generated single header should rely on the dedicated single-header test suite, not on snippets alone.

## Headers

- Prefer the narrowest semantic header that keeps a file readable, especially in snippets and focused tests.
- Keep `include/nxpp.hpp` for broad showcase code or files that genuinely span multiple semantic areas.

## Benchmarks

- Benchmark runs that generate benchmark CSV files are always user-driven.
- The assistant must not launch benchmark runs autonomously.
- If benchmarks are relevant, explain the command and wait for the user to run it.

## GitHub Actions

- Prefer up-to-date major versions of official GitHub actions such as `actions/checkout` and `actions/setup-python`, especially when GitHub announces runtime deprecations.
- Inspect only the workflow files relevant to the requested change.
- Do not rewrite unrelated workflows.

## What to Avoid

- Do not rewrite entire files unless necessary.
- Do not massively change naming, formatting, or structure without request.
- Do not introduce new libraries, Docker, CI, linters, or formatters without request.
- Do not modify out-of-scope files.
- Do not perform broad repository scans without a clear reason.
- Do not fetch or print all issues, all comments, all logs, or all test output.
- Do not keep working in a bloated session when a new focused session would be cheaper and clearer.

## Work Strategy

- Start with a minimal correct solution.
- Improve only what is necessary for the task.
- Prefer narrow file inspection over broad exploration.
- Prefer small patches over large rewrites.
- If a change is risky, propose a safer alternative.
- If the context becomes too large, stop and recommend continuing in a fresh session with a short handoff summary.
