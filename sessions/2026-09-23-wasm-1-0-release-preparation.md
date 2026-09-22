# WASM 1.0 release preparation — 2026-09-23

## Scope

Prepare the independent `@mik1810/nxpp-wasm@1.0.0` package candidate after
the completed architecture roadmap audit. No release tag or publication is
part of this task.

## Decisions

- Keep the WASM package's version and release notes separate from the native
  C++ release series.
- Document the intentional 0.6-to-1.0 API break, supported Node majors, and
  browser boundary without claiming that the candidate is published.
- Keep the runtime and generated facade artifacts unchanged; this task changes
  version metadata and release documentation only.

## Changed files

- `wasm/package.json`, `wasm/package-lock.json`
- `wasm/RELEASE_NOTES.md`, `wasm/README.md`, `wasm/WASM.md`,
  `wasm/ARCHITECTURE.md`
- `CHANGELOG.md`, `SESSIONS.md`, this record

## Verification

- `npm --prefix wasm ci`: passed.
- `npm --prefix wasm run build:types`: passed.
- `npm --prefix wasm run check:raw-contract`: passed.
- Node contract and npm-pack consumer scripts with build skipped: passed.
- `npm pack --dry-run --json` from `wasm/`: version `1.0.0` and intended
  package files confirmed.
- Generated facade, raw declarations, and runtime artifacts: no diff.
- `git diff --check`: passed.
- npmjs lookup for `@mik1810/nxpp-wasm@1.0.0`: no published version found.

## Next action

Review and push the candidate commit when explicitly authorized, then require
the full CI gates. Decide separately whether to create the `wasm-v1.0.0` tag;
that tag activates the release workflow and registry publication.
