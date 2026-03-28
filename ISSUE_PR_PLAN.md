# ISSUE_PR_PLAN.md — atomic issues / PRs for `nxpp`

This file lists small, independent, low-overlap tasks that can be opened as GitHub issues or implemented as narrow PRs.

The goal is to avoid one giant "docs cleanup" blob PR.

---

## Suggested sequencing

Recommended order:

1. README cleanup
2. source-of-truth cleanup across markdown files
3. caveat visibility improvements
4. testing-story clarification
5. inline header docs
6. generated docs
7. deeper API / correctness work

---

# Documentation / repo hygiene PRs

## PR 1 — Rewrite `README.md` as the current user-facing source of truth

**Scope**
- Replace the current README with a cleaner structure
- Keep the accurate function-level complexity model
- Move caveats higher
- Add explicit links to repository markdown files
- Refresh the opening example

**Files**
- `README.md`

**Acceptance criteria**
- opening section reflects the current project
- README explicitly states that complexity means public-call complexity
- multigraph caveat appears near the top
- README includes links to `TODO.md`, `CHANGELOG.md`, `SESSION.md`, `docs/README.md`
- README describes snippet parity honestly

---

## PR 2 — Add `NOTES.md` as repository-maintainer documentation

**Scope**
- Add a maintainer-oriented markdown note describing:
  - current drift risks
  - documentation strategy
  - source-of-truth rules
  - future docs-generation plan

**Files**
- `NOTES.md`

**Acceptance criteria**
- notes are internal/maintainer oriented
- notes do not pretend to be user-facing docs
- notes clearly separate current truth from future ideas

---

## PR 3 — Make `docs/README.md` honest and useful

**Scope**
- Replace the placeholder text with a short docs plan
- Explain what will eventually live in `docs/`
- Link back to the root README

**Files**
- `docs/README.md`

**Acceptance criteria**
- file is no longer a vague placeholder
- file states whether docs are manual, generated, or planned
- file links to root README and future docs direction

---

## PR 4 — Add a "source of truth" section to `TODO.md`

**Scope**
- Add a short note at the top saying what `TODO.md` is for
- Ensure it does not duplicate README-style overview material

**Files**
- `TODO.md`

**Acceptance criteria**
- `TODO.md` clearly describes open work only
- no broad user-facing overview language remains if redundant

---

## PR 5 — Trim state drift in `SESSION.md`

**Scope**
- Add a short note near the top explaining that `SESSION.md` is a historical dev log
- Avoid using it as the main current-status document

**Files**
- `SESSION.md`

**Acceptance criteria**
- `SESSION.md` self-identifies as historical context
- it no longer competes with the README as the main public truth

---

## PR 6 — Add a compact repo layout table to the README

**Scope**
- Add / refine a concise table mapping important paths to their role

**Files**
- `README.md`

**Acceptance criteria**
- includes `include/`, `snippet/`, `scripts/`, workflow, `main.cpp`, markdown docs
- layout is concise and accurate

---

## PR 7 — Split README sections for “overview” vs “reference”

**Scope**
- Keep the README usable as an entry page
- Reduce density in the first half
- Keep API detail lower down or behind anchor links

**Files**
- `README.md`

**Acceptance criteria**
- first screen answers what the project is, what it is not, and main caveats
- detailed reference is clearly separated below

---

## PR 8 — Add explicit “testing status” wording

**Scope**
- Clarify that snippet parity is helpful but not a full test suite

**Files**
- `README.md`
- optionally `TODO.md`

**Acceptance criteria**
- README uses wording like “parity / regression harness”
- README does not overstate test maturity
- TODO still tracks the missing formal test suite

---

## PR 9 — Add inline doc comments to public graph methods

**Scope**
- Add brief structured comments above core public methods in `include/nxpp.hpp`

**Files**
- `include/nxpp.hpp`

**Acceptance criteria**
- comments cover purpose, params, returns, and caveats where needed
- comments exist at least for graph mutation, attribute access, and traversal entry points

---

## PR 10 — Add inline doc comments to algorithm wrappers

**Scope**
- Document shortest paths, components, flow wrappers, generators, and utilities in-header

**Files**
- `include/nxpp.hpp`

**Acceptance criteria**
- major free functions are commented
- comments mention when return types are richer than raw Boost output
- comments mention narrow `"weight"` semantics where relevant

---

## PR 11 — Add Doxygen config scaffold

**Scope**
- Add a first documentation generation scaffold
- No need to fully publish a docs site yet

**Files**
- `Doxyfile` or equivalent docs config
- optionally `docs/README.md`

**Acceptance criteria**
- docs can be generated locally from the header comments
- project root documents how to run the docs generator

---

## PR 12 — Add docs-generation instructions

**Scope**
- Explain how to generate docs locally
- Optionally explain future HTML/PDF hosting direction

**Files**
- `docs/README.md`
- optionally `README.md`

**Acceptance criteria**
- a maintainer can generate docs without guesswork
- instructions match the actual tooling committed

---

# Correctness / API issues

## Issue 1 — Redesign multigraph edge identity

**Type**
- design / correctness issue

**Problem**
Public APIs still treat `(u, v)` as an edge handle in places where multigraphs can have parallel edges.

**Files likely touched**
- `include/nxpp.hpp`
- `README.md`
- `TODO.md`
- snippets / tests later

**Acceptance criteria**
- public API can identify one specific parallel edge
- docs explicitly describe multigraph behavior
- ambiguous `(u, v)` behavior is removed or tightly specified

---

## Issue 2 — Specify multigraph behavior for existing public calls

**Type**
- documentation / API issue

**Problem**
The current public behavior of:
- `has_edge`
- `get_edge_weight`
- `get_edge_attr`
- `G[u][v]`
- `remove_edge`

is not precise enough for multigraphs.

**Acceptance criteria**
- each of these calls has documented behavior
- README and header comments match the implementation

---

## Issue 3 — Audit `remove_edge()` metadata cleanup in multigraph cases

**Type**
- correctness issue

**Problem**
`remove_edge()` and edge metadata cleanup need explicit multigraph auditing.

**Acceptance criteria**
- metadata cleanup remains correct after multigraph edge removals
- edge-property cleanup behavior is documented

---

## Issue 4 — Add regression tests for `remove_node()` descriptor remapping

**Type**
- correctness / test issue

**Problem**
`remove_node()` depends on descriptor shifting and map repair.
This deserves dedicated regression coverage.

**Acceptance criteria**
- tests cover removal in the middle of a populated graph
- node IDs remain correct afterward
- subsequent algorithms still work

---

## Issue 5 — Clarify which APIs implicitly create missing nodes

**Type**
- API clarity issue

**Problem**
Some calls create nodes implicitly (`node`, `operator[]`, `add_edge`), while read-style calls do not.

**Acceptance criteria**
- policy is documented
- behavior is consistent or intentionally justified

---

## Issue 6 — Standardize exception wording

**Type**
- API polish issue

**Problem**
Error messages are still mixed in style and specificity.

**Acceptance criteria**
- exceptions follow one consistent wording style
- node-not-found / edge-not-found / wrong-type cases are predictable

---

## Issue 7 — Add compile-time constraints for `NodeID`

**Type**
- API / template clarity issue

**Problem**
The library assumes properties about `NodeID` that are not yet documented or constrained strongly enough.

**Acceptance criteria**
- documentation states expectations clearly
- optional concepts / static assertions enforce them where useful

---

# Testing issues

## Issue 8 — Add a real assertion-based test suite

**Type**
- testing issue

**Problem**
The repo has parity snippets but lacks a focused unit/integration test suite.

**Acceptance criteria**
- dedicated tests exist outside snippets
- tests have assertions, not only output diffs
- tests run in CI

---

## Issue 9 — Add edge-case tests for empty / singleton / disconnected graphs

**Type**
- testing issue

**Acceptance criteria**
- explicit tests cover:
  - empty graphs
  - single-node graphs
  - disconnected graphs
  - unreachable shortest paths

---

## Issue 10 — Add attribute failure tests

**Type**
- testing issue

**Acceptance criteria**
- tests cover:
  - missing node attributes
  - missing edge attributes
  - wrong-type reads
  - optional-return helpers

---

## Issue 11 — Add multigraph-specific tests

**Type**
- testing issue

**Acceptance criteria**
- tests cover:
  - parallel edge insertion
  - read semantics
  - delete semantics
  - attribute behavior under parallel edges

---

## Issue 12 — Add flow-wrapper tests

**Type**
- testing issue

**Acceptance criteria**
- tests cover:
  - capacity attribute handling
  - weight attribute handling
  - min-cost flow wrappers
  - staged `push_relabel_maximum_flow` + `cycle_canceling`

---

# Documentation / usability issues

## Issue 13 — Document all thin aliases explicitly

**Type**
- docs issue

**Problem**
Some API entry points are compatibility aliases to another function.
That is useful, but should be explicit.

**Acceptance criteria**
- README or generated docs clearly mark alias wrappers
- users can see which functions are "main" vs thin aliases

---

## Issue 14 — Add examples for richer result wrappers

**Type**
- docs issue

**Problem**
The strongest ergonomic value of the project is in wrappers like:
- `dijkstra_shortest_paths`
- `MinimumCutResult`
- `lookup_map`

These should be shown more directly.

**Acceptance criteria**
- README or docs include examples using these richer results
- examples are small and realistic

---

## Issue 15 — Add a “utility wrappers beyond NetworkX/BGL” docs section

**Type**
- docs issue

**Problem**
This design strength exists but should be highlighted more prominently.

**Acceptance criteria**
- docs explicitly call out C++-ergonomic helpers
- section includes examples of easier-to-consume return types

---

## Issue 16 — Clarify weighted-overload semantics

**Type**
- docs / API issue

**Problem**
The `"weight"` overload story is intentionally narrow, but easy to misunderstand.

**Acceptance criteria**
- docs explicitly say that named weighted overloads currently refer to the built-in edge weight property
- unsupported weight-key usage is documented honestly

---

# Build / packaging issues

## Issue 17 — Add `CMakeLists.txt`

**Type**
- build issue

**Problem**
The repo still lacks a standard build entry point.

**Acceptance criteria**
- project can be built and demo/test targets are discoverable
- header-only usage is documented

---

## Issue 18 — Document external-usage story

**Type**
- packaging/docs issue

**Problem**
Users need a minimal story for consuming the header externally.

**Acceptance criteria**
- docs explain include path expectations
- docs explain Boost dependency plainly
- docs explain a minimal consumer build example

---

## Issue 19 — Define versioning and release policy

**Type**
- project hygiene issue

**Acceptance criteria**
- versioning policy is written down
- release expectations are documented
- CHANGELOG ownership is clear

---

## Issue 20 — Extend CI beyond the current Linux snippet workflow

**Type**
- CI issue

**Acceptance criteria**
- practical matrix for at least one more platform and/or compiler
- failures remain readable
- snippet review remains intact

---

# Investigation issue

## Issue 21 — Inspect GitHub Actions summary for deprecated Node/runtime warnings

**Type**
- investigation issue

**Problem**
A warning was noted about a deprecated Node runtime in Actions summaries.

**What to do**
- inspect current workflow run summaries
- verify whether the warning still appears
- identify whether it comes from:
  - historical runs
  - a hidden / older action
  - environment/runtime messaging
  - another workflow

**Acceptance criteria**
- issue is either confirmed with source or closed as obsolete
- fix applied only if a real current cause exists

---

# Future-vision issue (not immediate priority)

## Issue 22 — Evaluate portability of the `nxpp` design to other compiled ecosystems

**Type**
- future exploration issue

**Problem**
There is interest in whether the ergonomic layer idea could be reimagined in Rust, Zig, or similar ecosystems.

**Important note**
This is not a direct “port Boost” task.
It would be a new design effort on top of another backend/ecosystem.

**Acceptance criteria**
- issue remains exploratory only
- does not outrank current C++ API, docs, and test priorities

---

## Suggested "first small batch" if time is limited

If only four PRs happen soon, make them:

1. README rewrite
2. NOTES.md
3. docs/README cleanup
4. testing-status clarification

If only four issues are opened soon, open:

1. multigraph edge identity redesign
2. real assertion-based test suite
3. `remove_node()` regression tests
4. inline public API docs + generated docs scaffold
