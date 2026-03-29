#!/usr/bin/env python3

from __future__ import annotations

import re
import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: extract_release_notes.py <tag>", file=sys.stderr)
        return 1

    tag = sys.argv[1]
    version = tag[1:] if tag.startswith("v") else tag

    changelog_path = Path("CHANGELOG.md")
    content = changelog_path.read_text(encoding="utf-8")

    pattern = re.compile(
        rf"^## \[{re.escape(version)}\] - [^\n]*\n(?P<body>.*?)(?=^## \[|\Z)",
        re.MULTILINE | re.DOTALL,
    )
    match = pattern.search(content)
    if not match:
        print(f"could not find changelog entry for version {version}", file=sys.stderr)
        return 1

    body = match.group("body").strip()
    if not body:
        print(f"changelog entry for version {version} is empty", file=sys.stderr)
        return 1

    print(body)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
