# WASM trusted publishing

- Date: 2026-09-21
- Scope: replace local token-based package publication with a tag-gated GitHub
  Actions workflow.
- Decision: use `wasm-vX.Y.Z` tags, npm Trusted Publishing with staged
  maintainer approval, and the workflow `GITHUB_TOKEN` for GitHub Packages.
- Reproducibility: pin release builds to Node.js 24 and Emscripten 6.0.9.
- Credentials: do not create or store long-lived npm or GitHub Packages tokens.
- Files: `.github/workflows/wasm-release.yml`, `wasm/package.json`,
  `wasm/README.md`, `CHANGELOG.md`, `SESSIONS.md`.
- Verification: workflow YAML parsed successfully; the pinned setup-emsdk v16
  tag exists; npm 11.16 supports staged publishing; TypeScript build and npm
  package dry-run passed; the package excluded `.npmrc` and `node_modules`;
  `git diff --check` passed. The user separately verified all 104 native tests,
  the WASM build, Node contract tests, and npm consumer test.
- Limit: GitHub can validate and execute the workflow only after it is pushed;
  external publication is intentionally not part of this task.
- Next: commit and push the workflow, then configure it as the trusted
  publisher on npmjs before preparing a new package version.
