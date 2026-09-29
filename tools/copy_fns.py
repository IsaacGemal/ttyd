#!/usr/bin/env python3
"""Fill a rel_gen'd source's function hints with functions from an already written source, and carry over its includes.
    python3 tools/copy_fns.py NEW.c FROM.c FUNC [FUNC ...] [--sub OLD=NEW ...]
--sub replaces a literal text in the copied functions (e.g. a different effect type). Statics/bss tables the functions
need still have to be edited by hand."""
import re, sys
args = sys.argv[1:]
subs = []
if "--sub" in args:
    i = args.index("--sub")
    subs = [a.split("=", 1) for a in args[i + 1:]]
    args = args[:i]
new, frm, *fns = args
s, f = open(new).read(), open(frm).read()
for fn in fns:
    m = re.search(rf"(?://[^\n]*\n)*static (?:void|USER_FUNC)\(?\s*[^\n]*\b{fn}\b[^\n]*\{{\n.*?\n\}}\n", f, re.S)
    body = m.group(0)
    for a, b in subs:
        body = body.replace(a, b)
    s, n = re.subn(rf"// TODO: write {fn} [^\n]*\n(?://[^\n]*\n)*", lambda _: body + "\n", s)
    if n != 1:
        sys.exit(f"no hint for {fn}")
    s = re.sub(rf"static void {fn}\(void\);[^\n]*", f"static USER_FUNC({fn});", s)
s = s.replace("\n#pragma force_active off\n", "\n")
inc = [l for l in re.findall(r"^#include [^\n]*", f, re.M) if l not in s]
open(new, "w").write("\n".join(inc) + "\n" + s if inc else s)
