# Versioning Policy

`nxpp` follows [Semantic Versioning 2.0.0](https://semver.org/) for repository
releases after `1.0.0`.

| Increment | When |
| --- | --- |
| **PATCH** `x.y.Z` | Backwards-compatible bug fixes, documentation fixes, test-only changes, and packaging metadata fixes that do not add public API |
| **MINOR** `x.Y.0` | New backwards-compatible public API, such as new methods, overloads, aliases, options, or supported integration surfaces |
| **MAJOR** `X.0.0` | Breaking public API changes, including removed or renamed methods, changed signatures, changed default template parameters, or changed documented default behavior |

## Breaking-change criteria

For stable surfaces, a change requires a major version when it can break
source-level, build-system, or documented behavior compatibility for an existing
consumer. This includes:

- removing, renaming, or moving public methods, free functions, aliases, result
  fields, headers, CMake targets, or install/export files;
- changing public signatures, template parameters, return shapes, exception
  contracts, or documented default behavior;
- changing the meaning of an existing public method in a way that makes correct
  old code compile but behave differently;
- removing deprecated compatibility aliases or shims;
- raising required C++ standard, CMake version, or Boost version for a stable
  consumption surface;
- changing stable single-header or CMake package consumption in a way that
  breaks existing documented usage.

Deprecated compatibility aliases remain supported until the next major release
unless a specific deprecation notice says otherwise. Removing a deprecated alias
from the stable C++ public headers is still a breaking public API change, so it
requires a major version and migration notes.

The stable C++ public headers, CMake target, and tested single-header release
asset follow this repository-level policy. Experimental surfaces, including the
WebAssembly package and secondary packaging channels, are described separately in
[`docs/STABILITY.md`](docs/STABILITY.md).

Pre-`1.0.0` rapid iteration is historical only. Once `1.0.0` was declared, the
policy above is the compatibility contract for stable release tags.

## Historical SemVer audit

The release history since `1.0.0` includes two documented breaking changes that
were shipped under minor versions. Existing tags are not rewritten, but future
releases should use these as explicit guardrails:

- `1.2.0` removed `Graph::to_dot(std::ostream&)` and
  `Graph::to_dot_string()` in favor of `nxpp::viz::to_dot(...)` and
  `nxpp::viz::write_dot(...)`. That removed stable C++ member APIs and should
  have been a major release.
- `1.4.0` changed weighted `Graph::edges()` from returning weighted
  three-element tuples to endpoint pairs and introduced `weighted_edges()` for
  the old data shape. That changed observable behavior for existing weighted
  graph callers and should have been a major release.

## Release compatibility review

Before cutting a release, review every user-visible change against this policy:

- identify the affected stability surface from `docs/STABILITY.md`;
- classify the change as patch, minor, or major before updating version
  metadata;
- confirm stable C++ headers, `nxpp::nxpp` CMake consumption, and the tested
  single-header release asset remain source/build compatible for patch and
  minor releases;
- check whether a deprecation shim can preserve compatibility until the next
  major release;
- include migration notes for every major release and for any compatibility shim
  that changes the recommended path.

Major-release migration notes should identify the removed or changed API, show
the replacement, and state whether a compatibility path remains available.
