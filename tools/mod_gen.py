#!/usr/bin/env python3
"""Write a REL's base unit (rels/<mod>/src/<mod>.c: _prolog, _epilog, _unresolved) from its disassembly.

    python3 tools/mod_gen.py MOD

Handles the plain shape: `_prolog` runs the constructors, then calls relSetEvtAddr("<mod>_NN", <mod>_NN_init_evt) and
relSetBtlAddr("<mod>", tbl1, tbl2) in order; `_epilog` runs the destructors, possibly with calls before or after the loop
(taken from the disassembly). Anything else in the unit (extra functions) is not handled: the tool says so and stops.
The declarations of the symbols come from the headers the REL already has; missing ones go into rels/<mod>/include/<mod>.h."""
import glob, os, re, sys

mod = sys.argv[1]
asm = open(f"build/G8MJ01/{mod}/asm/{mod}.s", errors="replace").read()
fns = re.findall(r"^\.fn (\w+), \w+", asm, re.M)
extra = [f for f in fns if f not in ("_prolog", "_epilog", "_unresolved")]
if extra:
    sys.exit(f"{mod}: extra functions {extra}: not the plain shape")
body = {n: b for n, b in re.findall(r"^\.fn (\w+), \w+\n(.*?)^\.endfn", asm, re.M | re.S)}
strings = {}
for name, s in re.findall(r"^\.obj (str_\w+), \w+\n\s*\.string \"((?:[^\"\\]|\\.)*)\"", asm, re.M):
    strings[name] = s.split("\\000")[0]


def calls(text):
    out, syms = [], []
    for line in text.split("\n"):
        line = re.sub(r"^/\* [0-9A-F ]+ \*/\t", "", line).strip()
        m = re.match(r"lis r\d+, (\w+)@ha", line)
        if m and m.group(1) not in ("_ctors", "_dtors"):
            syms.append(m.group(1))
        m = re.match(r"bl (\w+)", line)
        if m:
            out.append((m.group(1), syms))
            syms = []
    return out


prolog = calls(body["_prolog"])
body_e = body["_epilog"]
epilog = calls(body_e)
# where does the destructor loop sit relative to the calls in _epilog?
lines_e = [re.sub(r"^/\* [0-9A-F ]+ \*/\t", "", l).strip() for l in body_e.split("\n")]
loop_at = next(i for i, l in enumerate(lines_e) if l.startswith("lis r3, _dtors@ha"))
before = []
after = []
k = 0
for i, l in enumerate(lines_e):
    if l.startswith("bl "):
        (before if i < loop_at else after).append(l.split()[1])
lines = []
decls = set()
for name, syms in prolog:
    if name == "relSetEvtAddr":
        s, evt = syms[0], syms[1]
        lines.append(f'    relSetEvtAddr("{strings[s]}", {evt});')
        decls.add(("evt", evt))
    elif name == "relSetBtlAddr":
        s, t1, t2 = syms[0], syms[1], syms[2]
        lines.append(f'    relSetBtlAddr("{strings[s]}", {t1}, {t2});')
        decls.add(("tbl", t1))
        decls.add(("tbl", t2))
    else:
        sys.exit(f"{mod}: _prolog calls {name}: not handled")
# declarations already available?
hdr_text = ""
own_hdr = f"rels/{mod}/include/{mod}.h"
for h in glob.glob(f"rels/{mod}/include/*.h") + glob.glob(f"include/rel/{mod}/*.h") + glob.glob("include/data/*.h"):
    if os.path.abspath(h) != os.path.abspath(own_hdr):
        hdr_text += open(h).read()
new_decls = []
for kind, sym in sorted(decls):
    if re.search(rf"\b{re.escape(sym)}\b", hdr_text):
        continue
    new_decls.append(f"EVT_DEF({sym});" if kind == "evt" else f"extern u32 {sym}[];")
incs = ['#include "data/mapdata.h"', '#include "forcedata.h"']
os.makedirs(os.path.dirname(own_hdr), exist_ok=True)
open(own_hdr, "w").write('#pragma once\n\n#include "evt/evt_cmd.h"\n\nvoid _prolog(void);\nvoid _epilog(void);\nvoid _unresolved(void);\n' + ("\n" + "\n".join(new_decls) + "\n" if new_decls else ""))
incs.insert(0, f'#include "{mod}.h"')
# headers that declare the symbols
have = []
for h in sorted(glob.glob(f"rels/{mod}/include/*.h")):
    t = open(h).read()
    if any(re.search(rf"\b{re.escape(s)}\b", t) for _, s in decls) and os.path.basename(h) != f"{mod}.h":
        have.append(os.path.basename(h))
incs = [f'#include "{h}"' for h in have] + incs
n_neg = len(re.findall(r"^\.obj negone_one\$", asm, re.M))
has_three = "_three$localstatic" in asm
ext = []
blk = """     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)"""
for i in range(n_neg):
    ext.append(f"extend_data({mod}_{i},\n{blk}")
if has_three:
    ext.append(f"extend_data({mod}_{n_neg},\n     f64, _three, 3.0,\n     f64, _half, 0.5)")
out = "\n".join(incs) + """

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

#pragma force_active on
// clang-format off
""" + "\n".join(ext) + """
// clang-format on

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
""" + "\n".join(lines) + """
}

void _epilog(void) {
    voidfunctionptr* dtor;

""" + "".join(f"    {c}();\n" for c in before) + """    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
""" + "".join(f"    {c}();\n" for c in after) + """}

void _unresolved(void) {}
"""
os.makedirs(f"rels/{mod}/src", exist_ok=True)
open(f"rels/{mod}/src/{mod}.c", "w").write(out)
print(f"{mod}: wrote rels/{mod}/src/{mod}.c ({len(lines)} calls, {len(new_decls)} new declarations)")
