---
name: nxpp-release-workflow
description: Prepare, audit, or publish nxpp C++ or WASM releases, including version alignment, changelog and release notes, package verification, and credential-safe publication.
---

# nxpp release workflow

First determine whether the task concerns the stable C++ release, the
experimental WASM package, or both. They have independent versions and support
contracts.

## Preparation

- Keep post-release work under `CHANGELOG.md` `Unreleased` until a version is
  chosen.
- Update `RELEASE_NOTES.md` only during declared release preparation.
- For C++ releases, align CMake, Conan, vcpkg, changelog, documentation, and the
  intended Git tag. Run `python3 scripts/check_release_metadata_versions.py`.
- For WASM releases, align `wasm/package.json`, generated runtime and facade
  artifacts, package documentation, and the npm-pack consumer check.
- Review public API and compatibility impact before choosing SemVer.

## Credentials and publication

Never request that a token be pasted into repository files, commands that may
be logged, chat output, or committed configuration. Use user-level npm
authentication or an approved trusted-publishing flow. Confirm registry,
account, package scope, version, and dry-run contents before publication.

Publishing, tagging, pushing, creating a GitHub release, or modifying registry
state requires explicit user authorization immediately before the external
mutation. Stop after a failed or ambiguous publication attempt and report the
result; do not retry blindly.

The GitHub release workflow is `.github/workflows/release.yml`. A normal push
to `main` must not publish a release.
