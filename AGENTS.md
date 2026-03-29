# AGENTS.md

## Objective
Act as a coding agent inside a university-level computer science project.
Prefer solutions that are simple, readable, correct, and easy to explain to an instructor.

## General Rules
- Before making significant changes, read the main project files and understand the structure.
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
- If you change public behavior, update the README or relevant comments.
- If you add a command, explain how to run it.
- `CHANGELOG.md` must be updated every time an issue is completed, it must report the version compliant to the semantic versioning standard x.y.z, update the version when you think it's appropriate.
- `SESSION.md` must always be updated in append-only mode.
- `SESSION.md` must be updated every time important work is done, so it is expected to change more often than `CHANGELOG.md`.
- The purpose of `SESSION.md` is to preserve enough chronological context for the model to resume work reliably in later sessions.
- Every time an issue is finished and the related work is pushed, the assistant must always remember to update `README.md`, `CHANGELOG.md`, and `SESSION.md`.

## What to Avoid
- Do not rewrite entire files unless necessary.
- Do not massively change naming, formatting, or structure without request.
- Do not introduce new libraries, Docker, CI, linters, or formatters without request.
- Do not modify out-of-scope files.

## Work Strategy
- Start with a minimal correct solution.
- Improve only what is necessary for the task.
- If a change is risky, propose a safer alternative.
