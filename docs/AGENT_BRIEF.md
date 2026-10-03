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
   real logic to the next reader.
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

16. **Reusing code without saying where it came from.** Other open-source decompilations may already have what
   you need, and using them is fine if the project's license is CC0 or MIT (check its LICENSE file first; a
   project with no license is off-limits). Name every project and file you took code from in your report, keep
   anything you download or clone inside ./tmp in this directory (never /tmp or elsewhere), and don't search the
   rest of the machine. The merge lists everything you fetched.
   The MIT/CC0 reference decomps are already on disk, read-only, at ~/ttyd-refs/<project> (ac-decomp, spm-decomp, tp,
   tww, musyx; ~/ttyd-refs/README.md has the commits and licenses): read them there, do not clone them again.

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

- Linking a fully matched unit that still says `NonMatching`: run `python3 tools/autolink.py UNIT.c` first (REL units that are byte-identical just link). If the checksum fails, `python3 tools/rel_cmp.py MOD UNIT` (REL) or a `doldiff` of the dol names the section: a `.bss` offset shift means the uninitialised globals are declared in the wrong order (MWCC lays them out last-declared-first; declare them in the reverse of the original's address order, and a global the original has in `.bss` must be uninitialised); a short data section needs its trailing `gap_*` object when the next unit's addresses shift (`#pragma explicit_zero_data on` + `force_active on` + `__declspec(section ".sdata") static u32 gap[N] = {0};`); data tables nothing references by name need `#pragma force_active on` around them or the linker drops them (the dol comes out 512 bytes short).
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
