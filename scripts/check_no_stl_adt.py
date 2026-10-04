#!/usr/bin/env python3
"""Fail the build if student ADT code uses forbidden STL containers.

This is a *policy* check, not a linter. It is intentionally simple and
course-friendly.

Scope:
  - include/lacc/adt/
  - src/adt/

Allowed STL headers (common utilities):
  - <string>, <optional>, <utility>, <cstddef>, <cstdint>, <stdexcept>, <functional>
  - <vector> is allowed as a backing array (course policy can tighten later)

Forbidden containers/adapters (examples):
  - unordered_map, unordered_set, map, set, multimap, multiset
  - list, forward_list, deque
  - priority_queue, queue, stack
"""

from __future__ import annotations

import pathlib
import re
import sys


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
SCAN_DIRS = [
    REPO_ROOT / "include" / "lacc" / "adt",
    REPO_ROOT / "src" / "adt",
]


FORBIDDEN_INCLUDES = {
    "<unordered_map>",
    "<unordered_set>",
    "<map>",
    "<set>",
    "<list>",
    "<forward_list>",
    "<deque>",
    "<queue>",
    "<stack>",
}


FORBIDDEN_TOKENS = [
    r"\bstd::unordered_map\b",
    r"\bstd::unordered_set\b",
    r"\bstd::map\b",
    r"\bstd::set\b",
    r"\bstd::multimap\b",
    r"\bstd::multiset\b",
    r"\bstd::list\b",
    r"\bstd::forward_list\b",
    r"\bstd::deque\b",
    r"\bstd::priority_queue\b",
    r"\bstd::queue\b",
    r"\bstd::stack\b",
]

FORBIDDEN_RE = re.compile("|".join(FORBIDDEN_TOKENS))
INCLUDE_RE = re.compile(r"^\s*#\s*include\s*(<[^>]+>)", re.MULTILINE)


def iter_cpp_files(d: pathlib.Path):
    if not d.exists():
        return
    for p in d.rglob("*"):
        if p.is_file() and p.suffix in {".hpp", ".h", ".cpp", ".cc", ".cxx"}:
            yield p


def main() -> int:
    violations: list[str] = []

    for d in SCAN_DIRS:
        for path in iter_cpp_files(d):
            text = path.read_text(encoding="utf-8", errors="replace")

            # Forbidden include headers
            for inc in INCLUDE_RE.findall(text):
                if inc in FORBIDDEN_INCLUDES:
                    violations.append(f"{path.relative_to(REPO_ROOT)}: forbidden include {inc}")

            # Forbidden container tokens
            m = FORBIDDEN_RE.search(text)
            if m:
                violations.append(
                    f"{path.relative_to(REPO_ROOT)}: forbidden STL container usage: '{m.group(0)}'"
                )

    if violations:
        print("ERROR: Forbidden STL containers detected in ADT implementation:\n")
        for v in violations:
            print(" -", v)
        print(
            "\nPolicy: ADT implementations must not use STL container types/adapters "
            "(unordered_map/map/set/list/deque/queue/stack/priority_queue, etc.)."
        )
        return 2

    print("OK: No forbidden STL containers found in ADT code.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
