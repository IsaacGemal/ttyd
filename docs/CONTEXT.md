# TTYD decomp: the ledger

The single record of every decision on this fork and why it was made. New chapters go at the bottom, dated, with who
made the call. The "Current state" section is the only part that gets rewritten in place; everything below it is
append-only history (correct an old chapter by adding a new one that supersedes it, and say so).

Private notes (inference provider, accounts, keys, capacity) go in `docs/CONTEXT.private.md`, which is
gitignored, never here: this repo will be public.

Agents running in bulk never edit this file. They write their own `LOG.md`/`REPORT.md`; the orchestrator folds
anything worth keeping into a chapter here.

## Current state (rewrite this section as things change)

- New orchestrator session? Read "The orchestrator's role: how to pick this up" (near the end) first, then the
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
  readability (scripts must be EVT macros, never hex).
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

## Why TTYD (2026-09-24, Isaac + Claude Opus 5.5)

Picked over Luigi's Mansion (`IsaacGemal/lm-decomp`, kept private and parked):

- **Pure C.** All 130 source files are `.c`. C++ makes a large swarm fight over shared class layouts: parameter
  types are part of mangled symbol names, header inlines get copied into every caller, a base-class field shifts
  every derived class, and virtual-function order sets vtable slots. Every one of those is a local change that
  passes locally and silently breaks other files. C keeps only the mild version (struct fields, `static inline`
  helpers), which Snood already handled by treating canonical declarations as fixed.
- Stale upstream (last commit Jul 5), MIT license, no AI policy stated in the repo.
- The remaining work is almost all game code; the SDK is done. The RELs (per-area modules loaded at runtime) are
  0.3% done and fairly self-contained per area, which suits a queue.
- Bigger than Luigi's Mansion (3.8 MB of code vs 2.1 MB) and a better-known game.

The old local `swarm` branch (Sep 19, ~132 small functions) predates the Snood method and is not carried forward.

## Carried over from Snood (2026-09-24)

Snood (GBA, `~/snood`) went to 100% byte-matched with this method; its `notes/AGENT_WAVES.md` is the long form.

- A model's claim of a match is never evidence. Only the checker over the full extent is. Here that's objdiff.
- Bounded tasks with hypotheses beat open-ended "match this". Re-verify every result independently before merging.
- Fix the process that produces bad output (prompts, rules, tools), not the output by hand.
- Every result carries a `Model:` line, and commits credit the model that did the work (see `CLAUDE.md`).
- Opencode `run` dies on a single provider 500 and loses its work. Count missing reports after every wave.

## Scaling design, first draft (2026-09-24, Isaac + Claude Opus 5.5)

Target is ~100 concurrent agents. Pieces:

- **Unit of work = one source file (translation unit), leased.** Metrowerks lays out a whole file together, so
  two agents in one file interfere. A lease has a heartbeat; a dead agent's lease expires and the file goes
  back in the queue.
- **One agent, one git worktree.** Agents never push. They hand back a patch.
- **One serialized merger.** Applies the patch to current main, rebuilds, and checks with objdiff that the target
  improved and nothing else regressed. Only then commits.
- **Shared headers have an owner.** Struct and prototype changes go through a separate, careful path, not
  whichever agent needs them first.
- **Easy first, escalate the stuck.** Order the queue by difficulty; cheap model first, stronger models and the
  permuter for what's left.
- **Retry and checkpoint.** Retry provider 500s with backoff; save partial progress.
- **Resource limits.** Per-agent cgroups; watch disk and I/O. The tower handled ~40 Snood agents.
- **Automatic cross-pollination.** A periodic job reads new match logs, extracts compiler idioms that are new,
  and appends them to the shared notes every wave reads.

## Reference: how others pushed agent swarms (2026-09-24)

- **Bun, Zig to Rust** (bun.com/blog/bun-in-rust): ~64 Claude instances, 535K lines in 11 days, ~$165K. A test
  suite of 1M+ assertions was the ground truth; compiler errors grouped by file became the work queue; 1
  implementer + 2 adversarial reviewers each. Failures: agents running `git stash`/`reset` on each other (fixed
  with worktrees and banning those commands), disk I/O stalls, disks filling (fixed with cgroups). Rule worth
  copying for readability review: "If you need a paragraph-long comment to justify why the workaround is OK,
  the code is wrong."
- **OpenAI, Navier–Stokes** (openai.com/index/navier-stokes-solution): ~10,000 concurrent agents in groups that
  talked within the group, 88 hours to a result plus 17 for the Lean proof. Different groups got different
  framings of the problem; insights were periodically consolidated and fed back to all groups; the model was
  swapped mid-run as a better one arrived.
- Common pattern: a large, loosely coordinated search plus a checker that can't be fooled. Our checker (byte
  match) is stricter than either of theirs, so most of our work is the system around it.

## Build parity: `hardware.c` renamed to `HARDWARE.C` (2026-09-24, Claude Opus 5.5)

The configure step lists `musyx/runtime/HARDWARE.C`, and the unit name must match `config/G8MJ01/splits.txt`,
but the file on disk was `hardware.c`. macOS's case-insensitive filesystem found it anyway; Linux (the swarm
machine, and decomp.dev's CI) did not, so that unit's 38 matched functions counted as unmatched there. That was
the whole gap between a Mac build (2,065 functions) and decomp.dev (2,027). Renaming the file (not the configure
entry, which un-maps the unit) makes Mac and Linux agree: 2,065 / 9,770, MusyX 286 / 333. A local build is the
source of truth; decomp.dev lags it by this fix until upstream picks it up.

## Sep 19 swarm workspaces cleared (2026-09-25, Isaac + Claude Opus 5.5)

The tower's `~/ttyd-ws` (165 per-agent copies of the source from the Sep 19 run, ~890 MB) was deleted: that run
predates the Snood method and its layout won't be reused. Its 146 `LOG.md` files (~580 KB) were kept on the Mac
in the gitignored `archive/sep19-swarm-logs/`, unreviewed. They are notes on what mwcc needed to match
specific functions, raw material for the shared compiler notes.

## Operating principles, first pass (2026-09-25, Isaac; written up by Claude Opus 5.5)

1. **The human era is the reference for good work.** Tag `human-baseline` marks `2df569d`, the last upstream
   commit before this fork; everything at or before it is human work. It's the model for what a good merge
   looks like: file layout, header conventions, naming, how a unit moves from `NonMatching` to `Matching` in
   `configure.py`, and when compiler flags get set per file (`extra_cflags=["-O4,s", "-inline none"]`). The best
   worked examples are upstream PRs #10-#13 by Ilwyd: small (3-12 files), one area each, e.g. #12 `motion/mot_walk`,
   "All functions matching". NWPlayer123's direct commits are larger batches (up to ~40 files). Agents should be
   pointed at these, and `git diff human-baseline..HEAD` is exactly the AI-era work.
2. **Merges are richer than patches.** Probably one branch per unit, and the merger opens a PR on this private
   repo with the objdiff before/after in the body and merges it once the checker passes. That gives each
   change a reviewable record, which is also the provenance record for how each function was reached.
   (Not settled; decide when the queue is designed.)
3. **First tests on DeepSeek V4.1 Flash,** because that inference path is known to work. Scaling capacity is
   Isaac's to arrange.
4. **Try Pi as the harness** (github.com/badlogic/pi-mono, `@earendil-works/pi-coding-agent`, MIT). Minimal
   agent with 4 tools (read, write, edit, bash); `pi --print` / `--mode json` for headless runs, `--mode rpc` and
   a TypeScript SDK for embedding in an orchestrator; OpenAI-compatible endpoints via `models.json`; persistent
   sessions; and **automatic retry** (`agent_end.willRetry`), which opencode lacks: one provider 500 killed half
   the Snood recon wave. Plan: run the same small task set through opencode and Pi and compare matches, time
   and tokens before choosing.
5. **Escalate the stuck.** Most functions should fall to the cheap model; the rest need the big guns, the way
   Snood's last mile (`tick`, the register-allocation forensics) needed a frontier model working by hand. The
   queue must record why an attempt failed so the next tier starts from it, not from zero.
6. **One oracle.** A function is matched when the local objdiff build says so over its full extent, nothing
   else: not a model's claim, not a fuzzy score, not decomp.dev. Mac and Linux builds must agree (they do, as of
   the `HARDWARE.C` fix). Every tier and every harness answers to the same checker.

## Refinement: emulate the unit of work, and the two oracles (2026-09-25, Isaac; written up by Claude Opus 5.5)

Refines principles 1 and 6 above.

- **What to copy from the human era is the unit of work, not the prose.** Code written by agents for other
  agents to read has different needs from code written for humans, so the human style isn't the target. The
  target is how they define a finished unit: one file, all functions matched, cleaned up, and flipped from
  `NonMatching` to `Matching` in `configure.py` so it links, in one self-contained change.
- **Progress has two tracks, each with its own oracle.** objdiff reports, for code: fuzzy match, perfect
  (matched) and fully linked; for data: matched and fully linked. "Matched" means the compiled object equals
  the original per function or symbol. "Linked" means the unit is marked `Matching`, goes into the real link, and
  the final DOL/RELs still hash to `config/G8MJ01/build.sha1`. Linked is the stricter bar and the real finish
  line; matched is a step toward it. Fuzzy is a progress hint for agents, never a pass. (Snood collapsed the two:
  its single check was "the whole ROM rebuilds byte-identical", which is the linked bar for everything at once.)
- **Probably code first, data later.** Someone suggested agents work on code and leave data for later: data
  placement is hard to get right until the code that references it is in C, and linking a unit settles its data
  anyway. Not decided; revisit when designing the queue.

## Measuring progress autonomously: the ratchet (2026-09-25, Isaac + Claude Opus 5.5)

Goal: the system measures its own progress, so even if some share of agent work is bad, grinding on converges
on the fully matched, linked game. That holds only if three things are true, so they are rules:

1. **Nothing merged can make any number go down.** Every merge regenerates `report.json` and runs
   `objdiff-cli report changes <previous> <current>` (built into objdiff). Any function, unit or data symbol
   that got worse rejects the merge, even if the target improved. The last accepted `report.json` is the ratchet.
2. **Bad work must not be able to pass the checker.** The byte check can be satisfied by cheating, so the merger
   also lints the diff. The obvious one: Metrowerks accepts `asm` functions, so an agent could paste the
   original instructions and "match" trivially. Human game code uses `asm` in exactly one place
   (`memcpy_as4` in `src/system.c`); new `asm`, new `#pragma` tricks, and edits to `configure.py` flags outside
   the unit being worked on are rejected or flagged for review. The list grows as agents find loopholes.
3. **Stuck work leaves a trail.** Convergence isn't automatic: the long tail is where Snood spent most of its
   effort. Failed attempts record what they tried and how close they got, so the next attempt or tier starts
   from there.

**Progress display, both tracks:** code (fuzzy / matched / linked) and data (matched / linked), overall and per
category (game DOL, RELs, SDK, MusyX), from the report at each merge, plotted over time like decomp.dev. The
headline number is code linked; matched is shown next to it as work in the pipeline.

## Local CI: `tools/ci.py` (2026-09-25, Isaac + Claude Opus 5.5)

The gate every change passes, human or agent: `python3 tools/ci.py`. GitHub's workflow is disabled on this fork
(it pulls doldecomp's private build image, which carries the game files, and gets denied).

1. **Lint** the diff against `--base` (default `HEAD`): no new inline `asm`, no new `#pragma`
   (`--allow-pragma`), no edits under `config/ orig/ tools/ .github/ libs/ assets/` (`--allow-protected`), and
   `configure.py` changes limited to `Object(...)` lines.
2. **Build**: configure + ninja. Ninja itself fails if a linked module stops hashing to `build.sha1`.
3. **Ratchet**: `objdiff-cli report changes` against `build/ci/accepted.json`; any function, unit or total that
   got worse fails the run. (The template's own `ninja changes` only prints regressions and exits 0.)
4. **Summary** of both tracks with deltas: code fuzzy / matched / linked, data matched / linked.

`--accept` makes a passing build the new baseline (the merger does this after each merge); `--init` sets the
first one. The baseline lives in `build/`, so it is per machine. Tested on 2026-09-25: a one-character change
to a matched function failed at the ratchet (`weaponGetPowerDefault` 100% -> 99.5%), and an added `asm`
function failed at lint. Clean run: ~25 s on the Mac.

Next: run it on the tower as a self-hosted GitHub runner while the repo is private (the game stays on the
tower). Before going public that runner must stop running pull-request code.

## Parked ideas

Considered and deliberately not built yet. Pick one up by adding a dated chapter.

- **Self-hosted GitHub runner on the tower** (parked 2026-09-25). Supersedes the "Next" line of the local-CI
  chapter. Would run `tools/ci.py` on each push and show checks in GitHub's UI. Not needed now: the merger calls
  `tools/ci.py` directly and records the result in the PR or commit. Revisit when the repo goes public, for a
  visible "build passing" badge; then it must run only on pushes to `main`, never on pull requests (see the
  private go-public checklist). Not implemented, not tested.

## Harness: Pi over opencode (2026-09-25, Claude Opus 5.5)

Decision: swarm agents run on [Pi](https://github.com/badlogic/pi-mono) (`@earendil-works/pi-coding-agent`),
not opencode, which ran every Snood agent. The reasons are specific failures from Snood's record
(`~/snood/notes/AGENT_WAVES.md`) and a head-to-head trial on Snood cleanup work on 2026-09-25:

- **opencode lost work on failures it could have survived.** One provider 500 made `opencode run` exit, and
  anything not yet written to disk was gone: on Sep 24 three of four recon agents hit one within 23 seconds and
  two had to be rerun from zero. Pi retries transient provider errors itself (`retry.*` in
  `~/.pi/agent/settings.json`, 3 attempts by default) and saves every session to disk, so a dead or timed-out
  agent can be resumed with `pi --session <id>` and told to finish, instead of starting over.
- **opencode stalled on its own permission prompts.** Snood round 5's G02 hung on a permission prompt and wrote
  nothing; round 7's G04 hit an external-directory denial and left no report. Pi has no approval prompts in
  headless mode. The flip side: its tools run with the process's full permissions, so isolation has to come from
  the OS (a worktree per agent, resource limits, no secrets in the environment beyond the model key).
- **opencode was a black box; Pi is measurable.** opencode's output was colored text with no token counts, so
  Snood's spend could only be estimated afterwards. Pi's `--mode json` emits a structured event for every turn
  and tool call with exact usage: input, cached input and output tokens. That is how the trial showed the
  inference difference (below), and it's what the orchestrator needs to watch 100 agents and spot one that has
  hung.
- **Less baggage.** Pi's base prompt is ~2,300 tokens and has 4 tools (read, write, edit, bash); the task
  prompt is nearly all of an agent's context. It can also be driven from TypeScript (SDK) or over stdin (RPC)
  if the orchestrator outgrows spawning processes.
- **Trial result.** Two Pi agents build-tested 15 register pins and asm barriers in Snood; each confirmed the
  unmodified file matched first, tested one removal at a time, and wrote a correct report and patch. All 3
  "removable" results and a spot-checked "load-bearing" one reproduced independently, and the removals were
  committed (Snood `865a9a5`). On DeepSeek's API turns took 2-3 s with ~90% of input served from cache, and
  each agent finished in under 30 s. Prompt caching is what makes that possible: agents re-send their whole
  growing conversation every turn, so an endpoint without it is several times slower on the same work.

Known gotchas: Pi needs Node 22+ (the tower's `~/.zshenv` now puts nvm's Node 24 first); headless runs must
close stdin (`</dev/null`) or Pi waits for piped input; Pi doesn't tell the model its own name, so the prompt
has to supply the exact `Model:` line; and agents must still write their report as they go, since a hard kill
(`timeout`) ends the process whatever the harness. Still untested: Pi's automatic retry under real 500s, because
the providers used in the trial never failed.

## Code smells to track: Melee's cleanup index (2026-09-25, Isaac + Claude Opus 5.5)

After Melee hit 100% its team started tracking cleanup with `tools/cleanup_index.py`
(doldecomp/melee, rendered at doldecomp.github.io/melee/cleanup/). It counts "sites" by their outermost form, in
three families:

- **move** (a struct field nobody has named yet): `cast+offset` `*(f32*)((u8*)p + 0xE0)`, `cast+index`
  `((T*)p)[n]`, `raw-offset` `p + 0xNN`, `m2c-field` (`M2C_FIELD(p, T*, off)`), `abs-address` `(T*)0x80xxxxxx`.
- **retype** (types not worked out): `struct-ptr-cast`, `prim-ptr-cast` `(u8*)p`, `deref-cast` `*(T*)p`,
  `ptr-to-int` `(u32)p`.
- **hack** (code shaped for the compiler, not the reader): `PAD_STACK(n)`, `#ifdef MUST_MATCH` regions,
  `#pragma` (except push/pop), `goto`, `longjmp`.

Run unmodified against this repo (script copied beside `src/` and `include/`), as of `5762fcd`:

| | TTYD now | Melee now (100% matched) |
|---|---|---|
| total sites | 413 in 45 files | 8,020 in 649 files |
| move / retype / hack | 55 / 306 / 52 | 778 / 3,787 / 3,455 |
| biggest | 148 prim-ptr-cast, 79 deref-cast, 40 abs-address, 33 pragma, 19 goto | 2,162 struct-ptr-cast, 2,147 PAD_STACK, 668 goto |

Lessons for the swarm:
- **This is the readability counterpart of the byte ratchet.** Matching can't go down, and these counts shouldn't
  go up per merge either: a merge that adds match hacks or raw offsets should say why. Candidate for `tools/ci.py`
  as a second, softer ratchet (warn, then fail once the counts are stable).
- **Melee's `MUST_MATCH` pattern** keeps the hack out of the readable code: the clean version is the default and
  the compiler-shaped version sits in `#ifdef MUST_MATCH` (147 files use it). Worth adopting so agents' hacks are
  labelled and removable later.
- **`#pragma` is not automatically a cheat in Metrowerks code.** The human-written TTYD source already has 33
  (`opt_propagation`, `dont_inline`, `pool_data`, ...), so `tools/ci.py`'s blanket "no new #pragma" rule is too
  strict; loosen it to an allowlist of these known ones, still flagging anything else.
- Snood's cleanup recon found the same families by hand (register pins = hack, duplicate aliases = retype).

## Melee as the reference, and guarding against slop (2026-09-25, Isaac; written up by Claude Opus 5.5)

1. **Other finished decomps are sources of truth; Melee first.** Melee (doldecomp/melee, local at `~/melee`)
   reached 100% this month: same console, same compiler family, ~3.9 MB of code like ours, finished by the
   people who know this work best. Study it actively, not only when stuck: its docs and notes, its tools
   (`cleanup_index.py`, m2c/permuter usage), how it names and organizes structs and headers, its `MUST_MATCH`
   and `PAD_STACK` conventions, and its git history, which shows how a project gets from 0 to 100 and how the
   long tail was finished. Also relevant: other Metrowerks GameCube decomps (Pikmin 2, Wind Waker, Sunshine,
   Twilight Princess) for shared libraries and idioms.
2. **Guarding against slop as the problems get harder.** Agents can keep going indefinitely; review time can't.
   As the work moves to harder functions, we'll want better ways to make sure agents aren't producing slop
   (code that matches but reads badly or cuts corners), ideally as automated checks or flags so review can stay
   on the judgment calls. The finished repos, Melee especially, are what those checks can be modelled on: its
   cleanup index is one example. We don't know yet exactly what this looks like. What we have now, at minimum:
   lint, byte match, and no regressions (`tools/ci.py`).

## Wave 1: 12 agents, 20 minutes (2026-09-25, Claude Opus 5.5; agents DeepSeek V4.1 Flash)

Setup: Pi + DeepSeek V4.1 Flash via DeepSeek's API, on the tower, one agent per source file, each in its own copy
of the repo with a warm build (`~/ttyd-agents/<file>`), 20-minute cap, told to update REPORT.md after every
function. Files: battle_event_cmd, battle_audience, event, evt_npc, mapdrv, battle_unit, pmario_sound, party,
animdrv, battle, npcdrv, mario. A helper showed each agent its file's match % per function and an
original-vs-yours instruction diff (objdiff one-shot mode).

Result: **2,065 -> 2,407 functions (+342)**, game code 942 -> 1,284; code matched 12.16% -> 13.09%; data matched
4.62% -> 5.14%. 10 files merged after `tools/ci.py` passed; 2 rejected by the ratchet:
- `pmario_sound`: deleted 4 human stub functions (small fuzzy scores to 0) and used raw offsets.
- `battle_audience`: ~50 new matches, but 7 existing functions collapsed (`BattleAudience_Init` 52% -> 0%).
  Patch kept in `~/snood/scratch/ttyd_wave1/`; the new matches could be salvaged.
`event` went to 219/219 mostly by renaming already-correct functions to their original symbol names from
`config/G8MJ01/symbols.txt` (objdiff pairs functions by name). `mario` hit the cap but lost nothing, because its
report and file were current.

Cost: 1,849 turns, ~301M tokens, 99.4% of them cached input (297.6M cached, 1.7M uncached, 1.9M output); about
$2-3 at DeepSeek's list prices. The tower sat at load ~0.3 throughout.

Lessons:
- **Incremental reports work.** The one agent that hit the cap lost nothing.
- **The ratchet earns its keep.** Both rejections were caught without reading the diffs.
- **`#pragma` needs an allowlist, not a ban.** Two merges needed `--allow-pragma` for `dont_inline`, which the
  human code already uses.
- **Smells come with the speed.** Merged code adds raw offsets and casts (e.g. `*(f32*)(mario + 0x8c)` where
  `MarioWork.position` exists). Melee matched first and built its cleanup index after reaching 100%, so these
  are left for a later cleanup pass, not blocked at merge.
- **A shared temp path in a helper is a race.** Every agent's helper wrote the same `/tmp` file for the first
  few minutes; per-workspace paths now.

## Planned soon: Codex as the PR reviewer (2026-09-25, Isaac; written up by Claude Opus 5.5)

Not built yet; to work on shortly. The Bun port's pattern: implementer plus adversarial reviewer. Here:

1. The orchestrator picks which agent results to accept from a wave (after `tools/ci.py` passes each).
2. It opens **one PR per wave** on this repo with those merges.
3. **Codex (GPT) reviews the PR.** In Isaac's experience it is by far the most thorough out-of-the-box
   reviewer. CI has already settled "does it match", so the review can focus on what CI can't judge: raw offsets
   where a named field exists, deleted or disabled code, workarounds, anything that games the checker. Wave 1's
   `pmario_sound` (deleted stubs) and `evt_npc` (`*(f32*)(mario + 0x8c)` instead of `MarioWork.position`) are good
   calibration cases for whether it catches what the orchestrator caught.
4. The orchestrator fixes what it finds, then merges.

Open: Codex usage limits (Isaac will upgrade the plan if needed), and how to trigger and collect the review.
An alternative to compare later: a separate no-context review agent on a different model, given a
"find problems" prompt.

## Pragma policy (2026-09-25, Claude Opus 5.5, at Isaac's request)

Codex's review of wave 2 called the agents' `#pragma` toggles match hacks. Decision: keep them, with rules.
1. **Whole-file compiler settings go in `configure.py`,** as per-file flags (`extra_cflags=["-O4,s"]`), the way
   the human-written units do it; not as a pragma at the top of a source file. (Done for `battle_camera.c`.)
2. **Narrow `dont_inline` / `opt_propagation` toggles are allowed.** They are ordinary Metrowerks tools that the
   original developers used, and the human code here uses them the same way (`hitdrv.c`, `evtmgr.c`). Banning
   them would push agents toward worse tricks.
3. **Every pragma an agent adds carries a one-line reason,** and the cleanup index counts them so growth is
   visible. A pragma with no reason goes back to the agent. Melee kept 308 at 100%, so the count alone isn't a
   failure. `tools/ci.py` allows only these known kinds (`--allow-pragma` for review of anything new).

## Partial functions: allowed if labelled, never empty (2026-09-25, Isaac; written up by Claude Opus 5.5)

Codex's reviews of wave 2 found the same failure four times: a function that isn't byte-matched but looks
implemented (`evt_shop_setup` returned after an empty loop; `btlDispPoseAnime` was 95% similar but cleared the
wrong flags; `battleCameraMoveTo` and `battleCameraMain` were empty). CI can't see these, since the byte check only
proves matched functions. A stricter "only functions that reach 100% may change" rule was considered and set
aside for now because it throws away real partial progress. The rule instead:

1. **Labelled.** Any function an agent adds or changes that doesn't reach 100% carries a marker comment directly
   above it, stating the score and what still differs, e.g. `// NONMATCHING (95%): wrong flag mask at 0x204`.
   The merger checks this mechanically against the report's per-function scores.
2. **Never empty.** An unmatched function with an empty or near-empty body (a stub standing in for real code) is
   rejected outright.
3. **Never worse.** A function's score may not drop (the existing ratchet); deliberate removals of wrong code use
   `tools/ci.py --allow-regress FUNC`.

Revisit if labelled partials keep causing trouble; the strict rule is the fallback.

## Wave 2 and the first Codex-reviewed PR (2026-09-25, Claude Opus 5.5; agents DeepSeek V4.1 Flash; review GPT-5.6 Sol)

12 Pi agents (DeepSeek V4.1 Flash via DeepSeek's API), 10 new files plus retries of wave 1's two rejects, with two
new prompt rules (never delete/disable existing functions; use named fields). All 12 passed CI (both retries now
did); each landed as its own commit on `wave-2`, opened as PR #1, reviewed twice by Codex (`codex exec review`
with a rules file, GPT-5.6 Sol, medium), fixed, and merged (`4ed1a8a`). 2,414 -> 2,722 functions: +306 from the
agents, +2 from a header type fix found in review.

What review found that CI can't: unmatched functions that look implemented (empty or incomplete bodies, wrong
flags), raw offsets where headers name the field, private struct copies that disagree with the headers. Two
header types were wrong (`mShopPoints` is s16, `mStarPowersObtained` u16), and fixing the second matched two more
functions. Each full-PR review found a different subset of issues (9, then 10 new), so one pass over a ~6,300-line
diff isn't an audit.

Takeaways: the one essential new safeguard vs Snood is the partial-function rule (labelled, never empty); with it,
"CI + that rule" is the merge bar. Codex review is an occasional audit, not a gate. Readability issues are cleanup
passes, as on Snood and Melee. Left for a follow-up: four duplicated structs (`ShopWork`, `StatusWindowValue`,
`BattleStageWork`, the second `PaperSoundEffect`). GitHub is the source of truth for merged state.

## Per-file notes: an empirical log for each source file (2026-09-25, Isaac; written up by Claude Opus 5.5)

Design, not yet built. The unit of work is the source file, so knowledge about a file lives in one place that
mirrors it: `src/<path>.c` -> `notes/<path>.md` (tracked). An agent assigned a file gets that file's notes (plus
the shared compiler-idioms file) in its prompt, and nothing else.

The notes are a scientific log: measured, append-only, never rewritten.
- **Measurements come from the build, not the agent.** The merge script writes each function's score before
  and after (from CI's report comparison), with date, model and commit.
- **The agent supplies only change and observation:** what it tried, and what it saw in the instruction diff
  ("original loads lha, ours lhz"). No interpretation or adjectives; a guess is labelled as one.
- One entry per function per attempt, including failed and reverted ones, so the next agent starts from what
  was already tried.
- Knowledge that isn't about one file goes elsewhere: data types into the headers in `include/`, compiler tricks
  into a shared idioms file, process lessons into this ledger.
- Raw evidence (full reports, Pi sessions) stays in scratch, or on Traces if Isaac approves sharing.

Example entry:

    ## 2026-09-25 · GLM-5.3-Flash · commit db9e3ff
    - BattleStatusEffectMain: 5.26% -> 100%
      change: BSE_* helper definitions wrapped in #pragma dont_inline on/reset
      observed: without it, BSE_SleepDelete was inlined into Main (original has a bl)

## Wave 3: 24 agents, a lint step, and a second reviewed PR (2026-09-25, Claude Opus 5.5; agents DeepSeek V4.1 Flash; review GPT-5.6 Sol)

24 Pi agents (DeepSeek V4.1 Flash), one existing source file each (the 24 with the most unmatched small functions),
30-minute cap. The prompt added two rules: the partial-function rule, and "when you've been through the list once,
go back to the near-misses and keep working until stopped". 23 of 24 files passed CI and landed as their own
commits on `wave3` (PR #2): **2,722 -> 3,113 functions (+391)**, then +1 from a review fix. Rejected:
`sac_bakugame`, whose agent was stopped mid-edit and left the file not compiling.

**New merge step: `lint_patch.py`** (in the orchestrator's scratch for now). After CI passes it rejects a newly
introduced empty stub below 100%, and flags without rejecting: partial functions missing a `// NONMATCHING` label,
raw offset casts, structs and externs declared in a `.c` file, and pragmas with no reason. On wave 3 it flagged
56 unlabelled partials, 272 raw-offset lines, 25 local structs and 126 local externs. Its first version wrongly
rejected six good files (a comment-stripping bug, and it didn't exempt stubs already on `main` or functions that
really are empty in the original); fixed and re-tested by blanking a real function.

**Codex review, round 1** was given the lint output and told not to repeat it. It found five things lint can't:

| Finding | Resolution |
|---|---|
| 193 added `#pragma`s forcing matches | Most were `optimize_for_size` toggled around functions. Compiling 8 battle files at `-O4,s` in `configure.py` (like `battle_camera`) keeps every match, so they're gone. The `dont_inline` / `opt_propagation` ones are needed (removing them loses matches); each now has a reason checked against the original asm or by removal. 5 unused pairs removed. |
| Local externs contradicting headers | `psndSFXOn_3D` returns a value in the original: the header was wrong, now `s32`. Two others fixed. Two kept (the matches depend on them), each commented. |
| `ExtWorkFull` duplicating `ExtWork` | Full layout moved into `extdrv.h`, casts dropped; one more function matched. |
| Raw offsets for named fields | `protectId`, `poseTable` |
| `evt_bero` offsets, wrong header comment | Field names; `animeArg1/2` are `u16` (the original loads them with `lhz`); `animeArg4` is at 0x38 |

Lessons:
- **Codex's advice isn't always the fix; its findings usually point at one.** "Remove the pragmas" would have lost
  matches; the real fix was a per-file flag. CI caught each fix that quietly broke a match (three times).
- **Many battle files were compiled at `-O4,s`.** Agents rediscover this function by function with pragmas; the
  next wave's prompt or idioms file should say to check the file's flags first.
- **A timed-out agent can leave its file broken mid-edit.** The launcher should keep the agent's last version
  that built.
- **About a third of the agents still finished early** despite the keep-going line.
- Still open: the 56 unlabelled partial functions (the percentage is mechanical, "what still differs" needs a
  look).


## Per-file notes deferred (2026-09-25, Isaac; written up by Claude Opus 5.5)

Not built yet, on purpose. Notes pay off when several agents make repeated passes at the same hard function, which
is mostly the end game (around 90%). Until then each file gets one pass, and each agent's `REPORT.md` (per-function
before/after and what it tried) is kept in the orchestrator's scratch, so the notes can be backfilled from it later.

## Wave 3 review, round 2 (2026-09-25, Claude Opus 5.5; review GPT-5.6 Sol)

Round 2 confirmed round 1's fixes and found five new issues, all fixed; 3,114 functions before and after.
- **A real bug:** `BattleAudience_NoUsedHaitiRand` used its counter uninitialized. The agent's 93.52% was partly
  earned by the bug; initializing it where the original does (before the loops) scores 96.97%.
- **One signature for the font width functions,** declared five ways across seven files. The original callers
  mask the result to 16 bits, so both return `u16`; declared once in `fontmgr.h`.
- **Two more split layouts** like `ExtWork`: `ShadowEntry` and `ProjShadowWork` now have their full layouts in
  `shadowdrv.h`.
- **Raw offsets** in `winmgr` and `mapdrv` replaced by named fields; `ItemData::star_piece_price` is `u16`.

Lessons:
- **Two rounds found different things again** (as in wave 2): round 1 the broad patterns, round 2 bugs and
  signatures inside individual functions. A second round is worth it on a PR this size.
- **Named fields don't always compile to the same code.** Five shadow setters only match in their byte-offset
  form (the original adds the field offset before the store), and a header edit that silently didn't apply
  left callers with no prototype. CI caught both.
- **Types the original reads unsigned** (`lhz`) keep turning up declared signed in the headers (`animeArg1/2`,
  `star_piece_price`). Worth a mechanical check across the headers later.

## The orchestrator's role: how to pick this up (2026-09-25, Isaac; written up by Claude Opus 5.5)

For a new Claude session taking over. Isaac sets direction; the orchestrator (a Claude Code session on Isaac's
Mac) runs the waves, merges, reviews and keeps this ledger. Agents do the matching; the orchestrator never
takes an agent's word for anything, only the build's.

**Start here.**
1. `docs/CONTEXT.private.md`, if it exists (gitignored, on Isaac's Mac only), has the inference provider, keys,
   the tower, capacity, and what can and can't go public.
2. Check the live state: `gh pr list`, `git log --oneline -15`, and `python3 tools/ci.py` on `main` (it should
   pass with nothing changed). GitHub is the source of truth for what's merged.
3. Ask Isaac what's next. Don't launch a wave, spend on inference or share traces without him.

**One wave, end to end** (wave 3 is the worked example):
1. **Pick files.** Existing `src/` files with the most unmatched small functions, from
   `build/G8MJ01/report.json`; skip files another open PR touches. Later waves will add "finisher" agents on
   nearly complete files (see "Linking" below).
2. **Launch** 12-24 Pi agents on the tower, one file each, a copy of the repo per agent, 30-minute cap. The
   prompt is in the launcher (`launch.py`); it carries the rules from this ledger (no asm, pragmas only with a
   reason, never delete existing functions, the partial-function rule, named fields, write `REPORT.md` with a
   `Model:` line).
3. **Merge each result** with the merge script: apply the patch, `tools/ci.py` (build, byte-match, ratchet),
   then `lint_patch.py` (rejects new empty stubs; flags unlabelled partials, raw offsets, local structs and
   externs, pragmas without reasons). One commit per file on a `waveN` branch, crediting the agent's model
   with `Co-Authored-By:` and the orchestrator's model too. Reject files that don't build; don't hand-repair
   an agent's half-finished edit.
4. **Open one PR per wave**, then **Codex review** (`codex exec`, GPT-5.6 Sol, medium effort, read-only
   sandbox) with the rules file and the lint output, told not to repeat the lint. Two rounds on a big PR:
   they find different things.
5. **Fix every finding yourself**, each checked by `tools/ci.py` before committing. A finding points at a
   problem; its suggested fix may be wrong (removing "match-forcing" pragmas would have lost matches; the real
   fix was a per-file compiler flag). `--allow-regress FUNC` only for a small fuzzy drop in a named, unmatched
   function, said in the commit message.
6. **Write the ledger chapter** (results, what review found, lessons), then Isaac merges the PR.

**Rules the orchestrator keeps.**
- Nothing about the inference provider, accounts, keys or costs in this repo: it will be public. A local
  pre-commit hook blocks the obvious cases; private notes go in `CONTEXT.private.md`.
- Every number in a commit message or this ledger comes from the build. Report failures and rejections as
  plainly as successes.
- Ask Isaac before adding hooks, cron jobs or other standing setup, before force-pushing anything but your own
  PR branch, and before sharing agent sessions anywhere.
- Codex runs on GPT-5.6 Sol at medium effort; never a larger model (it burns Isaac's usage).
- Delete with `trash`, not `rm -rf`; Python packages with `uv`.

**Where the tooling lives.** `tools/ci.py` is in the repo. The wave tooling (launcher, the agents' `check.py`
helper, merge script, `lint_patch.py`, Codex rules files) is still in the orchestrator's scratch on Isaac's Mac,
`~/snood/scratch/ttyd_wave3/`, next to each agent's `REPORT.md`, patch and session log. The provider-neutral
parts (merge script, lint, linking) should move into `tools/` so a new session doesn't depend on that folder.

**Linking.** A file counts as linked only when the whole file matches (code and data) and it's flipped from
`NonMatching` to `Matching` in `configure.py` with the build's checksums still passing. Waves don't do this on
their own. Planned: an auto-link step in the merge pipeline that tries the flip for every file at 100% and keeps
it only if the checksums pass. A test on Sep 25 linked 7 of 10 candidates; `swdrv`, `gxsub`, `seqdrv` and
`synth_dbtab` show 100% in the report but fail to link, which is work for a finisher agent.

**Open threads as of this chapter:** 56 partial functions without a `// NONMATCHING` label; `sac_bakugame`
(rejected in wave 3); four duplicated structs from wave 2 (`ShopWork`, `StatusWindowValue`, `BattleStageWork`,
`PaperSoundEffect`); agents that stop early; the launcher should keep a timed-out agent's last version that
built; headers that declare signed what the original reads unsigned.

## Auto-linking built (2026-09-25, Isaac + Claude Opus 5.5)

Supersedes "Planned" in the Linking paragraph of the orchestrator chapter. `tools/autolink.py` tries every file
the report shows fully matched but `configure.py` still marks `NonMatching`: it flips it, rebuilds, and keeps
the flip only if the checksums pass. The merge script runs it after each merged file. First run: 15 of 26
linked (linked code 8.12% -> 8.26%). The 11 that fail are the finisher agents' first queue: `battle_party`,
`npc_data`, `seqdrv` (linker errors); `ac_table`, `item_data`, `swdrv`, `mot_walk`, `gxsub`, `synth_dbtab`,
`dsp_import` (checksum mismatches, so something the report doesn't measure still differs);
`unit_purple_teresa` (doesn't build when linked). The linked number moves in small steps until finisher agents
complete large files like `event` and `battle`.


## Preparing for the hard tail (2026-09-25, Isaac + Claude Opus 5.5)

What Snood's `tick` (3,680 B, 41% -> exact in about 20 commits) needed, and how TTYD gets the equivalent:
1. **A scoreboard for one huge function:** split into address windows, each scored separately while the whole
   function compiles. Port Snood's `link_diff.py --regions` when the >3 KB functions (120 of them) come up.
2. **Seeing inside the compiler.** Snood's compiler (agbcc) is open source, so its passes could be dumped. TTYD's
   (`mwcceppc.exe`, GC/2.6) is a closed binary. Candidates to get that back:
   [mwcc-rs](https://github.com/zcanann/mwcc-rs) (a Rust reimplementation aiming at byte-exact output, with
   profiles for GC/1.3-2.7 including 2.6, and `--emit-artifacts` to dump its intermediate stages; register
   coloring is its hardest part) and [JackPriceBurns/mwcc](https://github.com/JackPriceBurns/mwcc) (partial
   decompilation of Melee's version, with allocator notes). First experiment when the time comes: compile our
   matched functions with mwcc-rs's 2.6 profile and see what share it reproduces exactly.
   decomp-permuter already supports MWCC (Melee uses it).
3. **Accumulated knowledge:** an idioms file, near-miss log and hard-function notes, built up over the project.
4. **Patient, verified small steps by a strong model.**

Cheap now, expensive later, so do soon:
- **A per-file compiler-flag sweep.** Wrong flags make functions impossible (wave 3's eight `-O4,s` battle
  files), which looks exactly like "very hard" in the endgame.
- **Keep every attempt:** for each function that ends below 100%, its best version and what was tried, not
  just the agent's final file.
- **Start the idioms file:** fold each wave's "compiler idioms" report sections into one shared file.
- Keep fixing types in review and linking continuously, so those problems don't all arrive at the end.

## The plan for the hard tail, finalized, and the agent brief (2026-09-25, Isaac + Claude Opus 5.5)

Supersedes the "do soon" list in "Preparing for the hard tail". We have the real compiler (every check runs
`mwcceppc.exe`), but not its insides, so the plan is to understand it from outside and keep the tail small.

**Agents learn from past waves through `docs/AGENT_BRIEF.md`,** attached to every agent's prompt by the
launcher. It holds the rules and every mistake that compiled correctly but was caught in review (calls relying
on leftover registers, uninitialized variables, local declarations and struct copies that contradict headers,
unneeded pragmas, how to read signedness from the asm). When a review finds a new kind of mistake, add it to
the brief, and add a lint check where one is possible. The launcher's own prompt stays about the task.

**In order:**
1. **Now (small, compounding):**
   - A **probe tool**: compile a C snippet with a given file's exact flags and print the asm, so an agent (or
     the orchestrator) can run controlled experiments on the real compiler.
   - A **per-file compiler-flag sweep** (wrong flags make functions look impossible).
   - **Keep every attempt**: for functions that end below 100%, the best version reached and what was tried.
   - An **idioms file**, folded from each wave's reports, and headers fixed as reports and reviews find
     problems.
2. **Around 60-80%:** set up decomp-permuter against this build (it supports MWCC; the tower can run it
   overnight on the hardest near-misses) and fold in Melee's written knowledge of the compiler.
3. **Throughout:** rerun the mwcc-rs comparison every few months (`scratch/mwcc_rs_test/compare.py`; on
   2026-09-25 it compiled 14 of 124 TTYD files and reproduced 10 of 2,816 functions). Consider offering TTYD to
   mwcc-rs as a reference project.
4. **Only if the tail stalls:** a swarm project to reverse-engineer the 2.6 register allocator itself.

## Wave 4: new files, and a Claude reviewer alongside Codex (2026-09-25, Claude Opus 5.5; agents DeepSeek V4.1 Flash; review GPT-5.6 Sol and Claude Opus 5.5)

12 Pi agents each wrote a game source file that didn't exist yet (the build already lists these files; only the
`.c` was missing), 30-minute cap, with a new-file prompt (start from the file's asm, look up types in
`include/` first, build after one small function). All 12 merged: **3,114 -> 3,503 functions (+389), 15.50% ->
16.82% of code**, about 32 functions per agent against 17 in wave 3. `evt_pouch` (30/30) and `evt_map` (29/29)
are complete; auto-link tried `evt_pouch` and its data doesn't match yet. Unlike wave 3's files, most unmatched
functions left in the game are in files with no source yet (2,208 in game files, 2,764 in area modules), so this
is where the easy work is.

Merge fixes: `configure.py` must be rerun when a source file first appears (the agents' `check.py` and the merge
script do it), and the "touched other files" check must ignore the agent's own new file.

**Three review passes, each finding different things:**
- Codex round 1: calls relying on values left in registers (footmark calls, `nokonokoGetStatus`), an
  uninitialized ray distance, a struct overlay (`MapObjectTail`), an unneeded pragma, raw offsets.
- Codex round 2 (after a usage-limit pause): more register-carried arguments, two of them in code that was
  on `main` before this PR (`psndBGM_rate`, `partyEntry`), a signed/unsigned division, prototypes
  contradicting headers.
- **Claude** (a session on Isaac's work account, with six subagents): bugs in the *logic of unmatched
  functions*, which CI can't see and neither Codex round caught. Five flag constants shifted a byte too far
  (`oris 0xa1` read as `0xa1000000`), a landing path clearing the wrong Mario flags, a matrix concat into the
  wrong destination, a divide by 10 that should be 28, functions called with no prototype, and a half-right
  earlier fix. It also confirmed the header changes against every caller. Its independence was partial: it
  had read Codex's logs before the request arrived.

**Headers were often the problem, not the agents.** `mario_pouch.h` (19 accessors declared s16 where the original
uses int), `psndSFXOn`/`psndSFXOn_` (return values), `marioChgMot` (the MarioMotion enum's 0xFFFF sentinel makes it
16-bit), `psndBGMOff_f_d`, `marioSetForceMove`, `PartyEntry` (25 fields moved in from a local overlay). Each
change was checked by rebuilding every caller; two `#define` workarounds for the pouch header are gone.

**Review routine from now on (Isaac):** one Codex round and one Claude round per PR, rather than two Codex
rounds. They complement each other: Codex on patterns and call boundaries, Claude on the semantics of
unmatched functions against the asm. Claude reviews run in a session on Isaac's work account; messages to
it need his approval unless it runs in the same permission mode as the orchestrator.

**mwcc-rs, measured:** compiling all 124 TTYD source files with both compilers, mwcc-rs declined 110 and
reproduced 10 of 2,816 functions exactly (instruction scheduling differs from real 2.6 on the files it does
compile). Not usable for forensics yet; see "The plan for the hard tail, finalized".


## Everything TTYD needs now lives in this repo (2026-09-25, Isaac + Claude Opus 5.5)

Supersedes the "Where the tooling lives" paragraph of the orchestrator chapter and any `scratch/...` paths in
earlier chapters. The wave tooling had grown up in the orchestrator's scratch folder inside another project;
it's now here.
- **Committed, in `tools/`:** `ci.py`, `autolink.py`, `name_offsets.py`, `merge_agent.sh` (merge one agent's
  result: patch, new file or replaced file; CI, lint, commit, auto-link), `lint_patch.py`, `agent_check.py`
  (copied into each agent's workspace as `check.py`), `mwcc_rs_compare.py`, and `prune_agents.py` (the tower's
  cron runs it every 30 minutes over `~/ttyd-agents`, 50 GB cap).
- **In `scratch/` (gitignored):** each wave's launcher, prompts and results (`scratch/waveN/<agent>/`: REPORT.md,
  final.c or src.patch, the Pi session log, lint output) and review logs. The launcher stays out of git
  because it names the inference setup.

## Orchestration moves to another session; merge bar, cadence and limits (2026-09-25, Isaac; relayed by the incoming orchestrator, written up by Claude Opus 5.5)

From wave 6, a different Claude session orchestrates (it has more usage), and the session that ran waves 1-5
becomes the Claude reviewer. Only one session orchestrates at a time.
- **The orchestrator merges PRs itself,** using its judgement. The bar: `tools/ci.py` passes, then one Codex
  round and one Claude round, with their findings fixed. The `human-baseline` tag is the reference for "good
  enough": once two reviews have caught most of the bugs, merge.
- **Cadence:** one wave every 2 hours, driven by `/loop`.
- **When a usage limit is hit** (Codex's is expected first): pause the pipeline at the current step and tell
  Isaac. Don't skip a review, don't switch Codex to a larger model, and don't merge without the reviews.
- **Stray processes on the tower** (pending Isaac's decision): no new cron job. The launcher's `timeout` stops
  each agent; after each wave the orchestrator checks the tower for leftover `ninja`, `wibo`, `mwcceppc` or
  `objdiff-cli` processes under `~/ttyd-agents` and kills them. Revisit if it keeps happening.

## Wave 5: finishers link two files; a review finds the most bugs yet (2026-09-26, Claude Opus 5.5; agents DeepSeek V4.1 Flash; review GPT-5.6 Sol and Claude Opus 5.5)

24 agents: 22 wrote new game files, 2 were **finishers** told to make a fully matched file link. First wave with
`docs/AGENT_BRIEF.md` attached, and first merged with `tools/merge_agent.sh`. **3,503 -> 3,780 functions (+277),
16.82% -> 17.96% of code, linked 8.26% -> 8.45%.** The new files averaged ~12 functions each, against ~32 in
wave 4: harder files further down the list, and review found more wrong logic.

**Finishers work.** `evt_pouch` only needed its functions in the original order (all 30 matched, but a linked
file must match the original's layout). `evt_map` also needed its data reordered, a one-byte script constant
fixed (no function score showed it), and four globals placed with `#pragma section` (declaring them `const`
doesn't work; tested). Both link. The brief now tells every agent to write in the original order.

**Free matches from compiler flags.** Three agents noted their file wants `-O4,s`; setting it in `configure.py`
matched 11 more functions (`battle_mario` 5 -> 14 of 15). The brief now says to report this instead of
using a file-wide pragma.

**What review found.** Codex: one return value dropped by a `void` definition (`BattlePartyAnimeLoad`), local
prototypes contradicting headers. Claude (six subagents on per-file logic, cross-file checks in its own context):
twelve logic bugs in unmatched functions (a flag bit misread with IBM numbering, the `== N` carry idiom read as
`<` four times, an inverted clamp, a mis-scoped `else`, wrong constants), calls with no prototype reading
float results from `r3`, and two files (`sac_suki`, parts of `sac_genki`) wrong throughout. Ten functions from
those two were removed so they return to the queue; the rest were fixed and re-measured.

**New tooling.** `tools/lint_patch.py` now runs clang `-Wimplicit-function-declaration` (flag `IMPLICIT`), which
catches calls with no prototype. `tools/label_partials.py` keeps `NONMATCHING` labels in step with the build
(adds missing ones, updates percentages, rewrites false ones, records known issues). Headers gained prototypes
for a dozen functions defined in `src/` but never declared.

**Open, low severity:** `BtlUnit_snd_se` has three signatures across files (one belongs in `battle_unit.h`);
`test_kururing_mapdisp` is registered as a display callback but defined taking a `MapObject*` (unclear which is
right); `evt_sub` reuses `EventEntry`'s 0x88 as a pointer (needs a union); three known issues recorded in labels
(kinopio `Wait`, `sac_deka` `create_timing`, `party_yoshi` `L_getStick`); 17 local redeclarations of functions
their headers already declare.


## Review routine: sequential, so each reviewer also checks the last round's fixes (2026-09-26, Isaac)

Supersedes "one Codex round plus one Claude round" run in parallel (wave 5), which left the orchestrator's
own fixes unreviewed. Per PR: **Codex reviews -> the orchestrator fixes and commits -> Claude reviews the whole
PR including those fix commits -> the orchestrator fixes -> merge.** Codex goes first because its usage runs
out first and it does the broad sweep; Claude goes second because it reads logic against the asm more deeply,
which is what checking fixes needs. Only the last round's fixes go unreviewed, and there are usually few.
Wave 5's fix commits (ce5b249..59e9d2f) get this review after the fact.

## Ideas for wave 6: three review rounds, and no cross-session messaging (2026-09-26, Isaac; written up by Claude Opus 5.5)

Floated at the end of Sep 25, not decided; settle them when wave 6 starts.
- **No session-to-session messages for now.** Sessions messaging each other was hard for Isaac to follow.
  Wave 6 goes back to the earlier pattern: the orchestrator launches the wave, merges, and runs the reviews
  itself, or Isaac starts a review session by hand.
- **Three review rounds per PR instead of two.** Every round so far has found real bugs the previous one
  missed (wave 5: Codex, then Claude, then a Claude review of the fix commits, each with new findings).
- **A cheaper third round.** Run it on the swarm's own inference, with about 4 agents instead of per-file
  fan-out, as an independent pass. Slower, but that may be fine.
- **Pace.** Code matched went from 12.16% to 17.96% in the first day of waves. Melee took about six years, the
  last two fast. The pace from here depends on compute and on how quickly review can keep up.

## `tools/semdiff.py`: logic differences in partial functions, back-tested on past bugs (2026-09-26, Isaac; built by Claude Opus 5.5)

Most bugs reviews found in waves 4-5 were in functions that don't match yet, where the score says how close
the bytes are, not whether the logic is right. `tools/semdiff.py UNIT` compares each partial function with
the original as multisets of meaning-bearing tokens, ignoring register numbers, stack offsets and order:
constants, flag masks (rlwinm read as the value mask), logic immediates, signed or unsigned compares and
`cror`, calls, field offsets with width and signedness, a few meaning-changing opcodes (`divw`, `extsb`,
`fabs`, ...), and referenced data resolved to its contents, so an anonymous `@633` and `float_0_8041b020`
compare equal. Operations are compared by count; constants and data by presence, since the compiler loads
constants once or repeatedly as it likes. `tools/lint_patch.py` now reports its output as `SEMDIFF` leads
(not rejections), and also sees `USER_FUNC(name)` definitions, which its STUB and UNLABELLED checks had missed.

**Back-test** (`tools/semdiff_backtest.py`, ~12 s): 21 cases from wave 4 and 5 reviews. For each, the file is
compiled as it was at the commit before the fix and at the fix, in a scratch worktree with that commit's
flags; a case passes if the expected token is flagged before and gone after.
- 17 of 17 expected detections pass: flags ORed a byte too far, the hip-attack and paper-mode masks, `/10` for
  `/28`, `__div2i` for `__div2u`, `tan`/`fabs` with no prototype (int-to-double conversion of r3), the inverted
  clamp, three `== N` read as `<`, `0x1C000` for `0x20000`, `0x114d80` for -250000000, `<=` for `>=`, the
  zeroed stand-in array, `fabs` as a call rather than the instruction.
- 4 known limits, not expected to be caught: a wrong matrix destination (a stack address), an `else` on the
  wrong `if`, a missing case fall-through, and an argument left in a register by luck in a matched function.
- Hold-out, bugs not looked at while building it: 11 of 11 flagged with a relevant token (e.g. a missing
  `return 2`, a wrong field offset, alpha stored in the wrong byte, the 0xF00 mask, wrong SFX strings).
  Functions wrong throughout (sac_suki, main_weapon) show dozens of differences.
- Noise: on 79 partial functions wave 5's reviewers had checked and called correct, 13 are flagged, most
  with real code differences (the re-fetched `marioGetPtr`, and `fabsf` called where the original uses the
  instruction in `marioCreviceWallChk`, which no review caught).
- Over all 72 files the waves touched: 418 of 1,025 partial functions flagged, mostly from waves 1-3, whose
  partials were never reviewed for logic. A backlog for a review or finisher pass.

Add a case to the back-test whenever a review finds a bug in a partial function.

## Our tools will have bugs, and some will only show up late (2026-09-26, Isaac; written up by Claude Opus 5.5)

**Assume it.** Every tool in `tools/` was written fast, by models, against the cases in front of us. The ones
found so far were silent, not crashes, and each let things through for a while:
- `label_partials.py` never saw `USER_FUNC(name)` definitions, counted every "extra" instruction twice, and
  swapped the function name and text of every `--note` (so no note was ever written).
- `lint_patch.py`'s STUB and UNLABELLED checks never saw `USER_FUNC(name)` (event functions) through wave 5,
  and then missed `static USER_FUNC(name)`.
- `semdiff.py` (since parked, see below) read no rlwinm masks at all in its first version, and its Codex
  review found 13 more issues.
These are the easy ones, found because a reviewer looked. The rare ones will appear when the project reaches
situations nobody has tested: whole files linking, data and scripts, the area modules (RELs), the last hard
functions, a new compiler flag, a file layout objdiff reports differently.

**What to do about it:**
- **The build's checksums are the only oracle that can't be wrong in our favour.** A linked file hashing to
  `build.sha1` is proof. Everything else (scores, the ratchet, lint, semdiff, labels) is a tool that can have a
  blind spot. Weigh conclusions that rest on them accordingly.
- **When something is surprising, suspect the tool before the code or the agent.** A clean lint on a file
  that feels wrong, a score that doesn't move, a check that never fires.
- **Every tool gets tests on real past cases** (Melee's `tools/check/tests/` pattern; the parked
  `semdiff_backtest.py` on `wip/semdiff` is an example), and a new case whenever a bug is found in or by it.
- **When a tool bug is found, ask what it let through before now,** and re-run the fixed tool over earlier
  waves. E.g. the USER_FUNC blind spot means waves 1-5's event functions were never STUB- or
  UNLABELLED-checked.
- **Tool changes get reviewed like game code**, and each bug found in a tool goes in this ledger with what it
  may have let through.
- **No flag is not evidence of correctness.**

## `semdiff.py` parked: tried, and set aside for Melee's approach (2026-09-26, Isaac; written up by Claude Opus 5.5)

Supersedes "`tools/semdiff.py`: logic differences in partial functions" above. **Work in progress, not on
`main`:** the tool, its back-test and its Codex-reviewed revision live on the branch `wip/semdiff` (commit
`a7b5ed9`), with the review's findings and what was fixed in that branch's ledger.

Why it was set aside (Isaac's call): Melee, which finished this month and is the reference for how to do this,
solved the same problem (partial functions with wrong logic) differently. It builds and runs the non-matching
game: match-only tricks sit behind `MUST_MATCH`, a `--linkable` build links unfinished units, and bugs show up
as behaviour (Aug 2026: "fix inverted scale copy in non-matching builds", "fix infinite spin in non-matching
builds"). A homegrown heuristic that Melee never needed shouldn't sit in the merge path. `lint_patch.py` no
longer calls it; its `USER_FUNC` fixes stay. Revisit `semdiff` only if the runnable build can't be made to
work, or as a comparison against it.

## Melee's workflow adopted, and agents run the merge's checks themselves (2026-09-26, Isaac; another Claude Opus 5.5 session surveyed ~/melee and started it, Claude Opus 5.5 finished it)

From a survey of `~/melee` (Melee reached 100% and is the reference):
- **`-requireprotos` by default** (as in Melee's CI): a call with no prototype is a build error, not a lint
  warning. Turning it on found five real bugs (a call to a function that doesn't exist, a result read from a
  leftover register, a struct passed by value to a pointer parameter, an `f32` result read as an int, SDK
  macros called as functions) and put 30 missing prototypes in headers. 3,780 -> 3,785 functions.
- **`MUST_MATCH`** is defined in matching builds (not with `--non-matching`); code that exists only to reproduce
  the original bytes goes inside `#ifdef MUST_MATCH`. **`PAD_STACK(n)`** is in `include/matching.h` (not
  `macros.h`: the SDK already has one its sources include as `<macros.h>`).
- **`tools/tests/`** tests the checks themselves, starting with the three bugs found in them; `ci.py` runs them
  first. `ci.py` also ends with a note when units are fully matched but not linked.
- `ci.py`: non-`Object` edits to `configure.py` need `--allow-configure` (no longer implied by
  `--allow-protected`); `#pragma once` isn't a "new pragma"; `--allow-regress` also takes a unit path, for when
  fixing wrong code lowers a data score.
- Not done: Melee's `--linkable` build (building every complete unit into a runnable game). Few TTYD units
  are complete yet; revisit when finishers have completed more.

**Agents check their own work before stopping.** `check.py --final UNIT` (from `tools/agent_check.py`) rebuilds,
so `-requireprotos` errors show, then runs the merge's lint against the clean copy of the file the workspace
came from (`/dev/null` for a new file) and lists what to fix and how. The wave launcher template asks every
agent to run it before finishing. Most of what reviews caught mechanically in waves 4 and 5 should now be fixed
by the agent itself; the next waves will show whether it is.


## Wave 6: 19 files link, three review rounds (2026-09-26, Claude Opus 5.5; agents DeepSeek V4.1 Flash; review GLM-5.3 Flash, GPT-5.6 Sol and Claude Opus 5.5)

Isaac had this session orchestrate the whole wave, launch to merge. 24 agents for 30 minutes: 11 wrote new files,
13 were finishers, one for each file that matched fully but failed to link. **3,785 -> 3,844 functions (+59),
code 18.01% -> 18.38%, linked code 8.45% -> 9.35%, linked data 1.89% -> 4.11%.** (The merge commit's subject
says 21 files linked; it's 19.)

**Finishers are the cheapest progress we have.** All 13 linked, most in under ten minutes. Causes were
functions or data out of the original's order, missing data or strings, symbols with the wrong linkage, and a
REL (`unit_purple_teresa`) that had only its code. One new file (`battle_enemy_item`) linked on arrival.
**autolink was too strict:** it wanted 100% data in the report, but `item_data` and `npc_data` link with
byte-identical output while the report scores their data at 41% and 66%. It now tries every file whose code is
100% and lets the checksums decide, which linked 5 more files, including `event.c`. It also matches configure.py
objects to report units by module now (60 unit paths exist in more than one REL).

**Three review rounds, each followed by fixes.** Each found things the previous ones missed:
1. **Four GLM-5.3 Flash agents**, a quarter of the PR each, plus the orchestrator's own cross-file check. GLM
   covered about 5 of 24 files in 40 minutes and found one real problem. The cross-file check (every local
   declaration in the PR against headers and other files' definitions) found nine contradictions. Fixing them
   into headers matched 4 functions: three `cardmgr` functions had been correct all along under the wrong names,
   and `_btlcmd_CheckWeaponTargetNum` needed `BattleSamplingEnemy`'s bytes declared `s8`.
2. **Codex:** 3 bugs in the orchestrator's own tool changes (REL support in the merge script and lint could
   discard an uncommitted file or check the wrong module), an unmarked `goto`, a raw offset, a prototype
   mismatch, and an unlabelled partial function.
3. **Claude (another session, one message):** a bug introduced by the Codex fixes (every REL merge was
   rejected, deleting a new REL file), local struct copies of `BattleACDispParams` (now in `BattleActions`
   at 0x290), `volatile` float reads left unmarked in a linked file (now `MATCH_F32()` in `matching.h`), and a
   NONMATCHING comment that blamed the compiler for the source's own unsigned casts (fixing them: 93% -> 95.5%).
   It found no wrong logic in any partial function.

**Lessons.**
- Fix commits need review like everything else: round 3 found a high-severity bug that round 2's fixes
  introduced.
- The orchestrator's cross-file declaration check is cheap and productive. Run it on every PR before the
  reviews.
- Test tool changes on a real case (a REL merge on a throwaway branch) before relying on them.
- The tower sync must copy every tracked file, not a folder list: stale SDK headers under `libs/` broke the
  tower's build.

**Open, low severity:** `BattleActions.outputParams` is probably `s32` (compared signed); `MarioWork.multiTimer`
is read as `s32`; `wCamVal1` (0x2A8) holds an `s32` in `mot_hip` (noted in mario.h); many local prototypes
that agree with their definitions still belong in headers (`marioSetFallPara` and friends, `partyChgRunMode`,
`set_damage_root_ypos`, `mapGrpFlagOn/Off`); the OFFSET lint doesn't see raw offsets hidden in macros;
`BattleDrawEnemyHP`'s label needs a note on what's missing; 10 fully matched files still fail to link
(`battle.c`, `dispdrv`, `evt_hit`, `evt_msg`, `mot_stay`, five MusyX files), the next finishers' queue.


## Latent bugs, and two ways to estimate them, both parked (2026-09-26, Isaac; written up by Claude Opus 5.5)

Every PR will ship bugs no review caught, and we don't know what kinds we haven't seen yet (Isaac). The only
guarantee we fully trust is the checksum of a linked file (`build.sha1` comes from the disc); scores, the
ratchet and lint are our own tools. Two ways to measure what reviews miss, **shelved for later**:
- **Bug seeding:** plant a few realistic bugs in a PR, recorded privately, see what share the reviewers
  catch, and remove them before merging. That share estimates the reviewers' catch rate on real bugs.
- **Capture-recapture:** two independent reviews of the same PR; estimated total ≈ A's count × B's count ÷ the
  overlap. Needs reviews run in parallel, against the sequential routine, so it would be a one-PR experiment.


## Wave 7: near-misses in existing files, and the biggest code gain yet (2026-09-26, Claude Opus 5.5; agents DeepSeek V4.1 Flash; review GPT-5.6 Sol and Claude Opus 5.5)

Orchestrated end to end by the work-account session, by Isaac's choice. Before launching, the unmatched code was
broken down: 24% in RELs, 24% in game files with no source, and 32% in files that already have source, of which
functions at 80% or more were 7% of all code and unwritten functions 17%. New files had been getting harder (32
functions per agent in wave 4, 12 in wave 5). So the wave was reworked (Isaac approved): **14 agents on existing
files, told to finish near-misses first and then write missing functions** (a new prompt), 5 new files, 5 finishers.

**3,844 -> 4,024 functions (+180); code matched 18.38% -> 19.74% (+1.36, the largest gain of any wave); linked
code 9.35% -> 9.78%.** Existing files gave +146 (battle_event_cmd +66, mario_motion +13, evt_mario and
battle_audience +12 each); the 5 new files are fully matched (33 functions); all 5 finishers link.

**Findings from linking and merging.**
- `battle.c` and `dispdrv.c` were built without deferred inlining: with `-inline auto,deferred` MWCC emits
  functions in reverse source order and a file's .data before its jump tables. The finishers used
  `#pragma defer_codegen off`; configure.py now has `cflags_static_nodefer` for them instead. (evt_hit and
  mot_stay linked by reversing the source's function order, which is what the original source looked like.)
- Linking battle.c showed a switch with its cases mapped wrong at 100%: **objdiff doesn't compare jump-table
  data**, so switches in files that don't link can be wrong at any score.
- battle.c's `.data` split contains eleven jump tables of other files' switches; six functions are declared
  `extern u8 name[]` so the tables can point inside them. Known issue: the fix is moving those tables to their
  files in splits.txt.
- An agent cut off by the time limit left `mot_talk` subtracting `camYaw` twice (the ratchet caught the score
  drop); main's version was kept. Cut-off agents' partial functions need the closest review.
- The `.comment` section of the split objects (`build/G8MJ01/obj`) is written by dtk, not the original
  compiler: every object says the same version. It is no evidence about the original's compiler build.
- autolink listed units with no source file as link candidates: data-only units (and two MusyX data files) count
  as 100% code. On rel-setup it now requires a source file.

**Reviews: Codex, then two fresh Claude subagents**, each round followed by fixes, after the orchestrator's own
declaration check (which found a float argument passed through a leftover register in `partyMoveBehindMario3`).
- Codex: 8 findings, 5 real (StartAvoid compared the whole damage code instead of its low byte, so its effect
  never showed; the unit-work float commands converted numbers instead of copying float bits; five
  contradictory `BtlUnit_snd_se` prototypes), 1 false positive (a macro that already contains the comparison).
- Claude round 2: 3 High, all real and missed by Codex (both footmark clears wiped both party slots' buffers; a
  known bug in KinopioWait, one variable used as both period and step, whose "Known issue" note an agent had
  replaced with a false label), a linked file reading one past a table's end, file-wide pragmas instead of
  configure.py flags, labels.
- Claude round 3: no High or Medium. It checked every change in the fix commits against the asm and callers,
  and found two bugs in label_partials.py (a label for a function inside a comment; 99.996% written as 100.00%).
- label_partials.py itself had a bug the reviews exposed: a label one blank line above its function was missed
  and a second one added (nine in evt_mobj). All three are fixed, with tests.

**Lessons.**
- Near-misses in existing files are where progress is: the best code gain per wave, and finishing them removes
  the partial functions where every logic bug so far has been.
- Claude and Codex find different bugs: round 2 found three highs that round 1 didn't. Keep three rounds.
- A reviewer's suggested fix can be wrong in ways the reviewer didn't check (the macro case); verify every
  finding against the asm and definitions before changing code.
- Tools run between reviews (label_partials.py) need the same scrutiny as agent code.

**Open, low severity:** some match-only code not yet under MUST_MATCH (`unk_8014b824`, `party_roll`, two
Kinopio functions); `BattleSamplingEnemy`'s `s8 attackDirection` may be the wrong width; `BattleAudience_GetExist`
probably returns `u8`; windowdrv's two `unk_` names replaced descriptive ones; the three MusyX files that match
but don't link live under libs/musyx/src, which the launcher and merge script don't handle; the REL launcher.


## Wave 8: launched before wave 7 merged; reviewers wrong too (2026-09-26, Claude Opus 5.5; agents DeepSeek V4.1 Flash; review GPT-5.6 Sol, Claude Opus 5.5, GPT-5.6 Sol)

Orchestrated by the personal-account session while the work-account session finished wave 7. **4,024 ->
4,151 functions (+127), 1 file linked (evt_audience).** 18 agents took near-misses and missing functions in
existing files, 6 wrote new files; 30 minutes, 18:48-19:18. PR #9, merged ~20:10.

**Overlapping waves works.** Wave 8 launched at 18:48 from wave 7's branch tip, on files wave 7 hadn't touched,
in a separate git worktree (`~/ttyd-wave8`), because the other orchestrator was reviewing in `~/ttyd`. When
wave 7 merged 20 minutes later, wave 8's branch fast-forwarded: wave 7's last fixes touched none of its files.
Rules: a worktree per concurrent orchestrator; never sync the tower mid-wave; merge the later wave only after
the earlier one lands.

**Reviews: most real bugs of any PR so far, and reviewers were wrong too.**
- Round 1 (Codex): 8 findings. Its most useful one was that the lint file handed to reviewers was empty:
  `lint_patch.py UNIT origin/main` treated the ref as a file path and crashed on every unit (fixed; BASE is a
  ref whenever git knows it).
- Round 2 (Claude): 10 high findings, 9 of them logic bugs in partial functions (reversed colour bytes, swapped
  operands, `case 7` for `case 6`, a missing `case 3`, a missing reload, a loop one iteration too long,
  `pouchRemoveItem` always TRUE, a width that was always 0, wrong offsets kept "for the ratchet").
- **Round 1 was wrong once, and the orchestrator acted on it unchecked:** Codex said `fadedrv`'s "FadeEntry
  header bug" labels were false. They were right: the header's padding made `FadeEntry` 0xA4 bytes where the
  original's stride is 0xA8. Round 2 caught it; fixing the header matched 13 functions. Round 3 made one wrong
  claim too (a 0% function reported at 100%), caught this time by checking first. **Check every reviewer
  claim against the asm or the report before acting on it.**
- Round 3 (Codex): header types that code had worked around with casts (`BtlUnit_GetWidth/Height` return
  s32, `IconEntry.flags`/`iconId` u16, `NpcEntry.balloonType` s8), fixed in the headers.
- A few fixes cost a partial function a little (at most 0.65%) for correct logic; allowed with
  `--allow-regress`.

**Open:**
- Predating the PR: iconMain's uninitialized `data`, the empty `iconGX`, `L_getStick`'s -128 case.
- `mobjHitObjPtrToPtr` returns `MapObject*` but callers use it as `MapObjectEntry*` (retail reads
  `animName` at 0x15), which means `HitEntry.mapObj`'s type is wrong. (Resolved in the cleanup PR, 59abc86: it returns `MapObjectEntry*`.)
- icondrv's `iconDispGx`/`iconDispGxAlpha` take `Vec` by value where the original passes a pointer.
- `BtlUnit_CheckStatus` is cast to an `s8` parameter in battle_disp.
- Unmarked match-only tricks in evt_bero, evt_npc (`_intplGetFloat`'s regrouping rounds differently from the
  original), icondrv, battle_stage(_object), evt_twoddisp, evt_batstage, mario_balloon.
- Local `IconFrame` struct overlaying `IconBinaryEntry`, a third `CameraParams` copy.
- `ac_power_gauge_lv`, `ac_timing_wide` and `mot_dokan` are written in the original's function order, but
  under `-inline auto,deferred` they must be reversed to link.



## Local prototypes moved to headers; CI now rejects new ones (2026-09-26, Codex / GPT-6 Sol; finished by Claude Opus 5.5)

Isaac had Codex start with a batch of 100 and then continue; it ran until its usage limit. Result: the ~1,700
function prototypes that .c files declared for themselves are in the headers of the files that define them
(94 new headers), and the local copies are gone. 17 remained at first, 16 after review 1 (below), listed with reasons in
`tools/local_prototype_exceptions.json`: they are cases where the original's own files disagree (the
psndSFX_* argument widths three ways, psndSFXOnEx_3D, cloudGetStatus, BattleCheckCounter), two SDK stats
functions, and one match-only declaration (battle_stage_object's by-pointer windowDispGX_Waku_col, which only
steers register allocation; the original passes the colour by value like everyone else). fadedrv's `z`, first
listed as a name collision, is simply local in the original and is now static (review 1 of PR #10).
`tools/local_prototypes.py` (run by `tools/ci.py`) fails on any other non-static function declaration in
src/ or rels/, and on an exception whose declaration changed or no longer exists. CI passed after every batch
with all measures unchanged (4,153 functions); an independent count found exactly the 17 exceptions of the time.

**Agents now put prototypes in headers** (Isaac, Sep 26): the prompt and `docs/AGENT_BRIEF.md` (mistake 12) say to
add a missing prototype to the defining file's header, created if needed, and never to change existing header
lines. The launcher template collects each agent's header additions as `include.patch`;
`tools/merge_agent.sh` checks it with `tools/header_patch.py` (added declarations only), applies it and commits
it with the agent's file. `check.py --final` flags local prototypes, so agents fix them before stopping.


## Bug pattern: a local prototype hiding a missing argument (2026-09-26, Isaac; written up by Claude Opus 5.5)

`win_main.c`'s `itemUseFunc` declared `effRecoveryEntry` locally without its `s32 count` parameter and called
it without the count (how many heart particles to show). The compiler accepted it, because it only checks a
call against the prototype it can see, and the file's own copy agreed with the call. The score was 99.8%
because the original happened to have the value in `r4` already (it loads `item->hp_restored` there for a
zero test), so only a register name differed. Found by the cross-file declaration check; with the prototype
in a header the defining file also includes, the build would have refused it. Fixed in 59abc86 (the function
now matches). This is why local prototypes are moving into headers and why the agent brief's mistake 1 now
carries this example. A near-match whose only differences are argument registers deserves a look for a
missing argument.


## Claims for parallel waves (2026-09-27, Isaac + Claude Opus 5.5)

With more than one orchestrator, two waves could pick the same unit: the agent time of whichever merges second
is wasted (the ratchet rejects it), and header additions could collide. `tools/claims.py` keeps a shared
SQLite table of which wave owns which unit (`scratch/claims.sqlite` in the main checkout, shared by every
worktree). The launcher claims its units before starting agents and skips any another wave holds; the
orchestrator releases the wave's claims once its PR merges; claims expire after 24 hours so a crashed wave
doesn't lock files. The design follows itsgrimetime/melee-decomp's claims database for Melee agents.

Claims cover units, not headers. Two waves working on different units can both append to the same header, and
`header_patch.py`'s append-only merge only reconciles patches within one wave, so the second wave's PR will conflict
textually in git at that header's end. Resolve it by keeping both sides' lines. A REL unit is claimed without its
REL name, so the same path in two RELs is one claim.



## Reference repos, and study them before building tools (2026-09-27, Isaac; written up by Claude Opus 5.5)

Sources of truth for how a mature GameCube decomp does things, all cloned in `~/`:
- `~/melee`: Super Smash Bros. Melee (doldecomp), the main reference so far.
- `~/ac-decomp`: Animal Crossing.
- `~/SFA-Decomp`: Star Fox Adventures.
Also studied, not a source of truth: [itsgrimetime/melee-decomp](https://github.com/itsgrimetime/melee-decomp), AI-agent
tooling for Melee (claims database, worktree per agent, a self-hosted decomp.me).

**Working rule (Isaac):** before building a tool, look at how these repos solve the problem and reuse their approach
or code. Our own tools have had bugs in every wave; build only what they don't cover, and keep what we build as
small and read-only as possible. Example: Melee marks prototypes of undecompiled functions `UNK_RET fn(UNK_PARAMS)`
(src/placeholder.h) rather than guessing a signature.


## Review 1 of the cleanup PR: moved prototypes carried old bugs (2026-09-27, Claude Opus 5.5; review by a fresh Claude Opus 5.5)

The prototype move copied each local prototype into a header as it was, so local prototypes that were wrong became
the canonical declarations: missing arguments (`npcCheckInterrupt`, `_npcGroupDead`, `effItemGetEntry`,
`partyGetHp`, `tou_gamen_screen_tev`), `void` where callers use the result (seven functions, e.g.
`BattleAudienceAddAudienceNum`, `effSplashEntry`, `party_slit`), and an event function declared as a plain one
(`evt_paper_delete`). Four C callers were dropping real arguments and now pass them (two functions newly match).
17 functions that the original keeps local to one file had been put in headers (the sac_* files each have their
own `main_base`, `disp_2D` ...); they are `static` in their files now.

`tools/proto_asm.py` (read-only, a report, not a CI gate) compares header prototypes of undecompiled functions
with the original's asm: argument registers read on entry, callers using `r3`/`f1`, and `.fn NAME, local`. It found
every case the reviewer found by hand plus seven more, each then checked by hand. Its scan is rough (linear, up to
the first call), so a clean run is not proof.

The merge script applied agents' header patches with `patch`, which left `.orig` files, silently reversed an
already-applied patch, and would apply a patch aimed outside include/. `tools/header_patch.py --apply` now appends
each added line to its header unless already present (agents may only add lines, and patches from one wave
overlap, so context-based patching is the wrong tool), and rejects any path that isn't a header under include/.


## Reviews 2 and 3 of the cleanup PR (2026-09-27, Claude Opus 5.5; reviews by Codex and a fresh Claude Opus 5.5)

Review 2 (Codex) found three more local functions in headers (`paper_on`, `paper_off`, `evt_cloud_main`, missed
because `proto_asm.py` didn't read `USER_FUNC` declarations), header patches whose lines could land in the wrong
header, and claims loopholes (non-positive durations, `./src/x.c` vs `src/x.c`). That makes 20 functions made static.

Review 3 (a fresh Claude) found:
- The four sound functions whose callers disagree in the original (`psndSFX_pit`, `psndSFX_vol`,
  `psndSFX_get_vol`, `psndSFXOnEx_3D`) had no workable declaration for an agent: a local one was rejected, and a
  header one broke the files with their own widths. They are now exceptions for any file (`"path": "*"`), declared
  with the widths that file's asm uses.
- `static` hid unchecked prototypes: CI skipped every static declaration, and MWCC accepts a call to a static
  function that is never defined (a warning only). A static declaration now passes only if the original has the
  function local to that unit, or the file defines a function the asm doesn't have at all (inlined everywhere).
  This caught ten functions in battle_disp.c and DEMOInit.c that are global in the original; their prototypes are
  in the headers now (no change in matching).
- `BtlActRec_AddPoint` takes a `u8` (all four callers truncate with `clrlwi`), stale NONMATCHING labels, and smaller
  gaps in the header-patch, prototype and claims checks. The merge script now also runs `proto_asm.py` when an
  agent added header lines and prints any findings as a warning to check by hand.


## Wave 9: the other half of the first parallel pair (2026-09-27, Claude Opus 5.5; agents DeepSeek V4.1 Flash; reviews Claude Opus 5.5, GPT-5.6 Sol, Claude Opus 5.5)

Ran alongside wave 10 (TTYD Claude #2) from the same tower sync, in its own worktree (`~/ttyd-wave9`), units claimed
through `tools/claims.py`. 24 agents, 30 minutes, 15:21-15:51: 18 on the existing files with the most near-misses
that wave 8 hadn't touched, 6 new files (five small effects, one action command). All 24 merged. **4,155 -> 4,217
functions (+62), three new effect files linked.** PR #12, landed first (16:45); wave 10 merged onto it cleanly.

**Merging.** 19 of 24 merged on the first pass, about 6 s each. The four new effect files were rejected because
their agents created `include/eff/<effect>.h` holding the effect's work struct, which the declarations-only header
rule refused, although 19 of the 29 existing effect headers do exactly that. `tools/header_patch.py` (4c54563) now lets a
header the patch creates define types (struct/union/enum bodies, fields, enum members); reviews 2 and 3 (8c1acfa,
1077950) added the brace tracking, so a created header still can't define global data, function bodies or macros.
Lines added to existing headers stay declarations only. npcdrv's agent put a data extern in `npcdrv.h`; moved into the .c by hand.

**What the three reviews found** (every finding checked against the asm or the build before acting):
- Review 1 (fresh Claude): `animPoseSetAnim`/`animPoseSetEffectAnim` clamped an out-of-range index only on the
  string path (the original clamps both). Wave 9's own agent introduced it in ed09e9a, which added the numeric path. **Two of its "the named field compiles
  identically" claims were false**: the build dropped `btlevtcmd_GetPartyTechLv` 100 -> 96.71% and
  `psndMapChange` 98.86 -> 96.17%. The raw-offset forms went under `#ifdef MUST_MATCH` instead. An agent's `memcpy`
  (a call the original doesn't make) was reverted to the Vec assignment at a deliberate 89.57 -> 85.22%.
- Review 2 (Codex): **review 1's own fix had a bug**: `p = (u8*)&psnd;` went inside `#ifdef MUST_MATCH` while the
  function's tail uses `p` in both builds, so a `--non-matching` build would write through an uninitialized
  pointer. CI builds only the matching configuration and couldn't see it.
- Review 3 (fresh Claude, asked to compile every changed file without `-DMUST_MATCH`): the `#else` sides are all
  valid; one MUST_MATCH block had become unnecessary after review 2's fix (confirmed by CI, removed).

**The day's parallel-wave lessons** (waves 9 and 10 together):
- `check.py`'s plain `ninja` ran 18 jobs per agent; 48 agents took the 16-core tower to a load of ~550. `ninja -j2`
  dropped runnable processes from ~690 to ~110 within a minute of patching the running workspaces.
- The launcher runs under `nohup`, so nothing told the orchestrator wave 9 had finished; it sat done for 10
  minutes. The launch recipe now starts a tracked background wait on `launch.log`'s "all done".
- Landing was undramatic: the waves shared no headers, git merged the two common files, and the second PR's CI on
  top of the first came out at exactly the sum (4,217 + 81 = 4,298).
- **Near-miss files are running out.** Per near-miss agent: wave 8 about 5.9 functions (its total less review fixes), waves 9 and 10 about 2.5 (from the merge commits);
  new files did as well or better even when tiny. Waves 11 and 12 moved to missing functions and library ports.

## Wave 10: the first wave run in parallel with another (2026-09-27, Claude Opus 5.5 ("TTYD Claude #2"); agents DeepSeek V4.1 Flash; reviews Claude Opus 5.5, GPT-5.6 Sol, Claude Opus 5.5)

Ran alongside wave 9 (the other orchestrator): its own worktree (`~/ttyd-wave10`), its own inference key, units
kept apart by `tools/claims.py`. 24 agents, 30 minutes, 15:22-15:53: 18 on near-misses in existing files, 6 new
files. Merged 23 of 24; fontmgr was rejected (cut off with the file not compiling). **4,155 -> 4,236 functions
(+81) on its own; 4,298 after merging main with wave 9 in, exactly main's 4,217 + 81.** PR #11.

**Running two waves at once.** Both waves' agents compiling at once put the tower at load ~600 (16 cores):
`check.py` ran plain `ninja`, 18 jobs per agent. It now runs `ninja -j2` (patched into running workspaces
mid-wave; the template has it). The two PRs touched two files in common (configure.py, one declaration in
battle_stage_object.c) and git merged them without conflicts; the second to land merges main in once rather than
rebasing its ~30 commits.

**15 of 24 agents ran to the time limit.** Next waves get 45 minutes (Isaac).

**Merge-time fixes by the orchestrator.** Three agents put a struct in an existing header or changed a header
line, which header patches may not do; the structs were moved into the .c files and koopa_hit's `void` ->
`void*` (checked in the asm) applied by hand, each noted in its commit. Five files' agents used
`optimize_for_size` pragmas file-wide; they build with `-O4,s` in configure.py instead (no score change).

**What the three reviews found** (every finding checked against the asm before acting):
- Review 1 (fresh Claude): 6 high, all logic bugs in partial functions: an inverted condition and a missing
  `cont = 1` in battleAcMain_StickRotate, `|= 0x100` for `oris 0x100` (0x01000000) in Vivian's disp_off, score
  popups losing their last digit, `pouchAddKpaCoin` declared `void` when it returns the coin wrap (its callers'
  1000-point bonus was missing), and two in depthShadowEnd (`== 2` for an unsigned `> 2`; texture filters 1,1 for
  0,0). Its best medium: `iconDispGx`/`iconDispGxAlpha` take `Vec*`, not `Vec` (open since wave 8); fixing the
  header and 52 call sites matched iconDispGx and raised 14 functions in 10 files by 6-30 points. Also wrong
  header types `MarioWork.multiTimer` and `BattleActions.outputParams` (both s32) and two KpaWork copies merged.
- Review 2 (Codex): `marioHitCheckVec` returns the HitEntry*, not a BOOL; a third CameraParams copy (one in
  camdrv.h now); match-only code review 1 had only commented, moved under `#ifdef MUST_MATCH` as the brief asks.
  It re-checked review 1's fixes against the asm and found them right.
- Review 3 (fresh Claude): **float constants CI can't see.** Agents wrote 3.1416f etc. from the .sdata2 symbols'
  rounded names; the original holds 3.1415927. objdiff doesn't compare literal values, so functions at 100% had
  the wrong constant (GetRingOffset, getRidePos). Fixed in six files and added to the agent brief (rule 9).
  Also `smartReInit` is `L_smartReInit` in the original, so CI never scored it (renamed; an extra store removed),
  and `_get_icon_id` inlined where the original calls it (dont_inline: two functions 25% and 72% -> 94%).

**Lessons.**
- A score says nothing about constants in the literal pool; a float literal needs checking against the asm's
  `.float` even at 100%.
- A C function whose name differs from the original's symbol is invisible to CI: no score, no label, no ratchet.
- Header types were again the biggest lever: one prototype fix (`Vec*`) did more than several agents.

**Open:** ItemData.icon_id and discount_price are read with `lhz` (u16), and iconDispGx's iconId looks u16
(`clrlwi r5, 16`), but changing them moved battle_disp's scores both ways; battle_attack_audience's .sdata2 floats
stay writable (const makes MWCC fold them: 99% -> 73%); kpa_act's vec3_802fe200 keeps `__declspec(section)`;
evt_win reads the u16 fields through puns under MUST_MATCH; symbols.txt gives `kpa_fall` local scope though
mot_jump calls it; fontmgr (rejected) is back in the queue.

## Wave 11: missing functions first, and three Claude reviews (2026-09-27, Claude Opus 5.5 ("TTYD Claude #2"); agents DeepSeek V4.1 Flash; reviews Claude Opus 5.5 x3)

Ran alongside wave 12 (the other orchestrator, runtime libraries). 24 agents, 45 minutes, 17:19-18:05: 12 on
started files, told to write the **missing** functions smallest first instead of polishing near-misses, and 12
new files. Merged 23 of 24; evt_fade's agent died after 36 s on a provider error. **4,298 -> 4,477 (+179) at
merge, 4,481 after the three reviews' fixes; PR #13, merged first.** About 7.4 functions per agent, against
about 2.7 per agent on near-misses in wave 10; only 2 agents hit the 45-minute cap.

**Harness.** Each agent's `check.py` now copies the last source that built to `lastgood.c`, so an agent cut off
mid-edit still leaves a mergeable file. The orchestrator moved battle_break_slot's four struct bodies from a
header into the .c and replaced eight files' `optimize_for_size` pragmas with `-O4,s` (no score change).

**Codex was out of usage,** so all three reviews were fresh Claude sessions, each given what the previous ones
covered least. Every finding was checked against the asm before fixing:
- Review 1 (4 high): missing fall-throughs in seq_battleMain (cases 2-4) and two sac_bakugame switches, and
  bakugame's weapon left zero-filled where the original is initialised .data; also lightGetNearObj returns its
  count, and the msgWindow_* functions are void.
- Review 2 (1 medium): evt_offscreen zeroed its bounding box only under MUST_MATCH, though the original returns
  the zeros after five failed frames. Also one MessageData in msgdrv.h instead of three overlays, real types for
  offscreenGetTexObj, a missing smartFree in msgLoad (older than the wave), unk_8013be14 returning s32 (91% ->
  100% once written that way).
- Review 3 (3 high): ac_monosiri_target shifted a sprite twice and gave another the wrong z rotation;
  bakuGameMarioSurprise gave Yoshi Flurrie's pose and Flurrie Yoshi's, **which only the jump table shows**
  (objdiff doesn't compare jump-table entries; now brief rule 15). Also unit_party_christine didn't compile
  without MUST_MATCH.

**Lessons.**
- Pointing started-file agents at missing functions nearly tripled the yield per agent.
- Switches are a blind spot: fall-throughs and swapped cases both survive high scores. The last two waves'
  high-severity findings were mostly switch bugs.
- Compiling each changed file without `-DMUST_MATCH` catches `#else` branches that don't build; review 3 did it
  by hand.

**Open:** battle_actrecord, offscreendrv, eff_puniballoon, evt_offscreen and evt_light are fully matched but fail
autolink on a checksum (finisher work); battle_break_slot's .sdata2 constants stay writable (const: 68% -> 64%);
sac_bakugame's static weapon data is a `void*` array until BattleWeapon names enough fields; the BattleWork
+0x1F2C byte view; vivian_init_801809C0's name; fontmgr and evt_fade go back in the queue.

## Wave 12: the Metrowerks runtime libraries (2026-09-27, Claude Opus 5.5; agents DeepSeek V4.1 Flash; reviews Claude Opus 5.5 x3)

Ran alongside wave 11. The three libraries every GameCube game links (TRK_MINNOW_DOLPHIN.a, MSL_C.PPCEABI.bare.H.a,
Runtime.PPCEABI.H.a: 91 files in TTYD's splits) were never started, and ac-decomp (CC0) has most of them.
**4,298 -> 4,530 (+232) on the branch; 4,481 -> 4,713 on main after wave 11 landed first (PR #14, merged second).**
- **Import, no agents: +131.** configure.py got three library blocks (GC/2.6 with ac-decomp's flags, include paths
  and `-DFIX_SQRT_LINKAGE` only there, so no game unit's flags change). ac-decomp's 63 same-named sources were
  copied into libs/metrowerks/src/ and the 63 headers they include into libs/metrowerks/include/; autolink.py
  linked 36 files.
- **24 agents, 45 minutes, on the files that still differed: +100.** 15 merged on the first pass (targimpl +12,
  dolphin_trk_glue +10, most +1 to +5) and six new files after merge_agent.sh learned to recognise an agent's own
  new file under libs/ (CircleBuffer +14, main +9, UDP_Stubs +9, alloc, math_ppc, critical_regions). msghndlr made
  no change; MWCriticalSection_gc wrote a .cpp beside its .c and was rejected; ansi_fp's +5 was dropped (below).
  About 4.2 functions per agent.
- **Reviews: +1 net** (ansi_fp -5, then +3, +2, +1 from fixes). 45 library files are linked; 14 fully matched ones
  fail the checksum or the link (listed in a8165a1), several from the CircleBuffer/main split disagreement.

**Harness.** check.py now builds only the agent's object (`ninja -j2 <object>`) and scores it with a one-unit copy
of objdiff.json, the same scoring as the full report (review 3 compared 109 units) at about a tenth of the CPU:
48 agents re-diffing the whole project was most of the tower's load. It takes the unit's source from objdiff.json
(libraries, `FILE_POS.C`). Each agent's temp files go to its own `./tmp` (`TMPDIR`), pruned with the workspace.

**Agents on the network.** Nothing stopped agents fetching, and five did: tww, tp, bfbb, sms, pikmin, SFA-Decomp,
Melee, Prime. 21 of ansi_fp's lines came only from PrimeDecomp/prime, which has no license; the file went back to
ac-decomp's version and the branch was rewritten without those commits. Isaac's rule (brief rule 16): reuse from
CC0/MIT decomps is fine if the report names the source, downloads stay in the workspace, and no unlicensed project
or machine-wide search. `tools/provenance.py` lists every fetch, clone and path outside the workspace from the
transcript; the launcher saves it and merge_agent.sh puts it in the commit. Review 3 scored every library function
against 8 CC0 and 3 unlicensed projects: nothing on main follows an unlicensed one (scanf.c is bfbb/mkdd's; the
Melee URL the scanf agent tried was a 404).

**What the three reviews found** (every finding checked against the asm or by compiling):
- Review 1: fwrite missing its critical region, `__write_console`'s console-type test, an ARAM constant, the TRKEvent
  type, and the ansi_fp licensing.
- Review 2: provenance.py missed most fetch and search forms, with a test that tested nothing; two TRK functions
  one to four lines from 100% (TRK_main's MWTRACE, TTYD's 3-line TRKMessageSend); stale labels; three wrong claims
  in the wave 9 chapter.
- Review 3: more provenance.py holes (`git clone --depth 1 git@...` named the repository "1"; an empty transcript
  read as clean); ddh_cc_write matched only by indexing past one string array into the next, and the plain form is
  100%; the tools missed the upper-case `FILE_POS.C`; merge_agent.sh would have committed a stray `src/` copy of a
  library file.

**Wave 11 vs wave 12.** Both 24 agents for 45 minutes on DeepSeek V4.1 Flash. Wave 11 (missing functions in game
files): +183, about 7.4 per agent. Wave 12's agents: +100, about 4.2 per agent, on files the import had already
taken the easy functions from; the import itself (+131) took no agent time.

**Open:** the CircleBuffer/main split (main.c and CircleBuffer.c fail to link); the two ARAM functions (tp's CC0
version reaches about 97% with two one-line asm dcbi); `__cvt_sll_flt` (hand-written asm); float.c's link error;
the 14 unlinked library files; 16 small MSL and 4 TRK files never started.

## Upstream's first commit since the fork (2026-09-27, Isaac; done by Claude Opus 5.5)

doldecomp/ttyd had no commits between our fork point (2df569d, Jul 5) and 62131fc on Sep 27 ("Decompile Glitz
Pit Rankings", NWPlayer123, MIT: rels/tou2 and its configure.py entry). Isaac: cherry-pick it. Merged as PR #17
with `-x`, plus one commit making `rankingControll` static (the original has it local, and our CI rejects a
local prototype for a non-static function). +2 functions. Upstream commits can be taken the same way.

## Wave 14: most bugs any review has found (2026-09-27, Claude Opus 5.5 ("TTYD Claude #2"); agents DeepSeek V4.1 Flash; reviews Claude Opus 5.5, GPT-5.6 Sol, Claude Opus 5.5)

Ran alongside wave 13 (the other orchestrator, effect/ files). 24 agents, 45-minute cap, 20:04-20:44: 16 new
files and 8 started files (missing functions first). **All 24 stopped on their own after 12-40 minutes**; none
hit the cap. Merged 24/24 (three header rejects salvaged by moving structs and data externs into the .c).
**4,713 -> 4,837 at merge; 4,840 on main after the fixes and upstream's tou2; PR #15.**

Six agents fetched from doldecomp/ttyd. It is MIT and the fork's own upstream, and upstream had nothing in these
files past our fork point, so nothing was reused.

**Review 1 found 16 high-severity logic bugs, all in partial functions**, against 3-4 in earlier waves:
inverted tests (eight status checks in BattleChoiceSamplingEnemy, compareCamRoad's early return, kpa_swim's
`>=`), wrong masks and constants (0x40000 for 0x400000, 0x40 for 0x20, four in _btlSamplingEnemy, Yoshi for
Bobbery, 18 for 24, 90 for 120), a store to the wrong Vec, ConsumeItem striding itemDataTable by 0x1C instead
of 0x28, missing calls and passes, an uninitialised ray distance, and two placeholder bodies
(_fbatFirstAttackAnnouceDisp, written from the asm: 7.65% -> 99.6%; partySearchGround, removed and back in the
queue). Reviews 2 (Codex) and 3 (Claude) found five more: _btlSamplingEnemy's loops ran to a stale count,
sac_muki read its sound names past the end of its .rodata, evt_mobj_switch_float_blk posed the wrong object,
and three return types. Every file the wave changed now compiles without `-DMUST_MATCH` (mapdrv and
unit_party_christine-style breakage caught before review this time).

**Why so many.** Every one sits in a function below 100%, where nothing but a review reads the logic, and this
wave wrote more of those from scratch than any before (16 new files). The agents also stopped early instead of
rechecking their partial functions.

**What the reference repos do (Isaac asked).** Melee and Animal Crossing avoid the problem: a function is done
only at 100%, and partial attempts are rarely merged (Melee's build has 3 partial functions of ~19,800).
SFA-Decomp, which is agent-driven like us, keeps partial functions and built scans for exactly this bug class:
`tools/wrongvalue_scan.py` (same instruction sequence, different immediate or symbol: a wrong constant, mask,
offset or argument), `tools/a71_mnhist_scan.py` (per-function opcode histograms: an extra or missing
operation, not register colouring) and a whole-function load/store comparison keyed on displacement
(docs/aug05-A103-wrong-computation-sweep.md). SFA-Decomp is CC0. Nearly all of wave 14's high findings are of
the kinds these catch. Claude #1 is porting them (Isaac), to run in `check.py --final` and at merge.

**Open:** partySearchGround (513 instructions) and evt_fade/fontmgr back in the queue; calcCamRoad reads a
seventh float its caller never sets; MarioWork.poseTime is read with lhz though the header says s16;
lightGetNearObj takes a Vec by value where both callers pass their own Vec (maybe `Vec*`); BattleUnit+0x188 is a
0x1C-byte sound-mode record, not the header's guessed Vec; kpa_dash/kpa_walk compare the stick magnitude
unsigned where the original is signed (an s32 scores lower).

## Wave 13: effect files, and why reviews started finding so many bugs (2026-09-27, Claude Opus 5.5; agents DeepSeek V4.1 Flash; reviews Claude Opus 5.5, GPT-5.6 Sol, Claude Opus 5.5)

Ran alongside wave 14. 24 agents, 45 minutes, on the 24 largest effect files nobody had started (all new files).
**4,713 -> 4,745 (+32) on the branch; 4,840 -> 4,872 on main (PR #16, merged second).** Few whole functions (about
1.3 per agent) but code matched 23.72 -> 24.25% and fuzzy 32.06 -> 35.57%: effect functions are large and
float-heavy, and most ended at 80-98%.

**Merging.** 11 merged on the first pass. 12 were rejected because agents put their effect structs in existing
include/eff/ headers, which only created headers could do; `tools/header_patch.py` now lets any header gain whole
type definitions, applied as blocks and refused when a header already defines the name differently (reviews 1-3
closed further holes: tags vs typedef names, commented-out definitions, context lines inside a new struct). Data
externs agents put in headers were moved into their .c files. Four prototype clashes between agents in shared
headers (effSnowDustN64Entry, tri2, unk_80224298, effNumberGX) were resolved in favour of the defining file.

**Reviews found the most wrong code of any wave**, every item in a partly matched function where no score can see
it: 9 high in review 1 (GX arguments one register too high, ONE for ZERO, wrong vertex formats, missing calls, a
signed range test), 3 medium from Codex, and 13 logic bugs plus 8 wrong data tables in review 3 (three tables had
every word byte-reversed; a loop counter shared by nested loops so a pass never drew; a degenerate quad). Two of the
fixes were themselves wrong until checked against the asm (review 1's "light mask 0"; review 2's `[i - 1]`), so
every reviewer claim, including simple-looking constants, gets checked.

**Why so many (Isaac asked).**
- Detection: waves 13 and 14's reviewers scripted comparisons (call arguments, call order, data bytes, loads and
  stores) over every partial function; wave 11's review read by hand. Rerun after review 1's fixes, the same
  script still found missed bugs (kemutest's Disp 91.62 -> 98.37 once its duplicated branches were restored).
- Volume: waves 13/14 left 71 and 107 partly matched functions (wave 11: 53) and about twice the code. Effect code
  is mostly GX calls with enum arguments, which agents mistranslated systematically.
- Agents stopped at a median 18-21 minutes of 45 (wave 11: 29) but made more tool calls: the per-unit check.py made
  checks faster, and agents declare themselves done after one pass in every wave.


## semcheck: a wrong-code checker for agents, tried and dropped (2026-09-28, Claude Opus 5.5; reviews Claude Opus 5.5, GPT-5.6 Sol, Claude Opus 5.5)

**The idea.** Reviews of waves 13 and 14 found wrong code (constants, inverted tests, missing calls, wrong tables) in
one partly matched function in three to five, all invisible to a match score. Star Fox Adventures' decomp (CC0) has
scans for this (tools/wrongvalue_scan.py, a71_mnhist_scan.py, the sweep in docs/aug05-A103-wrong-computation-sweep.md),
so PR #18 built `tools/semcheck.py` on their rules: per partly matched function, flags for call arguments, constants,
float values, loads/stores by offset, instruction counts and data bytes, with source lines, shown to agents in
`check.py --final` (brief rule 17: fix what the asm confirms, spend spare time on the list).

**What the three reviews measured.**
- Review 1 planted 27 bugs in a fully matched file: the first version caught 15 of 22 (wrong floats, callees and
  strings were invisible under objdiff's `functionRelocDiffs=none`). After fixes, 26/26.
- Review 2 (Codex) planted bugs in five other units and found more gaps (pointer tables, a float changed to one
  already used, indirect calls, reordered GX writes, arithmetic swaps), and that objdiff leaves some unchanged
  symbols' bytes out of its JSON, which had produced false leads; semcheck grew its own ELF reader.
- Review 3 replayed two past reviews from before and after their fixes: **about 70% of the reviewers' real findings
  were flagged, but 951 of 989 flags were still there once every real bug was fixed** (roughly 9 in 10 are noise).
  Some noise would make an agent break correct code (nine switch-order flags asking to swap correct status IDs). It
  also found blind spots left after two rounds of fixes (tables starting with a zero byte, float `>=`/`<=` inverted,
  strings reached through a base register, stack-buffer aliasing on real code).

**Why it was dropped (Isaac).** The finished decomps are the source of truth: if they don't hand agents a one-size
checker, there is probably a reason, and here it was measurable. SFA uses these scans as a one-off sweep read by a
person; their own sweep of 204 partial functions found zero wrong computations. This codebase is large and varied,
and a general checker either misses things or buries the real ones in noise. Agents are capable of the grunt work if
told what to look for, so the brief lists the bug types reviews keep finding (rule 17), with no tool behind it.
PR #18 was closed unmerged; the branch `semcheck` keeps the code, the mutation harness
(tools/tests/semcheck_mutants.py) and the reviews' notes, should a narrower version be worth trying later.

**Lessons.**
- Before building a tool the reference projects don't have, find out why they don't. The sweep doc said it: fuzzy
  is a summary, and no gate can see a wrong computation.
- Measure precision on real history (before/after a review's fixes), not only recall on planted bugs: 26/26 planted
  bugs looked like success while nine flags in ten were noise.
- Things that did hold up: the planted-bug method for testing a checker, and that objdiff's JSON omits unchanged
  symbols' bytes (`data_diff`), which matters to any tool reading it.

## Wave 16: effect files, and data compared byte for byte before review (2026-09-28, Claude Opus 5.5 ("TTYD Claude #2"); agents DeepSeek V4.1 Flash; reviews Claude Opus 5.5, GPT-5.6 Sol, Claude Opus 5.5)

Ran alongside wave 15 (the other orchestrator, from its own copy of main on the tower). 24 agents, 45 minutes,
19:04-19:50, on the 24 largest unstarted non-n64 effect files; three hit the cap. The prompt added wave 13's effect
lessons: a work struct goes in the effect's own header as one block, prototypes go in the defining file's header,
data tables are copied word for word from the asm, and brief rule 17 before finishing. All 24 merged (four header
rejects salvaged; `effMObjBrokenEntry` returns its `EffectEntry*`, fixed in its own commit). **4,872 -> 4,902 (+30),
fuzzy code 40.24%; PR #21, merged first.** Effect files are slow: seven have no function at 100% yet.

**Data compared before review.** A short script (scratch/wave16/datacmp.py, pyelftools) compares every data object a
wave defines with the same symbol in the original object, relocations masked. It found five wrong tables that no
score shows: eff_updown's scale_data with every word byte-reversed and icon_id with its halfwords swapped, eff_levelup's
y_scale and eff_nozle's scale_dt missing their leading zero bytes, eff_sandars' first value in the wrong byte; plus
eff_mugi's Vec[3] where the original has three Vecs. All rewritten from the asm. This is the bug class wave 13's review
3 found by hand; the script is worth running on every wave.

**Reviews.** Review 1 found six logic bugs in partial functions (a matrix product in the wrong order, a radius step
that grew without bound, a mirror on the wrong branch, a counter never zeroed, swapped x/z, an icon drawn during the
intro) and a callback missing its second argument. Review 2 (Codex) found ParticleEmit never set its "use the second
Gaussian value" flag; the fix tipped MWCC into inlining EmitterEmit (88% -> 40%), undone with dont_inline. Review 3
found nothing above low. iconGetTexObj takes a u16 id now (the original loads it with lhz).

**Open:** ItemData.icon_id is read unsigned (lhz) everywhere, but making it u16 drops battle_disp (92% -> 81%), as in
wave 10; eff_naniga reads it through a u16 pointer under MUST_MATCH. 16 of the 24 files only extern their own
.sdata2 constants, so they can't link yet (eff_mario_balloon is 3/3 at 100%). eff_sheep's double_to_int is writable
(const drops the unit's matched data until the rest of its .rodata exists). Colour constants typed as u32 in batten,
sandars, nozle and spark; locals named after registers in several files.

## RELs: the first whole REL built from source (2026-09-28, Claude Opus 5.5)

Isaac asked for a hands-on look at the RELs before planning REL waves: decompile a function or two and find out what
prep a wave needs. It went on to a complete REL. **aaa.rel (Mario's house in the prologue) now links from our
source and matches its checksum: aaa.c (_prolog, _epilog, _unresolved) and aaa_00.c (three functions, 11 event
scripts, the map's tables, ~110 strings).** Linked data +0.17 points from one small map.

**The shape of the RELs.** 31 RELs, 606 units, 2,780 functions (910 KB code) and 4.3 MB data; 5 units configured
before this. A third of the functions are copies: 1,859 distinct bodies (700 KB), counted as identical instructions
once disassembler auto-names (str_, float_, ...) are ignored. Enemy units are compiled into every area that uses
them (unit_kuriboo in 12 RELs; 60 units shared, 156 copies), map helpers are copy-pasted (krb_get_dir: 25 copies in 14 RELs), and
every REL has the same entry file. Of the distinct bodies, 1,311 are 300 bytes or less. aaa_00 is typical: 720 B of
code against 8.5 KB of data, so data is most of the REL work.

**How a REL unit is written.**
- Event scripts are `EVT_BEGIN` macros, never hex. tools/ttydasm (upstream, Rust; `ttydasm extract -m aaa -s NAME
  -p .`) turns a script into macros, reading build/G8MJ01/<rel>/<rel>.plf. Its output compiled to the right bytes
  unchanged apart from `PTR()` on pointer arguments where the macro doesn't cast (a CALL's arguments after the
  first, SET_VALUE, SPAWN_EVENT_GET_ID; CALL_CHILD and SPAWN_EVENT already do). Fixed here: strings (it printed the
  NUL padding inside the literal, and its escaping wasn't C), ELSE's indentation, and floats, which it now prints
  as the shortest decimal FLOAT() encodes to the same word (-0.555, not -0.5546875; FLOAT stores value*1024
  truncated). It still treats every symbol as a script, so it can't do tables (it panics on ie_npc, a name list).
- Tables get real types with named fields. NpcSetupInfo and BeroEntry existed; DoorEntry (evt_door.h) and
  TreeAccessEntry/KusaAccessEntry (evt_sub.h) are new, named from how evt_door_param and the dol's tree and bush
  scripts read them; a field the dol code doesn't explain stays field_0xNN. unit_purple_teresa (wave 6) has its scripts as hex arrays; it matches but should be redone.
- Functions sit just before the first script that uses them: draw's constants are in .rodata between evt_prologue's
  strings and evt_prologue2's, the script that calls mapdraw. Strings are merged and placed in order of first use.
- The scripts call dozens of dol functions and scripts no header declared (35 for this one map); they now go in
  their owning headers (evt_map.h, evt_item.h and evt_bg.h are new). Worth doing for all RELs at once.

**What the compiler and linker need.**
- `#pragma pool_data off`: without it MWCC loads a function's constants through one base register; the originals
  load each one. dig_00.c already had it. It may belong in cflags_rel.
- The REL linker strips anything unreferenced that isn't in the generated ldscript's FORCEACTIVE (globals from
  symbols.txt), so an unused static (aaa_00's evt_door2) and the MSL floats extend_data makes need
  `#pragma force_active on`.
- `extend_data` indices must be unique across the whole REL: aaa.c and aaa_00.c both defining forcedata0 kept one.
- The objdiff score of a REL function is right, but a unit is only done when the REL's checksum matches; the object
  compare that found the problems is section bytes plus relocations resolved to section and offset.

**Prep before a REL wave, in order.**
1. Object paths (done for shared units, see the 2026-09-29 chapter): an `Object` with `modules=[...]` builds to
   build/G8MJ01/src/rel/<mods>/<name>.o; a plain object still builds to build/G8MJ01/src/<name>.o, so a plain REL
   unit named like another REL's or a dol unit collides ("Duplicate object name").
2. REL-aware agent tools: check.py, lint_patch, label_partials, claims and merge_agent assume MarioSt/ and src/
   (a scratch copy of agent_check.py needed three lines). Each REL needs rels/<rel>/include/.
3. ttydasm: tables, and a way to extract a whole unit's data in order.
4. The missing script declarations for all RELs, and the configure.py entries (scriptable from the splits).
5. A small test wave on one or two RELs before scaling.

**Open.** objdiff scores aaa/aaa's data at 92% although the REL matches (the "aaa_00" string is a 7-byte object, 8 in symbols.txt with its pad byte); the checksum
is the proof. The splits put evt_snd_* in party/party_motion.c (its .text runs to 0x8014CD30, evt_snd.c is only
0x80077EC4-0x80077F88), so sound declarations are split across party_motion.h and evt_snd.h; probably a splits fix.
evt_door.c keeps a local DoorParam (0x4C) for the same data as DoorEntry, and door_position_sub's parameters are
still hitIn/hitOut where DoorEntry says closeHit/openHit.
The three ttydasm fixes (strings, ELSE's indentation, floats) could go upstream to doldecomp/ttyd as a PR; not
done yet.

## Sep 29: system.c linked (Claude #2, Isaac's call)

Isaac asked for distABf, the last unmatched function in system.c (99.95%). Its instructions were already right;
what differed was data, and fixing it linked the whole unit (linked code +0.19 points, linked data +0.23).
- R_build_date and R_build_time ("Jun 17 2004", "17:20:01": __DATE__ and __TIME__) belong to main.c, not
  system.c. distABf's inline sqrtf loads its constants from the pool base, which the original puts 0x18 bytes in;
  moving the two strings into main.c (splits.txt changed) puts system.c's pool at the right place.
- The rest of the pool was short a debug format string, three doubles and an assert message from functions the
  linker stripped; stubs now emit them in order. An unused initializeVec added a 12-byte vector the original
  doesn't have and is gone.
- intplGetValue used PI (0x40490FDB) where the original loads 3.141592f (0x40490FD8). It scored 100% because
  objdiff doesn't compare literal values; only linking showed it. Worth remembering for any unit that scores
  complete but won't link.
## Every REL script, extracted and compiled back (2026-09-28, Claude Opus 5.5)

To size REL work, ttydasm was run on every script in the RELs, and each unit's scripts were compiled with MWCC
and compared with the original word for word, pointer targets included. **7,275 scripts (3 MB) in 564 units;
all extract, and 7,262 compile to the original (99.8%).** The scripts (scratch/rel-scan/: objs.py lists every
REL data object and picks out scripts, one.sh extracts one, roundtrip.py compiles and compares) are the check to
run before any REL wave.

Fixed on the way (ttydasm, evt_cmd.h): a word in the address range with no relocation is a value, mostly
EVTDAT_POINTER_MAX (-250000000), the "no value" argument battle commands take (5,454 uses; 971 scripts used to
abort); `-a ADDRESS` picks one of several same-named symbols (3,113 scripts share a name within their REL, e.g.
every enemy's attack_event); pointer targets resolved by symbol index, since in a REL built from our source the
anonymous string symbols repeat across objects and it printed the wrong strings; READ_INT2-4 and READ_FLOAT2-4
took one destination; DEBUG_PUT_MSG and DEBUG_PUT_REG were missing (designer debug prints, e.g. 「アニメ開始」).

Left: two scripts point into the middle of a table (las firebardesc, nok entry_unit_data; ttydasm prints the
table's name without the offset), and muj_05's command_seikai has three RETURNs and four END_SCRIPTs, which the
empty END macro can't write. Five scripts' strings have global dtk names (dat_*); they compare fine.

**Scope beyond the scripts.** The scripts use 1,010 dol names; 669 have no declaration (612 functions, 57 data).
REL tables outside scripts are 756 KB: battle about 400 KB (weapons 106 KB, battle groups and btlparty setups
~145 KB, unit/parts/defence/status 90 KB, pose tables 62 KB), NPC setups 61 KB, doors/entrances/trees/bushes
66 KB, MSL floats 43 KB (extend_data), the rest assorted (minigames, recipes, credits, splines). Strings are
452 KB and come with the scripts. Code is 910 KB, 1,859 distinct bodies. Next: a data wave, prepared by the
declarations sweep, the battle table types, ttydasm writing a whole unit, and REL object paths and tools.

## Sep 29: finishing the last function of nearly-linked units (Claude Sonnet 5.5, for the "Ambitious work session" orchestrator)

A unit links only when every function is 100% and its data and function order match, so units whose data was done and
that were one or two functions short were the queue. **15 units linked, `tools/ci.py` PASS: functions 4,902 -> 4,994
matched, linked code 11.19% -> 11.80%, linked data 4.94% -> 5.79%.** Each link was proven by the checksum ("32 files OK").

**Finished functions**
- `effThunderDisp` (eff_thunder_n64): the `type < 3 && type >= 0` test was a `switch` (cases 0-2 together, default),
  which MWCC lowers to the original's `cmpwi/bge/b` chain. The alpha-patched colour is a local inside an inlined
  helper, which puts its stack slot below the by-value temporaries. The unit also defined its own copies of the .sdata2
  floats, the .rodata double and the "ThunderN64" string that the code's literals already emit (so each existed twice),
  and its four colour words were non-`const` (.sdata, not .sdata2).
- `DVDMgrRead` (dvdmgr): the original's loop is not rotated; `while (1) { if (done) break; ... }` reproduces it,
  where `while (!done)` gets rotated.
- `partyChkJoin` (mario_party): the result passes through a temporary (`clrlwi r0; mr r3, r0`). All 14 functions were
  then 100% and it still would not link: under `-inline auto,deferred` MWCC emits functions in reverse source order, so
  the file is written in reverse (the same finding as evt_hit and mot_stay).
- `marioBalloonMain`/`marioBaloonInit` (mario_balloon): each hit-object block has its own `hit` variable (a shared one
  swapped r27/r30 from the second block on), and `marioChkBero` belongs to the third block's `else`, so the
  `0x00800000` path skips it. `marioBaloonInit` needs `#pragma opt_propagation off` to keep its 0 in r31 across
  `marioGetPtr` (needs `--allow-pragma`; the reason is commented at the pragma).
- `__close_all` (ansi_files): the loop sits between `__begin_critical_region(2)` and `__end_critical_region(2)`.
- 32-byte one-function units, written from scratch: `eff_butterfly`, `eff_hit`, `eff_stardust`, `eff_breath_fire`
  (forward to the N64 effect; three new `include/eff/n64/*_n64.h` prototype headers, types from the callers' argument
  registers) and the Sun fdlibm wrappers `w_asin`, `w_fmod`, `w_hypot`, `w_log`, `w_pow`, `w_sqrt`.

**Improved, not finished (left `NonMatching`, labels say what differs)**
- `effNumberGX` 97.94% -> 98.58%: a `case 3:` sharing the default gives the original's dispatch (compare 2, then 0,
  then 1); local declaration order does not change the remaining register allocation (32 permutations tried).
- `mbtowc` 95.62% -> 98.47%: nested ifs with one `return result`; the first `!s` test still compiles to `beq end`
  where the target has `bne over; b end`. `wcstombs` stays 98.36%.

**Tried without result:** `mot_hip` (one `fmuls` with its operands swapped; a dozen source shapes), `evt_cloud_main`
(FP register rotation in the screen-space math), `battleAcMain_CraneTiming` (the target keeps two base pointers,
`work+0x1F20` and `work+0x1F4C`, in registers; every derivation folds one into the other).

**Lessons**
- A unit that scores complete but will not link: compare its objects' section bytes with `build/G8MJ01/obj/<unit>.o`
  (pyelftools). Two causes came up: constants emitted twice (explicit definition plus the code's own literal) and
  function order.
- `switch` shape is a hint about the source: extra `case` labels that share the default change MWCC's dispatch tree.
- Registers assigned per variable: splitting one variable into one per block changes allocation without changing the
  code, and is worth trying on any "same instructions, different registers" near-miss.

## Sep 29: fully matched but not linked was mostly function order (Claude Sonnet 5.5, second near-miss round)

The list from `python3 tools/autolink.py --list` (30 units at the start) was almost all one problem: under
`-inline auto,deferred` MWCC emits a file's functions in reverse source order, so a unit whose functions are written in
the original's order comes out backwards, every function scores 100%, and the checksum fails. **16 units linked (15
from that list, `eff_irekaeru` from the constants problem below); code linked 11.80% -> 12.43%, linked data 5.79% ->
5.81%, functions 4,994 -> 4,995 (`effIrekaeruDisp`); `tools/ci.py` PASS, "32 files OK".** The list is down to 18.

**Diagnosing a unit that will not link** (throwaway scripts, not committed): compare the unit's compiled object with
`build/G8MJ01/obj/<unit>.o` with pyelftools: the order of its function symbols, and each section's size and bytes.
"Ours is exactly the reverse" means write the file's functions in reverse. A size gap of a few bytes at the end of
`.rodata`/`.sdata`/`.sdata2` is the original object's tail padding, not a difference.

**Linked by reversing function order:** `evt_case`, `evt_offscreen`, `evt_light`, `evt_fade`, `evt_lecture`,
`offscreendrv`, `battle_actrecord`; libraries `critical_regions.gamecube`, `math_ppc`, `UDP_Stubs`, TRK `dispatch` and
`support`, `scanf`. Notes:
- A function defined after its callers needs a prototype in a header (the CI check rejects local ones): four `_check_*`
  helpers in `battle_actrecord.h`, `balloon_0/1` in `eff_puniballoon.h`, `effMarioBalloonMain/Disp` in
  `eff_mario_balloon.h`. `static inline` helpers stay above their users; only definitions that are emitted move.
- `battle_actrecord`'s `dont_inline` pragma region moves with its functions, which `tools/ci.py` reports as new pragmas
  (`--allow-pragma`).

**Linked by fixing the unit's own constants:**
- `eff_irekaeru`: the source declared its .sdata2 floats as externs and defined its strings and zero vector as globals,
  so nothing defined the floats and the strings sat in the wrong place. The floats and strings are now the literals the
  code uses (MWCC pools them in first-use order of the emitted functions, which reproduced the original layout), the
  colour word is a `const u32`, and the alpha-patched colour is a local in an inlined helper, which puts its stack slot
  where the original has it (99.93% -> 100%, as in `effThunderDisp`).
- `eff_puniballoon`: `10.0` came before `0.1` in the original's .sdata2; writing balloon_0's `sin` result to a temporary
  (`f32 s = sin(...); scale += s * 0.1f;`) gives that order and every function stays 100%.
- `eff_mario_balloon`: its constants are deliberately read through `const volatile` externs (each use reloads). The
  definitions have to be *after* the functions (as its .rodata ones already were): defined above them, MWCC folds the
  reads into pooled literals and .sdata2 doubles.
- Zero-initialised data the original keeps in .data needs an explicit initializer (`InformantWork _jdt = { 0 };` in
  `evt_johoya`), otherwise it lands in .bss.

**Improved, still `NonMatching`:** `johoya_set` 99.43% -> 99.81% (only `extsh r0, r5` vs `extsh r5, r5`; the masked byte
goes first in the sum, `value` is cast to `s32` for the shift).

**Tried without result:** `npc_hide_onoff` (needs three saved registers; the `event ? X : X` return keeps the third
alive at the cost of a compare), `_hammer_star_effect` (the `lwz` of `userdata[0]` is scheduled before the float
arguments; reading it inline or in a block did not move it), `effUranokoMain` (f0/f1 swapped around `1.0 - scale`),
plus the earlier round's `mot_hip`, `evt_cloud_main`, `battleAcMain_CraneTiming`.

**Still unlinked (18):** `battle_event_subset` (no data written at all), `sac_common`, `unit_mario` and `eff_uranoko`
(link errors on constants their sources only declare), `evt_kinopio` (checksum), the four MusyX files, TRK
`serpoll`/`targimpl`/`mslsupp`/`main`/`CircleBuffer`, `alloc`, `printf`, `float`, `uart_console_io_gcn`. `eff_uranoko`
and `unit_mario` need the `eff_irekaeru` treatment.

## Sep 29: data-only units written from their disassembly, and the last library units (Claude Sonnet 5.5, third near-miss round)

**20 units linked; `tools/ci.py` PASS, "32 files OK". Relative to main after PR #25: code linked 12.43% -> 12.81%,
linked data 5.81% -> 6.46%, matched data 7.64% -> 7.73%, functions 4,995 -> 4,996 (`mailGetPtr`).** The "fully matched but not
linked" list is down to 6 (`battle_event_subset` was linked afterwards, see the next chapter). Every dol unit now has a source file (`configure.py` prints no "Missing source file").

**Data-only units, and `tools/asm2c_data.py` (superseded by `tools/dol_gen.py`, which writes scripts as EVT macros and puts
declarations in the owning headers; asm2c_data.py is only still needed for units with only .rodata/.sdata2).** Twelve dol units had no source at all: their data was linked from the
original object, which counts for nothing in matched data. `python3 tools/asm2c_data.py <unit> > src/<unit>.c` turns a unit's
disassembly (`build/G8MJ01/asm/<unit>.s`) into C: strings as `char` arrays sized as the original, words as `u32` arrays with
symbol addresses as `(u32)(void*)sym`, `.rel sym, .L_x` as `sym + offset`, numeric mixed-width objects as `u8` arrays,
loose padding as a static, and `__declspec(section)` for objects too small for their section (.data <= 48 bytes, .rodata
<= 8, .sdata2 strings > 6). Written and linked this way: `party_data`, `battle_audience_kind_data`,
`battle_database_common`, `seqdef`, `battle_evt_cmd`, `unit_bomzou`, `unit_koura`, `unit_system`, `unk_0x8025de70`,
`DEMOFont`, MSL `locale`, and (with a hand-written `mailGetPtr`) `mail_data`. The types are raw (`u32` arrays), not the
game's; they are a byte-exact starting point to name later. Lessons:
- A unit whose configure.py name doesn't match its split is invisible: `party/party_data.c` was the split `data/party_data.c`
  and three data units (`battle_audience_kind_data`, `battle_database_common`, `seqdef`) were not in configure.py at all.
  Add an entry at the unit's place in splits.txt order (link order follows the list).
- Data referenced only from RELs is dead-stripped from the DOL link unless it is under `#pragma force_active on`
  (`unk_0x8025de70`, `unit_system`). The same pragma on `unit_bomzou` and `unit_koura` breaks the link (the original
  had stripped some of their symbols), so it is per unit; a missing symbol shows in `MarioSt.elf` (pyelftools) and the
  DOL diff starts at the first shifted address.
- Definitions of `const` data referenced from code must not appear before the code in a file that reads them through
  named externs (see `eff_mario_balloon`).

**Other links**
- `sac_common`: function order (the previous round's recipe).
- TRK `main`/`CircleBuffer` (the "CircleBuffer" unit is really CircleBuffer.c plus the gdev_cc functions of EXI2_GDEV_GCN
  main.c, to be split later; its data belongs to the gdev part): splits.txt gave `main.c` the rodata, bss and sbss of `CircleBuffer.c`, which is why they
  "failed the checksum" for a day; `CircleBuffer.c` now owns its ranges (`.rodata 0x803022E0-0x803023C0`, `.bss
  0x803E4260-0x803E4780`, `.sbss 0x80418C30-0x80418C38`), defines its own buffers (they were externs nothing defined),
  and its two structs lose their padding fields (objects are 0x1C and 4 bytes). Its ratchet score first read 14.51%
  because the strings were literals and the objects oversized.
- `serpoll`: `gTRKInputPendingPtr` is in .sbss (`__declspec(section ".sdata")`, as in `main.c`).
- `mslsupp`, `alloc`: helpers the original inlines and never emits (`__read_file`..., `SubBlock_merge_next`) were emitted
  as functions; `static inline` fixes it, and MWCC keeps unused `static` (non-inline) functions.
- `uart_console_io_gcn`: `__close_console` belongs to `mslsupp`. `printf`: only its four public functions needed reversing.

**Tried without result:** `effUranokoMain` (f0/f1 swapped around `1.0 - scale`; matching it needs the unit's `.sdata2`
order 1.0 then 0.125, which no source shape gave); `npc_hide_onoff`; `_hammer_star_effect`. Reverted: `eff_uranoko` with
literals instead of externs (`Main` drops from 100% to 99.81%, which the ratchet rejects).

**Still unlinked (6):** `unit_mario`, `eff_uranoko`, `evt_kinopio`, `targimpl` (extra functions the original inlines; source order mixes asm
and C), the four MusyX files `synthmacros`, `synth_dbtab`, `snd_service`, `dsp_import`, and MSL `float`.

## RELs: 165 units linked from a generator, and wave 17 set up (2026-09-29, Claude Opus 5.5)

Isaac asked for an overnight REL wave. Prep showed most of it doesn't need agents: **245 REL units have no functions**,
only data, and a generator (scratch/rel-scan/gen.py) writes a unit's whole source from the original: the MSL floats
as extend_data (indices unique per REL), each script from ttydasm (`PTR()` where the macros don't cast), every other
object as a `u32` array marked `// TODO: type` (strings inline, interior pointers as base plus offset), in the
original order, all in one force_active block. Every generated unit was compiled, compared with the original object
(section bytes and relocation targets, strings by content; tools/rel_cmp.py) and linked. **164 linked with no
changes by hand; with hei_09 below, 165. Linked data 4.94% -> 13.94%.** Units left out: object names another unit
shares (enemy units in several RELs: the object path collision is still open), the 9 battle databases (don't
compile yet), and auto_* rodata units.

- A section a few bytes shorter than the original's is its trailing alignment padding; the linker adds it back.
- Scripts can end in padding after END_SCRIPT (ttydasm now stops there).
- The 660 declarations the scripts need were generated into the defining files' headers first
  (scratch/rel-scan/declsweep.py); three were dropped where the source defines the function with one argument.
- BeroEntry.nextareaArg1 is the destination map's name, not a number: linking hei_09 showed it.
- tools/agent_check.py takes REL units as MOD/UNIT; `--final` runs tools/rel_link.py, which compares the object
  and then tries the link, printing LINKED or NOT LINKED with what differs.
- The generated files' other tables are raw u32 arrays marked `// TODO: type` (256 in the first 118 units; 1,485 in
  198 linked files once wave 17's units are in): typing them is agent work, and the link keeps it honest.

**Wave 17** (scratch/wave17): the 104 unique-name REL units with 1-3 functions, none over 1.2 KB (34 KB of code,
732 KB of data). Each agent gets its unit's generated source (data done, verified; a `// TODO: write NAME here`
where each function goes, declarations so it builds), writes the functions, types the raw tables, and runs
`check.py --final` until LINKED. 32 agents at a time over the list, 30 minutes each, from ~/ttyd-w17base on the
tower (this branch plus the units' configure entries).

## Sep 29: the data units as readable source, and tools/dol_gen.py (Claude Sonnet 5.5)

The first version of the data-only units (PR #26) was raw: scripts as hex words, engine functions as `extern u8 name[]`,
strings as named byte arrays. Redone; every file still links (checksum, "32 files OK"):
- **`tools/dol_gen.py UNIT`** (modeled on `tools/rel_gen.py`, dol version) writes a unit's data from its disassembly:
  event scripts through `tools/ttydasm` as `EVT_BEGIN` macros (`PTR()` where the macros don't cast), other objects as
  `u32` arrays marked `// TODO: type`, strings inline as literals, headers found by searching `include/` for the
  names used (`extern s32 name[]` only where none declares it), `--force-active` for data nothing in the DOL references,
  and `--append FILE` to add a unit's data to a source that already has its functions. Strings are pooled by MWCC in
  order of first use, so the generator interleaves the `.data` and `.sdata` objects so each new string lands where the
  original pool has it, and an empty string is a pool entry like any other (`STRING("")` came first, the original had it
  last). It needs a linked build (ttydasm reads `MarioSt.elf`), so regenerate from a good tree, then swap the file in.
- **Written this way:** `unit_bomzou`, `unit_koura`, `unit_system` (scripts as EVT macros, tables as `u32`),
  `battle_database_common` and `battle_audience_kind_data` (tables), and `battle_event_subset`: 29 scripts (17 KB) as EVT
  macros appended to its six functions, which are now in reverse order like every deferred-codegen file. ttydasm can't read
  some `battle_item_*` tables that look like scripts; they are written as tables.
- **Typed by hand:** `party_data` is `PartyData partyDataTbl[20]` and three `char*[N][15]` pose tables (types from
  `party.c`'s externs; `PartyData` moved into `party.h`, and the 17 party functions the table names now have prototypes in
  their `party/*.h`); `seqdef` is `SequenceCallback seq_data_80302E40[8][3]` with the sequence headers' declarations
  (`seq_gameExit`, `seq_gameover.h` added); `mail_data` is a `MailEntry` table with literals.
- **Kept raw on purpose:** `battle_evt_cmd` is the jump table of a switch in `btlevtcmd_ACSuccessEffect`, which belongs with that
  function's unit; `DEMOFont`, MSL `locale` and `unk_0x8025de70` (its strings are used only from RELs).
- Objects the original keeps in `.data` though they are under 48 bytes need `__declspec(section ".data")`
  (`unit_bomzou`'s 8-byte `lbl_80398960`, which is padding).

## Sep 29: data written for units whose functions were done, and how MWCC orders literals (Claude Sonnet 5.5)

Units with nearly all functions at 100% but data never written can't link. `tools/dol_gen.py UNIT --append FILE` now adds a
unit's data to a source that already has its functions (generated tables/scripts as in the previous chapter), and does
the ordering work that linking needs. **`evt_door` (24/24 functions, 21 KB) and `evt_johoya` (16/16) link; the data of
`battle_mario` (64 KB `.data`, byte-identical), `evt_sub` (11 KB) and `evt_movefloor` is written but they stay `NonMatching`
on a partial function or a pool order** (below).

**What `--append` does.** It leaves alone objects the source already defines and compiler-made ones (jump tables, `Vec`
aggregate constants, float/double pool entries); scripts that name a scalar variable the source defines get `PTR(&var)`;
strings a script uses that another unit owns (`str_802C...`) are named (`STRING((void*)str_802C32C0)`, `extern const char`),
not copied as literals (the copy shows up as extra `.rodata`); a table the header declares as `EVT_DEF` is `s32` with `(s32)`
casts (`(u32)` in an `s32` array, or `(s32)` in a `u32` one, is "illegal constant expression"); the unit's global function
prototypes go into its header (CI rejects them in the `.c`), with `#include`s for the types they use; static ones are
forward-declared from their own signature.

**How MWCC orders literals** (worked out on `evt_door` and `battle_mario`; the generator's merge follows it):
- A *string in a data initializer* and an *aggregate constant in a function* (`Vec p = {0,0,0}`, the compiler's
  `vec3_802e....`) enter `.rodata` when parsed, in source order. Strings *inside functions* and `.sdata2` floats
  enter at codegen, in text order. So a file's functions and data have to be interleaved: functions in reverse text order
  (deferred codegen), data in address order, each function placed where its `vec3`s land in the original pool. The
  generator merges the two sequences greedily against the original's `.rodata` order.
- Text order is the reverse of definition order under `-inline deferred`, and prototype order changes nothing (swapping prototypes changed nothing).
- Weak `sqrt`/`sqrtf` statics (`_half$localstatic...`) in a unit's object are merged by the linker; they are not a difference.
- An object a `.c` defines by name (`const Vec vec3_802eeff0 = ...`) goes into the pool where it is defined; the original has
  it as a literal at its use. `evt_movefloor` and `battle_mario` had such named objects; they are literals now.

**Small fixes:** `npc_hide_onoff` is an index loop (`for (i = 0; npcs[i] != NULL; i++)`), 95.29% -> 100%; `johoya_set`
returns `s16` (100%; the return register is not extended, and `evt_johoya.h` said `s32`: one existing header line
changed); `work->speed *= -1.0f` keeps the operand order the literal form loses (`moving_floor_reverse` -> 100%).

**Not done:** `battle_mario`'s `.sdata2` has `1.0` (from `_mario_fire_ball_controll`) before `0.6` (`_fire_wave`), the reverse of
what MWCC gives when their text order is `_fire_wave`, `_mario_fire_ball_controll`; no source order gives both, and
`_hammer_star_effect` is 97.6%. `evt_moving_floor_init` 99.85% (the 0.5/1.0 pool order and an `fsubs` register). `evt_sub`:
four partial functions. `unit_party_clauda`: its weapon tables are typed `BattleWeapon` in `include/battle`, whose struct has
no room for the pointers they contain, and the struct is not to change tonight. `evt_kinopio` is full of named string and
pool-base hacks.

## 2026-09-29: shared REL units, one source for many RELs (Claude #3, orchestrator's request)

- An `Object` with `modules=[...]` builds to `build/G8MJ01/src/rel/<module(s)>/<unit>.o` (project.py `resolve()`); plain
  objects still build to `build/G8MJ01/src/<unit>.o`. objdiff unit names (`<mod>/<unit>`), report.json, checksums and
  dol paths are unchanged. `rel_link.py` and `rel_cmp.py` read the object path from `objdiff.json` `base_path`;
  `agent_check.py` already did. project.py refuses a name that is both plain and module-restricted.
- `Rel("common", [...])` in configure.py holds the shared units: one source under `rels/common/src/`, one `Object` per
  REL (`share_unit.py` adds them). The copies of a unit differ only in trailing split padding, so every copy compiles
  from the same source; three units first had v1/v2 sources that differed only in `extend_data` ids and were merged.
- The unit's exported data table is named per REL (`unit_X_<module id>_data_<offset>`) and referenced by that name from
  the module's battle database, so the sources call it `UNIT_DATA_SYM` and each `Object` passes
  `extra_cflags=["-DUNIT_DATA_SYM=<name>"]` (a REL that never references it gets `unit_X_local_data`).
- `extend_data(id)` is an inline function `forcedata<id>`; two files in one REL with the same id are merged by the
  linker and the second one's constants vanish (this cost gor 0x50 bytes of .data). Shared sources name their ids
  `<unit>_<n>`, which is unique by construction.

## 2026-09-29: review fixes for #25-#28 (Claude #3), landed on the #28 branch

- `sac_common` scripts are EVT macros with inline strings (its matched_data drops 100 -> 90.85 because inline literals get
  `@N` names, not the dtk `str_` names: allowed). `eff_mario_balloon` writes its constants as literals; the `const volatile`
  externs are gone and the unit still links.
- Engine functions and weapon tables the units used are declared in the header of the file that defines them
  (`battle_event_cmd.h`, `battle_event_default.h`, `battle_mario.h`, `battle_party.h`, `evt_sub.h`).
  `tools/dol_gen.py` now looks the owner up itself and only falls back to a stand-in `extern` (with a warning) when no
  header-owning source defines the name.
- `evt_sub_mail_event` is a macro script that ends in a bare `RETURN`; its END word is the separate `lbl_803080CC`.
  `tree_data` and `kusa_data` are `FLOAT(...)` rows. The `opt_propagation` pairs are back to the original scope: `off` from
  `evt_sub_get_coin` through `evt_sub_check_intersect`, and around `evt_sub_spline_get_value_manual`.
- The old `sequence/seqdef.c` stub is gone; the table is `seq_data`. `CircleBuffer.c` keeps its objects `static` under their
  real names.

## 2026-09-29: shared units with functions (Claude #3)

Linked so far (every copy by its REL's checksum): unit_barriern_satellite, unit_barriern_z_satellite, unit_borodo,
unit_green_chorobon, unit_gundan_zako_magician, unit_honenoko, unit_red_honenoko, unit_karon, unit_ura_noko,
unit_togenoko_ace, unit_jyugem, unit_hyper_jyugem, unit_heavy_bom, unit_kuriboo (10 of 12 RELs) and unit_patakuri (2 of 3).

- Workflow: `rel_gen.py MOD unit/unit_x`, write the function(s) at rel_gen's TODO hint (`fn_hint.py`; `tools/unit_fn.sh` does
  hint + includes + `share_unit.py` + configure), check with `agent_check.py MOD/unit/unit_x FUNC`, then
  `perm_decls.py` (declaration-order search: it found 100% for four functions that plain code left at 97-99%), then
  `rel_link.py` per REL. `fn_insert.py` copies an already matched function into another unit's source (the same static
  helper appears in several units: chorobon/honenoko/togezo `_copy_status`, `_krb_get_dir`, the barriern satellites).
- rel_gen no longer ends the file with `#pragma force_active off` when the unit has functions: with `-inline deferred`
  the function is emitted at the end of the file, where the pragma is already off, so its symbol lacks the force-active
  flag and the linker drops the (unreferenced) static function from .text.
- Codegen habits that decided matches: `id = evtGetValue(..); id = BattleTransID(event, id);` compiles with a `mr r0,r3`
  temporary, the nested call `BattleTransID(event, evtGetValue(..))` does not; separate variables for two ids;
  `mSizeChangeTurns/Strength` copy as `lbz/stb`; a `switch` with `case 3:` merged into `default` gives the pivot-at-2 shape.
  A field typed `s16` in a header but loaded with `lhz` (item icon) means the header's type is wrong: the fix is `u16`
  in the header (item_data.h's icon_id), not a local struct; unit_borodo.c's local ItemIcon view is a workaround to
  undo when that header changes (wave 10 found battle_disp drops if it changes alone, so fix its users together).
- Shared sources whose data refers to another unit's per-REL table name use `UNIT_REF_<UNIT>` with a `-D` per REL
  (`share_unit.py` does it).
- Solved later (see "Non-deferred REL modules"): the gon and mri copies were compiled without `-inline deferred`.
## Wave 18: the generated REL tables typed with existing structs (2026-09-29, Claude Opus 5.5 orchestrating, DeepSeek V4.1 Flash agents)

Claude #1 launched it at Isaac's request (one wave per orchestrator); Claude #2 collected and reviewed it. 151 of the
generator's linked units, one agent each (24 at a time, 20 minutes), replaced `// TODO: type` u32 arrays with struct
types already in include/, no header edits, and had to stay LINKED (tools/agent_check.py --final).

**140 units taken, 419 of their 738 raw tables typed; 96 units have none left. Bytes unchanged (32 checksums OK),
so the scores don't move; the gain is readable source.** 144 finished LINKED; 4 of those changed nothing; 7 ended
NOT LINKED and stay as generated (6 hit the time limit mid-edit, basabasa_green stopped with a file that doesn't
build). The launcher's other_changes.txt listed "Only in rels/X: include" for every unit: empty directories the
workspace setup created, not agent changes.

Typed as: BeroEntry 116 (every bero_entry_data/bero_data table), BattleDispPose 46, BattleDataEntry 40,
BattleUnitKindPart 30, BattleWeapon 25, BattleUnitDefense 21, NpcSetupInfo 19, BattleUnitDefenseAttr 18,
NpcBattleSetupEntry 15 (btlgrpsel_*), HitDamageReturn 14 (hitDmgRetPoints), TreeAccessEntry 11 and
KusaAccessEntry 11 (ki/tree and kusa/ueki tables), BattleVulnerableStatus 6, BattleUnitSetup 3, DoorEntry 2,
BattleStageObjectData 2, ItemType 1 (a -1-terminated key list).

**Left raw, and why: the header types, not the agents.** Nearly every leftover is a battle unit's own tables:
- BattleUnitKind: the unit's name string sits at 0x4, which the header has as `u8[8]`, and 0x10-0xB7 is one opaque
  block, so no unit could be typed without dropping a relocation.
- BattleVulnerableStatus (`regist`) is 0x16 bytes in the header; the objects are 0x18. BattleUnitDefenseAttr is
  5 bytes against 8. A few agents typed them anyway (the tail padding still links); most left them raw.
- BattleUnitKindPart's 0x24-0x37 is opaque; typed parts keep those bytes verbatim.
- 5-word pose_sound_timing_table entries have no struct.
Fixing those four headers (BattleUnitKind first) is the next step; after it, a rerun over the same units would
type most of what's left.

**Review (one fresh Claude, semantic):** no table has the wrong type; every typed table was traced to the command
that receives it (BeroEntry to evt_bero_*, NpcSetupInfo to evt_npc_setup, HitDamageReturn to
evt_hit_damage_return_set, BattleUnitSetup to btlevtcmd_SpawnUnit, ...). Fixed: false or contradictory comments
(three files claimed the 5-byte defence structs can't be used, while five others use them and link), a `PTR()` in a
C initializer, and a note on the Blooper arm's data table, which has no terminator and runs on into the next object
(the original's symbols have them as two objects). Left as inconsistencies for the rerun: part flags written as
symbols in some files and hex in others, item-select lists as ItemType or s32, `regist` typed in 6 files and raw in
30. More header follow-ups it found: BeroEntry.animeArg3/4 hold event pointers and strings (typed s32, so every
row casts); NpcBattleSetupEntry.field_8 is the stage-data pointer; BattleUnitKind.mParts should be
BattleUnitKindPart*; BattleWeapon's 0x0-0x18 is opaque (every weapon spells 1.0f as four bytes); BattleUnitSetup's
field_4 holds alliance, attack phase and position; BattleStageObjectData's offset comments are wrong;
ITEM_STEEPLE_KEY_0x19 is used at Riverside Station, so 0x18/0x19 may be the Station Keys. Full list:
scratch/wave18m/review.md (gitignored).
## Wave 17: 82 REL units with functions linked (2026-09-29, Claude Opus 5.5; agents DeepSeek V4.1 Flash)

104 unique-name REL units with 1-3 functions (none over 1.2 KB), smallest first; 24 agents at a time over the list,
30 minutes each, 00:41-02:35. Each got its generated source (data equal to the original; a TODO where each function
goes, placed before the first script that uses it; declarations so the file built) and the job: write the
functions, type what tables it could, `check.py --final` until LINKED. **82 LINKED** (each rechecked by a fresh
`tools/rel_link.py` at collection), all merged; 18 headers gained lines (7 new, several under the existing
include/rel/<mod>/ convention), none lost one. **Functions 4,994 -> 5,131; linked data 13.94% -> 23.67% on this
branch.** Small units took 2-4 minutes; the 2-3 function ones used most of the 30.

Not linked (22; the last 3 timed out after the first count; their generated data is still good, the functions
aren't):
- ~10 function mismatches (.text bytes or size differ): ordinary near-misses, plus two where the agent never
  wrote the function (hei_01, mri_13).
- 3 constant/string pool differences (.rodata size or bytes; a float literal in dou_04), the literal-order rule
  (see the grind chapters) applies.
- 6 where tools/rel_cmp.py says the object equals the original but the REL's checksum fails (unit_faker_mario_gra,
  unit_hyper_togekuri, unit_boss_rampell, unit_yami_togekuri, las_21, win_00). The compare misses something;
  first look shows the symbol tables differ (the original names every string, ours are anonymous), which
  shouldn't matter to the REL, so the cause is still open. Until found, only the checksum counts.
- The agents started from the old brief, before rule 18 (reverse function order); some multi-function units may
  fail only on that.

**Review of PR #30** (one fresh Claude): nothing high; fixed: rel_link.py restores the unit's previous state on any
failure (it used to leave it NonMatching), two duplicate prototypes, stale generator TODO comments in 7 linked
files, include/rel/mri/mri.h's 12 functions declared as USER_FUNC instead of `extern s32 x[]`, the counts above.
Follow-ups, not done: (1) several unit headers declare other units' symbols (aji_09.h/aji_16.h: evt_emergency from
aji_17; gor_04.h: luigi_gor_first_talk from gor.c; las_20.h: move_dai; unit_boss_gesso_right_arm.h; jin_10.h uses
dtk-generated names) and REL headers live in two layouts (include/rel/<mod>/ and rels/<mod>/include/), with
evt_iwamove in both, disagreeing; (2) the generator's stand-in `extern s32 X[];` lines (534 lines, 261 names; 44 are
functions such as hei_test_water_tev, 54 are dtk-made names that will break once their units are decompiled): a
declaration sweep for REL-local symbols like the dol one; (3) las_08.c exports a named global for a 0.7f literal
(float_0p7_las_00012688), a workaround to redo with the literal-order rule; (4) about 30 functions return bare 0/2
instead of EVT_RETURN_*; (5) BeroEntry.animeArg3/4 hold event pointers but are s32; (6) rel_gen.py overwrites a
unit's header on rerun.

- `cflags_rel` now has `-fp_contract on` like the DOL flags: without it REL code never fuses `a*b + c` into `fmadds`
  (bllizard's `eff_ice_barrier` needed it). All 32 checksums and the ratchet were unchanged by the flag. Also linked:
  unit_bllizard (bom, jon).

## 2026-09-29 (afternoon): non-deferred REL modules, wave 17 leftovers (Claude #3)

- **gon and mri compile without deferred inlining** (`-inline auto`): functions come out in source order, not reversed,
  and a function's literals are pooled where the function sits among the scripts, so its position matters
  (`tools/place_fn.py` finds it; run it on a module that has the right flags). This is why the gon/mri copies of
  kuriboo, patakuri and togekuri looked different: the first ledger note called it "0.0f pooled before the last string".
  configure.py: `cflags_rel_nodefer`, used for the whole gon and mri Rels; shared units in `common` get it per Object
  (`cflags=[*cflags_rel_nodefer, "-i rels/common/include"]`) and a `v2/` source with the functions moved.
  Other modules that fail with function-order or pool-order diffs may be the same (moo, hom, eki are still unexplained).
- Shared units can have real variants: togekuri's gon copy defines `hypotf` and orders its functions differently from the
  tik/hei copies (`v2/`), so the earlier "copies only differ in padding" held for the function-free units only.
- **The six wave-17 units where `rel_cmp.py` said equal but the checksum failed** lost their static functions in the
  link: the agents ended the file with `#pragma force_active off`, and with deferred inlining a function is emitted
  after the pragma has been switched off, so its symbol has no force-active flag and the linker drops it when nothing
  references it. `rel_cmp.py` now compares the per-symbol force-active flag of every function
  (".comment" section, bit 0x8) and says so. Fixed by removing the trailing `off` (boss_rampell, yami_togekuri, las_21,
  win_00, faker_mario_gra, hyper_togekuri).
- Agent hacks found and removed while linking their units: named `const` floats standing in for pool entries (mri_04,
  gon_12's `unusedA/unusedB`, custom_satellite's `VecData`/`double_to_int`), which existed only because the module's
  flags were wrong; the literals are back.
- Constant pool order follows the order of the constants in one expression (`dou_04`: writing `(1.5f * a - 1.5f) +
  (0.7f * b - 0.7f)` in one line pools 1.5 before 0.7 while the calls still run b first).
- `tools/adopt_agent.py` brings an agent's scratch directory into the tree; `tools/rel_diff.py` shows where a built REL
  first differs from the original; `tools/copy_fns.py`, `tools/place_fn.py` as above.
- Linked in this pass: unit_togekuri (gon, tik, hei), kuriboo (all 12), patakuri (all 3), bubble, hermos, phantom, teresa,
  jyugem/hyper_jyugem earlier, wave 17's 6 above plus badge_borodo, borodo_king, barriern_custom_satellite, boss_kanbu3,
  dou_04, gon_06, gon_12, mri_04, mri_08, hei_01. Written but not matching: wanwan/burst_wanwan (chain_init, chain_main at
  99.8%/99.7%: the first two loads swap r4/r5), ice_pakkun (`shadow_scale_return/small`: the argument setup for
  `intplGetValue` schedules differently, same as battle_seq_end.c), hammer_bros (helper functions inline unless wrapped in
  `#pragma dont_inline`; two of three at 88-89%). Still open in wave 17: eki_03/hom_00 (`test_glare`, `sun_tev`, `tokei`),
  moo_03/moo_04 (`mario_kirimomi`), mri_13, mri_16 (`ss_check` 86.9%).
- Review fixes for #34: `vivian_comeback` is static and returns EVT_RETURN_YIELD (no header, no `cats` pragma); the
  `ItemIcon` view of `itemDataTable` (the steal-item functions load `icon_id` with `lhz`; ItemData has it as s16, and
  changing it drops `btlUnitItemDisp` from 91.6% to 82.9% because its dead `(f32)BtlUnit_GetHeight` statement stops
  compiling like the target) lives once, in `include/data/item_icon_view.h`; `MarioWork.riderDelta` (+0x174) is named;
  agent pragmas (`opt_propagation`, `opt_common_subs`, `fp_contract`, `defer_codegen`) were removed where the literal
  and index-assignment forms above make them unnecessary; NONMATCHING labels added to the unlinked units. The
  `mri_puni:evt_make_texbuf` -0.01 (64.86 -> 64.85) is a side effect of compiling mri without deferral; that unit is still
  NonMatching.
## Wave 20: the battle structs completed, and the tables typed by a renderer instead of agents (2026-09-29, Claude Opus 5.5)

Isaac's go, via Claude #1: complete the battle structs, then type the ~2,000 raw `u32` tables (the generators' `// TODO: type`)
with them. Claude #1 suggested an agent wave for the second half; it turned out not to need one.

**The structs, each field from the code that reads it in the original asm.** BattleUnitKind (BtlUnit_Entry copies
most of it into BattleUnit, whose names it inherits; BtlUnit_GetCoin, BtlUnit_GetTalkTogePos, BSE_Fire,
btlseqTurnFirstProcess, _get_itemsteal_param, BattleDamageDirect name the rest), BattleUnitKindPart (btlDispMain's
base alpha), BattleWeapon (the 46 status bytes mapped case by case through _getSickStatusParam's jump table;
chances and costs u8, turns and strengths s8), BattleUnitSetup (its item drop table from npcSetupBattleInfo),
BattleGroupSetup, BattleSetupData (npcSetupBattleInfo reads nearly all of it), ItemDropData (merged with npcdrv's
duplicate NpcItemDropEntry). BeroEntry.animeArg3/4 and NpcBattleSetupEntry.stageData are pointers. The defence
tables (5 bytes) and BattleVulnerableStatus (0x16) were right all along: the "8 / 0x18" wave 18 saw is alignment
padding. Two corrections fell out: BattlePartyAnimeLoad reads the part's model name (the old mParts type pointed
it at the part name), and the part count is u8 (making it s8 cost two functions' scores, caught by the ratchet).

**tools/struct_render.py.** Renders a table as a typed initializer straight from dtk's disassembly of the original
(bytes, relocations, `.rel` labels; strings dtk calls vec3 recognized), so a rendered table can't hold a wrong
value; the checksum proves the rest. `--all` types the battle graph: roots by name and size (units, groups, party
setups, weapons, group selections, setup tables, spawn entries, the named tables scripts hand to btlevtcmd_*),
then every typed pointer out of them. Single objects become scalars, lists arrays; references (`x` / `&x`, index
arithmetic in the old raw element size), externs and casts are fixed across the tree, and a second pass renders
from the updated declarations. It is idempotent. `--check-layouts` compiles its layouts against the headers as
offsetof assertions.
- **1,834 tables typed** (245 weapons, 203 party setups and spawn entries, 183 groups, 146 group selections,
  110 units with their parts and defence/status/pose/data tables, 110 item lists, 18 string lists, 10 setup
  tables, ...). Raw tables: 1,973 -> 311. All 32 checksums pass; scores don't move (the bytes don't).
- **Left raw:** 214 audience animation bytecode lists (opcodes, want macros, not structs), ~30 item/key lists (want
  ITEM_* names), and ~65 mixed tables: stage data that dtk merged with the weapons after it (each REL's
  `*_btlstage_data` holds several stage records and hazard weapons; the fix is symbols in that REL's symbols.txt,
  then the renderer), pose_sound_timing_table (5-word records, no struct yet), btl_*_setup_no_tbl, map-specific
  shop/quiz/elevator data. None of these fit an existing struct, so an agent wave under wave 18's rules (no new
  structs) had little to do and wasn't run.
- **Skipped:** unit_purple_teresa (hand-built in wave 6: strings as named char arrays, scripts as hex) needs redoing
  before the renderer can touch it.

**Surprises and lessons.**
- Index arithmetic changes meaning with the type: nok's scripts spawn `&entry_unit_data[12]` in u32 units; typed as
  BattleUnitSetup that is entry 1. The checksum caught it; the renderer now converts such uses and refuses others.
- Casts in a non-linked unit aren't proven by any checksum: for NonMatching files the objdiff ratchet is the only
  guard (it caught the numParts signedness), and review 1 compiled battle_mario.c separately to prove its data.
- MWCC rejects a `const char**` inside a script's `PTR()`, so string lists are `char*`.
- The CI baseline in a long-lived worktree goes stale: re-accept it on main before judging a branch.

**Review (one fresh Claude).** 0 wrong names on ~60 checked against the asm; 2 high (the u8/s8 weapon bytes;
fix_refs could mangle sizeof, arithmetic, comments), 7 medium, 10 low; fixed except those listed next.

**Follow-ups:** stage-data symbol splits (then ~200 casts go away); BattleSetupData.field_0x20 is `[16][2]` in the
header but rendered flat; unit_system's split points its tables into the middle of `regist`; some BattleUnitType
names look swapped against the unit files (unit_boss_cortez_claw is UNIT_CORTEZ_SWORD, moamoa tentacle a/c) and want
checking against the game; audience bytecode macros; item-list ITEM_* names; unit_purple_teresa.

## Wave 19: REL units with up to 12 functions (2026-09-29, Claude Opus 5.5 "TTYD Claude #1"; agents DeepSeek V4.1 Flash)

71 unique-name REL units with up to 12 functions (none over 4 KB), from tools/rel_gen.py skeletons (rel_gen now
leaves a function's static locals, `name$NNN`, commented where they belong and no longer declares .bss objects
extern); 24 agents at a time, 30/45/60 minutes by function count, 16:05-18:00. The prompt had brief rule 18 and
the literal-order rule. **46 came back LINKED; 37 merged. Functions 5,255 -> 5,465; code linked 14.53% -> 15.73%;
linked data 40.72% -> 48.18%.**

The base predated #33 and #34, which cost 9 units at collection:
- #33 made BeroEntry.animeArg3/4 pointers: the agents' `(s32)&x_close` / `(s32)"door"` casts were dropped (18
  files). 7 enemy units whose agents typed battle tables with the old struct layouts no longer compile (too many
  initializers): las unit_boss_kamec_obaba, unit_boss_koopa, unit_boss_rampell_las; gor unit_lecture_christine;
  tik unit_kamec; tou2 unit_boss_champion, unit_kamec_red. Their functions are written (in
  scratch/wave19/<unit>/final.c); they need their tables re-rendered with tools/struct_render.py.
- #34 builds mri (and gon) without deferred inlining; mri_02 and mri_05 were written for the old flags (agents
  reversed their functions) and fail mri.rel's checksum: rerun with the right order.
- An agent's stand-in `extern s32 nannpc_ext_main_sub_fast[]` in dou_11 clashed with the prototype another agent
  added to evt_nannpc.h: the header line stays, the stand-in went.
25 not linked, and 4 units the generator couldn't write (las_03, tik_02, gon_11, las_14: .skip/.string data)
plus tou2 unit_wanawana (its own tanf) weren't queued.
Lesson: a wave's base should be rebuilt from main right before launch when other PRs are landing the same day;
struct and flag changes in between cost units at collection.

**Review of PR #35** (one fresh Claude): one high, fixed where it still links. The agents had brought back dtk-named
globals standing in for literals (`const f32 float_1p7_dou_00004750 = 1.7f;` and the like): written as plain
literals in dou_05, dou_11 and nok_01's floats, all still linking. Named `Vec` objects remain in nok_01, pik_01,
las_28 and tou_00 (6 of them): as compound literals the units don't link, because where such a constant lands
depends on where its function sits (see aaa_00's draw); moving the functions is the fix, not done. Also fixed:
nanNPCOption and nannpc_ext_main_sub_fast declared in evt_nannpc.h instead of `extern s32 x[]`;
mario->riderDelta instead of +0x174/+0x178 in aji_19 and dou_11; dou_10.h declares evt_dou_normal_puka as the
function it is (USER_FUNC); header lines left from the dropped units removed; the rel_gen.py fix this chapter
describes is now in the PR (it had stayed on another branch). Follow-ups: rsh_02.h declares fbatHitCheckAll as
returning a pointer (the original's callers read r3) while src/driver/npcdrv.c defines it void; muj_10.h keeps a
copy of evt_nannpc.c's NanNpcEntry/NanNpcWork (breaks once evt_nannpc.c links); ~83 bare `return 0/1/2` in event
functions; several tables tools/struct_render.py could now type (unit_mahorn, gundan_zako_group3, unit_kamec_*,
battle_database_las's 180 arrays).

## Sep 29 evening: pushing the autonomous setup (Isaac; written up by TTYD Claude #1)

Isaac's directive for the night (18:47): push the autonomous agents as far as they go before ~23:00, then review
how many waves ran and what a fully autonomous flow should look like. The goal is unchanged: a matching decomp with
reasonably clean code. Roles: TTYD Claude #1 (Opus 5.5, session "Ambitious work session") orchestrates two agent
waves at a time, its own and TTYD Claude #2's, launching the next from the backlog when one finishes, and merges
reviewed, CI-clean PRs (merging delegated Sep 29 afternoon); TTYD Claude #3 (Sonnet 5.5) works for Claude #1 on
code, the hard grind of linking functions (code over data: data has become mechanical). Usage headroom is ample
(resets Thursday). The knowledge base is growing (this ledger, the brief, the tools) and the reference decomps
of other games are there for agents to learn from. One concern to watch: agents poking around on the web;
Claude #1 checks each workspace's provenance.txt at collection and surfaces anything outside the repo.
Size estimate asked for tonight: the finished decomp is roughly 0.8-1.2 million lines (about 450k today: src 157k,
rels 183k, libs 99k, include 12k), about half of it event scripts and data tables.

## Wave 21: one untouched function per agent (2026-09-29, TTYD Claude #1; agents DeepSeek V4.1 Flash)

Isaac's idea: skip the scaffolding, give each agent one function nobody had touched, and see. The queue was the
smallest 0% function of each dol game file up to 1.5 KB (200 files, median 342 bytes; 136 files had no source, so
the agent created the file with just that function). 24 agents at a time, 30 minutes each, 17:44-19:32, from a
fresh base on the tower (with its own copy of build/tools: rebuilding a base re-downloads the tools, and
overwriting the shared ones fails while other agents run them). **158 of 200 functions at 100%, 32 at 90-99%, 9 at
50-89%, 1 dropped** (eff_syuryou: its data struct clashed with the one wave 19 put in eff_syuryou.h). **Functions
5,465 -> 5,623.** Small functions took 2-5 minutes.

Collected in one batch (all files, header additions unioned, one build, scores from the report) rather than
merge_agent.sh per unit, which runs CI 200 times. Fixed at collection: two agents declared effHokoriN64Entry with
different argument orders (ABI-identical; kept the order its caller uses); one agent's mobjdrv.c edit dropped three
other functions from 99.9% to 84-88% (the file keeps main's version; its mobjCalcMtx attempt was 83.6%).
Web access: 33 agents fetched other TTYD decomps (doldecomp/ttyd, MIT; one also cloned Rainchus/ttyd-decomp-us,
whose license we haven't checked) to see whether a function already existed upstream; their reports say nothing
was copied. Several also listed or read paths outside their workspace (~/ttyd, other agents' workspaces), which the
brief forbids; the per-agent provenance.txt files in scratch/wave21 have the details. Worth tightening in the
prompt: say what's allowed to fetch.
Near-misses (90-99%) for Claude #3: scratch/wave21/scores.json.

**Review of PR #37** (one fresh Claude): no wrong code in the 17 partial functions checked against the asm; no
function scores lower than on main; all 239 new declarations in the defining file's header; nothing copied. Fixed:
8 missing or stale NONMATCHING labels (all from the 12 agents that wrote no REPORT.md); a duplicate new header
include/driver/himo.h (vivihimo is in himo.h) and include/sequence/seq_gameover.h (folded into seq/seq_gameover.h).
Follow-ups: data reached as offsets past another symbol (brief rule 13) in battle_seq.c:284 (_rule_disp),
sac_genki.c:49, sac_deka.c:132, battle_status_icon.c:112-117; item_data.h's ItemDataView shadow of ItemData; two
different effKemuri2Data structs (eff_kemuri.h, eff_kemuri2_n64.c); unmarked match tricks (static volatile const
Vec in battle_disp.c, const volatile data in eff_blow_n64.c, a base+0x34 pointer in eff_rothammer_fail_n64.c, an
unreset opt_propagation off there); dont_inline pragmas agents wrapped around neighbouring existing functions in 7
battle files. Six battle files now carry optimize_for_size pragmas (battle_information, battle_message,
battle_pad, battle_unit_event, battle_seq, battle_damage): candidates for -O4,s in configure.py instead.
What agents fetched, fully: doldecomp/ttyd and several MIT forks, AxioDL/musyx (MIT), SeekyCt/spm-decomp (CC0),
jdaster64/ttyd-tot (GPL-3.0, one header read), Rainchus/ttyd-decomp-us (no license: off-limits), sourcegraph and
decomp.me; nothing borrowed. 48 agents also used /tmp, which the brief forbids; wave 23's prompt says to stay
inside the workspace.

## Grind 9-10: MusyX/TRK/MSL linked, battle_database units, tooling (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3")

Dol near-miss and fully-matched-but-unlinked units. Linked: StdReverb/reverb, hw_memory, hw_dolphin, snd_service,
synthmacros, synth_dbtab, dsp_import, TRK targimpl, MSL float.c; plus grind-9's jon_06, end, dokugassun and the
battle_database units of jon/eki/gon/gor/gra/hei/jin/mri/tou2.

- **`symbols.txt` name.** `U_HandleReverb` was the name of a function that is `HandleReverb` in the source; the
  mismatch scored a matching function 0%. Renamed.
- **SAL_HOOKS has three slots** (malloc, mallocPhysical, free), not SND_HOOKS's two: `salHooks` is 0xC bytes and
  `sndSetHooks` copies malloc into both of the first two. salInitDspCtrl and salInitAi call `salMallocPhysical`; the
  DSP PB is 0xC4 bytes (a low-pass block `_PBLPF` at 0xBA, `streamLoopCnt` moved to 0xC2).
- **Two `align:32` data splits** (dsp_import, hw_dspctrl). An asm neighbour's `.data` start is 32-aligned in the
  original (it holds a 32-aligned object); without the split flag the object before it was 0x10/0x18 short and every
  later `.bss` address moved. `tools/dol_diff.py` shows this as a run of words differing by the same delta.
- **hw_dolphin** keeps `salWaitForDSPThreadQueue` as a zero-initialized `u64` cast to `OSThreadQueue*`: a real
  `OSThreadQueue` with an initializer goes to `.sdata`, without one to the end of `.sbss`, and the original has it
  between `hwIrqLevel` and `salDspInitIsDone`.
- **Report 100% is not link-ready.** synthmacros had two switch cases (0x5e/0x5f) calling swapped functions: the
  jump table is in `.data`, which the code score doesn't see. The dsp_import DSP microcode in the tree was another
  MusyX revision (7488 B; retail is 7872 B), so the unit was 100% of the wrong data.
- **targimpl** builds with `-inline auto,deferred`: non-asm functions are emitted in reverse source order, asm ones in
  place, and literals pool in source order, so the source order is reconstructed from the retail addresses (asm
  first, then the C functions reversed). The four TRKTarget helpers the retail build inlines and never emits
  (Versions, SupportMask, CPUType, FlushCache) are deleted from the file, not kept: msghndlr.c (NonMatching) is the
  only caller and links from the asm object. The TRKPPCAccess* helpers are `static`, so they inline and vanish. The
  asm bodies were moved, not written (MetroTRK's own).
- **hw_dspctrl** `salBuildCommandList` 83.1 -> 84.7%: the retail mix control uses per-channel bits (0x1/0x2/0x4,
  0x10/0x20/0x80, 0x200/0x400/0x1000) and compType 6; the first-time setup and the update path (roughly source lines
  760-960) still need porting. `HandleDepopVoice` matches. `hwRemoveSample` (99.1%) keeps `len` in r6 where we use r5;
  HARDWARE stays NonMatching for it.
- **extend_data ids** (`tools/ci.py`, per rels/<mod>/src): two files of one REL with the same `extend_data(id)`
  produce one `forcedata<id>` inline; the linker merges them and the second one's constants vanish (jon_06's ids
  0-2 against another jon file's, found as a gor/jon `.data` size shortfall). Ids in shared units are `<unit>_<n>`.
- **battle_database_jon** is typed now (struct_render pipeline run on that file alone). Grind 9 left it raw because
  the render stopped compiling; the pass that fixes references and casts (run_all) was what it needed. v2
  unit_dokugassun is typed the same way; `struct_render.asm_path` resolves `rels/common/src/vN/` variants through
  configure.py's `source=`.
- **Tools:** `tools/dol_diff.py` (changed words with the symbol they fall in), `place_fns.py`, `mod_gen.py`
  (base unit of a REL: refuses to overwrite).
- **CI baseline** `build/ci/accepted.json` is per worktree and goes stale when the worktree changes branches:
  `python3 tools/ci.py --init` on a clean checkout of main before trusting a ratchet failure.

## Real asm in the original libraries (Sep 29, Isaac asked for a note; written by TTYD Claude #1)

The rule "no inline asm: a match produced by pasting assembly is worthless" (brief) has one legitimate exception:
functions the ORIGINAL library was written in asm. PR #36 is the first to rely on it and needed `ci.py
--allow-asm`: MetroTRK's targimpl.c has 9 `asm` functions (they are asm in the original MetroTRK; they moved, their
bodies are unchanged), and MusyX's reverb.c `DoCrossTalk` is `static asm` in AxioDL/musyx (MIT) too, so its edit
there is legitimate. The review checked every asm block against that standard and found no paste.
How to tell the difference: an asm function is legitimate only in library code (libs/: MetroTRK, the runtime, the
SDK, MusyX), and only when another source of that library (a public decomp such as AxioDL/musyx, or the SDK's own
sources) writes the same function in asm; hand-scheduled code with no compiler prologue pattern is a hint, not
proof. Game code (src/, rels/) was compiled C: asm there is always a paste. Reviews of any PR that needs
`--allow-asm` should name, for each asm function, the source that shows it's asm in the original.
(Also on this date: linked data passed 50%, 50.77% at 0e624af.)

## Grind 11-12: -O4,s battle files, four more links, hw_dspctrl (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3")

- **The six optimize_for_size files** (battle_information, battle_message, battle_pad, battle_unit_event, battle_seq,
  battle_damage) are built with per-Object `extra_cflags=["-O4,s"]`, like battle_detect and its neighbours; the
  pragmas are gone. Every function scores the same or better (battle_pad 32 -> 73%, battle_unit_event 91 -> 99.75%).
- **Linked:** eff_confetti, evt_env (autolink), seq_gameover (the evt data appended with dol_gen; `sys_00` and the
  empty string must be defined/used where the original pools them: `""` is a sdata2 literal, not an object),
  unit_object_switch (data appended with dol_gen; `_data` sits between damage_event and defence).
- **hw_dspctrl `salBuildCommandList` 99.5%** (was 83.1): the retail mixer control is per-channel (0x1/0x2/0x4 L/R/S,
  0x8 delta; 0x10/0x20/0x80 AuxA with 0x40/0x100 delta; 0x200/0x400/0x1000 AuxB with 0x800/0x2000 delta; 0x4000 for
  the non-STD studio), the low-pass block is updated on changed bits 0x400/0x800, cycles use `dspMixerCyclesMain`
  indexed three ways (+0x229 when the low-pass is on), and a compressor command (0x12 with `compressorTable`, 6720 B
  of `.data`) follows the master-studio loop. What is left is register colouring: `endAddr` is r7 in the original
  (r5 here) in every loop case, and the compType 6 loop-buffer end address associates `(len + base) - 1` differently.
  A declaration-position search for `endAddr` changed nothing.
- **Not fixed (wave 21 near-misses):** BattleInformationSetDropMaterial (the `i = field_0x8` load is scheduled into the
  compare block in the original; hoisting it in the source did not do it), cylinderShadowDraw (GXColor temporaries
  swap slots and the display-list calls end in a different branch shape), _lvup_spot_move (intplGetValue argument
  scheduling, as before).

## Grind 13: synthvoice linked (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3")

MusyX synthvoice.c: retail splits `voiceAllocate` into a static `voiceAllocateFind` (returns the voice with direct
`return voice` at each exit, one shared fail label for the priority check, `-1` as s32) and two thin callers,
`voiceAllocate` (unlinks the voice from the free list and adjusts the running counts) and `voiceAllocatePeek` (returns
the stolen voice's allocId). voiceAllocate is inlined into `voiceBlock`, which sets `allocId = 0xFFFFFFFF`. Also
written: `vidGetPublicId`, `synthKillVoicesBySampleReferences` (sampleId, MusyX >= 2.0.1); `vidRemoveVoiceReferences`
updates the master list's root too; `vidMakeNew` restarts the list walk when a fresh vid ends up below the current
node (`goto retry` to the start of the walk; the compiler reuses the `vidRoot` it already loaded). The new code
is gated like upstream: the split at MusyX >= 2.0.1 (Peek at 2.0.3), vidGetPublicId, the restart and the master-list
root update at >= 2.0.2; the pre-2.0.1 `voiceAllocate` stays under `#else`, untested (the project builds 2.0.3).

## Grind 14: HARDWARE linked, aram/vsamples ported to the 2.0.2 API (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3")

- **HARDWARE.C linked.** Three functions the source lacked: `hwGetSampleExtraData`, `hwSetFilter` (filter on/coefs and
  changed bits 0xC00/0x800/0x400) and `hwLowPassFrqToCoef` (needs `musyx/snd.h`, or `sndCos` is an implicit int
  function; the constant is exactly 2*pi = 6.2831855f, and the clamp is one nested ternary). `hwRemoveSample`'s
  `aramInfo` parameter was live: `aramRemoveData(data, len, aramInfo)`. Gates: `hwGetSampleExtraData` 2.0.2,
  `hwSetFilter`/`hwLowPassFrqToCoef` 2.0.1 (as the header).
- **hw_aramdma** (>= 2.0.2): `aramStoreData`/`aramRemoveData` take the `ARAMInfo*` (no `aramWrite` global; the stream
  buffer allocator reads `dataARAMDefaultGetInfo()->aramWrite`, declared in synthdata.h), `aramGetUserBytes` and
  `aramGetFirstUserAddress` are new, `aramInit` uses the physical allocator; the older API stays under `#else`. The
  aram side is gated at 2.0.2 because `hwRemoveSample`'s `aramInfo` parameter is gated there in this tree (upstream
  gates the aram side at 2.0.1). `aramStoreData`'s loop waits on the queue first (`aramSyncTransferQueue()`, >= 2.0.1),
  which took it from 94% to 100%: all 14 functions match. The unit stays NonMatching on layout only: the original's
  `aramUploadData` sits right after `aramQueueCallback` (ours is emitted after `aramStoreData`, where it is first
  inlined) and `aramQueueLo` precedes `aramQueueHi` in `.bss` (ours the reverse; declaration order and first-reference
  order made no difference). `aramExit` is gone for >= 2.0.2 (the retail build has none), `aramQueueInit` is inline.
- **synth_vsamples**: `SND_VIRTUALSAMPLE_INFO` gains `vid` and `seqID` after `instID` and the union grows a `start`
  member (`extraData`), so `VS_BUFFER` is 0x2C (info is 0x1C long, the union at +0xC); compType 6 (16-bit PCM,
  `off1 = last * 2`) in `vsUpdateBuffer`; `vsSampleUpdates` called `voiceInStartup` (no such function) where the
  original calls `hwVoiceInStartup`. `vsSampleStartNotify` fills the start info and frees the buffer when the
  callback returns non-zero; it is 95.6%: the original keeps three copies of the voice id (raw r29, u8 r27, a
  re-extended r30) and one more callee-saved register (`_savegpr_26`); a `u8 voice` plus a `u8`/`u32 hwVoice` copy did
  not reproduce it. The unit stays NonMatching for that one function.
- **snd3d** (not started): 0/12 functions match; the emitter/room structs are another MusyX revision (flags at 0xC, no
  room/door roots).

## Grind 15: synthdata linked, small game-code links (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3")

- **synthdata (and s_data) had no source paired with them.** dtk named their splits `data/musyx/runtime/synthdata.c` and
  `data/musyx/runtime/s_data.c`, so objdiff compared the original object against nothing (0% for all 26 functions)
  while `libs/musyx/src/.../synthdata.c` compiled to an unrelated unit name. Renamed both splits to
  `musyx/runtime/...` (config/splits.txt is a protected path: the rename is the only edit): synthdata is 26/26 at 100%
  and linked (7.4 KB). s_data is another MusyX revision (the group stack with `gsDefault`/`ARAMInfo` at +0x60c:
  `dataInitStack` 15%, `sndSeqPlayEx` 68%, `seqPlaySong` 73%, `sndPushGroup`/`sndPopGroup` 91%); it stays NonMatching.
- **unit_object_tree** linked: `_rumble_tree` (same as `_rumble_switch` on `mPosition.y`), the unit data generated with
  dol_gen and typed with struct_render; with `-inline deferred` the source order is the reverse of the text order
  (`_rumble_tree` above `_turn_tree`).
- **effSmallStarCamChg** written (forwarder): eff_small_star linked. **evt_paper_entry** 99.2% (r30/r31 swap of
  `name`/`inBattle`), **mot_yoshi_post** 76% (the two rodata Vec copies are scheduled differently; a non-const source
  Vec would explain it but lands in .data), **evt_twoddisp** `twoddisp_disp` 95.3 -> 98.9% by declaration order (the
  rest: an `extsh` on the icon id and the GXColor temporaries' stack slots; changing `iconDispGx` to a `u16` id fixes
  it there and regresses ac_timing_wide and others).

## Grind 16: SDK units and small unwritten game functions (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3")

- **DEMOInit linked.** `DEMOGetCurrentBuffer` was named `L_DEMOGetCurrentBuffer` in symbols.txt (renamed: a protected-path
  edit, the only one). Fixes: the non-AA pixel format is `GX_PF_RGBA6_Z24`, the two hang-message buffers are both 4096
  bytes with `fullmsg` declared last, `DemoStatEnable` is `u8`, the "invalid TV format" panic reports line 518 (the retail
  file was longer: `#line 518`), and the uninitialized statics are declared in the reverse of their `.sbss` order.
- **DEMOPad linked** from the reference SDK source (`DEMOPadCopy` is global in the original; the reference declares
  `DemoNumValidPads` before `Pad` so the uninitialized statics come out in retail order).
- **arc.c** (the ARC archive reader): `ARCConvertEntrynumToPath`, `ARCGetCurrentDir`, `entryToPath`, `myStrncpy` and the
  getters are written from the DVD reference (`dvdfs.c`) with the FST/strings pointers cached in locals; the
  `myStrncpy` char is `signed char` (the dol builds `-char unsigned`, the SDK signed). 8/10 at 100%; `ARCInitHandle` 99.98%
  (registers); `ARCConvertPathToEntrynum` 72.7% (the original keeps far fewer registers live). `ARCOpen` keeps the SDK's
  `0 > entry ||` (the retail build has the extra `blt`). `ARCClose` takes the file info now.
- **BattleInformationInit** matches: the four setters are `dont_inline` (they are real calls in retail, including
  `BattleInformationSetResult`, which is defined after `Init`). **romFontGetWidth** 98.4% (the `wp` and halfword loads swap r6/r7). Retail adds the previous glyph's width after a newline or an
  unknown byte (a bug in the original), so neither branch has its own continue or load.
- **battle_monosiri** and **battle_acrobat** linked (Claude #3). Acrobat: `-O4,s` for the stmw prologue; the window test is
  `cur > start && cur <= end` (not a chained compare); `BattlePadCheckTrigger`/`RecordTrigger` return `s32` (the tests are
  `cmpwi`). `btlataudGetMoveEventPtr` matches (`case 0: break;`); `_attack_aud` 99.7% (`args` lands in r31, target r29).
- **battle_monosiri**'s table is 0xC4 with no padding: `eff_fukidashi`'s `.data` starts at 0x8036BEA0 with `align:16` (the
  12 bytes before it are that alignment). The `{-1, 1}` pair is `eff_fukidashi`'s.
- **mot_hip** linked (`14.0f * marioGetScale() / 2.0f` gives the target's operand order).

## Grind 17 (Claude #3)

- **battle_unit_event** linked: `BattleRunPhaseEvent` entered the event only when the code pointer was NULL (`if (!code)`); retail has `if (code)`. Declaration order `valid` before `i` in `BattleCheckEndUnitInitEvent`.
- **eff_scanning**, **eff_uranoko**, **eff_peach_fukidashi** linked. `effScanningMain` calls `animGroupBaseAsync` (the
  commented-out call was real) and copies its `Vec` through a second local; `effScanningDisp` written. The `fmuls f1, f0, f1`
  operand order of `-cam->viewYaw * deg2rad` comes from `rad = -cam->viewYaw; rad = deg * rad;` (a `rad` temp). A pooled
  constant order (1.0 before 0.125) came from `f32 k = 0.125f;` (uranoko). Named `.sdata2` constants: `extern const f32` in the
  uses, then the definitions in address order after the last function, so the code loads them by symbol and the section
  matches; the functions go in reverse address order in the source (peach_fukidashi).
