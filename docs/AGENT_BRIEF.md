# Agent brief: matching TTYD functions

Read this before you start. It is what earlier agents got wrong, so you don't repeat it. Your task prompt says
which file is yours; this brief says how to work on it.

## What counts

A function is done only when it compiles to exactly the original bytes: 100% in `check.py`. Your work is
merged only if the whole project still builds, nothing that matched before gets worse, and a review finds no
tricks. A match produced by a trick is thrown away, and the review looks for tricks.

Wave agents use `check.py` for local iteration. Do not run or install the decomp-permuter in a wave workspace;
the dedicated `tools/perm_loop.py` process handles permuter jobs under its shared CPU limit.

## Before writing code

- **Read `check.py FUNC` by its marks.** `!!` is an instruction that really differs. `~~` differs only in the data
  symbol (the original names `float_0_804210b0`, our unlinked object has `@322` or `...rodata.0`): it goes away when
  the unit links, so never chase it. The last line counts both; fix the `!!` count to zero and the function is done.
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
   real logic to the next reader. Anything defined under MUST_MATCH and used outside it (a helper, a macro, a
   local) needs an `#else` form too, even if that is a `#define` to the plain version: the merge compiles the
   file without MUST_MATCH, and `check.py --final` now runs that compile too (a `NATURAL` line).
8. **Pragmas that aren't needed.** Every `#pragma` needs a one-line reason, and must be needed: if the function
   still matches without it, remove it. Never write `#pragma scheduling <cpu number>` (`601`, `801`, ...): it
   makes this compiler loop forever, and check.py refuses it. `scheduling off` / `reset` are fine.
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

16. **Reusing code without saying where it came from.** The network is not available: commands you run have no
   internet access, so `curl`, `git clone` and package installs fail. The MIT/CC0 reference decomps are on disk,
   read-only, at ~/ttyd-refs/<project> (ac-decomp, spm-decomp, tp, tww, musyx; ~/ttyd-refs/README.md has the commits
   and licenses); using their code is fine, and your report names every project and file you took code from.
   Do not read, list or search outside this directory and ~/ttyd-refs (no `find /`, no `ls ~/ttyd-agents`, no other
   agents' workspaces or bases).
   **Never kill processes by name** (`pkill -f mwcceppc`, `killall`): stop only what you started, by its own PID
   (`kill $!`). A stuck compile is stopped for you by the launcher's timeout.
   **No background jobs and no searches of your own** (`nohup ... &`, a cloned decomp-permuter, `-j` above 2): your
   slot has a fixed CPU share, and the project runs the permuter in its own lane (tools/perm_run.py). Work in the
   foreground, one build at a time.

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

## Source shapes that change code (from bfbb, CC0; each marked rule re-measured on GC/2.6 with `-O4,p -inline auto,deferred`)

When instructions match and registers or scheduling do not, the source form is usually the reason. Verified on 2.6:

- **Float tests:** `if (f)` and `if (!f)` emit `fcmpu f, 0` (variable first); `f != 0.0f` and `f == 0.0f` emit `fcmpu 0, f` (zero first).
  `a <= b` / `a >= b` always go through `fcmpo; cror eq, lt|gt, eq`; `a < b` and `!(a > b)` are a plain `bge`/`bgt`. The branch mnemonic
  tells you which spelling the author used. `a < b ? a : b` and `a < 0 ? -a : a` stay a compare and `fneg`: no `fsel`/`fabs`; an
  `fsel`/`fabs` in the target came from a helper or intrinsic.
- **`switch`:** 6 arms compile to a `cmpwi` tree, 7 or more to a `bctr` jump table, and 7 arms with a hole in the range still table: the
  arm count decides, not density. A `cmpwi` tree where the target has `bctr` means you are short an arm (possibly an empty `case`).
- **Loops:** `for (i = 0; i < N; i++)` is `li 0 ... addi / cmpwi N / blt` (body first); `for (i = N; i > 0; i--)` is `subic. / bgt`; `i = N;
  while (--i)` is `b` to the test, `subic. / bne`, and runs the body N-1 times (the immediate is not the trip count).
- **Division/modulo by a constant:** the magic number names the divisor and signedness (`lis 0x5555; addi 0x5556; mulhw` is `% 3`,
  `0x6667 mulhw` is `/ 10`, `0xcccd mulhwu` unsigned `/ 10`, `srawi + addze` is `/ 8` where a bare `srawi` is `>> 3`). `(j + 1) % 3` keeps the
  `mulhw` and `mulli`; `(j == 2) ? 0 : j + 1` compiles to a branch-free select with no multiply (and frees the callee-saved register that
  held the magic constant), so pick by what the target contains.
- **Masks:** `*u16 &= 0xfffd` emits `andi.`; `&= ~0x2` emits `rlwinm 31, 29`.
- **Small constant-trip fills:** `p[0] = ...; p[1] = ...; p[2] = ...;` lets the optimiser scalarise a local array away (no stack slots, no
  stores); the same fills in a `for (i = 0; i < 3; i++)` loop are unrolled but keep the array and its stores. A frame that is short by one
  array-sized chunk with the right instruction count is this.
- **Register numbering (callee-saved, GPR and FPR):** parameters take the lowest numbers; named locals take higher ones in *declaration*
  order, the first declared getting r31/f31 (declared `a, b, c` with the parameter in r29, `a` is r31 and `b` r30; declared `c, b, a` they
  swap).  A struct whose address is never taken is scalarised and its fields are numbered by *first write*, not field
  order (`v.z = ..; v.x = ..;` puts `z` in f31); plain scalars follow declaration order. If the target's registers are a rotation of
  yours, permute declaration order first, or replace such a struct with scalars declared in write order.
- **Allocator model and levers (from MarkMcCaskey/decomp-scripts `mwcc-compiler-notes.md`, MIT; written for GC/1.2.5, checked on GC/2.6
  with small repros and mwccdbg/rasim):** the allocator is Chaitin with Briggs-style optimistic colouring, per class in the order
  vector, GPR, FPR. Copy coalescing is aggressive and the LOWER-numbered web survives; liveness and dead-definition elimination run
  before it, so a copy that is dead never becomes a candidate. Simplify removes any node below K neighbours, then the lowest
  cost/degree; select pops in reverse, takes the lowest free bit of the volatile set, else claims r31, r30, ... on demand (f31... for FPRs),
  so a callee-saved register's identity is a function of select order. Our rasim replays this exactly (0 of 260 differ on
  nameKirinukiGX). A swap that survives every source experiment is often a coalescing-root or rank difference.
  *Holds on 2.6:* declaration order of long-lived locals (first declared gets r31; `a,b,c` vs `c,b,a` swaps them); a parameter that
  lives across calls colours after every named local (`int L1(int p)` with locals a, b: a r31, b r30, p r29); an alias that is a real
  computation (`a2 = a + z`) is a new web and costs one more callee-saved register.
  *Did not move anything in a minimal repro on 2.6:* a plain copy local (`T* q = p;`, `int a2 = a;`, declared before or after, with
  or without an initialiser: identical code to the uncopied form, the copy is propagated away before allocation); declaration with
  initialiser vs separate assignment; splitting a reused local when the two lives do not overlap. His "initialiser-bearing pointer
  local" lever therefore needs a copy that survives propagation (a loop-carried pointer, an address used by several blocks, or a
  `+ zero` alias as in tou_gamen_screen_tev); try it on the real function and check mwccdbg for the vreg, not the register.
  `int` vs `u16` changes emitted code (a `clrlwi`), so it is a lowering lever, not a pure allocator one. Wrapping a local in a
  one-field struct memory-homes it (stack slot) and was not tried as a pure allocator lever.
  *Classes that source edits do not reach* (his three, which match our NO-SOURCE-LEVER stops): (1) terminal callee-saved permutations
  over compiler-materialised webs (`@N` CSE temps, the i2f magic, global-address webs) that no nameable local owns; (2) one extra
  callee-saved register at peak pressure, hundreds of downstream diffs from one frontend lowering difference (the `stmw`/FPR-save
  start differs); (3) many-to-many reallocations. rasim's `search` over the `@` temps of nameKirinukiGX reaches 1-2 wrong, which says the
  creation order of the frontend temps is the whole difference, and that order is fixed by the expression order.
  *Stage cheat-sheet:* expression shape or folded/propagated values -> frontend optimizer (IRO_*); missing/extra CSE, hoisted loop
  values -> backend pass sequence; instruction order only -> scheduling or peephole; register identity only -> coalescing roots or select
  order; stack offsets and frame size -> aggregates and addressed locals; `stmw rN` / FPR save count -> lowest claimed callee-saved
  register (select order or peak pressure). Addressed locals take stack slots in reverse declaration order (inferred), so a wrapper
  struct declared before one can fill an existing hole without moving the rest.
- **Pinning an early parameter save (`mr r28, r3` before the first load):** when retail copies a parameter into its
  callee-saved register at the top and ours does it later, a dead use of the parameter at its first use can pin the
  copy without adding an instruction: `strcmp((const char*)(s64)(s32)name, ...)` (npcSetStayPose/TalkPose, 98.84 -> 100;
  the sign-extend round trip leaves a value the scheduler orders against, then the backend folds it away). A ternary
  (`name ? name : name`) also pins it but leaves a dead `cmplwi`; `name + 0`, a `u32` cast, `(0, name)` and a local copy
  are folded too early to help. Under MUST_MATCH, with the plain use in `#else`.

- **`int` vs `s32` locals (from tw2004, CC0):** they are different types to the front end. For an `int` local, `n += 3` is folded into every
  later use (`addi r3, rN, 3` at each call and compare); for an `s32`/`long` local it stays one in-place `addi rN, rN, 3` followed by plain
  uses. When a `+= const` is folded in your build and in place in the original (or the reverse), try the other type before restructuring.
  The same choice moves loop hoisting and unroll guards (an `s32` counter keeps the `li 0; cmpwi; bge` guard that `int` folds away).
- **Two-value choices:** `h = (n == 2) ? 6 : 7` compiles branch-free (`subi/nor/srawi`); `h = 7; if (n == 2) h = 6;` compiles to `cmpwi / li /
  bnelr / li`. A `li; bne; li` in the target is the second form.
- **Evaluation order:** on 2.6 the right operand of a comparison or an addition, and the last argument of a call, is evaluated first
  (`f(0) < f(1)` and `f(0) + f(1)` call `f(1)` first; `use(g(), h())` calls `h()` first). The call order in the target tells you how the
  expression was written; m2c's `t = f(1); if (f(0) < t)` is the source `f(0) < f(1)`.

Reported by bfbb, not re-measured here (all general MWCC; try them when stuck, measure before relying):
- `x *= k` can survive where `x = x * k` is folded; splitting a fused multiply-add into a named local (`t = a * b; t += c`) blocks `fmadds`
  and can free a register.
- `if (a == k) { X } else { Y }` vs `if (a != k) { Y } else { X }`, and an early-out `if (!A || !B) { ...; continue; }` vs `if (A && B) { ... }`,
  swap which block is physically first; the target's block order tells you which. Collapsing early returns into one `||` condition
  produces different exit structure.
- A load that cannot be hoisted above a call has a recoverable source position: if the target loads after a `bl` where you load before,
  move the expression below the call.
- Local stack slots go by size descending (ties by declaration order) at descending addresses; permute the declarations when `r1+offset`s
  are permuted.
- `char buf[N] = "..."` copies N bytes into the frame (unrolled up to ~64 bytes, an `mtctr`/`lwzu` loop above); `const char* p = "..."` is a
  pointer load. `volatile` is evidence of the original's source only if it takes the function to byte-exact 100%.
- (tw2004, not re-measured) A value computed before a call and kept in a saved register needs its own local (`s32 paid = money * 2;`, not
  `money *= 2;` which sinks the shift below the call). Passing the expression (`f(x + n)`) vs the variable just stored (`p = x + n; f(p)`)
  changes which register holds it. An initialiser in the declaration (`f32* p = &x;`) can give a different callee-saved number from
  declaring bare and assigning on the next line. `a[0] = a[1] = a[2] = 0` stores 2, 1, 0. `if (A) { if (B) x = 1; }` and `if (A && B) x = 1;`
  allocate different saved registers. A one-case `switch` with `default: return` gives `bne/beq next; b end`.
- Measuring: a whole-function percentage can fall when a hunk is fixed (the tail shifts), so judge the region you touched; a stale `.o`
  explains more false regressions than any tool difference, so re-run `ninja` to completion before believing a -1; objdiff pairs symbols by
  name and cannot see definition order, so 100% in objdiff is not byte-exact (the DOL checksums are).

## Data and pool: run `tools/pool_plan.py` first

If a unit's functions are (nearly) 100% but `.sdata2`/`.rodata` differ, or the unit is fully matched and won't link, run
`uv run --with pyelftools python3 tools/pool_plan.py UNIT` before touching the source, and follow what it prints literally.
It builds the unit once with all floats as literals (your source is restored afterwards), compares the compiler's pool with
the original's, and prints per slot whether the entry must be a **named object** (a prefix of the section in a deferred
unit, a suffix in a nodefer one, plus anything unreferenced) or a **literal** the compiler pools, which functions need a
constant read by name or reloaded, and the traps for that entry. Then:

1. `... pool_plan.py UNIT --apply` writes the float part of the plan (literals, the kept named objects defined after the
   code, `RELOAD_F32` at the reload sites) and rebuilds. Strings, `Vec` templates, `dat_*` words and the `.sbss2`/gap
   objects are listed under ACTIONS: do those by hand (recipes below and in the ledger).
2. `... pool_plan.py UNIT --check` rebuilds and reports the first mismatching slot with a diagnosis: a swapped pair, an extra
   entry (a literal where the target loads the named object, or a product/negation the compiler folded) or a missing one.
   Fix that slot only, rebuild, `--check` again. Don't guess ahead of the first mismatch: one fix usually moves every later
   slot.
3. Finish with `tools/link_diag.py UNIT` and the ci.py checksums. Re-score every function after each change: the plan
   keeps layout right, and a function that drops is listed by name with the constants it uses.

**Base-register units.** If the plan prints `BASE-REGISTER unit`, the original reaches its whole `.rodata` through one base register
(`#pragma pool_data on`): strings have no relocation of their own (`addi rX, rBase, off`) and the int->float double is `lfd f1, 0x..(rBase)`.
A named `str_xxxx` object lands before the pooled double, which puts the double at the wrong offset (the `lfd 0x10` vs `0xf8` diffs). Make the
strings plain literals at their uses, add `#pragma pool_data on`, delete any `double_to_int_*` definition and the MUST_MATCH `base + off` hacks
(`--apply` does these), then use `--check` to order the literals. Nine of the eleven class-d units were of this kind; ac_signal_timing's
actionCommandDisp went to 100% this way.

**Classify a stuck pool.** `uv run --with pyelftools python3 tools/sdata2_diff.py UNIT [--literal]` compares our `.sdata2`/`.rodata`
with the original's by value (names mean nothing: dtk names every stripped entry) and says IDENTICAL / PAD / ORDER / CONTENT; CONTENT
lists EXTRA, MISSING and duplicate values with the functions that emit them, tagging `neg` (an EXTRA value whose negation the target
holds: the original negates at runtime, we folded the literal) and `prod` (a MISSING value that is a product or quotient of two we
hold). `--literal` builds with every float written as a literal first, so what remains is the real gap.

**Spellings that fix a `--literal` gap** (each verified on eff_kemuri, which still isn't linked: with literals MWCC also hoists
constants into more FPRs than the original, which costs code score; pool order and hoisting are separate problems).
- `neg` (EXTRA `-x` where the target holds `x`): a literal negation folds. Keep the positive value in a local and negate that:
  `f32 p12p8 = 12.8f; n12p8 = -p12p8;` instead of `n12p8 = -12.8f;`. For `-1.0f * half` (folded to -0.5): `f32 one = 1.0f;` once, then
  `-one * half`; the product stays a runtime fmuls and 1.0 stays its own pooled entry.
- `prod` (MISSING `a*b` the target pools): the literal product folded away; the same local trick (`-p12p8` then `* 0.5f`) brought
  12.8 back as its own entry.
- GXColor / `u32` colour words in .sdata2 (`dat_xxxx` = 0xffffffff, 0x80808080 ...): one `static const GXColor c670 = { 0xFF, 0xFF,
  0xFF, 0xFF };` per original object, declared at the top of the function that uses it in the original's source order (the words land
  before the floats, in declaration order). Uses of the named extern become uses of the local; the header's extern goes.
- Statement order of the preheader assignments does not change pool order (tried all 12 orders); moving a literal's first textual use
  later only helps if the compiler does not hoist it.

**Hoisting: literals vs named constants (measured on minimal repros, MWCC 2.6, deferred flags).** A float *literal* used directly is
loaded lazily at each use (and reloaded in loops); a named `extern` constant is loaded once into a callee-saved FPR early and kept.
That is the code-score gap behind most "must be named" units, and it is not a pool problem. Spelling effects:

| spelling | effect |
|---|---|
| `x = 1.0f; ... use(x)` (literal, local) | lazy load after the calls; not hoisted |
| `extern const f32 k; x = k;` | early load into a callee-saved FPR (named symbol, wrong pool) |
| `extern __declspec(section ".sdata2") f32 k` | same as the const extern |
| file-scope `static const f32 k = 1.0f;` defined before use | folds to a literal: same as the literal row |
| `const f32` local | folds like the literal row |
| literal-initialised locals + `#pragma opt_propagation off` around the function | **early load into callee-saved FPRs, value pooled as an anonymous literal**: the named behaviour with the right pool |
| table `static const f32 t[]` / volatile read | early load, but through a table symbol (extra object) |
| `opt_lifetimes`, `opt_loop_invariants`, `opt_common_subs`, `opt_dead_assignments`, `opt_strength_reduction`, `scheduling` off | no effect on the lazy loads |

Loop-invariant hoisting is the other lever: a value computed in a loop body from loop-invariant inputs (`dsize_half = dsize * 0.5f`,
a kept `size = 16.0f`) is hoisted all the way to the function preheader, while the original often computes it where it is written
(eff_kemutest's `effKemuTestDisp`: 98.1% -> 100% with `#pragma opt_loop_invariants off` on that function). Pool order follows
instruction emission, and hoisted instructions pool after the loop body's own literals, so the pragma also changes which literal
pools first; check `sdata2_diff.py` after adding it.

Use `opt_propagation off` on the one function (`#pragma opt_propagation off` before it, `reset` after) and give each constant the
original keeps in an FPR its own initialised local, in the original's order. eff_kemuri's `effKemuri2Entry` went 79.8% -> 99.93% this
way with all constants literal. It is not free: in functions whose loops already have the right hoists (`effKemuriDisp`) the pragma
cost 3-5 points; there, name only the constants the original keeps (`f32 one = 1.0f;` and pass `one` where the original stores the kept
register) and leave the pragma off. A negated product (`-one * half`) then stays a runtime multiply.

**mwcc-debugger: see why a register differs** (`tools/mwccdbg.py`, `tools/rasim.py`; adapted from tw2004, CC0). It runs MWCC 2.6 (our compiler)
under retrowin32's x86 emulator and dumps the PCode between backend passes and the register allocator's priority list: each variable, its
register, its neighbour count, and the register the original has where ours differs (`!ORIG`). About 7 s per function:

    python3 tools/mwccdbg.py effect/n64/eff_timestamp_n64 effTimestampDisp          # dumps in build/mwccdbg/FUNC, summary.txt
    python3 tools/rasim.py build/mwccdbg/FUNC replay [--fpr]                         # rebuilds the colouring; must report 0 differ
    python3 tools/rasim.py build/mwccdbg/FUNC search 39 48 --summary                 # best declaration order of vregs 39..48

`replay` reproduces the compiler's colours exactly (0 differences on effTimestampDisp, GPR and FPR); `search` permutes the named locals'
vregs (declaration order, first declared = highest number; `@N` CSE temps sit above them) for the original's registers, thousands of
orders a second with no compile. If even a wider range finds nothing (effTimestampDisp: still 3 of 47 wrong), the original has different
live ranges, not a different declaration order: change what creates the temps (one value more or fewer in a register, a reused local).
Variables with more than 28 neighbours jump to the top of the priority list and take r31/f31. **Agents: never install, clone or build
any of this yourself** (it is already set up and mounted read-only for you; if `tools/mwccdbg.py` fails, say so in your report and
carry on with the brief's levers). Setup, done once per machine by a person, no sudo (Linux or macOS; needs gdb, cmake, Rust; the
clones live in `$MWCCDBG_HOME`, default `~/mwccdbg`):

    micromamba create -p ~/mwccdbg/env -c conda-forge gdb cmake git          # gdb on PATH (or MWCCDBG_PATH)
    git clone https://github.com/cadmic/mwcc-debugger ~/mwccdbg/mwcc-debugger  # no licence: run it, never copy it into the repo
    git clone https://github.com/encounter/retrowin32 ~/mwccdbg/retrowin32 && cd ~/mwccdbg/retrowin32 && git checkout gdb-stub
    cargo build -p retrowin32 -F x86-unicorn --profile lto                    # about 3 minutes; use nice on a shared machine

**Using it (agents).** One function at a time, from the workspace root:

1. `python3 tools/mwccdbg.py UNIT FUNC` (REL units: `MODULE/path`). It compiles the unit as it is now; everything is written under
   `build/mwccdbg/FUNC/`, never outside the workspace. Each of your commands runs in its own network namespace (no network, its own
   loopback), so the 127.0.0.1:9099 lock no longer serialises runs across agents, and needn't: each namespace also has its own gdb
   port 9001, so two runs never meet. Don't "fix" the lock.
2. Read `build/mwccdbg/FUNC/summary.txt`: per allocation pass (only the LAST pass of a class is final), each variable in priority
   order with its register, neighbour count, and `!ORIG rN` where the original has a different register. Named locals show their
   names; `@NNN` are compiler temps; fix the first variable that differs. 0 `!ORIG` lines means the gap is not register colouring.
3. `python3 tools/rasim.py build/mwccdbg/FUNC replay [--fpr]` must say 0 differences. Then `rasim.py ... search LO HI --summary`
   (LO..HI = the vreg numbers of the named locals, first declared = highest) prints the declaration order that gives the most of the
   original's registers: best 0 wrong means writing the declarations in that order is enough. Confirm with a real build.
4. If search finds nothing, change what creates or ranks the variables (each changes the numbering, then re-dump):
   declaration order; a temp in or out of a named local (`t = a * b; f(t)` vs `f(a * b)`); assignment-in-argument
   (`f(x = g())`); moving a block into a `static inline` helper (its locals become `@` objects ranked above named locals);
   `int` vs `s32` for a local; initialiser in the declaration vs a separate assignment.

**How the allocator ranks variables (what a lever has to change).** Variables go on a priority list in "levels": sweep all variables in ascending
vreg order; one whose remaining neighbours (physical registers always count) are at most 28 is pushed and stops counting as a neighbour; repeat
from the first vreg. The list is coloured from the LAST pushed, each taking the lowest free volatile register, else the next callee-saved from r31
down. So (1) within a level the higher vreg wins; (2) a variable with more than 28 neighbours waits for a later level and wins over everything
in earlier ones; (3) vregs are numbered: parameters (lowest, in order), named locals in reverse declaration order (first declared highest), `@`
CSE/inline temps, then the backend's objectless temps in initial-code order. A parameter can therefore never outrank a named local by
declaration order; copies of it (`p = p0`, `m = m + 1`, `register`, an address-taken local, an inline helper's local) were checked on minimal
repros and all coalesce into the parameter or rank above it. What does move a variable: becoming a different web (a second definition), or
changing who it overlaps so it drops to an earlier or later level. `python3 tools/rasim.py DUMP neighbors --summary` (or `name=reg ...`) finds the
single interference edge whose removal or addition gives the original's registers (it reported one for evt_mobj_rideswitch_lightblue, puniModeParentpre
and puniswSetindex; none for the param-vs-local cases battleSortPhaseMoveTable, btlseqEnd_DispMain, screen_tev). Two webs with equal degree are ordered
by vreg, i.e. by creation order: evt_mobj_*switch* differ only because our string-address temp (`lis`) is created before the `gp` load temp (the `asyncName`
assignment comes first in the initial code even when written inside the call as `f(asyncName = str, !!gp->inBattle, NULL)`), where retail created it after. Open: a spelling that makes the `gp->inBattle` load come first without a named local (a named local shifted r27-r31 by 30+ instructions).

**Scheduling-only differences** (same instructions, same registers, a different order inside a block): `tools/sched750.py`
(tw2004, CC0; extended here to floating point) replays MWCC 2.6's list scheduler on a dump. `python3 tools/sched750.py predict build/mwccdbg/FUNC B23`
prints the model's first-pass and last-pass order for block B23 next to the compiler's real one (`validate DIR...` reproduces 95.6% of 4129
blocks across our dumps, 92.2% of those with FP ops), and `deadsearch DIR BLOCK target.txt` finds a dead value or extra copy that gives the
original's order (confirm with a build). Calls (`bl`) and record forms are not modelled: a block containing one is skipped. Known misses: an
op the real scheduler issues late where the model issues it in an idle cycle, and `lis` pairs; check that `predict` reproduces the compiler's
current order before trusting a `deadsearch` lead.

The tool needs the repo's build.ninja and `build/G8MJ01/obj` (the original objects); a function's dump is for the source as it is in the
tree (`--src copy.c` compiles a scratch copy with the unit's flags).

**Levers Codex proved** (full attempt logs, including what failed, in `docs/codex/`; the ledger has the table "Codex's wins, and why each worked"):
- Place such helpers before EVERY function in the file (or at the very end for the tail-data flush). A helper in the middle generates immediately and so flushes the functions queued before it first, which reverses the text order of those functions (mot_roll: clrRollEvtFlag and its neighbours moved to the front, 32 bytes shifted the whole DOL). Check the symbol order after adding one.
- Helper functions position pool entries and force tail data in a deferred unit: an unused helper placed between two functions pools its constant at that point; an immediately generated `u32 -> f32` helper at EOF flushes deferred code ahead of `force_active` tail words so they land last ([fadedrv](codex/fadedrv.md), [koopa](codex/koopa.md)).
- Block-scoped assignment inside the inner loop, just before the use, limits how far MWCC hoists (to the inner-loop preheader only), which also fixes pool order ([eff_kemutest](codex/eff_kemutest.md)).
- `DUP_OURS` in `sdata2_diff.py` means we pool a value twice that the original merges: remove the duplicate constants and keep runtime products and registers through locals ([eff_fire](codex/eff_fire.md)).
- `.sbss`/`.sbss2` scratch declarations are often in reversed order, and a trailing gap word may be needed ([windowdrv](codex/windowdrv.md), eff_fire).
- Assignment-in-argument (`__memAlloc(HEAP_DEFAULT, rounded = OSRoundUp32B(size))`) and moving a block scan into a `static inline` helper change web ranks where declaration order cannot: inline-expansion objects are numbered differently from the function's own locals ([fontmgr](codex/fontmgr.md)). Checked with `mwccdbg.py`: the helper's `ptr` and `i` become `@` objects numbered above `dvd`, `rounded` and the other named locals, so they colour later and take the low volatile registers (r6, r7) while the loaded header temp gets r8; as plain locals they were named webs ranked by declaration. To lower or raise a variable's rank, move it into (or out of) an inlined helper or an argument expression.
- A wrong value can hide behind a "fix" (battle_unit's sentinel): checksum it ([battle_unit](codex/battle_unit.md)).

**Conversion double before strings.** If the target's `.rodata` has an int->float double (`double_to_int_*`) ahead of a string that a
deferred function pools, our build pools the double later, since a deferred unit emits functions in reverse source order and each
function pools its strings before its own conversion double. Under `#ifdef MUST_MATCH` add an ordinary helper that is generated
immediately, wrapped in `#pragma defer_codegen off` ... `#pragma defer_codegen reset`:
`static f32 pool_i2f(s32 v) { return (f32)v; }` (`u32` for the `_mask` double; a second helper returning a string literal pools
that string at its slot too). Nothing calls it and the linker strips the body. Reference: `kpaFireBreath` in `src/koopa.c`,
`yoshi_pool_i2f` in `unit_party_yoshi.c`. `pool_plan.py` prints an `I2F HELPER` note when it sees the pattern.

The plan's rules, so you can read its output: pooled entries land in the order the compiler first emits them (text order,
expression-tree order inside a function); named objects land before the pool in a deferred unit and after it in a nodefer
unit; a `const` object defined before its use is folded away, so define named objects after the code; the first entry the
all-literal build gets wrong is where the named prefix has to end. It cannot see why a function's *code* needs a constant
by name beyond the per-constant builds, and it does not plan strings or templates that data initialisers pool at parse
time (it lists their owners in the order the source must define them).

## Code shapes that fixed near-misses

- Linking a fully matched unit that still says `NonMatching`: run `python3 tools/autolink.py UNIT.c` first (REL units that are byte-identical just link). If the checksum fails, `python3 tools/rel_cmp.py MOD UNIT` (REL) or a `doldiff` of the dol names the section: a `.bss` offset shift means the uninitialised globals are declared in the wrong order (MWCC lays them out last-declared-first; declare them in the reverse of the original's address order, and a global the original has in `.bss` must be uninitialised); a short data section needs its trailing `gap_*` object when the next unit's addresses shift (`#pragma explicit_zero_data on` + `force_active on` + `__declspec(section ".sdata") static u32 gap[N] = {0};`); data tables nothing references by name need `#pragma force_active on` around them or the linker drops them (the dol comes out 512 bytes short).
- **Exception to the trailing `gap_*` rule: a `.data` that ends in a switch jump table.** MWCC emits a function's jump table after all source-level data, so a `gap_*` object you define lands *before* the table (mot_jump: at `.data+0`), the symbols no longer line up and objdiff scores the section 0% although every byte matches. Leave that `.data` gap out; the table is the section's last object.
- **Defined `jumptable_*` copies block the link.** Some files define `static u32 jumptable_XXXXXXXX[] = {...}` under
  MUST_MATCH, a copy of a switch table the compiler already emits on its own, only so objdiff can pair the original's
  named table. Nothing references them, and they make `.data` too big by exactly their size (mri_puni: eight copies,
  572 bytes; without them `.data` is byte-identical). For the link attempt, delete them; the unit's `.data` score
  then drops (the compiler's tables are anonymous), which is expected: if the checksum passes, say so in REPORT.md
  and the merge excuses the unit with `tools/ci.py --allow-regress UNIT`. Keep `jumptable_*` objects that are real
  data tables dtk happened to name that way (evt_bero's and win_party's string-pointer tables are referenced).
- A unit whose functions are all at 100% but whose source lists them in the original's text order (Main first) has its pool in the wrong order: the original's `.text` order is the reverse of the source order. Compare with `.fn` order in the unit's asm; reorder the functions (forward-declare statics, put prototypes in the header).
- The all-literal pool (`/tmp`-style litpool: replace every named `float_*` extern by its literal and drop the definitions) matches `.sdata2` once the function order is right; keep named only the objects the original names in a script/table (colours, `zero_*`). Where a compare writes `x != 0.0f` the literal form puts the constant first (`fcmpu f0, f4`) and the named form keeps the variable first: `eff_blow_n64` stays at 99.6% for that reason.
- **Linking a unit whose functions are all 100% (what worked on mot_damage, mot_dokan, eff_blow_n64, eff_hit_n64, eff_mobj_broken):** flip it to `Matching`, run `uv run --with pyelftools python3 tools/fn_order.py UNIT --write` (source order is the reverse of `.text`; forward-declare what the reordered code needs), build, then `uv run --with pyelftools python3 tools/link_diag.py UNIT` for the word-level data diff and the first differing dol address. Then:
  - An undefined symbol at link time is the unit's own data not ported: define it in C (`.sdata2` floats as `static const f32 float_X = ...;` *before* the code, `.data`/`.sdata` tables as plain arrays, `.sbss2` objects with the `#pragma section ".sdata" ".sbss2"` trick of eff_mist2).
  - `static const f32` defined before use is folded by MWCC into its literal pool, in first-use order, and the unreferenced object is dropped; an `extern` declared and defined after the code is a real object that is emitted *before* the pool, and any literal of the same value used elsewhere then pools a second word. A built `.sdata2` longer than the original's is that duplicate; link_diag shows it as a tail of extra words.
  - A string used once is a literal in the code, not a named `const char` object (an object lands first in the section, a literal lands with the pool). A 6-byte-or-smaller string goes to `.sdata2`, longer to `.rodata`.
  - Do not define the compiler's own pool entries yourself (the i2f double `double_to_int_*`, literal `Vec` initialisers `(Vec){0,0,0}`): the explicit object and the pooled copy both land in the section. Leave the symbol undefined; the linker resolves the original name from the section it belongs to.
  - If the code needs the constant *read from the object* (no folding of `-48.0f * 0.5f`, no hoist into a callee-saved FPR, a reload per use), use a `MUST_MATCH` volatile read of the named object (`#define ONE (*(volatile const f32*)&float_1_x)`, see `MATCH_F32` in `matching.h`); a temp (`f32 one = ONE;`) when the target loads once and moves with `fmr`. A local (`f32 sixteen = 16.0f;`) stops constant folding without touching the pool.
  - A `.rodata` Vec the code copies by value is defined `const volatile` under `MUST_MATCH`, otherwise the optimizer folds the known contents (eff_hit_n64, eff_magic3_n64).
  - What remains after that is usually one function's register allocation (eff_magic3_n64, eff_kumokumo_n64, eff_bomb): queue it for the permuter.
- `tools/data_check.py` (run by ci.py as `[data]`) reports a data object whose bytes differ from the original object's: a table written `{4,0,0}` for `{0,4,0}` scores the same in objdiff. `--allow-data-mismatch NAME` excuses an object.

- Pool and string order in a unit follows the function order in the source, not just each function's text. Run `uv run --with pyelftools python3 tools/fn_order.py UNIT`; if it prints a different `want:` order, `--write` it and rebuild. `battle_item_data` linked once its functions were in the order that emits the target's `.text` order (its `.sdata2` and `.rodata` string order then matched with no other change). Keep the reorder only if no function's score drops.
- A `volatile` comma hack around `intplGetValue` arguments (`((volatile f32)x, wp->counter)`) is a leftover from before the prototype fix: with the declared parameter order the plain call loads the float arguments first. `countdown` and `battle_item_data` lost the hack and still match.
- **Pool recipes that linked eff_sheep, eff_mist2, eff_fireworks_n64, battle_mario:** (1) Try the all-literal form first: every float, string, `(Vec){0,0,0}` copy and the i2f double left to the compiler (eff_sheep, eff_fireworks_n64), then fix only the one or two constants whose load the text needs by name. (2) Named f32 objects (defined after the code) come before the code's pooled constants but *after* the parse-time pool entries (string literals used in data initialisers such as EVT scripts), so a single named constant defined at the very end of the file is how battle_mario's `1.0` lands ahead of `0.6`/`0.0`. (3) A duplicate zero word means some sites load a literal 0.0f while others load the named `float_0`: name only the words *before* the original's zero and leave zero and everything after it literal (eff_mist2: named prefix of 5, zero literal). `tools/pool_prefix.py` sweeps exactly this, but only helps once `math.h` is `math_ext.h` and the rodata doubles/strings are literal. (4) After a `#pragma section ".sdata" ".sbss2"` object, put `#pragma section ".sdata" ".sbss"` back, or the file's later zero-initialised local statics land in `.sbss2` and shift everything after them (eff_fireworks_n64). (5) `link_diag.py` prints `dol: 0 bytes differ` when the unit is not flipped to Matching; the checksums in `ci.py` are the verdict.
- **Where pool entries come from (unit_party_vivian, eff_sheep):** string literals in data initialisers (EVT scripts, tables) and the `(Vec){0,0,0}` copy templates in function bodies are pooled at *parse* time, in source order, so a template lands between two data blocks when its function sits between them; function-body strings and the i2f doubles are pooled when the function is generated, in `.text` order, after all of those. Consequences: (1) put the `tools/dol_gen.py` data region *before* the functions, then slide the function that owns a template (vivian: `battle_evt_majo_disp_on`) one data object at a time and watch `link_diag.py`'s first differing `.rodata` offset until it jumps past the template; keep the functions in their reverse-`.text` relative order. (2) Once a unit's data is fully ported the `base + 0x3E8` string-pool hacks and `(volatile f32)` comma hacks that were written against a half-ported unit are unnecessary: use plain string literals and plain calls and re-score. (3) `#pragma force_active on` around the whole ported data region, `explicit_zero_data` + `force_active` for the zero `gap_*` words at section ends, and `__declspec(section ".data")` for a 4-byte data gap (it would land in `.sdata` otherwise). A linked dol that is short by exactly the size of some unreferenced tables is the linker stripping them.
- **Float pool order, tested with the project's compiler on small files (cam_road, eff_sleep, fadedrv, mario_hit):** (1) a float literal is pooled at its first appearance in the expression tree, left to right, so `3.14f * (a * 0.5f / 180.0f)` pools 3.14, 0.5, 180 while the same maths in two statements pools 0.5, 180, 3.14; to move a constant earlier, move its operand left, not the statement. (2) `cos(0.0f)` and a `static const f32` argument fold to an `lfd` of a *double* (an extra 8 bytes in `.rodata`); a plain local `f32 zero = 0.0f; cos(zero)` emits the `lfs` of the pooled float the original has. Such a local does not stop folding inside float arithmetic (`two / (one + x)` still folds). (3) A dead float literal (unused local, `if (0)`, a compare whose branch skips nothing) is gone before pooling; a dead `double` compare whose branch jumps to the next statement still pools its double (collisionCurve's `1.0000001`), and a function-local `static const char s[16] = "..."` used by a dead assignment lands in the pool at that function's slot (cam_road's two debug strings). (4) In a `-inline auto,deferred` unit an `extern` float defined after the code lands *before* the pooled literals; in a non-deferred unit (`cflags_static_nodefer`, source order = text order) it lands after them, so a named prefix cannot be built there: use all-literal and fix the few non-foldable sites. (5) `-O4,p` negates a literal at compile time (`-0.8333f` pools the negative); the original's `lfs` + `fneg` of a positive pooled word needs the value to come from a non-constant, which is why bomhei's two negated sites stay named.
- **Read tables from the original object, don't guess them:** `uv run --with pyelftools python3 tools/symdump.py UNIT [SYMBOL] [--as u32|f32|raw|str]` lists a unit's data symbols largest first and dumps one with relocated words shown as the symbol they point to (guessed data cost gauge_wakka, a `{1,1,1}` vec, `defaultWeapon_Dummy_NoItem` twice and an SFX string); `tools/rodatalayout.py UNIT [SECTION]` prints the original's and our `.rodata`/`.sdata2` side by side with symbol names and `|` on mismatched rows (eff_break: the target's last `.sdata2` word is a `gap_*` object, ours is the pooled `@221` the code loads).
- A 4-byte `GXColor` the target keeps in `.sdata2` between strings: define it after the data that holds the strings (`__declspec(section ".sdata2") const GXColor name = {...};`, at the end of the file). Defined at the top it lands before every string.
- A `.sdata` object the original 32-aligns (GX vertex arrays: `Pos`, `Nrm`, `Col` in `himo`) is `ATTRIBUTE_ALIGN(32)`; the padding the disassembly shows as `gap_*` objects then comes for free.

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
  A temp for the call result, or inlining it back into the expression, therefore changes the pool order without
  changing any instruction.
- **A product of two literals folds** (`-16.0f * 0.5f` becomes `-8.0f`), so the target's two separate constants
  vanish: `f32 sixteen = 16.0f; f32 half = 0.5f; f = -sixteen * half;` keeps both.
- **A constant assigned to a local before the variable it multiplies fixes the FP load order and the registers** without
  touching the pool: `f32 k = 0.03125f; ... x = x * k + y;` (eff_nokotarou), `f32 half = 0.5f;` (eff_kemuri9_n64),
  `f32 zero = 0.0f; if (f30 == zero)` (eff_lensflare_n64). The literal is still pooled in its place; the local makes
  the compiler hoist the load the way the target does. Try it when the pool is already right and only the FP loads
  or one callee-saved register differ.
- **Arguments are evaluated in the order of the declared parameters, not of the registers.** A callee that takes both
  integer and float arguments loads them from the call's parameter order, so `f32 f(s32 a, s32 b, f32 x)` loads `a`, `b`,
  then `x`, while `f32 f(s32 a, f32 x, s32 b)` loads `a`, `x`, `b`: the registers (`r3`, `r4`, `f1`) are the same either
  way, so the assembly never shows the declared order, only the order of the loads before the call. When a near-miss
  differs only in where the loads before one callee sit, move the float parameters in the declaration: `intplGetValue`
  is `(mode, start, end, currStep, steps)` (not `(mode, currStep, steps, start, end)`), which fixed 18 functions in one
  header change; callers keep writing the old order through a macro in `include/system.h`. Look for the same callee
  across several near-misses before fixing the call sites one by one.
- **A cast that only exists to avoid an `extsb` or a signed/unsigned compare means the header type is wrong.** A call
  followed by `extsb r3, r3` that the target does not have means the function does not return `s8` (and the field it
  returns is probably `u8`); `cmplwi r3, 0` where the target has `cmpwi r3, 0` means it does not return `u32`.
  `(s32)BattlePadCheckRepeat(...)` and `((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(...)` were such
  casts; fix the declaration in the header (and the definition) once instead of casting at every call.
- **A call result passed on as an argument: nested or through a local decides one `mr`.** `f(id, g(x))` compiles to
  `bl g; mr r4, r3; mr r3, rID; bl f` (the result goes straight to its argument register). `s32 v = g(x); f(id, v);`
  compiles to `bl g; mr r0, r3; mr r3, rID; mr r4, r0; bl f` (the local is parked in `r0` while the first argument
  is set up). Read which of the two the target has and write that; it changes three instructions and nothing else
  (`btlevtcmd_SetPartsBaseScale`: 97.98% to 100% by moving `evtGetValue(event, args[1])` into a local; the mirror image
  fixes the cases where ours has the `r0` route and the target does not).
- Inlined float polynomials (sin/cos approximations used 10+ times in one function): write the coefficients as non-const locals at the top of the inline (`f32 c1 = 0.16605f, c2 = 1.0f;`). MWCC then hoists the constant loads once per instance like the original; `const` locals, or literals in the branches, rematerialise them per branch (hit_check 68 -> 89%).
- A function that inlines many callees: raise `#pragma inline_max_total_size(60000)` and `inline_max_size(1000)` around it (reset after), and put `#pragma dont_inline on` / `reset` around the definition of any callee the original still calls.
- A decision chain on a small integer result where the original shows `cmpwi 1; bne; cmpwi 2; beq; cmpwi 4; bne` is an `if/else if (a || b)` chain, not a `switch`; a `switch` makes a different tree.
- Do not wrap a dense `switch` in `if (x <= N)`: the switch already emits that range check (a second `bgt` appears).

- **Check the shape before the registers: `python3 tools/diff_shape.py UNIT FUNC`.** It prints whether the `bl` sequence is the same and which immediates the original has that yours lacks. A function whose calls and immediates all match differs only in scheduling and registers (permuter work); one that doesn't has a structural difference. This is how a wrong reconstruction shows up: a `__strtold` written from the generic MSL source scored 75% with one call fewer; the m2c draft had a different hex path (nibble index from 2, limit 17, no leading-exponent-zero case) and the rewrite from it reached 96%.
- **The original's tail is duplicated per branch when the target has two copies of the same calls.** `__dec2num` has `__minus_dec, __minus_dec, __equals_dec` twice, once after the guessed-low loop and once after the guessed-high loop, each loop with its first iteration peeled (`n2d(); while (cond) { ...; n2d(); }`) and their own `decimal` locals on the stack. Written once after an if/else the compiler shares the tail (calls 13 vs 18); write it in both branches. The same shows up as `if (c) ioff = A; else ioff = A;` feeding one `PSMTXTrans` (actionCommandDisp), which keeps both computations.
- **A `bl tolower`/`bl isdigit` where the target has none, and a smaller frame in the original, means an inlined table lookup.** `tolower` in the SDK `arc.c` is `c == -1 ? -1 : __lower_map[(u8)c]` inline (`ARCConvertPathToEntrynum` 72.65% to 100%). Declare `extern const unsigned char __lower_map[];` and write the one-line inline.
- **A function that reads many rodata strings/vectors as `addi rX, rBase, 0x..` from one base register** (the base is the unit's first rodata object, `vec3_802fe418`) gets there with the MUST_MATCH macro `#define FB(off, sym) ((const char*)&vec3_xxx + (off))` and `FV(off, sym)` for Vec reads; the `#else` form uses the named symbol (eff_fall: `effFallMain` 85.4 to 86.6, win_log `LV`/`RO`). The i2f magic double the compiler pools itself stays anonymous.
- **`clrlwi r,r,16` where yours has `extsh` on an argument means the original converts as u16.** Don't change the shared prototype if other linked units need `extsh`; cast the callee at the call site (`((void (*)(Vec*, u16, u16, f32))iconDispGx)(...)`, evt_win).
- **`extrwi. r0,r0,1,29` is `(x >> 2) & 1`, not `x & 4`** (`rlwinm. 0,29,29`); a bit test written the other way is the whole difference.
- **A float literal divided in several statements (`/ 1000000.0f`) is reloaded per use; the same constant as a named `extern const f32` is hoisted into a callee-saved FPR.** Where the target reloads, use the literal (itemseq_Bound `itemTicksToSec`).
- **A `static inline` double constant read through the unit's base register** (`*(const f64*)((const u8*)&vec3_xxx + 0x48)`) avoids the compiler emitting a second anonymous copy of a double the unit already defines, which otherwise shifts every later `.rodata` offset and fails the ratchet.

- **`divw` by a constant in the target where yours has `lis 0x9249; mulhw`:** the original was built with the size optimiser for that function. `#pragma optimize_for_size on` / `reset` around it keeps the divide (and often fixes loop shapes that came out as `bdnz`): `ac_repeatedly_lv` `actionCommandDisp` 90.30 to 99.88.
- **s16 `psq_l`/`psq_st` with GQR5 in the target is the SDK's `OSs16tof32`/`OSf32tos16` after `OSInitFastCast()`,** never a cast (`libs/dolsdk2004/include/dolphin/os/OSFastCast.h`; `effFlameDisp` 55 to 96). The header's `__OSf32tos16` temporary is an `f32` so the inlined temps sit 4 bytes apart; the old `f32 v[4]` Vec padding is not needed. A target that writes GQR5 from a computed value (hitdrv) is inline asm and out of scope.
- **A target compare chain `cmpwi 1; beq; bge; cmpwi 0; bge` on a value you computed with `& 1` is a `switch`,** not an if/else chain on the masked value (gor_03 slot).

- **A bool materialised with `mfcr; extrwi; xori; cntlzw; srwi.` around a float compare is `!x`,** not `x == 0.0f`
  (which branches directly): `(!dt ? eps : dt)` matched effCalcMayaAnimMatrix's zero-interval guard.
- **`Mtx` locals sit on the stack in reverse declaration order** (the last declared at the lowest offset): read the
  target's `addi r3, r1, off` per call and declare the matrices from the highest offset down (effdrv).
- **mwccdbg's summary.txt can name the wrong original register.** Its ORIG column maps through the final object and
  once reported "0 wrong" while three GPRs differed. Read the real colouring in `backend-19-after-regalloc.txt` and
  give rasim the targets explicitly (`rasim.py DIR search LO HI r37=r9 r53=r10 ...`).
- **A variable that must outrank the function's frontend temps goes in a `static inline` helper.** Inline-expansion
  objects get vreg numbers above the frontend temps, which no declaration order can reach: effCalcMayaAnimMatrix's
  `frame` in f0 and its GQR value in r9 matched only once the channel loop and the GQR write were inline helpers.
- **Rodata `vec3_*`/`jumptable_*`/`dat_*` objects holding zeros where the code stores a computed value are the
  compiler's templates for local initialisers.** Write the local, not the object: `(Vec){x, y - 4.0f, 0.0f}` (non-constant
  compound literals compile in C mode; `Vec v = {x, ...}` does not), `const char* names[2] = {"a", "b"};`,
  `GXColor cols[2] = {...};` inside the loop that re-copies them. Templates pool at parse time in source order, so
  the unit's .rodata then falls into place (win_party: .rodata and .sdata2 byte-identical once the named copies went).
- **The window draw functions take `Vec`/`GXColor` by value:** callers copy each argument into a fresh stack slot
  (`winFontSet`, `winFontSetR/Edge/Label`, `winIconSet`, `winTexSet`). win_main.h declares pointers, which compiles to
  the same callee, so call through a by-value cast at the call site (win_party's `winFontSetV`) instead of hand copies.
- **Several stack slots initialised from one .sdata2 colour word are locals copied from one file-scope
  `static const GXColor`,** not separate literal initialisers (those pool one template each) and not copies of one
  local (copy propagation merges those into one slot).

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
