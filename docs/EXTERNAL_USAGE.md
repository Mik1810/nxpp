# External Usage

This document explains how to consume `nxpp` from another project today.

`nxpp` is currently a **header-only C++20** library built on top of the
**Boost Graph Library (BGL)**.

That means:

- there is no compiled `nxpp` library to link
- your compiler must be able to find Boost Graph headers
- you can consume `nxpp` from the modular `include/` tree, from the installed
  CMake package, or from the tested single-header release asset

## Option 1: modular headers from the repo or a vendored checkout

If you vendor the repository or otherwise copy the `include/` tree into your
project, put that `include/` directory on your compiler include path.

Then use:

```cpp
#include <nxpp.hpp>
```

or narrower semantic headers such as:

```cpp
#include <nxpp/graph.hpp>
#include <nxpp/shortest_paths.hpp>
```

Minimal example:

```cpp
#include <nxpp.hpp>

int main() {
    nxpp::GraphInt graph;
    graph.add_edge(0, 1, 2);
    return graph.has_edge(0, 1) ? 0 : 1;
}
```

Minimal compile command:

```bash
g++ -std=c++20 -I/path/to/nxpp/include app.cpp -o app
```

If Boost is not installed in a standard compiler search path, add the
appropriate include directory for your Boost installation too.

## Option 2: installed CMake package

`nxpp` now provides an installable/exported CMake package config.

After installing it into a prefix, consumers can use:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app LANGUAGES CXX)

find_package(nxpp CONFIG REQUIRED)

add_executable(my_app app.cpp)
target_link_libraries(my_app PRIVATE nxpp::nxpp)
```

Minimal consumer-side configure example:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/nxpp/install/prefix
cmake --build build
```

This path assumes:

- the package was installed into a prefix visible to CMake
- Boost is available to the consuming CMake project
- the consumer wants the installed-package layout instead of a vendored checkout

## Option 3: tested single-header release asset

GitHub releases publish a tested `nxpp.hpp` asset.

This is the intended single-file distribution for external consumers.

Place that file somewhere under your project's include/vendor directory and put
that directory on the compiler include path.

Then use:

```cpp
#include <nxpp.hpp>
```

Minimal compile command:

```bash
g++ -std=c++20 -I/path/to/vendor/include app.cpp -o app
```

## Boost dependency

`nxpp` depends on Boost Graph headers.

On Ubuntu / Debian the repository CI installs:

```bash
sudo apt-get install libboost-graph-dev
```

The header also performs a compile-time include check and fails early if the
required BGL headers are missing.

## Repo-local dist/nxpp.hpp vs release asset

Inside the repository:

- `include/nxpp.hpp` is the canonical umbrella header
- `dist/nxpp.hpp` is a generated artifact used for validation and release prep

For external users:

- prefer the tested `nxpp.hpp` asset attached to a GitHub release
- do not treat a locally edited `dist/nxpp.hpp` as the source of truth

## Which option to choose

- use repository-local headers when you are compiling directly inside a clone of the repo
- use vendored `add_subdirectory(...)` when `nxpp` is checked into your source tree as a dependency
- use the installed package when you want a normal `find_package(nxpp CONFIG REQUIRED)` workflow
- use the release single-header asset when you want the narrowest possible single-file integration path

## Current scope of the external story

Today the documented external-consumption story is intentionally minimal:

- include the modular headers, use the installed CMake package, or use the release single header
- install Boost Graph headers
- compile as C++20

It does not yet assume:

- a system package manager integration such as Debian / Ubuntu packaging

Conan support now exists as a local recipe in the repository:

- `conanfile.py` models `nxpp` as a header-only package
- the recipe declares Boost as a Conan dependency and forces the Boost package into header-only mode
- the intended first step is local recipe validation, not remote publication

Minimal local Conan validation command:

```bash
conan create . --profile:build=default --profile:host=default -s:h compiler.cppstd=20
```

This assumes:

- Conan is installed in your environment
- you have a usable default Conan profile
- Boost can be resolved by Conan in the configured remotes/profile setup

vcpkg support now exists as a local overlay-port path in the repository:

- `packaging/vcpkg/ports/nxpp/portfile.cmake`
- `packaging/vcpkg/ports/nxpp/vcpkg.json`
- the current intent is local overlay-port validation first, not immediate public-registry publication
- the overlay install path has been validated locally with the command below

Typical local overlay-port install shape:

```bash
vcpkg install nxpp --overlay-ports=/path/to/nxpp/packaging/vcpkg/ports
```

This assumes:

- `vcpkg` is installed in your environment
- the overlay port points at a local checkout of this repository
- Boost Graph is available through the vcpkg dependency graph for the selected triplet

## Minimal vendored CMake consumption

If you vendor the repository, you can also consume it through CMake:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app LANGUAGES CXX)

find_package(Boost REQUIRED)
add_subdirectory(external/nxpp)

add_executable(my_app app.cpp)
target_link_libraries(my_app PRIVATE nxpp::nxpp)
```

This minimal CMake path currently assumes:

- `nxpp` is available as a subdirectory or vendored checkout
- Boost can be found by your normal CMake setup
- you only need a header-only interface target

## Repository-side CMake test path

For repository development there is also a minimal CMake-driven path for the
formal test suite.

Configure with:

```bash
cmake -S . -B build -DNXPP_BUILD_TESTS=ON
```

Then build and run the registered tests with:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

This path currently covers the main formal modular test binaries. It is meant
as a cross-platform repository build path, not as a replacement for the
existing shell scripts or for the single-header validation flow.

The repository now keeps platform-specific runners under:

- `scripts/unix/` for the Unix/macOS `.sh` entry points
- `scripts/windows/` for the PowerShell counterparts
