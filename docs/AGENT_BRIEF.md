# Agent brief: matching TTYD functions

Read this before you start. It is what earlier agents got wrong, so you don't repeat it. Your task prompt says
which file is yours; this brief says how to work on it.

## What counts

A function is done only when it compiles to exactly the original bytes: 100% in `check.py`. Your work is
merged only if the whole project still builds, nothing that matched before gets worse, and a review finds no
tricks. A match produced by a trick is thrown away, and the review looks for tricks.

## Before writing code

- **Look up what already exists.** `grep` `include/` for every function, struct and global you use. If a header
  declares it, include the header; don't redeclare it. A local copy with different types is the most common
  review finding.
- **Read the original assembly for the whole function** (`build/G8MJ01/asm/<path>.s`) before writing C. Note
  every argument passed in `r3`-`r10` and `f1`-`f8`, and every value used after a call returns in `r3`/`f1`.
- **Check your file's compiler flags** in `configure.py` (`extra_cflags`). Some files are built with `-O4,s`.
  If every function in your file seems to want the same pragma, the file probably wants a flag instead: note
  it in your report rather than adding the pragma everywhere.

## Mistakes that got past the build and were caught in review

Each of these compiled to the right bytes and was still wrong. Don't do them.

1. **Calling a function without the arguments it takes,** relying on the value already sitting in `r3`.
   If the original loads `r3` before a `bl`, your C passes that value. Declare the callee with its real
   parameters (check its definition in `src/` or its header).
2. **Ignoring a return value the original uses,** or declaring a function `void` that returns something. If
   the caller uses `r3` after the call, the function returns a value; if its header says `void`, the header is
   probably wrong: say so in your report.
3. **Uninitialized variables.** If the original stores to a stack slot before passing its address (e.g. an
   in/out distance for a ray cast), your C must initialize it too.
4. **Local declarations that contradict a header,** or a `#define` that hides a header's prototype so you can
   redeclare it. If your match needs a different type than the header has, the header may be wrong. Report
   it with the evidence (e.g. "callers don't sign-extend the result, so it returns int, not s16"); don't
   work around it.
5. **Copies of existing structs.** Don't define a local struct overlaying one from `include/`. If you need a
   field that isn't named yet, list it in your report with its offset and type.
6. **Raw offsets where the header already names the field.** `*(f32*)((u8*)mario + 0x8C)` is `mario->position.x`.
   Raw offsets are fine only where the header has padding there; list those in your report.
7. **Pragmas that aren't needed.** Every `#pragma` needs a one-line reason, and must be needed: if the function
   still matches without it, remove it.
8. **Empty or placeholder bodies.** Never write `{}` or `return 0;` in place of real code. If you can't make a
   real attempt, leave the function out.

## Signedness hints from the assembly

- `lhz`/`lbz` loads mean unsigned (`u16`/`u8`); `lha`/`extsb` mean signed. If the header disagrees, report it.
- A caller that uses a returned value without `extsh`/`clrlwi` treats it as a full `int`.

## Partial functions

A function you write or change that doesn't reach 100% needs a comment directly above it:
`// NONMATCHING (NN%): what still differs`. Be specific ("r29/r30 swapped", "the loop counts down in the
original"). Keep it updated when the score changes.

## Your report

Update `REPORT.md` after every function (you can be stopped at any time). One line per function: name, size,
score before and after, and what made it match or what still differs. At the end, list:
- header problems you found (wrong types, missing fields, missing prototypes), with evidence;
- raw offsets you used because no field exists;
- compiler behaviour you learned that would help the next agent.
