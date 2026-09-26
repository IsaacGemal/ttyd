#!/usr/bin/env python3
"""Flag differences between a function and the original that register allocation and scheduling can't explain.

  python3 tools/semdiff.py UNIT [FUNC ...]            every function in UNIT below 100%, or just FUNCs
  python3 tools/semdiff.py -1 ORIG.o -2 OURS.o [FUNC ...]   two objects directly (used by the back-test)
  options: --all (include functions at 100%)  --json

UNIT is the source path without src/ and .c, e.g. party/party_yoshi. Run after a build.

A partial function's score says how close it is, not whether its logic is right: a wrong flag mask, an inverted
test or a missing call can sit at 95%. This compares the *meaning-bearing* parts of each function as multisets,
ignoring register numbers, stack offsets and instruction order:
  imm    integer constants (li/lis/addi/subi/mulli/srawi...): constants, divisors, == N tests
  logic  ori/oris/xori/xoris/andi./andis. immediates: flags set, toggled or tested
  mask   rlwinm-family masks, as the value mask (`rlwinm 0,15,13` -> 0xFFFDFFFF): bit tests and clears
  mem    loads/stores off a non-stack base, as (width and signedness, offset): which field, read how
  cmp    signed or unsigned compare; cror (>= / <= tests)
  call   called functions
  ref    data a function references, by content (so `@633` and `float_0_8041b020` compare equal)
  op     a few opcodes that change meaning: divw/divwu, mulhw/mulhwu, extsb/extsh, fabs, fctiwz, neg, fneg
Masks, logic immediates, compares, the opcodes above and calls are compared by count (each is an operation, so
one more or fewer means different code). Constants, data refs and field offsets are compared by presence only,
since the compiler loads a constant once or several times as it likes. Each line is a lead for a reviewer, not proof of a bug.
"""
import json
import re
import struct
import subprocess
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OBJDIFF = ROOT / "build" / "tools" / "objdiff-cli"
OPS = {"divw", "divwu", "mulhw", "mulhwu", "extsb", "extsh", "fabs", "fnabs", "fctiwz", "neg", "fneg"}
IMM_OPS = {"li", "lis", "addi", "subi", "addis", "subis", "ori", "oris", "xori", "xoris", "andi.", "andis.",
           "mulli", "subfic", "addic", "addic.", "subic", "subic.", "srawi", "srawi."}
# Token kinds whose count matters (each is an operation); for the rest (constants the compiler may load once
# or twice, data refs, field offsets) only presence on one side and absence on the other is reported.
COUNTED = {"mask", "logic", "cmp", "op", "call"}
VOLATILE = {"r0"} | {f"r{i}" for i in range(3, 13)}
CMP_OPS = {"cmpw": "s", "cmpwi": "s", "cmplw": "u", "cmplwi": "u"}


# --- data references, resolved to their contents -------------------------------------------------

def elf_symbols(path):
    """name -> (section bytes, offset, size) for every defined data symbol in an ELF32 big-endian object.
    Section bytes are None for .bss/.sbss (all zeros)."""
    data = Path(path).read_bytes()
    shoff, = struct.unpack_from(">I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from(">HHH", data, 0x2E)
    secs = [struct.unpack_from(">IIIIIIIIII", data, shoff + i * shentsize) for i in range(shnum)]
    shstr = secs[shstrndx][4]
    out = {}
    for s in secs:  # whole sections too, for references through a local section symbol ("...rodata.0")
        name = data[shstr + s[0]:data.index(b"\0", shstr + s[0])].decode(errors="replace")
        if name and s[5] and s[1] in (1, 8):  # PROGBITS / NOBITS
            out.setdefault("section:" + name, (None if s[1] == 8 else data[s[4]:s[4] + s[5]], 0, s[5]))
    for s in secs:
        if s[1] != 2:  # SHT_SYMTAB
            continue
        strtab = secs[s[6]]
        for off in range(s[4], s[4] + s[5], 16):
            name_i, value, size, info, other, shndx = struct.unpack_from(">IIIBBH", data, off)
            if shndx == 0 or shndx >= 0xFF00 or size == 0 or (info & 0xF) != 1:  # defined objects only
                continue
            name = data[strtab[4] + name_i:data.index(b"\0", strtab[4] + name_i)].decode(errors="replace")
            sec = secs[shndx]
            body = None if sec[1] == 8 else data[sec[4]:sec[4] + sec[5]]  # SHT_NOBITS: zeros
            out[name] = (body, value, size)
    return out


class Data:
    """Reads what a symbol (plus an offset) holds, from our object first, then the original's."""

    def __init__(self, own, other):
        self.own, self.other = own, other

    def read(self, name, off=0, n=None):
        if name.startswith("..."):  # objdiff's name for a section symbol: ".." + section name + "." + index
            e = self.own.get("section:" + name[2:].rsplit(".", 1)[0])
            if e is None:
                return None
            body, value, size = e
            n = n or 32
            return b"\0" * n if body is None else body[off:off + n]
        e = self.own.get(name) or self.other.get(name)
        if e is None:
            return None
        body, value, size = e
        n = n or (size - off if off < size else 32)  # past the symbol's end: whatever the block holds there
        if body is None:
            return b"\0" * n
        return body[value + off:value + off + n]

    def key(self, name, off=0, kind=None):
        """A symbol's contents as a comparable key; `kind` is how an instruction reads it (f32/f64/i4/...)."""
        if kind:
            b = self.read(name, off, {"f32": 4, "f64": 8, "i4": 4, "i2": 2, "i1": 1}[kind])
            if b is None or len(b) < {"f32": 4, "f64": 8, "i4": 4, "i2": 2, "i1": 1}[kind]:
                return f"{name}+{off:#x}"
            if kind == "f32":
                return f"f32:{struct.unpack('>f', b)[0]:g}"
            if kind == "f64":
                return f"f64:{struct.unpack('>d', b)[0]:g}"
            return f"{kind}:{int.from_bytes(b, 'big'):#x}"
        b = self.read(name, off)
        return content_key(b) if b is not None else f"{name}+{off:#x}"


def content_key(blob):
    if not blob.strip(b"\0"):
        return "zeros"
    b = blob[:24]
    if len(blob) in (4, 8) and not (b.rstrip(b"\0").isascii() and b[:1].isalpha()):
        if len(blob) == 4:
            return f"f32:{struct.unpack('>f', blob)[0]:g}"
        return f"f64:{struct.unpack('>d', blob)[0]:g}"
    s = b.split(b"\0")[0]
    if len(s) >= 3 and all(32 <= c < 127 for c in s):
        return f'str:"{s.decode()}"'
    return "bytes:" + b.hex()


# --- instruction tokens --------------------------------------------------------------------------

def rlw_mask(mb, me):
    if mb <= me:
        m = ((1 << (32 - mb)) - 1) & ~((1 << (31 - me)) - 1)
    else:
        m = ((1 << (32 - mb)) - 1) | (0xFFFFFFFF & ~((1 << (31 - me)) - 1))
    return m & 0xFFFFFFFF


def rlw(op, n):
    """(sh, mb, me) for rlwinm and its simplified mnemonics, from their numeric args."""
    if op.startswith("rlwinm"):
        return n[0], n[1], n[2]
    a = n[0]
    b = n[1] if len(n) > 1 else 0
    return {"slwi": (a, 0, 31 - a), "srwi": ((32 - a) % 32, a, 31), "clrlwi": (0, a, 31),
            "clrrwi": (0, 0, 31 - a), "rotlwi": (a, 0, 31), "rotrwi": ((32 - a) % 32, 0, 31),
            "extrwi": ((b + a) % 32, 32 - a, 31), "extlwi": (b, 0, a - 1),
            "clrlslwi": (b, a - b, 31 - b)}.get(op.rstrip("."))


LOAD_KIND = {"lfs": "f32", "lfsu": "f32", "lfd": "f64", "lfdu": "f64", "lwz": "i4", "lwzu": "i4", "lha": "i2",
             "lhz": "i2", "lbz": "i1", "stfs": "f32", "stfd": "f64", "stw": "i4", "sth": "i2", "stb": "i1"}


def tokens(sym, data):
    """Meaning-bearing tokens of one function. `data` resolves referenced data to its contents."""
    out = Counter()
    base = {}  # register -> (symbol, offset) while it holds a data address
    for row in sym.get("instructions", []):
        ins = row.get("instruction")
        if not ins:
            continue
        parts = ins["parts"]
        op = parts[0]["opcode"]["mnemonic"]
        args = [p["arg"] for p in parts[1:] if "arg" in p]
        text = ins["formatted"]
        nums = [int(a.get("signed", a.get("unsigned"))) for a in args if "signed" in a or "unsigned" in a]
        nums += [int(a["opaque"]) for a in args if a.get("opaque", "").isdigit()]  # rlwinm's sh/mb/me
        regs = [a["opaque"] for a in args if "opaque" in a and not a["opaque"].isdigit()]
        m = re.search(r"([\w@$.]+)@(ha|h|l|sda21|sda2i16)\b", text)
        reloc, rkind = (m.group(1), m.group(2)) if m else (None, None)
        dest = regs[0] if regs and not op.startswith(("st", "cmp", "fcmp", "b", "mt")) else None

        if op == "bl" or (op == "b" and "relocation" in ins):
            out[("call", text.split()[-1])] += 1
            for r in [r for r in base if r in VOLATILE]:
                del base[r]  # a call clobbers the volatile registers
            continue
        mem = re.match(r"^(l|st)(w|h|b|f[sd])(a|z)?(u)?(x)?$", op)
        if mem and not mem.group(5) and len(nums) == 1 and len(regs) >= 2 and not reloc:
            b = regs[-1]
            if b in base:  # a load/store through a data address: compare what it reads
                sname, soff = base[b]
                out[("ref", data.key(sname, soff + nums[0], LOAD_KIND.get(op.rstrip("u"))))] += 1
            elif b != "r1":
                width = {"w": 4, "h": 2, "b": 1, "fs": 4, "fd": 8}[mem.group(2)]
                kind = "s" if mem.group(1) == "l" and mem.group(3) == "a" else ""
                out[("mem", f"{'ld' if mem.group(1) == 'l' else 'st'}{width}{kind} {nums[0]:#x}")] += 1
            if dest and mem.group(1) == "l":
                base.pop(dest, None)
            continue
        if reloc:
            if rkind == "ha" or rkind == "h":
                base[dest] = (reloc, 0)
                continue
            if rkind == "l" and op in ("addi", "ori"):
                base[dest] = (reloc, 0)
                out[("ref", data.key(reloc))] += 1
                continue
            if mem:  # lfs f1, x@sda21 / lwz r3, x@l(r4)
                out[("ref", data.key(reloc, 0, LOAD_KIND.get(op.rstrip("u"))))] += 1
            else:
                out[("ref", data.key(reloc))] += 1
            if dest:
                base.pop(dest, None)
            continue
        if op == "addi" and len(regs) >= 2 and regs[1] in base and nums:  # address of something inside a block
            sname, soff = base[regs[1]]
            base[dest] = (sname, soff + nums[-1])
            out[("ref", data.key(sname, soff + nums[-1]))] += 1
            continue
        if op == "mr" and len(regs) == 2 and regs[1] in base:
            base[dest] = base[regs[1]]
            continue
        if dest:
            base.pop(dest, None)

        if op in CMP_OPS:
            out[("cmp", CMP_OPS[op] + (f" {nums[-1]:#x}" if op.endswith("i") and nums else ""))] += 1
            continue
        if op == "cror":
            out[("cmp", "cror " + ",".join(regs))] += 1
            continue
        if op.rstrip(".") in OPS:
            out[("op", op.rstrip("."))] += 1
        if op in IMM_OPS and nums:
            if op in ("addi", "subi", "addic", "subic") and regs[1:2] == ["r1"]:
                continue  # stack address
            v = nums[-1]
            if op in ("subi", "subic", "subic.", "subis"):
                v = -v
            if op in ("lis", "addis", "subis", "oris", "xoris", "andis."):
                v = (v & 0xFFFF) << 16
            logic = op in ("ori", "oris", "xori", "xoris", "andi.", "andis.")
            hexy = logic or op == "lis"
            out[("logic" if logic else "imm", f"{v & 0xFFFFFFFF:#x}" if hexy else str(v))] += 1
            continue
        if op.startswith(("rlwinm", "slwi", "srwi", "clrlwi", "clrrwi", "rotlwi", "rotrwi", "extrwi", "extlwi",
                          "clrlslwi")) and nums:
            r = rlw(op, nums)
            if r:
                sh, mb, me = r
                out[("mask", f"{rlw_mask(mb, me):#010x}" + (f" rot{sh}" if sh else ""))] += 1
    return out


# --- driver --------------------------------------------------------------------------------------

def run_objdiff(args):
    tmp = ROOT / "build" / "_semdiff.json"
    r = subprocess.run([str(OBJDIFF), "diff", *args, "-o", str(tmp), "--format", "json"], capture_output=True,
                       text=True, cwd=ROOT)
    if r.returncode:
        sys.exit("objdiff-cli failed: " + r.stderr[-1000:])
    return json.loads(tmp.read_text())


def compare(orig_o, ours_o, diff, funcs=None, include_matched=False):
    orig_data, ours_data = elf_symbols(orig_o), elf_symbols(ours_o)

    left = {s["name"]: s for s in diff["left"]["symbols"] if s.get("kind") == "SYMBOL_FUNCTION"}
    right = {s["name"]: s for s in diff["right"]["symbols"] if s.get("kind") == "SYMBOL_FUNCTION"}
    results = []
    for name in (funcs or sorted(right)):
        if name not in left or name not in right:
            continue
        pct = right[name].get("match_percent")
        if pct is None:
            continue
        if pct >= 100 and not include_matched:
            continue
        a = tokens(left[name], Data(orig_data, ours_data))
        b = tokens(right[name], Data(ours_data, orig_data))
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
    print(f"{len(flagged)} of {len(results)} functions flagged")


if __name__ == "__main__":
    main()
