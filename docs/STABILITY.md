# Public API, packaging, and support status

`nxpp` ships several **consumption surfaces** (C++ headers, CMake, packaging
recipes, generated docs, and an experimental WebAssembly npm package). They do
not all carry the same stability expectations. This file is the **source of
truth** for that split.

## Recommended first-class paths (C++)

For most C++ work, use one of these:

1. **Modular public headers** with `#include <nxpp/...hpp>` and **either**
   vendored CMake (`add_subdirectory(nxpp)`) **or** an **installed** CMake
   package (`find_package(nxpp CONFIG REQUIRED)`). This is the main maintained
   integration path for library code. Details: [`EXTERNAL_USAGE.md`](EXTERNAL_USAGE.md).

2. **The tested single-header release asset** (`nxpp.hpp` from a **tagged
   GitHub release**), if you want a single file drop. The repository workflow
   rebuilds, tests, and publishes that file as part of the release process.

3. **The generated Doxygen / GitHub Pages site** for declaration-level
   reference. It tracks the public headers and is the normal API read path next to
   this markdown layer.

Treating everything else in the table as **first-class** would overstate
support. Secondary or experimental channels are still useful, but not the
default story.

## Support and compatibility matrix

| Surface | Status | Compatibility / support promise |
| --- | --- | --- |
| C++ public headers under `include/nxpp/`, including [`nxpp.hpp`](../include/nxpp.hpp) | **Stable** | Treated as the public API. Repository releases are **SemVer `X.Y.Z`**. `CHANGELOG.md` records user-visible C++ changes. Patch releases are expected to be compatible for normal upgrade paths. |
| CMake target `nxpp::nxpp` (vendored or installed) | **Stable** | The exported interface target, install rules, and package config are part of the **documented** consumption contract. They are expected to stay coherent with the SemVer line. |
| Single-header `nxpp.hpp` release asset | **Stable artifact** | Rebuilt, tested, and published from release automation. Treated as a **supported distribution** for that release tag, not an alternate API fork. |
| Generated API reference (Doxygen / Pages) | **Stable documentation** | Tracks the public headers; layout may evolve, but it is a **first-class** reference path. |
| In-repository Conan recipe (`conanfile.py`) | **Experimental** | Supported for local validation and follow-up on upstream publication. **Best effort** between releases. |
| Repository-hosted vcpkg overlay | **Experimental** | Supported as an **in-repo** port path. **Best effort**; curated-registry timing is separate. |
| AUR package (`packaging/aur/`) | **Community / secondary** | A real additional channel, but not the primary support contract. **Best effort** relative to the tagged release. |
| Node.js package `@mik1810/nxpp-wasm` (Embind + JS/TS facade) | **Experimental** | Public npm surface and ABI can move faster than the C++ API. The package uses its **own** SemVer; breaking changes are **possible** while the lane matures. |
| TypeScript facade (`wasm/dist/`, re-exported types) | **Experimental** | Versioned and released **with** the experimental wasm package. |
| Browser WebAssembly | **Not supported** | Future or investigation only; see [`../wasm/WASM.md`](../wasm/WASM.md). |

## How to read "experimental"

- **Experimental** does **not** mean unsupported documentation. It means the
  surface is **not** given the same upgrade contract as the C++ headers and
  primary CMake path.
- If you need maximum predictability, stay on the **stable** rows and follow
  [`EXTERNAL_USAGE.md`](EXTERNAL_USAGE.md) for include and dependency guidance.
- If you use an experimental path, read that path's own guide (`wasm/README.md`,
  AUR local README, packaging notes) for known limits and test expectations.

## Related guides

- [`EXTERNAL_USAGE.md`](EXTERNAL_USAGE.md): how to consume the stable paths
- [`../wasm/WASM.md`](../wasm/WASM.md): scope and experimental wasm lane
- [`TEST.md`](TEST.md): what CI and local runners actually verify
- [Root `README.md`](../README.md): high-level project entry
