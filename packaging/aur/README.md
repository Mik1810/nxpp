# AUR Packaging

This directory contains the repository-hosted AUR packaging path for `nxpp`.

Current scope:

- `PKGBUILD` targets the latest published repository release, not the moving
  `main` branch
- the package is intentionally release-based (`nxpp`), not a VCS package such
  as `nxpp-git`
- installation uses the existing CMake install/export path that already
  installs the headers and `nxppConfig.cmake` files

Notes:

- this is an active secondary distribution channel based on the release
  package shape (`nxpp`)
- `makepkg` / `namcap` validation should be run from an Arch Linux environment
- `.SRCINFO` should be regenerated from `PKGBUILD` in the Arch environment
  whenever package metadata changes

Local Docker smoke path:

- `docker build -t nxpp-aur-smoke packaging/aur/docker`
- `docker run --rm nxpp-aur-smoke`

This fixture installs `yay`, then installs `nxpp` from the live AUR package
and checks that the expected header and CMake package files exist.
