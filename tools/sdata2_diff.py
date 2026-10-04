#!/usr/bin/env python3
"""Classify the gap between a unit's .sdata2/.rodata pool and the original's.

    uv run --with pyelftools python3 tools/sdata2_diff.py UNIT [--no-build] [--literal] [--json]

UNIT is the source path without src/ and .c (effect/eff_fire). Compares our object's entries with the target's by value
(not by name: dtk names every stripped pool entry, so a name says nothing about how the original wrote it) and prints per
section one verdict and the entries behind it:

  IDENTICAL   same entries, same order
  PAD         identical except zero bytes at the section tail (a size-only difference)
  ORDER       same multiset of values, different sequence (the first displaced entries and who emits them)
  CONTENT     values differ; the classes below say how

CONTENT classes, per value: EXTRA (ours has more copies than the target: the functions that emit them), MISSING (the
target has more copies than ours; a "dup" when ours has at least one: the target keeps a value twice that we merge) and
DUP_OURS (ours keeps two copies the target merged, the same as EXTRA of a value the target has). --literal first builds
the unit with every float_* constant written as a literal (pool_plan.literalise) and compares that build instead, which
shows the gap that remains when no named object is in the way; the source is restored afterwards. --json prints the
classification for tools."""
import json
import os
import struct
import subprocess
import sys

_argv = sys.argv
_flags = {a for a in _argv[1:] if a.startswith("--")}
_pos = [a for a in _argv[1:] if not a.startswith("--")]
if not _pos:
    sys.exit(__doc__)
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.argv = [_argv[0], _pos[0]]
import pool_plan as pp  # noqa: E402  (reads argv at import)

sys.argv = _argv

SECTIONS = (".sdata2", ".rodata")


def key_of(blob):
    return blob.rstrip(b"\0") if len(blob) > 4 else blob


def entries_of(obj, sec):
    """[{addr, name, blob, key, users}] for the section's entries; users = functions whose text refers to the entry."""
    data = obj.sec.get(sec, b"")
    ents = [dict(sec=sec, addr=a, name=n, blob=data[a:a + sz], key=key_of(data[a:a + sz]), users=[])
            for a, sz, n, _ in obj.objects(sec) if not n.startswith(("gap_", "pad_"))]
    funcs = obj.funcs()
    for off, _, _, tsec, addr in obj.relocs(".text"):
        if tsec != sec:
            continue
        fn = next((nm for a, sz, nm in funcs if a <= off < a + sz), "?")
        for e in ents:
            if e["addr"] <= addr < e["addr"] + max(len(e["blob"]), 1):
                if fn not in e["users"]:
                    e["users"].append(fn)
                break
    return ents


def classify(target, ours):
    """Pure comparison of two entry lists (dicts with key, name, users). Returns a dict with verdict and details."""
    tk = [e["key"] for e in target]
    ok = [e["key"] for e in ours]
    if tk == ok:
        return dict(verdict="IDENTICAL")
    if _strip_tail(tk) == _strip_tail(ok):
        return dict(verdict="PAD", tail=len(tk) - len(ok))
    tc, oc = {}, {}
    for k in tk:
        tc[k] = tc.get(k, 0) + 1
    for k in ok:
        oc[k] = oc.get(k, 0) + 1
    if tc == oc:
        i = next(i for i, (a, b) in enumerate(zip(tk, ok)) if a != b)
        return dict(verdict="ORDER", first=i, target=_row(target[i]), ours=_row(ours[i]),
                    displaced=sum(1 for a, b in zip(tk, ok) if a != b))
    extra, missing = [], []
    for k in set(tc) | set(oc):
        d = oc.get(k, 0) - tc.get(k, 0)
        if d > 0:
            es = [e for e in ours if e["key"] == k][-d:]
            extra += [dict(_row(e), target_has=tc.get(k, 0), _v=_f32(k)) for e in es]
        elif d < 0:
            es = [e for e in target if e["key"] == k][-(-d):]
            missing += [dict(_row(e), ours_has=oc.get(k, 0), _v=_f32(k)) for e in es]
    _tag_folds(extra, missing, tk, ok)
    cls = {"EXTRA": [e for e in extra if e["target_has"] == 0],
           "DUP_OURS": [e for e in extra if e["target_has"] > 0],
           "MISSING": [e for e in missing if e["ours_has"] == 0],
           "DUP_TARGET": [e for e in missing if e["ours_has"] > 0]}
    for rows in (extra, missing):
        for r in rows:
            r.pop("_v", None)
    return dict(verdict="CONTENT", classes={k: v for k, v in cls.items() if v}, extra=len(extra), missing=len(missing))


def _f32(blob):
    return struct.unpack(">f", blob)[0] if len(blob) == 4 else None


def _tag_folds(extra, missing, tk, ok):
    """Annotate EXTRA f32 rows whose negation the target holds (`neg`: the original negates at runtime, we folded the
    literal) and MISSING f32 rows that are a product/quotient of two values ours holds (`prod`: the original multiplies at
    runtime where we folded, or the reverse)."""
    tvals = {_f32(k) for k in tk if len(k) == 4} - {None}
    ovals = [_f32(k) for k in ok if len(k) == 4 and _f32(k) is not None]
    for e in extra:
        v = e.get("_v")
        if v is not None and -v in tvals:
            e["fold"] = "neg"
    for m in missing:
        v = m.get("_v")
        if v is None:
            continue
        for a in ovals:
            if a and any(abs(a * b - v) <= 1e-6 * max(1, abs(v)) or abs(a / b - v) <= 1e-6 * max(1, abs(v)) for b in ovals if b):
                m["fold"] = "prod"
                break


def _strip_tail(keys):
    out = list(keys)
    while out and not out[-1].strip(b"\0"):
        out.pop()
    return out


def _row(e):
    return dict(name=e["name"], value=pp.show(pp.kind_of(e["sec"], e["name"], e["blob"]), e["blob"]),
                users=e["users"][:4])


def report(sec, res):
    print(f"== {sec}: {res['verdict']}")
    v = res["verdict"]
    if v == "PAD":
        print(f"   target is {res['tail']} bytes longer: zero bytes at the section tail")
    elif v == "ORDER":
        print(f"   first displaced entry {res['first']}: target {res['target']['value']} ({', '.join(res['target']['users'])}) "
              f"vs ours {res['ours']['value']} ({', '.join(res['ours']['users'])}); {res['displaced']} entries out of place")
    elif v == "CONTENT":
        for cls, rows in res["classes"].items():
            print(f"   {cls} ({len(rows)}):")
            for r in rows[:12]:
                print(f"     {r['value']:<24} {r['name']:<28} {', '.join(r['users'])}" + (f"   [{r['fold']}]" if r.get("fold") else ""))


def build_literal():
    """Build with every float_* constant as a literal; returns the source text to restore, or None on failure."""
    sp = pp.source_path()
    orig = open(sp).read()
    new, _ = pp.literalise(orig, pp.Obj(pp.target_path))
    open(sp, "w").write(new)
    return sp, orig


def main():
    restore = None
    if "--no-build" not in _flags:
        if "--literal" in _flags:
            restore = build_literal()
        r = subprocess.run(["ninja", pp.ours_path], capture_output=True, text=True)
        if r.returncode:
            if restore:
                open(restore[0], "w").write(restore[1])
            sys.exit("build failed:\n" + "\n".join(r.stdout.splitlines()[-10:]))
    try:
        tobj, ours = pp.Obj(pp.target_path), pp.Obj(pp.ours_path)
        out = {sec: classify(entries_of(tobj, sec), entries_of(ours, sec)) for sec in SECTIONS}
    finally:
        if restore:
            open(restore[0], "w").write(restore[1])
            subprocess.run(["ninja", pp.ours_path], capture_output=True, text=True)
    if "--json" in _flags:
        print(json.dumps(out))
        return
    for sec in SECTIONS:
        report(sec, out[sec])


if __name__ == "__main__":
    main()
