## Standing rules

The rules still in force, each linked to the chapters that decided it; where a later chapter changed a rule, this is the
current version. Update it here when a new chapter changes one. The working detail is in `docs/AGENT_BRIEF.md`
(matching) and `docs/CLEANUP_GUIDE.md` (cleanup).

**Build and parity**
- The linked build stays byte-identical: `ninja` recreates `build/G8MJ01/ok`. A model's claim of a match is never
  evidence; only the checker is. ([Snood](@{Carried over from Snood}), [local CI](@{Local CI:}))
- Every changed C file also compiles without `-DMUST_MATCH` (`configure.py --non-matching`); `ci.py` runs this as its
  natural step. ([Melee's workflow](@{Melee's workflow adopted}), [mmsrc](@{MUST_MATCH source shapes outside battle}))
- Mac and Linux builds agree: file names keep the case `configure.py` and `splits.txt` use (`HARDWARE.C`). Quote the
  tower's numbers; a Mac's `report.json` or `build/ci/accepted.json` can be stale, so never `--accept` from it.
  ([build parity](@{Build parity:}), [current state](@{Current state}))

**CI and ratchets**
- `python3 tools/ci.py` before every commit; commit only on PASS. The ratchet fails if any function, unit or total (code
  and data, matched and linked) gets worse; `--allow-regress NAME` only for a deliberate fix or removal of wrong code,
  said in the commit. ([local CI](@{Local CI:}), [orchestrator's role](@{The orchestrator's role}))
- Lint: no new inline asm, no new `#pragma` (`--allow-pragma`), no edits under `config/ orig/ tools/ .github/ libs/
  assets/` (`--allow-protected`), `configure.py` edits only to `Object(...)` lines (`--allow-configure`). Unguarded
  function-pointer casts (`tools/fncast_lint.py`, AGENT_BRIEF rule 7) and known compiler-hang triggers
  (`tools/hang_guard.py`) fail too, whatever the flags. ([local CI](@{Local CI:}), [Melee's workflow](@{Melee's workflow adopted}),
  [incident](@{Incident: hung compiles}))
- Data ratchet: objdiff scores a data relocation by its symbol, so `tools/data_check.py` compares the bytes of every initialised
  data object a diff defines or changes with the original's (`--allow-data-mismatch NAME` excuses one); a data section's
  score may not drop either. ([Grind 87](@{Grind 87:}), [eff_kemuri](@{Readability: eff_kemuri MUST_MATCH blocks (2026-10-20, Claude Haiku 5.5 + Claude Opus}))
- No function prototypes in .c files (statics and `tools/local_prototype_exceptions.json` excepted); with
  `-requireprotos` a call with no prototype is a build error. ([local prototypes](@{Local prototypes moved to headers}), [Melee's workflow](@{Melee's workflow adopted}))
- The cleanup index is a soft ratchet: `ci.py` prints it with its delta and warns, never fails, when it rises.
  ([cleanup index](@{Cleanup index: a readability metric}), [review backlog](@{Review backlog, Oct 10-17}))

**Pragmas**
- Whole-file settings go in `configure.py` as per-file `extra_cflags`, never as a pragma at the top of a file. A scoped
  pragma stays only if the object needs it, with a one-line reason (`// needed: without it FN X%`); cleanup never adds
  one. Never `#pragma scheduling <cpu number>`: this mwcceppc loops forever on it. ([pragma policy](@{Pragma policy (2026-09-25}), [hack review](@{Matching-hack review}), [pragma removal](@{Optimisation pragmas: removed}), [incident](@{Incident: hung compiles}))

**MUST_MATCH and structural fakes**
- Code that exists only to reproduce the original bytes goes under `#ifdef MUST_MATCH`, the natural form under `#else`.
  ([code smells](@{Code smells to track}), [Melee's workflow](@{Melee's workflow adopted}))
- MUST_MATCH is for small local tricks. Refused even when guarded: functions or inlines that only steer registers (above
  all reshaped copies of existing helpers), calls through casts that reorder or retype a prototype, inlining, recursion
  or loops expanded by hand, and `defer_codegen off` or unused functions that only place pool constants. A duplicate
  helper needs outside evidence (a symbol, another version, the SPM reference). If a fake is the only route to 100,
  stop at the best natural score with an honest note. ([structural fakes](@{Structural fakes are refused}))
- One data-only exception: a labelled `STRIPPED FUNCTION PLACEHOLDER` at the stripped function's slot, under MUST_MATCH,
  reproducing only its leftover constants or strings, steering no real code, with its own ledger entry (AGENT_BRIEF
  rule 7; precedent `mapdrv_sdata2_order`). ([mapdrv placeholder](@{mapdrv links: a STRIPPED FUNCTION PLACEHOLDER}))
- A guard goes only when the natural form compiles to the same object (`tools/obj_equiv.py`, then the linked checksum),
  and the matching form then becomes the only version; where nothing matches, keep the block and record what was
  tried. What stays guarded: volatile reads and writes, type puns (`*(GXColor*)&`, punned copies and stores), int round
  trips, dead stores and self-assignments, redundant tests and ternaries, function-pointer casts, register- or
  stack-steering temps, and inlines or helpers whose names lack outside evidence. A retail uninitialised read keeps a
  well-defined `#else`. Data layout (strings, `gap_`/`force_active`, section declspecs, data definitions, `.sbss2`
  templates) was out of scope for the guard-removal passes; changes there that matched are recorded as leads, not
  applied. ([mmbattle](@{mmbattle:}), [mmsrc](@{MUST_MATCH source shapes outside battle}), [mmsrc review](@{Review of n9-q-o-mmsrc}), [hack review](@{Matching-hack review}))

**Partial functions**
- A function below 100% carries a marker with its score and what differs (`// NONMATCHING (95%): ...`), never has an
  empty or stand-in body, and its score doesn't drop (`--allow-regress` only to remove wrong code). A fake found in
  review is fixed honestly even if its function falls below 100. ([partial functions](@{Partial functions:}), [review backlog](@{Review backlog, Oct 10-17}))

**Asm**
- Game code (src/, rels/) is C. An asm function there needs an entry in `tools/asm_exceptions.json` (class, evidence,
  approval, date): `original-asm` (hand-written in the original, shown by the asm itself) or `compiler-artifact` (bytes
  GC/2.6 provably can't produce from C, under MUST_MATCH with a C `#else`). Lint refuses any other.
  ([asm policy](@{Inline asm in game code}))
- Library code keeps asm only where another source of that library writes the function in asm; `--allow-asm` covers
  only `libs/metrowerks/src`, and the review names that source. Cleanup leaves asm alone. ([library asm](@{Real asm in the original libraries}), [asm policy](@{Inline asm in game code}), [cleanup guide](@{Cleanup guide from Melee practice}))

**Data layout**
- Under `-inline auto,deferred` a file's functions come out in reverse source order, so source order is the reverse of
  the asm's; `tools/fn_order.py` checks it. ([function order](@{Sep 29: fully matched but not linked was mostly function order}), [Grind 38](@{Grind 38:}))
- Pool constants: named objects land first in their section in definition order, literals after them in first-use
  order; a writable `.sdata2` object defined after the code lands behind them instead ([Grind 58](@{Grind 58:})). A named constant becomes a literal only where its unit's object stays obj_equiv-equivalent; ones a MUST_MATCH
  conditional names, address-taken, sizeof'd or volatile stay named. Pool order comes from the original's own
  functions, not helper functions. ([address literals](@{Tooling: address-name literals}), [literal sweep](@{Readability: address-named constants back to literals}), [Night 2](@{Night 2:}))
- Trailing `gap_*` objects in `.data`, `.sdata`, `.sbss` and `.bss` are defined where the next unit's addresses depend
  on them (`.sdata2`/`.rodata` gaps can't be); `.sbss` statics follow their
  first mention in the source; string padding and alignment NULs the object needs stay. ([Grind 58](@{Grind 58:}), [literal sweep](@{Readability: address-named constants back to literals}))

**Naming**
- Names from the game (non-address names in `symbols.txt`, strings in the binary) are never renamed. Unknown meaning
  stays `field_0xNN` at its offset (`pad_0xNN` only for real gaps) or the address name; no invented placeholders,
  value-only or numbered names. A wrong name is worse than a placeholder. Fields are camelCase.
  ([cleanup guide](@{Cleanup guide from Melee practice}))
- Every new name carries its evidence in its chapter (the consumer that proves it); where only roles are observed,
  record them and keep the placeholder. New helper and inline names need outside evidence (a symbol, another version,
  the SPM reference). ([win_log](@{Readability: win_log raw WW_ offsets}), [mmsrc review](@{Review of n9-q-o-mmsrc}))
- Symbol renames go through `tools/rename_symbol.py`; a function rename edits `symbols.txt` too, so its branch runs CI
  with `--allow-protected`. ([symbol renames](@{Tooling: address-name literals}))

**Attribution**
- Every model-authored result carries a `Model:` line, and every commit with model-authored work ends with a
  `Co-Authored-By:` trailer naming the model that actually did it (your own for your own work, the report's `Model:`
  for merged agent work); if that isn't recorded, find out before citing it. PR descriptions don't repeat it (`CLAUDE.md`).
  ([Snood](@{Carried over from Snood}), [attribution notes](@{Nights 2-6: patterns and attribution}))

**References and cleanup**
- Melee (doldecomp/melee, `~/melee`) is the reference: study it and the other finished decomps in `~/` before building
  a tool. It has no licence file, so write our own and copy no Melee code. ([Melee](@{Melee as the reference}), [reference repos](@{Reference repos}), [cleanup guide](@{Cleanup guide from Melee practice}))
- Code from the CC0/MIT reference decomps (`~/ttyd-refs`) may be reused when the report or PR names the project and
  file; never from an unlicensed project. ([Wave 12](@{Wave 12: the Metrowerks}), [ttyd-refs](@{Grind 63:}))
- `tools/cleanup_index.py` (placeholder and address names, raw offsets, function-pointer casts and match hacks, weighted per kLOC) is the cleanup scoreboard; quote it
  before and after each change. `docs/CLEANUP_GUIDE.md` holds the cleanup rules: raw offsets first, then placeholder
  names, retypes, and match hacks last; cleanup never adds a hack site. ([cleanup index](@{Cleanup index: a readability metric}), [cleanup guide](@{Cleanup guide from Melee practice}), [review backlog](@{Review backlog, Oct 10-17}))

**Working with Isaac**
- Ask Isaac before launching a wave, spending on inference, sharing agent sessions or traces, adding hooks, cron jobs or
  other standing setup, and force-pushing anything but your own PR branch. Every number in a commit message or the
  ledger comes from the build; report failures as plainly as successes. ([orchestrator's role](@{The orchestrator's role}))
