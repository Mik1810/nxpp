#!/usr/bin/env python3

from __future__ import annotations

import re
import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) == 3 and sys.argv[1] == "--latest-version":
        content = Path(sys.argv[2]).read_text(encoding="utf-8")
        match = re.search(r"^## \[([^\]]+)\]", content, re.MULTILINE)
        if not match:
            print(f"could not find any version header in {sys.argv[2]}", file=sys.stderr)
            return 1
        print(match.group(1))
        return 0

    if len(sys.argv) != 2:
        print("usage: extract_release_notes.py <tag> | --latest-version <path>", file=sys.stderr)
        return 1

    tag = sys.argv[1]
    version = tag[1:] if tag.startswith("v") else tag

    notes_path = Path("RELEASE_NOTES.md")
    content = notes_path.read_text(encoding="utf-8")

    pattern = re.compile(
        rf"^## \[{re.escape(version)}\](?: - [^\n]*)?\n(?P<body>.*?)(?=^## \[|\Z)",
        re.MULTILINE | re.DOTALL,
    )
    match = pattern.search(content)
    if not match:
        print(f"could not find release notes entry for version {version}", file=sys.stderr)
        return 1

    body = match.group("body").strip()
    if not body:
        print(f"release notes entry for version {version} is empty", file=sys.stderr)
        return 1

    print(body)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
