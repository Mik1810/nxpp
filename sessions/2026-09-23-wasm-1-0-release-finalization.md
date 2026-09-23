# WASM 1.0 release finalization — 2026-09-23

## Scope

Close the independent `@mik1810/nxpp-wasm@1.0.0` release after registry
approval and publish a GitHub Release for the existing `wasm-v1.0.0` tag.

## Decisions

- Keep the WASM GitHub Release distinct from the native C++ `v*` series and
  leave the native release marked as the repository's latest.
- Preserve the preparation record as a historical snapshot; current release
  status belongs in package documentation and this finalization record.
- Do not move the existing release tag or republish the package.

## Changed files

- `wasm/RELEASE_NOTES.md`, `wasm/README.md`, `wasm/ARCHITECTURE.md`
- `CHANGELOG.md`, `SESSIONS.md`, this record

## Verification

- WASM release workflow for `wasm-v1.0.0`: passed, including Node 22/24/26
  packed consumers and registry publication steps.
- npmjs: `@mik1810/nxpp-wasm@1.0.0` is public and tagged `latest`.
- Fresh npm registry installation and graph-operation smoke test: passed.
- GitHub Release `wasm-v1.0.0`: created with the package release notes; native
  `v1.4.7` remains the repository's latest release.
- Documentation diff: `git diff --check` passed.

## Next action

For the next WASM change, select a focused issue and prepare a new package
version only when its public contract and verification scope are defined.
