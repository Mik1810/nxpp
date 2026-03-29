# docs/

This directory is for longer-form and eventually generated documentation.

Today, the main user-facing source of truth is still the root [`README.md`](../README.md).

What currently lives here:

- [`API_REFERENCE.md`](API_REFERENCE.md): detailed public API tables, alias reference, proxy syntax, and algorithm/helper reference
- [`API_ARCHITECTURE.md`](API_ARCHITECTURE.md): the public API placement policy for graph methods and namespace-scope helpers
- [`GRAPH_CONFIGURATION.md`](GRAPH_CONFIGURATION.md): graph selector/configuration policy and supported BGL customization surface

What should eventually live here:

- longer guides that would make the root README too heavy
- docs-generation instructions once the tooling is committed

Current status:

- there is not yet a full generated docs site
- this directory should not pretend otherwise
- the root README is now intentionally smaller and should act as the overview / navigation entry point
- when root docs and `docs/` disagree, the root README describes the current user-facing reality while the files under `docs/` hold the deeper technical detail
