# NOTES.md — extended audit and documentation notes for `nxpp`

This file is meant as a working note for repository cleanup, documentation strategy, and issue planning.

It explains:

1. what is already true in the repository today
2. what is still wrong or drifting
3. what should change in `README.md`
4. what should move out of `README.md`
5. what can become atomic issues / PRs

---

## Executive summary

`nxpp` is already stronger technically than an old first impression would suggest.

The repository today clearly contains:

- a real single-header public surface in `include/nxpp.hpp`
- a broad wrapper layer over Boost
- a snippet-based parity / regression harness
- a current `TODO.md`
- a current `CHANGELOG.md`
- a `LICENSE`
- a user-facing `README.md`
- a placeholder `docs/README.md`

So the repository is **not** "missing documentation entirely".

The real problem is different:

- the repository still suffers from **documentation drift**
- the current `README.md` is trying to do too many jobs at once
- the strongest caveats are not emphasized early enough
- the distinction between **parity harness** and **formal tests** is still too weak
- the reference is richer now, but the landing page is too heavy

---

## What is true today

### 1. The repo is no longer just a toy wrapper

The core header contains a lot more than a handful of simple graph calls.

It includes:

- graph wrappers and aliases
- proxy-based attribute syntax
- checked attribute accessors
- traversal wrappers
- weighted and unweighted path helpers
- components and SCC helpers
- MST helpers
- flow, min-cut, and min-cost flow wrappers
- generators
- utility helpers such as `degree_centrality`, `lookup_map`, `visitor`, and `two_sat_satisfiable`

That means any README that still presents the project as "just a tiny NetworkX-like toy layer" is outdated.

### 2. The README already improved compared with the likely older state

Some older notes are still directionally correct, but they are no longer literally true.

For example:

- there **already is** a function-by-function complexity reference for many public calls
- the README **already does** explain that complexity refers to the full public `nxpp` call
- the README **already does** mention utility functions that are not just strict NetworkX/BGL ports
- the README **already does** list many aliases, not just a handful

So the current task is not "start from zero".
It is "restructure, tighten, and make the docs source-of-truth cleaner".

### 3. The repo has a visible snippet-based verification layer

This matters because it is one of the most positive repository signals.

The repo contains:

- `snippet/`
- `scripts/test_single_snippet.sh`
- `scripts/log_snippet_folder.sh`
- `.github/workflows/snippet-review.yml`

This is enough to say there is a real parity/regression story.

But it is not enough to say there is a mature test suite.

That distinction matters.

---

## What is still wrong

### 1. `README.md` is doing too many jobs

Right now the README is simultaneously trying to be:

- landing page
- architecture note
- caveat note
- API manual
- status dashboard
- repository map

That makes it heavy and harder to trust as the first page.

A good split should be:

- `README.md` = concise but accurate overview + caveats + links
- generated docs / API docs = detailed reference
- `TODO.md` = open work
- `CHANGELOG.md` = history of changes
- `SESSION.md` = historical log

### 2. The source-of-truth story is still muddy

The repository currently has several markdown files describing state.

That is useful, but only if each one has a clean role.

Right now the risk is:

- the README talks about current state
- `TODO.md` also talks about current state
- `SESSION.md` also contains state-ish descriptions
- `docs/README.md` promises future docs
- older references to `ROADMAP.md` may still exist conceptually even if that file is no longer part of the visible root

This makes drift likely.

### 3. The biggest caveat is not presented early enough

The multigraph problem is the highest-risk area of the public API.

The current implementation still exposes several multigraph-relevant operations through `(u, v)`-style access, but `(u, v)` does not stably identify one parallel edge.

That affects user expectations around:

- `has_edge(u, v)`
- `get_edge_weight(u, v)`
- `get_edge_attr(u, v, key)`
- `G[u][v]`
- `remove_edge(u, v)`

This caveat should be up top, not buried.

### 4. The testing story is still easy to overstate

The repo has a useful snippet harness.
That is good.
But it is not the same thing as:

- unit tests with assertions
- property-based tests
- edge-case coverage
- regression tests targeted at specific invariants

The README should avoid sounding like parity harness == full test suite.

### 5. `main.cpp` and `main.py` are not a clean mirrored spec anymore

`main.cpp` now exercises much more of the current feature surface.

`main.py` looks older and covers a smaller / earlier slice.

That is fine, but those files should be described honestly as demo / smoke scripts rather than as a formal mirrored spec of the whole project.

### 6. `docs/README.md` is still a placeholder

That is not bad by itself.
But it means the README should not imply that a full docs site already exists.

Instead, it should explicitly say:

- full docs are planned
- the current README is the main user-facing manual
- generated docs or a docs site are still future work

---

## README problems to fix specifically

### A. The opening example was not strong enough anymore

The old opening example style was too small and too early-phase compared with the current project.

A better opening example should show:

- graph mutation
- attribute access
- one of the richer result wrappers

Good candidates:

- `dijkstra_shortest_paths`
- `minimum_cut`
- checked accessors such as `get_edge_attr<T>`

### B. The project docs links needed to be explicit

The README should link repository docs clearly:

- `README.md`
- `TODO.md`
- `CHANGELOG.md`
- `SESSION.md`
- `docs/README.md`

Not as throwaway mentions, but as a small "Project documents" section.

### C. The repository layout needed a real section

A concise path table helps immediately:

- public header
- snippet harness
- scripts
- workflow
- demo files
- docs files

### D. The "utility value" of the project needed stronger emphasis

One of the best things about `nxpp` is that it is not only imitating upstream APIs.

It also returns shapes that are easier to use in C++.

That includes:

- `SingleSourceShortestPathResult`
- `MaximumFlowResult`
- `MinimumCutResult`
- `lookup_map`
- grouped component helpers
- checked attribute accessors

This should be presented as a deliberate design strength.

### E. The complexity policy needed to stay explicit

This point must stay visible because it is one of the most important documentation decisions in the project.

The docs should keep saying:

> complexity means the full public `nxpp` call, not just the wrapped algorithm

That includes internal translation maps, cleanup work, and result materialization.

---

## Why the complexity angle matters so much

This repository is not documenting bare textbook algorithms.
It is documenting a wrapper layer.

That wrapper layer does meaningful extra work around the algorithm core.

Examples:

- `remove_node()` does metadata cleanup and mapping repair
- attribute access performs `std::any` lookups and casts
- many helpers materialize vectors/maps instead of streaming iterators
- some flow wrappers rebuild an auxiliary graph
- some path helpers reconstruct and materialize full paths

So the correct documentation unit is **function complexity**, not just algorithm complexity.

This is exactly the right instinct and should remain a central documentation rule.

---

## What should probably move out of the README later

The current README can stay detailed for now, but the long-term direction should be:

- short / medium README
- generated API docs elsewhere

The best long-term home for:

- every overload
- every parameter
- every return type
- every alias wrapper
- every small utility

is generated documentation from inline code comments.

That can later become:

- Doxygen HTML
- Doxygen + LaTeX/PDF
- a docs site generated from those comments

The important point is not the tool name.
The important point is to stop maintaining the full API reference manually in only one giant README forever.

---

## Documentation strategy that makes sense

### Phase 1: keep one strong README
Do this now.

- accurate landing page
- accurate caveats
- accurate function reference
- clean links to other md files

### Phase 2: annotate the public header
Add inline comments for public API in `include/nxpp.hpp`.

That gives:

- editor hover help
- clearer public intent
- a base for generated docs later

### Phase 3: generate docs
Once inline docs exist, generate:

- HTML API docs
- optionally PDF / LaTeX docs
- optionally GitHub Pages

That would let the README get shorter without losing the function-by-function reference.

---

## Notes on repository truth vs aspiration

The repo should distinguish clearly between:

### What already exists
- header-only library
- Boost dependency
- parity harness
- GitHub Actions snippet workflow
- multiple algorithm wrappers
- MIT license

### What is planned but not done
- real test suite
- fuller docs site
- CMake packaging story
- release/versioning story
- multigraph redesign
- broader CI matrix

This distinction matters for trust.
The README should describe the first group clearly, and the second group honestly.

---

## Future ideas that are interesting but not immediate priority

### Porting the idea to Rust / Zig / other compiled languages

This is an interesting idea at the "project vision" level.

But it should not distract from the current priorities.

Why?

Because a Rust or Zig version would not be "porting Boost".
It would effectively be:

- a reimplementation of the `nxpp` ergonomic layer
- on top of a different graph backend
- with a different type system, ownership model, and ecosystem

That is a separate project.

So it belongs in "future ideas", not in current core issues.

---

## Notes on the GitHub Actions "node deprecated" warning

This needs careful wording.

From the visible workflow file, the repository currently uses:

- `actions/checkout@v4`
- `actions/setup-python@v5`

Those are modern action versions.

So if a warning like "Node deprecated" appears in a run summary, possibilities include:

- an old historical workflow run
- a warning from a dependency internal to a run environment
- a workflow file that is not in the visible path examined here
- cached or older metadata from a previous action runtime change

Conclusion:

- it is worth checking
- but it should be investigated from actual run logs / summary, not assumed from the current YAML alone

That means it is a valid note, but lower-confidence than the documentation issues above.

---

## What is most worth fixing first

If only a few things are done next, they should be:

1. clean and accurate README
2. explicit source-of-truth split among markdown files
3. multigraph caveat moved high in the docs
4. clear statement that snippet parity is not the same as a full test suite
5. inline public API comments in the header
6. generated docs after that

These six steps would improve trust and usability more than adding another feature right now.

---

## Recommended source-of-truth policy

This is the cleanest repository policy:

### `README.md`
Current user-facing truth.

Should answer:

- what the library is
- what it is not
- what works well
- what is risky
- how to build it
- what the main public API looks like
- where to find more project files

### `TODO.md`
Open work only.

Should answer:

- what still needs to be fixed / designed / added

It should not try to re-explain the current whole product state.

### `CHANGELOG.md`
Historical changes only.

### `SESSION.md`
Historical development notes only.

Useful for author context, not for first-time user truth.

### `docs/`
Generated docs and long-form docs later.

---

## Summary verdict

The repository is in a better place technically than a stale README would suggest.

The real cleanup is not "make it sound cooler".
It is:

- reduce drift
- separate roles
- emphasize caveats honestly
- keep the function-complexity mindset
- move toward generated API docs

That is the right next step for this project.
