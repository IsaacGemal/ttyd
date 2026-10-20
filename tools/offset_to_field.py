#!/usr/bin/env python3
"""Rewrite raw offset casts that land on an existing field into field accesses.

    uv run --with libclang tools/offset_to_field.py [PATH...]              dry run: counts by outcome
    uv run --with libclang tools/offset_to_field.py --list [PATH...]       ... plus every site and why
    uv run --with libclang tools/offset_to_field.py --apply PATH...        rewrite the files in place
    uv run --with libclang tools/offset_to_field.py --verify --build-cmd CMD PATH...
                                                                           rewrite, build, keep only files whose
                                                                           build stays byte-identical
    python3 tools/offset_to_field.py --dwarf-index OUT.json                 (on a built tree) MWCC layouts for --dwarf

With no PATH the whole tree (src/, rels/*/src) is scanned. A PATH may be a file or a directory inside the repo;
only those .c files are ever written (relative include paths are resolved from the repo root, wherever it runs).
Add `--compile-commands FILE` (configure.py's compile_commands.json from a built tree) for per-file include
paths and defines, `--dwarf FILE` (from `--dwarf-index` on a built tree) to check every rewrite against MWCC's
own layout, and `--skip-base TAG` to leave a struct's casts alone.

    *(f32*)((u8*)unit + 0x174)          ->  unit->moveSpeedY
    (Vec*)((u8*)pl + 0x98)              ->  &pl->pos
    ((Foo*)((u8*)p + 0x20))->x          ->  p->foo.x
    ((f32*)((u8*)p + 0x40))[i]          ->  p->arr[i]       (only when the field is an array of f32)

Offset macros: a file-local `#define NAME(p, off) (*(T*)((u8*)(p) + (off)))` (WW_S32, WW_F32, WW_U16, WW_PTR, MW_S32,
MF32, GND_PTR's `type` parameter, MWVEC, ...) is analysed as the cast it expands to, and a use such as
`WW_S32(work, 0x214)` is rewritten to `work->field` under the same rules. The macro's type is its own cast's, so a
field of any other type is never picked (no pointer retyping through a macro). A macro whose every use is rewritten
has its #define dropped; a macro with any use left keeps it. Macros of other shapes are not recognised.

Types and layouts come from clang (libclang, target powerpc-unknown-eabi, the project's include paths and
defines), so the base expression's declared type is whatever the compiler sees, through typedefs, struct
members, casts and calls, and field offsets are clang's record layout. That layout is the PowerPC EABI one
MWCC's `-align powerpc` also follows; the byte-identical build is the real check (a wrong offset or a wrong
type changes the emitted code), and --verify reverts any file whose rewrite doesn't survive it.

A site is rewritten only when all of these hold:
- the cast is written out in the file itself (not in a macro body or a macro expansion) and is not inside an
  `#if`/`#ifdef` block that mentions MUST_MATCH (either arm);
- the byte cast is spelled `(u8*)` and the offset is an integer literal;
- the base expression has type pointer to a complete struct/union;
- the offset is the start of a field (descending into nested structs and arrays) whose type is the cast's
  pointee type, ignoring `const`, never `volatile`, never a bitfield;
- the offset does not land inside a union (at any depth): such a site is listed as "union: needs a human" and no
  member is picked, even when only one member matches;
- the result is dereferenced (`*`), member-accessed (`->`), indexed into an array field, or used as a plain
  pointer value (`&base->field`); pointer arithmetic, `++`/`--`, compound assignment and storing the address
  in a variable (later code may stride past the field) are left alone;
- the field's name is not a placeholder (`unk*`, `pad*`, `field_*`, leading `_`): those are left for naming passes.
One deliberate type exception, for raw casts only: a pointer read through a different pointer type, `*(T**)((u8*)p + 0x10)`
on a `U* next` field, becomes `(T*)p->next` (same word loaded, cast kept), and only when it isn't written to.
"""
import argparse
import atexit
import functools
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

try:
    import clang.cindex as ci
    K = ci.CursorKind
except ImportError:
    ci = None  # only --dwarf-index works without it
CLANG_ARGS = [
    "-target", "powerpc-unknown-eabi", "-fsyntax-only", "-ferror-limit=0", "-funsigned-char", "-fdeclspec",
    "-nostdinc", "-w",
    # MWCC-isms clang would otherwise reject: musyx's C `bool`, its fabs intrinsics, loose function pointers
    "-Dbool=_Bool", "-D__fabsf=__builtin_fabsf", "-Wno-error=incompatible-function-pointer-types",
    "-Wno-error=implicit-function-declaration", "-Wno-error=int-conversion",
]
# used when there is no compile_commands.json (configure.py writes one next to build.ninja)
DEFAULT_FLAGS = [
    "-Iinclude", "-Ilibs/dolsdk2004/include", "-Ilibs/dolsdk2004/include/libc", "-Ilibs/musyx/include",
    "-Ibuild/G8MJ01/include", "-DBUILD_VERSION=0", "-DVERSION_G8MJ01", "-DSDK_REVISION=1", "-DMUST_MATCH",
    "-DNDEBUG=1", "-DMUSY_VERSION_MAJOR=2", "-DMUSY_VERSION_MINOR=0", "-DMUSY_VERSION_PATCH=3",
]
COMPILE_FLAGS = {}  # source path -> its -I/-D flags, from compile_commands.json
STUB_DIR = None     # empty stand-ins for generated includes that aren't on this machine
U8_CAST = re.compile(r"\(\s*u8\s*\*\s*\)")
INT_LIT = re.compile(r"(0[xX][0-9A-Fa-f]+|0|[1-9]\d*)[uUlL]*")  # no octal
SIMPLE = re.compile(r"[A-Za-z_]\w*(\s*(->|\.)\s*[A-Za-z_]\w*|\s*\[[^\[\]]*\])*")
PLACEHOLDER = re.compile(r"(unk|pad|field_|_)", re.I)
ADDR_SIMPLE = re.compile(r"&\s*(" + SIMPLE.pattern + r")")
# an offset macro's body: (*(T*)((u8*)(base) + (off))), T written out or a parameter (GND_PTR's `type`)
MACRO_BODY = re.compile(r"\(\*\(\s*(?P<cast>[^()]+?)\s*\)\s*\(\(\s*u8\s*\*\s*\)\s*\(\s*(?P<base>\w+)\s*\)"
                        r"\s*\+\s*\(\s*(?P<off>\w+)\s*\)\s*\)\s*\)")


def pack_short_enums(text):
    """`enum __attribute__((packed))` for every enum definition in a `#pragma enumsalwaysint off` region."""
    parts = re.split(r"(^[ \t]*#[ \t]*pragma[ \t]+enumsalwaysint[ \t]+\w+.*$)", text, flags=re.M)
    off = False
    for i, part in enumerate(parts):
        m = re.match(r"\s*#\s*pragma\s+enumsalwaysint\s+(\w+)", part)
        if m:
            off = m.group(1) == "off"
        elif off:
            parts[i] = re.sub(r"\benum\b(?=\s*\w*\s*\{)", "enum __attribute__((packed))", part)
    return "".join(parts)


@functools.lru_cache(maxsize=None)
def short_enum_headers():
    """MWCC's `#pragma enumsalwaysint off` sizes an enum to its values; clang ignores the pragma. Feed clang
    in-memory copies (unsaved files) of the headers that use it, with their enums packed."""
    out = []
    for d in [ROOT / "include", ROOT / "libs"] + sorted(ROOT.glob("rels/*/include")):
        for h in sorted(d.rglob("*.h")):
            t = h.read_text(errors="surrogateescape")
            if "enumsalwaysint" in t:
                out.append((str(h), pack_short_enums(t)))
    return out


def load_compile_commands(path):
    for e in json.loads(Path(path).read_text()):
        d = Path(e["directory"])
        f = Path(e["file"])
        try:
            rel = (f.relative_to(d) if f.is_absolute() else f).as_posix()
        except ValueError:
            continue
        args = e.get("arguments") or e["command"].split()
        flags, i = [], 0
        while i < len(args):
            a = args[i]
            if a in ("-I", "-D", "-U") and i + 1 < len(args):
                flags.append(a + args[i + 1])
                i += 1
            elif a.startswith(("-I", "-D", "-U")):
                flags.append(a)
            i += 1
        COMPILE_FLAGS[rel] = flags


def clang_args(rel):
    flags = COMPILE_FLAGS.get(rel)
    if flags is None:
        flags = list(DEFAULT_FLAGS)
        parts = rel.split("/")
        if parts[0] == "rels":
            flags.append(f"-Irels/{parts[1]}/include")
    args = CLANG_ARGS + flags
    if STUB_DIR:
        args.append(f"-idirafter{STUB_DIR}")
    return args


def parse(rel, text):
    """libclang TU for one source; generated includes this machine lacks are stubbed empty (in a temp dir removed
    at exit) and the parse retried."""
    global STUB_DIR
    path = ROOT / rel
    for _ in range(8):
        tu = ci.Index.create().parse(str(path), args=clang_args(rel),
                                     unsaved_files=[(str(path), text)] + short_enum_headers())
        missing = [re.match(r"'(.+)' file not found", d.spelling) for d in tu.diagnostics if d.severity >= 4]
        missing = [m.group(1) for m in missing if m]
        if not missing:
            return tu
        if STUB_DIR is None:
            STUB_DIR = tempfile.mkdtemp(prefix="offset_to_field-stubs-")
            atexit.register(shutil.rmtree, STUB_DIR, True)
        made = False
        for name in missing:
            stub = Path(STUB_DIR) / name
            if not stub.exists():
                print(f"{rel}: '{name}' not found, stubbed empty", file=sys.stderr)
                stub.parent.mkdir(parents=True, exist_ok=True)
                stub.write_text("")
                made = True
        if not made:
            return tu
    return tu


DWARF_MEMBER = re.compile(r"^( *)(.*?)\b(\w+)(\[[^\]]*\])*; // offset 0x([0-9A-Fa-f]+), size 0x([0-9A-Fa-f]+)$")
DWARF_CLOSE = re.compile(r"^( *)\} (\w+)(\[[^\]]*\])*; // offset 0x([0-9A-Fa-f]+), size 0x([0-9A-Fa-f]+)$")


def parse_dwarf_dump(text):
    """{tag: {dotted member path: (offset, size)}} for the top-level structs/unions of a `dtk dwarf dump`.
    Inline anonymous members contribute dotted paths with offsets from the top-level start."""
    out, cur, stack = {}, None, []
    for line in text.split("\n"):
        if cur is None:
            m = re.match(r"^(?:struct|union) (\w+) \{", line)
            if m:
                cur, stack = {}, [[]]
                out[m.group(1)] = cur
            continue
        if line.startswith("}"):
            for path, off, size in stack[0]:
                cur[path] = (off, size)
            cur = None
            continue
        if re.match(r"^ *(?:struct|union) \{$", line):
            stack.append([])
            continue
        m = DWARF_CLOSE.match(line)
        if m and len(stack) > 1:
            inner = stack.pop()
            base = int(m.group(4), 16)
            stack[-1].append((m.group(2), base, int(m.group(5), 16)))
            stack[-1].extend((f"{m.group(2)}.{p}", base + o, sz) for p, o, sz in inner)
            continue
        m = DWARF_MEMBER.match(line)
        if m:
            stack[-1].append((m.group(3), int(m.group(5), 16), int(m.group(6), 16)))
    return out


def dwarf_index(out_path):
    """Run on a built tree: dump each compiled source's object DWARF with dtk and store the struct layouts as
    JSON {source path: {tag: {member path: offset}}}."""
    ninja = (ROOT / "build.ninja").read_text().replace("$\n", " ")
    pairs = re.findall(r"^build (\S+\.o): mwcc\w* +(\S+\.c)\b", ninja, re.M)
    dtk = ROOT / "build/tools/dtk"
    res = {}
    with tempfile.TemporaryDirectory() as td:
        for obj, srcp in pairs:
            if not (ROOT / obj).exists():
                continue
            dump = Path(td) / "d.txt"
            r = subprocess.run([str(dtk), "dwarf", "dump", str(ROOT / obj), "-o", str(dump)], capture_output=True)
            if r.returncode or not dump.exists():
                continue
            layouts = parse_dwarf_dump(dump.read_text(errors="replace"))
            res[srcp] = {t: {p: o for p, (o, _) in m.items()} for t, m in layouts.items()}
    Path(out_path).write_text(json.dumps(res, separators=(",", ":"), sort_keys=True))
    print(f"{len(res)} objects indexed into {out_path}")


DWARF = {}
SKIP_BASES = set()


def check_layout(rel, rec, segs):
    """Compare clang's offsets along a member path with MWCC's (the object's DWARF, see --dwarf).
    'agree', 'disagree' or 'unchecked' (no DWARF for the file or none of the structs on the path)."""
    dw = DWARF.get(rel)
    if dw is None:
        return "unchecked"
    t, checked = rec.get_canonical(), False
    for seg in segs:
        if seg.startswith("["):
            t = t.get_array_element_type().get_canonical()
            continue
        name = seg[1:]
        tag = t.get_declaration().spelling
        off = t.get_offset(name)
        if off < 0:
            return "unchecked"
        if tag and "(" not in tag and tag in dw:
            if name not in dw[tag]:
                return "disagree"
            if dw[tag][name] != off // 8:
                return "disagree"
            checked = True
        ft = None
        for f in t.get_fields():
            if f.spelling == name:
                ft = f.type
        if ft is None:
            # inside an anonymous member: stop checking deeper steps
            return "agree" if checked else "unchecked"
        t = ft.get_canonical()
    return "agree" if checked else "unchecked"


def must_match_mask(text):
    """Set of line numbers (1-based) inside an #if/#ifdef/#elif mentioning MUST_MATCH, any arm."""
    lines = text.split("\n")
    stack, masked = [], set()
    for i, line in enumerate(lines, 1):
        s = line.strip()
        m = re.match(r"#\s*(if|ifdef|ifndef|elif|else|endif)\b", s)
        if m:
            d = m.group(1)
            if d in ("if", "ifdef", "ifndef"):
                stack.append("MUST_MATCH" in s)
            elif d == "elif":
                if stack:
                    stack[-1] = stack[-1] or "MUST_MATCH" in s
            elif d == "endif":
                if stack:
                    stack.pop()
            continue
        if any(stack):
            masked.add(i)
    return masked


def define_mask(text):
    """Line numbers belonging to #define directives (with continuations)."""
    masked, cont = set(), False
    for i, line in enumerate(text.split("\n"), 1):
        if cont or re.match(r"\s*#\s*define\b", line):
            masked.add(i)
            cont = line.rstrip().endswith("\\")
        else:
            cont = False
    return masked


class Mac:
    """A file-local offset macro: its parameters and its body with whitespace normalised."""
    def __init__(self, name, params, body):
        self.name, self.params, self.body = name, params, body

    def subst(self, args):
        """The body with each parameter replaced by its argument, as the preprocessor does it (one pass)."""
        m = dict(zip(self.params, args))
        return re.sub(r"\b\w+\b", lambda t: m.get(t.group(0), t.group(0)), self.body)


def parse_macros(text):
    """{name: Mac} for the function-like #defines of one file whose body is an offset cast. A name defined twice is
    dropped (its uses are then left alone)."""
    lines = text.split("\n")
    found, seen = {}, Counter()
    i = 0
    while i < len(lines):
        mt = re.match(r"\s*#\s*define\s+(\w+)\(([^)]*)\)(.*)$", lines[i])
        i += 1
        if not mt:
            continue
        name = mt.group(1)
        params = [p.strip() for p in mt.group(2).split(",") if p.strip()]
        body = mt.group(3)
        while body.rstrip().endswith("\\") and i < len(lines):
            body = body.rstrip()[:-1] + " " + lines[i]
            i += 1
        seen[name] += 1
        m = MACRO_BODY.fullmatch(re.sub(r"\s+", " ", body).strip())
        if m and m.group("cast").endswith("*") and m.group("base") in params and m.group("off") in params:
            found[name] = Mac(name, params, re.sub(r"\s+", " ", body).strip())
    return {n: m for n, m in found.items() if seen[n] == 1}


def match_args(text, open_idx):
    """(args, index of the closing paren) for the call whose '(' is at open_idx, or None."""
    depth, args, start = 0, [], open_idx + 1
    for i in range(open_idx, len(text)):
        ch = text[i]
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
            if depth == 0:
                last = text[start:i].strip()
                if args or last:
                    args.append(last)
                return args, i
        elif ch == "," and depth == 1:
            args.append(text[start:i].strip())
            start = i + 1
    return None


class Exp:
    """One offset-macro use in a file: its call text and the byte spans of the call (o0, o1) in the original and of
    its expansion (s0, s1, with the body ending at body_end) in the expanded text."""
    def __init__(self, name, line, call, s0, body_end, s1, o0, o1):
        self.name, self.line, self.call = name, line, call
        self.s0, self.body_end, self.s1, self.o0, self.o1 = s0, body_end, s1, o0, o1


def expand_macros(text, macros):
    """(expanded bytes, [Exp], [(line, call, name)] for uses on MUST_MATCH lines). Each use outside #define lines and
    comments is replaced by its expansion; newlines of a multi-line call are kept so line numbers do not move."""
    if not macros:
        return text.encode("utf-8", errors="surrogateescape"), [], []
    enc = functools.partial(str.encode, encoding="utf-8", errors="surrogateescape")
    mm, defs = must_match_mask(text), define_mask(text)
    pat = re.compile(r"\b(" + "|".join(sorted(map(re.escape, macros), key=len, reverse=True)) + r")\s*\(")
    out, exps, masked = [], [], []
    so = oo = done = 0
    for m in pat.finditer(text):
        i = m.start()
        if i < done:
            continue
        line = text.count("\n", 0, i) + 1
        if line in defs:
            continue
        pre = text[text.rfind("\n", 0, i) + 1:i]
        if "//" in pre or pre.count('"') % 2:
            continue
        call = match_args(text, m.end() - 1)
        if call is None:
            continue
        args, close = call
        mac = macros[m.group(1)]
        if len(args) != len(mac.params):
            continue
        ctext = text[i:close + 1]
        if line in mm:
            masked.append((line, ctext, mac.name))
            continue
        core = mac.subst(args)
        body = core + "\n" * max(0, ctext.count("\n") - core.count("\n"))
        seg, eb, ob = enc(text[done:i]), enc(body), enc(ctext)
        out.append(seg)
        so, oo = so + len(seg), oo + len(seg)
        exps.append(Exp(mac.name, line, ctext, so, so + len(enc(core)), so + len(eb), oo, oo + len(ob)))
        out.append(eb)
        so, oo = so + len(eb), oo + len(ob)
        done = close + 1
    out.append(enc(text[done:]))
    return b"".join(out), exps, masked


def touches_union(t, off, depth=0):
    """True when byte `off` of record t lies inside a union member, at any depth."""
    if depth > 8:
        return False
    t = t.get_canonical()
    union = t.get_declaration().kind == K.UNION_DECL
    for name, foff, ftype, bitfield, bitrem in fields_of(t):
        ct = ftype.get_canonical()
        size = ct.get_size()
        if size <= 0 or not (foff <= off < foff + size):
            continue
        if union:
            return True
        rel = off - foff
        if ct.kind == ci.TypeKind.CONSTANTARRAY:
            et = ct.get_array_element_type().get_canonical()
            if et.get_size() <= 0:
                continue
            rel %= et.get_size()
            ct = et
        if is_record(ct) and touches_union(ct, rel, depth + 1):
            return True
    return False


def drop_dead_macros(text, names):
    """Remove the #define of each name in `names` that no longer appears outside #define lines. (new text, [names])."""
    lines = text.split("\n")
    blocks = {}
    for idx, line in enumerate(lines):
        mt = re.match(r"\s*#\s*define\s+(\w+)\(", line)
        if mt and mt.group(1) in names:
            end = idx
            while lines[end].rstrip().endswith("\\") and end + 1 < len(lines):
                end += 1
            blocks[mt.group(1)] = (idx, end)
    if not blocks:
        return text, []
    block_lines = {k for a, b in blocks.values() for k in range(a, b + 1)}
    rest = "\n".join(l for k, l in enumerate(lines) if k not in block_lines)
    dead = {n for n in blocks if not re.search(r"\b" + re.escape(n) + r"\b", rest)}
    drop = {k for n in dead for k in range(blocks[n][0], blocks[n][1] + 1)}
    return "\n".join(l for k, l in enumerate(lines) if k not in drop), sorted(dead)


def strip_parens(c):
    while c.kind == K.PAREN_EXPR:
        kids = list(c.get_children())
        if len(kids) != 1:
            break
        c = kids[0]
    return c


def last_child(c):
    kids = list(c.get_children())
    return kids[-1] if kids else None


def unqual(t):
    """Canonical spelling without top-level const (volatile kept, so it never matches a plain type)."""
    t = t.get_canonical()
    s = t.spelling
    s = re.sub(r"^const\s+", "", s)
    s = re.sub(r"\s+const$", "", s)
    return s


def is_volatile(t):
    return t.get_canonical().is_volatile_qualified()


def is_record(t):
    t = t.get_canonical()
    return t.kind == ci.TypeKind.RECORD and t.get_size() > 0


def fields_of(t):
    """[(name or None, byte offset, type, is_bitfield)] of a record type, anonymous members named None."""
    out = []
    for f in t.get_canonical().get_fields():
        off = f.get_field_offsetof()
        if off < 0:
            continue
        name = f.spelling
        anon = "(anonymous" in name or "(unnamed" in name
        out.append((None if anon else name, off // 8, f.type, f.is_bitfield(), off % 8))
    return out


def find_paths(t, off, want, depth=0):
    """All member paths (lists of suffix strings) inside record type t at byte offset `off` whose type is `want`."""
    if depth > 8:
        return []
    paths = []
    for name, foff, ftype, bitfield, bitrem in fields_of(t):
        if bitfield or bitrem:
            continue
        size = ftype.get_canonical().get_size()
        if size <= 0:
            continue
        if not (foff <= off < foff + size):
            continue
        seg = [] if name is None else ["." + name]
        rel = off - foff
        paths += match_in(ftype, rel, want, seg, depth)
    return paths


def match_in(ftype, rel, want, seg, depth):
    ct = ftype.get_canonical()
    found = []
    hit = want(ct) if callable(want) else unqual(ct) == want
    if rel == 0 and not is_volatile(ct) and hit:
        found.append((seg, ct))
        return found
    if ct.kind == ci.TypeKind.CONSTANTARRAY:
        et = ct.get_array_element_type()
        esz = et.get_canonical().get_size()
        if esz > 0:
            idx, erel = divmod(rel, esz)
            found += match_in(et, erel, want, seg + [f"[{idx}]"], depth + 1)
    elif is_record(ct):
        for p, pt in find_paths(ct, rel, want, depth + 1):
            found.append((seg + p, pt))
    return found


def array_field(t, off, elem):
    """Paths to an array field starting at `off` whose element type is `elem` (for p[i] indexing)."""
    res = []
    for name, foff, ftype, bitfield, bitrem in fields_of(t):
        ct = ftype.get_canonical()
        size = ct.get_size()
        if bitfield or size <= 0 or not (foff <= off < foff + size):
            continue
        seg = [] if name is None else ["." + name]
        if foff == off and ct.kind == ci.TypeKind.CONSTANTARRAY:
            et = ct.get_array_element_type()
            if unqual(et) == elem and not is_volatile(et):
                res.append(seg)
                continue
        if is_record(ct):
            res += [seg + p for p in array_field(ct, off - foff, elem)]
    return res


def render(base_text, segs):
    """`base->a.b[2]` from the base expression text and member segments (first segment starts with '.')."""
    path = "".join(segs)
    if path.startswith("."):
        path = path[1:]
    b = base_text.strip()
    while b.startswith("(") and b.endswith(")") and balanced(b[1:-1]):
        b = b[1:-1].strip()
    m = ADDR_SIMPLE.fullmatch(b)
    if m:
        return f"{m.group(1)}.{path}"
    if SIMPLE.fullmatch(b) or postfix(b):
        return f"{b}->{path}"
    return f"({b})->{path}"


def postfix(b):
    """True when b is an identifier followed only by calls, indexes and member accesses (`f(x)[2].y`)."""
    m = re.match(r"[A-Za-z_]\w*", b)
    if m:
        i = m.end()
    elif b.startswith("("):
        # a parenthesised primary expression, which must be followed by a postfix operator (else it's a cast)
        d, i = 0, 0
        while i < len(b):
            d += {"(": 1, ")": -1}.get(b[i], 0)
            i += 1
            if d == 0:
                break
        if d or not re.match(r"\s*(\[|->|\.)", b[i:]):
            return False
    else:
        return False
    while i < len(b):
        if b[i].isspace():
            i += 1
        elif b[i] in "([":
            close = {"(": ")", "[": "]"}[b[i]]
            d, j = 0, i
            while j < len(b):
                if b[j] in "([":
                    d += 1
                elif b[j] in ")]":
                    d -= 1
                    if d == 0:
                        break
                j += 1
            if j >= len(b) or b[j] != close:
                return False
            i = j + 1
        else:
            m = re.match(r"(->|\.)\s*[A-Za-z_]\w*", b[i:])
            if not m:
                return False
            i += m.end()
    return True


def balanced(s):
    d = 0
    for ch in s:
        if ch == "(":
            d += 1
        elif ch == ")":
            d -= 1
            if d < 0:
                return False
    return d == 0


def pt_is_ptr(t):
    return t.get_canonical().kind == ci.TypeKind.POINTER


def is_lvalue(node, parents):
    """True when `node` (an expression) is written: assignment target, ++/--, or has its address taken."""
    i = len(parents) - 1
    child = node
    while i >= 0 and parents[i].kind == K.PAREN_EXPR:
        child = parents[i]
        i -= 1
    if i < 0:
        return False
    par = parents[i]
    kids = list(par.get_children())
    if par.kind in (K.BINARY_OPERATOR, K.COMPOUND_ASSIGNMENT_OPERATOR) and len(kids) == 2:
        if kids[0].hash != child.hash:
            return False
        toks = [t.spelling for t in par.get_tokens()]
        lhs_toks = len(list(kids[0].get_tokens()))
        op = toks[lhs_toks] if lhs_toks < len(toks) else ""
        return op.endswith("=") and op not in ("==", "!=", "<=", ">=")
    if par.kind == K.UNARY_OPERATOR:
        toks = [t.spelling for t in par.get_tokens()]
        return bool(toks) and (toks[0] in ("&", "++", "--") or toks[-1] in ("++", "--"))
    return False


class Site:
    """One (u8*) cast: outcome 'ok' (with replacement text `new` for the byte span `span`) or why it was skipped."""
    def __init__(self, path, line, text, outcome, new=None, span=None):
        self.path, self.line, self.text, self.outcome, self.new, self.span = path, line, text, outcome, new, span
        self.macro = None  # the offset macro's name when the cast came from one
        self.at = None     # byte offset of the cast in the analysed text


def analyse_text(rel, text, exps=()):
    """Sites in one source text, with the offset-macro expansions `exps` already in it (see analyse)."""
    path = ROOT / rel
    raw = text.encode("utf-8", errors="surrogateescape")
    tu = parse(rel, text)
    mm, defs = must_match_mask(text), define_mask(text)
    main = str(path)

    def src(c):
        e = c.extent
        return raw[e.start.offset:e.end.offset].decode("utf-8", errors="replace")

    sites = []

    def visit(c, parents):
        # Extents of code from a macro expansion cover the macro call, so a cast that comes from (or ends in) a
        # macro never shows up as `(u8*)...` / `(T*)...` source text below and is skipped.
        if c.kind == K.CSTYLE_CAST_EXPR and c.location.file and c.location.file.name == main:
            t = src(c)
            if U8_CAST.match(t):
                site = classify(c, parents)
                site.at = c.extent.start.offset
                sites.append(site)
        parents.append(c)
        for ch in c.get_children():
            visit(ch, parents)
        parents.pop()

    def classify(u8cast, parents):
        line = u8cast.extent.start.line
        here = src(u8cast)

        def skip(why):
            return Site(rel, line, here, why)

        if line in defs:
            return skip("macro")
        # u8cast must be the LHS of `+ literal`
        i = len(parents) - 1
        while i >= 0 and parents[i].kind == K.PAREN_EXPR:
            i -= 1
        if i < 0 or parents[i].kind != K.BINARY_OPERATOR:
            return skip("not a base + offset cast")
        binop = parents[i]
        kids = list(binop.get_children())
        if len(kids) != 2 or (strip_parens(kids[0]).hash != u8cast.hash and kids[0].hash != u8cast.hash):
            return skip("not a base + offset cast")
        op = raw[kids[0].extent.end.offset:kids[1].extent.start.offset].decode(errors="replace").strip()
        if op != "+":
            return skip("not a base + offset cast")
        if line in mm or binop.extent.end.line in mm:
            return skip("MUST_MATCH")
        lit = src(kids[1]).strip()
        while lit.startswith("(") and lit.endswith(")"):
            lit = lit[1:-1].strip()
        m = INT_LIT.fullmatch(lit)
        if not m:
            return skip("offset not a literal")
        off = int(m.group(1), 0)
        base = last_child(u8cast)
        if base is None:
            return skip("not a base + offset cast")
        bt = base.type.get_canonical()
        if bt.kind != ci.TypeKind.POINTER:
            return skip("base not a pointer")
        rec = bt.get_pointee()
        if not is_record(rec):
            return skip("untyped base")
        if rec.get_canonical().get_declaration().spelling in SKIP_BASES:
            return skip("base type excluded (--skip-base)")
        # outer cast
        j = i - 1
        while j >= 0 and parents[j].kind == K.PAREN_EXPR:
            j -= 1
        if j < 0 or parents[j].kind != K.CSTYLE_CAST_EXPR:
            return skip("no outer cast (byte pointer)")
        outer = parents[j]
        if not re.match(r"\(\s*[^()]*\*\s*\)", src(outer)):
            return skip("outer cast not a pointer")
        pt = outer.type.get_canonical()
        if pt.kind != ci.TypeKind.POINTER:
            return skip("outer cast not a pointer")
        want_t = pt.get_pointee()
        if is_volatile(want_t):
            return skip("volatile")
        want = unqual(want_t)
        if touches_union(rec, off):
            return skip("union: needs a human")
        base_text = src(base)
        # how the cast pointer is used (through parens and implicit conversions)
        k = j - 1
        while k >= 0 and (parents[k].kind == K.PAREN_EXPR
                          or parents[k].kind == K.UNEXPOSED_EXPR and len(list(parents[k].get_children())) == 1):
            k -= 1
        use = parents[k] if k >= 0 else None
        usetxt = ""
        if use is not None and use.kind == K.UNARY_OPERATOR:
            ukids = list(use.get_children())
            usetxt = raw[use.extent.start.offset:ukids[0].extent.start.offset].decode(errors="replace").strip() \
                if ukids else ""
            if not usetxt:
                usetxt = "postfix"
        if use is not None and use.kind == K.ARRAY_SUBSCRIPT_EXPR:
            ukids = list(use.get_children())
            if strip_parens(ukids[0]).hash != outer.hash:
                return skip("pointer arithmetic")
            arr = array_field(rec, off, want)
            if len(arr) != 1:
                return skip("indexed, no array field")
            if any(PLACEHOLDER.match(sg.lstrip(".")) for sg in arr[0] if sg.startswith(".")):
                return skip("lands in padding/unk field")
            if check_layout(rel, rec, arr[0]) == "disagree":
                return skip("clang/MWCC layouts disagree")
            head = render(base_text, arr[0])
            start = use.extent.start.offset
            end = ukids[0].extent.end.offset
            return Site(rel, line, src(use), "ok", head, (start, end))
        paths = find_paths(rec, off, want)
        retype = None
        in_macro = any(e.s0 <= u8cast.extent.start.offset < e.s1 for e in exps)
        if not paths and not in_macro and pt_is_ptr(want_t) and use is not None and use.kind == K.UNARY_OPERATOR \
                and usetxt == "*" and not is_lvalue(use, parents[:k]):
            # a pointer read through a different pointer type: `(T*)base->field` reads the same word
            paths = [p for p in find_paths(rec, off, lambda ct: ct.kind == ci.TypeKind.POINTER)
                     if not is_volatile(p[1].get_pointee())]
            if paths:
                retype = re.sub(r"\s+", " ", src(outer)[1:src(outer).index(")")]).strip()
                retype = re.sub(r"\s*\*$", "", retype)
        if not paths:
            return skip(classify_miss(rec, off, want))
        segs = paths[0][0]
        if not segs:
            return skip("offset 0 is the base itself")
        if any(PLACEHOLDER.match(sg.lstrip(".")) for sg in segs if sg.startswith(".")):
            return skip("lands in padding/unk field")
        lay = check_layout(rel, rec, segs)
        if lay == "disagree":
            return skip("clang/MWCC layouts disagree")
        acc = render(base_text, segs)
        if retype:
            e = use.extent
            new = f"({retype}){acc}"
            par = parents[k - 1] if k >= 1 else None
            if par is not None and par.kind in (K.UNARY_OPERATOR, K.ARRAY_SUBSCRIPT_EXPR, K.MEMBER_REF_EXPR,
                                                K.CALL_EXPR, K.CXX_UNARY_EXPR):
                new = f"({new})"
            return Site(rel, line, src(use), "ok", new, (e.start.offset, e.end.offset))
        if use is not None and use.kind == K.UNARY_OPERATOR and usetxt == "*":
            e = use.extent
            # (*(T*)(...))++  ->  base->field++ : a postfix expression needs no parens of its own
            # (but keep them after a keyword or identifier: sizeof(*(T*)(...)))
            if k >= 1 and parents[k - 1].kind == K.PAREN_EXPR:
                before = raw[:parents[k - 1].extent.start.offset].rstrip()[-1:]
                if not (before.isalnum() or before == b"_"):
                    e = parents[k - 1].extent
            return Site(rel, line, src(use), "ok", acc, (e.start.offset, e.end.offset))
        if use is not None and use.kind == K.MEMBER_REF_EXPR:
            # ((T*)(...))->m  ->  base->field.m ; replace up to and including the arrow
            first = list(use.get_children())[0]
            fe = first.extent.end.offset
            rest = raw[fe:use.extent.end.offset].decode(errors="replace")
            mm_ = re.match(r"\s*->", rest)
            if not mm_:
                return skip("member access through '.'")
            return Site(rel, line, src(use), "ok", acc + ".",
                        (first.extent.start.offset, fe + mm_.end()))
        binop_txt = ""
        if use is not None and use.kind in (K.BINARY_OPERATOR, K.COMPOUND_ASSIGNMENT_OPERATOR):
            ukids = list(use.get_children())
            binop_txt = raw[ukids[0].extent.end.offset:ukids[1].extent.start.offset].decode(errors="replace").strip()
            if binop_txt in ("+", "-", "+=", "-="):
                return skip("pointer arithmetic")
        if use is not None and use.kind == K.UNARY_OPERATOR and usetxt in ("++", "--", "postfix"):
            return skip("pointer arithmetic")
        if use is not None and (use.kind == K.VAR_DECL or binop_txt == "="):
            # p = (T*)(...): later code may step through it past the field (a strided table view)
            return skip("address stored in a variable")
        e = outer.extent
        # '&a->b' is a unary expression, like the cast it replaces
        return Site(rel, line, src(outer), "ok", "&" + acc, (e.start.offset, e.end.offset))

    visit(tu.cursor, [])
    return sites, raw


def analyse(rel, text=None):
    """Sites in one file, with the file's raw bytes. Offset-macro uses are analysed as their expansions, and a site
    inside one is reported at the call (text, line, macro) and rewritten as a whole call, or left alone when its
    span is only part of the expansion."""
    if text is None:
        text = (ROOT / rel).read_text(errors="replace")
    raw = text.encode("utf-8", errors="surrogateescape")
    shadow, exps, masked = expand_macros(text, parse_macros(text))
    sites, _ = analyse_text(rel, shadow.decode("utf-8", errors="surrogateescape"), exps)

    def owner(p):
        return next((e for e in exps if e.s0 <= p < e.s1), None)

    def orig_pos(p):
        return p - sum((e.s1 - e.s0) - (e.o1 - e.o0) for e in exps if e.s1 <= p)

    hit, out = set(), []
    for s in sites:
        e = owner(s.at)
        if e is None:
            if s.outcome == "ok":
                a, b = s.span
                if any(x.s0 < b and a < x.s1 for x in exps):
                    s.outcome, s.new, s.span = "macro: partial use", None, None
                else:
                    s.span = (orig_pos(a), orig_pos(b))
            out.append(s)
            continue
        hit.add(id(e))
        s.line, s.text, s.macro = e.line, e.call, e.name
        if s.outcome == "ok":
            a, b = s.span
            if e.s0 <= a < b <= e.s1 and shadow[e.s0:a].strip() in (b"", b"(") \
                    and shadow[b:e.body_end].strip() in (b"", b")"):
                s.span = (e.o0, e.o1)
            else:
                s.outcome, s.new, s.span = "macro: partial use", None, None
        out.append(s)
    for e in exps:
        if id(e) not in hit:
            out.append(Site(rel, e.line, e.call, "not analysed by clang"))
            out[-1].macro = e.name
    for line, call, name in masked:
        out.append(Site(rel, line, call, "MUST_MATCH"))
        out[-1].macro = name
    return out, raw


def classify_miss(rec, off, want):
    """Why no field matched: offset in padding/unnamed space, mid-field, or a type mismatch."""
    hits = []
    for name, foff, ftype, bitfield, bitrem in fields_of(rec):
        size = ftype.get_canonical().get_size()
        if size > 0 and foff <= off < foff + size:
            hits.append((name, foff, ftype, bitfield))
    if not hits:
        return "no field at offset"
    for name, foff, ftype, bitfield in hits:
        if bitfield:
            return "bitfield"
    if any(n and PLACEHOLDER.match(n) for n, *_ in hits):
        return "lands in padding/unk field"
    # walk down to the innermost field containing off
    leaf = innermost(rec, off)
    if leaf and leaf[0] != 0:
        return "mid-field"
    return "type mismatch"


def innermost(t, off, depth=0):
    for name, foff, ftype, bitfield, bitrem in fields_of(t):
        ct = ftype.get_canonical()
        size = ct.get_size()
        if size > 0 and foff <= off < foff + size:
            rel = off - foff
            if ct.kind == ci.TypeKind.CONSTANTARRAY:
                et = ct.get_array_element_type().get_canonical()
                if et.get_size() > 0:
                    rel %= et.get_size()
                    if is_record(et) and depth < 8:
                        return innermost(et, rel, depth + 1) or (rel, et)
                    return (rel, et)
            if is_record(ct) and depth < 8:
                return innermost(ct, rel, depth + 1) or (rel, ct)
            return (rel, ct)
    return None


def rewrite(raw, sites):
    """Apply non-overlapping 'ok' sites (outermost-first conflicts resolved by dropping the later one)."""
    oks = sorted((s for s in sites if s.outcome == "ok"), key=lambda s: s.span[0])
    out, pos, applied = [], 0, []
    for s in oks:
        a, b = s.span
        if a < pos:
            s.outcome = "overlaps another rewrite"
            continue
        out.append(raw[pos:a])
        out.append(s.new.encode())
        pos = b
        applied.append(s)
    out.append(raw[pos:])
    return b"".join(out), applied


def collect(paths):
    files = []
    if not paths:
        paths = ["src"] + sorted(str(p.relative_to(ROOT)) for p in ROOT.glob("rels/*/src"))
    for p in paths:
        ap = (ROOT / p) if not Path(p).is_absolute() else Path(p)
        if not ap.resolve().is_relative_to(ROOT):
            sys.exit(f"{p}: not inside the repo ({ROOT})")
        if not ap.exists():
            sys.exit(f"{p}: not found")
        if ap.is_dir():
            files += sorted(ap.rglob("*.c"))
        elif ap.suffix == ".c":
            files.append(ap)
    return [f.resolve().relative_to(ROOT).as_posix() for f in files]


def fixpoint(rel, text):
    """Rewrite until nothing more converts (inner rewrites can expose outer ones).
    (new text, applied sites, names of offset macros whose #define was dropped)."""
    applied_all = []
    for _ in range(4):
        sites, raw = analyse(rel, text)
        new, applied = rewrite(raw, sites)
        if not applied:
            break
        applied_all += applied
        text = new.decode("utf-8", errors="surrogateescape")
    text, dropped = drop_dead_macros(text, {s.macro for s in applied_all if s.macro})
    return text, applied_all, dropped


def run_build(cmd):
    r = subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True)
    return r.returncode == 0, r.stdout[-2000:] + r.stderr[-2000:]


def verify(files, cmd, log):
    """Group-test the rewrites: build with a set applied; on failure split it, until single bad files are found."""
    orig, new, counts = {}, {}, {}
    for rel in files:
        text = (ROOT / rel).read_text(errors="surrogateescape")
        nt, applied, _ = fixpoint(rel, text)
        if applied:
            orig[rel], new[rel], counts[rel] = text, nt, len(applied)
    log(f"{len(new)} files with rewrites, {sum(counts.values())} sites")
    good, bad = [], []

    def write(rels, which):
        for r in rels:
            (ROOT / r).write_text(which[r], errors="surrogateescape")

    def test(group):
        write(group, new)
        ok, out = run_build(cmd)
        log(f"  build with {len(group)} file(s) {'OK' if ok else 'FAILED'}: {', '.join(group[:4])}"
            f"{' ...' if len(group) > 4 else ''}")
        if ok:
            good.extend(group)
            return
        write(group, orig)
        if len(group) == 1:
            bad.append(group[0])
            log("    " + out.strip().splitlines()[-1] if out.strip() else "")
            return
        mid = len(group) // 2
        test(group[:mid])
        test(group[mid:])

    ok, out = run_build(cmd)
    if not ok:
        sys.exit("baseline build fails; fix that first:\n" + out)
    test(sorted(new))
    ok, out = run_build(cmd)
    log(f"final build {'OK' if ok else 'FAILED'}")
    return good, bad, counts


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="*")
    ap.add_argument("--apply", action="store_true", help="rewrite the files in place")
    ap.add_argument("--verify", action="store_true", help="rewrite, build, revert files that break the build")
    ap.add_argument("--build-cmd", default="ninja", help="command that fails unless the build is byte-identical")
    ap.add_argument("--list", action="store_true", help="print every site")
    ap.add_argument("--only", help="with --list: only sites with this outcome")
    ap.add_argument("--dwarf", help="JSON from --dwarf-index: skip sites where clang's layout disagrees with MWCC's")
    ap.add_argument("--compile-commands", help="per-file include paths and defines (default: "
                    "compile_commands.json at the repo root, which configure.py writes; else built-in defaults)")
    ap.add_argument("--skip-base", action="append", default=[], metavar="TAG",
                    help="leave casts on this struct (tag name) alone; repeatable")
    ap.add_argument("--dwarf-index", metavar="OUT", help="(on a built tree, no libclang needed) write the "
                    "objects' struct layouts from their DWARF to OUT")
    a = ap.parse_args()
    if a.dwarf_index:
        dwarf_index(a.dwarf_index)
        return
    if ci is None:
        sys.exit("needs libclang: run as `uv run --with libclang tools/offset_to_field.py ...`")
    SKIP_BASES.update(a.skip_base)
    cc = Path(a.compile_commands) if a.compile_commands else ROOT / "compile_commands.json"
    if cc.exists():
        load_compile_commands(cc)
    elif a.compile_commands:
        sys.exit(f"{cc} not found")
    if a.dwarf:
        DWARF.update(json.loads(Path(a.dwarf).read_text()))
    # PATHs are taken relative to the repo root, and so are the compile flags' relative -I paths
    os.chdir(ROOT)
    files = collect(a.paths)
    if a.verify:
        good, bad, counts = verify(files, a.build_cmd, lambda s: print(s, flush=True))
        print(f"kept {len(good)} files ({sum(counts[r] for r in good)} sites); reverted {len(bad)}:")
        for r in bad:
            print(f"  {r} ({counts[r]} sites)")
        return
    tally, per_file, macro_tally = Counter(), Counter(), Counter()
    for rel in files:
        try:
            if a.apply:
                text = (ROOT / rel).read_text(errors="surrogateescape")
                nt, applied, dropped = fixpoint(rel, text)
                if applied:
                    (ROOT / rel).write_text(nt, errors="surrogateescape")
                    if dropped:
                        print(f"{rel}: dropped #define {', '.join(dropped)}", flush=True)
                sites, _ = analyse(rel, nt)
                sites = applied + [s for s in sites if s.outcome != "ok"]
            else:
                sites, raw = analyse(rel)
                rewrite(raw, sites)
        except ci.TranslationUnitLoadError as e:
            print(f"{rel}: clang failed: {e}", file=sys.stderr)
            continue
        for s in sites:
            tally[s.outcome] += 1
            if s.macro:
                macro_tally[s.outcome] += 1
            if s.outcome == "ok":
                per_file[rel] += 1
            if a.list and (not a.only or a.only == s.outcome):
                t = " ".join(s.text.split())
                print(f"{s.path}:{s.line}: [{s.outcome}] {t}" + (f"  ->  {s.new}" if s.outcome == "ok" else ""))
    total = sum(tally.values())
    print(f"{total} (u8*) casts in {len(files)} files")
    print(f"  {'convertible' if not a.apply else 'converted':32} {tally['ok']:6}")
    for k, v in tally.most_common():
        if k != "ok":
            print(f"  {k:32} {v:6}")
    if macro_tally:
        print(f"  of which offset-macro uses ({sum(macro_tally.values())}):")
        for k, v in macro_tally.most_common():
            print(f"    {k:30} {v:6}")
    if per_file:
        print("  top files:")
        for r, n in per_file.most_common(15):
            print(f"    {n:5}  {r}")


if __name__ == "__main__":
    main()
