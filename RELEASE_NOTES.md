# Release Notes

These notes are written for GitHub releases and can be more narrative than the
version entries in `CHANGELOG.md`.

## [0.7.6]

### Highlights

- Added automated GitHub release creation from pushed `v*` tags through `.github/workflows/release.yml`.
- Moved release-note extraction into `scripts/extract_release_notes.py`, so the release workflow now pulls a versioned release body from this file instead of relying on ad-hoc local notes.
- Kept `.github/workflows/single-header.yml` focused on CI validation of the generated standalone header, while the dedicated release workflow now owns release publication.
- Updated the repository workflows to `actions/checkout@v6` and `actions/setup-python@v6`, removing the Node 20 deprecation path and aligning the repo with GitHub's Node 24 transition.

### Verification

- `bash scripts/build_single_header.sh`
- `bash scripts/run_single_header_tests.sh`
- `python3 scripts/extract_release_notes.py v0.7.6`

### Assets

- `nxpp.hpp` is attached from the tested `dist/nxpp.hpp` build output.

## [0.7.5]

### Highlights

- Added `scripts/run_single_header_tests.sh` so the full formal suite can be recompiled directly against `dist/nxpp.hpp`.
- Tightened the standalone-header release path so the generated asset is validated by the dedicated single-header suite before publication.
- Kept the small smoke test in place as a fast extra check on the generated file.

### Verification

- `bash scripts/run_tests.sh`
- `bash scripts/build_single_header.sh && bash scripts/run_single_header_tests.sh`

### Assets

- `nxpp.hpp` remains the standalone header asset built from `dist/nxpp.hpp`.

## [0.7.4]

### Highlights

- Extended the standalone-header automation so published releases can receive the tested CI-built `nxpp.hpp` asset instead of relying on a purely manual local upload path.
- Kept `dist/nxpp.hpp` available as a workflow artifact for normal CI runs while preparing the release flow for tighter automation.

### Verification

- `bash scripts/build_single_header.sh`
- `bash scripts/run_tests.sh`

### Assets

- `nxpp.hpp`
