#!/usr/bin/env python3
"""Try compiler-flag variants on a dol unit and compare every function's score.

    python3 tools/flag_sweep.py battle/ac/ac_timing_a [--quick] [--json OUT]

Run from the repo root after `ninja` has configured the project. UNIT is the source path without src/ and .c.
Each variant recompiles the unit with the compile line from build.ninja, changing the -O level, the inline mode and
whether the file's `#pragma optimize_for_size` lines are kept. Objects go to a scratch dir; nothing in build/ is touched.

Prints one row per function (score under the flags the unit builds with today, then each variant) and a summary that
names the variants where no function scores lower than it does today.
"""
import concurrent.futures as cf
import itertools
import json
import os
import re
import subprocess
import sys
import tempfile

args = [a for a in sys.argv[1:] if not a.startswith("--")]
quick = "--quick" in sys.argv
out_json = sys.argv[sys.argv.index("--json") + 1] if "--json" in sys.argv else None
if out_json in args:
    args.remove(out_json)
if len(args) != 1:
    sys.exit(__doc__)
unit_path = args[0]
src = f"src/{unit_path}.c"
unit = "MarioSt/" + unit_path
ninja = open("build.ninja").read()
m = re.search(rf"^build (build/G8MJ01/src/{re.escape(unit_path)}\.o): mwcc_sjis\w* \$\n.*?\n  cflags = (.*?)\n  basedir", ninja, re.M | re.S)
if not m:
    sys.exit(f"{unit_path}: no mwcc rule in build.ninja")
base_flags = re.sub(r"\$\n\s*", "", m.group(2))
base_flags = re.sub(r"\s+", " ", base_flags).replace("-MMD", "").strip()
# quoted args (-pragma "cats off") stay one argument on the shell command line
project = json.load(open("objdiff.json"))
entry = next(e for e in project["units"] if e["name"] == unit)
text = open(src).read()
has_pragma = bool(re.search(r"^\s*#pragma\s+optimize_for_size", text, re.M))

opts = ["-O4,p", "-O4,s", "-O2,p", "-O2,s"] if not quick else ["-O4,p", "-O4,s"]
inlines = [None, "-inline auto"] if not quick else [None]
prag = [True, False] if has_pragma else [True]  # True: keep the file's pragmas


def variant_name(opt, inl, keep):
    return "/".join(filter(None, [opt or "asis", (inl or "").replace("-inline ", "inl:"), "" if keep else "nopragma"]))


def set_flag(flags, prefix, new):
    parts = re.findall(r'(?:"[^"]*"|\S)+', flags)
    kept, skip = [], False
    for p in parts:
        if skip:
            skip = False
        elif p.startswith(prefix):
            skip = prefix == "-inline" and p == "-inline"  # its mode is the next word
        else:
            kept.append(p)
    return " ".join(kept + [new])


variants = [("today", None, None, True)]
for opt, inl, keep in itertools.product([None] + opts, inlines, prag):
    if opt is None and inl is None and keep:
        continue
    variants.append((variant_name(opt, inl, keep), opt, inl, keep))

srcdir = os.path.dirname(src)
work = tempfile.mkdtemp(prefix="flag_sweep_")


def function_bytes(path):
    """{function: its machine code} from the object's .text, to tell 'same score' from 'same code'."""
    from elftools.elf.elffile import ELFFile
    with open(path, "rb") as f:
        e = ELFFile(f)
        text_sec = e.get_section_by_name(".text")
        data = text_sec.data() if text_sec else b""
        out = {}
        for sym in e.get_section_by_name(".symtab").iter_symbols():
            if sym["st_info"]["type"] == "STT_FUNC" and sym["st_size"] and e.get_section(sym["st_shndx"]).name == ".text":
                out[sym.name] = data[sym["st_value"]:sym["st_value"] + sym["st_size"]]
        return out


def build(v):
    name, opt, inl, keep = v
    idx = variants.index(v)
    outdir = os.path.join(work, f"v{idx}")
    os.makedirs(outdir)
    flags = base_flags
    if opt:
        flags = set_flag(flags, "-O", opt)
    if inl:
        flags = set_flag(flags, "-inline", inl)
    source = src
    if not keep:
        # the pragma-free copy lives in this variant's scratch dir; `-i` keeps the file's own relative includes working
        source = os.path.join(outdir, os.path.basename(src))
        stripped = re.sub(r"^\s*#pragma\s+optimize_for_size.*$", "", text, flags=re.M)
        with open(source, "w") as f:
            f.write(stripped)
        flags += f" -i {srcdir}"
    cmd = f"build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe {flags} -c {source} -o {outdir}"
    r = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    obj = os.path.join(outdir, os.path.splitext(os.path.basename(source))[0] + ".o")
    if r.returncode or not os.path.exists(obj):
        return name, None
    one = dict(project, units=[dict(entry, base_path=os.path.abspath(obj), target_path=os.path.abspath(entry["target_path"]))])
    with open(os.path.join(outdir, "objdiff.json"), "w") as f:
        json.dump(one, f)
    rr = subprocess.run(["build/tools/objdiff-cli", "report", "generate", "-p", outdir, "-o", "-"], capture_output=True,
                        text=True, env={**os.environ, "RAYON_NUM_THREADS": "1"})
    if rr.returncode:
        return name, None
    u = json.loads(rr.stdout)["units"][0]
    code = function_bytes(obj)
    return name, {f["name"]: (f.get("fuzzy_match_percent", 0.0), int(f["size"]), code.get(f["name"])) for f in u.get("functions", [])}


with cf.ThreadPoolExecutor(max_workers=6) as ex:
    results = list(ex.map(build, variants))
results = {n: r for n, r in results}
today = results.get("today")
if today is None:
    sys.exit("the unit's own flags do not build")
names = [n for n, _, _, _ in variants if results.get(n) is not None]
fns = sorted(today, key=lambda f: -today[f][1])


def weighted(r):
    return sum(p * s for p, s, _ in r.values()) / sum(s for _, s, _ in r.values())


print(f"{unit_path}: {len(fns)} functions, {len(names)} variants built (pragmas in file: {has_pragma})")
print("  (same code = the function's machine code is byte-identical to today's; instructions equal, relocation targets not compared)")
for n in names:
    r = results[n]
    drops = [f for f in fns if f in r and r[f][0] < today[f][0] - 1e-9]
    gains = [f for f in fns if f in r and r[f][0] > today[f][0] + 1e-9]
    same = sum(1 for f in fns if f in r and r[f][2] == today[f][2])
    full = sum(1 for f in r if r[f][0] >= 100)
    print(f"  {n:34s} 100%: {full:3d}  weighted {weighted(r):6.2f}  gains {len(gains):3d}  drops {len(drops):3d}  same code {same:3d}/{len(fns)}")


def lost_exact(n):
    return [f for f in fns if today[f][0] >= 100 and (f not in results[n] or results[n][f][0] < 100)]


safe = [n for n in names if n != "today" and not lost_exact(n)
        and all(results[n].get(f, (0,))[0] >= today[f][0] - 1e-9 for f in fns)]
print("no-drop variants (every function at 100% today is still 100%, no partial scores lower):", ", ".join(safe) or "none")
for n in safe:
    gains = [f for f in fns if results[n][f][0] > today[f][0] + 1e-9]
    changed = [f for f in fns if results[n][f][2] != today[f][2] and f not in gains]
    if gains:
        print(f"  {n}: " + ", ".join(f"{f} {today[f][0]:.2f}->{results[n][f][0]:.2f}" for f in gains))
    if changed:
        print(f"  {n}: different code at the same score: " + ", ".join(changed))
if out_json:
    json.dump({"unit": unit_path, "results": results}, open(out_json, "w"))
subprocess.run(["trash", work])
