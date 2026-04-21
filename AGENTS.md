# AGENTS.md

## Objective
Act as a coding agent inside a university-level computer science project.
Prefer solutions that are simple, readable, correct, and easy to explain to an instructor.

## General Rules
- Before making significant changes, read the main project files and understand the structure.
- Do not cite yourself in commit messages or comments; write as if you are a human developer contributing to the project.
- Do not change more than necessary.
- Follow the existing coding style in the repository.
- Avoid unnecessary refactoring.
- Do not introduce new dependencies without clear justification.
- If you make assumptions, state them explicitly.
- If requirements are ambiguous, choose the most conservative option.
- Before starting work on any issue, the assistant must first propose in chat what it plans to do.
- The assistant and user discuss/adjust that proposal first.
- The assistant may start editing files or running implementation steps only after the user explicitly approves (`OK`).
- If the agents runs a command in terminal, it have to set a timout of 30 seconds, and if the command does not finish within that time, it should exit notifying the user of the timeout.

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

## Safety and Robustness
- Validate inputs when appropriate.
- Handle errors and edge cases.
- Do not include secrets, tokens, passwords, or keys in the code.
- Do not modify sensitive configuration files unless strictly necessary.

## Testing and Verification
- After every change, run existing tests if available.
- If no tests exist, perform at least minimal verification consistent with the project.
- If adding non-trivial logic, propose or add tests if the project already uses a testing framework.
- Do not claim something works without verifying it.

## Expected Output
When completing a task:
1. briefly explain what you changed;
2. list the modified files;
3. mention any assumptions or limitations;
4. describe how you verified the result.

## Documentation
- Every time we close an issue, we must discuss about the version and eventually the release in GitHhub, and update the `CHANGELOG.md` file accordingly.
- If you change public behavior, update the README or relevant comments.
- If you add a command, explain how to run it.
- `CHANGELOG.md` must be updated every time an issue is completed, it must report the version compliant to the semantic versioning standard x.y.z, update the version when you think it's appropriate.
- `CHANGELOG.md` is the concise technical history of the project; keep it versioned and relatively compact.
- `RELEASE_NOTES.md` is the richer release-facing document; GitHub release bodies should be derived from it instead of from `CHANGELOG.md`.
- `SESSION.md` must always be updated in append-only mode.
- `SESSION.md` must be updated every time important work is done, so it is expected to change more often than `CHANGELOG.md`.
- The purpose of `SESSION.md` is to preserve enough chronological context for the model to resume work reliably in later sessions.
- Every time an issue is finished and the related work is pushed, the assistant must always remember to update `README.md`, `CHANGELOG.md`, and `SESSION.md`.

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
- `publish:all` first publishes to npmjs (`publish:npm`), then `postpublish` publishes to GitHub Packages (`publish:github`).
- Keep npm and GitHub registry credentials in user-level `~/.npmrc` only; never commit tokens or repository-local auth files.
- Recommended version bump before publish: `npm version patch --no-git-tag-version` in `wasm/`.

## WASM Development Workflow
- Use this mental model: core C++ (`include/nxpp/` + `tests/`), WASM binding layer (`wasm/include/nxpp_wasm/` + `wasm/src/`), TypeScript facade (`wasm/ts/` -> `wasm/dist/`).
- If you touch core C++ behavior, run `bash scripts/unix/run_tests.sh`.
- If you touch WASM bindings or TS facade, run in order:
	- `bash wasm/scripts/build_wasm_node_module.sh`
	- `npm --prefix wasm run build:types`
	- `bash wasm/scripts/run_wasm_node_contract_tests.sh`
- Keep smoke examples aligned with the current public surface:
	- `wasm/nxpp_example.js`
	- `wasm/nxpp_example.ts`
- For wasm public API changes, treat these as mandatory checkpoints:
	- runtime build passes
	- TS facade build passes
	- Node contract tests pass
	- docs are updated (`wasm/README.md` and `wasm/WASM.md` when relevant)
	- release/history docs are updated (`CHANGELOG.md`, `SESSION.md`, and `RELEASE_NOTES.md` when release-facing)

## Single Header
- `include/nxpp.hpp` is the canonical umbrella include in the repository.
- `dist/nxpp.hpp` is a generated artifact and must not be versioned in git.
- The generated single-header release asset must come from the tested `dist/nxpp.hpp` output, not from a manually edited or unverified file.
- Validation of the generated single header should rely on the dedicated single-header test suite, not on snippets alone.

## Headers
- Prefer the narrowest semantic header that keeps a file readable, especially in snippets and focused tests.
- Keep `include/nxpp.hpp` for broad showcase code or files that genuinely span multiple semantic areas.

- Compile benchmark runs that generate benchmark CSV files are always user-driven: the assistant must not launch them autonomously and should wait for the user to run them.

## GitHub Actions
- Prefer up-to-date major versions of official GitHub actions such as `actions/checkout` and `actions/setup-python`, especially when GitHub announces runtime deprecations.

## What to Avoid
- Do not rewrite entire files unless necessary.
- Do not massively change naming, formatting, or structure without request.
- Do not introduce new libraries, Docker, CI, linters, or formatters without request.
- Do not modify out-of-scope files.

## Work Strategy
- Start with a minimal correct solution.
- Improve only what is necessary for the task.
- If a change is risky, propose a safer alternative.
