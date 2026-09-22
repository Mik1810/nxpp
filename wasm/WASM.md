# Building and verifying nxpp WASM

This is the maintainer guide for the experimental Node.js package. For usage
and the supported API, start with the [package README](README.md). The
[architecture](ARCHITECTURE.md) explains layer ownership and 1.0 completion
gates; the [API policy](API_POLICY.md) defines the public contract.

## Scope and dependencies

The native C++ library owns graph algorithms and semantic state. The
Emscripten/Embind layer exposes selected graph capabilities to JavaScript;
the TypeScript facade handles public typing, validation, result shapes, error
normalization, and graph-handle disposal. Consumers install prebuilt assets
and do not need Boost or Emscripten.

Maintainer builds require:

- Emscripten (`em++` on `PATH`)
- Node.js and npm
- Boost headers, optionally located through `BOOST_INCLUDE`

The package supports Node.js 22, 24, and 26 experimentally. Its separate
browser demo is smoke-tested but is not a supported npm surface.

## Local build and checks

Run commands from the repository root unless a command changes directory:

```bash
npm --prefix wasm ci
npm --prefix wasm run build:types
bash wasm/scripts/build_wasm_node_module.sh
```

The Node contract and packed-consumer checks build the module themselves by
default. After a successful build, avoid rebuilding it twice:

```bash
NXPP_WASM_NODE_CONTRACT_SKIP_BUILD=1 bash wasm/scripts/run_wasm_node_contract_tests.sh
NXPP_WASM_NPM_PACK_SKIP_BUILD=1 bash wasm/scripts/run_npm_pack_consumer_test.sh
```

The consumer check packs the package, installs the tarball in an isolated
fixture, checks public TypeScript declarations, and exercises a graph
operation through the package root. CI can pass a previously packed tarball
with `NXPP_WASM_NPM_PACK_TARBALL` so every supported Node major checks the
same artifact.

Additional verification:

```bash
bash wasm/scripts/run_wasm_tests.sh
NXPP_WASM_INCLUDE_LARGE=1 bash wasm/scripts/run_wasm_tests.sh
NXPP_WASM_EMIT_TSD=1 bash wasm/scripts/build_wasm_node_module.sh
npm --prefix wasm run check:raw-contract
bash wasm/scripts/verify_wasm_runtime_reproducibility.sh
```

The generated raw declaration is authoritative for Embind constructor and
method shape. Reviewed TypeScript refines DTO types that Emscripten emits as
`any`. The contract check detects drift between them.

## CI and browser investigation

[`wasm-experimental.yml`](../.github/workflows/wasm-experimental.yml) builds
the Node module once, runs the Node contract, formal suite, raw-contract and
reproducibility checks, and verifies the TypeScript facade output. It packs
one artifact and runs the installed consumer and declaration check on Node
22, 24, and 26. It also builds and smoke-tests a separate browser demo in a
headless browser.

The browser path lives under `wasm/examples/browser-demo/`. It uses its own
Emscripten output and a browser-specific loader, not `runtime/node.mjs`. The
smoke check demonstrates module loading and one graph algorithm only; it does
not establish browser API parity, bundler compatibility, or npm package
support. Serve the demo over HTTP rather than opening its HTML file directly.

For a local browser check:

```bash
bash wasm/scripts/build_wasm_browser_demo.sh
bash wasm/scripts/run_wasm_browser_smoke_test.sh
```

Local overhead benchmarks compare native C++, raw WASM, and the facade on
representative workloads. They are diagnostics, not release claims:

```bash
bash wasm/scripts/run_overhead_benchmarks.sh
```

Benchmark result CSV files are user-driven and should not be generated as
part of routine documentation or release checks.

## Release checklist

WASM releases are independent of the native C++ version. The tag-triggered
[`wasm-release.yml`](../.github/workflows/wasm-release.yml) is the only
publication path. Do not push a `wasm-vX.Y.Z` tag until publication is
explicitly approved.

1. Confirm the Node/browser support statements in this guide, the package
   README, and the root README match the tested surfaces.
2. Align `wasm/package.json` and `wasm/package-lock.json` versions; prepare
   `CHANGELOG.md` and `wasm/RELEASE_NOTES.md` for the declared release.
3. Rebuild the Node module and run TypeScript, Node contract, formal, raw
   contract, reproducibility, and packed-consumer checks above. Confirm the
   package contents and public exports are intentional.
4. Review the version and tag, then, with explicit approval, push the matching
   `wasm-vX.Y.Z` tag. The release workflow rebuilds once and verifies one
   tarball on all declared Node majors before publication.
5. Confirm the release workflow passed. Review and approve the staged npmjs
   package with 2FA; then confirm npmjs and GitHub Packages expose the new
   version.

The workflow stages npmjs publication through Trusted Publishing and GitHub
OIDC. GitHub Packages uses the workflow's short-lived `GITHUB_TOKEN`. Never
put long-lived registry credentials in the repository or its scripts.

## Troubleshooting

If `em++` is not found, source your local emsdk environment and retry:

```bash
source /path/to/emsdk/emsdk_env.sh
```

If Boost headers are not found, pass their parent include directory:

```bash
BOOST_INCLUDE=/path/to/boost/include bash wasm/scripts/build_wasm_node_module.sh
```

For API behavior, inspect the [Node contract tests](test/node_api_contract/)
and the public declarations instead of copying old method lists from this
guide. Work beyond the current surface is tracked in the
[WASM issue list](https://github.com/Mik1810/nxpp/issues?q=is%3Aissue+label%3Awasm).
