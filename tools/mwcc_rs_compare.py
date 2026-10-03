#!/usr/bin/env python3
"""Compare mwcc-rs (GC/2.6 profile) against the real mwcceppc on TTYD's own source files.

For each src/*.c in the build, take its exact compile command from build.ninja, compile with the real compiler
and with mwcc-rs using the same flags, then compare every function's disassembly (instructions + relocations).
Run from the repo root on a machine with mwcc-rs built at ~/tools/mwcc-rs and pyelftools installed (the tower:
~/tools/mwcc_rs_test/.venv/bin/python). Writes build/mwcc_rs_results.json.

  python3 compare.py [--limit N] [UNIT ...]
"""
import json
import os
import re
import shlex
import subprocess
import sys
import tempfile
from pathlib import Path

MWCC_RS = Path.home() / "tools/mwcc-rs/target/release/mwcc"
TIMEOUT = 120


def compile_commands():
    """src path -> (real compiler argv, flags) from ninja."""
    out = {}
    for line in subprocess.run(["ninja", "-t", "commands"], capture_output=True, text=True).stdout.splitlines():
        if "mwcceppc.exe" not in line or " -c src/" not in line:
            continue
        cmd = line.split(" && ")[0].split(" || ")[0]  # drop the compile-timeout tail (tools/project.py)
        argv = shlex.split(cmd)
        src = argv[argv.index("-c") + 1]
        out[src] = argv
    return out


def functions(obj):
    """name -> (bytes with relocated fields zeroed, relocation list) for every function in the object."""
    from elftools.elf.elffile import ELFFile
    from elftools.elf.relocation import RelocationSection
    with open(obj, "rb") as fh:
        elf = ELFFile(fh)
        symtab = elf.get_section_by_name(".symtab")
        secs = {i: elf.get_section(i) for i in range(elf.num_sections())}
        relocs = {}
        for sec in elf.iter_sections():
            if isinstance(sec, RelocationSection):
                target = sec["sh_info"]
                for r in sec.iter_relocations():
                    sym = symtab.get_symbol(r["r_info_sym"])
                    name = sym.name or (secs[sym["st_shndx"]].name if isinstance(sym["st_shndx"], int) else "")
                    name = re.sub(r"@[0-9]+", "@N", name)
                    relocs.setdefault(target, []).append((r["r_offset"], r["r_info_type"], name, r["r_addend"]))
        out = {}
        for sym in symtab.iter_symbols():
            if sym["st_info"]["type"] != "STT_FUNC" or not isinstance(sym["st_shndx"], int) or not sym["st_size"]:
                continue
            sec = secs[sym["st_shndx"]]
            lo, hi = sym["st_value"], sym["st_value"] + sym["st_size"]
            data = bytearray(sec.data()[lo:hi])
            rl = []
            for off, typ, name, add in relocs.get(sym["st_shndx"], []):
                if lo <= off < hi:
                    rl.append((off - lo, typ, name, add))
                    k = off - lo
                    # mask the relocated field so only the instruction shape is compared
                    if typ in (10,):          # R_PPC_REL24: low 26 bits
                        data[k] &= 0xFC; data[k + 1] = data[k + 2] = 0; data[k + 3] &= 0x03
                    else:                     # 16-bit fields (ADDR16_*, EMB_SDA21, ...): low half
                        data[k + 2 if k + 2 < len(data) else k] = 0
                        if k + 3 < len(data):
                            data[k + 3] = 0
                        if typ == 109 and k + 1 < len(data):  # R_PPC_EMB_SDA21 also rewrites rA
                            data[k + 1] &= 0xE0
            out[sym.name] = (bytes(data), sorted(rl))
        return out


def run(argv, out_obj):
    try:
        r = subprocess.run(argv, capture_output=True, text=True, timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        return "timeout"
    if r.returncode or not os.path.exists(out_obj):
        return (r.stderr or r.stdout).strip().splitlines()[-1][:200] if (r.stderr or r.stdout).strip() else "failed"
    return None


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    limit = int(sys.argv[sys.argv.index("--limit") + 1]) if "--limit" in sys.argv else None
    cmds = compile_commands()
    srcs = sorted(cmds)
    if args:
        srcs = [s for s in srcs if any(s == f"src/{a}.c" for a in args)]
    if limit:
        srcs = srcs[:limit]
    results = {}
    tot = {"files": 0, "rs_failed": 0, "funcs": 0, "same": 0}
    with tempfile.TemporaryDirectory() as tmp:
        for src in srcs:
            argv = cmds[src]
            flags = [a for a in argv[argv.index("mwcceppc.exe") + 1 if "mwcceppc.exe" in argv else 0:]]
            i = next(k for k, a in enumerate(argv) if a.endswith("mwcceppc.exe"))
            flags = argv[i + 1:]
            o = flags.index("-o")
            flags = flags[:o] + flags[o + 2:]
            flags = [f for f in flags if f != "-MMD"]
            real_o, rs_o = f"{tmp}/real.o", f"{tmp}/rs.o"
            for p in (real_o, rs_o):
                if os.path.exists(p):
                    os.remove(p)
            err = run(argv[:i + 1] + flags + ["-o", real_o], real_o)
            if err:
                results[src] = {"real_error": err}
                continue
            tot["files"] += 1
            err = run([str(MWCC_RS), "--build", "2.6"] + flags + ["-o", rs_o], rs_o)
            real = functions(real_o)
            tot["funcs"] += len(real)
            if err:
                tot["rs_failed"] += 1
                results[src] = {"functions": len(real), "rs_error": err}
                print(f"{src}: mwcc-rs failed ({err[:90]})", flush=True)
                continue
            rs = functions(rs_o)
            same = [n for n, body in real.items() if rs.get(n) == body]
            tot["same"] += len(same)
            results[src] = {"functions": len(real), "same": len(same),
                            "differ": sorted(n for n in real if n not in same)}
            print(f"{src}: {len(same)}/{len(real)} functions identical", flush=True)
    Path("build/mwcc_rs_results.json").write_text(json.dumps({"totals": tot, "files": results}, indent=1))
    print(json.dumps(tot))


if __name__ == "__main__":
    main()
