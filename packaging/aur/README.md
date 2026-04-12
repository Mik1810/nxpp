# AUR Packaging

This directory contains an AUR-oriented packaging skeleton for `nxpp`.

Current scope:

- `PKGBUILD` targets the latest published repository release, not the moving
  `main` branch
- the package is intentionally release-based (`nxpp`), not a VCS package such
  as `nxpp-git`
- installation uses the existing CMake install/export path that already
  installs the headers and `nxppConfig.cmake` files

Notes:

- this is a repository-hosted packaging skeleton, not an automatically
  published AUR package
- `makepkg` / `namcap` validation should be run from an Arch Linux environment
- if this channel is adopted for real, `.SRCINFO` should be regenerated from
  `PKGBUILD` in the Arch environment before publication

Local Docker smoke path:

- `docker build -t nxpp-aur-smoke packaging/aur/docker`
- `docker run --rm nxpp-aur-smoke`

This fixture installs `yay`, then installs `nxpp` from the live AUR package
and checks that the expected header and CMake package files exist.
