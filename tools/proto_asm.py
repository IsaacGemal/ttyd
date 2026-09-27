#!/usr/bin/env python3
"""Check header prototypes against the original's assembly, for functions that have no C definition yet.

  python3 tools/proto_asm.py            report problems (exit 1 if any)
  python3 tools/proto_asm.py --all      also check functions that are defined in C (informational)

For a function defined in C, the compiler checks its header prototype against the definition. For one that
isn't decompiled yet, the prototype was written from how callers used it, and nothing checks it. This compares
it with the original (build/G8MJ01/**/asm/*.s):
- ARGS: the function reads an argument register (r3-r10, f1-f8) before writing it, beyond the prototype's
  parameters: the prototype (and probably its callers) drop an argument.
- VOID: the prototype returns void, but a caller reads r3 or f1 right after the call: it returns a value.
- LOCAL: the original has the function local to one file (`.fn NAME, local`) and no global of that name
  exists: it belongs as a static forward declaration in that file, not in a header. Checked for every
  prototype, decompiled or not.
The argument scan is linear up to the first call or return, so it can miss arguments first read later, and it
reports false ARGS where a loop's test sits at the bottom and loads a register the scan meets first (--all shows
such cases for decompiled functions that match 100%). A clean result is not proof, and every finding needs
checking by hand in the asm. Run after a build.
"""
import collections
import glob
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
REG = re.compile(r"\b([rf])(\d{1,2})\b")
FN = re.compile(r"^\.fn (\w+), (\w+)")
NOWRITE = re.compile(r"^(st\w*|cmp\w*|fcmp\w*|b\w*|mt\w*|tw\w*|dcb\w*|icbi|sync|isync|psq_st\w*)$")
PROTO = re.compile(r"^\s*(?:extern\s+)?([A-Za-z_][\w\s\*]*?)\b(\w+)\s*\(([^;{}]*)\)\s*;", re.M)
USERFUNC = re.compile(r"^\s*USER_FUNC\((\w+)\)\s*;", re.M)
FLOAT = re.compile(r"^(?:const\s+)?(f32|f64|float|double)$")


def asm_functions():
    """name -> list of (scope, instruction lines) for every function in the original's asm."""
    out = collections.defaultdict(list)
    for s in glob.glob(str(ROOT / "build/G8MJ01/**/asm/**/*.s"), recursive=True):
        cur = None
        for line in open(s, errors="ignore"):
            m = FN.match(line)
            if m:
                cur = (m.group(2), [])
                out[m.group(1)].append(cur)
            elif line.startswith(".endfn"):
                cur = None
            elif cur is not None and "*/\t" in line:
                cur[1].append(line.split("*/\t", 1)[1].strip())
    return out


def split(ins):
    op, _, rest = ins.partition(" ")
    return op, [o.strip() for o in rest.split(",")] if rest else []


def arg_regs(lines):
    """(int args, float args) the function reads before writing, scanning up to its first call or return."""
    written, read = set(), set()
    for ins in lines:
        op, ops = split(ins)
        if op in ("bl", "blr", "bctr", "bctrl", "blrl") or op.startswith("b") and op.endswith("lr"):
            break
        regs = [REG.findall(o) for o in ops]
        dest = None if NOWRITE.match(op) or not ops else regs[0][0] if len(regs[0]) == 1 and ops[0] == "".join(regs[0][0]) else None
        for i, rs in enumerate(regs):
            for kind, n in rs:
                r = (kind, int(n))
                if (i == 0 and dest == (kind, n)) or r in written:
                    continue
                read.add(r)
        if dest:
            written.add((dest[0], int(dest[1])))
        if op.startswith("stmw") or op.startswith("lmw"):
            continue
    ints = max([n - 2 for k, n in read if k == "r" and 3 <= n <= 10], default=0)
    floats = max([n for k, n in read if k == "f" and 1 <= n <= 8], default=0)
    return ints, floats


def return_used(callers, name):
    """True if some caller reads r3 or f1 after `bl name` before writing it."""
    for lines in callers:
        for i, ins in enumerate(lines):
            if ins != f"bl {name}":
                continue
            for nxt in lines[i + 1:i + 6]:
                op, ops = split(nxt)
                if op.startswith("b") and op != "bl":
                    break
                if op == "bl":
                    break
                regs = [REG.findall(o) for o in ops]
                dest = None if NOWRITE.match(op) or not ops else (regs[0][0] if regs[0] else None)
                used = [r for j, rs in enumerate(regs) for r in rs if not (j == 0 and dest == r)]
                if ("r", "3") in used or ("f", "1") in used:
                    return True
                if dest in (("r", "3"), ("f", "1")):
                    break
    return False


def params(args):
    """(int registers, float registers) the prototype's parameters occupy, as the highest register used: a 64-bit
    integer takes an aligned pair (r3:r4, r5:r6, ...), skipping a register if needed. None for varargs."""
    args = args.strip()
    if "..." in args:
        return None
    if args in ("", "void"):
        return 0, 0
    reg, floats = 3, 0
    for a in args.split(","):
        toks = a.replace("*", " * ").split()
        if len(toks) > 1 and toks[-1] != "*" and re.fullmatch(r"\w+", toks[-1]):
            toks = toks[:-1]  # drop the parameter name, if there is one
        t = " ".join(toks)
        base = t.replace("const ", "").strip()
        if "*" not in t and FLOAT.match(base):
            floats += 1
        elif "*" not in t and re.fullmatch(r"(s64|u64|OSTime|long long|unsigned long long)", base):
            reg += (reg - 3) % 2  # pairs start at an odd register: r3, r5, r7, r9
            reg += 2
        else:
            reg += 1
    return reg - 3, floats


def c_definitions():
    names = set()
    for f in glob.glob(str(ROOT / "src/**/*.c"), recursive=True) + glob.glob(str(ROOT / "rels/**/*.c"), recursive=True):
        t = open(f, errors="ignore").read()
        names.update(re.findall(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;{}]*\)\s*\{", t, re.M))
        names.update(re.findall(r"^\s*(?:static\s+)?USER_FUNC\((\w+)\)\s*\{", t, re.M))
    return names


def main():
    check_all = "--all" in sys.argv
    fns = asm_functions()
    all_lines = [lines for defs in fns.values() for _, lines in defs]
    defined = c_definitions()
    problems = []
    for h in sorted(glob.glob(str(ROOT / "include/**/*.h"), recursive=True)):
        text = re.sub(r"//[^\n]*|/\*.*?\*/", "", open(h, errors="ignore").read(), flags=re.S)
        rel = str(Path(h).relative_to(ROOT))
        decls = [(" ".join(m.group(1).split()), m.group(2), m.group(3)) for m in PROTO.finditer(text)]
        # USER_FUNC(name); is an event function: s32 name(EventEntry* event, BOOL isFirstCall)
        decls += [("s32", n, "EventEntry* event, BOOL isFirstCall") for n in USERFUNC.findall(text)]
        for ret, name, args in decls:
            if name not in fns or ret.startswith("typedef"):
                continue
            scopes = {s for s, _ in fns[name]}
            if scopes == {"local"}:
                problems.append(f"LOCAL  {rel}: {name} is local to one file in the original")
                continue
            if name in defined and not check_all:
                continue
            p = params(args)
            glob_defs = [lines for s, lines in fns[name] if s != "local"]
            if p is not None and len(glob_defs) == 1:
                ai, af = arg_regs(glob_defs[0])
                if ai > p[0] or af > p[1]:
                    problems.append(f"ARGS   {rel}: {name}({args.strip()}) declares {p[0]} int/{p[1]} float "
                                    f"parameters; the original reads {ai} int/{af} float")
            if ret == "void" and return_used(all_lines, name):
                problems.append(f"VOID   {rel}: {name} is declared void; a caller uses r3/f1 after the call")
    print("\n".join(problems) if problems else "no problems found")
    return 1 if problems else 0


if __name__ == "__main__":
    raise SystemExit(main())
