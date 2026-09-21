---
name: nxpp-issue-workflow
description: Plan and carry out work on one nxpp GitHub issue, including bounded triage, approval, verification, and a compact per-task session record. Use only for a selected issue, not broad backlog analysis.
---

# nxpp issue workflow

Work on one issue at a time.

## Before approval

1. Read `AGENTS.md`, `git status --short`, and `git diff --stat`.
2. Fetch only the selected issue with
   `gh issue view <number> --json number,title,body,labels`.
3. Summarize the requirements without reproducing the full issue body.
4. Name at most five files to inspect.
5. Propose the smallest implementation and verification plan, then wait for an
   explicit `OK`.

Do not fetch unrelated issue bodies or comments. Use issue-list metadata only
when the user explicitly asks for triage.

## After approval

Inspect narrow ranges, make the smallest patch, and use the relevant C++ or
WASM skill for implementation checks. Preserve unrelated work.

Create `sessions/YYYY-MM-DD-issue-N.md` with:

- scope and acceptance criteria;
- important decisions;
- changed files;
- verification commands and outcomes;
- next action.

Add the record to `SESSIONS.md`. Do not store raw logs, diffs, or the issue
body. Close or mutate the GitHub issue only when the user has authorized that
external action and the acceptance criteria are satisfied.
