# WASM package release notes

The WASM package has its own version and release process, independent of the
native C++ library. These notes describe the `1.0.0` release candidate; they
do not indicate that the package has been published.

## 1.0.0 candidate

This is the first major version of the experimental Node.js package. It
completes the issue-backed architecture roadmap and makes the supported
package boundary explicit.

### Breaking changes from 0.6.0

- Create a runtime context with `await createNxpp()` before constructing
  graphs. The default singleton and global graph constructors are removed.
- Construct one of the eight graph families from that context, such as
  `new nxpp.DiGraphInt()`, and call `graph.dispose()` when finished.
- `loadNxppRuntime()` and the `@mik1810/nxpp-wasm/runtime` subpath are removed.
  The raw Embind module and internal `runtime/` and `dist/` paths are not
  public imports.
- The TypeScript facade is the public API. Its graph methods return normalized
  JavaScript values and errors rather than exposing raw Embind objects.

### Supported scope

- Packed-package consumers are tested on Node.js 22, 24, and 26.
- The package ships prebuilt JavaScript and WASM assets. Consumers do not need
  Boost or Emscripten at install time.
- The browser demo is smoke-tested separately; browser use is not a supported
  npm package API in this release.
- The native C++ library remains the algorithmic source of truth. Only the
  selected graph classes and methods documented in the package README are
  exposed through WASM.

See the [package README](README.md) for migration and usage examples and the
[build guide](WASM.md) for verification and release gates.
