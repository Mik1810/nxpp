# Versioning Policy

`nxpp` follows [Semantic Versioning 2.0.0](https://semver.org/) for repository
releases after `1.0.0`.

| Increment | When |
| --- | --- |
| **PATCH** `x.y.Z` | Backwards-compatible bug fixes, documentation fixes, test-only changes, and packaging metadata fixes that do not add public API |
| **MINOR** `x.Y.0` | New backwards-compatible public API, such as new methods, overloads, aliases, options, or supported integration surfaces |
| **MAJOR** `X.0.0` | Breaking public API changes, including removed or renamed methods, changed signatures, changed default template parameters, or changed documented default behavior |

The stable C++ public headers, CMake target, and tested single-header release
asset follow this repository-level policy. Experimental surfaces, including the
WebAssembly package and secondary packaging channels, are described separately in
[`docs/STABILITY.md`](docs/STABILITY.md).

Pre-`1.0.0` rapid iteration is historical only. Once `1.0.0` was declared, the
policy above is the compatibility contract for stable release tags.
