#!/usr/bin/env python3
"""Flag differences between a function and the original that register allocation and scheduling can't explain.

  python3 tools/semdiff.py UNIT [FUNC ...]            every function in UNIT below 100%, or just FUNCs
  python3 tools/semdiff.py -1 ORIG.o -2 OURS.o [FUNC ...]   two objects directly (used by the back-test)
  options: --all (include functions at 100%)  --json

UNIT is the source path without src/ and .c, e.g. party/party_yoshi. Run after a build.

A partial function's score says how close its bytes are, not whether its logic is right: a wrong flag mask, an
inverted test or a missing call can sit at 95%. This compares the parts of each function that carry meaning, as
multisets, ignoring register numbers, stack offsets and instruction order:
  imm     integer constants (li/lis/addi/subi/mulli/srawi...): constants, divisors, `== N` tests
  logic   ori/oris/xori/xoris and non-contiguous andi./andis. immediates: flags set, toggled or tested
  mask    rlwinm/rlwnm masks and contiguous andi./andis., as the value mask (`rlwinm 0,15,13` -> 0xfffdffff);
          a trailing "." means the result also sets CR0 (a test)
  insert  rlwimi (bit-field insert), as the mask and rotation
  cmp     signed or unsigned compare (with its immediate); cror (the >= and <= tests)
  call    a called function; `tail` a tail call (b); `call <indirect>` a bctrl/blrl through a pointer
  load    what a load off a non-stack base reads: a field (width, signedness, offset), or data by identity
  store   what a store writes to, the same way
  addr    the address of data taken (addi x@l, or a block address plus an offset)
  op      opcodes that change meaning: divw/divwu, mulhw/mulhwu, extsb/extsh, fabs, fctiwz, neg, fneg
Data is identified by contents if it's read-only or has a made-up name (constants, strings, tables, jump
tables, including what their pointers point to), so an anonymous `@633` and the original's
`float_0_8041b020` compare equal; and by name and offset if it's a named writable variable, so using the
wrong global shows up.

Operations (mask, logic, insert, cmp, call, tail, store, op) are compared by count: one more or fewer is
different code. Constants, loads and addresses are compared by presence only, since the compiler loads a
value once or several times as it likes; so a missing *second* use of a constant is not caught.

What this does not see: branch conditions and block structure (beq/bne flip with layout, so they're left out),
which registers feed a compare, arguments passed as stack addresses, and anything in a function that matches.
No flags means nothing on this list differs, not that the function is right. Each line is a lead for review.
"""
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OBJDIFF = ROOT / "build" / "tools" / "objdiff-cli"
OPS = {"divw", "divwu", "mulhw", "mulhwu", "extsb", "extsh", "fabs", "fnabs", "fctiwz", "neg", "fneg"}
IMM_OPS = {"li", "lis", "addi", "subi", "addis", "subis", "ori", "oris", "xori", "xoris", "andi.", "andis.",
           "mulli", "subfic", "addic", "addic.", "subic", "subic.", "srawi", "srawi."}
COUNTED = {"mask", "logic", "insert", "cmp", "call", "tail", "store", "op"}
VOLATILE = {"r0"} | {f"r{i}" for i in range(3, 13)}
CMP_OPS = {"cmpw": "s", "cmpwi": "s", "cmplw": "u", "cmplwi": "u"}
NO_DEST = ("st", "cmp", "fcmp", "b", "mt", "psq_st", "dcb", "icb", "sync", "isync", "tw", "cror", "crxor",
           "crclr", "crset", "crnot", "crand", "creqv", "crnor", "crorc", "crandc", "mcrf")
# Names the compiler or the splitter made up (pools, jump tables, templates): compared by contents wherever
# they live, since agents often redeclare the original's constants as ordinary globals.
AUTO_NAME = re.compile(r"^(@|\.\.\.|jumptable_|lbl_|float_|double_|str_|vec3_|dat_|unk_|zero_|string_)")
READ_ONLY = (".rodata", ".sdata2", ".sbss2", ".text", ".ctors", ".dtors")
KIND_SIZE = {"f32": 4, "f64": 8, "i4": 4, "s2": 2, "u2": 2, "s1": 1, "u1": 1}


# --- data: what a reference means ----------------------------------------------------------------

class Obj:
    """The symbols, sections and data relocations of one ELF32 big-endian relocatable object."""

    def __init__(self, path):
        data = self.data = Path(path).read_bytes()
        shoff, = struct.unpack_from(">I", data, 0x20)
        shentsize, shnum, shstrndx = struct.unpack_from(">HHH", data, 0x2E)
        secs = [struct.unpack_from(">IIIIIIIIII", data, shoff + i * shentsize) for i in range(shnum)]
        cstr = lambda base, i: data[base + i:data.index(b"\0", base + i)].decode(errors="replace")
        self.secs = [{"name": cstr(secs[shstrndx][4], s[0]), "type": s[1], "off": s[4], "size": s[5]} for s in secs]
        self.syms = []   # by index: (name, section index, value, size, type)
        self.by_name = {}
        for s in secs:
            if s[1] != 2:  # SHT_SYMTAB
                continue
            for off in range(s[4], s[4] + s[5], 16):
                name_i, value, size, info, _, shndx = struct.unpack_from(">IIIBBH", data, off)
                name = cstr(secs[s[6]][4], name_i)
                self.syms.append((name, shndx, value, size, info & 0xF))
                if name and 0 < shndx < 0xFF00:
                    self.by_name.setdefault(name, (shndx, value, size, info & 0xF))
        self.relocs = {}  # section index -> {offset: (symbol index, addend)}
        for s in secs:
            if s[1] == 4:  # SHT_RELA
                tgt = self.relocs.setdefault(s[7], {})
                for off in range(s[4], s[4] + s[5], 12):
                    r_off, r_info, addend = struct.unpack_from(">IIi", data, off)
                    tgt[r_off] = (r_info >> 8, addend)

    def section_named(self, name):
        return next((i for i, s in enumerate(self.secs) if s["name"] == name), None)

    def read(self, shndx, off, n):
        s = self.secs[shndx]
        if s["type"] == 8:  # NOBITS
            return b"\0" * n
        return self.data[s["off"] + off:s["off"] + min(off + n, s["size"])]


class Data:
    """Resolves a reference (symbol + offset, read as `kind`) to a key that means the same in both objects."""

    def __init__(self, own, other):
        self.own, self.other = own, other

    def locate(self, name):
        """(object, section index, value, size) for a symbol, ours first; section symbols by section name."""
        if name.startswith("..."):  # objdiff's name for a section symbol: ".." + section name + "." + index
            i = self.own.section_named(name[2:].rsplit(".", 1)[0])
            return (self.own, i, 0, self.own.secs[i]["size"]) if i is not None else None
        for o in (self.own, self.other):
            e = o.by_name.get(name)
            if e:
                return (o, e[0], e[1], e[2])
        return None

    def key(self, name, off=0, kind=None, depth=0):
        loc = self.locate(name)
        ident = re.sub(r"\$\d+$", "", name) + (f"+{off:#x}" if off else "")
        if loc is None:
            return ident  # undefined on both sides: compare by name
        o, shndx, value, size = loc
        sec = o.secs[shndx]["name"]
        if not sec.startswith(READ_ONLY) and not AUTO_NAME.match(name):
            return "var " + ident  # a named variable: which one is what matters
        # read-only: contents, including where its pointers point
        n = KIND_SIZE[kind] if kind else (size - off if off < size else 32)
        start = value + off
        blob = o.read(shndx, start, n)
        ptrs = []
        for r_off in sorted(k for k in o.relocs.get(shndx, {}) if start <= k < start + len(blob)):
            sym_i, addend = o.relocs[shndx][r_off]
            tname, t_shndx, _, _, t_type = o.syms[sym_i]
            t_sec = o.secs[t_shndx]["name"] if 0 < t_shndx < len(o.secs) else ""
            if t_sec.startswith(".text") and (t_type == 3 or AUTO_NAME.match(tname) or not tname or addend):
                target = "code"  # a jump-table entry (function + offset) or a label; a function pointer keeps its name
            elif t_type == 3 or AUTO_NAME.match(tname):  # an anonymous constant or string: its contents
                if t_type == 3:
                    tname = ".." + t_sec + ".0"
                target = self.key(tname, addend, None, depth + 1) if depth < 1 else "data"
            else:
                target = re.sub(r"\$\d+$", "", tname) + (f"+{addend:#x}" if addend else "")
            ptrs.append(target)
        if ptrs and all(p == "code" for p in ptrs):
            return f"table of {len(ptrs)} code pointers"
        return typed_key(blob, kind) + (" -> [" + ", ".join(ptrs) + "]" if ptrs else "")


def typed_key(blob, kind=None):
    if kind == "f32" and len(blob) == 4:
        v = struct.unpack(">f", blob)[0]
        return f"f32:{v!r}" if v == v else f"f32:nan{blob.hex()}"
    if kind == "f64" and len(blob) == 8:
        v = struct.unpack(">d", blob)[0]
        return f"f64:{v!r}" if v == v else f"f64:nan{blob.hex()}"
    if kind and kind in KIND_SIZE:
        return f"{kind}:{int.from_bytes(blob, 'big'):#x}"
    if not blob.strip(b"\0"):
        return "zeros"  # any length: pools of zeros are laid out differently
    s = blob.split(b"\0")[0]
    if len(s) >= 3 and all(32 <= c < 127 for c in s):
        return f'str:"{s.decode()}"'
    if len(blob) == 4:
        return typed_key(blob, "f32")
    if len(blob) == 8:
        return typed_key(blob, "f64")
    return f"bytes[{len(blob)}]:{blob[:12].hex()}..{hashlib.sha1(blob).hexdigest()[:8]}"


# --- instruction tokens --------------------------------------------------------------------------

def rlw_mask(mb, me):
    if mb <= me:
        m = ((1 << (32 - mb)) - 1) & ~((1 << (31 - me)) - 1)
    else:
        m = ((1 << (32 - mb)) - 1) | (0xFFFFFFFF & ~((1 << (31 - me)) - 1))
    return m & 0xFFFFFFFF


def rlw(op, n):
    """(sh, mb, me) for rlwinm/rlwimi and their simplified mnemonics, from their numeric args."""
    op = op.rstrip(".")
    if op in ("rlwinm", "rlwimi") and len(n) >= 3:
        return n[0], n[1], n[2]
    if not n:
        return None
    a = n[0]
    b = n[1] if len(n) > 1 else 0
    return {"slwi": (a, 0, 31 - a), "srwi": ((32 - a) % 32, a, 31), "clrlwi": (0, a, 31),
            "clrrwi": (0, 0, 31 - a), "rotlwi": (a, 0, 31), "rotrwi": ((32 - a) % 32, 0, 31),
            "extrwi": ((b + a) % 32, 32 - a, 31), "extlwi": (b, 0, a - 1), "clrlslwi": (b, a - b, 31 - b),
            "inslwi": ((32 - b) % 32, b, b + a - 1), "insrwi": ((32 - (b + a)) % 32, b, b + a - 1)}.get(op)


def contiguous(m):
    """(mb, me) if m is one run of ones in IBM numbering (possibly wrapping), else None."""
    for mb in range(32):
        for me in range(32):
            if rlw_mask(mb, me) == m:
                return mb, me
    return None


MEM = re.compile(r"^(l|st)(w|h|b|f[sd])(a|z)?(u)?(x)?$")
LOAD_KIND = {("w", None): "i4", ("h", "a"): "s2", ("h", "z"): "u2", ("b", "z"): "u1", ("fs", None): "f32",
             ("fd", None): "f64", ("h", None): "u2", ("b", None): "u1", ("w", "z"): "i4"}


def decode(ins):
    parts = ins["parts"]
    op = parts[0]["opcode"]["mnemonic"]
    args = [p["arg"] for p in parts[1:] if "arg" in p]
    nums = [int(a.get("signed", a.get("unsigned"))) for a in args if "signed" in a or "unsigned" in a]
    nums += [int(a["opaque"]) for a in args if a.get("opaque", "").isdigit()]  # rlwinm's sh/mb/me come as text
    regs = [a["opaque"] for a in args if "opaque" in a and not a["opaque"].isdigit()]
    return op, nums, regs


def written(op, regs):
    """Registers an instruction writes (general-purpose ones; enough for tracking data addresses)."""
    if op in ("lmw",) and regs:
        return [f"r{i}" for i in range(int(regs[0][1:]), 32)]
    w = []
    if regs and not op.startswith(NO_DEST) and not op.startswith("psq_st"):
        w.append(regs[0])
    m = MEM.match(op)
    if m and m.group(4) and len(regs) >= 2:  # update form also writes its base
        w.append(regs[1] if m.group(5) else regs[-1])
    return w


def tokens(sym, data):
    """Meaning-bearing tokens of one function. `data` resolves referenced data."""
    rows = [r["instruction"] for r in sym.get("instructions", []) if r.get("instruction")]
    targets = {int(i["branch_dest"]) for i in rows if "branch_dest" in i}
    writes = Counter()
    for ins in rows:
        op, _, regs = decode(ins)
        if op.startswith("l") and regs[-1:] == ["r1"]:
            continue  # restoring a saved register from the stack isn't a new value for this count
        writes.update(written(op, regs))
    out = Counter()
    base = {}  # register -> (symbol, offset) while it holds a data address
    for ins in rows:
        op, nums, regs = decode(ins)
        text = ins["formatted"]
        if int(ins.get("address", -1)) in targets:  # a join point: keep only bases that can't have been reassigned
            base = {r: v for r, v in base.items() if writes[r] <= 1}
        m = re.search(r"([\w@$.]+)@(ha|h|l|sda21|sda2i16)\b", text)
        reloc, rkind = (m.group(1), m.group(2)) if m else (None, None)
        wr = written(op, regs)

        if op in ("bl", "bctrl", "blrl") or (op == "b" and "relocation" in ins):
            if op == "bl":
                out[("call", text.split()[-1])] += 1
            elif op == "b":
                out[("tail", text.split()[-1])] += 1
            else:
                out[("call", "<indirect>")] += 1
            base = {r: v for r, v in base.items() if r not in VOLATILE}  # a call clobbers the volatile registers
            continue

        mem = MEM.match(op)
        if mem or op.startswith("psq_") or op in ("lmw", "stmw"):
            load = op.startswith(("l", "psq_l"))
            role = "load" if load else "store"
            if mem:
                width = {"w": 4, "h": 2, "b": 1, "fs": 4, "fd": 8}[mem.group(2)]
                kind = LOAD_KIND.get((mem.group(2), mem.group(3))) if load else \
                    {"w": "i4", "h": "u2", "b": "u1", "fs": "f32", "fd": "f64"}[mem.group(2)]
                sign = "s" if load and mem.group(3) == "a" else ""
                desc = f"{width}{sign}"
            else:
                kind, desc = None, op
            b_reg = regs[1] if (mem and mem.group(5)) else (regs[-1] if len(regs) >= 2 else None)
            if reloc:  # lfs f1, x@sda21 / lwz r3, x@l(r4)
                out[(role, data.key(reloc, 0, kind))] += 1
            elif mem and mem.group(5):  # indexed: base + register
                if b_reg in base:
                    out[(role, f"{desc} [{data.key(base[b_reg][0], base[b_reg][1])} + index]")] += 1
                else:
                    out[(role, f"{desc} [index]")] += 1
            elif b_reg in base and nums:
                sname, soff = base[b_reg]
                out[(role, data.key(sname, soff + nums[0], kind))] += 1
            elif b_reg and b_reg != "r1" and nums:
                extra = f" w{nums[1]} q{nums[2]}" if op.startswith("psq_") and len(nums) >= 3 else ""
                out[(role, f"{desc} {nums[0]:#x}{extra}")] += 1
            if mem and mem.group(4) and b_reg in base and nums:  # update form: the base moves
                base[b_reg] = (base[b_reg][0], base[b_reg][1] + nums[0])
                wr = [r for r in wr if r != b_reg]
            for r in wr:
                base.pop(r, None)
            continue

        if reloc:
            if rkind in ("ha", "h"):
                base[regs[0]] = (reloc, 0)
                continue
            if rkind == "l" and op in ("addi", "ori"):
                base[regs[0]] = (reloc, 0)
                out[("addr", data.key(reloc))] += 1
                continue
            out[("addr", data.key(reloc))] += 1
            for r in wr:
                base.pop(r, None)
            continue
        if op in ("addi", "subi") and len(regs) >= 2 and regs[1] in base and nums:
            sname, soff = base[regs[1]]
            soff += nums[-1] if op == "addi" else -nums[-1]
            base[regs[0]] = (sname, soff)
            out[("addr", data.key(sname, soff))] += 1
            continue
        if op == "mr" and len(regs) == 2 and regs[1] in base:
            base[regs[0]] = base[regs[1]]
            continue
        for r in wr:
            base.pop(r, None)

        if op in CMP_OPS:
            out[("cmp", CMP_OPS[op] + (f" {nums[-1]:#x}" if op.endswith("i") and nums else ""))] += 1
            continue
        if op == "cror":
            out[("cmp", "cror " + ",".join(regs))] += 1
            continue
        if op.rstrip(".") in OPS:
            out[("op", op.rstrip("."))] += 1
        if op in ("andi.", "andis.") and nums:
            v = (nums[-1] & 0xFFFF) << (16 if op == "andis." else 0)
            c = contiguous(v) if v else None
            if c:  # the same test rlwinm. would do; the compiler picks either
                out[("mask", f"{v:#010x}.")] += 1
            else:
                out[("logic", f"and. {v:#x}")] += 1
            continue
        if op in IMM_OPS and nums:
            if op in ("addi", "subi", "addic", "subic") and regs[1:2] == ["r1"]:
                continue  # a stack address
            v = nums[-1]
            if op in ("subi", "subic", "subic.", "subis"):
                v = -v
            if op in ("lis", "addis", "subis", "oris", "xoris"):
                v = (v & 0xFFFF) << 16
            logic = op in ("ori", "oris", "xori", "xoris")
            if logic:
                out[("logic", f"{'xor' if op.startswith('x') else 'or'} {v & 0xFFFFFFFF:#x}")] += 1
            else:
                out[("imm", f"{v & 0xFFFFFFFF:#x}" if op == "lis" else str(v))] += 1
            continue
        if op.rstrip(".") in ("rlwnm", "rotlw") and nums:
            mb, me = (nums[-2], nums[-1]) if op.startswith("rlwnm") else (0, 31)
            out[("mask", f"{rlw_mask(mb, me):#010x} rot-by-register" + ("." if op.endswith(".") else ""))] += 1
            continue
        r = rlw(op, nums)
        if r and op.rstrip(".") in ("rlwinm", "rlwimi", "slwi", "srwi", "clrlwi", "clrrwi", "rotlwi", "rotrwi",
                                    "extrwi", "extlwi", "clrlslwi", "inslwi", "insrwi"):
            sh, mb, me = r
            kind = "insert" if op.rstrip(".") in ("rlwimi", "inslwi", "insrwi") else "mask"
            out[(kind, f"{rlw_mask(mb, me):#010x}" + (f" rot{sh}" if sh else "") + ("." if op.endswith(".") else ""))] += 1
    return out


# --- driver --------------------------------------------------------------------------------------

def run_objdiff(args):
    fd, tmp = tempfile.mkstemp(prefix="semdiff_", suffix=".json", dir=ROOT / "build")
    os.close(fd)
    try:
        r = subprocess.run([str(OBJDIFF), "diff", *args, "-o", tmp, "--format", "json"], capture_output=True,
                           text=True, cwd=ROOT)
        if r.returncode:
            sys.exit("objdiff-cli failed: " + r.stderr[-1000:])
        return json.loads(Path(tmp).read_text())
    finally:
        os.unlink(tmp)


def compare(orig_o, ours_o, diff, funcs=None, include_matched=False):
    orig, ours = Obj(orig_o), Obj(ours_o)
    left = {s["name"]: s for s in diff["left"]["symbols"] if s.get("kind") == "SYMBOL_FUNCTION"}
    right = {s["name"]: s for s in diff["right"]["symbols"] if s.get("kind") == "SYMBOL_FUNCTION"}
    results = []
    for name in (funcs or sorted(right)):
        if name not in left or name not in right:
            continue
        pct = right[name].get("match_percent")
        if pct is None or (pct >= 100 and not include_matched):
            continue
        a = tokens(left[name], Data(orig, ours))
        b = tokens(right[name], Data(ours, orig))
        missing, extra = [], []
        for k in sorted(set(a) | set(b)):
            na, nb = a[k], b[k]
            if na == nb or (na and nb and k[0] not in COUNTED):
                continue
            t = f"{k[0]} {k[1]}" + (f" (original {na}, ours {nb})" if na and nb else "")
            (missing if na > nb else extra).append(t)
        results.append({"function": name, "percent": round(pct, 2), "missing": missing, "extra": extra})
    return results


def main():
    argv = sys.argv[1:]
    as_json = "--json" in argv
    include_matched = "--all" in argv
    argv = [a for a in argv if a not in ("--json", "--all")]
    if argv[:1] == ["-1"]:
        orig_o, ours_o, funcs = argv[1], argv[3], argv[4:]
        diff = run_objdiff(["-1", orig_o, "-2", ours_o])
        label = ours_o
    else:
        unit, funcs = argv[0], argv[1:]
        cfg = json.loads((ROOT / "objdiff.json").read_text())
        u = next(x for x in cfg["units"] if x["name"] == "MarioSt/" + unit)
        orig_o, ours_o = ROOT / u["target_path"], ROOT / u["base_path"]
        diff = run_objdiff(["-p", ".", "-u", "MarioSt/" + unit])
        label = unit
    results = compare(orig_o, ours_o, diff, funcs or None, include_matched)
    if as_json:
        print(json.dumps(results, indent=1))
        return
    flagged = [r for r in results if r["missing"] or r["extra"]]
    for r in flagged:
        print(f"{label} {r['function']} ({r['percent']}%)")
        for t in r["missing"]:
            print(f"    original only: {t}")
        for t in r["extra"]:
            print(f"    ours only:     {t}")
    print(f"{len(flagged)} of {len(results)} functions flagged (no flags is not proof a function is right)")


if __name__ == "__main__":
    main()
