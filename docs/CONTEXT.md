# TTYD decomp: the ledger

The single record of every decision on this fork and why it was made. New chapters go at the bottom, dated, with who
made the call. The "Current state" and "Standing rules" sections are the only parts that get rewritten in place;
everything else is append-only history (correct an old chapter by adding a new one that supersedes it, and say so).

The matching-era chapters (2026-09-24 to 2026-10-18) are in `docs/CONTEXT-matching.md`, which is frozen; new
chapters still go at the bottom of this file. Search both: `grep -n PATTERN docs/CONTEXT*.md`.

Private notes (inference provider, accounts, keys, capacity) go in `docs/CONTEXT.private.md`, which is
gitignored, never here: this repo will be public.

Agents running in bulk never edit this file. They write their own `LOG.md`/`REPORT.md`; the orchestrator folds
anything worth keeping into a chapter here.

## Current state (rewrite this section as things change)

- **Fresh build, 2026-10-17:** 9,761 / 9,769 functions matched (99.92%), fuzzy 99.99709%, 1,228 / 1,233 units
  complete. Eight functions are open in three units: `mri/mri_puni` (4), `MarioSt/driver/mapdrv` (2),
  `MarioSt/driver/animdrv` (2). `driver/envdrv` now links, all 22 functions matched;
  `party/party_hit` links, all 11 functions matched. `build/G8MJ01/report.json`
  on the Mac is not kept current; quote the tower's (`scratch/tci.sh`, which ratchets against the baseline accepted
  after each merge). A Mac checkout's own `build/ci/accepted.json` can be days old and then shows false regressions:
  never `--accept` from it.
- **Data tables typed (2026-10-22, n9-q-datatype):** the raw `u32` tables' pointer words are typed initialisers now
  (ptr_to_int 1571 -> 32); what's left raw and why is in the chapter "Initialised data tables typed".
- **tou/tou linked (2026-10-06, Codex GPT-6).** 36/36 functions at 100%; both pools IDENTICAL, original function order, natural compile passes, all 32 build checksums pass. See the dated chapter below and CODEX_ATTEMPTS.md.

- **⚠️ Ledger audit planned (Isaac, Oct 2).** This ledger has drifted: some entries are outdated, and some claims were wrong when written. Later, go through it step by step, check each claim against the repo, the build and the git history, and correct or remove what's wrong. Until then, treat any number or claim here as a lead to verify, not a fact. (This section itself still describes Sep 29.)
- New orchestrator session? Read "The orchestrator's role: how to pick this up" (in `docs/CONTEXT-matching.md`) first, then the
  chapters after it; the newest chapters (Sep 28-29: the REL chapters, waves 17 and 18, Claude #3's grind rounds)
  supersede it where they differ. The agents' rules live in `docs/AGENT_BRIEF.md`, attached to every prompt.
- Fork of [doldecomp/ttyd](https://github.com/doldecomp/ttyd) at `2df569d` (Jul 5, 2026), private at
  `IsaacGemal/ttyd`. `upstream` is fetch-only; its push URL is `DISABLED`. `gh` defaults to this repo.
- Target: `G8MJ01` (Japan, Rev 0), the only version the project supports. The disc is already extracted into
  `orig/G8MJ01/`.
- Progress (main 8f20e58, Sep 29 02:50, after PRs #22-#31): 5,182 of 9,770 functions matched, 27.44% of code;
  linked code 14.02%, **linked data 36.94%** (4.94% the evening before). All 32 checksums (dol + 31 RELs) pass.
  Most of the data jump is REL units written by generators (tools/rel_gen.py, tools/dol_gen.py) and proven by
  linking; ~1,000 of their tables are still raw `u32` arrays marked `// TODO: type`.
- Sessions (Sep 29): "TTYD Claude #1" (Claude Opus 5.5; its session is titled "Ambitious work session") and "TTYD Claude #2" (Opus 5.5) orchestrate waves,
  one wave each; "TTYD Claude #3" (Claude Sonnet 5.5) grinds link blockers and shared REL units itself, no agents.
  Isaac merges or explicitly delegates merging; he authorized the Sep 28-29 overnight run.
- Reviews: one fresh Claude review per PR (Codex out of usage), findings fixed before merge. A unit that links is
  proven byte-for-byte by the checksum, so reviews look at what it can't see: names, types, headers, hacks,
  readability (scripts must be EVT macros, never hex). Since 2026-10-22 (n9-q-hexevt) no script in src/ or rels/ is
  hex or EVT_CMD words any more; the 21 hits `grep -rE '0x000[0-9A-F]005[BCDE]' src rels` still gives are data.
- In flight: Claude #3's branch grind-7 (one more shared unit, and `-fp_contract on` in cflags_rel: a compiler
  flag change, checksums pass; Isaac to look), to be rebased on main as one PR.
- Next, in rough order: (1) complete the battle structs (BattleUnitKind, BattleVulnerableStatus, unit defence and
  attribute tables, BattleWeapon's pointer fields) and rerun a wave-18-style typing pass over the ~1,000 raw tables;
  (2) a REL wave over rooms with more functions, from generated skeletons (agents start from the brief with rule
  18); (3) the 20 shared enemy units with functions, the 22 wave 17 units that didn't link, and the 6-unit gap in
  tools/rel_cmp.py (compares equal, checksum fails); (4) REL header clean-up and the generator's stand-in
  `extern s32` declarations (see the PR #30 review in the "Wave 17" chapter); (5) the dol's code, still the bulk.
- **⚠️ Assume our tools have bugs we haven't found yet (Isaac, Sep 26).** Tonight alone: ttydasm printed wrong
  strings, aborted on 13% of scripts and picked the wrong one of two same-named scripts; evt_cmd.h had wrong
  macros; rel_cmp.py calls 6 units equal that don't link. When a result surprises you, suspect the tool first.
  See the chapter "Our tools will have bugs".

## Standing rules

The rules still in force, each linked to the chapters that decided it; where a later chapter changed a rule, this is the
current version. Update it here when a new chapter changes one. The working detail is in `docs/AGENT_BRIEF.md`
(matching) and `docs/CLEANUP_GUIDE.md` (cleanup).

**Build and parity**
- The linked build stays byte-identical: `ninja` recreates `build/G8MJ01/ok`. A model's claim of a match is never
  evidence; only the checker is. ([Snood][1], [local CI][2])
- Every changed C file also compiles without `-DMUST_MATCH` (`configure.py --non-matching`); `ci.py` runs this as its
  natural step. ([Melee's workflow][3], [mmsrc][4])
- Mac and Linux builds agree: file names keep the case `configure.py` and `splits.txt` use (`HARDWARE.C`). Quote the
  tower's numbers; a Mac's `report.json` or `build/ci/accepted.json` can be stale, so never `--accept` from it.
  ([build parity][5], [current state][6])

**CI and ratchets**
- `python3 tools/ci.py` before every commit; commit only on PASS. The ratchet fails if any function, unit or total (code
  and data, matched and linked) gets worse; `--allow-regress NAME` only for a deliberate fix or removal of wrong code,
  said in the commit. ([local CI][2], [orchestrator's role][7])
- Lint: no new inline asm, no new `#pragma` (`--allow-pragma`), no edits under `config/ orig/ tools/ .github/ libs/
  assets/` (`--allow-protected`), `configure.py` edits only to `Object(...)` lines (`--allow-configure`). Unguarded
  function-pointer casts (`tools/fncast_lint.py`, AGENT_BRIEF rule 7) and known compiler-hang triggers
  (`tools/hang_guard.py`) fail too, whatever the flags. ([local CI][2], [Melee's workflow][3], [incident][8])
- Data ratchet: objdiff scores a data relocation by its symbol, so `tools/data_check.py` compares the bytes of every
  initialised data object a diff defines or changes with the original's (`--allow-data-mismatch NAME` excuses one); a
  data section's score may not drop either. ([Grind 87][9], [eff_kemuri][10])
- No function prototypes in .c files (statics and `tools/local_prototype_exceptions.json` excepted); with
  `-requireprotos` a call with no prototype is a build error. ([local prototypes][11], [Melee's workflow][3])
- The cleanup index is a soft ratchet: `ci.py` prints it with its delta and warns, never fails, when it rises.
  ([cleanup index][12], [review backlog][13])

**Pragmas**
- Whole-file settings go in `configure.py` as per-file `extra_cflags`, never as a pragma at the top of a file. A scoped
  pragma stays only if the object needs it, with a one-line reason (`// needed: without it FN X%`); cleanup never adds
  one. Never `#pragma scheduling <cpu number>`: this mwcceppc loops forever on it. ([pragma policy][14],
  [hack review][15], [pragma removal][16], [incident][8])
- Where every scoped pair of one kind in a file can go with one flag on its Object line (`-inline noauto`, `-O4,s`,
  `-pragma "opt_X off"`, or an inline limit such as `-pragma "inline_max_size(N)"`, as TWW/TP/AC do) and the object
  stays identical, the flag replaces them; a flag that holds for only part of a file isn't added. ([file flags][39])
- A per-file `-char signed` (or `unsigned`) on an Object line is allowed where it removes casts and puns and the object
  stays identical, as TP does for 20 files. ([cleanup decisions][40])

**MUST_MATCH and structural fakes**
- Code that exists only to reproduce the original bytes goes under `#ifdef MUST_MATCH`, the natural form under `#else`.
  ([code smells][17], [Melee's workflow][3])
- MUST_MATCH is for small local tricks. Refused even when guarded: functions or inlines that only steer registers (above
  all reshaped copies of existing helpers), calls through casts that reorder or retype a prototype, inlining, recursion
  or loops expanded by hand, and `defer_codegen off` or unused functions that only place pool constants. A duplicate
  helper needs outside evidence (a symbol, another version, the SPM reference). If a fake is the only route to 100, stop
  at the best natural score with an honest note. ([structural fakes][18])
- One data-only exception: a labelled `STRIPPED FUNCTION PLACEHOLDER` at the stripped function's slot, under MUST_MATCH,
  reproducing only its leftover constants or strings, steering no real code, with its own ledger entry (AGENT_BRIEF rule
  7; precedent `mapdrv_sdata2_order`). ([mapdrv placeholder][19])
- A guard goes only when the natural form compiles to the same object (`tools/obj_equiv.py`, then the linked checksum),
  and the matching form then becomes the only version; where nothing matches, keep the block and record what was tried.
  What stays guarded: volatile reads and writes, type puns (`*(GXColor*)&`, punned copies and stores), int round trips,
  dead stores and self-assignments, redundant tests and ternaries, function-pointer casts, register- or stack-steering
  temps, and inlines or helpers whose names lack outside evidence. A retail uninitialised read keeps a well-defined
  `#else`. Data layout (strings, `gap_`/`force_active`, section declspecs, data definitions, `.sbss2` templates) was out
  of scope for the guard-removal passes; changes there that matched are recorded as leads, not applied. ([mmbattle][20],
  [mmsrc][4], [mmsrc review][21], [hack review][15])
- Each trick family is decided once and applied everywhere ([guard consistency][38]). Guarded where the plain form gives
  a different object, plain where it gives the same one: comma sequencing (`(x = f(), x)`, `(0, x)`), a cast to the
  value's own type, const-stripping or volatile reads of named constants, round trips through another type, dead stores,
  self-assignments, redundant tests and ternaries on a 0/1 test, `x ^ 0`, and calls of a named function through a cast
  (a different prototype, or its own type to stop inlining). Plain C, never guarded: signedness, width and enum-to-int
  casts that change a value or a comparison (one on an `== 0` test or re-narrowing a narrowed value is a trick), a cast to the callee's parameter type, `& 0xFFFFFFFF` on a 64-bit value, an assignment inside an argument, an
  if/else storing 0/1, float-to-int-to-narrow conversions, float bit tests, puns forced by a field's or object's declared
  type (typing debt: retype it instead), function-pointer casts of values or to an API's callback type, and `volatile`
  on hardware registers or on fields a callback writes (declared in the struct).

**Partial functions**
- A function below 100% carries a marker with its score and what differs (`// NONMATCHING (95%): ...`), never has an
  empty or stand-in body, and its score doesn't drop (`--allow-regress` only to remove wrong code). A fake found in
  review is fixed honestly even if its function falls below 100. ([partial functions][22], [review backlog][13])

**Asm**
- Game code (src/, rels/) is C. An asm function there needs an entry in `tools/asm_exceptions.json` (class, evidence,
  approval, date): `original-asm` (hand-written in the original, shown by the asm itself) or `compiler-artifact` (bytes
  GC/2.6 provably can't produce from C, under MUST_MATCH with a C `#else`). Lint refuses any other. ([asm policy][23])
- Library code keeps asm only where another source of that library writes the function in asm; `--allow-asm` covers only
  `libs/metrowerks/src`, and the review names that source. Cleanup leaves asm alone. ([library asm][24],
  [asm policy][23], [cleanup guide][25])

**Data layout**
- Under `-inline auto,deferred` a file's functions come out in reverse source order, so source order is the reverse of
  the asm's; `tools/fn_order.py` checks it. ([function order][26], [Grind 38][27])
- Pool constants: named objects land first in their section in definition order, literals after them in first-use order;
  a writable `.sdata2` object defined after the code lands behind them instead ([Grind 58][28]). A named constant
  becomes a literal only where its unit's object stays obj_equiv-equivalent; ones a MUST_MATCH conditional names,
  address-taken, sizeof'd or volatile stay named. Pool order comes from the original's own functions, not helper
  functions. ([address literals][29], [literal sweep][30], [Night 2][31])
- Decided 2026-10-22 (Isaac, from Melee/TWW/TP/AC/SPM precedent; [cleanup decisions][40]): a named pool constant whose
  only job is a MUST_MATCH pool-order trick may go together with that trick, so its uses become literals, when the
  object stays identical (TWW, TP and AC have no named pool floats). A guarded named template may become a compiler
  template (`@NNN`) the same way, as #1120/#1122 did. Initialised data tables held as raw `u32`/`s32` words may be
  typed (structs, pointer members, typed work pointers) when `tools/data_check.py` shows identical bytes. Effect and
  work `userdata` stays `void*` with a typed local (SPM's `userWork`, Melee's `user_data`); no union of pointer types.
- Trailing `gap_*` objects in `.data`, `.sdata`, `.sbss` and `.bss` are defined where the next unit's addresses depend
  on them (`.sdata2`/`.rodata` gaps can't be); `.sbss` statics follow their first mention in the source; string padding
  and alignment NULs the object needs stay. ([Grind 58][28], [literal sweep][30])

**Naming**
- Names from the game (non-address names in `symbols.txt`, strings in the binary) are never renamed. Unknown meaning
  stays `field_0xNN` at its offset (`pad_0xNN` only for real gaps) or the address name; no invented placeholders,
  value-only or numbered names. A wrong name is worse than a placeholder. Fields are camelCase. ([cleanup guide][25])
- Every new name carries its evidence in its chapter (the consumer that proves it); where only roles are observed,
  record them and keep the placeholder. New helper and inline names need outside evidence (a symbol, another version,
  the SPM reference). ([win_log][32], [mmsrc review][21])
- Symbol renames go through `tools/rename_symbol.py`; a function rename edits `symbols.txt` too, so its branch runs CI
  with `--allow-protected`. ([symbol renames][29])

**Attribution**
- Every model-authored result carries a `Model:` line, and every commit with model-authored work ends with a
  `Co-Authored-By:` trailer naming the model that actually did it (your own for your own work, the report's `Model:` for
  merged agent work); if that isn't recorded, find out before citing it. PR descriptions don't repeat it (`CLAUDE.md`).
  ([Snood][1], [attribution notes][33])

**References and cleanup**
- Melee (doldecomp/melee, `~/melee`) is the reference: study it and the other finished decomps in `~/` before building a
  tool. It has no licence file, so write our own and copy no Melee code. ([Melee][34], [reference repos][35],
  [cleanup guide][25])
- Code from the CC0/MIT reference decomps (`~/ttyd-refs`) may be reused when the report or PR names the project and
  file; never from an unlicensed project. ([Wave 12][36], [ttyd-refs][37])
- `tools/cleanup_index.py` (placeholder and address names, raw offsets, Melee's pointer-retype families, function-pointer
  casts and match hacks, weighted per kLOC) is the cleanup scoreboard; quote it before and after each change. `docs/CLEANUP_GUIDE.md` holds the cleanup
  rules: raw offsets first, then placeholder names, retypes, and match hacks last; cleanup never adds a hack site.
  ([cleanup index][12], [cleanup guide][25], [review backlog][13])

**Working with Isaac**
- Ask Isaac before launching a wave, spending on inference, sharing agent sessions or traces, adding hooks, cron jobs or
  other standing setup, and force-pushing anything but your own PR branch. Every number in a commit message or the
  ledger comes from the build; report failures as plainly as successes. ([orchestrator's role][7])

[1]: CONTEXT-matching.md#carried-over-from-snood-2026-09-24
[2]: CONTEXT-matching.md#local-ci-toolscipy-2026-09-25-isaac--claude-opus-55
[3]: CONTEXT-matching.md#melees-workflow-adopted-and-agents-run-the-merges-checks-themselves-2026-09-26-isaac-another-claude-opus-55-session-surveyed-melee-and-started-it-claude-opus-55-finished-it
[4]: #must_match-source-shapes-outside-battle-effect-driver-mario-party-window-event-manager-top-level-src-2026-10-20-claude-opus-55-orchestrating-eight-claude-opus-55-workers-n9-q-o-mmsrc
[5]: CONTEXT-matching.md#build-parity-hardwarec-renamed-to-hardwarec-2026-09-24-claude-opus-55
[6]: #current-state-rewrite-this-section-as-things-change
[7]: CONTEXT-matching.md#the-orchestrators-role-how-to-pick-this-up-2026-09-25-isaac-written-up-by-claude-opus-55
[8]: CONTEXT-matching.md#incident-hung-compiles-ran-for-a-day-on-the-tower-2026-10-02-claude-opus-55-orchestrator-review-by-codex
[9]: CONTEXT-matching.md#grind-87-cipy-checks-the-bytes-of-the-data-objects-a-change-touches-2026-10-02-claude-sonnet-55-ttyd-claude-3-orchestrators-request-after-215
[10]: #readability-eff_kemuri-must_match-blocks-2026-10-20-claude-haiku-55--claude-opus-55-n9-q-mmkemuri
[11]: CONTEXT-matching.md#local-prototypes-moved-to-headers-ci-now-rejects-new-ones-2026-09-26-codex--gpt-6-sol-finished-by-claude-opus-55
[12]: #cleanup-index-a-readability-metric-with-a-soft-ratchet-2026-10-19-claude-haiku-55-night8-cidx-reviewed-by-claude-opus-55
[13]: CONTEXT-matching.md#review-backlog-oct-10-17-merges-2026-10-17-isaac--claude-opus-55
[14]: CONTEXT-matching.md#pragma-policy-2026-09-25-claude-opus-55-at-isaacs-request
[15]: #matching-hack-review-2026-10-19-claude-opus-55-night8-hacks
[16]: #optimisation-pragmas-removed-where-the-object-doesnt-need-them-2026-10-20-claude-opus-55-n9-q-o-pragma
[17]: CONTEXT-matching.md#code-smells-to-track-melees-cleanup-index-2026-09-25-isaac--claude-opus-55
[18]: CONTEXT-matching.md#structural-fakes-are-refused-guarded-or-not-2026-10-17-isaac--claude-opus-55
[19]: CONTEXT-matching.md#mapdrv-links-a-stripped-function-placeholder-and-the-project-reaches-100-linked-2026-10-18-isaac--claude-opus-55
[20]: #mmbattle-original-source-shapes-for-srcbattle-must_match-blocks-2026-10-20-claude-opus-55-n9-q-o-mmbattle
[21]: #review-of-n9-q-o-mmsrc-helpers-and-steering-arms-back-under-must_match-2026-10-21-claude-opus-55-reviewer
[22]: CONTEXT-matching.md#partial-functions-allowed-if-labelled-never-empty-2026-09-25-isaac-written-up-by-claude-opus-55
[23]: CONTEXT-matching.md#inline-asm-in-game-code-the-exception-policy-2026-10-04-isaac
[24]: CONTEXT-matching.md#real-asm-in-the-original-libraries-sep-29-isaac-asked-for-a-note-written-by-ttyd-claude-1
[25]: #cleanup-guide-from-melee-practice-2026-10-20-isaac-written-up-by-claude-opus-55-n9-q-guide
[26]: CONTEXT-matching.md#sep-29-fully-matched-but-not-linked-was-mostly-function-order-claude-sonnet-55-second-near-miss-round
[27]: CONTEXT-matching.md#grind-38-six-more-units-linked-the-function-order-check-2026-09-30-claude-sonnet-55-ttyd-claude-3
[28]: CONTEXT-matching.md#grind-58-gaps-sbss-order-small-data-constants-eleven-more-links-2026-10-01-claude-sonnet-55-ttyd-claude-3
[29]: #tooling-address-name-literals-and-symbol-renames-2026-10-19-claude-opus-55-n9-q-addr-reviewed-by-claude-opus-55
[30]: #readability-address-named-constants-back-to-literals-tree-wide-2026-10-20-claude-opus-55-n9-q-addrall
[31]: CONTEXT-matching.md#night-2-data-ports-without-helpers-and-the-defer_codegen-lever-2026-10-06-claude-opus-55-orchestrating-claude-3-sonnet-55-claude-4-opus-55-deepseek-v41-flash-waves-codex
[32]: #readability-win_log-raw-ww_-offsets-onto-logwin-fields-2026-10-20-claude-haiku-55--claude-opus-55-n9-q-winlog
[33]: CONTEXT-matching.md#nights-2-6-patterns-and-attribution-2026-10-06-to-2026-10-10
[34]: CONTEXT-matching.md#melee-as-the-reference-and-guarding-against-slop-2026-09-25-isaac-written-up-by-claude-opus-55
[35]: CONTEXT-matching.md#reference-repos-and-study-them-before-building-tools-2026-09-27-isaac-written-up-by-claude-opus-55
[36]: CONTEXT-matching.md#wave-12-the-metrowerks-runtime-libraries-2026-09-27-claude-opus-55-agents-deepseek-v41-flash-reviews-claude-opus-55-x3
[37]: CONTEXT-matching.md#grind-63-ttyd-refs-the-shared-read-only-reference-decomps-2026-10-01-claude-sonnet-55-ttyd-claude-3
[38]: #must_match-guarding-made-consistent-across-the-tree-2026-10-21-claude-opus-55-n9-q-o-consist
[39]: #whole-file-compiler-settings-moved-to-configurepy-2026-10-22-isaac-done-by-claude-opus-55-n9-q-o-flags
[40]: #cleanup-decisions-from-melee-tww-tp-ac-and-spm-precedent-2026-10-22-isaac-written-up-by-claude-opus-55

## Parked ideas

Considered and deliberately not built yet. Pick one up by adding a dated chapter.

- **Self-hosted GitHub runner on the tower** (parked 2026-09-25). Supersedes the "Next" line of the local-CI
  chapter. Would run `tools/ci.py` on each push and show checks in GitHub's UI. Not needed now: the merger calls
  `tools/ci.py` directly and records the result in the PR or commit. Revisit when the repo goes public, for a
  visible "build passing" badge; then it must run only on pushes to `main`, never on pull requests (see the
  private go-public checklist). Not implemented, not tested.
- **Cleanup index** (parked 2026-10-02, Isaac). Melee publishes one ([doldecomp.github.io/melee/cleanup](https://doldecomp.github.io/melee/cleanup/), 6,057 sites in 597 files): every place matched C is ugly for matching's sake (pointer moved by a literal offset, pointer retyped, `MUST_MATCH`/pragma/`PAD_STACK`, `goto`), each counted once by its outermost form, regenerated on every push. A rough grep here suggests far more (~6k `(u8*)p + 0xNN`, ~9k `*(T*)p`, ~2k `MUST_MATCH` blocks, ~600 offset macros). Their `tools/cleanup_index.py` and template are sparse-cloned at `~/melee-cleanup` for reference; the repo has no license file, so study it and write our own rather than copying. Ideas: publish it, ratchet it in `ci.py`, later run cleanup waves (naming struct fields first; the check is objective: stays 100% and linked). After matching, not before.

## Leftovers found while playing the matched build (2026-10-18, Isaac + Claude Opus 5.5)

Running the byte-identical build in Dolphin, plus a mod branch that draws a debug overlay, turned up some developer
leftovers. None of this changes the matching build; it's here so nobody has to find it again.

- **A second language mode.** `GlobalWork.language` (0x16C) indexes `language_dir[] = {"msg", "emsg"}` in msgdrv.c.
  JP retail always uses 0; the `emsg/` folder is empty on the JP disc. A US save converted to JP carries `language = 1`:
  every message box then reads "No messages." in red, and the pause menu's Mario page crashes in `winBgGX`
  (from `winMarioDisp`). Setting it to 0 (and fixing the slot checksum) makes the save load cleanly.
- **The crash screen.** `N_systemErrorHandler` (src/unk_0x8025de70.c) formats a full report (exception type,
  registers, back chain) and draws it, but first returns into `psndExit(); while (1) {}` whenever
  `N_getDebugMode() >= 0`, which is the retail case. Removing that check on a mod build brings back the developers' crash
  screen.
- **The E3 demo's stage table.** `event_dt` in src/event.c lists story checkpoints per chapter
  ("ステージ０" to "エンディング", story values up to 406); `eventStgDtPtr`/`eventStgNum` serve it to seq_e3.c's
  stage-select menu.
- **`dbg_hitObjDisp`** (hitdrv) is in the linker map as UNUSED: a collision-drawing debug function the linker stripped.
- **Save layout, JP vs US.** Each 0x4000 card slot holds GlobalWork at +0x8, PouchData at +0x13E0 and deadInfo at
  +0x19B4; JP's data size is 0x2140 with version "009", US's 0x2260 with "010". The first 0x1EB4 bytes have the same
  layout, which is what made the conversion work.

Tooling outside the repo: `~/Dolphin/mcp` (a CLI plus an MCP server) launches builds in Dolphin with the GDB stub on,
drives input through Dolphin's pipe controller, takes screenshots, reads memory by symbol, and symbolizes the crash
screen's report. The overlay lives on the local branch `mod/debug-overlay`; it is not meant for main.

### Cleanup index: a readability metric with a soft ratchet (2026-10-19, Claude Haiku 5.5, night8-cidx; reviewed by Claude Opus 5.5)

With matching done, the next measure is how much of the source a reader still has to decode. `tools/cleanup_index.py`
counts, per file, `unk_`/`field_0x` names, address-named symbols (`lbl_8...`, `func_8...`), raw `(u8*)` offset
casts, gotos, `MUST_MATCH` blocks, optimisation pragmas, abstract function-pointer casts and inline asm, weights
them, and reports the weighted count per 1000 code lines (lower is better). `tools/ci.py` prints it with its delta
against the base as step 5 and warns, never fails, when it gets worse.

Starting point (2911743e): **77.691 per kLOC**, 54,539 weighted over 702k code lines. `field_0x` (20,942) and
address names (13,986) dominate; the worst files are mri_puni.c, battle_audience.c, winmgr.c, battle_seq_end.c and
battle_event_cmd.c. String literals aren't stripped before matching and `must_match` counts directive lines, so treat
small deltas as noise.

### Readability pilot: npcdrv names (2026-10-19, Claude Haiku 5.5, night8-read1; reviewed by Claude Opus 5.5)

First pass at naming instead of matching; every commit was checked byte-identical ("32 files OK"), which proves the
renames changed nothing. NpcEntry: `field_108` → `gait` (1 stay, 2 walk, 3 run: the speed-picked animation in
npcdrv's move update), `field_140` → `interruptFlags` (pending found/lost/return/blow/dead reactions consumed by
`npcCheckInterrupt`), `field_31E` → `hazardImmuneFlags` (one bit per hazard class before `npcGroupDead`; 0x1F from
`set_bottle_flag_init`; one write of bit 1 is unexplained), `wFbatHitCheckRelated` typed u16 (drops four casts).
FieldBattleData gets `state`, `talkNpc`, `talkEvtId`, `ruleKeepResult`, `endEvt`, `endEvtId`, `majinaiEvtId`, which
removes the raw `+0x..` casts in npcdrv.c, seq_battle.c, battle_seq_end.c and evt_npc.c. `NpcEventType` names
`wNpcEventType`'s values. npcdrv.c: unnamed-field uses 184 → 138, typed raw casts 69 → 23.

Blocked: splitting NpcBattleInfo's tail (0x9C-0xC8) into fields wasn't byte-identical (the roll loop indexes the
`field_9C` array), and FieldBattleData's `FieldBattleInfo` at 0x20 keeps its cast because battle_information.h
includes npcdrv.h.

### Matching-hack review (2026-10-19, Claude Opus 5.5, night8-hacks)

Every commit builds to 32 files OK, and every touched file also compiles without `-DMUST_MATCH`.
- **Replaced with natural code:** `evt_puni_set_lsw_index` now calls `puniswSetindex` instead of a hand-copied body
  (under `opt_lifetimes off`, as its other inlining callers); mario_balloon's `ret = ret;`; unneeded volatile reads in
  unit_piders, unit_churantalar, unit_boss_gundan_zako_group2, eff_ice_n64 and eff_kemuri5_n64 (colour words typed
  GXColor); an unused volatile extern in eff_akari_pose_n64.h.
- **Optimisation pragmas:** 61 of the 341 regions were unneeded and are gone (mostly `optimize_for_size` in files already
  built -O4,s); 21 needed ones got a `// needed: without it FN X%` reason (rule 8).
- **Labelled under MUST_MATCH with a reason and an `#else`:** puniswSetindex's `b[2]`/`b[4]` re-masks (plain: 88.5%);
  winmgr's `x = x` / `zero = zero` guards (plain: 92-94%); the remaining match-only volatile reads (party_nokonoko,
  seq_load, the action commands, eki_06, muj_01, eff_funemizu, battle_audience_kinopio, eff_blow_n64, tou_02).
  mapSetMaterialTev's sine comma stays guarded; six natural forms were tried and are listed in its comment.
- **Open (Isaac's call):** unit_barriern.c:1978 and unit_boss_batten_leader.c:3080 carry reshaped, unguarded copies of
  Init helpers taking the same table twice through a volatile parameter, only to steer registers. Rule 7 refuses
  these; their comments say a plain call leaves both functions 80 bytes short.

### Readability: GlobalWork names (2026-10-19, Claude Haiku 5.5, night8-rd-gw; reviewed by Claude Opus 5.5)

Byte-identical renames of `gp` fields: `mapInitEvtId` (0x164, seq_mapchange waits on it), `playerName` (0x11BC),
`saveSlot` (0x11D0, the memcard write target), `lastFileSlot` (0x11B8), `soundSurround` (0x1274),
`resetButtonDown`, `restartChecked`, `keyDataReady`, and the rumble state (`rumbleEnabled`, `rumbleSuppressed`,
`rumbleHeld[Since]`, `rumbleLocked`/`LockStarted`/`LockedSince`: 30 s of continuous rumble locks a pad for 30 s).
All 11 raw `(u8*)gp + 0x..` casts are gone; mot_walk's `extern s32 gp` is now `GlobalWork*`. In review, Opus split
`lastMapName` (0x14C) out of what had been a 0x20-byte `currentAreaName`: seq_mapchange and seq_gameover copy the
current map there before leaving for the title screen or game over. Unnamed `gp->` field uses 75 → 2 (`unk8`, only ever
written 0). `field_C` (a game-over/debug-battle gate nothing sets) stays unnamed.

### Readability: hitdrv names (2026-10-19, Claude Haiku 5.5, night8-rd-hit; reviewed by Claude Opus 5.5)

HitEntry `unk6C` → `animMtx` (mapCalcAnimMatrix output, used instead of localMtx when animated), `unkA8` →
`vectorCount` (length of `vectors`), `siblingActive` → `nextActive` (the active list), and `lbl_80418408` →
`hitActiveHead` (its head; config symbol unchanged). `HitFlag` names flags 0x10/0x20/0x40/0x80 (animated, map-object
bound, matrix dirty, deleted); `HIT_SPARE_ENTRIES`, `HIT_CULL_BACK`, `HIT_ATTR_DAMAGE_RETURN` replace literals, and
hitMain's animation walk uses `MapEntryAnimData*` instead of raw offsets. Still open: the 0x1 flag, hitMain's
animation child list (a count then `MapFileJoint*`s, typed in mapdrv's animation_table), and raw `hitGetAttr` bits in
party/item code. The debug-overlay branch still says `lbl_80418408`/`unkA8` and needs the new names when rebased.

### Readability: winmgr names (2026-10-19, Claude Haiku 5.5, night8-rd-win; reviewed by Claude Opus 5.5)

The window manager was the third-worst file by the cleanup index (429 per kLOC); it's now 164. WinMgrEntry gets
`state` (idle/opening/closing), `animStep` (0..8 through winMgrSeq's ramps), `alpha`, `x`/`y`/`width`/`height`
(users in win_item, evt_shop, evt_win, evt_sub, evt_window and itemdrv updated), `message`, `scrollPos`,
`scrollRows`. WinMgrDesc's first eight bytes are `openStyle` (zoom+rotate, scale+fade, instant) and `frameStyle`,
then `x`, `width`, `height`, `color`; every WinMgrDesc initializer is rewritten to match. WinMgrSelect gets `kind`,
`state`, `cursor`, `scrollTop`, `iconX`/`iconY`, `baseY`, `items` (the new `WinMgrSelectItem`: grey/skip flags and
an id), `numItems`; party and badge loops use `PARTNER_*`, `BADGE_MIN`/`BADGE_COUNT`. `field_0x` uses in
winmgr.c/.h 664 → 17; overall index 77.44 → about 76.4.

Left: `WinMgrDesc.field_0x10` (y; read by `get10` under MUST_MATCH), `WinMgrSelect.field_0x38` (a cancel gate, gauge
gate and leading item id at once), and five `unk_8023....` functions whose names live in symbols.txt and REL callers.

### Readability: battle_event_cmd (2026-10-19, Claude Haiku 5.5, night8-rd-bec; reviewed by Claude Opus 5.5)

Small, header-free pass: raw casts that already had fields (`mMoveSpeedY` at BattleUnit 0x174, `mOwner` at
BattleWorkUnitPart 0x4EC, `commandMenu` at BattleWork 0x171C), `kInactive`, `UNIT_MARIO` and
`TYPE_PARTNER_MIN/MAX`. About 290 raw casts remain because they land in unnamed padding of shared battle structs.
**Backlog for a BattleUnit/BattleWork naming pass** (use counts from battle_event_cmd.c): BattleUnit 0x168 (s32
move-frame countdown, 61 uses; BattleWorkUnitPart 0xA0 is its pair), 0x17C/0x180 (f32 remaining distance), 0x178,
0x188, 0x1B0, 0x1B8, 0x1C0; BattleWorkUnitPart 0xB0-0xB8 (f32), 0xC0 (sound work), 0x204 (flags), 0x4D0 (a
`BtlUnitPartPoseSound` documented only in a comment); `PartsAttribute_Flags` lacks 0x10000000; BattleWork's
`actionCommands` size looks wrong (accesses at 0x1CC8-0x1D14 and 0x2744/0x2748 land inside it); BattleStage
0xB24-0xB38 looks like two Vecs. Two sites near battle_event_cmd.c:4514 compute `-(a - a)` from 0x1B8: a match trick
that belongs under MUST_MATCH.

### Readability: battle_seq_end work struct (2026-10-19, Claude Haiku 5.5, night8-rd-bse; reviewed by Claude Opus 5.5)

`BattleSeqEndWork` was a 0x2AC-byte blob read through raw offsets; it's now a real struct (same size): `timer`,
`stockExp`, `expRemaining`, `lvupPending`, `lvupSelect`, `confirmSel`, `dispFlags` (the new `BattleSeqEndDisp`
bits), the 19 exp-star icon arrays, `waitEvtId`, `objs[3]` (the HP/FP/BP choices, `LvupSelectObj`, moved from the
.c), `effect`, `whistleTimer`, `confettiEvtId`, `rankUpEvtId`. Unit-kind literals use `UNIT_*`/`TYPE_PARTNER_*`,
the guard check uses `kShellShielded`. Raw casts in battle_seq_end.c 217 → 8; file index 445 → 239 per kLOC.

Found on the way: `_audience_Whistle_control` switches on `unk0` with case labels 0x07000007-0x0700001B (sequence
states), but `unk0` is only ever zeroed, so the switch never takes a case: apparently dead in retail. Shared-struct
leftovers: BattleWork byte at 0x180DC (`|= 0x20` in `_disable_enemy_unit_disp`), unnamed BattleWork.flags bits
0x40/0x100/0x400000, `EventEntry` 0x88/0x8C used as scratch s32s, and the `0x07000000` step values (no enum yet).

### Readability: battle_audience names (2026-10-19, Claude Haiku 5.5, night8-rd-aud; reviewed by Claude Opus 5.5)

battle_audience.c was the second-worst file by the cleanup index (215 per kLOC); it's now 101. Named:
BattleAudienceItem (thrown items: `timer`, `memberId`, `pos`, `scale`, `angleZ`, `vel`/`accel`, `target`,
`hitResult`), the AP-star particles (`BattleAudienceStar`, now typed and stepped with `++` instead of `+= 0x60`),
members (`animCur`/`animWait`, colour bytes, jump offset/velocity/acceleration, `normal`, `texIndex`), sounds and the
audience window, and the audience totals (`typeWeights[12]`, `apGauge` (cap 0x320), pleased/displeased streaks,
item-throw counts, `phaseEvtList`, `joyLevel`, the present-item fields). Caution kept: the member cursor at 0x28 is an
s32 advanced by byte counts, so typing it as a pointer would silently change the step sizes; it stays raw. Next
target: battle_audience_kinopio.c, which reads `member->field_0x54 + N` about 100 times.

### Readability: seq_load names (2026-10-19, Claude Haiku 5.5, n9-rd-load; reviewed by Claude Opus 5.5)

seq_load.c's `LoadWork` (the file-select state) gets names from its uses: `flags` bits (0x10 all slots empty, 0x20
no usable save, 0x40 memcard event running, 0x80 rerun the card check), `step`, `cursor` (win_dt index: 0-3 slots,
4 delete, 5 copy, 6 title, 7 sound, 8 rumble), `prevCardCode`, the sound and rumble cursors and committed values,
the cursor's animated and target position, `copySrc`/`copyDst`, `message`, `menuEvt`, `printWindowId`. `WinDt` gets
`texId`/`texId2`, `navUp`/`Down`/`Left`/`Right` (0x1000/0x2000/0x4000/0x8000) and `cursorOffX`/`Y`. The save slot
reads in loadDraw go through `CardSaveSlot` (`data.flags`, `data.global.playerName`), the palette through
`FileEntry.data` (`LOAD_TEX_PALETTE()`), and the sound mode through `SND_OUTPUTMODE_*`. Index 563 → 242 weighted
for the file; overall 74.98 → 74.52.

Left: `card->saves[wp->cursor].data.flags` breaks the checksum (the array index codegen differs from the raw
`(wp->cursor << 14)`), so those reads stay raw; the byte-stepped scans in loadMain stay raw too (they're tuned to
retail's registers). The GlobalWork-internal slot reads (level, play time, mask, field17c) have no names yet.

### Readability: msgdrv names (2026-10-19, Claude Haiku 5.5, n9-rd-msg; reviewed by Claude Opus 5.5)

MessageData (one text box's state, 0x7A58 bytes) and MsgDataEntry (one parsed glyph or command) get names from
msgMain, msgAnalize, msgDisp and evt_msg: `flags` (callback-once, key advances, drive the talk anim of
`battleUnitId`, history queue drawn, text held, no history), the choice counters (`choiceIndex`, `choiceState`,
`choiceMade`), `entryPos`, `textTime`/`effectStart`/`lastFrameTime`, the scroll state, `history[0x6E]` with its
count and cursor, `callback`, `sfxVolume`, msgAnalize's running pen/cursor state, the select-window fields, the talk
animation toggle; entries get `arg` (glyph flags 0x1 pop-in, 0x2 shake, 0x4 wave, 0x8 talk, 0x10 SE, or a command's
payload), `x`/`y`, `iconOffsetY`, `startTime`, `scale`. `window->type` uses `WINDOW_TYPE_*`. Users in evt_msg and the
kpa_01/kpa_07/nok_00/nok_01/pik_00 RELs updated. msgdrv.c index 323 → 102 per kLOC; most of the remaining `field_0x`
are WindowEntry's (windowdrv.h). Kept raw: msgMain's repeated `((MessageData*)smart->address)->` (caching it isn't
byte-identical) and the match-trick casts under comments.

### Readability: BattleUnit / BattleWorkUnitPart padding (2026-10-19, Claude Haiku 5.5, n9-rd-bunit; reviewed by Claude Opus 5.5)

Works through the backlog from the battle_event_cmd entry. BattleUnit gets the move state the btlevtcmd movers use
(`moveFrames` 0x168, `moveAngle` 0x178, `moveDist` 0x17C, `moveProgress` 0x180, which the hop code also reuses as a
height scale), the hop state (`moveStep`, `hopAngle`, `hopAngleStep`, `hopY`), and `mBodyPartsId`/`mTalkPoseName`/
`mStayPoseName` (0x2D0-0x2D8, replacing battle_unit.c's local `BattleUnitAnimFields` overlay). BattleWorkUnitPart
gets the same move fields, `partFlags` (0x204), and `poseSound`/`poseSoundEventId` (replacing the local
`BattleUnitPartFields` overlay). `PartsAttribute_Flags.kAbsolutePosition` (1 << 28) names the bit that makes
Set/GetPartsPos use `mPosition` directly. Raw unit/part casts 411 → 225; battle_event_cmd.c 800 → 318 weighted.
Still local or unnamed: BtlUnitSeMode, battle_unit.c's HpGaugeLocal (duplicates BattleWorkHpGauge; codegen-sensitive),
the part's pose-anim block at 0x1C0, and BattleUnit 0x29C-0x2CC.

### Readability: mri_puni names (2026-10-19, Claude Haiku 5.5, n9-rd-puni; reviewed by Claude Opus 5.5)

mri_puni.c was the worst file in the tree (286 per kLOC); it's now 108. PuniWork gets `systemMode`, `area` (from
evt_puni_setup, mirrored in SW 0x299), `flags`, `levelY[4]`, `yMode`/`mapKind`, `respawnPos`, the two y-bands,
`switchPuni[4]`, `targets[10]` (a new `PuniTarget`), `phase`/`prevPhase`, the random seeds, the swarm sound state,
`arrivePos`/`arriveDistSq` pairs and the fight-sound counters. PuniData (one Puni, 0x5A8 bytes) gets `flags`, `mode`,
`state`, anim indices, `angle`, `pos`, `vel`, `accel`, the `parentPuni`/`leftPuni`/`rightPuni` links,
`target`, timers, `effect`, `direction`, `scale`, and the jump fields (`height`, `vertVel`, `jumpVelStart`,
`gravity`). Array bounds use `PUNI_DATA_MAX` and `sizeof(PuniData)`. Kept raw: five functions (`puniModePoint`,
`puniModeExit`, `puniModeEnter`, `puniModeMass`, `unk_mri_00012d58`) whose `u8* p` base can't be retyped without
changing register allocation, and `((PuniData*)p)->` accesses elsewhere for the same reason. `flags` stays s32
(u32 changes evt_puni_flag_onoff).

### Readability: PartyEntry names (2026-10-19, Claude Haiku 5.5, n9-rd-party; reviewed by Claude Opus 5.5)

The field partner's PartyEntry (0x188 bytes) gets `timer` (0x24), `motionStep` (0x38, the party_motion_* switch
selector), `moveAngle`, `spinAngle` (p_roll), `targetPos`/`targetDist`, `groundY`, `height` (partyGetHeight),
`radius`, `playerTargetYaw`, and the hit results `frontHit`/`groundHit`/`headHit` (check_front,
partySearchGround/Head). party_motion's offset macros use the fields where they exist (412 sites), single-bit
`PartyFlags` literals use the enum (385 sites, across mobjdrv, npcdrv, evt_*, mario_*, camdrv, mot_dokan), and
member-id compares use `PartyMember` (54). Raw PartyEntry casts 761 → 371; party_motion.c 206 → 100 per kLOC.
Open: the tail 0x174-0x187 is a per-partner union (Vivian, Bobbery and Flurrie each use it differently) and needs a
per-character layout; nokonoko_use's NKF(0x10C) stays a macro (the member form changes its code).

### Readability: WindowEntry names (2026-10-19, Claude Haiku 5.5, n9-q-window; reviewed by Claude Opus 5.5)

WindowEntry (0x48 bytes, windowdrv.h) gets `state` (u16 at 0x0, the msgdrv window state machine switched on by
msgWindow_Main, msgWindow_Clear_Main and selectWindow_Main; values 1-11), `priority` (s16 at 0x4, set by
windowEntry's parameter, drawn at dispEntry depth 400 - priority), `stateTime` (OSTime at 0x20, stamped on fade
state changes and re-stamped each frame of the history scroll; header now includes dolphin/os.h) and `dispFlags`
(s16 at 0x2C, set by _evt_msg_print and passed to windowDispGX_Message: 0x1 tail, 0x4/0x8 tail placement;
msgWindow_Disp also tests 0x10 for an anchor already in screen space). evt_msg.c's `WindowVec` overlay is gone in
favour of `window->anchor`, and `type == 1` uses `WINDOW_TYPE_SMALL`. Never-read bytes are `pad_0x1C`, `pad_0x2E`,
`pad_0x44`. WindowEntry field_0x uses 77 → 0. Open: `userdata` still carries its "verify?" TODO, and
windowDispGX_Message's `a2` parameter keeps its raw name (renaming the windowdrv.c drawing code is a separate pass).

### Readability: BattleWork names (2026-10-19, Claude Haiku 5.5, n9-q-bwork; reviewed by Claude Opus 5.5)

The earlier backlog note that BattleWork's `actionCommands` has the wrong size is false: BattleActions is 0x30C
bytes (0x1C90-0x1F9C), and `field_1F9C` pads exactly to the stylish fields at 0x271C. The accesses at 0x1CC8-0x1D14
land in BattleActions' own padding, which now has names: `acFlag` (0x34, BattleWork 0x1CC4, btlevtcmd_AcSetFlag),
`acParams[8]` (0x38, btlevtcmd_AcSetParam/AcSetParamAll/AcGetParam), `acGaugeParams[4]` (0x78,
btlevtcmd_AcSetGaugeParam) and `guardFrames` (s16 at 0x88, counted down by ac_button_down/ac_stick_rotate).
0x2744/0x2748 lie past it, in the status-window block. BattleWork gets `unitIdTable[64]` (0x120),
`phaseEvtTable[64]` (0x220, `PhaseEvtEntry` moved from battle_seq.c), `sampling` (0x428, `BattleSampling`/
`BattleSamplingEntry` moved from battle_detect.c into battle_detect.h; battle_seq_command.c's partial
`BattleTargetSampling` copy is gone), `stockExp` (0xF00), the status-window hide/show flags `statusWinScriptOff`
(0x273C, nok `_status_window_off`), `statusWinLectureOff` (0x2740, gor `_set_lecture_mode`), `statusWinSystem`,
`statusWinEvent`, `statusWinAPRecovery` (0x2744-0x274C), `afterReaction[64]` (0x16F40) and `stageFlags` (u8 at
0x180DC: 0x2/0x4/0x8 from the stage layer switches, 0x20 for the ceiling). battle.c's `BattleWorkOverlay`/`BW_OVL`
is gone. `BATTLE_MAJINAI_END` (0x400000) joins BattleFlags. Raw BattleWork casts 1548 → 1474. Open: flags
0x40/0x100/0x40000000, `field_EFC`, `partyChangeId` at 0x1C68 (inside commandMenu, read raw in BattleChangeParty),
and the byte-stepped `u8* sampling` reads in the barriern/battle_event_cmd helpers.
### Readability: evt_shop and win_item (2026-10-19, Claude Haiku 5.5, n9-q-shop; reviewed by Claude Opus 5.5)

evt_shop's file-local ShopWork (0xD8 bytes) gets `objs` (ShopObj* at 0x4, replacing the `(ShopObj*)` and
`*(const char**)(... + i * 8)` casts), `shopper` (0xC, a new file-local `ShopperData`: `name`, `fookEvt`, `buyEvt`,
read by shopper_name, get_fook_evt and get_buy_evt), `itemDbOverrides` (0x10, the id / sell price table that
item_data_db_arrange applies and item_data_db_restore undoes), `acquired[6]` (0x14, bit 0 once held or bought),
`numItems`, `mainEvt`/`buyEvt` (0x24/0x28), `shownPoints` (0x38, point_disp's animated counter),
`nameWin`/`helpWin`/`pointsWin` (0x50-0x58, the three window_desc entries), `openIdx` (0xD0, -1 when no item's
windows are open) and `buyItemId` (0xD4). ItemDbEntry's halves are `altSellPrice`/`savedSellPrice`, and the
redundant `*(u16*)&...sell_price` casts are gone (sell_price is already u16). Item literals use ITEM_CONTACT_LENS,
ITEM_SUPER_LUIGI1-5 and, in win_item, ITEM_MAILBOX_SP. win_item's 0x21C/0x220 accesses use the existing
`WindowWork.itemScrollRow[2]`. evt_shop 314 -> 211 and win_item 304 -> 284 per kLOC; overall 69.987 -> 69.812.
Open: ShopWork `field_0x8` (item id / price pairs, also read through a byte offset in point_disp's price loop),
`field_0x2C` (selected index) and the never-read `field_0x30`/`field_0x34`; win_item's `WW_*` overlay offsets
0x20C-0x2E8 (the rest of WindowWork's item-window tail, which needs a layout shared with the other win_* files
before naming); PouchData `field_0x5B4` (per-slot shop point reward bits, set in evt_shop but a header field outside
this pass).
### Readability: audience members for kinopio (2026-10-19, Claude Haiku 5.5, n9-q-kino; reviewed by Claude Opus 5.5)

BattleAudienceMember's 0x54-0x8F block becomes five Vecs: `scale` (the drawn scale, `baseSize` times the texture
size in BattleAudienceDispAudience), `baseSize` (reset to 1; kinopio squashes and stretches x/y, the Magnum
Battender Mk II suck-in shrinks it), `drawOffset` (added to the quad's top corners; kinopio sways x and lifts y),
`velocity` and `accel` (kinopio's per-frame integration). 0x12C is a union because it really is reused per type and
status: `timer` (the BULKY_BOBOMB fuse and the KOOPA shell countdown, both set to 5 in battle_audience.c; the shell's
last case clears it, matching the KOOPA countdown that drives it), `targetId` (the member a Pakkun eats), `evtId`
(polled with evtCheckID after the fall-object and nozzle checks) and the f32 `bobAngle` (AUDIENCE_BOO); 0x130 is
`bobOffsetY`, 5·sin(bobAngle), added to the quad's y. The renames reach battle_audience.c, the mkII rel and one
`#ifdef MUST_MATCH` block in BattleAudienceCtrlProcessKinopioFire (identifiers only). In review, 32 more
velocity/accel sites written as `*(f32*)(member->field_0x8 + 0x70..0x80)` (in the intrude cases) moved to the
fields too.
Left: kinopio's `field_0x4` (the per-status step counter), `field_0x8` (per-status scratch: an s32 counter, f32s at
+4/+8/+0xC, a u8 at +0x10) and `field_0x30` (an s32 frame counter), `field_0xE8` (facing, set to 90/270 from the sign of velocity.x; other
callers not checked), and the `*(f32*)(member->field_0xC0 + n)` reads, which run past the 12-byte pad into
`jumpOffset` (+0xC..+0x14), `jumpVelY` (+0x1C) and `jumpAccelY` (+0x24) and want rewriting as those fields with a
build check. battle_audience.c's raw `(u8*)member + 0x54/0x60/0x6C/0x78/0x84` resets and its 0x54 read in the disp
code weren't forced by this rename and stay as they were.
### Readability: map object names (2026-10-19, Claude Haiku 5.5, n9-q-mobj; reviewed by Claude Opus 5.5)

MapObjectEntry (mobjdrv.h) gets `velocity` (Vec at 0x5C; mobjMain adds velocity.y to position.y), `gravity` and
`bounce` (0x68/0x6C, evt_mobj_set_gravity_bound's two args; landing scales velocity.y by -0.5 * bounce),
`msgWindowId` (0x1A8), `timer` (0x1B0, the per-frame countdown of the timed types), `childName[0x10]` (0x1B8,
strcpy'd name of the object this one spawns), `effect` (0x1C8), `update` (0x1D0, per-type callback run by
mobjMain), `eventCode` (0x1D4, the script mobjRunEvent starts), `state` (0x1DC, per-type step; 0x63 is done, which
mobjCheckItemboxOpen tests), `triggerFlag` (0x1E0, the entry->flags bit that last hit Mario), `flagVar` (0x1E4, the
script variable set on activation) and `paperTex` (0x208, the smartEntry GXCopyTex fills). Users in mobjdrv.c,
evt_mobj.c, eki_06 and las_18 follow, the raw `(u8*)obj + 0x1A8/0x1AC/0x1B0/0x1B8/0x1C8/0x19C` casts are field
accesses, and evt_mobj's `unk1D4`/`unk1E4` locals are now `eventCode`/`flagVar`. Three float-block functions need
`&obj->childName[0]` rather than `obj->childName` to stay byte-identical. evt_mobj.c 438 → 356 weighted. Left
unnamed because they change role per type: 0x1A0 (item id), 0x1A4 (coin count, fire bar part count, or switch anim
index with state bits), 0x1AC (jump stand launch height for marioChgJumpStandMotion vs fire bar Z angle in
degrees), 0x1B4 (timer reload value vs fire bar sound handle) and 0x1D8 (a second event script).
### Readability: itemdrv names (2026-10-19, Claude Haiku 5.5, n9-q-item; reviewed by Claude Opus 5.5)

ItemEntry (itemdrv.h) gets `spawnType` (0x2, itemEntry's a3; its switch picks the starting mode), `getScript`
(0x20, run by itemseq_tryGet on pickup), `subMode` (0x26), the WinMgr ids `winMainId`/`winHelpId`/`winFullId`
(0x28-0x30), `status` (u32 at 0x38, replacing the ITEM_STATUS and IT_* offset macros), the bounce inputs
`moveSpeed`/`moveDir`/`jumpPower` (0x50-0x58), the timers `bounceTime`/`lastTime`/`curTime` (0x60-0x70),
`lifeTime` (s64 at 0x78, ms left, blinks at or below 3000; the `0x7C = v; 0x78 = 0` store pairs are now
`lifeTime = v`) and `boundRate`/`gravity`/`boundLimit`/`boundCount`/`boundNextDir`/`boundNextSpeed` (0x80-0x94,
set by the evt_item_set_* user funcs). ItemWork gets `getEffect`/`getSelect`/`getEvent`/`getMsgId` (0xC-0x18,
itemseq_GetItem's per-pickup state). Literals use the item, badge-range, MARIO_MOTION_* and CAMERA_* enums. The
renames reach evt_item.c, evt_mobj.c and las_04.c. itemdrv 386 → 77 weighted. The worker's new MUST_MATCH guard
in itemMain (`*(u64*)&entry->lifeTime == 0`) is gone: writing the decrement as `entry->lifeTime -= elapsed` (same
semantics as the explicit `(s64)((f32)lifeTime - elapsed)`) stops MWCC reusing the compare's loaded value, so the
plain s64 compare matches. Left: itemBoundHitFloor's existing volatile 0x38 guard, the lecture event's 0xBC/0xC0
and the get-effect's userdata 0x38 stores, the never-read pads at 0x34/0x4D/0x5C, and unnamed mode, subMode and
status values.
### Readability: eff_kemuri and eff_starstone (2026-10-19, Claude Haiku 5.5, n9-q-eff; reviewed by Claude Opus 5.5)

effKemuriData (eff_kemuri.c) gets `kind` (int2: picks the scale table and texture), `x`/`y`/`z`, `frame` (indexes
touchdown_scale/heavy_scale), `scaleX`/`scaleY`, `velX` (damped by 0.85) and `offX`, `scale`, `sheetU0`/`sheetU1`
(texture-matrix column of this frame and the next, 16 over the lifetime) and `lifetime` (32 or 64 frames).
effKemuri2Data gets `speed`, `velY`, `dirX`/`dirZ` (sin/cos of the drift angle), `spinVel`/`spin`, `pulseScale`,
`scale` and `heading` (camera-relative yaw that picks the facing quad). effStarStoneData gets the fields only
eff_starstone.c names: `spreadAX`/`spreadAZ`/`spreadBX`/`spreadBZ` (0x48-0x54, the two tip pairs swung by camera
yaw once state passes 2), `tintR`/`tintG`/`tintB` (0x5C-0x5E, TEVREG0 colour) and `sfxHandle` (0x60). GX literals
in both files use their enums (GXSetTevColor's 1 is `GX_TEVREG0`, GXSetTevOp's 4 `GX_PASSCLR`, 0 `GX_MODULATE`).
Weighted cleanup: eff_kemuri.c 505 → 385, eff_starstone.c 441 → 398.
Open: effStarStoneData's other fields. sac_suki, sac_zubastar and sac_genki write 0x8-0x20, 0x38 and 0x5F through
the struct; win_log, itemdrv, sac_muki and sac_bakugame poke 0x4, 0x8-0x20, 0x34, 0x38 and 0x5C-0x5F by raw offset;
sac_deka has its own DekaStarEffectData overlay. Naming them is a pass over those files too. Candidate names:
0x4 `flags` (1 falling, 2 no dust/shadow for idx >= 7, 4/8 shine sound wanted/playing, 0x10, 0x20 skip draw),
0x14 `scale`, 0x18/0x1C/0x20 `rotY2`/`yaw`/`rotZ` (y, z, y rotations in degrees; the callers' `scale2` source
fields are misnamed), 0x24/0x28 `scrollU`/`scrollV` (texture-matrix translation, U cycling by 0.01), 0x2C `timer`
(mod 60; a sparkle every 10), 0x30 `edgeColor` (from edge_color[], the channel material colour), 0x34
`dustEffect` (effSnowDustN64Entry), 0x38 `camId`, 0x3C `state`, 0x40 `gravity`, 0x44 `velY`, 0x5F `alpha`.
### Readability: shadowdrv names (2026-10-19, Claude Haiku 5.5, n9-q-shadow; reviewed by Claude Opus 5.5)

The char, proj and depth shadow work blocks (shadowdrv.h) share one named layout up to 0x100: `flags` (0x1 texture
valid, 0x2 allocate the texture buffer at draw time, 0x4 half-resolution copy), `drawCount`, `configIdx`, `lookAt`,
`projection` (Mtx44 at 0x3C), `projTexMtx`, `depthMtx`, `texObj`, `texBuffer`, `color`; CharShadowWork adds
`entryCount`, `projectionEnabled`, `defaultKind`, `defaultYScale`, `defaultCollision`. ShadowEntry gets `kind` (0
polygon, 1 texture, 2 cylinder, 3 projected-texture caster), `yScale`, `offscreenId`, and `camId` becomes s16 (the
`*(s16*)&entry->camId` casts are gone). ShadowConfigFields moves to the header with `followCamera`,
`halfWidth`/`halfHeight`, `nearZ`/`farZ`, `isOrtho`, `depthMode`, `sizeIdx`, `depthBias`; the config getters
return it, so mapdrv's two `*(s32*)((u8*)config + 0x3C)` reads are `config->depthMode`. GX literals are GXEnum.h
names throughout (the MUST_MATCH colour helpers included), `camId == 1` is `CAMERA_OFFSCREEN2` and new entries
default to `CAMERA_3D`. shadowDispProjMtx is now under `#ifdef MUST_MATCH` because the plain-C #else reads
`w->projection` directly and no longer needs it. Open: the SHADOW_MAT_COLOR/SHADOW_FOG_COLOR fn-cast macro bodies
keep their literals, and the MUST_MATCH double spelling of the projection matrix stays.
### Readability: action commands (2026-10-19, Claude Haiku 5.5, n9-q-sac; reviewed by Claude Opus 5.5)

The star-stone action commands sac_bakugame, sac_scissor, sac_zubastar and sac_genki get their own work structs
named. BakuGameWork (sac_bakugame, 0x2B8 bytes): `phase`/`phaseTimer` and `stage`/`counter`/`duration` (the outer
and inner bakuGameMain switches), the time gauge `gauge`/`gaugeBoost`/`gaugeDecay`/`gaugeFill`, `countdown`,
`cooldown`, `intensity` (camera shake and animPoseDrawMtx), `team`/`selTeam`, Mario's drop (`fromPos`, `landPos`,
`fallSpeed`), the heiho's `heihoState`/`heihoPos`/`heihoFrom`/`heihoTo`/`heihoVelY`, the star stone's `starPos`/
`starFrom`/`starTo`/`starScale`/`starRot`, the per-team jump velocities and the two SFX handles; BakuGameWorkEntry
gets `state`, `frame`, `meter` and `iconScale`. The BattleWork bytes at 0x1F2C are `BakuGameGaugeColor`
(`r`/`g`/`b`/`a` plus `dg`/`db`): BattleAcDrawGauge multiplies its bar colour by them when flag 1 is set, and
bakuGameDisp2D sweeps g and b between 0x80 and 0xF5 once the gauge is full. ScissorWork: stick/cursor state
(`prevStickX`, `stickX`, `cursorX`, `cursorVelX`, `lastPointX`), `pointBuf`/`pointCount`, `triBuf`/`triCount`,
`hitUnitIds`/`hitPower`, `comboCount`, `audienceVol`, the timer (`timerPhase`, `timerFrame`, `timeLeft`,
`timerPos`), `captureBuf` and the star-stone Vecs. ZubaStarWork: `phase`/`sequence`/`gaugePhase` with their timers,
`timeLeft`, `hitCount`, `cursor`/`cursorHistory`, the slash `cutStart`/`cutEnd`/`cutFrame`, `curStar`, the gauge
fields and `gaugeSfx`; ZubaStarBomb: `pos`, `center`, `from`/`to`, `angle`/`angleVel`, `velY`. All four files read
the star stone's child effect as `effStarStoneData::field_0x34` instead of `userdata + 0x34`, and sac_genki indexes
`&genki->objects[idx]`. Weighted density: bakugame 787 → 94, scissor 383 → 91, zubastar 344 → 80, genki 331 → 326.
Left: sac_genki's address-named float constants (`float_0_80421d84` and friends) stay, since they are the .sdata2
pool the MUST_MATCH paths reference by address; effStarStoneData's own fields and the first two BattleAcDrawGauge
arguments (BakuGameWork 0xDC/0xE0) belong to eff_starstone and battle_ac and are named there, not per caller;
GenkiWork's `field_44`/`field_48`/`field_60` effect pointers and BakuGameGaugeColor's never-touched 0x4 byte are
still unnamed.

### Readability: PartyEntry per-partner tail (2026-10-19, Claude Haiku 5.5, n9-q-ptail; reviewed by Claude Opus 5.5)

PartyEntry's 0x174-0x187 tail is now `union { ... } tail;` with one struct per user: `jump` (`jumpAccel`, the
generic jump in partyRideMain and evt_party_jump_pos), `yoshi` (`jumpVel`, `jumpAccel`), `bobbery` (`motionTimer`,
`marioMotion` and `syncFrame` for the mario_controll states, `countdownSfxId` 0x93C, `alarmSfxId` 0x93D),
`flurrie` (`breathFx`, `bobAngle`, `bobStep`), `koops` (`heldItem` from the shell search, `slideDecay`, `dashing`,
`moveFlags`) and `vivian` (`charge`, also read by party.c for her shadow size, `moveSpeed`, `poseTime`, `ripple`).
Outside the tail: the input snapshot at 0x150-0x15D (`button`, `buttonNew`, `buttonRepeat`, `buttonUp`, `stickX/Y`,
`substickX/Y`, `triggerLeft/Right`, copied from GlobalWork by partyUpdateKeyData), `dispOffset` (0x7C, added to
position for the draw matrix, bobbed by Flurrie and Vivian, set by p_roll), `moveDelta` (0xAC,
evt_party_set_hosei_xyz) and `groundSlope` (0xE0, partyGroundSlope). Yoshi's `P_F`/`P_S`/`P_U16` shims are gone.
party_yoshi.c 316 → 232 per kLOC, party_bomhei.c 288 → 58; overall 69.987 → 69.262. Open: partyUpdateKeyData,
the draw matrix and the 0xAC/0x7C zeroing in party.c/evt_party.c still use raw offsets; the s32 zero-stores to
float tail slots in cloud_init and the nokonoko inits stay raw (`stw` of r0, not `stfs`); 0x88 is still padding.
### Readability: mapdrv (2026-10-19, Claude Haiku 5.5, n9-q-mapdrv; reviewed by Claude Opus 5.5)

MapObject (mapdrv.h) gets `flags` (0x0), `renderMode` (0x4, the dispEntry render mode _mapEnt picks from the
joint's drawMode; 2 when mapTestXLU says translucent), `cameraId` (0x5, the dispEntry camera, CAMERA_3D from
_mapEnt; the lecture rels' lect_map_cam_chg_sub retarget it), `color` (0xC, a colour scale with flag 0x40 and the
TEV flush colour with 0x100000, so it is not only the flush colour), `center` (0x10), `localMtx` (0x4C, trans * rot
* scale), `animLocalMtx` (0x7C, localMtx with the matrix animation applied by mapCalcAnimMatrix in mapMain),
`offScreenId`/`entryIdx` (0xDC/0xDE) and `dlBuf[8]`/`dlSize[8]` (0xF4/0x114, makeDisplayList). The flag bits
with a visible effect are the MapObjectFlags enum (HIDDEN, CLIP, GROUP, IN_GROUP, COLOR, OFFSCREEN, DISPLAY_LIST,
DIRTY, NO_BLEND, NO_BLEND1, ANIM_MTX, NO_DISP, NO_FOG, FLUSH, USER_MTX, BBOX, TEV_CALLBACK_MASK) and are used by
name in mapdrv.c; 0x4, 0x8, 0x8000, 0x400000 and the mapSetMaterialTev-only bits stay hex. MapEntry gets
`blend[3]` (0x16, scales for entry flags 0x2/0x4/0x8), `chunkBuf`/`chunkBufSize` (0x8C), `tevCallback[16]`
(0x110), `dlTotal`/`dlData` (0x168); the old `field_0x8C + 8/0xC/0x18` reads are the existing `textureTable`,
`lightTable` and `vcdTable`. The TEX-format read in mapSetPolygonVtxDesc is vcdTable (0xA4, the "vcd_table"
chunk, the same slot its POS path and hitdrv's HIT_VTX_FRAC read); its local is now `vcdFmt`. MapEntryAnimData
reads use `flags`/`frame`/`rate`/`data`. The renames reach envdrv, hitdrv, battle_stage_object, eff_majinai, eki_03,
gor_00, las_08, rsh and the seven evt_lect files (pure renames; evt_lect_tik's unused MAPOBJ_FLAG macro is gone).
mapdrv.c 457 → 233 weighted. Open: MapEntry's 0x170/0x174/0x178 ARAM copies of dat/chunk/tpl are still byte
offsets, and 0x178 runs past the 0x178-byte entry into the next entry (or MapWork.field_2F0); mapMain and
mapCalcAnimMatrix still step the anim cursor as a `u8*` with byte offsets; mapSetTevCallback keeps its
`mapWork + activeGroup * 0x2F4 + 0x110` byte store because an indexed `tevCallback[idx]` store changes codegen;
MapObject.unkAC (the mapObjRotate/Translate/Scale matrix, read in hitdrv, evt_door, evt_light, aji_13, dou_11) and
the GXReadBoundingBox shorts at 0xEC stay unnamed.
### Readability: NpcBattleInfo (2026-10-19, Claude Haiku 5.5, n9-q-npcbi; reviewed by Claude Opus 5.5)

NpcBattleInfo's head and tail get names: `stageData` (0x0, the selected loadout's BattleStageData; six
battle_stage_object casts and seq_battle's `target + 0x230` read become `->stageData`), `flags` (0x4), `fixDropItem`
(0x8, an item the npc_event dead event always drops when nonzero, read by evt_npc_get_drop_fixitem),
`audienceMode` (0xAC), `audienceWeights[16]` (0xB0, s8, rolled from the setup's min/max pairs; battle_audience
reads them as s8 weights for kinds 0-11), `music` (0xC0, the BGM seq_battleInit starts) and `battleRules[3]` (0xC4).
The tail split that the npcdrv entry above lists as blocked now matches. The rules are the Glitz Pit match
conditions: tou_03 passes GSW 502-504 to evt_npc_set_battle_rule, and battle_init (seq_battle.c) copies the three bytes to
FieldBattleInfo+0x18/+0x1A/+0x1B, where `_rule_disp` uses the first as the `_rule_msg_table_*` index and prints
the second. BattleSetupData gets `mapName`, `fixDropItem`, `flags`, `audienceMode` and `audienceRanges[16][2]`. The
memsets use sizeof. The new `NpcBattleFlags` enum names the bits the code proves: 0x1/0x2/0x4/0x8 zero the coin,
heart, flower and item drops; 0x10 disables Run Away (btlseqFirstAct also runs the wall-close check on it); 0x40
turns a loss into a return to the field at 1 HP instead of seqSetSeq(5) (game over); 0x100 adds 1 to the coin
multiplier; 0x01000000/0x02000000/0x04000000 disable Jump, Hammer and partner change (set by jon's trap events);
and 0x10000000 stops Bump Attack and First Attack from winning outright.
Open: `BattleAudienceSettingAudience` keeps its raw `setupBase` pointer (now written as `+ offsetof(NpcBattleInfo,
audienceMode)`); reading `setup->audienceMode`/`audienceWeights` directly drops it to 99.83%. Flag bits 0x20 (a
Merlee curse table without curse type 4), 0x100000 (btlseqFirstAct skips Mario-kind units) and 0x20000000 (Mario's
end-of-battle voice) stay hex. `battleRules[2]` (copied to FieldBattleInfo+0x1B) has no reader found yet;
FieldBattleInfo's `field_0x14` bytes want names to match. `field_9C` (0x9C-0xAC) and `field_C7` are unnamed.
npcRollBattleParams still reads the setup's pairs through a byte pointer instead of `audienceRanges`.
battle_seq_end's `seqSetSeq(SEQ_MOVE, ...)` passes battle enum SEQ_MOVE (5) where SequenceType SEQ_GAME_OVER is meant.
### Readability: REL group 2 — Shadow Queen, atmic teresa, gor_00, end credits (2026-10-19, Claude Haiku 5.5, n9-q-rels2; reviewed by Claude Opus 5.5)

unit_boss_black_peach.c: HandElem gets `mirror`, `poseIdx`, `startTime`, `stagger` (random 0..20 subtracted from
phase·speed), `wavePhase` (wraps at 0x870 in the 1000-hand wave), `speed` and `posX/Y/Z` (the pose matrix
translation); HandPose gets `duration` (frames), `firstFrame` and `frameCount` (the run of same-name hand_data
rows); HandWork gets `phase`, `poseCount`, `frameTimer` (a 30-frame countdown hold in the drag events, a count up
to 200 in queen_hand_small_1000hand_1_evt) and `state`. ShadowQueenData names the three part pose handles and the
two "locator12" group indices. The queen's thunder/shock setup writes go through EffShockData and
EffPokopiThunderData; in review those shared fields were named from the effects' own Disp code: shock 0x30-0x44 are
`tev1R/G/B` and `tev2R/G/B` (GXSetTevColor 1 and 2), pokopi thunder 0x30-0x38 `tev1R/G/B`, 0x3C `tev1A` (halved as
the bolt ages) and 0x40-0x48 `tev2R/G/B`, renamed across both effect files, las_29 and the queen.
unit_atmic_teresa.c: TeresaEntry gets `animMode` (0 loops frames 0-7, 1 plays 8-15 once), `timer` (the animation
clock), `delay`, `counter`, `velocity`, `startPos` (one Vec replacing three floats, byte-identical), `targetPos` and
`easeMode` (the intplGetValue mode).
gor_00.h: NanNpcEntry now mirrors src/event/evt_nannpc.c's layout (name, pose, flags at 0x0-0x17 instead of a
leading pad; width, animTable etc.) with gor's names where its sequences use a slot: `basePos` (nannpc's
homePosition), `startTime`, `facingAngle`, `state`, `timer`, `index`, `stepX`, `targetPos`, `moveDone` and
`endFlag` (nannpc work[0]/work[1]). 0x84 is `field_0x84`: gor never touches it, but it lies in nannpc's per-entry
scratch, so it isn't padding.
end_unknown.c: CreditsWork's 0x08/0x0C/0x10 are alphas (setChanMatColorGX/setTevColorGX take them as the u8 alpha
of a fixed colour), so they are `matAlpha`, `overlayAlpha` (the overlay pass runs when > 0) and `tevAlpha`.
`image` is the FileEntry* fileAllocf returns, and the palette reads are `*(TEXPalette**)WORK->image->data`
rather than a local CreditsImage struct. The record word at 0x04 is `layout` (placement 0-4, copied into
CreditsWork and CreditsPoseEntry and into GW(12)); `texId` (0x08) runs 0..49; 0x00 is 0 in all 50 records and
unread. `_mapAlloc(sizeof(CreditsPoseEntry))` is 0x44 as before.
Index 68.554 (origin/main) → 68.534.
Open: NanNpcEntry 0x70/0x78/0x7C stay field_ names because each sequence uses them differently (step total vs next
state; movePos step/angle vs jump-arc terms vs sway bounds vs suifu speed), and `timer` is ms in some states and a
step count in seq00's walk. end_unknown's functions and data still carry address names (unk_end_*, dat_end_*,
unk_end_0000321c is still a u32 array rather than CreditsEndingRecord[50]). Swapping a literal for sizeof() is free
for a constant like the 0x44 alloc, but where the size multiplies an index MWCC can switch to unsigned-multiply
codegen (size_t); build-check every such swap.
### Readability: koopa (2026-10-19, Claude Haiku 5.5, n9-q-koopa; reviewed by Claude Opus 5.5)

KpaWork gets `hitFlame` (0x18, kpaGetHitFlame / kpaFireMain), `hitobj` (0x1C, kpaGetHitobj / kpaHitCheck),
`moveRate` (0xD0, kpa_walk/kpa_dash from `_rateTbl`/`_rateTbl2`), `attackTimer` (0x140, counted down per attack
step), `bubbleTimer` (0x150, kpa_swim) and typed placeholders `field_0x10`/`field_0x14` (f32, written only by
kpa_walk) and `field_0x144` (read only by attack mode 0x17). KpaFireNode gets `radius`, `noHit`, `yaw` and
`nextEfp`. New `KpaFlags`: `KPA_FLAG_NO_HIP` (0x10, set on kpa_swim entry, blocks kpaHipAttackOk),
`KPA_FLAG_SPLASH_PENDING` (0x100, kpa_fall over water / swim surface jump, cleared when kpa_swim plays SFX 0x827),
`KPA_FLAG_MUTEKI` (0x10000) and `KPA_FLAG_ALT_POSE` (0x01000000, picks the KPA_*_2 stay/walk pose); `KPA_SFX_NONE`
for `sfxId`. Bowser runs through `marioMotTbl`'s own slots (mot_jump/mot_hip/... call the kpa_* functions; 0x25-0x27
are kpa_swim/powup/powdown), so `MARIO_MOTION_*` is the right enum for every marioChgMot and currMotionId compare,
including JUMP_3 (8) and the PLANE/DAMAGE/DAMAGE_TOGE early return, which mirror the same checks in mario.c.
`riderDelta` replaces the raw MarioWork 0x174-0x17C casts. Index: koopa files slightly up (placeholders now count
as field_0x), overall 69.701 -> 69.71. Open: flag 0x10000000 (set by kpaPowDown at level 0, no reader found) and
who clears KPA_FLAG_NO_HIP; writers of `field_0x10`/`0x14`/`0x144`; the older `unkD8`/`unkE0` header names, which
escape the index; `(f32)MW_U16(mario, 0x28)` in kpa_walk/kpa_dash stays raw because `(f32)(u16)mario->poseTime` changes the
codegen; the MarioWork 0x7C-0x88 jump parameters, 0x110, 0x120, 0x1AC, 0x214-0x220, 0x246/0x248, 0x2A0/0x2A2 and
0x2C4 are still unnamed on main.
### Readability: BattleUnit leftovers and SE mode (2026-10-19, Claude Haiku 5.5, n9-q-btlunit2; reviewed by Claude Opus 5.5)

`BtlUnitSeMode` and `BtlUnitSeModeSrc` move from battle_unit.c into battle_unit.h and replace the pads at
BattleUnit 0x188/0x1C0 and BattleUnitPart 0xC0/0xF8: the same two structs at both places (BtlUnit_LoadSeMode and
_MoveSoundControlCore take unit+0x1C0/0x188 or part+0xF8/0xC0, and the 0x1C gap between them holds the unit's hop
fields in both). SeMode: `sfxA`/`sfxB` (sfxA alone loops at the unit's position with its handle in `sfxHandle`;
with sfxB set they alternate, odd `beat`s playing sfxA), `delay` (frames before the first beat, copied into
`countdown`), `intervalA`/`intervalB` (frames after each), `param8`/`paramA` (written by BtlUnit_SetSeMode, never
read). SeModeSrc holds the four presets LoadSeMode's `mode` picks; mode 2 is the two sound names only
(btlevtcmd_SetRunSound writes them). BattleUnit gets `seqPhase` (0x2C, compared with SEQ_PHASE),
`mMoveColorCur` (0x30E, eases toward mMoveColorLv), `mExtraWork`/`mExtraWorkFree`/`mSmartEntry` (0x310-0x318,
freed by BtlUnit_Delete); BattleUnitPart gets `detectOffsetX` (0x1A0: only an x offset, added to the sampling
entry's x in _btlSamplingEnemy and set by btl_hitoffsetx), `motionSpeed` (0x20C), `dispCallback` (0x210),
`statusPoseStep`/`statusPoseKind` (0x214/0x215: statusPoseControl switches on the kind, 2/3/7 one-pattern and
4/5/6 two-pattern overlays, and the _pose_* functions step 0x214 from 0 to 1 or 2) and `evtColor` (0x218).
Literals use UNIT_MARIO, UNIT_SHELL_SHIELD, UNIT_VIVIAN and TYPE_PARTNER_MIN/MAX (0xE0 inclusive to 0xE7
exclusive, as the old `>= 0xE0 && < 0xE7`; the MAX macro is now parenthesized), FlowerSaver is
`badgesEquipped.FlowerSaver` (0x2DC + 0x15), and the unit-id loops use `wp->unitIdTable[]`. battle.c,
battle_disp.c, battle_detect.c, battle_event_cmd.c and unit_boss_batten_leader.c change by pure renames. In
review, the worker's `effectStopped`/`statusPoseState` became `statusPoseStep`/`statusPoseKind`, and battle_unit.c's
local `BattleWeaponFields` is gone: BtlUnit_GetWeaponCost, CheckWeaponCost and PayWeaponCost read
`weapon->item`/`baseFpCost`/`baseSpCost` from BattleWeapon, byte-identical. BtlUnit_Spawn's MUST_MATCH store
keeps its `&((s32*)((u8*)wp + 0x120))[i]` form: `&wp->unitIdTable[i]` changes the addressing. battle_unit.c 302
→ 148 weighted. The pinch-status literals 0x10000000/0x20000000 stay until the BattleUnitStatus_Flags enum lands.
Open: the part pose block at 0x1C0 (anim handle at +0, flags at +0x44, bytes at +0x54/+0x55) is still
byte-indexed in battle_disp; the attack event at unit 0x29C is written both as an s32 and as a void*; and the
byte-stepped unit regions at 0x27, 0x264 and 0x2B4 stay pads. battle_event_cmd.c's LoadSeMode/SetSeMode callers
and _MoveSoundControlCore still pass and read raw offsets.
### Readability: filemgr and seq_load (2026-10-19, Claude Haiku 5.5, n9-q-filemgr; reviewed by Claude Opus 5.5)

FileEntry gets `type` (0x1) and `inlineData` (0x4, the slot `data` points at when arcOpen supplied the file);
`_fileGarbage` uses the s16 `references` field instead of a cast. A new `FileType` enum (filemgr.h) names the
relocation types: the 58 callers pass only 0, 4 and 5, which are `FILE_TYPE_RAW` (map data, a/vivian.bin,
a/ag2tg.bin, mono.dat), `FILE_TYPE_TPL` (every .tpl) and `FILE_TYPE_ANIMPOSE` (a/<name> model files, read as
AnimationPoseData); `FILE_TYPE_CANCELED` (11) is what dvdReadDoneCallBack stores when the read returns
`DVD_RESULT_CANCELED`. Super Paper Mario's filemgr numbers them the same way (GENERIC/TPL/ANIMPOSE; SPM puts
EFFDAT at 11 and moves the cancel marker to 12). Types 1-3 and 6-10 have no caller, so they stay `FILE_TYPE_<n>`
with their pointer layout in the enum comment; type 1 relocates the same 25 words as ANIMPOSE and type 2 the
same 8 words (0x24-0x40) that ANIMPOSE relocates in each animation's data, but neither is proven to be that
struct. The switch cases and every caller use the enum. The ANIMPOSE cases relocate through the struct now
(`FILE_RELOC(POSE(data)->shapes, data)` and so on, plus `mpAnims[j].data` in the animation walks), byte-identical.
That needed a header fix: AnimationPoseData's padding at 0x1A0 was 4 bytes short, so `groups` and `mpAnims` sat
at 0x1A4/0x1A8 instead of 0x1A8/0x1AC. 0x190, 0x1A0 and 0x1A4 are file pointers (`void* field_0x...`), and
animdrv.c's ANIM_POSE_DATA_GROUPS/ANIMS offset macros became plain `->groups`/`->mpAnims`.
seq_load's loadDraw reads level, star powers, GSW0 and play time through CardSaveSlot. Play time is GlobalWork's
`retraceLocalTime` (0x20): viPostCallback adds each retrace's delta to it and clamps it, the new-game init zeroes
it, win_mario prints it as `msg_menu_mario_num_playtime`, and cardmgr copies all of GlobalWork into the save. The
name is misleading; `playTime` would fit, not renamed here. LoadWork's 0x2 is `pad_0x2`: true alignment padding
between the u16 `flags` and the `file` pointer, and LoadWork is local to seq_load.c.
Weighted cleanup: filemgr.c 245 → 175, seq_load.c 242 → 231, animdrv.c 366 → 362.
Open: the raw-offset relocation lines for types 1-3 and 6-10 (no struct and no caller to name one from), the
AnimData words at 0x24-0x40 (AnimData is still an opaque `u8[0x44]`), the FileEntry state values (0 free, 1
allocated, 2 garbage, 3 async pending, as SPM's FILE_EMPTY/ALLOC_CALLED/WAITING_GARBAGE/ASYNC_CALLED), and
seq_load's seven save-flag reads `*(u16*)((u8*)card + (cursor << 14) + 0x2000)`: as `card->saves[cursor].data.flags`
the build fails (checked again for the three `wp->cursor` sites).
### Readability: rsh/eki/jin map RELs (2026-10-19, Claude Haiku 5.5, n9-q-rsh; reviewed by Claude Opus 5.5)

jin_00's `TeresaEntry` (the 200 boo boxes of senteresa_ext_main) gets `velX/Y/Z`, `angle` (degrees, computePos),
`offsetX/Y/Z`, `phase`, `spinSpeed`, `startX..Z`/`endX..Z` (senteresa_syugo's lerp), `timer`, `alpha` and `state`.
The moamoa swarm in rsh_05, rsh_06 and eki_00 (near-duplicate code) now uses one set of names in all three files:
the per-moamoa `MoamoaPose` (0x50) has `state`, `stateTimer`, `target` (0x8), `frames`, `queuedState`,
`savedX`/`savedZ`, `offset` (0x30, only rsh_06 reads it), `groupIndex` (0x3C, index into `MoamoaWork.groups`),
`poseTime` and `kind` (0x4C, index into group_tbl); the animation-group table is `MoamoaGroup` (`name`, `anim`,
`enabled`, `cycleFrames`, `firstPose`, `numPoses`) built from `MoamoaGroupDef`; `MoamoaWork` has `poses`/`poseNum`,
`targets` (the three gathering points, moamoa i heads for i % 3), `nearHits`, `hitCount`, `idleFrames`,
`groups`/`groupCount`, `angleToMario`/`distToMario` and the cloud-breath fields; rsh_05 adds `train`/`prevTrainY`
and `escapeEvt`, rsh_06 the chain fields (`chain`, `chainSlot`, `chainSlots`, `chainGroup`). In review, rsh_06's
types and arrays were renamed to match (its group-table type had been `MoamoaPose` and the entry type
`MoamoaEntry`; `MoamoaGroup` there was group_tbl's name pair and is now `MoamoaKind`), eki_00's `animId`/`groupIndex`
became `groupIndex`/`kind` (eki's `groupIndex` had meant the field rsh calls `kind`), and the 0x8 Vec went from
`position` to `target`: it is never the moamoa's position, it is moamoa_jump's landing target and a per-frame
velocity in the fly-off states. Party checks use `PARTY_MEMBER_BOBBERY`, `PARTY_MEMBER_FLURRIE` and
`PARTY_FLAG_IS_BEING_USED` (party.h). Overall 69.467 -> 68.666 per kLOC.
Open: the struct layouts still differ slightly (rsh_05 `nearHits` is `s16[3]`, rsh_06 `u16[2]` plus padding, eki
three `u16`s; rsh_06's `groups[1]` is padded out to 0x550), so a shared header needs those types reconciled and a
rebuild of all three; MoamoaPose 0x14/0x1C/0x20 change meaning per state (jump vertical speed / horizontal step /
gravity, the fly-off velocity's per-frame change, the wander radius) and stay unnamed, and `target` is itself
two-role; MoamoaWork `field_0x40` (frames in Mario motion 0x12) and `field_0x558`; eki_00's gathering points stay
three separate Vecs (`target0`-`target2`, indexed through `MOA_TARGET`) and `nearHits0`-`nearHits2`, because
writing them as `Vec[3]`/`u16[3]` changes the compiled code and breaks the checksum; TeresaEntry `phase` doubles
as state 0x15's lerp progress and `offsetX/Z` are relative to g_vecRotCenter while orbiting but to Mario once stuck.
### Readability: evt_win names (2026-10-19, Claude Haiku 5.5, n9-q-evtwin; reviewed by Claude Opus 5.5)

evt_win.c's local unit-window structs get names from their uses. UnitWinObj: `hitName` (compared with hitGetName)
and `evtScript` (run when the object is used). UnitWinWork: `items`, `taken[6]` (bit 0 = sold or empty), `mainEvt`/
`useEvt`, `hideTimer`, `dispCoin`/`dispNum` (counted toward the pouch value), `cursorRow`, `cursorX`/`cursorY`,
`listY`, `excludeId` (a partner id in the party lists, a stat row in the stat list), `mode`, `freeState`,
`rowCount`, `coinTick`, `winId[3]`, `selObj` and `useTaken`. party_id_table uses `PARTNER_*` (it indexes pouch
partyData, like winmgr.c's table), Yoshi's colour lookup passes `PARTNER_YOSHI`, and badge ranges use
`BADGE_MIN`/`BADGE_MAX`. gor_01.c's private UnitWinWork view uses the same names. evt_win.c 710 → 532 weighted.
Kept raw: `*(u32*)obj & 1` on the MapObject, the `mode` values (no enum yet; 2-18 select the list), and the
`flags` bits.
### Readability: fadedrv and battle_stage (2026-10-19, Claude Haiku 5.5, n9-q-fade; reviewed by Claude Opus 5.5)

FadeEntry (0xA8 bytes, fadedrv.h) gets `step` (0x8, the per-type state machine in fadeMain), `alpha` (0x24, 0..1),
`angle` (0x28, degrees for the texture rotation), `texMtx` (Mtx at 0x2C, built by disp_texture and loaded into
`GX_TEXMTX0`), `elapsed` (0x5C, +16.67 ms per frame), `groupName`/`animName` (0x60/0x64), `pose`/`paperPose`
(0x6C/0x70, -1 = none), `poseColor` (GXColor at 0x74, animPoseSetMaterialEvtColor), `scale` (0x90), `frameCount`
(0x94), `sfxHandle` (0x98, replaces `*(s32*)&field_0x94[4]`) and `delayFrames` (0xA4). FadeWork gets `activeEntry`
(0x350, -1 = none) and `softAmount` (0x354, fadeTecSoftOn/Off). fadeEntry's white remaps use `FADE_IN_BLACK`,
`FADE_OUT_BLACK`, `FADE_OUT_WHITE`, `FADE_IN_WHITE2` (9/10/17/18); 16, 11 and 19+ have no enum names yet.
BattleStage (0xB3C) is now the real struct in battle_stage.h (the .c's BattleStageWork, lights and BattleStageDark
moved there): BattleStageDark has two eased ramps, `alpha` (drawn by BattleStageDispDark) and `baseAlpha` (what
the reset helpers ramp back to), each with From/To/Frames/FramesTotal/Ease. The 0xB24 and 0xB30 Vecs are
`stageSize` and `audienceAreaSize`, named after btlevtcmd_GetStageSize / GetStageAudienceAreaSize, which return
them; stageSize.y is the stage object spawn height, stageSize.z the depth up to which BattleGetFloorHeight returns
0, and audienceAreaSize.y the audience floor height (battle_audience, the fall attack). BattleSpotEntry gets
`pos`/`rot`/`scale`/`color`, and ScreenWork (0x2C, `_mapAlloc(sizeof(ScreenWork))`) `texBuffer`, `captured`,
eased `scale`/`offsetX`/`offsetY` with targets, `tick` and `noiseMode`; its never-read 0x28 stays `field_0x28`.
fadedrv.c 189.6 -> 18.0 per kLOC (worker's measurement before review).
### Readability: effects, group 5 (2026-10-19, Claude Haiku 5.5, n9-q-eff5; reviewed by Claude Opus 5.5)

Work structs named in eff_snowman_n64 (`frame`, `alpha`, `rotY`, `offX/Y/Z`, `velX/Y/Z`, `hScale`/`vScale` from the
hscale tables, `gravity`, `animIdx`, `bounceCount`), eff_crystal_n64 (`timer`, `variant`, `tev0R/G/B` + `alpha` for
GX_TEVREG0, `scaleStep`, `camId`), eff_broken_barrier_n64 and eff_fire_dust_n64 (same 0x6C layout, same names:
`spreadXZ`/`spreadY`, `velX/Y/Z` eased toward `velTargetX/Y/Z`, `cycle` indexing scale_data, `rotZ`, `tev1R/G/B/A`
for GX_TEVREG1 and `tev0R/G/B` for GX_TEVREG0, all read from the base element), eff_flame_n64 (`lagX/Y/Z`,
`scaleX/Y` and `baseScale`, `scrollU/V` and `scroll2U/V` with `...Speed` eased at 0.02 toward `...SpeedTarget`;
both pairs translate GX_TEXMTX1, one per quad; `tev0Rgb`/`tev1Rgba`) and eff_fukidashi (`type`, `offset`,
`angleBase`, `height`, `spinY`, `tiltZ`, `timer`, `animFrame`, `animCell`, `number`, `duration`, `target`, `alpha`,
`camId`, `phase`; `camId` also renamed in the usu/win evt_lect copies). Entry params named for crystal, fire_dust
and flame. GX literals use their enums (0x7D is `GX_PTIDENTITY`, 0x1E/0x21 `GX_TEXMTX0/1`, GXSetChanCtrl's light
mask 1 `GX_LIGHT0`, 0 `GX_LIGHT_NULL`). Review: every swap checked by value and enum type against GXEnum.h,
including the non-MUST_MATCH `#else` branches (none of the swapped lines sits in one; flame's natural-build
GXSetTevColor pair is TEVREG0/TEVREG1 as in its MUST_MATCH helpers). Fixed in review: barrier/fire_dust 0x10/0x14
shared names (`spreadX` vs `radius`/`height`), TEV colour fields renamed after their registers (`base*`/`color*`
did not say which register they load), fukidashi `duration` comment had the two anim tables swapped, `target`
sentinels spelled out (-2 partner, -3 extra party member, NULL absolute), fukidashi 0x2C back to `field_0x2C`
(never read), flame scroll comments (both go to GX_TEXMTX1), fukidashi's remaining PNMTX literals.
Index 64.833 -> 63.884.
Open: fukidashi's entry params are still `a`..`i`; `field_0x50` (fukidashi), `field_0x64`/`field_0x68`
(barrier/fire_dust, written each frame, never read) and snowman's 0x18-0x20 have no reader. flame's `lagX/Y/Z` and
snowman's `alpha`/`vScale` are written but never read (snowman's Disp is empty). crystal's Disp branches on `type`
while Main uses `variant`; barrier and fire_dust load GX_TEVREG1 but their single TEV stage never reads C1/A1.
barrier keeps u32 colour fields where fire_dust has s32.
### Readability: battle audience (2026-10-19, Claude Haiku 5.5, n9-q-aud; reviewed by Claude Opus 5.5)

battle_audience.c's raw member casts use #872's BattleAudienceMember names (`scale`, `baseSize`, `drawOffset`,
`velocity`, `accel`, `normal`, `mRotationOffset`, `animPos`), the sound code uses BattleAudienceSound `flags` and
`timeLeft`, and the koopa shell timer names AUDIENCE_KOOPA. In review the shared fields got names from all their
users (battle_audience.c, battle_audience_kinopio.c, evt_audience.c, sac_bakugame.c, the battender rel): `step`
(0x4, the ctrl-process switch case; the koopa timer jumps it to 4), `counter` (0x8, repeats left or frames into an
intplGetValue move), `motionTimer` (0x30, the squash/sway cycle frame), `animLoop[3]` (0x34, the -6/-7 loop counts
indexed by animDepth - 1), `dispPos` (0xC0, position + jumpOffset, plus bobOffsetY for AUDIENCE_BOO; the thrown
item is placed from it) and `yaw` (0xE8, degrees; the draw code flips the sprite for (0, 180]). Kinopio's
`*(f32*)(member->field_0xC0 + n)` reads are now `dispPos`/`jumpOffset`/`jumpVelY`/`jumpAccelY`. The old
`field_0x8` array splits into `counter` and the generic `field_0xC`/`field_0x10`/`field_0x14`/`field_0x18`, since
0xC is a repeat count, a walk direction (+-1) or a walked distance depending on the status. The one `-4` store to
`field_0x18` stays `*(s8*)&member->field_0x18`: a plain u8 store loads 0xFC instead of -4. battle_audience.c
471 → 378 weighted, battle_audience_kinopio.c 413 → 103; index 68.533 → 67.941.
Left: 0xEC-0xF7 (a BattleUnit* target at 0xEC in KinopioTransEvt, an s16 member index at 0xF0 in KinopioEat, an
s32 at 0xF4 that KinopioSingSub sets to 3 and battle_audience.c counts down for status 0xD; per-status, so unnamed), the never-read pads at
0x1E/0x45/0xD8/0xE0, the past-the-end `(u8*)member + 0x134` store in Entry_Sub, the byte-stepped `animPos`
advances, `field_0xC`-`field_0x18` meanings per status, and evt_audience's 180/0 yaw against kinopio's 90/270
(both pick the flip, but the convention differs).

### Readability: effects, group 2 (2026-10-19, Claude Haiku 5.5, n9-q-eff2; reviewed by Claude Opus 5.5)

Eight effects get their own data-struct names: EffFunemizuData (`speed`, `yaw`, layer scales `scaleA`/`scaleB`/
`scaleAY`/`scaleBY`, `texScrollA`/`B`, `timer`, droplet `velY`/`gravity`/`delay`/`alpha`, `scale`, `camId`; the
muj_20, muj_21 and gor_00 `fune_eff` user funcs that fill it now use the names), effCoolLeafData, effKemuTestData,
effBreathFireData (`kind`, `childrenLeft`, `seed`, start/end/bend curve points, three-stage scale ease, two TEV
colours), effMiniGameWork (`state`, `timer`, `vel`/`accel`, `swing`), effSnowDustData (entry-vs-particle uses of
`life`/`angle`/`alpha` noted per field), effKemuri11Data and effPacknBfireData (colour 1 shares G at 0x2C). GX
literals become enums in all of them (TEV stages/args/ops, texgen, GX_TEXMTX0/1, GX_IDENTITY, GX_PTIDENTITY,
vertex attrs/formats, GX_TRIANGLES/QUADS), and `dispEntry(4, ...)`/`camGetPtr(4)` use CAMERA_3D. field_0x uses in
these files 1039 → 15; weighted target score 1983 → 825. Review checked every literal-to-enum mapping (snow_dust's
were regex-converted) against GXEnum.h and found them right; it corrected three field comments. Open:
eff_snow_dust_n64.c's effSnowDustN64Entry and effSnowDustMain sit wholly inside `#ifdef MUST_MATCH` (pre-dating
this pass), so a non-MUST_MATCH build doesn't define them and needs a portable #else; the remaining unnamed
`field_0x` slots (kemutest 0x2C/0x40, snow_dust 0x20/0x2C, kemuri11 0x18, minigame 0x2C, packn_bfire 0x3C/0x5C)
and packn_bfire's `vx`/`vy`/`vz`, which hold the target point, not a velocity.
### Readability: action commands, part 2 (2026-10-19, Claude Haiku 5.5, n9-q-sac2; reviewed by Claude Opus 5.5)

sac_common's sac_handbeat/sac_cheer write `BattleAudienceSound.timeLeft` instead of `*(s32*)((u8*)ptr + 0xC)`.
sac_deka: DekaTiming gets `kind` (0x08; 1 must be pressed in the window, anything else boos when pressed, and
kind 2 is drawn as a plane and never times out) and `offset` (0x10, added to the icon position, zeroed in state 2);
DekaWork gets `timingSlideY` (0x4C, eased from -40 to 0 over 30 frames when the timing icons appear) and
`starScaleFrom`/`starScaleTo` (0x444/0x448, the intplGetValue endpoints of the star's pulse). yuka_disp's GX loop
reads `DekaYukaMesh.disp`/`texU`/`texV` by index instead of byte offsets from `u8* base`. end_muki and end_suki
read the star stone's child effect as `effStarStoneData.field_0x34` instead of a pointer-offset cast. Open:
sac_muki's main_icon/main_star still write the star stone through `f32* data` indices and main_cursor writes
the stamp effect through `u8` offsets; both wait on real names for effStarStoneData and effStampData, since
`field_0x` member names read no better than the offsets.
### Readability: pause-menu WindowWork (2026-10-19, Claude Haiku 5.5, n9-q-win; reviewed by Claude Opus 5.5)

WindowWork (include/win/win_main.h) is one flat struct whose submenus keep their state in consecutive bands, not
overlays: root 0x1C-0x184, win_mario 0x160-0x198, win_party 0x19C-0x208, win_item 0x20C-0x3DA (+0x1208), win_badge
0x3DC-0xD64, win_log 0xD68-0x1204. No offset is read with two types by two files, so names stay flat (no union of
per-menu structs). Named: `lectureStep` (0x14), `lectureEvent` (0x18, winLectureOn's evt_lecture_msg; win_main's
old comment calling it a misplaced `currentEvent` was wrong: `currentEvent` at 0x3C is winMain's evt_item_use /
evt_diary_read event, and both offsets are real), `openTabId` (0x1C), `rootState` (0x24), `winTpl`/`mailTpl`/
`mapTpl` (0x28-0x30), `fbCopy`/`fbCopyReady` (0x34/0x38), `savedState` (0x124), the shared cursor `posX`/`posY`/
`targetX`/`targetY` (0x150-0x15C), `marioPose` (0x188) and `mapMarioPose` (0xE1C; mario_change rebuilds both),
the party block `partyPose[7]`, `partyId[7]`, `selPartyDt`, `partySel`, `partyCount` (0x1E0: partners joined,
counted by winPartyInit and read by win_root and win_item's use-target list), `partyAngle`/`partyAngleTarget`,
`lastPartySel`, `partyState`, `wazaCount`, `wazaSel`; the badge block `badgeState`, `badgeTab`, `badgeSel[2]`,
`badgeScrollRow[2]`, `badgeScrollPos[2]`, `badgeScrollTarget[2]`; `equipListCount` (0xD64) and `useTargetWin[2]`
(0x1208). WindowPartyDt: `partnerId`, `iconId`, `nameMsg`, `descMsg`, `model`, `anim`, `famicomModel`/
`famicomAnim` (used when flags & 0x2000, famicom Mario) and `waza`. win_log's LogWin overlay follows the header
(`mapTpl`, `savedState`, `targetX`/`targetY`, `mapMarioPose`). Index 69.877 -> 69.748 (worker, pre-rebase), then
69.572 -> 69.550 for the review pass. Open: moving WinRoot's tab block (0x40 tab selection, 0x44 tab count,
WinRootTab[5] at 0x48, WinRootRec[5] at 0xC0) into WindowWork broke sort_7_2_func/sort_7_3_func, which address the
tattle list at 0x1050 as `&work->field_40[0x1010]`; only that u8-array base matches, so `field_40` must keep
starting at 0x40 until another spelling is found. Still unnamed: 0x404 equipped-badge list (0xC-byte records),
win_item's 0x20C-0x3DA (its ItemWorkTail overlay and `WW_*` offsets, incl. the 0x26C mail line and 0x3DA),
win_log 0xD68-0x1204 beyond LogWin's names, win_mario 0x160-0x198 and win_root 0x164-0x184.
### Readability: Smorg and X-Naut Crump formation units (2026-10-19, Claude Haiku 5.5, n9-q-units; reviewed by Claude Opus 5.5)

unit_boss_moamoa (Smorg): MoamoaWork names `animPoses[5]` (AnimationPose handles: [0] the main unit, [1-3] the
units in unit work[3..5], [4] work[6] from moamoa2_on), `center` (a Vec: the pose-cloud centre guruguru spins
around), `spinAngle` (degrees, +60/7 per frame, wrapped at 360, converted with 0.017453292f), `spinScale` (2.0 at
guruguru start, -0.00444 per frame down to 0.8, scales the offset from `center`), `bodyScaleGoal` (moamoa_set_bodyScale)
and `bodyScale` (eases toward it by 0.1 per frame). MoamoaWorkPose: `flags` (bit 0: the pose follows the
guruguru spin or bomb scatter instead of its joint), `jitter` (a fixed +-10 random offset set at init),
`animIdx` (index into `anims`), `poseTime` (renderTime the current cycle started; reset on an animIdx change and
on each wrap) and `ownerIdPtr` (an `s32*` at `mainUnitId` or the unit work word holding the owning part's unit
id, -1 once it is gone). MoamoaWorkAnim takes rsh_05's MoamoaGroup names for the same layout: `groupName`,
`animName`, `cycleFrames`, `firstPose` and `numPoses` (an ext_moamoa_data index range). cycleFrames is 12.0f or
the data entry's frame depending on `pose->type`, where `pose` is whatever the preceding type loop left behind
(as in the original). The X-Naut Crump formation (unit_boss_gundan_zako_group1/2/3) shares one header now,
`unit_boss_gundan_zako_common.h`, with ZakogunEntry (`from`/`to` interpolation endpoints, `velX`/`velY`/`velZ`,
group3's `offsetX`/`offsetY` from the ball centre) and ZakogunWork (`phase` 0/1/2 picking zakogun_ext_main1/2/3,
group3's ball `radius`/`angle`/`speed`, `landedCount`, group2's ceiling overlay `ceilFrom`/`ceilTo`/`ceilFrame`/
`ceilFrames`/`ceilSfxHandle`, which replaces group2's WORK_* offset macros) plus the helper prototypes;
group1.h and group3.h include it, and group3's `extern s32 zakogun_get_kanbu_id_evtcmd[]` became USER_FUNC. All
three files use the same names for the same fields. Byte-identical.
Weighted cleanup: unit_boss_moamoa.c 170 → 51, zako_group1.c 95 → 3, zako_group2.c 75 → 2, zako_group3.c 88 → 1.
Open: MoamoaWorkPose 0x34 and 0x40 are multi-role (guruguru: start position and rotation axis; bomb: velocity
and position), so they stay `field_0x34[3]`/`field_0x40[3]`; a union or role-neutral names would be next.
MoamoaWorkPose/MoamoaPoseArg 0x14 is unused by name. The four `*(s32*)part->field_0x1C0` reads (the part's
AnimationPose handle) and the AnimationPose +0x15C (world-position vertex smartEntry, after
animPoseWorldPositionEvalOn) and +0x160 (its count) raw offsets wait on BattleUnitPart/AnimationPose names.
MoamoaPoseArg duplicates MoamoaWorkPose's offsets. ZakogunEntry 0x0 and ZakogunWork 0x1C-0x33 and 0x40-0x47 are
unnamed.
### Readability: mri_puni, part 2 (2026-10-19, Claude Haiku 5.5, n9-q-puni2; reviewed by Claude Opus 5.5)

PuniMap gets `yMax`/`yMin`, `width`/`height`, `dataSize`, `hasData`, `gridSrc`, `valid` and `grid` (the cells
puniMapFindbuff flood-fills from Mario's cell each frame; `valid` is 0 when Mario is off the map). PuniWork gets
`windDisabled`, and `marioPuni` is now a `PuniData*`. PuniData's named fields cover the para fight (`hp`, `maxHp`,
`hpRegen`, `regenTimer`/`regenInterval`, `attack`/`defense`/`damage`), the swarm tree (`treeId`, `depth`,
`parentDist`/`parentDistPrev`), motion and display (`prevPos`, `speed` = |pos - prevPos|, `maxSpeed`,
`dispOffset`, `rotZ`, `rotOffsetY`, `moveTarget`, `awayDir`, `hitVel`/`hitDir`/`hitNormal`, `windPower`,
`marioDist`), `statusFlags` and `animDebounce`. The review renamed what the uses contradicted: `hitChance`/
`hitChanceAlt` are contest weights, not chances (`fightWeight`: the attacker wins with odds own / (own +
defender's); `chaseWeight`: against a PUNI_MODE_ESCAPE defender, own / (own + 100 - defender's)); `engaged` is
`foughtThisFrame` (puni_puni_main clears it every frame); `nearFlag2*`/`nearFlag4*` are `nearAttract*`/
`nearRepel*`, after the target flags. `marioDist` keeps its name: it has one role, and puniAnimation only offsets
the PARENT animation phase by marioDist / 8. More fields came out of the review: `pose` (0x0, the PuniPose this
entry drives, so the `*(u32*)(*(u32**)(p + 0x0))` writes are `pose->flags`), `attackRange` (0x19C), `modeFunc`
(0x56C, the PUNI_MODE_CUSTOM handler beside `moveFunc`), `waterPos` (0x150), `counter` (0x1E4), and the
recovery block `recoverType`/`lostPos`/`recoveredPos` (0x58C..0x5A7). `parentPuni`/`leftPuni`/`rightPuni`/`target`
are `PuniData*`, which removes the `(PuniData*)`/`(s32)`/`*(PuniData**)&` casts. The header now has
`enum PuniMode` (the 31 values, read from the unk_mri_00012d58 dispatcher; 14 and 30 have no case),
PUNI_TARGET_ACTIVE/ATTRACT/REPEL (the Piders are REPEL targets, ATTRACT is where puniModeMass gathers) and
PUNI_STATUS_*, used throughout mri_puni.c. PuniSearchEntry and PuniFallTarget were local copies of PuniTarget,
and PuniFallActor was HitEntry, so all three are gone. puniModeMass reads its fields by name. In punidrv.h,
PuniPose gets `facing`, `rotZ`, `rotOffsetY`, `shadowY`, `scale`, `texId`, `billboardYaw`/`billboardYawTarget`
and PUNI_POSE_VISIBLE/EASE_TURN/NO_SHADOW/SHADOW_Y; PuniSortEntry gets `poseId`/`key`, and PuniDrvWork gets
`shadowMtx`/`fadeOff`. The PUNI_MODE_CUSTOM call keeps its zero-argument cast under MUST_MATCH, with
`modeFunc(self)` in the #else. Weighted: mri_puni.c 822 → 84, punidrv.c 56 → 14. Open: PuniData `flags` (0x4) bits
and the PuniWork `phase` values (2/3/5, read by puniModeALLCheck) are still literals; `field_0x582` (set to 10,
never read here), PuniWork `field_0x458` (an SFX handle evt_puni_force_se_off stops) and `field_0x47C`, and
PuniPose `field_0x24` stay unnamed; puniModeHagure still uses raw offsets (0x1E8, 0x220, 0x2AC..0x2B4) into
`unk21C`; mri.c and mri_03.c set `modeFunc`/`moveFunc` as `*(void**)(p + 0x56C/0x570)`; `radius` doubles as the
step length in puniModeAway and puniModeWind, which may mean a second role rather than a reuse.
### Readability: battle_menu_disp (2026-10-19, Claude Haiku 5.5, n9-q-bmenu; reviewed by Claude Opus 5.5)

battle_menu_disp.c's local BattleMenuWork (0x124 bytes, now allocated as `sizeof(BattleMenuWork)`) gets
`mainIconTable`/`weaponEntries` (the Init arguments), `phase` (ring/scroll state 0, 1, 2, 0xA, 0x14, 0x1E, 0x28),
`inputReady` (read by BattleMenuKeyOKInACT), `lastCursorPos`, `ringMoving`, the per-icon ring arrays
`iconFlags`/`iconState`/`iconFrame`/`iconFromSlot`/`iconSlot` (slot indexes into kakutable), the sub-list scroll
`scrollPos`/`scrollFrom`/`scrollTo` and per-slot `slotFlags`/`slotState`/`slotFrame`, `bounceFrame` (cursor arrow)
and `partyBtnAngle`. Entry tables get `enabled`, `label`, `iconId`, `helpMsg`, `fpCost`, and the party table's
`hp`/`maxHp` s16 pair replaces a `(s16*)entry` halves cast. The multi-item table at wp+0x1b6c has its own layout
(enabled at 0x0, label 0x4, helpMsg 0x8), so it is a separate `BattleMenuMultiEntry`. Weapon reads use BattleWeapon
`item`/`description`/`baseSpCost`; `wp + 0x1bb4/0x1bc0` is `&wp->commandMenu.cursor[0]/[1]`; `0xDE` is
`UNIT_MARIO`. Weighted score 472 → 171. Open: the four tables (wp+0x1724, 0x19e8, 0x1aac, 0x1b6c) still sit
inside BattleCommand.unk8 in the shared battle_seq_command.h, the main-icon table (stride 0x14) reuses
BattleMenuDispEntry's leading fields, and `field_0x10`, `field_0xF8`, `field_0x114` stay unnamed.
### Readability: winmgr (2026-10-19, Claude Haiku 5.5, n9-q-winmgr; reviewed by Claude Opus 5.5)

WinMgrDesc `y` (0x10, copied to entry->y; evt_window's one_msg_desc comments follow) and `disp` is now typed
`void (*)(WinMgrEntry*)`, so winMgrDisp calls it without a function-pointer cast. WinMgrSelect `firstItem` (0x38):
winMgrSelectEntry's second argument, put in front of the list for kinds 0-5, 12 and the negative-pointer id lists,
with a dotted line under it in select_disp; itemdrv passes the item just picked up when the pouch is full, event
menus pass 0, and choosing row 0 of a cancellable menu then returns -1. winMgrSelectEntry's parameters are
`kind`/`firstItem`/`canCancel` (canCancel sets select flag 0x100, which lets B cancel), winMgrSetSize's are
`x`/`y`/`width`/`height`, and select_main's A/B checks use PAD_BUTTON_A/PAD_BUTTON_B. WinMgrEntry.flags bits are
documented in the header (1 in use, 2 drawn, 4 busy for winMgrAction, 8 free after close); `field_0x40` stays: the
struct is all 4-byte members, so those 4 bytes are a real field nothing in the repo reads, not compiler padding.
GXSetFog's type is GX_FOG_NONE. Overall 69.701 -> 69.681. Open: rename unk_80237448 (select cursor getter),
unk_802379e4/unk_80237a2c (qqsort comparators by type_sort_order / star_piece_price) and unk_802399f8/unk_80239b2c
(party tech-level checks) once rename_symbol.py lands; the keyGetDir* stick bits (0x100/0x200/0x1000/0x2000) and the
psndSFXOn / icon ids have no repo names yet; WinMgrSelect `pad_2`.
### Readability: npcdrv (2026-10-19, Claude Haiku 5.5, n9-q-npcdrv; reviewed by Claude Opus 5.5)

NpcEntry's timers are named from npcMain's update block: `lastUpdateTick` (0x198, gp->renderTime at the last
update), `frameTicks` (0x190, the gap since then, 16 ms when it was over 500 ms), `moveElapsed` (0x188, ticks since
the current move began, cleared by npcdrv's interrupt dispatch and the evt_npc/evt_mobj/las_05 move events) and
`airTicks` (0x178, accumulates while flags 0x20000 is set, zeroed when npcMoveG lands); all four are OSTime.
Each has an f32 seconds copy made through OSTicksToMicroseconds / 1e6: `moveElapsedSec` (0x1A0), `airTime` (0x180,
the t in npcMoveG's gravity integral), `moveDt` (0x1A4, already named) and `lastUpdateSec` (0x1A8). Also named:
`offscreenId` (0x110, evt_npc_set_offscreen, -1 none), `savedDirection` (0x148, kept while the npc turns toward
Mario on a hit, on noticing him or for a talk, then restored), `shadowScale` (0x154, the shadow radius multiplier the
evt_npc_majo_disp_* events ease), `maxFallSpeed` (0x1D0, limitSpd; npcMoveG clamps the per-frame fall to it, half
in water), `attackMode` (0x2F8, evt_npc_set_attack_mode, copied to FbatHitInfo.attackMode), `wallHit` (0x2FC,
HitEntry* from npcHitCheckSide, the old `field_2FC != 0` wall tests), `groundHit` (0x300, HitEntry* npcMoveG's floor
probe found; the `*(u32*)field_300` reads in npcdrv, gon_11, jon and las_26 and the a_magu/mobj checks),
`setupIndex` (0x314, the npc's index in the map setup and its bit in fbat deadInfo[0].mask), `baseCoin` (0x316, the
lead unit kind's BattleUnitKind.baseCoin, now read by name) and `fbatHitKind` (0x318, the fbatHitCheck code that
started the contact, 0 for a talk; nothing reads it back). `ENC(npc, u32, 0x128)` in the talk check is the existing
`talkEvt`. NpcWork 0x10 is `talkCheckNpc`; FirstStrikeInfo gets `announceKind` (0x4, the code given to
fbatSetAttackAnnounce, the same space as FbatHitInfo.kind), `timer` (0xC, banner frames) and `bannerX`/`bannerY`.
All renames are byte-identical. Open: the two `ENC(&info, s32, 0) = 0x400000` hammer sites in npcdrv.c stay under
MUST_MATCH (writing `info.kind` in both arms lets MWCC if-convert to a select); `field_10C`, `field_170`,
`field_184`, the flag bits 0x20/0x100000/0x20000000 and the `ENC(mario, NpcEntry*, 0x1D4)` MarioWork store stay
as they are.
### Readability: statuswindow and cam_road (2026-10-19, Claude Haiku 5.5, n9-q-statcam; reviewed by Claude Opus 5.5)

StatusWindowWork (statuswindow.h) gets `flags` with a `StatusWindowFlag` enum (`STATUS_WIN_OPEN`/`CLOSED` wanted
state, `OPENING`/`CLOSING` ramps waiting to start, `FORCE_OPEN`/`FORCE_CLOSE` holds and their `FORCE_*ING` ramps,
`BADGES`, `DISP_OFF`), the open ramp (`animTime`, `animDuration`, `animSpeed`, `openAmount`, `openFrom`,
`openTarget`), the icon origin lerped between `openX/Y` and `closedX/Y`, the displayed stat copies (`hp` through
`maxAp`, 0x50-0x64), `marioPos`/`idleFrames`/`toggleCooldown`/`manualToggle` for the idle auto-open, and
`blinkMask`/`changedMask`/`blinkTimer` with a `StatusWindowBlink` enum. statuswindow.c adds timing and clamp
constants (`STATUS_WIN_IDLE_SECONDS`, `STATUS_WIN_BLINK_PERIOD`, `STATUS_WIN_MAX_COIN`...), `ICON_SP_BANNER` and
the `STATUS_DT_*` alwaysDt indices. MUST_MATCH blocks only swap identifiers for the same loads (`*(f32*)wp->field_0x14`
→ `wp->openFrom`); their `0x11`/`0xA`/`5` literals stay raw. cam_road: CamRoadState (`valid`, `onRoad`, `flags`,
the raw inputs, `road`, `dir*`, `radius`, `yaw`, `roll`, `segment`, `shiftXRate`) and CamRoadWork (`blended`,
`current`, `previous`, `hokanFrom`, the hokan blend fields, distance/side rates and weights, `cmp*W` compare
weights, `mapKind` with `CAM_ROAD_MAP_101`/`OTHER`, the `noRoad*` and `cam*` input copies) now live in cam_road.h,
so gor_02 get_camcurve reads `previous.valid`/`previous.road` and dou_05 cam_displacement writes `cmpDistW` instead
of raw offsets. The duplicate CamRoadNode overlay is folded into CamRoadCurve; CamRoadMap gets `subs`, `lists`,
`curvePts`, `verts`, `indices`; CollisionCurveData/CollisionCurveHit get segment ranges, point ranges, the hit
`x/y/z`, normal and `segment`/`flags`, and `param` (0x00, interpolated from the curve points' .y channel, clamped to
0..1 by calcCamRoad and otherwise unread). Index: statuswindow 424 → 104, cam_road 471 → 38 weighted.
Open: CamRoadMap `unk_E8`, `unk_104`, `unk_108`, CamRoadWork `unk_204`/`unk_208`, the CamRoadCurve pads.
Observations, not changed: calcCamRoad indexes `unk_104` as `CamRoadSub*` (0x68 stride) and then reads the record
as CollisionCurveData's mode-1 fields at 0x60/0x64, so the 0x68 stride is real but the record is a curve-point
range rather than a CamRoadSub; it wants its own type. statusWinMain calls `psndSFXOn((const char*)0x2A)` and
`0x2B` with integer sound ids; psndSFXOn forwards the pointer to `__psndSFXOn` as an s32, and the cast pattern
appears at about 200 sites repo-wide, so it reads as an id-or-name parameter typed too narrowly, not a statuswindow bug.
### Readability: animdrv names (2026-10-19, Claude Haiku 5.5, n9-q-anim; reviewed by Claude Opus 5.5)

AnimationPoseData's padding at 0x1A0 was 4 bytes short, so `groups` sat at 0x1A4 and `mpAnims` at 0x1A8 despite
their comments; retail has them at 0x1A8/0x1AC, which animdrv.c reached through local ANIM_POSE_DATA_GROUPS/ANIMS
macros (nothing used the misplaced fields). The gap is now `mBufferVisibility` (0x1A0) and `mBufferNode` (0x1A4),
with `mBufferTexAnim` at 0x190, so `data->groups`/`data->mpAnims` read the right words and the macros are gone; no
other offset moved. Also on AnimationPoseData: `xluFlags` (0xC4, bit n drawn at xluMode n, bit 3 animPoseTestXLU),
`radius`/`height` (0xC8/0xCC, halved sizes). AnimData gets `frameCount`, `frame2Count`, `clip`, `frames`, the five
delta tables and `frames2` (replacing the ANIM_DATA_* macros); AnimTexBind gets `palId` (0x4). AnimationPose gets
the animPoseMain frame state (`animTime`, `clipTime`, `keyFrame1/2`, `keyAlpha`, `evtFrame1/2`, `evtValue`), the
camera-facing rotation (`drawRotY`, `facingSign`, `targetRotY`), the paper block 0x94-0x11B (`paperAnim`,
`paperTexData[2]`, `paperTexObj[2]`, `paperScaleX/Y`, `paperOffsetY`, `paperTexTrans/Scale[2]`, `paperLeft/Right/
Top/Bottom`, replacing the POSE_* macros) and the world-eval buffers `groupWorldPos`/`groupWorldMtx` with counts
(0x15C-0x168), used raw before in npcdrv, end_unknown, rsh_06, evt_mobj, win_log, bom, battle_unit, eff_nice and
unit_mario. animdrv 366 → 179 weighted. Open: `paperAnim` holds either a name or a search index (typed
`const char*`); 0x16C and the AnimData/ANIMDATA_F44-F54 tail are still unnamed; eff_nice reads AnimData clip raw.
The layout fix was merged with #884's (filemgr), which had closed the same gap with `void*` fields: the typed
names above win, and filemgr's relocation calls use them.

### Tooling: address-name literals and symbol renames (2026-10-19, Claude Opus 5.5, n9-q-addr; reviewed by Claude Opus 5.5)

Address names (`float_0p5_80420df8`, `str_80301540`, `unk_802564b4`) were 13,993 of the cleanup index's weighted
48,769 on main. What a rename needs depends on who reads the name:
- **C only**: a name that never reaches the dol's symbol table as a function, i.e. statics, file-local objects and
  constants. Data objects score by their bytes, so objdiff doesn't care; every C file that names the symbol must
  still change together (the linker).
- **C plus `config/G8MJ01/symbols.txt`**: functions. objdiff pairs a function with the original by its symbols.txt
  name, so a source-only rename drops it to 0 and the unit's .text score falls. A dol symbol a REL references is
  also in the dol's FORCEACTIVE list, which dtk writes from symbols.txt (`yuuSetWindowEvt`). Data objects get the
  symbols.txt edit too, only to keep the names in step.
- **Constants back to literals** are a different job: no name is needed at all, but the bytes can change. Named
  objects pool first in definition order and literals after them in first-use order, and a named constant is
  sometimes loaded or scheduled differently, so converting a whole file at once is often not byte-identical.
  Each constant is tried and kept only if the object stays link-equivalent, on top of those already kept.

Tools (all in `tools/`, run on the tower; uv supplies pyelftools):
- `obj_equiv.py OLD.o NEW.o` (or `--tree OLD_DIR NEW_DIR`): link-time interchangeability. Every allocated
  section's size, alignment and bytes; every relocation with its target resolved to a place (a symbol the object
  defines becomes section+offset, so `float_x` and the compiler's `@123` at the same slot are equal; undefined
  symbols stay names); every global both define must keep its place and size. Removed/added globals are listed, not
  failed. The linked checksum stays the final word.
- `addr_literals.py SRC.c ... | --all [--apply] [--kinds float,str,agg] [-j N]`: candidates are float, string and
  const Vec/GXColor objects that only their own .c names, never address-taken, sizeof'd or volatile, and not named
  anywhere in a `#ifdef`/`#ifndef MUST_MATCH` conditional (match-only code is left alone). It compiles variants to
  a temp dir next to an untouched source (a `.qa_` copy, deleted after), compares with obj_equiv, and only
  `--apply` writes, and only to the repo's own sources. Without `--apply` it just reports. `--all` takes ~90 s.
- `rename_symbol.py OLD NEW [OLD NEW ...] [--dry-run]`: renames across src, include, rels, libs and symbols.txt;
  refuses when NEW is spelled anywhere (or in a REL's symbols.txt), when two pairs share a NEW, when OLD is also a
  REL symbol, or when OLD isn't found; matches whole symbols only (`foo` doesn't hit the static `foo$123`).
- `ci.py`'s ratchet reports a function renamed in place (same unit, same address, same or better score) as
  `renamed:` instead of a 100 -> 0 regression. Each successor excuses one lost function; a score drop, a move to
  another unit, a function gone without a same-address successor, or a missing address still fails
  (`tools/tests/test_ci_moved.py`). symbols.txt is protected, so a rename branch runs CI with `--allow-protected`.

`tb.sh` gotcha: its rsync keeps the Mac file's mtime. When a tool rewrites sources on the tower and the Mac copy is
synced back over them (or a file returns to an older version with an old mtime), ninja can see the object as newer
than the source and keep a stale object, so the checksum "passes" on the old code. Touch the changed sources on the
Mac before the confirming build.

Pilot: evt_memcard 40/40 strings literal (172 -> 22 address names), eff_opuku_wprotect_n64 11/20 (115 -> 79),
ac_stick_keep_left 8/13 (84 -> 49); function renames `memcard_buf_reset` (wraps cardBufReset),
`yuuSetWindowEvt`/`evt_yuu_get_window_evt`/`yuuWindowEvt` (the yuu counter REL stores the script window_open runs
as a child event; the user func reads it back). Cleanup index 69.467 -> 69.125. Review: the first pilot pass also
rewrote constants inside MUST_MATCH conditionals (371 -> 30 across the three files); the guard keeps them, and
with them the constants that only pooled in place alongside them.

Recommendation: run `addr_literals.py --all --apply` later as its own branch, when other branches aren't holding
the files (a dry run on this tree: another 12,128 -> 9,285 address names in the files with candidates, index to
about 65.1), then the full build and CI. Don't mechanically rename the float constants that must stay named (the
ones match-only code uses, or whose removal reorders the pool): leave them to the tool's verdict, and give them real
names only by hand where the meaning is clear.
### Readability: battle status icons and lottery (2026-10-19, Claude Haiku 5.5, n9-q-iconlot; reviewed by Claude Opus 5.5)

BattleWorkStatusIconEntry (battle_status_icon.h) gets its fields: `type` (StatusEffectType), `turnCount` (|turns|,
drawn as a number when non-zero), `texId` (0 skips the texture), `scale`/`pivot`/`rotation`/`offset` Vecs, `color`
and `flags` (0x1 no backing plane, 0x2 no texture; the allergy icon blinks by setting 0x2 every 16 frames).
BattleWorkStatusIcon names `mGuruguruAngle` (0x790, +4 degrees per frame), `mAllergyCounter`, `mRegenHpCounter`,
`mRegenFpCounter`, `mBiribiriState` and `mBiribiriDuration` (0x796-0x79E). IconDispTableEntry's columns are `texId`
and `flags`; `_disp_table` ids and _bsi_regeneration's checks use the StatusEffectType names (`STATUS_HP_REGEN`,
`STATUS_FP_REGEN`). lottery_info (evt_lottery.h) is fully named: `mFlags` (bits documented in the header: 0x1
initialised, 0x2 ticket held, 0x4 clock went backwards, 0x8 bought again today, 0x10 new day for this slot,
0x1000-0x8000 tier 0-3 won today), `mDrawTime` (0x8), `mTodayNum` (0x10, -1 while the clock is suspect),
`mBuyTime` (0x18, the origin of `mTierDays`), `mTicketNum` (0x20), `mTierDays[4]` (0x22; tier 0 matches all four
digits and is rescheduled every 336-395 days, tier 3 the last digit every 5-10) and `mSlotTime[2]` (0x30). The old
`u8 field_0x2[]` byte view is gone; winmgr.c, evt_win.c and win_root.c read `mTicketNum`. Weighted cleanup:
battle_status_icon.c 270 → 130, evt_lottery.c 225 → 7.
Open: the `dat_804221f0`-`dat_80422210` / `float_0p10207_80422208` icon colours are GXColor words still named by
address (and read through `*(GXColor*)&`); the IconDispTableEntry `texId` values (13-34) are not tied to an icon
enum; evt_lottery's MUST_MATCH slot clear (`q[12]`/`q[13]` through a `(u8*)info + arg1 * 8` base) and the
right-to-left tier-miss clause stay; lottery_info's 0x2-0x7, 0x12-0x17 and 0x2A-0x2F have no readers anywhere.
### Readability: battle_seq_end (2026-10-19, Claude Haiku 5.5 and Claude Opus 5.5, n9-q-seqend)

The SEQ_END sub-sequence steps 0x07000000-0x0700001F are now `BattleSeqEndStep` (battle.h, next to
`BattleSequence`), named from what each case of btlseqEnd does: `SEQ_END_INIT`, the lose path (`LOSE_FADE`,
`LOSE_FADE_WAIT`), the field end event (`WIN_EVT`, `WIN_EVT_WAIT`), the win setup, exp count, Merlee, level-up,
rank-up and victory-pose steps, then `FADE_OUT`, `FADE_WAIT` and `RETURN`. btlseqEnd, `_audience_Whistle_control`'s
case run and the six `BattleSetSeq(wp, SEQ_END, 0x7000000)` calls in battle_seq.c/battle.c use them. The lose path's
`seqSetSeq(SEQ_MOVE, ...)` was the battle enum's 5 standing in for `SEQ_GAME_OVER`. EventEntry 0x88 is now a union of
`wInterpRelatedTime` and `s32 userdataEx[2]`: battle_seq_end's spot and level-up-object user functions keep scratch
ints there (Haiku's first pass aliased them as `((s32*)&event->wInterpRelatedTime)[n]`). New flag names, each
proven from its setter: `BATTLE_ESCAPED` (BattleWork.flags 0x10, btlevtcmd_InterruptStop(0) at the end of
btldefaultevt_Escape) and a `BattleUnitStatus_Flags` enum for mStatusFlags: `kStatusFlipped` (0x1, Koops'
dmg_turn_event/wakeup_event, blocks BtlUnit_CanActStatus), `kStatusDanger`/`kStatusPeril` (0x10000000/0x20000000,
BtlUnit_CheckPinchStatus against mDangerHP/mPerilHP). Mario's 0x10 attribute is the existing `kVeiled`, status 0x1B is
`STATUS_INSTANT_KILL`.

Open: battle results (BattleInformationSetResult 1 win, 3 lose, 4/5 interrupted; 2 is read by npcdrv) need an enum in
battle_information.h; BattleWork.flags 0x20 (InterruptStop(1), no caller in src), 0x40 (result 4, setter unknown)
and 0x100 (`check_battleflag 256`: camera mode 0x12, the curtain event's walk-off); NpcBattleFlags 0x20000000
(PAUSE3 voice) has no name yet;
BattleSeqEndWork `unk14`/`unk28` (zeroed beside `lvupSelect`/`confirmSel`, never read) and `unk0`; mAttributeFlags
0x21000000 and unit->flags 0x80000000 in `_disable_enemy_unit_disp`; battle_mario.c's raw `event + 0x88/0x8C` casts
can move to `userdataEx` too, and evt_sub's pointer use of 0x88 could join the union.

### Tooling: offset_to_field.py (2026-10-19, Claude Opus 5.5, n9-q-offtool; reviewed by Claude Opus 5.5)

`tools/offset_to_field.py` rewrites raw byte-offset casts into the field they already land on:
`*(f32*)((u8*)unit + 0x174)` -> `unit->moveSpeedY`, `(Vec*)((u8*)pl + 0x98)` -> `&pl->pos`,
`((f32*)((u8*)p + 0x40))[i]` -> `p->arr[i]`. It parses each file with libclang (powerpc-eabi, the file's own
-I/-D flags), so the base's type comes from the compiler. A site is rewritten only when the offset is an
integer literal, it is the exact start of a field whose type is the cast's pointee (const ignored), and the
field is not volatile, not a bitfield and not a placeholder name (`unk*`, `pad*`, `field_*`, `_*`). In a
union, exactly one member path may match. The cast must not sit in a macro or inside a MUST_MATCH
`#if`/`#ifdef` (either arm), and its pointer must not be stepped through or stored in a variable. One
exception on type: a pointer read through another pointer type keeps its cast, as `(T*)p->next`. Every
rewrite is also checked against MWCC's own layout from the objects' DWARF.

How to run it. Run it on the Mac, where libclang comes from uv; no build ever runs on the Mac. You need two
files from a built tree on the tower: its `compile_commands.json`, which configure.py writes and tb.sh does
not sync, so `scp pop-os:ttyd-n4-TAG/compile_commands.json`, and a DWARF index made by
`tb.sh TAG 'python3 tools/offset_to_field.py --dwarf-index /tmp/dw.json'`. Then:
`uv run --with libclang tools/offset_to_field.py --compile-commands CC --dwarf DW [--list] [PATH...]` gives a
dry-run table of outcomes, and the same with `--apply PATH...` rewrites the files. Then build with tb.sh and
require "32 files OK". `--verify --build-cmd CMD` builds and bisects, reverting files that break. Use
`--skip-base TAG` to leave alone a struct another branch is editing.

Results. The pilot's dry run found 189 convertible casts out of about 4650 (u8*) casts. 133 were applied in 34
files, and 24 more landed in BattleWork's newly named `acParams`. After the readability branches of 2026-10-19
landed, about 110 of those survive; on the files they rewrote, main's version was kept. A rerun on that tree,
with a fresh DWARF index, found 195 convertible casts, and then 163 after a second rebase. Of those, 139 were
applied in 29 files through `--verify`, and all were byte-identical. The whole-tree table now reads 108
convertible out of 4021. Most of the rest land on unnamed space: 1823 casts hit padding or `unk`/`field_`
names, and 884 have a `void*`/`u8*` base. Rerun the tool after each naming pass: a field that gets named turns
its casts convertible. Regenerate the DWARF index too, since a stale one blocks the newly named fields as
"layouts disagree".

The enumsalwaysint fix. MWCC's `#pragma enumsalwaysint off` sizes an enum to its values, and clang ignores
the pragma. Without a fix, every struct holding such an enum came out with int-sized members and wrong
offsets. The tool feeds clang in-memory copies of those headers, adding `__attribute__((packed))` to each enum
in the off regions.

Limits. Clang's layout is the one that decides, and DWARF is only a cross-check: a file with no DWARF entry
for a struct goes unchecked, so the byte-identical build is the real verdict. Generated includes missing on
the Mac (`assets/*.inc`, `texPalette.h`) are stubbed empty, with a note on stderr. About 25 files still parse
with clang errors (declaration conflicts inside function scope, none of them in struct layouts). The tool
never names fields itself, and it leaves pointer arithmetic, non-literal offsets and casts on untyped bases
alone. Open: the 108 convertible casts are in files that open branches were editing at the time:
battle_event_cmd (48), evt_mario (19), win_root (15), mario, mario_hit, mario_motion, evt_mobj, battle_seq and
evt_lect_win. Check those files' claims before running `--apply` on them.
### Readability: muj map RELs (2026-10-19, Claude Haiku 5.5, n9-q-muj; reviewed by Claude Opus 5.5)

muj_10.h's NanNpcEntry mirror of src/event/evt_nannpc.c now uses the names evt_nannpc.c and gor_00.h use for the
same offsets (`homePosition`, `startTime` 0x50, `facingAngle` 0x60, `state` 0x68, `timer` 0x6C, `field_0x88`,
`work[6]`); bytes muj doesn't read stay `field_0x80[8]`/`field_0x94[0xC]`. The free slots carry muj's uses:
`field_0x70` (state after a tobikakari jump, or elmos1000tai's entry number), `radius` (0x74, elmos1000tai's
distance from the formation centre, also scaling its fade-in delay), `jumpQuad`/`jumpLinear` (0x78/0x7C, the
tobikakari arc y = hoseiPosition.y + t * (jumpQuad * t + jumpLinear), which lands on homePosition at t = 1) and
`spot` (0xA0, a `TobikakariSpot {Vec home; s32 group;}` in muj_00's Tobidat block, replacing `void* data` and
its `+ 0xC` reads). muj_20: MarcoShip (`angle`, `angleStep`, `angleLimit`, `settled`, `state` 0-3, `stateTime`;
marcoship_stop_func writes state 2, not `EVT_RETURN_DONE`); LogoWork's 0x4 was misnamed `alpha` and is the y
offset (`offsetY`, -50 to 0 fading in, 0 to 100 fading out), 0x8 is `alpha`, 0x0 is `FileEntry* tpl` (mariost.tpl,
read as `*tpl->data` instead of `+ 0xA0`). FieldBattleData 0x10 is `firstAttackKind` (written by fbatHitCheck:
jump kinds 0x20000-0x80000, hammer 0x100000-0x400000, 0x800000, 0x1000000; tou_03 forces 0x10000000; seq_battle
maps it to BattleInformationSetFirstAttack 1-7); npcdrv, seq_battle, tou_03 and koburon_get_encount_info read it
by name. Index 57.167 -> 56.915 (worker) -> 56.898 (review), on main at dc327ded. Open: no shared NanNpcEntry header yet; evt_nannpc.c,
gor_00.h and muj_10.h each declare the layout, and a shared one in include/evt/evt_nannpc.h would need gor's and
muj's per-REL slot names (0x70-0x84, 0xA0) folded into generic ones. gor_00.h calls 0x88 `targetPos`, but
evt_nannpc_move_position2 stores the move's start position there. FirstStrikeInfo and the 0x800000/0x1000000/
0x10000000 attack kinds are unnamed; npcdrv.c has an orphaned comment above fbatAttackEnabled about fbatHitCheck's
second argument.
### Readability: effStarStoneData and effStampData (2026-10-19, Claude Haiku 5.5, n9-q-starstone; reviewed by Claude Opus 5.5)

effStarStoneData (eff_starstone.h) gets `starType` (idx % 7: colour-table index; 0 and 6 draw the diamond_1 and
diamond_7 models through effStarStoneDisp_1, 1-5 the diamond_2_2 tip pairs through effStarStoneDisp_2), `flags`,
`x`/`y`/`z`, `scale`, `rotY` (whole star), `tipRotY`/`tipRotZ` (the tip pairs, applied after rotY in Disp_2; pair B
uses the negated angles), `scrollU`/`scrollV` (texture-matrix translation; only U moves), `timer` (0..59),
`matColor` (packed GXColor for GXSetChanMatColor), `dustEffect` (the snow-dust child), `camId`, `state`, `gravity`/
`velY` (the bounce), and `tevReg0R/G/B/A`: the GX_TEVREG0 colour that TEV stage 1 multiplies into the vertex
colours. Its alpha is the star's opacity and is also what effStarStoneMain passes to U_shadowSetMode, whose `mode`
is the shadow's TEV alpha, so it is one value used twice, not two roles. effStampData (eff_stamp_n64.h) gets
`scaleY`/`scaleX` (0x1C/0x20, the PSMTXScale factors) and `tevReg0R/G/B` (0x38) / `tevReg1R/G/B` (0x3B): the
stamp's TEV stage computes lerp(C0, C1, raster alpha), and GXSetTevColor(1, ...)/(2, ...) load GX_TEVREG0/GX_TEVREG1
(GX_TEVPREV is 0), so the worker's tevReg1/tevReg2 names were one register off and are renamed. effStampDisp's GX
setup now uses enum names, each checked by value and enum type in GXEnum.h; the starstone MUST_MATCH calls pass
GX_TEVREG0. Users updated: sac_bakugame/genki/muki/suki/zubastar, battle_damage (STAMP_COLOR), itemdrv, win_log and
the bom/gor/kpa/las/mri/muj/nok/pik/tik/tou/usu map RELs. sac_scissor's local ScissorEffectData (posX..rotZ at
0x8..0x20) was an overlay of effStarStoneData and is gone. Cleanup index 55.936 -> 55.333 on origin/main 042dadc2;
every touched C file compiles without MUST_MATCH. Open: effStarStoneData `field_0x6`, `field_0x58` (set to 90 in
the entry, not read in eff_starstone.c) and the `flags` bits still need names (1 set when the fall starts and
tested in the Disp tip loops, 2 no dust child (idx >= 7), 4 shine loop wanted, 8 shine loop playing, 0x10 no landing
sound, 0x20 not drawn); tipRotY/tipRotZ are never initialised by effStarStoneEntry; the
dust child is written through raw offsets (0x04..0x0C, 0x4C..0x54, 0x68) because eff_snow_dust_n64 has no public
struct; sac_scissor's `starRot.x/.z` feed tipRotY/tipRotZ, so that ScissorWork field is likely misnamed;
effStampData `scale` (0x30) is the ring radius, not a scale, and `field_0x10..0x18` stay unnamed.
### Readability: effects, group 6 (2026-10-19, Claude Haiku 5.5, n9-q-eff6; reviewed by Claude Opus 5.5)

Work structs named for eff_release_n64, eff_whirlwind_n64, eff_kemuri10_n64 (0x1C-0x4B is `Mtx orient`, the
up/side/dir columns PSMTXConcat reads), eff_thunder_n64 (`scrollU0`/`scrollV0`/`scrollU1`/`scrollV1`, their speeds
and update intervals, the `flare`/`charge` children), eff_itemget, eff_pokopi_pipo_n64 and eff_fall_leaf_n64;
GX, camera and PSMTXRotRad axis literals in those files are enums or chars. TEV colour fields are named after
the register they feed (`tevReg0R`..`tevReg0A`, `tevReg1R`..). The review checked every GX swap against
GXEnum.h by value and enum type, and natural-compiled all 16 touched C files on the tower against the base
revision: the `.text` and relocations of every natural object are identical, so the `#else` branches the
checksum doesn't cover are unchanged. Review fixes: effItemGetData's 0x0 is the glow `type` (0-10, which
itemdrv picks from the item kind), not an item id; never-read fields went back to `field_0x` with what they hold
in the comment (release 0x28, pokopi 0x10[6] and 0x64); EffWhirlwindData moved to its header so battle_mario's
and battle_event_subset's whirlwind spawns write `->x/y/z` instead of raw offsets, and effWhirlwindN64Entry's
parameters are `type, mario, life, size`; EffCrystalData moved to its header so tik_05's crystal_bgmode writes
`EffCrystalData.camId` and map_bgmode/map_bgmode_end write `EffTreasureMapData.cameraId`, where they had cast
those effects to effItemGetData (one through `*(s32*)&...->velY`); the camera ids there and in sys.c are
CAMERA_3D / CAMERA_3D_EFFECTA; the MUST_MATCH-only effFallLeafMainData took the particle names; the remaining
`GXSetChanMatColor(4, ...)` under MUST_MATCH in eff_itemget use GX_COLOR0A0; thunder's `life` comment no longer
claims a grow-in. Index 59.234 -> 58.392 (worker, after rebase onto cfbcdb84), 58.353 after the review. Open:
eff_thunder still writes EffAkariChargeData's `field_0x18`..`field_0x30`, waiting on n9-q-eff7's names; never
read or unused: release 0x60-0x6C (zeroed at init), kemuri10 0x0 and the second RGB copy at 0x67-0x69,
whirlwind 0x47, fall_leaf's header 0x10-0x23 and particle 0x24-0x2F; the itemget particles reuse
effItemGetData, so `velX`..`angle` mean nothing on the header and `type`/`scaleMode`/`camId` nothing on a particle.

### Readability: effects, group 3 (2026-10-19, Claude Haiku 5.5, n9-q-eff3; reviewed by Claude Opus 5.5)

Two work structs named and one entry's parameters. effCloudData: `frame`, `color0` RGB + `alpha` (GX_TEVREG0) and
`color1` RGB (GX_TEVREG1), the moving `head`/`vel` XYZ and a 30-entry trail ring (`trailX/Y/Z`, `trailFrame`,
`trailActive`, `trailHead`, `trailLength` = cumulative path length driving texture U). effConfettiN64Data's
`unk_0x10[0x24]` is now `velX/Y/Z`, `accelY`, `lifetime`, `delay` and the head-only `frame`/`alpha`/`count`.
effBreathFireN64Entry's parameters take the group 2 field names (`kind, x, y, children, z, seed, endX, duration,
endY, endZ`). The breath fire, kemuri11, funemizu and minigame structs this branch also named had already landed
in group 2 (#890); on rebase group 2's names were kept. GX literals became enums, checked against GXEnum.h per
parameter type (GXSetTevColor 1/2 = GX_TEVREG0/1, texgen 0x3C = GX_IDENTITY, 0x7D = GX_PTIDENTITY); camera 4 is
CAMERA_3D in the confetti and funemizu entries. Open: the `float_*_8xxxxxxx` pool constants in these files stay
(MUST_MATCH references them by address).
### Readability: battle_disp and battle_detect (2026-10-19, Claude Haiku 5.5, n9-q-bdisp; reviewed by Claude Opus 5.5)

BattleSampling's two filter words are `targetClassFlags`/`targetPropertyFlags` (0xAC0/0xAC4): every caller passes
the weapon's fields of those names, and BattleSamplingEnemy's parameters now carry them too. BattleSamplingEntry
gets `baseX/baseY/baseZ` (0xC-0x14: the part's world position before the hit-cursor offset, x plus `xBias`, z plus
5; the worker's `hitX` had it backwards, x/y/z at 0x4 are the cursor position), `xBias` (0x18, the attack-direction
and `detectOffsetX` push, weighted x10 in the sort), `forwardLv` (0x1C) and `zSide` (0x1D, 0 when baseZ < -30).
Status, unit-kind and attribute literals are named where the enum value and the use agree: STATUS_INSTANT_KILL as
the usual "defeated" check, STATUS_TINY/HUGE/FAST/SLOW/HOLD_FAST, STATUS_INVALID for _GetStatusPoseType's "no
status" result, kInactive/kFloating/kCeiling/kShellShielded/kVeiled on units, the CannotTarget/kUntattleable part
bits, the Shadow Siren kinds (and Doopliss as Vivian) that run animPoseVivianMain, TYPE_PARTNER_MIN/MAX. The
worker's `kUnknown_0x10`/`kUnknown_0x40000` substitutions went back to hex. battle_disp reads BattleUnitPart's
`partFlags`, `statusPoseStep`/`statusPoseKind`, `evtColor` (scaled by the owner's `mMoveColorCur`) and
`dispCallback` (now typed `void (*)(part, mode)`) by name; 0x216 is `statusPoseTimer`, the u16 countdown of each
statusPoseControl stage. The `#pragma dont_inline` pairs around btlDispEntAnime, btlDispChangeAnime,
btlDispChangePeraAnime, btlDispPoseAnime and btlGetScreenPoint are gone: nothing in the file calls them, the
matching build is byte-identical, and the natural (`--non-matching`) battle_disp.o disassembles identically
with and without them (only the anonymous `@NNNN` literal numbers shift). battle_detect.c 115 → 55 weighted,
battle_detect.h 9 → 1, battle_disp.c 141 → 131; overall 63.441 → 63.330.
Open: the target class/property bits stay hex (worth an enum shared with BattleWeapon: class 0x10000 is Mario
only, property 0x1000000 keeps only the front-most candidate per zSide row); part attributes 0x10, 0x20000,
0x40000, 0x2000000 and unit attributes 0x40, 0x1000000 are unnamed; why STATUS_HOLD_FAST suppresses the
mStatusFlags 0x20000000/0x10000000 poses in btlDispMain is unknown; BattleSamplingEntry.field_0x20 (set to -1,
no reader here) is unnamed; the unit-kind range checks use the first kind of the next block (UNIT_UNUSED_TREE,
UNIT_UNUSED_TESTNPC) as bounds rather than category macros. battle_disp still reads the pose handle and pose flags
as `*(s32*)(part + 0x1C0)` / `+ 0x44` (field_0x1C0 is one array), the float-bob counters at 0x1A6/0x1A8 and the
0x4F4/0x4FC bytes raw, and btlDispAnimeSpeed/btlDispAnimeSpeedReset take a `BattleUnit*` that is really a part
(callers cast `(BattleUnit*)part`; it writes the part's `field_0x208` and multiplies by `motionSpeed` through `unit->hpGauge`). battle_seq.c
tests the part-attribute `kUnknown_0x40000` against a unit's mAttributeFlags.
### Readability: effects, group 4 (2026-10-19, Claude Haiku 5.5, n9-q-eff4; reviewed by Claude Opus 5.5)

Work structs get names in eff_dogabon_attack_n64 (25 pieces as parallel arrays: pos/start/target rings, scale,
rotation, velocity, per-piece `state`/`stateTimer`/`waitTimer`), eff_radiation_n64 (struct in its header; head
`x`/`y`/`z`, `life`/`age`/`alpha`, per-piece `offset`/`rot*` with their start values and velocities, `lifetime`),
eff_kameki_tornade_n64 (a chain of 8 `node*` arrays, two colours, two texture scrolls), eff_fire_spark_n64 and
eff_kemuri1_n64 (`speed`, `velY`/`velYAccel`, `dirX`/`dirZ`, `sheetU0`/`sheetU1`, the two texture columns the
draw cross-fades). GX call literals become GXEnum.h names throughout the five draws, dispEntry's first argument is
CAMERA_3D, and the texgen post-matrix 0x7D/125 is GX_PTIDENTITY. In review: kameki's colours load GX_TEVREG0 and
GX_TEVREG1 (GXSetTevColor ids 1 and 2; GX_TEVPREV is 0), not "TEVREG1/TEVREG2" as the field comments said, and
its `texScrollU`/`V` are `texScroll0`/`1` (both are S offsets, one per texture matrix); radiation's `additive` is
`noDepth` (its draw state is ordinary SRCALPHA/INVSRCALPHA blending with z test and z write off, in disp layer
8), and the never-written Vec at 0x04 is `unsetPos` rather than `sortPos`; the MUST_MATCH-only GXSetTevColor and
GXSetChanMatColor literals became enums too. Byte-identical; the five files' natural objects disassemble the same
before and after the whole series (index 62.669 → 61.763 on its base).

Lesson: the checksum never compiles `#else` code, so an enum swap or rename on a natural-only line is unchecked
by the build. The worker once mapped a literal 1 to GX_TEVREG1 there (GXSetTevColor's 1 is GX_TEVREG0). For any
literal → enum pass, check each value and the parameter's enum type against GXEnum.h, then compile the touched
files without MUST_MATCH (tools/ci.py `natural`) and compare their disassembly with the base's.

Open: radiation's Main copies `unsetPos` (head 0x04) as the dispCalcZ position, but no code writes it, so the
sort key is whatever the heap held (retail bug or a field set elsewhere that is not decompiled yet; las_29 writes
only `x`/`y`/`z`). Radiation `holdTimer` is only ever set to 0, and 0x1C/0x20 (`a5`) and the 0x34-0x54 scroll
group are written but never read by the draw. Kemuri1's Disp skips entries whose own `sheetU0` > 15, a field only
the head writes. kemuri1 0x14-0x2C (the case 0/1 jerk chain and the 0.94 dampers) and 0x38 are still field_0x.
### Readability: REL group 3 — demo, Glitz Pit, gon_03 (2026-10-19, Claude Haiku 5.5, n9-q-rels3; reviewed by Claude Opus 5.5)

dmo_00 DemoWork gets `bookAngle` (book_r), `page` (-1 = none, next() counts up, demo_cap captures at page 4),
`captured`, `fade` (adjust's argument; picture alpha is 255 * (1 - fade)), `scale` (demo_scale ramps 0.6 -> 1.5),
`posX`/`posY`, `captureBuf`, `tplFile` (demo.tpl), `winOpen`/`winAlpha` (demo_winonoff; alpha 0..0xCC in steps of
10). The draw callbacks use `CAMERA_2D`/`CAMERA_3D`/`CAMERA_3D_IMAGE` and GX enums for blend, alpha compare, z mode,
texgen, cull, vertex descriptor/format and `GX_QUADS`; every swap was checked by value and enum type against
GXEnum.h and camdrv.h. gon_03 SenkaronEntry: `timer`, `velX/Y/Z`, `flags` bit 0 (face away from Mario, set 1 time in
50 when a wander leg starts). tou_02 FanPose `state` (0 wandering, 10 hop, 20 drift, 100/101 settled) and `timer`;
ExtFanWork `hammerFrames` and `idleCount` (read back by evt_end_count and scaling the crowd volume); check_hammer
and ext_fan_main use `MARIO_MOTION_HAMMER`/`HAMMER_2` (0x12/0x13) and `PARTY_MEMBER_FLURRIE`. tou_03 ScreenWork
(`texBuf`, `captured`, eased `scale`/`offsetX`/`offsetY` with their targets, `frame`, `useNoise`, `noiseBuf`);
KyakuEntry follows rsh_05's MoamoaGroup (#885): `enabled` (always 1 here), `cycleFrames`, `firstPose`, `numPoses`;
KyakuState `cycleStart`. starstone_sound_onoff/evt_boss_stone_r and eff_pos go through `effStarStoneData` and
`effPokopiPchargeData` instead of byte offsets (effStarStoneData `flags` bit 4, `rotY`); _firstattack_setting
writes FieldBattleData `firstAttackKind` and `field_14` instead of `(u8*)fbat + 0x10/0x14/0x1C`. evt_yuunpc and evt_yuuranking unchanged. Review: removed two
committed `.c-e` sed backups, renamed KyakuEntry `poseCount` -> `numPoses` and named `enabled` to match MoamoaGroup,
tightened the gon_03 flag and idleCount comments. Natural compile of all four files is clean. Index 54.714 -> 54.363.
Open: FieldBattleData 0x14/0x1C (`field_14`, `field_14[8]`),
DemoWork `field_0x00`, ScreenWork `field_0x28` (set to -1, otherwise unread), FanPose `field_0x6`/`0x38`-`0x40`/
`0x46`. The next shared-struct target is ExtPose `field_0x4` (yaw) and `field_0x10` (pose/frame index), which
gon_03, tou_02, tou_03, rsh_05 and the other ext users write (about 20 REL sources).

### Readability: battle stage objects (2026-10-19, Claude Haiku 5.5, n9-q-bso; reviewed by Claude Opus 5.5)

BattleStageObject (battle_stage_object.h) gets `mPositionOffset1` (0x10, added to mPosition on every layer; the
layer-2 rotation pivots about its y; _nozzle_set_offset writes y) and `mPositionOffset2` (0x28,
btlevtcmd_SacBgsetMove writes x/y). 0x34-0x58 are the change-detection copies `mLastPosition`,
`mLastPositionOffset1`, `mLastRotation`, `mLastPositionOffset2`: BattleStageObjectInit seeds them with the 999
`BSO_UNSET` sentinel and BattleStageObjectMain re-applies the transform to the map object when any differs. 0x75 is
`pad_75`. New enums: `BattleStageObjectFlags` (`BSO_FLAG_HIDDEN` 0x1: Main keeps map group flag 1 on while set, and
the map draw loop skips objects with that flag; set by _mobj_destroy and the if_body ceiling in battle_seq_end.c;
`BSO_FLAG_DIRTY` 0x2: forces a transform re-apply, then cleared; set by the battle_seq.c scale code) and
`BattleStageObjectSpawnType` (`BSO_SPAWN_TREE`/`SWITCH`/`NONE` for BattleStageObjectData's
mSpawnSpecialUnitType). _mapobj_data_touch_scale reads `obj->data->mLayer` instead of a raw +6. Open:
`field_0x72` (only zeroed, in battle_stage_object.c), and btlevtcmd_BtlStageObjFlagOnOff takes its flag from the
script.

### Readability: battle action-command minigames (2026-10-19, Claude Haiku 5.5, n9-q-ac; reviewed by Claude Opus 5.5)

Each src/battle/ac file's work area at BattleWork+0x1F4C (`BattleActions.field_0x2BC`) has field names taken
from its uses: GaugeTimingWork (`gauge`, `sweep`, `timeLimit`, `inZone`, `phase`, `tapMode` (1: a press stops
the needle, 0: hold and release), `buttonMask`, `flashTimer`, `laps`, `sfxId`), AcRepeatedlyWork (`curMask`/
`maskA`/`maskB`, `holdMask`, `goal`/`goalStep` (the bouncing mark `power` must reach), `power`, `decay`,
`presses`, `full`, `probeHeld`), RepeatedlyLvWork (`gauge`, `level`, `segmentSize`, `fillState`, `releaseMode`),
AcSignalTimingWork in ac_signal_timing.h (`hits`/`misses`, `curSignal`, ok-window bounds, `latched`, `expected`),
StickRotateWork (`inputDir`, `expectDir`, `rollDir`, `steps`, `fastSteps`), ShotTargetWork (cursor `spin`,
`inRange`, X/Y wobble period/timer/amplitude), AcShotWork/TargetMarkWork (`minValue`/`maxValue`, sweep bounds,
`rotSpeed`, `rollAngle`), TimingWideDecWork (`buttonMask`: field 0x00 is a BattlePad mask, not an icon type;
`pressMode`, `frame`, `phase`), LstRcKeepWork (`leftHold` for pad bit 0x40000 (stick left), `cRightHold` for
0x800000 (C-stick right)), plus the end timers and token-roll bytes in the rest. Words no code touches are
`pad_0x..`. The 51 raw `(u8*)work + 0x1Cxx/0x1Fxx` reads in the ac files go through `actionCommands` (acParams,
acFlag, guardFrames, dispParams, field_0x2BC). ac_signal_timing's one remaining `acParams[9]` store (past the
array, BattleActions 0x5C) is `outputParams[1]` like its siblings. MUST_MATCH blocks changed only identifiers.
field_0x uses in src/battle/ac + include/battle/ac 1138 → 217; what is left is BattleActions' own `field_0x4`,
`field_0x8`, `field_0x30`, `field_0x2BC` and BattleACDispParams' `field_0x0`/`field_0xC`/`field_0x10`/
`field_0x24`, which need evidence from battle_ac.c and the other ac users before naming.
### Readability: MarioWork names (2026-10-19, Claude Haiku 5.5, n9-q-mario; reviewed by Claude Opus 5.5)

MarioWork (mario.h) gets the jump integrator `jumpVelY`/`jumpAccelY`/`jumpJerkY`/`jumpJerkStepY` (0x7C-0x88; each
frame the next one down is added in, and jumpVelY goes into position.y), the per-frame pad copy marioUpdateKeyData
writes (`buttonHeld`/`buttonNew`/`buttonRepeat`/`buttonUp` as u16 truncations of gp's u32s, `wStickDir1/2` and
`substickX/Y` as s8 like gp's, `triggerLeft/Right`, `unusedInput`, `padDirs`/`padDirsPressed` from DemoPad[0].dirs,
and `inputMask` at 0x284), `dispReqDir` (0x1AC), `spinSpeed` (0x2A0), the ground slope block `slopeAngle` and
`slopeTiltX/Y/Z` (0x214-0x220; X is the tilt about X from the normal's z/y, Z the tilt about Z from x/y), the wall
hold `wallDist`/`wallYaw`/`wallHoldTimer` (0x1C8-0x1D0), `frontWallDist` (0x210), `hitobjFront`/`hitobjUnder`,
`sfxIds[3]`, `partyUseTimer` (0x3B), `dispHalfTurn` (0x43, a 0/1 facing flip) and `lastGroundPos` (0xD4).
MarioWork casts and offset macros 2104 → about 1060. MUST_MATCH branches keep their raw offsets, except party_cloud's
`pm->unk1AC`, which no longer exists. Review fix: narrowing the padding arrays had left mario_hit indexing
`field_0x1F0[0x20..0x30]` and `field_0x1C0[0x8..0x10]` past their new ends (the offsets were still right); those
now use the members, apart from the two MUST_MATCH `wallDist` stores. Commit 04e3dbff's message overstates
mot_roll's slopeAngle reads: mot_roll compares it against 40 (the eki_04 check) and 1; the 30-degree check is in
mario_hit.
Left: the per-motion scratch at 0x2A4/0x2A8 (mot_roll flags, mot_hip and mot_roll SFX ids, an f32 in mot_plane)
and 0x2B8-0x2DC (mot_jabara, mot_slit) needs per-motion structs, not one name; 0x19C is both a move direction and
the wall yaw copy; several Vec blocks (0x98-0xC8, 0xE0, 0x104-0x134) are still unnamed. MarioFlags isn't applied
to the raw `flags` literals yet. The worker held back because it thought two names contradicted the code, but both
hold: 0x08000000 (`IS_TOUCHING_CEILING`) is set by marioChkOverhead when marioSearchHead hits, and 0x01000000
(`TUBE_MODE`) is the roll state, which is Tube Mode. Applying the enum is the next pass.

### Readability: evt_mobj and mobjdrv (2026-10-19, Claude Haiku 5.5, n9-q-mobj2; reviewed by Claude Opus 5.5)

Raw offsets replaced with existing names: MarioWork `hitobjStandOn` (0x1E4, also in mobjMain's per-frame hit cache),
PartyEntry `frontHit` (0x130), NpcEntry `hazardImmuneFlags` (0x31E), and MapObjectEntry `unk1A0`/`unk1A0[4]`/`unk1B4`/
`childName` (0x1A0/0x1A4/0x1B4/0x1B8). `unk1A0` and `unk1B4` stay neutral: each object type gives them its own meaning
(item id, coin or part count, switch anim index | state bits; sound handle or timer reload). New
`MapObjectEntryFlags` in mobjdrv.h, each bit proven by mobjMain/mobjDisp*/mobjEntry/mobjDelete: `MOBJ_FLAG_ACTIVE`
(0x1), `RECALC_MTX` (0x4), `TRIGGERED` (0x8, set when a hit test fires; the tests skip while it is set),
`GRAVITY` (0x40), `NO_CULL` (0x80), `RETRIGGER` (0x100, clears TRIGGERED every frame), `PAPER_TEX` (0x400), `BLINK`
(0x2000), `NO_DISP` (0x4000) and the hit tests `TRIGGER_HEAD` (0x400000, MarioWork 0x1F4 = marioSearchHead's result;
also picks shadow type 1), `TRIGGER_STAND_ON` (0x200000), `TRIGGER_HIP_DROP` (0x1000000), `TRIGGER_PARTY`
(0x800000), `TRIGGER_BOBBERY` (0x2000000, its in-range case reports TRIGGER_PARTY), `TRIGGER_KPA_HEAD` (0x4000000),
`TRIGGER_KPA_LV3` (0x8000000), `TRIGGER_KPA_HIP` (0x10000000) and `TRIGGER_KPA_FLAME` (0x20000000). Applied in
mobjdrv.c and to every MapObjectEntry `flags`/`triggerFlag` literal in evt_mobj.c; unnamed bits stay hex inside
the OR (`MOBJ_FLAG_TRIGGER_PARTY | 0x10000`). Byte-identical. Index 64.833 (origin/main) -> 64.700; evt_mobj
raw_offset 49 -> 13, mobjdrv 32 -> 22.
Open: trigger bits 0x10000, 0x40000 and 0x80000 test MarioWork HitEntry pointers at 0x1FC and 0x1E0, which are
unnamed on main (0x1E0 is also read by mario.c, mario_hit.c, mario_motion.c and mario_balloon.c); 0x2 and 0x800
both suppress the shadow and 0x10/0x20/0x200/0x1000 have no reader in mobjdrv, so they stay hex. MapObjectHitEntry's
`unk4[0x88 - 0x14]` tail looks like an embedded MapFileJoint (hitEntryMOBJ takes a joint) and wants typing against
mapdrv.h. Untyped effect userdata: fire_func writes `eff->userdata + 0x78/0x7C` (eff_fire work, no struct yet).
Also still raw: MarioWork 0xD8 (f32 compared with position.y in evt_mobj), the rideswitch Vec template reads at
+0x7C/+0x88/+0x94, ImageEntry 0x110 (an OSTime inside `field_0x10C`), and mobjMain's `(HitEntry**)wp` cache over
MapObjectWork `field_0x8`.
### Readability: effects, group 7 (2026-10-19, Claude Haiku 5.5, n9-q-eff7; reviewed by Claude Opus 5.5)

Work structs named in eff_kemuri5_n64 and eff_kemuri6_n64 (shared names where the roles match, though kemuri6
has an extra `rotZ` at 0x18 so the offsets differ by one f32: `alive`, `timer`, `alpha`, `scaleX/Y/Z`,
`baseScaleX/Y`, `swayAngle`, `velY`, `accelY`, `accelStep`; kemuri5 `driftSpeed`/`dirX`/`dirZ`, kemuri6 `type`
gating the mario-stay hold), eff_pturn_n64 (the flat `effPturnN64Data` view folded into `PturnN64Elem`; the
Entry's second-element writes are now `data[1].x` etc.), eff_akari_charge_n64 (`life`, `counter`, `alpha`,
`scale`, and `reg0R/G/B` / `reg1R/G/B` after the GX_TEVREG0/1 they load, renamed through the EffAkariChargeData
accessors in eff_thunder_n64, eff_queen2, aji_02/05/12 and mri_01), eff_nozle (`scrollX0/Y0/X1/Y1` for the two
texture matrices, `rotX/rotY`, petal `offsetX/Y/Z`, `scaleX/Y`, `shrinking`) and eff_nice (NiceEffectWork
`scaleX/Y/Z` for the acrobat sub-entries, `scale` for the whole effect, renamed in battle_seq, battle_damage and
battle_event_cmd; `state`/`stateTimer` for the acrobat state machine). GX literals use their enums;
dispEntry 4/8 are CAMERA_3D/CAMERA_2D (camdrv.h). Review: every swap checked by value and enum type against
GXEnum.h, including the `#else` branches (akari's natural GXSetTevColor is GX_TEVREG1, as its MUST_MATCH line);
all 15 touched C files compile without MUST_MATCH. PturnN64Elem stays one struct: the head-only and
particle-only fields sit at disjoint offsets, and the two that both use (x/y/z, life) now say what each means
(head: world position and a countdown held at 100; particle: offset in the head's transform and an age).
Fixed in review: nozle's GXSetVtxDesc arguments read `2 == 2 ? GX_INDEX8 : GX_INDEX16`; nozle petal 0x34 back to
`field_0x34` (written once, never read). Index 61.129 -> 60.457.
Open: kemuri5 `field_0x4`/`field_0x40`, kemuri6 `field_0x34`/`0x44`-`0x4C`, akari `field_0x34`/`0x3C`/`0x40` and
particle `field_0x38`, nozle `field_0x00`/`0x28`-`0x30`/`0x38`-`0x40` (both structs) have no confident reader.
kemuri6's `accelStep` is only ever 0. pturn's `active` is set to 1 and never cleared. NiceEffectWork's
0x24/0x2C/0x34-0x64 gaps are unmodelled. aji_02 and queen2 still cast userdata to EffAkariChargeData, a partial
view of EffAkariChargeWork private to eff_akari_charge_n64.c.
### Readability: effects pilot batch from the DeepSeek cleanup swarm (2026-10-19, DeepSeek V4.1 Flash, n9-q-ds1; reviewed by Claude Opus 5.5)

One DeepSeek V4.1 Flash agent per file, edits confined to that file, then an Opus pass over every rename. eff_break:
BreakEffect `type`, `timer` (head: the hold limit for state 3; particles: countdown/count-up and ty_data index),
`texOffsetY` (0x14, the V translation loaded into GX_TEXMTX0, not a world offset), `state` (0-5), `scale`;
effBreakEntry's second argument `maxTimer`. eff_fire: locals only (`scaleTable`/`tableCount`/`interp`,
`frameCount`, `tevColor`/`tevKColor`, `halfWidth`/`quadSize`/`halfSize`, `leftX`/`rowHeight`, `modelMtx`,
`color0`/`color1`, `packedYOffsets`); `col_tbl` and `scale_dt` keep their map names (symbols.txt, scope:local).
eff_mobj_broken: `gridX/Y/Z`, `spacing`, `remain`, `sinA`/`cosA`, `zeroAxis`, `pass`, `kPi`/`kTwoPi`/`kDeg360`/
`kOne`/`kMinY`/`kOffset`/`kDegToRad`; `k16605`/`k4967` stay value-named (0.16605 is the sine polynomial's cubic
coefficient, so the swarm's `kSinQuad` was wrong). eff_mugi: entry parameters `x`/`y`/`z`/`baseAngle`, `type`,
`dirSign`. eff_spirit: `mtxTrans`/`mtxRot`/`mtxScale` and GX enum names in effSpiritDisp. eff_blow_n64:
effBlowParticle `type` (0x00, the entry argument), `state` (0x04, was misnamed `type`: 0 respawns particles, 1 -> 2
when the head's life runs out, 2 deletes), `spawnVx/Vy/Vz`, `burstAx/Ay/Az` (added at life 4-6), `accX/Y/Z`,
`rotSpeed`, `spawnRotZ`, `alpha`, `age`, `delay`. eff_recover_heart_n64: RecoverHeartData `alpha`, `timer`, `delay`,
`amount` (0x40, drawn as one or two digits from texture 0x53; the swarm's `duration` was wrong) and `amountNegative`.
eff_small_star_n64: GX enum names only; `kaiten` is the map's name (kaiten$412) and stays. eff_sstar_n64:
effSstarN64Entry `targetX/Y/Z`, `speed`, `camVelX`/`camVelZ`/`horizSpeed`; Disp's `base`. eff_star_power_n64: a
comment on the 0x48..0x54 previous-frame copies only; the swarm's `#define` aliases over the global float pool
names were dropped. Matching build 32 files OK; all ten files natural-compile equivalent to the originals. Open:
header-defined structs (effFireData, MobjBrokenWork, EffStarPowerData, SstarWork, effSmallStarN64Data) keep their
field_0x names; EffStarPowerData 0x30 is the third color2 channel (driven like color2R/G); effRecoverHeartN64Entry's
`time` parameter is the recovered amount; RecoverHeartData 0x20/0x30 are written but never read.

### Readability: BattleCommand names (2026-10-19, Claude Haiku 5.5, n9-q-bcmd; reviewed by Claude Opus 5.5)

BattleCommand (0x574 bytes, battle_seq_command.h; BattleWork.commandMenu at 0x171C) replaces `unk8[]` with its five
menu tables: `actionTable[6]` (0x8, `BattleCommandAction` 0x14), `weaponTable[21]` (0x80, `BattleCommandWeapon`
0x1C), `operationTable[7]` (0x2CC, `BattleCommandOperation` 0x1C), `partyTable[8]` (0x390, `BattleCommandParty`
0x18) and `multiItemTable[3]` (0x450, `BattleCommandMultiItem` 0x18), each ending exactly where the next begins and
the last at `cursor` (0x498). The state fields are `commandState` (0x0, the BattleCommandInput state machine),
`menuFlags` (0x4), `actKind` (0x540, action mType; get_actkind), `savedActKind` (0x544, restored in state 0 after
the 0xB/0xC overrides), `operationKind` (0x548), `partyChangeId` (0x54C = BattleWork 0x1C68, BattlePartyAnimeLoad;
closes the BattleWork entry's open item), `multiItemKind` (u8 0x550), `cancelled` (u8 0x551), `restoreId` (0x554),
`restoreState` (u8 0x558), `idleFrames` (0x560, 0x384 frames switch the idle camera), `disableReason` (0x564) and
`eventId` (0x568). Every table entry's disable code is `mDisableReason` (action 0x10, weapon 0x18, operation 0x8,
multi-item 0x14): it is copied into `disableReason` when a greyed entry is picked, and `_get_msg` maps it to the
`btl_msg_cmderror_*` text (1 no FP, 2 no AP, 4/5 no Jump/Hammer target, 6 no escape, else "not now", which the
builders write as 3). The multi-item byte at 0xE is `mKind` (1 Item, 2 Double Dip, 3 Triple Dip). battle_menu_disp.c's
raw `wp + 0x1724/0x19e8/0x1aac/0x1b6c` and `cmd + idx * 0x1c + 0x8c` reads are now fields, and battle_seq_command.c
uses BattleCommand throughout. battle_seq_command.c field_/unk density 109.03 → 97.66 per kLOC; overall 69.987 →
69.911. Open: battle.h still carries the deprecated duplicate `BattleWorkCommand`/`BattleWorkCommand*` types with
the same layout, used by the party `__makeTechMenuFunc` builders; fold them into BattleCommand in a separate pass.
`field_55C`, weapon `field_0x4`, and the menuFlags bits beyond 0x1 stay unnamed.
### ExtPose field names and flags (2026-10-19, Claude Haiku worker ExtPose, n9-q-extpose; reviewed by Claude Opus 5.5)

The extdrv sprite crowds (bom, eki, gon, jin, las, rsh, tou, usu, yuu, nannpc and the gundan/teresa/moamoa units)
drive one `ExtPose` per sprite. Names, from extMain/extPoseDraw/extDrawShadow:
- `heading` (0x4): the sprite's facing angle in degrees. extMain compares it with the camera angle to pick which way
  the sprite turns; users set 90/270, `angleABf` results or random angles.
- `groundY` (0x8): world Y the shadow is moved to when `EXT_POSE_SHADOW_AT_GROUND` is set (evt_nannpc sets it from
  a script float).
- `texId` (0x10): index into `ExtWork.textures` and the per-texture display lists; also the sort key.
- `yaw` (0x14) eases 10% per frame toward `targetYaw` (0x1C), then writes the rotation into `*matrix`. 0x18 is
  unnamed.
- `ExtPoseFlags`: `EXT_POSE_VISIBLE` (0x1, drawn and sorted by texture), `EXT_POSE_BILLBOARD` (0x2, yaw follows the
  camera: camera angle when facing it, camera + 180 when facing away; without it yaw is left alone),
  `EXT_POSE_NO_SHADOW` (0x4), `EXT_POSE_SHADOW_AT_GROUND` (0x10). Single-bit literals on ExtPose `flags` use the
  enum; combined values (`|= 7`, `= 3`, `|= 0x13`) stay numeric, and the many `flags` literals in the same files on
  other structs (MoamoaPose, MoamoaWorkPose, KaronPoseEntry, FanPose, the yuu/nannpc entries) are not ExtPose bits.
- `ExtSortEntry` `{index, key}`; `ExtMakeTextureEntry.animName` (0x4, the animation `animPoseDisp_MakeExtTexture`
  renders); `ExtWork.shadowMtx` (0x3C, set by extLoadShadowMtx, concatenated into every shadow).

Build byte-identical (32 files OK). Cleanup index 55.254 -> 54.980 against the merge base (field_0x 10,872 ->
10,680).
### Readability: party_cloud, mot_roll, mot_ship (2026-10-19, Claude Haiku 5.5, n9-q-party2; reviewed by Claude Opus 5.5)

PartyCloud gets `maxDist` (0xC, the cap on `unk8`, the breath distance, and the divisor for the breath effect's
scale; 120 at init), `useTimer` (0x30, frames left in an event-driven use, from `sysMsec2Frame(party->unk16C)`),
`sfxId` (0x34, the psndSFXOn_3D handle, 0xFFFFFFFF when none) and `mode` (0x38, s16, cycles 0/1/2 per frame and
picks the hit-ray height 8/20/40 above Mario in getHitBreatheout2). The old trailing pad was
`field_0x30[0x48 - 0xC]`, so the struct measured 0x6C; it is now 0x48, matching the `__memAlloc`/`memset` size in
N__cloud_use. Nothing takes its sizeof and nothing outside party_cloud.c uses the type, so no offset or caller
moved. The breath effect userdata goes through effKumokumoData (`x/y/z`, `scale`, `rotation`, `camera`,
`color1R/G/B`, `color2R/G/B`). mot_roll's motStruct writes use MotRoll `velX`/`velY` (the names its other readers
already had); mot_ship's funemizu userdata goes through EffFunemizuData (`x/y/z`, `speed`, `yaw`, `camId`), the
0x1E0/0x1EC clears are `hitobjFront`/`hitobjPush = NULL`, and the cos term reads `slopeAngle` like the sin term
beside it. Review: every name checked against its readers and offsets, access widths unchanged (sfxId is s32
where the old compare read a u32; the compare against 0xFFFFFFFFu still converts to unsigned), MUST_MATCH and
`#else` both updated. Fixed in review: the `mode` comment said it selected the breath reach; it selects the ray
height. Matching build 32 files OK; mot_roll, mot_ship and party_cloud natural-compile. Index 46.227 -> 46.148.
Open, MarioWork: 0x2A4 (a motion flag word), 0x2A8 (read as an s32 sfx handle where the header has the f32
`wCamVal1`), 0x98/0x9C/0xB0/0xB4 inside the 0x98-0xC8 pad (mot_roll's reinit writes 6.0/-6.0 there), 0x2E (read
as a u16 in party_cloud where the header has the MarioMotion enum `currMotionId`), 0x39; 0x1DC and 0x1F0 (cleared beside the hit objects in
mot_ship) are still pad. PartyCloud 0x10-0x1C (the sway angle and its direction flag in getHitBreatheout2) and
`unk0`-`unk2C` keep their names. PartyEntry: `field_8` (flag word), `field_32`/`field_33`/`field_3B`, the raw u8
at 0x3D, the raw Vec at 0x94 (and 0x98/0x9C) in cloud's target setup, the raw s32 at 0x17C/0x180, `field_10C`/
`field_110` (direction copies) and `unk16C` (event use duration in ms).
### Readability: effects, yuu/gor/hom RELs, evt_img and evt_bero from the DeepSeek cleanup swarm, wave 3 (2026-10-19, DeepSeek V4.1 Flash, n9-q-ds3; reviewed by Claude Opus 5.5)

Thirty files, one DeepSeek V4.1 Flash agent each; 74a2d570 carried 25 of them and the five REL files (yuu_02,
yuu_03, hom_00, both evt_yuuranking) were left uncommitted in the worktree and land with the review. About 220
renames were traced to their consumers; 9 were wrong and are fixed: eff_kemuri9 `frameU`/`frameV` are two
consecutive U frames (TEXMTX0/TEXMTX1, blended), now `texFrame`/`nextTexFrame`; eff_pokopi_pcharge Disp's `xSpeed`
is the head's 0x38 (vx_data) added to each particle's x before the z rotation, a radial offset, now
`radialOffset`; evt_bero's `startIsAnchor` had the wrong polarity (0 anchors start at the hit object), now
`reverse` as in bero_get_position_normal; gor evt_yuuranking 0x1C `goal` is the player's final score/time (stored
into the rank table), now `result`, and the top case-2/3 block's `rowValue` (work's running value, not a row) is
`timeValue`; yuu_03 ryushi_disp `orbitRadius` is the x sway amplitude (3 * sin, x only), now `swayAmplitude`;
evt_ryushi_clr `halfScale` (half-cell parity offset) back to `half` and `zExtent` (xwp->unk10, the z spacing) to
`zSpacing`. Comments fixed: eff_stone_smoke scale_data2 is a repeating seven-frame cycle; eff_queen's 1000 means no
time limit; yuu4 gamerule's "ring"/"partition" comments replaced with what the code does (spins puro_*/pera_*;
states 2-4/6-8 push Mario along z at the particle speed, ramping in 2/6); eff_opuku_wgun's register note follows
`s` -> `offX`. Made consistent: gor evt_yuuranking now uses yuu's field names (`freeFlags`, `displayMode`, `result`,
`rankIndex`, `currentValue`; rank-table `rankValue`/`rowValue`), yuu's `playFlags` (bit 2 is the wait-button bit) is
`flags2` again, evt_bero keeps the file's `kind` for lwData[5], yuu_03 uses yuu_02's `wasAllocated`, and eff_starpoint's
pre-existing `lowAlpha` local (it feeds `delay`) is `delay`. Churn reverted: yuu_03's work/data ->
minigameWork/minigameData (siblings and yuu_03.h use work/data), `i` -> `index`, eff_opuku_wgun's copy of
system.c's initializeVec. Kept: hom_00 test_glare's `bounds` view of field_0xEC with MAPOBJ_FLAG_BBOX /
MAPOBJ_FLAG_DISPLAY_LIST, eff_holography's file-local 55 x 0x134 vertex-table struct, yuu_02's goto-free gate
crossing, GX enum names (each checked by value and enum family). Matching build 32 files OK; all 30 files compile
to objects equivalent to the pre-batch ones in both the matching and the natural build (tools/obj_equiv.py), no
linker-map name renamed, no new #define. Open: eff_ice `spin` is the phase driving the rotZ/rotY wobble;
eff_water_damage colorR/G/B (70, 180, 255) are never read in the file; eff_kemuri7 0x18..0x2C scale/rot names rest
on the init values (1, 1, 1, 0, yaw, 0) and only rotY is read; eff_kemuri9 `startLife` (0x2C) and `lifetime`
(0x3C) both hold a2 and only 0x3C is read; the rank-table views (A14/A74/AD4, PiantaParlorWork 0x14/0x74/0xD4) and
the YuRankWork `field_10`/`field_14` names sit on single-line #defines; yuu_03.h fields (Yuuminigame4Data unk08..unk1C,
RyushiEntry unk04..unk40) are still unkNN, with the agents' proposals in tools/swarm/data/c3_logs.
### Readability: MarioWork pad overruns and raw offsets in src/mario (2026-10-19, Claude Haiku 5.5, n9-q-mario2; reviewed by Claude Opus 5.5)

Pad overruns fixed: mobjdrv's `field_0x1DC + 4` is `hitobjFront`; mot_yoshi's clears are `hitobjFront`,
`hitobjHead`, `hitobjUnder`; mot_damage's `field_0x1F0 + 0x14` is `hitobjDamageReturn`; mario.c's slope reads at
`field_0x1F0 + 0x24/0x28/0x30` are `slopeAngle`/`slopeTiltX`/`slopeTiltZ`; mario_hit's two MUST_MATCH wallDist
stores (`*(f32*)&field_0x1C0[8]`) are plain `wallDist` in both builds. About 160 raw MarioWork casts converted in
evt_mario, mario_motion, mario_kemuri, mario_sbr, mario, mario_hit and mobjdrv. New names, each checked against its
readers and writers: `airTimerPrev` (0x54) and `hitobjStandOnPrev` (0x208), copied at the end of every marioMain
path, read by kemuri as the landing test; `peakYpos` (0x12C, marioSetPeakYpos); `floorY` (0x1C0, searchUnder's hit
y or -3000, the rounded ground y in marioChkGnd, the shadow height); `touchNpc` (0x1D4, marioGetTouchNpcPtr only:
no writer in C yet); `hitobjHead` (0x1F4, marioSearchHead*/kpaSearchHead results); `hitobjDamageReturn` (0x204,
the stood-on hit with attr 0x40000000, whose hitGetDamageReturnPos mot_damage returns Mario to);
`hitobjBalloon` (0x20C, chkBalloon hit from searchFrontWall, preferred for hitobjFront); `dirEventStart/Target/
Msec/Frame` (0x290-0x29C, marioSetDirEventMain; the setter isn't in C); `reloadMapOnBottomless` (0x2C8 u32,
N_marioReloadMapOnBottomlessOn/Off, marioBottomless). `field_0x11C` is a Vec (landing-distance reference in
evt_mario/marioLandOn, y the overhead clamp in mario_hit). Fixed in review: splitting `field_0x2AC` and `unk104`
left new overruns, now `field_0x2BC + 4` (the s16 coin-drop countdown in marioMain), `field_0x2B8` (yoshi's z
rotation in the display matrix, also marioChgJumpStandMotion's argument), `*(f32*)field_0x130` (roll Y scale) in
mario.c and `*(s32*)field_0x2B4` in peach.c; mot_damage's `field_0x28C + 0x14` (0x2A0) is `damageRootY`, an
anonymous union with `spinSpeed` (set_damage_root_ypos writes it from mot_hip, mot_plane, party_yoshi and
marioLandOn). Matching build 32 files OK; all ten changed C files natural-compile equivalent to the pre-branch
objects (tools/obj_equiv.py), mario_hit included, so the removed `#ifdef MUST_MATCH` blocks were redundant in both
builds. Open, MarioWork: the 0x98-0xC8 Vec block (party_vivian and mot_damage index `field_0x98` by byte; mario.c's
display matrix reads 0x98-0xC4 through it); 0x2A4 (a motion flag word; party_yoshi adds it to spinSpeed as an
f32); 0x2A8 `wCamVal1`, an f32 step in mot_damage but an s32 sfx handle in mot_hip; 0x114 and 0x164 (mario_hit's
last GND_PTR use: 0x164 = 0x114 - position.y, and 0x114 is the y of the Vec at 0x110 marioInit and peach seed from
position); `unk19C` is an f32 move direction set beside `directionView` everywhere but still a u8[4]; 0x52 holds
airTimer at the jump peak; 0x1C4, 0x1DC, 0x1F0, 0x1FC, 0x2AC/0x2B0 (per-motion scratch: mot_bottomless speed and
angle, marioLandOn's landing distance) keep field names; peach.c's `unk170 + 0xC` (0x17C) still overruns into
riderDelta.
### Readability: effects, battle_information, badge shop, evt_sub, sound and boss units from the DeepSeek cleanup swarm, wave 4 (2026-10-19, DeepSeek V4.1 Flash, n9-q-ds4; reviewed by Claude Opus 5.5)

Thirty-four files, one DeepSeek V4.1 Flash agent each (a16e74b4). About 150 renames traced to their consumers;
these were wrong and are fixed: gor evt_goki_sub_intpl_init's second pair is Z (the script feeds LW(0)/LW(1) to
evt_mapobj_trans x and z), `startY`/`endY` -> `startZ`/`endZ`; evt_gor_villager_urouro_check_func moves the wander
target (args 0/1) back to the home point (args 2/3) when it strays past args[4], so `newX`/`newZ`/`threshold`/
`prevX`/`prevZ` are `homeX`/`homeZ`/`radius`/`x`/`z`; macho_gance wariai_enzan_*'s first argument is the script's
LOOP counter, which counts down (`step` -> `remaining`), and wariai_enzan_1f_intpl(_w)'s args 5/6 are intplGetValue
modes, 14..27 mirrored, not times (`time`/`time2` -> `mode`/`mode2`; the mirrored second ratio is computed from the
first mode in retail, now said in a comment); eff_thunderflare's two RGB triplets feed GX_TEVREG0/1 (stage 1 blends
them by the texture), `colorR/G/B`/`color2R/G/B` -> `tevReg0R/G/B`/`tevReg1R/G/B` and the inlines
`flareSetTevReg0/1`; evt_badgeshop bottakuru's `filled` is also the badge_bottakuru_table index stored into the GSW
(`n` again) and bargain's `badgeCount` is a 2-bit slot value, `slotValue`; evt_twoddisp's loop counter kept the name
`offset` after it became an index, now `i`; eff_hagetaka_flush `color_index` -> `colorIndex`. Comments fixed:
gor mario_and_party_sleep's countdowns are for dozing off (LW(1) puts Mario in M_N_5A with the snore loop), not
waking; battle_information's drop table uses the first entry whose threshold is at or above the HP/FP percent.
eff_toiki's GXSetNumChans takes a count, `GX_TRUE` -> 1. Struct copies removed: eff_damage_star's DamageStarAnim
duplicated effdrv.c's EffectMayaAnim and eff_freeze's effFreezeSnowDustData the head of eff_snow_dust_n64.c's
effSnowDustData; both structs (with EffectMayaAnimData) moved to effdrv.h / eff_snow_dust_n64.h and the copies
deleted. Made consistent: effMissStarEntry's `lifetime` parameter is the spawn delay stored in `delay` (header
too); sound.h's SoundSSPlayCh/Ex prototypes use the .c's `name`/`useAram`. Kept: GX enum swaps (each checked by value
and family; snowfall's GXSetTevColor(1) is GX_TEVREG0), the unit enum constants, battle_information's drop-param
names (threshold, overall chance, count, per-drop chance), stardust2's `velX`/`velY`/`accelY`/`age`/`life`/
`vtxIndex`, evt_sub's SplineWork `knots`/`points`/`tangents` (spline_maketable's own names). Matching build 32
files OK; all 36 touched C files compile to objects equivalent to the pre-batch ones without MUST_MATCH
(tools/obj_equiv.py, old tree with old headers). Open: DamageStarData 0x24/0x3C/0x40 (life, displayed damage,
last retrace), effFreezeData 0x24/0x25 (vertex index, flicker timer), thunderflare `field_0x00` (written, never
read) and EffectMayaAnimData's fields stay unnamed; agents' proposals in tools/swarm/data/c4_logs.

### Readability: effects, battle icon/status and cam_shift, wave 2 of the DeepSeek cleanup swarm (2026-10-19, DeepSeek V4.1 Flash, n9-q-ds2; reviewed by Claude Opus 5.5)

Twenty files, one DeepSeek V4.1 Flash agent each, then an Opus pass over every rename. Eight agents copied a header
struct into their .c file to rename its fields; the names now sit on the header structs and the copies are gone:
CamShiftWork (`bank` is camRoadMain's bank argument, `targetBank`, `stepScale`, `stickAccum`, `scaleActive`,
`prevTime`, `stickAccumMax`/`maxStep`/`scaleRate`, `lastMarioX/Z`), effMagic1Data (`offsetX/Y/Z`, `velX/Y/Z`, `scale`,
`age`, `timer`, `alpha`; unit_kamec reads `scale` and `offsetX`), effKemuri4Data (`active`, `lifetime`, `textureIndex`,
`scaleX/Y/Z`, `rotX/Y/Z`, `matrix`, `spin`, `driftScale`, `floating`, `swirl` and its velocity/accel/jerk,
`sinAngle`/`cosAngle`), effHokoriData (`velY`, `accelY`, `alpha`, `scaleY`/`scaleXZ` and steps, `tevReg0R/G/B` and
`tevReg1R/G/B` for GXSetTevColor(1/2, ...)), effFlowerData (`velX/Y/Z`, `accelY`, `accelYDelta`, `angleX/Y` and
their velocities, `life`, `age`), EffThruHammerData (`position`, `scaleX/Y/Z`, `life`, `age`, `rotY`/`rotZ`, `alpha`,
`texScrollA/B` (V offsets loaded into GX_TEXMTX0/1) and speeds, `delay`, `tevReg0R..tevReg1B`), effTogeFlushData
(`angle`, `angleSpeed`, `innerScale` (the +/-4 quad), `outerScale` (+/-8), `innerAlpha`, `outerAlpha`, `life`,
`frame`) and BattleIcon (0x20..0x60: btlevtcmd_BtlIconJumpPosition's `moveStartPos`, `moveCurrentPos`,
`moveTargetPos`, `moveFrames`, `moveSpeedXZ`, `moveSpeedY`, `moveAngle`, `moveDist`, `moveDirection`; 0x5C is never
touched and stays `field_0x5C`). battle_status_effect's BattleWork overlay is gone too: BattleStatusChangeEntry and
BattleStatusChangeMsgWork moved to battle_status_effect.h and BattleWork has `statusChangeMsg` at 0x18D88, so
battle_seq.c clears and sets `statusChangeMsg.mAnnouncedFlags` by name. BattleStatusChangeMsgSetAnnouce keeps its
byte-offset base: `&_battleWorkPointer->statusChangeMsg.entries[type]` compiles 8 bytes longer. Map names restored:
`main_dl` (eff_magic1, scope:local), `y_data` (eff_butterfly) and the fifteen `float_*_8041f7d8..f810` .sdata2 names
in eff_opuku_jetwrain (the swarm's `heightDamping` was the velocity field's 0.98 damping anyway). Wrong names fixed:
eff_thruhammer 0x10/0x14/0x18 are `dirX/dirY/dirZ`, 0.5 times the rotZ-rotated up vector (-sin, cos, 0); the swarm
called 0x14 `dirZ`. eff_kemuri8's `frameU`/`frameV` (fields and Disp locals) are `frame`/`nextFrame`: both feed U
offsets, of GX_TEXMTX0 and GX_TEXMTX1, for a 16-frame blend. eff_timestamp's `vertexAlpha[10]` is `pieceAlpha` (one
GX_TEVREG0 alpha per drawn piece) and `colorAlpha` is `whiteFade` (GX_TEVREG1 alpha; TEV stage 1 lerps the colour
toward white by it). eff_rothammer_fail's three identical record structs are one effRotHammerFailRecord (head plus
one particle). eff_hokori's GXSetNumChans(GX_FALSE) is GXSetNumChans(0). Kept as the swarm wrote them: eff_akari_pose,
eff_bomb, eff_expbom, eff_rubble (with its rubbleVtxData overlay of stg_hahen_v), eff_stardust (strip offsets are now
`[N * 14]`), eff_horao_spell (redundant externs dropped), battle_status_effect's field names. The checker's cam_shift
`flag`/`step` and eff_bomb `flag` hits are locals that share a name with unrelated symbols. Matching build 32 files OK;
all 22 changed C files (the 20 plus battle_seq.c and unit_kamec.c) natural-compile equivalent to the pre-batch
objects. Open: effTimestampN64Data `colorR/G/B` (70/180/120) are written but not read in the unit;
battle_status_effect's BattleStatusChangeEntry serves both the per-unit list (`mCounter` is a 10-frame stagger there,
`mFlags` the caller's p1) and the message list (a countdown and a Mario/partner/enemy bit set), so those two names fit
only one use each; effKemuri8Data in the header still overlaps the file-local effKemuri8Particle.

### Readability: battle_event_cmd's BattleUnit/BattleUnitPart fields, and the pose and event slots tree-wide (2026-10-19, Claude Haiku 5.5 then Claude Opus 5.5, n9-q-btlevt)

Haiku's three commits on battle_event_cmd.c, checked field by field: the 48 offset_to_field conversions land on the
right members (BattleUnit seMode 0x188 / seModeSrc 0x1C0, BattleUnitPart seMode 0xC0 / seModeSrc 0xF8, each mode's
sfx names, delay and intervals at the right width, part statusPoseKind 0x215 u8, motionSpeed 0x20C f32);
GetConsumeItem's cursor read is `commandMenu.cursor[11].field_0` (0x171C + 0x498 + 11 * 0xC = 0x1C38); the kind
literals are UNIT_MARIO / TYPE_PARTNER_MIN / TYPE_PARTNER_MAX. _MoveSoundControlCore's BtlUnitSeMode* prototype
matches its only two callers. Opus then named what Haiku had proved but left as `field_0x` because other files use
it: BattleUnitPart 0x1C0 `poseId` (the animPoseEntry handle, -1 when empty, released by animPoseRelease) and 0x1C4
`poseName` (char[0x40]: btlevtcmd_AnimeChangePoseDirect strcpy's the animation name there, btlUnitPartsDisp passes
it to animPoseSetAnim, black_peach strcmp's it, BtlUnit_Entry clears its first byte); BattleUnit 0x29C
`attackEventCode` (the action script battle_seq starts into attackEventId: weapon mAttackEvtScript, the default
wait/defend/appeal/escape scripts, BtlUnit_GetData 2/3) and 0x2B4/0x2B8 `entryEventCode`/`entryEventId`
(btlevtcmd_SetEventEntry's script, started for each unit at battle start; the id is cleared once evtCheckID says it
ended). Every user in src/ and rels/ went over, raw `(u8*)part + 0x1C0` casts included (battle_unit, battle_seq,
battle_mario, battle_disp, eff_majinai, unit_heavy_bom, batten_leader, black_peach). battle_disp's
`part->field_0x1C0 + 0x44` and `field_0x1C0[0x54]`/`[0x55]` indexed past the old 0x44-byte block: they are
`partFlags`, `statusPoseStep` and `statusPoseKind`. _MoveSoundControlEndCore now takes (BattleUnit*, BtlUnitSeMode*)
and _MoveSoundControlCore's owner is a BattleUnitPart*. Both handle checks keep the unsigned compare against
0xFFFFFFFF: `sfxHandle != -1` is an instruction shorter and broke the checksum. Left: BattleUnit 0x2A0 and 0x2A8
(attack flags: bits 1/2/8/0x10 steer battle_seq's action dispatch) stay unnamed. Matching build 32 files OK; all 19
changed C files compile with and without MUST_MATCH, and the natural objects are equivalent to the pre-branch ones
(tools/obj_equiv.py). Cleanup index 43.102 -> 42.632 (field_0x 5554 -> 5434, raw_offset 1981 -> 1876).
### Readability: party_motion, party_move, party_nokonoko and party_yoshi offsets (2026-10-19, Claude Haiku 5.5, n9-q-party3; reviewed by Claude Opus 5.5)

PartyEntry gains `followDist` (0x40, the distance from Mario at which party_motion_stay starts the partner walking;
the behind/beside routines and partyWalkInit/partyMoveFlyInit reset it to 50) and `velX`/`velZ` (0xA0/0xA8, the
push velocity partyMarioHosei sets from Mario's speed and partyGetMoveDirSpd adds to moveDelta; 0xA4 is never
touched). With those and the existing `targetPos`, `moveAngle`, `targetDist`, `motionStep`, `timer`, `radius`,
`height`, `speed`, MarioWork `field_0x2B0`, `field_0x11C`, `field_0x128`, `dispReqDir`, `hitobjHead` and
`hitobjDamageReturn`, the four files' byte-offset accesses (raw casts plus the M_F/NKF macros) drop from 236 to 149,
MUST_MATCH arms included where the named form is the same expression. Kept raw because the named form compiles
differently: party_nokonoko's twelve `NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C))` updates (`party->field_10C`
reschedules each add; the other 0x10C/0x110 uses are named), and its `u16 mot = *(u16*)&mario->currMotionId`
(the plain enum read is 4 bytes longer). MarioMotion is a u16 enum (`enumsalwaysint off`, max 0xFFFF) at 0x2E, so
that cast reads the field itself; party_yoshi's `mario->currMotionId != 0x1A` compiles the same as the cast and is
plain. Matching build 32 files OK; the four C files natural-compile to objects equivalent to the pre-branch ones
(tools/obj_equiv.py, old tree with old headers). Open, MarioWork (mario.h was owned by q-mot2 during this branch):
0x2B4, the f32 z twin of `field_0x2B0` (party_yoshi's stick inertia; L_yoshiFlyMove negates it); 0x2A0/0x2A4/0x2A8
in party_yoshi are Yoshi's flutter y velocity (the `spinSpeed`/`damageRootY` union, clamped at -10), an f32 the
velocity and it add into each other (position.y = 0x2A8 + 0x2A4), and the flutter base y (`wCamVal1`'s slot, f32
here), so 0x2A4 wants an f32 and the union another member; 0x39 an s8 flag; 0x52 an s16 (airTimer at the jump peak); 0x18C an f32 fly speed (L_yoshiFlyMove,
0.1 * the inertia length); 0x1DC and 0x1F0 HitEntry pointers checkWall stores; the 0x98/0xA4 Vec pair party_yoshi
copies as one YoshiZeroVec6 (party_vivian and mot_damage index `field_0x98` by byte too). PartyEntry still raw in party_move: 0x3D, 0x3E
(u8), 0x44 (u32, set to 0x10 beside followDist) and 0x52 (s16 contact counter partyMarioHosei returns: up to 100 while touching Mario, decays after).
### Readability: MarioWork display Vecs, airTimerPeak, moveDir and the 0x2A4-0x2AC motion words (2026-10-19, Claude Haiku 5.5, n9-q-mot2; reviewed by Claude Opus 5.5)

The 0x98-0xC8 pad is four Vecs, checked against mario.c's marioDisp/marioDispBlurSub matrix: `dispOfs` (0x98) and
`dispOfs2` (0xA4) are added to position (blurPos for the blur copies) for the final translation; `dispPivot` (0xB0)
is translated by -pivot before the display rotations and +pivot after them; `dispRot` (0xBC) holds x/z rotation
degrees (y is never read; the y rotation is unk1A8 - targetYaw). 0x52 is `airTimerPeak` (s16): marioJump and
koopa_motion store airTimer when jumpVelY turns non-positive, landing clears it, marioBoots (< 6) and
marioChkHipAttack (>= 10) test it. 0x19C is `moveDir` (f32), the toMovedir() direction set beside directionView
(evt_lecture saves it as stMdir4); the worker's `moveDir`/`wallYawCopy` union was dropped in review, since
mario_hit's three wall checks just set the move direction to wallYaw. 0x2A4/0x2A8/0x2AC are per-motion scratch words
that each motion types differently (mot_slit s32 counter, mot_ship anime frame, mot_roll flag word, mot_hammer s16,
mot_jabara s32 y): the unions hold the f32 view (`field_0x2A4`; the existing `wCamVal1`) and the s32 sound handles
`motionSfxId0`/`motionSfxId1` (mot_hip's two loops; mot_roll keeps its handle at 0x2A8 too). Review renamed the
worker's `unk2A4`/`sfxId2A4`/`sfxId2A8`, which embedded offsets. `field_0x2AC` is f32; its only named users (mot_plane,
mot_damage, mot_bottomless) read it as f32, and mot_roll/mot_jabara's s32 stores stay raw, so no codegen moved.
mot_hip (55 sites), mot_jump (24) and mot_plane's MarioWork sites are named; mot_jump's MW/MWS16/MWS32 and mot_plane's
MWVEC are gone (mot_jump keeps MWF32 for 0x2B4, mot_plane MF32/MFVEC for MotionPlaneData and the 0x18C pad). Matching build 32 files OK;
all 17 changed C files compile to objects equivalent to the pre-branch ones in both the matching and the natural build
(tools/obj_equiv.py, old sources with the old header). Overlap: n9-q-party3 converts party_yoshi's MarioWork casts to
existing fields only (field_0x2B0, field_0x11C, hitobjHead, hitobjDamageReturn) and uses none of the renamed ones;
its party_yoshi still has raw 0x98/0xA4 (dispOfs/dispOfs2), 0x19C (moveDir) and 0x2A4/0x2A8 that can take the new
names after both land. Open, MarioWork: raw 0x52 in koopa_motion, mot_ship, mot_jabara, party_yoshi and evt_mario
(evt_koopa_hip_attack); raw 0x19C in mot_hammer, party.c, party_yoshi; the 0x2A4-0x2AC raw users above; 0x18C (unk18C: a speed
copied from baseSpeed in mot_plane, mot_ship and party_yoshi) is still a pad; 0x114/0x164, 0x1C4, 0x1DC, 0x1F0,
0x1FC, 0x2B0/0x2B4/0x2B8 keep field names; peach.c's `unk170 + 0xC` still overruns into riderDelta.

### Readability: DeepSeek swarm wave 5, own-header mode (2026-10-19, DeepSeek V4.1 Flash agents; reviewed by Claude Opus 5.5, n9-q-ds5)

Eleven agents each cleaned one file plus its own header: imgdrv (ImageShadow, ImageEntry), pmario_sound
(SoundEnvList, SoundEffectEntry, BGMListEntry, PaperSoundBGM), and the eff_laser, eff_ripple, fireworks, guruguru,
magic_horao, sunlight, snow, vapor and rappa effect structs. The agents' checker compiled only 12 header users, so
the batch left the full build broken; the review carried the imgdrv renames through the rest: dmo_00 (`matrix`),
evt_img (`posX/posY`, `captureWidth/Height`, shadow `left/top/dstFormat/texFormat/offscreenId/captureMode`,
`loopTimes`, `startTime`), evt_mobj, evt_cloud and fadedrv (the raw `*(OSTime*)((u8*)img + 0x110)` writes are now
`startTime`) and gor_02's `set_depthPlaneOffset` (raw `+ 0x11C`). Found by compiling all 312 users of the changed
headers by hand with `-maxerrors 50`, with and without MUST_MATCH; only these files failed.

Names corrected in review:
- ImageShadow 0x18 `srcFormat` -> `texFormat`: it is the GXInitTexObj/GXGetTexBufferSize texture format (the
  copy's destination format is `dstFormat`, 0x14); nothing is a source format.
- PaperSoundBGM 0x10 `fadeTimer` -> `scopeTimer`: psndBGMScope sets 0xFF (in scope, level 100), 0 (out of scope,
  the BGM follows psnd.unk58) or 0x1E (frames until out of scope); psndBGMMain counts it down. No fade.
- EffGuruguruData 0x10/0x14 `scale`/`targetScale` -> `radius`/`targetRadius` (the particles' orbit radius;
  `drawScale` is the matrix scale), and the entry's last parameter likewise.
- effRappaN64Entry's `angle`/`angleOffset` -> `yaw`/`pitch`: theta and phi of the direction
  (cos phi sin theta, sin phi, cos phi cos theta); phi sweeps from `pitch`.
- PaperSoundEffectDetail (pre-existing names, exposed by the agent's `slot->pan = entry->volume`): 0x9 is the
  entry's base volume (`vol * baseVol / 127`), so `pan` -> `baseVol`, and 0xA, passed to SoundEfxSetPan /
  SoundSSSetPanCh, `unkA` -> `pan`. Its `padB[3]`/`pad16`/`pad1C` pointer views became `reverb` (0xB), `unkC` (u16,
  SoundEffectEntry.field_0x10, compared when a slot is evicted), `unk16` and `Vec position` (0x1C).
Checked and kept: `depthPlaneOffset` (map name `set_depthPlaneOffset`, scope:local in mri and gor), SoundEnvList
`span` (calc3D's parameter), `startTime` (OSTime at 0x110; ImageEntry and PaperSoundBGM become 8-aligned, sizes
0x158/0x38 unchanged and psbgm sits at 0x803D9C48). `#include <dolphin/os.h>` in imgdrv.h supplies OSTime. Grade:
121 new names checked, 5 wrong (srcFormat, fadeTimer, guruguru scale/targetScale, rappa angleOffset), about 4%.

Matching build 32 files OK. Natural (no MUST_MATCH) objects of all 312 header users equal the pre-batch ones
(284 byte-identical, 28 obj_equiv-equivalent). Cleanup index 45.810 -> 44.786 (field_0x 6,684 -> 6,059, raw
offsets 2,309 -> 2,262). Open: SoundEnvList 0x5, SoundEffectEntry 0xE/0x10/0x11, effMagicHorao 0x14/0x1C,
effRappa 0x1C/0x20, EffRipple 0x14/0x28 and effLaser 0x24 stay field_0x; SoundEnvList positions are still u32.
### Readability: effects, evt_map/evt_uranaisi, Glitz Pit RELs and boss units, wave 6 of the DeepSeek cleanup swarm (2026-10-20, DeepSeek V4 Pro, n9-q-ds6; reviewed by Claude Opus 5.5)

Seventeen files, one DeepSeek V4 Pro agent each (the first full Pro wave), then an Opus pass over every rename. About
180 names, constants and comments checked; 11 needed fixing, all but one in comments or GX enums. Wrong name: tou2's
evt_tou_set_ranking_win_lose first argument is 0 after a win and 1 after a loss (tou_03's match script), so `won` is
`lost`. Wrong GX families: eff_rothammer_success's GXSetNumChans(GX_COLOR0)/GXSetNumTexGens(GX_TEXCOORD1) are counts,
back to 0/1; tou's monitor_tev GXSetTevSwapMode(..., GX_CH_RED, GX_CH_RED) takes swap tables, now GX_TEV_SWAP0.
Comments fixed: eff_mahojin's TEV callback blends the colour toward (E8,00,E8) by mahojin_rate (GX_CC_A0 as the lerp
factor) and passes alpha through (the agent said alpha fades); its rim-piece comment claimed lit/dim pieces, now says
which animations play; eff_kiss's four .sbss2 colour templates: only 0x80423310/18 keep black RGB, 0x80423314/1c get
RGB from color_rotation_data; FallWork.height is the target's hit position above the floor (elements within radius/2
land at y + height), not a drop height, and `roll` turns about the x = -200 pivot (no "shell rim"); evt_map's
effFire field_0x88 makes effFireDisp multiply by the map blend colour (TEV stage 3, CPREV * C1) when map blending is
on; aji_04's msg callback state 3 is the A/B press at a 0xFFFE wait point (msgdrv's choiceState 2), not a choice, so
lwData[5] counts waits passed. Struct copy removed: tou2.h's BeltData was effStarStoneData (the belt is
effStarStoneEntry(9, ...)), as tou.c's already was in this wave; tou2 now casts to effStarStoneData (`rotY`). Made
consistent between the tou and tou2 RELs' duplicate structs: DispWork `scroll`/`targetScroll`/`page`, RuleEntry
`kind`/`param` (NpcBattleInfo.battleRules[0]/[1]; tou.c had `type`/`field_0C`), fighter `defaultScore`. Kept: eff_fall's
FallWork names (unk50..58 stay unnamed, a per-type vector), eff_splash `delay`/`gotColor`, rankup `state`/`alpha`,
rothammer_success `rotSpeed`/`life`/`frame`/`lifeMax`, kumokumo `rotation`/`frame`, evt_uranaisi `storyId`/`mode`/
`lastLabelId`, gor_irai `cursorX/Y` and PAD_TRIGGER_L/R, unit_kamec_white's attribute/status/token constants (values
and enum families checked), unit_boss_koopa's field_18FF0 offsets (the command-disable counters). Matching build
OK; all 17 C files and the 16 users of their changed headers (plus eff_fire.c) natural-compile equivalent to the
pre-batch objects (tools/obj_equiv.py, old tree with old headers). Open: msgdrv.h's `choiceState`/`choiceMade`/
`choiceIndex` describe the 0xFFFE button-wait, not a choice; BattleWork field_18FF0 still wants named slots; the tou
and tou2 RELs keep separate copies of RankEntry/FighterEntry/RuleEntry/DispWork.
### Readability: address-named constants back to literals, tree-wide (2026-10-20, Claude Opus 5.5, n9-q-addrall)

`tools/addr_literals.py --all --apply` over src/ and rels/*/src: 892 objects in 128 files became literals (462 f32,
5 f64, 212 strings, 168 Vec and 45 GXColor compound literals), each kept only where its unit's matching object stays
obj_equiv-equivalent. Address-named references 13,757 -> 10,928; cleanup index 41.05 -> 37.09. What stays named:
objects a MUST_MATCH conditional names (match-only code is left alone), objects another source or header names,
objects whose address or sizeof is taken (`STRING((char*)&float_0_...)` script arguments, `*(GXColor*)&dat_...`),
volatile ones, and the ones whose literal pools or schedules differently from the named object (named objects come
first in their section in definition order, literals in first-use order), which the per-candidate search rejects.
Float spellings are the original initializers, so a few stay long where the shorter spelling is another value
(0.29999998f is not 0.3f, 0.040000003f is not 0.04f); three strings keep alignment NULs the object needs.
Review: all 128 files also compile without MUST_MATCH; 121 natural objects equal the pre-sweep ones, and the other 7
(shadowdrv, evt_shop, winmgr, mot_hammer, mot_jabara, mot_stay, party_bomhei) differ only in pool layout, with every
load reading the same bytes as before. The tool also rewrote retail names inside comments into literals (nonsense
like "the named `30.0f`"); those comments in battle_disp, battle_event_cmd, sac_genki, eff_nice and himo were
restored or reworded, two orphaned section headers went (imgdrv, eff_lensflare_n64), and the tool now leaves
comments alone and drops a header that only headed removed declarations right before a function. Matching build OK.

## battle_seq_end: exp star icons typed (2026-10-20, Claude Haiku 5.5, reviewed by Claude Opus 5.5)
`BattleSeqEndWork.iconPtr` is `IconEntry*[0x13]` (iconNameToPtr's return type), so the seven
`(const char*)(work->iconPtr[j] + 0x18)` casts read `work->iconPtr[j]->name` (IconEntry's inline name array at 0x18) and
the two `(u32)` stores go; `_lvup_spot_move`/`_lvup_spot_off` write `color.a` instead of `((u8*)&color)[3]`. Matching
and natural objects unchanged apart from debug info. Open: BattleSeqEndWork unk0 (read only by a switch whose cases
never fire), unk14/unk28 (written zero, never read), unk1C/unk20 (unused); the file's remaining index points are
address-named data (str_/dat_/lbl_/float_ symbols that can't become literals) and MUST_MATCH-only fn-pointer casts.
### Readability: evt_win unit-window symbols named (2026-10-20, Claude Haiku 5.5 + Claude Opus 5.5, n9-q-evtwin2)

Nine `unk_` symbols in evt_win named with tools/rename_symbol.py (symbols.txt, evt_win.c/.h, gor_01/gor_03). The
three WinMgr callbacks and their descriptor table are clones of evt_shop's `point_disp`/`help_disp`/
`help_main`/`window_desc` (same sizes, same bodies), so they take those names under the file's `unitwin_` prefix:
`unitwin_point_disp` (shop-point counter), `unitwin_help_disp`, `unitwin_help_main`, `unitwin_window_desc`; the
fourth callback, which draws the item name and a price over each object, is `unitwin_selltable_disp` after
`evt_unitwin_selltable_setup`. `evt_unitwin_select_func` is `select_evt`'s user func (pairs with
`evt_unitwin_main_func`/`main_evt`); `coin_count_sfx_timer` throttles SFX_COUNT_COIN1 to one per 7 frames. The two
USER_FUNCs the gor RELs call: `unitwin_set_taken` (gor_01's badge shop passes the bought item; a nonzero value marks
the object taken when its use event ends) and `unitwin_lottery_disp_onoff` (gor_03's lottery board; flag 0x100 shows
the ticket-number window), named like `unitwincoin_disp_onoff`. Haiku's first pass used camelCase (`unitwinHelpDisp`,
`coinCountSfxTimer`, ...); the file's names are snake_case, so the review respelled them. Matching build OK; the three
changed C files compile without MUST_MATCH. Landing needs `--allow-protected` (config/G8MJ01/symbols.txt).
### Review: n9-q-kemuri (eff_fire, eff_opuku_wprotect_n64, eff_kemuri) (2026-10-20, Claude Opus 5.5)

Names traced to their consumers. eff_fire `field_0x38` is not an intensity: Disp2 writes it to all four channels of
TEVREG2, but only A2 is read, as stage 1's lerp weight from the colorIndex1 frame's texture alpha to the colorIndex2
frame's, so it is `frameBlend` (Main sets 0x80, a half crossfade). 0x68..0x74 are Disp3's bounding box in screen
pixels (x + 304, 240 - y, clamped to 608x480), and 0x70 starts as the y-up maximum and ends as the y-down top, so
`screenLeft`/`screenRight`/`screenTop`/`screenBottom` (were screenMinX/MaxX/MaxY/MinY, wrong for 0x70/0x74 after the
flip). wprotect's `drop*` arrays rise (velocity eased toward +10, respawn past y 480) inside the water shell, so they
are `bubble*`; `alpha2` is TEVREG1 alpha, the shell's alpha where its texture alpha is 0 (`baseAlpha`). Kept:
eff_fire `scaleX`/`scaleY`/`useMapBlend`, wprotect `lifetime`/`timer`/`colorR/G/B`/`alpha`, eff_kemuri GX_LIGHT_NULL,
`vtx + 28` (WprotectVtx is 14 bytes). GX enums checked against their parameters; the remaining eff_fire casts became
GX_CA_KONST and GX_TEV_COMP_A8_GT, wprotect's position-matrix 0s GX_PNMTX0. Matching build OK; all 13 objects built
from the changed sources and users (eff_fire, eff_kemuri, wprotect, evt_map, battle_event_default, battle_database_las,
las_29, unit_bubble/phantom/hermos in each REL that builds them) compile without MUST_MATCH and are obj_equiv-equivalent
to the pre-branch ones.
### Readability: BattleAudienceKind fields, the audience item weapon and BattleWork's force fields (2026-10-20, Claude Haiku 5.5, n9-q-audience; reviewed by Claude Opus 5.5)

BattleAudienceKind 0xC/0x10 are `offsetX`/`offsetY` (added to the member's position for a thrown item), 0x14 is
`apWeight` (per kind, times the member count, summed and square-rooted in BattleAudience_ApRecoveryBuild), 0x18/0x1C
are `scaleX`/`scaleY` (times the texture size in BattleAudienceDispAudience). BattleAudience 0x137FC is
`BattleWeapon itemWeapon` (0xC0, the thrown item's weapon_data_* copy; the combo bonus adds to `hpDamageWork[0]`,
battle_attack_audience's `_damage` passes it to BattleCheckDamage), and BattleAudience_Case_ActionCommandBad takes the
BattleWeapon* battle_ac.c already passes. battle_audience.h now includes battle_database_common.h (no cycle; that
header includes only types and mario_pouch.h). Review: the worker's 0x19084 for the forced reel value was wrong
(battle - 0x6F84 is 0x1907C). BattleWork 0x19068 is `forceFlags`, a word nothing in the game writes whose bits each
force a random outcome: 0x1/0x2 object falls (0x2 onto the audience), 0x10/0x20/0x40 stage destroy A1/A2/B,
0x80/0x100/0x200 iron frame, nozzle work, nozzle turn, 0x10000 the audience count from 0x19074
(`forceAudienceCount`), 0x20000 frequent audience item throws, 0x40000 the 1-in-20 audience kind swap, 0x80000 bingo
reels stopping on 0x1907C (`forceSlotValue`); 0x19078 is `forceAudienceKind` (1-12 maps to one audience kind for the
whole crowd). battle_event_cmd, battle_stage_object, battle_seq_command and the gor lecture unit now read those names
(0x19080/0x19084, the lecture's message-window flag and id, stay a pad). The `base - 0x6F98` reads in
BattleAudienceSettingAudience, BattleAudienceCtrlProcess, BattleBreakSlot_Main and BattleBreakSlot_PointInc stay raw
with a comment naming the fields: `wp->forceFlags` recomputes the addis per access (+1 to +4 instructions), and
`((BattleWork*)(base - 0x20000))->field` adds a subis, so the hoisted `wp + 0x20000` pointer is the original shape.
Also: the dead `aud` locals in BattleAudience_CheckReactionPerPhase/BattleAudienceItemCtrlProcess went (the one read,
`aud + 0x137B0`, is `audience->typeWeights[7]`), and the combo bonus is `hpDamageWork[0] += itemCombo - 1`. Matching
build OK; all 161 C files that changed or include battle.h/battle_audience.h/battle_audience_kind_data.h compile, with
and without MUST_MATCH, to objects equivalent to the pre-branch ones (tools/obj_equiv.py --tree, old tree with old
headers).
### Readability: winmgr select/party names, win_root fb callbacks, Luigi book strings (2026-10-20, Claude Haiku 5.5, n9-q-winui; reviewed by Claude Opus 5.5)

Renamed with tools/rename_symbol.py (symbols.txt, sources, headers, gor_02/tik_11): `unk_80237448` ->
`winMgrSelectCursor` (returns `select->cursor`, read by evt_win_item_select), `unk_802379e4`/`unk_80237a2c` ->
`winMgrSelectCmpTypeSort`/`winMgrSelectCmpStarPrice` (qqsort comparators on `type_sort_order` and
`star_piece_price`), `unk_8014d5dc`/`unk_8014d338` -> `winRootFbCopy`/`winRootFbDraw` (copy the EFB to
`work->fbCopy`, then draw it full-screen), `unk_800d06b4` -> `pouchHiraganaToKatakana` (SJIS 0x829F..0x82F1 shifted
to katakana for the sort keys). The two USER_FUNCs map scripts call, by what they check: `unk_80239b2c` ->
`party_can_rank_up` (some joined partner is below the rank cap: 1, or 2 once GSWF(1186) is set; gor_02's
Merlon) and `unk_802399f8` -> `party_can_transfer_rank` (two or more partners, one with a rank to give and one
below the cap; tik_11's shop moves a rank from one partner to another, `unitwin_setpartylank` +1/-1). Snake case
after the gor USER_FUNCs `unitwin_set_taken`/`unitwin_lottery_disp_onoff`. Strings named by their table role, like
`item_000_name`: `str_802F4CE8..` -> `luigi_book_{1-5}_page{1-12}` (the `luigi_book_N` entries),
`party_labelname_table`/`mario_status_labelname_table` entries -> `labelname_<suffix>`, `mario_status_name_table`
-> `status_name_hp/fp/bp`; `str_802FFD38..` -> `party_name_msg0..6` (the `name_party<N>` keys; the content name itself
clashes with the literal), `dat_80421a14..24` -> `waku1_color`/`waku3a_color`/`waku3b_color`/`waku4_color`/
`waku5_color` (the frame colours winMgrDisp passes to winMgrDispWaku1..5). win_root's
`**(TEXPalette***)(work + 0x28 ... + 0xA0)` reads became `*(TEXPalette**)work->winTpl->data` and the anim clip read
`anim->clip`. Review: the worker's names for winmgr's select-layout Vec/GXColor constants
(`select_disp_tmpRow`, `select_luigi_colorWhite`, ...) only restated the one local each initialises, with a
`select_luigi_` prefix for select_disp_luigi that reads like the `select_luigi_desc` table; they went back to their
address names (`vec3_802ffa98..`, `dat_80421a28..74`), which addr_literals left named because they must stay named.
`evtPartyTechMixed`/`Low`, `winMgrWakuColor*` and `partyNameMsg*` were respelled as above (winmgr's data is snake
case; the evt prefix belongs to the evt core). Matching build OK; all seven changed C files natural-compile
equivalent to the pre-branch objects (old tree with old headers), differing only in the renamed extern names.
Landing needs `--allow-protected` (config/G8MJ01/symbols.txt).
### Readability: battle_seq_command weapon flag (2026-10-20, Claude Haiku 5.5, n9-q-bstage)

BattleCommandWeapon 0x4 is `mEnabled` (u32, type unchanged). The select code only accepts a weapon whose 0x4 is nonzero
(_btlcmd_UpdateSelectWeaponTable sets 1 where mDisableReason is 0 and 0 otherwise, and the random-weapon candidate loop
and the confuse path skip zero entries), and it is the same role as the BOOL mEnabled at 0x4 of BattleCommandAction and
BattleCommandOperation. All 17 uses in battle_seq_command.c renamed. battle.h's BattleWorkCommandWeapon 0x4 stays
`field_0x4`: the unit party files (unit_party_sanders/christine/...) write it through that struct, so renaming it is a
separate change touching src/unit/party. Left unnamed in battle_seq_command: unit 0x2A8 (an s32 of event-state bits
read in battle_seq.c and battle_disp.c; no single bit meaning is proven), the BattleWorkHpGauge reads in
BattleDrawEnemyHPBar and the enemy HP gauge (field_0x2/0x8/0x14/0x18; meanings not proven from the draw code alone),
the `(u8*)wp + 0x16F..` action-record counters (their index into field_16F1C does not map 1:1 to the
BattleActRecordConditionType values). Also kept: the fn_ptr_cast calls to btlDispTexPlane and the other casts (they
match the original's call shape), the goto blocks (control flow), and battle_stage.c's address-named float constants
(addr_literals reports 0 of 21 convertible in battle_stage.c and battle_seq_command.c) and its six pooled `unk_8042301x`
zero words (kept as address names; the comment at the declaration says why). Matching build OK (tower ninja, ok marker
recreated); CI ratchet not run because the tower copy has no accepted baseline.

### Readability: party_motion, party_move and party_yoshi leftovers (2026-10-20, Claude Haiku 5.5, n9-q-party4)

PartyEntry gains four names. `noFloorCount` (u8, 0x4E) counts frames a partner has walked with no floor under it:
party_motion's walk routine increments it when partySearch finds no floor, clears it on the other paths, and the
`> 0xa` test in the stay routine turns the partner toward Mario. `touchCount` (s16, 0x52) is the contact counter
partyMarioHosei returns (up to 100 while touching Mario, decays by 1 after). `tiltX` (f32, 0x64) is the x-axis tilt in
degrees: party.c's draw matrix does PSMTXRotRad 'x' by it, and party.c's dokan branch writes it from dispRot.x + 180.
`spinPivot` (Vec, 0x88) is the pivot party.c translates by -spinPivot before the z (spinAngle) and x (tiltX) rotations
and +spinPivot after; party_roll sets it to (-6, 6) at the start of a roll and zeroes it at the end. party_move's
0x3D/0x3E/0x3F bytes are `field_3C[1]`, `field_3C[2]`, `field_3C[3]` (the code already indexed field_3C[1]); the
signed read of 0x3F stays `(s8)` so the load is unchanged.

party_yoshi uses the MarioWork names that now exist: `airTimerPeak` (0x52, cleared at takeoff), `moveDir` (0x19C,
set beside directionView and read back into party->field_FC). The flutter words 0x2A4 are an f32 accumulator
(`0x2A4 += 0x2A0` each frame, position.y = 0x2A8 + 0x2A4), so they use the union's f32 member `field_0x2A4`, not the
motionSfxId0 handle view. party_yoshi's MUST_MATCH arms (YoshiZeroVec6 copies of 0x98/0xA4) are left raw.

Open, PartyEntry: 0x14C (HitEntry* stored when a ground hit has attr 0x40000000; no reader in party code); 0x44 (u32,
set to 0x10 beside followDist; no reader in these files); 0xE8 (f32, tested against 0.0f in party_move's
partyMoveMain; no name yet); party_nokonoko's 0x178-0x180 s32 zero stores, which span koops union members of other
types (f32, s16), and its twelve 0x10C NKF updates (the named form compiles differently, per n9-q-party3).
Open, MarioWork (mario.h not edited here): 0x2A0 is party_yoshi's flutter velocity but also the spinSpeed/damageRootY
union; 0x2A8 is an f32 flutter base y in party_yoshi (wCamVal1's slot, whose doc says step); 0x2B4 is the f32 z twin of
field_0x2B0; 0x18C is the f32 fly speed in party_yoshi; 0x39 is an s8 flag; 0x1DC and 0x1F0 are HitEntry pointers.

Verification: `configure.py && ninja` (matched build) recreates build/G8MJ01/ok, 32 files OK, 100% matched. Natural
compiles (no -DMUST_MATCH) of the four files from HEAD and from this branch are equivalent under tools/obj_equiv.py.
Index (tools/cleanup_index.py): 25394 -> 25341 weighted (36.223 -> 36.147 per kLOC); raw_offset 1653 -> 1623; field_0x
4513 -> 4520 (the seven field_0x2A4 uses are the existing union member, not new placeholders).
### Readability: NameEntryWork fields and seq_load's map, bero and fade names (2026-10-20, Claude Haiku 5.5, n9-q-seqload)
### Readability: NameEntryWork fields and seq_load's map, bero and fade names (2026-10-20, Claude Haiku 5.5, n9-q-seqload; reviewed by Claude Opus 5.5)

NameEntryWork (include/nameent.h) fields are named by their use in nameent.c: `flags` (0x1 accepts button input, set by nameEntWait and tested by nameMain; 0x2 the name was cancelled, read by nameEntIsCancel), `phase` (0x4: 0 idle, 1 load name.tpl, 2 run nameMain, 3 unload), `step` (0x8: 0 edit, 100 yes/no confirm, 101 done; nameEntWait returns step != 101), `type` (0xC, nameEntOn's argument), `cursorCol`/`cursorRow` (0x10/0x14), `nameLen` (0x18, characters entered), `charCursorX`/`charCursorY` (0x2C/0x30, eased toward `charCursorTargetX`/`charCursorTargetY` at 0x34/0x38), `name[16]` (0x3C, eight two-byte characters; the old TODO size is 16), `charTable` (0x50, hiragana_tbl or katakana_tbl), `maskAlpha` (0x54, passed to nameMaskGX), `dialogCursor` (0x5C, 0 yes, 1 no), `tpl` (0x60). 0x2, 0x4C and 0x58 are pads. Only nameent.c touches the struct.

seq_load.c: the strings passed as seqSetSeq's second argument are map names (`map_aji_10`, `map_nok_01`, ...); the third argument (named `bero` in include/driver/seqdrv.h) is `bero_nokotarou`, `bero_resha_return`, `bero_e_bero`, `bero_prologue`. The OFF_f_in_out strings are `animgroup_OFF_f_in_out_{mario,kuppa,peach}`, passed to evt_sub_animgroup_async in evt_continue. `copy_angles` is the 4x4 table indexed `copySrc * 4 + copyDst` (the copy-arrow rotation). `fade_color_*` are the seven black GXColors passed to fadeEntry, named for the state that uses each. `unk_800f2398` is `seqLoadPrepCardRecheck`: the last call in evt_memcard_delete/copy once the write was tried (success or miss), it clears fade slot 4 and copies the event's `printWindowId` into the work; the event then sets result 1, loadMain sets flags 0x80 and state 1 reruns evt_memcard_check with that window (the worker's `seqLoadCardCheckFade` read as a card-check fade, and the flags comment's "0x80 delete/copy failed" was wrong: result 0 is the declined yes/no, 1 the carried-out write). Left as they were: `unk_8016af5c` (win_mario.c, the zero-padded zenkaku number formatter; another unit's file), `gap_80349DF0` (14 unused +/-1.0 floats), the `vec3_*` and `dat_8041bd84..bdd8` colour objects (roles not proven for each), `double_*` pool objects, and the card-flag raw offsets (`+ 0x2000`, `<< 14`) that CardData does not name yet.

Matching build OK (ok file recreated on the tower); seq_load.c, nameent.c and evt_memcard.c natural-compile without MUST_MATCH. Index: 36.223 -> 35.818 per kLOC.

### Readability: MarioWork names for koopa_motion, mot_jabara and mot_roll (2026-10-20, Claude Haiku 5.5, n9-q-koopam)

New names in `include/mario/mario.h`, each traced to its readers and writers: `rollScaleY` (0x130, f32; mot_roll's
squash/stretch state machine writes 1.0, 1.4 and 0.4, mario.c's two `PSMTXScale` calls read it, both updated);
`kpaSpeed` (0x2C4, f32, the signed walk/swim speed read and written by koopa_motion's kpaStickSpd and kpaSwimMove; koopa.c's
two resets converted too); `rollFlags` (0x2A4 union member, u32; mot_roll's bits 0x10, 0x20, 0x100, 0x200, 0x1000,
0x4000, 0x10000); `jabaraHitY` (0x2A8 union member, s32; mot_jabara stores `(s32)(sp3C + wPlayerCollisionBox.y)`,
truncating the hit top to whole units, and converts it back with `(f32)` for position.y); `jabaraSpeed` and
`jabaraAccel` (0x2B0 and 0x2B4 union members beside `field_0x2B0` and `field_0x2B4`, so party_yoshi and peach.c are unchanged: each frame accel is added to speed, and speed to dispRot.x).
Existing fields now used: dispOfs/dispPivot/dispRot x/y/z (0x98-0xC4 in mot_roll and mot_jabara), moveDir (0x19C),
airTimerPeak (0x52, koopa_motion and mot_jabara), hitobjHead (0x1F4, mot_roll), field_0x11C.y (0x120), field_0x1C4,
motionSfxId1 (mot_roll).

Left, and why:
- mot_roll's 0x2A8 reads stay unsigned (`(u32)m->motionSfxId1`): the original tests them against 0xFFFFFFFF. Stores of
  integer 0 to 0x2AC, 0x2B0 and 0x2B4 stay raw, since an f32 field would change the store from an integer to an `lfs`.
- 0x1F0 (a `void*` HitEntry in koopa_motion and mot_roll) is not hitobjPush on G8MJ01: hitobjPush is 0x1EC JP (the
  4-byte JP/US shift that also moves hitobjStandOn and hitobjJumpFrom; casedrv and mot_walk read it there). mario_hit
  sets 0x1F0 to every wall searchFrontWall touches, but hitobjPush only for bero walls or after 20 frames of pushing.
  No name yet beyond the header comment; it has readers in mario_hit, party_yoshi, mot_ship and koopa.c too.
- koopa_motion's 0x2A0 is a u16 counter over the spinSpeed/damageRootY union; no member added. 0x18C (a speed copied
  from baseSpeed), 0x78 and 0x7A have no proven name.
- mot_jabara's 0x41, 0x42, 0x2BA and 0x2BB are per-motion state bytes with no proven names. 0x114 and 0x1F4 keep their
  types (s32 reads and `void*` reads would change the expression type).
- `field_0x11C` (Vec), `field_0x1C4` and `field_0x2A4` are still placeholders; renaming the first needs evt_mario and
  mario_hit updated too. Those uses are why koopa_motion and mot_jabara show a few more field_0x sites in the index.
- `float_*_80xxxxxx` constants are address-named but pooled; value-only names collide across addresses, so left.

Build: `ninja` recreates build/G8MJ01/ok and the 32 modules pass; changed C files natural-compile (without MUST_MATCH).
Index 36.223 -> 36.006 (koopa_motion 287 -> 288, mot_jabara 245 -> 252, mot_roll 200 -> 43 weighted).

### Readability: mapdrv, animdrv and mobjdrv address names (2026-10-20, Claude Haiku 5.5, n9-q-mapanim; reviewed by Claude Opus 5.5)

Renames, all through `tools/rename_symbol.py` (C, headers and symbols.txt together), each proven by where the value is read:
- `unk_80418458` -> `mapDispDisabled` (mapdrv.c). `N_mapDispOff` stores TRUE and `N_mapDispOn` FALSE; `mapDisp` returns early
  when it is set; `mapUnLoad`/`_mapLoad`/`bmapUnLoad` clear it. The type stays `u32` (the match-only path compares it signed).
- mobjdrv.c float constants whose value is their whole name, with the address suffix dropped: `float_deg2rad` (0.017453292f,
  pi/180, used as a degrees-to-radians factor by every MTXRotRad call), `float_2p5`.
- mobjdrv.c strings, named by their contents: `str_HOBJ_` ("HOBJ_"), `str_MOBJ_` ("MOBJ_"), `str_MOBJ_tilde` ("MOBJ~"),
  `str_S_1` ("S_1"). They are compared or strcpy'd in place of the literal.
- mobjdrv.c `vec3_zero0`/`vec3_zero1`/`vec3_zero2` (the zero Vecs at .rodata 0x802C3D10/1C/28; the comment that explains why
  mobjMain and mobjCalcMtx share them now names them by the new names).
- animdrv.c `colorWhite0`..`colorWhite5` (the six {0xFF,0xFF,0xFF,0xFF} sdata2 GXColors, assigned to materialColor and
  materialColor2 in the pose setup paths) and `vec3_zero3`..`vec3_zero6` (four zero Vecs read as lightPos and position
  defaults). The numbering is the order in the object; the values are the proof.

Left, and why:
- The float constants that are also file-local statics elsewhere (`float_10`, `float_0`, `float_1`, `float_neg1`,
  `float_neg0p5`, `float_5`, `float_25`, `float_20`, `float_0p1`, `float_0p5`): rename_symbol refuses any NEW already spelled
  in the tree (party_yoshi, party_nokonoko, evt_mobj and others). They are 72 address-named references in mobjdrv.c.
  addr_literals.py found 0 of 17 convertible in this file (pool order changes the bytes).
- `lbl_803083C8`..`lbl_803083F8` (`{-1, 1}` / `{1, -1}` pairs in .data): nothing in the tree reads them and no name is proven.
- `dat_80419a28` (`const s32 = 0x2f`, force_active, unreferenced): no consumer to name it from.
- `unk_8005bb80()` (mobjdrv.c, the test in mobjMain's trigger path): its meaning isn't proven from this file.
- `field_0x` in mapdrv.c/animdrv.c header structs not listed below, `goto`s, and raw offsets: offset_to_field.py found 0
  convertible casts across the three files (117 casts: offsets that aren't literals, untyped bases, or padding), so the
  remaining raw offsets need struct work, not a tool pass.

Also renamed (mapdrv.h, MapMatTexMtx and MapMaterialTex, used only by mapSetTextureMatrix in mapdrv.c): field_0x0/0x4/0x8/0xC/0x10
-> `transX`/`transY`/`scaleX`/`scaleY`/`rotation`, and MapMaterialTex's `field_0xB` -> `texCount`. Proof: the function builds
`PSMTXTrans(tx, ty)` from field_0x0 and -field_0x4, `PSMTXScale(sx, sy)` from field_0x8/0xC, and `PSMTXRotRad` with
0.017453292f * -field_0x10 (degrees); the loop runs `texCount - 1` down to 0 over `entries[]`, the same byte MapMaterial calls
texCount. field_0x14/0x18 stay unnamed: they only feed a half-size centre term, and no caller shows what they measure.

Check: tb.sh q-mapanim build OK (`build/G8MJ01/ok` recreated); natural compile of the three files without -DMUST_MATCH
rc 0. Cleanup index 36.223 -> 36.059 (mapdrv 45.8 -> 39.1, animdrv 54.8 -> 46.9, mobjdrv 154.4 -> 110.3).

Review (Claude Opus 5.5): kept `mapDispDisabled`, MapMatTexMtx's `transX`/`transY`/`scaleX`/`scaleY`/`rotation`
(checked against mapSetTextureMatrix: tx is 0x0 as is, ty is -0x4, the scale reads 0x8/0xC, the z rotation is
0.017453292f * -0x10, so the header's "negated" notes are right) and `texCount`, and the four mobjdrv strings
(`str_HOBJ_`, `str_MOBJ_`, `str_MOBJ_tilde`, `str_S_1`): the address name hides the literal at the strncmp/strcpy/
animPoseSetAnim call, and a name that is the literal can't mislead. Reverted to their address names with
rename_symbol, as the n9-q-winui review did for its value-restating colour/Vec names: `vec3_zero0..6` and
`animWhite0..5` (called `colorWhite` in the entry above; the code said `animWhite`). Seven identical zero Vecs and
six identical white colours numbered across two files read as one series, which they aren't, and say nothing a reader
gets from the one assignment each feeds. `float_deg2rad`/`float_2p5` went back to `float_deg2rad_8041a0dc`/
`float_2p5_8041a0e8`: the value was already in the name, and dropping only the address suffix made them the odd two
out of the file's `float_<value>_<addr>` pool. mapdrv.c's local MapMaterial stays: it covers 0x0-0x110 (color,
texRefs[8] at 0xC, a pointer at 0x110) while MapMaterialTex is the 0xB count plus `entries[]` at 0x2C, so folding
them needs a real merged struct, not a substitution. animdrv.c is now as on main. Matching build OK
(`build/G8MJ01/ok` recreated); mapdrv/animdrv/mobjdrv compile to objects equivalent to the pre-branch ones in both
the natural and the MUST_MATCH build (tools/obj_equiv.py, old tree with old headers), differing only in the renamed
globals; ci lint passes with `--allow-protected` (symbols.txt; no ratchet baseline in this worktree).
### Readability: evt_shop ShopWork fields, shop colour and map-name symbols, evt_mobj sound and prefix strings (2026-10-20, Claude Haiku 5.5, n4-q-shop2)
### Readability: evt_shop ShopWork fields, shop colour and string symbols, the pouch's shop-point mask, evt_mobj sound and prefix strings (2026-10-20, Claude Haiku 5.5, n4-q-shop2; reviewed by Claude Opus 5.5)

ShopWork's `field_0x8` is `items` (item id / price pairs: `items[i*2]` indexes itemDataTable for the name and icon,
`items[i*2+1]` is the price name_disp draws over each object); `field_0x2C` is `selectedItem` (set with `openIdx`
when a shelf item opens; get_value, help_disp, name_disp and `acquired[]` read it). Nothing in the matched
evt_shop.c touches 0x30/0x34 and ShopWork is file-local, so they are `pad_0x30[8]`. PouchData 0x5B4 is
`shopExchangedMask` (mario_pouch.h): bit i is set by exchange_shop_point once `shopPointList[i]`'s prize is given,
chk_shop_point skips set bits, reset_shop_point clears it, and list_disp draws those rows red. The twelve
`dat_8041bbe0..bc0c` colours are named by what they colour: `shop_item_name_color` (name_disp, the selected item's
name), `shop_price_color` (name_disp, the price over each object), `shop_points_color` (point_disp),
`shop_list_title_color` (title_disp, the prize list's "msg_shop_point_list4" title), and list_disp's column headers
`shop_list_points_head_color`/`shop_list_item_head_color` ("msg_shop_point_list5" ポイント, "msg_shop_point_list6"
けいひん) and row pairs `shop_list_{price,unit,item}_{exchanged_,}color` (the cost, the small ポイント unit after it,
the prize name). Strings: the names shopper_talk_evt compares with `IF_STR_EQUAL` against evt_sub_get_mapname are
`shop_map_mri_07`/`usu_01`/`rsh_03_a`/`_b`/`_c` (and `shop_map_rsh_03`, evt_shop_main_func's strncmp); the three it
`SET`s into LW(14) for evt_msg_print are message keys in those maps' text (mri_07.txt, usu_01.txt, rsh_03.txt), so
they are `shop_msg_stg2_shop_04_1`, `shop_msg_usugurashop_04_1`, `shop_msg_stg6_shop_04_1`;
`shop_msg_scroll_once_stop` is the `<scrl_auto><once_stop>` tag string. Floats and format: `shop_icon_y_offset`
(6, icon above the shelf object), `shop_item_name_y` (108), `shop_price_digit_width` (12, the price shifts right by
one digit width per extra digit), `shop_icon_name_fmt` ("shp%d"). Left: `str_802ECD50` (add_shop_point reaches the
map names by its `+0x10`/`+0x28`/`+0x90` offsets), "me"/"mario", the `%s`/`%d` formats, the `float_0p5`/`float_30`/
`float_10` constants (more than one role each), evt_shop_work (only the `_wp` cast uses it).
In evt_mobj.c, `dat_8041a134` is `kpa_pole_score_color` (kpa_disp_pole_score's only colour), `str_802C3EC8`/
`str_802C3ED8` are `mobj_offscreen_prefix`/`mobj_image_prefix` (the "o_"/"i_" that mobj_lv_blk_evt prefixes to the
object name for offscreenEntry and imgEntry), and the SFX names that evt_snd_sfxon_3d plays are `sfx_mobj_*` and
`sfx_kuppa_*`, the lowercase of their text. Left: `dat_8041a130` (the breaking-floor colour). The worker proposed
naming `base + 0x230/0x690/0x8ec` and `base + 0x3B8/0x3C8` as MapObjectEntry slots in mobjdrv.h; review found `base`
is not an object but a MUST_MATCH pool base (`negone_one_374` in mobj_koopa_pole_evt, `vec3_802c3dc8` in
evt_mobj_koopa_pole), so the offsets are `kpa_pole`, `kpa_pole_move`, `kpa_move_to_toride` and the anim names
"MOBJ_goal_pole"/"MOBJ_goal_hata" (str_802C4180/4190), which the natural branches already name. Nothing for
mobjdrv.h. Matching build OK; evt_shop.c and evt_mobj.c compile without MUST_MATCH to objects equivalent to the
pre-branch ones (tools/obj_equiv.py). Landing needs `--allow-protected` (config/G8MJ01/symbols.txt).

### Readability: filemgr state values (2026-10-20, Claude Haiku 5.5, n9-q-filemgr2)

`FileState` (filemgr.h) names `FileEntry::state`: 0 FREE (`_fileGarbage` moves the slot to the available list and sets 0), 1 ALLOCATED (`_fileAlloc` sets it, `fileFree` counts references down from it), 2 GARBAGE (`fileFree` and `dvdReadDoneCallBack`'s cancel path set it; `_fileGarbage` frees the memory and recycles the slot), 3 ASYNC (`fileAsync` creates it; `_fileAlloc` moves it to ALLOCATED when it claims one; `entry` is set only while the DVD read is pending). The enum is an int enum so `u8 state` comparisons and stores compile to the same code.

`FILE_ENTRY_COUNT` (0x400) is the `FileEntry` pool size in `fileInit`. It is not the 0x400 `_filename` vsprintf buffer, which stays a literal. `file->entry->status = 8` is `DVDMGR_CLOSED` (bit 3 in dvdmgr.h, the close flag `DVDMgrClose` sets). `_fileAlloc`'s callback parameter is `FileCallback`, the same type.

Left: the raw-offset relocation lines for types 1-3 and 6-10 (no struct and no caller for them), the AnimData words at 0x24-0x40 inside ANIMPOSE walks, `FileEntry` 0x8-0x20 and 0x60-0xA0 (unknown), `data[1]` (read as the length in `fileGarbageMoveMem`; it lives at the handle's +4 word for arcOpen and in the smart block for DVD reads, and neither is typed), the `_fileGarbage(int)` argument (both modes are visible, but the names would be guesses), and `archiveType` 0/1/2 (no archive-type enum is proven; animdrv only passes them).

## koopa_hit.c and mario_motion.c raw offsets (2026-10-20, Claude Haiku 5.5)

mario_motion.c: `mot_talk` reads `fbatGetPointer()->talkNpc` (FieldBattleData, npcdrv.h, pointer at 0x4) and then its
`field_10C` (NpcEntry, `const char*` at 0x10C, the pose name passed to marioChgPose); both are now typed. `mot_shadow`
writes the display rotation `mario->dispRot.x/y/z` (Vec at 0xBC, mario.h; the y store is kept, as in the original).
`offset_to_field.py --apply` converted these seven stores. koopa_hit.c: the unused MW_F32/MW_U32/MW_U16 macros are
removed; `MW_S16` stays for the one 0x78 read in kpaEnemyHitChk (`== 0` returns 4). Left for the mario.h owner:
- 0x78 is `u8 unk78[2]`; kpaEnemyHitChk and the MARIO_FLAG checks read it as s16 (koopa_hit.c, `MW_S16(mp, 0x78) == 0`).
  Needs an `s16` at 0x78.
- 0x1DC is `u8 field_0x1DC[4]`; mario_motion.c reads it as a `void*` hit-object pointer
  (`*(void**)mario->field_0x1DC` in mot_* around lines 1199-1205, passed to hitGetAttr/mobjGetHint/caseCheckHitObj).
  Needs a `void*` at 0x1DC.
- 0x1FC is already `void* field_0x1FC`, assigned 0 in mario_motion.c (~line 489); no name yet.
- 0x2B8 is `f32 field_0x2B8`, written with marioChgJumpStandMotion's `arg` and with 100.0f on the blue jump stand, red switch
  and Yoshi cases (mario_motion.c ~lines 292-315, 1017-1028). Needs a name.
- 0x2AC/0x2B0 already have header names (`field_0x2AC`, `field_0x2B0`/`jabaraSpeed`) but mario_motion.c still spells them
  `field_0x`; naming the header field `jabaraSpeed` or a new name is a header change.
Not touched: the `float_*_80420eXX` constants in koopa_hit.c. A bare-value rename (`float_0`) clashes with the same
name already used in nameent.c, battle_stage.c, battle_icon.c, party_yoshi.c and party_nokonoko.c, so the address
suffix stays. `unk_800a4e40` (mot_slit.c, paper-mode reset that sets motion 0x15 and a 0x2D8 s16) has no proven name.
### Readability: CounterPreCheckWork names in battle_damage.c (2026-10-20, Claude Haiku 5.5, n9-q-bdamage; reviewed by Claude Opus 5.5)

BattleCheckCounter's scratch struct (`CounterPreCheckWork` in battle_damage.h) is named by where each field is consumed in BattleCheckDamage. `hitLands` (0x0) starts at 1 (BattleInitCounterPreCheckWork) and is cleared only by counter kinds 0x2B..0x30 (spiky, fiery, icy, electric part); BattleCheckDamage's `while (work.hitLands)` body is the whole normal hit on the target, so those counters cancel the hit and go straight to the counter tail, while the explosive, payback, poison, hold fast and Return Postage kinds leave it set and the hit still lands. Three flags each add `damage * 50 / 100` to `counterDamage` in the tail: `halfDamage` (0x4, set by the payback status and poison counters, so it is named by effect), `holdFast` (0x18, the hold fast status; it also plays effBombEntry, the thunder sfx and BattleFogForceStop) and `returnPostage` (0x1C, the Return Postage badge). `inflictBurn`/`inflictPoison`/`inflictElectric`/`inflictFreeze` (0x8/0xC/0x10/0x14) are set only by the matching `...Status` counter attributes and each calls BattleSetStatusDamage with the matching STATUS_* on the attacker. `counterDamage` (0x28) counts one per spiky, fiery, icy, electric or explosive counter that matched, gets the halves added and is raised to 1, then is the damage passed to BattleDamageDirect on the attacker; `damageCode` (0x24) is that call's damage code and the nonzero "a counter matched" test; `hitEventCode` (0x2C) is a damage code (0x1F..0x24) BattleRunHitEvent runs on the target, set only by kinds 0x2B..0x30; `counterType` (0x20) picks BattleCheckCounter's return value (0x30 has no case and returns 0). Index: battle_damage.c 76.06 -> 39.66; overall 35.19 -> 35.06 per kLOC. Built with the tower's ninja (`build/G8MJ01/ok` recreated); battle_damage.c compiled without -DMUST_MATCH is equivalent to the pre-branch object (tools/obj_equiv.py).

Left in battle_damage.c: the other 31 `field_0x` uses are BattleUnit (`field_0x26E`, `field_0x264`, `field_0x27C`), BattleWeapon (`field_0x6D`) and mKindParams members that belong to their headers. ac_gauge_timing.c is unchanged: its `dat_80421870` is `BattleACDispParams.field_0xC`, which ac_shot.c reads as bytes (a colour), so -1 has no proven name; its float pool macros keep the file's address-suffixed convention; BattleActions `field_0x4`/`field_0x8`/`field_0x30` are shared with eight ac_* files, and `field_0x8` is both the setup argument and the post-token countdown, so a name would be wrong for part of its uses.
### Readability: win_mario WindowWork fields, LinkDt and SuperActionEntry names, colour data (2026-10-20, Claude Haiku 5.5, n9-q-winmario)
### Readability: win_mario WindowWork fields, LinkDt names, superActionTable as BattleWeapon, colour data (2026-10-20, Claude Haiku 5.5, reviewed by Claude Opus 5.5, n9-q-winmario)

The Mario page's state lives in WindowWork's padding, so the offsets are named in include/win/win_main.h: `marioLinkIdx` (0x160, the current linkDt entry; set in winMarioInit2 and by MARIO_TRAVERSE), `marioHoldTimer` (0x18C, winMarioDisp drives the pose's local time only while it is 0; winMarioMain sets it to 30 after a loop), `marioState` (0x190, 0 = link cursor, 1 = super action cursor), `superActionSel` (0x194, the cursor in superActionTable) and `superActionCount` (0x198, the star powers owned: winMarioInit counts the bits of `mStarPowersObtained`, and the loops in winMarioMain and winMarioDisp run to it). Nothing outside win_mario.c touches 0x160 or 0x18C-0x19C. The `WW_S32`/`WW_F32`/`WW_PTR` raw-offset macros are gone with them.

`LinkDt` (win_mario.c's .data table): `targetX`/`targetY` (copied to work->targetX/Y, as f32), `poseAnim` (the pose anim when flags & 0x2000 is clear), `starPoseAnim` (the famicom pose anim, compared with "S_1" to pick the S or R set), `reqItem` (MARIO_TRAVERSE calls pouchCheckItem on it; 0 means always reachable), `helpMsg` (winMsgEntry's message), `next[4]` (the neighbour per d-pad direction). `field_8` and `field_12` are never read and keep their names. `superActionTable` (battle_mario.c) holds `BattleWeapon*`s, so the local `SuperActionEntry` view is gone and the page reads `name` (winFontSetLabelV), `description` (winMsgEntry) and `baseSpCost` (the SP cost printed beside it).

`unk_8016af5c` is `winZenkakuPadStr`: it writes the number in full-width digits (the zenkaku zero 0x824F on the left) padded to `width`, and the winMarioDisp play-time clock and seq_load.c's time string both use it. The `.sdata2` colours are named by the code that reads them: `fukiShadowColor`, `fukiBubbleColor` and `fukiIconColor` for fukiDrawL, the `...R` set for fukiDrawR (the shadow is drawn first, offset by 5, the bubble body second, the item icon's tint last), and `winWhiteColor`/`winBlackColor` for the white and black text colours in winMarioDisp.

Left: the `str_80...` anim names and message-ID strings (winMarioInit and winMarioMain share them; `str_8041D810` is also used by win_log); the `0xC4` raw cast in winMarioDisp (the WinRootRec array is private to win_root.c, so the field cannot be named from here); `*(volatile u32*)0x800000F8` (the bus clock, no header names it); the `(const char*)0x20005`-`0x20013` SFX ids (pmario_sound.h has no names for them); the `goto tail` and MARIO_TRAVERSE; and the match-only casts under MUST_MATCH.

Verify: tower build ok (build/G8MJ01/ok recreated). The compiled win_mario.o keeps its .text, .rodata, .data, .sdata and .sdata2 bytes (only the symbol names of the renamed function and colours change). win_mario.c and seq_load.c compile without MUST_MATCH.
### MarioWork readability: prevPosition, riderDelta, peachStayTimer, PeachFlags; evt_mario offset casts (2026-10-20, Claude Haiku 5.5, n9-q-peach)
- `MarioWork.prevPosition` (0x110, Vec, carved out of `unk104`): mario.c copies `position` into it at the frame boundary (reinit, airTimerPrev/hitobjStandOnPrev copy) and peach.c's peachMain does the same; party_vivian reads its y as `position.y - prevPosition.y`. mario_hit still reads 0x114 raw through GND_PTR.
- peach.c's `unk170 + 4/8/0xC` stores were `riderDelta.x/y/z` (0x174-0x17C) past the 4-byte `unk170` pad; now named. `unk170` stays as an unused 4-byte pad so the layout is unchanged.
- `peachStayTimer` (0x2B4, s32 union member next to `jabaraAccel`; party_yoshi's f32 use and mot_jump's f32 stay as they are): peach_stay's counter in state 0x14, cleared on entry, capped at `sysMsec2Frame(0x2710)`.
- `PeachFlags` enum for `peachFlags`: AFTERSHOWER 0x10, INVISIBLE 0x20, HAPPY 0x40, UNEASY 0x80, ANGRY 0x100, ORDINARY 0x200, CARRY 0x400 (the peachSetSts* setters), STS_MASK 0x7F0, TRANSFORMED 0x2000 (peachTransformOn/Off, mario.c's checks).
- peach.c pose strings named by their use: `str_80420254` peach_pose_talk (peach_talk, the auto-talk pair with stay), `str_8042025C` peach_pose_dash, `str_80420264` peach_pose_walk, `str_8042026C` peach_pose_stay (peachChgPose fallback, peach_stay reset). Left: P_U_1, P_N_1, the zako set (S_1/T_1/Z_1/W_1/R_1), and the float_47/float_20 box sizes.
- evt_mario.c: 21 `(u8*)mario/party + off` casts converted by offset_to_field.py (dispOfs/dispPivot/dispRot, party dispOffset/spinPivot); the landing branch's `u8* mp` alias now uses jumpVelY, field_0x128, field_0x11C.x/z and airTimerPeak. MUST_MATCH casts and the party 0x70 vectors stay raw.
- Left in mario.h, held for the coordinator's review: 0x78 (`unk78[2]`, read as s16 in koopa_hit), 0x1DC (`u8 field_0x1DC[4]`, a HitEntry* written from hitobjFront in mario_hit and read as a hit object in mario_motion; no single role proven, and mot_yoshi/party_vivian zero it as u32), 0x1FC (`void* field_0x1FC`, reset per frame in marioMotion and compared to mobj hits in mobjdrv; no setter found outside name-match clearing), 0x2B8 (f32: jump-from z in mot_jump, the jump-stand arg in marioChgJumpStandMotion, 100.0f in switch cases, the z tilt of motion 0x1A in mario.c; mot_jabara reads the same slot as an s16 timer, so the union would need per-motion names).
- Left: `field_0x32 + 2` s32 frame counter at 0x34 (peachMain increments it, no reader found), `unk1A4`/`unk1A8`, `unk_800e23e0`.
### Effect work-struct field names: glass, opuku_jetw (2026-10-20, Claude Haiku 5.5, n9-q-effhdr)
- `effGlassData`: 0x18/0x1C/0x20 are `color1R/G/B` (effGlassDisp's TEVREG0 colour, also the type-1 `color3` path); 0x28/0x2C/0x30 are `color2R/G/B` (TEVREG1). 0x24 was already `alpha`. Left 0x34: written in effGlassN64Entry only, never read.
- `effOpukuJetwData`: 0x18/0x1C/0x20 are `color1R/G/B` (effOpukuJetwSetTev0a/0b), 0x28/0x2C/0x30 are `color2R/G/B` (effOpukuJetwSetTev1); 0x24 is `alpha` (the TEV alpha, faded in by Main while counter < 8); 0x38 is `rot` (degrees, PSMTXRotRad 'z'); 0x3C/0x40 are `scaleX`/`scaleY` (PSMTXScale in Disp, 1.0 at entry).
- `effOpukuJetwPoint` (adata/a2data entries): 0x0/0x1 are `posX`/`posY` (PSMTXTrans x and 10 + y); 0x2/0x4 are `scaleX`/`scaleY` (read from the column's row-0 entry, times 0.01); 0x6 is `rot` (degrees, plus dat_ry); 0x8 is `alphaScale` ((alpha * v) >> 8 as the TEV alpha).
- Users updated: rels/tik/src/unit/unit_boss_gesso.c's effOpukuJetwData writes (`rot`, colour zeroing). eff_freeze_n64.c already uses the glass names.
- Left unnamed: `unk_80423120`-`unk_80423128` (glass), `unk_804231c8`-`unk_804231d0` (opuku_jetw) and `unk_80423160` (kameki_tornade) are zero GXColor templates; their names are address-based and not renamed here.
- eff_break's `BreakEffect.field_0x18` is written (= 0) and never read, so it stays. eff_sunlight's `pad0` is the byte at 0x1F, a real gap. eff_stardust2 and eff_kameki_tornade have no field_0x names left in their headers or sources.

### Readability: eff_small_star_n64 and eff_sstar_n64 names (2026-10-20, Claude Haiku 5.5, n9-q-effss)

- eff_small_star_n64: `effSmallStarN64Data` fields `spinAngle` (0x1C, z rotation in degrees, drawn with deg2rad, += `spinSpeed` every frame), `spinSpeed` (0x20), `alpha` (0x24, the GXColor alpha in Disp), `life` (0x28, head particle only: counts down, deleted below 0) and `frameCount` (0x2C, head only: frames since spawn; drives the wave phase in Main and the colour rotation base in Disp). Entry parameters `count` and `dirX/dirY/dirZ`. Locals: `perpX/Y/Z` (the unit vector perpendicular to dir, the velocity basis), `yawRad` (camera yaw in radians, Entry), `spreadDeg` (case 3 spread), `cosC2`/`cosC0` (cosine polynomial coefficients of x^2 and the constant term), `sq` (squared reduced angle), `rIdx/gIdx/bIdx` and `colorBase` (Disp). The two .sdata2 constants 0x8041FC8C/0x8041FC90 are `kCosX4` (0.03705) and `kCosX2` (0.4967): a polynomial cosine approximation 1 - 0.4967x^2 + 0.03705x^4, which Main evaluates for velY. Header prototype parameters are `count`, `dirX/Y/Z`. `kaiten` stays (map name kaiten$412).
- eff_sstar_n64: `SstarWork` fields `canBounce` (0x0: set at spawn, cleared by the first floor hit, gates the floor check), `bounceCount` (0x1C, f32: the trail is drawn while it is <= 1, the effect ends at 10), `rollAngle` (0x24: body z rotation, starts at the launch heading, += `rollSpeed` per frame once bouncing starts), `rollSpeed` (0x2C: +-20, x0.7 per bounce), `horizVel` (0x34: minus the signed horizontal launch speed, the input to the trail angle) and `histIndex` (0x3C: ring index into mtx[8]). Header prototype parameters are `targetX/Y/Z` and `speed`.
- Left as found: `field_0x28` in SstarWork (written 0 at spawn, never read); `SstarVtx.unk`; `lbl_8041FDA0`, `unk_80423260` and the other `.sdata2` float constants (address names kept; value-only names would invent meaning or clash with names already used elsewhere in the repo, and rename_symbol refuses those); `angle` in sstar's Entry (it holds the camera yaw, then the launch heading).
- Verified: tower build recreates `build/G8MJ01/ok` with 32 modules OK; the two units compile without MUST_MATCH (the `ci.natural` command, run by hand: the tower copy has no .git, so `ci.natural` itself sees no diff there). Index: eff_small_star_n64.c 378.6 -> 300.0, eff_sstar_n64.c 319.1 -> 266.0 (the rest is address names: .sdata2 constants and the lbl_/unk_ data).

### Readability: MarioWork 0x1DC, 0x28C and 0x78 names, prevPosition in mario_hit (2026-10-20, Claude Haiku 5.5, n9-q-evtmario2; review fixes Claude Opus 5.5)

- `actionHitObj` (0x1DC, `void*`, was `u8 field_0x1DC[4]`): mario_hit's marioSearchFrontWall_staying clears it on the frame buttonNew has 0x100 (A) and sets it to hitobjFront when a front wall is found; party_yoshi does the same from partySearchWallFront on an A press. marioChkJump (mario_motion) and the balloon check (mario_balloon) read it (hitGetAttr, mobjGetHint, caseCheckHitObj) to decide whether the press acts on the object. marioResetHitObj, marioMain/marioMoveMain, koopa.c's per-frame reset, mot_ship, mot_yoshi and party_vivian clear it.
- `koopaWork` (0x28C, `struct KpaWork*`, forward-declared in mario.h, was `u8 field_0x28C[4]`): `KPA(m)` is now `((m)->koopaWork)`. evt_koopa_fire/evt_koopa_weary/evt_koopa_chk_dead use KpaWork fields (field_0x144, flags), and marioReInit keeps it in a `struct KpaWork*` local across its memset.
- `noHitEnemyCt` (0x78, `s16`, was `u8 unk78[2]`): named after the original symbols kpaNoHitEnemyCheck (returns it) and kpa_nohitenemy_ct_clear (rels/kpa, zeroes it). kpaPowDown sets sysMsec2Frame(3000), koopa.c decrements it each frame while nonzero, kpaEnemyHitChk returns 4 (enemy hit) only when it is 0, kpaMSetMutekiTimer and evt_mobj's koopa pole clear it. The MW_S16 macros in koopa.c, koopa_hit.c and koopa_motion.c are gone.
- mario_hit's `GND_PTR(wp, f32, 0x114)` is `wp->prevPosition.y`; mario_balloon's raw 0x1E0 reads are `hitobjFront`.
- KpaWork's field_0x144 comment now names its writer, evt_koopa_fire.

Left, and why:
- 0x164 (f32 in the unk160 pad, written by marioChkGndRollProbe as prevPosition.y - position.y): nothing in the tree reads it, so it has no name and stays `GND_PTR`.
- 0x1FC (`void* field_0x1FC`): the only setter is the reset to 0 in mario_motion; mario.c clears it by name and mobjdrv reads it. No name.
- 0x2B8 (f32): the jump-from z (mot_jump), the jump-stand argument (mario_motion), 100.0f in its switch cases, the z tilt of motion 0x1A (mario.c), and mot_jabara's s16 timer. The roles differ per motion, so no single name.
- 0x1F0 (HitEntry*): unchanged; see the n9-q-koopam entry.
- evt_mario.c's `*(f32*)((u8*)mario + 0x7C)` is inside MUST_MATCH and stays raw.

Build: tower `ninja` recreates build/G8MJ01/ok; koopa.c, koopa_hit.c, koopa_motion.c, mario.c, mario_hit.c, mario_motion.c, mario_balloon.c, mot_yoshi.c, mot_ship.c, party_vivian.c, party_yoshi.c, evt_mario.c, evt_mobj.c and rels/kpa/src/kpa.c natural-compile without -DMUST_MATCH (rc 0, no errors).
### Readability: Shadow Queen effect structs (2026-10-20, Claude Haiku 5.5, n9-q-queen)

eff_queen's effQueenData: `widthScale` (0x10, the X/Z factor on `scale`, 1.0 at spawn), `scale` (0x14, the Y scale and the base of X/Z), `lifetime` (0x20, a countdown where 1000 means no limit; below 100 the alpha fades), `alpha` (0x24, +3 per tick toward the cap, -3 while fading), `alphaCap` (0x28, 255 at spawn; the draw cap is 0.8 of it). `eff_queen_set_alpha_evt` in rels/las unit_boss_black_peach.c writes `alphaCap` through effQueenData now, byte-identical. eff_queen2's effQueen2Data: `scale` (0x10, the current scale, multiplied by 10 in Disp), `baseScale` (0x14, the camera distance / 10 at spawn, the start of the 0x78-frame ramp to 3.0), `alpha` (0x18, the GXColor alpha: ramps to 128 over 0x3C frames in phase 0, falls by 2 from phase 3), `phaseTimer` (0x20, frames into the current phase), `state` (0x24, phases 0-4), `chargeEntry` (0x28, the akari charge effect started in phase 1 and soft-deleted in phase 3).

Left: `field_0x0` in both structs (the `type` argument is stored and never read) and `field_0x1C` in both (never read or written in the .c files); the first parameter of effQueenEntry stays `arg0` in the header for the same reason. `unk_80423360` (eff_queen2.c, the .sbss2 GXColor template read as the material colour base) keeps its address name, like the glass and opuku_jetw templates. eff_queen2 still casts `chargeEntry->userdata` to EffAkariChargeData, a partial view of the akari charge work. Index 34.727 -> 34.614.

## yuugijou (Pianta Parlor work) readability, yuugi
- `PiantaParlorWork` (include/evt/evt_yuugijou.h). `flags` 0x00: bit 0 set by yuugijou_montemedal_disp_onoff (count display on), bit 1 read by yuugijou_get_takeoutflag and set by montemedal_return, bit 2 from yuugijou_set_yupflag (monteCountDisp moves the display up 60), bit 3 set by yuuminigame_window_end and read by evtYuuWindowEndChk. `keptTokens` 0x04: montemedal_keep stores tokens there and zeroes them, montemedal_return restores, get_montemedal_keep and add_montemedal_moto use it. `tokens` 0x08 (existing). `dispTimer` 0x0C: monteCountDisp's `& 3` tick. `dispTokens` 0x10: the displayed count stepping toward tokens; montemedal_wait waits for it to match.
- `rankUsed`, `rankScore`, `rankMonte` are `[4][6]` at 0x14, 0x74 and 0xD4. init fills rows 0-3 with the same 24-byte row stride, and the rel evt_yuuranking copies (yuu and gor) index `yuwp + mode*24 + 0x14/0x74/0xd4`, with the comment "six slots each for the used slot flag, the score/time value, and the monte count". Slot 5 is the rel's scratch slot.
- `field_0x134` stays: not written in src/, no reader found.
- Colours renamed with tools/rename_symbol.py by consumer: `dat_804214b0` waku_col (windowDispGX_Waku_col), `dat_804214b4` icon_col_left (icon 0x13F at x -260), `dat_804214b8` icon_col_right (icon 0x1D6 at x -230), `dat_804214bc` number_col (iconNumberDispGx).
- Locals: init `q` scaled, monteCountDisp `x` baseY, add_montemedal `flag` syncDisp, montemedal_return `tmp` kept.
- Left: `monte_snd_flag` (original-looking static), `gap_08_80418B5C_sbss`, the scale700/scale2000b helpers. monte_disp's rows 2 and 3 use 0x1A9C8 inline and scale2000b uses 0x12110, so they are not merged.
- Verified: the tower ninja build recreates build/G8MJ01/ok. The MUST_MATCH object is byte-identical to the build's. The header users (src event.c, cardmgr.c, mario_pouch.c, mariost.c, win_main.c and rels gor_03, gor/yuu evt_yuuranking, yuu_00-03, yuu evt_yuucounter) compile with and without -DMUST_MATCH at -maxerrors 50.

### Readability: eff_mahorn particle fields and colours (2026-10-20, Claude Haiku 5.5, n9-q-mizu)

- eff_mahorn `field_0x14` is `growth` (effMahornData, 0x14). It is 0 at spawn, Main eases it toward 20 by 1/8 of the gap each frame, and Disp multiplies it into the particle scale.
- eff_mahorn `field_0x24` is `delay` (0x24). Entry sets it to `i * 4` per particle, Main counts it down, and Disp skips the particle while it is nonzero.
- eff_mahorn's Entry parameter `unk` is `timer`: it becomes the header's lifetime (`data->timer`), which Main counts down, fades alpha over the last 16 frames and deletes at below 0. A value of 0 or less gives 1000, which Main never counts down. evt_eff.c passes its eighth script argument.
- eff_mahorn `dat_80422240` / `dat_80422244` are `mahornColorOdd` / `mahornColorEven`: Disp takes `(i & 1) ? &dat_80422240 : &dat_80422244` as the base colour and sets `.a` from the particle alpha. Odd particles also keep the unflipped Y scale.
- Left: eff_mahorn's `unused` parameter and header field 0x0 (stored, never read; the only caller passes a script argument). The MUST_MATCH `volatile` pool constants and the `zero` / `negEight` locals stay as they are.
- Cleanup index: total 34.850 -> 34.808 per kLOC; eff_mahorn.c 410.2 -> 304.7.
- Verify: tower build recreates build/G8MJ01/ok.
### Readability: eff_stone_smoke_n64 and eff_akari_sflush_n64 work-struct fields (2026-10-20, Claude Haiku 5.5, n9-q-effhdr2)

`effStoneSmokeN64Data` (eff_stone_smoke_n64.h) and `EffAkariSflushData` (eff_akari_sflush_n64.h) had their `field_0x` names replaced by what each `.c` reads and writes. The GX colour names follow the GX enum: `GXSetTevColor(1, ...)` is GX_TEVREG0 and `(2, ...)` is GX_TEVREG1, so the 1 set is `color1*` and the 2 set `color2*`, as in effGlassData/effOpukuJetwData.

- stone_smoke: 0x14 `age` (elapsed frames, the scale keyframe index), 0x18/0x1C/0x20 `color1R/G/B` (TEVREG0), 0x24 `alpha` (set to 0xFF, becomes life * 16 when life < 16), 0x28/0x2C/0x30 `color2R/G/B` (TEVREG1), 0x34 `scale` (the spawn scale argument, multiplied in Disp), 0x38 `scaleKey` (0.01 * scale_data/scale_data2, the keyframe factor). eff_dogabon_attack_n64.c's three writes to 0x28/0x2C/0x30 (100, 0, 0 on the smoke it spawns) are `color2R/G/B`, so that spawn sets TEVREG1 to (100, 0, 0).
- akari_sflush: 0x10 `life` (the countdown, 1000 when no count is given, 16 on the restart flag), 0x14 `age`, 0x18/0x1C/0x20 `color1R/G/B` (GXSetTevColor(1)), 0x24 `alpha`, 0x2C `growScale` (starts at 1.0, grows by 0.3 * (8 - life) in the last 8 frames, multiplies each ring's scale), 0x30 `lastRing` (0; the ring loop runs i = 0..lastRing and spaces the rings with lastRing + 1). The header's existing `scale` (0x28) is unchanged.

Left: eff_thunderflare's `field_0x00` (written in the reset loop, never read; the rule for never-read fields). eff_rappa's `field_0x1C`/`field_0x20` (effRappaData) are never referenced by eff_rappa_n64.c or any other user, so they stay. The `unk_804232xx`/`unk_8042328x`/`unk_804230a0` GXColor templates in thunderflare, timestamp, vapor, stone_smoke, rappa and akari_sflush are address-named data that is not a struct field, so they are not renamed here. Vapor's `timer` and timestamp's effTimestampVtx were already named.

Matching build 32 files OK, and the tower build recreates build/G8MJ01/ok. The natural (no MUST_MATCH) and MUST_MATCH compiles with -maxerrors 50 of the four files that include the two headers (eff_stone_smoke_n64.c, eff_akari_sflush_n64.c, eff_dogabon_attack_n64.c, evt_eff.c) each exit 0 with no errors. Cleanup index 34.727 -> 34.614 per kLOC (field_0x 4,075 -> 3,996).
### Readability: eff_cool_leaf_n64 and eff_stamp_n64 (2026-10-20, Claude Haiku 5.5, n9-q-leaf)

eff_cool_leaf_n64's Main takes its sway locals from what they feed: `sway` is sin(2 x rotZ), which the type 0/1 loops
turn into `swayPush` (0.2 x sway) added to velX, and type 1 adds `doublePush` (2 x swayPush); `tiltSin` is sin(rotZ),
the factor on velY's damping; `randIndex` is the effTblRandN64 index (age + 20 per leaf); `age` is the frame count
the old `counter` held. The type-2 MUST_MATCH temp (the sin/cos results that pin the FPRs) is `trig`, separate from
`sway`, so no local changes meaning between cases. Entry's `rnd01`/`rnd11`/`rnd361`/`rnd101` are `randX` (x = 75 +),
`randY` (y = 70 +), `randAngle` (rotZ, and rotY = 360 - it) and `randSpin` (spin = 0.1 x it); `ptr` is `leaf`. Disp's
`work` is the view matrix times the base transform (`viewMtx`, the same name effStampDisp uses) and `scale` is the
texture matrix (`texMtx`). The param of effCoolLeafSetColor is `alpha`. `dat_8041ec50` is `coolLeafTevColor`: the
u32 0x14641400 read as the GXColor base of GX_TEVREG0, whose alpha is then overwritten; renamed with rename_symbol.py
(symbols.txt updated). The two GX_PNMTX0 literals are enums.

eff_stamp_n64: effStampData `scale` (0x30) is `radius`, the petal ring radius: Main eases it toward 10 for the first 10
frames and then toward 100, and Disp places each petal at radius x cos/sin(angle). Main's `count` (a copy of
data->counter) is `counter`, Entry's ring pointer `p` is `petal`, and the GX_LIGHT_NULL literal is the enum. The
header's `field_0x10`/`0x14`/`0x18` stay unnamed: Entry writes 0 to them on every petal and nothing reads them.
unk_80423270/unk_80423274 stay (`.sbss2` GXColor seeds): the TEVREG0 seed is fully overwritten, and only the
alpha of the TEVREG1 seed reaches the register, so nothing shows what the seed is meant to be. The sstar
`.sdata2` float constants keep their names. Index 34.727 -> 34.723 total; eff_cool_leaf_n64.c 391.5 -> 383.1, eff_stamp_n64.c unchanged. Verified: both
objects are `same` under tools/obj_equiv.py against the pre-change build (only the renamed data symbol differs),
the tower build recreates build/G8MJ01/ok, and both files natural-compile without MUST_MATCH.

### Readability: hdrmisc header and unit names (2026-10-20, Claude Haiku 5.5, n9-q-hdrmisc; reviewed by Claude Opus 5.5)

- seq_title's TitleWork: `step` (0x8, the titleMain switch), `tex5PosX`/`tex5PosY` (0xC/0x10) and `tex6PosX`/`tex6PosY` (0x14/0x18, the x/y that disp2Vec writes into the translation passed with texture 5 to unk_80177008 and texture 6 to winTexSet), `fade` (0x1C, the 0..1 factor on the press-start alpha, +0.1 per frame in step 3, -0.1 in step 21), `phaseFrame` (0x20, the intplGetValue frame in steps 2 and 21), `waitTimer` (0x24, the countdown that clears the BGM stream's 0x8000 flag at zero), `idleTimer` (0x28, counted in step 3; past 0x474 the title goes to step 20), `pulseCounter` (0x2C, +1 per disp2, the sine phase of the press-start pulse and the texture scroll in pressStartGX, which read it through a raw offset before). The strings are named by their contents: `str_demoTplPath` ("%s/demo.tpl"), `str_c_peach`, `str_c_nazo`, `str_MOBJ_GoldenTreasureBox`, `str_MOBJ_BigTreasureBox`, `str_c_roten`, `str_c_kuribo_n`, `str_c_tyusan`. system.c's `buildDateStr`/`buildTimeStr` (`dat_ptr_8040f260`/`264`) point at main.c's R_build_date ("Jun 17 2004") and R_build_time ("17:20:01"), the __DATE__/__TIME__-format strings disp2 prints with FontDrawString.
- mot_plane's MotionPlaneData: `pitch` (0x4, negated into dispRot.z, 90+pitch is the sincosf angle whose cosine moves position.y, clamped against planeData[3]/[4]), `heading` (0x8, copied to directionView, whose sincosf moves x/z; revise360 wraps it and flags&1 holds it to 60..150 or 210..330), `bank` (0xC, `heading += bank * planeData[14]` each frame, and in chkCollision its sin/cos tilt the wing-tip probe sideways against the heading and vertically), `stallTimer` (0x20, counts frames while baseSpeed <= planeData[16] with the stick held nose-up, reset otherwise; reaching the planeData[17] time switches to sub-motion 0x3C), `sfxId` (0x28, the psndSFXOn_3D handle that psndSFXOff stops, -1 when none). The 0x10..0x1F and 0x24 words stay MF32 offsets.
- battle_break_slot: the slot's `timer` (0x8, the frame count each state compares against), the reel's `stateTimer` (0xC, frames in the reel's current state, reset with every state change), `scrollStep` (0x50, added to pos.y each frame, held at 20 or above while the reel slows) and `scrollDelta` (0x54, added to scrollStep each frame), `alpha` (0x58, the alpha of both card planes, faded 255 -> 0 over 60 frames). `str_recovery` is the "recovery" effect name in the item event scripts.
- seq_mapchange strings, by contents: `str_tou_03`, `str_relPathFormat` ("%s/rel/%s.rel"), `str_tou`, `str_tou2`, `str_tst`, `str_jon`.

Review (Claude Opus 5.5): reverted with rename_symbol, as the n9-q-winui and mapdrv reviews did for constants that restate a value or the one call they feed: the six seq_title zero/one Vecs (`tex5Translate` ... `tex7Scale`, back to `vec3_802c11a0`..`dc`), the eleven seq_title fade GXColors (`loadFadeWhite` ... `step20SlowFadeBlack`, back to `dat_80419480`..`a8`), battle_break_slot's three colours (`slotIndicatorColor`/`slotCurrentColor`/`slotNextColor`, back to `dat_80420524`/`28`/`2c`), chuchurina's `monoBaseColor` (`dat_8041ddb8`), seq_mapchange's `.sbss2` `fadeColor` (`unk_80422f60`, like the other .sbss2 GXColor templates) and `defaultLangCodes` (`table_802C1028`, the initialiser of the local `lang` pair; nothing makes it a default). `subsetevt_check_no_status` went back to `unk_evt_803510ec`: the script returns 0 if the unit in LW(3) has STATUS_INSTANT_KILL (27), STATUS_FREEZE (9) or STATUS_STOP (2, checked twice) and 1 otherwise, so it does not test for "no status" (a poisoned or sleeping unit still returns 1), and what the three have in common for the kiss attacks that call it is not proven. `pad_0x4` in TitleWork went back to `field_0x4` (never accessed, but not an alignment gap).

Left: the value-named `float_*` constants and `zero_80420520`; `unk_80177008` (win_main.c); chuchurina's `lbl_8037EC10`..`EC40` and `lbl_80003500`; `slot->field_14` (written 2.0f/3.0f per break, never read); `bdsw->field_0x114` (BadgeShopWork).

Build: tower `ninja` recreates build/G8MJ01/ok; seq_title.c, seq_mapchange.c, battle_break_slot.c, battle_event_subset.c, mot_plane.c, system.c, unit_party_chuchurina.c and unit_party_christine.c compile without -DMUST_MATCH (rc 0).
### Readability: end_unknown REL names, rename_symbol.py for REL symbols (2026-10-20, Claude Haiku 5.5, n9-q-relren)
### Readability: end_unknown REL names, rename_symbol.py for REL symbols (2026-10-20, Claude Haiku 5.5, n9-q-relren; reviewed by Claude Opus 5.5)

`tools/rename_symbol.py` renames a REL's own symbols now: a name defined in one `config/G8MJ01/<rel>/symbols.txt` is
renamed in that map and in the REL's sources and the headers they reach through #include, and it refuses when a file
outside that set spells the name. A name defined in several REL maps needs `--rel NAME`; the other RELs' copies keep
their name, and the rename refuses when a file it would edit is also seen by one of those RELs. Headers are followed
through `#include "..."` only, and files under rels/common/src belong to no REL's set, so a name spelled there is
refused. A dol symbol that a REL map also spells is still refused. NEW is checked against every source and every
map (dol and REL). The NEW-clash, word-boundary and pair checks are unchanged; a repeated OLD is refused.

end_unknown.c (the q-endu entry on n9-q-endu was blocked on this): `unk_end_*` and `unk_evt_end_*` functions and
scripts, `dat_end_*`/`zero_end_*`/`vec3_end_*`/`dat_ptr_end_*` data, and the bss slots, renamed by where each is
consumed:
- evt_ext_entry callbacks: `credits_ext_init` (extEntry's init slot), `credits_ext_update` (cb0, per frame),
  `credits_ext_draw` (cb1) and `credits_ext_disp` (its dispEntry callback, extDraw).
- Event scripts: `evt_credits_start` (relSetEvtAddr "end_00"), `evt_credits_sequence` (evtEntry stored in
  CreditsWork.eventId), `evt_credits_fade` (SPAWN_EVENT_GET_ID in the sequence), `evt_credits_end` (SPAWN_EVENT at the end),
  `evt_credits_pose_loop` (SPAWN_EVENT in start), `evt_credits_moamoa` (the moamoa ext setup script). The npc scripts
  are named by the npc they create (`evt_credits_gesso`, `_gonbaba`, `_batten2`, `_atomic`, `_korutesu`, `_para`,
  `_batten`, `_queen`, from their STRING("...") arguments).
- CreditsWork setters called from scripts: `credits_set_mat_alpha`, `credits_set_overlay_alpha`, `credits_set_mode`,
  `credits_set_tev_alpha` (each writes one field), `credits_draw` (dispEntry callback that reads GW(11)),
  `credits_disp_start` (registers it), `credits_record_evt` (the per-record text and layout step), `credits_init`,
  `credits_free` (from _epilog), `credits_font_reset`, `credits_npc_setup` (animPoseSetGXFunc and the tev callback
  for one npc), `credits_npc_kind` (the wait-kind result 0/1/2 read by the pose loop), `credits_pose_evt` and
  `credits_pose_disp`, `credits_pose_gx`, `credits_tev_callback`, `credits_rand_range`, `credits_moamoa_setup` and
  `credits_moamoa_free`.
- Data: `credits_work` (the 14-word block dat_ptr points at, CreditsWork's size) and `credits_work_ptr`,
  `credits_ending_records` (the 50 x 0x24 CreditsEndingRecord table; only renamed, not retyped),
  `credits_pose_states`, `credits_ext_setup`, `credits_chain_counts`, `credits_chain_groups`, `credits_ext_npc_name`/
  `credits_ext_anim_name` (the "c_n_moa_a"/"S_1" strings), and the GXColor/Vec constants by their use
  (`credits_image_mat`, `credits_pass0_mat`, `credits_pass1_mat`, `credits_mode1_tev`, `credits_mode2_tev`,
  `credits_overlay_mat`, `credits_overlay_cam_pos`, `credits_overlay_cam_up`, `credits_overlay_target`).
- `dat_80419a28` (main map, 0x2F = g_moamoa_chain_tbl[47]'s length) is `moamoa_chain_count`.

Left: `unk_end_000009a4` (a USER_FUNC in end_unknown.h with no caller in the tree; no consumer to name it by);
`negone_one`, `one_negone`, `neg_one` and `zero_end_00003038` (extend_data and `.rodata` values, value-only).
`evt_credits_moamoa` is the event in the c_g_moamoa entry of R_credits_npc_info and creates モアモア. The raw
`(u8*)credits_ending_records + offset` sites in credits_record_evt stay, because they carry the MUST_MATCH CSE
workaround, as does the register-pinning `off` in credits_pose_evt. Typing
credits_ending_records and R_credits_npc_info is still open and needs a per-step build check.

Verified: tower `ninja` recreates `build/G8MJ01/ok` after the end renames and again after the moamoa rename (32 files OK);
end_unknown.c compiles without MUST_MATCH (the ci.natural command run by hand). Index (tools/cleanup_index.py):
whole tree 34.468 -> 34.244; end_unknown.c 113.415 (weighted 175) -> 18.146 (weighted 28).

### Readability: mot_ship, party_cloud offset casts (2026-10-20, Claude Haiku 5.5, n9-q-rawmot)
- offset_to_field `--apply` on mot_ship.c, party_cloud.c and party_yoshi.c. mot_ship: 0x9C is `dispOfs.y`, 0x52 is `airTimerPeak`, 0x19C is `moveDir`.
- mot_ship's 0x2A4 is an s32 paper animation time: marioReInit_ship sets it to 0x58, it is passed to marioSetPaperAnimeLocalTime each frame, and the landing counts it down to 0x2C. It is `paperAnimeTime`, a new member of the 0x2A4 union in mario.h beside `motionSfxId0` (mot_hip's sound handle), not `motionSfxId0`.
- mot_ship's f32 0x2B0 is the jump speed added to `dispOfs.y` and is now `field_0x2B0`. Its 0x11C/0x124/0x128 landing reference are `field_0x11C.x`, `field_0x11C.z` and `field_0x128`, the evt_mario names.
- party_cloud: PartyCloud 0x10/0x14/0x18/0x1C are `sweepOffset1`, `sweepDown1`, `sweepOffset2`, `sweepDown2` (replacing `field_0x10[16]`). getHitBreatheout2 sweeps two hit rays from `unk4` by an offset that moves 1 per frame between -20 and 20, and the flag is 1 while the offset decreases. 0x4 and 0x8 are `unk4` and `unk8`.
- party_yoshi's 0x2B4 is the f32 inertia z component (inertia() accumulates it from the stick, L_yoshiFlyMove negates it). It stays raw: `jabaraAccel` is mot_jabara's accel and does not fit.
- Left: party_cloud 0x17C/0x180 (`= 0` integer stores into the flurrie union's f32 `bobAngle`/`bobStep`, a float name would change codegen), 0x2E (u16 read of the `currMotionId` enum, the header's MarioMotion), 0x3D (nested cast), 0x94 (MUST_MATCH); mot_ship 0x7C/0x80 (MUST_MATCH), 0x1F0 (HitEntry), 0x18C (pad); party_yoshi 0x18C, 0x1F0, 0x39 and 0x2B4 as above.
- Index 33.803 -> 33.596 per kLOC (`tools/cleanup_index.py`).
- Build: tower `ninja` recreates build/G8MJ01/ok. mot_ship.c and party_cloud.c natural-compile without -DMUST_MATCH (rc 0).

Review (Claude Opus 5.5): every converted site checked against its motion. 0x2A4 is the frame count marioSetPaperAnimeLocalTime stores in poseTime and gives animPoseSetLocalTime, so `paperAnimeTime` stands; 0x9C/0x52/0x19C/0x11C/0x128 match their existing roles; the sweep fields match getHitBreatheout2 (the two rays start in opposite directions: sweepDown1 0, sweepDown2 1). MarioWork stays 0x2E0, PartyCloud's 0x10..0x20 span is unchanged. No changes.
- Review follow-up: mot_ship's five `((void (*)(s32))marioSetPaperAnimeLocalTime)(...)` calls are now direct calls, and the stale "header declares (s16)" comment is gone; mario.h already declares the s32 prototype, and the build stays byte-identical. (Claude Opus 5.5)
### Readability: battle raw offsets, battle_stage_object and battle_database_jon (2026-10-20, Claude Haiku 5.5, n9-q-rawbtl)

BattleUnit gets `stageObjectId` (0xB2C, s32, the BattleStageObject id of a tree or switch unit: the spawn in
battle_stage_object writes it and unit_object_tree/unit_object_switch read it through BattleGetObjectPtr;
unit_object_tree's `field_0xAE4[0x48]` read is now `->stageObjectId`), `ceilFallEventCode` (0x2BC, void*, set by
btlevtcmd_SetEventCeilFall, whose raw write in battle_event_cmd.c now uses it; _fall_frame_wait_unit_ready passes it to
evtEntry) and `ceilFallEventId` (0x2C0, s32, set from the entry's eventId; _fall_frame_wait_unit_ready returns
EVT_RETURN_BLOCK while evtCheckID holds it). The four `(u8*)event + 0x8C` nozzle SFX ids in _nozzle_work_effect are
`event->userdataEx[1]` (evtmgr.h: userdataEx is s32[2] at 0x88). In battle_database_jon, the three
`(u32)((u8*)&jon_btlobject_data + 0x30/0x60/0x90)` entries are `&jon_btlobject_data[2]/[4]/[6]`: sizeof
BattleStageObjectData is 0x18 and 0x30/0x60/0x90 are 2/4/6 of those.

Left, and why:
- battle_database_jon's other 73 casts, `(u8*)jon_btlstage_data + 0x1B4 / 0x368 / 0x51C` (60/15/1 sites), are the
  starts of stage blocks 1-3 in the u32[436] blob that still says `TODO: type`. The blocks are 0x1B4 bytes each (a
  BattleStageData header followed by its inline objects and tables), so there is no field to name without typing
  the whole blob. The same `(u8*)&X_btlobject_data + N` address-arithmetic shape is in 13 other battle_database_*.c
  files (aji, dou, eki, gon, gor, gra, hei, jin, las, mri, muj, tik, tou); they were not changed here.
- battle_stage_object: `base`/`w` pointers at wp + 0x20000 (the 0x20000-based `base - 0x7Fxx` reads, 26
  "no outer cast" sites) and the stageData tail offsets 0x10/0xD0/0x194/0x198/0x19C stay raw; 0x180C4 (f32),
  0x180F4/0x180F8 (nozzle and fall-object table pointers) and the fog pair 0x1829C (s32, counted down by
  BattleFogEndCheck) / 0x182A0 (EffectEntry*, soft-deleted by both fog functions) are still unnamed; both fog
  functions keep the raw offsets.
- battle_stage_object's 2497/2498 MUST_MATCH casts stay.

Build: tower ninja recreates build/G8MJ01/ok (all units 100% code, battle_stage_object 100%, battle_database_jon
100%). Natural (no -DMUST_MATCH) compile of battle_stage_object.c, unit_object_tree.c, unit_object_switch.c and
battle_database_jon.c returns 0.
Index (`tools/cleanup_index.py`): 33.803 per kLOC (23696 weighted) before, 33.762 (23667) after.
Review (Claude Opus 5.5): the 0x2BC/0x2C0 pair was `fallWaitEventCode`/`fallWaitEventId`; its writer is
btlevtcmd_SetEventCeilFall, so it is `ceilFallEventCode`/`ceilFallEventId`, and that command's raw store now uses the field.
The nozzle_l NOZZLE_WORK_BODY argument was a fourth `event + 0x8C` site.

### Readability: typed btlobject arrays indexed instead of byte offsets (2026-10-20, Claude Haiku 5.5, n9-q-btldb)
- `(u8*)&X_btlobject_data + N` becomes `(u32)&X_btlobject_data[N / 0x18]` where X_btlobject_data is declared `BattleStageObjectData[...]`. Same addresses; the outer `(u32)` cast stays.
- Converted (21 sites): eki 3 (`[8]`, `[12]`, `[16]`), las 9, mri 3 (`[11]`, `[22]`, `[33]`), muj 3, tik 3 (`[8]`, `[13]`, `[15]`).
- Left: the u32-declared `X_btlobject_data[]` blobs marked `// TODO: type` in aji (8 refs), bom (2 refs, no offsets), dou (7), gon (6), gor (3), gra (3), hei (12), jin (7), tou (tou2 rel, 3). Retyping them means rewriting each initializer from u32 words into struct form (floats as bit patterns), which is more than an access-expression change, so they are not touched here.
- Already typed with no offset sites: moo, rsh, win. jon was done in cf1918eb.
- Index unchanged: 33.358 per kLOC before and after (`tools/cleanup_index.py --rev`), since the tool does not count these expressions.
- Build: tower `ninja` recreates build/G8MJ01/ok.
### Readability: win_root and win_item item, mail, sort and use state (2026-10-20, Claude Haiku 5.5, n9-q-rawwin)

### Readability: win_root and win_item item, mail, sort and use state (2026-10-20, Claude Haiku 5.5, n9-q-rawwin; review Claude Opus 5.5)

The raw offsets in these files are `WW_S32(work, 0x..)` / `WM_S32(0x..)` macros, which offset_to_field.py doesn't convert. The names went into WindowWork (include/win/win_main.h) by where each field is read and written, keeping every offset and the struct size:

- `sortReverseNext` (0x180): which comparator win_root's next sort press uses. winSortEntry clears it; each sort_*_func passes pouchSortItem(n) when it is 0 and (n + 7), the `_r` comparators, when it is 1, then flips it (the mail sorts pick compare_func4/5 or their `_r` versions the same way).
- `itemState` (0x20C), `itemTab` (0x210: 0 = held items, 1 = key items, the STR_MOCHI_ITEM and STR_MOCHI_DAIJI choice), `itemSel[2]` (0x214, cursor per tab; sort_1_* use [0], sort_2_* use [1]).
- `mailCount` (0x254, pouchReceiveMailCount), `mailSel` (0x258), `mailPage` (0x25C, mailSel / 5), `mailScroll` (0x268), `mailLines` (0x26C, u16 from FontGetMessageWidthLine, +10 when the mail has a picture).
- `useItemId` (0x2D4), `useItemSlot` (0x2D8, the index pouchRemoveItemIndex takes), `useTargetSel` (0x2DC, 0 = Mario, else the joined partner), `useTimer` (0x2E0, cursor blink while picking the target, then removal after framerate * 2).
- `skipList[0x79]` (0x2E8, s16: the readers load it with lha) and `skipCount` (0x3DA): winMakeSkipList's key items, those not in menu_skip_list. The file-local ItemWorkTail overlay that held them is gone; win_item reads and writes the WindowWork fields.
- WinRoot overlay (win_root.c): `tabSel` (0x40) and `tabCount` (0x44), the tab index and count (tabs stride 0x18). They stay in the existing overlay because WindowWork's field_40 has to remain a u8 array (see its comment).

win_main.c's `WM_S32(0x2D4/0x2D8/0x2DC)` reads are now `wp->useItemId`, `wp->useItemSlot` and `wp->useTargetSel`.

Codegen: `((s32*)((u8*)work + 0x214))[sel]` in winItemMain stays raw (comment at case 10): the original indexes off a computed base (`lwzx`/`stwx`), and `work->itemSel[sel]` fails the matching build. `work->skipList[idx]` does match. winItemMailLine's raw 0x26C and winItemDisp's `*(s16*)(entry + 0x2E8)` pointer walk stay as well.

Left: the float positions at 0x224-0x252 (WW_F32; the book and mail slides share them), the Luigi book state at 0x2C8-0x2D0, the mail index pairs at 0x26E (the qqsort base), the sort-menu words at 0x174-0x184, and win_root's 0xC0-0x13C, 0x244-0x250 and 0xE94+ fields; their roles weren't proven.

Verify: tower `ninja` recreates build/G8MJ01/ok. win_root.c, win_item.c and win_main.c compile without -DMUST_MATCH. Index: 33.311 -> 33.296 per kLOC.

## battle_database_aji: btlobject data typed (2026-10-20, Claude Haiku 5.5)

`aji_btlobject_data` is `BattleStageObjectData[27]` (was `u32[162]`, 0x288 bytes, matching the symbols.txt size).
Each 0x18 record is decoded field by field: name pointer (string literal), the high and low halves of word 1 as
mSpawnSpecialUnitType and mLayer (spawn is 2 for all 27), the three floats of word 2-4 as mPosition, and the first two
bytes of word 5 as mNumFramesToStartFalling and mNumFramesToFall (pad bytes are zero). The six
`(u32)((u8*)&aji_btlobject_data + N)` stage references are `(u32)&aji_btlobject_data[N/0x18]` (5, 8, 11, 14, 17, 20).
The stage table stays `u32`, as in jon. Build: tower ninja recreates build/G8MJ01/ok; natural (no MUST_MATCH) compile
of battle_database_aji.c returns 0. Index: 33.311 per kLOC (23351 weighted) before and after.

## battle_database_gor: btlobject data typed (2026-10-20, Claude Haiku 5.5)

`gor_btlobject_data` is `BattleStageObjectData[6]` (was `u32[36]`, 0x90 bytes per symbols.txt). Records are decoded
the same way as battle_database_aji: name string, spawn (2 for all six) and layer from word 1, the position floats,
and the frame counts from word 5 (start 0, fall 60 for every record). The one stage reference,
`(u32)((u8*)&gor_btlobject_data + 0x60)`, is `(u32)&gor_btlobject_data[4]`. The file now includes
battle/battle_stage_object.h, as jon does. Build: tower ninja recreates build/G8MJ01/ok; natural (no MUST_MATCH)
compile of battle_database_gor.c returns 0.

## battle_database_gra: btlobject data typed (2026-10-20, Claude Haiku 5.5)

`gra_btlobject_data` is `BattleStageObjectData[22]` (was `u32[132]`, 0x210 bytes per symbols.txt). Decoded as in
battle_database_aji: names are string literals, spawn is 2 for all 22, layer is the low half of word 1, positions
are the three floats, and the frame counts come from word 5 (start 0/10/20/30, fall 60/50/40/30 in matching pairs).
The one stage reference, `(u32)((u8*)&gra_btlobject_data + 0xF0)`, is `(u32)&gra_btlobject_data[10]`. The file now
includes battle/battle_stage_object.h. Build: tower ninja recreates build/G8MJ01/ok; natural (no MUST_MATCH) compile
of battle_database_gra.c returns 0. Index: 33.311 per kLOC (23351 weighted) at the start of this chapter, unchanged
by the three typings.

### Readability: typed dou_btlobject_data (2026-10-20, Claude Haiku 5.5, n9-q-btltype2)
- `dou_btlobject_data` is `BattleStageObjectData[51]` (was `u32[306]`). Each 0x18-byte record is name, spawn (high half of word 1), layer (low half), three position floats, start and fall frame counts (first two bytes of word 5), and a zero pad; the floats are the shortest decimal literal that round-trips the original bits.
- The five `(u8*)&dou_btlobject_data + N` references become `(u32)&dou_btlobject_data[N/0x18]` (0x78, 0xa8, 0x168, 0x2a0, 0x3c0 → [5], [7], [15], [28], [40]); the index-0 reference is unchanged.
- Build: tower `ninja` recreates build/G8MJ01/ok. Natural (no -DMUST_MATCH) compile of battle_database_dou.c returns 0.
### Readability: typed gon_btlobject_data (2026-10-20, Claude Haiku 5.5, n9-q-btltype2)
- `gon_btlobject_data` is `BattleStageObjectData[10]` (was `u32[60]`). Each 0x18-byte record is name, spawn (high half of word 1), layer (low half), three position floats, start and fall frame counts (first two bytes of word 5), and a zero pad; the floats are the shortest decimal literal that round-trips the original bits. `battle/battle_stage_object.h` is included.
- The four `(u8*)&gon_btlobject_data + N` references become `(u32)&gon_btlobject_data[N/0x18]` (0x30, 0x60, 0x90, 0xc0 → [2], [4], [6], [8]); the index-0 reference is unchanged.
- Build: tower `ninja` recreates build/G8MJ01/ok. Natural (no -DMUST_MATCH) compile of battle_database_gon.c returns 0.
### Readability: typed hei_btlobject_data (2026-10-20, Claude Haiku 5.5, n9-q-btltype2)
- `hei_btlobject_data` is `BattleStageObjectData[59]` (was `u32[354]`). Each 0x18-byte record is name, spawn (high half of word 1), layer (low half), three position floats, start and fall frame counts (first two bytes of word 5), and a zero pad; the floats are the shortest decimal literal that round-trips the original bits. `battle/battle_stage_object.h` is included.
- The ten `(u8*)&hei_btlobject_data + N` references become `(u32)&hei_btlobject_data[N/0x18]` (0x138 through 0x558 → [13], [19], [25], [31], [37], [43], [49], [52], [55], [57]); the index-0 reference is unchanged.
- Build: tower `ninja` recreates build/G8MJ01/ok. Natural (no -DMUST_MATCH) compile of battle_database_hei.c returns 0.
### Readability: typed jin_btlobject_data (2026-10-20, Claude Haiku 5.5, n9-q-btltype2)
- `jin_btlobject_data` is `BattleStageObjectData[18]` (was `u32[108]`). Each 0x18-byte record is name, spawn (high half of word 1), layer (low half), three position floats, start and fall frame counts (first two bytes of word 5), and a zero pad; the floats are the shortest decimal literal that round-trips the original bits. `battle/battle_stage_object.h` is included.
- The five `(u8*)&jin_btlobject_data + N` references become `(u32)&jin_btlobject_data[N/0x18]` (0x30, 0x90, 0xc0, 0x150, 0x180 → [2], [6], [8], [14], [16]); the index-0 reference is unchanged.
- Build: tower `ninja` recreates build/G8MJ01/ok. Natural (no -DMUST_MATCH) compile of battle_database_jin.c returns 0.
### Readability: typed tou_btlobject_data (2026-10-20, Claude Haiku 5.5, n9-q-btltype2)
- `tou_btlobject_data` (tou2 rel) is `BattleStageObjectData[14]` (was `u32[84]`). Each 0x18-byte record is name, spawn (high half of word 1), layer (low half), three position floats, start and fall frame counts (first two bytes of word 5), and a zero pad; the floats are the shortest decimal literal that round-trips the original bits. `battle/battle_stage_object.h` is included.
- The `(u8*)&tou_btlobject_data + 0x90` reference becomes `(u32)&tou_btlobject_data[6]`; the index-0 reference is unchanged.
- Build: tower `ninja` recreates build/G8MJ01/ok. Natural (no -DMUST_MATCH) compile of battle_database_tou.c returns 0.

### Tooling: offset_to_field.py offset macros and union outcome (2026-10-20, Claude Haiku 5.5, n9-q-o2fmac; review fixes Claude Opus 5.5)

`tools/offset_to_field.py` now recognises file-local offset macros, `#define NAME(p, off) (*(T*)((u8*)(p) + (off)))`
with `p` and `off` parenthesised in the body (WW_S32/WW_F32/WW_U16/WW_PTR in win_root.c and win_item.c, MW_S32/MW_U16/...
in koopa_motion.c, P8/PF32 in party.c, BU_S32, BSO_WP_*, MW_PTR, and `ENC(p, T, off)`-style type parameters). Each use
is analysed as the expansion of its body, the same cast a raw site is, and a site is reported at the call. A rewritten
use becomes the field access (`S32(*pp, 4) += 1` -> `(*pp)->y += 1`, `&S32(w, 4)` -> `&w->y`), and the #define (and an
#undef of it) is dropped once the name appears nowhere else in that file: a use left in a MUST_MATCH or `#if 0` branch,
a comment, or another macro's body keeps it. Only uses after the #define and before an #undef of it are expanded (a
same-named function before or after is left alone); uses in comments and strings are skipped; a name defined twice is
not recognised. Macros of other shapes (WW_F32X, GND_CENTI, NKF(off) with a fixed base, headers' macros) are not
recognised. The exact-type rule holds for macro uses: no pointer retyping through a macro (the retype exception stays
for raw casts only).

Union: any offset that lands inside a union, at any depth (through nested structs and arrays of any rank), is now
"union: needs a human" and no member is picked, even when exactly one member matches. This supersedes the older
sentence in "Tooling: offset_to_field.py" above that says a union may pick its one matching member.

`--list` on src/window/win_root.c, src/window/win_item.c, src/koopa_motion.c on main b8ba7a3b (570 (u8*) casts, 430
of them macro uses): convertible 17 (all raw casts in win_item.c; no macro use converts); lands in padding/unk field 401
(400 of them macros, mostly win_root.c `WW_*`); union: needs a human 11 (all `MW_U16(mario, 0x2A0)`-family in
koopa_motion.c); type mismatch 11; untyped base 35; not a base + offset cast 47; no outer cast 28; offset not a literal
17; MUST_MATCH 2; outer cast not a pointer 1. Whole tree: 3107 casts, 1235 macro uses, convertible 173 (85 macro uses,
70 of them P*/PF32 in party.c).

Check (before the rebase onto b8ba7a3b): `--apply` on party.c, win_root.c, koopa_motion.c, battle_damage.c and npcdrv.c (74 rewrites, no #define dropped:
every macro still has uses) built byte-identical on the tower (build/G8MJ01/ok recreated); those rewrites were reverted, not committed.

## battle_stage: BattleStageData covers the 0x1B4 stage record (2026-10-20, Claude Haiku 5.5, n9-q-stagedata)

`jon_btlstage_data` is 0x6D0 bytes: four 0x1B4 records that differ only in mObjects (`&jon_btlobject_data[0/2/4/6]`).
Each field's role comes from the readers in src/battle:
- 0x0 / 0x4: the dir strings. seq_battle's `bmapLoadRank` reads 0x4 as the specific dir. Nothing in src reads 0x0, so
  its name is the header's existing one; its value ("bti_01") matches the rank literal bmapLoadRank passes.
- 0x8 / 0xC: `mNumObjects` / `mObjects` (BattleStageObjectInit).
- 0x10 and 0xD0: `BattleWeapon`s `bgsetAttackA1` / `bgsetAttackB`. `_get_bgset_attack_data` returns `stage + 0x10` for
  kind 0 and `stage + 0xD0` for kind 2, and NULL unless the weapon's `hpDamageFn` (0x1C) is set. The kind is LW(9) of
  `_object_turn_event_core`, which `_object_turn_event_A1` / `A2` / `B` set to 0 / 1 / 2, so the two weapons belong to
  the A1 and B turn events (A2 has none). The jon bytes match BattleWeapon field for field:
  baseAccuracy 100 at +0x10, superguardType 2 at +0x13, targetClassFlags 0x02100060 at +0x64, element/actionLevel
  at +0x6C/+0x6E, counterResistanceFlags 0x7FF at +0x78, field_B4[3] 100 at +0xB4. A1's hpDamageFn is NULL in all
  four records; B's is weaponGetPowerDefault with hpDamageWork[0] 1.
- 0x194 / 0x198 / 0x19C: `objectTurnEventA1` / `A2` / `B`. BattleStage_DestroyA1/A2/B run the stage's pointer and fall
  back to `_object_turn_event_A1/A2/B` when it is NULL (jon has NULL in all three).
- 0x190 and 0x1A0-0x1B3: no reader in src; zero in every jon record. Left as `field_0x190` and `field_0x1A0[20]`.

battle_stage.h now includes battle/battle_database_common.h for BattleWeapon; battle_database_jon.c and seq_battle.c
include battle/battle_stage.h. seq_battle.c's `bmapLoadRank` takes the `BattleStageData*` and reads
`mSpecificStageDataDir`.

Left: the other 17 databases still declare `u32 X_btlstage_data[]` at the 0x1B4 stride.

Verify: the tower `ninja` recreates build/G8MJ01/ok; natural (no MUST_MATCH) compiles of battle_stage_object.c,
battle_stage.c, battle_seq.c, seq_battle.c and battle_database_jon.c return 0. Index: 33.300 -> 33.051 per kLOC
(23341 -> 23185 weighted).

### Readability: party.c offset macros converted (2026-10-20, Claude Haiku 5.5, n9-q-o2fparty)
- `offset_to_field.py --verify` on src/party.c converted 70 of 140 `(u8*)` sites (`P8`/`P16`/`P32`/`PF32`/`PS8` macro uses); 0 reverted, the gate passed on the first full run. The rewrites were read site by site against the PartyEntry and MarioWork comments and the consumers.
- Names used: PartyEntry `position`, `tiltX`, `dispOffset`, `spinPivot`, `button`/`buttonNew`/`buttonRepeat`/`buttonUp`, `triggerLeft`/`triggerRight`; MarioWork `position`, `moveDir`, `dispReqDir`, `dispRot.x`, `wPlayerCollisionBox.y`. Left raw: 36 that land in padding or `field_*` (e.g. `P8(party, 0x34)`, `PF32(party, 0x124)`), 27 type mismatches, 6 non-literal offsets (`partyPoseTbl`), 1 not a base + offset.
- Build: tower `ninja` recreates build/G8MJ01/ok. Natural (no -DMUST_MATCH) compile of src/party.c returns 0.
- Index unchanged at 33.300 per kLOC: `tools/cleanup_index.py` counts `(u8*)` casts and does not count the `P*`/`PF32` macro uses this converts.

### Readability: offset_to_field sweep outside party.c (2026-10-20, Claude Haiku 5.5, n9-q-o2fall)
- `tools/offset_to_field.py --verify` over the 25 files with convertible sites (`src/party.c` excluded; the brief's `src/party/party.c` does not exist). 86 of 103 sites kept. `win_item.c` (17 sites) was reverted because its rewrite broke the build; its casts stay raw. Union sites stay raw.
- Names come from the existing header declarations: `AnimData::clip` (animdrv.h, 0x24); `AnimationPose::groupWorldPos`/`groupWorldPosCount`/`groupWorldMtx`/`groupWorldMtxCount` (0x15C-0x168); MarioWork `dispOfs`, `dispPivot`, `dispRot`, `moveDir`, `prevPosition`, `hitobjHead`, `hitobjDamageReturn`; PartyEntry `spinPivot`, `moveDelta`, `followDist`; MapObject `cameraId` (mapdrv.h 0x5); MobjWork `childName`; BattleWork `commandMenu.partyChangeId`, `restoreId`, `restoreState`; BattleUnit `seqPhase`; BattleIcon `moveCurrentPos`; WindowWork `flags`, `buttonTrg`, `dirRep` (win_main.h).
- Comments that called these fields unnamed or padding are removed in the kirimomi (moo_00, moo_03), `lect_map_cam_chg_sub` (evt_lect_win.c) and `BattleChangeParty` (battle.c) sites.
- Converted per file: rels/aji/src/event/evt_shuryolight.c 2; rels/aji/src/unit/unit_boss_magnum_battender_mkII.c 1; rels/las/src/event/evt_shuryolight.c 2; rels/las/src/unit/unit_boss_black_peach.c 3; rels/moo/src/moo_00.c 8; rels/moo/src/moo_03.c 7; rels/moo/src/moo_04.c 9; rels/mri/src/mri_01.c 4; rels/rsh/src/unit/unit_boss_moamoa.c 8; rels/win/src/event/evt_lect_win.c 1; src/battle/battle.c 1; src/battle/battle_icon.c 2; src/battle/battle_seq.c 4; src/driver/casedrv.c 1; src/effect/eff_nice.c 4; src/event/evt_bero.c 10; src/event/evt_damage.c 1; src/event/evt_mobj.c 1; src/event/evt_party.c 4; src/motion/mot_hammer.c 2; src/motion/mot_jabara.c 1; src/motion/mot_slit.c 2; src/window/win_log.c 1; src/window/win_mario.c 1; src/window/win_party.c 6.
- Build: tower `ninja` recreates build/G8MJ01/ok. Natural (no -DMUST_MATCH) compile of each of the 25 changed files returns 0.
- Index: `tools/cleanup_index.py` 33.300 -> 33.109 per kLOC (raw_offset 1402 -> 1335).

### MUST_MATCH survey and cleanup: equivalence without -DMUST_MATCH (2026-10-20, Claude Haiku 5.5, n9-q-mmsweep, TAG mmsweep)

- Survey: all 397 .c files under src/ and rels/ containing MUST_MATCH were compiled twice with the ninja flags of their own object (once as built, once with `-DMUST_MATCH` removed) and compared with `tools/obj_equiv.py`. 0 failed to compile; 19 are equivalent, 378 differ. The harness is `build/survey/survey.py` on the tower (scratch, not tracked).
- Equivalent (19): rels/common/src/unit/unit_ice_pakkun.c, rels/common/src/v1/unit/unit_monochrome_pakkun.c, rels/common/src/v1/unit/unit_pakkun_flower.c, rels/common/src/v2/unit/unit_monochrome_pakkun.c, rels/common/src/v2/unit/unit_pakkun_flower.c, rels/common/src/v3/unit/unit_monochrome_pakkun.c, rels/gor/src/gor_02.c, rels/jin/src/unit/unit_atmic_teresa.c, rels/las/src/unit/unit_boss_batten_leader.c, rels/las/src/unit/unit_boss_batten_satellite.c, rels/muj/src/unit/unit_poison_pakkun.c, rels/tou2/src/unit/unit_boss_macho_gance.c, src/battle/ac/ac_timing_wide.c, src/battle/battle_sub.c, src/effect/eff_hibashira.c, src/effect/eff_number.c, src/effect/eff_ultra_hammer.c, src/effect/n64/eff_kemuri2_n64.c, src/party/party_vivian.c.
- The survey compared one object per source. rels/common sources build into several rel modules (jon, bom, mri, muj, tou2, win, ...); the 15 edited files below were rechecked module by module against their HEAD sources, all equivalent.
- src/event/evt_yuugijou.c is NOT equivalent: with and without -DMUST_MATCH the objects differ at .text+0x7f1 (byte 2086). Its yuugijou_init workaround stays until the difference is resolved.
- Cleanup, 15 files: the `#ifdef MUST_MATCH` branch is removed and the `#else` body kept, along with the comments and MUST_MATCH-only declarations that only described the workaround. Files: rels/common ice_pakkun, v1 monochrome, v1 pakkun_flower, v2 monochrome (helper) and v3 monochrome; gor_02 (intpl_f), atmic_teresa (teresa_intpl), batten_satellite (bts_intpl), poison_pakkun, macho_gance (gance_intpl), ac_timing_wide (two BattlePad checks lose their signed cast), battle_sub (the volatile extern), party_vivian (the volatile store), eff_hibashira (the MUST_MATCH `position` local and its copy), eff_number (the NUM_COL macro and `mask` are removed; uses become the colour fields). Not touched: the unconditional MUST_MATCH code and comments in batten_leader, kemuri2_n64, ultra_hammer, and the v2 pakkun_flower helper, whose comment also describes the opt_propagation setup.
- Verify: tower `ninja` (after `configure.py`) recreates build/G8MJ01/ok. Object check for the 15 edited files, per module, against the HEAD sources: all equivalent under obj_equiv.
- Index: 33.298 per kLOC (23341 weighted over 700968 code lines) before, 33.239 (23295 over 700838) after.
- Not committed: tools/ci.py cannot run in this worktree yet (the tower copy has no git metadata, and build/ci/accepted.json has no baseline for this branch), so the commit gate in CLAUDE.md is not met.

## battle_stage: nine more stage tables typed as BattleStageData (2026-10-20, Claude Haiku 5.5; tail names Claude Opus 5.5, n9-q-stage2)

- Header tail, layout and size unchanged. Readers are all in `BattleStageObjectInit` (battle_stage_object.c), which now
  reads them as fields instead of through the removed `BSO_STAGE_EVT` / `BSO_STAGE_S8` offset macros:
  - 0x190 `void* initEvent`: started with `evtEntry` when non-NULL. The blobs that set it point it at the original-named
    `stg_02_1_init_evt` (mri), `stg_08_1_init_event` / `stg_08_6_init_event` (las).
  - 0x1A0 `void* layerEvents[4]`: layer N's event is started when non-NULL and the stage has layer-N objects; its event
    id goes to BattleWork 0x180E4 + 4N. Blobs: `&bgset_rsh_bg_event` in [2] (rsh), `&bgset_*_event` / `_evt` in [3]
    (moo, muj, tou, las).
  - 0x1B0 `u8 field_0x1B0`: no reader; 0 in every record.
  - 0x1B1 `s8 layerDestroyed[3]`: read as s8; a set byte for layer 0/1/2 ORs `stageFlags` with 0x2/0x4/0x8, the bits
    `_mobj_destroy` sets when it destroys that layer and that setup also sets for a layer with no objects. The old
    u32 words 0x00010101 (tik, tou, las) and 0x00010001 (mri) are `0, { 1, 1, 1 }` and `0, { 1, 0, 1 }`.
  jon's records name the slots too (`initEvent` NULL, `layerEvents` `{ 0 }`).
- Converted, one commit each: moo (1 record), rsh (1), win (1), tou2 (2), tik (4), mri (4), muj (4), jin (6), las (10).
  Each `u32 X_btlstage_data[N]` is now `BattleStageData X_btlstage_data[K]` in jon's layout: object pointers are
  `&X_btlobject_data[i]`, strings and functions are written by name, the weapons use jon's field order, and the three
  flag words are 8-digit hex. Every converted record has `icon` 0, `item` ITEM_NULL, `special` 0, `element` 0,
  `acHelpMsg` NULL, `field_0x14` 0.0f and `targetWeightingFlags` 0, so those fields are unambiguous. The
  `(struct BattleStageData*)` casts at the reference sites are gone: record 0 is the bare name, record k is
  `&X_btlstage_data[k]`. `battle_database_tou.c` already included battle_stage.h; the rest gained the include.
- Left: aji, bom, dou, eki, gon, gor, gra and hei keep `u32 X_btlstage_data[]`; their casts are untouched.
- Verify: tower `ninja` recreates build/G8MJ01/ok after the header change and after each file. Natural (no
  -DMUST_MATCH) and MUST_MATCH compiles of every converted file and battle_stage_object.c return 0.
  `tools/cleanup_index.py` 33.051 -> 32.248 per kLOC (23185 -> 22770 weighted; `--rev a704ccf4` to HEAD). The non-blank
  line total rose 701484 -> 706100 because the initializers are longer.

## battle_stage: eight stage tables typed as BattleStageData (2026-10-20, Claude Haiku 5.5; tail adapted by Claude Opus 5.5, n9-q-stage1)

`X_btlstage_data` in battle_database_{aji,bom,dou,eki,gon,gor,gra,hei}.c is now `BattleStageData X_btlstage_data[N]` with
full initializers, N = blob bytes / 0x1B4: aji 7, bom 1, dou 6, eki 4, gon 5, gor 2, gra 2, hei 11. The `(struct
BattleStageData*)` casts at the btlgrp reference sites are gone; they are `&X_btlstage_data[k]`, or bare `X_btlstage_data`
for k = 0. Each file includes battle/battle_stage.h. Values were decoded from the big-endian words; pointers keep their
symbols (dir strings, `&X_btlobject_data[i]`, `weaponGetPowerDefault`).

The tail uses the n9-q-stage2 names (`initEvent`, `layerEvents[4]`, `field_0x1B0`, `layerDestroyed[3]`; see the entry
above). `initEvent` is NULL in all eight files. Event pointers: aji[6] `layerEvents[3]` = `&bgset_aji_space_event`,
bom[0] `layerEvents[3]` = `&bgset_bom_snow_event`, dou[3..5] `layerEvents[2]` = `&bgset_dou_wave_event`. The old last
word 0x00010101 (bom[0], dou[0..1], dou[3..5], eki[0..1]) is `0, { 1, 1, 1 }` and 0x00000101 (gra[1]) is
`0, { 0, 1, 1 }`. Records whose tail is all zero stop at `layerEvents`, as jon's do.

`bom_btlobject_data` is still `u32[66]`, so bom's mObjects carries a `(BattleStageObjectData*)` cast. Retyping that
array is left for its own change.

Left: nok and pik still declare `u32 X_btlstage_data[]`; with n9-q-stage2 every other `btlstage_data` symbol in
config/G8MJ01 is typed.

Verify: tower `ninja` recreates build/G8MJ01/ok on top of n9-q-stage2, and natural (no MUST_MATCH) and MUST_MATCH
compiles of the eight battle_database_*.c return 0. Index on the n9-q-stage2 tip 32.248 per kLOC (22770 weighted over
706100 code lines) -> 31.420 (22348 over 711268). The line count grew because of the new initializer lines.

## battle_stage: nok and pik stage tables, bom object array typed (2026-10-20, Claude Haiku 5.5, n9-q-stage3)

`nok_btlstage_data` and `pik_btlstage_data` are now `BattleStageData X[1]` with full initializers, in jon's field order.
Both blobs are one 0x1B4 record. mObjects is `&nok_btlobject_data[0]` and `&pik_btlobject_data[0]`, and the
`(struct BattleStageData*)` casts at the btlgrpsel references are gone (`nok_btlstage_data`, `pik_btlstage_data`).
Each stage's tail (0x190-0x1B3) is all zero, so initEvent, the object-turn events, and layerEvents `{ 0 }` are NULL and
field_0x1B0 / layerDestroyed are not written out. Both weapons are decoded from the blob: bgsetAttackA1 has
baseAccuracy 100, superguardType 2, targetClassFlags 0x02100060, targetPropertyFlags 0x20000000, counterResistanceFlags
0x000007FF, and bgsetAttackB additionally has hpDamageFn `weaponGetPowerDefault` and hpDamageWork[0] 1. nok's A1 has
the same `weaponGetPowerDefault` hpDamageFn.

`pik_btlobject_data` was also still `u32[12]` and is now `BattleStageObjectData[2]` (`"C"` on layer 3 at the origin,
`"A2"` on layer 1 at (0, 0, -74)), so pik's mObjects needs no cast. `bom_btlobject_data` is `BattleStageObjectData[11]`
(nine objects with layer 1 or 0, then `"B"` on layer 2 and `"C"` on layer 3), and bom's mObjects drops its
`(BattleStageObjectData*)` cast; bom's only reference to the array is that stage table.

Left: no `btlstage_data` or `btlobject_data` blob is `u32` in rels/ any more.

Verify: tower `ninja` recreates build/G8MJ01/ok after each of the three files. Natural (no MUST_MATCH) and MUST_MATCH
compiles of battle_database_{nok,pik,bom}.c return 0. `tools/cleanup_index.py` 31.420 -> 31.408 per kLOC (22348
weighted both; 711268 -> 711538 code lines).

### Readability: party_motion and party_move unk_ names (2026-10-20, Claude Haiku 5.5, reviewed by Claude Opus 5.5, n9-q-unkparty)

Address-named functions and the two statics in party_motion.c, renamed with tools/rename_symbol.py (symbols.txt, sources, headers, party.c, evt_mario.c, party_chuchu/cloud/nokonoko.c, pmario_sound.c, seq_battle.c). Each name comes from the code that calls it and what it writes:
- Speed: `partySetSpeed` (unk_800c9398) sets speed2 and speed to value times gravity (0.75 in shallow water). `partySetTargetSpeed` (unk_800c93d0) sets speed2 and eases speed toward it, 5% of the gap up and 10% down. `partyStepSpeed` (unk_800c92c4) is that easing with no new target; chuchu and nokonoko call it every frame.
- Pose and sound: `partyChgSpeedPose` (unk_800c8f24) picks walk pose 2 or run pose 3 from the speed and calls `partyStepSfx` (unk_800c90d4), the footstep counter in field_2D (run cadence when the value is 1). `partyChgIdlePose` (unk_800c9044) picks sleep pose 6 when Mario is in deep sleep or the sleep flag is set, otherwise pose 1.
- Facing: `partySetDispDirToMario` (unk_800c8e74) writes field_110 (the display direction, elsewhere copied from Mario's dispReqDir) as 180 or 0 from the party-to-Mario angle relative to the camera yaw (range 90..270).
- Hit test: `partyHitIsBoxMobj` (unk_800c8dd0) is true when the hit's MOBJ name is in the file's boxNames list (boxes, jump stands, switches, blocks).
- Distance: `partyGetTargetDistYFrom` (unk_800c9f48) is the absolute y gap to Mario for id 0, else to the other party target or Mario.
- Run mode 1: `partyIdleMain` (unk_800c7c98) is driveParty's field_34 == 1 handler, built like party_move_mode_stay: it switches on field_36 and runs `party_motion_idle` (unk_8014bbc8) for motion 0, as party_move_mode_stay runs party_motion_stay. party_motion_idle keeps the partner in place (idle pose, wall and floor checks, Mowz's search animation) and switches to run mode 2 once Mario is more than 31 away.
- Ground: `partyUpdateFloor` (unk_8014ba3c) probes head and ground hits, sets groundHit and the grounded and shallow-water flags, snaps to groundY, and starts a fall when no floor is found. `partyFollowHeight` (unk_8014b824) eases y toward Mario's height (+5, or +1 with flags2 0x8000) at 5% or 10%.
- `partyResetTouchWallCt` (unk_8014c47c) sets touch_wall_ct to 60; party.c calls it after chuchu_reinit.
- `upstairs_frames` (unk_804189ec, s32) and `upstairs_step_y` (unk_804189e8, f32), snake_case like their neighbour touch_wall_ct, are party_motion_upstairs' frame countdown (set to 3) and per-frame y step, (field_120 - y) / 3.
- Flurrie (party_cloud.c, cloud* prefix like cloud_init and cloudGetBreathDist): `cloudBobInit` (unk_801820d0) zeroes tail.flurrie.bobAngle and sets bobStep 4.25, as cloud_move does; `cloudBobUpdate` (unk_8018206c) advances the angle and sets dispOffset.y to 3 + 3 sin(angle).
- `psndBGMOn_f` (unk_800d7270, pmario_sound.c) is the wrapper evt_snd_bgmon_f calls: psndBGMOn_f_d with the caller's fade time and 0x2EE as d, as psndBGMOn is psndBGMOn_f_d with both fixed.

Left: unk_8014cc24 (a USER_FUNC in party_motion.c that tou_00 calls with the same value twice as the fade arguments; the meaning of the repeat is not proven) and unk_800ba2dc (party.c, the field_34/field_35 mode-change handler; its branches do different end actions per mode and no single name covers them).

Verify: tower `ninja` recreates build/G8MJ01/ok (matched build, checksum holds). Natural compiles (no -DMUST_MATCH) of the nine changed C files return 0. Index (tools/cleanup_index.py) on current origin/main: 31.408 -> 31.040 per kLOC (22348 -> 22086 weighted over 711538 code lines); unk_name 1169 -> 1038, address_name 10253 -> 10122.
### Readability: yuu and gor REL unk_ names from their consumers (2026-10-20, Claude Haiku 5.5, n9-q-unkrel)

Names from `tools/rename_symbol.py --rel` in the yuu and gor maps, by where each symbol is consumed.

yuu_02.c: `monte_single_init` (USER_FUNC called by `monte_single_event`, the script yuuminigame_init_3roll starts;
it allocates the one-entry MonteEntry table in monte_p), its dispEntry callback `monte_single_disp`, and the bss `monte_entries` (the MonteEntry table fall_rtn fills and
monte_disp draws). camShiftReset in minigame03_end is `minigame03_cam_reset`.

yuu_03.c: `yuuminigame4_entry_init` (USER_FUNC called by `yuuminigame4_entry_event`, the subEvent of
yuuminigame_init_4slit), its dispEntry callback `yuuminigame4_entry_disp`, and the bss `yuuminigame4_entries` (the
match-only inline is `yuuminigame4_init_work`). The prefix follows the file's yuuminigame4_* functions and the
`_event` suffix its ryushi_event. camShiftReset in minigame04_end is
`minigame04_cam_reset`.

gor.c: `inemuri_sfx_set` (stores the id from SFX_VOICE_MARIO_SLEEP1 in inemuri_snd) and `inemuri_sfx_stop` (stops it
when mario_and_party_sleep deletes its loop), with the bss `inemuri_sfx_id`.

gor_00.c: npcEnt's regularEvent slot (0x10 of NpcSetupInfo) gives `gor_00_mony_regl` and `gor_00_peton_regl` (the
file's <npc>_regl style; plain mony_regl/peton_regl are taken by muj_02); their CALL_CHILD targets are `mony_leave` and
`peton_leave`; `mony_peton_restart` (SET GSW 278, then restarts both regular events) is called from the marco script;
`marco_party_move` is spawned by marco_talk;
`epilogue_npc_clear` (USER_FUNC) is called first in epilogue_npc_entry.

gor_02.c: `powerupya_talk` (the talk slot of the powerupya npc in npcEnt, beside powerupya_table_talk and
powerupya_direct_talk) and its children `powerupya_stardust`, `powerupya_mario_move`, `powerupya_party_move` and
`powerupya_watasu` (CALL_CHILD'ed on a watasu pick); `powerupya_watasu_items` (the item list {108, -1} passed to evt_win_item_select);
`gor_get_mario_dir` (reads mario's dispReqDir into a variable); `evt_gor_hitobj_off` and `evt_gor_hitobj_on` (the
A_kabe/A_saku/A_kmn/A_gomi hit objects).

Left: unk_yuu_00001160, unk_yuu_000011e0 and unk_yuu_0000341c (USER_FUNCs or helpers with no caller in the tree);
unk_gor_0000041c, unk_gor_0000050c and unk_gor_bss00000000 (no caller in the tree; the buffer is used only in gor.c);
unk_evt_gor_000277f4 (no caller); unk_8009f9c8 (dol function shared by evt_nomal_cange in five RELs) and
unk_800e23e0 (dol function shared by powerupya_party_lank_rtn and tik_11.c). Their names need the function bodies.

Verified: tower `ninja` recreates `build/G8MJ01/ok` after each REL's renames (the yuu and gor sets); the natural
(no MUST_MATCH) compiles of yuu_02.c, yuu_03.c, gor.c, gor_00.c and gor_02.c return 0. Index: whole tree
31.420 per kLOC (22348 over 711268) -> 31.270 (22241 over 711268); unk_name 1169 -> 1062. Per file: yuu_02.c unk_name 30
-> 2, yuu_03.c 26 -> 2, gor_02.c 30 -> 1, gor_00.c 20 -> 2, gor.c 16 -> 8.

## memory: smart heap and map allocator fields named (2026-10-20, Claude Haiku 5.5, n9-q-memory)

`smartEntry` (memory.h): `file` (0x8) is the FileEntry an entry backs. `_fileAlloc` and `fileAsync` set it on the smartAlloc'd entry, `smartGarbage` reads `file->state` and `file->entry` and moves its data with `fileGarbageMoveMem`, and `smartFree` clears it. `freeAfter` (0x10) is the free bytes between this entry and the next one in the heap: `smartAlloc` tests it to place a new entry right after, `smartGarbage` rewrites it while compacting, and `smartFree` folds the freed entry's size and gap into the previous entry's. Left as `field_0xC` (u16, written 1 on alloc and 0 on free) and `field_0xF` (u8, never touched): nothing reads them.

`SmartWork` (memory.h): `usedHead` (0xE008) and `usedTail` (0xE00C) are the head and tail of the used list that `smartAutoFree`, `smartGarbage` and `smartAlloc` walk. The free list stays `head`/`tail`, which `smartAlloc` takes from and `smartFree` appends to.

`MapAllocEntry` (memory.c): bytes 0xC-0x20 of the header are never read or written, so they are `pad_0xC`.

Users: `src/manager/filemgr.c` `smart->file = newentry` in `_fileAlloc` and `fileAsync`. No rels/ file touches these fields. Verified: tower `ninja` recreates `build/G8MJ01/ok`; natural (no MUST_MATCH) compiles of memory.c and filemgr.c return 0. Index: 30.889 per kLOC (21979 weighted) -> 30.808 (21921); field_0x 3533 -> 3475.
### Readability: evt_memcard and cam_road unk_ names (2026-10-20, Claude Haiku 5.5, reviewed by Claude Opus 5.5, n9-q-memcam)

cardmgr slot watch: while flag 0x8000 is set, cardMain probes slot A every frame with `-(CARDProbe(0) == 0)` (0 = card
in, -1 = none, the encoding cardmgr's probe path also writes to `chan`), sets 0x4000 when the result differs from the
stored state, stores it, and counts the timer down. So the mechanism is a card inserted/removed latch, not a retry
or a "different card" (serial) check. Names: `cardSlotWatchBegin` (0x800B0ED0: seeds `slotWatchChan` (0xE4) from
`chan`, `slotWatchTimer` (0xEC) = 10, sets 0x8000), `cardSlotChanged` (0x800B0E80: TRUE when 0x8000 and 0x4000 are both
set), `cardSlotWatchEnd` (0x800B0EAC: clears 0x8000 and 0xC000). cardIsExec reports busy while `slotWatchTimer` is
nonzero. CardWork has no flags enum, so the two bits are documented on the `flags` field comment. evt_memcard USER_FUNCs:
`memcard_slot_watch_begin` (0x80256490), `memcard_slot_changed` (0x80256428, writes 1/0 into its argument) and
`memcard_slot_watch_end` (0x80256404). The scripts start the watch before the format / make-data prompts, read
`memcard_slot_changed` after each yes/no select and on 1 print `msg_out_mcard_warnning` and take the nosave path, and
end the watch on the damage / save-miss exits. cam_road: CamRoadMap `unk_104` is `curveRanges`, the 0x68-byte record
base that CamRoadCurve::curveIdx indexes; calcCamRoad hands each record to collisionCurve in mode 1, which reads only
`curvePtsStart`/`curvePtsCount` (0x60/0x64), a point range into `curvePts`. Left as they were: CamRoadMap `unk_E8`
(calcCamRoad reads its first float as an angle, calcCurrentCamState reads it as a Vec, so the element type is not
settled), `unk_108` (only the load fixup touches it), and CamRoadWork `unk_204`/`unk_208` (no reader outside
camRoadSetup/camRoadReset; unk_204 is kept across reset). Index: whole tree 30.808 -> 30.733 per kLOC
(21921 -> 21868 weighted).
### Readability: evt_nannpc, evt_cloud, evt_movefloor (2026-10-20, Claude Haiku 5.5, n9-q-evtobj)

evt_cloud.c's CloudEntry: `breathLimit` (0xA8, 180; once `breathCount` (0xAC) passes it the cloud is blown away: mode 0x14 plays A_2 and runs `endScript`) and `breathCount` (+2 per frame of cloudGetHitBreathout contact, and per frame of the 30-frame grace after it). `event` (0xB0, the cloud's main EventEntry, whose lwData[0] points back at the entry), `endScript` (0xB4, the script evtEntry'd when the cloud vanishes), `waitTimer` (0xB8, grace countdown in modes 4 and 0x15), `mapGrpNames` (0xBC, the NULL-terminated list used when flags & 2), `capWidth`/`capHeight` (0xC0/0xC2, set by evt_cloud_set_cap_size) and `lastLoopTimes` (0xC4, the img->loopTimes at the last poof sound). The REL clouds (gon_11.h, mri_00.h, las_14.h) are a separate 0x1B0 struct (mapGrpName[8][0x20], plus a bounding box), so they keep their own definitions; all three now use the same names for 0x188-0x198 and 0x1AC (las_14's maxCount, counter, script, timer and lastTime renamed to match), and gon_11 and mri_00 take las_14's `left`/`top`/`width`/`height` for 0x19C-0x1A8 (the _get_boundingbox outputs passed to _img_alloc_capture).

evt_nannpc.c's NanNpcEntry: `velocity` (0x3C, added to position each frame and damped by 0.6), `range` (0x4C, set by evt_nannpc_set_size; nanNPCOption pushes entries apart within it and pulls them home past 0.8 of it; not `radius`, because muj_10's 0x74 is already `radius`), `startTime` (0x50, renderTime at the start of a jump), `facingAngle` (0x60, set by evt_nannpc_set_dir, pose heading is its negation), `jumpStartX/Y/Z` (0x68-0x70, the integer position at jump start), `moveStart` (0x88, origin of evt_nannpc_move_position2). gor_00.h and muj_10.h take `velocity`, `range` and (muj) `moveStart`; their 0x68-0x70 keep REL names (state, timer, field_0x70). Left: 0x16 (written, never read), 0x74[0x14] and 0x94[0x10] (no access in evt_nannpc.c), and NanNpcWork's `field_0x8` (the s16 table filled from evt_nannpc_init, read nowhere in the tree).

evt_movefloor.c's MovingFloorWork: `axisPos` (0x0C, the coordinate written to pos[index] each frame), `progress` (0x10, elapsed ms on the current leg, reversed by moving_floor_reverse), `prevLifetime` (0x18, the event lifetime at the last evt_moving_floor_main2, the base of the next dt), `duration[3]` (0x68, ms per leg per axis; speed = distance / duration and get_end compares progress with duration * time), `seHalted` (0x78, set by moving_floor_halt_se after moving_floor_stop_se when moving_floor_bero_check sees an area exit; set_se then never restarts the SE and stop_se/delete skip psndSFXOff. It only gates the SE, not the floor's movement) and `turnSpeed` (0x80, added to turn each frame). Left: 0x14 (never accessed) and 0x74 (read only by moving_floor_get_stmsec, which converts it ms to frames for the script's loop; its meaning in the script is not clear enough to name).

Verified: tower `ninja` recreates `build/G8MJ01/ok`; the natural (no MUST_MATCH) and MUST_MATCH compiles of evt_cloud.c, evt_nannpc.c, evt_movefloor.c, gon_11.c, mri_00.c, gor_00.c, gor.c, muj_00.c, muj_10.c, muj_20.c and muj.c all return 0 (run by hand with -maxerrors 50). Index: whole tree 30.889 per kLOC (21979 over 711538) -> 30.617 (21785 over 711538); field_0x 3533 -> 3339.

Review (Claude Opus 5.5): `halted` renamed `seHalted`, las_14.h brought into line with gon_11/mri_00, the gon_11/mri_00 bounding box named; tower `ninja` recreates `build/G8MJ01/ok`, and the MUST_MATCH and natural mwcc compiles of evt_movefloor.c, gon_11.c, mri_00.c and las_14.c return 0. Index after rebase and review: 30.513 per kLOC (21711 over 711538); field_0x 3265.
### Readability: tik REL names for gesso arms, tik_00 and tik_05 (2026-10-20, Claude Haiku 5.5, n9-q-gesso)

### Readability: tik REL names for gesso arms, tik_00 and tik_05 (2026-10-20, Claude Haiku 5.5, reviewed by Claude Opus 5.5, n9-q-gesso)

Names from `tools/rename_symbol.py --rel tik` and by hand, by where each symbol is consumed.

Gesso arms (unit_boss_gesso_right_arm.c, unit_boss_gesso_left_arm.c, unit_boss_gesso.c):
- Each arm's `unk_evt_tik_*` script is the event that runs for data tags 0x18-0x24 of its dataTable. battle_event_default.c's
  counter-damage switch (damage codes 43-55) calls RunDataEventChild with tag 24-36, so the scripts are the counter-damage
  events: `gesso_right_arm_counter_damage_event` and `gesso_left_arm_counter_damage_event`. The 13-entry tables they sit in
  (`unk_tik_00028ebc`, `unk_tik_00029f1c`) are the arms' dataTables: `gesso_right_arm_counter_data_table` and
  `gesso_left_arm_counter_data_table`. The two arms keep separate copies: each file defines its own table, so sharing one
  event would move the object data.
- `unk_evt_tik_000243c8` in unit_boss_gesso.c is SPAWN_EVENT'd from battle_entry_event beside the two arm spawns and runs
  the `_test_rtn` USER_FUNC: `test_rtn_event`.

tik_00.c (uranai and starmaniac):
- `unk_tik_000005e8` is called from uranai_init and sets renderMode 10 on the "supotto" map object: `uranai_supotto_mode`.
- `unk_tik_000005c0` (psndSetFlag 0x80) and `unk_tik_00000598` (psndClearFlag 0x80) are called from starmaniac_talk: the
  same snd_off/snd_on pair as evt_kinopio.c: `starmaniac_snd_off` and `starmaniac_snd_on`.
- `unk_evt_tik_0000f438` is the 4th word of the starmaniac npcEnt entry (its init slot, beside uranaisi_init): `starmaniac_init`.

tik_05.c:
- `unk_tik_0000247c` takes one argument and calls N_mapDispOn or N_mapDispOff; the tizu sequence calls it with 0 before the
  paper entry and 1 after the fade: `map_disp_set`.
- BrokenWork (tiz_broken's texture and shard buffers): `texCopied` (draw sets it after GXCopyTex), `texBuffer`,
  `entryBuffer`. BrokenEntry, from tiz_broken and tiz_broken_draw: `state` (0 flying, 1 once x/y leave the 340/260 box),
  `timer` (the wait before draw, checked against 0), `quadrant` (0-3, the diagonal of the draw angle), `impulseX`,
  `impulseY`, `impulseZ` (added to x/y/z and decayed by param_tbl[2]), `spinAngle` (degrees about y) and `spinSpeed`
  (grows by param_tbl[4]), `velX`/`velY` (constant per-frame steps from the angle). x, y, z were already named.
- MahojinWork (include/eff/eff_mahojin.h) 0x28/0x2C are the draw argument and function that eff_mahojin.c's Disp calls
  and that starstone_setting fills with the star stone entry and effStarStoneDraw: `drawArg` and `drawFunc`.
- disp_treasure_map_func writes the map_map effect's fields through EffTreasureMapData (eff_treasure_map.c reads x/y/z,
  alpha at 0x14 and angle at 0x20 from that type), not through MahojinWork; the angle store is the same f32 at 0x20.

Left: MahojinWork's `field_0x14` (include/eff/eff_mahojin.h; eff_mahojin.c only zeroes it and tik_05 no longer touches it).
rels/tik/include/tik_11.h still has one `unk_` name, outside this set.

Verified: tower `ninja` recreates `build/G8MJ01/ok` (32 files OK); natural (no MUST_MATCH) compiles of the six changed C
files return 0. Index (tools/cleanup_index.py): 30.889 -> 30.731 per kLOC (21979 -> 21866 weighted over 711538 code
lines); field_0x 3533 -> 3476, unk_name 931 -> 875.

### Readability: unit_party_yoshi gundan particle and area fields (2026-10-20, Claude Haiku 5.5, n9-q-yoshiu)

`YoshiGundanDispWork` (the 0x30-byte particle used by `_gundan_yoshi_run_effect` and `_btlYoshiDisp`) and the
`YoshiGundanArea` accumulator are named from their consumers:

- `posX` (0x00): the x argument of `btlDispTexPlane` and of `effKemuTestEntry`; the area sums it into `avgPosX`.
- `posY` (0x04): added to the display base y of 20 and to the kemu effect's y base of 5.
- `posZ` (0x08) and `velZ` (0x14): z sway clamped to +/-80, reversing `velZ` at each limit.
- `velX` (0x0C): per-frame x step, stops at x = 300; its sign also picks `x_scale` in `_btlYoshiDisp`.
- `animTimer` (0x24): sub-frame counter that wraps at 2 and advances `tplIndex`.
- `tplIndex` (0x25): index into `_yoshig_tpl_no_tbl`.
- `startDelay` (0x28): frames to wait before the particle moves.
- `state` (0x2C): 0 init, 1 running, 999 finished.
- `avgPosX` (area 0x3008): the sum of all particles' posX, scaled by 1/256 for the SFX pan and read as an average
  by `_wait_yoshig_run`.

Left: `field_0x10` (written, never read), `field_0x18[12]` and `field_0x26[2]` (never accessed), and `_wait_yoshig_run`'s
`*(f32*)((u8*)area + 0x3008)`, which still uses the offset (the cast is untyped and the rewrite was not needed for the name).

Verified: tower `ninja` recreates `build/G8MJ01/ok`; natural and MUST_MATCH compiles of unit_party_yoshi.c return 0 with
no diagnostics. Index (tools/cleanup_index.py): 30.279 -> 30.197 per kLOC (21545 -> 21486 weighted); field_0x 3208 -> 3149.
### Work struct field names: eff_opuku_jetwrain, eff_fireflower (2026-10-20, Claude Haiku 5.5, n9-q-effn64b)
- `effOpukuJetwrainData`: 0x14 `counter` (Main's frame count; the sine phase and the fade-in below 16). 0x1C/0x1D/0x1E are `color1R/G/B` (10, 110, 255) and 0x20/0x21/0x22 are `color2R/G/B` (190, 220, 255), both u8 and read by Disp. 0x1F is `alpha` (Main's fade, halved into the TEVREG0 alpha in Disp). 0x23 is `height[19][13]` (s8 wave heights, the grid Disp draws) and 0x11A is `velocity[19][13]` (s16, relaxed each frame with the 0.98 damping, then added into height).
- `effFireFlowerData`: per-piece 0x20 is `velYAccel` (added to velY each frame), 0x24 is `rot` (degrees about z in Disp) and 0x4C is `delay` (frames before the piece moves). Head 0x28 is `angle` (degrees about y, +18 per frame in state 6 up to 720), 0x2C is `offsetX` (the x offset Disp applies), 0x34 is `texFrame` (0-3, the sprite row), 0x38 is `color2A` (TEVREG1 alpha) and 0x3C is `color1A` (TEVREG0 alpha, scaled by 0.9 in state 6). The head's 0x3C and the pieces' 0x3C share the field; Disp reads only the head's.
- Left: fireflower 0x48 (written by Entry and Main, never read). eff_radiation_n64 is unchanged: its remaining field_0x are 0x1C/0x20 (the `a5` argument, written and never read) and the 0x34-0x54 accumulator group, which Main updates only from its own step fields and which never reaches Disp, so no name is proven.

Verified: tower `ninja` recreates `build/G8MJ01/ok` (32 files OK); natural (no MUST_MATCH) compiles of both changed C files
return 0. Index (tools/cleanup_index.py): 30.279 -> 30.139 per kLOC (21545 -> 21445 weighted over 711538 code lines);
field_0x 3208 -> 3108.
### Readability: aji_04 statue mouth and battender scream fade names (2026-10-20, Claude Haiku 5.5, n9-q-mkii)

aji_04.c: `unk_aji_00000ed0` is the msg callback every kuizu_sekizou message passes as PTR. On state 5 (a glyph is
shown) it starts the mouth script when it isn't running, so it is `sekizou_mouth_callback` (declared in aji_04.h).
That script (`UNK_0x00022c3c`, spawned by the callback) rotates the kuti and kuti2 mapobjs and is `sekizou_mouth_anim`.

unit_boss_magnum_battender_mkII.c: `unk_aji_00007668` is the USER_FUNC run as a sibling of aud_suikomi. Its first call
sets the countdown to 120 and reads the scream volume; each frame it ramps screamId's volume from `scream_start_vol` to 0
and stops when the countdown ends, so it is `aud_suikomi_fade`, with `scream_fade_timer`, `scream_start_vol` and
`scream_vol` (the bss slots 0x168, 0x165 and 0x164).

ArmWork fields (the arm `work` entries): `prevPos` (0x00) and `pos` (0x0C), the previous and current unit positions
(`w->prevPos = w->pos` before BtlUnit_GetPos fills pos); `rotZ` (0x18) and `targetRotZ` (0x1C), the eased and the
target z angle (rotZ moves a third of the way to atan(dy/dx) each frame and is passed to BtlUnit_SetRotate).
TargetWork fields (wp_target): `state` (0x0, the 0/5/10/15 phase switch in aud_target_mark) and `timer` (0x4, the frame
counter compared with 60, 40, 20 and 90).

Left: the field_0x slots in the BattleUnitKind and BattleWeapon initialisers (battender's own unit kind and weapon
tables, field_0x13/14/30/32/3C/54/58/8C and field_0x14/19/1B/6D/6F), which are fields of the shared battle structs in
include/battle/battle_unit.h and need their own pass across every unit that uses them.

Verified: tower `ninja` recreates `build/G8MJ01/ok` (32 files OK); natural (no MUST_MATCH) compiles of aji_04.c and
unit_boss_magnum_battender_mkII.c return 0. Index (tools/cleanup_index.py): 30.279 per kLOC (21545 over 711538) ->
30.183 (21476 over 711538); unk_name 846 -> 820, field_0x 3208 -> 3165.
**yuuranking (yuu, gor) and win_main names (q-yuurank, Haiku 5.5).**
- `unk_yuu_00008e40` (yuu) and `unk_gor_0000dfc4` (gor) are the same helper: a message drawn at (x, y) with its width scaled to fit `maxWidth`, called from disp_func with `165` and `scale` 1.0. Named `yuu_draw_msg_fit` in yuu and `gor_draw_msg_fit` in gor. The two names cannot match: `tools/rename_symbol.py` refuses a NEW name already spelled anywhere in the tree, so the second REL's rename always refuses when both use the same name.
- The Pianta Parlor rank macros in both copies' disp_func (`A14`/`A74`/`AD4`, the `yuwp + mode*24 + 0x14/0x74/0xd4` casts) now read `yuwp->rankUsed`/`rankScore`/`rankMonte[mode][i]` (gor: `[field_14][i]`). Both objects are byte-identical to the pre-change build. The alloc-side `YBASE_A/B/C` array-view macros stay: switching them to the struct fields changes the object (the emitted store sequence moves), so they are left as they were.
- win_main: `unk_80175eb0` is the lecture-step gate in evt_lecture_msg (returns 0 until `lectureStep` reaches its argument, then sets flag 0x800): `win_lecture_wait`. `unk_80175e98` is the closing step that clears 0x800: `winLectureEnd`. `unk_80177cf8` calls `allPartyForceSlitOff` and returns 2 in evt_item_use: `party_force_slit_off`. `unk_evt_803757f0` is the 15-word sub-script that sets the breed pose (`evt_party_set_breed_pose`) and is run from evt_item_use and the kime table: `kime_pose_breed`.
- Left: `unk_80177008` (win_main.c) draws one textured quad at translate/scale/color with the 2D camera; its callers are seq_title.c and muj_20.c and it sits right before `winTexSet` (0x80177278), so the body is a sibling of `winTexSet`/`winTexSetRot`, but no name is proven. Its header declaration in include/win/win_main.h is unchanged.

Verified: tower `ninja` recreates `build/G8MJ01/ok` after each commit; natural (no MUST_MATCH) compiles of the yuu and gor evt_yuuranking.c and win_main.c return 0. Index (tools/cleanup_index.py): 30.279 -> 30.190 per kLOC (21545 -> 21481 weighted over 711538 code lines); unk_name 846 -> 801.

### Readability: MarioWork placeholder names for the item-get timer, control/key-off counts, hit pointers and coin drop (2026-10-20, Claude Haiku 5.5, reviewed by Claude Opus 5.5, n9-q-marioc)
- `itemGetDisableTimer` (0x7A, s16): marioItemGetDisable sets 20 and sets MARIO_FLAG_ITEM_GET_DISABLED (0x4000); marioItemGetChk counts it down and clears the flag at 0. No other reader.
- `ctrlOffCount` (0x38) and `keyOffCount` (0x39, s8): nesting counts behind marioCtrlOff/On (flag 0x2) and marioKeyOff/On (flag 0x8). Read by marioChkCtrl, N_marioChkUseParty, marioCtrlOffChk/KeyOffChk and the marioMakeDispDir gate in mario.c; `keyOffCount` also by npcdrv's talk check and party_yoshi's key-data and follow checks.
- `field_0x34` (u32, was `*(u32*)(field_0x32 + 2)`): not named. Bumped by mario.c, peach.c and koopa.c motions; read only for bit 0 gating blur capture.
- `hitobjWall` (0x1F0, `void*`, was `u8 field_0x1F0[4]`): raw searchFrontWall HitEntry that mario_hit's wall checks store on both branches (also when hitobjPush is not set; unlike hitobjFront it is never swapped for hitobjBalloon); party_yoshi's wall check sets it too. Read by marioChkToge (attr 0x800), mot_roll, mot_ship, koopa_motion and party_yoshi's inertia; cleared by mot_yoshi, party_vivian, mot_ship, koopa.c and marioResetHitObj.
- `hitobjHammer` (0x1FC, `void*`): the hit the hammer struck. mot_hammer copies its hitEstimate target (0x200) into it at impact and stores the case-0x14 front ray hit; marioMotion clears it each frame; mobjdrv (hammer trigger 0x10000 and hint) and casedrv (MARIO_MOTION_HAMMER_2 case) read it.
- `coinDropTimer` (0x2C0, s16; the `field_0x2BC` array is now `u8[4]` + this + `field_0x2C2`): npcdrv sets it to sysMsec2Frame(3000) beside marioSetMutekiTime(3000) when a field battle ends with result 4 (escaped, battle_seq_end); marioMain drops a coin every 30 frames while it runs.
- Raw casts on these offsets converted in koopa.c, koopa_motion.c, casedrv.c, npcdrv.c, mot_ship.c, mot_roll.c, party_yoshi.c and mot_hammer.c (byte-identical). mot_hammer's `M_S32(mario, 0x1FC) = M_S32(mario, 0x200)` stays raw with 0x200.
- Open, MarioWork: 0x20 (`u8[4]`, zeroed in marioReInit, no other reader); 0xE0 (`Vec`, mot_dokan stores position there, mario_motion and mot_damage read it, reinit sets -10000 as a sentinel); 0x2B8 (f32 jump height from marioGetJumpHeight and marioChgJumpStandMotion, but mario.c's yoshi branch rotates by it about z, mot_hammer uses it as an s32 sfx handle and mot_jabara as an s16 timer, so no single name is proven); 0x200 (mot_hammer's hitEstimate result at hammer start, cleared at 489, copied into 0x1FC at impact).

Verified: tower `ninja` recreates `build/G8MJ01/ok`; game code 100% matched (8276/8276 functions); natural (no MUST_MATCH) compiles of the 15 changed C files return 0. Index (tools/cleanup_index.py) against current main: 29.869 -> 29.772 per kLOC (21253 -> 21184 weighted); field_0x 3006 -> 2959, raw_offset 928 -> 917.

### Readability: sys REL SysWork and SysWorkEntry fields (2026-10-20, Claude Haiku 5.5, n9-q-relsys)

Names from the reads and writes in `display_func` and the `display` user function (rels/sys/src/sys.c; struct in rels/sys/include/sys.h). Offsets and size are unchanged.

- `SysWork`: `spinAngle` (0x08, f32; +2.5 per frame, wraps past 180; the Y rotation of the big quad, with `palIndex` as its texture), `palIndex` (0x0C; texture index into effect_sys.tpl, cycled 0..3), `frameCount` (0x10; texture scroll offset, wrapped at 0x500), `stage` (0x14; index into `msg_tbl`, set from display's event argument), `reveal` (0x18; high byte is the number of characters shown, low byte the alpha of the next one; +0x30 per frame up to 0x2710), `textAngle` (0x1C, f32; wave phase of the letters, +0.8 per frame, wrapped at 360).
- `SysWorkEntry`: `timer` (0x10; the stagger delay starting at `i * 0x14 + 1`, counted down in phase 0, then the spin time 0..0x5A in phase 1), `phase` (0x14; 0 waiting, 1 spinning, 2 done), `palIndex` (0x18; texture index from the language table, passed to `TEXGetGXTexObjFromPalette`).
- No other file includes `SysWork`.

Verified: tower `ninja` recreates `build/G8MJ01/ok` (game 100% matched, 8276/8276 functions); natural (no MUST_MATCH) compile of sys.c returns 0.

### Readability: eki_00 MoamoaWork fields (2026-10-20, Claude Haiku 5.5, n9-q-relsys)

Two `MoamoaWork` fields in rels/eki/src/eki_00.c, named from their reads and writes:

- `hammerFrames` (0x40, s16): written by the per-frame update (`ext_moamoa_main`), +1 per frame while `mario->currMotionId == MARIO_MOTION_HAMMER`, else 0. `moamoa_hit_check` reads it as `> 10` next to motion 0x12.
- `bounceCount` (0x558, s32): `moamoa_hit_check`'s stomp branch bounces Mario (`marioChgMot(5)`) and counts up while it is below 2. It is cleared while `mario->flags & 0x30000` is clear.

The main comment's `MoamoaPose.field_0x02` is now `MoamoaPose.state`.

Left as `field_0x`:

- `MoamoaPose` 0x14, 0x1C and 0x20 are reused by state. In state 3 (`moamoa_jump`) they are vertical velocity, horizontal speed and gravity. In state 0x30 they are the per-frame changes to `target` x, y and z. In states 0x33 and 0x34, 0x1C is a height and 0x20 a radius.
- Never read: `MoamoaPose` 0x06, 0x3E and 0x48; `MoamoaWork` 0x0C and 0x10.

Verified: tower `ninja` recreates `build/G8MJ01/ok` (game 100% matched); natural (no MUST_MATCH) compile of eki_00.c returns 0. Index (tools/cleanup_index.py): 29.721 -> 29.710 per kLOC (21148 -> 21140 weighted); field_0x 2923 -> 2915.

### Work struct field names: eff_bush, eff_horao_spell (2026-10-20, Claude Haiku 5.5, n9-q-effn64c)
- `BushN64Data`: 0x10/0x14 are `velX`/`velY` (particle velocity added to x/y), 0x18/0x20 are `rotZ`/`rotY` (degrees, the Disp rotations), 0x1C is `swayPhase` (degrees, random-walked each frame, it drives the rotZ and rotY increments). The head's 0x24 is `alpha` (TEV colour alpha) and 0x2C is `age`. 0x30 is never read, so it stays `field_0x30`.
- `effHoraoSpellData`: 0x10 is `life` (Main counts it down; 1000 is endless), 0x14 is `age`. 0x18/0x1C/0x20 are `color1R/G/B`, 0x24 is `color1A` (TEVREG0 alpha, faded by Main), 0x28/0x2C/0x30 are `color2R/G/B`, 0x34 is `color2A` (TEVREG1 alpha) and 0x38 is `scale`.
- Left: the `.sbss2` zero GXColor templates in eff_horao_spell (`unk_80423150/54/58`), eff_thruhammer (`unk_80423298..804232A4`) and eff_kemuri11 (`unk_80423180..8042318C`) keep their address names, as with glass, opuku_jetw, kameki and stamp: each is a zero template copied then overwritten, and a zero alpha that passes through to GXSetTevColor proves no role beyond that. eff_kemuri11's `field_0x18` (written 0 at child spawn, never read).

Verified: tower `ninja` recreates `build/G8MJ01/ok`; natural (no MUST_MATCH) compiles of eff_bush_n64.c and eff_horao_spell_n64.c return 0. Index (tools/cleanup_index.py) against current main: 29.710 -> 29.577 per kLOC (21140 -> 21045 weighted); field_0x 2915 -> 2820.
### Cleanup index: pool constants split out of address_name, offset macro calls counted (2026-10-20, Claude Haiku 5.5, n9-q-idxtool)
- tools/cleanup_index.py: `address_name` (weight 1) now covers only address-named data, function, label and string symbols (dat_, vec3_, str_, lbl_, func_, L_, jumptable_). It no longer counts `unk_` names (already `unk_name`, which made 609 of them count twice) or value-named compiler pool constants.
- New `pool_constant` category, weight 0: `float_*`, `double_*`, `double_to_int_*`, `zero_*` and `one/negone/half/two_*` addresses. Shown in the table, not in the index.
- `raw_offset` (weight 2) also counts each call of a file-local offset macro (`#define NAME(p, off) (*(T*)((u8*)(p) + (off)))`, same shape as offset_to_field.py). The #define itself is not counted: 70 offset macros, 1123 calls, so the net is +1053 sites.
- Index on origin/main, old script vs new: 29.772 -> 23.405 per kLOC (21184 -> 16654 weighted over 711544 code lines). Step by step: pool split out 29.772 -> 21.300; unk_ no longer double counted 21.300 -> 20.446; offset macro calls 20.446 -> 23.405. tools/ci.py measures HEAD and base with the same script, so deltas from here on compare like with like; an unchanged tree now reports 0.000.
- Category counts, new script on origin/main: raw_offset 1970 (was 917), address_name 3443 (was 10079), field_0x 2959, must_match 2417, opt_pragma 1461, goto 485, unk_name 775, fn_ptr_cast 334, inline_asm 7, pool_constant 6027 (weight 0, not in the index).
- Tests: tools/tests/test_cleanup_index.py covers pool/unk separation, offset macro calls (including a continuation-line macro) and the duplicate-define rule. Macro detection agrees with offset_to_field.parse_macros on every tree file (70 macros, no mismatch).
- No C files changed, so no build verification was needed.

### Cleanup index review: offset macros with a fixed base or an index, reference points under both definitions (2026-10-20, Claude Opus 5.5, n9-q-idxtool)
- Review of the entry above. Pool and unk_ split checked against the tree: every float_/double_/zero_ name in source is an .sdata2/.rodata pool constant (double_to_int_ is the 0x4330000080000000 int-to-double constant); no one_/negone_/half_/two_ address name is in source today; dat_, vec3_, str_, lbl_, L_, jumptable_ and suffixed function names (kuribo_init_800C23B4) still count as address_name.
- Offset macro rule widened: a parameter in the offset, or in an index applied to the result, makes an offset macro, so fixed-base macros (NKF(off), M_F(off), WM_S32(off)), WW_F32X(w, idx, off) and evt_yuuranking's A14L(i)/A74F(i) now count per call instead of once at the #define. A macro whose parameter is only the base (ANIM_FRAME_TIME(f), WIN_BADGE_TEX(w)) is a named accessor and still counts once; a statement macro with a raw offset somewhere in its body (WIN_KIRINUKI_SCALE) is not an offset macro. On main: 88 offset macros (was 70), raw_offset 2024 (strict-shape rule 1970).
- Old script = origin/main's tools/cleanup_index.py; new = this branch. Index per kLOC (weighted):

| revision | old | new |
|---|---|---|
| aea79288 (100% matched, 2026-10-17) | 77.719 (54562) | 75.054 (52691) |
| 2d92902a (start of overnight 9, 2026-10-19) | 70.097 (49212) | 66.342 (46576) |
| 1e542f53 (origin/main) | 29.577 (21045) | 23.362 (16623) |

- New-definition counts. aea79288: field_0x 20942, raw_offset 9433, address_name 5137, must_match 2414, unk_name 2012, opt_pragma 1591, goto 491, fn_ptr_cast 363, inline_asm 7, pool_constant 7955. 2d92902a: field_0x 18317, raw_offset 7760, address_name 5141, must_match 2438, unk_name 1982, opt_pragma 1471, goto 491, fn_ptr_cast 352, inline_asm 7, pool_constant 7958. 1e542f53: raw_offset 2024, address_name 3443, field_0x 2820, must_match 2417, opt_pragma 1461, goto 485, unk_name 775, fn_ptr_cast 334, inline_asm 7, pool_constant 6027.
- Not covered by either definition: REL-local address names (dat_gor_0001a46c, vec3_..._00012340, float_..._0000xxxx; about 900 in rels/, besides unk_ ones that unk_name still counts) end in a section offset, not an 8xxxxxxx address, so address_name and pool_constant miss them.
- tools/ci.py runs the same script on HEAD and base, so its delta compares like with like. tools/tests passes (unittest discover).
### Readability: las_04 firebar (2026-10-20, Claude Haiku 5.5, n9-q-relfld)

FirebarEntry/FirebarDesc in las_04.h and las_04.c: `prevAngleDiff` (entry 0x10, last frame's compAngle, compared
against the new one for the sign crossing in rot and trans), `speed` (0x18 and desc 0x18: rot adds it to the angle
rate, trans adds it to position.x), `targetX` (0x28, the x that trans's state 1 compares against), `baseRadius`
(0x2C and desc 0x24: segment 0's distance from the pivot in rot), `pulsePhase` (0x40, degrees, advanced by 6 and
wrapped at 360, feeds the sin() that sets the disp pass's alpha and shadow volume), and desc `angleSpeed` (0x10).
Left as field_0x: desc 0x14 and entry 0x20 (desc 0x14 is copied to entry 0x20, which nothing reads), and desc 0x20,
which is the initial angle for type 0 and the target x for type 1.
### Readability: rsh_05 moamoa work (2026-10-20, Claude Haiku 5.5, n9-q-relfld)

MoamoaWork in rsh_05.c: 0x40 is `hammerFrames` (frames Mario has stayed in motion 0x12; the sweep fires past 10),
and 0x558 is `bounceCount` (bounces off the moamoa's head while Mario is airborne, capped at 2, reset when neither
0x10000 nor 0x20000 is set on Mario's flags).
Left as field_0x: MoamoaPose 0x14, 0x1C and 0x20, which carry different values per state. In a jump 0x14 is the
vertical velocity (stepped by 0x20 as gravity), in the breath push (state 0x30) 0x14/0x20 are the x/z acceleration and
0x1C the y acceleration, and in the wave states (0x33/0x34) 0x1C and 0x20 are the height and sideways amplitudes.
MoamoaWork 0x2 and 0x48 are never read. rsh_06.c keeps its own MoamoaPose copy (chain at 0x48, a different layout)
and is not touched here.
### Readability: gor_00 NanNpcEntry slots (2026-10-20, Claude Haiku 5.5, n9-q-relfld)

gor_00.c and gor_00.h: no renames. The 32 field_0x uses in gor_00.c are all NanNpcEntry 0x70, 0x78 and 0x7C, and each
slot has more than one role across the seqs: 0x70 is the step total of a movePos move and the
next state after a jump, walk or suifu's state 100 move; 0x78 is the movePos step, a jump's quadratic term, the
sway's right bound and suifu's speed (140-260); 0x7C is the movePos angle, a jump's linear term and the sway's left
bound. Per the rule these stay field_0x. The other header slots are already named or unread: 0x16 and 0x94 are never
read by gor, and 0x84 is not used by gor_00 (it lies in evt_nannpc's per-entry scratch, as noted above). The header's
0x68-0x6C (`state`, `timer`) and 0x74 (`index`) differ from evt_nannpc.c's jumpStartX/jumpStartY and field_0x74 by
design: gor's sequences use those slots for their own roles. muj_00.h, muj_10.h and muj_20.h carry their own copies.

### Readability: eff_kemuri1 and eff_pokopi_pcharge field_0x names (2026-10-20, Claude Haiku 5.5, n9-q-effn64d)
- eff_kemuri1_n64.c's effKemuri1Data (defined in the .c, not in include/eff/): field_0x18/1C are `speedJerk`/`speedSnap`, used only in types 0/1, where Main steps the chain speed += field_0x14; field_0x14 += speedJerk; speedJerk += speedSnap. field_0x14, field_0x28 and field_0x2C keep their offset names: each is an additive increment in types 0/1 (speed += 0x14; velYAccel += 0x28; 0x28 += 0x2C) and a per-frame multiplier in types 3-7 (speed *= 0x14, velYAccel *= 0x28, velY *= 0x2C), and no one name covers both. The struct comments list both roles.
- Left: field_0x38 (written per type in Entry, never read anywhere in the file).
- eff_pokopi_pcharge: include/eff/n64/eff_pokopi_pcharge_n64.h's effPokopiPchargeData: field_0x3C is `rot` (degrees, Disp rotates by it), field_0x40 is `rotSpeed` (degrees added to rot each frame), field_0x44 is `scaleMul` (root only, set to 1.0, multiplies scale for scaleXZ). No other file uses this struct. field_0x38 keeps its offset name: on a particle it is the x step Main adds each frame, on the root a fixed x offset (vx_data[a]) that Disp adds to every particle.
- Not renamed: the unk_ names in eff_starstone.c (unk_804232e0..ec) and eff_kiss.c (unk_80423310..31c) and eff_pokopi_pcharge.c (unk_80423208/0c/10). They are .sbss2 zeroed GXColor templates copied and then overwritten per draw (kiss's comment says so), so they keep their address names as the other .sbss2 templates do.
- Verified: tower `ninja` recreates build/G8MJ01/ok; natural and MUST_MATCH mwcc runs (-maxerrors 50) of both .c files print no errors (the only output is the post-compile transform_dep step, which runs on a -MMD file the by-hand command did not write). Index (tools/cleanup_index.py): 23.362 -> 23.325 per kLOC (16623 -> 16597 weighted); field_0x 2820 -> 2794. Review (Claude Opus 5.5) reverted the multi-role names (kemuri1 0x14/0x28/0x2C, pokopi 0x38) to offset names and renamed kemuri1 0x18/0x1C.
### Readability: peach.c pose strings (2026-10-20, Claude Haiku 5.5, n9-q-peachstr, split from n9-q-seqload)
- `peach.c` (93 -> 31 address-named occurrences): 30 pose strings renamed `peach_pose_<state>_<base>` from their contents (state = transformed, aftershower, invisible, uneasy, happy, angry, ordinary, carry; base = stay, walk, dash, talk), each used in one `PEACH_FLAG_*` branch of `peachChgPose`; Opus review checked every branch against the string. Left: the `P_Z_1` key and its per-state Z outputs (meaning of Z unproven), `P_U_1` and `P_N_1` in the sub-motion switch. `mario_kemuri_80130474` stays: symbols.txt has `mario_kemuri` as scope:local, and dtk globalises it as `mario_kemuri_80130474` in the split objects (peach.o and koopa.o reference that name), so the C name has to match.
- The seq_load colour and Vec names from the same branch are held (inferred names, not landed).
### Readability: MarioWork slit and jabara fields, mot_slit and mot_jabara offset casts (2026-10-20, Claude Haiku 5.5, n9-q-motslit)

`offset_to_field.py` converted nothing as the tree stood (every site was padding, a union or a type mismatch), so the names went in first and the tool then converted the plain-typed sites.

- `mario.h`, slit's trailing words: `slitState` (s16, 0x2D8, the state machine step; mot_slit's switch and motSlitCancel/marioChkSlitEnd compare it), `slitPhaseTimer` (s16, 0x2DA, set to 30 on the 0x64, 0x6E and 0x78 entries and counted down to the next state), `slitTimer` (s32, 0x2DC, counted down in states 0x2, 0x50-0x51, 0x5A-0x5C). The 0x268/0x26C pair is `slitFloorValid` (s32) and `slitFloorPos` (Vec): marioClearSlitFloor zeroes the flag, marioGetSlitFloorPos returns the Vec when it is nonzero, and state 0x5A sets both.
- Per-motion union members, with one name per role: `slitPushCount` (s32, 0x2A4 union, counts frames pressed into an attr 0x2000 hit), `slitPrevY` (f32, 0x2AC union with field_0x2AC, the y state 10 compares against), `slitTurnTimer` (s16, 0x2B4, set to 6 and counted down in state 0x15) and `slitStillCount` (s16, 0x2B6, neutral-stick frames in state 0x14) in an anonymous struct beside `peachStayTimer`/`jabaraAccel`, `slitFlags` (s16, 0x2B8, bit 0) and `slitTurnSide` (s16, 0x2BA, 1 or 2) beside the mot_jabara struct below.
- mot_jabara's struct at 0x2B8 (also an anonymous struct in the union with field_0x2B8): `jabaraTimer` (s16, the rising-swing countdown in swingMain), `jabaraState` (s8, 0x2BA, swingMain's switch) and `jabaraAnimTime` (u8, 0x2BB, the local time animPoseSetLocalTime gets from it).
- Converted: mot_slit.c all 128 non-MUST_MATCH casts (74 by the tool, the rest by hand with the same names); mot_jabara.c 0x2B8, 0x2BA and 0x2BB sites by hand (`mario->jabaraTimer`, `jabaraState`, `jabaraAnimTime`). Left raw in mot_jabara: the s8 0x2BB reads at 721/743/771 (their s8 type differs from the u8 member), the u8 0x2BA read at 901, the s32 stores to 0x2A0-0x2B8 at 463-469, 0x1F4 (void* vs s32), 0x41/0x42 (per-motion bytes with no proven name) and the 0x2B0/0x2B4 s32 zero stores.
- mot_slit.c left raw: MUST_MATCH branches (0x246, 0x253 read as u16/u8 in U_marioSlitContinueChk), none else.
- Verification: the matched build recreates build/G8MJ01/ok (two runs, after the header and after the jabara names); natural compiles of mot_slit.o and mot_jabara.o exit 0 without -DMUST_MATCH. Index (tools/cleanup_index.py) 23.362 -> 22.798 per kLOC (16623 -> 16222 weighted); raw_offset 2024 -> 1824; field_0x 2820 -> 2819.
### Readability: win_log LogWin fields for the raw WW_ offsets (2026-10-20, Claude Haiku 5.5, n9-q-winlog)
### Readability: win_log raw WW_ offsets onto LogWin fields (2026-10-20, Claude Haiku 5.5 + Claude Opus 5.5, n9-q-winlog)
- offset_to_field.py --list on win_log.c: 0 convertible. 104 of the 129 offset-macro uses sit under the `#ifdef MUST_MATCH` guard (the 1417-1491 block, both arms), and 25 (0xD6C-0xFB4 in winLogInit2) land on WindowWork's opaque `field_D68` band, so nothing was rewritten by the tool. The tool's verify pass therefore ran only its baseline build.
- The WW_S32/WW_F32 calls (130 with the winLogExit stone array) now access LogWin's existing offset-named fields (`e28`..`fb0`); no field was renamed. `work` sites in winLogInit2 and winLogMain2 go through `w`/`lw` (LogWin*), the MUST_MATCH second-alias sites keep `new_var` as `((LogWin*)new_var)->`. The WW_ macros are gone (WW_PTR was already unused). WindowWork is unchanged.
- Observed roles, recorded for a future name with outside evidence (not applied: no invented names). winLogMain sets targets on tab open/close, winLogMain2 eases `cur += (target - cur) / 6` (`/ 8` map and stones, `/ 4` stone angle), winLogDisp draws the current values:
  - map position e2c/e34 (passed to mapGX), targets e28/e30 (Y is 20 or -480 per tab; e28 is 0 everywhere).
  - star stones: position e68/e70, targets e64/e6c; alpha e78 (GXColor alpha), target e74 (255 open, 0 close); orbit angle e80, target e7c (360/7 steps).
  - card and badge boxes (winHakoGX at x + X, y + Y): e84/e8c and f80/f88, targets e88/e90 and f84/f8c (-130/120 open; 330/-250, or 320/-240 in the init, close).
  - selected item texture (0x95, 0x96) positions: ea8/eb0 and fa4/fac, targets eac/eb4 and fa8/fb0 (-200/60 open, -200/-300 close).
  - ea0/ea4 and f9c/fa0: eased toward page * 0xE8, never read by a draw call.
- Left raw, each for a stated reason: the `p = (u8*)work` row-phase loops (winLogMain, 6 sites: they step the LogRow array by 0x20 with a byte pointer, and rewriting them changes the loop shape), `LOG_TEXPAL`'s `*(u8**)((u8*)work + 0x28)` (a type-punned read of winTpl, used once in winLogDisp), the `((WinLogEntry*)((u8*)work + 0xD78))[w->sel]` index in winLogInit2 (indexed, left as it was), the `0xC4`/`0xC8` `idx * 5` reads in winLogDisp (the field_40 band that win_root's records share), and `ep`/`new_var` loop bases in winLogMain2. The `anim->clip` and `ad` casts are other structs.
- Not touched: the `x1028`..`x104C` LogWin names (no raw offset), and the card/badge icon arrays.

Verified: tower build recreates build/G8MJ01/ok (1231/1231 files matched); natural (no MUST_MATCH) compile of win_log.c with -maxerrors 50 returns 0. Index (tools/cleanup_index.py): win_log.c 292 -> 32 weighted (134.562 -> 14.753/kLOC); tree 23.362 -> 22.997 per kLOC (16623 -> 16363 weighted).
### Readability: battle_seq_command goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotobseq)
### Readability: battle_seq_command goto removal (2026-10-20, Claude Haiku 5.5 + Claude Opus 5.5, n9-q-gotobseq)

battle_seq_command.c: 14 of 37 `goto` removed, no names changed (37 -> 23 in the file).
- BattleCommandDisplay_ActMenuMain, BattleCommandDisplay_WeaponSelectMenuMain: `goto end` inside the
  `while (BattleMenuKeyOKInACT)` body became `break`; the `end:` label went (the loop's trailing `break` falls through
  to the same code).
- Left: WeaponMultiItemCancelCheckMain, OperationMenuMain, MultiItemMenuMain, ChangePartySelectMenuMain. Their gotos
  only go away inside a `do { ... } while (0)` wrapper (a goto in disguise, not kept); `while (key) { ...; break; }`
  changes the object (95.4-96.0% for the last three).
### Readability: mot_jabara goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotojabara)
- `src/motion/mot_jabara.c`: 19 gotos -> 0, no names changed. `swingMain`'s `goto end` is an early `return`. In `mot_jabara`, the jumps to `block_72`, `block_121` and `block_177` are `break`s out of the switch each one followed (the labels sat directly after those switches). `goto do_32` became a fallthrough into `case 0x32`. The `goto block_37` into the else branch of the `hit != NULL` test became a `break` when `var_r28 == 0`, with the body hoisted under `if (var_r28 != 0 || ...)`. Labels `end`, `block_37`, `block_72`, `block_121`, `block_177` and `do_32` are gone.
- Verified: tower `ninja` recreates build/G8MJ01/ok (MUST_MATCH build); natural compile without MUST_MATCH prints only the existing no-prototype warnings. Index (tools/cleanup_index.py): 23.183 -> 23.130 per kLOC (16496 -> 16458 weighted); mot_jabara 311 -> 273 weighted.

### Readability: win_item offset sweep (2026-10-20, Claude Haiku 5.5, n9-q-offitem)
- `winItemMailLine` returns `&((WindowWork*)w)->mailLines` (0x26C, u16) instead of `(u16*)((u8*)w + 0x26C)`. Build matches on the tower.
- Left raw, with reasons: the 17 `((s32*)((u8*)work + 0x214))[sel]` sites in winItemMain. `work->itemSel[sel]` breaks the match (tried by offset_to_field.py --apply; reverted). The `WW_F32` positions at 0x224-0x252 and 0x260-0x264, and the Luigi book state at 0x2C8-0x2D0, sit in the opaque `field_224`/`field_260`/`field_26E` bands. The mail index pairs at 0x26E-0x26F (`b[0x26F]`, `p[0x26E]`) walk a computed base. `WW_S32(work, 0x2C)` is `mailTpl` (FileEntry*) and would need a type change. `item_disp`'s `entry + 0x2E8` and `*(f32*)(work + itemTab*4 + 0x224)` are computed pointer walks. win_root's `WW_F32((u8*)w + idx*0x14, 0xC4/0xC8)` is not a fixed base.
- Index: win_item.c 312 -> 310 weighted (282.609 -> 280.797/kLOC); tree 22.254 -> 22.251 per kLOC.
- Verify: tower `ninja` recreates build/G8MJ01/ok (MATCH).
### Readability: party_kuribo goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotokuribo)
- `src/party/party_kuribo.c` (28 -> 0 gotos): all 28 were in `pre_kurio_use`. The three `goto found` hit checks (hitobjPush, hitobjFront, hitobjStandOn) are nested null/attr-flag tests falling through to the single `obj` lookup; the 25 `goto done` branches of the animName `strcmp` chain are an `if / else if` chain ending in the `msg_kuri_map` default, followed by the unchanged `if (msg != NULL)` poke block. No names changed. Build stays MATCH on the tower; natural and MUST_MATCH mwcc compiles of the file are clean.
- Repo-wide `goto` category: 485 -> 457 (`tools/cleanup_index.py`, 23.183 -> 23.106 per kLOC).

### Cleanup guide from Melee practice (2026-10-20, Isaac; written up by Claude Opus 5.5, n9-q-guide)
- New `docs/CLEANUP_GUIDE.md`: the rules cleanup workers and reviewers follow, each tagged as Melee practice, our
  precedent or both: what to fix first, naming, placeholders, types and tables, casts, match hacks, inlines and
  macros, a don't list, a review checklist and a Melee-vs-us table. Sources: `~/melee-cleanup` pulled to f0cdbb0a
  (2026-10-16), sparse checkout widened to docs/, tools/, .github/ and samples of src/melee/{lb,gr,mn,it,ft}; its
  CONTRIBUTING, docs, cleanup_index.py and template; the rendered index; recent cleanup PRs (#3620-#3677).
- Licence: Isaac relayed that Melee is CC0. Not confirmed at f0cdbb0a: no LICENSE/COPYING at the repo root, GitHub's
  licence field is null and `repos/doldecomp/melee/license` returns 404, and no CC0 text appears in the tree. Only
  bundled tools carry their own licences (dat-cli, flags, replace-symbols AGPL-3.0; m2ctx MIT; table-typer
  Unlicense). So the "study, don't copy" note under "Cleanup index" (parked 2026-10-02) still stands; the guide is in
  our own words and copies no Melee code. If Melee adds a CC0 file, adapting its index logic becomes an option.
- Melee's index at f0cdbb0a: 5,718 sites in 592 files. pad-stack 2075, struct-ptr-cast 1609, prim-ptr-cast 524,
  deref-cast 411, must-match 225 (data order 104, code 99, helper 15, asm 5, macro 2), cast+offset 215, ptr-to-int
  194, goto 171, cast+index 140, longjmp 75, pragma 42, raw-offset 35, abs-address 2, m2c-field 0. No weights, no
  names counted. Melee's script run unmodified over our src/, include/ and rels/*/src at 7ee724b0: 20,871 sites
  (37,242 at b8ab226c): cast+offset 2193, cast+index 457, raw-offset 162, abs-address 28, struct-ptr-cast 2686,
  prim-ptr-cast 2793, deref-cast 3716, ptr-to-int 2360, must-match 2264, pragma 3726 (440 of them `#pragma once`,
  which Melee would also count), goto 486. Ours on the same tree: 23.362 per kLOC.
- Biggest differences: their index ignores names and ours is mostly names; our index doesn't see their retype family
  (about 11,500 casts here, only fn_ptr_cast counted); their field style is snake_case with `xNN` placeholders, ours
  camelCase with `field_0xNN` (keep ours: TTYD's own symbols are camelCase); their policy discourages AI naming of
  fields and globals, and the one AI-named set they kept (itflipper) is backed by a debugger-evidence doc and marked
  tentative, which is the standard our CONTEXT evidence lines aim at.
### Readability: eff_kemuri MUST_MATCH blocks (2026-10-20, Claude Haiku 5.5, n9-q-mmkemuri)
### Readability: eff_kemuri MUST_MATCH blocks (2026-10-20, Claude Haiku 5.5 + Claude Opus 5.5, n9-q-mmkemuri)

eff_kemuri.c: the `f32 k` temp in effKemuri2Main's alpha scale is gone (the plain `* float_0p8_8041c6f0` form matches). The "Kemuri\0" entry string and the force_active gap_8041c72c dummy stay: the code still matches without them, but CI's data ratchet drops eff_kemuri matched_data 100 -> 85.11, so both are needed for the data sections.

Kept, with the reason each failed: the Vec scratch copies around dispCalcZ in effKemuriMain and effKemuri2Main and the redundant effect->userdata re-read in effKemuri2Main (removing them, alone or together, changes the output); the opt_common_subs pragma pair around effKemuriDisp; the raw GXWGFifo writes, the one/left and unity temps in effKemuriDisp and the kemuriTevColor, KEMURI_SIXP4 and quad-temp shapes in effKemuri2Disp; the __declspec(section ".data") on touchdown_scale and heavy_scale; the unk_80423010/14 .sbss2 templates.
### Readability: mot_hammer2 goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotohammer)
- `src/motion/mot_hammer.c`: 14 gotos -> 0, no names changed. In `mot_hammer2`, the `level_low`/`level_1`/`level_0` chain became an `if / else if` ladder on `M_S8(mario, 0x41)` (>= 4, >= 3, >= 2, else), with each branch's trailing `goto common_6320` dropped. `case 0xA` now falls through the `common_6320` block into `case 0xB`, and the old `case 0xB: goto common_63C4` became that fallthrough label. Labels `level_low`, `level_1`, `level_0`, `common_6320`, `common_63C4` are gone.
- Verified: tower `ninja` recreates build/G8MJ01/ok (MUST_MATCH build); the natural compile without MUST_MATCH succeeds. Index (tools/cleanup_index.py): 22.162 -> 22.123 per kLOC (15769 -> 15741 weighted); mot_hammer 310 -> 282 weighted.
### Cleanup: party_nokonoko goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotonoko)

`nokonoko_use` had 65 gotos (the only ones in the file). 58 are gone, the build stays MATCH after each commit. No names changed.
- The three `switch (party->useMotionId)` blocks fall through to each other, so every goto from one switch to the start of the next one is a `break`: 21 from the first, 10 from the second, 16 from the third. The `L_800CC620`, `L_800CCD8C` and `L_800CD8A8` labels are gone with them.
- Early exits to the end of the function (1 from the prologue, 6 from the second switch, 4 from the third) are `return`, and the `L_800CD8BC` label is gone.
- Kept: 7 gotos in `case 20`, which jump between the `marioChkKey()` test and the finish block. Three structured forms (one combined `||` condition with a `break`, its negated `&&` twin, and an `if`/`else` chain that duplicates the finish code) all changed the object, so the gotos stay.
### Readability: party_yoshi gotos (2026-10-20, Claude Haiku 5.5, n9-q-gotoyoshi)

party_yoshi.c: 33 gotos -> 8, no names added. Matched build 32 files OK, and the natural (#else) build of the
file is obj_equiv-equivalent to the pre-branch natural object.
- searchGround: the hit-null skip and the L_2804/L_2968 tail are one `if (hit != NULL)` wrap plus an if/else on
  hit at the end; the three `goto end` became `return`. checkWall's `goto end` is a `return`. 8 gotos gone.
- yoshi_use: case 7 is one block (the two L_30B0 entries and the L_312C exits became breaks); the L_312C, L_3588,
  L_3754 and L_4188 switch-end jumps are breaks; case 0xD and the L_3354 section are an if/else on
  `mario->flags & 0x20`, with the field_34 == 8 exit as a wrap. Unused labels removed. 15 gotos gone.
- Left: yoshi_use's two `goto L_3588` (the grounded jump to 0x14 skips the rest of the section; a flat
  `GROUNDED && (EVT ? frm >= 0 : btn & 0x100)` and an else-if chain both changed codegen) and the four
  `goto L_3CE4` (skip the L_3C2C tail after searchGround2 or the airTimer exits; no shape without duplicating the
  tail or a flag). marioYoshiForceCancel and yoshi_exit keep `goto not_toge` / `goto exit_not_toge`: duplicating the
  field_34 block into the toge else changed the object size (+0x2C), and a flag was not added.

### Readability: MotionPlaneData typed bands (2026-10-20, Claude Haiku 5.5, n9-q-typeplane)

- include/mot/mot_plane.h: the `u8 field_0x10[16]` band is now `f32 field_0x10`, `field_0x14`, `field_0x18`, `field_0x1C` (each read and written as f32 in mot_plane.c), the `u8 field_0x24[4]` band is `f32 field_0x24`, and the 0x2C..0x2F bytes stay `u8 field_0x2C[4]` (no reads). `field_0x30` and `field_0x3C` are `Vec` (both passed whole as `&DATA->field_0x30` to PSVECSubtract and marioHitCheckVec, and their components read as `.x/.y/.z` where the code reads f32s at 0x30..0x44). The header includes `<dolphin/mtx.h>` for `Vec`. Size stays 0x48; no names were added or renamed (flags, pitch, heading, bank, stallTimer, sfxId unchanged).
- src/motion/mot_plane.c: every MF32/MFVEC site on MotionPlaneData is a plain field access (`DATA->field_0x14`, `d->field_0x30.z`, `&DATA->field_0x3C`). The three `mario->motStruct` sites use `((MotionPlaneData*)mario->motStruct)->field_0x10`/`field_0x14`. The MFVEC macro is gone. MF32 stays only for `MF32(mario, 0x18C)` (MarioWork.unk18C, left alone).
- Build: tower `ninja` recreates build/G8MJ01/ok (1231/1231 files matched); mot_plane.c compiles without -DMUST_MATCH (mwcc rc 0).
### Readability: evt_yuunpc goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotoyuunpc)
- `rels/yuu/src/event/evt_yuunpc.c` (`yuunpc_pose_minigame4_main`): 22 gotos -> 2. No names changed. The `!(flags & 2)` early jump to `pose_sub` became a `if (entry->flags & 2) { ... }` wrap (the body is re-indented; `git diff -w` shows the real change). The `n == 3 / n == 4` jumps to `flags_switch2` became `if (n != 3 && n != 4)` around the height block. The `loop_test` jumps (case 1 and case 2 `r0v == -1`) are `break`s out of the state switch. The `cbec`/`cbec2`/`d670`/`d764` jumps in cases 1 and 2 became `if / else if` chains, with the `r28`/`r27` tails run after the chain. Labels `height_switch`, `region5`, `flags_switch2`, `pose_sub`, `loop_test`, `cbec`, `cbec2`, `d670`, `d764` are gone.
- Left: the two `goto bf30` in case 0 (`goto` into the middle of an `else` tail). Both the `||`-condition form and a reordered form of the merged test changed the object (rel MISMATCH), so the goto stays.
- Verified: tower `ninja` recreates build/G8MJ01/ok (MUST_MATCH build); the file has no `#ifdef` regions inside the touched function. Index (tools/cleanup_index.py): 22.162 -> 22.107 per kLOC (15769 -> 15729 weighted); goto category 452 -> 432.

### Cleanup: mobjdrv goto removal, partial (2026-10-20, Claude Haiku 5.5, n9-q-gotomobj)

`src/driver/mobjdrv.c`: 14 gotos -> 9, no names changed, build stays MATCH.
- Removed: the two `goto noShadow` in the shadow block of the entry loop, which became one `if (!(...) && !(...))` around the shadow code; and the three `goto apply` in the KPA (`else`) trigger branch (KPA_LV3, KPA_FLAME, 0x80000), which became an `else if` tail. The KPA_STAND_ON test was already last, so it is the final `else if`.
- Kept: the nine `goto apply` in the non-KPA trigger chain (HAMMER, HEAD, STAND_ON, HIP_DROP, the 0x40000 front test, and the PARTY and BOBBERY blocks). Four forms all changed the object: a single `else if` ladder with the PARTY and BOBBERY tests duplicated; the same ladder with the PARTY/BOBBERY blocks as `v22 == 0`-guarded ifs; a fully flat `v22 == 0`-guarded sequence; and the first five as `else if` followed by guarded PARTY/BOBBERY blocks. The `apply:` label stays for those gotos.
### Readability: win_root WindowWork band as the WinRoot overlay (2026-10-20, Claude Haiku 5.5, n9-q-offroot)

win_root.c's `WW_S32/F32/PTR/U32(work, 0xNN)` reads (317 sites, all in the WindowWork bands win_main.h leaves opaque)
now go through the file's existing `WinRoot` overlay (`WROOT(work)`, the same base pointer). The overlay grows from
0x40-0x124 to cover every offset win_root.c reads, 0x128-0x1044 included, with members named only by offset
(`field_0x128`, `field_0xE94`, ...) and gaps as `pad_0xNN`. Each member's type is the one every win_root.c site reads:
F32 at 0x128/0x12C, 0x164-0x170, 0x244/0x24C/0x260, 0xEA0, 0xF9C; S32 elsewhere. The 0xE94-0x1044 members are the
win_log band (LogWin's cardCnt/badgeCnt/charCnt names live in win_log.c and are not shared), so they stay offset-named.
`WinRootRec` (the 0x14-byte records at 0xC0, names a..e unchanged) is retyped to match its reads: `a`, `d`, `e` are
S32 and `b`, `c` are F32, as every `WW_*` site on those offsets reads them. The winRootDisp record loop now holds
`WinRootRec* e = &WROOT(work)->recs[i]`, reads `e->a`/`e->d`, and its `base + 0xB8` read (the previous record's `d`,
guarded by `i > 0`) is `WROOT(work)->recs[i - 1].d`. Other 0xC0-0x123 reads are `recs[k].a..e` by offset.

Left raw: 0x13C/0x140/0x144 (14 sites). They are read or written as a pointer (PTR/const char*), as S32 (the `= 0` and
`= a3` writes) and as U32 (`!= 0` at 0x13C), so no single member type fits; the offsets are not given a member. The
`WW_S32/U32/PTR` defines stay for them; the unused `WW_F32` and `WW_U16` defines are gone.

Index (`tools/cleanup_index.py --top 40`): total 22.080 -> 21.575 per kLOC; raw_offset 1693 -> 1390 sites;
win_root.c 794 -> 435 weighted (298.4 -> 160.8 per kLOC). field_0x names rose 2754 -> 3001 (each offset-named member
counts as one); the net is still down. Matching build recreated at every batch (MATCH after each of 8 batches of 40
sites and the macro cleanup); win_root.c natural-compiles without MUST_MATCH.
### Readability: party.c raw offsets, field_34 (2026-10-20, Claude Haiku 5.5, n9-q-offparty)

party.c: 6 of 62 offset-macro uses became plain `party->field_34` (P8(party, 0x34) in partyDokanInit's switch, the
two switches, the `== 3` check and the three stores). field_34 is a plain u8, the same width and signedness. No names
added or changed.
- Left, 56 offset-macro uses plus 8 other raw casts: pointer fields read as u32 (0x12C, 0x130, 0x134, 0x138, 0x140);
  an offset inside a band with no field of its own (0x124, 0x13C, 0x144, 0x148, 0x15E, MarioWork 0x3E); a field of the
  other signedness (P16(party, 0x28), P32(party, 0x24), P8(party, 0x158..0x15B), PS8(party, 0x30)); MarioWork 0x2E,
  a MarioMotion enum read as u16.
- Left as raw, and not converted: the six `*(char**)((u8*)partyPoseTbl + id * 0x3c + N)` sites (N = 16/32/36) map to
  `partyPoseTbl[id][4/8/9]`, but the rewrite changed codegen (DOL checksum failed); the match needs the raw form.
- Index (tools/cleanup_index.py): party.c 182 -> 170 weighted (69.678 -> 65.084 per kLOC); tree 22.215 -> 22.198 per kLOC.

Verified: tower `ninja` recreates `build/G8MJ01/ok` (after the pose-table rewrite was reverted); natural compile of
party.c (no MUST_MATCH, -maxerrors 50) exits 0.

### Readability: msgAnalize gotos (2026-10-20, Claude Haiku 5.5, n9-q-gotomsg)

- src/driver/msgdrv.c msgAnalize: 10 gotos -> 1. The `loop:` label is now `for (;;)` with `continue` at each back-edge; `goto done` is `return;` in case 0 (the `done:` label went with it); the `aftertag` block is inlined at the end of `case '<'` (its only jump source); `defaultchar` is the code after the switch, reached from the former `default:`. Byte-identical: tower `ninja` recreates build/G8MJ01/ok (1231/1231 files matched).
- Left: `goto parsed` (the tag scanner's `'>'` exit from the `for (n = 0x80 ...)` loop, skipping the `after = NULL` fallthrough). The structured forms need a flag or a loop-condition hack on `after`, both ruled out.
### Readability: ac_gauge_timing, pmario_sound, itemdrv goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotoacg)
- `src/battle/ac/ac_gauge_timing.c` (10 -> 4 gotos): the six `goto tail` jumps are `break`s out of the `acParams[2]` switch, and the `tail:` label is gone. The four `gaugeCheck1`/`gaugeSkip1`/`gaugeCheck2`/`gaugeMod2` jumps stay inside the `MUST_MATCH` double-test blocks, which exist to force the branch layout. Build MATCH; natural compile without `-DMUST_MATCH` is clean.
- `src/driver/itemdrv.c` (8 -> 7): `itemseq_GetItem` case 5's `goto state5Tail` became an `else` around the pouch pick, and the `state5Tail:` label is gone. Left: `goto itemseq_setFlag` x3 and `goto state1` x2 in case 0. Both a single `||` condition and an `else if` form that merges the 0x2000 set changed the object (MISMATCH), so the gotos stay. The `goto bound_add_time` and `goto bounce_zero` in `itemseq_Bound` are the shared-tail forms the comment above that function documents, and stay.
- `src/pmario_sound.c` (8 -> 8, unchanged): `psndBGMOn_f_d` and `psndENVOn_f_d` both exit through a shared `done:` tail (`psndBGMMain(); return 1;` / `return 1;`). Replacing each `goto done` with an early return (duplicating the tail) changed the object, both separately and together, and the `no_channel` path does the same. The gotos stay.
### Readability: mot_hammer2 goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotohammer)
- `src/motion/mot_hammer.c`: 14 gotos -> 0, no names changed. In `mot_hammer2`, the `level_low`/`level_1`/`level_0` chain became an `if / else if` ladder on `M_S8(mario, 0x41)` (>= 4, >= 3, >= 2, else), with each branch's trailing `goto common_6320` dropped. `case 0xA` now falls through the `common_6320` block into `case 0xB`, and the old `case 0xB: goto common_63C4` became that fallthrough label. Labels `level_low`, `level_1`, `level_0`, `common_6320`, `common_63C4` are gone.
- Verified: tower `ninja` recreates build/G8MJ01/ok (MUST_MATCH build); the natural compile without MUST_MATCH succeeds. Index (tools/cleanup_index.py): 22.162 -> 22.123 per kLOC (15769 -> 15741 weighted); mot_hammer 310 -> 282 weighted.
### Readability: mot_hammer raw offsets to MarioWork fields (2026-10-20, Claude Haiku 5.5, n9-q-typehammer)
- `src/motion/mot_hammer.c`: 53 of 115 raw `M_F32`/`M_S8`/`M_S32` offset accesses became existing fields, no names added. f32 0x2A4 (5 sites) -> `field_0x2A4` and f32 0x2B0 (33 sites) -> `field_0x2B0`; both are the unnamed union members, so no motion's name is borrowed. s8 0x43 -> `dispHalfTurn`. Band splits: 0x3A (`s8 field_0x3A`, 10 sites) and 0x200 (`s32 field_0x200`, 4 sites), both bands read only by mot_hammer.c. The `M_F32` macro went (no uses left).
- `include/mario/mario.h`: `u8 field_0x3A[1]` -> `s8 field_0x3A`, `u8 field_0x200[4]` -> `s32 field_0x200`. Offsets and sizeof(MarioWork) unchanged.
- Left raw (62 sites): 0x41 and 0x42 (22): `mot_jabara.c` reads both bytes as `u8` (MW_U8) and as `s8` (MW_S8), so no single type. 0x2B4 s16 (23): the union member `slitTurnTimer` is mot_slit's name. 0x2A0 (s32 vs f32 union), 0x2A8 (s32 vs f32/s32 union, `motionSfxId1` is mot_hip's), 0x2B8 (s32 vs f32 union), 0x2A4 s16/u16 (3), 0x44 (u32 vs s32), 0x1FC (void* vs s32). 0x48 at line 257 stays raw inside MUST_MATCH: the comment there says the raw accessor blocks the clamp's if-conversion.
- Verified: tower `ninja` recreates build/G8MJ01/ok (MUST_MATCH build); the natural compile of mot_hammer.c without MUST_MATCH returns 0. Index (tools/cleanup_index.py): mot_hammer.c 282 -> 228 weighted; tree 22.123 -> 22.047 per kLOC (15741 -> 15687 weighted).

### Readability: battle_stage_object BattleWork raw offsets (2026-10-20, Claude Haiku 5.5, n9-q-typestage)

- include/battle/battle.h: the `u8 field_180DD[0x182B0 - 0x180DD]` band is split by offset. `s32 field_180E0`, `field_180E4`, `field_180E8`, `field_180EC`, `field_180F0` (the layer and fog event ids; every access to them in the tree is s32), `s32 field_1829C` (the fog counter, s32 in every user), and the rest stays `u8` bands: `field_180F4[0x1829C - 0x180F4]` (void*/u8*/u32* slots) and `field_182A0[0x10]` (EffectEntry* in the fog code, s32 in the other user). Size and every offset are unchanged.
- src/battle/battle_stage_object.c: the BSO_WP_S8/S32/PTR macros are gone (the layer-event stores in BattleStageObjectInit are `wp->field_180E4`..`field_180F0`). BattleStage_DestroyA1/A2/B and the two fog functions read the named fields; DestroyA1/B drop their `u8* base` alias.
- Build: tower ninja recreates build/G8MJ01/ok; natural compile of battle_stage_object.c returns 0.
- Left, and why: the object is byte-identical only when these sites keep the `base`/`w` pointer (`w = wp + 0x20000`, the expression the original compiled from). Converting them to `wp->field_X` moves the MUST_MATCH object by +0x44 (battle_stage_object drops to 76.5% matched). Each was tested alone against the HEAD object: BattleStageObjectInit's `base - 0x7D76`/`0x7D64` stores (0x1828A, 0x1829C), _set_mobj_shake, BattleStage_WallCloseCheck, BattleStage_IronFrameFallCheck, _fall_frame_wait, _fall_frame_delete, BattleStage_NozzleWorkCheck, _nozzle_type_init, _nozzle_work_wait, _nozzle_work_attack_set, BattleStage_ObjectFallCheck and _object_fall_wait all change the object. Also left raw: 0x182A0 (conflicting types), 0x180F4/0x180F8 (pointers of several kinds), 0x180C4/0x180D0 (Vec inside field_180C0, which battle_event_cmd and the rels index as `field_180C0[0x1C]`, so the band cannot be split without touching those files), and the computed `base[idx + 0x1C4]`/`0x1C7` arrays and `w[index - 0x7D7C]`.
- Other files still on the raw offsets that now have named fields: battle_seq.c (`p - 0x7d68`, `0x7d58`, `0x7d5c`, `0x7d54`, `0x7f20`, `0x182A4`), battle_audience_kinopio.c (`0x18298`). Not changed here.
- Index (`tools/cleanup_index.py`): this file's raw_offset 21 -> 10, weighted 106 -> 84, 37.803 -> 30.011 per kLOC; whole tree 21.639 -> 21.608 per kLOC.
### Readability: eff_pokopi_thunder_n64 raw offsets (2026-10-20, Claude Haiku 5.5, n9-q-typepokopi)
- `include/eff/n64/eff_pokopi_thunder_n64.h`: EffPokopiThunderData's `pad_0x10[0x18]`, `pad_0x4C[0x4]` and `pad_0x110[0x8]` split into offset-named members: `field_0x10`..`field_0x24` (six f32), `field_0x4C` (f32), `field_0x110` (s32), `field_0x114` (f32). Offsets and sizeof unchanged. Types proven by every access in `src/effect/n64/eff_pokopi_thunder_n64.c`: 0x10-0x24, 0x4C and 0x114 are written and read as f32; 0x110 is written from `rand() % 360` and read as an s32 angle index. The other users (`rels/las/src/las_29.c`, `rels/las/src/unit/unit_boss_black_peach.c`) touch only the named tev fields.
- `src/effect/n64/eff_pokopi_thunder_n64.c`: all 26 `((u8*)data + 0xNN)` casts became `data->field_0xNN`.
- Left raw: the five `(u8*)yuragi_data_table[...] + t1 * 0x18` / `+ 0xF0` sites in effPokopiThunderDisp. They are stride-0x18 offsets into the `u8 yuragi_data[0x108]` and `yuragi_data2` tables; typing them would change data definitions, which this pass does not touch.
- Verified: tower `ninja` recreates build/G8MJ01/ok; the file compiles by hand with and without `-DMUST_MATCH` (`-maxerrors 50`) with no output. Index (tools/cleanup_index.py): 22.042 -> 22.018 per kLOC (15682 -> 15665 weighted); this file's raw_offset 26 -> 0, weighted 63 -> 37.

### Cleanup: battle_audience goto removal in BattleAudience_CheckReactionPerPhase (2026-10-20, Claude Haiku 5.5, q-gotoaud)

The six `goto scan4/scan9/scan12` jumps are gone. In cases 4, 9 and 12 the scan loop moved into the inner switch as the
last case, and the `case 0` block falls through into it after `phaseStep = 5`; the old `scan` labels sat between two
`break`s and were only reached by goto. The `default` arm is unchanged. Build stays byte-identical (`build/G8MJ01/ok`).
Index for the file 70.48 -> 68.15 per kLOC (345 -> 333 weighted); gotos in the file 12 -> 6.

Kept in `BattleAudienceItemCtrlProcess`: `goto deactivate` (two, at the top of the loop), `goto remove` (three: the
empty item table, a null target, the timer-zero case 6) and the MUST_MATCH `goto case4_body`. The `deactivate` and
`remove` tails are shared by five jump sites; a structured form needs either the four-statement tail duplicated or a
flag local, and the flag is a new name. The MUST_MATCH goto is the case-3 hit-to-case-4 jump the comment above the
function already calls load-bearing.
### Readability: evt_yuuranking goto removal (2026-10-20, Claude Haiku 5.5, n9-q-gotoyrank)
- `rels/yuu/src/event/evt_yuuranking.c` and `rels/gor/src/event/evt_yuuranking.c`: 9 gotos each -> 0, same change in both. No names changed.
- `evt_yuuranking_disp_func`: the `displayMode != 0` jump to `rank` became `if (work->displayMode == 0) { ... } else { ... }` around the mode switch, the end block and the rank block (the bodies are re-indented; `git diff -w` shows the real change). The four `goto end` became `break` at case level (the switch is followed by the end block). The `goto finish` and `finish:` tail were dropped with the else.
- `YUU_RANK_SETUP` / its gor twin: the three `goto done` became `break` (two inside the slot-search `for`) and the trailing one in the `else` branch was dropped (it was the last statement before `done:`). The `done:` label is gone.
- Verified: tower `ninja` recreates build/G8MJ01/ok (MUST_MATCH build); both files compile without `-DMUST_MATCH` (mwcc rc 0).
- Index (tools/cleanup_index.py): 21.584 -> 21.534 per kLOC (15355 -> 15319 weighted); goto category 307 -> 289.
### Readability: goto cleanup in gor_03, mot_slit, evt_uranaisi (2026-10-20, Claude Haiku 5.5, n9-q-gotomisc)

gor_03.c: the sel loop in goryuugijo_main_slot is a `while (k < 2)` loop. Its two `goto sel_inc` become `break`s out of the
switch, so the `k++` runs at the loop end, and the `sel_body`/`sel_inc`/`sel_test` labels are gone. The `for` form still
emits `mtctr`/`bdnz` (see the comment above the function), so `while` keeps the original `li r9,0; b test` shape.
Matching build recreated; natural compile without MUST_MATCH exits 0. gor_03 code gotos 6 -> 2.

Left in gor_03: slot_rotate_sub's two `goto done`. Writing the rotation tail once and folding it into the `wait == 0`
branch and the outer else (7 lines duplicated) breaks the match; the skip path has to jump over the rotation.

Left in mot_slit.c (7 gotos, unchanged): the MUST_MATCH `goto zero`/`goto done` in U_marioSlitContinueChk and
slitContinueChk. A single default `state = 0` with nested ifs did not match, with named fields or with MW_U16/MW_U8.
`goto keyOffPath` jumps into the else of the marioChkKey() test and would need the 60-line block duplicated, or a second
marioChkKey() call; not done.

Left in evt_uranaisi.c (7 gotos, unchanged): the `goto next` in the loop of the function that reads the story tags (3),
which skip `count++` and jump to `storyId++`. Nested `if` forms with and without `&&` did not match. The MUST_MATCH
`goto gswf_done` (both table functions) did not match with `break` plus `if (name == NULL) gswf = -1;`, or with a
pre-loop `gswf = -1`.

Index (`tools/cleanup_index.py`): goto 302 -> 298 sites, weighted 604 -> 596.
### Readability: battle_seq.c act-selection gotos (2026-10-20, Claude Haiku 5.5, n9-q-gotonpc)

battle_seq.c case 0x6000006 (the attack-event choice): the eight `goto act_done` became one if / else-if chain whose
final `else` is the default `evtEntry(attackEventCode)`. The checks run in the same order, and `irand` still runs only
when its status check passes. No names changed. battle_seq.c now has no gotos.

Verified: tower `ninja` recreates `build/G8MJ01/ok`; natural compile of battle_seq.c (no MUST_MATCH, -maxerrors 50)
exits 0.

Left in npcdrv.c (8 gotos). Each form below was built and the DOL checksum failed, so the match needs the goto form:
- fbatBattleMode first-strike `afterStrike` (2 gotos): one `&&`/`||` condition, and a repeated-flag else-if chain. Both
  changed codegen.
- fbatBattleMode coin-bonus `after` (1 goto): a `mult != 0` guard around the three bonus `if`s. Changed codegen.
- npcMain MOVE/RETURN `found` (2 gotos): folded into one else-if on the distance and angle test. Changed codegen.
- npcMain `interrupt` (3 gotos): jumps out of the blow, territory, find and move stages to `npcCheckInterrupt`. Each
  stage declares its own locals and sets its own flag, so a structured form needs nested else blocks that repeat the
  later stages or a flag variable.

### Readability: win_item WW_ offsets onto a file-local overlay (2026-10-20, Claude Haiku 5.5, n9-q-ovlitem)

`src/window/win_item.c`: 93 `WW_F32`/`WW_S32` sites moved from raw offsets to a file-local `WinItemWork` overlay over the same `WindowWork*` (accessed through `WINITEM(w)`, a cast macro). WindowWork and win_main.h are unchanged. Build stays MATCH; the natural compile (no `-DMUST_MATCH`) is clean.
- Added: `WinItemWork` with the offset-named members `field_0x224`..`field_0x250` (f32, the list and book slide positions), `field_0x260`, `field_0x264` (f32, the mail slide position) and `field_0x2C8`, `field_0x2CC`, `field_0x2D0` (s32, the Luigi book state). Every member is used only in win_item.c, at the type declared here. Gaps are `pad_0xNN`. No semantic names: the book and slide roles are the existing comments in WindowWork.
- Left raw: the 18 `((s32*)((u8*)work + 0x214))[sel]` sites (itemSel). `work->itemSel[sel]` was tried in its own batch and breaks the build (`build/G8MJ01/ok` not recreated); reverted. The `0x2C` sites (mailTpl, a `FileEntry*` read as s32, compared as u32 and cast to `FileEntry*`), so the types disagree and they stay under `WW_S32`. The `WW_F32((u8*)w + idx * 0x14, 0xC4/0xC8)` reads in winItemDisp (computed base), the `item_disp` float walk `(u8*)work + itemTab*4 + 0x224`, the `(u8*)work` byte walks over the mail pairs at 0x26E-0x26F, and the pointer walks in winItemMain are computed.
- Removed: the unused `WW_S16` and `WW_U16` macros.
- Index: win_item.c 310 -> 242 weighted (280.797 -> 215.111/kLOC); raw_offset tree-wide 1629 -> 1544; overall 21.570 -> 21.474 per kLOC.

### Readability: PartyEntry bands and party.c offset macros (2026-10-20, Claude Haiku 5.5, n9-q-typeparty)
- `include/party.h` `PartyEntry`: four opaque bands split into offset-named members, sizes unchanged. `field_0x124` is `f32` (all 11 accesses in party.c are PF32 reads and writes of the rotation step in the dokan and ride routines). `field_0x13C` is `void*`: cleared to NULL in the reset blocks, and party_yoshi.c stores `groundHit` into it through `P_P(0x13C)`, so the pointer store is the evidence. `field_0x144` and `field_0x148` are `u32`: only 4-byte zero stores in party.c, nothing reads them anywhere in the tree, so the type is the store width and nothing more. `field_0x15E` and `field_0x15F` are `u8`: only 1-byte zero stores (P8). `field_14C` stays a 4-byte `u8` band: no code touches 0x14C.
- party.c offset-macro uses that land on a named field are now field accesses: P16 0x28 (`field_28`), the reset blocks' P32 0x130/0x12C/0x138/0x134/0x140 as `frontHit`/`field_12C`/`groundHit`/`field_134`/`headHit` = NULL, P8 0x34 (`field_34`), P32 0x24 (`timer`), P8 0x158-0x15B (`stickX`/`stickY`/`substickX`/`substickY`, s8 fields written through u8 stores of the same bytes), and `partyVecAt` at 0xAC/0x94/0x88/0x7C (`moveDelta`, `targetPos`, `spinPivot`, `dispOffset`).
- Left raw, with reasons: `PS8(party, 0x30)` (`field_30` is u8 and this site reads it as s8); `PS8(player, 0x3E)` (MarioWork `field_0x3E[0]` is u8); the five `P16(mario, 0x2E)` and `*(u16*)(playerPtr + 0x2E)` reads (`currMotionId` is a 2-byte `MarioMotion` enum, so the expression type would change); `partyVecAt(party, 0x64)` (a Vec across tiltX, field_68 and spinAngle). The `partyPoseTbl` casts are not struct accesses.
- Not touched: party_yoshi.c `P_P(0x13C)` could become `party->field_0x13C` in that file.
- Index (tools/cleanup_index.py): src/party.c raw_offset 69 -> 14, weighted 182 -> 95 (36.426/kLOC); tree raw_offset 1693 -> 1638; tree 22.159 -> 22.045 per kLOC (15767 -> 15686 weighted).
- Verify: tower `ninja` recreates build/G8MJ01/ok (MATCH) after the header commit and after the party.c commit; natural compile of party.c without MUST_MATCH produces an object with no errors.

### Readability: function-pointer casts, battle/mario/evt (2026-10-20, Claude Haiku 5.5, n9-q-fnbattle)
- `src/battle/battle_menu_disp.c`: `FontDrawColor` is declared locally as `void FontDrawColor(GXColor color)`, matching `fontmgr.c`. The MUST_MATCH cast and the `&color` / `&c` calls became `FontDrawColor(color)`; the MUST_MATCH/#else split collapsed. `fn_ptr_cast` 8 -> 7.
- Left, with the reason each one stays:
  - `battle_seq_command.c` (11): six `BattleMenuDisp_*_Main` calls pass `wp` through a `void (*)(BattleWork*)` cast, but the definitions are `void f(void) {}` stubs, and those casts sit in MUST_MATCH blocks. `windowDispGX_Waku_col` (MUST_MATCH, float-first order), three `btlDispTexPlane` calls with an extra `s32` argument (the definition has 7 parameters), and `BtlUnit_GetData` returning a runtime `void*` are also argument-changing.
  - `battle_menu_disp.c` (7): `iconDispGx2` and `iconDispGxCol` pass a `u16` icon id where the definitions take `s16`/`s32`; `btlDispGXQuads2D` passes an `s32` alpha where the definition takes `u8`. Each cast changes an argument type, so each stays.
  - `mario_pouch.c` (8) and `evt_nannpc.c` (1 of 7): `qqsort` comparators. `comp_aiueo`/`comp_kind` take `s16*`, `nannpc_zcompare` takes `ExtSortEntry*`, and `qqsort` takes `int (*)(void*, void*)`. No single signature is shared, and the definitions are not to be changed.
  - `evt_nannpc.c` (6 more): `evtGetValue` returns `s32` that is converted to a function pointer (`a2`, `a3`, `subfunc0..2`, `callback`).
- Verified: tower `ninja` recreates build/G8MJ01/ok; `battle_menu_disp.c` compiles without `-DMUST_MATCH` (clang -fsyntax-only, 0 errors).

### Cleanup: function-pointer casts in win_main, party and PartyData (2026-10-20, Claude Haiku 5.5, q-fnwin)

`PartyData` (include/party.h) types `init_func`, `move_func`, `use_func` and `bye_func` as `void (*)(PartyEntry* party)`. Every function the table in src/data/party_data.c stores in those four fields has that prototype, so the `((void (*)(PartyEntry*))party_dp->...)(party)` casts in party.c are gone and `driveParty`'s `case 3` holds the use function in a `void (*fn)(PartyEntry*)` local. `post_func` and `exit_func` stay `void*`: their `(PartyFunc)` casts are unchanged. In `partyGoodbyeInit`/`partyGoodbyeMain` the now-redundant `(PartyFunc)` casts and their local typedefs went.

In win_main.c, the `POUCH_GETHP`/`POUCH_SETHP`/`POUCH_SETFP`/`POUCH_SETAP` macros are gone; their callers use `pouchGetHP`/`pouchSetHP`/`pouchSetFP`/`pouchSetAP` directly, because the real prototypes already match. `iconGetTexObj` is called without its cast (`GXTexObj*, u16` matches). The `POUCH_GETFP`/`POUCH_GETAP`/`POUCH_GETPARTYHP`/`POUCH_SETPARTYHP` casts stay: their callees return or take `s16`, and the casts widen them to `s32`, which the comment above them describes.

Left in these four files, with reasons:
- win_root.c (14): the `winMailLabelV`, `winFontSetLabelV`, `winFontSetV` and `winBadgeExit` casts sit under `MUST_MATCH` and change the call (by-value `Vec` against pointer parameters, or an extra argument). The ten `qqsort` comparator casts go from `int (*)(const void*, const void*)` to qqsort's `int (*)(void*, void*)`; every qqsort caller in the tree uses that cast, and removing it needs a change to the `qqsort` definition in src/system.c.
- win_item.c (11): the four `winFontSetV`/`winFontSetRV`/`winIconSetV`/`winFontSetPitchV` macros are `MUST_MATCH`-only by-value casts. The seven `pouch` casts widen `s16` returns and `PartyId, s16` arguments to `s32`.
- party.c (3): `pouchSetPartyColor` is called with `(s32, s32)` against `(PartyId, u16)`; `marioGetScreenPos` is called with `Vec*` against a by-value `Vec`; `animPoseSetPaperAnim` takes an extra `BOOL` argument.
- win_main.c (4): the `POUCH_GET*`/`POUCH_SETPARTYHP` widening casts above.

Verified: tower `ninja` recreates build/G8MJ01/ok; party.c, win_main.c and src/data/party_data.c compile by hand without `-DMUST_MATCH` (`-maxerrors 20`). Index (tools/cleanup_index.py) fn_ptr_cast: win_root 14 -> 14, win_item 11 -> 11, party 11 -> 3, win_main 9 -> 4; total 334 -> 321.
### Readability: marioMotTbl function-pointer casts (2026-10-20, Claude Haiku 5.5, n9-q-fnmotion)
- `src/mario/mario_motion.c`: the 54 `(void (*)(void))` casts in `marioMotTbl` are gone. Every function in the table is `void f(void)` at its definition or in a header, so the table's `void (*)(void)` fields take them directly. The 19 motion functions in other units were declared `extern s32 name[]`; they are now `void name(void);` prototypes (declarations only). No struct or header changed.
- `src/battle/battle_status_icon.c`: left at 13 casts. All sit under `#ifdef MUST_MATCH` with a natural `#else` branch. Two `btlDispTexPlane` casts add a trailing `s32` argument the prototype does not have. The `BtlUnit_CheckStatus` and `BtlUnit_GetStatus` casts pass `s8` where the prototype takes `StatusEffectType`, so the cast truncates `type`.
- Verified: tower `ninja` recreates build/G8MJ01/ok; the natural compile of mario_motion.c without MUST_MATCH returns 0. Index: mario_motion.c fn_ptr_cast 54 -> 0 (weighted 141 -> 33).

### Structure: motion prototypes in their owning headers (2026-10-20, Claude Opus 5.5, n9-q-fnmotion)
The 19 `void mot_*(void)` prototypes mario_motion.c declared locally for marioMotTbl now live in each defining unit's header (include/motion/mot_damage/dokan/hip/jabara/roll/ship/stay/walk.h, a new include/motion/mot_yoshi.h, and include/party/party_cloud.h for mot_cloud), and mario_motion.c includes them. tools/local_prototypes.py passes; build unchanged.
### Readability: MarioWork raw offsets to fields in koopa, koopa_motion, mot_stay, mot_jabara, mario_balloon (2026-10-20, Claude Haiku 5.5, n9-q-typemario)
- `include/mario/mario.h`: three bands split, every offset and sizeof(MarioWork) unchanged. `u8 unk18C[4]` -> `f32 field_0x18C` (every MarioWork access in the tree is f32: mario.c, mot_plane, koopa_motion, mot_ship, party_yoshi, itemdrv). `u8 field_0x56[6]` -> `u8 field_0x56[2]`, `s16 field_0x58`, `s16 field_0x5A` (the only users are s16 MWS16 sites in mot_stay.c). `u8 field_0x260[8]` -> `u32 field_0x260`, `u32 field_0x264` (only users are the u32 MW_U32 sites in mario_balloon.c). `mario.c` marioMoveMain: `*(f32*)wp->unk18C = f31` -> `wp->field_0x18C = f31`; the mot_plane.c accessor comment names the new member.
- Existing fields used: `mot_stay.c` 0x11C is `Vec field_0x11C` (copy of position, same type); koopa.c 0x1AC is `dispReqDir` (f32, in both the MUST_MATCH twin and the natural branch of the breath function); koopa.c 0x110 is `prevPosition` (4 `prevPos = &mp->prevPosition` sites); koopa.c 0x1F4 and 0x1E0 are `hitobjHead` and `hitobjFront` (void* writes of 0 or a void* hit); mot_jabara.c 0x1F4 is `hitobjHead` (9 sites: pointer compares, `hitGetAttr`/`jabaraGetHitDepth` arguments, and the `marioSearchHead[_jabara]` results, which return void*).
- koopa_motion.c 0x18C -> `field_0x18C`. mot_stay.c 0x58 and 0x5A -> `field_0x58`/`field_0x5A` (11 sites). mario_balloon.c 0x260/0x264 -> `field_0x260`/`field_0x264` (16 sites).
- Macros with no callers left are removed: mot_stay.c `MW`, `MWU16`, `MWS16`, `MWVEC`; mario_balloon.c `MW_U32`; mot_jabara.c `MW_U16`, `MW_S16`, `MW_U32`, `MW_VEC`; koopa_motion.c `MW_F32`, `MW_S32`.
- Left raw, and why:
  - 0x14 (koopa.c, 13 sites): read as s32 (MW_S32) in some sites and u32 (MW_U32) in others.
  - 0x2A0/0x2A2 (koopa_motion.c, mot_jabara.c): the spinSpeed/damageRootY union is f32 and these sites read u16 or s32.
  - 0x2A4, 0x2AC, 0x2B0, 0x2B4, 0x2B8 (mot_stay.c, mot_jabara.c): every union member at these offsets is f32 or another motion's name (peachStayTimer, motionSfxId0, paperAnimeTime, slitPushCount, slitPrevY, slitFlags); the accesses are s32 writes and counters.
  - 0x41/0x42 (mot_jabara.c, 13 sites): the same byte is read as u8 and as s8.
  - 0x2BA, 0x2BB (jabaraState s8, jabaraAnimTime u8: the sites read the other signedness), 0x24E/0x24F (wStickDir1/2 are s8, read as u8), 0x28 (poseTime is s16, read as u16): type conflicts.
  - battle_mario.c: no MarioWork access. Its 22 raw sites are on the event, unit and BattleWork (`wp`) pointers and were not touched.
- Verified: tower `ninja` recreates build/G8MJ01/ok (MUST_MATCH build). Natural compile (no -DMUST_MATCH) and MUST_MATCH compile by hand with `-maxerrors 50` return 0 for koopa.c, koopa_motion.c, mot_stay.c, mot_jabara.c, mario_balloon.c, mario.c and mot_plane.c. Index (`tools/cleanup_index.py --rev HEAD` vs working tree): koopa.c raw_offset 26 -> 13; koopa_motion.c 16 -> 15; mot_stay.c 24 -> 12; mot_jabara.c 34 -> 25; mario_balloon.c 16 -> 0; battle_mario.c 22 -> 22. Tree 20.872 -> 20.776 per kLOC (14849 -> 14781 weighted).
### Readability: battle_seq / battle_audience_kinopio BattleWork raw offsets (2026-10-20, Claude Haiku 5.5, n9-q-offbseq)
- `include/battle/battle.h`: the `field_180F4` and `field_182A0` bands are split by offset. `s32 field_18298` (0x18298) sits between the 0x180F4 band and `field_1829C`; `field_182A0[0x4]` stays a u8 band (0x182A0 is a void*/EffectEntry* slot in battle_stage_object.c's fog code), followed by `s32 field_182A4`, `field_182A8`, `field_182AC`. Size and every offset are unchanged. Every user in src/ and rels/ reads 0x18298, 0x182A4, 0x182A8 and 0x182AC as s32 (battle_stage_object.c `w - 0x7D68/0x7D5C/0x7D58/0x7D54`, battle_seq.c, battle_audience_kinopio.c).
- `src/battle/battle_audience_kinopio.c`: `*(s32*)((u8*)work + 0x18298)` became `work->field_18298`.
- `src/battle/battle_seq.c`: the case 0x2000009 pointer `(s32*)((u8*)wp + 0x182A4)` became `&wp->field_182A4`.
- Left raw: the five `p - 0x7D68/0x7D58/0x7F20/0x7D54/0x7D5C` accesses in BattleWaitAllActiveEvtEnd (0x18298, 0x182A8, 0x180E0, 0x182AC, 0x182A4). Converting them to `wp->field_X` changes the MUST_MATCH object, even with the `u8* p = wp + 0x20000` alias kept (the build breaks in every rel), so the function stays raw.
- Verified: tower `ninja` recreates build/G8MJ01/ok; battle_seq.c and battle_audience_kinopio.c compile by hand with and without `-DMUST_MATCH` (`-maxerror 50`).
- Index (tools/cleanup_index.py): battle_seq.c raw_offset 22 -> 21; battle_audience_kinopio.c raw_offset 6 -> 5; tree-wide 20.675 -> 20.670 per kLOC (14709 -> 14705 weighted).

### Structure: battle_mario.c event, BattleWork offsets (2026-10-20, Claude Haiku 5.5, n9-q-offbmario)

Raw offsets in battle_mario.c: 22 -> 11 (weighted 44 -> 22; index 5.415 -> 2.707). No header changes.

- EventEntry 0x88 and 0x8C in `_tatsumaki_effect` are `event->userdataEx[0]` and `event->userdataEx[1]` (s32, the
  union member already in evtmgr.h). `EffectEntry*` casts on 0x8C follow the same change.
- BattleWork 0x18FE1 (the `_majinai_powerup_check` byte store) is `wp->field_18FE1[0]`. The band's element 0 is
  u8 in every other user (battle_seq_end.c, battle_seq.c, battle_damage.c), so no split was needed.
  alias. battle_actrecord.c reads the same byte as `WP(0x24)`, also u8.

Left raw:
- EventEntry 0x90 and 0x94 (`field_0x90[2]`, u32) are read and written as s32 in `_tatsumaki_effect` and
  `psndSFXOff`. Signedness differs from the field; retyping `field_0x90` is a separate change.
- `*(u16*)((u8*)unit->mKindParams + 0x14)` in `mario_get_renzoku_count_max`: BattleUnitKind `field_0x14` is s16, read
  here as u16.
- `(u8*)effect->userdata + N` and the `EffectEntry*` userdata offsets: EffectEntry is outside this task's scope.
- BattleWork 0x16F24 in _record_renzoku_count stays raw: indexing field_16F1C by an offset difference is not clearer.

## strings: address-named string symbols renamed by content, four files (2026-10-20, Claude Haiku 5.5, n9-q-strteresa)

Renamed `str_XXXXXXXX` to `str_` plus the literal's text (non-identifier characters to `_`, repeats collapsed, trailing `_` dropped). The name is the literal, so it can't mislead. Each content was read from the definition in the .c, or from the original dol's .rodata for extern symbols.
- `rels/pik/src/unit/unit_purple_teresa.c`: 22 renamed (REL symbols, `--rel pik`). `str_00002628` is `str_btl_un_purple_teresa`; the 21 others are the SFX names (`str_SFX_ENM_TELESA_*`, `str_SFX_BTL_DAMAGE_*`), `str_c_teresa_p`, and the one-letter pose/attack ids (`str_N_1`, `str_Y_1`, ... `str_A_1`, `str_A_2`). Same-named symbols in dou, tou2 and mri keep their address names, as those are separate objects.
- `src/event/evt_kinopio.c`: 16 renamed (dol). Includes `str_802ECE50` (`str_BGM_FF_GO_SLEEPING1`), the NPC model and motion names (`str_c_jugem_c`, `str_bom_02`, ..., `str_PM_S_3A`), `str_mario`, `str_M_S_1`. Skipped `str_802ECE64` and `str_802ECE84` (Shift-JIS bytes, not ASCII).
- `src/effect/eff_recovery.c`: 13 renamed (dol, each a `#define` to its literal). `str_802F3558` is `str_SFX_HP_RECOVER_SHINE1`; `str_802F35E8` is `str_Recovery`.
- `src/battle/battle_event_cmd.c`: 11 renamed (dol externs, read from the dol's .rodata). `str_802EE048` is `str_SFX_SYSTEM_NICE_25PERCENT1`.
- Skipped: `str_000026BC` in pik (`"S_1"`): the name `str_S_1` already exists (`src/driver/mobjdrv.c`, dol symbol), so the rename would clash.
- Non-ASCII skipped: `str_802ECE64`, `str_802ECE84`. No other skips: all candidate names were unique tree-wide, and none was over 40 characters.
- Index: 20.675 -> 20.556 per kLOC (14709 -> 14624 weighted).
- Build: `build/G8MJ01/ok` recreated after each file group. The battle, eff and evt files also compile without `-DMUST_MATCH` (extern branch of the ACSE macros in battle).
### Readability: hard gotos revisited, evt_uranaisi / fsort / pmario_sound / mot_slit (2026-10-20, Claude Sonnet 5.5, n9-q-s-gotohard2)

- `src/event/evt_uranaisi.c` uranaisi_data_make_next (3 gotos -> 0): the story-tag loop is `for (storyId = 0;; storyId++)` and the three `goto next` are `continue`; the `next:` label and the trailing `storyId++` are gone. Object byte-identical (a nested-`if` rewrite that duplicates `count++` grows the function by 4 bytes, which is why the earlier try failed; the `continue` form keeps the single shared `count++`).
- Left in evt_uranaisi.c: the two MUST_MATCH `goto gswf_done`. In the asm `gswf` shares r4 with the table name pointer: the found path loads `table[i].value` into r4 inside the loop and branches past the `li r4,-1` that follows the loop exit. Tried, all mismatched: `for (;;)` with an `if (name == NULL) { gswf = -1; break; }` arm first (+4 bytes, top-tested loop); `break` out of the loop then `gswf = name != NULL ? value : -1` (+0xC).
- `src/system.c` fsort (6 gotos, unchanged): the asm has `b done; b lscan_test` after the scan loop, i.e. an unreachable loop-entry jump of the lscan `while` that sits right behind an unconditional branch. That is the signature of a label jumped into a loop from outside; a structured form has no way to enter the lscan loop except from the c<0 arm. The nested form (lscan loop inside the `else if (c < 0)` arm of the scan loop, `continue` for the swap path, `break` after the lscan loop) compiles, but puts the lscan code between the scan body and its test (block order differs). Not matched.
- `src/pmario_sound.c` psndBGMOn_f_d / psndENVOn_f_d (unchanged): duplicating the `psndBGMMain(); return 1;` tail at the three early exits and inlining `no_channel` grows the function by 0x28 bytes; MWCC does not merge the tails.
- `src/motion/mot_slit.c` (unchanged): U_marioSlitContinueChk. A flat chain `else if (!D && E) state = 1; else if (D && (E || F)) state = 2; else state = 0;` matches everything except the second test of D (the original tests dispFlags once; the chain re-tests it and keeps the first result in r3, 3 instructions differ). `state = 0` first plus empty arms puts `state` in a callee-saved register; nested ternaries get if-converted (subfc/addze). `goto keyOffPath` still needs the block twice or a cached `marioChkKey()` result (a new local).

- strrest: remaining `str_XXXXXXXX` string symbols renamed to content names through `tools/rename_symbol.py` (format-only strings as `str_fmt_<spec>`, e.g. `str_fmt_04d`, `str_fmt_fff`). 91 renames: src/event and headers 17 (evt_map, evt_sub, evt_win, evt_cam, include/evt/evt_mobj.h), src/motion, mario, party, window, data, nameent, parse, pmario_sound and headers 34, src/battle (ac, sac, headers) 23, src/effect 7 and src/effect/n64 5, rels/tou 1 (`str_00001FA0` -> `str_fmt_ss`, `--rel tou`). Build MATCH after each batch.
- strrest skipped: owned by another worker (peach, evt_mobj, battle_disp, mot_hammer, evt_shop, shadowdrv, battle_seq_end, battle_audience_kinopio; includes `str_toride`/`str_toride_`); already renamed on the strfune/strmisc/strteresa/peachstr branches (231 symbols); non-ASCII (`str_802FE1D8`, `str_802ECE64`, `str_802ECE84`, rels tou `str_00001F74`, REL-only las `str_00000668`/`str_00000674` whose data is not in the sources); duplicate names (`M_S_1`, `M_I_Y`, `A_1`, `MOBJ_HiddenHatenaBlock`, `%d`); name already exists (`str_MOBJ_Lv1Block` etc., `str_M_J_1A/5A/1C`, `str_gor_01`, `str_A_2`, `str_S_1`, `str_Z_1`); empty or no identifier text (`str_8041EDBC`, `str_80421980`, `"<<<"` pair); over 40 chars (`str_80419848`).
### Cleanup: content names for address-named strings in eight units (2026-10-20, Claude Haiku 5.5, q-strmisc)

Renamed 54 `str_XXXXXXXX` symbols to `str_` + the literal's text (non-identifier characters to `_`, repeats collapsed, trailing `_` dropped), with `tools/rename_symbol.py`; config/G8MJ01/symbols.txt updated with the same pairs. Per file, distinct symbols renamed: peach.c 11, evt_mobj.c 8, battle_disp.c 8, evt_shop.c 6, mot_hammer.c 6, shadowdrv.c 5, battle_seq_end.c 5, battle_audience_kinopio.c 5. Extern-only strings (battle_disp.c, evt_shop.c `str_802ECE38`, battle_seq_end.c `str_802FDDF0`) got their text from the built dol's .rodata.

Skipped, left address-named:
- `str_802C1B88` "actual depth from the light (16bit depth)" (shadowdrv.c): 41 characters, over the 40-character limit.
- `str_802C4138` "toride" and `str_802C4140` "toride_" (evt_mobj.c): both would become `str_toride`.

Printing `%s`/`%d`/`%2d` literals gave `str__s`, `str__d`, `str__2d` (leading `_` kept, only trailing `_` dropped).

Verified: tower build recreates build/G8MJ01/ok. `tools/cleanup_index.py` address_name 3319 -> 3193 (weighted 14503 -> 14377).
- Opus follow-up: format strings named str_fmt_s/str_fmt_d/str_fmt_2d; "toride"/"toride_" named str_toride/str_toride_ (trailing underscore kept so both get a name).
- After rebase: evt_shop's "mario" string keeps its address name (str_8041BC10); str_mario names the evt_kinopio copy.

## s-mmpmot: party_motion.c MUST_MATCH reduction

- `src/party/party_motion.c` went from 52 to 16 `MUST_MATCH` lines. Every block was tried with its `#else` alone; only the two `k = 0.05f` stagings (and the `f32 k` local) matched, so those became the plain `0.05f * (...)` form.
- The rest of the matching arms were small enough to read normally (an extra `ytmp`/`readId`/`w` local, `angle += 180.0f; angle = revise360(angle)`, `case 0:` ahead of `default`, `!party->speed`, the one-compound-test angle check, the `-3.0f` staging, the recomputed 1.4 product, the `Vec pos` copies of the named rodata vectors) and now stand alone without `#ifdef`. The jump targetPos stores keep `Vec pos = FV(...)` but one assignment per line.
- Left under `MUST_MATCH`: the data-layout pieces (PM_RODATA, the reverse-order `.sbss` dummy, the FB/FV macros), the `(f32*)(u32)(s64)(s32)&vA0.z` movePos argument (px/pz locals, `s32` casts, and swapped store order all change the object), the nested `f0 = ... (f1 = ... (f6 = ...))` travel-time expression and the `party->targetDist = (f6 = party->targetDist)` self-store in party_motion_jump (the plain form shifts the f5/f6 colouring), and the three `hitFlag` redundant-NULL re-tests (party_motion_damage, two in the hit-height probe loop; ternary, `&&` and single-expression rewrites all move branch targets).
- Verified: tower `ninja` recreates build/G8MJ01/ok; party_motion.c also compiles without `-DMUST_MATCH` (`-maxerrors 50`).

### Readability: gotos left by the first goto pass (2026-10-20, Claude Sonnet 5.5, n9-q-s-gotohard1)

Removed 7 gotos, build MATCH (tower `ninja` recreates `build/G8MJ01/ok`):
- win_main.c `winMain` case 0 (6 gotos: 5 `check_open`, 1 `open_menu`): the 40-line open body is no longer the `else` arm of the
  `flags & 0x1000` test. The `else` arm is just `openTabId = 0`; the non-flag arm does the system-level / menu-disable / fade
  gate, then picks `openTabId` with an else-if chain (each failed gate or unmatched button is a `break`), then the
  `keyGetButton(0) & 0x40` `break`; the open body follows both arms at the case's own indent. Identical object on the first try.
- npcdrv.c `npcMain` (1 of 3 `interrupt` gotos): the blow check is `if (blow) { interruptFlags |= 8; } else { territory,
  find and move stages }`.

Left, with what was built (each differs from retail):
- npcMain `interrupt` (2 gotos, territory and find stages). Territory stage: a nested `else` needs the later stages after
  both the hit and no-hit exits, so the stage has to be hoisted: `hit = FALSE;` before `if (territoryType != NOTHING &&
  ...)` then `if (hit) {|= 4} else if (state == FIND) {...} else if (MOVE || RETURN) {...}` gives 99.84 (one extra `li r5,0`
  ahead of the territory test; retail sets `hit` only inside it). Find stage: `if (hit) {|= 2} } else if (MOVE || RETURN)`
  (valid since the states are exclusive) is 99.97 and moves the move stage's `hit` from r25 to r26; swapping the local
  declaration order in that block, or dropping the find stage's own `BOOL hit` (99.94), did not fix it.
- npcMain `found` (2 gotos): not retried (the earlier folded else-if changed codegen; the dy/range checks that follow are
  reached from both the near and the in-angle paths, so a nested form duplicates them).
- fbatBattleMode `afterStrike` (2 gotos): retail shares one `dbg_battleskip == 2` tail between the first-strike-win
  failure, the badge checks failing, and the not-attempted path. `if (!NO_WIN) { if (A) TT else if (B) TT else if (dbg==2)
  TT else FF } else if (dbg==2) TT else FF` (duplicated tail) is 97.78: MWCC does not merge the two copies.
- fbatBattleMode `after` (1 goto): not retried beyond the earlier guard; the only goto-free shapes repeat the three bonus ifs
  in two arms or test `mult` again.
- battle_detect.c `_btlSamplingEnemy` ac4 keep chain (13 gotos): the "goto shape is load-bearing" comment holds. Retail
  tests `targetPropertyFlags & 1` once, then three separate `li r0,0; b end` exits (untattleable, no monosiri, no tattle tag),
  then the rest. Full else-if chain with three `(flags & 1) && X` arms: 99.31 (the flag is retested and reloaded after each
  call). `(flags & 1) && (A || B || C)` as the first arm and the rest as else-ifs: 99.44 (the three exits merge into one
  `keep = 0` block). `keep = 1; if (flags & 1) { if A keep = 0; else if B ...; }` then a guarded chain: 99.26
  (`li`/`cmpwi keep` added). The flag test must dominate three distinct exits and fall through to the rest, which needs a goto
  or a duplicated rest-of-chain.
- `dedup_keep_done` (1 goto): `if (noDup == 0) keep = 0; else keep = 1; } else keep = 1;` is 99.72 (two `keep = 1` copies,
  one extra `b`); `keep = 1;` before the test (99.52) and `noDup = 1;` hoisted above the flag test (99.04) change the
  registers and the order.

## evt_mobj: MUST_MATCH cleanup (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmobj)

`src/event/evt_mobj.c` MUST_MATCH lines 47 -> 26. No names added.
- `itemEntry` is now declared `(name, type, a3, a6, a7, a8, evtId, a5)` (header, itemdrv.c definition, all callers in src and `rels/las/src/las_04.c` reordered). The old order made the two `((ItemEntry* (*)(..., f32, f32, f32, s32, s32))itemEntry)` casts in evt_mobj (badgeblk, kururing_floor) necessary; both casts and badgeblk's two-step `y` sum are gone. Every other caller still matches.
- Kept as the only version, no `#ifdef`: `LVB_TEMPLATE` typed 16-byte copy (strcpy and memcpy both mismatch), the red/white switch `switch`, the koopa block `switch (kpaGetLevel())` with the empty `case 2`, `case 0x5f:` ahead of `default` in the pole state machine, `mobj_rideswitch_lightblue`'s switch-scope `Vec v` plus a `Vec w` temporary in case 2, and the `mobjSwitchAnimIndex` inline in `mobj_switch_float_blk`. Duplicate arms (two `animPoseSetAnim "S_1"`) and an empty block removed.
- Left (26 lines): `#pragma pool_data`, the `.data`/`.rodata` anchors (`negone_one_374`, `mobj_vec3_*_template` and their uses), the string padding in `evt_mobj_switch_blue`, the six invented string-pool functions (tornadoswitch_red, rideswitch_a/b, arrow, trap, bombrock), the `base + offset` accesses to `kpa_pole`/`kpa_pole_move`/`kpa_move_to_toride` and the `base` in `evt_mobj_koopa_pole`, and the two `jumptable` indexings (`jumptable[type]` mismatches: the explicit `type << 2` form changes the gp-load/slwi schedule).

- n9-q-s-mmkiss: src/effect/eff_kiss.c MUST_MATCH 44 -> 5 lines. Each block tested alone against the baseline object with tools/obj_equiv.py. None of the `#else` forms matched (negOne, the *0.03125f readers, the dispEntry Vec copy, helper-returned GXColor, the 0.5f locals, effKissDisp2Color, the shared-body switch), so the MUST_MATCH shape became the only version and its comments now describe what the shape does. One readable rewrite matched: the second loop's offset is declared per iteration as `f32 offset = 0.0f;` (replaces the `off2` local). Left under MUST_MATCH: the `.sdata2` SDATA2_CONST declspec (data layout) and the two goto/brace blocks in effKissDisp (the `switch` with a single shared body emits two fewer branches than retail, and the goto form is the only one that matches). Natural (no -DMUST_MATCH) compile of the file succeeds; full build recreates build/G8MJ01/ok.

## s-mmaud: battle_audience.c MUST_MATCH reduction

- `src/battle/battle_audience.c` went from 42 to 14 `MUST_MATCH` lines. Each block was tried with its `#else` alone against the baseline object (`tools/obj_equiv.py`).
- `#else` alone matched, so the plain form is the only version: `audience->itemThrowCount++` in BattleAudienceCtrlProcess, the four `memcpy(&audience->itemWeapon, weapon_data_*, 0xc0)` calls (the `tbl` base-pointer local went), the `posY`/`+0x10` pair in BattleAudienceWinCtrlProcess, and the unused `BA_ZERO` macro.
- The MUST_MATCH shape read fine and became the only version: the struct-assignment table copies (deltaMultiplier, typeWeights, haiti rate/offset, FallObject tbl1/tbl2, with their typedefs; `memcpy` changes the object), the `pouchRank` local read before the copies, `j + akk + offset`, the `total == 0` guard, the `BattleAudienceJoyAdd2` inline, and the `DispAudience_color`/`DispApSrc_color` inlines. The `INIT_EXIST_ARG` macro became a plain `(u32)index` cast (an unsigned local in its place does not match).
- Left under `MUST_MATCH`: the data-layout pieces (non-const `.rodata` `vec3_802f9744`/`vec3_802f9768`, the `vec3_802f972c` slot and `p_vec3_802f9744`, with their uses in the item vecA block and the ApSrc star scale); the three volatile Vec/frame copies in BattleAudienceApSrcEntry (non-volatile and reordered forms both shift the schedule); the `goto case4_body` pair in BattleAudienceItemCtrlProcess (the structured form is 1 instruction different); and the `((void (*)(Mtx, s16, u16))iconDispGx2)` cast (the header's s16 differs from the u16 the original reads).
- Verified: tower `ninja` recreates build/G8MJ01/ok; battle_audience.c compiles without `-DMUST_MATCH` (`-maxerrors 50`).

## s-mmshadow: shadowdrv.c MUST_MATCH reduction

- `src/driver/shadowdrv.c` went from 41 to 13 `MUST_MATCH` lines. Each block was tested alone against the baseline object with `tools/obj_equiv.py`, then all together with the full build.
- Went to one version with no `#ifdef`: the three identical by-value mat-colour helpers became one `shadowMatColor(GXColor)` used by all draw functions (shadowDisp, Polygon, Texture, the material passes, and the Projection mat colour); `shadowTevColor` stays as the only form; `shadowInit`'s `unsigned long long` buffer sizes; the hit-query `q` pointer local; the `f31`-before-`f2` declaration order; the nine-word camera-basis copy is now the plain `camPos = vec3_...` assignments (it matches as written); `GXSetProjection` takes `(w)->projection` directly, and `shadowDispProjMtx` (the `lookAt + 0x30` spelling) is the only form of `proj`.
- Left under `MUST_MATCH`: the Projection function's four outgoing-argument GXColor slots (the `SHADOW_MAT_COLOR`/`SHADOW_FOG_COLOR` function-pointer-cast macros and the locals; fog, battle and else-branch colours each change the stack layout when written plainly, alone or together, with or without the shared helper), `PROJ_ZERO` (the cast read that places the scheduling cut; plain, `0.0f` and a `zero` local all change the object), and the Texture draw's FIFO block (the `volatile` zero/one reads give the separate loads retail has; the plain `GXPosition3f32`/`GXTexCoord2f32` form is 0x28 bytes shorter) together with its `size`/`negSize` and `positive`/`negative` locals.
- Verified: tower `ninja` recreates build/G8MJ01/ok; shadowdrv.c also compiles without `-DMUST_MATCH` (`-maxerrors 50`).

- n9-q-s-mmbero: src/event/evt_bero.c MUST_MATCH 28 -> 8 lines. Each block tested alone against the baseline object with tools/obj_equiv.py. No `#else` form matched (the enterEvent/exitEvent ternaries, including a pointer-typed ternary, the declaration-order locals of the ray-cast function, the single hitY store pair, the direct BeroSX/BeroSY reads, the literal name tables, no empty case 2, no second `distance` store), so the MUST_MATCH shape became the only version and its comments now describe what the shape does. Left under MUST_MATCH: the `jumptable_802C4970` name-table backing store and the three `names[]` copies that read it (the object is data layout and the copies depend on it), the two `"p_dokan_x\0\0"`/`"p_dokan_y\0\0"` padded strings, and the `flush_pools` helper with its `defer_codegen` pragma (section and .bss ordering). Natural (no -DMUST_MATCH) compile succeeds; full build recreates build/G8MJ01/ok.

## s-mmwin: windowdrv.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmwin)

- `src/driver/windowdrv.c` went from 34 to 8 `MUST_MATCH` lines. Each block was first tried with its `#else` alone (none matched), then rewritten; each kept shape was checked with `tools/obj_equiv.py` against the baseline object, and the full tower `ninja` recreates `build/G8MJ01/ok`. The file also compiles without `-DMUST_MATCH`.
- Now unconditional: one shared `windowSetFog(GXColor)` by-value inline for all four `GXSetFog` calls (the kanban/system/waku copies were identical); the direct `GXSetTevColor(..., (GXColor){...})` literal in `windowDispGX_Kanban` (the `tevColor` local is gone); the `zero` local in `windowDispGX_System`; the `x4 = x3; x4 += 32.0f` pair; the `px/py/ph/pw/pa9/pa10` parameter copies in `_windowDispGX_Message` (the `#define` aliases and `#undef`s are gone); the three `{ GXTexObj* o = wakuTexObj; s32 i = N; if (dark) { i = M; } ... }` texture selects (`dark ? M : N`, an if/else with two loads, and an index-only local all change the object; the `o` pointer local is needed); `windowCheckID` as `WindowEntry* entry = &WinObjects[id]; return entry->flags & 2;` (the raw `(u8*)WinObjects + id * 0x48 + 2` cast is gone).
- Left under `MUST_MATCH`: the reverse-order `.sbss` `gap_` dummy with `force_active` (data layout); the FIFO interleave in `windowDispGX_System` (`GXWGFifo.f32 = 0.0f; y3 = ...; GXWGFifo.f32 = y3; y4 = ...; GXWGFifo.f32 = 0.0f;`; the plain `y3`/`y4` then `GXPosition3f32`, `GXPosition3f32(0.0f, y3 = ..., 0.0f)` and `32.0f + x3` forms all change the FPR colouring); and the `goto ret` / `ret:;` pair in `_windowDispGX_Message` (the two `return`s emit separate epilogues).
- strfune: string symbols renamed to their contents (`str_` + the text, non-identifier characters to `_`, repeats collapsed, trailing `_` dropped). 82 renames, all dol symbols, through `tools/rename_symbol.py`, which updated the sources and config/G8MJ01/symbols.txt. eff_funemizu.c 32 (`str_dou_02` .. `str_tik_20`, the sound/effect ids), win_mario.c 28 (`str_M_S_1`, `str_M_H_1`, `str_msg_menu_mario_lv` ..), evt_mario.c 22 (`str_DOKAN_TATE_DOWNIN` .., `str_M_J_1A` ..). Build MATCH.
- strfune skipped: `str_8041D580` ("S_1", win_mario.c). `str_S_1` already names the dol symbol at 0x8041A0FC (config/G8MJ01/symbols.txt, src/driver/mobjdrv.c), so the rename would clash.
- After rebase: str_nok_00, str_pik_04 (eff_funemizu) and str_M_S_1 (win_mario) clashed with same-text strings named by n9-q-strteresa; these copies keep their address names (str_802FE848, str_802FE868, str_8041D578).

## s-mmdeka: sac_deka.c MUST_MATCH reduction

- `src/battle/sac/sac_deka.c` went from 39 to 7 `MUST_MATCH` lines (the blocks that were only an `#ifdef` pair now stand alone). Each block was tried with its `#else` alone against the baseline object with `tools/obj_equiv.py`; the combined result is a byte-identical build (tower `ninja` recreates build/G8MJ01/ok) and the file also compiles without `-DMUST_MATCH`.
- `#else` alone matched, so the hack went: the `one/zero/two` constant locals, the `half` local in the ripple smoothing loop, the duplicated `btl_camera_shake_h`/`psndSFXOn` copy, the `row` byte-offset base and its `0xC1C/0xC28/0x1834` reads (back to `mesh->disp[0][j]` and `mesh->normal[0][j]`), the `py/hy` pair, the `y` temps at the beat bar and both note transforms, the `fr/d` locals, `f <= 30.0f`, the `c30/c20` hoists (and their dead declarations), and the `70.0f` literal.
- The MUST_MATCH shape reads fine, so it became the only version with its comment reworded: the `disp2D_color` by-value helper and its three call sites, the `hx = half * x` local, `left`/`right` recomputed at the plane transforms, the `ratio` temp in both note loops, function-scope `tm`/`tj` in create_timing, and `f = beatPeriod - 7.0f - 2.0f`.
- Left under `MUST_MATCH`: the two rodata `Vec` blocks (data layout: non-const objects in `.rodata` via `__declspec`), the `volatile GXColor color` in disp_2D (a plain local coalesces it with the helper's return slot), and the yuka_disp one-iteration outer `for (j...)` loop (without it MWCC emits the GX FIFO base after the induction-variable inits). Tried and rejected: `mid - hw - e8` / `mid + hw + e8` for the plane transforms, plain color assignment with the volatile kept, `#else` for the left/right/ratio sites, `x`-based `half * x * x` for starVel, tm/tj as loop-scope locals.

## s-mmfont: fontmgr.c / eff_fall.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmfont)

- `src/manager/fontmgr.c` MUST_MATCH lines 25 -> 3, `src/effect/eff_fall.c` 25 -> 3. No names added. Each block was tried with its `#else` alone against the baseline object (`tools/obj_equiv.py`).
- fontmgr: none of the `#else` forms matched (inlined block scan, `rounded` in the allocation argument, `int`/`s32` conversion pairs in `FontDrawPosEdge`, the `f32 x0` newline reset, function-scope icon scalars, by-value `GXColor` temp vs `FontDrawColor_()`, direct width read vs `FontDrawStringShake_Width`, `FontDrawMessage_Edge` inline), so the MUST_MATCH shape is now the only version and its comments say what the shape does. Left under MUST_MATCH: the three `(s8)arg1[0] == '/' && (u32)(s8)arg1[0] == 0x2f72` dead-test disjuncts (retail emits the compare).
- eff_fall: one readable form matched: effFallDispWater's `height = GXGetTexObjHeight(&texObj)` local stays (its `#else`). The rest (`rad`/`nf31`/`half` locals, `effFallCol8`/`effFallColA`/`effFallCol68` by-value colour helpers) became the only version. Left under MUST_MATCH: `FALL_SFX` (`(char*)(u32)` literal round trip) and the `(const char*)(s32)sfx` argument cast in the sound call, both casts through integers.
- Natural (no -DMUST_MATCH) compiles of both files succeed; full build recreates build/G8MJ01/ok.

### Readability: eff_kemuri MUST_MATCH blocks, second pass (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmkemuri)

eff_kemuri.c: 52 `MUST_MATCH` lines -> 33 (the `one`/`left` locals, `unity` temps and vertex writes of effKemuriDisp and the vertex writes, `KEMURI_SIXP4` and `x6`/`y4`/`px`/`py` of effKemuri2Disp, with their `#else` GXPosition3f32/GXTexCoord2f32 forms, the "Kemuri\0" entry string, the `opt_common_subs` pragma pair around effKemuriDisp, the two `__declspec(section ".data")` tables, the `.sbss2` unk_80423010/14 templates and the `gap_8041c72c` force_active dummy). No names added or changed; the build recreates build/G8MJ01/ok and the unit compiles without `-DMUST_MATCH`.
- Each block was tested alone with tools/obj_equiv.py against the baseline object. No `#else` arm matched on its own, and neither did GXPosition3f32/GXTexCoord2f32/GXParam1f32 helper forms (they load the arguments as temporaries, so the zero/one constants are loaded once instead of per store), nor `one` in place of the `unity` temp, nor a function-level `GXColor color`.
- So the matching shape is now the only version: the `position` copy before dispCalcZ and the re-read of `effect->userdata` (effKemuriMain/effKemuri2Main), `kemuriTevColor` and the `p8`/`p12`/`n12p8` locals. Their comments no longer say "MUST_MATCH" and describe what each shape does.
- The effKemuriDisp and effKemuri2Disp vertex-write blocks (and `unity`) stay under `MUST_MATCH` with the readable `#else` (raw FIFO writes do not read fine). The `opt_common_subs off` pragma stays under `MUST_MATCH` (its natural build just has CSE on); removing it changes the object.
- n9-q-s-mmfire: src/effect/eff_fire.c MUST_MATCH 29 -> 4 lines (4 blocks left), rels/mri/src/mri_puni.c 28 -> 4 lines (the opt_common_subs group left). Each block was tested alone against the baseline object with tools/obj_equiv.py (rel: the rel's mri_puni.o). Readable forms that matched and are now the only version: eff_fire `scale *= 0.03125f` (replaces `/ 32.0f`), the plain `position = vec3_802f8c10;` (both branches were identical), mri_puni `self->modeFunc(self)` (replaces the `void (*)(void)` cast). The remaining MUST_MATCH shapes became the only version with comments reworded to say what the shape does: non-const f32 locals (stretch/widen, Disp2/Disp3/SmokeDisp dimensions, fade), the uninitialised px/py/pz/sc assigned in pz, sc, px, py order, the effFireDisp2Color/Disp2KColor/Disp3KColor/SmokeColor GXColor-returning inlines, `/ 128.0f` in effFireSmokeDisp, the volatile zero-vector reads in effFireDisp3 (also tried a direct `v = vec3; v.x = ..` and a `Vec tmp = vec3` initialiser: both differ), and in mri_puni the redundant b[2]/b[4] re-masks, the `-(s32)(u32)v` abs, named zero for the fcmpu order, the `base` local, the split `||` flag, the nearAttractIdx reload, the Teki/Mikata search inlines, the `*(u16*)&prevMode` store, `actor == targets->hit` operand order, the `inc` locals and the `val` local. Left under MUST_MATCH: eff_fire's .rodata declspec objects vec3_802f8c10 and vec3_802f8c4c and the effFire_pool_u2f/effFire_pool_str and effFire_pool_smoke_speed pool helpers (data layout), and mri_puni's opt_common_subs pragma group in evt_puni_jump_position (the natural `elapsed` form differs). Natural (no -DMUST_MATCH) compiles of both files succeed; full build recreates build/G8MJ01/ok.
- n9-q-s-mmfire: src/effect/eff_fire.c MUST_MATCH 29 -> 9 lines (8 blocks left), rels/mri/src/mri_puni.c 28 -> 7 lines (the opt_common_subs group plus the abs cast and the two prevMode u16 stores left). Each block was tested alone against the baseline object with tools/obj_equiv.py (rel: the rel's mri_puni.o). Readable forms that matched and are now the only version: eff_fire `scale *= 0.03125f` (replaces `/ 32.0f`), the plain `position = vec3_802f8c10;` (both branches were identical), mri_puni `self->modeFunc(self)` (replaces the `void (*)(void)` cast). The remaining MUST_MATCH shapes became the only version with comments reworded to say what the shape does: non-const f32 locals (stretch/widen, Disp2/Disp3/SmokeDisp dimensions, fade), the uninitialised px/py/pz/sc assigned in pz, sc, px, py order, the effFireDisp2Color/Disp2KColor/Disp3KColor/SmokeColor GXColor-returning inlines, `/ 128.0f` in effFireSmokeDisp, the volatile zero-vector reads in effFireDisp3 (restored under #ifdef MUST_MATCH with the plain copy as #else; also tried a direct `v = vec3; v.x = ..` and a `Vec tmp = vec3` initialiser: both differ), and in mri_puni the redundant b[2]/b[4] re-masks, named zero for the fcmpu order, the `base` local, the split `||` flag, the nearAttractIdx reload, the Teki/Mikata search inlines, `actor == targets->hit` operand order, the `inc` locals and the `val` local. Left under MUST_MATCH (reinterpret and volatile tricks restored on review): eff_fire's four volatile zero-vector reads, eff_fire's .rodata declspec objects vec3_802f8c10 and vec3_802f8c4c and the effFire_pool_u2f/effFire_pool_str and effFire_pool_smoke_speed pool helpers (data layout), and mri_puni's opt_common_subs pragma group in evt_puni_jump_position (the natural `elapsed` form differs). Natural (no -DMUST_MATCH) compiles of both files succeed; full build recreates build/G8MJ01/ok.

## s-mmmisc: party_move.c and battle_event_cmd.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmisc)

- `MUST_MATCH` lines: src/party/party_move.c 21 -> 12, src/battle/battle_event_cmd.c 20 -> 11. Each block was tried alone with its `#else` against the baseline object with `tools/obj_equiv.py` (battle_event_cmd.c needs the per-file `-O4,s`), then the kept shapes together; the full tower `ninja` recreates `build/G8MJ01/ok` and both files compile without `-DMUST_MATCH`.
- Readable form matched and became the only version: `partyFlyMain`'s discarded `anotherPartyGetPtr(party->currentSlotId);` call (no `target =` store); `partySetDispDirToMario` with just the `viewYaw` local (no `sum` local); `btlevtcmd_GetPartyTechLv`'s pouch read as `PouchPartyData* d = &pouchGetPtr()->partyData[i]; value = d->techLevel;` (the `(u8*)pouchGetPtr() + i * 0xe` cast is gone); `btlevtcmd_DivePartsPosition` calls `BattleGetUnitPtr(_battleWorkPointer, id)` and the dead `btlId = id;` is gone.
- Match shape kept as the only version (comments reworded): the `zero` local in `partyMoveMain` (the `PARTY_MOVE_ZERO` macro is gone), the `readIdx` temp and the `marioFlags` temps in the party footmark/fly code, the `args[0]`, `args += 2`, `args[-1]` read order in the draw-lots command, and the `t` temps in `btlevtcmd_OnToken`/`OffToken` (`|=`, `&= ~` and the plain forms all change the object).
- Left under `MUST_MATCH` with the readable `#else`: the u32 word copies and u32 loop counters for footmarks (type-punned stores), the inlined `partyChgSpeedPose` body in `partyForceWalk`, the `goto false_ret` in `partyGetAppearPos2` (plain `return FALSE` changes the object), the two dead zero `Vec` pool templates (data layout); in battle_event_cmd.c the `ACSE_STR`/`ACSE_SYSSFX` string macros and the `"SFX_BTL_ACROBAT_MISS1"` literal in `psndSFXOn_3D` (string layout), the volatile reads (`GetRGB` args, the `mask` read, both `cur` y reads, `args[4]`), and the two floats-first `intpl_sub` casts.

## s-mmkemut: eff_kemutest.c and npcdrv.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmkemut)

- `src/effect/eff_kemutest.c` went from 22 to 6 `MUST_MATCH` lines and `src/driver/npcdrv.c` from 22 to 6 (`grep -c`). Each block was tried with its `#else` alone (none matched; the natural kemutest build differs only through the data blocks), then folded; each kept shape was checked with `tools/obj_equiv.py` against the baseline object and the full tower `ninja` recreates `build/G8MJ01/ok`. Both files still compile without `-DMUST_MATCH`.
- eff_kemutest.c, now unconditional: the scratch `Vec` copies (`v88/v94/va0`, `vbc`, `vc8/vd4/ve0`, `effKemuTestQuad vac`) indexed as `(&v.x)[i]` (the `(f32*)&v` casts are gone, same object); the `position` second copy of `disp` in `effKemuTestMain`; the `kemuTestColor` inline; the shadow `half`/`dsize_half`/`size` locals declared in a block right before `GXBegin` (the function-top declarations and the later assignment block are gone). The two comment-only `#ifdef` blocks (the sdata2 note and the empty tail) were deleted.
- npcdrv.c, now unconditional: `npcOffscreenSetup` / `npcOffscreenSetup2` as the only code in `npcDisp_*` and `npcDisp_offscreen_*` (the four 40-line `#else` copies are gone, and the then-unused `color` local of the two offscreen functions); the last `sqrtf` refinement as `x = x * (...); return x;`; the hand-inlined `npcKoopaWaterCheck` with the hoisted `fallSfx` local in `npcMain`; the `k` temporary in the first-strike banner; `radius = (...) / 2.0f` in `npcCheckHitMarioSide` (the one-line `npcHitSideMul` inline is gone).
- Tried and failed: merging `npcOffscreenSetup` into `npcOffscreenSetup2` (the stack slot of `color` changes); dropping the `// stale store` lines; `return x * (...)` for the sqrt tail; `radius = radius * 0.5f`, `*= 0.5f` and `0.5f * (...)` for the radius; dropping `dirptr = dirptr + 0` or moving the `- 1` into the pointer in the bom_dir case.
- Left under `MUST_MATCH`: eff_kemutest.c: the `.sdata`/`.sbss2` colour template and `gap_` dummy with `force_active` (data layout), the `"KemuTest\0\0\0"` string padding (data), the dead `spd = 0.0f` store, the `dirptr` no-op add. npcdrv.c: the reverse-order `.sbss` `gap_08_804186C4_sbss` dummy (data), the two `(const char*)(s64)(s32)name` strcmp casts (casts through integers), the two `ENC(&info, s32, 0) = 0x400000` stores (type-punned).

## s-mmplane: mot_plane.c and statuswindow.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmplane)

- `src/motion/mot_plane.c` went from 24 to 9 `MUST_MATCH` lines, `src/statuswindow.c` from 22 to 9. Each block was tried with its `#else` alone against the baseline object (`tools/obj_equiv.py`), then kept as one version where only the shape mattered; the full tower `ninja` recreates `build/G8MJ01/ok` and both files compile without `-DMUST_MATCH` (`-maxerrors 50`). No names added.
- mot_plane.c, now unconditional: the `inactive` local, the `value`/`lim`, `b`/`a`/`one`, `half`/`value`, `neg20`/`value` and `neg2`/`zero` local shapes (declaring them with initialisers changes the registers), `if (mario->baseSpeed)`; the `{ zero; unk138 = zero; unk140 = zero }` block went back to two plain stores (the `#else` matches).
- statuswindow.c, now unconditional: the close-if-open `switch` calls the existing `statusWinCheck()` (`switch ((wp->flags & 0x11) != 0)` before) with `STATUS_WIN_CLOSED | STATUS_WIN_CLOSING` / `~(STATUS_WIN_OPEN | STATUS_WIN_OPENING)` in place of `0xA` / `~5`, in `statusWinMain` (3x) and `valueUpdate`; the cos/lerp temporaries, the `float_0_8041c900 == openAmount` operand order, the `curCoin` local and `valueMoveParty` (making every `valueMoveUpdate` call take the target by pointer changes the object).
- Left under `MUST_MATCH` in statuswindow.c besides the casts below: the three negated tests as `!(statusWinCheck() ? 1 : 0)` (the `#else` `!statusWinCheck()` folds the boolean; `statusWinCheck() == 0` and `if (statusWinCheck())` change the size) and the `t = 1.0f; t = 1.0f - x` dead-store pair (the single store changes the object).
- Left under `MUST_MATCH`: mot_plane `currSubMotionId` `(s32)` casts, the `pragma opt_propagation off/reset` pair (removing it changes the pool order), the three volatile Vec/f32 store groups, and the `planePoolAngle`/`planePoolRadius` pool-order functions; statuswindow the `SW_ICON_COL`/`SW_NUM_GX` function-pointer-cast macros with their colour locals, and the two `((void (*)(Vec*, s16, f32, u16))iconDispGx)` casts.

## s-mmhit: mario_hit.c and evt_mario.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmhit)

- `src/mario/mario_hit.c` MUST_MATCH lines 22 -> 15, `src/event/evt_mario.c` 22 -> 6. Each block was tried with its `#else` alone against the baseline object with `tools/obj_equiv.py`, then the kept versions together with the full tower `ninja` (recreates build/G8MJ01/ok). Both files also compile without `-DMUST_MATCH`. No names added.
- mario_hit.c, now one version: the unused `Vec* php` local in `marioSearchUnder` is gone; the identical `#ifdef`/`#else` `v = vec3_802c58b0` in `marioChkFrontStep`; the hit/attr/name chain in `marioChkOverhead` is `if (hit == NULL) ok = 0; else if (!(attr)) ok = 0; else {...}` (the `&&` forms change the object); `dir`/`up` in `marioSearchHead_roll`/`_jabara` are `(Vec){ 0, 1, 0 }` literals (the named reads cost the schedule). `vec3_802c5760/576c/5814/5820` stay: the uninitialised statics are `.sbss` layout.
- mario_hit.c, left: `MEM_VEC` (section declspec), the uncalled `marioChkLandonRoll` and `marioChkFrontStep_old` (they exist to pool constants at their position), the `hit = hit == NULL ? NULL : hit` dead compares (three, in `marioSlitChkWallAround`), and the `marioSearchHead_roll` `goto check_hit2` / dead `&out2 != NULL` block (the plain form is different), plus the natural-build `#ifndef` vec definitions.
- evt_mario.c, now one version: `mario->scale = vecs[0]` and `party->field_70 = vecs[1]` / `vecs[2]` in `evt_mario_dokan_prepare` (the volatile copies and the named `vec3_...` forms are gone); the framerate product in `evt_mario_party_door_move` and `evt_mario_party_bero_move` is `s32 n = (s32)((f32)gp->framerate * (dist / speed));` before the speed division (the unused `tmp` locals went); `evt_koopa_hip_attack` case 2 reads `y = mario->jumpVelY` and reuses `y` (no `vy` local).
- evt_mario.c, left: the `STR_MUJ_01` padded string and the `EM_VEC`/`EM_STR` base-register accesses with `base` (rodata layout and string pooling; `vecs[N]` indices would lose the symbol names), the `f32 vec` home with the `(Vec*)&vec` / `(void*)searchUnder2` casts (type-punned store; the s32 header prototype), and the volatile flags/Vec copy in the party-switch function.

## s-mmanim: animdrv.c / msgdrv.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmanim)

- `src/driver/animdrv.c` MUST_MATCH lines 20 -> 6, `src/driver/msgdrv.c` 22 -> 5. No names added. Each block was tried with its `#else` alone against the baseline object (`tools/obj_equiv.py`); the full build recreates build/G8MJ01/ok and both files compile without `-DMUST_MATCH`.
- `#else` alone matched: animdrv's second bounds branch in animPaperPoseDispSub (`c = x; if (x < y) c = y;`, the `max2` local is gone); msgdrv's `msgHistory(smart, data->scrollTarget)` call in msgMain (the byte-offset append is gone).
- msgdrv: the local `FontDrawColor` prototype now takes `GXColor` by value (as battle_menu_disp.c already declares it, ABI-neutral), so the `msgDrawColor` by-value inline and all seven `#ifdef` pairs around it are gone.
- The MUST_MATCH shape became the only version (comments reworded): animdrv's `1u` in the texIdRemap index, the `wrapS = wrapFlags; wrapS &= 1;` split, the `(s8)` cast on the loaded `mpGroupVisibility` element (two sites), the `max1`/`btmp` locals, `c + x` addition order for paperRight (two branches), the negated `-(left - paperLeft)` / `-(top - paperTop)` texture translations; msgdrv's `mask`/`button` split in msgWindow_Main states 3 and 11, the function-scope `value` for states 9 and 10, the `selectWindowColor` inline.
- Left under MUST_MATCH: animdrv's `defer_codegen off` pool helper (data/pool order), the `return other != 0` goto pair (every structured form still gives the branchless `(x | -x) >> 31`), the two goto-based search helpers `animPoseSearchAnimPaper`/`animPoseSearchAnim` (goto-free rewrites with `else if`, early `return 0` and two sequential ifs all differ), the `(AnimationPose*)param` redundant cast in animPaperPoseDispSub; msgdrv's two `.sbss2`/`.sdata` colour declspec pairs and the `"global\0"` padded string (data layout), the `iconDispGxAlpha` call through a `u16`-iconId function-pointer cast, and the `windowEntry` call through an `s32(*)(s32)` cast.

## s-mmyuu: mot_hammer.c / yuu_03.c / evt_yuunpc.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmyuu)

- `MUST_MATCH` lines: `src/motion/mot_hammer.c` 19 -> 6, `rels/yuu/src/yuu_03.c` 19 -> 3, `rels/yuu/src/event/evt_yuunpc.c` 19 -> 0. No names added. Each block was tried with its `#else` alone against the baseline object (`tools/obj_equiv.py`); the full tower `ninja` recreates `build/G8MJ01/ok` and all three files also compile without `-DMUST_MATCH`.
- `#else` alone matched: mot_hammer's `c = cos(float_0_8041a9a8)`; yuu_03's removal of the `makeVecB` forward declaration; evt_yuunpc's `s32 q = i / 4;` declaration form (equivalent to the kept `q;` + later assignment, so the declaration-only form stays).
- The matching shape became the only version, comments reworded: mot_hammer's `clampHammerType` helper (the unused `minHammerTicks10` is gone), the `t` clamp temp, the `px__`/`py__`/`pz__`/`one__` locals at effUltraHammerEntry, the `f32 p` / `five` locals around `cos` and the wall-probe positions; yuu_03's `ryushiColor0/1` by-value colour helpers, the reciprocal-order `t` expression, `half`, `zSpacing`, `t0`-`t3`/`half`/`t1` allocation locals, `makeVecB` and the `cellOffset` form; evt_yuunpc's `yuunpc_pose_change_sub` inline copy (its guard now reads `entry->field_64 != a` like `yuunpc_pose_change`, same object), `yuunpc_pose_lookup`, the `angle` temp, `+ -560.0f` style constants and the `q` statement order. Making `yuunpc_pose_change` a wrapper of the helper changes its object, so the copy stays.
- Left under `MUST_MATCH`: mot_hammer's four `volatile` field_68/field_6C stores (raw volatile writes), the dead `hitGetAttr(hit) & 0x800005` compare kept alive by `hit = (HitEntry*)((u32)hit | 0)` (dead compare / int cast), and the `*(Vec*)&dir3 = *(Vec*)&dir3t` punned store; yuu_03's `yuu4_init_work` helper plus its call (raw `(f32*)((u8*)&entries[i] + 8)` stores; named-field versions with entry locals, direct `entries[i]`, `Vec*` and `f32*` forms each lose one instruction) and the `volatile f32 entryX` read.

## s-mmeff2: sac_zubastar.c / eki_00.c / pmario_sound.c / eff_release_n64.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmeff2)

- MUST_MATCH lines: `sac_zubastar.c` 17 -> 11, `rels/eki/src/eki_00.c` 17 -> 7, `src/pmario_sound.c` 16 -> 12, `src/effect/n64/eff_release_n64.c` 16 -> 7. No names added. Each block was tested alone (grouped where blocks pair up) with its `#else` against the baseline object using `tools/obj_equiv.py` (the rel's `eki_00.o` for eki_00); none of the `#else` forms matched except the `#ifndef` color temporaries in eff_release_n64, which went with the inline setters.
- The MUST_MATCH shape became the only version: zubastar's `sfx` string local held across the bomb loop and the by-value `GXColor` literals for `GXSetChanMatColor`/`iconDispGxCol`; eki_00's `(x + 20.0f) / 2.0f` forms, the `px` temp, the inlined sine argument with a `d` delta, `moaSoundVec`, `moaPlayHit`, the `ft` temp and the `dz, dy, dx` declaration order; pmario_sound's explicit `case 1: case 3:` arms; eff_release_n64's `releaseMakeVec`, `releaseSetColor1/2/3`, and the `case 5:` label before `default`.
- Left under MUST_MATCH: zubastar's volatile bomb count/pos copy, `.rodata` Vec declspec objects, the single-case `switch` guard, the `zubastar_points_init` template with its `ZubaStarPoints4` copy, the `b18` offset base with its `Z18/Z20/Z2C` macros and the `iconDispGx` cast; eki_00's padded `"J\0\0"` compare string, the volatile `moaVecV` copies and `*(volatile f32*)` matrix stores, the `voiceSfx` union and its `(u32)` cast; pmario_sound's math header guard and `math_ext.h` include, the volatile `pos.z` and `sfxId` reads, the `(u32)bgm + 0x20` cast, the `SoundEfxSetPitch` function-pointer casts, and the uncalled `psndENV_vol` (pool order); eff_release_n64's byte-offset `AnimeData` read and the `vtxBase + off` vertex-array addresses.
- Natural (no -DMUST_MATCH) compiles of all four files succeed; full build recreates build/G8MJ01/ok.

## s-mmmix4: eff_lensflare_n64, eff_batfukidashi, battle_seq_end, rsh_06 MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix4)
- `src/effect/n64/eff_lensflare_n64.c` went from 15 to 0 `MUST_MATCH` lines and `src/effect/eff_batfukidashi.c` from 15 to 0; `rels/rsh/src/rsh_06.c` from 15 to 8; `src/battle/battle_seq_end.c` stays at 15. Each block was tried with its `#else` alone against the baseline object (`tools/obj_equiv.py`; the rel's `rsh_06.o` for rsh), then all together with the full tower `ninja` (`build/G8MJ01/ok` recreated); every changed file also compiles without `-DMUST_MATCH`. No names added.
- Now unconditional (no `#else` form matched): lensflare `position`/`fade`/`frame` locals, the five `lensflareSetColorN` void inlines (one shared `GXColor`-taking helper changed the object) and the literal `0.1f` rand expression; batfukidashi `position` local, the `effCoinFukidashiColor` inline return and the `half` 0.5f local; rsh_06 `(box.x + 20) / 2` operand order, the `px` local, the `s`/`c`/`d` sine block, the `Vec t` copy before `v3` and the `ft` temp in case 0x34.
- Left under `MUST_MATCH`: battle_seq_end's volatile zero read, the `s32 i` with `(s8)i` casts (the plain `s8 i` loop differs), the function-pointer-cast calls to `windowDispGX_Waku_col` and `btlDispTexPlane` and `effPointGetEntry`, the volatile `entry->rot/scale/color` stores and the `force_active` pragma pair around the `.sdata2` data; rsh_06's `vec3_rsh_00013cec` declspec object, the `"J\0\0"` padded string, the volatile `offset` read, `moamoa_vec_v` with its volatile accesses, and the `sv` union with the integer cast to `psndSFXOn_3D`.

## envdrv, winmgr: MUST_MATCH cleanup (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmenv)

- `src/driver/envdrv.c` went from 26 to 1 `MUST_MATCH` line, `src/manager/winmgr.c` from 25 to 13. Each block was tested alone against the baseline object with `tools/obj_equiv.py`, then all together with the full build.
- envdrv: none of the `#else` forms matched (vertex macro, tc/alpha/red locals, blur-colour helpers, the `half` local, x0s/var_r22_3 order, tx/ty order, envFF_Sqrtf, Mtx44 frustumE, the `c` cos temp, the texMtx byte pointer). The MUST_MATCH shape became the only version and the comments now say what the shape does. Left: the `#ifndef MUST_MATCH` `x0 = y0 = x1 = y1 = 0;` init in the water function (dropping it would leave possibly-unset reads in the natural build).
- winmgr: one natural form matched, the select_disp_party icon scale `scaleIcon = vec3_802ffb7c;` (the `const volatile` anti-hoist read there was not needed). The `GXSetFog` function-pointer-cast calls (four sites), `get10` and the `(WinMgrSelect*)(void*)select` cast became the only version. Left under `MUST_MATCH`: the `WINMGR_F32` macro (changes `.sdata2` size), the three `defer_codegen` pragma blocks, the volatile arrow-scale reads, the dead `x == width` guards (four sites) and the dead `firstItem` compare (three sites).
- Verified: tower `ninja` recreates build/G8MJ01/ok; both files also compile without `-DMUST_MATCH` (`-maxerrors 20`).

## s-mmmix6: ac_air_gauge.c / rsh_05.c / unit_barriern.c / party_yoshi.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix6)

- `MUST_MATCH` lines: `src/battle/ac/ac_air_gauge.c` 14 -> 3, `rels/rsh/src/rsh_05.c` 14 -> 7, `rels/common/src/v1/unit/unit_barriern.c` 14 -> 14, `src/party/party_yoshi.c` 13 -> 12. No names added; `build/G8MJ01/ok` recreated, the three changed C files also compile without `-DMUST_MATCH`.
- Became the only version (comments reworded): ac_air_gauge's `ac`/`dec`/`disp` locals in all three functions, the empty `case 4:`, the `difficulty` temp assigned in the first `else if`, `type = ac->acParams[7]`, the named `step`; rsh_05's `(box.x + 20.0f) / 2.0f` (both sites), the `px` temp, the `s`/`c`/`d` riderDelta block, the `ft` temp, the `dz, dy, dx` declaration order, and `moaSoundVec` (now unguarded, the call site only keeps the `voiceSfx` union guarded); party_yoshi's `!dist`.
- Left under `MUST_MATCH`: the `.sdata2` declspec on `dat_80420a70`, the `gv` volatile read, rsh_05's volatile pvec copy and matrix stores, the `voiceSfx` union, `moaPlayHit` (`(const char*)0x6C3`), the `MOAMOA_POSE_CHANGE` function-pointer cast; all of unit_barriern (the `(p = ..., p)` comma tried as a plain local does not match; volatile-comma temps; the volatile `InitInside`/`InitInlineOut` helpers; the duplicate `InitInline` body; the `args[-1]` store order); all of party_yoshi's volatile Vec copies, the `(char*)vec + 0x30` pool pointer and the `ten_pool_helper` pool slot.

## s-mmmix3: eff_sleep / eff_particle / battle_seq_command / mario_sbr MUST_MATCH reduction (n9-q-s-mmmix3)

- MUST_MATCH lines: eff_sleep.c 16 -> 0, eff_particle.c 16 -> 5, battle_seq_command.c 16 -> 6, mario_sbr.c 15 -> 8. Each block was tested alone (and in coupled groups) with `tools/obj_equiv.py` against the baseline object; no names added.
- eff_sleep: no `#else` arm matched, so the shapes are the only version with reworded comments (radius assigned inside the cos multiply, the `position` copy before dispCalcZ, the half-scale placed at the sin call site, `effSleepColor`, the `negativeHalfWidth` product).
- eff_particle: the emitter constants' plain field-by-field `#else` matches and is the only version; both `dont_inline` pragmas around EmitterEmit are gone (same object). The pointer-scratch `__fpclassifyf`/`particle_sqrtf` and the `firstScratch`/`secondScratch` locals stay as the only version (the by-value `#else` is a one-byte diff); the unused `particle_fpclassify`/`particle_isnan` macros went. Left: `pool_data`/`defer_codegen` pragmas and the non-const `.rodata` Vecs (data layout).
- battle_seq_command: the five `BattleMenuDisp_*_Main` function-pointer casts went by fixing the header and definitions to take `BattleWork* wp` (the target passes wp in r3); plain calls match. Left: the no-op `(s32)(s8)` argument cast, the float-first `windowDispGX_Waku_col` function-pointer cast, and the `(BattleWork*)(void*)param` cast (each changes scheduling/registers when removed).
- mario_sbr: the fused `280.0f == (dir = ...)` loads (x3), the `(s32)` enum ternary casts and the `head` ring pointer are now the only version. Left: the volatile `rubSet3C` stores and the three volatile `unk04`/`unk08` accesses, the `invert` switch with `t` (the plain if/else coalesces the temp), and the `#ifndef MUST_MATCH` marioSetBottomlessResetPosition (not in this game version).
- Verified: tower `ninja` recreates build/G8MJ01/ok; all four files compile without `-DMUST_MATCH`; `tools/fncast_lint.py` clean.

## n9-q-s-mmmix8 (MUST_MATCH reduction: eff_fireworks_n64, eff_las_mon, eff_itemget, sac_suki)

No names added. MUST_MATCH lines before -> after: eff_fireworks_n64.c 12 -> 2, eff_las_mon.c 12 -> 4,
eff_itemget.c 12 -> 0, sac_suki.c 12 -> 9. The linked build is byte-identical (build/G8MJ01/ok recreated); the four
files also compile without -DMUST_MATCH.

- eff_fireworks_n64: the two empty stub blocks and the unused GXColor-returning helpers are gone. The only versions are
  the 80.0f literal, SetColor0/1 helpers, w/h/t temporaries, `zero` local and the `s`-as-ey quad. Guarded: the zero
  Vec dead store in effFireworksMakeVec and its call site.
- eff_las_mon: the `position` copy local, the effLasMonColor inline and its call are unconditional. Guarded: the
  "LasMon\0" string padding and the empty `case 1`.
- eff_itemget: itemColor0/itemColor1 and the in-loop half-size computations are unconditional, with the #else arms
  deleted (none of them matched).
- sac_suki: make_tev_color is unconditional. Guarded: SukiTable6 and SukiBytes6 type-punned copies, the memcpy #ifndef,
  and the iconDispGx function-pointer cast (callers elsewhere pass s16/u16/s32, so no single header prototype fits).

## s-mmmix9: battle_camera.c / mario.c / eff_sunlight_n64.c / eff_glass_n64.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix9)

- MUST_MATCH lines: `src/battle/battle_camera.c` 12 -> 6, `src/mario/mario.c` 11 -> 3, `src/effect/n64/eff_sunlight_n64.c` 11 -> 6, `src/effect/n64/eff_glass_n64.c` 11 -> 4. No names added. Each block was tried with its `#else` alone (grouped where blocks pair up) against the baseline object with `tools/obj_equiv.py`; none matched, so the matching shape became the only version where it reads as normal C. The full tower `ninja` recreates build/G8MJ01/ok; all four files also compile without `-DMUST_MATCH`; `tools/fncast_lint.py` passes.
- MUST_MATCH shape became the only version: battle_camera's `bcam_get_unit` inline, the two `_t` product temporaries for `base` and the split `cy` subtraction; mario's `marioCharModeColor` helper (both uses) and the `marioScreenPos` inline; eff_sunlight's `inc * s1` temporary and `effSunlightSetColor1/2`; eff_glass's `effGlassSetColor1/2/3` and `effGlassFrame`. The now-unused `color1`/`color2`/`color3` locals went with them.
- Left under MUST_MATCH: battle_camera's `.rodata` Vec declspec objects (section declspecs), the `dxc` copy through `_t`, the `#ifndef` `yc = mid` read of an uninitialised register and the `zero`/`tmp` adds; mario's `yuu_true`/`yuu_false` goto pair, the comma-assigned `notKey` (plain `!marioChkKey()` forms all change the size) and the one-case `switch`; eff_sunlight's and eff_glass's `position = disp` copy before `dispCalcZ`, and eff_sunlight's `volatile` read of `float_0_8041ff80`.
- n9-q-s-mmmix12: MUST_MATCH lines (grep -c) mot_roll.c 10 -> 8, mario_pouch.c 10 -> 9, evt_nannpc.c 10 -> 8, eff_naniga.c 10 -> 4. Became the only version (comments reworded to say what the shape does): mot_roll `kOneTenth` local (declaration and the `dispOfs.y + kOneTenth` add), mario_pouch `pouchGetPtr()` in pouchSortItem, evt_nannpc `nannpcExtSetup()` inline call in nannpc_ext_disp, eff_naniga `effNanigaMaterialColor` local-then-set-alpha form, `40.f + data->end` and the `40.0f` literal in the second wrap loop of effNanigaMain. Stayed guarded: mot_roll pollPoolPrefix/rollFlushPools pragma and force_active data, and the five volatile reads (targetYaw, directionView); mario_pouch `base + id * sizeof(ItemData)` in comp_aiueo/comp_aiueo_r (a typed `const ItemData* base` with `base[id].name` was tried and fails the checksum) and the pouchReceiveMail `slot[2]` form; evt_nannpc `pos = savedPos` dead copy and `pos = pos`, the `s64 id` in evt_nannpc_entry, the u32 `rawElapsed` in evt_nannpc_jump_position; eff_naniga `ICON_ID` u16 punning and the duplicate `initializeVec2` inline (pool layout). Full build recreates build/G8MJ01/ok; changed files compile without -DMUST_MATCH.
- n9-q-s-mmicon: src/battle/battle_status_icon.c MUST_MATCH 30 -> 6 lines. Each block tested alone against the baseline object with tools/obj_equiv.py. None of the `#else` forms matched individually. Readable rewrites that matched: the `turnCount` negate-select (five sites) is `turns >= 0 ? turns : -turns` (the old comment claiming a ternary fails applied only to `turns < 0 ? -turns : turns`), and the biribiri `BIRI_VEC` macro is gone (the function indexes `vec[12..17]` from `vec3_803011e8` like its siblings). The MUST_MATCH shape became the only version, comments reworded: the thirteen `BtlUnit_CheckStatus`/`BtlUnit_GetStatus` s8 function-pointer casts (an `(s8)type` argument cast or an `s8 type` local do not match: the former is hoisted into its own register, the latter adds an extsb at the store; changing the prototypes to s8 breaks BtlUnit_CheckRecoveryStatus in battle_unit.c, whose own calls pass the int through unextended), the two `btlDispTexPlane` casts with the extra trailing s32 0, and the guruguru `tmp` Vec copy. Left under MUST_MATCH: the four `.rodata` declspec Vecs (data layout). Natural (no -DMUST_MATCH) compile succeeds; full build recreates build/G8MJ01/ok.

## s-mmmix11: eff_gonbaba_breath, eff_funemizu, fadedrv, battle_stage MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix11)

- `MUST_MATCH` lines (any mention, comments included): `src/effect/eff_gonbaba_breath.c` 11 -> 0, `src/effect/eff_funemizu.c` 11 -> 5, `src/driver/fadedrv.c` 11 -> 7, `src/battle/battle_stage.c` 11 -> 6. No names added. Each block was tried against the baseline object with `tools/obj_equiv.py`; the full tower build recreates `build/G8MJ01/ok` and all four files compile without `-DMUST_MATCH`.
- Plain form became the only version: the `intplGetValue` function-pointer casts in gonbaba_breath and battle_stage (the cast was to the prototype's own type, and `system.h` already reorders the arguments), the second `f5/f4/f1` literal order in gonbaba_breath, `FUNE_DATA` and its `base` register in funemizu (tables now named directly), fadedrv's `animName` local.
- MUST_MATCH shape became the only version: gonbaba_breath's first `f5/f4/f1` order, the two-statement `speed`, `position = initializeVec(...)`, `size`/`negSize`/`half` in the quad code; funemizu's in-loop `dz = d * 0.5f`; fadedrv's `makeFadeTevColor` inline; battle_stage's `sgcol`/`sgrow` split declaration and `idx2` temporaries.
- Left under `MUST_MATCH`: funemizu's display-list pointer-walk macros, volatile `FUNE_RELOAD` and `oneR` reload, and the `*(GXColor*)&dat_8042146c` helper; fadedrv's `+ zero` adds, `fadedrvPoolPi`, pad/force_active/section data and the flush helper; battle_stage's GXWGFifo writes and the `zero` alias with function-scope counters.
- `src/effect/n64/eff_kemuri1_n64.c` MUST_MATCH lines 15 -> 0, `eff_fireflower_n64.c` 15 -> 7, `eff_stardust_n64.c` 14 -> 4, `eff_magic1_n64.c` 14 -> 2. No names added; build/G8MJ01/ok recreated and objects checked against the baseline with `tools/obj_equiv.py`.
- The MUST_MATCH shape became the only version (comments reworded): kemuri1's `arg` sin/cos local, split reviseAngle expression, direct 0.94f stores, s30/c34 temps, the SetChanMatColor inlines and the size/size2 pair; the `Vec position` copy in the fireflower, stardust and magic1 Main functions; fireflower's `fadeScale`; stardust's `p` temp and SetTevColor inline (its unused `GXColor color` local is gone); magic1's shift-based phase macros, the magic1Color1/2 inlines and the `t` product.
- Left under MUST_MATCH: fireflower's reordered-prototype function-pointer cast for effRotHammerSuccessN64Entry and its `vtx + offset` vertex-array casts through lbl_8039CA10; stardust's comma-expression `((f32)i, (f32)i)` (two sites); magic1's swapped `cosAngle`/`angle` names in the ring loop.

## MUST_MATCH reduction: eff_akari_charge_n64, eff_updown, eff_mizutama, eff_ibuki (s-mmmix10)
- No new names. `eff_ibuki`: `DATA_AT` (anchor `lbl_803B4860` + offset) dropped for the tables themselves (same relocation targets), the `base` local removed. Colour helpers (`effIbukiColor0/1`) and the `sixteen` local are now the only version. The angle comma expression and the `opt_loop_invariants` pragma stay guarded (no other form matches).
- `eff_akari_charge_n64`: `position` copy, `zero` local, `akariColorProj/Part` are now the only version; the comment-only MUST_MATCH block was removed. `eff_updown`: per-case `a = alpha`, the `k`/`t`/`wave` temps and the `p = &pos` alias are the only version (`#else` alone does not match). `eff_mizutama`: `position` copy is the only version; the volatile `float_0`/`float_1` externs and the chained `zero` assignment stay guarded.
- MUST_MATCH lines before/after: akari 11/0, updown 11/0, mizutama 11/7, ibuki 11/4.

## s-mmmix20: MUST_MATCH reduction (eff_stone_smoke, eff_snowman, eff_pokopi_pcharge, eff_crystal, eff_confetti N64)
Structural only, no names added. Made the MUST_MATCH shape the only version (guards and #else arms dropped): the
position scratch-copy Vec in Main, the cached frame/velX/timer locals, the per-colour GXSetTevColor inlines
(stone_smoke, confetti colour0), the crystal sin temp, confetti makeConfettiVec. Still guarded: confetti
makeConfettiColor1 (`*(GXColor*)&float_...` type-punned read). Build MATCH.

## MUST_MATCH reduction (s-mmmix13)
- eff_opuku_jetwrain_n64 (10 -> 0 lines): all guards gone, MUST_MATCH shapes are the only version (makeTevColor inline,
  `12 > i` loop, innerEdge/x2 declaration order, xBase/fieldBase/fieldEnd placement, `x = jPos + x`); none of the #else arms matched.
- eff_flame_n64 (10 -> 5): OSFastCast include, OSs16tof32/OSf32tos16 vertex transform, and effFlameSetColor1/2 are unconditional
  (unused `color` local removed). Guarded: the "FlameN64" string padding, the position copy in effFlameMain (single local does not match).
- eff_balloon_n64 (10 -> 9): the OSf32tos16 `out` local is unconditional. Guarded: the two embedded-assignment `wave` blocks
  (split or #else forms differ), and the tri1WithUnused function-pointer cast.
- hitdrv (10 -> 8): hitDeleteUpdateActiveList removed, hitDelete calls hitUpdateActiveList (matches). Guarded: hitFindElem
  (dead locals, void* name punning), hitSetGQR5 inline asm, and the MapWork* cast in the ray-query return path.
- n9-q-s-mmmix16: MUST_MATCH lines (grep -c) unit_party_chuchurina.c 8 -> 7, party_cloud.c 8 -> 3, nameent.c 8 -> 3, mot_slit.c 8 -> 8, koopa.c 8 -> 5. No names added. Became the only version (comments reworded): chuchurina's duplicate `evtSetValue(event, out, item)` arms; party_cloud's `cloudMoveToMarioStep` (now through `party->targetPos`; only the `opt_common_subs off`/`reset` pragma pair is unguarded, since the pragma pair was already used earlier in the file), the `getHitBreatheout2_mode` inline and the `dz, dx` declaration order; nameent's `x1/x2` operand order, the `ofs2 += 2, count2++` loop increment, `NAME_WIN_SCALE2` with hh2/ww2 and the bottom-row `y - w + 16.0f` quads; koopa's `mp2`/`py` temporaries in the hit-flame setup. Stayed guarded: chuchurina's `weaponBase + offset` table (raw blob offsets), `_chuchu_pick_drop`/item-steal getters, `item = (u16)id` and `(u16)id` range test; party_cloud's `RELOAD_MARIO` redundant reload, the assign-in-condition `unk8`/`maxDist` clamp (the plain two-load form fails the checksum), and the `cloudSetMarioSpeed` pool function; nameent's `nameWinGX` function-pointer casts; mot_slit's two `zero`/`done` gotos, `tmp` GXColor type-punned copies, `s8` punned stick reads and `speed = speed`; koopa's sdata2 declspec/definition blocks, the `defer_codegen` pragmas and the `kpaFireBreath` helper twins. Full build recreates build/G8MJ01/ok; fncast_lint passes.
- n9-q-s-mmmix16 correction: party_cloud.c keeps the `opt_common_subs off`/`reset` pragma pair under MUST_MATCH (party_cloud.c MUST_MATCH lines 8 -> 5 in total); the previous entry said it was unguarded.
- n9-q-s-mmmix16: MUST_MATCH lines (grep -c) unit_party_chuchurina.c 8 -> 7, party_cloud.c 8 -> 5, nameent.c 8 -> 3, mot_slit.c 8 -> 8, koopa.c 8 -> 5. No names added. Became the only version (comments reworded): chuchurina's duplicate `evtSetValue(event, out, item)` arms; party_cloud's `cloudMoveToMarioStep` (now through `party->targetPos`; its `opt_common_subs off`/`reset` pragma pair stays guarded), the `getHitBreatheout2_mode` inline and the `dz, dx` declaration order; nameent's `x1/x2` operand order, the `ofs2 += 2, count2++` loop increment, `NAME_WIN_SCALE2` with hh2/ww2 and the bottom-row `y - w + 16.0f` quads; koopa's `mp2`/`py` temporaries in the hit-flame setup. Stayed guarded: chuchurina's `weaponBase + offset` table (raw blob offsets), `_chuchu_pick_drop`/item-steal getters, `item = (u16)id` and `(u16)id` range test; party_cloud's `RELOAD_MARIO` redundant reload, the assign-in-condition `unk8`/`maxDist` clamp (the plain two-load form fails the checksum), and the `cloudSetMarioSpeed` pool function; nameent's `nameWinGX` function-pointer casts; mot_slit's two `zero`/`done` gotos, `tmp` GXColor type-punned copies, `s8` punned stick reads and `speed = speed`; koopa's sdata2 declspec/definition blocks, the `defer_codegen` pragmas and the `kpaFireBreath` helper twins. Full build recreates build/G8MJ01/ok; fncast_lint passes.
- n9-q-s-mmmix19: MUST_MATCH lines (grep -c) dmo_00.c 8 -> 0, party.c 7 -> 1, mot_jabara.c 7 -> 5, evt_npc.c 7 -> 2, eff_thruhammer_n64.c 7 -> 0. No names added. Every block was built alone against the baseline (tower `ninja` + `build/G8MJ01/ok`). Became the only version (comments reworded): dmo_00 demo_desk/demo_img named temporaries and the `b` assignment inside the GXProject call; party partyMakeDispDir `want` cache (no `#else` arm matched, alone or paired); mot_jabara `JABARA_F32` nonconst view and marioChkJabara's chained `wMotionTimer = check = 0`; evt_npc `int elapsed`, the `subf32` inline with `delta` before dx/dz, and the color copy, which now reads `tmp = ...; color = tmp; npcSetColor(desc, &color)` (the comma expression was not needed); eff_thruhammer `s` local for dirX and the thruColor3/thruColor4 void inlines. Stayed guarded: party's animPoseSetPaperAnim function-pointer cast (the definition takes two args, other callers use the same cast); mot_jabara's volatile `RDBL`, the `defer_codegen` and `opt_common_subs` pragmas (each removal fails); evt_npc's `yaw = yaw` self-assignment (dead store; the plain form fails). Full build recreates build/G8MJ01/ok; all five files compile without -DMUST_MATCH; `tools/fncast_lint.py` clean.
- n9-q-s-mmmix19: MUST_MATCH lines (grep -c) dmo_00.c 8 -> 0, party.c 7 -> 1, mot_jabara.c 7 -> 5, evt_npc.c 7 -> 2, eff_thruhammer_n64.c 7 -> 6. No names added. Every block was built alone against the baseline (tower `ninja` + `build/G8MJ01/ok`). Became the only version (comments reworded): dmo_00 demo_desk/demo_img named temporaries and the `b` assignment inside the GXProject call; party partyMakeDispDir `want` cache (no `#else` arm matched, alone or paired); mot_jabara `JABARA_F32` nonconst view and marioChkJabara's chained `wMotionTimer = check = 0`; evt_npc `int elapsed`, the `subf32` inline with `delta` before dx/dz, and the color copy, which now reads `tmp = ...; color = tmp; npcSetColor(desc, &color)` (the comma expression was not needed); eff_thruhammer `s` local for dirX. Stayed guarded: eff_thruhammer's thruColor3/thruColor4 void inlines and their four calls (the `*(GXColor*)base` pun reads) with the readable `#else`; party's animPoseSetPaperAnim function-pointer cast (the definition takes two args, other callers use the same cast); mot_jabara's volatile `RDBL`, the `defer_codegen` and `opt_common_subs` pragmas (each removal fails); evt_npc's `yaw = yaw` self-assignment (dead store; the plain form fails). Full build recreates build/G8MJ01/ok; all five files compile without -DMUST_MATCH; `tools/fncast_lint.py` clean.
- s-mmmix14 (MUST_MATCH reduction: party_nokonoko, eff_pturn_n64, eff_magic_horao_n64, eff_fire_spark_n64): MUST_MATCH
  lines 9/9/9/9 -> 5/4/1/4. Only versions now: NK_STR (both arms identical), fadd32 and its use, pturn's three
  effSetVtxDescN64 vertex-array arguments (the #else named arrays match; vtxBase gone), horao's makeHoraoColor/
  makeHoraoColor2 inlines and the named `s` sin temp (the #else forms did not match, so the readable shape is kept),
  fire_spark's setFireSparkColor1/2/KColor helpers (#else arms did not match) and the literal-0.0f PSMTXTrans.
  Guarded: marioGetScreenPos function-pointer cast, the two volatile speed re-reads, the duplicated dead distance
  store, pturn/fire_spark's `position` scratch copy, and horao's .sbss2/gap_ declspec block.
  Checked with tools/obj_equiv.py against the baseline objects.

## s-mmmix7: party_bomhei.c / unit_party_vivian.c / mot_ship.c / evt_telop.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix7)

- `MUST_MATCH` lines: `src/party/party_bomhei.c` 13 -> 9, `src/unit/party/unit_party_vivian.c` 12 -> 0, `src/motion/mot_ship.c` 12 -> 5, `src/event/evt_telop.c` 12 -> 9. No names added. Each block was tried with its `#else` alone and with the MUST_MATCH arm alone against the baseline object (`tools/obj_equiv.py`); the full tower `ninja` recreates `build/G8MJ01/ok`.
- `#else` alone matched (vivian): the `battle_evt_majo_disp_on` first-call zero stores (the `volatile` casts are not needed), the `__makeTechMenuFunc` weapon entries via `&partyWeapon_Vivian*` (the `lbl_80388868` base register is gone), and two empty `#ifdef MUST_MATCH`/`#endif` pairs. The stale "naming each weapon scores 89%" and "MUST_MATCH comma expression" comments went with them.
- The MUST_MATCH shape became the only version, comments reworded: vivian's `unk_8017d03c` (`a1`-`a3`, `base`, statement order); bomhei's `gravity` temps (two) and `quarter` temps (two); mot_ship's `!speed` test, the `jumpVelY`/`jumpAccelY` stores with `(dy + dy) / 306.0f` (the `(u8*)mario + 0x7C/0x80` offsets became the named fields, same object), the single-expression `n` rounding, and the four `t` temps in shipMove; evt_telop's `telopMakeColor`/`telopMakeColorS10`/`telopSetFontColor` inlines (the `color`/`colorS10` locals are gone) and the `a`/`b` PSMTXTrans temps.
- Left under MUST_MATCH: bomhei's `dat_802ccf60` layout padding, the `"M_J_1B\0"` padded string, the `*(s32*)&field_22[2] = (s16)frames` punned stores (two), the `angles` struct-copy from `dat_802ccf60`, and the three `accepted` do-while(0) blocks in check_front (a dead `hit == NULL` compare; the `#else` form and a flag-only `accepted = FALSE` form both change the object size); mot_ship's `(zero = 0.0f)` dead store, the two `- -f30` compares, and the `ftmp` reload before the swirl clamps; evt_telop's `volatile` FP constant declarations and definitions, the volatile reads (`s`, `z`, `one` temps) and the `scale = scale` empty-then ratio test.
- n9-q-s-mmmix18: MUST_MATCH lines (grep -c) eff_pokopi_pipo_n64.c 8 -> 7, eff_peach_fukidashi.c 8 -> 2, battle_damage.c 8 -> 8, tou.c 8 -> 6, kpa.c 8 -> 7. No names added. `#else` alone matched: tou's `GXSetViewport(0.0f, 0.0f, 128.0f, 128.0f, 0.0f, 1.0f)` (the named z/w locals are gone), and kpa's `pouchGetKpaCoin()` call once its prototype and definition return `u32` (header and mario_pouch.c; the only caller is kpa_status_disp), which removes the function-pointer cast. MUST_MATCH shape became the only version: the two-step `angle = -viewYaw; angle = 0.017453292f * angle` (pokopi_pipo Disp) and `rad` (three sites in peach_fukidashi Disp; the single-expression form and `0.017453292f * -viewYaw` fail), peach_fukidashi's empty `case 2:` before `default:`, and tou's second read of `scroll` through `dwp[0]`. Stayed guarded: pokopi_pipo's `position = disp` copy, the `f32 k = -1.0f` multiply and `velY - velY`; peach_fukidashi's word-copy of the banner string; all of battle_damage (the `s8` ohkoChance pun, three function-pointer casts, `s32 rate` with its `(s8)` casts - `s8 rate` alone fails - and the pooled SFX string locals); tou's `TOU_F32` pun and `monitorColor` (GXColor pun; the in-place form fails); kpa's `iconDispGx` function-pointer casts (argument order differs from the header, other units call it with other signatures). Full build recreates build/G8MJ01/ok.

## MUST_MATCH reduction (s-mmmix17)
- eff_star_power_n64: `starPowerColor` inline is now the only form (guard and local-GXColor #else arms gone). Still guarded: the `.rodata` Vec declspec and the `f7` embedded assignment.
- eff_rappa_n64: `tmp`/`randValue`/`cosPhi` temporaries are the only form. Still guarded: the anonymous zero-Vector in `initializeVec`.
- eff_radiation_n64: `radiationSetColor` inline is the only form; `scaleZ` keeps the repeated expression (`scaleZ = scaleX` does not match). Still guarded: string padding and the `tri1` function-pointer cast.
- evt_bero: all blocks stay (name-table copies depend on the guarded `.rodata` jumptable, string padding, `flush_pools`).

## MUST_MATCH reduction: s-mmmix26 (2026-10-20, Claude Sonnet 5.5)

- `MUST_MATCH` lines: `src/effect/eff_majinai.c` 6 -> 0, `src/effect/eff_damage_star.c` 6 -> 0, `src/effect/eff_break.c` 6 -> 4, `src/driver/itemdrv.c` 6 -> 5, `src/driver/imgdrv.c` 6 -> 6, `src/driver/effdrv.c` 6 -> 6. No names added. Each change was checked against the baseline objects with `tools/obj_equiv.py`; the full tower `ninja` recreates `build/G8MJ01/ok`.
- `#else` alone matched: itemdrv `itemseq_Bound`-area `dirX == 0.0f && dirZ == 0.0f` compare (the operand-swapped MUST_MATCH arm is gone).
- The MUST_MATCH shape became the only version, comments reworded: majinai's `effMajinaiGetInBattle` inline and the `position` copy of `disp` (it must be declared before `work`); damage_star's `damageStarSetAmbColor`/`SetMatColor`/`SetTevColor` helper chain (the `#else` inline calls change `.sdata2` and bytes) and the named `f05`; eff_break's `t` temp in effBreakMain (`float_20 * data->scale` and `data->scale * float_20` both fail).
- Left under MUST_MATCH: eff_break's redundant language test, the volatile zero in `PSMTXTrans`, the volatile `GXWGFifo` writes and the volatile `float_1_804210bc`; itemdrv's `vec3_802cac04`/`vec3_802cac1c` `.rodata` declspecs and the volatile `status` read; imgdrv's two `? 1 : 0` argument ternaries and the identical-arm `flags & 8` if/else; effdrv's `"\0"` format-string padding and the hand-written GQR5/psq_l asm helpers.

## s-mmmix25: MUST_MATCH reduction (eff_kameki_tornade_n64, eff_hokori_n64, eff_ultra_hammer, win_main, evt_win)
Structural only, no names added. The #else arm alone matched (guards dropped) in eff_hokori_n64 (vtx arrays reached by
their own symbols, no lbl_8039D5D0 offsets) and eff_ultra_hammer (wakka tables by symbol, wakkaDrawDL() without base).
MUST_MATCH shape made the only version: hokori `k = 0.9f` local, kameki SetVtx (SDK fast-cast inlines) and
kamekiTornadeSetColor (GXSetTevColor inside the inline), win_main fog-colour helpers, evt_win `half = 0.5f` local.
Still guarded: kameki string padding and SetColor2 (type-punned `*(GXColor*)&dat_8041f1d8` load), win_main GXPosition3f32
FIFO macro and winFontSetC (reconstructed function), evt_win function-pointer casts, `*(u16*)` icon_id punned loads,
the buttons do/while(0) tail and unitwin_icon_delete. Build MATCH.

## s-mmmix21: MUST_MATCH reduction (mixed files)

- MUST_MATCH lines (before -> after): `eff_machinegun.c` 7 -> 0, `battle_disp.c` 7 -> 1, `ac_stick_keep_left.c` 7 -> 3, `ac_monosiri_target.c` 7 -> 5, `eff_toge_flush.c` 7 -> 7, `windowdrv.c` 7 -> 7. No names added; build/G8MJ01/ok recreated, objects checked against a baseline build with `tools/obj_equiv.py`.
- The MUST_MATCH shape became the only version (comments reworded): machinegun's `Vec position` copy and `f`/`h` split of `x0` (plus an empty guard); battle_disp's switch in btlDispTexPlainGX and the one-case switches in `_pose_def` and `_pose_one_pattern` (the unused `str_*` externs went with the dead #else); stick_keep_left's `sfxName[2]` array and `stickLeftAdd25` inline; monosiri_target's `ac` local.
- The plain `BattleActionCommandGetDifficulty(work)` call now matches (the header already says `s32`), so the function-pointer casts in `_getmaxcount` and stick_keep_left's ok_frame index are gone.
- Left under MUST_MATCH: battle_disp's `BtlUnit_CheckStatus` cast (the prototype is shared by 139 callers); stick_keep_left's `case 0x3EB` dead `n >= 0` compare and `iconDispGx` cast; monosiri_target's `vec3_80300880[11]` rodata array and `iconDispGx` cast; toge_flush's non-const `.rodata` Vec, the `*(GXColor*)&unk_80423070` colour inlines and their switches; windowdrv's `gap_` dummy, GXWGFifo writes and `goto ret` (a plain `return` changes the code).

## s-mmmix23: MUST_MATCH reduction (win_badge, sound, romfont, himo, evt_party, evt_item)

No names added. MUST_MATCH line counts before -> after: win_badge.c 6 -> 6, sound.c 6 -> 4, romfont.c 6 -> 1,
himo.c 6 -> 6, evt_party.c 6 -> 4, evt_item.c 6 -> 6. Objects are obj_equiv-equivalent to the pre-change ones and
build/G8MJ01/ok is recreated; romfont, sound and evt_party also compile without -DMUST_MATCH.
- romfont.h `RomFontChar::width` is now u16 (the asm loads it with lhz). That removed both `*(u16*)&glyph->width`
  reads, and `curX`/`curY` are plain s32 with the s16 `x`/`y` copies for the GXPosition3s16 calls as the only form.
  Only the second-load `cur` in romFontFindCharW stays guarded.
- sound.c: `sdd_eff`/`sdd_dvd` are the pointer-returning inlines unconditionally (the macro form scores lower).
- evt_party.c: the `fr`/`spd`/`frames` order in evt_party_move_pos2 is the only form (the one-expression form swaps
  the int->float and fctiwz slots).
- Stayed guarded: win_badge `arg0` copies (a typed copy also fails) and the winFont*/winIcon* function-pointer casts;
  sound SoundDVDChan (u32 cast) and the SoundSSPlayChEx path helper; himo's constant locals (including the unused c15
  store), the s32 pointer locals and spline_getvalue int-pointer calls; evt_party's `f32 pos` for a Vec and the
  searchUnder2 pointer cast (header returns s32; fixing it touches mario_hit.c and evt_mario.c); evt_item's pool
  constant and `rawStep` shape.
### Readability: filemgr offsets (2026-10-20, Claude Sonnet 5.5, n9-q-s-offfilemgr)

`fileGarbageDataAdrSet`'s raw `*(u32*)((u8*)data + 0xNN) += (u32)data` runs now go through file-local header views
`FileType1`, `FileType3` (also used for types 6 and 10), `FileType7`, `FileType8` and `FileType9`: a `pad_0x0` gap then
`u32 field_0xNN` members for exactly the relocated words, named after the `FileType` enum. FILE_TYPE_2 and the
per-animation walk in FILE_TYPE_ANIMPOSE use `AnimData`'s existing `clip`/`frames`/`posData`/`nrmData`/`texData`/
`visData`/`nodeData`/`frames2` through `FILE_RELOC`. No new names beyond `field_0xNN`. Build stays MATCH; filemgr.c
also compiles without -DMUST_MATCH. evt_win.c untouched: `unitwin_setpartylank`'s `(PouchPartyData*)((u8*)pouch +
offset)` accesses rewritten as `pouch->partyData[id]` (all four, the last pair only, the first three only) each broke
the match, and `unitwin_help_disp`'s `(u8*)list + offset` is the deliberate byte-offset form its comment describes.

### Readability: win_root remaining raw offsets (2026-10-20, Claude Sonnet 5.5, n9-q-s-offroot2)

win_root.c weighted 435 -> 341; raw_offset total 1014 -> 959. Build byte-identical (build/G8MJ01/ok recreated); win_root.c compiles without `-DMUST_MATCH`.
- Existing names only: the `compare_func*` item-id reads at +8 go through `SortRec.c`; the mail `+8` reads through `WinMailEntry.name`; `camGetPtr(8)->view` (0x11C); `anim->clip[2]`; `work->field_404` and `work->field_26E` (win_main.h bands) for the qqsort bases and the mail index pairs; `((u8*)a)[1]` for the pair bytes.
- New members, named by offset, inside existing WinRoot pads (size unchanged): `s16 field_0xEB8[0x6C]` and `s16 field_0xFB4[0x42]`; every user in the tree reads them as s16 (git grep: none outside win_root.c).
- Left raw: the 13 `((WinRootTab*)((u8*)tabs + tabSel * 0x18))` and `recs` sites (pinned by #588, array indexing scored lower); `WW_*(work, 0x13C/0x140/0x144)` (read as s32, u32 and void* in different sites, so no single type); `qqsort((u8*)work + 0x1050, ...)` (past the end of WinRoot, `&field_40[0x1010]` base noted in win_main.h); the `base + 0x404` / `p + 0x14` computed-base loops; `WIN_*_TEX` macros' `*(u8**)((u8*)w + 0x28)` (type-punned winTpl).
### Readability: win_item raw offsets (2026-10-20, Claude Sonnet 5.5, n9-q-s-offwinitem)
`src/window/win_item.c`: 18 raw-offset sites moved onto existing fields; no new names, no header changes. Build MATCH on the tower; the natural compile (no `-DMUST_MATCH`, `-maxerrors 20`) is clean.
- Mail index pairs: `p[0x26E]`/`p[0x26F]` and `b[0x26F]` walks are `work->field_26E[i * 2]` / `[mailSel * 2 + 1]` (existing WindowWork band; only win_item reads it as u8).
- `item_disp`'s `*(f32*)((u8*)work + itemTab * 4 + 0x224)` is `(&WINITEM(work)->field_0x224)[work->itemTab]`.
- The mail texture file is `work->mailTpl` (FileEntry*) instead of `WW_S32(work, 0x2C)` with casts; `WW_S32` is gone.
- `((s32*)((u8*)work + 0x214))[sel]` became `work->itemSel[sel]` at 10 of the 18 sites (tried site by site, each kept only if the build stayed MATCH).
- Left: the other 8 itemSel sites (the two reads near pouchHaveItem/useItemSlot, the `&...[sel]` pointer, two idx loads, the `count - 1` store and the two `/ 10` reads) break the match when converted; `item_disp`'s `entry + 0x2E8` (entry walks by 2, `skipList[i]` breaks the match); `WW_F32((u8*)w + idx * 0x14, 0xC4/0xC8)` in winItemDisp (computed base, WindowWork has no field there).
- Index: raw_offset tree-wide 1014 -> 996; win_item.c 236 -> 201 weighted; overall 18.097 -> 18.048 per kLOC.

## s-mmmix28: win_party.c / unit_party_yoshi.c / party_chuchu.c / npc_event.c / mario_motion.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix28)

- `MUST_MATCH` lines: win_party.c 5 -> 1, unit_party_yoshi.c 5 -> 3, party_chuchu.c 5 -> 5, npc_event.c 5 -> 2, mario_motion.c 5 -> 2. No names added. Checked with tools/obj_equiv.py against the baseline objects.
- Only version now: win_party's `work->partyId[i]` loop (the `u8* p` walk is gone) and `4 != winPartyDt[n].partnerId` (the `!= 4` spelling does not match); yoshi's `__makeTechMenuFunc` entries via `partyWeapon_Yoshi*` (the `lbl_80379A00` base pointer is gone); npc_event's `t = 20.0f; t = t * offset;` in `_2d_get_dead_jump_offset`; mario_motion's `f32* const q` and a single `marioSlitChkWallAround() != 0 || marioCreviceWallChk(...) != 0` return (the `goto fail` is gone).
- Guarded: win_party's by-value window-draw function-pointer casts; yoshi's `yoshi_pool_i2f` pool function, the sweat `t = t` dead store and the `btlDispTexPlane` function-pointer cast; chuchu's `ChuchuSearchState` blocks (the plain-local forms do not match); npc_event's dead compare in the jump setup; mario_motion's `str_win_*` force_active data and the BOTTOMLESS_* macros.

## Raw-offset cleanup: winmgr, animdrv (2026-10-20, Claude Sonnet 5.5, TAG s-offwinmgr)

- `winmgr.c` has no raw-offset sites (its `+ 0x..` hits are screen-coordinate arithmetic); unchanged.
- `animdrv.c`: raw_offset 46 -> 31. `AnimData` gains `field_0x44`, `field_0x48`, `field_0x50`, `field_0x54` (f32, `field_0x4C[4]` between), replacing the `ANIMDATA_F44/F48/F50/F54` byte-offset macros' bodies; no other file reads `AnimData` past 0x44. A file-local `AnimFrame` overlay (field names taken from the existing `ANIM_FRAME_*` macro names) replaces the eleven `ANIM_FRAME_*` byte-offset bodies.
- Left raw: `wp->poses + i * 0x170` and `wp->files + id * 0x10 + 8` (559, 813, 1231, 869) and the `poseOffset` sites: `wp->poses[i]` / `wp->files[id].handle` each break the build (tried one at a time); the `poseOffset` form is a documented match shape. Vertex/index buffer pointer arithmetic (1123, 2153-2160, 2241-2312, 2387) steps by byte counts over untyped buffers, not struct fields.

### Raw-offset cleanup: mapdrv, mobjdrv (Claude Sonnet 5.5, s-offmapdrv)
- `MapEntry`: the 8-byte opaque band at 0x170 is now `void* field_0x170; void* field_0x174;` (ARAM copies, only mapdrv reads them as pointers). bmapLoad's chunkBuf/chunkBufSize accesses use the named fields. Raw index 876 -> 871; build MATCH.
- Left raw: the 0x178 access (past the 0x178-byte MapEntry stride, no field to hold it); `MapObject` 0xEC..0xF2 bounds (splitting the u8 band into four u16 members changes the store scheduling in the clear block and breaks the match); `mapDispMapGrp_bbox` (reads MapEntry+0xEC, mid-field of `bbox`); material +0x10C (mid-field of `MapMaterialTex::entries`); MapWork `activeGroup * 0x2F4 + off` accesses; mobjdrv has only cubeDN data-layout offsets and `&color` casts.

## n9-q-s-offaud: raw offsets in battle_audience.c (2026-10-20)

- `BattleAudienceWindow`: the pad at 0x10 becomes `f32 field_0x10` (only battle_audience.c reads the window; it stores the `0.0f` next to `posY`). `*(f32*)((u8*)window + 0x10)` is now `window->field_0x10`.
- `BattleAudienceMember.animPos` (0x28, `s32*`): the eight `*(s32*)((u8*)member + 0x28) += N` cursor moves in BattleAudienceMain's script switch are now `member->animPos += N / 4`. Same object.
- Left raw: `(u8*)_battleWorkPointer + 0x20000`, `wpHigh` and `audlocal` (documented codegen levers that keep the addis recomputation); `setupBase` (offsetof base); `*(u8*)&info->mStylishCommandMultiplier` (a u8 read of an s8 field); `member + 0x134` in BattleAudience_Delete (the next member's flags, past the struct); `BattleBreakSlotGetPtr() + 0x14` (BattleBreakSlot and its reel type are private to battle_break_slot.c, `field_14` f32 there). raw_offset 10 -> 1 by the index's count.

## s-mmmix5: eff_mugi.c, eff_mahorn2.c, battle_unit.c, battle_icon.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix5)

- `MUST_MATCH` lines: `src/effect/eff_mugi.c` 14 -> 4, `src/effect/eff_mahorn2.c` 14 -> 7, `src/battle/battle_unit.c` 14 -> 9, `src/battle/battle_icon.c` 14 -> 8. Each block was tried with its `#else` alone against the baseline object (`tools/obj_equiv.py`); the full tower `ninja` recreates build/G8MJ01/ok. No names added.
- `#else` alone matched: battle_unit's `wp->unitIdTable[i] = id` (two sites; the `(u8*)wp + 0x120` pointer store is gone); battle_icon's `icon->position = icon->moveCurrentPos` reads in `btlevtcmd_BtlIconJumpPosition` (the fresh `p` base pointer is gone, along with its comments); mahorn2's identical `half`/`two` pairs.
- The MUST_MATCH shape became the only version (comments reworded): eff_mugi's `sinAngle` temp, the separate `j` loop index (two functions), and the `mugiKemuriColor`/`mugiKemuri2Color` inlines; eff_mahorn2's `k`/`t` locals in the case-3 scale blend and the `mahorn2ChanColor` inline; battle_unit's `half` local in `BtlUnit_GetExp` and the `s16 sum` temp in `BtlUnit_snd_se_pos`; battle_icon's `d = x - vx` local.
- Left under MUST_MATCH: eff_mugi's two volatile prologue reads in the Kemuri main functions; eff_mahorn2's `vec3_80301948[3]` array with the three Vec inlines and their `disp`/`targetDisp` users (data base register and stack-slot order; dropping any of them changes `.rodata`); battle_unit's `.sdata2` colour declspec, the `UnitStatusBlock` struct-copy casts (two sites), the `partyData` `s16*` indexed load, the `pouchGetAP`/`pouchAddAP` function-pointer casts and the local static double with its pun read; battle_icon's `u16` reinterpretations of the `s16` `flags`/`iconId` (six sites) and the two `BtlIcon_Entry` function-pointer casts.

### Readability: raw offsets in batten_leader, evt_lect_hei, bom_01, end_unknown, party_nokonoko (2026-10-20, Claude Sonnet 5.5, n9-q-s-offmisc)

raw_offset total 876 -> 831. Build byte-identical (build/G8MJ01/ok recreated); all five files also compile without `-DMUST_MATCH`.
- unit_boss_batten_leader.c: `eff_staff` and the Syuryou update in the unit script go through the existing `EffSyuryouData` (`position`, `scale`). `evt_eff_colorchange00`/`barriersizechange`/`firechange00` use a file-local `BattenEffUserdata` overlay (members `field_0xNN` plus `pad_0xNN`; each offset is read with one type in this file; no header declares this effect). `battenSatelliteTo_Init`/`_Main`/`_InitInline` use a file-local `BattenSatelliteSpline` overlay (`field_0x0`, `field_0x4`, `field_0x8[5]`, `field_0x1C[15]`, `field_0x58`, the 0x5C `__memAlloc` block).
- evt_lect_hei.c: the effect userdata stores go through `effItemGetData` (`camId`, `scaleMode`), `EffMarioBalloonData.cameraId` and `effFukidashiData.camId`. The `lbtn` byte-offset sites stay: `lbtn[index].iconId/alpha/seq/position/flags` each changed hei.rel, so the `index * 0x44` form is kept.
- bom_01.c: `PoseData` gets `s32 field_0x4` out of `pad_2[6]` (all six users read the pose count as s32; header is bom-local); `SightWork.file` is now `FileEntry*` so the `file + 0xA0` palette reads become `(TEXPalette*)file->data[0]`.
- party_nokonoko.c: the three `+ 0x180` zero stores (and the `NKS(0x180)` one) use `tail.koops.field_180`. The 0x178/0x17C zero stores stay raw: they write s32 zeros over an f32 and two s16 members, so a typed store would change the object.
- end_unknown.c: `credits_ending_records` `texId` and the next-record pointer use `CreditsEndingRecord` indexing. Left raw: `msgA`/`msgB` reads (they share `offset` with the pre-switch code; indexing changes the register use), and the `R_credits_npc_info + off` reads in `credits_pose_evt` (both the `[idx + 5]` and `npc[idx]` forms broke the match, as the comment above that function warns). The `(u8*)credits_ending_records` pEntry/lwzu site is under MUST_MATCH.
- Left in evt_lect_hei.c: `(u8*)b + 8` / `ppos + 4` under MUST_MATCH. In batten_leader: the `(u8*)xmemo` byte stores and `(u8*)__memAlloc`/`work[]` casts that are not offsets.
- n9-q-s-mmnice: MUST_MATCH lines eff_nice.c 18 -> 2, eff_sandars.c 17 -> 6, eff_recovery.c 17 -> 7, eff_mahorn.c 17 -> 5. Each block was tested alone against the baseline object with tools/obj_equiv.py. Readable forms that matched and are now the only version: eff_nice effAcrobatDisp's plain loop (`[i - 1]` texid index, `max = 10.0f` before the loop; the old comment about texIdx was stale), eff_recovery Main2's duplicate `pos = recoveryZeroVec6()` branches, eff_sandars' duplicate `zero = 0.0f` branches. Shapes that became the only version with comments reworded: eff_nice niceEvtColor helper and the acrobatMain `position` copy local; eff_sandars effSandarsColorAnim/Disp2 helpers, the `scaleRate`-before-`scale` store order and the half-size reads through the 32 / 0.5 pool constants (the plain `-16.0f` differs); eff_recovery's k24/k05/transX locals in Disp, the negHalf/half locals and in-loop +-20 assignments in Disp3, the half/k05/k16 locals in Disp4, and the rcA by-value colour helper for the i == 1 branch; eff_mahorn's `position` copy local, the `t` growth local, the negEight local, and the mahornColorCall helper. Left under MUST_MATCH: eff_nice's `(const char*)(s32)sfx` integer round trip; eff_sandars' MATCH_VOLATILE read of dat_80421b9c, and the zero-vector literal copy in effSandarsMain with the vec3_80300138 data definition (data layout); eff_recovery's recoveryPoolPrefix (data layout), the volatile effect->userdata read in Main2, the volatile `saved` colour with the rcC helper in Disp4 (a plain local differs), and the opt_loop_invariants pragma pair; eff_mahorn's volatile float_0/float_1 declarations and definitions and the volatile-read `zero` loop in effMahornEntry. Natural (no -DMUST_MATCH) compiles of all four files succeed; full build recreates build/G8MJ01/ok.

## Raw-offset cleanup: mot_hammer, party_yoshi (2026-10-20, Claude Sonnet 5.5, tag s-offhammer)

- raw_offset 1014 -> 958; build byte-identical (MATCH), both files also compile with `--non-matching`.
- mario.h: `field_0x41[2]` split into `s8 field_0x41` / `s8 field_0x42` (size unchanged; mot_hammer and mot_jabara read both as s8; mot_jabara stays raw).
- mot_hammer: `M_S8(mario, 0x41/0x42)` -> `field_0x41/42`, `0x44` -> `currSubMotionId`, `0x48` -> `multiTimer`, `0x1FC` -> `hitobjHammer` (cast from `field_0x200`); `M_S8` removed.
- party_yoshi: `pl+0x98/0xA4` -> `dispOfs/dispOfs2`, `extra+0x7C` -> `dispOffset`, `mario+0x18C` -> `field_0x18C`; `M_P`/`P_P` offsets -> `hitobjFront/Push/Wall/Head`, `field_0x13C`; `M_F(0x98/0xA0/0xA8)` -> `dispOfs.x/.z`, `dispOfs2.y`; the `M_P`/`P_P` macros are gone.
- Left raw: all 0x2A0/0x2A4/0x2A8/0x2B4/0x2B8 sites (union scratch words that each motion types differently: s16/u16/s32/f32); `party->useStruct + 4` (untyped base).
### Raw-offset cleanup: eff_starstone, evt_yuuranking (gor), unit_boss_batten_leader, evt_lect_hei/gor (2026-10-20, Claude Sonnet 5.5, n9-q-s-offmix4)

No new names. eff_starstone.c reads the snow-dust effect's userdata as `effSnowDustData` (topR/topG/topB 0x4C-0x54, camId 0x68,
x/y/z 0x04-0x0C). gor evt_yuuranking.c's `YBASE_*` macros are gone: `A14/A74/AD4` index `yuwp->rankUsed/rankScore/rankMonte`
directly (the A14L/A74L/AD4L/A74F/A14R views stay raw: they exist to fold the add order). evt_lect_gor.c: the `effitemget*`
helpers use `effItemGetData` (camId 0x38, scaleMode 0x20; the scripts create "itemget" effects), the balloon helper uses
`EffMarioBalloonData.cameraId` (0x1C, effMarioBalloonEntry), the fukidashi helper `effFukidashiData.camId` (0x4C; script
creates it with evt_eff_fukidashi). Left raw: `(u8*)b + 8` / `(u8*)ppos + 4` in evt_lect_gor.c (MUST_MATCH position bases), end_unknown.c's `off`/`offset` forms and bom_01.c's padding-landing sites (match tricks).

## s-mmmix24: MUST_MATCH reduction (eff_water_damage, eff_pokopi_thunder, eff_peach_dust, eff_opuku_wprotect, eff_opuku_wgun N64)
Structural only, no names added. Made the MUST_MATCH shape the only version (guards and #else arms dropped):
water_damage everything (the colour inlines, the hoisted data++, the unused `color` local); pokopi_thunder the named
`zero` and the SetColor1 inline; peach_dust scaleColor/peachDustColor; wprotect everything (f14*6+f14, the three
colour inlines); wgun opukuWgunSin and opukuWgunColor1. Still guarded: pokopi_thunder SetColor2 (`*(GXColor*)&`
type-punned read), peach_dust scalePtr (a redundant alias pointer), wgun Color2 (type-punned read) and the dispatch
goto pair (the switch does not match). Block counts (lines naming MUST_MATCH) 30 -> 9. Build MATCH.

## s-mmmix22: sac_deka / tou2 / tou_02 / pik_02 / unit_barriern / unit_barriern_custom MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix22)

- MUST_MATCH lines (grep -c): `src/battle/sac/sac_deka.c` 7 -> 7, `rels/tou2/src/tou2.c` 7 -> 3, `rels/tou/src/tou_02.c` 7 -> 5, `rels/pik/src/pik_02.c` 7 -> 7, `rels/common/src/v2/unit/unit_barriern.c` 7 -> 5, `rels/aji/src/unit/unit_barriern_custom.c` 7 -> 6. No names added. The full tower build recreates build/G8MJ01/ok; the changed files compile without -DMUST_MATCH; fncast_lint passes.
- MUST_MATCH shape became the only version (comments reworded): tou2's `j = 1 + nAbove + i` and the `+= ... / 8.0f` scroll step; tou_02's `/ 2.0f` field_0x38 expression and the one-expression `rad`; unit_barriern's `args += 2; evtSetValue(event, args[-1], ...)` in barriern_wawawa_set_target; unit_barriern_custom's same form.
- Tried and rejected (object differs, .text 8 bytes smaller): replacing the `(p = BattleGetUnitPtr(...), p)` comma temporary in the barriern_satellite_zanzou/zanzou2 functions with a separate `p = ...;` statement. Removing sac_deka's one-iteration outer loop around the GX loop also fails the checksum.
- Left under MUST_MATCH: sac_deka's `.rodata` declspec Vecs, volatile `color` and the outer-loop trick; tou2's AREA_TOU string padding and the u16-view flags store; tou_02's volatile Vec consts and volatile stores, and the FAN_SFX integer cast; pik_02's `c50`/`half_sym` const-cast and int-punned userdata copies; unit_barriern's comma-temporary zanzou calls and volatile BARRIERN_SET_UNITID; unit_barriern_custom's comma-temporary zanzou calls and the volatile-table `barriernSatelliteTo_InitInline`.

## s-mmmix15: eff_cloud_n64.c / eff_akari_sflush_n64.c / eff_toiki.c / eff_starstone.c / eff_spark.c MUST_MATCH reduction (2026-10-20, Claude Sonnet 5.5, n9-q-s-mmmix15)

- MUST_MATCH lines (grep -c): `eff_cloud_n64.c` 9 -> 2, `eff_akari_sflush_n64.c` 9 -> 4, `eff_toiki.c` 9 -> 2, `eff_starstone.c` 9 -> 8, `eff_spark.c` 9 -> 7. No names added. Each block was checked against the baseline object with `tools/obj_equiv.py`; the full tower `ninja` recreates build/G8MJ01/ok, all five files compile without `-DMUST_MATCH`, and `tools/fncast_lint.py` passes.
- #else alone matched (only version): eff_cloud's `position2` copy (single `position` local), eff_starstone's `STARSTONE_DL_LOOP2` (plain `ctr` loop, no `(s32)` cast).
- MUST_MATCH shape became the only version: eff_cloud's `effCloudTevColor0/1` inlines and call sites (the `color` local went); eff_akari_sflush's `effAkariSflushColor1/2` inlines and calls; eff_toiki's `toikiSetColor` (the `*(GXColor*)&` cast dropped, `unk_80423338` is already a GXColor), `toikiDrawDL` and its call, and the guard that also wrapped effToikiEntry/effToikiMain (they had no #else arm); eff_spark's `maxF` split.
- Left guarded: eff_cloud's string padding and the unrolled `trailActive` zeroing (the loop is a different size); eff_akari_sflush's `position = disp` copy; eff_toiki's `(u32)` cast on `lbl_803B2FA0`; eff_starstone's `STARSTONE_DL_LOOP` pointer-walk, the `starStoneTevColor/2` calls (type-punned `*(GXColor*)&` loads of s32 data) and the `(u32)` base cast; eff_spark's volatile float externs and definitions, `sparkColor` (type-punned load of a u32).
### Raw-offset cleanup: battle_stage_object, win_badge, evt_mobj (Claude Sonnet 5.5, s-offmix2)
- battle_stage_object: the effNiceEntry userdata store at +0x1C is `((NiceEffectWork*)e->userdata)->scale`. evt_mobj fire_func writes `effFireData.scaleX`/`scaleY` (+0x78/+0x7C) instead of byte offsets. win_badge: file-local `WinEquipRec` (three s32, `field_0x0/4/8`) overlays WindowWork.field_404 for the equipped-badge list reads and stores in winBadgeMain; the scroll read is `work->badgeScrollPos[work->badgeTab]`; the unused WW_F32X macro is gone. Raw index 838 -> 828; build MATCH.
- Left raw: win_badge winMakeEquipList and winBadgeDisp's `u8* entry` walk (the pointer must start at `work`; rebasing it on field_404 moves the 0x404 displacement into an addi), `(s32*)((u8*)work + 0x3E4) + sel` (`&work->badgeSel[sel]` turns lwz into lwzx and the null test into cmplwi), WW_F32 at `work + idx*0x14 + 0xC4/0xC8` (WinRootRec lives in win_root.c, not a header), `WIN_BADGE_TEX`. battle_stage_object: 0x180C4 (Vec inside the field_180C0 band that battle_event_cmd and the rels index), 0x182A0 (void*/EffectEntry* here, s32 at `base - 0x7D60`), 0x180F8 (u8* here, s32 at `base - 0x7F08`), `battle_stage_nozzle_data`/`fall_object_data` byte arithmetic and the BattleNozzleWork cast. evt_mobj: the `vec + 0x7C/0x88/0x94` Vec reads (not multiples of sizeof(Vec)), the `jumptable + (type << 2)` reads and the `(u8*)` base aliases of the template vectors.
### Raw-offset cleanup: party, party_cloud, npcdrv, itemdrv (Claude Sonnet 5.5, s-offmix3)
- New members, all named by offset: `FieldBattleData` 0x54C band is now `u8 field_0x54C; u8 field_0x54D; u8 field_54E[2]` (every user stored or tested a 0/1 byte; the 0x54D read in the talk-name check and the `field_54C[0]/[1]` users in npcdrv use them); `NpcBattleInfo` 0x9C band is now `void* field_0x9C[4]` (npcGetBtlSetupWork/npcSetBtlSetupWork were `npc + 0x2CC` indexed, the only users). `PartyEntry.field_30` is `s8` (its only reader, partyUpdateKeyData, read it as a signed byte).
- Converted to existing fields: `mario->currMotionId` in compares (party.c partyHello, partyRideYoshiMain; party_cloud N__cloud_use), `mario->touchNpc` (npcdrv talk check), `mario->field_0x18C` (itemdrv), `((effItemGetData*)eff->userdata)->camId` (the two 0x38 stores), `wp->getEvent->lwData[8]/[9]` (the lecture event's item and effect words, 0xBC/0xC0). Raw index 838 -> 823; build MATCH, files compile without MUST_MATCH.
- Left raw: reads of 0x2E into a `u16` local (partyGoodbyeInit, getRidePos, cloud_use x2; the enum read adds a clrlwi and breaks the match); `PS8(player, 0x3E)` (mario_hit reads the same band as `field_0x3E[1]`, a header split needs mario_hit changed); `field_3C` 0x3D store in party_cloud (party_move reads the band as `field_3C[1..3]`); party_cloud's 0x17C/0x180 zero stores (the union members at those offsets are f32 or other-partner s32, a typed store changes the object or names the wrong partner); `partyPoseTbl`/`partyDotPoseTbl` flattened reads (documented register-allocation shape); `partyVecAt` (a Vec at 0x64 overlaps tiltX/spinAngle); the `FieldBattleInfo` at `fbatData + 0x20` (overlaps `ruleKeepResult` at 0x39, shared with evt_npc/seq_battle); `npcRollBattleParams` (`audienceRanges` typed access gives npcSetupBattleInfo 99.6%); `stolenItems` byte-step loop; MUST_MATCH `ENC(&info, s32, 0)` and `item + 0x38` volatile read; itemdrv `itemDataTable + 0x20` u16 view (documented, header icon_id is s16).
- s-mmmix27 (MUST_MATCH reduction: battle_status_effect, ac_power_gauge_lv2, ac_power_gauge_lv, eff_kemutest): MUST_MATCH
  lines 6/6/6/6 -> 4/5/2/6. No names added. Only versions: lv2's `rate`/`drain` (the #else value-first form matches alone),
  lv's `ac` intermediate in battleAcMain_PowerGaugeLv and actionCommandDisp (the #else fails; comments reworded),
  battle_status_effect's `type` local for the priority byte (comment reworded). Guarded: the three `type + 0` dead
  additions (#else is 4 bytes smaller), the function-pointer casts (windowDispGX_Waku_col, BattleActionCommandGetDifficulty,
  iconDispGx, BattleAcDrawGauge), lv2/lv volatile reads, the dat_ .sdata2 declspec definitions, kemutest's sdata/gap_ block,
  padded "KemuTest" string, `spd = 0.0f` dead store, and the `dirptr = dirptr + 0` self-add (the plain form changes
  the .sdata relocations). Checked with tools/obj_equiv.py against the baseline objects; full build recreates build/G8MJ01/ok.

### Raw-offset cleanup: battle_seq_command, battle_seq, battle_unit, battle_mario (2026-10-20, Claude Sonnet 5.5, n9-q-s-offbseq2)

raw_offset 876 -> 851. Build byte-identical (build/G8MJ01/ok recreated); the changed files and the header's other users compile without `-DMUST_MATCH`.
- Existing names only: `part->field_0x1A2`/`field_0x1A4`; `unit->field_0xB4`.
- New members, named by offset, inside existing BattleUnit/BattleWorkUnitPart opaque bands (sizes unchanged), each read as one type by every user: BattleUnit `s32 field_0x28` (was the `field_0x27` array's +1; the battle_seq sort), `s32 field_0x258`, `s32 field_0x27C`/`field_0x280`, `s32 field_0x2A8` (the `*(s32*)unit->field_0x2A8` users in battle_disp, battle_event_cmd, battle_seq, battle_seq_command now read the member), `s32 field_0x2C8`, `Vec field_0xB4`; part `s16 field_0x1A2`/`field_0x1A4`. `EventEntry.field_0x90[0/1]` for battle_mario's tornado effect and sound handles.
- Left raw: the `BtlActRec_AddCount/AddPoint((u8*)wp + 0x16F..)` sites and battle_mario's `record` (battle_actrecord.c walks `field_16F1C` as one array, so the band can't be split into members); BattleUnit 0x2A0 (s32 in battle_event_cmd, void* in battle_seq), 0x2C4 (void* in battle_seq, s32 in unit_lecture_frankli_sac), 0x2CC (void* in battle_damage, s32 in battle_seq); part 0x4FC (u8 in battle_unit, s8 in battle_disp); `unit->mKindParams` +0x14 (the field is s16, read as u16); BattleWaitAllActiveEvtEnd's `p - 0x7dXX` reads (the field_182XX members exist but using them changes the object); `(u8*)wp->alliances + i * sizeof` (`&wp->alliances[i]` changes the object); BtlUnit_SetParamToPouch's `pouch + party * 0xE + 6` (`partyData[party].currentHP` changes the object); `wp + 0x120` inside MUST_MATCH; the `effect->userdata`/EffectEntry userdata +4/+8/+0xC sites (void* userdata with no struct); battle_seq_command's `(u8*)&wp->sampling` and `(u8*)sampling` arguments (BattleSamplingEnemy takes u8*).

## s-offtail2: raw-offset cleanup (item data, sac_scissor/muki, vivian, koopa, event_default, damage, audience_kinopio)
- raw_offset 723 -> 694 (tree); build byte-identical (MATCH); every changed C file also compiles without `-DMUST_MATCH`.
- battle_unit.h: `field_0x27C[8]` split into `s32 field_0x27C` / `s32 field_0x280` (battle_damage, battle_seq and battle_event_default all read both as s32; their `*(s32*)` casts and `+ 4` are gone).
- mario.h: `field_0x14[4]` -> `u32 field_0x14` (koopa.c and mario_hit.c read it as u32; the `*(u32*)` cast in mario_hit is gone). Five koopa.c `MW_S32(.., 0x14)` sites read it as s32 and stay raw.
- battle_audience.h: `field_0xEC[0xC]` split into `BattleUnit* field_0xEC`, `s16 field_0xF0`, `u8 field_0xF2[2]`, `s32 field_0xF4` (every user reads each offset as one type); kinopio and battle_audience updated.
- battle_damage: `_getSickStatusRate` reads the named BattleWeapon chance/strength fields instead of `w[0x..]`; `weapon->baseAccuracy`; `vulnStatus->galeForceRate`; `BattleAttackDeclareAll` uses `BattleSampling` (`count`, `order`, `entries`, `selectIdx`); unused `BU_*` macros removed.
- sac_scissor/sac_muki: work pointers via `&_battleWorkPointer->actionCommands.field_0x2BC[0]`; muki `points[i]`, `entries[i]` (non-MUST_MATCH branch).
- battle_item_data: `&((ThrowItemWork*)event->userdata[0])->pos`; vivian: `userdataEx[1]` for the 0x8C word, `mWeaponParams->name`; event_default: `mUnitKindParams->field_0x13`.
- Left raw: item_data `wp + 0x16f25..28` (act-record byte band indexed as an array by battle_actrecord and passed to BtlActRec_*); scissor/muki variable-offset `triBuf`/`points` strides (void* buffers); muki MUST_MATCH `off & mask` site; vivian `area + 0x90` (untyped extra work) and the `mWeaponParams + 4` u16 read (field is s16); unit `0x2CC` (void* in battle_damage, s32 in battle_seq); `wp->field_180C0 + 0x1E0`; `&weapon->field_0x6D` u8 views of an s8 field.

### Raw-offset cleanup: tail files (2026-10-20, Claude Sonnet 5.5, n9-q-s-offtail1)

raw_offset 723 -> 677, build byte-identical, the changed files also compile without `-DMUST_MATCH`. No new names; every new member is `field_0xNN` (or `pad_0xNN`).
- eff_torch: `effTorchData` gains `field_0x10/0x14/0x18` (f32, written and read as f32 only) and `field_0x1C` (u8), size 0x20 as allocated.
- mario_kemuri: `effKemuriData` moved from eff_kemuri.c to eff_kemuri.h so the `0x74` write is `->scale` (same for `effKemuri2Data.scale`); `PartyEntry.field_2C` (party.h) is now `s8` (its only user, mario_kemuri, reads it as s8).
- eff_fire_dust_n64, eff_broken_barrier_n64: the vertex slices are `&size8x8_tex22x22_vtx[N * 14]`, the form eff_stardust_n64 already uses (14 words = 0x38 bytes per slice).
- gor_03: `SlotData` fields replace the `u8*`/`s32*` offsets in the slot accessors, `goryuugijo_init_slot`, `door_init_slot`, `slot_neon_sub` (its `data` parameter is now `SlotData*`, gor_03.h updated), `goryuugijo_disp_slot`. A file-local `Monte` (0x50 bytes, `field_0xNN` members) types the monte_p entries for `monte_disp`, the `fall_rtn` update loop and the free path.
- win_party: `field_19C` (u8[4]) in WindowWork is `s32 field_0x19C` (win_party writes 0/1 to it; no other reader); `partyPose[i]` in winPartyExit and `WIN_PARTY_POSE`; `WIN_PARTY_TEX` via `winTpl->data`; the move description via `BattleWeapon::description`; `((f32*)anim->clip)[2]`. win_party's `WW_*` macros are gone.
- win_log: `LOG_TEXPAL` via `winTpl->data`; `rows[sel].name`; `AnimData` gets `field_0x4C` (f32, was u8[4]) and `field_0x58` (f32), so the tattle-camera bounds read `ad->field_0x44..0x58` in both the MUST_MATCH and #else branches (AnimData is 0x5C bytes now; nothing takes its sizeof).
- Left raw: seq_load's 7 `card + off + 0x2000` reads (`saves[wp->cursor].data.flags` for the three cursor sites changed the dol; the other four step a byte offset in a register-critical loop variable); win_log's `p`/`ep` pointer walks over `rows`/`entries` (the indexed form changed winLogMain and winLogMain2's allocation) and the `0xC4/0xC8` window-position reads (a `WinRootRec` view gave winLogDisp and winPartyDisp different register choices; same in win_party); gor_03's init-loop `(u8*)monte_p[0] + i * 0x50 + 8` base (the entry+8 biased pointer is what the original holds), the `upsidegara` u32-table slices, and `kuji_bomb_055_callback`'s `param[0]` (unknown object); mario_kemuri none left.

### Structure: private structs into shared headers, overlays folded (2026-10-20, Claude Opus 5.5, t-o-hdrs)

Structs that lived in one .c file while other files reached the same memory through byte offsets or their own
overlay structs now live in headers, and the raw sites that depended on them are field accesses. No new names:
moved structs keep their names, new members are `field_0xNN` or take the name the overlay already used. Build
byte-identical after every commit; all 1,230 objects also compile without `-DMUST_MATCH` (a full `--non-matching`
build, not only the changed files, since shared headers changed).

- **WindowWork (win_main.h).** WinRootTab and WinRootRec moved in from win_root.c. The 0x40 band is now a union of
  the `field_40` byte array (still needed: sort_7_2/7_3's `&field_40[0x1010]` base is the only spelling that
  matches; `LOGWIN(work)->chars` there changes the object, while sort_7_1 takes `chars` fine) and
  `tabSel`/`tabCount`/`tabs[5]`/`recs[5]`. The `work + idx*0x14 + 0xC4/0xC8` window-position reads in win_badge,
  win_item, win_log, win_mario and win_party are `work->recs[idx].b/.c` and match. The earlier failure (t-offtail1)
  came from a cast view, not from the header field. WinRoot's and WinItemWork's offset-named members (0x128-0x184,
  0x224-0x264, 0x2C8-0x2D0) replace WindowWork's opaque bands. Every user reads each offset with one type.
  0x13C/0x140/0x144 are `const char*`: winMsgEntry's `str`, its `a3`, and the last `str`. `a3` was declared s32,
  but win_log's callers already passed strings through `(s32)` casts, and winMsgDisp passes it to msgSearch. So
  the prototype's `a3` is `const char*` now, the casts are gone, and callers pass NULL for none. WinEquipRec (win_badge)
  moved in, and `field_404` is `WinEquipRec[200]` (0x404-0xD64). The WinRoot, WinItemWork and SortRec overlays and
  the WW_*/WM_* offset macros in win_root, win_item, win_badge and win_main are gone.
- **LogWin (win_log.h).** LogWin, LogRow and LogChar moved from win_log.c to win_log.h. win_root reads the
  card/badge/tattle counts, selections, pages and icon arrays through `LOGWIN(work)` instead of its own
  `field_0xE94..0x1044` members. WinRoot's `field_0xEB8[0x6C]`/`field_0xFB4[0x42]` were sized to fill gaps; LogWin's
  `cardIcon[100]`/`badgeIcon[58]` are the real sizes, and every win_root index is a runtime one. LogWin stays an
  overlay rather than moving into WindowWork, because its names collide there (`badgeSel` is win_badge's s32[2] at
  0x3E4) and its `state`/`sel`/`count` would read wrongly in the flat struct.
- **win_root codegen that now matches** (each pinned before): winRootMain's 13
  `((WinRootTab*)((u8*)tabs + tabSel*0x18))` reads as `work->tabs[work->tabSel]`/`recs[...]`, and its
  `#pragma opt_common_subs off` is no longer needed (same object without it); winBgMain's `WindowWork*` walk as
  `work->recs[i]`; sort_4_3_func and sort_5_3_func index `field_404[i]` and `cardIcon[i]` instead of a byte base (a
  mixed form, indexed reads with byte-base swaps, does not match); winMakeEquipList fills `field_404[count]`; the
  N_compare_func4_* comparators read `WinEquipRec.field_0x8`; the `WIN_*_TEX` macros read `winTpl->data`.
- **BattleBreakSlot / BattleBreakSlotReel** to battle_break_slot.h; BattleWork's 0x1616C band (exactly 0x134) is
  `BattleBreakSlot field_1616C` and BattleBreakSlotGetPtr returns its address; battle_audience reads `->field_14`,
  the las black peach and gor lecture user funcs set `->flags`.
- **Others moved to their header:** WinMailEntry (mail_data.h; win_item's mail body/flags reads; mail_data.c keeps
  MailEntry with a u32 at 0x0 for the definitions, so data is untouched); Footmark (party_move.h; party_motion's
  `u8* fmark` reads); UnitWinWork/UnitWinObj (evt_win.h; gor_01's partial copy gone); AcTimingADecWork (battle_ac.h;
  btlevtcmd_ac_timing_flag_onoff); effBreathFireData, effScanningData, effKamekiTornadeData (the aji drill tornado
  wrote it through BattenData), RecoveryData/RecoveryMayaAnim, EffAkariChargeWork (replaces the partial
  EffAkariChargeData view).
- **Overlays of header structs folded:** MapTevCallback (mapdrv.h) replaces eff_mahojin.h's TevCallbackParam,
  unit_mario's copy of it, battle_stage's ScreenTevParam and sac_suki's SukiTevParam in the dol and 13 RELs
  (field_C/10/14/18 are tevStage/texMap/texCoord/numIndStage, as mapdrv fills them); battle_status_effect's three
  BattleUnit/BattleUnitKind views are gone (BattleUnit gains `mKagegakureIcon` at 0x330 and `mStatusChange[6]` at
  0xAE4, the views' names); party_kuribo's PartyAppearPos is PartyEntry.targetPos.
- **Left, with reasons:** win_log's six row-phase `p = (u8*)work` loops (indexed `rows[i].phase` changes the
  object), its WinLogWork/WinLogEntry view (a 4-byte-shifted view of LogWin.rows, inside win_log only) and the
  `entry + 0xD78` walk; winBadgeDisp's `entry` walk (indexed changes the object); `&work->badgeSel[sel]` and win_item's
  8 itemSel sites (as before); BakuGameGaugeColor (BattleACDispParams.field_0xC is stored as s32 by
  ac_power_gauge and read as bytes by battle_ac and sac_bakugame); NanNpcWork/NanNpcEntry (muj_10.h and gor_00.h
  give 0x68-0x7C per-map meanings that differ from evt_nannpc's); evt_msg's EventVec (a Vec over EventEntry's
  `unk184`/`unk18C`, which evtmgr copies as an EventEntryMsg plus a u32); bom_01's BomSoundView (the comment there
  says sound.h conflicts with the unit's headers). Already-in-header effect userdata writes (evt_lect_*, tik_02,
  npc_event, sac_suki, tou_07/08, kamec_green, party_bomhei) are a separate pass.
- `tools/ci.py` was run with its unit-test step stubbed: in this worktree `tools/tests` fails 9 tests that don't
  touch the build (test_reloc_value_check cannot import elftools under system python; test_wave_launch times out
  on systemd scopes). Every other step passed for each commit.
- Index (`tools/cleanup_index.py`): 17.181 -> 16.970 per kLOC (12137 -> 11986 weighted); raw_offset 587 -> 540,
  field_0x 3701 -> 3646, opt_pragma 1459 -> 1457. win_root.c 341 -> 223 weighted (raw 20 -> 0), win_item.c 201
  -> 176, win_badge.c 41 -> 31; win_main.h 2 -> 40 (the offset-named members now in the header).
### Effect userdata through effect data structs (2026-10-20, Claude Opus 5.5, n9-q-o-effud)

`tools/cleanup_index.py` before (67430c55) and after: index 17.181 -> 17.051 per kLOC, raw_offset 587 -> 540, field_0x
3701 -> 3702 (tik_02 now writes `EffRippleData.field_0x14`), fn_ptr_cast 253 -> 254 (gor_01's starstone draw callback
is cast to `MahojinWork.drawFunc`'s type, as tik_05 already does). Build byte-identical after every commit; the whole
tree also compiles without `-DMUST_MATCH` (full `--non-matching` ninja, only the link fails as usual). `tools/ci.py`'s
own `tools/tests` step fails in this worktree for environment reasons (`elftools` not installed; the wave_launch tests
time out on systemd scopes), so CI was run with that step stubbed out; every other step passed.
- Rule used: a site is typed only where the effect is proven: an `effXxxEntry` in the same function, a work slot that
  only that Entry fills, or a name the same file's scripts create with `evt_eff`/`evt_eff64` (the set name maps to the
  Entry through `effdrv.c`/`eff_n64.c` and evt_eff's switch). No new member names.
- Structs moved unchanged from the effect .c to its header: `effBreathFireData`, `effKemuri10Data`, `RecoveryData`
  (with `RecoveryMayaAnim`), `effKamekiTornadeData`, `effScanningData`, `EffThunderflareData`, `effKemuri12Data`.
- **#1078 had reverted #1075 in two files.** The hei lecture helpers (itemget camId/scaleMode, balloon cameraId,
  fukidashi camId) and batten_leader's two Syuryou sites were back to byte offsets on main although the #1075 chapter
  above says they were converted; both are reapplied. The #1078 merge's evt_lect_hei/batten_leader hunks are exactly
  the reverse of #1075's.
- Lecture helpers (hei, muj, tou, mri, usu, gon): `effitemgetchange_cam`/`effitemget_open` write `effItemGetData`
  (`camId`, `scaleMode`), `efffukidashichange_cam` writes `effFukidashiData.camId`. The scripts that call them create
  `sub_bg` as "itemget" and `fuki` with evt_eff_fukidashi. The `_ptr` variants get `LW(9)` of `all_juyoitem_lecture`,
  which itemdrv sets to `wp->getEffect`, and only `effItemGetEntry` fills that. In mri, gon and muj (fukidashi) the
  named variants have no caller in the file: they are the same local functions, by original symbol name and body, as
  the lecture module in the other RELs.
- dol: npc_event (fire `scaleX`/`scaleY`/`alpha`, magic1 `timer` for the kamec fire magic name), evt_eff (breath fire
  `scale`/`baseScale`/`endScale`), evt_npc (kemuri10 `camId`), party_bomhei (sandars `camId`), win_main and sac_genki
  (recovery `field_40` and `x/y/z`), sac_suki (crystal `camId`), battle_mario (kameki tornade `x/y/z`, fire
  `position`), party_yoshi (sweat `position`).
- RELs: tik_02 (ripple_n64 `field_0x14`, hokori_n64 `tevReg0R/G/B`), dmo_00 (crystal and radiation `camId`), gor_01
  (`MahojinWork[i + 2].drawArg/drawFunc` replaces the `offset + 0x88/0x8C` counter, which is gone; `animId[0]`), muj and
  muj_10 (`effKamekiTornadeData.alphaScale`; muj_10's `tornade_alpha_func` is uncalled, typed as muj.c's copy),
  tou_07/tou_08 (starstone `camId`), tik_05 (crystal, treasure map and pokopi charge fields), tou_00 (kemuri12
  `position.x`), rampell jin/las (scanning `scale`), magnum_battender (hokori TEV colours), mri_puni (fukidashi
  `position`), kamec red/green/obaba/tik (thunderflare, pokopi charge and magic1 fields instead of `data[0xNN / 4]`
  and `data + 0xNN`), both dokugassun copies (`effVaporN64Data` colours and scale).
- Fix: kamec_white read its thunderflare effect through `ChargeEffectData` (same 0x48 layout, a different effect); it
  now uses `EffThunderflareData`.
- Left raw, not effect data: battle_stage_object's `event->userdata[2]` argument, unit_boss_koopa_tou's
  `event->userdata[0] + 0xC` (event work word), msgdrv's `window->userdata`, sac_genki's `recover_cmd_disable` (BattleWork
  offset). Open: `effRippleN64Data` in eff_ripple_n64.h is a partial copy of `EffRippleData` (rx/rz at 0x2C/0x30) and
  could be dropped for it; mri_puni's `PuniData.effect` is an `s32` that every user casts to `EffectEntry*`.
### Readability: function-pointer casts removed by fixing prototypes (2026-10-20, Claude Opus 5.5, n9-q-o-fncast)

Prototypes changed so callers call plainly; every change keeps the linked build byte-identical (build/G8MJ01/ok), every
changed C file compiles without `-DMUST_MATCH` (and a full `--non-matching` `all_source` compile is clean),
`tools/fncast_lint.py` and `tools/local_prototypes.py` pass. Header, definition and every caller changed together.

- Counts, `tools/cleanup_index.py` on 67430c55 vs this branch: index 17.181 -> 16.718 per kLOC (12137 -> 11802
  weighted); fn_ptr_cast 253 -> 132; must_match 1388 -> 1300; field_0x 3701 -> 3699; address_name 2690 -> 2687; the
  rest unchanged. Calls through a cast of a named function (`((T (*)(...))name)`): 194 -> 80.
  `tools/fncast_allowlist.json` 38 -> 4 entries (34 stale, about 15 of them already stale before this branch).
- What MWCC showed while doing this (useful for the next prototype fix):
  - Arguments are evaluated in the declared parameter order, so a cast that only moves the floats is a real
    prototype fact. Where the callers' casts disagreed, one order usually still served every caller: test it by
    rewriting all calls and building, not by reasoning.
  - Widening: an s16 local or s16 return passed to an s16 parameter gets no `extsh`; passed to an s32 parameter it
    does (ac_button_down, ac_stick_rotate). An s32 passed to a u16 parameter gets a `clrlwi`. An explicit `(u16)x`
    argument to an s32 parameter is scheduled earlier than the conversion a u16 parameter does at the call
    (evt_lect_mri, ac_lst_rc_keep), so it is not a substitute.
  - A caller that copies a struct into a local and passes `&local` (a by-pointer cast) sometimes matches a plain
    by-value call of the source (marioGetScreenPos), and sometimes the copy lands in another slot (winmgr's
    GXSetFog, battle_seq's windowDispGX_Waku_col, sac_genki, mri's mapGrpSetColor).
- New prototypes (old names kept; `unused` follows btlDispTexPlane2's existing parameter):
  - iconDispGx(f32 scale, Vec* position, s16 flags, s32 iconId) and iconDispGxAlpha(..., s32 iconId, ...): iconDispGx
    hands its id register to iconDispGxAlpha untouched and Alpha zero-extends it itself. battle_disp's local u16
    prototype is gone. _get_icon_id(s8 id) returns s32; three s16 icon locals that only hold constants
    (ac_button_down, ac_timing_wide, sac_bakugame) are s32 so the plain call stays unextended. iconDispGx2 takes a
    u16 id (evt_window's icon local u16).
  - btlDispTexPlane(f32 tX, f32 tY, f32 tZ, f32 sX, f32 sY, u32 id, GXColor color, s32 unused); btlDispTexPlane3 gains
    a trailing `s32 unused`: every caller sets r5/r9 = 0 and the callees ignore it.
  - windowDispGX_Waku_col(f32 x, f32 y, f32 width, f32 height, f32 curve, u16 gxTexMapID, GXColor color);
    FontDrawString(f32 x, f32 y, const char* str); intpl_sub(mode, start, end, currStep, steps) (battle_sub.c defines
    it behind `#undef`, like system.c's intplGetValue); BattleAcDrawGauge(x, y, ratio, type, count, t0, t1, t2,
    flags); animPoseDisp_MakeExtTexture(name, animName, f1, texObj, base, f2, out0..3); nameWinGX(x, y, z, w, tex,
    arg1); effPointGetEntry(type, x, y, z, value, wait); effRotHammerSuccessN64Entry(type, x, y, z, count, life). All
    65 Waku_col and 76 FontDrawString calls were reordered; gor_irai fetches its second label into `name` first so
    the string stays ahead of the 127.0f constant in .rodata.
  - badgeShop_add(u8*, s16 id, s32); BtlIcon_Entry(u16 iconId, ...); SoundEfxSetPitch(s32, s16 pitch) with psndSFX_pit's
    definition s16 too; evtEntry(void*, s32 priority, s32 flags) (all 196 calls unchanged); windowEntry(s32);
    pouchSetPartyColor(PartyId, s32); pouchGetFP/pouchGetAP/pouchGetPartyHP return s32, pouchSetPartyHP/pouchAddAP
    take s32 (the bodies are lha/sth either way); mapPreLoad(const char* map); winBadgeExit(WindowWork* work);
    BattleAudience_Case_ActionCommandGood(BattleWeapon* weapon) (as ..._ActionCommandBad);
    BattleAudience_Case_EnemyDamage(s32 damage); animPoseSetPaperAnim(..., BOOL unused); tri1(s32 x, s32 y, s32 z,
    s32 unused) (as tri2); effHagetakaFlushN64Entry/effBoobooN64Entry(type, unused, x, y, z, scale) (itemdrv.h's
    conflicting duplicate declaration removed).
  - Plain already: intplGetValue's and BattleActionCommandGetDifficulty's casts named the declared type; the
    forwarding helpers that held them (intplGetValueMain/FM, intpl_f, pakkun_small_intpl, ICON_DISP, GET_ICON_ID,
    BattleAcDrawGaugeFM, tri1WithUnused, POUCH_*) are gone.
- Left, with why (callers need different ABIs, or it is not a prototype question):
  - iconDispGx (6): evt_win (`iconDispGxS`: flags and id converted as u16), both boss koopa units (u16 id and the
    scale evaluated last; scale-last breaks kpa, statuswindow and battle_disp), ac_lst_rc_keep, ac_stick_keep_left and
    ac_signal_timing's ICON_DISP_B (u16 conversion at the call). iconDispGxAlpha (4): the mri/eki/tik/usu lecture
    units (u16 at the call; the other lecture units already declare a local u16 prototype, which is the
    alternative).
  - iconDispGxCol (6): battle_menu_disp's u16 `b` (a u16 parameter changes the callee); statuswindow, sac_genki and
    both evt_yuuranking pass their own colour local by pointer. iconNumberDispGx/iconNumberDispGx3D (7) and
    windowDispGX_Waku_col (6): the same by-pointer colour locals. winFontSet*/winIconSet (20): those units copy by
    value where every other caller passes a pointer (the FontDrawColor situation).
  - BtlUnit_CheckStatus (7), BtlUnit_GetStatus (6), BtlUnit_SetStatus (1): the definitions pass `type` on
    unextended (an s8 parameter changes CheckStatus/GetStatus/CheckRecoveryStatus/SetStatus), battle_status_icon,
    battle_disp and battle_damage sign-extend it; an explicit `(s8)type` argument schedules differently.
  - btlDispGXQuads2D (3): the callee takes a u8 alpha (an s32 one adds clrlwi to it); battle_menu_disp passes it
    unextended. envGlareFilter (1): s16 parameters change the callee.
  - GXSetFog (5), GXSetChanMatColor (2): SDK prototypes, by-pointer colour. HSV2RGB (1): sac_scissor's cast returns a
    struct. FontDrawColor (1): documented exception. moamoa_pose_change/main (2): the cast only stops inlining.
    system.c's `mode` (1): an int used as a function pointer.
  - Not calls (the rest of the index's fn_ptr_cast): qqsort comparator casts, `evtGetValue` results used as function
    pointers, struct fields typed `void*` that hold callbacks.
- tools/ edits (run with `--allow-protected`): local_prototype_exceptions.json's iconDispGx/iconDispGxAlpha reasons
  describe the s32 definition; fncast_allowlist.json pruned. `tools/ci.py`'s tests step fails in this worktree
  (test_wave_launch timeouts, no pyelftools for system python), unrelated to the source; every other ci.py step was run
  and passed before each commit (tests skipped, under `uv run --with pyelftools`).
### Readability: MUST_MATCH blocks in rels/ (2026-10-20, Claude Opus 5.5, n9-q-o-mmrels)

The battle pass's MUST_MATCH work, over the chapter and map RELs. Build byte-identical after every commit
(build/G8MJ01/ok); every changed C file compiles without `-DMUST_MATCH` (`configure.py --non-matching`, only the dol
link fails as usual); `tools/fncast_lint.py` and `tools/local_prototypes.py` pass; `tools/ci.py` passed before each
commit with its `tools/tests` step stubbed (the same 9 environment failures as the chapters above). No names added.

- Counts (`tools/cleanup_index.py`, e2103891 -> this branch): index 16.392 -> 16.291 per kLOC (11570 -> 11493
  weighted); must_match 1300 -> 1226, all of it in rels/ (rels/ 232 -> 158 sites); raw_offset 500
  -> 499; everything else unchanged. `grep -c MUST_MATCH` over rels/ 276 -> 189 lines.
- Method: every block's non-MUST_MATCH arm was compiled alone, and all of a file's at once, against the baseline
  object (`tools/obj_equiv.py`); then each remaining block was read and either its MUST_MATCH arm kept as the only
  version (when it is ordinary C) or a new natural form tried.
- Natural forms that match now (mostly because #1091 fixed intplGetValue's prototype): unit_barriern v1's
  volatile-comma intplGetValue calls (both zanzou subs); magnum_battender_mkII aud_suikomi's volatile arguments;
  muj_00's volatile step-count reads (the sz/ez/steps locals went too, the fields go straight into the call); one of
  gor_00 gundan1000tai_init's two `tmp` products. kpa_status_disp's icon id is a `u16` local, which gives the clrlwi
  with no cast. `PuniData.prevMode` (mri_puni.h) is `u16`: every read in mri_puni went through a `*(u16*)&` view, the
  stores match either way, and it holds a PuniMode like the u16 `mode`; the puns and two blocks are gone. Dead code:
  v1's uncalled barriernSatelliteTo_InitInline and its `#undef` guard. tou_03 screen_tev: buf/i/j/k are declared at
  function scope in both builds, so only the `zero` alias stays guarded.
- Same shape in every copy: unit_barriern v1 and z use the `args += 2; args[-1]` form that v2 and aji's custom copy
  already had (barriern_wawawa_set_target).
- MUST_MATCH arm kept as the only version (plain C: a named temp or constant local, operand order, a returning colour
  inline, a one-case switch, split declarations): aji_14 (`clear2`, a second base as in the first block), aji_17,
  magnum_battender_mkII aud_suikomi_fade, eki_02, end_unknown (two; its stale NONMATCHING note went), gon_03 (`half`;
  the `#undef sfxLanding` is unconditional now), gor_00/muj_00 `alphaColor`, gor_01, gor_10 (four), gra_00,
  unit_lecture_frankli_sac `spot_square_color`, hei_00 (two), hom_00 (`* 60 * 60`), jon (the GX quad sizes), las
  `mobjColor`, las_29, evt_shuryolight (three), unit_boss_koopa (`/ 2.0f`), punidrv (five: `k`, the while loop, `p`),
  tik_00, tik_05 (two colour inlines; its sqrtf tail stays guarded, as the `#else` is MSL's sqrtf), tou `monitorColor`, evt_lect_usu, usu_00 (three), evt_yuucounter, evt_yuutitle, and the `WAKU_BOTTOM` operand order in both
  evt_yuuranking copies (split out of the guarded fn-cast macros). las_05's `(s8)(1 - footstepToggle)` store is
  unguarded too: the dol's evt_npc.c, which the REL copies, writes it unguarded twice.
- Left, data layout (not touched in this pass): the `"J\0\0"` strings (rsh_00, rsh_06, eki_00), tou2's AREA_TOU
  padding, the `.rodata` declspec Vecs (rsh_06, yuu_02, usu_00, aji_13, magnum_battender_mkII), tou_02's `const
  volatile` Vec definitions, rsh.c's Zero256 object, unit_piders' out-of-line GX helpers.
- Left, with what was tried:
  - moamoa pvec rebuild (rsh_05, rsh_06, eki_00) and eki_06 sw_entry: the volatile keeps the const Vec's three loads
    below the preceding stores. Plain stores with the existing inline, and a returning inline built from the base
    (eki_06), both hoist the loads above the stores. The original reloads the way a non-const object would, so only
    a data-declaration change could do it. Same family: rsh_06's volatile `offset` read, muj_01's colour read.
  - sfx string addresses (unions in rsh_05, rsh_06, eki_00, hei_10, yuu_00, yuu_01, yuu_02; `(u32)` round-trip macros
    in las_05, bom_01, tou_02; comma folds in gor_03 and zako_group2). Tried: the literal at the call in hei_10 (the
    address is not hoisted and the string's .rodata slot moves) and function-scope locals set before the loop in
    las_05 (the copies get their own `mr` and the callee-saved registers shift).
  - elapsed-time intplGetValue (value divided into r4, compared, copied to a callee-saved register at the first
    call): mri.c (assignment inside the argument plus opt_propagation), mri_puni (opt_common_subs; its arm without
    the pragma and the plain form both divide straight into r28), win_00 (u32 round trip), muj_12 ((s32) casts).
  - barriern zanzou `(p = BattleGetUnitPtr(...), p)` (v1, v2, z, custom): a plain `p = ...;` statement moves the call
    ahead of the scale struct copy; a returning inline gets the order right but drops the `mr r0,r3; mr r4,r0` temp
    copy, with or without a local inside it. The volatile table parameters of InitInside/InitInlineOut (v1, z,
    custom) and v2's BARRIERN_SET_UNITID were not retried.
  - Call-site ABI casts: iconDispGx in las and tou2 boss koopa (a u16 local and a `(u16)` argument both move the
    1.0f load ahead), iconDispGxAlpha in the eki/tik/usu lectures (a `(u16)` argument changes registers and
    schedule), envGlareFilter (eki_03), mapGrpSetColor (mri.c), the by-pointer colour macros in both evt_yuuranking.
  - The lecture `ppos` base (`&lbtn[i] + 8`, loads at +4) in nine lecture files: the natural form needs a nested
    struct at 0x8, which would need invented names.
  - Redundant or dead code that reproduces the original: gor_03's dead OR terms, usu_00's self-assignment, las_05's
    dead sqrtf, frankli_sac's `id = 0`, gor_irai's self-cast, rsh_00's `| dm` and one-iteration loop, jin_00's f64
    round trip, win_04's u64 temp, mri_puni's `-(s32)(u32)v`, pik_02's puns and its c50/half_sym constants, tou's
    TOU_F32, tou2's u16 view, yuu_02/yuu_03's raw-offset fill inlines, yuu evt_yuuranking's raw-offset accessors,
    gor evt_yuuranking's asm body, tou's `defer_codegen` pragma, muj_20's add inline (`pos.x + 85.0f` swaps f0/f1),
    gon_03's sfx array.
  - Contrived copy chains kept as hacks rather than promoted: eki_03, hom_00 and jon's `q = q * 32; row = q`, and
    gor_03's `t0`/`t1` copies.

## mmbattle: original source shapes for src/battle MUST_MATCH blocks (2026-10-20, Claude Opus 5.5, n9-q-o-mmbattle)

Pass over the 231 `#if*MUST_MATCH` regions in src/battle/** looking for the source shape that gives the same object
without steering. Six Opus 5.5 sub-agents took disjoint file groups (ac1, ac2, audience, misc, seq, sac). They
compiled single units into scratch dirs and compared with tools/obj_equiv.py against a saved baseline. Then the
orchestrator (Opus 5.5) reviewed each diff and made the header changes. Every commit passed ci.py's lint, build,
natural-compile, data and ratchet steps; the tests step was skipped because it times out in worktrees, as in the
chapter above. Each header change was also checked with `obj_equiv.py --tree` over all 1,230 compiled objects (src and
rels). The full `--non-matching` ninja compiles every object, and only the link fails, as usual.

- **Counts:** src/battle regions 231 -> 145. `tools/cleanup_index.py` 16.293 -> 16.121 per kLOC on top of the rels
  pass; must_match 1226 -> 1140, fn_ptr_cast 131 -> 115.
- **Header types from the asm** (each field has a single reader; all callers in the tree are equivalent):
  - `BtlUnit_GetStatus`/`SetStatus`/`CheckRecoveryStatus`/`CheckStatus` take `s8 type`. The bodies switched on
    `(s8)type`, and the battle_status_icon/disp/damage callers sign-extend at the call. With all four changed together
    and the body casts dropped, the callees are identical. This supersedes the chapter above, which changed them one
    at a time and kept the casts. 14 function-pointer casts and btlseqEnd's `s32 i`/`(s8)i` loop go.
  - `BattleWorkPad.mHoldRepeatState/FrameCounter` s8, `BattleWorkIcon.flags/iconId` u16,
    `BattleAlliance.allianceId` u16. These remove the `*(u16*)&` and `(s8)` reinterprets.
  - `_CalcRollDir` returns s8 (bare `lbz` in the callee, `extsb` at each use).
  - `BattleCommandDisplay_TargetSelectMenuDisp(CameraId, void* param)` has the DispCallback shape, so its dispEntry
    cast goes too.
- **Source shapes that matched:**
  - Several volatile reads had gone stale since the prototype fixes and the plain form now matches: ac_air_gauge,
    ac_power_gauge_lv/lv2.
  - The `BattleActions* ac` intermediate (the ac units' common opening) is now the only form in gauge_timing, power_gauge, stick_rotate and pendulum_crane_timing.
  - `id = BattleTransID(event, id)` (event_default) and `mask = args[1]; mask = unit->mTokenFlags & mask`
    (CheckToken, as OnToken/OffToken do).
  - _attack_aud's x/y/z are one `Vec pos` (consecutive slots, already passed as one).
  - yuka_disp's one-pass wrapper loop is a real row loop over the [2][0x81] mesh.
  - Simpler forms: `for (i = 0, reel = ...; ; i++, reel++)` in break_slot, and a `(GXColor){alpha, alpha, alpha, 0xFF}`
    literal in menu_disp.
  - Declaration order instead of temps: battleCameraMain case 8 and _set_haikei_entry_scale (which also loses its
    obj2).
  - battleCameraMoveTo adds one (0, -z/4, z) offset to eye and target.
  - The permuter mask in end_muki, the SC_P macro and the `one` temps in sac_scissor, and the comma nudges in
    ac_signal_timing and star_disp2D_sub all go.
  - ac_lst_rc_keep and ac_stick_keep_left declare iconDispGx locally with their u16 id, the documented
    local_prototype_exceptions route the lecture units use, instead of a function-pointer cast.
- **Judgment calls, for review:**
  - Already-plain MUST_MATCH forms promoted:
    - KinopioIgnite's empty `case 3: break;`
    - zubastar_bomb's one-case `switch (work->phase)`, which zubastar_main also switches on (s-mmeff2 had called it
      steering)
    - DrawMainMenu's zero-on-left `==`
    - timing_a's unrotated `for (;;) { if (...) break; }`
  - KinopioFire grows both axes by one `f32 step` local, the idiom of the Boo bob in the same file.
  - _set_mobj_rotate_x writes through the function's existing `base = wp + 0x20000` pointer.
  - bakuGameMain's uninitialised `neg2` (retail reads it on counts 0x1B-0x1E) stays under MUST_MATCH, with a
    well-defined `#else`, like battle_camera's `yc` and break_slot's `j`.
- **Tried and dropped:** `static inline float fabsf(float x) { return __fabsf(x); }` in math_ext.h (as the SDK's
  fake_tgmath.h has it) makes sac_scissor's `scissor_abs` plain, but party_move.c, which `#define`s fabsf itself,
  then hits an MWCC internal error (Intrinsics.c:6190).
- **Left, with what was tried** (the sub-agents' reports, in `build/mmwork/` while the worktree lives):
  - Data definitions, out of scope for this pass. Each would remove its regions, and each was checked EQUIV on a
    scratch copy:
    - battle_audience: `vec3_802f9750`/`975c` as non-const `.rodata` Vecs like their siblings remove ApSrcEntry's 3
      volatile copies.
    - battle_seq_end `_lvup_spot_on`: dropping `dat_80420c28` for a `(GXColor){0xFF x4}` literal removes the
      volatile stores.
    - ac_shot_target: a writable `.sdata2` `dat_804218f8`, as ac_repeatedly and ac_shot already have.
    - ac_signal_timing: four zero Vecs as literals.
    - ac_monosiri_target: eleven separate `const Vec`s, matching symbols.txt's 0xC sizes.
    - ac_repeatedly: literals instead of the pool-order constants.
  - Declspec objects, pool helpers, string-layout macros (ACSE_STR, BSO_SFX, GT2_SFX, SFX comma initialisers) and
    `defer_codegen`.
  - Still steering after real attempts:
    - three volatile Vec loop copies (gauge_timing, gauge_timing2, button_down)
    - gauge_timing's `(A || A) && B` goto pair
    - event_cmd's GetRGB/JumpPosition/JumpPartsPosition/CheckDamage volatiles (5-20 variants each)
    - information's irand temps
    - the Waku_col/iconDispGxCol/iconNumberDispGx3D/btlDispGXQuads2D by-pointer and width casts
    - kinopio's dead-`step` reuse and Tbl pun
    - break_slot's colour inline
    - battle_damage's `s32 rate` with (s8) casts (an `s8 rate` is off by one compare)
    - status_effect's loop `+ 0`
    - stage's MATCH_F32 FIFO writes and tou_gamen_screen_tev
    - battle_unit's lhax read and UnitStatusBlock copies
    - seq/luck/ice comma pointers (MWCC never hoists `&weapon` or the string without them)
    - bakugame's +/-0x80 pointer and volatile args
    - deka/muki/scissor colour inlines and the HSV2RGB cast
    - zubastar's create_takaku volatile and b18 base
    - seq_end's in-argument `halfWidth =` assignment
    - seq_command's `(s32)(s8)` cast

### Optimisation pragmas: removed where the object doesn't need them (2026-10-20, Claude Opus 5.5, n9-q-o-pragma)

`tools/cleanup_index.py` before (e2103891) and after: index 16.392 -> 15.612 per kLOC (11570 -> 11011 weighted),
**opt_pragma 1457 -> 900**, must_match 1300 -> 1298 (party_cloud's two guards held only a pragma each); every
other category unchanged. Build byte-identical after every commit (`build/G8MJ01/ok`); all 85 changed units compile
under `configure.py --non-matching`; `fncast_lint.py` and `local_prototypes.py` clean. `tools/ci.py` was run before
each commit with its tools/tests step stubbed (the same 9 environment failures as the chapters above:
test_reloc_value_check can't import elftools, test_wave_launch times out on systemd scopes); every other step passed.
The narrowing commits needed `--allow-pragma`, because the lint reads a moved line as a new one.
- **Method.** One script per pass, run over every file with a counted pragma. It compiles the unit's object alone,
  with the ninja command and `-o` pointed at a temp dir, and compares it with the baseline object through
  `tools/obj_equiv.py`'s `compare()`. Pass 1 removed each pragma, or its on/reset (off/on) pair, on top of the removals
  already accepted in that file, keeping a removal only if the object was identical. Lines were blanked during the
  search and deleted afterwards, then each file was re-verified (only sdk/arc.c uses `__LINE__`). A rerun, every
  co-located group of pragmas removed together, and every file's pragmas removed at once all found nothing more. The
  narrowing pass moved each setter down and each closer up, one function at a time, until the object changed. A
  merge pass dropped the `reset` + setter between back-to-back regions of the same setting when no function body lay
  between them. `defer_codegen`, `force_active`, `section` and `pool_data` were not touched.
- **Removable (each kind with its count):**
  - `fp_contract`: 7 of 9. The REL cflags have carried `-fp_contract on` since these were written.
  - `dont_inline`: 266 lines in pass 1, then 188 more in merges. The function was never going to be inlined anyway:
    too big, defined after its callers with no deferred inline taking it, or only used by address. The merges also
    took 26 `opt_propagation`, 10 `opt_common_subs` and 10 `opt_lifetimes` lines.
  - `opt_common_subs`: 27. `inline_max_size`: 13, including rsh_05's four orphan resets.
  - `opt_propagation`: 9. `inline_max_total_size`: 4. `opt_lifetimes`: 3. `optimize_for_size`: 1, an orphan reset.
  - Several comments that called a pragma "required" or "load-bearing" were wrong: eff_starstone, eff_confetti,
    party_cloud (both pairs), kpa_stageclear_disp, battle_audience's PerAct/ApSrcCtrlProcess, among others. Those
    comments went with their pragmas.
- **Held, in every form tried (each one changes the object):** all of `opt_dead_assignments` (30),
  `opt_loop_invariants` (20), `opt_strength_reduction` (4), `optimization_level 2` (npcdrv, eff_ripple),
  `global_optimizer off` (eff_recover_heart), `inline_depth(5)` (mri_puni), sac_genki's `auto_inline off`, and all but
  one `optimize_for_size` (the files that use it are built `-O4,p`). Also held: 371 `dont_inline`, 176
  `opt_propagation`, 109 `opt_lifetimes`, 103 `opt_common_subs`, 6 `inline_max_size` and 6 `inline_max_total_size`
  lines.
- **Narrowed.** Regions in 31 files now begin and end at the functions that need them (38 pragma lines moved); most
  just skip static inline helpers ahead of the one real function. The rest:
  - Seven file-wide setters gained a `reset` after their last needed function: sac_genki, eff_mugi, eff_snow_dust (2),
    eff_stardust (2), unit_mario. Those are seven new lines; the object is identical.
  - mri_puni's push/pop moved its `pop` up. In npcdrv and eff_ripple the `push`/`pop` already scopes the region.
  - A reset that doesn't work right after its setter is not a scope: `#pragma inline_depth reset` does nothing, so
    mri_puni's push/pop stays.
- **End-of-file settings (for the next pass).** With `-inline auto,deferred`, an unpaired `#pragma dont_inline on`
  acts by being in force at the end of the file. In battle_audience_kinopio, battle_enemy_item, battle_seq_command and
  mario_kemuri the object is identical with the setter moved after the last function. Narrowing these makes them look
  like they wrap the last function, so they went back to their original place. eff_recover_heart's
  `global_optimizer off` behaves the same way and already sits before the file's last function. Paired `dont_inline`
  regions do act on the functions inside them. The opt_* settings tested (`opt_common_subs`, `opt_lifetimes`,
  `opt_propagation`) act per function.
- **Comment fixes found on the way:** ac_timing_wide's two `optimize_for_size` comments named each other's function.
  The asm shows battleAcMain_TimingWide and actionCommandDisp are the stmw/lmw prologues. eff_mugi no longer says
  effMugiKemuriMain needs CSE off. mot_hammer's propagation note no longer names marioChkHammer2. eff_breath_fire's
  "0.2f across the rand call" note described its Entry function, which doesn't need the pragma, so the note is gone.
- **Left for a later pass:**
  - Merging regions that have a function body between them. It would widen a setting to functions that don't need
    it, so it wasn't tried.
  - Pragmas inside function bodies (mot_hammer's `opt_dead_assignments`, several others). They are kept where they
    are; moving them outside the body was not tested.
  - The scripts (`build/opragma/` in this worktree: try.py, narrow.py, merge.py, eoftest.py, verify.py) are not
    committed.

## BattleWork / BattleUnit / BattleWorkUnitPart layout audit (2026-10-20, Claude Opus 5.5, n9-q-o-bwork)

Every opaque band of the three structs, with every access in src/ and rels/ (through the struct, raw offsets or
`wp + 0x20000` bases), typed where the users agree; where they disagreed, the asm of each user decided. Build
byte-identical at every commit (build/G8MJ01/ok recreated, and `tools/obj_equiv.py --tree` against a snapshot of the
pre-branch objects shows all 1,230 compiled objects equivalent); the full `configure.py --non-matching` ninja compiles
every object (only the dol link fails, as usual); fncast_lint and local_prototypes pass; `tools/ci.py` passed before
each commit with its tests step stubbed (the worktree timeouts noted above). This worktree had no accepted ratchet
baseline, so `ci.py --init` was run on the first build, whose linked modules already matched build.sha1.

- **Finding: the `wp + 0x20000` bases are sub-struct pointers.** Earlier passes kept `u8* base = (u8*)wp + 0x20000`
  with `*(s32*)(base - 0x7D68)` because `wp->field` recomputes the addis per access. A pointer to a sub-struct gives
  the original's code: MWCC folds the sub-struct's low half into each displacement and keeps one addis, and where the
  original indexes from the sub-struct (AnnounceSetParam, _nozzle_turn, _nozzle_work_wait) it is exactly that base.
  So the original had structs here. The new types are named by their BattleWork field (`BattleWorkField180C0` is the
  type of `field_180C0`), with `field_0xNN` members at sub-struct offsets and the BattleWork offset in the comments:
  - `BattleWorkField180C0` (0x180C0-0x182B0, 0x1F0 bytes; start proven by the indexed users): the stage-object work.
    Members: layer-2 rotation Vec and its last-applied copy (0x4/0x10), `stageFlags` (0x1C, moved in; also 0x10 wall
    close, 0x40 blocks the iron-frame fall), the object-turn and four layer event ids (0x20-0x30), the rank's nozzle
    and fall-object data blocks (0x34/0x38, u8*), `weaponAddr[2]` (0x3C, BattleWeapon*, the name already in the repo
    from `_nozzle_work_get_weapon_address`) and the two combined BattleWeapons (0x44), nozzle directions (0x1C4/0x1C7,
    u8[3]), nozzle type (0x1CA, s8), turn event ids (0x1CC, s32[3]), nozzle-work/wall/iron-frame/object-fall event ids,
    fog frames (0x1DC) and the fog effect (0x1E0). 0x182A0 was "s32 in one user": that s32 store was
    `effNozleEntry`'s result, so it is an `EffectEntry*` everywhere. battle_stage_object.c's local `BattleNozzleWork`
    went. Users: battle_stage_object (all functions), battle_seq (BattleWaitAllActiveEvtEnd, btlseqFirstAct),
    battle_audience_kinopio, battle_damage, battle_event_cmd, battle_seq_end, battle_mario, and the ceil/flag readers
    in rels (tou2 champion, muj gundan_zako 1/2 and poison_pakkun, rsh database, common pakkun/monochrome/ice).
    `wp->field_180C0[0x1C]` (one past the old band) was stageFlags in every reader.
  - `BattleWorkField18C70` (0x18C70-0x18D88; start proven): battle_message's announce x/y (s16), params s32[4] and the
    formatted message.
  - `BattleWorkField18FF0` (8 bytes): battle_ac_help's message and display type. Start not proven by an index.
  - `BattleWorkField19068` (0x18 bytes): `forceFlags`, `forceAudienceCount`, `forceAudienceKind`, `forceSlotValue`
    (names kept). Start not proven by an index; battle_audience and battle_break_slot read it through one pointer.
  Reviewer: the two unproven starts are the smallest structs the code allows; the original's may begin earlier.
- **BattleWork, flat members:** `field_2` u16 (battle_seq's burn-effect step), `field_EFC` s32 (frame countdown),
  `field_F04`/`field_F08` s32 (battle_disp's stock-exp display count and its frame counter; were `(char*)wp + 0xf04`),
  `field_18FE1` u8 (Merlee's curse type once it fires: 1 attack, 2 Mario's defence, 3 double exp) and `field_18FE2`
  u16, `field_19038[5]` (turn counts that disable jump/hammer/item and the partner's tech/item; recover_cmd_disable
  clears all five; battle_seq, battle_seq_command, las and tou2 koopa, sac_genki), `field_1904C` (nanika_luck_event id),
  `field_19064` u8, `field_19080` u32 / `field_19084` s32 (gor lecture's flag and window id).
  `lectureUnitId` (0x424): set only by the three `_battle_set_lecture_unit_id` (tik and gor symbols), read as
  `BattleTransID(-6)` and by the command menu.
- **Act record:** field_16F1C stays an array (battle_actrecord.c walks it). The `(u8*)wp + 0x16FNN` sites in
  battle_seq_command, battle_unit, battle_seq, battle_mario and battle_item_data, and battle_damage's
  `field_16F1C + N`, keep their offset form. The sac get_ptr
  functions take `actionCommands.field_0x2BC` instead of `+ 0x1F4C`; the header notes that those works run on into
  `field_1F9C`.
- **BattleUnit:** 0x2A0, 0x2C4 and 0x2CC are event-code pointers in every user (the s32 readings stored scripts' raw
  values). 0x2A0 is `confusionEventCode` (setter btlevtcmd_SetEventConfusion; battle_seq starts it on a confused
  unit's 50% roll) and 0x2C4 `actEndEventCode` (setter gor `_set_actend_event`; battle_seq runs it as the act ends,
  id in 0x2C8), named after their setters as `ceilFallEventCode` was; 0x2CC stays `field_0x2CC` (void*, the
  superguard type 4 event; no setter in the tree). 0x264/0x268 s32 and 0x26E/0x26F s8 (FP damage sums, FP lost,
  the superguard count copy), 0x30C u8 (borodo coin count), `mSmartEntry` a `struct smartEntry*`,
  `mBurnDamageFlameState`/`mbBurnDamageFlameActive` u16 (lhz/cmplwi in every read; battle_status_effect's casts and
  its "header likely wrong" note went). `BattleWorkHpGauge.field_0x0` is u16, so battle_unit.c's `HpGaugeLocal` copy
  went. chuchurina/yoshi/wanawana raw stores now name mSmartEntry, mExtraWork and the part's dispCallback.
- **BattleWorkUnitPart:** 0x1A6/0x1A8 u16 (gravity/float frame counts), 0x4F4 GXColor (btlDispMain's draw colour,
  copied to evtColor), 0x4F8 f32 (z offset; black_peach writes it), 0x4FC s8 (rotation order; the u8 reading was a
  zero store). `getGravityDispOffset`, `getFloatDispOffset` and `btlDispAnimeSpeed` take `BattleUnitPart*`: every
  caller passed a part cast to BattleUnit*, and their bodies read part offsets (0x1A6, 0x1A8, 0x1C0, 0x208, 0x20C)
  through that pointer. battle_disp.c has no raw unit/part offsets left.
- **No access anywhere in the tree** (left as bands): BattleWork 0x2750, 0x18FF8-0x19038, 0x19065, the force
  struct's 0x4-0xC, field_180C0's 0x0/0x1D/0x1CB; BattleUnit 0x186, 0x1BC, 0x25E, 0x136, 0x30F, 0x31C; part 0xBE,
  0xDC-0xF8, 0x1AA, 0x4FD.
- **Left, and why:** `_Nozzle_Change_Check` walks field_180C0 by byte (`base[typeOffset]`, `slot[0x73]`; indexing the
  members gives add + lbz instead of the original's precomputed lbzx; a comment maps the offsets).
  `BattleWaitAllActiveEvtEnd_NoBgSetEndWait`'s `void* p` (register steering, not a layout question). Outside the three
  structs: BattleSampling's `u8*` parameters (BattleSamplingEnemy, battle_stage_object's u8 stack arrays), the
  BakuGameGaugeColor view of `dispParams.field_0xC` (BattleACDispParams; now spelled through that field),
  `unit->work[i]` read as f32 by event_cmd (scripts keep ints and floats there), BattleUnitKind +0x14 read as u16 in
  battle_mario, the UnitStatusBlock MUST_MATCH copy, and PartyEntry raw offsets in aji_13/moo_04.
- **Index** (`tools/cleanup_index.py`, same script both sides): 16.121 -> 16.368 per kLOC (11366 -> 11540 weighted);
  raw_offset 497 -> 463, field_0x 3641 -> 3883. The rise is spelling, not new placeholders: the index counts
  `field_0x1D8` but not `field_18298`, and not `*(s32*)(w - 0x7D68)` or `(char*)` casts at all, so moving negative-base
  reads onto sub-struct members and renaming HpGaugeLocal's `field_C` to BattleWorkHpGauge's `field_0xC` add counted
  names. battle_stage_object 73 -> 157 weighted for the same reason.
### Duplicated REL helpers: one shared definition each (2026-10-20, Claude Opus 5.5, n9-q-o-dupe)

Many chapter/map RELs carry copies of the same helpers. The copies now come from one definition each, a header under
`include/rel/common/` included by every unit that compiled the copy, so each REL still links its own copy. Build
byte-identical after every commit (build/G8MJ01/ok); every changed C file compiles without `-DMUST_MATCH`
(`configure.py --non-matching`, 148 affected objects); `tools/fncast_lint.py` and `tools/local_prototypes.py` pass;
`tools/ci.py` passed before each commit with its `tools/tests` step stubbed (the same 9 environment failures as the
chapters above; the data step needs `uv run --with pyelftools`, and that run rewrites build.ninja with uv's python, so
rerun `python3 configure.py` after it). No configure.py change: `include/` is on every REL's include path, and the
cleanup index counts it (`rels/common/include/` is outside its scope, so code moved there would just vanish from it).

- **How the families were found:** every function's original asm (`build/G8MJ01/<rel>/asm`), instructions normalised
  (labels, `_<rel>_<addr>` suffixes), grouped by name and hash across source files; then the section rule below
  applied to each unit's `.text` order.
- **The constraint that decides everything: MWCC puts the functions of every included file in a `.text` section of
  their own, and the unit's own section links ahead of them.** With deferred inlining (every REL but gon and mri) a
  unit can therefore take a function from a header only if all its own functions come after it in the source, that
  is, the shared functions are the last ones of the original `.text`; gon and mri (built without deferred inlining)
  want the shared ones last in the source. Headers keep their place in the file because definition order sets the
  `.text` order; data (strings, pool constants, compound-literal templates, function-local statics) stays in the
  unit's own sections, in order. Found by experiment: the first lecture pass broke six RELs whose own variants sat
  in the middle of the run.
- **Headers** (85; named after the first function they hold or the family's file prefix, subdirectories per unit
  family): each comment says who includes it and that it goes where its run sits. Types shared by a family went in
  with it: `rel/common/evt_lect.h` (LectureButton, the lecture units' u16 iconDispGxAlpha prototype),
  `unit_barriern.h` (from `rels/common/include/unit_barriern_z.h` and three copies), `unit_piders.h` (from
  `rels/common/include/unit_piders.h` and churantalar's header), `unit_boss_koopa.h` (IconJump),
  `evt_shuryolight.h`, Majo3SortEntry in `majo3_sort.h`.
- **Families shared** (units in brackets): the lecture helpers (gon, hei, mri, tik, usu take all of them; eki, gor,
  muj, tou, win the run up to lecture_button_rtn; dou lect_mario_ido_sub/sum); the Yux/barriern functions
  (unit_barriern_custom, unit_barriern_z and the three satellite units all of them, v1 the barriern_zanzou entry
  points, v2 battleGetBarriernSatelliteInitPos); Piranha Plant pakkun_ceil_exist/shadow_scale_* (7); Poison Puff
  eff_poison_breath/barrier (3); Wizzerd hand/bunsin/shuffle/eff_magic (3); Pider yarn/target functions (v1 and
  churantalar all, v2 and v3 the ends); Bowser _icon_jump_icon and co. (las, tou2); evt_shuryolight's first three
  (aji, las); teresa_check_* (2); krb_get_dir (10 Goomba units); and two-to-four-unit helpers: hitIsGroup, ramp_r,
  resha_tev, honenoko_copy_status, kamec_get_live_num, make_name, setmertn, effect_stardust, wariai_enzan_1/1f,
  majin_callback, bgm_start_wait, killer_*, get_id/yoshi_exist, chain_init/main, _set_draw_callback,
  _fire_point_callback, npc_set_me/npc_dead, vivian_comeback, bubble_copy_status, _wave_offset, randf,
  _space_rotate, getYoshiColor, eff_scanning, eff_aura, togezo_copy_status, _leg_smoke_effect, majo3_sort,
  fontlenpitch, set_dir. 133 functions; rels/ lost 8,825 lines and gained 362, the headers are 3,411.
- **Hacks folded:** the four MUST_MATCH iconDispGxAlpha function-pointer casts (eki, mri, tik, usu) are gone: the
  shared lecture header declares the u16 prototype the other lecture units already declared (the documented
  exception). Both Bowser units' MUST_MATCH iconDispGx casts are gone the same way: `unit_boss_koopa.h` declares it
  with the u16 id and the scale last, as their asm passes it (the exception's "width and order your file's asm
  uses"; register assignment is identical, so the call is direct). barriern_z's two identical volatile-table helpers
  and custom's one are one `barriernSatelliteTo_InitInline`; lecture_button_disp's ppos hack is one copy for ten
  units; eki's hand-expanded lect_map_cam_chg_sub recursion is the plain recursive form; giant_bomb's `(void*)` cast
  on _fire_point_callback went (the callback takes animdrv's three arguments).
- **Unified while sharing:** aji's shuryolight structs take las's field names; dou's LectureButton `state` is `seq`
  (lecture_button_set_seq); barriernSatelliteTo_Init takes the `s32 id` it stores (z's form); the barriern/satellite
  zanzou entry points `return` their sub's result (custom/v1 fell off the end); pakkun_ceil_exist reads BattleWork's
  `stageFlags` instead of `field_180C0[0x1C]`; functions that were defined without `static` are static (all 133 are
  `scope:local` in every symbols.txt); stale NONMATCHING notes on functions that match went with their copies.
- Counts (`tools/cleanup_index.py`, 33e9c638 -> this branch): index 16.293 -> 16.249 per kLOC (11494 -> 11389
  weighted over 705471 -> 700896 code lines); must_match 1226 -> 1199; field_0x 3645 -> 3601; raw_offset 499 ->
  489; fn_ptr_cast 131 -> 125; unk_name 722 -> 720; the rest unchanged.
- **Left, and why** (asm-identical copies that one header can't serve):
  - The rest of the eki, gor, muj, tou and win lecture helpers: lecture_button_rtn, lecture_button_set_pos and (eki,
    muj, tou) effitemget_busaiku copy named `.rodata` Vec templates (`vec3_<rel>_*`), so their own copies sit in the
    middle and everything after them stays. Turning those templates into the compound literals the other five units
    use is a data-definition change, outside this pass; it would let all eleven units share every helper. dou's
    lect_status_on/off swap bodies against the others.
  - evt_kagemario (gra, jin, usu): callback reads per-REL named GXColor constants and is the first function.
    alpha_tev (dou_05, gor_00, muj_00): gor_00/muj_00 go through an alphaColor inline over a named zero template.
    moamoa_pose_init (eki_00, rsh_06 vs rsh_05): the `"J\0\0"` string padding differs. krb_comp_dir/get_attacker
    (the spiky Goombas): gra's and jon's SinTable is a `u32` table read through a cast, v2's an `f32` one.
  - Everything not at the end of its unit's `.text`: tou/tou2's ranking functions (behind their own `_prolog`),
    the yuu monte helpers, the REL `_prolog`/`_epilog`/`_unresolved` boilerplate (last in source), the shuryolight
    and magnum_battender functions after the first differing one, and so on.
- For a reviewer: battleGetBarriernSatelliteInitPos carries a function-local `static const Vec VecData[]`, which
  moved into its header with the function (the bytes and their place are unchanged). gon's two `#pragma
  dont_inline` pairs now wrap the lect_mario_ido.h and lect_map_cam_chg.h includes (same pragma lines, no new ones).

## MUST_MATCH source shapes outside battle: effect, driver, mario, party, window, event, manager, top-level src (2026-10-20, Claude Opus 5.5 orchestrating eight Claude Opus 5.5 workers, n9-q-o-mmsrc)

Task (Isaac): for each remaining `MUST_MATCH` block outside src/battle/, find the natural C the original most likely
had (statement/declaration order, an inline the duplicated body came from, an honest type, an early return, another
loop shape) and make it the only version; leave the block, and record what was tried, where nothing matches.
Volatile, puns, int round trips, dead stores, redundant ternaries and function-pointer casts stay guarded. Data layout
(strings, gap_/force_active, section declspecs, data definitions, .sbss2 templates) was out of bounds.

- Method: each candidate compiled with its object's exact ninja flags and compared with the pre-branch object by
  tools/obj_equiv.py (all allocated sections and relocations), plus a compile without -DMUST_MATCH. Harness (scratch,
  untracked): build/mmwork/try.py, resolve.py, blocks.py, dump.py; the eight workers' reports are in
  build/mmwork/agent1..8/REPORT.md (per block: the form that matched, or every shape tried). 206 files, split into
  eight disjoint groups.
- Result: 754 -> 417 MUST_MATCH regions in scope (545 -> 260 inside functions). Index (tools/cleanup_index.py)
  16.392 -> 15.748 per kLOC (11570 -> 11080 weighted): must_match 1300 -> 963, goto 238 -> 224, opt_pragma
  1457 -> 1451, fn_ptr_cast 131 -> 125, field_0x 3645 -> 3624, address_name 2687 -> 2654, unk_name 722 -> 679.
- Shapes that turned out to be the original's:
  - statusWinCheck's body as `if (...) return TRUE; return FALSE;`: every `!(statusWinCheck() ? 1 : 0)` and every
    `switch (statusWinCheck())` dispatch in statuswindow.c becomes a plain test.
  - `int`, not `s32`, for the intplGetValue step counters (evt_damage, evt_item, evt_nannpc, as evt_npc.c's `int
    elapsed`); with it two unguarded pragma pairs (evt_item, evt_nannpc) were unnecessary.
  - fontmgr's always-false compare is a multi-character constant: `arg1[0] == '/' && arg1[0] == '/r'` (0x2f72,
    unsigned in MWCC), the cmplwi in retail.
  - Duplicated bodies come from inlines: chkTreasureBox (mario_balloon; its discarded marioGetPtr() is the inline's
    own fetch), marioPartyKillSlot, bomhei_chkBombRock, uranai_table_search, hitHasAttr (party_motion's dead inner
    NULL test), toge_flush's seven colour setters as one, starstone's and horao_spell's twins merged, romfont's
    twin lookup merged. These helper names are new (no symbol exists for an inline); they describe their bodies.
  - The Vec built in one local and copied to another before dispCalcZ is a `makeVec`-style inline (eff_bomb,
    eff_energy, as eff_batten/eff_particle/eff_spirit/camdrv); elsewhere the copy itself was made the only version.
  - Footmark (include/party/party_move.h) holds a `Vec pos`, not x/y/z floats: every word-copy pun in party_move and
    party_motion becomes a struct assignment (MWCC copies a Vec with lwz/stw). New member name `pos`, from the
    existing x/y/z and the positions written into it.
  - FontDrawColor declared by value in evt_twoddisp (as fontmgr.c defines it); winIconSetV/winFontSetV inlines out of
    their guards in win_badge/win_mario/win_party/win_root.
  - Many guards were simply no longer needed (the #else arm matches now, mostly after #1091's prototype fixes):
    volatile reads in eff_recovery/eff_funemizu/party_nokonoko, eff_kemutest's dead store, eff_toiki's (u32) round
    trip, `#pragma opt_common_subs` in party_cloud, `#pragma pool_data` in eff_particle/evt_mobj/win_root.
- Made the only version unchanged (already plain C, no hack marker; listed per block in the reports): colour-building
  inlines without puns, the extra Vec copies, empty `case N: break;` labels, a one-case switch, yoda compares,
  declaration orders, named temporaries. Two keep a retail uninitialised read in the natural build too: win_root's
  `s32 row` and mario_balloon's `balloonType`.
- Left, by kind (each with its attempts in the reports): data layout; FIFO per-store reloads (`volatile` 0.0/1.0);
  colour helpers that read a u32 object through `*(GXColor*)&`; int round trips that pin address materialisation
  (eff_fall, eff_nice, npcdrv); pool-order helper functions and their pragmas; function-pointer casts on variadic
  font calls and by-value GXSetFog; a few gotos (windowdrv, mario, opuku_wgun) and redundant tests (eff_break's
  language test, cardmgr, evt_win) whose only EQUIV alternatives are themselves redundant.
- Leads, not applied because they touch data definitions or shared headers:
  - Literal `0.0f`/`1.0f` in plain GXPosition3f32/GXTexCoord2f32 calls reproduce the volatile FIFO reloads exactly in
    .text (shadowdrv, eff_break, eff_funemizu, evt_telop, eff_sunlight); only .sdata2 differs (two anonymous pool words
    beside the named float_0/float_1). Removing the named definitions in the same change may make them match.
  - Named zero Vecs and strings that are really the compiler's own templates: eff_peach_fukidashi (`char buf[0x20] =
    "\x81\x7E";` replaces eight punned word copies), eff_pointget, eff_stageclear, eff_syuryou (strings as literals),
    eff_mahorn2, eff_sweat_n64, mario_motion (strings by name, no force_active). All EQUIV at object level and the
    linked build was not affected, but each deletes or un-forces named data, so they were reverted; diffs in
    build/mmwork/proposals/.
  - Retyping u32 colour objects as GXColor frees the pun blocks: eff_nozle, eff_spark, eff_starstone, eff_batten,
    eff_funemizu, eff_thruhammer, evt_map, kameki, kumokumo, opuku_wgun, pokopi_thunder, sstar, recover_heart,
    butterfly, confetti. eff_teresa: its float definitions after the code (the eff_mahorn layout) remove READ_RODATA.
  - Header changes tested with a header copy: spline_getvalue with pointer parameters (himo, mapdrv); MarioWork.poseTime
    u16 (koopa_motion); SmartWork.waitsync s32 (memory); searchUnder2 returning HitEntry* with f32* out (evt_mario,
    evt_party; mario_hit unchanged); mariost.h `language` u32 (evt_telop, msgdrv, evt_shop, effdrv, battle_menu_disp
    cast it); arammgr's callback-written flags as volatile fields.
  - eff_release's vertex arrays by name match at object level but leave lbl_803A4C60 unreferenced outside
    force_active (deadstrip risk); untried in the link.
- Verified: `ninja` recreates build/G8MJ01/ok; all 136 changed C files are obj_equiv-equivalent to the pre-branch
  objects and compile with `configure.py --non-matching`; fncast_lint and local_prototypes pass; tools/ci.py passes
  every step (lint, build, natural, data, ratchet: no regressions) with its tools/tests step skipped, which fails in
  this worktree for environmental reasons (test_wave_launch's systemd scopes; pyelftools missing for the system
  python, so ci ran under `uv run --with pyelftools`).

## Review of n9-q-o-mmsrc: helpers and steering arms back under MUST_MATCH (2026-10-21, Claude Opus 5.5, reviewer)

- The new inline names (hitHasAttr, chkTreasureBox, marioPartyKillSlot, bomhei_chkBombRock, uranai_table_search,
  movefloorLerp, makeMiniGameColor, togeSetColor, and winLogAddTab0 renamed to winLogAddTab) had no outside evidence,
  so those files and hunks are back to their guarded pre-branch forms. The inline shapes still match at object level
  (see the entry above), so they can come back once each has a name with evidence.
- win_root's `row` and mario_balloon's `balloonType` are initialised in the natural build again (`= 0` under #else).
- Back under MUST_MATCH as register or stack steering: eff_horao_spell's `s16* p` vertex copy (with
  makeHoraoSpellColor/2p), the `size2 = size` pair in eff_kemuri8/9, eff_star_power's embedded `f7 =`, eff_fall_leaf's
  second record type, the `rad` steps in eff_scanning/eff_uranoko, eff_scanning's Vec copy, win_log's `f64` fabs
  temporary, party_cloud's playerPtr reload and its pragma, cam_road's atan2 scratch and negated compare, arcdrv's
  `infoPtr`, mobjdrv's `f64` fabs locals, evt_env's 32-bit masks, evt_lottery's `t` reloads, evt_telop's constant
  temporaries, and hitdrv's hitFindElem copy of hitNameToPtr.
- Result: MUST_MATCH lines outside src/battle 990 -> 656 (the branch alone reached 581); index must_match
  1300 -> 1030.

### Readability: remaining gotos, natural control flow only (2026-10-21, Claude Opus 5.5, n9-q-o-goto)

Every goto from the earlier passes was tried again, file by file, and each rewrite was checked with
`tools/obj_equiv.py` against a baseline object before the full build. Build byte-identical after each commit
(`build/G8MJ01/ok`); every changed unit compiles under `configure.py --non-matching`; `fncast_lint.py` and
`local_prototypes.py` clean; `tools/ci.py` PASS before each commit with its tools/tests step stubbed (the same 9
environment failures as the chapters above). No names, pragmas or flags added, and no new `do { } while (0)`
(win_mario's existing `MARIO_TRAVERSE` macro only moved above the else-if chain). One MUST_MATCH region is new:
evt_yuunpc's redundant condition (below), which replaced two gotos.
- **Counts** (`tools/cleanup_index.py`, origin/main 66b9a7bf -> this branch merged with it): index 15.026 -> 14.717
  per kLOC (10494 -> 10276 weighted); goto 229 -> 123; address_name 2661 -> 2655 (the `L_xxxx` labels); must_match
  841 -> 841; the rest unchanged.
- **Forms that matched.** `break` where the label sits right after the enclosing switch or loop (win_badge,
  battle_damage's `while (work.hitLands)`, ac_button_down, aji_17, yoshi_use's `L_3588`, koopa's hit loop);
  `continue` for evt_keijiban's story loop (as evt_uranaisi); top-tested `for (;;)` loops with `break` for the
  label/goto loops (gor_irai); wraps and
  if/else (mri_puni's four `if (!mag) goto end`, yuu_01, mot_roll, koopa, party_motion, battle_stage_object,
  ac_stick_rotate's roll block, puniMoveAtari, dou10_yoko_yari, eff_fukidashi); else-if chains (win_mario, win_party,
  win_item, battle_seq_command's confirm paths, mobjMain's first five triggers, evt_npc, unit_chorobon_gundan with
  the store written in both arms); early returns (marioYoshiForceCancel, which ends right after; windowdrv's
  `y2 <= pa10` exit, while its twin still needs the goto). Three less obvious ones:
  - gor_00 make_itemsel_table: the `0xA6..0xB0 except 0xAC` goto test is MWCC's own lowering of a `switch` with
    those ten cases sharing one body.
  - yoshi_use: the `default: goto L_3CE4` and the jumps out of case 0x16 go away by closing the switch before the
    L_3CE4 tail (which ran to the end of the function), so `default` and the jumps are `break`s.
  - evt_yuunpc: the dead retests around `goto bf30` are what MWCC emits for one condition, `(flags & 0x18) == 0 ||
    ((flags & 0x18) && (f27 >= x || (f27 < x && f28 < x)))`; the second `flags & 0x18` test reuses the CR. The
    redundant terms are under MUST_MATCH, with `(flags & 0x18) == 0 || f27 >= x || f28 < x` in `#else` (the
    simplified form alone is 12 bytes shorter).
- **MUST_MATCH regions.** windowdrv's second `goto ret` region is gone (its plain `return` matches). animdrv's
  animPoseSearchAnimIdx takes the early-return form main's #1099 gave animPoseSearchAnimPaper/animPoseSearchAnim.
  (#1099 had also taken eff_kiss's type filter, evt_window's loop, partyGetAppearPos2's `PARTY_APPEARPOS2_FAIL` and
  marioSearchHead_roll's region to natural code; the merge keeps main's versions of those.) Still under MUST_MATCH
  but goto-free (for a reviewer): ac_gauge_timing's two goto pairs are `((acFlag & 2) || (acFlag & 2)) && B`, and
  eff_opuku_wgun_n64's dispatch is `if (i == 0 || i == 0)`: the doubled test gives the original's `bne; beq` /
  `beq; bne` pair. The natural `#else` forms are unchanged; ac_gauge_timing's regions now cover only the condition.
- **Left (123), and why.** Each was tried; where none existed, a one-line reason was added at the goto.
  - MWCC merges `x == a || x == b` on one variable into a range check, so chains of separate `if (x == a) goto`
    stay: itemdrv's boot/hammer ids (a `switch` gives a compare tree), party_nokonoko case 20 (0x18/0x19).
  - Shared tails the original has once, where the structured form needs a second copy or a flag (MWCC never merges
    the copies): battle_seq_command's cursor-direction checks (16; an else-if chain reloads the cursor), battle_detect
    (15, as documented), pmario_sound (8), npcdrv (7), bomhei's two `found` blocks (6; the two-copy if/else is +8
    bytes), battle_audience (6), evt_mobj's `run_event` (4), itemdrv's bound tails, sac_deka's restart/rescan,
    sac_bakugame, gor_03, mot_slit, mobjMain's party test (2: a miss falls through to the Bobbery test), mariost,
    sound, hitdrv, msgdrv, database, battle_item_data, evt_yuuminigame/evt_goryuugijo, yoshi_exit (the copy of
    marioYoshiForceCancel that continues with `marioChgMot`; calling the function instead inlines it but schedules
    `vec` later), unit_party_yoshi's `active = 1` tail.
  - Search loops whose hit skips the not-found store after them (party_motion, eff_funemizu, unit_chorobon_gundan,
    animEffectAsync, evt_uranaisi's two MUST_MATCH ones): storing the default before the loop or testing the index
    after it changes the object; the shape is an inline's early return, and no such inline is known.
  - cam_road's two gotos to the next line keep dead compares the original has (an empty `if` body is dropped).
  - arc.c's `goto next_hier` is the SDK's own (tww/tp/ac dvdfs.c and tp's arc.c have it).
  - system.c fsort (6), mario.c's MUST_MATCH pair, battle_damage's All-or-Nothing skip, animdrv's `return other !=
    0` pair, mapdrv's `CLIP_TEST`, windowdrv's first exit: as documented in their comments and the chapters above.
- This supersedes the "left" lists in the earlier goto chapters for yoshi_use (`L_3588`, `L_3CE4`), win_mario's
  `goto tail`, mobjdrv's trigger chain (seven of nine), the ac_gauge_timing goto pairs and evt_yuunpc's `bf30`.

## CONTEXT.md split at 100% matched (2026-10-21, Isaac; written up by Claude Opus 5.5)

The parked idea "Split this file at 100% matched", done. It read, in "Parked ideas" until this split:

- **Split this file at 100% matched** (parked 2026-10-20, Isaac). Move the matching-era chapters (up to the
  last match, around 2026-10-17/18) to `docs/CONTEXT-matching.md` and keep this file, under its current name, as
  the live cleanup ledger, so CLAUDE.md, the guides, `tools/` and the worker prompts need no changes. Copy the
  rules still in force (pragma policy, build parity, data ratchet, structural fakes) into a short standing-rules
  section at the top with links to their dated chapters. Do it in one commit between waves, since every open
  branch appends here. Not needed for now: agents grep the file rather than read it whole.

- `docs/CONTEXT-matching.md`: every chapter from "Why TTYD" (2026-09-24) through "animPaperPoseDispSub LINKS animdrv"
  (whose last section records mapdrv linking and the project reaching 100% on 2026-10-18, #852), verbatim and in
  their original order, under a frozen header. Nothing more is appended there.
- `docs/CONTEXT.md` (this file): the preamble, now pointing at both files; "Current state"; the new "Standing rules";
  "Parked ideas"; every chapter from "Leftovers found while playing the matched build" (2026-10-18) on, verbatim.
- "Standing rules" is the only new text: the rules still in force, each linked to the chapters that decided it, kept
  current in place like "Current state". `tools/split_context.py` inserted it from `tools/context_standing_rules.md`.
- Reproduce: `python3 tools/split_context.py` on an unsplit `docs/CONTEXT.md`. Verify: `python3
  tools/check_context_split.py [ORIGINAL]` takes the original from origin/main's `docs/CONTEXT.md` (or the path
  given) and checks that every line and every section is in exactly one of the two files, unchanged and in order;
  the only differences it allows are the frozen header, the preamble edit, the "Current state" pointer to the
  orchestrator's-role chapter, the parked idea above moving here, "Standing rules" and this chapter.
- Search both: `grep -n PATTERN docs/CONTEXT*.md`.

## The hard remaining raw offsets (2026-10-21, Claude Opus 5.5, n9-q-o-raw3)

A pass over the 453 `raw_offset` sites that `tools/cleanup_index.py` counted on 66da3454: computed bases, strided
walks, struct views at an offset, reads past a struct's end, and sites whose users disagreed on the type. The
orchestrator (Opus 5.5) took the MarioWork motion files. Four Opus 5.5 workers took disjoint file groups (party,
drivers, battle/window, events/rels), each in a nested worktree under `build/wt/` that owned its own headers. The
orchestrator reviewed and cherry-picked their branches, dropped one commit and fixed three things (below). Every
changed object was compared with a snapshot of the pre-branch objects (`tools/obj_equiv.py`, one unit at a time while
working). After each header change the whole tree was compared (`--tree`, all 1,230 objects equivalent) and
`build/G8MJ01/ok` was recreated. The full `configure.py --non-matching` ninja compiles every object; only the dol link
fails, as usual. fncast_lint and local_prototypes pass. `tools/ci.py` passed before each commit, with its tests step
stubbed (the worktree timeouts noted above), and `ci.py --base 66da3454` passes over the whole branch.

- **Counts** (`tools/cleanup_index.py`, same script both sides): index 15.539 -> 15.034 per kLOC (10876 -> 10522
  weighted); **raw_offset 453 -> 230**; field_0x 3839 -> 3933 (new placeholder members, and typed reads that now
  spell `field_0xNN`); must_match 1111 -> 1109; everything else unchanged. Of the 230 left, 33 are the act-record
  band (out of scope), 13 sit in data initialisers (mobjdrv's cubeDN table, unit_system's `regist + 8`,
  eff_breath_fire's `stg_fire_breath_v + 0x46`), and most of the rest are inside MUST_MATCH arms or have a recorded
  reason below.
- **Recurring finding: the folded index.** Where retail computes `addi rI, rI, C` and then `lhzx`/`sthx`/`stfsx`
  (the member offset added to a scaled index or byte offset), every typed member form gives `add` plus a load
  displacement. That covers battle_unit's `pouch + party * 0xE + 6`, seq_load's three cursor reads, shadowdrv's
  shadowSetSize/SetCamId, sound's SoundEfxSetPan/SetSrndPan, koopa_hit's head/ride getters and envdrv's second
  texture-matrix read. These stay raw. A no-op cast of the array to its element pointer does give the displacement
  back (`(s32*)work->itemSel + sel`), but it exists only for the match, so the two win_item/win_badge sites that
  needed it were put back to their raw form.
- **MarioWork (orchestrator).** The 0x2A0-0x2BC per-motion union can hold one `field_0xNN` per offset, so each offset
  got the type with the most unnamed sites: koopa_motion's u16 counter pair `field_0x2A0`/`field_0x2A2`, mot_hammer's
  s32 swing type `field_0x2A8` and its s16 flag word `field_0x2B4`. The losing views stay raw:
  - party_yoshi's f32 at 0x2A0 (5 sites) and 0x2B4 (12), and mot_jump's f32 at 0x2B4 (2);
  - mot_hammer's s32 at 0x2A0 and 0x2B8 and its s16 at 0x2A4;
  - mot_stay's s32 counters at 0x2B0, 0x2B4 and 0x2B8;
  - mot_roll's and mot_jabara's integer zero stores.
  Other MarioWork changes:
  - `poseTime` (0x28) is u16. Every reader loaded it unsigned (`*(u16*)&` in mario.c, party_vivian and party_yoshi;
    koopa_motion raw), and only stores treated it as s16.
  - `unk160` is f32 (mario_cam's `*(f32*)unk160`), and 0x164 is `f32 field_0x164` (mario_hit).
  - party_yoshi's flutter base y uses the existing f32 view `wCamVal1`; the header comment now says so.
  - mot_jabara reads field_0x41/42, wStickDir1/2 and jabaraState; mot_slit's MUST_MATCH arm reads buttonHeld and
    triggerRight; mot_plane and mot_ship read field_0x18C.
  - mot_dokan's accessors (spelt `unsigned char*`, which the index pattern misses) all land on named fields now.
  - koopa.c's five `field_0x14` reads still change the object as u32 (the field is u32 for mario_hit), as recorded.
  - mot_stay's 0x2B4 is the same deep-sleep timer as `peachStayTimer`: peach_stay is mot_stay's Peach branch, and
    marioChkDeepSleep reads the slot for both characters. The name is Peach-specific, so mot_stay stays raw. A
    neutral rename would cover both, but that would be a new name, so it was not made here.
- **PartyEntry (party worker + orchestrator).**
  - The 0x3C, 0x44, 0xE8 and 0x14C bands are typed members. `field_0x13C` is a HitEntry* and `field_0x144` a void*.
  - 0x64-0x6F is one `Vec dispRot`, replacing `tiltX`/`spinAngle`. evt_mario copies it from Vec templates as it sets
    Mario's dispRot, partyDisp rotates by z then x as marioDisp does, and the moo rels step it beside
    `mario->dispRot.z`. **Reviewer: this reuses MarioWork's name for the partner's copy of the same display rotation.**
  - `field_4D[1]` was indexed past its end: `[0]` is party_motion's s8 floor-recheck counter (`field_4D`), `[2]`
    is party_vivian's s8 flag byte (`field_4F`), `+4` is party_move's footmark flag (`field_51`), and `+7` is
    `moveAngle`, which partyGetAppearPos4 now reads by name.
  - MarioWork `field_0x3E[2]` is split into s8 `field_0x3E` (the dokan type) and u8 `field_0x3F` (mario_hit's wall
    counter).
  - The five MarioWork 0x2E reads match through `MarioMotion` locals. The recorded clrlwi only appears with a u16
    local.
  - Left raw: party_nokonoko's twelve `NKF(0x10C) = revise360(...)` updates (24 sites; `party->field_10C` loads the
    ternary's constants in the other order and swaps fmadds operands, in five spellings), and party_nokonoko's and
    party_cloud's integer zero stores over the koops/flurrie f32 and s16 members (12).
  - Dropped in review: the worker's `((char**)partyPoseTbl[id])[N]` form, a no-op cast that exists only for the
    match. The byte-offset reads stay; partyRideYoshiMain's yoshiDotPoseTbl read is indexed.
- **Drivers (drv worker).**
  - `AnimData.clip` points at `AnimDataClip`: s32 `field_0x0` (animPoseMain's loop test) and f32 `field_0x4`/
    `field_0x8` (clip start and end). Every clip user reads members now, including win_party's
    `((f32*)anim->clip)[2]` and win_root's `&anim->clip[2]`.
  - `MapEntryAnimData.data` is a different record, `MapAnimTableEntry` (the "animation_table" chunk): `name` is the
    string mapSearchAnmObj strcmps, and the f32 frame length at 0x8 is `field_0x8`.
  - mapdrv.c's private MapMatTexRef/MapMaterial moved into mapdrv.h, and the MapMaterialTex overlay folded into them.
  - MapObject's 0xEC band is `bboxLeft/Top/Right/Bottom`, from GXReadBoundingBox's parameter names. The recorded
    "split breaks the match" no longer holds.
  - `PackedDisplayList.vertexCount` + padding is `u32 nbytes`, GXCallDisplayList's parameter, its only reader.
  - FieldBattleInfo moved into npcdrv.h and is embedded at FieldBattleData 0x20. `ruleKeepResult` is its byte 0x19,
    so the recorded overlap was just the embedded struct. battle_actrecord's `u8* rec` view of `wp->fbatInfo` now
    reads its members.
  - extdrv/punidrv `dlBuffer` is `u8 (*)[0x80]`; fontmgr's BinaryFontWidth has `entries[]`; npcdrv's ENC macro
    went.
  - Left raw:
    - mapdrv `entry + 0x178` reads past the 0x178-byte MapEntry.
    - The biased MapVtx walk (the vertex-typed walk is 4 bytes smaller).
    - animdrv's five `poseOffset` sites and the pos/nrm vertex pointers; its seven low-halfword index pointers (u16
      at +2 of 4-byte entries).
    - npcdrv's stolenItems byte loop.
    - icondrv/texPalette file-relative offsets.
    - The `__OSs8tof32((s8*)src + N)` delta reads: two different record kinds, and no struct exists.
- **Battle and windows (battle worker).**
  - BattleCheckConcluded reads `wp->alliances[i]` directly; its alliance pointer was the part that changed the
    object.
  - `BattleUnitKind.field_0x14` is u16 (one reader, lhz).
  - The partner weapon `area` copies are BattleWeapon (galeForceChance, dodgyTime, holdFastTime, burnTime).
  - The menu icon tables are BattleCommandAction/BattleCommandParty; their HP fields are s16 in battle_seq_command.h
    and battle.h.
  - sac_scissor's `triBuf` is `f32 (*)[6]`; YoshiGundanArea holds its 256 particles; MoamoaWork.train is a MapObject.
  - battle_event_cmd's sampling reads go through BattleSampling, including the GetTakeoffPosition stack buffer,
    passed as `(u8*)&sampling` because BattleSamplingEnemy takes u8*.
  - `member[1].flags = 0` in BattleAudience_Delete: retail's store at +0x134 is the next member's flags. A member
    with flag 0x80 takes two slots (BattleAudience_Entry gives it flags 0x85 and the next slot 0x101), so Delete
    clears both.
  - rsh's `_get_rnd_npc_data` returns `&data[index * 3]` (u32 tables of three-word records).
  - Left raw: tou2's ranking loop (`entries[i]` gives `mr` instead of `li` at loop entry); evt_win's help display
    and setpartylank (the multiply lands on the wrong side of the calls); seq_load's four loop reads (the loops swap
    their i/off locals); battle_stage/tou_03 `param + zero` (inside MUST_MATCH).
- **Events and rels (events worker + orchestrator).**
  - Both evt_yuuranking copies shift through `rankUsed/rankScore/rankMonte[mode][j]`, which removes gor's
    A14L/A74L/AD4L, yuu's YBASE views and yuu's MUST_MATCH `yuu_rank_get/set` arm. The recorded "folds the add
    order" no longer holds. A14R/A74F stay.
  - evt_lottery's MUST_MATCH slot clear is the plain `mSlotTime[arg1] = 0`.
  - evt_shop: get_msg reads `ShopperData.field_0xC[idx]`. The worker had named it `msg` after the `get_msg` symbol;
    it was renamed to the placeholder in review. The price read is `items[i * 2 + 1]`.
  - jon and gor_02 read `MessageData.choiceIndex`; kuragarisan_msgcb takes the smartEntry, as iri_12_msgcb does.
  - tik_00's saved lw/gw copies are `field_0x0[16]`/`field_0x40[32]`; muj_20 indexes `&work->entries[i]`;
    evt_lect_win stores through `&lbtn[idx]`; gor_03's monte init stores go through Monte.
  - Reverted in review: a mixed `npc[idx].name` / `npc + off` pair in end_unknown's credits_pose_evt (typing both
    changes the object, and one of each read as a hack).
  - Left raw: end_unknown's other seven; evt_mobj's `vec + 0x7C/0x88/0x94` (other symbols through a shared base) and
    MUST_MATCH jumptable reads; evt_uranaisi, eff_release and lecture_button_disp.h (inside MUST_MATCH; retested);
    evt_nannpc (`&entries[i]` differs here, unlike muj_20); eff_opuku_jetw.
- **Stale ledger reasons found** (each retested and now matching, so the recorded reason no longer holds):
  - `&wp->alliances[i]` (for direct member reads);
  - `mKindParams + 0x14`;
  - sac_scissor's "void* buffers" and vivian's `area + 0x90`;
  - the gor/yuu ranking views and the lottery clear;
  - the MapObject 0xEC split;
  - the FieldBattleInfo "overlap";
  - itemdrv's icon_id (u16 already);
  - the 0x2E clrlwi (it applies to a u16 local only);
  - the PartyEntry 0x3C-0x3F and 0x64 bands;
  - koopa_motion's "no member added" for 0x2A0;
  - shadowdrv's `lookAt + 0x30` projection;
  - mri_puni's strength-reduced offset comment.
- **Not counted by the index, worth a later pass:** offsets spelt with `(char*)`, `(s8*)` or `(unsigned char*)`, and
  `band + N` forms on byte arrays. This pass fixed the ones it met (mot_dokan, gor_03, party_move/party_vivian's
  field_4D, mario_hit's `obj + 0x15` -> MapObjectEntry.animName, battle_event_cmd's sampling, battle_actrecord).
  Still open:
  - animdrv's s8 delta reads;
  - partyRideYoshiMain's `(char*)partyPoseTbl` reads;
  - mapMain's `u8* anim` walk;
  - evt_shop's and evt_mobj's string-through-base reads.
  Candidates noted but not done: PartyEntry `field_70` is the partner's scale Vec (MarioWork's `scale`); mapdrv.h's
  unused `MapFileAnimEntry` duplicates MapEntryAnimData.
- The worker reports are in `build/raw3/reports/` while the worktree lives (not committed).

## Second pass over src/effect and src/driver MUST_MATCH blocks (2026-10-21, Claude Opus 5.5 orchestrating six Claude Opus 5.5 workers, n9-q-o-mmeff)

Task (Isaac): retry every MUST_MATCH block left in src/effect (241 lines) and src/driver (82) after #1099, assuming
some earlier verdicts are wrong (as the goto pass found), without repeating recorded attempts. Data layout was out of
bounds; volatile, puns, int round trips, dead stores, redundant tests, fn-ptr casts and steering helpers stay guarded.

- Method: each variant compiled with its object's ninja flags and compared with the pre-branch object by
  tools/obj_equiv.py (harness in the untracked build/mm2/: try.py, strip.py, regions.py). eff_kemuri by the
  orchestrator, the other 85 files in six disjoint groups; per-region attempts in build/mm2/agent0..6/REPORT.md.
  Every change: obj_equiv EQUIV, `build/G8MJ01/ok` recreated, `configure.py --non-matching` compile clean,
  fncast_lint and local_prototypes clean, `tools/ci.py` PASS per commit with its tools/tests step stubbed (the same 9
  environment errors as the chapters above).
- **Counts.** `grep -c MUST_MATCH` 323 -> 296 (src/effect 241 -> 217, src/driver 82 -> 79; 3 of those lines are
  comment-only, eff_kemuri2_n64). Per file: animdrv 3 -> 3, mapdrv 4 -> 3, shadowdrv 13 -> 11, eff_ibuki 4 -> 2,
  eff_recovery 6 -> 4, eff_scanning 3 -> 0, eff_uranoko 2 -> 0, eff_cloud_n64 2 -> 1, eff_fall_leaf_n64 5 -> 0,
  eff_kemuri2_n64 3 -> 0, eff_release_n64 7 -> 1; the other 75 files unchanged (eff_kemuri 33). Index
  (tools/cleanup_index.py) 14.248 -> 14.203 per kLOC (9948 -> 9913 weighted): must_match 840 -> 817, opt_pragma
  898 -> 894, goto 123 -> 121, field_0x 3938 -> 3936, address_name 2655 -> 2653.
- **Forms that matched.**
  - `x / 2.0f` for a half-size: MWCC turns it into `* 0.5f` after loop-invariant motion, so the multiply stays in
    the loop. That replaced the guarded `opt_loop_invariants off` pairs in eff_ibuki and eff_recovery (with their
    `half`/`k05` constant locals). Lead: the unguarded opt_loop_invariants pragmas in eff_bomb, eff_kemutest,
    eff_pointget and eff_opuku_jetw_n64 may yield the same way.
  - A hand-copied inline body is the inline: animEffectAsync's ag2tg search is a call to animGetAg2tgName (as
    animGroupBaseAsync, removing an unguarded goto too), spline_maketable's Newton steps are `spline_sqrtf(...)`,
    eff_scanning's built-and-copied Vec is system.h's `initializeVec` (file-local copy, as eff_puniballoon).
  - animEffectAsync's goto-ret pair is one else-if chain storing a `BOOL result`, returned once (every plain-return
    tail folds to the branchless form or merges the TRUE returns). Its last arm, the unsigned `(u32)other == 0`
    test with explicit 0/1 stores, stays guarded with `result = other != 0` under #else.
  - eff_scanning/eff_uranoko's `rad` steps are `0.017453292f * -camGetPtr(camId)->viewYaw`, eff_kemuri's form.
  - eff_cloud_n64's 30 unrolled stores are a loop bounded by `sizeof(a) / sizeof(a[0])`: an unsigned bound makes
    MWCC unroll a constant-trip loop fully (an s32 `< 30` doesn't). Lead for other hand-unrolled loops.
  - eff_fall_leaf_n64: one 0x30 record type for the header and the leaves (Entry walks `data + 1`); the guarded
    second type and Disp's casts go. The sin blend goes through the function's existing scratch `t`.
  - shadowdrv size/negSize: assigned after the vertex-format setup (statement order).
  - eff_release_n64: the five vertex arrays by name (the earlier lead), tried in the link: build ok, lbl_803A4C60
    stays defined.
- **Rejected in review (EQUIV, not applied).** mobjdrv's `f64 dx, dy, dz` fabs locals (the steering the mmsrc review
  put back; agent2/mobjdrv_f64.diff); kemuri8's B colour call to the unguarded kemuri8SetChanMatColorB (widens a
  helper without a name; A call and helpers are unguarded while B is guarded, for a reviewer); kemuri8/9 with the
  literal assigned to both size locals; eff_indirect `half = 32; half = -half / 2`; pokopi_pipo `velY -= velY`;
  imgdrv `== -1 ? FALSE : TRUE`; eff_fukidashi `c != 0U`.
- **Left, with what was tried** (5-30 shapes each; details in the reports):
  - eff_kemuri (33): FIFO 0.0/1.0 reloads need the volatile read with named constants. Without its pragma
    effKemuriDisp only swaps f31 (0.5) and f29 (int->float magic); 13 one/left/unity orders, initialisers and
    MATCH_F32 placements don't fix it. KEMURI_SIXP4's cast: the plain name puts 6.4 in f25 not f31; a `sixp4` local
    in five places doesn't help. The quad temps sit inside the FIFO region anyway.
  - Colour helpers (batten, spark, starstone, toge_flush, nozle, minigame, funemizu, energy, recovery rcC, butterfly,
    confetti, pokopi_thunder, opuku_wgun, thruhammer, sstar, recover_heart, kumokumo, kameki, horao_spell): retail
    builds each GXColor in the slot below its outgoing copy, per call. Caller-declared locals in any scope come
    before compiler temps, inline locals with them, so no in-caller form gives retail's slots. In-call compound
    literals `(GXColor){r, g, b, a}` give the code exactly; the unk_/dat_ words are MWCC's templates for them. EQUIV
    once those definitions are deleted (thruhammer, sstar, recover_heart, kumokumo; data lead, variants in
    build/mm2/agent4/).
  - Register/slot steering that more orders didn't move: eff_levelup elemIdx (~20 loop shapes), eff_ibuki comma,
    eff_kemutest dirptr, horao SetVtx2 `p`, star_power `f7`, sweat negate (all 24 decl orders), balloon `wave`,
    bomhei, arcdrv infoPtr, icondrv frames, animdrv pose cast (29 positions), mapdrv case-8 `wave`, shadowdrv colour
    slots, windowdrv FIFO interleave and `goto ret`, hitdrv hitFindElem (only an r26/r27 swap).
  - Redundant tests, int round trips, puns: mobjdrv inBattle, imgdrv ternaries/arms, eff_break language test,
    opuku_wgun `i == 0 || i == 0`, npcdrv round trips and punned stores, eff_fall/eff_nice `(s32)sfx`, eff_naniga
    ICON_ID, itemdrv status, fadedrv `+ zero`, envdrv's natural-only init.
  - Data leads (EQUIV in scratch, not applied): eff_hit/eff_magic3/eff_kumokumo_chip/eff_mobj_broken named zero Vecs
    as initialisers with the definitions deleted; eff_teresa/eff_stageclear constants defined after the code;
    eff_mahorn2's three Vec inlines go if its array is unguarded; eff_sunlight literal zero.

## Second pass over MUST_MATCH blocks in src/battle, event, motion, mario, party, manager, sequence (2026-10-21, Claude Opus 5.5 orchestrating ten Claude Opus 5.5 workers, n9-q-o-mm2)

Task (Isaac): retry every MUST_MATCH block left in src/battle (171 lines), src/event (90), src/motion (49),
src/mario (36), src/party (52), src/manager (18) and src/sequence (11) after #1096/#1099, assuming some earlier verdicts
are wrong (as #1100 and #1103 found), without repeating recorded attempts. src/effect and src/driver were #1103's.
Data layout was out of bounds; volatile, puns, int round trips, dead stores, redundant tests, fn-ptr casts and
steering helpers stay guarded.

- Method: as #1103. Each variant compiled with its object's ninja flags and compared with the pre-branch object by
  tools/obj_equiv.py (harness in the untracked build/mm3/: try.py, strip.py, dump.py, prag.py). Round 1: seven
  workers over disjoint file groups (366 regions). Round 2: three workers on the near-misses, rewriting each function
  from its asm instead of tweaking the source. Reports in build/mm3/agent1..7/REPORT.md and build/mm3/r1..3/REPORT.txt
  (subagents can't write .md files, so most round-1 reports are the orchestrator's condensed copies of their final
  messages). Every commit: obj_equiv EQUIV, `build/G8MJ01/ok` recreated, `configure.py --non-matching` compile clean,
  fncast_lint and local_prototypes clean, `tools/ci.py` PASS with its tools/tests step stubbed (8 environment errors
  in test_wave_launch's systemd scopes, as in the chapters above). prag.py removed each remaining unguarded opt_*
  pragma pair in every changed file: all 23 still change the object.
- **Counts.** `grep -c MUST_MATCH` 427 -> 383: battle 171 -> 156, event 90 -> 87, motion 49 -> 38, mario 36 -> 31,
  party 52 -> 47, manager 18 -> 18, sequence 11 -> 6. Per file: battle_event_cmd 7 -> 2, battle_audience 13 -> 11,
  battle_stage 5 -> 3, battle_seq_end 6 -> 5, ac_crane_timing 2 -> 1, sac_muki 4 -> 0, evt_npc 2 -> 0, evt_movefloor
  1 -> 0, mario_sbr 8 -> 3, party_yoshi 10 -> 6, party_cloud 3 -> 2, mot_slit 8 -> 3, mot_plane 9 -> 7, mot_jabara
  5 -> 3, mot_ship 5 -> 3, seq_load 6 -> 2, seq_logo 3 -> 2; the other 79 files unchanged. Index
  (tools/cleanup_index.py) 14.203 -> 14.122 per kLOC (9913 -> 9852 weighted): must_match 817 -> 782, goto 121 -> 115
  (mot_slit's two goto-tail chains), opt_pragma 894 -> 892, address_name 2653 -> 2642 (battle_event_cmd's natural-only
  `str_SFX_*` externs), field_0x 3936 -> 3935.
- **Forms that matched.**
  - An existing function instead of a hand copy: mot_slit's U_marioSlitContinueChk and its inline twin call
    `marioSlitButton()` (retail symbol, same file; inlined it gives the shared `state = 0` tail), and mot_plane_post
    ends with the file's planeReset inline, whose body it repeated.
  - The `sizeof(a) / sizeof(a[0])` loop bound (#1103's lead): mario_sbr's rubSet3C is a loop over unk3C, unrolled in
    full as retail has it; its 21 volatile stores and marioGetRub's volatile unk04/unk08 updates go.
  - Plain field forms once the volatile goes: JumpPosition/JumpPartsPosition clamp `mMoveCurrentPos.y` against
    `mMoveTargetPos.y` directly (no cur/tgt locals); party_yoshi's two 6-float volatile copies are
    `pl->dispOfs = vec[0]; pl->dispOfs2 = vec[1];`, the shape yoshi_use already had unguarded; battle_seq_end's
    volatile zero; seq_load's two colour reads inside `if (i == wp->cursor)` (a conditional block is never hoisted).
  - Assignment inside the condition: mot_ship's landing tests are `(height = m->position.y - sp0C) <= 0.0f ||
    height + f30 <= 0.0f`, replacing `- -f30`.
  - Honest types: seq_load's angle table is `f32 v[4][4]` indexed `[copySrc][copyDst]` (the byte-offset macro goes);
    mot_slit reads wStickDir1/2 into `s8` locals instead of through `*(s8*)&`.
  - Expressions: evt_npc_set_ry_lr is `reviseAngle((100.0f * yaw + 5.0f) / 100.0f)` with yaw read off
    `camGetPtr(4)` (evt_sub_get_coin's expression), replacing `yaw = yaw`; evt_moving_floor_init interpolates one
    `from`/`to` pair per arm; seq_logo is `width = 0.7f * (f32)romFontGetWidth(msg)`; sac_muki's colours are
    `(GXColor){r, g, b, entry->alpha}` (its guarded mukiAlphaColor inline goes).
  - Guarded arms that were already plain C: ac_crane_timing's `BattleActions* ac` (unguarded in ~10 other ac units),
    battle_audience's `star->scale = vec3_802f9744` (the natural-only `p_vec3_802f9744` pointer goes), battle_stage's
    function-scope buf/i/j/k (tou_03's form), battle_event_cmd's sound-name literals (the natural build had named
    externs nothing defines), party_cloud's clamp, mot_jabara's guarded opt_common_subs pair (not needed).
- **Not applied, for review:**
  - EQUIV but still a hack: evt_win L533 / evt_shop L219 `notCtrl || (notKey = !marioChkKey(), notKey)` (comma; the
    same shape is unguarded at statuswindow.c:195); mario.c L418 `notCtrl2 || (notKey = !marioChkKey())` (notKey
    never read, a dead store); ac_timing_a `((AcTimingADecWork*)ac->field_0x2BC)->flags` (bypasses `dec` in scope);
    battle_status_effect `(s32)list[i].type` (same as the `+ 0`); battle_unit `((PouchPartyData*)pouch)[id].currentHP`
    and battle_damage `*(s8*)&weapon->ohkoChance` (puns, but less raw than the current arms); evt_env's `&
    0xFFFFFFFF` masks, unguarded at evt_cam 111/114/144/151, evt_npc 2117, evtmgr_cmd 151, aji_06, aji_17 and gon_12
    but put back under the guard by the mmsrc review: one of the two should change.
  - mot_plane's `(s32)currSubMotionId` compare matches and is unguarded in mario_plane_cancel and ~46 other places,
    but stays guarded here as an int cast (header lead: `s32 currSubMotionId`, untested tree-wide).
  - battle_information's guarded `roll = irand(100); i = dropCount; if (roll < chance)` reads the count where retail
    does, but the store is dead on the false path; kept guarded as steering, as before.
  - battle_seq_end's halfWidth-in-argument has an unguarded twin in battle_seq_command.c:1827.
- **New leads.**
  - Inline names: party_cloud's RELOAD_MARIO goes if case 0xC's block is an inline that opens with its own
    `MarioWork* mario = party->playerPtr;`, as getHitBreatheout2 does (build/mm3/agent7/v/cl_inl3.c); the name
    `getHitBreatheout` is only inferred from the `2`, so not applied. mot_slit's `speed = speed` goes if
    slitSetWalkSpeed returns the speed (needs a new name).
  - mario_pouch's comparators almost certainly call pouchHiraganaToKatakana (inlined): two calls remove the r30/r31
    base swap and the prologue difference, leaving a c/d r4/r5 swap in the inlined loop (build/mm3/r1/pouch_v3.c;
    thousands of helper and caller orders tried). Header lead: `const char* src`.
  - evt_uranaisi's `offset` counter is an `infoId` index, as the matched evt_johoya.c johoya_data_make keeps one; right
    instructions, but colouring stays off (best 35 lines; build/mm3/r3/ura_*.py).
  - Data (EQUIV in scratch, not applied): non-const same-section definitions remove the volatile Vec/GXColor copies in
    mot_dokan, mot_damage, mot_yoshi, mot_hammer (L508-596), mot_plane's planeReset and L189, seq_load's in-loop colours
    (build/mm3/agent6/*nc*.c); compound literals with the named definition deleted in mot_yoshi, mot_damage, sac_deka
    (dat_80421e40/44/48, removes the volatile color), party_bomhei (dat_802ccf60 + its two angle regions); mot_hammer
    hitEstimate's dir vectors as compound literals give identical code, only the template placement differs;
    mario_motion's strings by name with L65 unguarded (the mmsrc review had reverted it).
- **Left, with what was tried** (per region in the reports):
  - battle/ac: crane MATCH_F64 (plain adds .rodata doubles); button_down/gauge_timing/gauge_timing2/signal_timing
    volatile Vec loop copies (Vec temp, while, in-loop initialiser, increment, literal); repeatedly's volatile clamps
    (-=, else-if, reversed compares, target reuse: constant in f1 not f0); ac_shot/button_down comma sfx (declaration
    orders, char*, before/top of loop, at call); shot_target volatile stores (5 orders); signal_timing pool order (plain
    assignments miss only by NG's addi landing last; initialisers, NG first, in-loop orders, literals); ICON_DISP_B u16
    cast; stick_keep_left's `n >= 0` (13 loop shapes, each one guard short or a doubled subic.); gauge_timing's doubled
    test (#1096's 15 shapes stand).
  - battle: event_cmd GetRGB/CheckDamage volatile args (7 and 4 orders, sibling's order); damage `s32 rate` + (s8)
    (s8 rate, callee returning s8/u8, decl orders) and ohkoChance; seq_end _lvup_spot_on volatiles, halfWidth (8
    shapes); audience case-0 zero-Vec helper, goto case4_body, ApSrcEntry volatiles; unit partyData pun (9 index forms)
    and status-block copies; kinopio's `step` (needs the unguarded opt_lifetimes off gone); stage MATCH_F32 and `+ zero`
    (7 tev shapes); menu_disp/message/status_effect/attack_audience Waku_col and colour casts (colour placement,
    compound literal: r6/r7 swap); seq comma initialisers and z/y/x locals; seq_command (s32)(s8); information irand
    temps; ac_help acHelpResolve (9 shapes).
  - sac: bakugame +/-0x80 round trip (9 shapes) and volatile args (9 select shapes); deka volatile colour; genki icon
    by-address casts (else reverses slots; the existing _drawGenkiTex inlines fix the slots, not the Vec copies);
    scissor_abs (__fabs/__fabsf/fabsf, swaps) and HSV2RGB cast; zubastar volatile count/Vec and b18 base.
  - event: evt_mobj jumpstand `(u8*)jumptable + (type << 2)` (~30 shapes, a gp/slwi swap); evt_mobj/evt_cam/evt_win
    stripped-function placeholders (no #else; dropping one changes .text); evt_win/evt_shop do/while(0) tails (~20
    shapes); evt_shop SHOP_F32 (plain re-pools .sdata2); evt_bero names[8] copies (initialisers triple the table);
    evt_mario EM_VEC/EM_STR base, searchUnder2 casts, volatile flags/Vec (7 shapes); uranaisi counter and table search
    (above; the goto keeps gswf short-lived); telop `scale = scale` (12 shapes; `scale = float_1` is one volatile
    reload off); nannpc `pos = pos` and `s64 id` (46 types and orders); evt_party f32 pos (6 positions); yuugijou
    rankMonte (scale700 now compiles in seconds, still DIFF); evt_sub one-iteration for (16 shapes: branchless or an
    extra b); lottery `t` reloads (loops don't unroll); evt_env masks (above).
  - mario: pouch base pun (above); sbr switch temp `t` (13 shapes incl. rubStick); mario.c gotos and comma tail
    (above); balloon/mario_party inline residue (no evidenced names); mario_party (s8) (6 shapes); mariost RUMBLE_MS
    is OSDiffTick's expansion (OSTime.h not on the include path).
  - motion: mot_roll volatile targetYaw/directionView (an f0/f1 or lis/lfs swap in every shape); mot_jump jumpVelY
    and the cam-follow clamp (register swaps in every shape); mot_ship `+ (zero = 0.0f)` (every literal folds) and
    the ftmp clamp (retail loads shipData[4] before SHIP_DATA's call); mot_hammer's empty attr test (dead-code
    elimination) and dir3 copy (data lead above); mot_plane's opt_propagation pair and planeReset volatiles;
    mot_jabara RDBL.
  - party: chuchu SearchState (24 statement orders x 5 `i = 0` positions; li;mr vs li;li); party_hit opt_propagation +
    volatile px/py/pz; bomhei `10.5f` temp (12 forms fold to li 10) and do/while(0) NULL retests; nokonoko double
    store; party_motion round trip, targetDist self-assignments (f5/f6 swap) and NULL retests; party_move's hand-inlined
    partyChgSpeedPose (a call auto-inlines with speed in f3 not f1; 240 body orders).
  - manager: winmgr GXSetFog casts (by value moves the colour slot 0x18 -> 0xc), volatile firstItem + `zero = zero`,
    const volatile Vec reads; cardmgr's dead timeout test (the constant stays in r30 only with it); filemgr CLUT pun (11
    shapes); arammgr volatile node flags (header lead).
  - Data layout regions (declspec objects, pool helpers, padded strings, force_active, defer_codegen) were skipped.

## Raw offsets the cleanup index didn't see (2026-10-21, Claude Opus 5.5 orchestrating five Claude Opus 5.5 workers, n9-q-o-hidden)

Task (Isaac): the raw3 pass found pointer-plus-constant spellings `tools/cleanup_index.py` doesn't count. Survey
them, type them where an honest form matches, leave the rest raw with a reason, and widen the index so it stops
under-reporting.

- **Survey.** A one-off regex (`build/raw_survey.py`, not committed) over the index's scope, minus what the index
  already counted. On fd6a9b00 it found 1,395 candidate sites:
  - 907 `(T*)(p + N)` through `u8*` locals or byte bands;
  - 200 `((T*)p)[N]` (104 byte-typed);
  - 92 `(u8*)p + N` without the outer paren, 41 `(s8*)`, 37 `(char*)`, 6 `(void*)`, 1 each `unsigned`/`signed char`;
  - 65 `(T*)p + N`, 23 `(T*)((u32)p + N)`, 22 offset-macro calls (evt_mario's EM_VEC/EM_STR, hitdrv).
  Six files held 60%: pmario_sound 232, mri_puni 170, sound 112, mapdrv 95, mri 95, filemgr 91. Some candidates are
  false positives: `->` read as `-`, value adds, real record or vertex arrays, string cursors.
- **Method.**
  - Five workers each had a disjoint file group and their headers, in nested worktrees under `build/wt/`: sound,
    mri, drivers/managers, battle/window/unit, events/effects/party/rels. Each change was compared with a snapshot
    of the pre-branch objects (`build/base_obj`, `tools/obj_equiv.py`), and the whole tree after each header change.
  - The orchestrator reviewed and cherry-picked the five branches, sent four fixes back (below), and did the last
    unit itself.
  - On the merged branch: `build/G8MJ01/ok` recreated, `obj_equiv --tree` equivalent for all 1,230 objects, and the
    full `--non-matching` ninja compiles every object (only the dol link fails, as usual). fncast_lint and
    local_prototypes pass.
  - `tools/ci.py` passed before each commit with its tools/tests step stubbed (the same 9 environment errors as
    above), and `--base fd6a9b00` passes over every merge.
- **Counts.**
  - Survey sites: 1,395 -> 281, judged against the old index. Of the 281, about 130 are survey false positives,
    and the rest are below or were already recorded.
  - Old script (`cleanup_index.py` as on fd6a9b00): index 14.203 -> 15.196 per kLOC (9913 -> 10607 weighted).
    raw_offset 230 -> 235, field_0x 3936 -> 4625, address_name 2653 -> 2648. The rise is the old blind spot: the
    typed reads now spell `field_0xNN`, which the old script counts, while the offsets they replaced counted
    nothing.
  - New script: index 17.415 -> 15.546 per kLOC (12155 -> 10851 weighted), **raw_offset 1351 -> 357**.
- **The index change** (`--allow-protected`). `raw_offset` is one pattern with four branches; the docstring and
  `tools/tests/test_cleanup_index.py` describe and test them:
  - `(u8*)p + off` with or without the outer paren;
  - the s8/char/unsigned char/signed char casts (a char cast's `+ 1` is a string cursor and doesn't count);
  - `((u8*)p)[N]` with a constant N > 0;
  - `(T*)(p + N)` with N not 1 (`+ 1` there is a step past a header record, as in fontmgr's `pfh + 1`).
  `+=`/`++` and `(T*)(off + (u32)data)` relocations don't count. Offset macros reuse the pattern, but evt_mario's
  EM_VEC/EM_STR are defined in both MUST_MATCH arms, so only their two #defines count (the defined-twice rule).
  Known over-counts: n64 vertex tables offset by an element count (`(effVtxArray*)(flowers_v + 0xb6)`, 3 sites)
  and file-relative offsets (`(u8*)header->data + chunk.offset`), which the old pattern also counted in its own
  spelling. Not counted: `((T*)p)[N]` for a non-byte T, and `band + N` with no cast.
- **sound (w1).**
  - `SoundStreamEntry` 0x38-0x64 are `field_0xNN`, `loadBuffer` is `u8*[2]` (the old `unk30` is `loadBuffer[1]`),
    `SoundStream` gets 0x1C-0x24, `StreamHeader` its two ADPCM infos, and `SoundWork` its arrays. The `u8* e/ep`
    walks are typed.
  - pmario_sound: `PaperSoundEnvWork` moved into the header (name kept) and types the env slots, the SFX fade and
    the BGM fade. `PaperSoundEffect` absorbed its `PaperSoundEffectDetail` view, which is gone.
  - Left raw:
    - SoundEfxSetPan/SetSrndPan (the folded index, as recorded);
    - psndENVOn_f_d's `psnd + ofs` with `lhzu`;
    - psndENVOff_f_d/psndENV_vol's `(u16*)&psnd.unk18 + i` (only the no-op cast matches);
    - BGMListEntry `params` bytes (a packed u32 in data initialisers);
    - calc3D's `((u8*)&res)[N]` (bytes of a packed return);
    - psndPushGroup's `tbl` (retyping the static is a data definition).
- **mri (w2).**
  - Every `u8* p = (u8*)puni` alias in mri_puni, mri, mri_03, mri_13 and mri_16 is gone; functions read through
    `PuniData*`.
  - PuniData `field_0x1E8` (s32) and `Vec field_0x2AC`; `effect` is an `EffectEntry*` (the s32 gave cmpwi where
    retail has cmplwi); `healIconTime` is s8.
  - `*(Vec*)(unk21C + 0x344)` is `hitNormal`. punidrv walks `PuniTexInfo*`.
  - mri_03's `dir` local is s16 (it is stored to `treeId`; a u16 moves a `lis`). **Reviewer: a local's signedness.**
  - Left raw: Hagure's `(s32)((u8*)puni + 0x220)`, a fake MarioWork base for effFukidashiEntry (its position lands
    on `field_0x2AC`). The container-of form `&field_0x2AC - offsetof(MarioWork, position)` also matches and was
    not used.
- **Drivers (w3).**
  - fileGarbageDataAdrClear uses the FileType views and AnimData like its inverse.
  - mapdrv: mapMain's `u8* anim` walk is `MapEntryAnimData*`; the animation_table track lists, tracks and keys are
    types in mapdrv.h; MapEntry.vcdTable is `MapVtxFmt*`.
  - animdrv: AnimData's delta streams and frames are typed records (`AnimFrame` moved into the header, the
    `ANIM_FRAME_*` macros are gone, and `__OSs8tof32(&src->field_0x1)` replaces the s8 delta reads).
  - EnvironmentWork 0x70 is split. memory, extdrv, parse and mobjdrv: offsets through their structs.
  - New types, named after the inline or member that consumes them:
    - `MapAnimTexMtxKey/Item`, `MapAnimAlphaKey/Item`, `MapAnimLightPosKey/Item`, `MapAnimLightColorKey/Item` and the
      five `MapAnim*List`;
    - `AnimPoseVecDelta/VisDelta/NodeDelta/TexDelta` (after `animPoseAdd*Delta`) and `AnimFrame2` (after
      `frames2`).
  - mapAnimColorObj animates the material's texture matrices, so its types are `MapAnimTexMtx*`, after the
    MapMatTexMtx they write.
  - Sent back in review: mapAnimColorObj's `(&entries[idx].transX)[k]` (indexing past a member into its
    neighbours) is the raw `(u8*)obj + idx * 0x1C + 0x2C + k * 4` again. Retail indexes transX..rotation as five
    channels, and four other typed forms differ.
  - Left raw:
    - mapSearchAnimObj's pair walk (r28/r29 swap);
    - _mapDispMapObj's packed-list walk (r24/r25);
    - the `vcd[N] + 4` count-word skips;
    - `+0x178` past MapEntry;
    - the biased MapVtx walks;
    - hitdrv's display-list byte stream;
    - envGlare's 8-byte walk;
    - effdrv's `count[1]` track array;
    - mobjdrv's MUST_MATCH base reads;
    - error_handler/unk_0x8025de70's stack-frame walk;
    - seq_demo's THP buffer.
- **Battle and windows (w4).**
  - Sampling buffers are `BattleSampling` in battle_detect, battle_stage_object, battle_item_data, the barriern,
    piders and bros units, and (orchestrator) battle_enemy_item's six item checks.
  - battle_stage_object's rank data are `BattleStageNozzleData`/`BattleStageFallObjectData` (named after the data
    symbols; definitions untouched), and `ObjectFallAttackWork` (after `_object_fall_attack`) types its
    BattleAlloc(0xEC).
  - Other new members: BattleCameraWork `Vec field_0x2C..0x74`, `BattleStageLightSpline` (after `spline`), and
    BattleACDispParams' 0x10-0x13 bytes as `field_0x10`, `dg`, `db` and `field_0x13` (`dg`/`db` from the old
    BakuGameGaugeColor).
  - Black peach's pose matrices are indexed as Mtx.
  - Retypes from their loads:
    - `BattleWeapon.icon` u16 and `field_0x6D` u8;
    - `blurColor` u8[8];
    - `mStylishCommandMultiplier` u8;
    - `BattleAudienceMember.field_0x18` s8.
  - Stale recorded lever: battle_audience's `audlocal` matches as `impendingBonuses`.
  - Sent back in review: `(GXColor*)&dispParams.field_0xC` views (a pun on an s32 outside MUST_MATCH). The colour
    bytes are back to their original raw reads: a packed RGBA s32 the ACs store as constants.
  - Left raw:
    - _Nozzle_Change_Check and the act-record band (skipped);
    - _combine_nozzle_weapon's MUST_MATCH `+0x80` bytes;
    - sac_zubastar's `b18` (MUST_MATCH);
    - win_badge/win_log as recorded;
    - win_item's `entry + 0x2E8` (retested `skipList[i]`, differs);
    - win_root's `field_40[0x1010]` (recorded) and its mail comparators' `((u8*)a)[1]` (u8 pairs in `field_26E`);
    - battle_event_cmd's `BattleCommand* tbl` local (needed).
- **Events, effects, party, rels (w5).**
  - mot_roll indexes `rollData`. mario_sbr, mot_slit and mot_stay read wStickDir1/2 as the s8 members they are.
  - gor_02's door work is two GXColors.
  - evt_cam keeps its rumble time in `userdataEx[0/1]`, read back as `(u32)` (a zero-extending value conversion).
    **Reviewer: this is a deliberate value conversion, not a pun.**
  - eff_pokopi_thunder indexes its rows, and eff_snow_dust/eff_toiki pass their vertex tables by name (the shared
    `lbl_` bases are gone; the definitions are untouched). eff_bush reads `clr0`/`tex0`.
  - PartyEntry `Vec field_C4`; party_vivian/bomhei/mario read `timer` and `field_2A` instead of `field_22[2]`.
  - nok_01, usu_00 and gor_03's message callbacks take the smartEntry (the jon precedent). YuunpcWp `s32 field_08[3]`.
  - The 0x38-byte block at 0x8 of the Monte entries in gor_03 and yuu_00..03 is a sub-struct (`MonteField8`,
    `YuuMonteField8`, `MonteEntryField8`, `Yuuminigame4WorkField8`, after the `BattleWorkField18C70` precedent).
    Retail holds `entry + 8`, and the parent-member forms differ. LectureButton has the same block, not touched.
    **Reviewer: names derived from placeholders.**
  - Sent back in review:
    - the `upsidegara` (s32[2][3][8]) and `footmarkBuf` (Footmark[2][0x50]) retypes: both matched with the bytes
      unchanged, but data definitions are off-limits, so their sites stay raw;
    - eff_kiss's GXColor view of a u32 (a pun);
    - evt_movefloor's `(s32)` read of an OSTime low word (an int cast).
  - Left raw:
    - evt_mario's EM_VEC/EM_STR (MUST_MATCH, a base over separate .rodata objects). Lead: they look like anonymous
      local-initialiser objects, a data-layout change;
    - evt_mobj's shared-base reads (recorded);
    - eff_splash's bytes of a u32 scalar;
    - party.c's partyPoseTbl (folded index, retested);
    - end_unknown, evt_lect_dou and party_yoshi as recorded.
- **Stale recorded reasons** (retested, now matching):
  - n9-rd-puni's "five functions can't be retyped";
  - n9-q-puni2's Hagure and `p + 0x56C/0x570` notes, and the `PuniData.effect` s32 note;
  - mapMain's `u8* anim` walk;
  - mapSetPolygonVtxDesc's `+ 0xA4` base;
  - mapSetTevCallback's indexed store (through a `MapWork*` local);
  - animdrv's "no struct" for the s8 deltas;
  - battle_audience's `audlocal`;
  - gor_03's kuji callback "unknown object".

## MUST_MATCH guarding made consistent across the tree (2026-10-21, Claude Opus 5.5, n9-q-o-consist)

Task (Isaac): #1103 and #1104 found the same match-trick shapes guarded in some files and plain in others (`&
0xFFFFFFFF`, the comma form, `(s32)` on motion ids, assignment inside an argument, the `(u32)x == 0` 0/1 tail,
volatile reads, self-assignments and dead stores outside MUST_MATCH). Survey every family, decide each once, and apply
the decision everywhere: guard a real trick with a plain `#else`, use the plain form wherever it gives the same object,
and drop guards from families that are natural C.

- Method (scratch, untracked, build/mmcons/): survey.py walks every game C file's libclang AST (parsed with
  -DMUST_MATCH) and counts each family's sites, guarded when the line is in a MUST_MATCH-only arm or the site comes from
  a macro defined in one. try.py compiles a file with its object's ninja flags and runs tools/obj_equiv.py against
  objects saved from the base build (or compiles it without -DMUST_MATCH). sweep.py tried every MUST_MATCH region's
  `#else` alone. casts.py removes redundant casts one file at a time, all together first and then cast by cast, keeping
  only removals that stay EQUIV. edits.py tries a plain form, keeps it when EQUIV and guards the site otherwise.
  armdiff.py token-diffs the two arms of every region, to find guards whose only difference is a family ruled natural.
  libclang reports byte offsets; the first cast pass sliced Python strings and so skipped casts after any Japanese text
  in a file (it failed safe; a second pass on bytes found 23 more). Every removed cast was checked to be a cast token.
- **Decisions, one per family** (the Standing rules carry the short form):
  - Comma sequencing, `(x = f(), x)` and `(0, x)`: a trick. Guarded; plain where the plain form matches.
  - `& 0xFFFFFFFF`: every site masks a 64-bit time to a word, and the `and x, -1` is in the asm. Plain C; evt_env's
    guard went.
  - Assignment inside a call argument: plain C. battle_seq_end's guard went (its twin in battle_seq_command was plain).
    The seven still under a guard are part of a comma form or a steering block (unit_barriern, mapdrv, eff_balloon).
  - Signedness, width and enum-to-int casts that change a value or a comparison (`(s32)currSubMotionId >= 0xC`): plain
    C; mot_plane's guard went. One that changes nothing in C is a cast for the compiler and stays guarded (reviewer):
    a `(u32)`/`(s32)` on an `== 0`/`!= 0` test (animdrv's three `(u32)fileId == 0`/`(u32)other == 0`, memory.c's
    `(s32)waitsync != 0`) and battle_damage's `s32 rate` re-narrowed by `(s8)`; none matches plain. seq_logo's stays
    for its redundant `-1` branch.
  - A cast to the value's own type: a no-op in C. Removed where the object is the same (375, counting the 61
    `(s32)currSubMotionId` casts that became same-type when the field was retyped; 3 enum-to-int casts that made no
    difference went too); guarded where it isn't (11 int, 2 float). A cast to the callee's parameter type stays as
    written (msgdrv's `kanjiGetWidth((u16)glyph)`, himo's spline_getvalue, whose pointer locals are s32 only under
    MUST_MATCH).
  - Round trips through another type (`(s32)(f32)`, `(s32)(s64)`, pointer-int-pointer): guarded. Float-to-int-to-narrow
    (`(u8)(s32)f`) is the standard conversion and stays plain.
  - Dead stores and self-assignments: guarded, or removed with their locals where the object is the same. A write-only
    local that holds a call result (the call is real) is plain. Unused locals whose initialiser is a .rodata template
    (sac_bakugame's `order`/`home`, npcdrv's `zero`, party_hit's `n`) are data layout, not touched.
  - Redundant tests and ternaries on a 0/1 test (`c ? 1 : 0`, same arms, `x != 0 && x > 0`, `x ^ 0`): guarded. An if/else
    storing 1/0 is plain C and replaced the ternary where it matched (gor_03, eff_hit). `mask ? 1 : 0`, `!!x` and
    `x != x` NaN tests are plain.
  - Type puns: const-stripping and volatile reads of named constants are tricks (mot_jabara's JABARA_F32 is now guarded
    like SHOP_F32, WINMGR_F32 and TOU_F32; eff_cloud's sqrtf test); 39 `*(GXColor*)&` casts of const GXColor objects
    matched without the cast and went. Puns forced by a declared type (u8 arrays, s32 lwData/userdata words, u32 colour
    data, x/y/z floats read as a Vec) are typing debt: plain until the field or object is retyped. Float bit tests are
    the SDK's fpclassify idiom.
  - Function-pointer casts: a call of a named function through a cast is guarded, including a cast to its own type to
    stop inlining (rsh_05's MOAMOA_POSE_CHANGE and moamoa_pose_main, now guarded). Casting a value (evtGetValue, a void*
    field, a module prolog) to a function type, or a function to an API's callback type (qqsort comparators,
    OSSetErrorHandler, drawFunc fields), is plain.
  - Volatile: guarded as a codegen device; plain on hardware registers (`*(volatile u32*)0x800000F8`) and on fields a
    callback writes. arammgr's ARAMWorkNode.flags and ARAMWork.garbageBusy are now volatile in the header; the guarded
    NODE_FLAGS/GARBAGE_BUSY casts and aramMgrToAram's unguarded cast of the same field went. envdrv's GXPosition3f32
    redefinition (statement-order steering) is guarded.
- **Survey, guarded / unguarded sites** (base 9e5a105c -> this branch, same script; SDK header-macro expansions and
  src/sdk excluded): comma 22/17 -> 28/0; self_assign 10/1 -> 11/0; same-type cast 4/319 -> 19/8 (the eight: msgdrv's
  five parameter casts and himo's three); redundant ternary 4/11 ->
  11/0; redundant op (`x ^ 0`, `x | 0`, `x + 0` outside an index) 2/10 -> 6/6 (the six are index `+ 0`); int round trip
  6/68 -> 8/51 (natural conversions); dead store (write-only local) 4/67 -> 4/55 (call results and templates); mask
  2/18 -> 0/20; fn-ptr cast 127/56 -> 131/52; pun 176/1391 -> 236/1295 (+60 guarded are JABARA_F32's uses); volatile
  266/14 -> 262/9 (hardware, and macros defined in both arms); assignment in argument 8/18 -> 7/18.
- **Other forms that matched.** The `#else` sweep found three regions whose plain form now matches: win_log's `new_var`
  base alias (all three regions), unit_party_chuchurina's lbl_8037EC10 weapon offsets (the named weapons share the base
  register), himo's spline_getvalue call. evt_win and evt_shop trade their do/while(0) `break` tails for the guarded
  comma form statuswindow uses. cardmgr's ten busy-wait loops, rsh_03's colour copy, npcdrv's homing test and
  eff_ibuki's second angle step are plain. MarioWork.currSubMotionId is s32 (all 23 users EQUIV). Twelve dead stores
  went with their locals (sac_deka, battle_acrobat, usu_00, evtmgr_cmd, winmgr, nameent, win_mario, evt_yuucounter,
  seq_e3's shadowed `keys`).
- **Counts** (tools/cleanup_index.py, base 9e5a105c -> this branch): index 14.122 -> 14.155 per kLOC (9852 -> 9875
  weighted); must_match 782 -> 804 (37 regions added, 15 removed, in 35 files); field_0x 3935 -> 3938 (lines duplicated
  into guards); address_name 2642 -> 2640; fn_ptr_cast, goto, opt_pragma, raw_offset unchanged. The rise is the 37
  tricks that were in the code unmarked (comma forms, no-op and round-trip casts, redundant ternaries, `x ^ 0`, dead
  stores, const-stripping reads, no-inline casts, envdrv's vertex macro), each tried plain first; the index doesn't count
  the 378 casts and 12 dead stores removed.
- **Left, for a reviewer.** tools/fncast_allowlist.json's two rsh_05 entries are stale (tools/ is protected; `python3
  tools/fncast_lint.py --stale` lists them). The 11 guarded same-type int casts (four togekuri `return (u16)mid`,
  battle_seq_command, icondrv, evt_badgeshop, winmgr x2, party.c, party_motion) may each be a mistyped local or prototype
  in our source rather than a trick: `s32`/`int`/`u32 mid` in togekuri differ, and PartyEntry.currentMemberId as u8
  changes ten rels. mario.c's marioDispBlur keeps its dead sinv/cosv under an unguarded `opt_dead_assignments off`
  (pragma family, not touched). Data-template dead locals as above.
- Verified: `ninja` recreates build/G8MJ01/ok; every game object (1,019) compiles under `configure.py --non-matching`;
  fncast_lint and local_prototypes pass; tools/ci.py PASS before each commit with its tools/tests step stubbed (9
  environment errors in test_wave_launch's systemd scopes, as in the chapters above).

## Optimisation pragmas, second pass: natural shapes instead of pragmas (2026-10-21, Claude Opus 5.5 orchestrating eight Claude Opus 5.5 workers, n9-q-o-pragma2)

Task (Isaac): the ~890 optimisation pragmas left after #1096 were each "needed", but #1103 showed some exist only
because the C around them has the wrong shape. For each scoped pragma: delete it, read the object diff
(tools/obj_equiv.py), look for a natural source shape that gives retail's code with the optimisation on; apply it if
it matches, otherwise keep the pragma and record what was tried. Volatile, puns, int round trips, dead stores,
redundant tests and steering temps stay guarded: a pragma beats an unguarded hack.

- **Method.** Harness in the untracked build/pr2/ (try.py compiles a variant with its object's ninja flags, every
  object a source builds, and compares with obj_equiv against a snapshot of the pre-branch objects; ddiff.sh prints a
  normalised asm diff; prag.py, sweep*.py; kinds.py counts opt_pragma per kind with cleanup_index's own regex). A
  re-run of #1096's single-removal test first: nothing new (the only hits were `on` closers whose removal just widens
  the region). Then eight workers on disjoint file groups (433 lines in 115 files; reports in build/pr2/v1..v8/REPORT.txt),
  the orchestrator reviewing every diff, then mechanical sweeps over every remaining region for the levers found on the
  way (`int` locals, all at once and one at a time; compound assignment; `*args++`; `* 0.5f` <-> `/ 2.0f`), then the
  header leads the workers couldn't touch. Every commit: obj_equiv EQUIV for each changed file, `build/G8MJ01/ok`,
  `--natural` compile, fncast_lint and local_prototypes clean, tools/ci.py PASS with --allow-pragma and its tools/tests
  step stubbed (the same 9 environment errors in test_wave_launch as the chapters above). After the two header
  changes, every object was also built under `configure.py --non-matching` (all compile; only the link fails).
- **Counts** (build/pr2/kinds.py, same regex and files as tools/cleanup_index.py; da911753 -> this branch):
  opt_pragma **892 -> 777**. opt_propagation 176 -> 126, opt_lifetimes 109 -> 90, opt_common_subs 99 -> 71,
  opt_dead_assignments 30 -> 26, opt_loop_invariants 16 -> 6, opt_strength_reduction 4 -> 0, global_optimizer 1 -> 0;
  dont_inline 371, defer_codegen 37, optimize_for_size 30, inline_max_size 6, inline_max_total_size 6, fp_contract 2,
  auto_inline 2, optimization_level 2, inline_depth 1 unchanged. Index (tools/cleanup_index.py) 15.506 -> 15.325 per
  kLOC (10819 -> 10689 weighted): opt_pragma 892 -> 777, must_match 810 -> 807, raw_offset 357 -> 351
  (eff_opuku_jetw_n64's byte-offset casts); address_name, field_0x, unk_name, goto, fn_ptr_cast unchanged.
  `git grep -c MUST_MATCH` over src/rels/include 957 -> 953.
- **Shapes that matched** (each EQUIV with the pragma gone):
  - Half sizes as divisions: `half = one / 2.0f; negHalf = -one / 2.0f` (eff_bomb), `min = -size / 2.0f; max = size / 2.0f`
    (eff_kemutest), replacing `0.5f` constant locals and opt_loop_invariants, as #1103's eff_ibuki/eff_recovery.
  - A loop written plainly: eff_opuku_jetw_n64's column index computed inside the row loop, `&table[j * columns]`,
    `rowPtr[col]`: MWCC's own strength reduction and loop-invariant motion then give the original's running sums, so
    opt_strength_reduction, opt_loop_invariants, seven locals and the byte-offset casts go. battle_audience's
    BattleAudienceJoy_Sub compares the roll against a running threshold (`limit = a; limit += b; limit += c`) and
    loses three pairs and the BattleAudienceJoyAdd2 helper. eff_recover_heart_n64 computes `life % 10`/`life / 10` in
    the branch that uses them (global_optimizer off). animdrv: `GX_TEXMTX0 + i * 3u` (opt_strength_reduction).
  - Compound assignment `x->f += d` for `x->f = d + x->f` (and `-=`): winmgr select_main, win_log winLogInit,
    party_vivian (two functions), party_motion behind_mario_fly, eff_kemuri5_n64, eff_snow_dust_n64, eff_stardust_n64.
  - `int` instead of `s32` (which is `long` here): GX index counters (envdrv envSetYami, mapdrv mapSetPolygon and
    mapSetPolygonVtxDesc: `counter + k` arguments recomputed per call as in retail), and elapsed-time locals (mri
    evt_mri_door_naname_func `int elapsed`, as evt_npc and evt_item already had): there it also removes the guarded
    `curStep = elapsed` argument assignment, and in mri_puni evt_puni_jump_position and muj_12
    muj_item_move_3d_position the guarded duplicate divide / `(s32)` casts with their MUST_MATCH regions. The other
    direction too: evt_badgeshop's `int i` -> `s32 i`, eff_sandars' `BOOL inBattle` -> `s32` (its callees' type),
    bom_01 depthDisp `u32 mtxId` (the GX type, as gor_10's copy).
  - Event arguments read with `evtGetValue(event, *args++)` (the tree's usual idiom): evt_bero (three functions),
    evt_batstage, evt_kinopio, unit_faker_mario (two), unit_boss_batten_leader, unit_party_yoshi, unit_mario,
    battle_event_cmd (btlevtcmd_check_battleflag, btlevtcmd_PreCheckDamage), battle_seq_command _get_msg, battle_seq_end
    _set_confetti_control_evtid, battle_stage_object _set_mobj_rotate_x. In single-argument functions the last
    increment is dead; recorded as house style, not as a dead-store trick (reviewer to confirm).
  - One local per value, or one value per local: sac_zubastar zubastar_get_kouten (if/else for the ternaries and copy
    blocks), database setupDataBase (battleInfoId), envdrv envGlare (a second counter), ac_button_down, and
    eff_stardust (split; its declaration order came from tools/rasim.py search), and the
    reverse in evt_npc _intplGetFloat (clamp the parameter), evt_sub (one `t`), evt_party, tik_05 (write back into
    `angle`), party.c partyMakeDispDir and partyGoodbyeMain (reuse `angle`/`py`).
  - An existing helper instead of its hand expansion: party.c party_damage_return calls partyChgPoseId(party, 8/9).
    Reading through an existing local: ac_stick_rotate's two Disp functions use `ac->` (opt_propagation).
  - Constant stores straight through the getter: mario_balloon `marioGetPtr()->field_0x260 = 0`, mot_slit.
    evt_eff_fukidashi's case 0 written like its other cases; party_move partySetDispDirToMario's yaw read in place.
  - Header fixes (not open to the workers): effCoinFukidashiEntry is `(type, x, y, z, coinNum, count)`, the order its
    body stores them (ints and floats use separate registers, so no register moves); mobj_recovery_blk's pragma and
    its y2/z2/x locals go. BattleUnit.mStatusFlags is u32 (it holds bits 28/29), with BtlUnit_Check/On/OffStatusFlag
    taking u32: btlseqEnd's danger/peril tests read the field directly and its opt_common_subs pair goes.
  - Also: evt_yuutitle's blends written `to * t + from * (1 - t)` drop a MUST_MATCH region of no-op casts (its pragma
    stays). Stale comments fixed beside kept pragmas: evt_item, eff_release_n64, eff_ripple_n64.
- **Rejected in review (EQUIV, not applied):** party_motion_homing_fly's opt_loop_invariants with `u32 id` plus a
  `s32 sid = id` copy (a register-steering temp; homing_walk's existing `sid` copy is now under MUST_MATCH);
  partyPreDisp with `(s32)party->field_2A` or an s32 local; battle_stage getSpline with a `0.0f` literal (adds a pool
  entry); effBreakDisp only with the volatile MATCH_F32 read widened; mapSearchDmdJointSub2 returning the recursive
  call (drops retail's result test); gor_00 gundan1000tai_init without opt_lifetimes, whose only matching
  declaration order (rasim) interleaves the two halves' locals; party_move's four target-distance functions calling
  the existing L_getTargetPos (exact instructions, one callee-saved register too many: strong evidence the original
  called it).
- **Left: 164 regions,** one line each with what was tried in [docs/PRAGMAS_KEPT.md](PRAGMAS_KEPT.md). By cause:
  - The sin/cos polynomial (about 51 regions in some 19 files: bom_01, eki_00, gor_10, hei_10, pik_02, rsh_05, rsh_06,
    tik_05, tou_02, envdrv, npcdrv, evt_item, evt_sub, eff_damage_star, eff_fall, eff_gonbaba_breath, eff_starpoint,
    eff_mobj_broken): retail holds the coefficients in callee-saved FPRs (propagation off) and each Horner accumulator in
    one register (lifetimes off). Every copy carries the same pair; literal coefficients make no difference. Needs a
    study of its own; the original may have been built with these settings for that code.
  - Named pool constants (about 27): a function reads a named .sdata2 constant twice and CSE keeps the first load
    (eff_kemuri, eff_mist, eff_mugi, eff_funemizu, kemuri4, kemuri10, butterfly, blow, breath_fire, koopa_motion,
    chuchurina mono_disp, ...). A literal gives the code but adds a pool entry; data layout, out of scope (lead).
  - Register colouring with no natural order (about 40; rasim searched where it mattered), large functions that reshuffle
    wholesale without the pragma (12: msgAnalize, mapMain, envDisp_DepthOfField, BattleSetStatusDamage, btlseqEnd's
    lifetimes, ...), and 5 that a guarded MUST_MATCH trick depends on.
  - eff_pointget effPointGetDisp: with the outer-loop invariants moved before the loop, one f25/f27 swap remains:
    retail's `f22 * 0.5` is hoisted by the backend (ranked after the int->float magic), ours by the frontend first.
- **Not in this pass:** dont_inline (371), defer_codegen (37), optimize_for_size (30, files built -O4,p), inline_* and
  auto_inline. They choose inlining, emission order or size-vs-speed, which no source shape changes.
- **Leads.** `int` elapsed/step locals for other MUST_MATCH regions: these files have both an `elapsed` local and a
  MUST_MATCH region (not checked): evt_nannpc, evt_telop, muj_20, gor_03, evt_lect_dou, win_00, ac_crane_timing,
  ac_gauge_timing2, itemdrv, fadedrv. The named-constant regions above, if the pool
  layout rule ever allows a literal. tou_03 screen_tev and battle_stage tou_gamen_screen_tev are one function. A worker's
  technique worth keeping: run tools/mwccdbg.py on the source with and without the pragma and diff the frontend dump
  (`frontend-02-ast-final-code.txt`); the new `EASS @N` temps are the subexpressions the optimisation starts sharing,
  which points at the line to rewrite (build/pr2/v5/fd.sh).

## The sin/cos polynomial family: no natural shape removes its pragma pair (2026-10-21, Claude Opus 5.5, n9-q-o-sincos)

The task: study the 51 kept pragma regions that wrap the inlined sine/cosine approximation as one problem, and
remove the pair wherever a natural shape matches. Nothing matched, so no source changed. This chapter and the new
family section at the top of [docs/PRAGMAS_KEPT.md](PRAGMAS_KEPT.md) record what was tried.

- **The copies.** `sin x = x(1 - 0.16605x^2 + 0.00761x^4)` and `cos x = 1 - 0.4967x^2 + 0.03705x^4` on [0, pi/2]
  (Abramowitz and Stegun 4.3.96/4.3.98), reduced from [0, 2pi) by compares against pi, pi/2 and 3pi/2, with the Horner
  steps repeated in every branch. Retail has it in 30 functions in 19 files: bom_01, eki_00, gor_10, hei_10, pik_02,
  rsh_05, rsh_06, tik_05, tou_02, envdrv, npcdrv, evt_item, evt_sub, eff_damage_star, eff_fall, eff_gonbaba_breath,
  eff_starpoint, eff_mobj_broken and eff_small_star_n64 (cos only), plus mario.c's unused copy. The kept regions are 24
  opt_propagation + 26 opt_lifetimes + npcDisp_blur's optimization_level 2 = 51, plus marioDispBlur's
  opt_dead_assignments. Our source writes it in several shapes: hand-expanded with f0..f7-named locals (evt_sub,
  eff_gonbaba_breath, gor_10), or file-local static inlines (npcSinf, effFallSinf, item_sin_approx, tikSinf,
  marioDispBlurSinf, ...). Every shape needs the same pair, so the pair doesn't come from the shape.
- **What the pair reproduces** (evt_sub_get_coin's asm read in full). The coefficients are loaded once before the
  first compare and held in f2-f4 across all four branches. Each Horner step overwrites the accumulator in place
  (`fmsubs f4,f4,f0,f3`), even in branches where f1 is free. The .sdata2 entries are unnamed literals in first-use
  order, coefficients before pi, and sin's and cos's 1.0 share one entry. So the source assigned literals to locals
  before the compares, and retail held each local in one register without propagating it.
- **No outside name.** Nothing in symbols.txt, libs/, include/ or MSL is this polynomial. `sinfd`, `cosfd`, `sincosf`
  and `movePos` call MSL `sin`/`cos`. The constants don't occur in SPM's main.dol or in ~/ttyd-refs. No shared inline
  was introduced (it would need an invented name) and the copies were left in place. Folding them into one helper
  wouldn't remove a pragma either.
- **Tried in isolation** (build/sc in this worktree, untracked: a test function compiled with the project's GC/2.6
  flags and compared with the pair's object). All of these give the pragma-less code: hand-written, `static inline`,
  `inline` and auto-inlined `static`; a statement macro and a whole-block macro; `register` locals; double literals;
  one-expression Horner; an inline taking the coefficients as arguments; the polynomial in a loop with the
  coefficients set before it. File-scope `static const`, plain `const` globals and local `static const` coefficients
  are folded like literals. The pair around the inline's definition has no effect (the caller's settings apply). A
  const array keeps the loads but still differs, and it changes data. `#pragma optimization_level 2` reproduces the
  pair in isolation.
- **Tried in the real files** (each file compiled with its own ninja command, then compared function by function
  with the built object):
  - **Lifetimes half only.** Dropping `opt_lifetimes` while keeping `opt_propagation off` fails in every file tried
    (60 to 246 lines). The accumulator's webs split onto the lowest free FPR, which is the opposite of retail's
    in-place colouring.
  - **optimization_level 2 for the pair.** Matches npcdrv (npcCheckHitMarioSide), the setting npcDisp_blur already
    uses, but not the other 17 files.
  - **In-file `#pragma global_optimizer off`.** Changed nothing.
  - **File-wide `-pragma "opt_propagation off" -pragma "opt_lifetimes off"`** with the region pragmas stripped:
    - every function matches in pik_02, tou_02, eff_starpoint and evt_item (10 regions);
    - evt_sub, hei_10 and rsh_06 break one function, gor_10, rsh_05 and eki_00 two, envdrv three;
    - bom_01 breaks 6, tik_05 7, npcdrv 23 and mario 16, and the four effect files break almost every other
      function.
    So a file-wide setting isn't the original build in general. Moving the 10 regions into configure.py would
    hide the setting rather than remove it, and it needs `--allow-configure`. Not done; it's Isaac's call (a lead).
- **Two copies need only the lifetimes half.** eff_mobj_broken and eff_small_star_n64 read their coefficients
  through named `extern const` .sdata2 objects defined after the code, which MWCC can't fold. That is those units'
  pool layout, not a shape for the others.
- **Reading.** marioDispBlur keeps only the compare skeleton of an unused copy (dead-assignment removal didn't run).
  The 30 retail functions look like code built with the global optimiser's propagation, lifetime and dead-assignment
  passes off, not like a source shape. Whether the original had pragmas around these functions or something else
  turned those passes off can't be decided from the bytes. The pair stays, with its reason, as the honest description.
- **Counts** (tools/cleanup_index.py, 14ea64ca and this branch): 15.325 per kLOC (10689 weighted), opt_pragma 777,
  must_match 807, raw_offset 351, unchanged; sin/cos regions 51 -> 51. `git grep` over src/rels/include: 329
  optimisation pragma lines (`opt_*`, `optimization_level`, `global_optimizer`), unchanged.
- **Checks.** Docs only. `build/G8MJ01/ok` recreated; fncast_lint and local_prototypes clean; tools/ci.py PASS with
  --allow-pragma and its tools/tests step stubbed (the same 9 environment errors as the chapters above:
  test_wave_launch's systemd scopes, and no `elftools` for test_reloc_value_check). The ratchet baseline was
  initialised from this worktree's unchanged 14ea64ca build.

## Integer local types, argument walks and compound assignment on every MUST_MATCH region (2026-10-21, Claude Opus 5.5, n9-q-o-intlead)

Task (Isaac): #1107 found that `int` vs `s32` (`long` here) locals, `*args++` walks over event arguments and compound
assignment produce code some guarded forms imitate. Try those levers on every remaining MUST_MATCH region in src/
and rels/, alone and combined; where the natural form matches, it becomes the only version. Data layout and guarded
volatile/pun/function-pointer-cast tricks stay.

- **Method.** Harness in the untracked build/il/ (regions.py lists every region with its enclosing function; try.py
  is #1107's, comparing with obj_equiv against a snapshot of the pre-branch objects; fdiff.py counts one function's
  normalised asm diff; combo.py runs every combination of per-line alternatives; gen*.py write the variants). Each
  variant turns a function's regions natural (all of them, and each one alone), then applies the levers to the
  whole function: `*args++` when its `args[N]` reads run 0, 1, 2... one per statement; `x = x op y` / `x = y op x`
  -> `x op= y` and the reverse; s32<->int and u32<->unsigned int on each local declaration alone, in every pair and
  all at once; s32<->u32 and int<->unsigned int; split multi-declarator lines with one variable retyped; s8/u8/s16/
  u16/BOOL locals widened to s32 and int; parameter types; locals of static inlines the function calls. File-scope
  regions that aren't data: natural arm with type swaps inside it and in each function that uses what it defines.
  About 27,400 compiles, then a per-function asm-diff ranking of every DIFF variant to find near misses by hand.
  Every commit: obj_equiv EQUIV, `build/G8MJ01/ok` recreated, `configure.py --non-matching` build of the changed
  objects, fncast_lint and local_prototypes clean, tools/ci.py PASS with its tools/tests step stubbed (the same 8
  test_wave_launch environment errors as the chapters above). The ratchet baseline was `--init`ed on the clean HEAD.
- **Counts.** must_match (tools/cleanup_index.py) **807 -> 800**; index 15.325 -> 15.315 per kLOC (10689 -> 10682
  weighted); `git grep -c MUST_MATCH` over src/rels/include 953 -> 946. Per lever:
  - `*args++` walk: 3 regions. btlevtcmd_GetRGB and btlevtcmd_CheckDamage (their volatile `args` pointer and
    volatile `args[4]` read; mm2 had tried 7 and 4 declaration orders), sac_bakugame bakuGameMain's volatile `args`.
  - `int` instead of `s32`: 1. win_00 win_item_move_3d_position `int curStep` (muj_12's copy already had it); its
    u32 copy and (s32) casts go.
  - A wider local type: 3. icondrv iconDispGx2 `s32 id` for the u16 icon id (the guarded no-op `(u16)iconId`);
    battle_seq_command _btlcmd_SelectWeaponDecide `int ownBelong` (no-op `(s8)BtlUnit_GetBelong`); winmgr
    winMgrSelectEntry case 11 `s32 v` (no-op `(s16)badgeShop_get`; case 13 already declares `s32 v`).
    battle_damage BattleSetStatusDamageFromWeapon keeps its guarded `s32 rate`: _getSickStatusRate returns u8
    chances, so only the s8 narrowing (the natural `s8 rate`, or the guarded casts) keeps values above 127 negative.
  - Compound assignment, either direction: 0. Unsigned and parameter types: 0.
- **Not applied.** EQUIV results that were artefacts: a region turned natural alone while a paired region still
  declares what it uses (party_motion homing_walk's `sid`, eff_toge_flush's unused `color`); obj_equiv EQUIVs on
  data regions (evt_bero's padded strings, battle_damage's pooled SFX strings, ac_stick_rotate's zero Vecs).
  mri_puni qrnd matches more closely with `u32 v`, which would make its `v < 0` test dead: rejected.
- **Near misses** (asm lines still different in the function, best lever): unit_lecture_frankli_sac lecture_init
  `int id` without its dead `= 0` (34 -> 7: retail loads `id` into r29 and copies it to r3 on both arms; ours loads r3
  and saves it on one; loop-scope and in-condition forms give the same); _btlcmd_SelectWeaponDecide's other region,
  `(s32)(s8)attackDirection` (3, any `belong` type); winmgr case 13's `(s16)` cast (4 with s32/int `v`); the four
  krb_comp_dir copies' `(u16)out` (27 -> 11 with `int out`; all 243 u16/int/s32 combinations of its five locals
  tried); hom_00/eki_03 sun_tev's `q` row base (78 -> 14 with `int row`; 48 combinations); unit_barriern_z's header
  barriern_satellite_zanzou comma (3, with or without the walk and int types).
- **Lead.** Most guarded "no-op cast" regions are a narrow local holding a value of the same narrow type; three of
  this pass's seven were that, fixed by declaring the local wider. The others left (krb_comp_dir, winmgr case 13,
  battle_damage's `(s8)rate`) have been tried with every local width; the callee's prototype
  types (int vs s32 parameters) are the untried lever, and need a tree-wide build per change.

## The six worst files by the cleanup index, deep-cleaned (2026-10-21, Claude Opus 5.5, n9-q-o-worst)

Task (Isaac): take evt_win, win_root, winmgr, mot_hammer, win_item and seq_load (the worst per kLOC in
`cleanup_index.py --top 15`) each as a whole, in every category except field naming: raw offsets, MUST_MATCH
blocks, pragmas, gotos and function-pointer casts, with the levers from #1099-#1109. Their address names are all
colour and Vec templates (`dat_`, `vec3_`), which stay by the naming rules, so the work was the five hack categories.

- **Method.** Untracked harness in build/tow/: try.sh compiles one unit with its ninja command and runs obj_equiv
  against objects saved from c9183558; var.py applies OLD/NEW blocks (in the unit or any header), compiles, reports
  EQUIV or the per-function normalised asm diff, and restores; fdiff.sh diffs one function. Every commit: obj_equiv
  EQUIV for every touched unit, `build/G8MJ01/ok` recreated, tools/ci.py PASS with its tools/tests step stubbed (the
  same 9 environment errors: test_wave_launch's systemd scopes and no `elftools` for test_reloc_value_check),
  fncast_lint and local_prototypes clean. After the header changes, every object was built under
  `configure.py --non-matching` (all compile; only the link fails, as before).
- **The window draw helpers take their Vecs and colour by value.** winFontSet, winFontSetR, winFontSetEdge,
  winFontSetLabel, winFontSetPitch and winIconSet are now defined and declared (window/win_main.h and win/win_main.h)
  by value; the definitions compile to the same code, since MWCC passes a by-value struct as a pointer to a caller
  copy. Every caller in win_root, win_item, win_party, win_mario, win_badge and win_log already passed them by value,
  through MUST_MATCH casts (`winFontSetV` and friends) or invented wrapper inlines (`winMailIconV`, `winMsgEdgeV`,
  `monoFontV`, `winIconSetV`, the natural arms' va_list forwarders); those calls are direct now and the wrappers are
  gone. winmgr's select displays are the one caller that passes its locals' own addresses (also for GXSetFog's fog
  colour): by value, MWCC copies each argument into a new temporary and every stack slot moves (all five display
  functions differ, 3-430 lines; passing the `tmp` locals directly drops retail's `pos = tmp` copy). winmgr keeps
  those calls under one MUST_MATCH block of pointer-passing macros named after the functions, so its call sites read
  as plain by-value calls in both builds; the four per-call GXSetFog regions went into the same block.
- **qqsort takes `int (*)(const void*, const void*)`** (and fsort's static `comp`). win_root's comparators already
  took `const void*` (the const reproduces a hoisted prologue load; `void*` parameters move it), and its `void*`
  ones compile the same with const, so all twelve comparator casts went. The other users follow, each EQUIV:
  winmgr's two select comparators, lightdrv, dispdrv, dvdmgr, evt_nannpc and evt_badgeshop return `int` from
  `const void*` uncast; comp_kind/comp_kind_r too; extdrv/punidrv's `compare` gained const. mario_pouch's
  comp_aiueo/_r keep their `(const s16*, s16*)` parameters (a codegen device, documented there), cast to the API
  type.
- **Raw offsets that became typed.** evt_win unitwin_help_disp: `s32* item = &wp->items[wp->index]` keeps base
  and scaled index apart for retail's lwzx. unitwin_setpartylank: the three levels as one chained assignment
  `HPLevel = attackLevel = techLevel = evtGetValue(...)` evaluates the element address before the second
  evtGetValue, which is where retail's `mulli` sits (#1104 had listed both as "multiply on the wrong side of the
  call"). seq_load: the four save-slot scans read `card->saves[i].data.flags`; converted together, MWCC's own
  strength reduction gives retail's running offset, and the one difference left (the later sound-cursor copy
  moved r25 -> r24) goes when `i`, not the old offset local, holds the previous sound cursor. win_root
  compare_func4/_r read the mail list's byte pairs through `const u8*` locals and type mailGetPtr's result.
- **MUST_MATCH regions that went.** win_root winMsgDisp's comma-form icon read is `itemDataTable[n].icon_id` (plain matches since the call no
  longer goes through the wrapper). The five window-file by-value cast blocks, winmgr's four GXSetFog regions.
  evt_win's FontDrawColorV and seq_load's loadDrawColor existed only to copy the colour under a pointer prototype:
  both files declare FontDrawColor by value as fontmgr.c defines it (the documented exception allows it), and
  evt_win's title colour is passed straight from its .sdata2 word, which drops a local and a u32 store pun.
- **Counts** (tools/cleanup_index.py, c9183558 -> this branch): index 15.315 -> 15.163 per kLOC (10682 -> 10573
  weighted); fn_ptr_cast 103 -> 66, must_match 800 -> 792, raw_offset 351 -> 338, field_0x 4627 -> 4626;
  opt_pragma 777, goto 115, address_name 2635 unchanged. Per file (`--top`, per kLOC; raw / must_match / pragma /
  fn_ptr_cast, gotos 0 throughout):
  - evt_win 134.543 -> 128.076 (215 -> 203 weighted): 6/2/0/0 -> 0/2/0/0
  - win_root 87.438 -> 74.460 (229 -> 193): 4/6/0/12 -> 0/3/0/0
  - winmgr 82.700 -> 79.821 (185 -> 178): 0/13/3/6 -> 0/10/3/4
  - mot_hammer 166.480 -> 169.811 (149 -> 153): 10/6/6/0 -> 11/8/6/0 (clampHammerType guarded, plain `#else`)
  - win_item 136.531 -> 132.004 (148 -> 139): 2/2/0/4 -> 2/1/0/0
  - seq_load 102.484 -> 97.179 (132 -> 124): 7/2/0/0 -> 3/2/0/0
  Outside the six: win_party 14 -> 5, win_mario 61 -> 52, win_badge 23 -> 18, mario_pouch 24 -> 16, evt_nannpc,
  evt_badgeshop, lightdrv, dispdrv and dvdmgr 2 each lower.
- **Left, with what was tried.**
  - evt_win: the `notCtrl || (notKey = !marioChkKey(), notKey)` comma (15 shapes; the closest, a `BOOL ng` set from
    both calls and tested once, is 3 lines off: `srwi` + `cmpwi` for retail's `srwi.`), and the `unitwin_icon_delete`
    stripped-function placeholder (data).
  - win_root: winMsgDisp's `(10.0f + (x - 250.0f)) - 10.0f` (`x - 250.0f` in `#else`), the redundant `!= 0 &&`
    before `> 0` (1 line off plain) and winHakoGX's retail uninitialised `row`.
  - winmgr: WINMGR_F32 (pool), the by-value block above, three guarded `defer_codegen` pragmas (emission order), three
    volatile `firstItem` dead compares, the anti-hoist Vec reloads, and case 13's `(s16)badgeShop_get`: with any local
    type (s16, u16, s32, int) and declaration placement only `v`'s register differs (r25 for r26).
  - mot_hammer: the M_S16/M_U16/M_S32 calls at 0x2A0, 0x2A4 and 0x2B8 need a member of a type their unions don't
    have, and the `field_0xNN` name at each offset is already taken by a differently typed member (koopa_motion's u16
    pair, the shared f32s), so a placeholder member can't be added. The four camera-parameter blocks' volatile stores
    (plain or reordered: 26-30 lines; mot_jabara and mot_roll write the same setter with a `(Vec){0, 40, 0}` compound
    literal, which here would need vec3_802c5a80 to become an anonymous .rodata entry), the dead attribute compare,
    hitEstimate's forced `dir3` copy (143-205 lines). Pragmas: without opt_propagation only effUltraHammerEntry's
    argument loads reorder (5 shapes tried); without opt_dead_assignments or opt_common_subs, 11 and 179 lines.
  - win_item: winItemMain's `itemSel` byte-offset element pointer (10 shapes; only a separate `s32* sels` base local
    matches, a steering temp) and item_disp's `entry + 0x2E8` walk (natural `work->skipList[i]` swaps r25/r26 with
    `half`; declaration order and int types make no difference); the STR_* string macros (data layout).
  - seq_load: the three `(wp->cursor << 14) + 0x2000` reads (retail's lhzx with the constant in the index; struct
    indexing folds it into the displacement, 6-24 lines, five spellings) and the two volatile colour reads.
- **For a reviewer.** winmgr's macros shadow the four function names under MUST_MATCH (the cast family, guarded,
  moved from five files to the one caller that differs). The header changes are prototypes only: window/win_main.h
  and win/win_main.h (six each), system.h, win_root.h, winmgr.h, evt_nannpc.h, evt_badgeshop.h; no WindowWork field
  was touched. mot_hammer's `clampHammerType` (an invented name) is a MUST_MATCH-only register-steering inline.
- **Leads.** seq_load's volatile colour reads might go plain if its colours were writable `.sdata2` words like
  evt_win's (a data-definition change, out of scope here). winmgr's tmp/pos/scale copies read like compound literals
  passed by value, which would need its vec3 templates as anonymous .rodata (data layout). Converting every site in a
  function at once, not one at a time, is what made seq_load's loops match: a single conversion's register ripple
  can hide a form that matches when applied throughout.
## Callee prototypes retyped so the casts at their call sites go (2026-10-21, Claude Opus 5.5, n9-q-o-proto)

Task (Isaac): #1109 found that a guarded "no-op cast" at a call site is often a type mismatch. Try the untried
lever: change the callee's prototype and definition (int/s32/u32 and the narrow types) and drop the casts at every
caller, keeping a change only if the whole linked build stays byte-identical, callee included.

- **Method.** Scratch in the untracked build/proto/. argcasts.py and retcasts.py (libclang, reusing #1106's
  survey.py parser) list every integer cast written on a call argument (with the callee's declared parameter type)
  and on a call's result (with its declared return type), marked guarded when the line is in a MUST_MATCH-only arm;
  mmcasts.py lists every integer cast in a MUST_MATCH arm with its context. proto_try.py retypes a callee's return or
  parameter in every declaration and definition, strips the recorded casts, and runs a full `ninja`
  (build/G8MJ01/ok recreated, ~20 s); batch.sh runs and reverts a list of those. Population: 14 guarded argument
  casts (BattleSetStatusDamage, BattleSamplingEnemy, badgeShop_get, envGlareFilter x4, sac_genki's float
  conversions, evt_telop's GX_FALSE macro, yuu_00's irand float conversion), the unguarded argument casts that are
  no-ops in C (a widening that the parameter narrows back, or a same-type cast), and every callee whose callers cast
  its result the same way. The parameter side turned out small; the result side is where callers carried the
  prototype's type.
- **Counts** (tools/cleanup_index.py, c9183558 -> this branch): must_match **800 -> 796**; index 15.315 -> 15.310
  per kLOC (10682 -> 10678 weighted); every other category unchanged. 166 call-site casts removed (the index
  doesn't count them), plus 4 guarded return casts and 4 casts in callee bodies.
- **Parameters changed:**
  - kanjiGetWidth(u32 code), like its siblings kanjiSearch, hankakuSearch and JUTFont_CodeToGlyph: msgdrv's five
    same-type `(u16)glyph` casts (left plain by #1106 as "a cast to the callee's parameter type") go. u16 with the
    casts removed changes msgdrv; s32, int and u32 all match.
  - BattleAudience_GetExist(u32 id): BattleAudience_Init's two `(u32)index` casts go (s32 without them changes the
    unit).
  - pouchCheckItem(s32 itemId): the u32 parameter's `s32 id = (s32)itemId` copy goes (with the return below).
- **Returns changed** (every caller cast the result to the new type; casts removed per callee):
  - u8, for 0/1 results cast `(u8)` by every caller: check_exe_phase_evt_status 26, BattleAudience_GetWaiting 8 (and
    its own `(u8)` on the return), BattleAudience_CheckReaction 2, BattleAudience_CheckReactionPerPhase 1,
    BattleBreakSlot_CheckReaction 2, unit_boss_batten_leader's battenSatelliteTo_InitInline 10 and
    battenSatelliteTo_Main 2.
  - u32: marioStGetSystemLevel 13 (GlobalWork.systemLevelFlags is u32 too; its 20 users are unchanged), fileAsyncf 13
    (one was end_unknown's `(void*)... == NULL`), SoundSSCheck 4, SoundSongCheck 3, SoundEfxCheck 1, psndBGMChkSilent 1,
    psndBGMStartCheck 1, itemStatus 2.
  - void*: kpaChkHitobjRide, like kpaGetHitobjRide: four `(u32)... != 0` tests become `!= NULL`, mario_hit's
    `(HitEntry*)` cast and the body's two `(s32)result` returns go.
  - s32: pouchCheckItem 45 (one inside win_mario's link macro; its counter is s32 too), pouchCheckMail 2,
    effTblRandN64 9 (the body already computes an s32 remainder), BattleACPadCheckRecordTrigger 6,
    BattleInformationGetResult 3 (FieldBattleInfo.result and BattleInformationSetResult's parameter are s32 with it).
  - s32 for the four static, uncalled krb_comp_dir copies (togekuri v1/v2, hyper, yami): the guarded no-op `return
    (u16)mid` becomes a plain return (the zero-extension is the return conversion). s32, int and u32 match; u16 and s16
    don't. These are the four MUST_MATCH regions removed.
- **Tried and failed** (the full build differs):
  - BattleSamplingEnemy's attackDirection (seven `(s32)` of an s8 field into an s8 parameter, one guarded): s8 without
    the casts, s16, s32, int, u8, u32. The cast steers the caller's registers (`lbz r0` + `extsb r9,r0` instead of
    `lbz r9` + `extsb r9,r9`); no prototype gives that.
  - envGlareFilter's four coordinates as s16 (hom_00's guarded `(s16)` casts, eki_03's guarded function-pointer
    call): the callee then sign-extends all four on entry; retail's definition is 32-bit.
  - badgeShop_get: id as s32/int (14 `(s16)` casts), return as s32/int (winmgr case 13's guarded `(s16)` cast; case
    11 matches plainly through the same prototype, so the prototype can't be the difference).
  - battle_damage's guarded `(s8)rate` pair: BattleSetStatusDamage's rate as s8, _getSickStatusRate returning s8, the
    local s8 or s32, in all six combinations.
  - iconDispGxCol's id as u16, s32 (casts only), u32, int (countdown's two `(u16)` casts): callee or caller changes.
  - hitGrpFlagOn/Off's flag as u16: the callee hands it to hitFlgOn(s32) and changes. hitObjFlagOn/Off as u16 match,
    but each has one value-narrowing caller and the pair would split from the Grp pair; not applied.
  - Returns: BattleAudience_GetExist, GetSysCtrl and GetEscapeChangeOK as u8 (their `return !(flags & m)` gains an
    8-bit mask: `extrwi` instead of `srwi`), BtlUnit_GetWeaponCost u8, BtlUnit_GetEnemyBelong s8, hitGetAttr u32,
    psndPushGroup u32, FontGetMessageWidth s32 (u16 is real: many plain callers), sysMsec2Frame s16.
  - With fileAsyncf now u32: animdrv's guarded `(u32)fileId == 0` / `(u32)other == 0` with either local u32 (three
    combinations), and seq_logo's `(u32)asyncResult` with a u32 local (both arms): still differ.
- **Not tried, with why.** JUTFont_CodeToGlyph's ten `(s8)*str` casts change the value (fontmgr is `-char unsigned`)
  and one caller passes a u16 code. _JUTFont_DrawPos's `(int)(s16)`/`(s32)(s16)` pairs are the documented CSE device,
  not a prototype question. swByteSet stores the whole s32 for index 0, so mri_puni's `(u8)` casts are its callers'.
  spline_getvalue's s32 pointer parameters: its callers keep the pointers in s32 fields and locals, so pointer
  parameters would only move the casts. TEXGetGXTexObjFromPalette and GXSetScissor are SDK prototypes;
  psndSFX_vol is a documented local-prototype exception. The function-pointer-cast leftovers #1091 lists (iconDispGx,
  BtlUnit_CheckStatus, ...) were not redone.
- **Lead.** A cast that every caller applies to a callee's result (`(u8)` on a 0/1 BOOL, `(u32)` on an `== 0` test of
  an s32, `(s32)` on a u32 count) was the callee's declared type in the original more often than not: 21 of the 32
  callees whose return was tried matched. The callee's own body is what refuses (an 8-bit mask on `!x`,
  extension of narrow parameters). The same scan over single-caller casts (yuunpc_pose_check, puni_rnd, msgWindow_Entry)
  wasn't run.
- Verified before each commit: `ninja` recreates build/G8MJ01/ok; every game object (1,131) compiles under
  `configure.py --non-matching`; fncast_lint and local_prototypes pass (tools/local_prototype_exceptions.json
  unchanged: no entry went stale); tools/ci.py PASS with its tools/tests step stubbed (the 9 test_wave_launch
  environment errors, as in the chapters above), ratchet baseline `--init`ed on the clean HEAD.

## Placeholder fields named from outside evidence (2026-10-21, Claude Opus 5.5, n9-q-o-names)

Pass over the ~3,800 `field_0xNN` members, renaming only those that outside evidence names. Candidates came from six
read-only Claude Opus 5.5 searches (one per area: window, mario/motion/party, battle, driver/event/manager, effect,
RELs) and a scan for short original-named functions that touch one placeholder. Every name below was checked by hand
before it went in. "Original" means a function name in the fork-point (2df569d) symbol maps, dol or REL. Names added
to symbols.txt on this fork don't count.

**Renamed** (evidence: accessor = an original function that only reads or writes that field; same struct = a name the
repo already uses at that offset of the same memory; SDK; string):

| struct | off | name | evidence |
|---|---|---|---|
| EffRippleData (ripple_n64 setter) | 0x2C/0x30 | rx, rz | accessor effRippleN64SetRxRz; EffRippleData already names both, so the effRippleN64Data overlay is gone |
| EffRippleData | 0x14 | rippleScale | accessor tik_02 ripple_scale_func (stores its float); 0x10 is already `scale` |
| BattleWorkField18FF0 | 0x0/0x4 | help, dispType | accessors BattleAcHelpSetHelp, BattleAcHelpSetDispType |
| BattleActions | 0x288 | gaugeSe | BattleAcGaugeSeInit/Update/Delete are its only users; Delete reads and clears only it |
| WindowWork | 0x17C | sortWait | accessor winSortWait (`return field != 0`) |
| MarioWork (0x2B8 union) | 0x2B8 | jumpHeight (new member) | accessors marioGetJumpHeight, marioChgJumpStandMotion; only the jump-stand users moved, mot_jumpSw/Npc and marioDispBlurSub keep field_0x2B8 |
| unit_party_clauda overlay | 0x18-0x30, 0x58 | color1R/G/B, color2R/G/B, rotation | same struct: effKumokumoN64Entry's effKumokumoData; overlay gone |
| rsh_05, eki_00 MoamoaWork | 0x48 | poseId | same struct as rsh_06 (unused in both) |
| las_29 map_color_tev arg | 0xC/0x10/0x14 | tevStage, texMap, texCoord | a TEV callback (evt_map_set_tevcallback), so MapTevCallback; overlay gone |
| hei_10 wait_anim_frame | 0x24 | clipTime | the overlay is AnimationPose; overlay gone |
| BattenEffUserdata (batten_leader) | 0x10 | barrierSize | accessor evt_eff_barriersizechange |
| BattleUnitPart | 0x78 | shadowScale | shadow_scale_memory copies only it; shadow_scale_small/return only interpolate it |
| BattleUnitPart | 0x208 | animeSpeed | setter btlDispAnimeSpeed |
| BattleUnitKind | 0x13 | escapeRate | accessor _get_escape_rate; the 191 `// field_0x13` data labels follow |
| BattleWorkField18C70 | 0x4 | param[4] | setter btlevtcmd_AnnounceSetParam |
| FieldBattleInfo | 0x14, 0x19 | storeCoin, ruleKeepResult | BattleStoreCoin adds only to the 0x14 word; FieldBattleInfo is FieldBattleData+0x20, whose 0x39 is ruleKeepResult (evt_npc_get_battle_rule_keep_result) |
| NpcBattleInfo | 0x9C | btlSetupWork[4] | accessors npcGetBtlSetupWork / npcSetBtlSetupWork |
| FieldBattleData | 0x54D | sandersBombTrigger | setter fbatSandersBombTriggerOn |
| BeroEntry | 0x18 | length | accessor evt_bero_get_info_length |
| MovingFloorWork | 0x74 | stmsec | accessor moving_floor_get_stmsec |
| BadgeShopWork | 0xF8 | bargainTable[5] | accessor badgeShop_getBargainTable |
| ARAMWork | 0x0 | stackIndex (u32[8]) | SDK: ARInit(u32* stack_index_addr, 8) |
| MapObject | 0xEC | bboxLeft/Top/Right/Bottom (u16, from #1102) | SDK: GXReadBoundingBox(left, top, right, bottom) from mapDispMapObj_bbox |
| MessageData | 0x7A28 | se | string: msgAnalize's "se" tag sets it (0xFFFA entries) |
| evt_shop.h evt_shop_work | 0x0-0x20 | gone; `_wp` is a ShopWork* | same struct: evt_shop_setup allocates a ShopWork and every user casts _wp back |

Structure changes that came with a name: the FieldBattleInfo u8[8] band split into an s32 and four u8 members (its s32
casts go), ARAMWork.stackIndex gets its SDK element type (the u32 casts go). Each built
to an identical object.

**Rejected, too weak** (kept as placeholders): WindowWork 0x164-0x184 (winSortEntry writes all of them), 0x128-0x14C
(winMsgEntry writes six at once); BattleActions 0x28C (BattleAcGaugeSeUpdate's value; "GaugeSe" already went to
0x288); BattleUnitPart 0x1A6/0x1A8 (getGravityDispOffset/getFloatDispOffset compute from them, not return them);
MarioWork 0x260/0x264 (balloon effect, but marioBalloonOn/Off are not accessors); WinEquipRec 0x0-0x8 (one builder
writes all three); AnimationWork 0xD8/0xDC (animSetPaperTexObj's eighth argument, named only by our parameter);
battle_stage_object's event-id fields (Destroy/Fog/Nozzle functions do more); ParseWork 0x54 (parsePop only
decrements it); tou_02 PoseGroup 0x18 (not provably the moamoa group struct); eki_00 MoamoaWork 0x0C/0x10 (rsh_05's
train/prevTrainY, but the types disagree); filemgr FileType1 (AnimationPoseData's offsets, but a different file type);
effNozleEntryData vs effNozleData (the head's names describe entry 0 only); NanNpcEntry jump terms (gor_00 uses the
same slots differently); BattleUnit 0x2A8/0x2C8/0x280 and the other BattleUnit placeholders (no single-field
accessor); every effect Entry parameter copied into a field (our parameter names); rsh_06 MoamoaWork 0x40/0x558, rsh_00 MoamoaPose 0x18
and mri MagnumBattenderWork 0x0-0x1C (their rsh_05/eki_00 and aji ArmWork source names were inferred from behaviour
in #885, #980, #983 and #977, with no outside evidence).

**Leads outside the `field_0xNN` spelling**, same standard of evidence, not applied here: MarioWork `unk1A8` dispDir
(evt_mario_get_dispdir), PartyEntry `field_10C` dispDir (evt_party_get_dispdir), MarioWork `unk4C` mutekiTime
(marioSetMutekiTime), MarioWork `field_F8` camCollision (marioUpdateCamCollision), PartyCloud `unk8` breathDist
(cloudGetBreathDist), PartyEntry `field_34`/`field_3B`/`field_36`/`field_18`/`field_1C` runMode/moveMode/mot/pose/paper
(partyChgRunMode, partyChgMoveMode, partyChgMot, partyChgPose, partyChgPaper; each also sets a flag), BattleWork
`field_1616C` breakSlot (BattleBreakSlotGetPtr), `field_19064` acResult (gor get_ac_result), `field_19038` cmdDisable
(recover_cmd_disable clears it), `field_16F1C` bytes 0x20/0x21 (_mario_ac_success_check / _mario_ac_failed_check),
rsh.h RshWindowTevParam `field_4`-`field_18` (a MapTevCallback).

BadgeShopWork 0x19 was `bargain_table`; badgeShop_getBargainTable proves 0xF8 is the bargain table, so 0x19 is
`field_0x19` again. Its only users are evt_badgeShop_throw_inc/dec/get_kind_cnt (lead: a throw-sale stock table).

- Verified at every commit: `ninja` recreates build/G8MJ01/ok; a full `configure.py --non-matching` compile of every
  C object passes; fncast_lint and local_prototypes pass; tools/ci.py passes (lint, build, natural, data, ratchet: no
  regressions) with its tools/tests step skipped for the environment failures recorded above, under
  `uv run --with pyelftools`.
- Index (tools/cleanup_index.py, same script both sides): 15.026 -> 14.730 per kLOC (10494 -> 10287 weighted);
  field_0x 3821 -> 3622, raw_offset 452 -> 448.

## The last function-pointer casts: callbacks typed, the rest are needed (2026-10-21, Claude Opus 5.5, n9-q-o-fncast2)

Task (Isaac): the 66 function-pointer casts `tools/cleanup_index.py` still counted after #1091, #1110 and #1111. Calls
through a cast of a named function: find the prototype every caller can use, checked against every caller's and the
callee's asm, and keep it only if the linked build stays byte-identical. Casts that are not calls (callback fields,
comparators, evtGetValue results): declare the field or variable with its function-pointer type so the cast goes. The
casts that are really needed stay, guarded where the rules say so.

- **Method.** Untracked scratch in /tmp: var.py applies OLD/NEW edits (unit or header), compiles the touched units,
  runs `tools/obj_equiv.py` against objects saved from 436e5a2c and prints a normalised per-function asm diff, then
  restores; natural_all.py compiles every game unit without `-DMUST_MATCH` in parallel. Every commit: `ninja`
  recreates build/G8MJ01/ok, all 1,228 compiled units build without MUST_MATCH, fncast_lint and local_prototypes
  clean, tools/ci.py PASS with `--allow-protected` and its tools/tests step stubbed (`~/ci_notests.py`; the same 9
  environment errors as the chapters above), ratchet baseline `--init`ed on a build byte-identical to 436e5a2c. At the
  end, a full `configure.py --non-matching` ninja (only the dol link fails, as usual), then reconfigured and rebuilt
  to `ok`. error_handler.c is NonMatching and built in neither configuration; it was compiled by hand with and without
  MUST_MATCH.
- **Counts** (tools/cleanup_index.py, 436e5a2c -> this branch): index 15.158 -> 15.113 per kLOC (10569 -> 10538
  weighted); fn_ptr_cast **66 -> 51**; must_match 788 -> 787; every other category unchanged. Of the 15 the index
  stops counting, 12 are removed and 3 (error_handler) are the same cast in the typedef spelling the index skips.
  `tools/fncast_allowlist.json` unchanged (its one entry, system.c, still matches; `--stale` lists nothing).
- **Removed (12):**
  - Fields typed with the one signature their caller uses: BattleUnit.mExtraWorkFree `void (*)(BattleUnit*)` (only
    ever NULLed in our code; the delete path calls it with the unit); AnimationPose.dispCallback `void (*)(void*
    userData, s32 groupId, Mtx mtx)`; CaseSetup/CaseEntry.activeFunc `s32 (*)(CaseEntry*)` (caseMain's condition 17
    calls it with the entry); MahojinWork.drawArg/drawFunc `EffectEntry*` and `void (*)(CameraId, EffectEntry*)`:
    gor_01 and tik_05 store effStarStoneDraw and its star stone entry, and nothing else writes them (2 casts).
  - BattleWeapon.hpDamageFn/fpDamageFn are `u32 (*)(BattleUnit*, BattleWeapon*, BattleUnit*, BattleUnitPart*)`, the
    type of all 13 weaponGetPower*/weaponGetFPPower* functions in battle_weapon_power.c, so BattleCalculateDamage and
    BattleCalculateFpDamage call them plainly (2) and BattleAudience compares the pointer without its two `(u32)`
    casts. weaponGetPower_Scissor and _ZubaStar returned s32 and _Deka was `(void)`: all three now return u32, Deka
    takes the four parameters it ignores, and each is declared in its sac header (the bodies compile the same).
    battle_weapon_power.h declares all 13 functions. 22 `extern s32 weaponGet...[];` declarations (battle_mario,
    seven party units, chuchurina's header, five REL units) stood in for those prototypes, so data tables could name
    the functions; they are gone, and the `(u32)weaponGetACOutputParam` words in the raw weapon tables are unchanged.
  - mario_pouch's comp_aiueo/comp_aiueo_r take qqsort's `(const void*, const void*)`: 4 casts. Their codegen device
    (#1110: `const s16* a, s16* b`) moves into the loads: `*(const s16*)a`, and `*(s16*)b` under MUST_MATCH with
    `*(const s16*)b` in `#else` (two new regions, review). Reading b through `const s16*` puts id_b in r31, as the
    old comment said for a `const` b.
  - HSV2RGB returns GXColor. MWCC returns a 4-byte struct in r3 (`lwz r3` from the local), so the body compiles the
    same with `return out;` in place of the `*(u32*)&out` pun. sac_scissor's SCISSOR_HSV2RGB cast and its punning
    `#else` inline go (the one MUST_MATCH region removed), mri's hakari_color assigns the result without a u32 store
    pun, and fontmgr's fontHSV2RGB stores straight into the caller's colour (`fontHSV2RGB(&rgb, hsv)`): returning
    the struct through the inline adds a temporary per call (+16 bytes of frame); calling HSV2RGB directly drops the
    by-value copies.
- **Still a cast, respelled (3), not a removal:** setupErrorHandler's three `(void (*)(u16, OSContext*, ...))` casts
  are spelled `(OSErrorHandler)`, the SDK's own typedef. The cast remains and is needed: systemErrorHandler takes
  dsisr/dar as fixed parameters and the handler type is variadic. The index doesn't count the typedef form, so 54
  casts are really left: the 51 below and these 3.
- **Also typed, not counted by the index:** animPoseSetDispCallBack's parameter has dispCallback's type, so it no
  longer converts a `void*` to a function pointer (MWCC accepts that silently; ISO C doesn't), and its three callbacks
  that disagreed now have the exact signature: _light_point_callback and _fire_point_callback take `void* userData`
  with `BattleUnitPart* part = userData;` as their first local, and nok_00's draw_poster takes `Mtx` and is passed
  without its `(void*)` cast. All compile the same.
- **Left (51), each tried:**
  - Values used as functions, plain C (17): evt_nannpc (6), evt_ext (3) and mri_puni (3) evtGetValue results;
    msgdrv's msgWindow_Entry `a`, an s32 that evt_msg fills from evtGetValue, so typing the parameter would only move
    the cast into evt_msg; seq_mapchange's REL prolog/epilog (2; OSModuleHeader's fields are `u32` in the SDK);
    battle_seq_command's BtlUnit_GetData result (a `void*`: MWCC would convert it implicitly, but ISO C needs the
    cast); system.c's intplGetValue `mode` (allowlisted).
  - A colour local's address passed where the prototype takes GXColor by value (25, guarded): Waku_col in
    battle_message, battle_seq, battle_status_effect and battle_attack_audience; sac_genki (4 iconNumberDispGx3D, 1
    iconDispGxCol); statuswindow (2 macros), both evt_yuuranking (3 macros each), shadowdrv (3), winmgr's block (4),
    mri's HAKARI_SET_COLOR. What MWCC does, measured here:
    - battle_message's `_disp`, by value (colour local, compound literal in the call, colour assigned before or
      after the width, `*&color`, or a by-value int-first prototype `(u16, GXColor, f32...)`): the colour word lands in
      the same slot and only r6/r7 swap between it and the second 0x43300000 constant. The by-value copy makes a new
      register for the colour at the call, after that constant; retail's colour register comes from the assignment
      statement, before it. No ordering of C statements puts a by-value copy before the float arguments, which
      include width conversions. The other three Waku_col sites carry the same comment and the same swap.
    - mri's hakari_color (8 shapes: declaration orders, a named result passed by value, the HSV2RGB result passed
      straight in): a by-value argument either adds a copy slot, or, passed straight from the call, takes the lowest
      slot. Named locals get slots in declaration order, first declared highest, with compiler temporaries below them.
      Retail has the result at the top, so a named local's address is passed without a copy.
    - sac_genki: passing the named colours by value does drop the copy, but the five icon colours then sit 8 bytes
      lower, under the two address-taken colours of the final btlDispTex4 blocks; retail has those two at the bottom.
      The same holds with block-scoped colour locals and with the plain `*(GXColor*)&dat_...` arguments.
    A by-pointer prototype doesn't fit either: 58 Waku_col, 38 iconDispGxCol, 21 iconNumberDispGx and 11
    mapGrpSetColor call sites outside these files pass by value and match through the by-value copy.
  - Cross-TU prototype mismatches the callee's body rules out (6, guarded): btlDispGXQuads2D (3; the only callers):
    the callee's alpha is u8 (s32 adds a clrlwi in it) and DrawSubIconSub passes its s32 `ca` unextended; with ca
    u8 or s8 the assignment from alpha gains a clrlwi/extsb, with alpha u8/s16/u16 its loads do. iconDispGxCol's
    u16 `b` (2): retail tests `b == 0` with `clrlwi.` and passes the register unextended; with b s32, u32 or int the
    test becomes cmpwi/cmplwi, and with u16 the call gains clrlwi. envGlareFilter (eki_03, 1): s16 parameters make the
    callee sign-extend (#1111).
  - Scheduling or inlining only (3, guarded): ac_signal_timing's ICON_DISP_B (5 shapes: the u16 local, the
    `(u16)(0x92 + i)` argument, the assignment after the Vec fields, an s32 local, the bare sum; 16-46 lines);
    rsh_05's moamoa_pose_change/main casts stop inlining: plain, pose_main inlines into ext_moamoa_main (+0x110
    bytes), while retail inlines pose_change in the evt_moamoa_* functions but calls it from ext_moamoa_main. Only a
    pragma could say that.
- **For a reviewer.** The weapon-power return
  type is u32 (13 of the 16 definitions and the header already said so; the only caller keeps an s32). fontHSV2RGB's
  signature changed (an existing inline). The field types name no new typedefs. The header changes are
  battle_database_common.h, battle_unit.h, animdrv.h, casedrv.h, eff_mahojin.h, fontmgr.h, battle_weapon_power.h and
  the three sac headers.
- **Lead.** A `*(u32*)&colour` pun on a function's return is often a GXColor return in the original: MWCC returns
  small structs in r3, so the callee's code doesn't change, and only callers that copy the result through an inline
  need care. The same may hold for other u32-returning colour helpers.

## The next ten worst files by the cleanup index, deep-cleaned (2026-10-21, Claude Opus 5.5 orchestrating four Claude Opus 5.5 workers, n9-q-o-worst2)

Task (Isaac): take sound, pmario_sound, filemgr, battle_stage_object, evt_mobj, battle_audience, animdrv,
battle_unit, battle_seq_command and mot_plane (`cleanup_index.py --top 20` after #1110) each as a whole, as #1110 did:
raw offsets, MUST_MATCH, pragmas, gotos and function-pointer casts, no field renames without hard evidence, no data
layout. The orchestrator took battle_unit, battle_seq_command and filemgr; workers took sound+pmario_sound,
evt_mobj, animdrv+mot_plane and battle_audience+battle_stage_object, editing only their own .c files.

- **Method.** Untracked harness in build/tw2/: try.sh compiles one unit with its frozen matching command and runs
  obj_equiv against objects saved from 436e5a2c (`NM=1` compiles without -DMUST_MATCH; `OVL=dir` puts a private
  include overlay ahead of include/, so workers tried header changes without touching shared headers); var.py and
  fdiff.sh as #1110's. The orchestrator reviewed every diff, applied the overlay headers, and ran the full build.
  Every commit: `build/G8MJ01/ok` recreated, every object (1,230) built under `configure.py --non-matching`,
  fncast_lint and local_prototypes clean, and tools/ci.py PASS on that commit's own tree (`--base` its parent;
  tools/tests stubbed as in the chapters above, pyelftools supplied through `uv run` for the data step).
- **Counts** (tools/cleanup_index.py, 436e5a2c -> this branch): index **15.158 -> 14.688** per kLOC (10569 -> 10237
  weighted); opt_pragma 777 -> 670, field_0x 4626 -> 4549, raw_offset 338 -> 281, must_match 788 -> 768,
  address_name 2635 -> 2625, fn_ptr_cast 66 -> 64; goto 115, unk_name 684 unchanged. Per file (`--top`, per kLOC;
  weighted; raw / must_match / pragma / goto / fn_ptr_cast):
  - sound 108.451 -> 107.685 (222 -> 220): 2/3/4/1/0 -> 2/3/2/1/0
  - pmario_sound 30.113 -> 29.849 (211 -> 209): 13/8/4/8/0 -> 13/6/4/8/0
  - filemgr 223.952 -> 136.307 (187 -> 110): 0/1/0/0/0 unchanged; field_0x 186 -> 109
  - battle_stage_object 61.493 -> 56.743 (168 -> 154): 16/2/14/0/0 -> 16/2/0/0/0
  - evt_mobj 26.616 -> 17.025 (133 -> 84): 14/25/10/4/0 -> 2/10/10/4/0; address_name 62 -> 52
  - battle_audience 26.834 -> 17.916 (124 -> 82): 1/9/44/6/0 -> 1/9/2/6/0
  - animdrv 39.272 -> 28.205 (123 -> 88): 18/4/2/0/1 -> 2/3/2/0/0
  - battle_unit 56.667 -> 31.615 (119 -> 65): 5/7/46/0/1 -> 1/7/2/0/0
  - battle_seq_command 40.386 -> 22.190 (113 -> 62): 24/2/3/16/1 -> 0/2/0/16/1
  - mot_plane 99.638 -> 104.640 (110 -> 106): 0/6/2/0/0 -> 0/4/0/0/0 (per kLOC rises because ~90 code lines went)
  Outside the ten: battle_seq 74 -> 72 (its act-record offset).
- **Per-file `-inline noauto` (new lever).** battle_unit, battle_audience, battle_stage_object and battle_seq_command
  lose all 99 `#pragma dont_inline` lines: built with `extra_cflags=["-O4,s", "-inline noauto"]` the four objects
  are identical, i.e. none of their functions is auto-inlined in retail (battle.c and battle_weapon_power already
  carry `-inline off`/`none`). battle_audience needs `noauto` rather than `off` (it has explicit inlines). The
  comments that described the pragmas went with them. Survey, not applied: 42 other src/ files (178 dont_inline
  lines: battle_camera 26, battle_menu_disp 14, battle_stage 12, battle_damage 10, evt_npc 8, sac_scissor 8, most
  of src/battle) are identical the same way; battle_disp, party, party_move, mot_roll, evtmgr and mario_hit are not.
  The list is build/tw2/noauto_survey.txt in this worktree.
- **Typing that removed casts and offsets.**
  - BattleSamplingEnemy, BattleChoiceSamplingEnemy, BattleSamplingEnemyUpdate and _btlSamplingEnemy take
    `BattleSampling*`: every caller passed one through `(u8*)` and the callee cast it back (battle_detect,
    battle_enemy_item, battle_event_cmd, battle_item_data, battle_seq_command, battle_stage_object).
    battle_seq_command's `u8* work` views of the sampling buffer read `count`, `order`, `selectIdx` and the entries.
  - The BattleCommand menu tables (battle_seq_command.h): names and descriptions are `const char*` (msgSearch's
    type; 46 `(char*)` casts go), `mDisableReason` and the Action/Operation `mType` are s32 and the weapon table's
    `mEnabled` BOOL (every reader cast them `(s32)`; 17 casts go). Every src user compiled EQUIV.
  - BattleDispPose.poseId is s32 (every reader cast it `(s32)`) and BattleUnitPart.poseTable is a BattleDispPose*
    (battle_disp's and battle_event_cmd's casts go). BattleUnit.mExtraWorkFree is a function pointer (#1113 typed it
    the same way; main's declaration kept).
  - `_check_weapon_type_attack` reads BattleWeapon's named status fields (`(s8)` on the u8 chance fields: the tests
    are signed) instead of `s8* p` offsets; `*(u16*)&...icon_id` (already u16) and `switch ((s32)u16)` casts went.
  - The BtlActRec_AddCount/AddPoint offsets index `wp->field_16F1C[0xNN - 0x1C]`, the form battle_actrecord.c uses.
  - filemgr: FILE_TYPE_1's 25 relocated words at 0x14C-0x1AC are AnimationPoseData's shapes..mpAnims (the same list
    the ANIMPOSE case relocates), so that case uses POSE()/FILE_RELOC and the FileType1 view goes.
  - _combine_nozzle_weapon takes `BattleWeapon*`; MapObjectEntry's 0x1A0/0x1B4 bands are `s32 unk1A0[2]` and
    `s32 unk1B4` (about 50 `*(s32*)` puns in evt_mobj go); MotionPlaneData.sfxId is u32 (five `*(u32*)&` puns go);
    AnimationPose.dispCallback is typed (as in #1113, whose comment is kept; the call cast goes) and
    animPaperPoseDispSub takes the DispCallback's `void*` (its MUST_MATCH cast goes).
- **Natural shapes that matched.**
  - animdrv: the five `poseOffset` sites are calls of the file's own animPoseSetMaterialLightFlagOn/Off (symbols),
    which inline to retail's `addi rI, rI, 0xEC; lwzx` folded index. The vertex pointers index their arrays once
    the byte-size locals go (raw3 had kept them); the index pointers are `&data->posIndex[n * 2] + 1`.
  - evt_mobj: reading an in-unit global by name compiles to the same section-base addressing (`...rodata.0` +
    offset) the `base + 0x7C` hacks spelled out (rideswitch_lightblue, koopa_pole, koopa_pole_evt); the nine template
    guards keep the defined template (their `#else` named externs nothing defines).
  - mot_plane: with literals instead of constant locals and `x += k * (t - x)` blends (a separate `t = 30.0f * dir`
    where retail has fmuls + fsubs), the opt_propagation pair and both its guards go.
  - battle_seq_command WeaponMultiItemCancelCheckMain: `cmd->` instead of `wp->commandMenu.` (opt_propagation).
    battle_stage_object _bgset_kemuri_effect: `*args++` (opt_propagation). battle_audience ApRecoveryBuild: the
    `delta` steering temp goes. sound SoundEfxMain: SoundEfxMuteCh/CheckCh take `u32 chan` like the file's other
    channel helpers (optimize_for_size). pmario_sound skips math.h in both builds, as evt_sub, seq_load,
    seq_title and two effects already do.
- **Rejected in review (EQUIV, not applied).** Data definitions are out of scope: pmario_sound's 262 psbgmlist
  initialisers as `u8 params[4]` (removes the six `((u8*)&entry->params)[N]` reads; .data identical), psndPushGroup's
  .sbss `tbl` as u8, animdrv's vivihimoData initialisers as `&_vivihimoData[0x100]`. psndSFXMain's `u8* info =
  (u8*)&res` view only respells the six byte offsets. `(s32)weapon != (s32)&ItemWeaponData_Naniga_Okorukana` in
  battle_seq_command (the plain compare differs) is now under MUST_MATCH with the plain compare in `#else`.
- **Left, with what was tried** (details in build/tw2/w/<worker>/ in this worktree):
  - sound: SoundEfxSetPan/SetSrndPan's folded index (12-20 lines, every index type); SoundSSPlayChEx_getPath's
    guarded steering inline (13 shapes, best 4); _sscallback's opt_propagation (720 declaration orders, best 228);
    `goto unlock` (2 lines off with `break` + a test).
  - pmario_sound: psndENVOn_f_d's `lhzu` walk (157 variants, best 5); calc3D's and psndENVMain's volatile reloads
    (34 and 460 lines); psndSFXChk's opt_propagation (79 variants, best 10); psndENVOff's dont_inline (per-file
    `-inline noauto` changes the unit); the gotos as recorded.
  - evt_mobj: the jumpstand `jumptable + (type << 2)` regions (132 more shapes, all 26 lines off); five
    opt_propagation pairs (38 shapes, best 8; a file-wide `-opt nopropagation` changes .text size); `run_event`.
  - battle_audience: opt_lifetimes in ApRecoveryBuild (720 declaration orders and more, best 10); `goto case4_body`;
    ApSrcEntry's volatile blocks (best 10); `setupBase` (11 lines with named fields).
  - battle_stage_object: the `_rate_mix` block's `+ 0x80` bases (16 raw offsets under MUST_MATCH; named fields are
    194 lines off); `BSO_SFX` (306 variants, none within 76).
  - battle_unit: BtlUnit_SetParamToPouch's `pouch + party * 0xE + 6` and its SetParamFromPouch twin (retail's
    folded `addi; sthx`; the only plain-looking match is a new `(s16*)` pun); the UnitStatusBlock struct-copy puns;
    ControlPoseSoundMain's pooled 0.7 and its pragma. battle_seq_command: the `(0, "...")` commas (any order or
    chained form: 5 lines, copies: 314); the cursor gotos and `(s32)(s8)` as recorded.
  - animdrv: the defer_codegen pool helper and the `(u32)x == 0` pair. mot_plane: planeReset's and case 0's volatile
    copies, the two pool-order regions.
- **For a reviewer.** The `-inline noauto` flags are configure.py Object lines. evt_mobj's koopa_pole_evt
  conversion is EQUIV only together with koopa_pole's: the order in which functions first use the two section bases
  sets mobj_itembox's registers. evt_mobj's address_name drop (62 -> 52) is partly the index's: the kept
  `mobj_vec3_802cXXXX_template` names don't end in the address, so the regex skips them. mot_plane's sfxId init is
  a block with its own `MarioWork* m`, the shape of the file's six other sfxId sites. SoundDVDChan stays guarded,
  now as a cast macro instead of a steering inline. battle_detect's definitions name the parameter `s` (the old
  alias), the header `sampling`.
- **Leads.** The 42-file `-inline noauto` list above. A folded `addi rI, rI, C; lwzx/stwx` index is often an
  inlined one-line setter from the same unit (animdrv's was). The section-base lever for other `base + offset`
  reads of in-unit globals (evt_shop, end_unknown, mobjdrv). The data-definition changes rejected above, if data
  layout comes into scope; also the nine `mobj_vec3_*_template` definitions renamed to their symbols.txt names
  (EQUIV, and MWCC no longer hangs on them, so those comments are stale). battle.h's BattleWorkCommand* mirrors and
  the unit_party_* weapon-table macros still cast msgSearch to `char*`. A BattleWeapon status sub-record at 0x80
  would clear battle_stage_object's 16 offsets if another version shows one.
## Second MUST_MATCH pass over the RELs, with the levers found since #1094 (2026-10-21, Claude Opus 5.5 with three Claude Opus 5.5 helpers, n9-q-o-mmrels2)

Task (Isaac): redo #1094's pass over every MUST_MATCH region in rels/ (and the shared include/rel/common headers)
with the levers #1100-#1111 found: int vs s32 locals, `*args++` walks, compound assignment, `/ 2.0f`, calling an
existing helper, loop shape, declaration order, callee prototypes. Where a natural form matches it becomes the only
version; otherwise one line of what was tried.

- **Method.** Untracked harness in build/mr2/: regions.py lists the 131 regions; var.py applies an OLD/NEW variant
  file (any file, headers included), compiles every object that depends on an edited path (from `ninja -t deps`),
  compares each with objects saved from 436e5a2c (tools/obj_equiv.py) and prints per-function normalised asm diff
  counts; gen.py writes variant sets. First every region's natural arm alone and each file's all at once (no EQUIV),
  then per-region variant sets. The sfx-string family, the const-reload/volatile family and the barriern/lecture
  families went to three helpers in their own worktrees (exploration only; their EQUIV variants were re-verified
  and committed here). About 1,500 compiles. Every commit: obj_equiv EQUIV for each touched object,
  `build/G8MJ01/ok` recreated, changed C files compile without -DMUST_MATCH (ci's natural step and the harness's
  NAT=1), fncast_lint and local_prototypes clean, tools/ci.py PASS with the tools/tests step stubbed (the same 9
  environment errors as the chapters above), ratchet `--init`ed on the clean HEAD. tools/ci.py run through `uv`
  rewrites build.ninja with a temporary python, so rerun `python3 configure.py` after it.
- **Counts** (tools/cleanup_index.py, 436e5a2c -> this branch): must_match **788 -> 780**; in rels/ **124 -> 117**,
  include/rel 7 -> 6. field_0x 4626 -> 4663
  (the barriern rename below, 34, and tou2's byte view, 3), so the index went 15.158 -> 15.194 per kLOC (10569 ->
  10593 weighted); everything else unchanged.
- **Natural now:**
  - jin_00 randf_syugo: `f32 r = (f32)rand(); r /= 32767.0f;` instead of the f64 round trip (1). Compound
    assignment; `r = r / 32767.0f` also matches, the one-expression forms don't.
  - mri_puni: qrnd, puniQrnd2 and the three puniSearchRandom copies (5). `v = Punigp->randSeedB; if
    (Punigp->randSeedB < 0) { v = -v; }` keeps the branch; testing the local turns into MWCC's branchless abs idiom,
    which `-(s32)(u32)v` and `(s32)(v ^ 0)` were stopping. New lever: test the value's source, not the local copy.
    26 spellings of the plain test failed first (closest `v *= -1`: mulli for neg, 2 lines).
  - barriern (4: the shared Inside header's volatile InitInline, v1's InitInside and InitInlineOut, v2's
    BARRIERN_SET_UNITID). BarriernSatelliteTo's word at 0x04 holds the `u32` spline table spline_maketable builds
    from; typed `u32*`, barriernSatelliteTo_Init's `work->field_0x4 = table` and `->work[2] = (s32)table` are two
    expressions, so MWCC rematerialises the address for the first and keeps the hoisted copy for the second, which
    the volatile parameter plus duplicate `inv` argument imitated. The 40 call sites pass the table once, uncast;
    v2's unguarded InitInline loses its volatile parameter too. The field was `unitId`, which its type now
    contradicts, so it is the `field_0x4` placeholder. (Helper; found by retyping, not by any of the listed levers.)
  - tou2 rankingControll (1): RankEntry's `u8 score[2]` is an anonymous union of `s16 score` and the byte view
    `field_0x8[2]` (rankingInit/rankingExit move it to and from GSW bytes one at a time; everything else read it
    through `*(s16*)`, eleven puns gone). The guarded u16-view flags store existed only for the score pun: plain
    `entry[nAbove]` stores give retail's sthx sequence. (Helper.)
  - yuu_03 evt_ryushi (1): `f32 dx = entry->x; dx -= cameraX;` and `entry->x += (f32)wrapOffset * step;` give
    retail's entry->x reload and f1 subtraction that the volatile read forced. (Helper.)
- **Left, with what was tried** (data layout untouched by rule: the `.rodata` declspec Vecs in aji_13,
  magnum_battender_mkII, rsh_06, usu_00, yuu_02; tou_02's `const volatile` Vecs; the `"J\0\0"` strings in eki_00,
  rsh_00, rsh_06; tou2's AREA_TOU; rsh.c's Zero256 object and the two debug_print_func regions tied to it (the
  natural `= {0}` moves the .rodata object); tou.c's TOU_F32 and defer_codegen, pik_02's c50/half_sym (pool);
  unit_piders' out-of-line GX helpers; gor evt_yuuranking's asm body):
  - The sfx-string family, 25 regions (eki_00, rsh_05, rsh_06 voiceSfx unions; hei_10; yuu_00/01/02 fall_rtn;
    gor_03 and zako_group2 comma folds; gon_03's array; BOM_SFX, KARON_SFX, FAN_SFX). Any local set from a constant
    address compiles to `addi r0; mr rN, r0` (95 type/order combinations in yuu_01, `char*`, `const char* const`,
    block scope, initialiser vs assignment, a static), and the literal at the call is not hoisted. What retail
    shows is a hoisted *integer* conversion of the literal at the call: with psndSFXOn_3D and psndSFXOn taking `s32`
    (as __psndSFXOn and psndSFXOnVol already do; ~378 callers pass numeric ids as `(const char*)0x...`) and
    `(s32)"SFX_..."` at each call, all 25 go and all 1,228 objects stay identical; npcMain can then call the real
    npcKoopaWaterCheck instead of its hand expansion, and the dol's BSO_SFX/FALL_SFX look the same. **Not applied:**
    it rewrites a dol API in 136 files (+833/-1024), removes 414 `(const char*)` casts but adds 341 `(s32)` casts,
    mostly on string names. It is Isaac's call; the patch (against 436e5a2c; yuu_00 conflicts with this branch's
    first commit) is build/mr2/sfx_s32_proposal.patch in this worktree. hei_10's third region would stay for its
    double Vec copy (`sfxOut = sfxPos`, 112 lines without it). gon_03 would keep two locals holding the same literal.
  - Const-template reloads (retail reloads a const Vec/colour inside a loop or below a store): moamoa_hit_check's
    pvec rebuild in eki_00/rsh_05/rsh_06 (natural 18 lines everywhere; 10 shapes, closest 14 with a local `Mtx*`;
    all three volatiles are needed together), eki_06 sw_entry (20), rsh_06 ext_moamoa_init (natural hoists the
    loads into saved registers), muj_01 sekai_yami_omise (6 shapes, 9-17), las_04 firebar_entry's word copy (162),
    tou_02 ext_fan_init's three stores (4/20/8, ordered against the `const volatile` definition). These need the
    templates as non-const data, out of scope.
  - yuu_00 fall_rtn's y range and yuu_01/yuu_02's z range: each is monte_rand written out with `max -= min`, guarded
    with the monte_rand call as `#else`; the call puts the subtraction in f0. A monte_rand that does `max -= min`
    itself matches the x calls in all three files but not these (the inline's parameter copy).
  - eki_00 and rsh_05 moaPlayHit: guarded in both, since the name has no outside evidence; rsh_05's plain call of it
    is EQUIV. rsh_05's moaSoundVec
    adds a return copy (32/370 lines); a hand copy in function or block scope takes stack slot 0x20, not 0x14.
    rsh_05's MOAMOA_POSE_CHANGE and moamoa_pose_main casts: retail eki_00/rsh_06 inline both, rsh_05 doesn't; no
    definition order or existing inline_max_total_size value keeps them out while the sin/cos inlines stay in.
  - barriern zanzou `(p = BattleGetUnitPtr(...), p)` (v1, v2, shared header; 6 regions): 3 lines natural; ~40
    shapes (unit reuse, `p = ...;` statement, nested calls, local scale copy, `*args++`, declaration orders, int
    index/id, BattleTransID split): any statement form moves the call ahead of the struct copy (13-50 lines).
  - Lecture-button `ppos` (shared header, dou; 4): 20 shapes, best 7 (natural). Only a new nested type at 0x8 plus
    a pointer local matched (hei), which is an invented type and a steering pointer.
  - sun_tev `q` chain (eki_03, hom_00, jon): 24 more shapes for hom_00, best 14 as before (`row` takes r29, not
    r31). test_glare: hom_00 15 shapes, all 2 lines; eki_03 5, all 4.
  - tou_03 screen_tev `zero` alias: 25 variants, all 16 lines (a plain copy of `param` merges into its web).
  - rsh_00 evt_moamoa_set_one (`| dm`, one-iteration loop): 28 variants (`*args++`, int id, three declaration
    orders, `entries + id`, a local entries pointer), 27-35 lines; retail materialises `&wp` (`addi; lwz 0`).
  - gor_03 kuzi_bomb_bomb_eff t0/t1: 25 variants (`*args++`, int, five declaration orders, no args local), all 3
    lines (the npcNameToPtr string through r0).
  - gor_irai irai_cancel_func self-cast (pouchCheckItem is s32 since #1111): ~65 variants (item s32/int/ItemType,
    seven loop shapes, testing `*items`), best 3.
  - unit_lecture_frankli_sac `id = 0`: 34 variants (int, declaration order, loop scope, `ids[i]` repeated, switch,
    testing `ids[i]`), best 7.
  - muj_20 fune_eff_add_x: either operand order and a cast give 4 (85.0f and pos.x land in swapped FPRs).
  - mri_16/tik_00 ss_check and win_04 ss_ch_check: 14 + 14 shapes (args local, walk, f32/s32/int temps, `static
    int ssf`), all 14 and 8 lines: retail's conversion-constant address in r6.
  - bom_01 sightBlink ternary: if/else and flag forms, 5-13 lines (retail normalises the 0/1 with subfe).
  - Left by rule, not retried at length: las_05's dead sqrtf (the dol's evt_npc uses the value; here it is
    overwritten), usu_00's self-assignment, evt_yuunpc's and gor_03 slot's redundant terms (retail has the retests),
    tik_05's reshaped MSL sqrtf tail, pik_02 hoshi_jump's float-to-int-slot puns, the by-pointer GXColor macros
    (mri.c HAKARI_SET_COLOR, both evt_yuuranking), eki_03/hom_00/jon copy chains.
- **For a reviewer:** mri_puni's form reads the global twice (no store between, same value); the barriern field
  rename trades a misleading name for a placeholder (index +34); tou2's anonymous union follows battle_audience.h
  and evtmgr.h.
- **Leads.** The psndSFXOn prototype above (25 REL regions plus the dol's BSO_SFX/FALL_SFX and the ac_button_down,
  ac_shot, eff_nice sfx locals). Testing a value's source instead of its local copy is a lever none of the earlier
  passes tried; it may apply to other abs/branch shapes in src/. Typing a word as the pointer it holds separates an
  int use from a pointer use and undoes CSE the way a volatile did; battle_stage.c's tou_gamen_screen_tev has
  tou_03's `zero` alias and was not tested.

## Placeholder fields named from outside evidence, second pass: the other spellings (2026-10-21, Claude Opus 5.5, n9-q-o-names2)

Second pass after #1112, over the leads its chapter listed in other placeholder spellings (`unkNN`, `field_NN`
without 0x, `unk_*`), then the next seams: getters and setters written as `USER_FUNC(name)` (#1112's scan read only
plain C definitions), REL overlays of a known dol struct, SDK parameters a field is passed to directly, and the data
symbols an original setter is always called with. Candidates came from five read-only Claude Opus 5.5 searches
(mario/party, battle, sound/driver/manager/window, effect/event, RELs) and three scans of our own; every name below
was checked by hand against every user before it went in. The evidence rules are #1112's: "original" means a name in
the fork-point (2df569d) symbol maps, dol or REL; names added on this fork and names inferred from behaviour don't
count.

**Renamed** (accessor = an original function that only reads or writes that field, or does so beside one flag bit;
SDK = a parameter name in libs/dolsdk2004; same struct = the name the repo already gives that memory with outside
evidence):

| struct | off | was | now | evidence |
|---|---|---|---|---|
| MarioWork | 0x1A8 | unk1A8 | dispDir | accessor evt_mario_get_dispdir |
| MarioWork | 0x4C | unk4C | mutekiTime | accessor marioSetMutekiTime (stores sysMsec2Frame(msec), returns it) |
| MarioWork | 0xF8 | field_F8 | camCollision | accessor marioUpdateCamCollision (also sets dispFlags 0x40000) |
| MarioWork | 0x1A4 | unk1A4 | forceDir | accessor evt_mario_set_force_dir (with MARIO_FLAG_USE_FORCE_DIRECTION) |
| MarioWork | 0x168/0x16C/0x170 | unk168/unk16C/unk170 | screenX/screenY/screenZ | marioMove stores marioGetScreenPos's results (GXProject's sx, sy, sz) there, marioChkInScreen tests x and y; 0x170 was `u8[4]` written through an `(s32*)` pun, now s32 |
| PartyEntry | 0x10C | field_10C | dispDir | accessor evt_party_get_dispdir |
| PartyEntry | 0x34 | field_34 | runMode | accessor partyChgRunMode (also N_partyForceChgRunMode) |
| PartyEntry | 0x36 | field_36 | mot | accessor partyChgMot |
| PartyEntry | 0x3B | field_3B | moveMode | accessor partyChgMoveMode |
| PartyEntry | 0x18 | field_18 | pose | accessor partyChgPose |
| PartyEntry | 0x1C | field_1C | paper | accessor partyChgPaper; retyped u32 -> char*, its casts go |
| PartyEntry | 0x2A | field_2A | mutekiTime | marioSetMutekiTime copies Mario's muteki time into it |
| PartyEntry | 0x70 | field_70 | scale | accessor usu_00 party_scale (sets only it) |
| PartyEntry | 0x40 | followDist | homingDist | accessor evt_party_set_homing_dist; followDist was an inferred name |
| PartyCloud | 0x8 | unk8 | breathDist | accessor cloudGetBreathDist |
| PartyCloud | 0x20-0x2C | unk20/24/28/2C | hitBreathout[4] | accessor cloudGetHitBreathout(n) returns slot n |
| BattleWork | 0x1616C | field_1616C | breakSlot | accessor BattleBreakSlotGetPtr |
| BattleWork | 0x19038 | field_19038 | cmdDisable[5] | recover_cmd_disable clears all five |
| BattleWork | 0x19064 | field_19064 | acResult | accessor gor get_ac_result |
| BattleWork | 0x16F3C/0x16F3D | field_16F1C[0x20/0x21] | marioAcSuccess/marioAcFailed | accessors gor _mario_ac_success_check/_mario_ac_failed_check; field_16F1C now ends at 0x16F3C |
| BattleWork | 0x18FF0 | field_18FF0 | acHelp | its only users are BattleAcHelpSetHelp/SetDispType/Main and _ac_help_disp |
| BattleWork | 0x18C70 | field_18C70 | announce | its only users are btlevtcmd_AnnounceMessage/AnnounceSetParam and battle_message's _disp |
| PaperSoundWork | 0x56 | unk56 | flag | accessors psndSetFlag/psndClearFlag/psndGetFlag |
| PaperSoundWork | 0x52 | unk52 | reverb | accessor psndSetReverb (stores the amount for its mode) |
| PaperSoundEffect | 0x12 | unk12 | dist | accessor psndSFX_dist |
| PaperSoundEffect | 0x14 | unk14 | pitch | accessor psndSFX_pit; read only as SoundEfxSetPitch's pitch |
| SoundWork | 0x214 | unk214 | cover | SoundOpenCover/SoundCloseCover are its writers |
| EnvironmentWork | 0xC0 | unkC0 | yamiView | accessor envSetYamiView; retyped u8[0x30] -> Mtx, casts gone |
| EnvironmentYami | 0x18 | unk18 | numIndStage | same struct: the map TEV callback's parameter (eki_03 sun_tev), MapTevCallback's numIndStage; SDK GXSetNumIndStages (bom_01, mri_puni) |
| OffscreenEntry | 0x3C/0x40 | unk3C/unk40 | copyFmt/texFmt | accessor evt_offscreen_set_texture_fmt sets both; SDK GXSetTexCopyDst fmt / GXInitTexObj format tell them apart |
| WindowWork | 0x404 | field_404 | equipList | accessor winMakeEquipList (beside equipListCount) |
| NpcEntry | 0x10C | field_10C | checkPose | accessor tik_02 geso_set_checkpose stores the data symbol geso_checkpose |
| aji_10 MonitorWork | 0x8 | field_8 | kanbuFlag | accessor monitor_kanbu_flag_onoff |
| aji_10 MonitorTevParam | 0xC-0x14 | field_C/10/14 | MapTevCallback | same struct: monitor_cap_tev is an evt_map_set_tevcallback callback; overlay gone |
| rsh RshWindowTevParam | 0x4-0x18 | field_4/C/10/14/18 | MapTevCallback | same struct (rsh_window_tev); SDK GXSetTevOrder(stage, coord, map) from 0xC/0x14/0x10; overlay gone |
| EffMist2Data | 0x38-0x3A | unk38/39/3A | colorR/G/B | tik_05 kemuri_set_color writes its r, g, b there (and a scale at 0x18); eff_mist2 builds the GXSetChanMatColor colour from them |
| InformantUnknown | 0x0/0xD/0x1A | unk0/unkD/unk1A | ketiNewjohoReadFlag/perfectNewjohoReadFlag/luigiNewjohoReadFlag | accessors johoya_{keti,perfect,luigi}_newjoho_setreadflag |
| InformantInfo | 0x0 | unk0 | readFlag | accessor johoya_keti_oldspeak_setreadflag (bit 0) |
| AreaData | 0x8/0xC | field_8/field_C | btlSetupTbl/btlSetupNoTbl | relSetBtlAddr stores its two arguments there, and every area REL passes its original data symbols btlsetup_<area>_tbl and btl_<area>_setup_no_tbl |
| MapVtxFmt | 0x44/0x48 | field_0x44/field_0x48[8] | posFrac/texFrac[8] | SDK: only passed as GXSetVtxAttrFmt's frac for GX_VA_POS and GX_VA_TEX0 + i |

Respelled `field_0xNN` at the same offset, no evidence, for consistency: CamRoadMap `unk_E8`/`unk_108`, CamRoadWork
`unk_204`/`unk_208`, WinLogEntry `unk_0x1C`, effKemuri3Data `unk_04`, gor_irai IraiWork `unk_0x94`, PartyData
`unk_6/7/8_func`. The other ~640 `unk_name` hits are address-named globals (`unk_8xxxxxxx`, `unk_<rel>_<off>`); none
of their addresses has a name in the fork-point maps, so they stay. The remaining `unkNN`/`field_NN` members have no
evidence and keep their spelling.

**Rejected or left as leads:**
- evt_door.c DoorParam 0x24/0x28/0x40 (outside_group, door_group and get_noclip_map read only them: outsideGroup,
  doorGroup, noclipMap). DoorParam carries two layouts: door_entry and its helpers read the nok yado_data layout,
  while evt_door_param, door_dark_flag, npc_hide_onoff and evt_door_set_param read DoorEntry's (evt_door.h). The names
  hold only once those four take a DoorEntry*, which adds an `(s32)` cast at each evtSetValue; not done here.
- Computed, not stored: BattleCameraWork unk68 (battleCameraGetPosMoveSpeed returns its length), PartyCloud unk4
  (cloudGetBreathDir returns toMovedir of it). Same reason #1112 rejected BattleUnitPart 0x1A6.
- By difference only: MarioWork unk150 (marioResetCamShiftRate's only write that marioResetCamShift lacks) and
  unk138/140/148 (marioResetCamShift's); both also reset field_64/68/6C.
- Parallel or inferred: PartyEntry field_110 (evt_party_set_dispdir writes it and copies it into dispDir, as Mario's
  dispReqDir), field_35 (the previous runMode), field_28 (partySetForceMove writes three fields).
- A bit, not the byte: PartyEntry field_4F (L_partyVivianTailStop/Start set and clear bit 0, N_vivianDisplayTailCheck
  tests it; no other bit is used).
- Module names only: FieldBattleData field_20 (the FieldBattleInfo the BattleInformation* functions take), EventEntry
  unk184/unk18C (evt_msg_toge writes them and more), KinopioData field_0-C (one function, breakfast, reads four
  fields), yuu YuunpcPoseEntry field_20 (yuunpc_set_jump_pose stores an f32: names the action, not the field),
  MapVtx field_0x0 (GXBegin's nverts, but MapVtx overlays the per-vertex records too).
- No debug or format string in the source labels a placeholder; matrix-math SDK parameters (`a`, `src`, `xS`) name
  nothing.

Method: rename the declaration, then compile every user with `-maxerrors 500` with and without `-DMUST_MATCH`, and
rename exactly the lines that report "'old' is not a struct/union/class member", until none do (a local script; the
member's other spellings on other structs are untouched, and a wrong edit shows up as the new name's error). The
CI ratchet baseline was `--init`ed on the clean 4ee6832b tree.

- Verified at every commit: `ninja` recreates build/G8MJ01/ok; all 1,251 C files compile without MUST_MATCH;
  fncast_lint and local_prototypes pass; tools/ci.py PASS with its tools/tests step skipped (`~/ci_notests.py`, the
  same environment errors as the chapters above), under `uv run --with pyelftools`.
- Index (tools/cleanup_index.py, same script both sides, 4ee6832b -> this branch): 14.478 -> 14.467 per kLOC
  (10090 -> 10083 weighted); field_0x 4426 -> 4442 (the `unk_` respellings, less posFrac/texFrac); unk_name 684 ->
  661. The index doesn't count the other spellings: `unkNN` tokens 3140 -> 2553, `field_NN` (no 0x) tokens
  5205 -> 4612 (git grep over src, rels, include).

## The next ten worst files by the cleanup index, deep-cleaned, third round (2026-10-21, Claude Opus 5.5, n9-q-o-worst4)

Task (Isaac): take battle_status_icon, party_motion, evt_mario, battle_disp, ac_shot, mot_jabara,
ac_monosiri_target, unit_boss_batten_leader, eff_star_power_n64 and eki_00 each as a whole, as #1110 and #1114 did:
raw offsets, MUST_MATCH, pragmas, gotos, function-pointer casts and puns typed away, no field renames without hard
evidence, no data layout, and guard any unguarded hack found. A parallel branch (worst3) owns mapdrv, envdrv,
sac_genki, party_yoshi, gor_03, mario_hit, party_nokonoko, eff_fire, battle_stage, battle_camera, shadowdrv and
battle_seq, so none of those was edited.

- **Method.** Untracked harness in build/tw4/: try.sh compiles one unit with its ninja command (`FSED` edits the
  flags, `NM=1` drops -DMUST_MATCH) and runs tools/obj_equiv.py against objects saved from 38a70e42; var.py applies
  OLD/NEW blocks and reports EQUIV or the per-function normalised asm diff (fdiff.py); regions.py tries each
  MUST_MATCH region's `#else` alone; pragmas.py removes each scoped pragma pair alone; ci.sh is tools/ci.py with its
  tools/tests step stubbed (the same 9 test_wave_launch environment errors as the chapters above) under `uv run
  --with pyelftools`; nm_all.sh builds every object under `configure.py --non-matching`. Every commit: obj_equiv
  EQUIV for each touched object, `build/G8MJ01/ok` recreated, ci.sh PASS on that commit's own tree, the changed C
  files compiled without -DMUST_MATCH; after the header changes all 1,230 objects built non-matching (only the link
  fails, as before); fncast_lint and local_prototypes clean. The ratchet baseline is the clean 38a70e42 build.
- **Counts** (tools/cleanup_index.py, 38a70e42 -> this branch): index **14.511 -> 14.459** per kLOC (10112 -> 10075
  weighted); opt_pragma 670 -> 650, raw_offset 281 -> 274, field_0x 4479 -> 4474 (a duplicated function body gone),
  must_match 762 -> 764 (three unguarded steering temps guarded, one region removed); goto, fn_ptr_cast,
  address_name, unk_name unchanged. Per file (`--top`, per kLOC; weighted; raw / must_match / pragma / goto /
  fn_ptr_cast):
  - battle_status_icon 174.447 -> 162.095 (71 -> 65): 0/4/6/0/0 -> 0/4/0/0/0
  - party_motion 21.257 -> 21.538 (71 -> 72): 0/16/18/1/0 -> 0/19/16/1/0
  - evt_mario 27.496 -> 27.554 (65 -> 65): 2/7/0/0/0 unchanged (per kLOC rises: 5 code lines went)
  - battle_disp 48.973 -> 48.973 (62 -> 62): 0/0/28/0/0 unchanged
  - ac_shot 124.236 -> 92.873 (61 -> 43): 4/4/10/0/0 -> 0/3/0/0/0
  - mot_jabara 56.850 -> 51.498 (61 -> 55): 9/4/2/0/0 -> 6/4/2/0/0
  - ac_monosiri_target 181.538 -> 176.471 (59 -> 57): 1/3/4/0/0 -> 1/3/2/0/0
  - unit_boss_batten_leader 18.795 -> 16.938 (58 -> 52): 0/0/0/0/0 (its one hack was unmarked; see below)
  - eff_star_power_n64 140.394 -> 140.394 (57 -> 57): 1/2/0/0/0 unchanged (12 puns gone, which the index doesn't count)
  - eki_00 25.980 -> 26.042 (55 -> 55): 0/8/18/0/0 unchanged (two `(&wp->field)[i]` views gone, uncounted)
- **File flags instead of pragmas.** battle_status_icon builds `-O4,s` like the other battle files (three
  optimize_for_size pairs go). ac_shot builds `-O4,s -inline noauto` (three optimize_for_size and two dont_inline
  pairs). ac_monosiri_target and party_motion add `-inline noauto` (one dont_inline pair each). Each object identical.
- **Typing that removed offsets and puns.**
  - ac_shot reads BattleACDispParams' RGBA word at 0xC through a `GXColor* tint` view (four `((u8*)disp)[0xC..0xF]`
    reads and the `u8* col` byte view go); the field stays an s32 (ten action commands store it whole from u32
    constants).
  - MarioWork.jabaraAnimTime (0x2BB) is s8: mot_jabara, its only user, reads it sign-extended everywhere, so twelve
    `(s8)` casts, the MW_S8 offset macro and a widened `s32 t` temp go (`++mario->jabaraAnimTime > 0x78`).
  - EffStarPowerData 0x48-0x54 are f32 (the previous frame's angles and scale, only ever f32): twelve `*(f32*)&` puns.
  - eki_00 MoamoaWork: `Vec targets[3]` and `u16 nearHits[3]`, rsh_06's layout of the same struct. The old comment
    said a Vec[3] broke the match; it no longer does. MOA_TARGET and evt_moamoa_target_off index them directly.
- **Natural shapes that matched.**
  - ac_shot: `_draw_target_mark(const GXColor* color, ...)` (header too) replaces the guarded `__restrict` qualifier
    (the const is what lets MWCC hoist the first colour load). Its `f32 zero/neg/one/half` constant locals and
    their blocks: `if (range)`, `if (aw->rotSpeed)`, `aw->rotSpeed *= -1.0f`, `aw->gauge += 1.0f`,
    `(s32)((min + max) / 2.0f)`. `range != 0.0f` and `0.0f != range` both swap the fcmpu operands; the bare
    float test keeps retail's.
  - party_motion: party_motion_jump and party_motion_homing_fly lose their `f32 zero` locals (`if (party->speed)`;
    `!= 0.0f` is 20 lines off in fly), jump's `ntx/ntz` block and bare `{ floor.y += 10.0f; }` are plain statements,
    and partyFollowHeight reads its 5.0f limit and party->position.y in place instead of reloading them into `f0`/
    `f1` (its per-branch `f3 = 0.1f` stores and the `f1 = 0.05f` operand-order temp change the schedule and stay).
  - evt_mario: evt_mario_paper_pose_dokan's `f32 zero` and evt_mario_set_dir's `s32 n` increment temp. Its
    `start`/`delta` split stays: it keeps retail's separate fmuls/fadds, which `-fp_contract on` would fuse, a
    difference in the computed value, not just in registers.
  - unit_boss_batten_leader: the "Open (Isaac's call)" item of the 2026-10-19 hack review. Its
    battenSatelliteTo_InitInline, an unguarded copy of battenSatelliteTo_Init taking the spline table twice (once
    through a volatile parameter), goes the way #1115 fixed barriern's: BattenSatelliteSpline's word at 0x04 is a
    `u32*` (the table spline_maketable builds from), so the store into it and the later `work[1] = (s32)table` are
    separate expressions with retail's rematerialised address. battenSatelliteTo_Init takes `u32* table` and
    returns u8 (its inlined copies in battenSatelliteToOutside test the result as a byte; with s32 they are 10
    instructions short); all twenty call sites call it once, uncast.
- **Guarded (were unguarded steering).** party_motion's three `f32* pz = &v.z` temps before movePos calls
  (homing_walk once, homing_fly twice): retail computes the z address before the x address, and the plain
  `movePos(&v.x, &v.z, ...)` is 2 lines off each. Now under MUST_MATCH with the plain call in `#else`; homing_walk's
  unneeded `px` temp went.
- **Left, with what was tried.**
  - battle_disp: all 14 pragma pairs. Each pair removed alone changes the object. Under `-inline noauto` only
    btlDispMain differs: retail auto-inlines the static btlDispAnimeSpeedReset (no symbol; its out-of-line body is
    generated and then dropped by the linker, but it pools 1.0f ahead of getFloatDispOffset's 2.0f). Marking it
    `inline` stops the body and moves the pool word (.sdata2 differs); calling btlDispAnimeSpeed directly would need
    the body kept as an unused function placing a pool constant, which the rules refuse. btlUnitItemDisp's
    opt_dead_assignments pair as PRAGMAS_KEPT records.
  - battle_status_icon: the four `__declspec(section ".rodata")` Vec regions (data); the `*(GXColor*)&dat_...`
    reads are puns forced by the u32/f32 .sdata2 definitions (data, plain by the guarding rule).
  - ac_shot: the `("SFX_...", "SFX_...")` comma (the psndSFXOn `s32` lead of #1115 is still Isaac's call) and
    _draw_target_mark's `Vec* vecs = (Vec*)vec3_802f92b8` alias. That cast is to the array's own element type, and
    removing only the cast makes the function 0x30 bytes longer, the same as the plain `vec3_802f92b8[i]` reads.
  - ac_monosiri_target: actionCommandDisp's opt_common_subs pair (the screen2 block puts float_4 in f2 and screen2.x
    in f1; `float_4 + x`, `x + float_4` and a 4.0f literal tried, 8-44 lines); the vec3 array, its extern block and
    the RODATA_D15 offset (data).
  - mot_jabara: the five `MW_S32(mario, 0x2A0..0x2B8) = 0` resets. There is no s32 member at those offsets, each
    `field_0xNN` name is taken by a differently typed member (as #1110 found for mot_hammer), and the f32 members'
    `= 0.0f` stores compile differently (lfs/stfs; 640 lines). JABARA_F32, RDBL and the guarded defer_codegen pair.
  - eff_star_power_n64: the embedded `f7 =` (14 shapes; 14-291 lines, except `4.0f` as a literal for float_4,
    identical code but a second pool word); the .rodata Vec declspec and `testsrc_akari_v + 0x118` (a u8 data array).
  - evt_mario: STR_MUJ_01, EM_VEC/EM_STR (data); the searchUnder2 `(void*)`/`(Vec*)&vec` casts need its prototype
    changed in mario_hit.c, which worst3 owns (the mmsrc chapter's header-copy test found the `HitEntry*`/`f32*`
    prototype EQUIV); evt_koopa_fire's `(s32)(f32)` round trip (49 lines plain); the volatile flags/Vec copy (18).
  - eki_00: the six sin/cos pragma lines (family study), the two dont_inline pairs (under `-inline noauto` five
    functions change, since retail auto-inlines the sin/cos and pose helpers here), the inline_max_total_size pair
    (needed: ext_moamoa_main stops inlining), and its eight regions (the `"J\0\0"` string, const-template volatiles,
    the sfx-string union, moaPlayHit without a name).
  - party_motion: getFrontFloor's goto (`break` plus `i >= 3`/`i == 3` test, or the -2000 store before the loop:
    164-166 lines). homing_walk's `sid` copy: partyGetTargetDist3D taking `u32 id` changes party_move (cmplwi) and
    leaves walk 58 lines off. The `(s8)currentMemberId` cast: retail tests the byte unsigned for the BOBBERY compare
    and sign-extended for the YOSHI one. homing_walk's `(s64)` address round trip (the pz-temp form is 518 lines
    off), the radius dead store, jump's nested travel-time assignments (18 lines without them, with or without the
    dead-assignments pragma), the three redundant `hit` re-tests and the 0/1 ternary. Every other pragma pair re-tested
    alone after the changes still differs (10-683 lines).
- **For a reviewer.** The bare float tests (`if (range)`, `if (party->speed)`) are the shape that matches; each
  `!= 0.0f` spelling swaps a compare's operands or moves registers. The header changes: ac_shot.h (const colour
  pointer), eff_star_power_n64.h (four f32 fields), mario.h (jabaraAnimTime s8), rels/las unit_boss_batten_leader.h
  (battenSatelliteTo_Init's prototype). The three `-inline noauto` and two `-O4,s` additions are configure.py
  Object lines. party_motion's index rises by one: three newly guarded hacks against two pragma lines removed.
- **Leads.** The `-O4,s` lever for battle code built at the default `-O4,p`: ac_timing_wide, ac_signal_timing,
  battle_ac_help, battle_ac, ac_repeatedly_lv and ac_gauge_timing each compile identical with `-O4,s` and none of
  their optimize_for_size pairs (8 pairs), and with `-O4,s -inline noauto` also without their dont_inline pairs
  (3 more); tested only (outside this task's files). evt_mario's and evt_party's searchUnder2 casts once mario_hit
  is free to edit. PRAGMAS_KEPT's line numbers for party_motion and ac_monosiri_target have shifted; its entries
  still hold, except party_motion_jump's dead-assignments diff, now 18 lines (was 8).

## The next twelve worst files by the cleanup index, deep-cleaned (2026-10-21, Claude Opus 5.5 orchestrating four Claude Opus 5.5 workers, n9-q-o-worst3)

Task (Isaac): take mapdrv, envdrv, sac_genki, party_yoshi, gor_03, mario_hit, party_nokonoko, eff_fire, battle_stage,
battle_camera, shadowdrv and battle_seq (`cleanup_index.py --top 30` after #1114) each as a whole, as #1110 and #1114
did: raw offsets, MUST_MATCH, pragmas, gotos, function-pointer casts and puns, guarding existing unguarded hacks; no
field renames without hard evidence, no data layout. The orchestrator took battle_camera, battle_stage and battle_seq;
workers took mapdrv+envdrv, eff_fire+shadowdrv+sac_genki, party_yoshi+party_nokonoko and gor_03+mario_hit, editing only
their own .c files (headers through a private include overlay, applied by the orchestrator).

- **Method.** Untracked harness in build/tw3/: try.sh (one unit with its ninja command, obj_equiv against objects saved
  from 4ee6832b; `NM=1`, `FLAGS=`, `OVL=`, `OUT=` per worker), var.py, fdiff.py, as #1114's. The first four workers
  stopped together about 25 minutes in, with no report and some files mid-experiment (overlay-dependent); fresh workers
  continued from their edits (checkpoints in build/tw3/w/*/checkpoint1) and reviewed them. Subagents could not write
  report files in this harness, so their reports came back as messages; the orchestrator saved them as
  build/tw3/w/*/REPORT.md. Every commit: tools/ci.py PASS on that commit's own tree (tools/tests stubbed for the eight
  test_wave_launch environment errors, pyelftools through `uv run`), `build/G8MJ01/ok` recreated, fncast_lint and
  local_prototypes clean; every header change compiled EQUIV and NM OK in every includer (mario_hit.h: 25 files;
  mapdrv.h/envdrv.h: 789 sources).
- **Counts** (tools/cleanup_index.py, 4ee6832b -> this branch): index **14.478 -> 14.487** per kLOC (10090 -> 10097
  weighted). raw_offset 281 -> 232, opt_pragma 670 -> 626, goto 115 -> 106, fn_ptr_cast 51 -> 50, must_match 770 -> 838,
  field_0x 4426 -> 4514, address_name 2625 -> 2632, unk_name 684 -> 690. The index rises because unguarded hacks were
  guarded (+68 regions) and typing turned uncounted casts into counted `field_0xNN` placeholders (mapdrv +63). Per file
  (per kLOC; weighted; raw / must_match / pragma / goto / fn_ptr_cast):
  - mapdrv 25.958 -> 37.698 (103 -> 150): 16/4/8/2/0 -> 7/12/6/0/0; field_0x 55 -> 118
  - envdrv 64.516 -> 72.158 (98 -> 113): 1/3/16/0/0 -> 0/16/16/0/0
  - sac_genki 59.110 -> 59.487 (97 -> 95): 0/6/2/0/5 -> 0/6/0/0/5 (42 code lines went)
  - party_yoshi 59.255 -> 70.405 (97 -> 113): 19/6/0/1/0 -> 15/18/0/1/0
  - gor_03 16.485 -> 15.608 (89 -> 85): 1/4/0/2/0 -> 0/9/0/2/0; field_0x 79 -> 72
  - mario_hit 33.648 -> 36.742 (89 -> 97): 0/4/4/0/0 -> 0/10/4/0/0
  - party_nokonoko 64.353 -> 5.166 (89 -> 7): 32/1/0/7/0 -> 0/6/0/0/0; address_name 9 -> 0
  - eff_fire 87.810 -> 92.369 (85 -> 92): 0/8/2/0/0 -> 0/13/2/0/0
  - battle_stage 82.902 -> 71.354 (80 -> 68): 1/3/22/0/0 -> 1/3/10/0/0
  - battle_camera 58.372 -> 51.618 (76 -> 67): 0/3/26/0/0 -> 0/10/0/0/0; field_0x 25 -> 35
  - shadowdrv 60.606 -> 60.317 (76 -> 76): 6/8/4/0/3 -> 4/15/4/0/2
  - battle_seq 26.345 -> 25.641 (72 -> 70): 0/3/4/0/1 -> 0/3/2/0/1
- **Per-file `-inline noauto`** (#1114's lever): battle_camera, battle_stage, battle_seq and sac_genki are identical
  without their 32 dont_inline lines and sac_genki's auto_inline pair. mario_hit is not (18 functions differ:
  marioHitCheckVec and others are auto-inlined in retail).
- **Typing.**
  - BattleCameraWork 0xFC/0x100 are f32s (the current btl_camera_shake_w x and btl_camera_shake_h y offsets; battle_camera
    is their only user). battle_seq's u64 mAnnouncedFlags is zeroed plainly (EQUIV; the two-word pun went).
  - mario_hit.h: eleven probes return HitEntry* (each returns a hitCheckVecFilter/marioHitCheckVec result or NULL); the
    casts in mario_hit, evt_mario, evt_party and mot_jabara go.
  - mapdrv.h types the map file's chunks after their chunk strings (vcd_table's MapVtxArray pointers, texture_table,
    material_name_table, animation_table), MapHeader.data/rel (u8*/u32*; SPM's MapFileContext has mainData/relTable
    there), the texture-info block (an anonymous struct whose 0x8/0xA/0xC take TEXHeader's width/height/data), and
    MapMatTexMtx's five channels also as an array (mapAnimColorObj indexes them). mapSearchDmdJoint/Sub/Sub2 take
    MapFileJoint* and const char* (SPM's prototype; Sub2's opt_lifetimes pair goes; the '|' test is `(s8)*name`).
  - envdrv.h: EnvironmentWork 0x98 is the EnvironmentBlur (name unk98 kept), 0xC0 an Mtx, EnvironmentYami.unk0 a
    MapObject* (MapTevCallback.obj at the same offset).
  - sac_genki's file-local GenkiStar: eighteen floats are six Vecs (whole-Vec copies at each offset; 16 puns go).
  - gor_03's upsidegara is indexed as the u32[48] it is; party_nokonoko's field_10C updates read the field once
    revise360's result is no longer staged through a local (the `f1` temp was what made the named field differ).
- **Names with outside evidence.** shadowdrv's repeated GX setup is SPM's polygonInit (0x80035FC0) and textureInit
  (0x80036174) (~/spm-refs/symbols.txt, called from shadowDisp/Projection/Polygon and Texture), now static inlines with
  plain calls: four MUST_MATCH regions and a function-pointer cast macro go. mapdrv's inlined material lookup is SPM's
  mapSearchDmdMaterial (0x8008C2EC). No struct field was renamed.
- **Reshaped copies removed** (structural fakes): mario_hit's marioHitCheckVecRound (callers call marioHitCheckVec),
  gndCenti/gndAngle(Plain); mapdrv's mapFindAnm (mapSearchAnmObj indexes `&wp->entries[i]` in its loop, which gives
  retail's registers in it and its four inlined copies) and mapSearchTextureIdx; written-out copies of party_yoshi's
  unk_800b474c and L_sweatEntry and party_nokonoko's nokonoko_use_post are calls.
- **Guarded** (unguarded before; each plain form re-tested first and still different, plain `#else` arms): invented
  inline names bcam_get_unit, getUnitPtr (battle_camera), marioHitCheckVecAttr, marioChkGndRollProbe (mario_hit),
  slot_count_hit, slot_stop_sub (gor_03), yoshiSearchHead, yoshiRideHeight (party_yoshi), eff_fire's four colour
  inlines, shadowdrv's shadowMatColor/shadowTevColor/shadowDispProjMtx, mapFlushColor; steering temps and copies
  (battle_camera's `_t`, mario_hit's `ofs`/`t`, shadowdrv's u64 locals, `wp`, `q`, void* work); puns (party_nokonoko's
  koops zero stores, party_yoshi's vec base pointer); dead stores (gor_03's angle wrap, its terminator copy's
  increments, mapdrv's negated d == 0 flag, which replaces the `__cntlzw` intrinsic). envdrv's steering forms that
  n9-q-s-mmenv had made the only version are guarded again under the current Standing rules.
- **Rejected in review.** party.h union members giving the koops 0x178/0x17C bands an s32 type only for punned zero
  stores (the pun respelled); a relabel of party_yoshi's ten_pool_helper as a STRIPPED FUNCTION PLACEHOLDER (nothing
  shows a stripped function there; the original comment is back); eff_fire's placeholder patch (pool layout, not
  EQUIV); mario_hit's `-pragma "inline_max_size(N)"` in extra_cflags (N 530..920 EQUIV and drops both pragma pairs, but
  it is a pragma moved to the command line with no precedent: Isaac's call).
- **Left, with what was tried** (details in the reports):
  - battle_stage: BattleStageDispDark's opt_common_subs (the -640/640 loads swap: 24 lines), the light-setter
    opt_propagation pairs (stage/light pointer folding; 28-42 lines; `light += idx` no better), drawCone's
    MATCH_F32 reads (the extern constants are const .sdata2 definitions). battle_seq: battleSortPhaseMoveTable's
    propagation (count takes r31 in every shape, 30 lines), `_rule_disp`'s Waku_col cast (passing the global by value is
    an r6/r7 swap, 12 lines), the `(0, x)` commas (54 lines plain; the weapon is a u8 data definition).
  - mapdrv: `entry + 0x178` (writes past MapEntry), the packed-list walk (6-16 lines), mapUnLoad's propagation (5 lines
    over ~70 variants), mapMain's common_subs (493), spline_getvalue's lifetimes (28). The single-use loader inlines
    (mapLoadHeader, mapLoadRelocate, mapLoadAnimTable, mapLoadDispFlags) and the mapAnim*/mapDrawObjectFog/spline_*
    inlines are invented names left unguarded: guarding them means hand-expanding most of _mapLoad (decision for a
    later pass; SPM's `Locate` may be evidence for mapLoadHeader). The `zero` locals stay plain.
  - envdrv: the sin/cos pairs (family decision), DoF's inline_max_size (278-856 lines), envSetWater (26/35),
    envSetYami (92), envDofFar as a call of envDofTaps (14), envGlare's 8-byte int walk (6).
  - shadowdrv: u64 bufferSize (any 32-bit type 4-6 lines), cylinder's common_subs (pool order), the Texture FIFO
    block, `shadowConfig + idx * 0x48` (u8[144] data). sac_genki: the icon-colour casts (fncast2's verdict; plain calls
    170 lines). eff_fire: Disp3's volatile zero-Vec reads (184-384).
  - party_yoshi: 0x2A0/0x2B4 raw (MarioWork's union names there are other types), yoshi_use's volatile copies,
    yoshi_exit's goto. party_nokonoko: fadd32's operand order (16 shapes, best 2). gor_03: slot_rotate_sub's gotos
    (115-198), fall_rtn's string commas (16), main_slot's dead OR terms (17). mario_hit's pragma pairs (see above).
- **For a reviewer.** mario_hit's and gor_03's `#else` arms are written-out copies of the guarded inlines (mario_hit's
  roll probe uses its own `probe` local so the natural build keeps the outer `hit`). party_yoshi's `!= 0xFFFFFFFFu`
  compares an s32 field unsigned, as retail. mapdrv's MapMatTexMtx union and MapVtxList (file-local) are new types;
  the new type names come from chunk strings. shadowdrv's setters keep an `entry` element pointer (retail's folded
  stbx; plain 8 lines).
- **Leads.** The `.sbss2` "templates" in eff_fire and shadowdrv are MWCC's own compound-literal templates: with
  `(GXColor){...}` at the sites (as SPM writes them) all of eff_fire's colour-inline guards and most of shadowdrv's
  colour regions go, EQUIV (data layout; patches in build/tw3/w/eff/). party_yoshi: one zero-Vec inline with compound
  literals removes its goto and 11 regions (needs an inline name and deletes three vec3 definitions; patch in
  build/tw3/w/party/). Typed data definitions that clear puns: shadowConfig, eff_fire's col_tbl and colours,
  sac_genki's vec3_*. `#pragma pool_data on` is redundant in party_yoshi and party_nokonoko. hitdrv can read the typed
  vcd_table; evt_map with a GXColor* mapGetFog; hei_09's envSetWater argument is a MapTevCallback*; the spline
  functions could take f32*. EventEntry 0x84 is a fourth userdata word (gor_03 writes it through a pun, muj reads
  userdataf[3] past a [3] array). A separate local holding a call result can be what keeps a named field from
  matching: fold it before keeping a raw offset.

## Cleanup index counts Melee's pointer-retype families (2026-10-22, Isaac; written up by Claude Opus 5.5, n9-q-idxmelee)

Isaac asked for the index to cover the categories Melee's cleanup index treats as important. Ours already had raw
offsets (Melee's cast+offset, cast+index and raw-offset), goto, MUST_MATCH, pragmas and inline asm, but not the
pointer retypes, which are Melee's largest families. `tools/cleanup_index.py` now counts them, written here from
Melee's category list (none of its code): struct_ptr_cast (1), prim_ptr_cast (1, `void*` and `char*` included as
Melee counts them), deref_cast (2), ptr_to_int (1), abs_address (2), pad_stack (1) and longjmp (2). The report also
prints the Melee-family site total, unweighted and without names, for comparing with Melee's own index.

- Calibration against Melee's script run unmodified over the same tree (2433c1fc): struct casts 2,708 vs 2,561,
  primitive casts 2,269 vs 1,875, deref casts 1,180 vs 1,257, ptr-to-int 2,694 vs 2,242. `(s32)"name"` arguments in
  event scripts are not counted: the script format stores every argument as s32, so they are data shape, not debt
  (counting them gave 9,184 ptr-to-int sites). Pragmas stay counted as optimisation pragmas only (606); Melee's
  script counts every pragma line, `#pragma once` included (2,873 here).
- The headline index on main went 14.471 -> 28.914 per kLOC under the new definition; compare numbers only within one
  definition. Melee-family sites on main: 10,696. Melee's own index on Melee (f0cdbb0a): 5,718 sites.


## Colour and vector templates as compound literals (2026-10-22, Claude Opus 5.5, n9-q-o-literals)

Many named `GXColor`/`Vec` objects (`dat_*`, `unk_*` in hand-placed `.sbss2`, `vec3_*`) were MWCC's own anonymous
templates for compound literals. They are now written as `(GXColor){...}` / `(Vec){...}` (or initialised local
arrays) at their uses, with the named definitions deleted along with the MUST_MATCH inlines, volatile copies,
`COPY_VEC`-style macros and GXColor*-cast helpers that existed only to load them. Isaac approved the data change
(the templates become `@NNN` locals; symbols.txt renames three in eff_sstar_n64, so CI needs `--allow-protected`).
Precedent: Wind Waker writes 84 such literals and Twilight Princess 113.

- **Done (13 files):** eff_thruhammer_n64, eff_recover_heart_n64, eff_kumokumo_n64, eff_sstar_n64, eff_glass_n64,
  eff_fire, shadowdrv, mot_damage, mot_yoshi, mot_dokan, mot_plane, sac_deka, party_bomhei. Build matches, data
  included.
- **Tried and left:** mot_hammer, and some named objects in eff_fire (`vec3_802f8c4c`), shadowdrv
  (`dat_804198ec`/`f0`) and mot_plane (`vec3_802c5c9c`): converting them changes section order or code.
- **Index:** 28.914 -> 28.423 per kLOC (MUST_MATCH 834 -> 800).
- **Next:** about 210 candidate files remain (n64 effects, other effects, battle/party, drivers, RELs). An
  interrupted six-agent sweep left committed but unreviewed work in the tower's `~/ttyd-src/.claude/worktrees/agent-*`
  branches; review those before redoing them.
## Whole-file compiler settings moved to configure.py (2026-10-22, Isaac; done by Claude Opus 5.5, n9-q-o-flags)

Task (Isaac, after checking precedent): where one per-file compiler setting replaces a file's scoped pragma pairs, put the
setting on the file's `Object(...)` line and remove the pairs. Wind Waker sets `-inline noauto` per object and
`-pragma "opt_propagation off"` per library; Twilight Princess sets `-inline noauto` per library and
`-pragma "nosyminline on"` per object; Animal Crossing sets `-O4,s` per object and even `-pragma "scheduling 7400"`
per object. The rule: a flag goes on an Object line only if the whole object compiles identically with it, and every
pragma line of the kind it replaces is removed (other kinds in the same file stay unless their own flag also holds).

- **Method.** An untracked harness (build/fl/ in this worktree): survey.py takes every src/ and rels/ source with an
  optimisation pragma (130 files; libs/ is protected and was left alone), and for every subset of the pragma kinds in
  the file strips those lines and compiles a copy with each candidate flag (`-inline noauto`/`off`, `-O4,s`,
  `-pragma "opt_X off"`, `-pragma "inline_max_size(N)"` over 19 values of N, `inline_max_total_size`, `inline_depth`,
  `-O2`), using the object's own command line from build.ninja, then compares it with the baseline object
  (tools/obj_equiv.py; every unmodified copy compared equal first). 2,055 compiles. A second pass over the 65 files that
  still have pragmas (2,596 compiles, adding `-pragma "dont_inline on"`, `auto_inline off`, `opt_X on`, `-O4,p`, `-O3`,
  `-O2,s`/`p` and noauto with the other -O level) found nothing more. Every commit: `build/G8MJ01/ok` recreated, every
  changed object built under `configure.py --non-matching`, fncast_lint and local_prototypes clean, tools/ci.py PASS
  with `--allow-configure --allow-pragma` (the tests step stubbed for the eight environment-only test_wave_launch
  errors; with pyelftools supplied the rest of tools/tests passes).
- **Applied (86 files, 86 Object lines; 274 optimisation pragma lines and one push/pop pair removed).**
  - `-inline noauto`, replacing all 167 dont_inline lines in 46 files: ac_air_gauge, ac_pendulum_crane_timing,
    ac_repeatedly, ac_repeatedly_lv, ac_shot_target, ac_signal_timing, ac_stick_rotate, ac_timing_a, battle_ac,
    battle_actrecord, battle_audience_kinopio, battle_break_slot, battle_damage, battle_enemy_item, battle_event_cmd,
    battle_icon, battle_information, battle_menu_disp, battle_pad, battle_seq_end, battle_status_effect,
    battle_unit_event, sac_bakugame, sac_deka, sac_muki, sac_scissor, sac_suki, sac_zubastar, evt_npc, fadedrv,
    eff_ripple_n64, koopa_motion, mario_kemuri, mariost, memory, mot_hip; RELs evt_lect_gon, gon_03, gon_11, rsh,
    tou2, tou_03, unit_fire_bros, unit_wanawana, and the tou2 copies of the shared unit_hammer_bros and v2/unit_piders.
    unit_hammer_bros's tik copy compiles identical with no flag (retail inlines the two target helpers there), so the
    UNIT_DATA_SYM token-paste gate that scoped the pragma to tou2 went too. mario_hit's dont_inline pair went with its
    inline_max_size flag (below).
  - `-O4,s`, replacing all 16 optimize_for_size lines: ac_gauge_timing, ac_repeatedly_lv, ac_signal_timing,
    ac_timing_wide, battle_ac, battle_ac_help (the rest of src/battle already builds `-O4,s`).
  - `-pragma "opt_X off"`, 78 lines in 32 files. opt_common_subs: ac_monosiri_target, ac_shot_target, eff_funemizu,
    eff_mist, eff_mugi, koopa_hit, eff_blow_n64, eff_breath_fire_n64, eff_butterfly_n64, eff_kemuri10_n64,
    eff_kemuri4_n64, eff_magic3_n64, eff_rothammer_fail_n64, eff_small_star_n64, eff_thunderflare_n64, eff_vapor_n64.
    opt_lifetimes: eff_gonbaba_breath, eff_starstone, eff_broken_barrier_n64, evt_sub, evt_uranaisi, gor_10, hei_10.
    opt_propagation: eff_flame_n64, evt_lottery, evt_yuutitle, rsh_05. Both lifetimes and propagation: eff_starpoint,
    evt_item, pik_02, tou_02. opt_loop_invariants: eff_pointget.
  - Inline limits, 15 lines: mario_hit `-pragma "inline_max_size(600)"` (Isaac's lead: both its pairs go; N from 521 to
    935 is identical, 1000, the old pragma's value, is not once the dont_inline pair goes; 600 is a pick inside the
    range, not evidence); envdrv `inline_max_size(2000)` and evt_badgeshop `inline_max_size(160)`, the values their
    pairs used (envdrv works from 1200 up, badgeshop from 140 to below 200); eki_00, rsh_05, rsh_06
    `inline_max_total_size(60000)`; mri_puni `inline_depth(5)` (its push/pop went with it).
  - Comments that only justified a removed pragma went (176 comment lines out, 13 rewritten in; mostly "dont_inline
    keeps X a real `bl`"); mixed ones keep their other content; mario_hit's roll-probe and mri_puni's recursion
    comments now name the unit's flag. docs/PRAGMAS_KEPT.md loses the 40 entries for the regions that went (16 of them
    sin/cos family), with a note at its top.
- **Counts** (tools/cleanup_index.py, 554ff9e6 -> this branch): index **28.914 -> 28.532** per kLOC (20148 -> 19874
  weighted); opt_pragma 606 -> 332; every other category unchanged. Code lines 696,833 -> 696,546.
- **Left, and why.** No per-file flag holds for: dont_inline in party, party_move, pmario_sound, mot_roll, battle_disp,
  evtmgr, eki_00, rsh_00, rsh_05, rsh_06, end_unknown, unit_boss_black_peach and mri_puni (57 lines; retail auto-inlines
  something else in each, e.g. battle_disp's btlDispAnimeSpeedReset); the remaining opt_propagation (103 lines),
  opt_lifetimes (63), opt_common_subs (38), opt_dead_assignments (26) and opt_loop_invariants (4) pairs, where the
  setting file-wide changes some other function; battle_sub's fp_contract pair (`-fp_contract off` moves code);
  npcdrv's and eff_ripple_n64's optimization_level 2 (`-O2`, `-O3`, `-O2,s` and the pragma form all differ). Not in
  scope: `#pragma pool_data` (346 lines, data layout) and `defer_codegen` (pool placement); libs/ (protected, SDK and
  runtime sources).
- **For a reviewer.** The flags are configure.py Object lines only; check the form against TWW's
  (`'-pragma "opt_propagation off"'`). mario_hit's N is the one number here with no evidence behind it beyond the
  range. The opt_* flags say the setting holds for every function in the file; for one-function effects that is the
  same claim the pair made, for multi-function files (koopa_hit, evt_item, evt_sub, the RELs) it is a stronger one that
  the build supports. Comment removals are the bulk of the diff; no code line changed apart from ac_timing_wide's
  duplicate actionCommandDisp prototype (it sat inside the removed region) and the hammer_bros gate macros.
- **Leads.** None of the remaining pragma kinds has a per-file flag; the next lever for them is per-function source
  shapes (PRAGMAS_KEPT.md). A library-level `-pragma "pool_data off"`, if data layout comes into scope: 317 of the
  pool_data lines are `off`.

## Colour and vector templates as compound literals, round 2 (2026-10-22, Claude Opus 5.5 with five Claude Opus 5.5 helpers, n9-q-o-literals2)

Continues the n9-q-o-literals chapter (Isaac's approval and the TWW/TP precedent are there): named `GXColor`/`Vec`
objects that were MWCC's compound-literal templates are written as `(GXColor){...}`/`(Vec){...}`, initialised locals or
local arrays at their uses, and the definitions go with the helpers, puns, volatile copies and MUST_MATCH regions that
only loaded them.

- **Salvage.** The six interrupted round-1 sweep worktrees (`~/ttyd-src/.claude/worktrees/agent-{a35ec,a65ef,ab325,
  ad811,adf08,aed5e}`) held 96 file conversions (committed and uncommitted) plus two symbols.txt edits. All applied to
  current main (seven after resolving #1121's pragma removals) and the full build stayed byte-identical; reviewed and
  landed in five commits (RELs 10, n64 effects 47, effects 21, action commands 9, drivers/events 9).
- **The rest.** The ~186 remaining candidate files (every file with a hand-placed `.sbss2` section, a
  `*(GXColor*)&dat_/unk_` pun, a `vec3_*`/`dat_*` GXColor or Vec object) went to five helpers in nested worktrees, one
  batch each (other effects 30, n64 effects 35, battle/party/mario 36, drivers/events/windows/sequences 34, RELs 51).
  Every file got an outcome; per-file reports were in the helpers' scratch dir. About 104 more files converted fully or
  in part.
- **Method and checks.** Each kept change: `build/G8MJ01/ok` recreated (dol and all REL checksums, code and data),
  tools/ci.py PASS (tests step stubbed, `~/ci_notests.py`; pyelftools from a venv) with `--allow-protected` for
  symbols.txt, which includes the natural compile of every changed file without MUST_MATCH and the data ratchet;
  fncast_lint and local_prototypes clean.
- **gap_ objects stay.** Round 1's salvage and two helpers had deleted the `.sbss2` `gap_*` pad after a template
  wherever the build stayed identical. The task's rules keep gap/force_active dummies, so all of them were restored (19
  files: ac_shot, eff_damage_star, eff_energy, eff_mist, eff_nozle, eff_magic1/magic_horao/opuku_jetw/opuku_wcurtain/
  opuku_wgun/pokopi_thunder/rappa/rothammer_fail/snowfall/whirlwind_n64, and, by the helpers, eff_fukidashi,
  eff_kemutest, seq_mapchange, win_log) at the original's offset (checked against the target split). With a gap back, objdiff no
  longer pairs the template by position, so its symbols.txt line is renamed to the compiler's `@NNN`
  (`scope:local`), as round 1 did for eff_sstar. snowfall's gap uses its map name `gap_10_8042326C_sbss2`. The restored
  `#pragma section` lines are the original's: ci lint passes against the branch base, not against the previous commit.
- **symbols.txt.** 47 address-named templates renamed to their `@NNN` locals, each only because the data ratchet
  dropped without it.
- **Other changes riding on the conversions** (all byte-identical): bgdrv's `bgReEntry` calls `bgEntry` instead of a
  hand-expanded copy of it (retail inlines it); evt_batstage's and sac_suki's tables became initialised local arrays
  and their blob/copy structs went; eff_syuryou's and mario_motion's strings became literals (their base was the
  template; mario_motion's force_active MUST_MATCH block went); itemdrv, evt_telop, kpa_act and evt_yuutitle declare
  FontDrawColor by value, as fontmgr defines it; sac_zubastar's `#else` read undefined `vec3_802ff7xx` (natural
  build broken) and now uses literals; existing definitions moved within their files in eff_funemizu, eff_mugi,
  eff_stageclear, eff_teresa, eff_fukidashi, evt_win, seq_load, seq_title, win_item, msgdrv, koopa_hit, sac_genki,
  mot_hammer, mario_hit, ac_shot, ac_shot_target, ac_repeatedly_lv, battle_break_slot and gor_01, so the templates keep
  the original's order (layout unchanged). eff_damage_star's existing colour helper now takes the element, not its
  alpha (the alpha load has to sit inside it; no other form matched).
- **Index** (tools/cleanup_index.py, f867c03c -> this branch): **28.041 -> 24.571** per kLOC (19519 -> 17016
  weighted); must_match 800 -> 692; deref_cast 1126 -> 795; address_name 2475 -> 1076; unk_name 619 -> 305;
  struct_ptr_cast 2707 -> 2696; prim_ptr_cast 2266 -> 2261; field_0x 4562 -> 4558; Melee-family sites 10328 -> 9873.
  Code lines 696,079 -> 692,530; about 600 named definitions deleted.
- **Left, and why.** Most leftovers are templates whose literal lands in the wrong slot: a literal's template is
  emitted with its function, so where the original's named objects sit before or among strings, a defer_codegen
  placeholder's pool double or another function's literals, no literal reproduces the order (most REL files, winmgr,
  eff_fire, eff_particle, eff_kiss's `str_804213F0/F4`, which are colour templates under string names, the yami_view
  position/up Vecs, the ac_monosiri_target/ac_shot_target Vecs). Objects read from several functions are real shared
  constants (party_yoshi, party_bomhei, party_chuchu, mot_slit, windowdrv, jin_07); a few need non-const objects or
  differ only in stack slots or registers (evt_map, koopa_hit, sac_genki's disp_3D Vecs, battle_stage's `.sbss2`
  colours). animdrv's six `.sdata2` colours convert only if its `moamoa_chain_count` force_active pair moves up the
  file, which lint counts as a new pragma; not applied. Per-file outcomes were in the helpers' reports.
- **For a reviewer.** The yami_view-style camera functions in eight RELs now mix a literal target with named position
  and up Vecs; that mix is what the original's `.rodata` order says. The moved definitions listed above. The data
  check passed for every changed object (183).
## Pointer-retype casts removed by fixing declarations (2026-10-22, Claude Opus 5.5 orchestrating eight Claude Opus 5.5 workers, n9-q-o-retype)

Task (Isaac): work down the retype families #1119 added to the index (deref_cast, struct_ptr_cast, prim_ptr_cast,
ptr_to_int), fixing the declaration behind a cast rather than the cast: struct members, unions, prototypes, locals;
removing `(T*)` casts C does implicitly; typed locals for `void*` work pointers; leaving casts that steer codegen as they
are (no new MUST_MATCH regions).

- **Counts** (`tools/cleanup_index.py`, rebased onto cee4c1fa (#1122) -> this branch): index **24.571 -> 21.493** per
  kLOC (17,016 -> 14,888 weighted). struct_ptr_cast **2,696 -> 1,464**, prim_ptr_cast **2,261 -> 1,901**, deref_cast
  **795 -> 526**, ptr_to_int 2,694 -> 2,694, abs_address 16 -> 2, raw_offset 225 -> 215; field_0x 4,558 -> 4,608
  (placeholder members added where a byte block became typed fields, and raw reads that now name an existing
  placeholder). Melee-family sites 9,873 -> 7,988. must_match, opt_pragma, goto, fn_ptr_cast unchanged.
- **Method.** Scratch in the untracked build/retype/. sites.py lists every counted site with its context (code, macro,
  file-scope data initialiser); chk.py compiles one file with the build's own command into a temp dir and compares it with
  a snapshot of the original objects through tools/obj_equiv.py (`--nm` also compiles without MUST_MATCH, `--users H`
  every .c that includes header H transitively), so workers could verify in parallel without touching build/. The tree
  was split into eight disjoint groups (src/event; src/effect; src/battle+unit; src/driver+manager;
  src/window+mario+sequence+top level; src/motion+party+data; rels gor/tou/yuu/end/tou2/dou; the other rels), each
  owning its .c files and the headers only its own files include; headers included across groups were proposed as
  patches, tested with private header copies, and applied by the orchestrator afterwards. Every commit was checked out
  and built on its own: build/G8MJ01/ok recreated and all 1,223 objects obj_equiv-equal to the original.
- **What changed, by kind.**
  - Implicit conversions: `*(TEXPalette**)entry->data` -> `*entry->data` (FileEntry.data is a `void**` handle), casts on
    `_mapAlloc`/`__memAlloc`/`smartAlloc(...)->address`, `WinMgrEntry.param`, `mExtraWork`, `userdata` assigned to a
    typed local, `void*` SDK parameters, string literals, same-type casts.
  - Declarations: file-local structs and globals typed (party_move's `footmarkBuf` is `Footmark[2][0x50]`; YuunpcEntry's
    float triples are Vecs and its partial views YuunpcMapWork/ErrPer go; filemgr's relocation switch takes one typed
    local per file type; battle_menu_disp uses battle_seq_command.h's command tables instead of private copies; evt_door's
    door functions take the header's DoorEntry*; uninitialised .bss/.sbss work pointers get their element types).
  - Shared headers: ItemData.buy_price u16 (ItemDataView goes); InformantInfo/InformantWork, CaseEntry.field_0x94,
    FieldBattleData.field_570, BattleACDispParams/BattleActions words, defence tables s8, mPowerBounceRecord u16;
    PartyCloud.hitBreathout and cloudGetHitBreathout HitEntry*; MarioWork's thirteen hit fields HitEntry*;
    const char* names for msgGetCommand, door_position_sub, door_dark_flag_sub, itemNameToPtr,
    bero_get_position_pipe_sub, pouchSetYoshiName, pouchHiraganaToKatakana, animPoseDisp_MakeExtTexture and the battle
    command tables' mName; BattleDamageDirect takes the BattleUnitPart* its callers passed as s32;
    effStarStoneData.matColor GXColor, effNanigaData's position a Vec, LensflareEntry's head fields, effBalloonData's
    BalloonSeg ring; effSetVtxDescN64 takes `void*` (all it does is hand the pointer to GXSetArray, and its 119 callers
    cast u32 data, file-local vertex structs and buffers to the fake `effVtxArray`).
  - Typed locals: qsort comparators (`const T* pa = a;`), one-store userdata setters, AC disp callbacks
    (`BattleWork* work = param;`), event-word names typed once at their evtGetValue.
- **Per-user unions.** MarioWork.motStruct (0x288) and PartyEntry.useStruct (0x170) are `void*` that each motion or
  partner casts to its own struct at every access, and a typed local can't replace the casts: the original reloads the
  pointer after every store and call (tried in mot_plane, party_bomhei, party_cloud: DIFF). Each became an anonymous
  union of the `void*` and one pointer per user, as EventEntry already does. The members are named after the struct they
  point to (`jabaraWork`, `motionPlaneData`, `motRoll`, `motionShipData`, `bomheiWork`, `partyCloud`, `partyYoshiUse`):
  no meaning beyond the type is claimed, but they are new member names; Isaac may prefer another spelling.
- **Refused or reverted by the orchestrator.** Data initialisers stay out of scope even when the bytes were verified:
  the `STRING((void*)"...")` / `STRING((char*)"...")` cast removals in evt_sub, battle_seq_end, sac_suki and sac_zubastar
  and rsh_00's npc_data were reverted (uninitialised .bss/.sbss retypes were kept: same size and section, verified).
  tou/tou2's `effStarStoneData** ud = (effStarStoneData**)&handle->userdata` (a new `void**` -> `T**` pun replacing
  eleven well-defined casts) was reverted. Three filemgr TPL relocations rerouted through the file's FILE_RELOC/
  FILE_UNRELOC pun macros were reverted (a pun moved into a macro the index counts once). bom_01's new member name `rot`
  became `field_0x18`.
- **Lessons.**
  - `const` on a pointee changes code in this compiler: mario_pouch's `*(const u16*)s` instead of `*(u16*)s` moved
    instructions. Test every const change.
  - `-char unsigned`: the `(s8*)` / `*(s8*)` views of char buffers (fontmgr, fontlenpitch, menu help text) are signed
    reads and stay; `const char*` there changes the object.
  - Effect `userdata` written more than once per function is the largest family left (~500 sites tree-wide): MWCC has no
    type-based alias analysis, so the original reloads `effect->userdata` after every store and call, and a typed local
    keeps it in a register (DIFF in every attempt). Only a per-effect union in EffectEntry would remove them.
  - Typed cursor locals sometimes match only at one declaration position (eff_nozle, eff_funemizu).
- **Left, by family** (4,047 code sites remain; approximate, families overlap): event-ABI int<->pointer conversions (`(T*)evtGetValue`, lwData/userdata
  words, `(s32)&x` to evtSetValue) ~800; sound ids passed through a name parameter (`psndSFXOn((const char*)0x2002A)`)
  ~450; colour/Vec/table puns on data objects declared as u32/f32 arrays (`*(GXColor*)&dat_*` ~330, `vec3_*`, fighterDt,
  sintbl) ~500, out of scope with data definitions; effect userdata reloads ~500; float bit tests (the MSL fpclassify
  shape copied per file) ~100; MUST_MATCH arms ~200; Mtx rows read as Vec ~50; variable-length file-format parsing
  (fontmgr, mapdrv, hitdrv, filemgr TPL, cardmgr) ~40; ptr_to_int in data initialisers 2,568 (hex scripts and raw
  tables).
- **Leads.** tou.c's `rank_wp`/`dwp` as RankWork*/DispWork* (~110 casts; one load pair swaps in
  evt_tou_print_ranking; tou2.c already declares them typed); end_unknown's `credits_work_ptr` as CreditsWork* (~30
  casts, an initialised data definition: matched in a test); BattleActions.field_0x2BC as a union of the per-AC work
  structs (51 casts; the structs must move into the ac headers first); EventEntry.field_84 used as u32, s32 and f32;
  `(Vec*)&data->x` into initializeVec inlines (~22, needs Vec members in shared effect headers and new names).
- Verified before each commit: `ninja` recreates build/G8MJ01/ok; tools/ci.py PASS with its tools/tests step stubbed (the
  same 9 environment failures as the chapters above; the data step needs pyelftools once a header used by a data table
  changes, so it was run under `uv run --with pyelftools`, after which configure.py must be rerun with the normal Python:
  it writes the uv interpreter's path into build.ninja); fncast_lint and local_prototypes pass. Worker reports with
  their `Model:` lines are in build/retype/G1-G8/REPORT.md (untracked).

### Event scripts: the last raw-hex scripts as EVT macros (2026-10-22, Claude Opus 5.5, n9-q-hexevt)

The standing rule says scripts are EVT macros, never hex; 39 files still held scripts as `s32`/`u32` arrays of opcode
words (4,463 hits for `grep -rE '0x000[0-9A-F]005[BCDE]' src rels`), and five more spelled small scripts as
`EVT_CMD(...)`/`OPCODE_*` words. All of them are `EVT_BEGIN` scripts now: **371 scripts in 39 files**, every one with
its symbol name, `static`, section declspec and position unchanged. Build byte-identical after every commit.

- **Method.** tools/ttydasm decodes from the linked binary, so it prints ELF names and string contents: it would have
  turned the named string objects (unit_purple_teresa's padded `str_*` arrays, evt_shop's `str_8041BC10`, the pool
  zero `zero_80420520` some scripts pass as `""`) into new literals and moved data, and it resolves same-named statics
  (`nice_event`, `evt_shake`, `main_evt`, `quake_evt`) to whichever copy it finds first. So the conversion read the
  C source instead (a one-off script, not kept): split each initialiser into words, decode them with evtmgr.h's
  opcode numbers and evt_cmd.h's macro table, and print each argument the way ttydasm does (LW/LF/GW/GSW/GSWF...,
  `FLOAT(x)` as the shortest decimal that encodes to the same word, `EVTDAT_POINTER_MAX`, decimal otherwise, flat
  `CASE_*`, no `END` line), but keep the source's own expression for every pointer argument. ttydasm on the built
  ELF was the cross-check: of the 396 scripts in the changed DOL files (older ones included), 345 print identically;
  the rest differ by those named strings, by ttydasm picking another file's same-named static, by literals the source
  spells with escapes (padding NULs; win_main's octal Shift-JIS for ゆうれい, kept as written), or are older scripts
  in the same files (`STRING((char*)"...")` casts, evt_bero's unindented pair).
- **Pointer arguments.** `(u32)&f`/`(s32)f` became `f` as a CALL's first argument and `PTR(f)` elsewhere (the macros
  cast only CALL's first argument, CALL_CHILD and SPAWN_EVENT); `&` was dropped only for arrays and functions, kept
  for scalars (`PTR(&BeroEX)`). String literals became `STRING("...")`, padding NULs kept inside the literal where the
  source had them (evt_memcard). A pointer to a `const` object keeps the source's `(char*)`/`(void*)` cast:
  MWCC refuses `(s32)` on a const pointer in an initializer ("illegal implicit conversion from 'const char *' to
  'char *'"), so `PTR((char*)str_A_1)`, `PTR((void*)&zero_80420520)` (83 sites).
- **Irregular scripts, written by hand from the decoder's output.** `npc_event.c`'s `met_init_event_common` has no
  END_SCRIPT in the original (a script that runs it goes on into `met_set_anim_wait_event`, the next object): an
  `EVT_BEGIN` closed with `};` and a comment. aji_02/aji_05's `hikaru_yuka_init` has an END_EVENT word after its
  END_SCRIPT: the END_SCRIPT is spelled `OPCODE_END_SCRIPT,` (as `EVT_END()` spells it) followed by `RETURN` and `};`
  (`EVT_CMD(0, OPCODE_END_SCRIPT),` there trips tools/local_prototypes.py, which reads the tag macro on the line above
  as a return type). bom_02's `mario_jump` runs into `lbl_3_data_76CC`: the symbol map starts that object at the third
  argument of the script's last CALL, so both objects stay and the CALL is written across them
  (`EVT_CMD(4, OPCODE_USER_FUNC), (s32)evt_npc_flag_onoff, 0,` / `STRING("ホワイト"), 32,`). Merging them would drop a
  symbols.txt symbol.
- **Not scripts, left as they are.** The 21 grep hits left in 6 files are data whose words happen to look like opcode
  words: battle_audience_kind_data's audience animation bytecode, eff_holography_n64's display list, muj_00's
  `kaizoku_vs_dai3_*pos` coordinate tables, las_29's `hikari_data`, gor_02's `stg5_list` item list and a
  party_chuchu search table. A sweep of every `s32`/`u32` array in src/ and rels/ found no other array, or tail or
  interior run of one, that decodes as a script. No new evt_cmd.h macro was needed (the missing ones, DEBUG_BP,
  DEBUG_MSG_CLEAR, DEBUG_NAME and DEBUG_REM, are never used).
- Stale `// TODO: type` markers above the converted scripts went with them; comments calling a script a "table"
  now say script. evt_bero's older `default_entevt`/`default_outevt` (unindented, inside `clang-format off`) are
  untouched.
- **Index** (`tools/cleanup_index.py`, 176dceaf -> this branch): 21.493 -> 19.987 per kLOC (14888 -> 13849 weighted);
  ptr_to_int 2694 -> 1572; prim_ptr_cast 1901 -> 1984 (the const-object casts above, which the index counted once as
  ptr_to_int behind the outer `(u32)` before). unit_purple_teresa 243 -> 57 weighted (2430 -> 121 per kLOC; the
  rest is its raw tables), npc_event 126 -> 16, evt_shop 198 -> 80, battle_stage_object 330 -> 262.
- Verified before each commit: `ninja` recreates build/G8MJ01/ok; tools/ci.py PASS with its tools/tests step stubbed
  (the same 9 environment failures as the chapters above), run under `uv run --with pyelftools`, then configure.py
  rerun with the normal Python; fncast_lint and local_prototypes pass. All 39 changed objects also built with
  `configure.py --non-matching` (ci's natural step looks REL files up by their stem, so this was done by hand too).

## The next twelve worst files by the cleanup index, deep-cleaned, fifth round (2026-10-22, Claude Opus 5.5 orchestrating five Claude Opus 5.5 workers, n9-q-worst5)

Task (Isaac): take evt_mobj, battle_stage_object, pmario_sound, sound, win_root, winmgr, eff_starstone, tou, mot_hammer,
evt_npc, win_item and filemgr (`cleanup_index.py --top 20` after #1124, under the Melee-family definition of #1119)
each as a whole, as #1110 and #1114 did: raw offsets, MUST_MATCH, pragmas, gotos, function-pointer casts and puns
typed away; guard existing unguarded hacks; no field renames without hard evidence, no data layout, no new pragmas,
gotos, PAD_STACK or new MUST_MATCH regions except guards. All twelve had been through #1110 or #1114 already, so most
of what is left is ABI or data layout (below).

- **Method.** Untracked harness in build/w5/: try.sh (one unit with its ninja command into a temp dir, obj_equiv
  against objects saved from d6d39c6f; `NM=1`, `OVL=` include overlay, `FN=` asm diff), regions.py and pragmas.py
  (each MUST_MATCH region's `#else` / each pragma pair alone), sites.py (the index's sites by line), users.py and
  hdrcheck.sh (every user of a header, matching and natural). Workers A (evt_mobj, evt_npc), B (battle_stage_object,
  eff_starstone, filemgr), C (pmario_sound, sound), D (win_root, winmgr, win_item) and E (tou, mot_hammer) edited only
  their own .c files, headers through a private overlay; their reports (with `Model:` lines) are in
  build/w5/w/*/REPORT.md (the harness refused report files from subagents, so the orchestrator saved them from their
  final messages). verify_commit.sh exports a commit's tree into build/w5/tree and compiles the 351 affected sources
  (the twelve files and every user of the four changed headers) matching and natural, obj_equiv against the saved
  objects: 0 problems at each of the five commits. The last commit's tree: `build/G8MJ01/ok` recreated, all 351
  objects built under `configure.py --non-matching`, tools/ci.py PASS (tools/tests stubbed for the same 9
  test_wave_launch environment errors, pyelftools through `uv run`), fncast_lint and local_prototypes clean.
- **Counts** (`tools/cleanup_index.py`, d6d39c6f -> this branch): index **19.986 -> 19.953** per kLOC (13848 -> 13828
  weighted; code lines 692,894 -> 693,020). raw_offset 215 -> 196, goto 106 -> 102, prim_ptr_cast 1983 -> 1966,
  struct_ptr_cast 1464 -> 1461, deref_cast 526 -> 521, opt_pragma 332 -> 331; must_match 692 -> 732 (guards on hacks
  that were unguarded), field_0x 4608 -> 4625 (named-field `#else` arms, the new WindowWork.field_0x1050). The twelve
  files 2383 -> 2360 weighted. About 70 `(u32)` value casts removed from pmario_sound are not counted by the index.
  Per file (weighted, per kLOC; categories that moved):
  - evt_mobj 285 -> 284 (57.402 -> 56.789): goto 4 -> 0, must_match 10 -> 17
  - battle_stage_object 262 -> 224 (94.756 -> 80.373): raw_offset 16 -> 2, prim_ptr_cast 50 -> 32, struct_ptr_cast
    11 -> 10, must_match 2 -> 4, field_0x 116 -> 123
  - pmario_sound 251 -> 243 (35.847 -> 34.685): raw_offset 12 -> 7, deref_cast 13 -> 12, prim_ptr_cast 16 -> 15,
    struct_ptr_cast 2 -> 1, must_match 6 -> 10, field_0x 157 -> 159
  - sound 233 -> 246 (114.048 -> 118.783): must_match 3 -> 15, raw_offset 2 -> 3, prim_ptr_cast 8 -> 7
  - win_root 201 -> 209 (76.368 -> 79.741): field_0x 184 -> 192
  - winmgr 198 -> 192 (88.630 -> 86.292): opt_pragma 3 -> 2, deref_cast 6 -> 3, must_match 10 -> 11
  - eff_starstone 194 -> 196 (113.450 -> 113.821): must_match 0 -> 1, prim_ptr_cast 2 -> 3
  - tou 179 -> 184 (48.774 -> 50.041): struct_ptr_cast 144 -> 143, deref_cast 6 -> 7, must_match 2 -> 6
  - mot_hammer 173 -> 173 (196.368 -> 192.650): must_match 7 -> 10, field_0x 100 -> 97
  - evt_npc 150 -> 150 (66.225 -> 66.050): prim_ptr_cast 130 -> 129, struct_ptr_cast 9 -> 8, must_match 0 -> 2
  - win_item 132 -> 135 (129.032 -> 130.561): raw_offset 2 -> 1, prim_ptr_cast 32 -> 33, must_match 1 -> 5
  - filemgr 125 -> 124 (148.633 -> 147.444): deref_cast 2 -> 0, prim_ptr_cast 6 -> 8, struct_ptr_cast 5 -> 6
- **Typing.**
  - pmario_sound.h: the ids and handles retail compares unsigned at every site (PaperSoundBGM.unk0/streamId,
    PaperSoundEffect.unk0/unk18, PaperSoundEnv.field_0x14; evt_snd and seq_* already compare streamId != 0xFFFFFFFF)
    are u32, as are the id/fade parameters of psndSFXOff, psndSFX_pos, psndSFX_dist, psndBGMOn_f_d, psndBGMOff_f_d,
    psndENVOn_f_d/psndENVOff_f_d and __psndSFXOn's a6 (psndSFX_vol/pit keep s32: party_motion's documented local
    prototypes). BGMListEntry.params is an anonymous union, `u32 params` first so the psbgmlist initialisers are
    untouched, with a `field_0xC..field_0xF` byte view (volume, pan, reverb, starts-in-scope) for the six
    `((u8*)&entry->params)[N]` reads. sound.h: SoundSSPlayCh/Ex take `const char* name` and `BOOL useAram`.
    182 users EQUIV.
  - battle_stage_object.h: _rate_mix/_rate_mix_3 take s8* (retail reads the chance bytes with extsb; the time and
    strength fields are s8), _Nozzle_Change_Check takes u8*. The guarded arm's fourteen `(s8*)(s + N)` casts go with
    it; its byte offsets from `(s8*)dst + 0x80` remain (the index no longer sees them once the casts are gone).
  - win/win_main.h: win_root read the tattle list at 0x1050 as `&work->field_40[0x1010]`, 0xF2C bytes past the end of
    a 0xE4-byte array (the union at 0x40 existed for that). WindowWork has `u8 field_0x1050[0x1B0]` there (LogWin's
    `chars`; `LOGWIN(work)->chars` itself is 32 lines off, CSE of the address) and the union goes. win_item's four
    floats at 0x224 are `field_0x224[2]`, `field_0x22C[2]` per tab (item_disp indexes them by itemTab).
  - tou.c: the uninitialised .bss rank_work, disp_work, champion_belt_onoff and gans_wp take their types (RankWork,
    DispWork, s32, GansWork*; same size and section; gans_wp has no other user).
  - evt_mobj's eventCode/unk1D8 locals are s32* at their evtGetValue (41 store casts go; count-neutral, the cast is
    at the ABI site now). filemgr's FILE_RELOC/FILE_UNRELOC assign instead of punning through `*(u32*)&`.
- **Hand-expanded copies and invented inlines that went.** winmgr calls winMgrSetSize, winMgrOpen, winMgrHelpInit,
  winMgrClose and winMgrDelete where it had their bodies written out (with the invented get10); pmario_sound's
  psndMapChange calls psndSFXAllOff (the invented psndMapChangeMarkAll and the fade block were its inlined body);
  evt_npc's subf32 (a plain subtraction in the same place is identical), win_item's winItemMailLine, win_root's
  mail_vec. filemgr's TPL pack walk is SPM's PackTexPalette (SPM symbols.txt 0x8019E6C0, between fileInit and
  fileGarbageDataAdrClear; disassembled, the same walk), so the inline takes that name; its unpack twin keeps
  fileUnpackTpl, since SPM's UnpackTexPalette is the SDK texPalette.c global here.
- **Other natural shapes that matched.** evt_mobj mobj_lv_blk_evt's four `goto run_event` as nested if/else-if.
  winmgr: one defer_codegen region (the deferred group is now written in reverse retail order), the volatile
  firstItem reads (the dead compare stays guarded), steering `it`/`items`/`table`/`lottery` temps. tou
  evt_tou_print_ranking: `w->scroll += t * k` replaces the steering re-read through dwp (the load-pair swap #1123
  recorded for a typed dwp goes with it). pmario_sound: psndSFXMain's view cast (compound assignment), the branchless
  psndSFXChk/BGMChk test as `if (x == -1) return -1;`. sound SoundInit's force-register copies. Constant locals in
  evt_mobj (scale, ten), tou disp_proc, win_root (two); win_root winHakoGX's `it[0xF0]` temps. Raw GSWF words in tou
  as GSWF().
- **Guarded** (unguarded before; plain forms re-tested first, natural `#else` arms): tou's monitorColor and the doInsert
  copy of insert (under MUST_MATCH `#define insert doInsert`, winmgr's precedent; needed only because the guarded
  defer_codegen keeps insert from inlining); mot_hammer's clampHammerType (was a MUST_MATCH-only invented inline, now
  a guarded local), pose-time clamp temp, effUltraHammerEntry argument temps, hitEstimate's `five`; evt_mobj's
  mobjSwitchAnimIndex and five `(s32)hit != 0` int tests (cmpwi; the pointer test is cmplwi); evt_npc's u32* flags pun
  and `zero`; battle_stage_object's _Nozzle_Change_Check byte walk; eff_starstone's starStoneDLLoop; winmgr's
  winMgrDispWaku0..5; win_item's itemSel no-op cast and skipList byte walk; pmario_sound's PaperSoundEffectU16 view
  and doubled flag tests; sound's dead store, six redundant NULL stores, sdd_eff/sdd_dvd and SoundSSGetStream.
- **Reverted by the orchestrator.** Invented local names (win_item keyItem/timer/rowOffset, evt_mobj grpName; the
  register names stay). battle_stage_object's `#pragma pool_data on` removal (EQUIV, .line only): pool_data is data
  layout, out of scope (lead below).
- **Left, with what was tried** (details in the reports):
  - ABI, by rule: `(const char*)evtGetValue`, `psndSFXOn*((const char*)0xNN)` (the s32 prototype is Isaac's open
    call), userdata words, `(s32)&x` to evtSetValue. These are most of evt_mobj's and evt_npc's prim_ptr_cast.
  - Data layout: eff_starstone's 173 ptr_to_int and battle_stage_object's 47 are initialised u32 tables; tou's
    fighterDt/ruleDt casts (initialised u32 tables; tou2 has typed versions); the `*(GXColor*)&dat_*` puns forced by
    u32 definitions; winmgr's templates; `*(s16*)&entry->pitch`.
  - tou rank_wp/dwp (~100 struct casts): initialised .data pointers. `static RankWork* rank_wp = &rank_work;
    static DispWork* dwp = &disp_work;` (tou2's form) is EQUIV with every cast gone (-104 weighted), not applied:
    patch build/w5/w/E/tou_rankwp.patch. TOU_F32 and its defer_codegen (each use alone 2-116 lines).
  - evt_mobj: 5 opt_propagation pairs (floatswitch_blue 3000 random shapes, koopa_badgeblk all 5040 placements; best
    = no pragma, 8 and 10 lines), the jumpstand regions (670 shapes, best 26 = the `#else`), fire_func's userdata
    reload (13). evt_npc: homing_target's userdata[1] (retail stores and reloads 0x7C), set_color's propagation (81
    shapes, best 2).
  - battle_stage_object: _combine_nozzle_weapon's `#else` (194 lines; only a sub-struct at 0x80 would give the shared
    base). filemgr's CLUT region (every natural shape 10 lines, r3/r4). eff_starstone's userdata casts (4-30 lines:
    reloads), the DL loop written in the caller (14-212).
  - winmgr: the second defer_codegen pair (fully deferred, float_24/6/neg144 land at .sdata2+0xc0, not +0xe8), the
    by-value macro block (300-894), the arrow Vec volatile reloads (599). win_item's walks (16; typed s16* walk 4).
    win_root's three regions (36, 1, 59); its repeated drawing inlines (wazaQuad, winSortPanel, winBookQuad,
    winRootTab*, WIN_KIRINUKI_*) stay plain: a natural form would be hundreds of expanded lines.
  - pmario_sound: the gotos (5), psndENVOn_f_d's byte-offset stores (76-77), psndENVMain's volatile reloads (147 each,
    588 together; each block is psndSFX_vol's body), calc3D (40), psndSFXChk's propagation (best 10), `((u8*)&res)[N]`
    (a local union is EQUIV but needs member names). sound: SoundEfxSetPan's folded index (12), `goto unlock` (2),
    SoundSSMain's macros (154).
  - mot_hammer: 0x2A0/0x2A4/0x2B8 raw (no member of those types; the field_0xNN names are taken), the four volatile
    camera blocks (32-168), the three pragma pairs (6, 11, 205 lines).
- **For a reviewer.** The u32 retypes in pmario_sound.h rest on retail's unsigned compares. The BGMListEntry union
  sits over a data table's member but leaves its initialisers as they were. winmgr's function order changed (the
  deferred group in reverse retail order). tou's `#define insert doInsert`. win_root's new field_0x1050 could be named
  `chars` from LogWin (the index would drop 8); left as a placeholder. sound's six NULL-store guards are noisy; they are
  the dead-store family. filemgr's PackTexPalette is the one inline renamed (outside evidence: SPM's symbol).
- **Leads.** tou_rankwp.patch (data-definition type only, same bytes and relocation). Per-file `-char signed` for
  evt_mobj is EQUIV and would remove its 5 `*(s8*)childName` puns and 18 `(s8)` name casts (every char read in its asm
  is extsb; a compiler-flag decision). `#pragma pool_data on` compiled out to the same object in battle_stage_object
  and may be a no-op in party_chuchu, party_yoshi, party_move, party_hit, party_motion, mapdrv and system.c. mot_hammer's
  0x2B8 is a sound handle (psndSFXOn_3D result, psndSFXOff, -1 when none), the evidence that named mot_hip's
  motionSfxId0/1: an s32 member would clear 4 raw offsets here and in mot_stay/mot_jabara. An EffectEntry userdata union
  would clear ~500 reload-forced casts tree-wide (naming decision). evt_mobj's `"o_"`/`"i_"` name templates as local
  char arrays and dat_8041a130/kpa_pole_score_color as const GXColor are EQUIV (data). psndENVMain's reload blocks are
  inlined psndSFX_vol calls; psndBGMMain's `psbgm[i & 0xF]` block looks like another inlined function.
## Fresh-eyes MUST_MATCH pass over rels/, src/party, src/mario, src/unit (2026-10-22, Claude Opus 5.5 orchestrating nine Claude Opus 5.5 workers, n9-q-mm3)

Task (Isaac): retry every MUST_MATCH block in rels/ (144 lines), src/party (72), src/mario (31) and src/unit (7) after
#1096/#1099/#1104/#1111, trying what the ledger doesn't record: declaration order and scope, honest local types,
splitting/merging locals, loop shape, early returns, operand order, assignment in the condition, existing helpers, a
matched sibling's shape. Files the parallel worst5 branch owns (evt_mobj, battle_stage_object, pmario_sound, sound,
win_root, winmgr, eff_starstone, tou.c, mot_hammer, evt_npc, win_item, filemgr) were not touched; data layout was out
of bounds.

- **Method.** Untracked harness in build/mmfresh/ (try.py: compile one file with its ninja flags into a temp dir,
  compare with the pre-branch object through tools/obj_equiv.py, print per-function normalised asm diffs; `--nat`
  compiles without MUST_MATCH; `--overlay DIR` shadows a header for a what-if). Round 1: six workers over disjoint file
  groups (gA party_motion/move/kuribo/cloud/hit; gB party_yoshi/bomhei/chuchu/nokonoko, unit_party_*; gC src/mario;
  gD eki/rsh/hom/jon; gE yuu/gor/pik/tou_02/tou2; gF the other RELs). Round 2: three workers on the near-misses, told to
  explain each difference from the retail asm and rewrite from it (r2a party/mario, r2b RELs, r2c the sfx-string
  family). Reports, with every shape tried and its diff count, in build/mmfresh/{gA..gF,r2a,r2b,r2c}.txt. Every commit:
  each touched object EQUIV (and every object including a changed header), `build/G8MJ01/ok` recreated, natural
  compile clean, fncast_lint and local_prototypes clean, tools/ci.py PASS with its tools/tests step stubbed (the same 9
  environment errors as the chapters above). At the end the whole tree's objects were obj_equiv'd against the
  pre-branch snapshot (`--tree`): all equivalent.
- **Counts** (`grep -c MUST_MATCH`, after the review's re-guards, rebased on 051713c4 (#1125)): rels **144 -> 134**,
  src/party **72 -> 63**, src/mario **31 -> 29**, src/unit **7 -> 6**. Per file: party_motion 27 -> 20, party_yoshi
  18 -> 16, mario_sbr 3 -> 1, unit_party_yoshi 2 -> 1, eki_00 9 -> 8, eki_03 4 -> 2, hom_00 4 -> 2, rsh_00 4 -> 3,
  rsh_05 9 -> 8, yuu_00 6 -> 4, yuu_01 6 -> 4, yuu_02 6 -> 5, gor_03 10 -> 9, gor evt_yuuranking 3 -> 2, yuu
  evt_yuuranking 2 -> 1, tik_05 1 -> 0, hei_10 3 -> 4 and rsh_06 8 -> 9 (re-guarded double copies). Index
  (tools/cleanup_index.py, 051713c4 -> this branch) **19.962 -> 19.920** per kLOC (13835 -> 13801 weighted):
  must_match 736 -> 716, opt_pragma 331 -> 327, fn_ptr_cast 48 -> 41, raw_offset 196 -> 194; deref_cast 521 -> 527
  and address_name 1076 -> 1078 (party_yoshi searchGround, below).
- **Forms that matched.**
  - **Declarations, not call sites.** Footmark ids are `s32` (partyGetFootmarkId and the six footmark accessors in
    party_move.h/.c, which took and returned u32 while the partyGetTarget* functions the same id goes to take s32):
    with one type MWCC stops giving the converted id its own saved register, so party_motion's homing_walk/homing_fly
    lose the signed `sid` copy, the three `f32* pz = &v.z` temps, the `(f32*)(u32)(s64)(s32)&v.z` round trip and the
    unguarded `opt_common_subs off` and `opt_loop_invariants off` pairs (r2a; homing_fly's lerps are written
    `position.y += 0.05f * (...)`). envGlareFilter takes `s16` coordinates: retail's definition never re-extends them,
    and written straight on its parameters (`x0 -= x0 & 1;`, clamps, stores; no temporaries) it matches, so eki_03's
    function-pointer cast and hom_00's volatile pointer at the test_glare calls go (r2b). Lesson: when every caller
    of a function needs a hack at the call, test the callee's prototype and definition with `--overlay` first.
  - **By-value compound literals.** Both evt_yuuranking copies pass their colours as `(GXColor){...}` straight into
    the call; MWCC builds those temps in source order, which is retail's contiguous block, so the WAKU_COL/ICON_COL/
    ICON_NUM fn-pointer-cast macros and 20 hand-ordered GXColor locals go, and the file's FontDrawColor local prototype
    takes the colour by value as fontmgr.c defines it (gE). mri.c's hakari_color doesn't follow: every by-value form
    is a stack-slot order difference (10 lines).
  - **An existing function instead of a hand copy.** party_yoshi searchGround calls unk_800b474c instead of a
    written-out body with a `(char*)vec + 0x30` string and a raw 0x2B4 store. It matches only when the zero-Vec copies
    above it take yoshi_use's guarded volatile form instead of the `const Vec* vec` base pointer, so a hand-expanded
    body (a refused structural fake) and two raw offsets are traded for the file's existing guarded idiom; that trade is
    the index's +6 deref_cast / +2 address_name.
  - **Invented inlines written out, consistently.** eki_00/rsh_05's moaPlayHit and moaSoundVec (no name evidence;
    moaSoundVec was unguarded) are written as rsh_06 already has them, `{ Vec t = template; t.x/y/z = ...; v = t; }`
    (gD); hei_10's sound position takes the same form, so its guard covers only the sfx call (r2c). yuu_00/01/02
    fall_rtn write every coin range as `max -= min; ... 100.0f * max`, and the monte_rand inline (no evidence) goes:
    retail keeps both subtractions in max's register (r2b).
  - **Variables that say what they hold.** gor_03 kuzi_bomb_bomb_eff switches on `state` and writes `next`
    (initialised from state): the t0/t1 copy chain goes. r2c traced why: the pre-allocation scheduler had hoisted the
    npcNameToPtr string above the copy of the second evtGetValue result; the extra dependency orders the copy first.
    mario_sbr marioGetStick is `invert = (flags & 0x02000000) || kpaGetStageViewType() == 0; y2 = invert ? 0 :
    -p->wStickDir2;` (switch temp and the `(s8)` on an s8 field go). jump2 picks its motion with an if/else storing
    0/1; unit_party_yoshi computes `halfHeight = 0.5f * BtlUnit_GetHeight(unit)` in one expression (the `t = t` goes).
  - tik_05's sqrtf returns `x * (last Newton step)` instead of reassigning x.
  - **Re-guarded after review.** mario.c marioCheckMenuDisable's `switch (!marioChkKey())` (a two-way switch on a 0/1
    test that keeps the materialised compare; retail mario_keyoff_wait has no jump table, and the REL copies chose the
    switch because if/else if-converts), rsh_00's store through `wp` right after `entry = &wp->entries[id]`, the `t -> v`
    double copy in eki_00, rsh_05, hei_10 and rsh_06, and party_motion homing_walk's `(radius = 40.0f)` in both places:
    each is now the MUST_MATCH arm, with the plain form as `#else`. They still replace worse tricks (the comma
    assignment, the one-iteration loop).
- **For a reviewer.** gor_03's `next`, unit_party_yoshi's `halfHeight` and party_motion's function-scope `mot` are
  plain C but were picked from EQUIV candidates; each is listed with its alternatives in the reports.
- **EQUIV but not applied (hacks by the rules).** mario.c yuu_ block as `if (notCtrl2 || (notKey = !marioChkKey()))`
  (notKey never read); party_cloud RELOAD_MARIO as a shadowing `MarioWork* mario = party->playerPtr;` block; jon
  raster_disp `q *= 32; row = q;`; muj_20 fune_eff's constant in its own local (`ofs = 85.0f`); barriern zanzou with
  the sub taking a `const BarriernZanzouScale*` built from a local copy (v1/v2 and the shared headers; diffs in
  build/mmfresh); gor_03 minigameselect_msg_set's terminator copy with dead `pos++`/`src++`; mariost RUMBLE_MS with a
  local `OSDiffTick` (the SDK macro, missing from libs/dolsdk2004's os.h).
- **Left, with what was tried** (shape counts and closest results per region in the reports):
  - party_motion: `(s8)currentMemberId` (4 shapes, 2-4 lines; mario_party's twin, 5 shapes), jump's targetDist
    self-stores (18 shapes, 19 lines), the redundant `hit == NULL` re-tests (an inlined NULL-safe attribute test with no
    name). party_move partyForceWalk's hand-inlined partyChgSpeedPose (53 bodies, 28 lines; the parameter's register
    comes from the inliner). party_hit partySearchHead (volatile + pragma: 329 lines without both).
  - party_yoshi: the remaining `vec` base pointer and volatile copies (retail copies dispOfs then dispOfs2 without
    interleaving, which MWCC does only for its own compound-literal templates: the data lead below), yoshiRideHeight/
    yoshiSearchHead (no names; 20-64 lines written out, 4 only by reusing the f30 local). party_bomhei `frames = 10.5f`
    (12 spellings all fold to `li 10`), its three do/while(0) blocks (retail keeps a dead `hit == NULL` compare; no
    named helper in the tree). party_chuchu ChuchuSearchState (~50 shapes, best 8: retail copies `found` into `i` with
    `mr`). party_nokonoko fadd32 (25 shapes, 2 lines: MWCC puts a folded constant first; mario_hit's `ofs = 5.0f`, 19
    shapes, 3 lines, same cause), punned zero stores, double store. unit_party_chuchurina's getters (36 shapes, 57-96
    lines: plain forms need seven saved registers, retail six).
  - mario: mario.c's yuu_ gotos (59 shapes, best 2: MWCC never merges two return blocks); mario_hit marioHitCheckVecAttr
    (written out, 68 lines of stack slots: retail gives each inline expansion its own area) and marioChkGndRollProbe
    (written out grows .rodata by 8); mario_pouch comparators (calling pouchHiraganaToKatakana: 40 lines, all 120
    local orders of the helper tried); mario_balloon's discarded marioGetPtr calls and uninitialised balloonType;
    mario_party's per-slot kill (no named function has that body).
  - RELs: rsh_00 `| dm` (96+10 shapes, 27-59 lines); the `q` row chains in eki_03/hom_00/jon (envdrv.c's matched yami
    water loop as a model: 14/14/18 lines); rsh_05's fn-pointer casts (retail inlines moamoa_pose_change everywhere
    except ext_moamoa_main); gor_03 slot_count_hit (26 shapes, 6 lines; any inline holding the loop matches, so the
    original had one) and its slot_stop_sub inlines; yuu_02/yuu_03 monte_fill/yuu4_init_work (11 shapes, 6 lines;
    retail starts the offset with `mr` of i); gor_irai irai_cancel_func (9 shapes, 8 lines); frankli `id = 0` (32
    natural); tou_03 screen_tev `zero` (9 shapes, 16 lines; battle_stage.c's twin has the same guard); barriern zanzou
    comma (9 new shapes, 3 lines); ss_check/ss_ch_check in mri_16/tik_00/win_04 (~38 shapes, 8-14 lines: retail loads
    the args before the int-to-float conversion); mri.c hakari_color (above); evt_lect_dou ppos (needs a member at 0x8
    in the shared evt_lect.h, no evidence for its type); bom_01's `? 1 : 0`, las_04/usu_00 self-assignments, las_05's
    dead sqrtf, evt_yuunpc/gor_03 redundant terms, pik_02 puns: by rule.
  - The sfx-string family (eki_00, rsh_05, rsh_06, hei_10, yuu_00/01/02, gor_03 fall_rtn, zako_group2, gon_03, bom_01,
    las_05, tou_02; 2-12 shapes each, including the kuzi-style split). r2c's finding: retail forms each name once, in
    the loop preheader, straight into a saved register (one `addi` plus a copy where two call sites share a literal).
    In test files only an *integer* conversion of the literal at the call, `(int)"X"`, gets that front-end
    loop-invariant motion (`opt_loop_invariants off` stops it); no pointer-typed spelling does, and a local set from an
    address and live across a call is always `addi r0; mr rN, r0`. That supports the mmrels2 chapter's
    `psndSFXOn(s32)` prototype explanation, still the only route (not applied; Isaac's call).
  - Data layout, skipped by rule: party_motion's FB/FV/PM_RODATA/.sbss dummy, party_move's dead zero Vecs, the padded
    strings (kuribo, bomhei, eki_00, rsh_00, rsh_06), party_hit/party_cloud/mario_hit stripped placeholders and pool
    helpers (party_yoshi, unit_party_yoshi), the declspec Vecs (aji_13, magnum_battender_mkII, rsh_06, usu_00, yuu_02),
    the const-template reloads (eki_00/rsh_05/rsh_06 pvec, eki_06 sw_entry, rsh_06 ext_moamoa_init, las_04, tou_02),
    rsh.c Zero256, tou2's AREA_TOU, pik_02's c50/half_sym, gor evt_yuuranking's asm body; mario_sbr's US-only
    marioSetBottomlessResetPosition (#ifndef, still called from evt_bero.c).
- **Leads.**
  - Header: `OSDiffTick` in libs/dolsdk2004/include/dolphin/os.h (protected path; it is in the metrowerks OSTime.h)
    would make mariost's RUMBLE_MS natural. A field at 0x8 in LectureButton (include/rel/common/evt_lect.h) for the
    ppos regions, if its type gets evidence.
  - The FontDrawColor by-value form probably carries over to the other `FontDrawColor(GXColor*)` local prototypes
    (tou2.c, gor_irai.c, dig_00, kpa, win_main, evt_window) and possibly battle_seq's `_rule_disp` Waku_col cast and
    sac_genki's icon colours; if all go, the local-prototype exception for FontDrawColor may not be needed.
  - src/driver/npcdrv.c:107's sqrtf still reassigns `x` before returning it; tik_05's `return x * (...)` form may fit.
  - Data: party_yoshi's three vec3_802ccc* zero Vecs as compound literals would remove its remaining copy hacks
    (siblings party_vivian, mot_dokan, mot_yoshi already write `(Vec){0.0f, 0.0f, 0.0f}`); party_nokonoko's float_30 and
    party_bomhei's 10.5f as non-const objects would remove fadd32 and `frames`.

## Fresh-eyes MUST_MATCH pass over src/effect, driver, battle, event, motion, window, manager (2026-10-22, Claude Opus 5.5 orchestrating eight Claude Opus 5.5 workers, n9-q-mm4)

Task (Isaac): retry every MUST_MATCH block in src/effect, driver, battle, event, motion, window and manager after
#1103/#1104 (whose verdicts were "compiler steering"), including the arms #1125 had just guarded, with the levers
those passes didn't try: declaration order and scope, honest local types, split/merged locals, loop shape, early
returns, operand order, assignment in the condition, an existing helper instead of a hand expansion, a matched
sibling's shape. A natural form that matches becomes the only version; otherwise one line of what was tried. rels/,
src/party, src/mario and src/unit were a parallel branch's (mm3) and not edited. Data layout (string padding,
`gap_`/`force_active`, section declspecs, data definitions, `.sbss2` templates) was out of bounds.

- **Method.** Untracked harness in build/mm4/ (adapted from build/w5 and build/mm3): try.sh (one unit with its
  ninja command, obj_equiv against objects saved from 051713c4; `NM=1`, `OVL=`, `FN=`), regions.py (each region's
  `#else` alone), pragmas.py, hdrcheck.sh. First a sweep of every region's `#else` alone across the 132 files (436
  regions; build/mm4/sweep/), then eight workers on disjoint file groups (effA: eff_kemuri + n64; effB: other
  effects; drvA: envdrv, mapdrv, mobjdrv, shadowdrv; drvB: other drivers; batA: battle/ac + sac; batB: battle/*.c;
  evt; mwm: motion + window + manager), each editing only its own .c files, headers through overlays. Reports with
  `Model:` lines and per-region attempts (shape -> asm lines differing): build/mm4/w/*/REPORT.txt. Every commit:
  `build/G8MJ01/ok` recreated, changed C files compile without -DMUST_MATCH, fncast_lint and local_prototypes clean,
  tools/ci.py PASS with the tools/tests step stubbed (the same 9 environment errors as the chapters above). The
  header changes were checked over every user (222 for mariost.h, rels/party/mario included), matching and natural.
- **Counts** (after review fixes, rebased on dc2b0f89 (#1126)). `grep -c MUST_MATCH` 512 -> 483: effect 139 -> 129,
  driver 101 -> 94, battle 122 -> 116, event 77 -> 73, motion 35 -> 35, window 21 -> 21, manager 17 -> 15. Per file:
  battle_ac 3 -> 1, battle_audience 3 -> 2, battle_stage_object 6 -> 5, sac_scissor 4 -> 2, mapdrv 14 -> 12, mobjdrv
  11 -> 6, eff_break 3 -> 2, eff_energy 2 -> 0, eff_naniga 4 -> 3, eff_stageclear 1 -> 0, eff_teresa 2 -> 0,
  eff_balloon_n64 4 -> 2, eff_kemuri12_n64 1 -> 0, evt_badgeshop 1 -> 0, evt_mario 7 -> 6, evt_npc 2 -> 0, cardmgr
  3 -> 2, winmgr 12 -> 11; the other files unchanged. Index (tools/cleanup_index.py) **19.920 -> 19.852** per kLOC
  (13801 -> 13751 weighted): must_match 716 -> 693, deref_cast 527 -> 520, raw_offset 194 -> 190, prim_ptr_cast
  1966 -> 1962, struct_ptr_cast 1460 -> 1459.
- **Natural now.**
  - `#else` arms that had become identical since the last pass (the sweep): evt_mario's volatile party-flag updates,
    battle_audience's volatile startPos store, eff_stageclear's volatile double read, eff_teresa's READ_RODATA macro.
  - Honest field types, each checked over every user: `GlobalWork.language` is `u32` (retail compares it unsigned at
    every site; 22 `(u32)gp->language` casts in effect, driver, battle, event and sequence go, and effdrv's
    `(u32)wp->language`). eff_break's guarded redundant language test is the two per-language letter counts,
    `sizeof(tex_jp)/sizeof(tex_jp[0]) + 1` then the US table's when language != 0 (both 7; element 0 is the head).
    `BadgeShopWork.field_0x10C` is `int` (the intlead lever on a field; u32 breaks badgeShop_ThrowCheck), so
    badgeShop_bargainGeneration's no-op `(s32)` cast goes. eff_naniga: `ItemData.icon_id` was already `u16`, so the
    ICON_ID pun goes.
  - New lever, tentative definitions: mobjdrv declared cubeDJ/headDJ/frontDJ/front2DJ `extern` although it defines
    them; declared `u32 cubeDJ[26];` (same file, defined below), MWCC reaches all four from one section-base register
    as retail does, and the guarded `negone_one_374 + 0x16a4..` byte base with its four memcpy regions goes. Tried on
    every other file in scope with extern declarations of its own objects (27 files): no further region moved.
  - Compound literals at the call: mapGrpSetFlushColor `mapGrpSetColor(name, (GXColor){r, g, b, a})` (the guarded
    inline goes; with it still defined `.sbss2` keeps a second template word), and mapObjSetFlushColor's unguarded
    copy of the same shape; eff_energy's mat colour (its guarded GXSetTevColor forwarder and unguarded invented
    effEnergySetMatColor inline go).
  - Expression shapes: eff_balloon_n64 keeps the raw sin/cos in `wave` and writes `0.1f * wave + 2.0f` at its use
    (the embedded `wave =` assignment goes; `sinOffset`, the sin in its own statement for the pool order, is
    guarded); eff_kemuri12_n64's
    ternary is an if/else storing 1/0 with `isLast` declared first; evt_npc_glide_position tests `if (dx || dz)`
    (the named zero goes); evt_npc_flag_onoff's mask is `int` (the u32 complement temp goes); cardMain's `(s32)(s64)`
    round trip is `chan = CARDProbe(0) ? 0 : -1` (chan: the SDK's parameter name for the card slot);
    winMgrSelectEntry calls badgeShop_get in the condition (no `v`, no `(s16)`).
  - battle_ac: N_normal_guard_frames loses its `volatile` (inside the guarded `.rodata` definition arm; same bytes and
    section), and the own-type pointer read becomes a plain element read.
  - battle_stage_object _Nozzle_Change_Check: the guarded byte walk (typeOffset, base, slot[0x73]) is member accesses;
    what stays guarded is one early `u8* dir = &w->field_0x1C4[index]` (the worker offered it as natural; the
    orchestrator kept it guarded as a steering pointer, with the plain subscript under `#else`).
  - sac_scissor: scissor_abs is `fabsf`, defined in the file exactly as the SDK's fake_tgmath.h defines it (math_ext.h
    only declares it; including fake_tgmath.h pulls `#pragma cplusplus on` and changes the object).
  - Unguarded steering locals made plain: fadedrv/msgdrv `half` (`/ 2.0f`), itemdrv `half` (`* 0.5f`) and `k`
    (`/ 4.0f`).
- **Reverted by the orchestrator** (EQUIV, not applied; patches in build/mm4/w/):
  - Named pool constants deleted so their uses become literals (data definitions): evt_telop (all eight; its MUST_MATCH lines
    9 -> 1 and its opt_common_subs pair goes; telop_literals_lead.patch),
    evt_item EI_HALF, ac_crane_timing MATCH_F64/MATCH_F32, eff_kemuri3_n64, eff_sunlight_n64, eff_bomhei_n64,
    eff_star_power_n64 (*_literals_lead.patch). Under the constraint that every definition stays, evt_telop and
    evt_item were retried: nothing matches (the volatile constants block every region; best 2 lines). This is the
    n9-q-addrall sweep applied to constants a MUST_MATCH conditional names, which the standing rule excluded; Isaac's
    call whether that exclusion still holds once the conditional goes with them. symbols.txt would keep the names.
  - Hacks offered as the only version: win_root's `(10.0f + (x - 250.0f)) - 10.0f` (a cancelling offset; unguarded
    siblings with non-cancelling offsets exist in the same function), battle_attack_audience's `*(GXColor*)&dat_8041ccc0`
    (a pun forced by the object's declared type, but the task said puns stay guarded).
  - Not applied by the workers themselves: eff_indirect `{ f32 w = 32.0f; half = -w / 2.0f; }`, eff_sweat
    `x0 = -x1; x0 *= h; x1 *= h;`, battle_camera `base = -(1.5f * (wa + wb) - float_2 * zoom)` (drops in for both
    `_t` regions), evt_yuugijou `(scaled = E) >= 0 ? E : 0` (dead store), fadedrv `+ (half - half)`, icondrv
    `frames = d->frames - 2`, npcdrv duplicated `info.dist` store, evt_win/evt_shop folded-notKey forms.
- **Left, with what was tried** (one line per file or family; per-region shapes and scores in the reports):
  - Data layout (skipped by rule; most `#else` arms differ only in pool or section bytes): eff_bomb, eff_kiss
    SDATA2_CONST (both `#else` EQUIV in .text and data: leads), eff_particle/mot_roll/mot_jabara/pendulum/winmgr/
    mot_plane/evt_shop defer_codegen and pool helpers, battle_seq_end force_active (EQUIV), battle_damage's named
    sfx char arrays (`#else` EQUIV: lead), evt_bero/evt_mario/mapdrv/msgdrv/effdrv/npcdrv/windowdrv/flame/kameki/
    opuku_jetw/radiation/cloud padded strings, evt_cam/evt_mobj/mapdrv stripped-function placeholders, the ac `-1`
    declspec words, mobjdrv's negone_one_374 anchor, shadowdrv's Texture FIFO block and mapdrv's case-8 `wave` (their
    `#else` gives identical .text, only .sdata2 differs), eff_fire/break/funemizu/mahorn/mizutama/spark/spirit/
    mobj_broken volatile constant reloads (all-literal forms 88-118 lines and data), eff_kemuri r0/r26-28, booboo,
    win_item's string macros, win_main winFontSetC, mot_jump's named 0.1/0.8 (literals give identical code, only the
    pool differs), ac_monosiri_target/ac_shot_target `vecs` arrays (split into symbols.txt's 0xC Vecs: identical code,
    DIFF only on symbol size).
  - eff_kemuri: every code region tried in all-literal form: Main/2Main match that way, effKemuri2Entry 148 lines (1.0/
    0.7/-20/0 held in saved registers across the sin/cos calls), effKemuriDisp 144 (`-one * 0.5` at run time),
    effKemuri2Disp 198; with `opt_propagation off` all four Entry/Main functions match, so the named constants and
    the FIFO volatiles stay. n64: cloud sqrtf `x > 0.0f` 2 (extra pool word), horao_spell OSs16tof32/OSf32tos16 2,
    kemuri8/9 47, opuku_wgun nested if 1 (retail's extra beq), release 26-81, sweat 3-62, pokopi_pipo by rule.
  - effB: starstone loops 14 (unsigned bound fixes loops 2-4, loop 1's r26/r23 swap remains); fall/nice sfx (psndSFXOn
    prototype, Isaac's call); fukidashi `(u32)` 4; levelup ternary 15 and elemIdx 4; ibuki comma 4; kemutest dirptr 4;
    fire zero-Vec copies (data); recovery R1-R3 (compound literal EQUIV but deletes the unk_80423044 template: data).
  - drvA: mobjdrv fabs f64 locals (fabsf inline 24-34, fabs 61) and doubled inBattle (1-296); mapdrv `flag = !flag`
    (7 x 6 types, best 4), `ofs` walk (all 120 declaration orders, 120), `(s8)*name` by rule; envdrv vertex macro 16,
    envGlare x0s 20, cos temp 8, envFF_Sqrtf (needs a reshaped sqrtf), frustumE 364; shadowdrv u64 bufferSize 6,
    ProjMtx 78, PROJ_ZERO 253.
  - drvB: animdrv `(u32)x == 0` tails (u32/FileEntry* locals 6, int 4); arcdrv infoPtr 2-4; fadedrv `+ zero` (36-79);
    hitdrv hitFindElem 14 (600 declaration orders); icondrv frames 56 (720 orders); imgdrv ternaries 18 and identical
    arms; itemdrv volatile status 3 (calling itemStatus()); npcdrv identical-arm ternary 4, `(s64)(s32)` 4 (24 orders),
    punned kind store 7-153; windowdrv FIFO interleave 16 (272 placements), `goto ret` 2; effdrv/hitdrv asm approved.
  - batA: gauge_timing doubled test 1; stick_keep_left loop 2; signal_timing 2 (sfx family) and ICON_DISP_B (a local
    u16 prototype fixes that call, adds clrlwi to four others: 8); repeatedly 37 (36 shapes); genki 30; bakugame
    22-25 and neg2 (retail uninitialised read); timing_a 46; shot_target volatile stores 6/24; zubastar b18 base 55;
    scissor HSV inline 12-15; sac_suki table (a local initialiser moves it off its .sdata2 slot); button_down/shot/
    gauge_timing2 sfx family.
  - batB: ac_help acHelpResolve 3, makeWakuColor 10 (slot swap); the Waku_col by-pointer family (attack_audience,
    message, seq, status_effect) 12 (r6/r7); battle_audience goto (the hitResult==2 path jumps into case 4's body);
    kinopio `step` 27-32 without either pragma, const-stripping reads 6, memcpy 17; battle_camera bcam_get_unit 16
    (3000 sampled declaration orders), getUnitPtr 3 (24 orders); battle_damage `s8 rate` prototype (caller EQUIV,
    callee 14); seq_command `(s32)(s8)` (s8 parameter proven by neg; extsb), `(s32)` pointer compare; battle_stage
    MATCH_F32 and the `zero` alias (a typed MapTevCallback* parameter is still 16); stage_object _combine_nozzle_weapon
    194; status_effect `+ 0` (_get_pri(s32) 21); battle_unit status copies, partyData pun, `*(const f64*)&` 20;
    break_slot uninitialised read by rule.
  - evt: lottery days/limit 16 (r24/r25); evt_mario `(s32)(f32)` 35, searchUnder2 (header lead below); evt_mobj
    jumpstand 26, the five cmpwi `(s32)hit != 0` tests (the only cmpwi hitCheckFilter tests in the dol; BOOL local 4),
    mobjSwitchAnimIndex 49 (32 shapes); nannpc `s64 id` 2-121; shop/win notKey tails 3/4/5; sub one-iteration loop
    3-10; uranaisi table search 9-15; telop and item: see the reverted list.
  - mwm: cardmgr dead store and dead timeout test (12 more shapes, 16 + 4 per inlined copy); filemgr CLUT pun 10
    (SPM's PackTexPalette writes a union's u32 member; non-compound 10); winmgr `zero = zero` (136 without), arrow
    Vec reloads 552-599, macros and Waku helpers as recorded; mot_hammer type copy 11-13, clamp 3, volatile Vec copies
    (compound literal 168), argument temps 10, dead attribute compare, `five`; mot_roll volatiles; mot_jump; mot_ship;
    mot_slit `speed = speed`; win_badge `arg0`; win_item walks (all 5040 integer declaration orders, best 16);
    win_log fabs temp and helper; win_main vertex macro; win_mario `d2`; win_root `!= 0 && > 0`, uninitialised `row`.
  - No unguarded pragma pair in a changed function became removable (balloon opt_dead_assignments 76 without it,
    release opt_common_subs 81, zubastar opt_propagation 26, telop opt_common_subs 24, evt_npc_set_color 2).
- **For a reviewer.** The two header retypes (GlobalWork.language u32, BadgeShopWork.field_0x10C int). mobjdrv's
  tentative definitions. sac_scissor's file-local fabsf (the SDK's own body; one could call it a renamed steering
  helper). battle_ac's dropped `volatile` on a guarded data definition. evt_npc's bare float test. cardmgr's `chan`.
  eff_break's sizeof counts (they explain the redundant test as two compile-time counts that happen to be equal).
- **Leads.**
  - sac_zubastar: its three `#else` arms together are the plain initialised `Vec points[4]` and match (each alone
    doesn't compile, which is why earlier passes missed it; the recorded hang was ZubaStarPoints4's initialiser). It
    turns the guarded `zubastar_points_init` object into a compiler template `@NNN` (same bytes and place), so it is
    a data-definition change: not applied, Isaac's call.
  - The deleted-constant patches above (eight files; the biggest readability win this pass found).
  - `searchUnder2(..., f32* out)` in include/mario/mario_hit.h with `*out = hit ? hitPos.y : -1.0f` in mario_hit.c
    removes evt_mario's two and evt_party's two regions (all 25 users EQUIV with mario_hit.c edited; that file is the
    mm3 branch's: build/mm4/w/evt/searchUnder2_*.patch).
  - The `(u32)gp->language` casts left in src/mario, src/party and rels/ are now no-ops (mm3's trees).
  - battle_enemy_item.c passes `(s32)wp->alliances[...].attackDirection` unguarded at six sites, the no-op widening the
    consist rule guards.
  - Unguarded steering the workers saw outside regions (adding guards was out of scope this time): fadedrv z()'s
    `one`, windowdrv `zero` and the x4 split, itemdrv `height`, npcdrv `(s8)name[0] == 0`, evt_nannpc `zero`/`offset`/
    `(void)strcmp`, mot_hammer hitEstimate's `p`, mot_slit `zero`, mot_ship's doubled `sp0C = 0.0f` and bare
    `marioGetPtr();`, eff_kemuri effKemuri2Entry's zero/neg20/p7/one and effKemuri2Disp's p8/p12/n12p8; the
    jabaraSetCamParam/rollSetCamParam and yuugijou scale700/scale2000b inline names have no outside evidence.
  - win_mario's early pose-name selection looks like a second inlined copy of the tail's (needs an evidenced name).

## Leftovers from #1125-#1127: language casts, searchUnder2, motionSfxId2, steering guards (2026-10-22, Claude Opus 5.5, n9-q-leftovers)

Task (Isaac): the leads the last three chapters left. (1) The `(u32)gp->language` casts made redundant when #1127 retyped
the field; (2) `searchUnder2` taking an `f32*`; (3) battle_enemy_item's six `(s32)` casts; (4) mot_hammer's 0x2B8 sound
handle, named like mot_hip's if the evidence carries over; (5) the "unguarded steering seen outside any region" lists from
#1126 and #1127, each guarded with a natural `#else` or removed if it is a no-op. Data layout was out of bounds.

- **Method.** Untracked harness in build/lo/. try.py compiles each source for every object it builds (from `ninja -t
  compdb`, so rel units like the three evt_kagemario copies resolve), then runs obj_equiv against objects saved before
  the branch (`--nat` compiles without MUST_MATCH; `--alt` compiles a variant file in its place; `--fn` diffs one
  function). alt.py writes variants, and users.py lists every unit that includes a header, following `.c` files
  included into other units as well. ci.sh is tools/ci.py with its tools/tests step replaced: it runs the tests and
  accepts only the 8 test_wave_launch environment errors (pyelftools comes from `uv run`). Each commit's files were
  EQUIV and NAT OK one by one, header changes over every user (25 for mario_hit.h, 192 for mario.h). CI PASSed after
  commits 2, 4 and 9. At the end, `obj_equiv --tree` over all 2,461 objects against the pre-branch snapshot showed no
  difference. `build/G8MJ01/ok` was recreated, and fncast_lint and local_prototypes are clean.
- **Counts** (`tools/cleanup_index.py`, 24f8d288 -> this branch): index **19.852 -> 19.879** per kLOC (13751 -> 13773
  weighted). must_match 693 -> 728, raw_offset 190 -> 184, struct_ptr_cast 1459 -> 1457, prim_ptr_cast 1962 -> 1963.
  The rise is the guards item 5 asked for (33 new regions) and battle_enemy_item's 6. Against it: the 4 regions
  searchUnder2 removed, mot_hammer's 5 raw offsets and win_badge's 1.
- **1. Language casts.** Only three were left, all in rels/: sys.c (2) and kpa.c (1). src/mario and src/party had
  none. They are gone, and both objects are identical.
- **2. searchUnder2(f32* out, ...).** mario_hit.c stores `*out = (hit != NULL) ? hitPos.y : -1.0f`. evt_mario's and
  evt_party's callers already gave it a one-float home under MUST_MATCH; that is now the only form, so their four
  regions and the `(Vec*)&` casts go. The locals keep their names (`vec`, `pos`) although they now hold the floor
  height.
- **3. battle_enemy_item.** The six `(s32)wp->alliances[...].attackDirection` casts are not no-ops. Retail
  sign-extends the s8 before the call, and without the cast MWCC passes the byte unextended to BattleSamplingEnemy's
  s8 parameter, so each function is one instruction short. They are guarded, with the plain argument under `#else`.
  An `s32 attackDirection` prototype was tried as the declaration-level fix; it changes battle_event_cmd,
  battle_seq_command and battle_item_data, so it was not kept.
- **4. motionSfxId2.** mot_hammer stores -1 at 0x2B8 on entry and the `psndSFXOn_3D(0x162)` result in the swing. Later
  it tests `(u32)` against 0xFFFFFFFF, calls psndSFXOff and stores -1 again. That is the pattern that named
  `motionSfxId0/1`, so the 0x2B8 union gets `s32 motionSfxId2; // sound handle in mot_hammer (-1 when none)`, and
  mot_hammer's five `M_S32(mario, 0x2B8)` sites use it. mot_stay's 0x2B8 counter and mot_jabara's zero store there are
  not handles and stay raw.
- **5. The steering lists.** #1126's chapter carries no such list. Its workers' reports were still in /tmp/mm3/*/
  REPORT.txt and gave three items. #1127's per-worker notes are gone from build/mm4/, but its session transcripts give
  the full lists, which add win_badge's selPtr, effKemuri2Main's re-read and effKemuriMain's `scale` to the ledger
  summary. Each plain form was compiled first:
  - **Plain now (no guard):** mot_slit's two `zero` locals. The thirteen `x == zero` tests become `!x`, and the two
    `zero == x` tests become `x == 0.0f` (all-literal `== 0.0f` is 96 lines off). evt_nannpc nannpcMoveEntry's `zero`
    becomes `if (!vlen)`.
  - **Guarded, plain form under `#else`** (lines differing for the plain form in brackets):
    - fadedrv z(): only the final `one *` multiply needs the local [13], so the early return and the first
      projection take `1.0f` and `one` is block-local in the guarded return.
    - windowdrv: `zero + ...`/`zero - ...` [703] and the two-statement `x4` [2].
    - itemdrv: `height = 15.0f` [10-30; a single computed `y` 262].
    - npcdrv: two `(s8)name[0] == 0` tests [2 each; retail `extsb.`], in mapdrv's form.
    - evt_nannpc: the `offset` induction variable [18; pointer walk 122] and the discarded strcmp [53 without the
      call], the latter omitted under `#else` (mario_balloon's precedent for discarded calls).
    - mot_hammer hitEstimate: the `{ f32 p = ...; c = cos(p); }` locals [9]; two of the three join the existing
      `five` regions.
    - mot_ship: shipWakeUpdate's discarded marioGetPtr() together with the 0/1-flag ternary after it [318-454 for
      the plain forms], and shipMove's doubled `sp0C = 0.0f` [153].
    - eff_kemuri: effKemuri2Entry's constant copies [144], effKemuri2Main's re-read [29], effKemuriMain's `scale`
      temps [24], effKemuri2Disp's p8/p12/n12p8 [20].
    - win_badge: selPtr's matching arm is `(s32*)work->badgeSel + sel` (0x3E4 is badgeSel; the own-type cast keeps
      retail's `work + sel*4` base), and the natural arm is `&work->badgeSel[sel]` [10].
    - tik_00 mario_key_onoff_wait, gor_01/gor_03 mario_keyoff_wait: the `switch (!marioChkKey())`, as #1126's review
      guarded mario.c's [8 for if/else and ternary].
    - gon_03: `half = 0.5f`, block-local [the literal differs].
    - jabaraSetCamParam, rollSetCamParam, scale700 and scale2000b (no outside evidence for the names): written out
      at the calls the objects differ (the cam-param stores move their Vec template in .rodata; yuugijou 12 lines
      each), so each inline and its calls are guarded, with the written-out code under `#else`.
- **For a reviewer.** `motionSfxId2` continues the slot numbering of motionSfxId0/1, which is a real series (one
  handle slot per offset). mot_slit's mixed `!x` and `x == 0.0f` spellings. The win_badge natural arm keeps a
  three-use pointer local. mot_ship's guard includes the `b ? 0.3f : 0.6f` flag ternary beside the listed
  marioGetPtr() call.
- **Left.** Unguarded steering seen in passing that was on neither list: eff_levelup's `halfConst`/`yConst` constant
  locals. The bare `marioGetPtr();` at the top of tik_00 mario_key_onoff_wait and of several mot_ship functions stays
  unguarded (most likely unused-local initialisers in the original, not mid-function steering).

## Cleanup decisions from Melee, TWW, TP, AC and SPM precedent (2026-10-22, Isaac, written up by Claude Opus 5.5)

Six open questions left by #1123-#1128, checked against the local reference checkouts (~/melee, ~/ttyd-refs/tww, tp,
ac-decomp, spm-decomp). Isaac approved the recommendations.
- **Named pool constants -> literals: yes.** File-scope named `const f32` pool objects: TWW 0, TP 0, AC 0, Melee 6
  (all library), SPM 1; TTYD 857. The others write literals and let the compiler build `@NNN` pools.
- **sac_zubastar's guarded template -> compiler template: yes** (the #1127 lead), the same move as #1120/#1122.
- **Typing initialised data tables: yes.** `(u32)&sym` inside initialisers: 0 in all five (their 13-35 casts are in
  code); TTYD about 1,300. They use typed structs with pointer members (TWW profiles, SPM `DokanDesc`); SPM's only
  word arrays are evt scripts, whose cast sits inside the macro.
- **Effect `userdata` union: no.** SPM keeps `void* userWork` in EffEntry, Melee `void* user_data` behind cast macros;
  TWW/TP use C++ inheritance. Nobody uses a union of pointer types.
- **Per-file `-char signed`: yes** where it removes casts and the object stays identical (TP: 20 per-file overrides).
- **Sound ids passed as names (`psndSFXOn((const char*)0x2002A)`, 401 calls; 221 pass strings): left as is.** The
  others take ids as integers or enums, and nobody casts a number to `char*`; but SPM's prototype is `const char*` and
  only passes strings, and the only cast-free form here is a new id-taking accessor, a name without outside evidence.


## Initialised data tables typed (2026-10-22, Claude Opus 5.5, n9-q-datatype)

The #1129 decision ("typing initialised data tables: yes") applied: the `(u32)&sym` / `(u32)sym` / `(u32)"str"` words
in raw `u32`/`s32` tables became typed initialisers (structs with pointer members, pointer lists, typed work
pointers). Symbol names, sections (declspecs kept), sizes and bytes unchanged; build byte-identical, data_check clean
and the objdiff ratchet flat after every commit. Event scripts stay `s32` words.

- **Method.** tools/struct_render.py (the wave-19 renderer, which reads the original's bytes and relocations from dtk's
  asm) driven by a one-off script, not kept: tools/ is protected. The driver parsed each struct's layout from the
  current header instead of struct_render's own LAYOUTS (several of those are stale against the headers), rendered
  nested structs and long arrays as indented blocks, kept named string objects and `const` string lists by name,
  turned a `sym + N` relocation into `&sym[k]`, and kept a symbol where dtk had read a flags word as an address
  (`(u32)lbl_80003500` in partyWeapon_ChuchurinaKiss's targetWeightingFlags: as a plain number objdiff's data score for
  the whole unit fell from 100% to 5%). Then struct_render's fix_index/fix_refs for `sym` <-> `&sym`, externs in the
  module's other files (REL symbols only within their module), the readers' casts by hand.
- **Typed** (about 500 tables): party units, unit_mario and unit_purple_teresa (BattleUnitKind / Part / Weapon /
  DataEntry / DispPose / BtlUnitPoseSound); the sac, item, audience and break_slot weapons; superActionTable
  (BattleWeapon*), win_party's `*_waza` (BattleWeapon*; WindowPartyDt.waza is `BattleWeapon**`); npcEnt and the other
  NPC lists in 30 map files (NpcSetupInfo), the 18-word door tables (DoorEntry), kusa/ueki/tree tables; the audience
  tables (BattleAudienceKind, `s32*` animation lists over `s32` word lists, BattleAudienceItemKind); the
  eff_starstone/nozle/toiki display-list lists (`void*`) and eff_ice's Maya tables (its own IceRSTEntry etc.);
  battle_stage_object's nozzle and fall-object data (BattleStageNozzleData/FallObjectData), `_entry_*` (BattleUnitSetup)
  and `_object_*_data` (BattleStageObjectData); gor_irai's event lists; rsh's random-npc tables; sys's texture-id
  lists (`s8`, `s8*`, `s8**`); the inns' kino_dt (KinopioData) and nok's door tables (DoorParam); 23
  pose_sound_timing_tables (BtlUnitPoseSound); hitDmgRetPoints/dmgRetPoint (HitDamageReturn); _map_attr_data
  (MapAttrData); the ext npc tables (ExtMakeTextureEntry; rsh_06/yuu as their readers' MoamoaGroupDef/YuunpcPoseArg);
  tou's fighterDt/ruleDt (the file's FighterEntry/RuleEntry); gor_02's follow (FollowPath), gor_01's uwnobj_list
  (UnitWinObj), gor_03's yuu_0N_data (BeroEntry); credits_ending_records and R_credits_npc_info.
- **Typed work pointers** (#1123's leads): tou's `rank_wp`/`dwp` (RankWork*/DispWork*, tou2's form; every cast gone,
  no load swap), end's `credits_work_ptr` (CreditsWork*; credits_work is a CreditsWork and the WORK cast macro went),
  tik_05's `broken_wp` (BrokenWork*).
- **Header changes, with their evidence.** DoorEntry.field_0x2C is `s32*` (door_down_se's evt_door_param 6 spawns
  it; the typed doors' `(s32)` casts went). KinopioData and DoorParam moved from evt_kinopio.c / evt_door.c to their
  headers, pointer fields typed from the data (message keys, bed events; DoorParam's 0x14-0x38 map object and hit
  names, 0x3C/0x40/0x44/0x48 npc/noclip-map/mobj/item lists). BattleStageFallObjectData.field_0x0 is a BattleWeapon
  (its bytes and relocation are one). FollowPath got a struct tag. New: `RndNpcData` in rels/rsh/include/rsh.h
  (type from `_get_rnd_npc_data`; `animName` is npcEntry's parameter, `tribeName` evt_npc_set_tribe's, `talkEvt` the
  NpcEntry field evt_rsh_set_rnd_npc_talk_evt sets) and `RootData` in aji_02.c (from `root_data`; `list`/`count` are
  hikaru_on_func's locals). yoshi's local PoseSoundEntry was a copy of BtlUnitPoseSound and is gone;
  `_yoshi_slide_move_sound` and `mario_pinch_pose_sound_callback` take the entry and state BtlUnit_ControlPoseSoundMain
  passes them.
- **Natural forms the typing made possible.** With CreditsNpcInfo/CreditsEndingRecord arrays, credits_pose_evt's and
  credits_record_evt's `((T*)((u8*)tbl + off))->f` became `tbl[idx].f` and the steering `off` local went: the object
  is identical, so the indexed form is the original's. rsh's `_get_rnd_npc_data` returns `&data[index]`, aji_02's
  hikaru_on_func reads `root_data[idx].list`.
- **String lists.** A list a `const char**` field points at became `const char*`, except where a script also passes it
  with PTR(): MWCC refuses `(s32)` on a `const char**` in an initialiser ("illegal implicit conversion from 'const char
  **' to 'char **'"), so those keep `char*` and a `(const char**)` cast in the entry (gor_01's door_data_01_npc).
- **Left, and why:**
  - defence / defence_attr / regist in the party units, unit_mario and unit_purple_teresa (19+19+9 casts at their
    BattleUnitKindPart/Kind users): the original objects are 8/8/24 bytes, the structs 5/5/22, and objdiff scores the
    symbol size (unit_party_clauda 100% -> 99.2% matched data when typed). Fixing it means symbols.txt sizes.
  - Word tables a script reads with SET_INT_READ_ADDR / READ_INT (event-ABI data with LW/GSW/FLOAT words): kpa_02-06's
    mobj lists, rsh_04's access_tbl, sharinDt/clankDt, the quiz tables, aji_13's tables, tik_00's name_tbl,
    batten_leader's light lists.
  - gor_03's shopper_data: ShopperData is local to evt_shop.c and ends in a flexible array, the other maps spell it as
    a `char*` list, and its one event pointer would need a cast.
  - mobjdrv's cube display tables (dead, force_active, no reader to name a struct from); usu's booboo_name ({name,
    bytes} pairs, no name evidence); the 17 `btl_*_setup_no_tbl` ({name, index}, read by dbGetDefData, whose
    parameter type is NpcTerritoryInfo: a shared type would need a name).
  - Lead: eff_ice's local Maya structs duplicate effdrv.c's EffectMayaTrack shape; one header type would also type
    eff_damage_star's and eff_recovery's tables.
  - The 1,021 `// TODO: type` markers left are nearly all pointer-free data: display lists, vertex/spline/colour
    tables, byte lists. ptr_to_int's 32 left: 23 in the script, shop and dead tables above, 9 event-ABI or address
    arithmetic in code.
- **Index** (`tools/cleanup_index.py`, d8940c4e -> this branch): 19.879 -> 17.128 per kLOC (13773 -> 11977 weighted);
  ptr_to_int 1571 -> 32, struct_ptr_cast 1457 -> 1245, prim_ptr_cast 1963 -> 1932, raw_offset 184 -> 177. `// TODO:
  type` markers 1554 -> 1021. battle_audience_kind_data, eff_starstone, gor_03 and tou left the worst ten.
- Verified before each commit: `ninja` recreates build/G8MJ01/ok; tools/ci.py PASS (tools/tests stubbed for this
  machine's environment failures, run under `uv run --with pyelftools`, configure.py rerun with the normal Python
  after); every changed C file also compiled without MUST_MATCH by hand (ci's natural step looks REL files up by stem);
  fncast_lint and local_prototypes pass.
## Named pool constants, the zubastar template and per-file `-char signed`, applied (2026-10-22, Claude Opus 5.5, n9-q-poollit)

Task (Isaac): apply the three decisions of the chapter above. (1) Delete named pool constants whose only job is a
MUST_MATCH pool-order trick together with the trick, starting from #1127's reverted list, then sweep the tree; deleted
objects' symbols.txt entries become `@NNN` as #1122 did; no helper or dummy functions. (2) sac_zubastar's three `#else`
arms as the plain `Vec points[4]`. (3) `-char signed` for evt_mobj and any other file with many `(s8)` char casts, kept
only where the object stays identical. Data layout otherwise untouched.

- **Method.** Untracked harness in build/pl/: objects saved before the branch (build/pl/base, 1,830 objects); try.py
  compiles a source (or a variant in its place) for every object it builds, from `ninja -t compdb`, and runs obj_equiv
  against them (`--nat` without MUST_MATCH, `--flags` for an extra compiler flag). pool_sweep.py groups each file's named
  scalar f32/f64 definitions with every top-level MUST_MATCH region that names them, and tries each group in mode A (its
  regions replaced by the natural arm, its constants literal) and mode B (constants literal, regions kept): all groups at
  once, then greedily from the highest address, each on top of those kept. lever_search.py hill-climbs over a few plain
  rewrites (below) on the all-literal variant and keeps a result only when it is identical. s8_greedy.py drops `(s8)`
  char-read casts one site at a time under `-char signed`. symrename.py maps each deleted object to the compiler's `@NNN`
  local at the same place and edits symbols.txt; hdrclean.py removes header externs nothing else names. ci.py there is
  tools/ci.py with its tests step accepting only the 8 test_wave_launch environment errors (pyelftools from a uv venv in
  build/pl/venv). Every commit: `build/G8MJ01/ok` recreated, changed files compile without MUST_MATCH, fncast_lint and
  local_prototypes clean, CI PASS (`--allow-protected` for symbols.txt, `--allow-configure` for the flags). At the end
  `obj_equiv --tree` over all 1,830 objects against the pre-branch ones: every one equivalent.
- **Counts** (d8940c4e -> this branch). `tools/cleanup_index.py` **19.879 -> 19.723** per kLOC (13773 -> 13653
  weighted): must_match 728 -> 703, deref_cast 520 -> 489, prim_ptr_cast 1963 -> 1933, opt_pragma 327 -> 325,
  address_name 1078 -> 1077; pool_constant (weight 0) 5857 -> 4933. `#if...MUST_MATCH` lines in src/ and rels/ 706 ->
  681. File-scope named f32/f64 definitions 1134 -> 867 (statics included), those a MUST_MATCH region or
  MATCH_F32/F64 names 139 -> 110. symbols.txt: 185 entries renamed to `@NNN` (scope:local). Code lines 692,849 ->
  692,222.
- **1. Named pool constants.**
  - #1127's list: evt_telop (all eight constants; seven of its eight MUST_MATCH regions and the opt_common_subs pair
    go, and the font origin is `(width - scale * msgWidth) / 2.0f + (-15.0f)` without the steering locals), evt_item (EI_HALF), ac_crane_timing (MATCH_F32/MATCH_F64 and the file-local MATCH_F64 macro), eff_kemuri3_n64
    (with its volatile FIFO helper kemuri3WriteF32) and eff_sunlight_n64 converted. eff_bomhei_n64 stays: the third
    `0.0` of its PSMTXTrans is a separate `lfs` in retail, which only the volatile read gives (a literal is copied with
    `fmr`). eff_star_power_n64 stays: with literals MWCC pools 128/127 ahead of 6.2832/7.1234/360 (nine words move).
  - The automated sweep converted 34 files (eff_kemuri3_n64 and eff_sunlight_n64 among them): party_yoshi (36, all its value-named statics), party_nokonoko (16),
    battle_break_slot (12), sac_genki (7), ac_lst_rc_keep, ac_stick_keep_left, ac_monosiri_target (8), battle_camera,
    sac_suki, eff_sandars, eff_spark, eff_stageclear, unit_party_chuchurina, mot_stay, and the n64 effects
    breath_fire, gareki, kemuri3, magic3, opuku_jetwrain, opuku_wprotect, rappa, ripple, snowfall, sunlight, sweat,
    thunderflare, water_damage; in the RELs bom_02, eki_02, las_08, unit_boss_batten_satellite and both
    evt_yuuranking units. Most were outside n9-q-addrall's reach (value-named statics, header externs, MATCH_F32
    reads) or in files that have changed since. Empty pragma pairs and comment headers
    that only wrapped the deleted definitions went with them (battle_break_slot, unit_party_chuchurina).
  - By hand: ac_repeatedly (all twelve; its four volatile-reload regions go), battle_damage (the static 0.5 behind
    MATCH_F32), pik_02's hoshi_jump (the invented `c50`/`half_sym` statics and their reads; only the punned userdata
    stores stay guarded), and with lever_search eff_kemuri7, eff_akari_pose, eff_butterfly, eff_fireworks and
    eff_thunderflare.
  - **Levers.** A literal operand is ordered differently from a named load in a commutative operation or a compare, so
    `x * 0.5f` swaps FPRs where retail had a named 0.5. The plain forms that give retail's order: `x / 2.0f` for
    `x * 0.5f` (telop, item, damage, pik_02, fireworks, akari_pose), compound assignment for `x = x * K` / `x = v + K`
    (crane `velocity *= -1.0f`, repeatedly `+= 0.01f` / `-= 0.01f`), and `if (x)` / `if (!x)` for a test against 0.0f
    (repeatedly, butterfly, kemuri7, thunderflare). Operand swaps (`K * x` vs `x * K`) never helped.
  - **Mode B** (literals inside a region that stays): battle_camera's two `_t` product temps and party_nokonoko's
    fadd32 and 0.4 regions keep their steering, now with literals.
  - **Left, and why** (86 files still define named floats; with every float literal they fail, nearly all on pool
    order): a trailing named
    `.sdata2` gap word lands ahead of every compiler literal (mot_jump, ac_shot_target, battle_stage, battle_attack_audience,
    eff_break, peach, mot_jabara); named strings or ints interleaved with the floats in `.sdata2` (mobjdrv, database,
    ac_repeatedly's `dat_8041c7a8`, the dat_ ints of the ac_* gauges, which are loaded with `lwz`); literals first used
    later than retail's named order (statuswindow, win_main's 0.5, eff_pturn's 360/4, battle_icon, battle_mario,
    eff_akari_sflush, koopa, himo, eff_kemuri, eff_mizutama, eff_mobj_broken, eff_mahorn); a named zero added at run time
    that a literal folds away (eff_cloud's `x + 0.0f`); FIFO vertex writes whose reload pattern only the volatile
    constants give (eff_spirit, eff_spark's float_0, shadowdrv); `.rodata` doubles placed among other named objects
    (battle_audience_kinopio); ac_monosiri_target's double inside the guarded `vecs` base. Per-file results are in
    build/pl/sweep_apply.json, lever_all.txt and classify.txt.
- **2. sac_zubastar.** The three arms together are `Vec points[4] = {...}` with plain element stores; the guarded
  `zubastar_points_init`, the ZubaStarPoints4 copy type (header) and the punned copy go. MWCC's own template for the
  initialiser is at the same `.rodata` place, so symbols.txt's `dat_802ff6d4` becomes `@297`. No hang (the recorded hang
  was the ZubaStarPoints4 initialiser).
- **3. `-char signed`.** evt_mobj (18 `(s8)name[i]` casts, five `*(s8*)childName` puns, and mobj_switch_float_blk's
  `s8*` name walk). Twelve more files where the flag keeps the object identical and then every char-read cast goes:
  fontmgr (60 of 61; the remaining `(s8)code` narrows a u32; msgAnalize's `msg++; (s8)*(msg - 1)` becomes `*msg++`
  like its siblings), both evt_yuuranking units (5 each; fontlenpitch.h's static takes `const char*`, which drops the 14
  `(s8*)buf` casts at its calls), mapdrv and npcdrv (their three guarded sign-extending tests: three MUST_MATCH regions
  go), las_14, gon_11, evt_twoddisp, evt_nannpc, eff_number, eff_pointget, unk_0x8025de70. Not flagged: evt_win (5 of
  7 go, but its two `((s8*)p)[i]` reads must stay and would then be no-op pointer casts, a guarded trick: lead);
  msgdrv, romfont and gor_03 change with the flag as they are (they have unsigned char reads); battle_stage_object,
  battle_damage, party, eff_opuku_jetwrain, evt_yuunpc and animdrv's `(s8)` casts narrow ints or u8 data, not chars.
- **For a reviewer.** symbols.txt: the task asked for `@NNN` on every deleted object, so 185 entries were renamed
  although the data ratchet passes without it (n9-q-addrall left its 462 names). The float truthiness tests (`if
  (mag)`, `if (!data->direction)`) and the `/ 2.0f` forms are what the object needs, not style. `+ (-15.0f)`-style
  additions keep the pool's negative constant. sac_genki's `f32 zero` block went (identical without it).
  ac_stick_keep_left's stickLeftAdd25 inline is still needed (a plain `25.0f + y` shifts one FPR) and has no outside
  name, so it is under MUST_MATCH with `yy = 25.0f + y` under `#else`. The thirteen new `extra_cflags` on Object lines.
- **Leads.** evt_win under `-char signed` with its two `((s8*)p)[i]` reads guarded (net: 5 casts for one region).
  eff_pturn and win_main are one literal's first use away from matching. The pool_sweep/lever_search harness could run
  again after other cleanups free more pools; it is in build/pl/tools (untracked).

## Sound and window headers named from their consumers; sound, pmario_sound, win_root, winmgr deep-cleaned (2026-10-24, Claude Opus 5.5, n9-q-fsound)

Task (Isaac): name and type the `field_0xNN` members of include/sound.h, include/pmario_sound.h and include/win/win_main.h
from their consumers (CLEANUP_GUIDE section 3), update every user, then deep-clean src/sound.c, src/pmario_sound.c,
src/window/win_root.c and src/manager/winmgr.c as #1110/#1114 did. Parallel branches owned src/battle and
mapdrv/animdrv/mario; none of their files was touched. Data layout was out of bounds.

- **Method.** Untracked harness in build/fs/: try.py compiles a source, or every user of a header (from `ninja -t deps`),
  into a temp dir and runs obj_equiv against objects saved before the branch (`--nat` without MUST_MATCH, `--alt` a
  variant file, `--fn` a function's asm diff); var.py applies OLD/NEW replacements to a copy; sites.py lists the
  index's sites by line; ci.sh/ci_local.py run tools/ci.py with the mgba venv's python (a uv-run python rewrites
  build.ninja's `$python` and re-downloads the tools) and accept only the 8 test_wave_launch.PassPipes environment
  errors. Every commit: each header's users EQUIV, changed files NAT OK, CI PASS, `build/G8MJ01/ok` recreated,
  fncast_lint and local_prototypes clean. At the end: `obj_equiv --tree` over all 1,830 objects against the
  pre-branch snapshot, every one equivalent; the twelve changed C files built under `configure.py --non-matching`
  (their only output is 25 "function has no prototype" warnings, all already present on origin/main: win_root's
  sort_*_func, some psndSFX_* functions and _sscallback), then reconfigured and rebuilt to `ok`.
- **Counts** (`tools/cleanup_index.py`, 35dc3b78 -> this branch): index **16.972 -> 15.891** per kLOC (11857 -> 11102
  weighted). field_0x 4624 -> 3886, raw_offset 177 -> 171, prim_ptr_cast 1902 -> 1897; everything else unchanged.
  Per file (weighted, per kLOC):
  - src/sound.c 246 -> 37 (118.783 -> 17.866): field_0x 209 -> 0
  - src/pmario_sound.c 243 -> 70 (34.685 -> 9.981): field_0x 159 -> 3, raw_offset 7 -> 1, prim_ptr_cast 15 -> 10
  - src/window/win_root.c 209 -> 18 (79.741 -> 6.868): field_0x 192 -> 1 (animdrv's `clip->field_0x8`, fmap's)
  - src/manager/winmgr.c 191 -> 190 (85.997 -> 85.547): field_0x 1 -> 0
  - include/sound.h 30 -> 0, include/pmario_sound.h 21 -> 3, include/win/win_main.h 38 -> 1 (unk_80177008, a symbol)
  - consumers: win_item 135 -> 49, win_badge 36 -> 28, win_party 12 -> 8, evt_badgeshop 20 -> 13; win_log 51 -> 59
    and win_log.h 2 -> 8 (below: invented placeholders turned into `field_0xNNNN`).
  sound.c, pmario_sound.c and win_root.c left the worst ten; winmgr is second (its 157 address names are colour and
  Vec templates, which stay).
- **sound.h** (consumers in sound.c unless named).
  - StreamHeader (the .stm header SoundSSPlayChEx_main copies): 0x0 `format` (its switch: 0 PCM16, 1 sets flags 0x80,
    which _sscallback decodes with ssDecodeDPCM, 2 sets 0x100 and passes the ADPCM info to sndStreamAllocEx with flag
    0x10001); 0x8/0xC `playEnd`/`playLoop` (copied to SoundStream's; 0xFFFFFFFF = no loop, else flags 0x200);
    0x10/0x18/0x20/0x24 pairs as arrays `readEnd[2]`, `readLoop[2]`, `dpcmStartSample[2]`, `dpcmLoopSample[2]` (copied
    per channel); 0x5C/0xBC `adpcmInfo0/1` (copied to entries[0/1].info: one per channel).
  - SoundStreamEntry: 0x28 `buffer` (sndStreamAllocEx's buffer argument; the header read's target first); 0x38 a
    union `playPos` (ssDecodeDPCM/ssCopyPCM8/16 advance it, compare with playEnd, wrap to playLoop) / `cachePos`
    (_ssDVDReadAsync_cache_next: the ARAM write offset, compared with the file length in cache_aram): two uses, one
    member each; 0x3C `bufferLength[2]` and 0x44 `bufferPos` (the decoders' end-of-buffer test `bufferPos >=
    bufferLength[curBuffer]`); 0x4C `curBuffer` (`loadBuffer[curBuffer]`); 0x4E/0x50/0x52 `dpcmStartSample`/
    `dpcmLoopSample`/`dpcmSample` (case 1 copies 0x4E into 0x52; the decoder adds each delta to 0x52 and resets it from
    0x50 at the loop); 0x54 `nextReady` (_ssDVDReadAsync_activeChk sets it when the other buffer's read lands; the
    decoders switch buffers only when it is set; SoundSSMainInt issues the next read while it is clear); 0x56
    `firstReadDone` (activeChk's second loop activates the stream once every channel has it); 0x58/0x5A/0x5C
    `dpcmScale`/`dpcmRunPos`/`dpcmRunLength` (ssDecodeDPCM's run escape: index 8 starts a run of `(byte >> 4) + 4`
    nibbles that reuse the last scale); 0x60/0x64 `readEnd`/`readLoop` (_ssPlayChRead clips reads to readEnd;
    activeChk restarts bytesLoaded at readLoop).
  - SoundStream: 0x2 `cacheSlot` (SoundSSPlayChEx's free sound.cacheUsed index; cacheAram[cacheSlot] is the ARQ
    address); 0x4 `dataOffset` (0x40 or 0x100 by format, added to every file read offset); 0x10/0x14 `curVol`/
    `fadeStep` (SoundSSMain steps curVol by fadeStep; SoundSSFadeoutCh sets fadeStep = curVol / frames; the mix
    parameter is curVol); 0x1C/0x20 `playEnd`/`playLoop`; 0x24 `samplesWritten` (_sscallback's count and return value).
  - SoundDVD: 0x2 `pan` (SoundDVDSetPanCh stores it, SoundDVDMain passes it back); 0x4 `vol` (the fade-in stops
    there); 0x8 `curVol` (split into AISetStreamVolLeft/Right); 0xC `fadeStep`.
  - SoundEffect 0x2 `prio` (SoundEfxPlayEx takes over a slot whose prio <= the new one's; its parameter renamed too);
    SoundGroup 0x2 `index` (SoundSLibLoadDVD's list position, SoundSongGetGroup's key).
  - SoundWork: 0xE8 `nextGroupId` (SoundLoadDVD2PushGroup), 0x1F4 `headingAngle` (SoundEfxMain's degrees -> heading;
    nothing writes it), 0x20C `interruptDepth` (the OSDisable/RestoreInterrupts nesting count), 0x21C/0x224/0x226
    `cacheAram[2]`/`cacheCount`/`cacheUsed[2]`, 0x330/0x33C/0x5C0/0x15C0 `outputHalf`/`dmaSamples`/`outputSamples`/
    `outputPos` (SoundAIDMACallback copies the DMA buffer big-endian into dmaSamples and every other sample into the
    two halves of outputSamples; bom_01's cannon sight reads the next one, so its BomSoundView follows).
  - Left: SoundEffect 0xA (only decremented), SoundWork 0xFC/0x104/0x108 (only stored), SoundTemp's bytes.
- **pmario_sound.h** (consumers in pmario_sound.c; nothing else reads these structs but seq_title/evt_snd's psbgm
  streamId).
  - BGMListEntry's byte view: `volume`, `pan`, `group`, `inScope`. 0xE was called reverb: psndBGMOn_f_d keeps a playing
    BGM whose entry has the same nonzero byte instead of restarting it (BGM_TITLE1 and BGM_FILE_MENU1 both carry 1,
    the stage BGMs count up), and nothing passes it to a reverb call; PaperSoundBGM 0xD, where it is copied, was
    `reverb` too and becomes `group`. 0xF: 1 starts the BGM in scope (scopeTimer 0xFF).
  - SoundEffectEntry 0x10 `prio` (__psndSFXOn evicts a slot whose prio is higher), 0x11 `delay` (added to the slot's
    start delay). __psndSFXOn's 4th parameter is a delay in ms (divided by 16.67 into the slot's frame delay; SPM's
    __spsndSFXOn names its 4th parameter `delay`) and its 7th the dist it stores; parameters renamed, prototype given
    names. SoundEnvList 0x2 was `flags`: it is the pssfxlist index psndENVMain passes (| 0x0N000000) as __psndSFXOn's
    name, so `sfxNo`.
  - PaperSoundEnv: `sfxNo`, `envId` (psndENV_stop's key), `volume` (psndENVGetVol), `mode` (psndENVMain's switch),
    `intervalRand`/`interval` (the `* 60 + irand(...) * 60` timer), `radius`/`radiusRand` (mode 0's random play
    distance), `step`, `timer`, `span` (passed as __psndSFXOn's dist), `sfxId` (the id __psndSFXOn returned; -1 none),
    `position` (__psndSFXOn's position).
  - PaperSoundEffect: `sfxNo`, `serial` (psndSFXChk compares id bits 8-23), `flags`, `prio`, `delay` (psndSFXMain counts
    it down before starting), `attenuation` (taken off every volume), `channel` (the SoundSS/Song/Efx channel).
    PaperSoundBGM 0x0 `bgmNo`, 0x4's comment (a channel), 0x20 `fade`.
  - PaperSoundEnvWork: `fadeState`/`fadeFrames`/`fadeTimer`, `silentState`/`silentLevel`/`silentFrames`/`silentTimer`
    (the documented fade record; "silent" from the original psndBGMChkSilent and silent_tbl); 0x1 is never touched:
    `pad_0x1`.
  - PaperSoundWork: `envFade[2]`, `envId[2]`, `envLpf[2]`, `envLpfTarget[2]`, `sfxFade`, `mapName` (psndMapChange's
    strcmp/strcpy), `sfxSerial` (stamped into each id), `repeatDelay` (__psndSFXOn's extra delay for an effect queued
    again in the same frame), `bgmLevel`/`sfxLevel`/`envLevel` (the percentages psndBGMMain, psndSFXMain and
    psndENVGetVol apply), `lockTimer` (refuses five effect ids for 6 frames).
  - Left: SoundEnvList 0x5 and PaperSoundEnv 0x7 (copied, never read), SoundEffectEntry 0xE (1 sets the slot's 0x200
    flag, which exempts it from psnd.flag 0x80's mute; the meaning of either flag is not proven), PaperSoundBGM 0xC,
    PaperSoundEffect 0x16 (only stored), PaperSoundWork 0x5B (only stored).
- **win/win_main.h.** WindowWork: 0x128-0x14C winMsgEntry/winMsgMain/winMsgDisp's message window (`msgX`/`msgY`,
  `msgState`, `msgStep`, `msgItemId` (> 0 an itemDataTable icon; -2 moves the name box), `msgTag`/`nameTag`/
  `lastMsgTag` (msgSearch's tag), `msgLine`/`msgLines`); 0x164-0x178 and 0x184 winSortEntry/winSortMain's menu
  (`sortX/Y`, `sortTargetX/Y`, `sortSel`, `sortCount`, `sortKind` = sort_data index); 0x19C `partyChange` (win_party
  sets it when A closes the menu; win_root returns -2 if partySel moved); 0x224/0x22C `itemScrollPos/Target[2]`
  (190 * itemScrollRow); 0x234-0x240 the Luigi book window (winBookGX) and 0x244-0x250 the mail window (winMailGX), each
  position easing to its target; 0x260/0x264 `mailPagePos/Target` (mailPage * 200); 0x26E `mailList` (the byte pairs
  win_root sorts); 0x2C8-0x2D0 `bookId`/`bookPages`/`bookPage` (luigi_book_list); 0x1050 `logChars` (LogWin.chars).
  WinEquipRec `equipped`/`badgeIdx`/`badgeId` (win_badge's toggle and pouchEquipBadgeIndex, the sorts' itemDataTable
  index). WinRootRec's `a..e` are `tabId` (winRootDisp's switch), `x`/`y` (every submenu Disp), `state`, `step`.
- **Outside the three headers, same rules.** window/win_log.h LogWin's invented `e28`..`fb0` placeholders: the map,
  crystal-star ring, card and badge windows' positions and targets (win_log's Disp draws at them, its Main eases them),
  the ring's alpha and angle (360/7 per stoneSel), the card/badge list scrolls (page * 0xE8; win_root's book pages read
  them) and picture frames. The tattle window's six floats (0x1028-0x1034, 0x1048/0x104C) are only stored, so they are
  `field_0xNNNN` now instead of `x1028`-style names (+6 to the index, the honest spelling). evt_badgeshop.h 0x19
  `throw_table` (evt_badgeShop_throw_inc/dec add to it; the struct's siblings are `special_table` etc.); winmgr.h
  WinMgrEntry 0x40 is never touched: `pad_0x40`. SoundSongPlayCh's prototype names `(chan, songData)` as defined.
- **Deep-clean results.** psndSFXMain's `((u8*)&res)[N]` reads of calc3D's packed result are a local anonymous union
  `res.vol/pan/span` (EQUIV; names from the calls they feed and calc3D's PAN[0..2]). Tried and left: psndENVMain's four
  volatile reloads as `psndSFX_vol(env->sfxId, vol)` (the #1125 lead; MWCC drops the reload, 0x10 bytes shorter, 594-658
  lines) with or without the loop-top local; psndENVOff_f_d's `&psnd.envId[id & 0xF]` (20 lines); psndENVOn_f_d's
  `goto done` as `return 1` (23); psndBGMOn_f_d's gotos written out (39/43); sound.c's `goto unlock` as a duplicated
  unlock and return (29); SoundDropData's written-out second half as a second _ssDropSeq() (178). winmgr's regions are
  the ones #1110/#1114/#1125 measured (pool order, 300-894-line by-value diffs); not retried.
- **For a reviewer.** The two renames of existing names: BGMListEntry/PaperSoundBGM `reverb` -> `group` and
  SoundEnvList `flags` -> `sfxNo` (evidence above). The playPos/cachePos union: cache_aram's reset of both entries
  before playback is written as playPos. `msgItemId` carries the -2 sentinel. SoundEfxCheck/SoundEfxCheckPlay keep an
  unguarded doubled `flags & 0x10` test (an inlined voice lookup); guarding it would add a region.
- **Leads.** SoundEffectEntry.pitch is read only as `*(s16*)&entry->pitch` and the table holds 0xFAAB-style values:
  an s16 field with negative initialisers would drop the pun (a data-definition change). win_log.c's local
  WinLogEntry/WinLogWork overlay is LogWin's rows shifted by 4 bytes (its `field_0x1C` is rows[n+1].type): a duplicate
  struct to fold into LogRow.
## Battle header fields named from their consumers: battle_unit, battle, battle_audience, battle_ac, camera, stage (2026-10-24, Claude Opus 5.5, n9-q-fbattle)

Continues "BattleWork names", "BattleCommand names" and "battle_audience names". Every name below comes from the code
that reads the field (CLEANUP_GUIDE section 3), checked against every user in src/ and rels/ found by a libclang
member-reference index (both with and without MUST_MATCH; renames were applied at the indexed columns, macro arguments by
hand). Build byte-identical at every commit (build/G8MJ01/ok recreated); every C object in src/ and rels/ compiles
without -DMUST_MATCH (all 1,225, not only the changed ones, since the headers reach them all); fncast_lint and
local_prototypes pass; `tools/ci.py` passed before each commit with its tools/tests step skipped (the 9 worktree
environment failures recorded above) and its data step run as `uv run --with pyelftools tools/data_check.py` (running
ci.py itself under uv rewrites build.ninja with uv's python and ninja then tries to re-download the tools). This
worktree had no ratchet baseline, so `ci.py --init` ran first on a byte-identical build.

**battle_unit.h** (field -> name -> consumer):
- BattleUnitKindPart 0x30 -> `shadowSize`; BattleUnitPart 0x1A2 -> `shadowSize`: BtlUnit_Entry copies the kind's into the
  part; btlDispMain multiplies it by the unit/part x scales, mSizeMultiplier and shadowScale and passes it to shadowEntry's
  size. 0x32 and its copy 0x1A4 stay (never read).
- Part 0x1A6 -> `gravityFrame`, 0x1A8 -> `floatFrame`: gravityOffsetControl/floatOffsetControl count them, and
  getGravityDispOffset/getFloatDispOffset use them as the sine phase.
- Part 0x4F4 -> `dispColor` (GXColor): btlDispMain builds it from color and the kind's baseAlpha, draws translucent when its
  alpha < 0xFF, btlUnitPartsDisp copies it to evtColor. 0x4F8 -> `sortOffsetZ`: btlDispMain adds it to the world z it
  passes to dispCalcZ, nothing else (black_peach sets it). 0x4FC -> `rotationOrder`: btlUnitPartsDisp's switch picks the
  concat order of the x/y/z PSMTXRotRad matrices.
- BattleWorkHpGauge 0x0 -> `style` (the switch in BtlUnit_HpGaugeInit/Main and BattleDrawEnemyHPBar; only 0 is ever
  stored), 0x2 -> `flags` (bit 0: BtlUnit_HpGaugeMain clears the unit's flag 1, hiding the gauge, once dispTimer runs out),
  0x4/0x6 -> `drainTimer`/`drainFrames` (intplGetValue's step and steps for the bar animation), 0x8/0xA ->
  `dispTimer`/`dispFrames` (counted down by HpGaugeMain; BattleDrawEnemyHP fades the gauge over its last 0x14), 0xC/0x10 ->
  `fromHp`/`toHp` (HP before and after the change; the drain interpolates their difference), 0x14 -> `fillRatio` (the bar
  width btlDispTexPlane draws), 0x18 -> `drainRatio` (style 1's black quad width).
- BattleUnit 0x28 -> `turnOrder`: btlseqTurnFirstProcess rolls it from the kind's turnOrder/turnOrderVariance,
  battleSortPhaseMoveTable sorts by it. 0xB4 and BattleUnitKind 0x3C -> `heldItemBaseOffset`: copied by BtlUnit_Entry,
  added with heldItemOffset to the position btlUnitItemDisp draws the held item at (as talkTogeBaseOffset + togeOffset).
- BattleUnit 0x27C -> `hitCount` (BattleCheckDamage adds 1 per call, missed and superguarded attacks too; btlseqMove clears it; BattleCalculateDamage subtracts
  it under weapon special 0x20); 0x280 -> `damageDealt` (BattleDamageDirect adds the attacker's damage, btlseqAct clears it
  per act, _get_heart/flower_suitoru_point cap HP/FP Drain at it); 0x2A8 -> `attackEventFlags` (written with every
  attackEventCode by _btlcmd_SetAttackEvent and btlseqAct; btlseqAct's 0x6000006 tests bits 1/2/8/0x10 to decide how to
  start it, battle_disp's shell-shield follow tests 4); 0x2C8 -> `actEndEventId` (evtCheckID'd until actEndEventCode's
  run ends); 0x2CC -> `superguardEventCode` (BattleCheckDamage runs it on the attacker for superguardType 4, through
  BattleRunHitEventDirect).
- BattleUnitSetup 0x8 -> `seqPhase`, 0x18 -> `detectOffsetX` (BtlUnit_Entry copies them into the unit / each part).
- BattleWeapon 0x6D -> `damagePattern`: passed as BattleDamageDirect's 7th argument, which stores it in the target's
  damagePattern; that parameter and battle_damage's local are renamed to match.
- BattleWorkUnitPartBlur.color and BattleWorkUnitPartBlurEntry.blurColor: s8[8]/u8[8] -> `GXColor[2]` (newest, oldest
  tint). _partsBlurControl copied them as two `*(u32*)&` words; the GXColor copies give the same lwz/stw, so the puns go.
  btlevtcmd_SetPartsBlur stores the channels, btlUnitPartsBlurDisp reads them.
- Alignment gaps -> `pad_`: unit 0x27, 0x186, 0x25E; part 0xBE, 0x1AA, 0x4FD (no access anywhere, each between a
  narrower field and a 4-aligned one).

**battle.h:**
- BattleWorkField180C0: 0x4 `layer2Rotation` (mapObjRotate's angles for layer 2, _set_mobj_rotate_x), 0x10
  `layer2RotationApplied` (BattleStageObjectMain's last-applied copy), 0x20 `objectTurnEventId`, 0x24-0x30 ->
  `layerEventId[4]` (the four layerEvents[i] ids; constant indices, same code), 0x34 `nozzleData`, 0x38 `fallObjectData`,
  0x44 `nozzleWeapon[2]`, 0x1C4 `nozzleDir`, 0x1C7 `nozzleTargetDir`, 0x1CA `nozzleType`, 0x1CC `nozzleTurnEventId`, 0x1D8
  `nozzleWorkEventId`, 0x1DC `fogTurns` (BattleFogEndCheck, which btlseqTurnFirstProcess runs once per turn, counts it
  down: turns, not frames as the old comment said), 0x1E0 `fogEffect`, 0x1E4/0x1E8/0x1EC `closeWallEventId`/
  `fallFrameEventId`/`objectFallEventId` (each started from that event and polled with evtCheckID). 0x1D/0x1CB are
  alignment pads.
- BattleStageNozzleData `dirWeight`/`typeWeight`/`typeWeapon` (_Nozzle_Change_Check, _nozzle_type_init,
  _nozzle_work_attack_set); BattleStageFallObjectData `typeWeight`/`typeWeapon` (_object_fall_attack; 0x0 still has no
  reader). Announce (0x18C70) `x`/`y`/`message` (battle_message's _disp draws message at x, y).
- BattleWorkPartyInfo `sizeMultiplier`/`statusBlock`/`statusFlags`: BtlUnit_SetParamToPouch saves the partner's
  mSizeMultiplier, the mSleepTurns-mInstantKillStrength block and mStatusFlags; BtlUnit_SetParamFromPouch restores them.
- The deprecated BattleWorkCommand family is gone: the seven party `__makeTechMenuFunc` builders now take
  `BattleCommand*` (same layout, as the BattleCommand chapter said), `weaponTable[n].mEnabled` for the old `field_0x4`.
  BattleWorkActRecord had no user and went.

**battle_audience.h:** BattleAudienceMember 0xC is a union of its three users: `repeatCount` (KinopioJoyRaiseArm counts it
down, BattleAudienceJoy_Sub sets 2 or 3; KinopioBooing counts it down too but always finds 0, so it runs once), `enterDist` (KinopioEnter adds enterSpeed until it reaches
enterLength) and `intrudeDir` (+1/-1, the side KinopioIntrude walks to). 0x10/0x14 `enterSpeed`/`enterLength`
(KinopioEnter only), 0x18 `colorBStep` (KinopioSleep adds it to colorB), 0xEC `targetUnit` (KinopioTransEvt, the unit a
Boo flies at, from BattleAudienceDetectTargetAll), 0xF0 `eatTargetId` (KinopioEat, BattleAudienceDetectPakkunEatTarget's
member), 0xF4 `singCount` (KinopioSingSub sets 3, the phase reaction's step 15 counts it down for status 0xD). Alignment
pads at member 0x1E/0x45/0xF2, star 0x5D, sound 0x9/0x21. The commented-out copy of an old BattleAudience layout went.

**battle_ac.h:** BattleActions 0x4 `autoFlags` / 0x8 `autoTimer` (every ac_* main tests bit 0 together with the acting
unit's kConfusedAction: the AC then resolves itself without input, after autoTimer frames when bit 1 is set; both come
from BattleActionCommandSetup's arguments), 0x30 `endFlags` (bit 0 set on reaching the end state; the delete callbacks
force that state only while it is clear), 0x28C `gaugeSePitch` (BattleAcGaugeSeUpdate's value, psndSFX_pit of gaugeSe),
0x2BC `acWork` (the running AC's work area, cast to each ac_*/sac_* work struct; kept a byte array, as effect userdata
stays void*). BattleACDispParams 0x0 `dispFrame`; 0xC `gaugeColor`, a union of `s32 rgba` (each AC stores its constant as
a word) and `GXColor color` (BattleAcDrawGauge's flag 1 tint, the flashing g/b): ac_shot's two `(GXColor*)` casts,
ac_repeatedly(_lv)'s `u8* p = (u8*)disp; p[0xD]` and BattleAcDrawGauge's `((u8*)work)[0x1F2C..0x1F2F]` became member
accesses, same object.

**battle_camera.h, battle_stage.h:** BattleCameraWork 0x2C/0x44 `cameraPos`/`cameraAt` (battleCameraMain copies them into
the camera at its start and back at its end), 0x38/0x50 `prevCameraPos`/`prevCameraAt` (its start-of-frame copy, put back
per axis under flags 4/8/0x10), 0xFC/0x100 `shakeOffsetX/Y`; the `unkNN` move-to state (btl_camera_set_moveto's
intplGetValue mode, timer, length and from/to pos/at: `moveToMode` ... `moveToAtTo`) and shake state (`shakeXMode`,
`shakeXFrames`, `shakeXTimer`, `shakeXFrom`/`To` and the Y set). BattleStageLightSpline `knotCount`/`knots`/`coef`
(getSpline's count/knots and coefficient tables).

**Left, and why:** BattleUnit 0x258 (only cleared), 0x264/0x268/0x26E/0x26F (only accumulated or copied, never read),
0x30C (borodo coin count, only written), 0x1BC and part 0xDC-0xF8 (untouched, not alignment: the part's band mirrors the
unit's hop fields but nothing reads it); BattleUnitKind 0x14 (mario_get_renzoku_count_max's floor for the returned max:
lead `renzokuCount...` once the function is understood), 0x54/0x58 (one user, unit_party_vivian's _hammer_star_effect, as
x/z offsets), 0x8C (BattleCheckDamage's % chance that damage patterns 0xE/0x10 apply; those patterns lead to
subsetevt_shot_damage in battle_event_subset, a lead); StarPowerInfo 0x9/0xB (copied from weapon 0x19/0x1B, never read);
member 0xD8/0xE0, window 0x1-0x7 and 0x10; BattleACDispParams 0x4, 0x10, 0x13, 0x24 (actionCommandDisp's third argument,
the stage index - 1 in two ACs); BattleWorkField19068 0x4, field_180C0 0x0; BattleIcon 0x14/0x18 (only their product
reaches iconSetScale). BtlUnitSeMode sfxA/sfxB hold sound-name pointers as s32: retyping them moves the casts from the four
psndSFXOn_3D calls to the evt-value stores. battle_audience.c's `((s8*)setupBase)[4 + kind]` read of
NpcBattleInfo.audienceWeights stays: rewriting it through an NpcBattleInfo* broke the dol.

**Review:** a fresh Claude Opus 5.5 read of every new name against its users (read-only) found no wrong names, union
members or pads; it corrected five comments (hitCount counts resolved attacks, not landed hits; drainRatio is not 0 outside
style 1; the blur tint interpolates toward [1]; attackEventFlags has more setters and bit 0x4; repeatCount is 2-3 only for
raise-arm), all fixed.

**For a reviewer:** the `attackEventFlags` bits and `autoFlags` are named as words, not their bits. The 0xC union's reset
store in BattleAudience_ChangeStatus goes through `repeatCount` (all three are cleared together). `hitCount` is the
attacker's count, `field_0x26F` the target's copy of it. The data-table labels (`// field_0x30`, `// field_0x3C`,
`// field_0x6D`: 1,404 lines, matched by the label before them) follow the renames.

Counts (`tools/cleanup_index.py`, 35dc3b78 -> this branch, same script): index 16.972 -> 15.879 per kLOC (11857 -> 11092
weighted); field_0x 4624 -> 3879, raw_offset 177 -> 173, deref_cast 489 -> 485, prim_ptr_cast 1902 -> 1900,
struct_ptr_cast 1245 -> 1243. `field_0x` occurrences per header (grep, comments included): battle_unit.h 49 -> 17,
battle.h 47 -> 3, battle_audience.h 25 -> 6, battle_ac.h 17 -> 6, battle_camera.h 8 -> 1, battle_stage.h 4 -> 1,
battle_database_common.h 5 -> 4.
## Driver and Mario header fields named from their consumers; mapdrv.c deep-cleaned (2026-10-24, Claude Opus 5.5, n9-q-fmap)

Task (Isaac): name and type the `field_0xNN` members of include/driver/mapdrv.h, animdrv.h, include/mario/mario.h and
the other include/driver and include/mario headers under CLEANUP_GUIDE section 3 (name from the reader, one name per
meaning, evidence per name, placeholder when unsure), update every consumer, then deep-clean src/driver/mapdrv.c as
#1110/#1114 did. Parallel branches: fbattle (include/battle, src/battle) and fsound (sound, pmario_sound, win_main,
win_root, winmgr).

- **Method.** Untracked harness in build/fmap/: every object saved before the branch (base/, 1,230); b.sh (full build,
  BUILD-OK only when `build/G8MJ01/ok` is recreated), t.sh (one object, obj_equiv against base), fd.sh (one function's
  disassembly against base), errsites.py (rename a header's fields to a sentinel and compile every includer with
  `-maxerrors 200`, with and without MUST_MATCH, to list every consumer line), fixer.py (rewrite `->OLD` on exactly the
  lines the compiler rejects, repeated until none are left), nat.py (every includer of the changed headers compiled
  without MUST_MATCH; 999 sources at the end), gate.sh (all of that plus CI). CI ran through the tower's
  `~/ci_notests.py` (tools/tests skipped: test_wave_launch and test_reloc_value_check fail here for environment reasons;
  no tool changed); its baseline was `--init`ed from this tree's first build, byte-identical to 35dc3b78. Every commit:
  BUILD-OK, nat.py 0 failed, CI PASS, fncast_lint and local_prototypes clean. At the end `obj_equiv --tree` over all
  1,230 objects against the pre-branch ones: every one equivalent.
- **Counts** (`tools/cleanup_index.py`, 35dc3b78 -> this branch): index **16.972 -> 15.948** per kLOC (11857 -> 11141
  weighted): field_0x 4624 -> 3959, raw_offset 177 -> 160, prim_ptr_cast 1902 -> 1887, struct_ptr_cast 1245 -> 1247,
  deref_cast 489 -> 488, must_match 703 -> 701. Per header, `field_0x` sites (weighted): mapdrv.h 64 -> 28 (65 -> 29),
  animdrv.h 37 -> 11 (38 -> 12), mario.h 39 -> 22 (41 -> 24), npcdrv.h 8 -> 2, mobjdrv.h 5 -> 0, envdrv.h 6 -> 3,
  effdrv.h 5 -> 2, casedrv.h 2 -> 1, extdrv.h 2 -> 1; mario_pouch, mariost, seqdrv, msgdrv, icondrv, imgdrv, lightdrv
  and offscreendrv unchanged. src/driver/mapdrv.c 177 -> 53 weighted (field_0x 114 -> 10, prim_ptr_cast 13 -> 4,
  struct_ptr_cast 12 -> 1); other big drops: mot_hammer 164 -> 67, animdrv.c 116 -> 61, koopa 62 -> 40, mot_stay
  51 -> 22 (its MWS32 raw offsets), mario_balloon 22 -> 3.
- **mapdrv.h** (field -> name -> reader):
  - MapMatTexRef 0x0 -> `texture` (block: `name` mapMatSearchTex's key in mapSetMaterial/mapBuildTexture, `texFmtIdx`
    the texfmt_tbl index mapBuildTexture's switch writes, `width`/`height`/`data` copies of TEXHeader's); 0xA and
    MapMaterial 0xA -> `renderMode` (_mapEnt: 2 or 1 asks for MapObject render mode 2 or 1).
  - MapMaterial 0x8 -> `matSrc` (mapSetMaterial: 0 GX_SRC_REG, 1 GX_SRC_VTX for GXSetChanCtrl), 0x10C -> `blendColor`
    (mapSetMaterialLastStageBlend's color2, mapTestXLU, mapAnimAlphaObj), 0x110 -> `tevMode` (its first byte is
    mapSetMaterialTev's drawMode). MapMatTexMtx's array view -> `channels` (mapAnimColorObj).
  - MapJointDrawMode unk2 -> `flags`, unk4 -> `lightMask` (mapSetLight: bit i lights light_table entry i), unk8 ->
    `attributes` (hitdrv copies it to HitEntry.attributes). MapAnimChannel unk4/unk8 -> `inTangent`/`outTangent` (the
    Hermite tan1 of the later key and tan0 of the earlier one in mapCalcAnimMatrix and the mapAnim* steps).
  - MapAnimTableEntry 0x8 -> `duration` (mapMain wraps at it, mapCheckAnimation and aji_06 read it; SPM's field and
    its mapGetPlayDuration symbol agree), 0xC-0x1C -> `jointTracks`/`texMtxTracks`/`alphaTracks`/`lightPosTracks`/
    `lightColorTracks` (what mapMain hands each list to); MapAnimTable `count`/`entries`; MapAnimTexMtxItem 0x4 ->
    `texMtxIdx` (indexes MapMaterial.entries).
  - MapVtxArray 0x4 -> `data`; MapVtxFmt -> `posArray`, `nrmArray`, `clrArray[2]`, `texArrayCount`, `texArray[8]`
    (the GXSetArray calls in mapSetPolygon/mapSetPolygonVtxDesc; hitdrv's posArray read was a raw word). MapVtx and
    MapVtxList moved here from mapdrv.c: `posIdx`/`nrmIdx`/`clrIdx`/`texIdx` (mapSetPolygon sends them as GX_INDEX16
    POS, NRM, CLR0, TEXi in that order; hitdrv indexes posArray with posIdx), `count`; normalLists are MapVtxList*.
    PackedDisplayList's first word is the relocated list, `u8* data`.
  - MapEntry 0x170/0x174 -> `datAram`/`chunkBufAram` (bmapLoad's aramMgrToAram copies, bmapUnLoad's way back).
    MapObject unkAC -> `userMtx` (mapObjRotate/Translate/Scale; hitdrv, evt_door, evt_light, aji_13, dou_11).
    MapFileLight 0x3C/0x40 -> `flags`/`version` (_mapLoad passes flags to lightEntry only when version is 0x12D60D8).
    The texture and material name tables' `count`/`names`, `name`/`material`, `count`/`entries`.
  - New enums with a reader for every bit: MapEntryAnimFlags (ACTIVE, DONE, LOOP, PAUSE, RESTART, LV0-LV3 from
    gp->systemLv*Time, HIT_UPDATED from hitdrv) and MapEntryFlags (FOG, BLEND, BLEND2, VER1_02: the "ver1.02" packed
    format). MapEntry flag 0x8 (blend[2], set only by eff_majinai) stays hex.
- **animdrv.h**: AnimDataClip `loop`/`start`/`end` (animPoseMain wraps or clamps; the window and effect files read
  end/start), AnimFrame2 `value` (pose->evtValue), the delta records' `skip`/`dx`/`dy`/`dz`/`delta`, the node record's
  `startTanIdx`/`endTanIdx` (tanfTbl indices of animHermite's two tangents) and the texture record's
  `bindIdxDelta`/`transSDelta`/`transTDelta`; AnimData 0x44/0x50 -> `Vec bboxMin`/`bboxMax` (animPaperPoseDispSub's
  paper left/right from x, bottom/top from y; win_log copies all three components), replacing the ANIMDATA_F* macros;
  AnimationWork 0xD8/0xDC -> `s32 useTexAlpha[2]` (renderProc takes TEV stage 0 alpha from the raster when the paper
  texture that replaced the pose's has 0). mbUseFloatScratch BOOL and mFloatScratchRP/WP s32, the types their readers
  cast to (casts and a pun go).
- **mario.h (MarioWork)**: `kpaStageFlags` (0x14, kpaPreInit's 2D-stage bits; s32 removes koopa.c's MW_S32 macros),
  `frameCount` (0x34, bumped once per main step; marioUpdateBlurPos records on odd counts), `hammerSfxIdx` (0x3A),
  `dokanType` (0x3E, evt_mario_paper_pose_dokan's type; mot_dokan, partyDokanInit), `wallPushCount` (0x3F, 20 frames
  against a wall make it hitobjPush), `rubCount` (0x41, marioGetRub's count) and `chargeLevel` (0x42; both shared: mot_jabara's 0/2/3 and
  mot_hammer2's 1-3 pick the release), `stayTimer` (0x5A), the camera block from mario_cam and camdrv
  (`camShiftSideTarget/Cur`, `camShiftUpTarget/Cur`, `camZoomWeightTarget/Cur` eased at `camShiftRate`, the names
  marioResetCamShift/marioResetCamShiftRate reset; `camZoomWeight`/`camZoom`/`camZoomFocusOfs`, camdrv's zoom blend;
  `camZoomOffDelay`/`camZoomOffTimer`/`camZoomOffTime`; `camFollowPos`, camdrv's follow point; `prevCamPosX`),
  `jumpFromPos` (0x11C, takeoff position), `stepUpY` (0x1C4, marioChkFrontStep's step top for UPSTAIRS), `sideHitNpc`
  (0x1D8, NpcEntry* from npcCheckHitMarioSide), `hitobjEstimate` (0x200, mot_hammer's hitEstimate target),
  `balloonEff`/`balloonNpc` (0x260/0x264, typed: mario_balloon's casts go). Union members named by their motion:
  `kpaPoseCounter`, `planeTurnRate`, `hammerDir`, `jabaraDir`, `hammerSwingType`, `jumpRiseY`, `hammer2AnimTime`,
  `shipDispStepY`, `stayJabaraWait`, `hammer2Flags`, `stayAHoldTimer`, and `sleepTimer` (0x2B4: mot_stay and
  peach_stay count it, marioChkDeepSleep tests the 10 s cap; was `peachStayTimer`); mot_stay's MWS32 macro goes.
- **Other headers**: BattleGroupSetup `heldItemWeight`/`dropTableWeight`/`noDropWeight` (BattleInformation's irand
  bands), FieldBattleInfo `ruleKind`/`ruleCount` (BtlActRec_JudgeTurnRuleKeep, the rule messages), FieldBattleData
  `majinaiCoinBonus` (npcdrv's coin multiplier after Merlee's coin curse); EnvironmentWork `viewMtx`/`modelMtx`/
  `modelViewMtx` and EnvironmentYami.unk4 -> `mtx` (MapTevCallback.mtx at the same place); MapObjectWork's five hit
  copies by the hits they copy; EffectMayaAnim `frame`, EffectMayaAnimData `duration`, EffectMayaAnimEntry `alpha`;
  CaseEntry `evtId`; ExtMakeTextureEntry `frame` (the pose frame animPoseDisp_MakeExtTexture draws; the old comment
  said scale).
- **mapdrv.c.** spline_maketable/spline_getvalue take `Vec* points, f32* knots, Vec* tangents` (EQUIV; getvalue indexes
  Vecs), so mapdrv's 13 table casts go and himo's two MUST_MATCH regions with them (its s32 table locals existed only
  for the s32 prototype); evt_sub's SplineWork and the barriern/batten satellite works are typed to match (barriern's
  `points` member was the knot table). _mapDispMapGrp_NoMaterial and the bbox callbacks take a CameraId (the
  self-casts go), mapObjGetPosSub a BOOL, mapCalcAnimMatrix a MapAnimData*, mapSetMaterial a MapMaterial*.
  Register-named locals (list19..15, f1/f5/f24/f25) are descriptive and mapSetTextureMatrix's copies go; GX and
  CAMERA_* literals are enums; stale match-era comments, the OBJ_FLAGS macro and NULL-as-0 go.
- **Left, and why.**
  - mapdrv.c: `entry + 0x178` (the tpl ARAM copy lands in the next entry's `count`; retail's entry pointer is the work
    base, so a count-first MapWork doesn't fit), _mapDispMapObj's `p + 0x10/0x14` packed walk (the indexed form swaps
    r24/r25 in every declaration order tried, a typed pointer adds `addi 16`), the five `flag = !flag` regions, the
    material search's `ofs`, case 8's `wave`, the stripped-function placeholder, the three pragma pairs, the
    spline_fpclassifyf/sqrtf copies (a tree-wide family).
  - mapdrv.h: MapMatTexMtx 0x14/0x18 and MapAnimTexMtxItem 0x8/0xC (only the rotation centre `0.5 * a * scaleX`,
    `(1 - 0.5 * b) * scaleY` reads them), MapError (written by mapErrorEntry, never read), MapMaterial 0x0/0x9,
    MapHeader 0x3C (only `|= 1`), MapFileLight 0x20, MapJointDrawMode unk0 (render mode request, one off from
    MapObject.renderMode), MapFileAnimEntry (no user).
  - animdrv: the `(u32)` on disableDraw/typeFlags `== 0` tests (the plain test differs and this task adds no guard);
    endTanIdx stays u8 read through `(s8)` (as s8 it moves a register); paperAnim's name-or-index convention.
  - MarioWork: 0x8 (one bit, set by mot_roll, waited on by evt_mario), 0x20/0x58/0x18C (stores only), 0xE0, 0x128
    (landing distance in evt_mario/mot_ship, rise sum in party_yoshi), 0x164, unk60, 0x2A2, 0x2AC and the party_yoshi/
    mario_motion/mot_dokan uses of 0x2A4/0x2B0/0x2B8, mot_hammer's M_S32(0x2A0)/M_S16(0x2A4) and mot_jabara's MW_S32
    zero stores (an s32 member only for zero stores was rejected in n9-q-o-worst3).
  - SequenceWork 0x10-0x1C (each sequence its own way), PouchData 0x7E/0x80 (zeroed only), LightEntry 0x3C (written
    only), FieldBattleInfo 0x1B, the gap arrays.
- **For a reviewer.** Files outside this task's set changed only where a renamed field's reader lives: battle_actrecord,
  battle_information, battle_seq (FieldBattleInfo/BattleGroupSetup) are fbattle's, win_root (one `clip->end`) is
  fsound's; expect small merge conflicts there. AnimData's z components and BLEND2/VER1_02 are the least direct
  evidence (win_log's full-Vec copy; the original's mapSetBlend2 and the "ver1.02" string). `rubCount` and `chargeLevel` carry
  one meaning in both mot_jabara and mot_hammer2, so neither name has a motion prefix. The spline prototypes, mapObjGetPosSub's BOOL and the
  callbacks' CameraId change existing header declarations (the n9-q-o-proto precedent); the barriern tables stay u32
  data, so their stores take a `(Vec*)` cast (barriern v2's struct_ptr_cast 5 -> 17 while its field_0x goes 27 -> 0).

## rels/common placeholder fields: the unit-local structs named, the shared-header labels left (2026-10-24, Claude Opus 5.5, n9-q-fcommon)

Task (Isaac): name and type the `field_0xNN` members used in rels/common/** (about 1,060 by grep) from their consumers in
the same unit files (CLEANUP_GUIDE section 3), touching include/battle/*.h only where a consumer here proves a field
#1134 left. Parallel branches: fmap (include/driver, include/mario; not touched), feff (src/effect; not touched).

- **What the 1,008 were** (by grep). 987 are the `// field_0xNN` labels on data-table initialisers in the unit
  files, naming shared-header members: BattleUnitKind 0x14/0x54/0x58/0x8C (66-67 each), BattleWeapon
  0x14/0x19/0x1B/0x6F (136 each) and BattleUnitKindPart 0x32 (175). The index ignores comments, so it saw only 74
  sites in rels/common and include/rel/common together: 73 in the unit-local structs below and borodo's write.
- **PidersYarnWork** (include/rel/common/unit_piders.h; users: the shared piders_draw_yarn_sub.h (v1, jon's
  churantalar) and the v2/v3 copies). Six f32 -> two Vecs: 0x0 `top` (piders_draw_yarn: the unit's home x/z with y
  set to 400.0f), 0xC `bottom` (home x/z with y = unit work 2, which every writer in the four units sets to 0 or eases
  from 400 to 0 in entry_event), 0x18 `alpha` (piders_draw_yarn's argument; piders_draw_yarn_sub and v2's
  piders_getMatColor put it in the GXSetChanMatColor alpha). piders_draw_yarn_sub draws the quad between the two ends.
  The header now includes <dolphin/mtx.h> for Vec.
- **BarriernSatelliteTo** (include/rel/common/unit_barriern.h; users: barriernSatelliteTo_Init/_Main.h, v1 and v2
  unit_barriern.c). The map/driver headers branch (#1135) already named and typed 0x4 `Vec* points`, 0x8 `f32 knots[5]`
  and 0x1C `Vec tangents[5]` from spline_maketable/spline_getvalue; this branch names the last placeholder, 0x58
  `field_58` -> `frame` (Init clears it, Main passes it to intplGetValue(4, frame, 0x3C, 0, 1), increments it and
  frees the work at 0x3C).
- **BarriernSatelliteToData** went: it was a second overlay of the syuryou effect's userdata (effSyuryouEntry's effect,
  kept in unit work 3), which include/eff/eff_syuryou.h already models as EffSyuryouData (pad 0x0, position 0x4, scale
  0x10). The three writers (barriern_wawawa_set_target.h, v1, v2) use `data->position.x` etc.; its invented `field_0`
  placeholder goes with it. barriern_wawawa_set_target.h includes eff/eff_syuryou.h itself now.
- **unit_hammer_bros.c:** the HammerBrosSamplingEntry/HammerBrosSampling typedefs had no user (the target helpers use
  battle_detect.h's BattleSampling) and went.
- **Left, and why.** The shared labels: nothing in rels/common reads those members (the units only initialise them),
  and the readers elsewhere are the ones #1134 recorded: BattleUnitKind 0x14 (mario_get_renzoku_count_max: the
  returned max is at least this, 9999 in 66 tables and 9 in red_honenoko; a lead for a `renzoku...` name once the
  function is understood), 0x54/0x58 (unit_party_vivian only; 0.0f everywhere here), 0x8C (BattleCheckDamage's chance
  for damage patterns 0xE/0x10; 100 everywhere but super_mahorn's two kinds, 0); BattleWeapon 0x19/0x1B (copied to
  BattleWork.impendingBonuses, a StarPowerInfo, never read), 0x14 and 0x6F (no reader anywhere in src/ or rels/); BattleUnitKindPart
  0x32 (copied to part 0x1A4, never read). BattleUnit 0x30C: borodo (and jon's badge_borodo, tou2's borodo_king) only
  write it. The struct casts off `unit->work[N]` (BarriernSatelliteTo*, EffectEntry*, Vec*) stay: work is BattleUnit's
  shared s32 array. lecture_button_disp's `(u8*)b + 8` base stays under MUST_MATCH (no field at 0x8 is proven).
- **Checks.** Each commit: `build/G8MJ01/ok` recreated, `obj_equiv --tree` over every object in build/G8MJ01/src
  against a snapshot taken before the branch (all equivalent), and `tools/ci.py` PASS with its tools/tests step stubbed
  (the 9 worktree environment errors recorded above); this worktree had no ratchet baseline, so `ci.py --init` ran
  first on the byte-identical build. The 15 objects that include the changed headers (piders in jon/mri/tou2,
  churantalar, the barriern/barriern_z/satellite/custom units in aji/moo/mri, hammer_bros in tik/tou2) built under
  `configure.py --non-matching` with no errors or warnings, then a normal reconfigure and rebuild to `ok`.
  fncast_lint and local_prototypes clean.
- **Counts** (after the rebase onto #1135). `field_0x` by grep: rels/common 1008 -> 988 (987 data labels + borodo's
  write), include/rel/common 17 -> 0. `tools/cleanup_index.py` (origin/main -> this branch, same script): index
  13.773 -> 13.669 per kLOC (9621 -> 9548 weighted); field_0x 2476 -> 2403, everything else unchanged.
  rels/common + include/rel/common: 168 -> 95 weighted, field_0x 74 -> 1.
- **For a reviewer.** `top`/`bottom` rest on the y values every writer gives (400.0f vs. unit work 2 in 0..400).

## Effect work-struct fields named from their consumers (2026-10-24, Claude Opus 5.5 orchestrating eight Claude Opus 5.5 workers, n9-q-feff)

Task (Isaac): name and type the `field_0xNN` members of the effect work structs in src/effect/** and include/eff/*.h
from the Main/Disp code that reads them (CLEANUP_GUIDE section 3), remove the casts the new types make unnecessary,
leave unused or write-only fields as placeholders. include/driver/*.h and include/mario/*.h were out of bounds (fmap's);
fcommon ran in parallel on rels/common. Data layout was not touched.

- **Method.** 103 effect units (each eff_*.c with its header; 1,054 `field_0x` occurrences) were split into eight
  groups of disjoint files, one Claude Opus 5.5 worker each, all editing this worktree. Untracked harness in build/feff/:
  try.py (fsound's, pointed at a snapshot of all 2,461 objects taken before the branch) compiles a .c, or every user of
  a .h, into a temp dir and runs obj_equiv against the snapshot (`--nat` without MUST_MATCH); vecify.py turns an x/y/z
  run into a `Vec` member and rewrites the accesses, putting back the lines the compiler rejects; ci.sh/ci_local.py run
  tools/ci.py with a pyelftools venv on PYTHONPATH and accept only the test_wave_launch.PassPipes environment errors.
  Every worker checked each header and its users EQUIV and NAT OK; the orchestrator reviewed each report and diff,
  then a fresh Claude Opus 5.5 reviewer read every new name, pad and union against its readers (read-only). It found
  one problem (snow_dust 0x20/0x2C left as fields though nothing touches them: now pads) and two comments to sharpen
  (toiki); everything else it confirmed, including all the corrected existing names. Final state: `obj_equiv --tree`
  over all 2,461 objects against the snapshot, every one equivalent; the 193 objects built from a changed .c or a user
  of a changed header compile under `configure.py --non-matching` (the two headers the review touched afterwards, snow_dust and toiki, were rechecked with try.py `--nat` over all their users); fncast_lint and local_prototypes clean. Each of the
  nine commits was checked out on its own and passed `tools/ci.py --base <parent>` with `build/G8MJ01/ok` recreated.
- **Counts** (`tools/cleanup_index.py`, origin/main 5619c3bf -> this branch rebased onto it, same script): index
  **13.669 -> 12.666** per kLOC (9548 -> 8847 weighted); field_0x 2403 -> 1721, struct_ptr_cast 1245 -> 1226 (the (Vec*)
  casts); nothing else changed. `field_0x` in the index (code only): src/effect 794 -> 260, include/eff 249 -> 102; by grep (comments
  included) src/effect 801 -> 261, include/eff 249 -> 103.

**Names, per struct** (field -> name: the consumer that proves it; file is the effect's .c unless named).
- EffSheepWork (eff_sheep.h): 0x2C velX (Main `x += velX` until x reaches 500), 0x18 velY (added to y, minus gravity,
  negated at y = 0), 0x1C gravity (`velY -= gravity`), 0x14 hopFrames (Entry's only read: `velY = gravity * hopFrames
  / 2`, the launch speed that lands after hopFrames = distance / velX frames).
- EffPuniBalloonData: 0x1C baseScale (balloon_0 multiplies the PSMTXScale factor by it), 0x24 rotZ (PSMTXRotRad 'z').
- effGonbabaBreathData 0x44 delay (Main skips the particle and Disp doesn't draw it while it counts down).
- ParticleEmitter/ParticleWork (eff_particle.h): 0x54 emitTotal (EmitterEmit: its integer part's change is the
  number emitted this frame), 0x58 emitRate (added to emitTotal), 0x5C/0x60 lifeBase/lifeSpread (ParticleEmit's
  `life = lifeSpread * gaussian + lifeBase`), 0xAC particleSpeed (PSVECScale of each new velocity); `head` ->
  numLive (EmitterEmit adds and caps it, effParticleMain decrements it when a particle dies: a count, not a list head).
- EffStarPowerData: x/y/z -> Vec position; 0x14 counter (drives the fade-in and the type-1 sines), 0x30 color2B
  (TEVREG1 blue beside color2G/A), 0x38 scale and 0x3C-0x44 rotX/Y/Z (Disp's PSMTXScale(0.4 * scale), PSMTXRotRad),
  0x48-0x50 prevRotX/Y/Z and 0x54 prevScale (the trailing half-alpha star's matrices; Main copies last frame's values).
- effWangFlushData (local): 0x28 pulseAlpha (type 0 quad's TEVREG0 alpha, 144 + 64 sin), `alphaScale` -> pulseScale
  (only ever PSMTXScale's argument).
- effStageClearWork: x/y/z -> Vec position; 0x1C age (++ per frame: the head's value scrolls the rainbow texture
  matrix, each piece's indexes scale_dt); 0x10 pad.
- ToikiData: 0x24 texOffsetX (PSMTXTrans -> GXLoadTexMtxImm(TEXMTX0); also the clock that gates the mist puffs),
  0x28 alpha (TEVREG0 alpha; the stage takes alpha from TEXA * RASA, so the fade is never visible: commented).
- effOpukuWcurtainData: x/y/z -> Vec position; 0x18-0x24 colorR/G/B/alpha (GXSetTevColor TEVREG0; Main fades the
  alpha in and out), 0x2C bottomY[12] (Disp's bottom vertex `y = 10 * (60 + bottomY)` and its t coordinate), 0x5C
  bottomVelY[12] (added to bottomY, -0.05 per frame, damped above 5).
- PeachDustParticle 0x28 frame (Disp: `frame & 7` picks the texture slice, < 0 hides the particle).
- EffRadiationData, effMagicHoraoData: no names (see Left); magic_horao x/y/z -> Vec position.
- MistWork: 0x28 riseSpeed (type 4: `y += riseSpeed`, besides velY), 0x2C riseAccel (added to riseSpeed), 0x30
  riseAccelDecay (riseAccel *= it, as kemuri1's velYAccelDecay; from 0.95 / -0.04 / 0.94 the speed stays positive), 0x40 yaw (sin/cos of it aim
  velX/velZ; the Entry argument minus viewYaw; Entry's `w` -> yaw).
- effNanigaData: posX/Y/Z -> Vec pos; 0x10 screenPos (the CAMERA_2D reel's translation and scissor centre, projected
  in Entry), 0x34 scroll (icon i at `scroll - 40 * i`; eased and wrapped by 40 * count), `color` -> alpha (only the
  pose material alpha).
- effSandarsData 0x34 delay (counts to 0 before a cone starts). EffShockData: 0x14 type1Alpha (alpha of type 1's fixed
  yellow TEVREG0), 0x18 tev1A (TEVREG0 alpha beside tev1R/G/B; `tev1` is GXSetTevColor's id 1, GX_TEVREG0), 0x1C life, 0x20 age (compared with 20 and 6), 0x24
  scale (PSMTXScale(0.25 * scale)), 0x2C offsetY (PSMTXTrans(0, offsetY, 0)).
- effPacknBfireData (local): `vx/vy/vz` -> targetX/Y/Z (Entry's `vel = (target - pos) / a1`; passed on to the child
  flames as their target).
- effBomheiData: x/y/z -> Vec position; 0x10 scale (PSMTXScale), 0x14 life (delete below 0), 0x18 age and 0x1C
  lifetime (`animFrame = 9 * age / lifetime`), 0x20 animFrame (integer part the texture row, fraction the TEVREG1 blend
  to the next row), 0x24 rotZ, 0x28 rotZVel.
- effPokopiPchargeData 0x38: union { velX (particles: `x += velX`), offsetX (root: added to every particle's x in
  Disp's PSMTXTrans) }.
- EffPokopiThunderData: 0x10-0x18 startX/Y/Z and 0x1C-0x24 endX/Y/Z (Disp's bolt points `start + i/11 * (end -
  start)`), 0x4C scale (only read as the strip half-width `8 * scale * ptScale[i]`), 0x110 phase (Disp's wobble index).
- effKemuri4Data textureIndex u8 -> s8 (read only as `(s8)`; cast gone). effSnowDustData: Entry's a6/a7 -> spreadX/Y.
- CoinFukidashiData: 0x10 offsetY (added to y for the coin and number), 0x14 coinNum (sprintf "%d", FontDrawStringMtx),
  0x20 life, 0x24 offsetIdx (offset_tbl index, wraps at 61).
- effMachinegunData: `scale` (0x18) -> speed (`position += speed * cos/sin(angle)`, *= 0.9; never reaches a scale call),
  0x10 -> scale (Disp's PSMTXScale); Entry's parameters follow (the one caller passes positional floats).
- effTorchData: 0x10 scale, 0x14/0x18 scaleX/scaleY (PSMTXScale(scale * scaleX, scale * scaleY, scale)).
- effBalloonN64Data: x/y/z -> Vec position; 0x10 rotZ, 0x14 targetRadius and 0x18 radius (`radius += 0.1 * (target -
  radius)`, the ring points' radius), 0x1C life, 0x20 phase (the wobble sines' phase; wraps 324000 -> 256), 0x24 segs.
- effBlowData 0x58 alpha (u32 -> s32: TEVREG0 alpha, effBlowParticle's alpha at the same offset). effCoinData (local)
  0x1C scale. effHagetakaFlushData: 0x10 life, 0x14 frame (scale and the color_rotation_data index), 0x24 alpha
  (TEVREG0), 0x28 baseScale and 0x2C scale (their product is PSMTXScale's; 0x2C grows and halves at the end).
- effMiniGameWork: posX/Y/Z -> Vec pos.
- ChargeEffectData 0x3C-0x44 color2R/G/B (TEVREG1, beside colorR/G/B in TEVREG0). DamageStarData: 0x24 timer
  (deletes below 0), 0x3C damage (sprintf "%d" and effNumberGX: the number on the star), 0x40 prevRetraceCount (s32
  -> u32; `retraceCount - prevRetraceCount` is the frames to simulate; the `(u32)` cast gone).
- effKemuri1Data (local), the three fields #984 left because each has two uses: 0x14 union { speedAccel (types 0/1:
  `speed += speedAccel`), speedDecay (types 4-7: `speed *= speedDecay`) }, 0x28 union { velYJerk (types 0/1: added to
  velYAccel), velYAccelDecay (types 3-7: velYAccel *= it) }, 0x2C union { velYSnap (types 0/1: added to velYJerk),
  velYDecay (types 4-7: velY *= it) }; the file already had speedJerk/speedSnap.
- effKemuri3Data (local, `field_NN` placeholders): 0x06 life (`--life <= 0` deactivates), 0x08 alpha (TEVREG0 alpha,
  A0 * TEXA), 0x18-0x20 scaleX/Y/Z, 0x24-0x2C rotX/Y/Z, 0x30 modelMtx (PSMTXConcat's result, Disp's position matrix),
  0x60 speed, 0x70 velY, 0x74 velYAccel, 0x78 velYJerk, 0x7C velYSnap, 0x80/0x84 dirX/dirZ (kemuri1's vocabulary; same
  update chain); 0x4 pad.
- EffOpukuWgunData 0x34-0x40 color1R/G/B/A (TEVREG0) and 0x44-0x4C color2R/G/B (TEVREG1), as opuku_jetw.
- SplashData `offsetX/offsetY` -> velX/velZ (Main adds them to position.x and .z). effRappaData `velX/Y/Z` -> dirX/Y/Z
  (a unit vector from sin/cos that Main scales by speed). RecoverHeartData: x/y/z -> Vec position; `scale` -> offsetX
  (only Disp's `position.x + offsetX`, 8 cos(phase)).
- HibashiraData (header and segments share it): 0x18 alpha (animPoseSetMaterialEvtColor alpha, segment * header /
  255), 0x1C lifetime (header countdown, 1000 holds, deletes below 0), 0x20 age (segment frame counter, fades past 5),
  0x24 delay (the segment's pose isn't created until it reaches 0). Header prototype a/b/c -> type/segmentCount/lifetime.
- EffPeachFukidashiData 0x18 rotZ. EffHolographyData: 0x10 lifetime, 0x14 frame (the vertex table and xlu_data
  index), 0x18-0x20 colorR/G/B (TEVREG0), 0x24 alpha, 0x28 scale (PSMTXScale(-scale, scale, scale)).
- effFireData `colorIndex1/2` -> frame/nextFrame (the TEXMTX0/TEXMTX1 offsets of the texture strip, wrapped at
  frameMax; no colour involved). SyuryouData (local) `color` -> alpha (only the material alpha). effOpukuWprotectData
  x/y/z -> Vec position.
- effLasMonData: 0x18 state (Main's switch), 0x1C alpha (material alpha), 0x20 life. MajinaiWork 0x10 scale
  (PSMTXScale; npcdrv.c sets it to 0.8). MobjBrokenWork: 0x18 rotAngle (PSMTXRotAxisRad, degrees), 0x1C dir (the
  travel angle in Main and the tumble axis (cos, 0, sin) in Disp: one angle), 0x20 speed, 0x24 velY, 0x28 gravity.
- effSleepData: 0x14 alpha (GXSetChanMatColor alpha), 0x18 lifetime (the wrong `time?` comment went), 0x1C age, 0x20
  delay. effFreezeData 0x24 frame / 0x25 frameTimer (u8 -> s8; four casts gone). effBombN64Data x/y/z -> Vec pos.
- Outside the groups (only `Vec` retypes): effMagic2Data `vx/vy/vz` -> Vec worldPos (what Main projects through the 3D
  camera) and x/y/z -> Vec position (the screen translation Disp uses); effMagic3Data, effSnowData x/y/z, effBreathFireData
  posX/Y/Z -> Vec; effOpukuJetwrainData x/y/z -> Vec position and height u8 -> s8[19][13] (read only through `(s8)`).
  Every `(Vec*)&data->x`-style cast in src/effect is gone (19 removed lines).
- Pads (no access by name or by offset off the work pointer in the asm, checked by the workers and the reviewer):
  nice, stageclear, whirlwind, batten, kemutest, nozle, packn_bfire, sweat, kemuri10 (indices 3-4 of `field_0x67[5]`),
  expbom, fire smoke, syuryou, kemuri6, queen2, splash, starpoint, rappa, kemuri3, kiss, akari_charge, bush, kemuri5,
  snow_dust.
- Parameters renamed where they only feed a now-named field (prototypes kept in step): kemuri2_n64, coin fukidashi,
  machinegun, balloon, bomhei, mist, holography, charge, kemuri1, scanning, recover_heart, akari_charge, kemuri5,
  snow_dust, hibashira.

**Left, and why.** 145 `field_0xNN` declarations stay in src/effect and include/eff (309 before), each only cleared, only written, or only feeding itself (a counter
nothing reads); most now carry a one-line comment saying so. Among them: radiation 0x1C/0x20 (Entry's a5 twice) and
0x34-0x54 (types >= 2 add 0x48.. into 0x34.. and wrap to [0, 256), nothing reads them); fire 0x4C-0x64/0x80/0x84;
flower 0x3C/0x40/0x44 (type 1 swings 0x40 between 0 and 20, nothing reads it); opuku_wprotect 0x30/0x34 (computed
each frame, Disp ignores them); hit_n64, kemuri8, pokopi_pipo, rubble, release, broken_barrier, fire_dust, stardust;
eff_kemuri.h's untouched byte runs (`u8 field_0x14[...]` etc. in effKemuri2Data/effKemuriData: no named access, but
not checked against the asm, so not made pads).

**For a reviewer.** The existing names corrected (machinegun scale/speed swap, fire frame/nextFrame, syuryou and naniga
alpha, packn_bfire target, splash velX/velZ, rappa dir, recover_heart offsetX, wangflush pulseScale, particle numLive)
and the unions (kemuri1, pokopi_pcharge). Borderline but kept: sheep `hopFrames` (named from Entry's projectile formula,
its only reader), mist `rise*` (from the sign of the chain), shock `type1Alpha`. One consumer edit outside src/effect:
src/driver/npcdrv.c's `work->scale = 0.8f` (MajinaiWork); fmap may touch that file. Leads: effGlassData `angle`/
`angleVel` is a 0..1 animation phase (Disp uses `4 * angle` as the sheet frame; renaming it touches eff_freeze);
effBlowData duplicates effBlowParticle; kemuri2's `active` is written once and never read; SstarVtx keeps `s16 unk[4]`.
## Motion and manager fields named from their consumers: MotionPlaneData, MarioWork's motion fields, EventEntry, JabaraWork, MotionShipData, CardWork (2026-10-25, Claude Opus 5.5, n9-q-fmotion)

Task (Isaac's prompt): name and type `field_0xNN` in src/motion/**, src/manager/** and their include/motion,
include/mot, include/manager headers from their consumers, under CLEANUP_GUIDE section 3; MarioWork only where a motion
file proves a remaining placeholder; remove the casts the new types make unnecessary. Parallel branch: fbtlsrc
(src/battle/*.c), so nothing here touches src/battle.

- **MotionPlaneData** (include/mot/mot_plane.h; mot_plane.c is its only user). 0x10 `speedAccel` (flyMain adds it to
  baseSpeed in the `-80 <= pitch < 0` branch, its only read), 0x14 `pitchSpeed` (flyMain: `pitch += it` in both pitch
  branches; integrated there, and in the `-80 <= pitch < 0` branch capped at 1 while the stick is pushed one way, by flags & 1), 0x18 `velY` (flyMain: `position.y += it` each frame;
  the held stick drives it to planeData[19], else it decays; the `pitch >= 0` branch also adds it, or its step, to pitch), 0x24 `pitchMin` (flyMain: `if (pitch <= it) pitch = it`;
  -22 over attr-0x38 ground, else eased to planeData[4]), 0x30 `hitRearPos` and 0x3C `hitFrontPos` (chkCollision: the
  origins of its marioHitCheck rays; 0x30 is planeData[40] = 28 back along the flight axis and planeData[39] = 6 down
  (bank-rotated) from the display centre, 0x3C 4 below it along the pitched down axis, and the segment from 0x30 to 0x3C goes to marioHitCheckVec). 0x1C stays `field_0x1C`: it is a second term
  added to pitch (set non-positive from the speed shortfall, relaxed to 0 while the stick is held) and nothing tells it
  apart from pitchSpeed by name. 0x2C (u8[4]) has no user and keeps its placeholder. chkCollision's local `normal` was
  marioHitCheckVec's third argument, which is the hit position (mario_hit.c copies `query.hitPos` there), so it is
  `hitPos`.
- **MarioWork** (include/mario/mario.h), the fields a motion file proves:
  - 0xE0 `fallReturnPos` (Vec): its readers put Mario back there after a fall: mot_bottomless (mario_motion.c) lands on
    its x/z at y+300 and treats y < -2000 as "none" (marioReInit stores y = -10000; a per-map fallback follows), and
    mot_damage uses it as the return target in bg mode in place of hitGetDamageReturnPos. mot_dokan stores position
    there at the end of each pipe exit.
  - 0x58 `lowHpPoseTimer` (s16): mot_stay only. Counts frames of the M_S_3 pose it picks when pouchGetHP() <= 5, wraps
    past 0xF0, and plays sounds 0xE1 at 15 and 0xE2 at 135.
  - 0x2A0 union: `hammerHitFrame` (s32, mot_hammer): `poseTime = min(multiTimer, it)`, and the impact (quake event,
    hitobjHammer) runs when multiTimer equals it; set from hitEstimate's type (7 counts as 3); 10 replaces it when flag
    0x800 is clear. Replaces the M_S32 raw accessor.
  - 0x2AC union: `planeEndY` (mot_plane: the exits put position.y at 1 + it, or 1 + it - 37 after a head hit, and the
    water sink runs until position.y < it - 100; it holds chkCollision's hit y, or position.y on a cancel or stall) and
    `damageVelY` (mot_damage and mot_damageToge: added to position.y each frame of the throw out of the damage floor,
    starting at 20 or 4, minus 1 a frame, at least 10). The f32 `field_0x2AC` member stays for mot_bottomless, whose
    movePos speed it is.
  - 0x2B4: an f32 `field_0x2B4` member replaces mot_jump's MWF32 raw store (the x of the map object jumped from; the z
    goes to field_0x2B8). Neither store is read back (party_yoshi reads its own f32 at 0x2B4 raw), and mario.c reads 0x2B8 as a z rotation in the yoshi motion, so
    both stay placeholders. mot_hammer2's s16 view of 0x2A4 (M_S16) stays raw: it is set to 0, then 10 every frame,
    and compared with 1, which gives no meaning.
  - Comments only, placeholders kept: 0x8 (mot_roll stores 0x10 at a roll's start and 0 at its end; evt_mario waits for
    0 after marioRollCancel; renaming it to a sentinel showed these two files are its only users, but the bit's wider
    meaning isn't shown), 0x18C (a move speed stored by marioMove and the plane, ship, yoshi and koopa motions, cleared
    by itemdrv; no reader in the C), 0x128 (two meanings: the x/z jump distance from jumpFromPos that evt_mario and
    mot_ship store, never read, and party_yoshi's summed rise checked against 0.7 * marioGetJumpHeight()).
- **EventEntry** (include/manager/evtmgr.h). 0x184-0x18F was `EventEntryMsg unk184` (two f32) plus `u32 unk18C`;
  evt_msg_toge writes three floats there and _evt_msg_print copies them to `window->anchor` as one Vec (for toge types
  2 and 3, through a local `EventVec` overlay). Now `Vec msgAnchor`; the overlay, the `*(f32*)&` pun and the split
  copies in evtChildEntry, evtDelete, evt_inline_evt and evt_inline_evt_id go, and the whole-Vec copies compile to the same
  object. Left: 0x90 (user-function scratch with different meanings in unit_party_vivian and battle_stage_object; the
  latter is fbtlsrc's file), 0x194, 0x1AC and EventWork 0x94 (no access in the C or in evtmgr's asm).
- **JabaraWork** (local to mot_jabara.c). 0x4 `swingAccel` and 0x8 `swingDecel` (swingMain adds the first to
  jabaraSpeed while jabaraTimer runs, capped at 40, and the second, negative, once it is 0, until the speed reaches 0),
  0x10 `accelStep` (added to or taken from jabaraAccel), 0x14 `ofsStepFlips` and 0x18 `prevOfsStep` (state 0x32: the
  +-0.6 dispOfs step is compared with prevOfsStep; each change bumps ofsStepFlips, whose 1st, 2nd and 3rd play sounds
  0x189-0x18B), 0x1C `headHitTimer` (frames A has been released while marioSearchHead finds a ceiling; past
  sysMsec2Frame(5000) the launch goes ahead anyway). 0xC stays `unkC` (only zeroed). mario.h's jabaraTimer comment uses
  the new names.
- **MotionShipData** (local to mot_ship.c). 0x08 `bobAngle` (`dispOfs.y = 3 sin(it) - 13`; also picks the stopped
  wake's strength), 0x0C `bobSpeed` (taken from bobAngle each frame, 4.25), 0x10 `wakeSpeed` (shipWakeUpdate: eased to
  0.3/0.6 and passed as EffFunemizuData speed while the ship is stopped), 0x1C `turnRate` (eased, added to dispDir each
  frame, as planeTurnRate is), 0x20 `turnTarget` (the size turnRate eases toward, signed by flags & 1), 0x28 `swirlDir`
  and 0x2C `swirlSpeed` (marioSetSwirlPower's third and second arguments; shipMove passes them to movePos as the angle
  and the distance; yuu_01 passes 8.0f and a hoko_tbl angle), 0x3C `harborPos` (checkHarbor/shipHarborTarget's out
  point; the landing moves to it). marioSetSwirlPower's parameters are `speed, dir` now (header too). 0x04 (written,
  never read) and 0x34 (no user) keep their unk names.
- **CardWork, CardMetadata, CardSaveSlot** (include/manager/cardmgr.h; cardmgr.c is CardWork's only user). 0xE0
  `operation` (cardMain switches on it: 1 write_main from cardWrite/cardErase/cardCopy, 2 write_header_main, 3
  read_main, 4 read_all_main from cardReadAll, 5 create_main from cardCreate, 6 the format steps from cardFormat, 7 the
  delete steps, 8 nothing, set after an error; 0 idle), 0xD8 `step` (every *_main switches on it and increments it),
  0xA4 `readSlot` (the slot read_main reads; read_all_main steps it 0..3), 0xAC `readBuf` (memcard_read's buffer for the
  header and save checks, kept void* since it is read as both CardMetadata and CardSaveSlot), 0xB0 `writeBuf`
  (cardErase/cardCopy prepare it; write_main writes it under flag 0x800, then copies it into data->saves), 0xBC
  `firstCopy` and 0xC0 `secondCopy` (write_main's steps 10 and 11 write the slot to `copy * 4 slots` past the header:
  the backup first when only the main copy is valid). CardMetadata 0x1E40/0x1E42/0x1E44 `rumbleEnabled`,
  `soundSurround`, `lastFileSlot` (make_header stores gp's fields of those names, read_all_main restores them). The
  never-touched `unk` byte gaps (CardWork 0x2, 0x18, 0xC4, 0xDC, 0xE8; CardSaveSlot 0x2, 0x213B) are `pad_0xNN`, and the
  0x4000 read sizes are `sizeof(CardSaveSlot)`.
- **Allocations:** mot_jabara, mot_roll and mot_ship allocate and clear their work with `sizeof(JabaraWork)`,
  `sizeof(MotRoll)` and `sizeof(MotionShipData)` instead of 0x20, 0x14 and 0x48.
- **Left, and why.** filemgr.c's FileType3/7/8/9 views (109 `field_0x` sites, the bulk of src/manager's): they are the
  file-relative pointers fileGarbageDataAdrSet/Clear relocate for file types 3, 6-10, which no caller in this build
  loads, so nothing shows what any of them points to (Super Paper Mario's filemgr leaves the same words unknown).
  FileEntry 0x8 and 0x60: no C access, but FileEntry is reachable from many files, so they keep their names. MotRoll
  0x8 (tested and cleared in mot_roll, never set in the C) and 0x10 (no user). mot_dokan's 0x2A4 store and mot_ship's
  0x128/0x18C stores (no reader). The s32 zero resets of the scratch words in mot_jabara (MW_S32) and mot_roll stay raw:
  they store integer 0 across members whose users type them f32, s16 or s32. The remaining motion casts are MUST_MATCH
  volatile/pun sites or `(const char*)` SFX ids. dvdmgr's `DVDEntry.unknown` and fontmgr's `BinaryFontWidthEntry.unknown`
  are written or read nowhere that shows their use.
- **Checks.** Each commit: `build/G8MJ01/ok` recreated, the changed C files built under `configure.py --non-matching`,
  then CI PASS through `~/ci_notests.py` (its tools/tests step fails here for environment reasons, as recorded above:
  no `elftools` for test_reloc_value_check and the test_wave_launch pipe timeouts; no tool changed). This worktree had
  no ratchet baseline, so `ci.py --init` ran first on the byte-identical first build. At the end all 1,018 game objects
  (every src/ and rels/ unit) compiled under `--non-matching` with no failures, then a normal reconfigure and rebuild to
  `ok`. fncast_lint and local_prototypes clean.
- **Counts** (`tools/cleanup_index.py`, origin/main 55fec9be -> this branch rebased onto it, same script): index
  12.666 -> 12.506 per kLOC (8847 -> 8735 weighted); field_0x 1721 -> 1623, raw_offset 150 -> 145, struct_ptr_cast 1226 -> 1224, deref_cast 484 -> 483,
  the rest unchanged. `field_0x` in this task's files (src/motion, src/manager, include/motion, include/mot,
  include/manager): 229 -> 139, of which filemgr.c's 109 are unchanged; outside filemgr: 120 -> 30. Per file:
  mot_plane.c 75 -> 9, mot_plane.h 8 -> 2, mot_damage.c 9 -> 0, mot_stay.c 7 -> 0, mot_dokan.c 3 -> 1, mot_jump.c 2 -> 4
  (the raw MWF32 store became `field_0x2B4`), evtmgr.h 6 -> 4; mario.h 22 -> 21, mario_motion.c 10 -> 6, mario.c 4 -> 3,
  evt_msg.c 2 -> 0. The index doesn't count `unkNN` field names, so the JabaraWork, MotionShipData and CardWork names
  don't move it.
- **Review.** A fresh Claude Opus 5.5 subagent re-read every use of every new name against CLEANUP_GUIDE section 3: no
  wrong names; it corrected four comments (pitchSpeed's cap, velY's pitch use, the hit points' offsets, and that
  party_yoshi reads 0x2B4), applied before the last commit.
- **For a reviewer.** The names that rest most on interpretation: `fallReturnPos` (named from its two readers, both
  return-after-a-fall paths, rather than from mot_dokan, its writer), `hammerHitFrame` (the stored value is
  hitEstimate's type, used as a frame count), `firstCopy`/`secondCopy` (0 = main, 1 = backup, the `validSave1/2` order)
  and MotionPlaneData's `hitFrontPos`/`hitRearPos` (front and rear relative to each other along the heading).
## Map REL placeholder fields named from their consumers: jin, las, mri, tik, hei, bom, muj, usu, dig, aji (2026-10-25, Claude Opus 5.5, n9-q-frels2)

Task (Isaac): name and type the `field_0xNN` members in the map RELs other than gor, rsh, eki, yuu, tou, tou2 and
common (non-database source and their headers), from consumers in the same REL (CLEANUP_GUIDE section 3); data-table
labels nothing reads stay; include/ outside include/rel/<those areas> untouched. Parallel branches: frels1, fsrcmisc,
fmotion, fbtlsrc. Nothing outside rels/ was edited.

- **What there was.** By grep, 4,014 `field_0x` in those RELs, 2,436 of them in the battle databases. Of the
  1,578 outside the databases, 223 were in code or REL-local declarations; the rest are `// field_0xNN` labels on
  unit-file initialisers of shared types (BattleUnitKind, BattleWeapon, BattleUnitKindPart, DoorEntry, KinopioData),
  which stay. The index counted 181 sites (it doesn't scan rels/*/include). kpa, gon, dou, gra, moo, nok, pik and win
  had no code sites. Along the way the same structs' other placeholder spellings (`field_4`, `unk0`) were named
  where a consumer proves them.
- **Method.** Untracked harness in build/frels2/: chk.sh builds named objects and runs obj_equiv against a snapshot of
  build/G8MJ01/src taken before the branch; ci_local.py/ci.sh run tools/ci.py with the mgba venv's python plus a
  `--target` pyelftools dir on PYTHONPATH (data_check needs elftools), accepting only the 8 test_wave_launch.PassPipes
  environment errors. Every commit: CI PASS, `build/G8MJ01/ok` recreated. At the end: `obj_equiv --tree` over every
  object against the snapshot (all equivalent); the 212 objects of the ten touched RELs built under
  `configure.py --non-matching` (only the warnings already there: `.rodata` RW objects and "no prototype" on
  untouched functions), then a normal reconfigure and rebuild to `ok`; fncast_lint and local_prototypes clean.
- **Per struct** (field -> name: the consumer that proves it).
  - jin_07 TeresaEntry: 0x0/0x4/0x8 -> `startX/Y/Z` and 0xC/0x10/0x14 -> `endX/Y/Z` (the h00 and h01 terms of
    computePosHermite and its inlined copy in senteresa_ext_main), 0x18 -> `progress` (the Hermite t; +0.015 per frame,
    the move ends at 1.0), 0x24 -> `startTangentX` (va.x, the h10 term), 0x28 -> `delay` (state 0 counts it to 0, then
    the box shows), 0x2C -> `state` (the switch). computePosHermite takes a TeresaEntry* now (it indexed an f32*:
    p[0]..p[9]). Style follows jin_00's TeresaEntry (`startX`, `endX`).
  - las_04 FirebarDesc 0x20: a union `startAngle` (type 0: copied to entry->angle) / `targetX` (type 1: firebar_entry's
    direction test against position.x; copied to entry->targetX, which _firebar_trans compares). The positional
    initialisers fill the first member; the data bytes are unchanged (CI's data step).
  - unit_boss_batten_leader: BattenSatelliteSpline 0x58 -> `frame` (intplGetValue(4, frame, 0x3C, ...), freed at
    0x3C; the same as #1136's BarriernSatelliteTo.frame); BattenEffUserdata 0x38/0x3C -> `posX/posY`
    (evt_eff_firechange00 reads them as pos, moves pos and writes it back).
  - mri unit_boss_magnum_battender MagnumBattenderWork: `prevPos`, `pos`, `rotZ`, `targetRotZ`, the names aji's copy
    (ArmWork) already has; arm_main: prevPos = pos, pos = the unit position, targetRotZ = atan of the movement, rotZ
    eased toward it and passed to BtlUnit_SetRotate as z, the effect placed at pos plus (0, -18, 0) through the arm
    matrix.
  - mri_puni: PuniWork 0x458 -> `sfx` (evt_puni_force_se_off passes it to psndSFXOff; nothing in mri sets it), 0x448
    `unk448` -> `blinkCount` (puni_disp_info_sub sets it while count != total; puniCountDisp then draws the count
    only every other 16 frames); PuniData 0x1E8 -> `turnsLeft` (puniModeHagure: copied from counter, parity picks 90
    or 270, counts down to state 20), 0x2AC -> `fukidashiPos` (the position of the fake MarioWork at 0x220 that
    effFukidashiEntry reads; y -1000 hides it); MriPuniAnimSet `unk0/unk28/unk3C` -> `anims`, `frameCounts`,
    `terminators` (puniAnimation: texId = anims[idx][...], count before the terminator, -2 holds the last frame);
    PuniTexInfo `unk0/4/8` -> `name`, `animName`, `frame` (puniMakeTexture passes them to
    animPoseDisp_MakeExtTexture; NULL name ends the table). puniMapSetData's and the init userfuncs' `unkN` locals
    named from the fields they store.
  - tik_00 UranaiWork: 0x0 -> `savedLw[16]`, 0x40 -> `savedGw[32]` (memcpy'd from and back to lwData/gwData), 0xE4 ->
    `texValid` (card_tev loads the texture only once uranai_cap sets it), 0xE8 -> `capEvent` (evtEntry/evtDelete of
    evt_uranai_cap), 0xF4/0xF8 -> `width`/`height` (uranai_cap's viewport, scissor and texture size).
    aji_10 MonitorWork `field_4` -> `texValid` (the same pattern: monitor_cap_capture sets it, monitor_cap_tev tests it).
  - hei_10 HeiExtEntry: 0x14 -> `velY`, 0x18 -> `framesLeft`, 0x1C -> `speed`, 0x20 -> `gravity`, 0x24 ->
    `nextState` (ext_main case 0x64: y += velY, velY -= gravity, x/z += speed * sin/cos toward position, land when
    framesLeft < 0 and take nextState); chb_jump's parameters named to match. hei_10's ExtEntry and
    unit_chorobon_gundan's ChorogunExtEntry (`field_0x8`) were copies of extdrv.h's ExtMakeTextureEntry and went.
  - bom_01: SightWork `field_4[5]` -> `levels` (shifted sound samples drawn as bars), 0x18 -> `rot` (drawBG's rot for
    the four quarters), `field_20/24` -> `leftCount/rightCount` (drawNum beside the left/right arrow, +1 every 4
    frames while that arrow blinks); PoseData 0x4 -> `poseCount` (extGetPoseNum, the states loop bound); PoseEntry
    `field_8/C/10` -> `duration`, `firstTexId`, `texCount` (pose_main: texId = firstTexId + (texCount - 1) * t, t =
    elapsed frames / duration); PoseState `field_0/1/4/10/14/18/1C/20` -> `state`, `nextState`, `target`, `speed`,
    `gravity`, `framesLeft`, `velY`, `entryIdx` (the same hop as hei_10 in ext_main case 0x64; entryIdx indexes
    entries in pose_main). bom_01's ExtTableEntry (whose `type` held "R_1"/"S_2" animation names) is
    ExtMakeTextureEntry now: both ext_data tables go to evt_ext_entry.
  - muj_10 NanNpcEntry 0x70: a union `nextState` (muj_00 tobikakari: the state taken when the jump lands) /
    `entryIndex` (muj_20 elmos1000tai_init stores the entry's index; nothing reads it); CortezWork `field_194/25C`
    -> `frame`/`step` (per-audience step switch and its frame counter, intplGetValue(0, frame, 30, ...)).
  - las_05 KaronPoseEntry `field_4/8/C/10` -> `timer`, `velX`, `velY`, `velZ` (added to the pose each frame, velY less
    0.3 gravity; timer is the drop delay, walk leg and escape run); LasWork `field_C` -> `frame` (frame % 20 picks
    the move_check_list slice), `field_14` -> `hammerFrame` (frames into MARIO_MOTION_HAMMER mod 15; hits at 10).
  - usu_00 KpaSenEntry: 0x4 -> `progress`, 0x8/0x14 -> `Vec start`/`Vec end` (state 1 lerps x/z from start to end),
    0x20-0x23 -> `stopTexFirst/Last`, `moveTexFirst/Last` (calcPose's range after and during the move); calcPose's
    parameters named. dig_00 anim_tbl `field_0/4` -> `npcName` (evtNpcNameToPtr_NoAssert, npcGetTribe), `animMask`
    (bits 0-5 evt_action_sub picks from).
- **Left, and why.** Written but never read: TeresaEntry 0x1C/0x20; FirebarDesc 0x14 -> FirebarEntry 0x20;
  HeiExtEntry 0x0/0x4 and HeiExtWork 0x0; UranaiWork 0xEC/0xF0 (uranai_bbox writes its first argument to all four
  and discards the other three; only 0xF4/0xF8 are read); PuniWork 0x47C (2 from mri and mri_puni), PuniData 0x582
  (10 at init); PuniPose 0x24; ZakogunEntry 0x0; KaronPoseEntry `field_1`, PoseState `field_24`, anim_tbl `field_8`
  (handed back to the script, whose use wasn't traced), dmo DemoWork 0x0, end CreditsEndingRecord 0x0.
  BattenEffUserdata 0x8/0xC/0x18-0x30: only the stores in evt_eff_colorchange00/firechange00, which no script in las
  calls, and the effect isn't identified; 0x18-0x30 look like the N64 effects' TEV colour triplets
  (eff_akari_charge_n64's reg0R..reg1B) but the rest of that layout doesn't fit, so no names. NanNpcEntry
  0x16/0x80/0x94 and NanNpcWork 0x8 mirror src/event/evt_nannpc.c's private layout (outside this branch). Shared
  headers: aji_13's `party->field_0x144` and `field_110` (party.h), borodo's BattleUnit 0x30C, BattleWork
  `field_180C0`, tik_05's EffMist2Data `unk18`. CloudEntry.mapGrpName stays `s8`: as `char` (dropping three casts)
  mri_00's .text+0x124 differs. PuniPaternData (mri_puni.c's `pat/num/end` overlay of MriPuniAnimSet) stays: its
  `u16*` against the set's `s16*` would change loads.
- **Counts.** `field_0x` by grep in those RELs and their headers: 4,014 -> 3,852 (outside the battle databases 1,578
  -> 1,416; in code and REL-local declarations, comments excluded, 223 -> 62). `tools/cleanup_index.py` (5619c3bf ->
  this branch, same script): index **13.669 -> 13.477** per kLOC (9548 -> 9414 weighted); field_0x 2403 -> 2269,
  every other category unchanged. These RELs (rels/<area>, include/rel/<area>): 1076 -> 942 weighted, field_0x
  181 -> 47. Per file (weighted): jin_07 37 -> 2, magnum_battender 25 -> 6, bom_01 38 -> 22, hei_10 39 -> 22,
  tik_00 28 -> 10, mri_puni 51 -> 41, batten_leader 71 -> 59, las_04 23 -> 20, muj_00 7 -> 4, muj_20 45 -> 44.
- **For a reviewer.** `leftCount`/`rightCount` rest on where drawNum puts them (x -220 / 200, beside arrows 2 and 3)
  and the +1 while the arrow blinks; they start at rand(). `stopTex*`/`moveTex*` rest on the states that read them.
  `sfx` (PuniWork 0x458) is named from its one consumer, psndSFXOff; nothing in the REL writes it. The FirebarDesc and
  NanNpcEntry unions follow the guide's one-member-per-use rule; `entryIndex` is named from its store (the index
  itself), since nothing reads it.
