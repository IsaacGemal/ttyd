#!/usr/bin/env python3
"""Put a REL unit's functions and event scripts/tables in the source order that gives the target's .rodata pool order.

    python3 tools/rel_pool_order.py MOD/UNIT [--write]       e.g. gor/gor_02

MWCC pools strings, float constants and aggregate templates in the order it first meets them while it reads the source. A REL
unit written as "all data, then all functions" therefore pools every script string before any function constant; the original
interleaved its functions and scripts. The target's .rodata is a sequence of objects; each is referenced by functions (.text)
and/or data objects (.data); the order of the objects tells which owner came first in the source.

For every .rodata object this takes its first owner (an owner already placed earlier cannot be the first user), builds the
owner order that reproduces the pool, and prints it. With --write, the owned top-level segments (functions, EVT scripts,
initialised tables, with the comment lines right above them) are moved into the slots they occupy, in the new order;
declarations, extend_data blocks and everything else stay where they are. Rebuild afterwards (tools/agent_check.py MOD/UNIT) and add the
forward declarations the compiler asks for. Units with `#if`/`#pragma` lines between segments are refused."""
import os
import re
import sys

unit = sys.argv[1]
write = "--write" in sys.argv
mod, name = unit.split("/")
src = f"rels/{mod}/src/{name}.c"
asm = open(f"build/G8MJ01/{mod}/asm/{name}.s", errors="replace").read()
text = open(src).read()


def norm(n):
    return n.replace("$", "_")


# ---- the target: sections, objects, references --------------------------------------------------------------------------
sections = re.split(r"(?m)^(?=\.section |\.text$|\.data$|\.rodata$|\.bss$|\.sdata$|\.sdata2$|\.sbss$)", asm)
rodata, owners_body = [], {}
cur_sec = None
for blk in re.split(r"(?m)^(?=\.obj |\.fn )", asm):
    m = re.match(r"\.(obj|fn) (\S+?),", blk)
    if not m:
        continue
    kind, nm = m.group(1), m.group(2)
    end = blk.find(".endobj") if kind == "obj" else blk.find(".endfn")
    body = blk[:end if end >= 0 else len(blk)]
    # the section of an .obj is the last "# .sec:0x..." comment before it, but the split above loses it: use the header line
    owners_body[nm] = (kind, body)
secmap = {}
for m in re.finditer(r"# \.(\w+):0x[0-9A-F]+ \| 0x[0-9A-F]+ \| size: 0x[0-9A-F]+\n\.(?:obj|fn) (\S+?),", asm):
    secmap[m.group(2)] = m.group(1)
for m in re.finditer(r"# \.(\w+):0x[0-9A-F]+ \| 0x[0-9A-F]+ \| size: 0x[0-9A-F]+\n\.obj (\S+?),", asm):
    if m.group(1) in ("rodata",) and not m.group(2).startswith("gap_"):
        rodata.append(m.group(2))
rset = set(rodata)
ident = re.compile(r"[A-Za-z_][A-Za-z0-9_$]*")
refs = {}  # owner -> set of rodata objects it references
for nm, (kind, body) in owners_body.items():
    sec = secmap.get(nm)
    if nm in rset or sec in ("rodata", "bss", "sbss") or nm.startswith("gap_") or nm.startswith("jumptable_"):
        continue
    used = {t for t in ident.findall(body[body.find("\n"):]) if t in rset}
    refs[nm] = used
cand = {r: [o for o, u in refs.items() if r in u] for r in rodata}

# ---- the source: owned segments ---------------------------------------------------------------------------------------
lines = text.split("\n")
segs = []  # (start_line, end_line_exclusive, owner)
i, n = 0, len(lines)
owned = {norm(o): o for o in refs}
while i < n:
    ln = lines[i]
    m = re.match(r"(?:static\s+)?EVT_BEGIN\((\w+)\)", ln)
    if m:
        j = i
        while not lines[j].startswith("EVT_END()"):
            j += 1
        segs.append([i, j + 1, m.group(1)])
        i = j + 1
        continue
    if not ln or ln[0] in " \t}#/*" or ln.endswith(";") and "{" not in ln:
        i += 1
        continue
    if ln.rstrip().endswith("{") or re.search(r"=\s*\{\s*$", ln):
        depth, j = 0, i
        while True:
            depth += lines[j].count("{") - lines[j].count("}")
            if depth <= 0 and j >= i:
                break
            j += 1
        mm = re.search(r"(?:USER_FUNC|EVT_DEFINE_USER_FUNC)\(\s*(\w+)\s*\)|(\w+)\s*(?:\[[^\]]*\]\s*)*=\s*\{\s*$|(\w+)\s*\([^()]*(?:\([^()]*\)[^()]*)*\)\s*\{\s*$", ln)
        if mm:
            nm_ = norm(mm.group(1) or mm.group(2) or mm.group(3))
            segs.append([i, j + 1, nm_])
        i = j + 1
        continue
    i += 1
segs = [s for s in segs if s[2] in owned]
# comment lines directly above a segment belong to it
for s in segs:
    while s[0] > 0 and lines[s[0] - 1].lstrip().startswith(("//", "/*", "*")) and not lines[s[0] - 1].startswith("// clang-format"):
        s[0] -= 1
segs.sort()


def absorb_helpers(segs):
    """`static inline` helpers (and their comments) directly above a function belong to it: they are not owners, but the
    function cannot move without them."""
    for s in segs:
        if s[2] not in fn_names_pre:
            continue
        while True:
            k = s[0] - 1
            while k >= 0 and not lines[k].strip():
                k -= 1
            if k >= 0 and lines[k].startswith("#endif"):
                depth, j = 0, k
                while j >= 0:
                    if lines[j].startswith("#endif"):
                        depth += 1
                    elif re.match(r"#\s*if", lines[j]):
                        depth -= 1
                        if depth == 0:
                            break
                    j -= 1
                if j < 0 or "inline" not in "\n".join(lines[j:k + 1]):
                    break
                s[0] = j
                while s[0] > 0 and lines[s[0] - 1].lstrip().startswith(("//", "/*", "*")):
                    s[0] -= 1
                continue
            if k < 0 or lines[k] != "}":
                break
            j = k
            while j >= 0 and not (lines[j] and lines[j][0] not in " \t}" and lines[j].rstrip().endswith("{")):
                j -= 1
            if j < 0 or not re.match(r"(?:static\s+)?inline\b", lines[j]):
                break
            s[0] = j
            while s[0] > 0 and lines[s[0] - 1].lstrip().startswith(("//", "/*", "*")):
                s[0] -= 1


fn_names_pre = {norm(o) for o, (k, b) in owners_body.items() if k == "fn"}
absorb_helpers(segs)
src_order = [s[2] for s in segs]
index = {o: k for k, o in enumerate(src_order)}
missing = [o for o in owned if o not in index]
if missing:
    print(f"note: {len(missing)} target owners not found as segments (e.g. {missing[:5]})", file=sys.stderr)

# ---- the owner order that reproduces the pool ----------------------------------------------------------------------
# Read while the source is parsed (so in source order): strings of data initialisers and aggregate templates (zero/vec3 objects).
# Generated at the end (text order, which is the reverse of the source order): the strings and float constants of function bodies.
# Scripts keep their .data order and functions their text order, so only the interleaving of the two sequences is free.
fn_names = {norm(o) for o, (k, b) in owners_body.items() if k == "fn"}
fn_text = [norm(o) for o, (k, b) in owners_body.items() if k == "fn"]
F = [o for o in reversed(fn_text) if o in index]          # functions in source order
D = [o for o in src_order if o not in fn_names]           # data segments in source order
if [o for o in src_order if o in fn_names] != F:
    print("note: the functions are not in source order (tools/rel_fn_order is dol-only: reorder them by text order first)", file=sys.stderr)


first_gen = next((k for k, r in enumerate(rodata) if re.match(r"(float|double)_", r)), len(rodata))  # the generated constants come after all the parsed ones
pos_of = {r: k for k, r in enumerate(rodata)}


def parse_time(r):
    if pos_of[r] >= first_gen:
        return None
    c = [norm(o) for o in cand.get(r, []) if norm(o) in index]
    if r.startswith("str_"):
        c = [o for o in c if o not in fn_names]
    return c


order, placed, curr, conflicts = [], set(), None, []
for r in rodata:
    c = parse_time(r)
    if not c or curr in c:
        continue
    free = [o for o in c if o not in placed]
    if not free:
        conflicts.append(r)
        continue
    curr = min(free, key=lambda o: index[o])
    order.append(curr)
    placed.add(curr)
if conflicts:
    print(f"{len(conflicts)} pool objects whose owners were all placed earlier: e.g. {conflicts[:6]}")
anch = set(order)
bucket = {None: []}
for seq in (F, D):
    pred = None
    for o in seq:
        if o in anch:
            pred = o
        else:
            bucket.setdefault(pred, []).append(o)
final = list(bucket[None])
for o in order:
    final.append(o)
    final += bucket.get(o, [])
if [o for o in final if o in fn_names] != F or [o for o in final if o not in fn_names] != D:
    print("the pool order contradicts the functions' text order or the scripts' .data order:")
    print("  functions:", [o for o in final if o in fn_names][:12], "vs", F[:12])
    sys.exit(1)
print(f"{unit}: {len(src_order)} segments, {len(order)} anchors")
if final == src_order:
    print("already in order")
    sys.exit(0)
moved = sum(1 for a, b in zip(src_order, final) if a != b)
print(f"{moved} slots change; first moves:", " ".join(f"{a}->{b}" for a, b in zip(src_order, final) if a != b)[:300])
if not write:
    sys.exit(0)
body = {s[2]: lines[s[0]:s[1]] for s in segs}
new, last_end = [], 0
for s, o in zip(segs, final):
    new += lines[last_end:s[0]]
    new += body[o]
    last_end = s[1]
new += lines[last_end:]
open(src, "w").write("\n".join(new))
print("written")
