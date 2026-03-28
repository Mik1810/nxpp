# docs/

This directory is for longer-form and eventually generated documentation.

Today, the main user-facing source of truth is still the root [`README.md`](../README.md).

What currently lives here:

- [`API_ARCHITECTURE.md`](API_ARCHITECTURE.md): the public API placement policy for graph methods and namespace-scope helpers

What should eventually live here:

- generated API reference from the public header
- longer guides that would make the root README too heavy
- docs-generation instructions once the tooling is committed

Current status:

- there is not yet a full generated docs site
- this directory should not pretend otherwise
- when root docs and `docs/` disagree, the root README describes the current user-facing reality
