#!/usr/bin/env python3
"""Put a written function where rel_gen left its TODO hint.
    python3 tools/fn_hint.py SOURCE.c FUNC BODY.c
Replaces the hint comment for FUNC with BODY's text, turns the forward declaration into USER_FUNC, and drops the
trailing `#pragma force_active off` (with -inline deferred it must stay on at the end of the file)."""
import re, sys

path, fn, body_path = sys.argv[1:4]
text, body = open(path).read(), open(body_path).read()
text, n = re.subn(rf"// TODO: write {re.escape(fn)} [^\n]*\n(?://[^\n]*\n)*", lambda m: body.rstrip("\n") + "\n\n", text)
if n != 1:
    sys.exit(f"{path}: no rel_gen hint for {fn}")
text = re.sub(rf"static void {re.escape(fn)}\(void\);[^\n]*", f"static USER_FUNC({fn});", text)
text = text.replace("\n#pragma force_active off\n", "\n")
open(path, "w").write(text)
