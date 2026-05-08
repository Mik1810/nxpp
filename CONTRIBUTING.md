# Contributing to nxpp

## Requirements

- CMake 3.16 or newer
- A C++20-capable compiler
- Boost Graph Library headers 1.86.0 or newer

The repository is header-only, but the formal checks build small test binaries
through CMake.

The official Boost support matrix starts at Boost 1.86.0. Compatibility CI
tests that minimum version on Linux and also tests the latest Boost release
available from `archives.boost.io`.

## Build and Test

Configure a local build tree with tests enabled:

```bash
cmake -S . -B build -DNXPP_BUILD_TESTS=ON
```

Build and run the registered tests:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

For the repository's focused Unix test script, run:

```bash
bash scripts/unix/run_tests.sh
```

When changing a narrow area, run the smallest relevant test first. Run the full
relevant suite for public API, shared infrastructure, release logic, or core
behavior changes.

## Code Style

- Follow the existing style in nearby files.
- Keep changes focused on one issue or one logical task.
- Prefer simple readable code over broad refactors.
- Add Doxygen comments for new public API.
- Add focused tests for new behavior or bug fixes.
- Do not add dependencies unless the change clearly requires them.

## Documentation and Release Notes

Update documentation when public behavior changes. For completed issue work,
keep the versioned project history in sync:

- `CHANGELOG.md` for concise technical history
- `RELEASE_NOTES.md` for release-facing notes
- `SESSION.md` for a compact chronological work log

The top versions in `CHANGELOG.md` and `RELEASE_NOTES.md` should match the
project metadata version.

## Pull Requests

Before opening a PR:

1. Keep the branch to one logical change.
2. Reference the relevant issue number.
3. Include tests or explain why no test applies.
4. Update docs and release/history metadata when appropriate.
5. Run the smallest relevant verification and report what passed.
6. Make sure CI is green before requesting review.
