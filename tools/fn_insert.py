#!/usr/bin/env python3
"""Put an already-matched function from another source into a rel_gen'd source.
    python3 tools/fn_insert.py NEW.c FROM.c FUNC [--at-hint]
Replaces NEW's forward declaration of FUNC with FROM's, removes rel_gen's TODO comment for it, and inserts FUNC's
definition (with the code around it in FROM up to the closing brace) where the TODO hint was (--at-hint) or at the end."""
import re, sys
new, frm, fn = sys.argv[1:4]
at_hint = "--at-hint" in sys.argv
s, f = open(new).read(), open(frm).read()
m = re.search(rf"^[^\n;]*\b{fn}\b[^\n;]*\{{\n", f, re.M)
end = f.index("\n}\n", m.start()) + 3
body = f[m.start():end]
decl = re.search(rf"^static [^\n]*\b{fn}\b[^\n{{]*;[^\n]*\n", f, re.M)
if decl:
    s = re.sub(rf"^static [^\n]*\b{fn}\b[^\n{{]*;[^\n]*\n", lambda _: decl.group(0), s, count=1, flags=re.M)
hint = re.search(rf"// TODO: write {fn} [^\n]*\n(?://[^\n]*\n)*", s)
if hint:
    s = s[:hint.start()] + ("@@FN@@\n" if at_hint else "") + s[hint.end():]
s = s.replace("\n#pragma force_active off\n", "\n")
if "@@FN@@" in s:
    s = s.replace("@@FN@@\n", body)
else:
    s = s.rstrip("\n") + "\n\n" + body
inc = [l for l in re.findall(r"^#include [^\n]*", f, re.M) if l not in s]
if inc:
    first = re.search(r"^#include [^\n]*\n", s, re.M)
    at = first.start() if first else 0
    s = s[:at] + "\n".join(inc) + "\n" + s[at:]
open(new, "w").write(s.rstrip("\n") + "\n")
