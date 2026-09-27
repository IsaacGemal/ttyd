#!/usr/bin/env python3
"""List what an agent fetched or searched outside its workspace, from its Pi transcript (events.jsonl, or .zst).

  python3 tools/provenance.py EVENTS [WORKSPACE]

Prints one line per network fetch (URL), git clone, or search of the machine outside WORKSPACE, found in the
commands the agent ran; nothing if there were none. The launcher saves this as provenance.txt next to the agent's
results; tools/merge_agent.sh shows it and records it in the commit, so reused code is on the record and its
license can be checked before merging. Read-only.
"""
import json
import re
import subprocess
import sys

URL = re.compile(r"https?://[^\s'\"\\)]+")
CLONE = re.compile(r"\bgit\s+clone\s+(\S+)")
SEARCH = re.compile(r"\b(?:find|grep\s+-r\w*|rg|locate)\s+(/(?!tmp\b)[^\s'\"]*|~[^\s'\"]*)")


def commands(path):
    data = (subprocess.run(["zstd", "-dc", path], capture_output=True).stdout if path.endswith(".zst")
            else open(path, "rb").read())
    for line in data.decode(errors="replace").splitlines():
        try:
            e = json.loads(line)
        except ValueError:
            continue
        if e.get("type") == "tool_execution_start":
            yield json.dumps(e.get("args", {}))


def findings(path, workspace=None):
    seen = []
    for cmd in commands(path):
        text = cmd.encode().decode("unicode_escape", errors="replace")
        for m in URL.finditer(text):
            seen.append(f"fetch  {m.group(0)}")
        for m in CLONE.finditer(text):
            seen.append(f"clone  {m.group(1)}")
        for m in SEARCH.finditer(text):
            where = m.group(1)
            if not (workspace and where.startswith(workspace)):
                seen.append(f"search {where}")
    return list(dict.fromkeys(seen))


if __name__ == "__main__":
    print("\n".join(findings(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None)))
