---
name: nxpp-cpp-change
description: Implement or review changes to nxpp native C++ headers, graph algorithms, tests, CMake consumption, or C++ packaging while preserving the header-only public contract.
---

# nxpp C++ change

Treat `include/nxpp/` as the native source of truth and `include/nxpp.hpp` as
the canonical umbrella include.

## Boundaries

- Preserve C++20 and Boost 1.86 minimum support.
- Keep the library header-only and the `nxpp::nxpp` CMake interface target
  coherent.
- Do not edit generated `dist/nxpp.hpp` or commit it.
- Preserve documented graph semantics, complexity, exception behavior, and
  compatibility aliases unless the approved task explicitly changes them.
- When public API changes are intentional, update compatibility coverage and
  user documentation and assess SemVer impact.

## Verification

Start with the focused test target for the touched behavior. For core or public
API changes, run or ask the user to run:

```bash
bash scripts/unix/run_tests.sh
```

Use the single-header and external-consumer checks when the umbrella header,
packaging, CMake export, or public include behavior changes. Run
`git diff --check` before handoff. Record outcomes, not logs, in the task
session file.
