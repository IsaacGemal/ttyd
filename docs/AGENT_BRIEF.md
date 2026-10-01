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
- **Write functions in the original's order: usually the REVERSE of the assembly's (rule 18).** A file can only be
  linked (counted as finished) when its functions, data and strings come out in exactly the original order. An
  out-of-order file matches every function and still can't link.
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
15. **Switches read from the code instead of the jump table.** objdiff doesn't compare a jump table's entries,
   so case bodies can be swapped, and a missing fall-through (`break` where the original runs into the next
   case) can still score high. Map each case value to its label in the `.obj jumptable_...` data, and check
   whether each case body ends in a branch or runs on.

16. **Reusing code without saying where it came from.** Other open-source decompilations may already have what
   you need, and using them is fine if the project's license is CC0 or MIT (check its LICENSE file first; a
   project with no license is off-limits). Name every project and file you took code from in your report, keep
   anything you download or clone inside ./tmp in this directory (never /tmp or elsewhere), and don't search the
   rest of the machine. The merge lists everything you fetched.

17. **The other wrong code reviews keep finding** in partly matched functions (waves 13-14; add to 9-11). Before
   you stop, go back over your partial functions for these, each checked against the asm:
   - Data tables: the asm's `.4byte 0x8080817F` is the bytes 80 80 81 7F in that order. Three effect tables had
     every word byte-reversed; compare your initializer with the `.obj` block byte by byte.
   - GX enums by number: the original passes plain integers, so check the enum's value, not its name
     (`GX_TEVREG0` is 1, not 0; `GX_CC_ZERO` is 0xF and `GX_CC_ONE` 0xC; `GX_INDEX8` is 2).
   - Signed or unsigned: `cmplwi`/`cmplw` is an unsigned test (`(u32)x <= 1` also rejects negatives); `cmpwi` is
     signed. The same for `srwi` vs `srawi` and `mulhwu` vs `mulhw` in a division by a constant.
   - Nested loops need their own counters: an inner loop that reuses the outer counter ends the outer loop early.
   - Every case of a switch, including the ones that share a body (rule 15), and a table walked one entry per
     iteration (`addi rN, rN, 4` in the loop) is `tbl[i]`, not `tbl[0]`.
   - A float compare's direction (`cror eq, gt, eq` vs `lt`), and a subtraction's operand order.
18. **Functions in the original's order, which is the reverse of the asm's.** Game code (and the RELs) is compiled
   with `-inline auto,deferred`, which emits a file's functions in reverse source order. So write your file's
   functions in the REVERSE of the order they appear in the asm (the last function in the .s comes first in the .c).
   Written in the asm's order, every function can reach 100% and the file still won't link. It applies to every
   file built with `-inline auto,deferred`: game code, the RELs, and in libs/ at least MetroTRK, MSL and the
   runtime. Check your unit's cflags in configure.py; battle.c and dispdrv.c are built without it, as are the
   Dolphin SDK libraries and gon's and mri's own sources (rels/gon, rels/mri, built with cflags_rel_nodefer; the
   shared units in rels/common compiled into gon or mri stay deferred unless their Object says otherwise): in
   those, write functions in the asm's order.

## Signedness hints from the assembly

- `lhz`/`lbz` loads mean unsigned (`u16`/`u8`); `lha`/`extsb` mean signed. If the header disagrees, report it.
- A caller that uses a returned value without `extsh`/`clrlwi` treats it as a full `int`.

## Code shapes that fixed near-misses

When the instructions match but the stack slots, register allocation or scheduling differ, try these first:
- **A `GXColor` passed by value lands in the wrong stack slot** (`color = *(GXColor*)&dat_...;` followed by setting
  some channels): move the build into an inline that returns the colour, setting the same channels the original
  sets, e.g. `static inline GXColor f(u8 a) { GXColor c = *(GXColor*)&dat_...; c.a = a; return c; }` and
  `color = f(a);` (effBattenDisp2; effButterflyDisp and effStardust2Disp set .r/.g/.b instead). Keep the inline and
  the call under `#ifdef MUST_MATCH`, with the plain assignment under `#else` (rule 7). It doesn't help when a
  function passes several colours by value.
- **A `Vec` temp copied from a named zero vector schedules too early** (`position = vec3_...;` at the top of
  `effXxxMain`): build it in an inline that starts from the literal `(Vec){ 0.0f, 0.0f, 0.0f }` and sets the fields,
  `position = makeVec(x, y, z);` (see eff_vapor_n64), placed above the unit's other rodata. The anonymous literal is deliberate here: it replaces the named zero vector only where the unit's
  `.rodata` score doesn't drop (CI checks that).
- **A blend `(C - v) * k + v` computes in the wrong order**: `f32 t = C - v; v = t * k + v;` under
  `#ifdef MUST_MATCH` (eff_mahorn).
- **A loop stepping two pointers increments in the wrong order**: put both in the loop header, in the target's
  order (`i++, data++`; eff_starpoint).
- **Float literals are pooled in the order the compiler first meets them,** function by function in `.text` order,
  and within a function in expression-tree order, with a call's argument constants pooled after the other constants of
  the statement that holds the call: `a = 0.04f * sinf(6.28f * x / 360.f) + 0.5f;` pools 0.5, 0.04, 6.28, 360, while
  `f32 s = sinf(6.28f * x / 360.f); a = 0.04f * s + 0.5f;` pools 6.28, 360, 0.5, 0.04 and compiles to the same code.
  When a unit's `.sdata2` differs from the target only in the order of some constants, a temp for the call result
  (or the reverse, folding it back) changes the pool without changing any instruction.
- **A product of two literals folds** (`-16.0f * 0.5f` becomes `-8.0f`), so the target's two separate constants
  vanish: `f32 sixteen = 16.0f; f32 half = 0.5f; f = -sixteen * half;` keeps both.

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
