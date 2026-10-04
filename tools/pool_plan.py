#!/usr/bin/env python3
"""Prescriptive plan for a unit's .sdata2/.rodata pool, and a loop that reports the first slot that is still wrong.

    uv run --with pyelftools python3 tools/pool_plan.py UNIT            plan: probe builds, then the per-entry table and actions
    uv run --with pyelftools python3 tools/pool_plan.py UNIT --apply    write the plan's float changes into the source, then diagnose
    uv run --with pyelftools python3 tools/pool_plan.py UNIT --check    rebuild, report the first mismatching slot and why

UNIT is the source path without src/ and .c (event/evt_item). Flags: --no-probe (static columns only, no builds),
--no-search (skip the per-constant builds), --no-trap-text (drop the trap lines). The source is restored after every
probe build; --apply is the only thing that keeps an edit (git diff shows it).

What it does
 1. Reads the original object (target .sdata2/.rodata with symbol names, and the relocations of .text/.data for the first
    function and the data object that use each entry).
 2. Builds the unit once with every float constant as a literal (and code-only strings as literals). The compiler's pool
    order from that build, compared with the target's, gives the smallest NAMED prefix (deferred unit: named objects land
    BEFORE the pooled literals) or suffix (nodefer unit: AFTER them) that reproduces the target. Unreferenced entries are
    always named; entries only data uses pool where that data is defined in the source.
 3. Functions whose score drops with literals need a constant by name: it builds once per candidate constant to find which
    single named constant restores them and extends the prefix to its slot. Constants the original loads twice in one basic
    block are read through RELOAD_F32 (a volatile read of the const object) in those functions; named objects the code reads
    once per block stay plain.
 4. Verifies the plan (builds with exactly that named set), prints identical / the first differing slot with its diagnosis:
    swapped pair (same function: operand order; different functions: function order), EXTRA entries (a second copy of a
    named value: use the object at these lines; a folded product or negation: keep one factor in a local), and entries ours
    LACKS (unreferenced: name it with force_active; magic int->float double: the conversion is missing).
Traps it flags per entry are the ones in docs/AGENT_BRIEF.md ("Data and pool"): cos() arguments fold to lfd doubles,
templates in inline bodies pool when the inline is defined, literal negations fold, named const objects are CSE-folded."""
import glob
import re
import struct
import subprocess
import sys

from elftools.elf.elffile import ELFFile

args = [a for a in sys.argv[1:] if not a.startswith("--")]
flags = {a for a in sys.argv[1:] if a.startswith("--")}
if not args:
    sys.exit(__doc__)
unit = args[0]
SECS = [s for s in (".rodata", ".sdata2")]
SMALL = {".sdata2"}

# ---------------------------------------------------------------- objects


def obj_paths():
    tp = glob.glob(f"build/G8MJ01/obj/{unit}.o") + glob.glob(f"build/G8MJ01/*/obj/{unit.split('/', 1)[-1]}.o")
    if "/" in unit:
        mod, rest = unit.split("/", 1)
        tp = glob.glob(f"build/G8MJ01/{mod}/obj/{rest}.o") + tp
    if not tp:
        sys.exit(f"no target object for {unit}")
    base = unit.split("/", 1)[1] if unit.split("/")[0] in REL_MODS else unit
    return tp[0], f"build/G8MJ01/src/{base}.o"


REL_MODS = {p.split("/")[-1] for p in glob.glob("rels/*")}
target_path, ours_path = obj_paths()
ours_name = ours_path[len("build/G8MJ01/src/"):-2]


def source_path():
    if unit.split("/")[0] in REL_MODS:
        mod, rest = unit.split("/", 1)
        return f"rels/{mod}/src/{rest}.c"
    return f"src/{unit}.c"


def unit_mode():
    """'nodefer' when the unit's configure.py Object compiles with -inline auto (no deferred)."""
    cfg = open("configure.py").read()
    name = unit.split("/", 1)[1] if unit.split("/")[0] in REL_MODS else unit
    m = re.search(r'Object\((?:Non)?Matching, "%s\.c"([^\n]*)' % re.escape(name), cfg)
    line = m.group(1) if m else ""
    return "nodefer" if "nodefer" in line else "deferred"


class Obj:
    def __init__(self, path):
        self.f = ELFFile(open(path, "rb"))
        self.sec = {}
        for s in self.f.iter_sections():
            if s["sh_type"] != "SHT_NOBITS":
                self.sec[s.name] = s.data()
        st = self.f.get_section_by_name(".symtab")
        self.syms = list(st.iter_symbols())
        self.symsec = {}
        for i, s in enumerate(self.syms):
            if isinstance(s["st_shndx"], int):
                self.symsec[i] = self.f.get_section(s["st_shndx"]).name

    def objects(self, sec):
        """[(addr, size, name)] named objects of a section, address order, sizes extended to the next object."""
        out = []
        for i, s in enumerate(self.syms):
            if self.symsec.get(i) == sec and s.name and s["st_info"]["type"] in ("STT_OBJECT", "STT_NOTYPE"):
                out.append((s["st_value"], s["st_size"], s.name))
        out.sort()
        res = []
        total = len(self.sec.get(sec, b""))
        seen = set()
        for k, (a, sz, n) in enumerate(out):
            if a in seen:
                continue
            seen.add(a)
            nxt = next((x[0] for x in out[k + 1:] if x[0] > a), total)
            res.append((a, nxt - a, n, sz))
        return res

    def relocs(self, secname):
        r = self.f.get_section_by_name(".rela" + secname)
        if r is None:
            return []
        out = []
        for rel in r.iter_relocations():
            sym = self.syms[rel["r_info_sym"]]
            tgt_sec = self.symsec.get(rel["r_info_sym"])
            out.append((rel["r_offset"], rel["r_info_type"], sym.name, tgt_sec, sym["st_value"] + rel["r_addend"]))
        return out

    def funcs(self):
        out = []
        for i, s in enumerate(self.syms):
            if self.symsec.get(i) == ".text" and s["st_info"]["type"] == "STT_FUNC":
                out.append((s["st_value"], s["st_size"], s.name))
        out.sort()
        return out


# ---------------------------------------------------------------- kinds


def kind_of(sec, name, data):
    n = name.lower()
    if n.startswith("double_to_int"):
        return "magic"
    if n.startswith("float_"):
        return "f32"
    if n.startswith("double_"):
        return "f64"
    if n.startswith("str_"):
        return "str"
    if n.startswith("vec3_"):
        return "vec3"
    if n.startswith("dat_") or n.startswith("unk_"):
        return "u32"
    if n.startswith("gap_") or n.startswith("pad_"):
        return "gap"
    if n.startswith("jumptable"):
        return "jumptable"
    if len(data) == 4:
        return "f32"
    if len(data) == 8 and data[:2] == b"\x43\x30":
        return "magic"
    if len(data) == 8:
        return "f64"
    if len(data) == 12:
        return "vec3"
    return "bytes"


def show(kind, data):
    if kind == "f32" and len(data) >= 4:
        return "%g" % struct.unpack(">f", data[:4])[0]
    if kind in ("f64", "magic") and len(data) >= 8:
        return "%g (double)" % struct.unpack(">d", data[:8])[0]
    if kind == "vec3" and len(data) >= 12:
        return "(%g, %g, %g)" % struct.unpack(">3f", data[:12])
    if kind == "str":
        return repr(data.split(b"\0")[0].decode("latin1"))
    if kind == "u32":
        return "0x" + data[:4].hex()
    return data.hex()[:24]


# ---------------------------------------------------------------- uses

FLOAT_LOADS = {48, 49, 50, 51}
BRANCH_OPS = {16, 18, 19}


def blocks_of(text, fstart, fend):
    """block id for every instruction word offset of a function (a branch ends a block; so does a branch target)."""
    targets = set()
    for off in range(fstart, fend, 4):
        w = struct.unpack(">I", text[off:off + 4])[0]
        op = w >> 26
        if op == 18 and not (w & 2):
            li = w & 0x03FFFFFC
            li = li - (1 << 26) if li & (1 << 25) else li
            targets.add(off + li)
        elif op == 16 and not (w & 2):
            bd = w & 0xFFFC
            bd = bd - (1 << 16) if bd & 0x8000 else bd
            targets.add(off + bd)
    out = {}
    blk = 0
    for off in range(fstart, fend, 4):
        if off in targets:
            blk += 1
        out[off] = blk
        w = struct.unpack(">I", text[off:off + 4])[0]
        if (w >> 26) in BRANCH_OPS:
            blk += 1
    return out


def analyse_uses(tobj, objs_by_addr):
    """uses[(sec, addr)] = list of dict(func, pos, kind) from .text relocs, plus data owners."""
    text = tobj.sec.get(".text", b"")
    funcs = tobj.funcs()

    def func_of(off):
        for i, (a, sz, n) in enumerate(funcs):
            if a <= off < a + sz:
                return i, n
        return None, None

    blkcache = {}
    uses = {}
    data_uses = {}
    for off, typ, name, tsec, taddr in tobj.relocs(".text"):
        if tsec not in SECS:
            continue
        ioff = off & ~3
        fi, fn = func_of(ioff)
        if fi is None:
            continue
        w = struct.unpack(">I", text[ioff:ioff + 4])[0]
        op = w >> 26
        if fi not in blkcache:
            a, sz, _ = funcs[fi]
            blkcache[fi] = blocks_of(text, a, a + sz)
        uses.setdefault((tsec, taddr), []).append(
            {"func": fi, "fname": fn, "pos": ioff, "load": op in FLOAT_LOADS, "block": blkcache[fi].get(ioff, 0), "op": op, "typ": typ}
        )
    for dsec in (".data", ".sdata", ".rodata", ".sdata2"):
        owners = [(a, sz, n) for a, sz, n, _ in tobj.objects(dsec)] if dsec in tobj.sec else []
        for off, typ, name, tsec, taddr in tobj.relocs(dsec):
            if tsec not in SECS:
                continue
            owner = next((n for a, sz, n in owners if a <= off < a + max(sz, 1)), "?")
            data_uses.setdefault((tsec, taddr), []).append((dsec, owner))
    return uses, data_uses, funcs


def calls_after(tobj, funcs, use):
    """name of the first call within 6 instructions after this use (cos/sin/tan fold traps)."""
    text = tobj.sec[".text"]
    calls = {o: n for o, t, n, s, a in tobj.relocs(".text") if t == 10}
    for k in range(1, 7):
        o = use["pos"] + 4 * k
        if o in calls or (o & ~3) in calls:
            return calls.get(o) or calls.get(o & ~3)
    return None


# ---------------------------------------------------------------- plan


def build_entries(tobj, sec, uses, data_uses):
    data = tobj.sec.get(sec, b"")
    res = []
    for addr, size, name, _ in tobj.objects(sec):
        blob = data[addr:addr + size]
        kind = kind_of(sec, name, blob)
        u = sorted(uses.get((sec, addr), []), key=lambda x: (x["func"], x["pos"]))
        res.append({"sec": sec, "addr": addr, "size": size, "name": name, "kind": kind, "data": blob, "uses": u, "duses": data_uses.get((sec, addr), [])})
    return res


def classify(entries, mode):
    """Mark each entry NAMED / LITERAL / MAGIC / DATA / PAD and compute the named prefix (deferred) or suffix (nodefer)."""
    for e in entries:
        if e["kind"] in ("gap",):
            e["cls"] = "PAD"
        elif e["kind"] == "magic":
            e["cls"] = "MAGIC"
        elif not e["uses"] and e["duses"]:
            e["cls"] = "DATA"
        elif not e["uses"]:
            e["cls"] = "UNREF"
        else:
            e["cls"] = "LIT"
    seq = [e for e in entries if e["cls"] != "PAD"]

    def tail_ok(items):
        """items must be pooled by the compiler in this order: text-used entries nondecreasing by (func, pos); no UNREF."""
        last = None
        bad = None
        for e in items:
            if e["cls"] in ("UNREF",):
                return False, e
            if e["cls"] in ("DATA", "MAGIC"):
                continue
            key = (e["uses"][0]["func"], e["uses"][0]["pos"])
            if last is not None and key < last[0]:
                return False, e
            last = (key, e)
        return True, None

    n = len(seq)
    if mode == "deferred":
        for k in range(n + 1):
            ok, why = tail_ok(seq[k:])
            # a MAGIC entry cannot be a named object: the prefix must stop before the first one
            if ok and not any(e["cls"] == "MAGIC" for e in seq[:k]):
                return seq, k, "prefix"
        return seq, n, "prefix"
    for k in range(n + 1):  # nodefer: literal head seq[:n-k], named suffix seq[n-k:]
        ok, why = tail_ok(seq[: n - k])
        if ok and not any(e["cls"] == "MAGIC" for e in seq[n - k:]):
            return seq, k, "suffix"
    return seq, n, "suffix"


def reload_info(e):
    """(max loads of this object in one block of one function, function names where >=2)."""
    per = {}
    for u in e["uses"]:
        if u["load"]:
            per[(u["func"], u["block"])] = per.get((u["func"], u["block"]), 0) + 1
    mx = max(per.values(), default=0)
    fns = sorted({e2["fname"] for e2 in e["uses"] for (f, b), c in per.items() if c >= 2 and e2["func"] == f})
    return mx, fns


def traps(e, entries, tobj, funcs):
    out = []
    k = e["kind"]
    vals = {}
    for o in entries:
        if o["kind"] == "f32" and len(o["data"]) >= 4:
            vals[o["name"]] = struct.unpack(">f", o["data"][:4])[0]
    if k == "magic":
        out.append("int->float conversion constant: the compiler emits it at the first conversion; never define it")
    if k == "f32" and len(e["data"]) >= 4:
        v = struct.unpack(">f", e["data"][:4])[0]
        for u in e["uses"]:
            c = calls_after(tobj, funcs, u)
            if c in ("cos", "sin", "tan", "sqrt", "pow", "fabs", "acos", "asin", "fmod", "floor"):
                out.append(f"argument of {c}() in {u['fname']}: a literal/static const there folds to an lfd of a double (an extra 8 bytes in .rodata); pass an f32 local instead")
                break
        if v != 0 and any(abs(v + w) < 1e-12 * max(abs(w), 1) and o != e["name"] for o, w in vals.items()):
            out.append("negation of another pool constant: a literal -x folds to its own entry, `-local` does not; the target pooled the negative itself")
        if v != 0 and 0 < abs(v) < 1:
            inv = 1 / abs(v)
            if abs(inv - round(inv)) < 1e-9 and round(inv) >= 2 and (round(inv) & (round(inv) - 1)) == 0:
                out.append(f"1/{round(inv)}: probably the compiler's strength-reduced reciprocal of `/ {round(inv)}.0f`; write the division, do not name it")
    if k == "vec3" and e["sec"] == ".rodata":
        out.append("Vec template: a `(Vec){..}` literal or named const Vec; templates in inline bodies pool when the inline is DEFINED, not called")
    if k == "str" and e["sec"] == ".sdata2":
        out.append("string of 6 bytes or less lives in .sdata2 (-sdata2 6): a plain string literal, in the pool's code order")
    if e["cls"] == "UNREF":
        out.append("no code or data references it: it exists only as a named object (force_active if the unit links), so it must be in the named prefix; dead constants vanish before pooling")
    if e["cls"] == "DATA":
        owners = sorted({o for s, o in e["duses"]})[:3]
        out.append("only .data/.sdata objects use it (" + ", ".join(owners) + "): it pools where that data is defined in the source")
    return out


FLOAT_DEF = (r"^(?:extern )?(?:__declspec\([^)]*\) )?(?:const )?(?:(?:MATCH_VOLATILE|volatile) )?f32 {n}(?: = [^;]+)?;[ \t]*(?://[^\n]*)?\n")


def f32_literal(blob):
    (v,) = struct.unpack(">f", blob[:4])
    for p in range(1, 10):
        c = "%.*g" % (p, v)
        if struct.unpack(">f", struct.pack(">f", float(c)))[0] == v:
            break
    else:
        c = repr(v)
    if "e" in c:
        c = ("%.20f" % float(c)).rstrip("0")
    if "." not in c:
        c += ".0"
    return c + "f"


def reload_map(tobj):
    """{constant name: {function names where retail loads it twice inside one basic block}} (reload semantics there)."""
    uses, du, funcs = analyse_uses(tobj, None)
    out = {}
    for e in build_entries(tobj, ".sdata2", uses, du):
        if e["kind"] == "f32":
            mx, fns = reload_info(e)
            if mx >= 2:
                out[e["name"]] = set(fns)
    return out


def reload_names(tobj):
    uses, du, funcs = analyse_uses(tobj, None)
    out = set()
    for e in build_entries(tobj, ".sdata2", uses, du):
        if e["kind"] == "f32" and reload_info(e)[0] >= 2:
            out.add(e["name"])
    return out


def literalise(text, tobj, keep=(), nonconst=(), reload=None):
    """Source text with every float_* pool constant of the unit turned into a literal at its uses (except `keep`)."""
    names = {}
    data = tobj.sec.get(".sdata2", b"")
    for addr, size, name, _ in tobj.objects(".sdata2"):
        if name.startswith("float_") and size >= 4 and size == 4:
            names[name] = f32_literal(data[addr:addr + 4])
    # strings only code uses become plain string literals too (named string objects land before the pool)
    code_syms = {nm for off, typ, nm, tsec, ta in tobj.relocs(".text") if tsec in SECS}
    data_syms = {nm for ds in (".data", ".sdata", ".rodata", ".sdata2") if ds in tobj.sec for off, typ, nm, tsec, ta in tobj.relocs(ds) if tsec in SECS}
    strs = {}
    for sec in SECS:
        d = tobj.sec.get(sec, b"")
        for addr, size, name, _ in tobj.objects(sec):
            if name.startswith("str_") and name in code_syms and name not in data_syms and name not in keep:
                raw = d[addr:addr + size].split(b"\0")[0]
                strs[name] = '"' + "".join(chr(b) if 32 <= b < 127 and chr(b) not in '"\\' else "\\%03o" % b for b in raw) + '"'
    for name, lit in strs.items():
        text = re.sub(r"^(?:extern )?(?:__declspec\([^)]*\) )?const char %s(?:\[[^\]]*\])?(?: = \"[^\n]*\")?;[ \t]*(?://[^\n]*)?\n" % name, "", text, flags=re.M)
        text = re.sub(r"\b%s\b" % name, lambda m: lit, text)
    n = 0
    addr_of = {name: addr for addr, size, name, _ in tobj.objects(".sdata2")}
    defs = []
    for name in sorted(keep, key=lambda k: addr_of.get(k, 0)):
        if name in names and not re.search(r"^(?:__declspec\([^)]*\) )?(?:const )?(?:volatile )?f32 %s = " % name, text, re.M):
            defs.append((name, names[name]))
    for name, lit in names.items():
        if name in keep:
            continue
        if not re.search(r"\b%s\b" % name, text):
            continue
        text = re.sub(FLOAT_DEF.format(n=name), "", text, flags=re.M)
        text = re.sub(r"\*\(\s*(?:const\s+|volatile\s+)*f32\s*\*\)\s*&\s*\(?\s*%s\s*\)?" % name, name, text)
        text = re.sub(r"(?:RELOAD_F32|MATCH_F32)\(\s*%s\s*\)" % name, lit, text)
        text = re.sub(r"\b%s\b" % name, lit, text)
        n += 1
    kept_lits = {names[k]: k for k in keep if k in names}
    if kept_lits:
        out_lines = []
        in_fn = 0
        for line in text.split("\n"):
            st = line.lstrip()
            code = line.split("//")[0]
            if line and line[0] not in " \t#}/" and "{" in code and ")" in code and not code.rstrip().endswith(";") and "=" not in code.split("(")[0]:
                in_fn = 1
            if in_fn and line[:1] in " \t" and not st.startswith(("//", "#", "*", "/*")):
                for lit, nm in kept_lits.items():
                    num = lit[:-1]
                    code2 = re.sub(r"(?<![\w.])%s(?:f)?(?![\w.])" % re.escape(num), nm, code)
                    code = code2
                    if num.endswith(".0"):
                        code = re.sub(r"(?<![\w.])%s(?:f)?(?![\w.])" % re.escape(num[:-2] + "."), nm, code) if False else code
                line = code + (("//" + line.split("//", 1)[1]) if "//" in line else "")
            if line == "}":
                in_fn = 0
            out_lines.append(line)
        text = "\n".join(out_lines)
    if reload:
        out_lines = []
        cur = None
        for line in text.split("\n"):
            code = line.split("//")[0]
            if line[:1] not in (" ", "\t", "#", "}", "/", "") and "{" in code and "(" in code and not code.rstrip().endswith(";"):
                mm = re.match(r"[^(]*?\b(\w+)\s*\)?\s*\(", code)
                m2 = re.match(r"\s*USER_FUNC\((\w+)\)", code)
                cur = m2.group(1) if m2 else (mm.group(1) if mm else None)
            if line[:1] in (" ", "\t") and cur and not line.lstrip().startswith(("//", "#", "extern")):
                for nm, fns in reload.items():
                    if nm in keep and cur in fns and re.search(r"\b%s\b" % nm, code):
                        code = re.sub(r"(?<!RELOAD_F32\()\b%s\b(?!\))" % nm, "RELOAD_F32(%s)" % nm, code)
                line = code + (("//" + line.split("//", 1)[1]) if "//" in line else "")
            out_lines.append(line)
        text = "\n".join(out_lines)
        if "RELOAD_F32" in text and "#define RELOAD_F32" not in text:
            incs = [m.end() for m in re.finditer(r"^#include[^\n]*\n", text, re.M)]
            at = incs[-1] if incs else 0
            text = text[:at] + "#ifdef MUST_MATCH\n#define RELOAD_F32(x) (*(const volatile f32*)&(x))\n#else\n#define RELOAD_F32(x) (x)\n#endif\n" + text[at:]
    if defs:
        for nm, lit in defs:
            if nm in nonconst:
                text = re.sub(r"^(\s*)extern const f32 %s;" % nm, r"\1extern f32 %s;" % nm, text, flags=re.M)
        decls = "".join(f"extern {'' if nm in nonconst else 'const '}f32 {nm};\n" for nm, lit in defs if not re.search(r"^\s*extern\b[^\n;]*\b%s\b" % nm, text, re.M))
        if decls:
            incs = [m.end() for m in re.finditer(r"^#include[^\n]*\n", text, re.M)]
            at = incs[-1] if incs else 0
            text = text[:at] + decls + text[at:]
        block = "\n// Named .sdata2 constants kept by tools/pool_plan.py, defined after the code in the original's order.\n"
        for name, lit in defs:
            zero = lit.startswith("0.0")
            decl = f'__declspec(section ".sdata2") f32 {name}' if name in nonconst else f"const f32 {name}"
            block += ("#pragma explicit_zero_data on\n" if zero else "") + f"{decl} = {lit};\n" + ("#pragma explicit_zero_data reset\n" if zero else "")
        text = text.rstrip("\n") + "\n" + block
    return text, n


def ours_tokens(sec):
    ours = Obj(ours_path)
    data = ours.sec.get(sec, b"")
    return [(a, nm, data[a:a + sz]) for a, sz, nm, _ in ours.objects(sec) if not nm.startswith(("gap_", "pad_"))]


def blob_key(b):
    return b.rstrip(b"\0") if len(b) > 4 else b


def solve_named(T, P, mode):
    """Smallest named prefix (deferred) / suffix (nodefer) of target float entries so that the compiler's all-literal
    pool order P, minus the named ones, equals the rest. T and P are lists of blob keys. Returns k or None."""
    n = len(T)
    for k in range(n + 1):
        named = T[:k] if mode == "deferred" else T[n - k:]
        rest = T[k:] if mode == "deferred" else T[:n - k]
        pool = list(P)
        for b in named:
            if b in pool:
                pool.remove(b)
        if pool == rest:
            return k
    return None


def fn_scores():
    """{function: fuzzy %} of this unit from a fresh objdiff report."""
    import json
    subprocess.run(["ninja", "build/G8MJ01/report.json"], capture_output=True, text=True)
    try:
        r = json.load(open("build/G8MJ01/report.json"))
    except (OSError, ValueError):
        return {}
    key = "/" + (unit if unit.split("/")[0] not in REL_MODS else unit)
    for u in r["units"]:
        if u["name"].endswith(key):
            return {f["name"]: f.get("fuzzy_match_percent", 0.0) for f in u.get("functions", [])}
    return {}


def probe(tobj):
    """Build the unit with every float constant turned into a literal, return the compiler's pool order for .sdata2."""
    sp = source_path()
    orig = open(sp).read()
    new, n = literalise(orig, tobj)
    base_scores = fn_scores()
    try:
        open(sp, "w").write(new)
        r = subprocess.run(["ninja", ours_path], capture_output=True, text=True)
        if r.returncode:
            return None, "all-literal build failed:\n" + "\n".join(r.stdout.splitlines()[-8:]), n
        probe.scores = (base_scores, fn_scores())
        return {sec: ours_tokens(sec) for sec in SECS}, "", n
    finally:
        open(sp, "w").write(orig)
        subprocess.run(["ninja", ours_path], capture_output=True, text=True)


def search_single(tobj, base, drops, uses, floats_by_addr):
    """For each constant used by a function that loses score, keep just that one named and report who recovers."""
    sp = source_path()
    orig = open(sp).read()
    cand = {}
    for d, f, b, l in drops:
        for (sec, addr), us in uses.items():
            if sec == ".sdata2" and addr in floats_by_addr and any(u["fname"] == f for u in us):
                cand[floats_by_addr[addr]["name"]] = floats_by_addr[addr]
    res = []
    try:
        for name, e in cand.items():
            new, n = literalise(orig, tobj, keep={name}, reload=reload_map(tobj))
            open(sp, "w").write(new)
            r = subprocess.run(["ninja", ours_path], capture_output=True, text=True)
            if r.returncode:
                continue
            sc = fn_scores()
            fixed = [f for d, f, b, l in drops if sc.get(f, 0) >= b - 0.005]
            worse = [f for f, b in base.items() if sc.get(f, 0) < b - 0.005 and f not in [x[1] for x in drops]]
            res.append((name, e, fixed, worse))
    finally:
        open(sp, "w").write(orig)
        subprocess.run(["ninja", ours_path], capture_output=True, text=True)
    return res


def verify(tobj, keep, uses, data_uses, write=False):
    """Build with `keep` named and every other float constant a literal; True when .sdata2 is identical and no function
    loses score. With write=True and a True result the new source stays; otherwise the source is restored."""
    sp = source_path()
    orig = open(sp).read()
    base = fn_scores()
    new, n = literalise(orig, tobj, keep=set(keep), reload=reload_map(tobj))
    ok = False
    try:
        open(sp, "w").write(new)
        r = subprocess.run(["ninja", ours_path], capture_output=True, text=True)
        if r.returncode:
            print("#   build failed:\n#   " + "\n#   ".join(r.stdout.splitlines()[-6:]))
            return False
        same = diagnose(tobj, ".sdata2", ours_tokens(".sdata2"), uses, data_uses, indent="#   ")
        sc = fn_scores()
        worse = [(f, b, sc.get(f, 0)) for f, b in base.items() if sc.get(f, 0) < b - 0.005]
        for f, b, a in worse:
            print(f"#   {f}: {b:.2f} -> {a:.2f}")
        ok = same and not worse
        return ok
    finally:
        if not write:
            open(sp, "w").write(orig)
        subprocess.run(["ninja", ours_path], capture_output=True, text=True)


def source_state(text, name):
    """DEF: the source defines the object; EXT: only declared extern; USE: only referenced; '-': not mentioned."""
    if not re.search(r"\b%s\b" % re.escape(name), text):
        return "-"
    if re.search(r"^[^\n;/]*\b%s\b\s*(?:\[[^\]]*\])?\s*=" % re.escape(name), text, re.M):
        return "DEF"
    if re.search(r"^\s*extern\b[^\n;]*\b%s\b" % re.escape(name), text, re.M):
        return "EXT"
    if re.search(r"^[^\n;/(]*\b%s\b\s*(?:\[[^\]]*\])?\s*;" % re.escape(name), text, re.M):
        return "DEF"
    return "USE"


def plan():
    tobj = Obj(target_path)
    uses, data_uses, funcs = analyse_uses(tobj, None)
    mode = unit_mode()
    sp = source_path()
    print(f"# pool plan for {unit}   ({mode} unit, source {sp})")
    print(f"# pooled entries land in the compiler's emission order; named objects land {'BEFORE' if mode == 'deferred' else 'AFTER'} the pooled literals.\n")
    pt, err, nlit = (None, "probe skipped", 0)
    if "--no-probe" not in flags and unit.split("/")[0] not in REL_MODS:
        pt, err, nlit = probe(tobj)
        if pt is None:
            print("# " + err + "\n")
        else:
            print(f"# probe: built the unit with {nlit} float constants as literals (source restored afterwards)\n")
            base, lit = probe.scores
            drops = sorted((b - lit.get(f, 0), f, b, lit.get(f, 0)) for f, b in base.items() if lit.get(f, 0) < b - 0.005)
            if drops:
                print("# functions that LOSE score when every constant is a literal (they need the named object, reloaded or CSE'd as retail does):")
                for d, f, b, l in reversed(drops):
                    print(f"#   {f:<34} {b:6.2f} -> {l:6.2f}")
                print()
                if "--no-search" not in flags:
                    ents = {e["addr"]: e for e in build_entries(tobj, ".sdata2", *analyse_uses(tobj, None)[:2]) if e["kind"] == "f32"}
                    res = search_single(tobj, base, drops, analyse_uses(tobj, None)[0], ents)
                    print("# single named constant that restores the dropping functions (slot = position in .sdata2):")
                    order = [e["name"] for e in sorted(ents.values(), key=lambda x: x["addr"])]
                    plan.needed = {}
                    for name, e, fixed, worse in res:
                        for f in fixed:
                            idx = order.index(name)
                            cur = plan.needed.get(f)
                            if cur is None or (idx < cur if mode == "deferred" else idx > cur):
                                plan.needed[f] = idx
                    for name, e, fixed, worse in res:
                        if fixed:
                            print(f"#   keep {name} (slot {order.index(name)}): restores {', '.join(fixed)}" + (f"; but lowers {', '.join(worse)}" if worse else "") + ("   [deferred unit: slots 0.." + str(order.index(name)) + " must then all be named]" if mode == "deferred" and order.index(name) else ""))
                    if not any(r[2] for r in res):
                        print("#   none: the functions need two or more named constants, or a use-site rewrite (RELOAD_F32 / local copy)")
                    print()
            else:
                print("# no function changes score when every constant is a literal: literals are enough for the code\n")
    elif unit.split("/")[0] in REL_MODS:
        print("# REL unit: no probe (pool_floats works on dol units); the static columns below still apply\n")
    try:
        srctext = open(sp).read()
    except OSError:
        srctext = ""
    cmds = []
    needed = list(getattr(plan, "needed", {}).values())
    final_keep = None
    for sec in SECS:
        if sec not in tobj.sec:
            continue
        entries = build_entries(tobj, sec, uses, data_uses)
        if not entries:
            continue
        seq = [e for e in entries if e["kind"] != "gap"]
        for e in seq:
            e["cls"] = "MAGIC" if e["kind"] == "magic" else ("UNREF" if not e["uses"] and not e["duses"] else ("DATA" if not e["uses"] else "LIT"))
        # named region from the probe
        k_f = None
        region = 0
        floats = [e for e in seq if e["kind"] == "f32"]
        if pt is not None and sec == ".sdata2":
            nonfloat = {blob_key(e["data"]) for e in seq if e["kind"] != "f32"}
            P = [blob_key(b) for a, nm, b in pt[sec] if len(b) == 4 and blob_key(b) not in nonfloat and (kind_of(sec, nm, b) == "f32" or nm.startswith("@"))]
            Tf = [blob_key(e["data"][:4]) for e in floats]
            k_f = solve_named(Tf, P, mode)
            if k_f is not None and needed:
                nf = len(floats)
                k_f = max(k_f, max(needed) + 1) if mode == "deferred" else max(k_f, nf - min(needed))
        unref_idx = [i for i, e in enumerate(seq) if e["cls"] == "UNREF"]
        if mode == "deferred":
            region = 0
            if k_f:
                region = seq.index(floats[k_f - 1]) + 1
            if unref_idx:
                region = max(region, max(unref_idx) + 1)
            named_ids = {id(e) for e in seq[:region]}
        else:
            nf = len(floats)
            start = len(seq)
            if k_f:
                start = seq.index(floats[nf - k_f])
            if unref_idx:
                start = min(start, min(unref_idx))
            named_ids = {id(e) for e in seq[start:]}
        # unreferenced and out-of-region named things are reported by the table
        print(f"== {sec}: {len(seq)} objects, {len(tobj.sec[sec])} bytes" + (f"; named float {'prefix' if mode == 'deferred' else 'suffix'} = {k_f}" if k_f is not None else ("; no named set reproduces the pool (see diagnosis below)" if pt is not None and sec == ".sdata2" else "")))
        for i, e in enumerate(seq):
            role = "NAMED  " if id(e) in named_ids else "literal"
            if e["cls"] == "MAGIC":
                role = "magic  "
            elif e["cls"] == "DATA" and id(e) not in named_ids:
                role = "data-own"
            first = e["uses"][0]["fname"] if e["uses"] else ("-" if not e["duses"] else "(data)")
            mx, rfn = reload_info(e)
            ldn = f"loads<= {mx}/block" if e["uses"] else ""
            e["src"] = source_state(srctext, e["name"])
            print(f" {i:3d} +0x{e['addr']:<4x} {e['kind']:<6} {show(e['kind'], e['data']):<24.24} {e['name']:<28.28} {role:<8} src:{e['src']:<3} first: {first:<28.28} refs {len(e['uses']):<3d} {ldn}")
            if "--no-trap-text" not in flags:
                for t in traps(e, seq, tobj, funcs):
                    print(f"        - {t}")
        print()
        named = [e for e in seq if id(e) in named_ids]
        if named:
            print(f"   NAMED objects ({len(named)}): define them in this order {'after the code (defined earlier the compiler folds their values into the code)' if mode == 'deferred' else 'after the code; in a nodefer unit they land AFTER the pooled literals'}:")
            for e in named:
                mx, rfn = reload_info(e)
                how = "const" if mx < 2 else "NON-const `__declspec(section \"%s\")` at the top of the file (retail reloads it between uses in %s)" % (sec, ", ".join(rfn[:3]))
                users = sorted({u["fname"] for u in e["uses"]})
                print(f"     {e['name']:<28} {show(e['kind'], e['data']):<22} {how}; use sites: {', '.join(users[:6]) or 'none'}")
            print("   Zero floats need `#pragma explicit_zero_data on` around the named block; unreferenced objects need `#pragma force_active on`.")
        acts = []
        for e in seq:
            named_now = id(e) in named_ids
            if e["cls"] == "MAGIC":
                if e["src"] in ("DEF",):
                    acts.append(f"{e['name']}: delete the definition (the compiler makes this constant itself)")
                continue
            if named_now and e["src"] in ("EXT", "-", "USE"):
                acts.append(f"{e['name']}: define it as a named object (after the code, in plan order)" + (" with force_active (unreferenced)" if e["cls"] == "UNREF" else ""))
            if not named_now and e["src"] in ("DEF", "EXT") and e["cls"] != "DATA":
                acts.append(f"{e['name']}: make it a literal at its uses and delete the " + ("definition" if e["src"] == "DEF" else "extern") + (" (floats: --apply does this)" if e["kind"] == "f32" else ""))
        if acts:
            print("   ACTIONS (source state vs plan):")
            for a in acts:
                print("     - " + a)
        owners = []
        for e in seq:
            for dsec, own in e["duses"]:
                if own != "?" and (dsec, own) not in owners and not e["uses"]:
                    owners.append((dsec, own))
        if owners:
            print("   data owners of pooled strings/constants, in the order the source must define them: " + ", ".join(f"{o} ({d})" for d, o in owners[:12]) + (" ..." if len(owners) > 12 else ""))
        lits = [e for e in seq if id(e) not in named_ids and e["cls"] in ("LIT", "DATA")]
        if lits:
            print(f"   LITERALS ({len(lits)}): write each at its uses as a literal and define no object (the compiler pools them in emission order).")
        if sec == ".sdata2" and k_f is not None:
            final_keep = [e["name"] for e in floats if id(e) in named_ids]
            cmds.append(f"uv run --with pyelftools python3 tools/pool_plan.py {unit} --apply")
        if pt is not None and sec == ".sdata2" and k_f is None:
            print("   DIAGNOSIS of the all-literal pool against the target (no named prefix/suffix repairs it):")
            diagnose(tobj, sec, pt[sec], uses, data_uses, indent="     ")
        print()
    if "--apply" in flags and final_keep is None and pt is not None:
        final_keep = []
    if final_keep is not None and pt is not None:
        print("# verifying the plan: building with these float constants named and the rest literal ...")
        ok = verify(tobj, final_keep, uses, data_uses, write="--apply" in flags)
        print("# " + ("VERIFIED: .sdata2 identical and no function loses score" if ok else "NOT VERIFIED: see the diagnosis above; fix the first slot, then run --check") + ("; source UPDATED (git diff shows the change)" if "--apply" in flags else ""))
    print("# commands (dol units): " + ("; ".join(cmds) if cmds else "(none)"))
    print("# then: uv run --with pyelftools python3 tools/pool_plan.py " + unit + " --check")
    return cmds


# ---------------------------------------------------------------- check


def tokens(obj, sec, kinds=None):
    data = obj.sec.get(sec, b"")
    res = []
    for addr, size, name, _ in obj.objects(sec):
        blob = data[addr:addr + size]
        if name.startswith(("gap_", "pad_")):
            continue
        res.append((addr, name, blob))
    return res


def literal_forms(kind, blob):
    if kind == "f32" and len(blob) >= 4:
        return [f32_literal(blob[:4]).rstrip("f").rstrip("0").rstrip(".") if False else f32_literal(blob[:4])[:-1]]
    return []


def diagnose(tobj, sec, o, uses, data_uses, indent="   ", src_lines=None):
    """Compare target objects with `o` (list of (addr, name, blob)); print the first mismatching slot and why. True if identical."""
    import difflib
    t = tokens(tobj, sec)
    tb = [blob_key(x[2]) for x in t]
    ob = [blob_key(x[2]) for x in o]
    sm = difflib.SequenceMatcher(None, tb, ob, autojunk=False)
    ops = [op for op in sm.get_opcodes() if op[0] != "equal"]
    if not ops:
        print(indent + "identical pool order and contents")
        return True
    if src_lines is None:
        try:
            src_lines = open(source_path()).read().split("\n")
        except OSError:
            src_lines = []
    tag, i1, i2, j1, j2 = ops[0]
    tent = build_entries(tobj, sec, uses, data_uses)
    by_addr = {e["addr"]: e for e in tent}
    vals = [struct.unpack(">f", e["data"][:4])[0] for e in tent if e["kind"] == "f32" and len(e["data"]) >= 4]
    P = lambda *a: print(indent + " ".join(str(x) for x in a))
    P(f"first mismatch at target entry {i1}" + (f" (+0x{t[i1][0]:x})" if i1 < len(t) else " (after the last target entry)") + f"; {len(ops)} differing region(s) in all")
    for i in range(max(0, i1 - 3), i1):
        P(f"  ok    {t[i][1]:<30} {show(kind_of(sec, t[i][1], t[i][2]), t[i][2])}")
    if tag in ("replace", "delete"):
        for i in range(i1, min(i2, i1 + 4)):
            e = by_addr.get(t[i][0])
            who = e["uses"][0]["fname"] if e and e["uses"] else ("(data) " + e["duses"][0][1] if e and e["duses"] else "none")
            P(f"  WANT  {t[i][1]:<30} {show(e['kind'], e['data']) if e else '':<18} first user: {who}")
    if tag in ("replace", "insert"):
        for j in range(j1, min(j2, j1 + 4)):
            kd = kind_of(sec, o[j][1], o[j][2])
            where = []
            for f in literal_forms(kd, o[j][2]):
                for ln, line in enumerate(src_lines, 1):
                    if re.search(r"(?<![\w.])" + re.escape(f) + r"f?(?![\w])", line) and not line.lstrip().startswith("//"):
                        where.append(ln)
            P(f"  HAVE  {o[j][1]:<30} {show(kd, o[j][2]):<18} source lines with that literal: {where[:8]}")
    # swaps of an adjacent pair
    if tag == "replace" and i2 - i1 == j2 - j1 == 2 and tb[i1] == ob[j1 + 1] and tb[i1 + 1] == ob[j1]:
        e1, e2 = by_addr.get(t[i1][0]), by_addr.get(t[i1 + 1][0])
        f1 = e1["uses"][0]["fname"] if e1 and e1["uses"] else "?"
        f2 = e2["uses"][0]["fname"] if e2 and e2["uses"] else "?"
        P(f"  adjacent pair swapped ({t[i1][1]}, {t[i1 + 1][1]}); target first uses: {f1} then {f2}.")
        if f1 == f2:
            P("  same function: the target's expression tree meets the first constant first. Swap operands (`(a + b) + c*d` for `c*d + (a + b)`), or declare locals initialised in the target's order.")
        else:
            P("  different functions: check function order (tools/fn_order_check.py) or name the pair in the prefix.")
    # extra and missing entries overall
    from collections import Counter
    ct, co = Counter(tb), Counter(ob)
    extra = [(j, o[j]) for j in range(len(o)) if co[ob[j]] > ct.get(ob[j], 0) and not co.update({ob[j]: -1})]
    co = Counter(ob)
    missing = [(i, t[i]) for i in range(len(t)) if ct[tb[i]] > co.get(tb[i], 0) and not ct.update({tb[i]: -1})]
    if extra:
        P(f"  ours has {len(extra)} EXTRA pooled entr{'y' if len(extra) == 1 else 'ies'} the target lacks (a literal the original loads by name, or a folded expression):")
        for j, (a_, nm, blob) in extra[:6]:
            kd = kind_of(sec, nm, blob)
            if kd == "f32" and len(blob) >= 4 and blob_key(blob) in set(tb):
                v = struct.unpack(">f", blob[:4])[0]
                tn = next((t_[1] for t_ in t if blob_key(t_[2]) == blob_key(blob)), "?")
                where = []
                for f in literal_forms(kd, blob):
                    for ln, line in enumerate(src_lines, 1):
                        if re.search(r"(?<![\w.])" + re.escape(f) + r"f?(?![\w])", line) and not line.lstrip().startswith("//"):
                            where.append(ln)
                P(f"    {nm:<10} {v:g}: a second copy of {tn}'s value: some literal still writes it where the target loads the object; use the object ({tn}) at these lines: {where[:10]}")
                continue
            if kd == "f32" and len(blob) >= 4:
                v = struct.unpack(">f", blob[:4])[0]
                miss_vals = [struct.unpack(">f", b_[:4])[0] for _, (_, _, b_) in missing if len(b_) >= 4 and kind_of(sec, "", b_) in ("f32",)]
                cand = vals + [-x for x in vals] + [1.0, -1.0, 2.0, -2.0]
                prods = [(a2, b2) for a2 in cand for b2 in cand if a2 and b2 and abs(a2 * b2 - v) < 1e-6 * max(abs(v), 1)]
                prods.sort(key=lambda ab: (-(any(abs(abs(x) - m) < 1e-9 for x in ab for m in miss_vals)), len("%g" % ab[0]) + len("%g" % ab[1])))
                where = []
                for f in literal_forms(kd, blob):
                    for ln, line in enumerate(src_lines, 1):
                        if re.search(r"(?<![\w.])" + re.escape(f) + r"f?(?![\w])", line) and not line.lstrip().startswith("//"):
                            where.append(ln)
                mprods = [ab for ab in prods if any(abs(abs(x) - m_) < 1e-9 for x in ab for m_ in miss_vals)]
                neg = [w for w in vals if abs(v + w) < 1e-9]
                if mprods:
                    hint = f"  = {mprods[0][0]:g} * {mprods[0][1]:g}: a product of constants folded; the target keeps {abs([x for x in mprods[0] if any(abs(abs(x) - m_) < 1e-9 for m_ in miss_vals)][0]):g} as its own pooled entry: put it in an f32 local (`f32 m = {abs([x for x in mprods[0] if any(abs(abs(x) - m_) < 1e-9 for m_ in miss_vals)][0]):.1f}f;`) and negate/multiply the local"
                elif neg and v:
                    hint = f"  is the negation of target constant {neg[0]:g}: `-x * y` folded when both are literals; negate a local (`f32 m = {abs(neg[0]):.1f}f; ... -m * y`) so the target's entries stay"
                elif prods:
                    hint = f"  = {prods[0][0]:g} * {prods[0][1]:g}: a constant product folded; keep one factor in an f32 local"
                else:
                    hint = "  replace that literal by the object the target loads the value from (or drop the duplicate named object)"
                P(f"    {nm:<10} {v:g}{hint}   source lines with that literal: {where[:6]}")
            else:
                P(f"    {nm:<10} {show(kd, blob)}: a duplicate/extra object of kind {kd}")
    if missing:
        P(f"  the target has {len(missing)} entr{'y' if len(missing) == 1 else 'ies'} ours LACKS (a use folded away, a dead constant, or an object that must be named):")
        for i, (a_, nm, blob) in missing[:6]:
            e = by_addr.get(a_)
            if e and not e["uses"] and not e["duses"]:
                P(f"    {nm}: unreferenced in retail -> a named object in the prefix (force_active)")
            elif e and e["kind"] == "magic":
                P(f"    {nm}: int->float conversion constant: the first conversion in code order creates it; check that conversion exists")
            elif e:
                P(f"    {nm}: first used in {e['uses'][0]['fname'] if e['uses'] else '(data)'}; check that use is not folded (negation, product, cos() argument)")
    return False


def check():
    if "--no-build" not in flags:
        r = subprocess.run(["ninja", ours_path], capture_output=True, text=True)
        if r.returncode:
            print("build failed:\n" + "\n".join(r.stdout.splitlines()[-12:]))
            return
    tobj = Obj(target_path)
    ours = Obj(ours_path)
    uses, data_uses, funcs = analyse_uses(tobj, None)
    ok = True
    for sec in SECS:
        t = tokens(tobj, sec)
        o = tokens(ours, sec)
        print(f"== {sec}: target {len(tobj.sec.get(sec, b''))} bytes / {len(t)} objects, ours {len(ours.sec.get(sec, b''))} bytes / {len(o)} objects")
        ok &= diagnose(tobj, sec, o, uses, data_uses)
        print()
    print("OK" if ok else "mismatch remains: apply the diagnosis above and run --check again")


if __name__ == "__main__":
    if "--check" in flags:
        check()
    else:
        cmds = plan()
        if "--apply" in flags:
            for c in cmds:
                print("$", c)
                subprocess.run(c, shell=True)
            check()
