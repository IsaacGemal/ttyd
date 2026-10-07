"""Resolve a permuter queue UNIT name to its source, build.ninja target and original object, for dol and REL units.

    dol unit   `event/evt_bero`        src/event/evt_bero.c        build/G8MJ01/obj/event/evt_bero.o
    REL unit   `yuu/event/evt_yuucounter` (module/path)  or the bare `event/evt_yuucounter` when only one REL has it
                                       rels/yuu/src/event/evt_yuucounter.c    build/G8MJ01/yuu/obj/event/evt_yuucounter.o

Both kinds build to build/G8MJ01/src/<path>.o (the REL module is not part of the ninja target), and a REL's include path
(`-i rels/MOD/include`) is already in its cflags, so the compile command comes straight from the unit's build.ninja rule."""
import glob
import os
import re

BUILD = "build/G8MJ01"


def resolve(unit, root):
    """-> dict(src, name, target, mod): src relative to root, name = the build/G8MJ01/src/NAME.o stem, target = original .o."""
    rels = {os.path.basename(p) for p in glob.glob(os.path.join(root, "rels", "*")) if os.path.isdir(p)}
    first, _, rest = unit.partition("/")
    if rest and first in rels and os.path.exists(os.path.join(root, "rels", first, "src", rest + ".c")):
        return dict(src=f"rels/{first}/src/{rest}.c", name=rest, target=f"{BUILD}/{first}/obj/{rest}.o", mod=first)
    if os.path.exists(os.path.join(root, "src", unit + ".c")):
        return dict(src=f"src/{unit}.c", name=unit, target=f"{BUILD}/obj/{unit}.o", mod=None)
    libs = [p for p in sorted(glob.glob(os.path.join(root, "libs", "*", "src", unit + ".c")))]
    if len(libs) == 1:  # a ported library (libs/metrowerks, libs/dolsdk2004, libs/musyx): a DOL unit
        return dict(src=os.path.relpath(libs[0], root), name=unit, target=f"{BUILD}/obj/{unit}.o", mod=None)
    hits = [m for m in sorted(rels) if os.path.exists(os.path.join(root, "rels", m, "src", unit + ".c"))]
    if len(hits) == 1:
        return dict(src=f"rels/{hits[0]}/src/{unit}.c", name=unit, target=f"{BUILD}/{hits[0]}/obj/{unit}.o", mod=hits[0])
    if len(hits) > 1:
        raise LookupError(f"{unit}: in several RELs ({', '.join(hits)}); write MODULE/{unit}")
    raise LookupError(f"{unit}: no src/{unit}.c, libs/*/src/{unit}.c or rels/*/src/{unit}.c")


def cflags(ninja_text, name, mod=None):
    """The cflags of the mwcc rule that builds build/G8MJ01/src/NAME.o (any mwcc rule variant). A unit name repeated
    across RELs builds to build/G8MJ01/src/rel/MOD/NAME.o instead; pass MOD to fall back to that rule."""
    for stem in [name] + ([f"rel/{mod}/{name}"] if mod else []):
        m = re.search(rf"^build {re.escape(BUILD)}/src/{re.escape(stem)}\.o: mwcc\w*[^\n]*(?:\n(?!\S)[^\n]*)*?\n  cflags = (.*?)\n  basedir",
                      ninja_text, re.M | re.S)
        if m:
            break
    if not m:
        raise LookupError(f"{name}: no compile rule in build.ninja")
    return re.sub(r"\s+", " ", re.sub(r"\$\n\s*", "", m.group(1))).strip()


def source_hash(unit, root):
    """First 12 hex digits of the sha1 of the unit's source file ('-' when it cannot be resolved or read): the permuter log
    records it so a run on source that has since changed no longer counts as the unit's result."""
    import hashlib
    try:
        return hashlib.sha1(open(os.path.join(root, resolve(unit, root)["src"]), "rb").read()).hexdigest()[:12]
    except (LookupError, OSError):
        return "-"
