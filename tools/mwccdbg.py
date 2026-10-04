#!/usr/bin/env python3
"""Dump MWCC's internal state for one function (cadmic/mwcc-debugger): the PCode before and after each backend pass and the
register allocator's priority list, to see WHY a register differs from the original.

    python3 tools/mwccdbg.py UNIT FUNC [--src other.c] [--out DIR]       UNIT: event/evt_bero, yuu/event/evt_yuucounter (REL)

Adapted from tw2004's tools/matching/mwccdbg.py (CC0). It runs the unit's own compile command (from build.ninja, GC/2.6, the
compiler this project builds with) under retrowin32's x86 emulator with the gdb stub, driven by cadmic/mwcc-debugger, which has no
licence: it is cloned and run in place, never copied into this repository. Output: DIR (default build/mwccdbg/FUNC):
backend-NN-*.txt, frontend-*.txt, regalloc-gpr/fpr-pass-N-assigned.txt and summary.txt (each variable's register in priority order
with its neighbour count, and the register the original has where ours differs: `!`). About 7 s per function. --src compiles a
scratch copy of the unit (same flags).

Setup, once per machine (Linux or macOS; needs gdb, cmake, Rust; no sudo): see docs/AGENT_BRIEF.md "mwcc-debugger". The clones live in
$MWCCDBG_HOME (default ~/mwccdbg: mwcc-debugger/ and retrowin32/ on its gdb-stub branch, built with `cargo build -p retrowin32 -F
x86-unicorn --profile lto`), and gdb must be on PATH (set MWCCDBG_PATH to prepend directories). Read the dumps with tools/rasim.py."""
import os
import pathlib
import random
import re
import shlex
import shutil
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import perm_unit  # noqa: E402

HOME = pathlib.Path(os.environ.get("MWCCDBG_HOME", pathlib.Path.home() / "mwccdbg"))
LOCK_PORT = int(os.environ.get("MWCCDBG_LOCK_PORT", "9099"))
UP = ("import fcntl, os, socket, struct, sys; "
      "fcntl.ioctl(socket.socket(), 0x8914, struct.pack('16sH14s', b'lo', 0x41, bytes(14))); "
      "os.execv(sys.executable, [sys.executable] + sys.argv[1:])")


class net_lock:
    """Machine-wide mutex: holds TCP port LOCK_PORT on 127.0.0.1 (waits for it, a run takes about 7 s)."""

    def __enter__(self, timeout=900):
        import socket
        deadline = time.time() + timeout
        while True:
            self.s = socket.socket()
            try:
                self.s.bind(("127.0.0.1", LOCK_PORT))
                self.s.listen(1)
                return self
            except OSError:
                self.s.close()
                if time.time() > deadline:
                    sys.exit(f"mwccdbg: waited {timeout} s for port {LOCK_PORT} (another run holds the lock)")
                time.sleep(random.uniform(1, 4))

    def __exit__(self, *a):
        self.s.close()


def compile_args(unit, src_override=None, out_obj="obj.o"):
    """mwcceppc.exe + flags for the unit, relative to the repo root, compiling src to out_obj."""
    res = perm_unit.resolve(unit, str(ROOT))
    flags = [t for t in shlex.split(perm_unit.cflags((ROOT / "build.ninja").read_text(), res["name"])) if t != "-MMD"]
    return res, ["build/compilers/GC/2.6/mwcceppc.exe", *flags, "-c", src_override or res["src"], "-o", out_obj]


def dis(fn, obj):
    """[(mnemonic, operands)] of fn in obj, in perm_objdump's layout."""
    r = subprocess.run([sys.executable, str(ROOT / "tools/perm_objdump.py"), str(obj)], capture_output=True, text=True,
                       env=dict(os.environ, PERM_FUNC=fn))
    return [tuple(ln.split("\t")[2:4]) for ln in r.stdout.splitlines() if ln.count("\t") >= 3]


def regmap(fn, target, ours):
    """{our register: the original's register}: the most common pairing over instructions that line up."""
    import collections
    import difflib
    if not (pathlib.Path(target).exists() and pathlib.Path(ours).exists()):
        return {}
    a, b = dis(fn, target), dis(fn, ours)
    reg = re.compile(r"\b([rf]\d+)\b")
    pairs = collections.defaultdict(collections.Counter)
    sm = difflib.SequenceMatcher(None, [x[0] for x in a], [x[0] for x in b], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            continue
        for x, y in zip(a[i1:i2], b[j1:j2]):
            ra, rb = reg.findall(x[1]), reg.findall(y[1])
            if len(ra) == len(rb):
                for p, q in zip(rb, ra):
                    if p not in ("r1", "r2", "r13"):
                        pairs[p][q] += 1
    return {k: c.most_common(1)[0][0] for k, c in pairs.items()}


def summary(out, want):
    """Per allocation pass: each variable in priority order, its register, neighbour count, and the original's register."""
    lines = []
    files = sorted(out.glob("regalloc-*-assigned.txt"))
    for f in files:
        cls = f.name.split("-pass-")[0]
        later = [g.name for g in files if g.name.split("-pass-")[0] == cls and g.name > f.name]
        lines.append("== " + f.name + ("   (superseded by %s: ignore the original's column here)" % later[-1] if later else ""))
        for m in re.finditer(r"^(r\d+|f\d+) -> (\S+)( [^\n]*)?\n(?:.*\n){4}\s*neighbors: (\d+)", f.read_text(), re.M):
            if m.group(2) not in ("r0", "f0") or m.group(3):
                w = want.get(m.group(2), "")
                lines.append("  %-5s -> %-4s %sORIG %-4s %3s nb  %s" % (
                    m.group(1), m.group(2), "!" if w and w != m.group(2) else " ", w or "?", m.group(4), (m.group(3) or "").strip()))
    return lines


def main():
    a = sys.argv[1:]
    if len(a) < 2:
        sys.exit(__doc__)
    opt = lambda k, d: a[a.index(k) + 1] if k in a else d
    unit, fn = a[0], a[1]
    out = pathlib.Path(opt("--out", str(ROOT / "build/mwccdbg" / fn)))
    script, emu = HOME / "mwcc-debugger/mwcc_debugger.py", HOME / "retrowin32/target/lto/retrowin32"
    for p in (script, emu):
        if not p.exists():
            sys.exit(f"{p} missing: see docs/AGENT_BRIEF.md \"mwcc-debugger\"")
    res, args = compile_args(unit, opt("--src", None), str(out / "obj.o"))
    if out.exists():
        shutil.rmtree(out)
    out.mkdir(parents=True)
    env = os.environ.copy()
    env["PATH"] = os.pathsep.join(filter(None, [env.get("MWCCDBG_PATH", ""), str(HOME / "env/bin"), "/opt/homebrew/bin", env.get("PATH", "")]))
    dbg = [sys.executable, str(script), "-e", str(emu), "-a", shlex.join(args), fn, str(out)]
    # The emulator's gdb stub listens on a fixed port (9001), so two runs at once would talk to each other's emulator. Linux with
    # a network namespace of its own per run: no conflict. Otherwise runs take turns on a lock that every process on the machine
    # shares, whatever its /tmp: a TCP port bound on 127.0.0.1 (released when the run ends, even if it dies).
    own_ns = (sys.platform.startswith("linux") and shutil.which("unshare")
              and subprocess.run(["unshare", "-n", "true"], capture_output=True).returncode == 0)
    if own_ns:
        r = subprocess.run(["unshare", "-n", sys.executable, "-c", UP] + dbg[1:], cwd=ROOT, capture_output=True, text=True, env=env)
    else:
        with net_lock():
            r = subprocess.run(dbg, cwd=ROOT, capture_output=True, text=True, env=env)
    if not list(out.glob("*before-regalloc*")):
        sys.exit((r.stdout + r.stderr)[-1500:])
    # the emulated run writes no object: compile ours with the build's own command to map the original's registers
    real = ["build/tools/wibo", "build/tools/sjiswrap.exe", *compile_args(unit, opt("--src", None), str(out / "obj.o"))[1]]
    subprocess.run(real, cwd=ROOT, capture_output=True)
    lines = summary(out, regmap(fn, ROOT / res["target"], out / "obj.o")) + [f"dumps in {out}"]
    (out / "summary.txt").write_text("\n".join(lines) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
