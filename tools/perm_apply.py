#!/usr/bin/env python3
"""Apply tools/perm_batch.py results (PB_DIR/results.jsonl).

    python3 tools/perm_apply.py

For each improved function: check that the recorded order is a permutation of the declaration lines the function has now,
rewrite the declarations, rebuild the unit, and keep the change only if the function improved and no function in the unit
dropped (the file is restored otherwise, and on any error or interrupt). Then the NONMATCHING label above each applied
function is updated to the new score, or removed (that one line) when the function reached 100%."""
import json
import os
import re
import subprocess

PB = os.environ.get("PB_DIR", "/tmp/perm_batch")
DECL = re.compile(r"^\s+(?!(?:return|goto|else|break|continue|case|do|if|while|for|switch|sizeof)\b)(?:(?:const|volatile|unsigned|signed|struct|enum|register|static)\s+)*[A-Za-z_]\w*(?:\s*\*+\s*|\s+)[A-Za-z_]\w*(?:\[[^\]]*\])*(?:\s*=\s*[^;]+)?(?:\s*,\s*\**\s*[A-Za-z_]\w*(?:\[[^\]]*\])*(?:\s*=\s*[^;]+)?)*;\s*(?://.*)?$")


def scores(unit):
    r = subprocess.run(["python3", "tools/agent_check.py", unit], capture_output=True, text=True).stdout
    d = {}
    for line in r.splitlines():
        m = re.match(r"\s+([\d.]+)%\s+\d+ B\s+(\S+)", line)
        if m:
            d[m.group(2)] = float(m.group(1))
    return d, "BUILD FAILED" in r


def load(path):
    return [json.loads(line) for line in open(path)] if os.path.exists(path) else []


applied = load(PB + "/applied.jsonl")
seen = {r["unit"] + "|" + r["func"] for r in applied}
recs = [r for r in load(PB + "/results.jsonl") if r["order"] and r["best"] > r["base"] + 1e-9 and r["unit"] + "|" + r["func"] not in seen]
byunit = {}
for r in recs:
    byunit.setdefault(r["unit"], []).append(r)
log = open(PB + "/applied.jsonl", "a")
for unit, rs in sorted(byunit.items()):
    path = f"src/{unit}.c"
    orig = open(path).read()
    keep = False
    try:
        before, _ = scores(unit)
        cur = orig
        used = []
        for r in rs:
            m = re.search(rf"\b{re.escape(r['func'])}\b[^;{{]*\)\s*{{\n", cur)
            if not m:
                continue
            lines = cur[m.end():].split("\n")
            n = 0
            while n < len(lines) and DECL.match(lines[n]):
                n += 1
            if sorted(lines[:n]) != sorted(r["order"]):
                continue  # not a permutation of what the function declares now: skip
            cur = cur[:m.end()] + "\n".join(r["order"]) + "\n" + "\n".join(lines[n:])
            used.append(r)
        if not used:
            continue
        open(path, "w").write(cur)
        after, fail = scores(unit)
        gain = {k: (before[k], after[k]) for k in before if after.get(k, 0) > before[k] + 1e-9}
        keep = (not fail) and bool(gain) and all(after.get(k, 0) >= v - 1e-9 for k, v in before.items())
        print(("OK  " if keep else "REV "), unit, {k: (round(a, 2), round(b, 2)) for k, (a, b) in gain.items()}, flush=True)
        if keep:
            for r in used:
                log.write(json.dumps(r) + "\n")
    finally:
        if not keep:
            open(path, "w").write(orig)

# the NONMATCHING label of each function applied so far
done_path = PB + "/comments_done.txt"
done_keys = set(open(done_path).read().split("\n")) if os.path.exists(done_path) else set()
done = open(done_path, "a")
for r in load(PB + "/applied.jsonl"):
    key = r["unit"] + "|" + r["func"]
    if key in done_keys:
        continue
    path = f"src/{r['unit']}.c"
    text = open(path).read()
    m = re.search(rf"\b{re.escape(r['func'])}\b[^;{{]*\)\s*{{\n", text)
    if not m:
        continue
    start = text.rfind("\n", 0, m.start()) + 1
    lines = text[:start].split("\n")[:-1]
    i = len(lines)
    while i > 0 and lines[i - 1].lstrip().startswith("//"):
        i -= 1
    idx = next((k for k in range(i, len(lines)) if "NONMATCHING" in lines[k]), None)
    if idx is None:
        done.write(key + "\n")
        continue
    if r["best"] >= 99.995:
        # the label line and the lines that continue its explanation, not the neighbouring comments (`// .text:0x...`)
        end = idx + 1
        while end < len(lines) and lines[end].lstrip().startswith("//") and not re.match(r"\s*//\s*(\.\w+:0x|0x[0-9A-F]+\.\.)", lines[end]):
            end += 1
        del lines[idx:end]
    else:
        lines[idx] = re.sub(r"NONMATCHING \(\d+(\.\d+)?%\)", f"NONMATCHING ({r['best']:.2f}%)", lines[idx])
    open(path, "w").write("\n".join(lines) + "\n" + text[start:])
    done.write(key + "\n")
    print("label", key)
