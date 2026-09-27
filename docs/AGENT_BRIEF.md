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
- **Write functions in the order the assembly has them.** A file can only be linked (counted as finished) when
  its functions, data and strings come out in exactly the original order. An out-of-order file matches every
  function and still can't link.
- **Say when your file wants different compiler flags.** If nearly every function in your file only matches
  with `#pragma optimize_for_size` (or differs only in `stmw`/`lmw` prologues and `divw`), the file is probably
  built with `-O4,s`. Say so in your report; the orchestrator sets it in `configure.py`. Never add a file-wide
  pragma instead.
- **Check your file's compiler flags** in `configure.py` (`extra_cflags`). Some files are built with `-O4,s`.
  If every function in your file seems to want the same pragma, the file probably wants a flag instead: note
  it in your report rather than adding the pragma everywhere.

## Mistakes that got past the build and were caught in review

Each of these compiled to the right bytes, or looked close enough, and was still wrong. Don't do them.

1. **Calling a function without the arguments it takes,** relying on the value already sitting in a register
   (`r3`-`r10`, or `f1`-`f8` for floats). If the original loads a register before a `bl`, your C passes that
   value. Declare the callee with its real parameters (check its definition in `src/` or its header).
   Real case: `win_main.c` declared `effRecoveryEntry(s32 type, f32 x, f32 y, f32 z)` locally, leaving out
   `s32 count`, and called it without the count. The original loads `item->hp_restored` into `r4` before
   the call (`lbz r4, 0x1d(r29)`); it did so anyway for a zero test, so the function scored 99.8% and the
   missing argument went unnoticed through several reviews. A local prototype hides this: the compiler only
   checks your call against your own copy. Before writing any prototype, find the function's definition or
   the original's callee and count its parameters.
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
7. **Match-only tricks left unmarked.** Code that exists only to reproduce the original bytes (a fake cast or
   temporary, an odd expression order, stack padding) goes inside `#ifdef MUST_MATCH`, with the natural form
   under `#else` where there is one; use `PAD_STACK(n)` from `matching.h` for padding. Unmarked tricks look like
   real logic to the next reader.
8. **Pragmas that aren't needed.** Every `#pragma` needs a one-line reason, and must be needed: if the function
   still matches without it, remove it.
9. **Wrong constants in functions that don't match yet.** Check every constant against the asm, even when the
   function is at 80% and "only registers differ": `oris r0, r0, 0xa1` is `| 0x00a10000`, not `0xa1000000`;
   `rlwinm r0, r0, 0, 16, 11` keeps IBM bits 16-11 (wrapping) and clears IBM bits 12-15, which are value bits
   16-19 (`1<<16` to `1<<19`, mask `0x000F0000`): PowerPC numbers bits from the top, so value bit = 31 - IBM
   bit. Mixing the two numberings is exactly how a wrong flag mask got written. A magic-number division tells
   you the divisor. A wrong constant in a partial function is a real bug that CI can't catch.
   Float constants too, even in functions at 100%: objdiff doesn't compare literal values. A symbol's name is
   rounded (`float_3p1416_80419d80` holds 3.1415927, not 3.1416): write the value from the `.float` line in the
   asm, not the name.
10. **Misreading common idioms.** `subi rX, rX, N; subic rX, rX, 1; subfe rX, rX, rX; and rX, rY, rX` means
   `x == N ? y : 0`, not `x < N`. `fcmpo; cror eq, gt, eq; bne` is a `>=` test. When unsure, write the C, build,
   and compare the instructions with `check.py FUNC`.
11. **Wrong argument order or destination,** e.g. which matrix a `PSMTXConcat` writes to. Map each argument
   register to your C.
12. **Calling a function with no prototype, or declaring one in your .c file.** The build rejects a call with no
   prototype (`-requireprotos`, as in Melee), and CI rejects function prototypes in .c files (static helpers
   excepted). Include the header that declares the function; if none does, add the prototype to the header of
   the file that defines it (`include/<path>.h` for `src/<path>.c`; for a REL module's `rels/<rel>/src/<path>.c`,
   `include/rel/<rel>/<path>.h`; the merge only accepts headers under `include/`), created if missing. Copy the definition's signature, or, if it isn't
   decompiled yet, read it from the original: the callee's asm shows which of `r3`-`r10`/`f1`-`f8` it reads and
   whether it leaves a result in `r3`/`f1` that callers use. Write each prototype on one line; the merge accepts
   only single-line declarations. If the original has the function local to one file (`.fn NAME, local` in the
   asm), it is not a header prototype: declare it `static` in that file. Never use `static` for any other
   function: CI checks each static declaration against the asm. Only add lines to headers; never change
   existing ones. If an existing header declaration looks wrong, report it with the asm evidence. Exception:
   `psndSFX_pit`, `psndSFX_vol`, `psndSFX_get_vol` and `psndSFXOnEx_3D` have no header prototype, because the
   original's files disagree on their widths; declare them in your .c file with the widths your file's asm uses
   (e.g. a `clrlwi r4, rX, 24` before the call means a `u8` argument).
13. **Writing a data or string address as a number,** or as an offset past a different symbol. Name the
   object (declare it as the original's symbol) instead.
14. **Empty or placeholder bodies.** Never write `{}` or `return 0;` in place of real code. If you can't make a
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
