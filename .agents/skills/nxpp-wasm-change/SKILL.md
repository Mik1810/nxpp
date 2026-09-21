---
name: nxpp-wasm-change
description: Implement or review nxpp Emscripten bindings, Node WASM runtime, TypeScript facade, npm package, contract tests, or experimental browser path without duplicating native algorithms.
---

# nxpp WASM change

Keep the native C++ library as the algorithmic source of truth.

## Layer boundaries

- `wasm/include/` and `wasm/src/` own Embind registration and bridge-safe data
  conversion.
- `wasm/ts/` owns JavaScript validation, error normalization, public result
  shapes, lifetime handling, and the TypeScript facade.
- `wasm/dist/` and the selected files under `wasm/build/` are generated publish
  artifacts; regenerate them when their sources change.
- Node.js is the supported experimental runtime target. Keep browser work
  separate and explicitly experimental unless the support policy changes.
- Do not reimplement graph algorithms in TypeScript.

## Verification

Choose the smallest relevant checks, then use the full contract when public
bindings or facade behavior change:

```bash
bash wasm/scripts/build_wasm_node_module.sh
npm --prefix wasm run build:types
bash wasm/scripts/run_wasm_node_contract_tests.sh
bash wasm/scripts/run_npm_pack_consumer_test.sh
```

These commands may be long; follow `AGENTS.md` and ask the user to run them when
appropriate. Compare generated artifacts with their sources and run
`git diff --check`. Never publish or change registry credentials as an implied
part of verification.
