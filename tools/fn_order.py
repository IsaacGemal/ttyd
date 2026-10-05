#!/usr/bin/env python3
"""Put a unit's functions in the source order that emits the target's .text order.

    python3 tools/fn_order.py UNIT [--write]     e.g. effect/eff_bomb, or gor/gor_03 for a REL unit
    python3 tools/fn_order.py --all              every unit not yet linked whose built .text order differs

MWCC (with -inline deferred) emits functions in reverse source order, so the source lists them from the highest target
address to the lowest. Prints the order the unit has now and the order it should have. With --write it rewrites the
source line by line:
  - each emitted function moves as one chunk with what sits in the gap above it: comment lines, `#pragma` lines,
    `#define`s, `static inline` helpers and complete `#if ... #endif` blocks. `#pragma NAME reset` and `#undef` lines
    straight after a function stay with that function;
  - `extern`/prototype declarations, `typedef`s, data definitions and event scripts found between functions are hoisted
    (in their order) to just before the first function, so the data order is unchanged;
  - forward references the new order creates get a prototype: `static` functions in the source, global ones in the
    unit's own header (include/<unit>.h, which the source then includes); anything else is reported to fix by hand.
It refuses (and writes nothing) when a function would see a different set of pragma settings or `#define`s than it
does now (a pragma or macro region spanning several functions), when an `#if` block straddles functions, or when the
non-blank lines of the file would change other than by the added prototypes. Rebuild with tools/agent_check.py UNIT
afterwards and keep the change only if no function's score dropped; CI needs --allow-pragma for moved pragma pairs."""
import collections
import glob
import json
import os
import re
import sys

PRAGMA = re.compile(r"^\s*#\s*pragma\s+(\w+)\s*(.*)$")
FUNC_HEAD = re.compile(r"^(?:static\s+)?(?:USER_FUNC\((\w+)\)|[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;]*\))\s*\{\s*$")


def paths(unit):
    """(source, original object, built object) for a unit name."""
    mod, _, rest = unit.partition("/")
    if rest and os.path.isdir(f"rels/{mod}/src"):
        return f"rels/{mod}/src/{rest}.c", f"build/G8MJ01/{mod}/obj/{rest}.o", f"build/G8MJ01/src/{rest}.o"
    return f"src/{unit}.c", f"build/G8MJ01/obj/{unit}.o", f"build/G8MJ01/src/{unit}.o"


def text_order(path):
    from elftools.elf.elffile import ELFFile
    with open(path, "rb") as f:
        e = ELFFile(f)
        idx = e.get_section_index(".text")
        out = [(s["st_value"], s.name) for s in e.get_section_by_name(".symtab").iter_symbols()
               if s["st_info"]["type"] == "STT_FUNC" and s["st_shndx"] == idx]
    return [n for _, n in sorted(out)]


def _braces(line, state):
    """Net `{`/`}` count of one line outside strings, char literals and comments; state carries an open /* comment."""
    n, i, in_c = 0, 0, state.get("c", False)
    while i < len(line):
        two = line[i:i + 2]
        if in_c:
            if two == "*/":
                in_c, i = False, i + 2
                continue
            i += 1
            continue
        if two == "//":
            break
        if two == "/*":
            in_c, i = True, i + 2
            continue
        c = line[i]
        if c in "\"'":
            j = i + 1
            while j < len(line) and line[j] != c:
                j += 2 if line[j] == "\\" else 1
            i = j + 1
            continue
        n += c == "{"
        n -= c == "}"
        i += 1
    state["c"] = in_c
    return n


def body_end(lines, k):
    """Index of the line that closes the function whose header ends (with `{`) on line k. Inside an #if/#else, only the
    first branch's braces count (MUST_MATCH and natural forms often each open the same block)."""
    st, depth, pp = {}, 0, []   # pp: per open #if, True while in a later (#else/#elif) branch
    for j in range(k, len(lines)):
        t = lines[j].strip()
        if re.match(r"#\s*if", t):
            pp.append(False)
            continue
        if re.match(r"#\s*(else|elif)\b", t) and pp:
            pp[-1] = True
            continue
        if re.match(r"#\s*endif\b", t) and pp:
            pp.pop()
            continue
        if any(pp):
            continue
        depth += _braces(lines[j], st)
        if depth == 0 and j >= k:
            return j
    raise ValueError(f"unbalanced braces after line {k + 1}")


def functions(lines):
    """[(header line, closing-brace line, name, is_static_inline)] for top-level function definitions; a header may
    run over several lines (wrapped parameters, or the `{` on the next line)."""
    out = []
    i = 0
    in_c = False
    while i < len(lines):
        l = lines[i]
        if in_c or l.lstrip().startswith("/*"):   # skip block comments (commented-out functions)
            body = l if in_c else l[l.index("/*") + 2:]
            in_c = "*/" not in body
            i += 1
            continue
        l = re.sub(r"\s*(//.*|/\*.*?\*/\s*)$", "", l)   # a trailing comment on the header line
        if re.match(r"(static\s+)?(inline\s+)?(USER_FUNC\(|[A-Za-z_])", l) and "(" in l and not l.rstrip().endswith(";") \
                and not re.match(r"\s*(typedef|struct|enum|union|if|for|while|switch|return|else|extern)\b", l) \
                and "=" not in l.split("(")[0]:
            k, joined = i, l
            while not joined.rstrip().endswith(("{", ";", "}")) and k + 1 < len(lines) and k - i < 8:
                nxt = re.sub(r"\s*(//.*|/\*.*?\*/\s*)$", "", lines[k + 1].strip())
                if not nxt or nxt.startswith(("#", "//", "/*")) or (")" in joined and nxt != "{" and not joined.rstrip().endswith(",")):
                    break
                k += 1
                joined += " " + nxt
            one = re.match(r"^(.*\))\s*\{[^{}]*\}\s*$", joined)   # a one-line body: `void f(void) {}`
            if one and FUNC_HEAD.match(one.group(1) + " {"):
                m1 = FUNC_HEAD.match(one.group(1) + " {")
                out.append((i, k, m1.group(1) or m1.group(2), bool(re.match(r"\s*(static\s+)?inline\b", l))))
                i = k + 1
                continue
            m = FUNC_HEAD.match(joined)
            if joined.rstrip().endswith("{") and m:
                j = body_end(lines, k)
                out.append((i, j, m.group(1) or m.group(2), bool(re.match(r"\s*(static\s+)?inline\b", l))))
                i = j + 1
                continue
        i += 1
    return out


def gap_items(gap, blocks=()):
    """Split the lines between two functions into items: (kind, lines). kind is 'reset' (belongs to the function above),
    'hoist' (declaration or data: moves before all functions) or 'lead' (goes with the function below). `blocks` are
    (start, end) index ranges in `gap` (inline helper definitions) that stay whole and go with the function below."""
    items, i = [], 0
    starts = dict(blocks)
    while i < len(gap):
        if i in starts:
            items.append(("lead", gap[i:starts[i] + 1]))
            i = starts[i] + 1
            continue
        t = gap[i].strip()
        if not t:
            items.append(("blank", [gap[i]]))
            i += 1
            continue
        pm = PRAGMA.match(gap[i])
        if pm and pm.group(2).split()[:1] == ["reset"] and all(k in ("reset", "blank") for k, _ in items):
            items.append(("reset", [gap[i]]))
            i += 1
            continue
        if t.startswith("/*") and "*/" not in t:   # a multi-line block comment stays one item
            j = i
            while j + 1 < len(gap) and "*/" not in gap[j]:
                j += 1
            items.append(("lead", gap[i:j + 1]))
            i = j + 1
            continue
        if re.match(r"#\s*define\b", t):   # one item, with its continuation lines
            j = i
            while gap[j].rstrip().endswith("\\") and j + 1 < len(gap):
                j += 1
            items.append(("define", gap[i:j + 1]))
            i = j + 1
            continue
        if re.match(r"#\s*if", t):   # a complete #if ... #endif block is one item
            j, d = i, 0
            while j < len(gap):
                if re.match(r"\s*#\s*if", gap[j]):
                    d += 1
                elif re.match(r"\s*#\s*endif", gap[j]):
                    d -= 1
                    if d == 0:
                        break
                j += 1
            if j < len(gap):
                items.append(("ifblock", gap[i:j + 1]))
                i = j + 1
                continue
        if t.startswith("#undef") and all(k in ("reset", "blank") for k, _ in items):
            items.append(("reset", [gap[i]]))
            i += 1
            continue
        # a statement/definition spanning lines up to its terminating ';' at brace depth 0
        if re.match(r"(extern\b|typedef\b|EVT_BEGIN\(|(static\s+)?(const\s+)?[\w\s\*]+\[?[^(]*=|[A-Za-z_][\w\s\*]*\b\w+\s*\([^)]*\)\s*;)", t) \
                and not t.startswith("#") and not re.match(r"(static\s+)?inline\b", t):
            j, depth = i, 0
            while j < len(gap):
                code = gap[j].split("//")[0].rstrip()
                depth += code.count("{") - code.count("}")
                if (depth == 0 and code.endswith(";")) or gap[j].strip().startswith("EVT_END()"):
                    break
                j += 1
            if j < len(gap):
                items.append(("hoist", gap[i:j + 1]))
                i = j + 1
                continue
        if False:
            pass
        items.append(("lead", [gap[i]]))
        i += 1
    return items


def state_at(lines, upto, kind):
    """Active pragma settings (kind 'pragma') or #define names (kind 'macro') before line index `upto`."""
    st = {}
    for l in lines[:upto]:
        if kind == "pragma":
            m = PRAGMA.match(l)
            if m and m.group(1) not in ("once",):
                arg = m.group(2).split("//")[0].strip()
                if arg.startswith("reset"):
                    st.pop(m.group(1), None)
                elif arg:
                    st[m.group(1)] = arg
        else:
            m = re.match(r"\s*#\s*(define|undef)\s+(\w+)", l)
            if m:
                if m.group(1) == "define":
                    st[m.group(2)] = True
                else:
                    st.pop(m.group(2), None)
    return st


def balanced_if(chunk):
    d = 0
    for l in chunk:
        if re.match(r"\s*#\s*if", l):
            d += 1
        elif re.match(r"\s*#\s*endif", l):
            d -= 1
        if d < 0:
            return False
    return d == 0


def split_regions(L, movable):
    """Give each movable function its own copy of any top-level `#pragma NAME value` ... `#pragma NAME reset` region that
    spans several of them: a `reset` after each but the last and the `value` line again before each but the first.
    Returns (new lines, the lines added)."""
    added = []
    while True:
        fns = [f for f in functions(L) if f[2] in movable and not f[3]]
        inside = set()
        for s, e, _, _ in functions(L):
            inside.update(range(s, e + 1))
        ins = None
        for i in range(len(L)):
            m = PRAGMA.match(L[i]) if i not in inside else None
            arg = m.group(2).split("//")[0].strip() if m else ""
            if not m or not arg or arg.startswith("reset"):
                continue
            j = next((k for k in range(i + 1, len(L)) if k not in inside and PRAGMA.match(L[k])
                      and PRAGMA.match(L[k]).group(1) == m.group(1)), None)
            if j is None or not PRAGMA.match(L[j]).group(2).strip().startswith("reset"):
                continue
            covered = [f for f in fns if i < f[0] and f[1] < j]
            if len(covered) < 2:
                continue
            on, off = f"#pragma {m.group(1)} {arg}", f"#pragma {m.group(1)} reset"
            ins = {}
            for x, y in zip(covered, covered[1:]):
                ins.setdefault(x[1] + 1, []).append(off)
                ins.setdefault(y[0], []).append(on)
                added += [off, on]
            break
        if ins is None:
            return L, added
        out = []
        for k, l in enumerate(L):
            out += ins.get(k, [])
            out.append(l)
        L = out


def reorder(text, want):
    """New source text with the emitted functions in `want` order (source order), or raise ValueError saying why not."""
    orig = text.split("\n")
    L, added = split_regions(orig, set(want))
    fns = [f for f in functions(L) if not f[3] and f[2] in want]
    if not fns:
        raise ValueError("no functions to move")
    first = fns[0][0]

    def regional(i):
        m = PRAGMA.match(L[i])
        return m and any(PRAGMA.match(x) and PRAGMA.match(x).group(1) == m.group(1)
                         and PRAGMA.match(x).group(2).strip().startswith("reset") for x in L[i + 1:])
    k = first
    while k > 0 and (L[k - 1].startswith("//") or not L[k - 1].strip() or regional(k - 1)):
        k -= 1
        if L[k].startswith("//") or regional(k):
            first = k
    prefix, chunks, hoist = L[:first], {}, []
    prev_end, prev = first - 1, None
    for s, e, name, _ in fns:
        lead = []
        helpers = [(a - prev_end - 1, b - prev_end - 1) for a, b, nm, inl in functions(L)
                   if (inl or nm not in want) and prev_end < a and b < s]
        for kind, ls in gap_items(L[prev_end + 1:s], helpers):
            if kind in ("define", "ifblock"):
                names = re.findall(r"#\s*define\s+(\w+)", "\n".join(ls))
                code = [x for x in ls if x.strip() and not re.match(r"\s*(#|//|/\*|\*)", x)
                        and not x.rstrip().endswith("\\") and not (ls.index(x) > 0 and ls[ls.index(x) - 1].rstrip().endswith("\\"))]
                plain = kind == "define" or all(re.match(r"\s*(extern\b|[\w\s\*]+\[?\]?[^(]*;$)", x) for x in code)
                if names and plain and not any(re.search(rf"#\s*undef\s+{n}\b", text) for n in names):
                    kind = "hoist"
                else:
                    kind = "lead"
            if kind == "reset" and prev:
                chunks[prev] += ls
            elif kind == "hoist":
                hoist += ls
            elif kind != "blank" or lead:
                lead += ls
        chunks[name] = lead + L[s:e + 1]
        prev_end, prev = e, name
    tail = L[prev_end + 1:]
    k = 0
    while k < len(tail) and (not tail[k].strip() or (PRAGMA.match(tail[k]) and PRAGMA.match(tail[k]).group(2).startswith("reset"))
                             or tail[k].strip().startswith("#undef")):
        if tail[k].strip():
            chunks[prev].append(tail[k])
        k += 1
    tail = tail[k:]
    for n, c in chunks.items():
        if not balanced_if(c):
            raise ValueError(f"an #if block straddles {n}: move it by hand")
    out = prefix + hoist + [""]
    pos = {}
    for n in want:
        if n in chunks:
            c = chunks[n]
            while c and not c[-1].strip():
                c = c[:-1]
            out += c + [""]
    out += tail
    old = collections.Counter(l for l in orig if l.strip()) + collections.Counter(added)
    new = collections.Counter(l for l in out if l.strip())
    if old != new:
        raise ValueError(f"line multiset changed: {dict(old - new)} {dict(new - old)}")
    # every moved function must see the same pragma settings and #defines as before
    def head(lines, name):
        return next(f[0] for f in functions(lines) if f[2] == name)
    for n in chunks:
        for kind in ("pragma", "macro"):
            a, b = state_at(orig, head(orig, n), kind), state_at(out, head(out, n), kind)
            if kind == "macro":
                body = re.sub(r"//[^\n]*|/\*.*?\*/", "", "\n".join(chunks[n]), flags=re.S)
                a = {k for k in a if re.search(rf"\b{k}\b", body)}
                b = {k for k in b if re.search(rf"\b{k}\b", body)}
            if a != b:
                raise ValueError(f"{n} would see different {kind} state ({a} vs {b}): move that region by hand")
    return "\n".join(out)


def forward_refs(text, names):
    """{name: prototype text} for functions referenced above their definition with no declaration before the use."""
    L = text.split("\n")
    defs = {f[2]: f for f in functions(L)}
    out = {}
    for n in names:
        if n not in defs:
            continue
        s = defs[n][0]
        before = "\n".join(L[:s])
        before = re.sub(r"//[^\n]*", "", before)
        uses = [m.start() for m in re.finditer(rf"\b{re.escape(n)}\b", before)]
        if not uses:
            continue
        decl = re.search(rf"(USER_FUNC\({re.escape(n)}\)\s*;|\b{re.escape(n)}\s*\([^)]*\)\s*;)", before)
        if decl and decl.start() < uses[0]:
            continue
        k, h = s, L[s]
        while not h.rstrip().endswith("{"):
            k += 1
            h += " " + L[k].strip()
        out[n] = f"USER_FUNC({n});" if re.match(r"(static\s+)?USER_FUNC\(", h) else h.rstrip()[:-1].rstrip() + ";"
        if re.match(r"static\s+USER_FUNC\(", h):
            out[n] = f"static USER_FUNC({n});"
    return out


def declared_in_headers(name, headers):
    return any(re.search(rf"(USER_FUNC\({re.escape(name)}\)|\b{re.escape(name)}\s*\()", t) for t in headers)


def main(argv):
    if "--all" in argv:
        report = json.load(open("build/G8MJ01/report.json"))
        bad = 0
        for u in report["units"]:
            if u.get("metadata", {}).get("complete"):
                continue
            unit = u["name"].split("/", 1)[1]
            src, orig, built = paths(unit)
            if not (os.path.exists(orig) and os.path.exists(built)):
                continue
            want, have = text_order(orig), text_order(built)
            have = [n for n in have if n in set(want)]
            want2 = [n for n in want if n in set(have)]
            if want2 != have:
                rev = want2 == list(reversed(have))
                print(f"{u['name']:50s} {len(want):4d} functions, {sum(a != b for a, b in zip(want2, have)):4d} out of place"
                      + ("  (exactly reversed)" if rev else ""))
                bad += 1
        print(f"{bad} unit(s) out of order")
        return 0
    unit = argv[0]
    src, orig, built = paths(unit)
    text = open(src).read()
    tgt = text_order(orig)
    fns = [f for f in functions(text.split("\n")) if not f[3] and f[2] in tgt]
    have = [f[2] for f in fns]
    want = [n for n in reversed(tgt) if n in have]
    print(f"{unit}: source order {len(have)} functions")
    if have == want:
        print("already in order")
        return 0
    print("now :", " ".join(have))
    print("want:", " ".join(want))
    if "--write" not in argv:
        return 1
    try:
        new = reorder(text, want)
    except ValueError as e:
        sys.exit(f"{unit}: {e}")
    refs = forward_refs(new, [f[2] for f in functions(new.split("\n"))])
    if refs:
        headers = [open(h, errors="replace").read() for h in glob.glob("include/**/*.h", recursive=True)]
        own = f"include/{unit}.h"
        statics = {n: p for n, p in refs.items() if p.startswith("static")}
        globals_ = {n: p for n, p in refs.items() if n not in statics and not declared_in_headers(n, headers)}
        missing_inc = {n: p for n, p in refs.items() if n not in statics and n not in globals_}
        if statics:
            first = new.split("\n")
            i = functions(first)[0][0]
            new = "\n".join(first[:i] + ["// Prototypes for the forward references the original function order needs."]
                            + list(statics.values()) + [""] + first[i:])
        if globals_:
            if not os.path.exists(own):
                sys.exit(f"{unit}: forward references need prototypes but there is no {own}: {sorted(globals_)}")
            h = open(own).read().rstrip("\n") + "\n" + "\n".join(globals_.values()) + "\n"
            open(own, "w").write(h)
            inc = f'#include "{unit}.h"'
            if inc not in new:
                new = re.sub(r"(#include [^\n]+\n)", rf"\1{inc}\n", new, count=1)
            print("added to", own, ":", " ".join(globals_))
        if missing_inc:
            print("already declared in a header (the source must include it):", " ".join(sorted(missing_inc)))
    open(src, "w").write(new)
    print("written")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
