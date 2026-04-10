#!/usr/bin/env python3

from __future__ import annotations

import json
import re
import sys
from pathlib import Path


def top_version(path: Path) -> str:
    content = path.read_text(encoding="utf-8")
    match = re.search(r"^## \[([^\]]+)\]", content, re.MULTILINE)
    if not match:
        raise ValueError(f"could not find a top version header in {path}")
    return match.group(1)


def cmake_version(path: Path) -> str:
    content = path.read_text(encoding="utf-8")
    match = re.search(r"project\(nxpp VERSION ([^ )]+) LANGUAGES", content)
    if not match:
        raise ValueError(f"could not find project version in {path}")
    return match.group(1)


def conan_version(path: Path) -> str:
    content = path.read_text(encoding="utf-8")
    match = re.search(r'^\s*version\s*=\s*"([^"]+)"', content, re.MULTILINE)
    if not match:
        raise ValueError(f"could not find recipe version in {path}")
    return match.group(1)


def vcpkg_version(path: Path) -> str:
    data = json.loads(path.read_text(encoding="utf-8"))
    version = data.get("version-string")
    if not isinstance(version, str) or not version:
        raise ValueError(f"could not find version-string in {path}")
    return version


def main() -> int:
    expected = top_version(Path("CHANGELOG.md"))
    versions = {
        "CMakeLists.txt": cmake_version(Path("CMakeLists.txt")),
        "conanfile.py": conan_version(Path("conanfile.py")),
        "packaging/vcpkg/ports/nxpp/vcpkg.json": vcpkg_version(
            Path("packaging/vcpkg/ports/nxpp/vcpkg.json")
        ),
    }

    mismatches = {
        path: version for path, version in versions.items() if version != expected
    }

    if mismatches:
        print(
            f"release metadata drift detected: expected version {expected} from CHANGELOG.md",
            file=sys.stderr,
        )
        for path, version in mismatches.items():
            print(f"- {path} has {version} instead of {expected}", file=sys.stderr)
        return 1

    print(
        f"release metadata versions aligned at {expected}: "
        + ", ".join(sorted(versions)),
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
