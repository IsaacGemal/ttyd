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
  commented-out call was real); `effScanningDisp` written. Under `MUST_MATCH`: the `fmuls f1, f0, f1` operand order of
  `-cam->viewYaw * deg2rad` comes from a `rad` temp (`rad = -cam->viewYaw; rad = deg * rad;`), `f32 k = 0.125f;` fixes
  uranoko's pool order (1.0 before 0.125), and scanning builds `disp` from a second `Vec` local. peach_fukidashi's `.sdata2`
  pool is plain literals (the colour is a `static const GXColor` at the top of `effPeachFukidashiDisp`), with the functions in
  reverse address order in the source. Its `vec3_802fdc28`/`dat_802fdc34` rodata are still stand-ins.
## Review routine from Sep 29 evening (Isaac: "use your judgement"; written by TTYD Claude #1)

Every PR: one fresh Claude (Opus) review, fixes, then the merger rebuilds (all sources compile, 32 checksums, ci)
and merges. Risky PRs also get a Codex review (GPT-6.1 Sol, `gpt-6.1-sol`, effort medium, read-only): tool
changes, compiler-flag or split changes, shared struct/header reworks, and waves with many unproven partial
functions. Plus a periodic Codex audit of everything merged since the last one (about daily), because the Sep 29
audit of 802807f..0f455e3 found nine tool bugs the per-PR Claude reviews had missed (it ran the tools on crafted
inputs rather than reading diffs): scratch/codex-audit/. Fixes after a review aren't re-reviewed, only rebuilt and
checked, so reviews should be asked to say which findings need a second look after fixing.

## Wave 23: the next untouched function per dol file (2026-09-29, TTYD Claude #1; agents DeepSeek V4.1 Flash)

Wave 21's recipe again, on the next 0% function of 179 dol files (median 576 bytes, nearly all in existing files),
45 minutes each, 19:48-21:40; the prompt now told agents to stay inside their workspace. **129 at 100%, 41 at
90-99%, 6 at 50-89%, 3 unchanged. Functions 5,724 -> 5,851.** Main had moved during the wave (Claude #3's PRs touched
some of the same files), so collection was a three-way merge per file (base: main at launch, ce513a4): 175 clean,
1 new, 3 kept main's version on conflict (mot_yoshi, unit_object_tree, battle_damage). Fixed at collection: agents'
duplicate prototypes where the definition says otherwise (tri2, SoundSSPlayChEx, effFallLeafN64Entry,
effFlowerN64Entry, effGlassN64Entry); eff_pturn_n64 kept at main's version (its header's struct merged badly);
evt_paper_entry defined by both #41 and an agent (kept the agent's, which is at 100% and in the right order). Lesson: a per-file three-way merge makes a wave
safe to collect onto a moving main; the remaining conflicts are prototypes two writers guessed differently.

Review (Claude + Codex) found no wrong logic in the 47 partials, but a merge regression and four rule breaks,
all fixed before merge: the agent's `#pragma optimize_for_size` pair in battle_message.c survived main's move of
that file to `-O4,s`, and its `off` dropped btlevtcmd_AnnounceSetParam to 72%; `nanNPCWork` is global in the
original but was `static` in evt_nannpc.c; star_control and winItemDisp read Vecs as offsets past a string (now
the named vec3 symbols, 88.5 -> 77.5% and 90.0 -> 65.9% until those units' .rodata is decompiled); envDisp_FF's
invented seed globals are literals again (97.9 -> 94.3%). Agents still left their workspaces: 43 of 179 used
the tower's shared /tmp (same file names at the same time) and 21 read the tower's ~/ttyd, which they didn't
write to. Next waves give each agent its own TMPDIR.

## Grind 18 (Claude #3)

- **win_mario**: `unk_8016af5c` and `winZenkakuStr` match (only pool symbol names differ): the digit table and the `"-"`, `""`, `"%d"` strings are literals (they pool into `.sdata2`/`.rodata` by themselves), the `char` buffer is `signed char`, `winZenkakuStr`'s index is `u32` (`unk_8016af5c` keeps `s32 i`), and the static buffer is used directly instead of through an `out` pointer.
- **battle_event_cmd**: the `btlevtcmd_*` handlers that fetch a unit match with `BattleWork* wp = _battleWorkPointer;` declared (after `args`) and the call nested as `BattleGetUnitPtr(wp, BattleTransID(event, id))` (no `id` temp); 7 more at 100%. `btlevtcmd_SpawnUnit` is an early return on NULL. `btlevtcmd_onoff_battleflag` reads `args[1]` and `wp` after the call (97.9%).

## Grind 19 (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3"; near-miss list from waves 21 and 23)

- **Zero-Vec temp**: the `effXxxMain` functions that open with `position = vec3_...; position.x = ...` need the copy to come
  from an inline that builds the temp from a literal `(Vec){ 0, 0, 0 }` (`makeVec(x, y, z)` / `initializeVec`), not from a
  named rodata `vec3_...`: the scheduler then keeps the `userdata` load ahead of the copy. The literal's pool entry is
  anonymous, so it replaces the named `vec3_...` object only where the unit's `.rodata` score does not drop (kemuri10, vapor,
  hokori, peach_dust; the helper sits above the unit's other rodata so the literal lands at offset 0). In units that also carry
  the extra anonymous 2^52 double pool (rappa, kemuri9, kemuri11, opuku_wgun/wprotect, recover_heart, star_power) the section
  score fell 5 to 40 points against main with the literal, so those keep the named `vec3_...` and the old code shape
  (rappa's `Main` reaches 100% with the literal; it stays at 98.29% here until the pool duplication is solved).
- **`tri2` takes `s32` arguments**: none of the 520 call sites masks or sign-extends an argument, so the parameters are
  int-wide (the callee stores halfwords, so the asm cannot show signed or not; `s32` it is). pikkyolo and thunderflare `Disp`
  match, jetwrain `Disp` improves.
- **Blend expression**: `(C - v) * k + v` wants a temp for `C - v` under `MUST_MATCH` (`f32 t = C - v; v = t * k + v;`,
  eff_mahorn). A `for` loop that steps two pointers wants the stepping in the header (`i++, data++`) when the target steps
  `i` first (starpoint). peach_dust `Main` also wants `colorIndex - 1` in the range test and the scale product written
  `0.01 * value * 0.4`.
- Three units the wave 23 branch had matched but not linked were linked with `tools/autolink.py`: evt_paper, evt_ext, evt_urouro.

## Grind 20 (2026-09-29, Claude Sonnet 5.5 "TTYD Claude #3")

- **By-value `GXColor` stack slots**: a `GXColor` built as `color = <global>; color.a = ...;` and passed by value lands its
  local at the higher slot (target: local at the lower slot, the by-value copy above it). Building the colour in an inline that
  returns it (`static inline GXColor f(u8 a) { GXColor c = <global>; c.a = a; return c; }`, `color = f(a);`) puts them the
  other way round. Under `MUST_MATCH` (see eff_sandars): effBattenDisp2, effButterflyDisp and effStardust2Disp match;
  akari_pose, booboo and batten `Disp` improve. It does not help when the function passes several by-value colours
  (shadowdrv, itemget, envDisp_Blur) or a colour built without a base.

## Wave 22: the remaining REL units with functions (2026-09-29, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

Scope (Claude #1, Isaac's go): the unique-name REL units with functions not yet configured (58 big ones: over 12
functions or one over 4 KB, 1,431 functions), wave 19's leftovers (25 not linked, mri_02/05 for the no-deferred
flags, with the previous attempt attached), and the REL entry files and units rel_gen couldn't write before: 113
units, 24 agents at a time, 18:44-22:52, per-unit budgets 30 min to 2 h (+30 min for a function over 4 KB).
Wave 19's 7 dropped enemy units were done without agents: their functions with the tables re-rendered by
tools/struct_render.py, each linked on its own.

**Result: 111 units collected, 41 linked; their functions: 1,490 of 1,777 matched** (plus wave 19's 7 enemy
units, all linked). Claude #1 changed the
collection mid-wave: every unit that builds and lowers nothing is taken, NonMatching unless it links, each partial
function labelled `// NONMATCHING (NN%)` (45 labels generated from objdiff's instruction diff where the agent left
none: "K of N instructions differ ..."). 41 came back LINKED; 17 hit the time limit; 96 ended on their own.

**Agents stop early.** Most exited (rc=0) well inside their budget with functions unwritten: dmo_00 stopped after
18 of 60 minutes with 11 of 16 functions at 100% and 3 not started. A continuation pass
(scratch/wave22/launch_continue.py) resumed rc=0 NOT LINKED units in their own workspaces with the rest of their
budget ("read REPORT.md, continue"), taking slots only as the main queue freed them; it ran 13 rounds and linked 1
more (mri_01). Most early stoppers had under 15 minutes left. For the next wave: resume automatically inside the
launcher (a loop until LINKED or the budget is spent), not as a second pass.

**Collection lessons.**
- Header additions must be applied as patches with context, unit by unit. Appending each added line to the end of
  the header (wave 19's collect) broke every struct an agent added to (a field landed outside its struct). Several
  agents of one REL also edited each other's headers (rels/<mod>/include): those diffs are collected too.
- Agents of one wave collide: two copies of one struct (EffPokopiThunderData, effVaporN64Data, YuuminigameWork,
  MonteEntry, ZakogunEntry), `extern` stand-ins for things another agent declared properly, a prototype one agent
  replaced while another kept the placeholder. Resolved by keeping main's layout and naming the fields the units
  use; gundan_zako_group2 (its ZakogunEntry has other names for group1/3's fields) is left out.
- A unit LINKED in its workspace can fail in the collected tree: unit_churantalar's code refers to its own copy of
  the int-to-double constant where the original uses the global double_to_int_jon_0000e888. Relink every unit after
  collection (tools/rel_link.py), don't trust the workspace's verdict.
- The new extend_data check (tools/ci.py, #45) caught a real collision on its first run: rel_gen numbers a unit's
  ids by its position in the REL, which collided with mri_puni.c's hand-picked ids.
- rel_gen: data objects needing byte granularity (.string, .byte, odd .skip) come out as u8 arrays; a data object
  whose first word merely looks like a command was read as a script (gor_03's slotgara, evt_yuunpc's
  chusan_act_tbl): a script's first word must now be a real opcode.

**Provenance.** The agents had network access and used it: curl and git clone to GitHub (the upstream
doldecomp/ttyd, its API, forks: Penguinjanator, FermataRest, lincoln-lm, theKidOfArcrania and others), and
grep.app / DuckDuckGo / searchcode for function names (nok_nok, gor_gor, gor_gor_irai, gra_gra_00, jin_jin_07,
las_las_04, pik_pik_02, rsh_rsh_00 and others; scratch/wave22's scan). The only successful downloads were upstream
doldecomp/ttyd files (MIT, this fork's base; e.g. src/event/evt_npc.c); the forks returned 404 and the search
sites returned pages without results; no collected file shares code with anything downloaded. Agents also read
other wave-22 workspaces and the tower's ~/ttyd copy (this project's own code). Next waves: the prompt forbids
network access and reading outside the workspace, and the tower's agents should run without network.

**Review round 1 (one fresh Claude; ~50 partial functions compared with the asm, plus an automated check of every
partial function's call arguments).** Four partial functions were wrong in ways no score shows: sys's
display_func passed a dozen wrong GX arguments (COLOR1A1 for COLOR0A0, GEQUAL for ALWAYS, a light and DF_CLAMP
where the original has none, PASSCLR for MODULATE, TEVREG1 for TEVREG0, stage 3's TEV inputs on stage 2, ...),
tik_05's tiz_broken dropped a negation, rsh's rsh_window_tev used a 2x4 texgen and tik_00's uranai_cap a
point-sampled texture. All fixed against the asm (display_func's 87 GX calls now pass the original's constants).
Also fixed: conflicting declarations between agents' headers, repeated prototypes, 104 bare returns in event
functions, stale placeholders, unbalanced optimizer pragmas (gor_10's reset brings tegami_delete to 100% and costs
depthDisp: 93.8% -> 72.2%), misplaced and stale labels. unit_boss_cortez's entry_honeduka had been rendered with
an all-zero initializer although it's .bss in the original: tools/struct_render.py now leaves .bss objects alone,
and the unit links. Lesson: an agent's own label ("every GX call is in the right order", "logic follows the asm")
is not evidence; partial functions need their call arguments checked against the asm, which a script can do for
every function (the constants loaded into r3-r10 before each `bl`).

**Review round 2 (Claude #1's Claude and Codex reviews).** More wrong logic, each fixed against the asm: sys's
display placed the letters mirrored (36*i - 18*(count-1), ours had the negation); yuu_03's evt_ryushi lacked a
whole switch (states 2-4 and 6-8 set the scroll speed); tou's rankingControll inserted the 0x1000 fighter at
position 0 instead of 1, searched for flag 4 where the original tests flag 2, and compared entries' ids with
swByteGet(0x1FB) where the original compares the indices (and moves the winner to the front); dmo_00's demo_book
lacked four calls; muj_20's elmos1000tai_init evaluated `0.9f * v * sin` in the wrong order and precision. Three
stubs an agent had left as bodies ("stubbed so the unit links": jin_00's 10 KB senteresa_ext_main, tou_03's
screen_tev and ext_kyaku_init) are TODO placeholders again, for the next wave. Five copies of original jump
tables whose functions are now written (their offsets point into the original's code) are gone; 15 whose
functions aren't written stay as generated data. batten_leader's satellite helpers use BattleUnit.work /
BattleUnitPart.work instead of raw offsets.

**Follow-ups:** 21 NonMatching units have their functions in the asm's order rather than rule 18's reverse order
(listed in Claude #1's review) and won't link until reordered; named literal globals (`const f32 float_0p5_... =
0.5f`) remain in tou.c, sys.c, las_05, bom_01, batten_satellite and three lecture files (the prompt forbade them;
the agents wrote them anyway); unit_boss_gundan_zako_group2 (left out) needs its ZakogunEntry renamed onto
group1/3's; MonteEntry is defined in yuu_02.h and yuu_01.c under different field names; EffPokopiThunderData's
fields at 0x30-0x48 are unnamed (the units write two colours there); unit_churantalar should link once its code
refers to double_to_int_jon_0000e888; tou.c names its local functions with dtk's `_27_text_` stand-ins; the three
stubbed functions and the other unwritten ones go to the next wave.

## Wave 24: the next untouched function per dol file (2026-09-29, TTYD Claude #1; agents DeepSeek V4.1 Flash)

Wave 23's recipe on 121 more files, 21:45-23:29, launched from e7a5263 and
collected onto main ff4f263 with `scratch/wave24/collect.py` (per-file three-way merge, header patches applied).
**73 at 100%, 44 at 90-99%, 3 at 50-89%, 1 reverted. Functions 5,888 -> 5,961.** 119 files merged cleanly; win_item
and win_mario conflicted with main and were merged by hand (item_disp's Vecs are the named vec3 symbols, not a
struct laid over str_802F4CE8). Reverted at collection: eff_toiki (the agent anchored the file's data by hand with
`#pragma section` and `__declspec(section)` externs and broke effToikiMain), and one agent's `u16` -> `u32` change to
DrawMainIconHukidasi, which is at 100% on main. The new section ratchet caught nine units whose data got worse
(named constants beside the literals that replaced them, `<math.h>` statics, a definition turned into an extern);
all nine are at or above main now. Agents searched Sourcegraph, Bing and GitHub for the function names and fetched
files from upstream doldecomp/ttyd, NWPlayer123/PaperMario2 and vinvirile/ttyd and Penguinjanator/ttyd (MIT; nothing
copied), from Rainchus/ttyd-decomp-us and Peeeech/TTYDRelDecomp (no license), and from jdaster64/ttyd-tot
(GPL-3.0), mostly without saying so in REPORT.md; the yoshi agent disclosed taking a function signature from the
GPL project. A per-agent TMPDIR didn't keep agents out of /tmp (31 still wrote there by absolute path), and some
read other agents' workspaces; the next launcher runs each agent under bubblewrap with its own /tmp.

## Wave 25: the next untouched function per dol file (2026-09-29/30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

46 files (the smallest untouched function up to 2 KB in each file not held by Claude #3), 23:40-00:27, launched from
82c6f01 (wave 24 before its review fixes) and collected onto main b22654f with scratch/wave24/collect.py: 46 merged
cleanly. **31 at 100%, 15 at 90-99%.** Fixed at collection: battle_mario.c's `extern s32 weaponGetPowerTatsumaki[]`
stand-in clashed with the prototype an agent added to battle_weapon_power.h, so it includes the header now; six
units whose data got worse were fixed the same way as wave 24 (literals where main names nothing, named externs
where it does, definitions in pool order). seq_title's L_titleInit drops to 91.55%: its 100% needed the strings
read as offsets past vec3_802c11a0. Review (Claude + Codex) fixes: eff_ripple's dat_8041fb98 was byte-reversed (white with alpha 0, not cyan);
effFireDustMain's discarded rand() was MUST_MATCH-only, which shifts every later random number in other builds;
ac_gauge_timing2 names its 13 rodata symbols instead of an invented struct; mobj_lock is back in its rule-18 place
(evt_mobj .sdata2 30.29 -> 29.81, accepted); sac_deka, eff_syuryou and battle_status_icon's biribiri keep their
offsets-past-a-symbol reads only under MUST_MATCH, with the named symbols under #else, because the named form
costs a 100% function in each. Follow-up: evt_mobj.c is mostly written in the asm's order, not rule 18's reverse.
The dol's cheap pool is nearly used up: after wave 26, 75 dol files have only functions over 1 KB left.

## Grind 21 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3"): pool-literal finisher, near-miss sweep

- **Units whose code was 100% but that failed at link** (undefined extern `.sdata2` constants) link once the constants are
  literals in the code, so the compiler builds the pool in first-use order: `float_*` externs become `1.0f`-style literals, the
  named `double_to_int_*`, `vec3_*` (zero vectors: `(Vec){ 0, 0, 0 }`) and `str_*` definitions go away, a `dat_*` colour
  becomes `(GXColor){ r, g, b, a }`, and a zero `.sbss2` scratch `GXColor` (`unk_*`) becomes `(GXColor){ 0, 0, 0, 0 }`
  (the compiler pools zero colour literals into `.sbss2`). `MATCH_F32` / volatile wrappers around the constants go too. The
  same trick, applied by a script over the unit, linked: eff_miss_star, eff_treasure_map, eff_laser, eff_dogabon_attack_n64,
  eff_coin_n64, eff_fire_ring_n64, eff_guruguru_n64, eff_rothammer_success_n64, eff_holography_n64, eff_freeze_n64,
  battle_message, eff_torch. Vertex tables the unit owns (`.data`) come from `tools/dol_gen.py`; a `u32` table where the code
  indexes a byte array needs `(u8*)` in the pointer arithmetic (holography: `ach_01_v + i * 0x134`).
- **Still open in that class**: eff_ice, eff_mizutama, eff_machinegun, eff_bush, eff_crystal, eff_kemuri4/6/7, eff_onpu,
  eff_packn_bfire, eff_rubble, eff_snowman, eff_ice_n64, evt_snd (each needs its own `.data`/`lbl_` object or has a
  pool-order or size mismatch), and hw_aramdma.
- **hw_aramdma `.bss` order** (parked): the target has `aramQueueLo` then `aramQueueHi`; ours emits Hi first whatever the
  declaration order. Tried: swapping the two declarations, swapping the two stores in the inline `aramQueueInit`, taking
  `&aramQueueHi` first, one wrapper struct holding both (changes the addressing: +8 bytes of `.text`), `__declspec(section
  ".bss")` (does not compile). Nothing moved Lo first.
- **Near-miss list**: `effKemuri2Main` (the `userdata` pointer lands in r31 instead of r7), `itemDelete` /
  `btlUnitItemDisp2` / `kpa_fall_80213C38` (the target lays `if (x >= a && x < b)` as `bge X; b Y` with the then-block after
  the `b`; no source form found), `lightGetNearObj` (`cmplwi` on the tail-loop count, casts made it worse), `sndCheckEmitter`
  (reads `flags` at 0xc where our `SND_EMITTER` has it at 0x10; library version difference), iconDispGx `s16`→`u16` (changes
  the dol checksum). Matches: effFallDispInseki (`rad` temp under `MUST_MATCH`), mario_kemuri_landon (`!x` keeps the compare
  order), mapGrpSetOffScreen (locals instead of a nested call), evt_nannpc_move_position2 (declaration order).

- **Pool-literal finisher, second round** (2026-09-30): eff_snowman_n64, eff_onpu_n64, eff_kemuri6_n64, eff_crystal_n64 and
  eff_rubble_n64 link. A `lbl_*` `.data` pair another unit reaches (`{ -1.0f, 1.0f }` in snowman and onpu) is a
  `__declspec(section ".data") f32 lbl_...[2]` defined after the unit's other `.data`. The pool order follows first use, and a
  `C * sin(x)` / `(f32)i * 0.8f` written with a temp (`f32 t = (f32)sin(x); ... C * t`, `f32 k = 0.8f;`, under `MUST_MATCH`)
  swaps two adjacent pool entries. Left open with one to sixteen differing words: eff_kemuri4 (compare operands of `a5 != 0.0f`
  keep their register order), eff_kemuri7 (the `field_0x60 == 0.0f` load order), eff_ice_n64 (one `fmuls` operand order),
  eff_packn_bfire (FP register allocation once the volatile constants become literals), eff_bush (Disp frame size),
  eff_ice/eff_mizutama/eff_machinegun/evt_snd (event-script data or `.data` objects not written).

## Grind 22 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **Function order**: a unit whose functions are all 100% but whose text differs everywhere (hundreds of words) has its
  functions in the wrong source order. The source lists them in reverse address order (eff_bush_n64 had Disp before Main;
  the target's address order is Disp, Main, Entry, so the source is Entry, Main, Disp). eff_bush_n64 links with that plus the
  `f32 k = 0.8f;` int->float temp under `MUST_MATCH`.
- **Tools** (new): `tools/pool_literals.py UNIT` (extern pool constants to literals), `tools/pool_data.py UNIT` (the unit's
  own `.data`/`.sbss` objects), `tools/pool_compare.py UNIT [section]` (pool entries in order, target vs ours, floats as
  hex; a swapped adjacent pair means the two constants are first used in the other order). A `.bss`/`.sbss` object's order
  follows its first reference in code emission order (functions are emitted in reverse source order), not declaration order.
- **What did not work** (so nobody repeats it): operand-order and declaration-order searches over the eleven remaining
  units of the class (kemuri4/7, ice_n64, packn_bfire) and over 130 near-miss functions moved one score by 0.2 points.

## Wave 26: sandboxed agents, no network (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

The smallest untouched function up to 2 KB in each of 46 dol files (median 1.1 KB; 30 over 1 KB), 60 minutes each,
00:30-01:20, from main b22654f. First wave with the hardened launcher: each agent ran under bubblewrap with its own
/tmp, the prompt forbade network use, and an agent that stopped without REPORT.md or `check.py --final` got one short
continuation (3 did). Result: zero web fetches, every agent left a report. **20 at 100%, 22 at 90-99%, 4 at 50-89%**
(the size-routing finding holds: Flash does worse above 1 KB). All 46 merged cleanly; fixed at collection: agents had
quietly edited existing code (animdrv's `!= 0` dropped from two 100% functions, a changed mobjCalcMtx expression, a
new `dont_inline` pragma) and envdrv/shadowdrv literals added pool entries where main names the constants.
After this wave the cheap dol pool is 16 small functions; 75 dol files have only functions over 1 KB left.

## Grind 24 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **Function-order sweep**: `tools/fn_order.py UNIT [--write]` compares a unit's target `.text` symbol order with the source
  and moves each function (with its comment lines) into the slot that emits the target order (source lists functions from
  the highest target address to the lowest). 147 units had a different order; of the 60 that are complete (every target
  function written), 36 could be reordered with no function scoring lower and the built order now equals the target's.
  Left alone: units that then fail to build because a `static` function is called before it is defined (would need a
  forward declaration), units where a function score drops (the reorder changes what `-inline auto,deferred` inlines:
  ac_monosiri_target, battle_status_effect, eff_particle, evt_item, koopa_motion), and two where `.sdata2` got worse
  (eff_mobj_broken, evt_shop). The reorder changes pool first-use order, so the section scores of 21 units improve.
- **eff_itemget**: reordering alone brings its `.sdata2` pool to four differing words (the -21.333334 / 42.666668 / 2.0 / 0.5
  first-use order in effItemUseDisp) but the two GXColor stack slots (black and white temp, inline or local) and the FP
  register naming of the `-f * 0.5f` init were not solved, so it is not in this change.

## Grind 25 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **eff_n64 links**: the five functions that were unwritten (`tri2`, `effGetTexObjN64`, `effGetSetN64`, `effSetVtxDescN64`,
  `effTexSetupN64`) and the unit's `.data` tables (`eff_set_n64`, `_rand_table`) came from the dead `src/effect/eff_n64.c`,
  which is deleted. Two things fixed the last words: the polarity of the two `if` tests (`if (!loaded) { ... } else { ... }`
  as the target lays them) and the function-local `dummy`, which the target has in `.sdata` at a 32-byte offset. That takes
  `static u8 dummy ATTRIBUTE_ALIGN(32) = 0;` under `#pragma explicit_zero_data on` (the same idiom as `effGetTexObj` in
  effdrv.c): without the pragma a zero initialiser goes to `.sbss`.
- **Matches by shape**: chuchurina `mono_off`, `mono_capture`, `_make_madowase_weapon`, `_get_itemsteal_param`, yoshi
  `_wait_yoshig_run`, `_check_swallow_attribute` (100%). `_check_swallow_attribute` needed only a declaration-order search
  (`tools/perm_decls.py`, plain declaration lines with the assignments after them). `_get_binta_hit_position` and
  `_get_nomikomi_hit_position` are the same function in two units; both stop at 92 to 93% on callee-saved register
  assignment (event/args/work), decls searched with 150 shuffles.
- **battle_sub**: `btlMovePos` written (80.67%). The original is built `-O4,s`; a flag sweep gives 91.51% for it and 89.05%
  for `btlsubResetMoveColorLvAll`. The flag change is in its own PR.
- **Flag sweep over every unit with a function below 100%** (295 units, `--quick`: -O4,p / -O4,s, pragmas kept or dropped):
  only -O4,s helps, in battle_sub, battle_status_icon (BattleStatusIconMain 86.59 -> 88.55), party_kuribo (7.49 -> 7.66) and
  eff_kemuri1_n64 (17.27 -> 17.88), besides the fourteen files of the earlier sweep. No unit prefers -O4,p over its
  current flags.

## Grind 26 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **`__makeTechMenuFunc`** written for yoshi, chuchurina (100%) and nokotarou (100%; its first two entries carry the icon as a
  compile-time constant, `TECH_MENU_ENTRY_ICON`, because those weapons are defined in that unit's own `.data`; the other two
  read it from memory). Same MUST_MATCH base-register trick as clauda/vivian/sanders/christine.
- **Header note**: `BattleActions.mAcDifficulty` is declared `s8` (battle_ac.h:41) but the asm reads it with `lbz` (zero-extend):
  `_get_itemsteal_param` and ac_lst_rc_keep want it as `u8`. `_get_itemsteal_param` casts at the use.
- yoshi `yoshi_original_color_anim_set` (98.4%): six strings through one base register and a seven-entry jump table (`case 6:`
  falling into `default:` is what gives the table its seventh slot).

## Grind 27 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **Six more units link**: eff_syuryou, eff_hokori_n64, eff_kemuri2_n64, eff_pikkyolo_n64, eff_pokopi_pipo_n64, evt_snd.
- **Data the unit only reaches through a base symbol is dropped by the linker** unless it is force-kept. eff_syuryou reaches its
  two `.rodata` strings as offsets from the vec3 anchor, and eff_hokori's `.data` (float pairs and vertex arrays) is reached
  through the previous unit's last object `lbl_8039D5D0`; with both compiled, all functions and all section scores were 100%
  yet the dol lost the bytes (hokori's 224-byte `.data`, syuryou's 32 bytes of `.rodata`/`.sdata2` shifted everything after).
  `#pragma force_active on` / `reset` around the definitions keeps them. The symptom to look for: every section 100%, autolink
  says checksum mismatch, and the dol's section sizes are short by the size of the unit's own data.
- The float pairs before the vertex arrays (`{ -1.0f, 1.0f }`) need `__declspec(section ".data")`, else 8-byte objects go to `.sdata`.
- **Converting `extern const f32` to literals moves FP loads** (pool_literals.py): hokori `effHokoriMain` and pokopi_pipo's
  `-1.0f` multiply came out at 99.1 to 99.7% until the constant went through a `f32 k = ...;` temp under `MUST_MATCH` (natural
  expression under `#else`), the same trick as the int-to-float `k` in bush. fire_dust (`main_dl`, zero GXColor literal
  schedules the load of the magic double differently) and kemuri10 (`fcmpu` operand order, `Disp` frame 0x10 larger) stay
  unconverted.
- evt_snd.c carried two dead stubs (`evt_snd_bgmon`, `evt_snd_sfxon_3d`) that party_motion.c defines for real: multiply-defined
  at link. They were not in the unit's target, so they are gone.

- **unit_party_nokotarou links** (19 KB of `.data`): `tools/dol_gen.py UNIT --append SRC > new.c` writes the unit's event scripts and tables
  (it prints the merged file: redirect it; the header declarations it adds to `include/` clash with other files' local
  `extern s32 X[]` stand-ins, so revert `include/battle/*` and declare the four handlers as local array externs). Three fixes
  after that: `$` in a local's name (`negone_one$374`) becomes `_374`; the data needs `#pragma force_active on/off`; and
  `__makeTechMenuFunc` keeps the `lbl_8038BEB8` anchor under MUST_MATCH (the anchor is the label 8 bytes before the unit's
  `.data`, so compiling the weapons in the same TU changes the `lis/addi` immediates by 8 unless the code uses the extern).
- eff_machinegun links: the `-16.0f` fold (`x0 = -32.0f * 0.5f` computed at compile time) is avoided with `f32 f = 32.0f;
  f32 h = 0.5f; x0 = -f * h;` under MUST_MATCH. eff_mizutama has the same fold but the target also re-multiplies `sz * 0.5f` in
  the loop (loop-invariant hoisting differs), left open at 95.6%.

## Grind 28 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **eff_ice links** (10.8 KB of `.data` through `tools/dol_gen.py --append`). Its `.data` in the original object ends with 24 zero
  bytes: the linker's padding before eff_torch's first object, a 1 KB texture that is 32-byte aligned (eff_torch's section
  alignment is 32 in the original, 8 in dtk's regenerated object). With `align:32` on eff_torch's `.data` in splits.txt and
  `ATTRIBUTE_ALIGN(32)` on `torch_obj2_texture` the linker pads the same way and eff_ice needs no pad objects. The `.sdata` and
  `.sdata2` tails are padded to 8 by the linker on its own.
- **nokotarou's `.data` split was 8 bytes off**: `party/party_chuchu` ended at 0x8038BEB8+8 with a `{ -1.0f, 1.0f }` pair
  (`lbl_8038BEB8`), and `__makeTechMenuFunc` addressed the weapons as `lbl_8038BEB8 + 0x844`. That pair is the first object of
  nokotarou's `.data` (the unit's base label), so the split now starts nokotarou at 0x8038BEB8; the MUST_MATCH anchor is gone and
  the function reads the weapons by name and compiles to the target. Symptom for other units: a dtk `lbl_XXXX` used as the base
  register of `lis/addi` with a big offset, sitting at the end of the previous unit's `.data`.
- **Systematic version of the 8-byte split error**: `python3 tools/split_scan.py` lists the 32 units whose `.data` ends with an
  8-byte `lbl_XXXXXXXX = {-1.0f, 1.0f}` that the next unit's code uses as its base register (the makeTech `weaponBase` anchors of
  clauda/yoshi/chuchurina/vivian/sanders are this pattern). Moving each boundary 8 bytes down and defining the pair first in the
  next unit's `.data` removes the anchors; only nokotarou's has been done.
- **struct_render on dol units**: `fix_index` treated a scalar definition (`static BattleUnitDefense defence = {`) as a use and
  refused to convert (only `[`-suffixed declarations were skipped), so the typed tables were left raw; it now also skips a
  definition or declaration ending in `=` or `;`. With that, `tools/struct_render.py FILE --auto` plus the two `run_all` passes
  (driver: monkey-patch `c_files` to the one file) types nokotarou's entry, data table, defence tables, resistance table, the
  first weapon and the pose tables (two `extern s32 pose_table_nokotarou_*[]` in rels/jin and rels/las become `BattleDispPose[N]`);
  the other four weapons, `unitdata_Party_Nokotarou` and `unitpartsdata_Party_Nokotarou` were typed by name
  (`SYMBOL=BattleWeapon` etc.). Left raw: `unk_evt_80390b30` and the effect mesh tables of eff_ice (no layout for them).

## Grind 29 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **Seven more units link**: eff_ice_n64, eff_kemuri4_n64, eff_kemuri7_n64, eff_kemuri10_n64, eff_fire_dust_n64,
  eff_packn_bfire_n64, eff_mahorn. Their functions were 100% since the earlier waves; what was left was the pool and data.
- **`tools/pool_define.py UNIT`** (new): defines the `extern const f32/f64/u32/GXColor/Vec NAME;` pool constants from the
  disassembly, in the original address order, *after the code that loads them*. This is the safe alternative to
  `pool_literals.py` (which replaces the names by literals and moved the FP loads of functions that were 100%): the code keeps
  loading the same named objects, so no function changes, and the unit's `.sdata2` becomes 100%. Defined before the code the
  compiler folds the values (`x * 0.03125f` becomes a constant) and the functions drop; a non-const `__declspec(section
  ".sdata2")` definition changes the code as well. `tools/pool_data.py` does the vertex tables (`.data`) first.
- **Other pool pitfalls seen while doing it**: (a) `#include <math.h>` pulls in the `fake_tgmath.h` `sqrt` inline whose
  function-local `static const double _half/_three` are emitted into `.rodata` (two extra doubles and a shifted unit): use
  `#include "math_ext.h"` (only the prototypes) as eff_kemuri2 does. (b) The int-to-float magic double
  (`double_to_int_XXXXXXXX`, 0x4330000080000000) is the compiler's own pool entry: an extern *and* a definition of it
  duplicates it; leave it out (pool_define skips it). (c) A string that is a named `const char[]` object is placed before the
  compiler's pooled constants; the target has the pooled double first, so use the literal (`effect->type = "FireDustN64";`).
  (d) A leftover literal (`ptr->z = 10.0f;` where retail loads `float_10_...`) pools a second copy: use the named constant.
  (e) A 0.0f the retail loop loads through the name of a `volatile`-declared pool object (eff_mahorn): read it once into a local
  under MUST_MATCH.
- **`.sbss2` zero GXColor objects**: `#pragma section ".sdata" ".sbss2"` then `__declspec(section ".sdata") GXColor unk_...;`
  (the eff_queen2 idiom). They are placed in the *reverse* of their declaration order, so declare the higher address first
  (fire_dust: `unk_804230fc` then `unk_804230f8`); with the wrong order the code loads the neighbouring object and the dol
  differs by two bytes at the two `lwz ...@sda21` sites.
- kemuri7 `effKemuri7Disp`: `if (data->field_0x60 == 0.0f)` with the pool constant loaded before the field needs
  `{ f32 z = 0.0f; if (data->field_0x60 == z) ... }` under MUST_MATCH once the constant is a literal; with `pool_define` the
  code is unchanged and none of it is needed.

## Grind 30 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **arammgr links** (all eight functions): `ARAMWork`/`ARAMWorkNode` named field by field (the header had unnamed tails), `wp` is
  `static ARAMWork* wp = &work;` (`.sdata`, not `.sbss`). `aramMgrGarbage` (a defragmenter: wait for pending nodes, unlink
  freed ones, then copy each moved block through a 32 KB bounce buffer) matched after: `*(volatile u16*)node & N` for each flag test
  (two loads, no CSE), a separate loop variable for the third loop, `(chunk - 1 + size) / chunk` (the compiler
  reassociates `chunk + size - 1`), and `for (i = 0, offset = 0; ...)` (register order of the two counters).
- icondrv `iconNumberDispGx`/`iconNumberDispGx3D` written (92%): digits by `/` and `%` (the target keeps a separate quotient
  for the `%`, so `rem % 100000` then `rem /= ...` is not the same as subtracting `q * 100000`), the top digit tested
  against the original number, six digits shown as five; return type is `s32` (the header said `f32`; every caller ignores it).

## Grind 31 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **Parallel declaration-order search** (`tools/perm_batch.py MIN MAX`, `tools/perm_apply.py`): for every partial function in a
  score band it compiles up to 70 orderings of the leading local declarations (single moves plus shuffles; initialised
  declarations are allowed, a variant that no longer compiles is skipped) from scratch copies, scores only that function with
  objdiff, and records the best; the applier rewrites the declarations, rebuilds the unit, keeps the change only if nothing in
  the unit dropped and updates the NONMATCHING comments. About one function in ten of the 97-100% band gained: five went to
  100% (BattleAudienceSoundWhistleKind, btlevtcmd_GetHitPos/SetPos/SetRotate/SetBaseRotate) and four improved (sac_muki
  end_muki, eff_kemuri11 Disp, eff_pokopi_pcharge Entry, evt_badgeshop badgeShop_set). A companion search that swaps the operands
  of one commutative operator or comparison at a time found only 0.02 to 0.2 point gains and the three 100% results the
  declaration search had already found: not worth running again.
- `tools/finish_unit.py UNIT`: pool_data, math_ext, pool_define, literal strings, the `.sbss2` idiom and autolink in one
  command, keeping a step only if no function or section drops; it links eff_kemuri4 from its pre-fix source.
- Written: `btlevtcmd_MarioJumpPosition` (96.7%), `btlevtcmd_DivePosition` (94.7%), `btlevtcmd_DivePartsPosition` (93.5%; the
  part offsets are the unit's minus 0xC8), countdown `Start` (100%), `SaveReStart` (100%; 64-bit locals in the order the target
  multiplies them), `Main` (97.1%) and `Disp` (94.4%; `GSW(0)` is 0xF5DE0180, the flag tests are `& 4` in Disp and `& 8` in
  Main). `GSW(n)` in evt_cmd.h is `n - 170000000`, so the target constant is `GSW(0)`.

- **Raw offsets in the new battle handlers** (BattleUnit has these as unnamed gaps; a header pass should name them):
  `0x168` frame counter, `0x178` angle, `0x17C` distance, `0x180` arc height / step counter, `0x1A4/0x1A8/0x1AC` per-frame
  position deltas, `0x1B0` arc angle, `0x1B4` arc angle step, `0x1B8` previous height, `0x188/0x1C0` sound-mode outputs of
  `BtlUnit_LoadSeMode` (BattleUnitPart: the same fields at 0xA0/0xB0/0xB4/0xB8/0xC0/0xF8, i.e. minus 0xC8).
  `btlevtcmd_MarioJumpPosition` computes `-(*(f32*)(unit+0x1B8) - *(f32*)(unit+0x1B8))` (a value minus itself) where the target
  loads the field twice and subtracts: an artefact of the original source (it was probably `prevY - y` with both reads
  compiled to the same location), kept because it is what the target does.
## Wave 28: near-misses, one per file (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

Instead of untouched functions, each agent got one file's closest near-miss (90-99%), 37 of them the only open
function left in their unit, with the brief's new "Code shapes that fixed near-misses" section, the file as it was in
./orig_file.c and the rule "never lower its score". Sandboxed, no network, 01:23-02:40, from main da08abe; all 48
merged cleanly. **29 of 48 reached 100%, 8 improved, 11 unchanged, 0 worse; 21 units are now code-complete.** The best
result per agent of any wave: finishing near-misses with the idiom pack beats starting large untouched functions.
Fixed at collection: eff_mist2's .sbss2 scratch lost its definition (restored with the eff_queen2 section idiom),
party_bomhei and evt_nannpc named constants where main's literals pool correctly. eff_hibashira's .rodata score
drops 95.82 -> 94.12 only because effHibashiraMain's zero Vec is now an anonymous literal: the bytes match the
original's, and main already carries the extra 2^52 double at the end.
The wave also fixed three bugs on main: evt_npc's moveFlags read args[10], win_main's TEVREG1 colour had its bytes in the wrong order, and evt_telop wrote .b instead of .a.

## Grind 32 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

Linked ten wave-28 code-complete units (`eff_kemuri12`, `kumokumo_chip`, `stamp`, `sweat`, `wangflush`, `glass`, `hagetaka_flush`, `stone_smoke`, `akari_sflush`, `bomhei`, all `effect/n64/eff_*_n64`).

- Every section can score 100 in objdiff and the dol still differ: a named `const f32` defined in the file is placed before all pooled literals, so the unit's `.sdata2`/`.rodata` gets the same value twice (once named, once as the compiler's own pool entry) or in the wrong slot. `tools/pool_floats.py` turns the named f32 constants into literals (the compiler then pools them in first-use order); `tools/finish_unit.py` runs it, drops the unit's own `double_to_int_*` definition and its named string objects as a fallback after a failed link. `pool_define.py` no longer defines constants the code never loads by name.
- Mixed cases: the constants the original loads by name from a slot that precedes the pooled ones stay named (`akari_sflush`: `dat`, deg2rad, 6.28, 360, 0.1), the rest are literals. A literal `0.0f` next to a named `float_0` in the same unit doubles the zero in `.sdata2` (`wangflush`, `bomhei`): use the named zero everywhere.
- `.data` gaps: a `gap_NN_..._data` object of zeros or a `{-1.0f, 1.0f}` pair after a vertex table is part of the unit; widen the table or add the `__declspec(section ".data") u32 lbl_...[2]` pair after it. A `gap_NN_..._sbss2` object is one more `GXColor` in the sbss2 idiom (declared in reverse).
- Section sizes of a linked unit differ from the target's object by the end padding (rodata/sdata2 end at 4 or 8 bytes instead of 16); the dol is what matters.
- `eff_stamp_n64` lost matched_data 92.31 -> 61.54 in the ratchet (its named string and double became compiler-pooled); the unit is now byte-exact and linked, excused with `--allow-regress`.
- Still open: `eff_teresa`, `eff_mist2`, `eff_hibashira`, `eff_nokotarou`, `eff_majinai` (sdata2 slots with strings/doubles), `evt_kinopio`, `battle_information`, `sunlight`, `batfukidashi`.

## Grind 33 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

Linked seven more wave-28 units: `eff_majinai`, `eff_hibashira`, `eff_teresa`, `eff_batfukidashi`, `eff_sunlight_n64`, `battle_information` (const `N_item_hog_table`), plus the ten of Grind 32 merged earlier.

- A table the target keeps in `.rodata` must be `const` (`battle_information`: a non-const `HogTable` landed in `.data`).
- `.sdata2` strings of at most 6 bytes sit in the constant pool in first-use order between the floats: use literals for them and for the floats after them (`majinai`, `hibashira`); the named (defined-after-code) constants come first, in definition order. `hibashira`: a block-scoped temp for `sin(...)` reorders the first uses of `3.14159f` and `0.5f`.
- `teresa`: constants that the target lists after the named ones in the order of first use in `Disp` (16, 0, 2 …) are literals; only the first constants (`dat`, deg2rad, 32, 0.5) stay named (`pool_floats.py --keep=NAME,...`).
- `pool_define.py` also reads the `extern const` declarations in the unit's own header (`sunlight`); a `.data` vertex array that no tool wrote is generated from the disassembly by hand (`testsrc__s_v`).
- Zero `gap_NN_*` objects at a section end are part of the unit: pad the last `.data` table with a zero word, extend the last `.rodata` string array (a 4-byte zero const would land in `.sdata2`), `static u32` in `.sbss`, `#pragma explicit_zero_data on` for `.sdata`.
- A string that nothing in the dol references by name is garbage-collected: `#pragma force_active on` (`evt_kinopio` `str_802ECF14`, read through `str + 0xC4`).
- Function order: `fn_order.py` reports the source order that emits the target's `.text` order; `event/evt_kinopio` has its 20 functions in the opposite order, and reversing them drops `paper_on` to 99.78% (r5/r6 swap for the `str` base). Data sections of `evt_kinopio` already equal the target (kept out of the tree: the unit's matched_data metric fell without the link).
- Still open: `eff_mist2` (`-16.0f * 0.5f` folds when the 16 is a literal; the named version drops Disp to 91%), `eff_nokotarou` (named `float_0/100/1` defined before the code fold and duplicate the pool), `evt_kinopio`.

## Wave 29: near-misses again (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

Wave 28's recipe on the next 48 dol files (units with one or two open functions), 02:42-03:55, from main 1b631ac: all 48
merged cleanly. **25 of 48 reached 100%, 13 improved, 10 unchanged, 0 worse**, confirming wave 28. Fixed at collection:
four units whose data got worse (deleted named definitions and .sbss2 objects in opuku_jetwrain, externs for constants
the unit pools itself in snow_dust and kemutest, a removed double in starpoint). The wave also fixed three bugs on
main: crane_timing's 0x3EB fall-through, opuku_jetwrain's TEV register, and release's case 2 fall-through.

**Follow-ups:** `BattleActionCommandGetDifficulty` returns a word (`lbz r3,0x1cbd(r3); blr`), but battle_ac.h types it
`s8`, so ac_stick_keep_left calls it through a function-pointer cast; romfont's glyph width is `u16` (the asm uses
`lhz`), but include/romfont.h types it `s16`.

## Wave 27: the REL finishing pass (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

Scope, per Claude #1's directive after #51, picked by `scratch/wave27/pick_units.py`: NonMatching REL sources with
an unwritten function, a function under 97%, or functions out of rule 18's order, plus six new shared units
(barriern v1/v2, barriern_z, monochrome_pakkun v1-v3, pakkun_flower v1/v2, piders v1-v3, super_mahorn; generated
with rel_gen + share_unit), dig.c (was empty) and tou2.c (upstream's partial file, kept, with a generated copy as
reference for its data). 48 units, 817 functions. Left out: near-misses at 97%+ with nothing else wrong (Claude
#3's), the older shared units, mri_puni (188 functions, too big for one agent), gundan_zako_group2 (needs its
ZakogunEntry renamed first), the shared events evt_shuryolight/evt_yuuranking/evt_kagemario (need per-module
symbol macros) and purple_teresa. 24 agents at a time, 00:42-02:44, time by work left (30-150 minutes).

New in the launcher: the prompt forbids the network and reading outside the workspace (no network command in any
log this time; a few agents read the base copy, one left and removed a backup file in ~), and a resume loop
restarts an agent that stops on its own before LINKED, up to three times within its budget (48 first rounds, 68
restarts; 9 agents ran out of time).

**Result:** 39 units collected, 12 objects linked (dig.c, dig_00, gra_00, moo.c, moo_03, mri_13, mri_16,
unit_super_mahorn in jon and las, unit_piders v1/v2/v3), +173 functions over main (7,559 -> 7,732). Nine units that
lowered a measure in the collected tree were left out: gor_03, gor_10, hei_10, bom_01, black_peach, batten_leader,
lecture_frankli_sac, rsh_05, muj_20. unit_churantalar links with its .rodata extended to 0x7858 (the five
constants at 0x7840-0x7858 are its own; dtk had put them in an auto split): a splits change, so it's its own PR
(#57) for Isaac, and the wave keeps the unit NonMatching.

**Found on the way:**
- Shared objects in gon and mri must be built without deferred inlining like the rest of those modules
  (`cflags=[*cflags_rel_nodefer, "-i rels/common/include"]`, as unit_kuriboo v2 already was). share_unit.py didn't
  write it, so the four new mri objects built with deferral while the prompt told their agents to use source order.
  The mri piders agent fixed its own configure.py line (which collection doesn't pick up); share_unit.py now writes
  the flag for gon and mri.
- rel_link.py marks a unit Matching in configure.py when it links, and addunit.py adds a module-level Object line
  rather than switching a shared unit's line: shared units are switched by hand at collection.
- rel_gen wrote `void atan2f(void)`-style placeholders for a REL's own copies of SDK functions (tou2's atan2f, the
  GX inlines in piders), which clash with the SDK header; it now includes the header instead. struct_render's
  fix_index took `static T x = {` definitions for uses. local_prototypes finds a shared unit's asm through the
  first module configure.py builds it for.

**Review** (one fresh Claude, with a script comparing calls, call constants, pool floats, strings, field offsets
and compare signedness per function; 69 of 107 partial functions differ only in registers/scheduling). Fixed:
moamoa_on's randf range, senteresa_ext_main (two randf ranges, `% 8`, s32 counters), ext_fan_main (GF(10), not
GF(65546); position.z), eki_00 (retail writes z over matrix[0][3] in ext_moamoa_init; moamoa_hit_check reads .y),
evt_ryushi_clr (missing per-entry clear, y/z jitter fields), item_jump's sound name and 2π, gansColor (white, not
cyan: a 100% function with a wrong constant), las_05's integer `rand() % 1000 + 1`, and constants (3.14f, 1/225,
57.29579f, 8/15, -50/3). Tool fixes: fix_index's keyword guard, rel_gen's prototype-only SDK match, a loud error in
local_prototypes.

**Follow-ups:** end_unknown `_prolog` has size 0x50 in config/G8MJ01/end/symbols.txt but is 0x60 (config change,
for Isaac); las_05 uses 25 more `float_*_las_*` externs nothing defines and tou.c 28 more of its own named floats
(the pool order is unsolved); `zanzou_count` (aji barriern_custom, v1 barriern) and jin_07's `g_nNumOutBox` should
be .bss; data sections below 100% in evt_yuunpc (.rodata 35%), mri barriern and eki_00 weren't reviewed; unwritten:
ext_moamoa_main in eki_00 and rsh_06 and two minigame4 functions in evt_yuunpc; the nine left-out units have work
in scratch/wave27/<unit>/final.c worth salvaging function by function.

## Wave 30: REL near-misses, one per file (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

Claude #1's wave 28 recipe applied to RELs, at its suggestion: one agent per REL file on the file's closest open
function, the brief's "Code shapes that fixed near-misses", the file as it was in ./orig_file.c, never lower a
score, bubblewrap /tmp, no network, a finisher continuation (launcher: scratch/wave30/launch.py, units from
scratch/wave30/pick_units.py). Files with one to four open functions whose closest one is written and at 90%+,
skipping everything #74 changed, mri_puni, the shared events and the old shared units (Claude #3's). 32 files,
24 agents at a time, one hour each, 03:21-04:03, units claimed in tools/claims.py as wave30.

**Result:** 20 of 32 target functions to 100%, 6 more improved, none lower, no header changes, no network.
Linked: gor_irai, unit_atmic_teresa, moo_04, aji_13, muj.c, evt_lect_hei (+6 objects). evt_lect_muj is
code-complete but uses named `zero_muj_*`/`float_0p5_muj_*` constants where retail pools anonymous literals, so it
doesn't link. batten_satellite's small gain needed an `opt_propagation` pragma without reaching 100%: left out.
Shapes that worked again: `/ 2.0f` instead of `* 0.5f` (register choice), a returning inline helper for a
by-value GXColor (stack slot order), collapsing a two-step update into one expression, `opt_propagation off`
around one function (mri.c). Two agents read files in the tower's ~/ttyd checkout despite the rule.

**Review** (one fresh Claude; every target function compared with relocations resolved to their bytes, and all
30 files built without MUST_MATCH). Fixed: rsh_00 `moamoa_init` scored 100% with `randf(-200, 600)` where retail's
range is 600 wide (`randf(-200, 400)`; the wave had removed its label, so nothing flagged it); gon_03
`senkaron_ext_main` passed -5 where retail's first jitter is `randf(5, 10)` (the same bug las_05 had in #74);
tik's `lect_map_cam_chg_sub` is byte-identical to dou's, so dou's nine-line recursion replaces the agent's inline
helper; sys.c `display` returns EVT_RETURN_BLOCK (retail `li r3, 0`, previously DONE: a behaviour change); stale
hom_00 labels. Behaviour fixes the agents made: tik_00 `card_tev` issues its 12 GX calls inside the `if` (retail
branches to the epilogue), and gor_irai's `irai_window_disp` colours had their bytes reversed (proven by the link).
Follow-up: evt_lect_muj links only if its named constants become literals, but then `lect_itemget_kesan` drops to
99.89% (two bytes at .text+0x521).

## Salvage of wave 27's left-out units (2026-09-30, Claude Opus 5.5 "TTYD Claude #2")

Local work while the tower waited on a Tailscale SSH check. Of the nine units wave 27 left out, gor_10 and
lecture_frankli_sac had since changed in wave 30; the other seven were rebuilt on the wave 30 tree one at a time
and every lowered measure traced. Three came back, each losing only on one spot:
- black_peach: pose_init lost 7% to the agent's named `vec3_las_000181a4` and an unsigned OSTime product; main's
  literal and signed maths restored.
- batten_leader: `defencecopy` typed as BattleUnitDefense (5 bytes) where retail's object is 8; kept `u8[8]`.
- gor_03: the agent's "logic fix" to `mario_keyoff_wait` inverted it (retail: BLOCK with no key held, YIELD
  while held); main's version restored. .data stays 0.1 lower: the placeholder `jumptable_0003A49C` data became a
  real switch whose compiler-made table points into code that doesn't match yet (--allow-regress gor/gor_03).
Left out: hei_10 (sin/cos inline helpers turned into macros under MUST_MATCH, and .data 100% -> 92%), bom_01 (11
named const objects standing in for literals), muj_20 and rsh_05 (.rodata literal positions shift, unexplained).
Their agent files stay in scratch/wave27/<unit>/final.c.

A fresh Claude review of the three found five real bugs, all fixed: `montemedalzpos[0]` -235.0f (not -234); gor_03
`fall_rtn` passed uninitialised outputs to `movePos`, which adds to them, and used 0.81667f for 49/60;
`goryuugijo_main_slot` tested flag `& 4` where retail tests `& 2`; black_peach's `hand_main` was made static while
the script and header use the global. batten_leader's `maer`/`nowv` are global as in retail. Follow-ups: gor_03's
`vec3_gor_0001a234/0001a240` may be local Vec initializers rather than objects; batten_leader's `VecData` should be
a function-static (retail `VecData$634`); `minigameselect_msg_set` compares unsigned where retail is signed.

Follow-ups done (Sep 30, Claude #2): evt_lect_muj links: its named `zero_muj_*`/`float_*_muj_*` externs became
literals, and `lect_itemget_kesan`'s `field * 0.5f` is written `/ 2.0f` (the same register-order idiom as
moo_04), which was the last two bytes. batten_leader's `VecData` is a static inside
`battleGetBattenSatelliteInitPos`, as retail's `VecData$634` shows (only the `$N` suffix and one register differ).

## The first unattended night (2026-09-29 23:40 to 2026-09-30 ~08:00, TTYD Claude #1 orchestrating)

Isaac went to bed with standing orders: keep waves running until 8 am, review every PR (one Claude review, plus
Codex for waves and tool/flag/struct changes), merge what passes, and hold split, compiler-flag and shared-tool
changes for him. Three Claude sessions ran: #1 orchestrated (waves 24-26, 28, 29, 31, collection, reviews, merges),
#2 ran the REL waves (22, 27, 30, the wave 27 salvage), #3 finished units and near-misses. A 45-minute timer was the
heartbeat.

**Result: 5,888 -> 7,836 matched functions; code linked 17.6% -> 23.7%; data linked 52% -> 62.7%; 27 PRs merged
after 03:30 alone.** The biggest single jumps were wave 22 (+1,490 REL functions) and wave 27 (+173); the most
efficient per agent were the near-miss waves 28 and 29 (29/48 and 25/48 to 100%, none worse).

What we learned, for the autonomous flow:
- **Near-miss waves beat untouched-function waves** once the cheap untouched pool is gone: give each agent one
  file's closest 90-99% function, the brief's "Code shapes" section, the original file, and "never lower the
  score". The dol's untouched pool under 1 KB was exhausted by wave 26.
- **Every wave's partials hid behaviour bugs that scores can't see**: byte-reversed colour tables, a dropped `& 7`
  index mask, wrong TEV register ids, wrong random ranges, flipped signs, a missing switch, calls that only
  happened under MUST_MATCH. The Claude + Codex review pair found at least one in every wave; neither alone did.
  Near-miss waves also fixed real bugs on main (evt_npc's args index, win_main's colour byte order, release's
  case fall-through, crane_timing's early return).
- **CI now ratchets data sections** (#49): functions reaching 100% while a unit's .rodata got worse was the most
  common collection regression (named constants left next to the literals that replaced them, `<math.h>` statics,
  extern'd constants the unit pools itself).
- **Sandboxing held**: bubblewrap per-agent /tmp plus "no network" stopped the /tmp collisions and the
  undisclosed fetches (wave 24 had fetched from GPL and unlicensed repos; waves 26+ fetched nothing).
- **The orchestrator merges; fix subagents only edit.** A fix subagent that ran `git stash` mid-merge silently
  dropped #63's changes; the regression gate caught it. Fix prompts now forbid history operations.
- **Peers go idle unless pushed**: a session that finishes a turn waits for a message. Every heartbeat now
  checks each peer's last activity and gives idle ones their next task.
- **Nothing that needs Isaac's approval should be assigned overnight**: Claude #3 correctly asked him before
  cloning decomp-permuter and then sat blocked for hours. And the tower's Tailscale SSH re-check (a sign-in only
  he can approve) stopped all waves from ~04:15.
- **Split errors are systematic**: 32 dol units' .data splits start 8 bytes late (a `{-1.0f, 1.0f}` pair the next
  unit uses as its base register); `tools/split_scan.py` in #68 lists them for one systematic fix.

Waiting for Isaac: #57 (churantalar split), #60 (flag sweep, Codex-reviewed), #68 (nokotarou split + typed data +
the 32-unit scan, Codex-reviewed), the decomp-permuter clone, and the Tailscale approval. Wave 31 (48 near-misses)
and Claude #2's wave 32 (55 REL near-misses) resume once the tower answers.

## Wave 31: near-misses, run across the Tailscale block (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

The next 48 dol near-miss files (units with two or three open functions), launched 03:57 from main 41767bb. The
tower's Tailscale SSH re-check blocked new ssh connections from ~04:15: 27 agents ran but their results couldn't be
copied back, and 21 never started. After Isaac approved the check at 12:25 the results were copied from the tower
workspaces and the 21 relaunched (scratch/wave31/RECOVERY.md). **32 of 48 reached 100%, 14 improved, 2 unchanged,
0 worse**, the best of the three near-miss waves. Fixed at collection: countdown and eff_mahorn2 had turned
literals the unit pools into externs of constants it no longer pooled.
countdown's MUST_MATCH build still names two pooled literals (`float_0_804204a0`, `float_300_804204bc`) as
externs; they, like main's `dat_80420498`/`dat_8042049c`, are undefined in our object and will block linking until
defined. The wave also fixed two bugs on main: recover_heart's `PSMTXTrans` tile offset is y, not x, and
starstone's state updates run only in states 2-5.

## Grind 34: decomp-permuter on the tower (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

Isaac approved running [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) (MIT); it runs on the tower only, never on the Mac.

- **Setup on the tower** (`pop-os`): `~/permuter/decomp-permuter` (a clone with its own `.venv`: `pynacl toml Levenshtein`), `~/ttyd-perm` (rsync of the repo without `.git`/`build`/`orig`, with `build`, `orig`, `build.ninja` symlinked to `~/ttyd`; re-sync `tools/ src/ include/` when main moves). `~/decomp-permuter` (another copy) is not ours.
- **No PowerPC objdump exists on the Mac or the tower.** `tools/perm_objdump.py` prints `dtk elf disasm` output in the layout the permuter's PPC scorer parses (`objdump_command` in `settings.toml`): one function (`PERM_FUNC`), branch targets as offsets, data symbols (`@ha/@l/@sda21`) as anonymous `@0`, call names kept.
- **`tools/perm_prepare.py UNIT FUNC`** builds `PERM_ROOT/inputs/UNIT__FUNC/` (`base.c` = the unit run through `mwcceppc -E` with the project flags, other functions reduced to prototypes; `target.o` = the original unit object; `compile.sh` = the project's wibo + sjiswrap + mwcc command; `settings.toml`). Two rewrites make pycparser's round trip compile: `__declspec(...)` removed, `__typeof__(sizeof(0))` -> `unsigned long`, `sizeof(T)` -> `(sizeof(T))`.
- **`tools/perm_run.py QUEUE`** is the supervisor: queue of `UNIT FUNC` lines, `--minutes` (default 20) per function under `timeout -k 30`, `nice -n 15`, `--jobs` x `--threads` capped at 8, one process group per run, overdue runs killed from `ps -o etimes`, everything killed on exit/interrupt, whole supervisor stops above load 20 or below 100 GB free. One log line per run in `~/permuter/runs.log` (tab separated): `time  unit  func  base_score  best_score  runtime_s  exit_reason` (`zero`, `finished`, `timeout`, `killed-overdue`, `load`, `disk`, `prepare-failed`, `permuter-error`, `interrupted`; best = lowest `output-SCORE-N` directory, `-` = none). Results stay in `inputs/UNIT__FUNC/output-*/source.c` (the preprocessed, round-tripped C): nothing is applied to the repo automatically; a win is re-expressed by hand in the natural source form, under `MUST_MATCH` with the natural form in `#else` when it is weird.
- Example line: `2026-09-30T13:54:01	koopa_motion	kpa_jump	25	-	60	timeout`. The scorer's base score for a function that is one register swap away is 25 (five register-allocation penalties).

## Wave 32: REL near-misses, second pass (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

Wave 30's recipe on every REL file whose closest open function is written and at 85%+ (58 files, 14 one function
from complete; scratch/wave32). Launched 12:27 once the tower's Tailscale check was approved, 24 agents at a time,
one hour each, claimed as wave32.

**Result:** 40 of 58 target functions reached 100% in the workspaces and 51 improved, none lower; after the
collection's checks, +38 functions (7,868 -> 7,906) and six more objects linked: mri.c, punidrv, evt_lect_gon,
evt_lect_gor, evt_lect_tou, evt_lect_win (win after its named zero/0.5/-15/1 constants became literals and
`/ 2.0f`, as for evt_lect_muj). Left out: end_unknown and yuu_03 (added `extern const f32 float_*` stand-ins and
lowered .rodata), gon_03 (.rodata lower for +0.8%), eki_03 (a pragma for +0.14%). The review round also reverted
poison_pakkun (its `start - start` departed from retail's zero load), yuu_00/usu_00 (comment-only) and
unit_barriern_z (a guarded helper for +0.03%). Pragmas kept, each reset after its function: bom_01
`opt_common_subs` (87% -> 99.66%), kpa `opt_propagation` (to 100%).

**Review** (one fresh Claude; semdiff on main and wave32, all files rebuilt without MUST_MATCH): no wrong constant,
flag, offset, signedness or return value in any target. Fixed: rsh_00's s16 `id` under MUST_MATCH (an extsh retail
lacks; now s32 in both paths), battender's `extern Vec vec3_aji_0001621c` (a `(Vec){0,0,0}` literal matches), the
gor lecture prototypes moved into rels/gor/include, las_29's needless MUST_MATCH guard, agent-note comments.
Behaviour bugs on main this wave fixed: dmo_00 `demo_book` concatenated its matrices in the wrong order; mri.c
`hakari_color` scored 100% with alpha 0 where retail's colour is 0x00FFFFFF; tou_02 `pose_main` indexed the group
table off `gp` instead of `fan_wp->groups`. The lecture units declare iconDispGxAlpha with a u16 icon id in their
own headers (the documented exception) and copy icondrv.h's iconFlagOn/iconFlagOff, since they can't include it.

## mri_puni: skeleton and plan (2026-09-30, Claude Opus 5.5 "TTYD Claude #2")

mri_puni (188 functions, the largest REL unit) was upstream's partial file: typed pattern tables, the four .bss
pointers and six functions, with most of its data missing. It is now rel_gen's full skeleton
(tools/mripuni/build.py, reproducible): all 78 data objects in order (.data 16% -> 93%), upstream's typed
tables and functions grafted in, every other function at a `// TODO: write` placeholder, extend_data ids 76/77
(upstream's 0/1 collided with other mri units' generated ids). The two empty stub bodies upstream had
(`evt_icon_disp_func`, `puni_disp_info_sub`) are placeholders again (--allow-regress for their ~1% scores).
mri is built without deferred inlining, so the placeholders are in the asm's order
(tools/mripuni/order.py moved 22): with `_make_texbuf` ahead of it, `evt_make_texbuf` inlines it as retail
does and goes 64.85% -> 100%.

rels/mri/include/mri_puni.h keeps upstream's structs and prototypes and declares the 26+ functions other mri scripts CALL as
USER_FUNC; the `extern s32 evt_puni_*[]` stand-ins in 20 mri files and include/rel/mri/mri.h are gone (those files
include mri_puni.h; the linked ones still link). rel_gen: a byte-array static local (`name$NNN`) is commented out
like a word one (it emitted `u8 h3$2957[2]`, which doesn't compile).

**Plan for a wave** (tools/mripuni/groups.py -> groups.json): 182 open functions, 88 KB, in 22 contiguous asm-order groups
of ~4 KB (puniModeSearch, 7.8 KB, alone). Structs first: PuniData (0x5A8 bytes) and PuniWork are almost all
padding, and 22 agents would each invent fields; one struct pass (an agent or the orchestrator) names their fields
from every access in the asm before the group wave. Then one agent per group in its own workspace, writing only its
group's functions; collection splices each function into the file by name (they're independent blocks) and
merges header additions; the file links only when all groups are done.

## Grind 35 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

Linked `eff_whirlwind_n64`, `eff_butterfly_n64`, `eff_fire_spark_n64`, `eff_rappa_n64` (all `effect/n64`).

- `.data` order is declaration order: a `{-1.0f, 1.0f}` pair goes after the vertex table, and `scale_data` before it in `fire_spark`. A `.data` table that `dol_gen.py --append` leaves as `u32 x[N];` is generated from the disassembly (`butterfly`: 630 words, `(u8*)` cast at its use).
- A trailing `gap_NN_..._data` after a compiler jump table is a zero word declared after the code with `#pragma explicit_zero_data on` (`whirlwind`); an `.sdata` gap is a longer array (`col_b[8]`, `color_rotation_data[40]`); an `.sbss2` gap is one more `GXColor` in the idiom, declared first.
- A literal `0.0f` kept under `MUST_MATCH` next to named constants makes `ld` load an anonymous zero at another `@sda21` offset: one byte differs in the dol (`fire_spark`). Keep the first named constants of the original's order (`dat`, deg2rad, ...) and make the rest literals (`pool_floats.py --keep=...`): the literals then pool in first-use order.
- The ratchet compares per-symbol data: `eff_whirlwind_n64` matched_data 91.18 -> 16.18 because its named string/double/colors became compiler-pooled; the unit is byte-exact and linked.
- Near-misses that stay open because the pool only matches with a named constant that changes the function (permuter targets, queued on the tower): `eff_magic3_n64` `effMagic3N64Entry` (99.84%, named `float_100`), `eff_peach_dust_n64` `effPeachDustDisp` (99.73%, named 0.7: f2/f4 allocation).

## Grind 36: first permuter wins (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

Tower batch 2 (`~/permuter/runs.log`) found, and these were re-expressed by hand:

- `iconGetTexObj` 97.54 -> 100% (permuter score 0): `IconBinaryEntry` is now modelled as its real layout (8-byte header + `IconFrame frames[]`; `IconFrame` moved to the header), and the natural loops read `data->frames[i]`. Under `MUST_MATCH` the frame table is walked through a local pointer from an inline accessor (`iconFramesOf`, `frames[i + 2]` from the entry start): the permuter's `inline_fn(arg) { return (IconFrame*)arg; }` + `new_var`. The accessor matters (a plain cast or `frames = data->frames` scores 96.65 / 95.38). Macros `ICON_FRAMES_DECL/INIT`, `ICON_FRAME` keep both forms. Same change: `iconDispGx2` 93.48 -> 96.0, `iconDispGxCol` 92.88 -> 95.5, `iconDispGxAlpha` 96.60 -> 98.56.
- `BattleAudienceCtrlProcessKinopioDamage` 97.50 -> 99.76: the 270.0f constant is loaded into a local before the stores (`f32 angle`, `MUST_MATCH` only; the natural form stores `float_270_8041e170` directly).
- Not applied: `kpaSwimMove` (220 -> 90) adds a duplicate `f31 = ... * fabs(speed)` statement inside a clamp; it is not natural code.
- Pitfalls: `perm_prepare` needs the tower's `build.ninja` to be the Mac's (copy it over `~/ttyd-perm/build.ninja`), and `mwcc -E` output holds Shift-JIS bytes (decoded with `errors="replace"`). `prepare-failed` lines in the log mean one of those two. The supervisor stopped once at load 20 (other waves), as designed.

## Wave 34: REL files outside #87, whole-file finishing (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

The 20 REL files with open functions that #87 didn't touch, one agent per file on all of its open functions
(closest first), wave 30's safety recipe, 45-120 minutes by work (scratch/wave34). gon_03 links; 12 functions reach
100% and 20 improve, none lower (7,907 -> 7,912 with what was kept). Left out: hei_10, rsh_05, yuu_02 (a section
score drops), gor_10 and usu_00 (a pragma without a match), and unit_boss_gundan_zako_group2: 7 of its 9 unwritten
functions match, but while zakogun_ext_main2 (3.6 KB) is missing its scripts and jump table point into a short
.text and the unit's matched_data falls from 88.7% to 0; its agent file stays in scratch/wave34 as the start for
finishing it. A fresh Claude review found no regression and three bugs on main the wave fixes: las_04
`firebar_entry` had its loop-sound check inverted; rsh `rsh_window_tev` passed the frustum's left/right with the
wrong sign, and read through NULL where retail walks its ancestors; unit_barriern_z passed `a` where retail's
first `intplGetValue` takes 4. Labels and agent notes cleaned up after it.

## Wave 33: near-misses (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

The next 48 dol near-miss files (closest function at 85% or better, units with one or two open functions), 13:43-15:20,
from main b369036. **18 of 48 reached 100%, 16 improved, 14 unchanged, 0 worse.** Lower than waves 28-31: the queue now
reaches into the 85-90% band. Fixed at collection: rothammer_fail had extern'd constants its unit pools itself;
battle_mario's `_hammer_star_effect` pinned its load order with extern'd constants, which cost the unit's .sdata2,
so it is back to main's version.
Header follow-ups: iconDispGx's icon id should be u16 (twoddisp, lst_rc_keep and battle_menu_disp cast around it), romfont.h's glyph width should be u16, and the battle_pad.h return types should be signed (timing_wide casts to s32).

## mri_puni structs (2026-09-30, Claude Opus 5.5 "TTYD Claude #2")

PuniWork and PuniData are rebuilt from every access the mri asm makes through Punigp and Punidat elements:
tools/mripuni/scan.py tracks registers per function (the globals loaded directly or through `addi G@l` + `lwz 0`,
`lwzu`, mulli/add 0x5A8 indexing and += 0x5A8 steps, `mr` copies, the puniGetPunidat/puniGetPunigp results, callee
parameters at any offset, state merged at branch targets) and structs.py renders typed fields: a Vec where a
PSVEC* call takes a pointer there, or where three words are copied as words and read as floats (triples whose
three members all show it win), f32 for any word read as a float, else s32/s16/u16 by access; upstream's names
kept. PuniWork is 0x4D0 bytes, as evt_puni_mem_init allocates (upstream's padding array ran to 0x448 while its
comments put showPuniCounter at 0x444, making it 0x4D4); with the fix evt_puni_mem_init matches. Codex's review of
the first version (#90) found the scanner missed `addi`-then-`lwz` global loads, interior pointers passed to
callees and state across branches (PuniWork 24 -> 53 offsets, PuniData 39 -> 85 after the fix), which had left
PuniWork 0x344-0x34C and PuniData 0x138-0x140 and 0x15C-0x164 as ints: they're Vecs. Fields are field_0xNNN until a
function shows what they are; wave agents use them and write a commented raw offset for anything else, which
collection promotes (rerun scan.py and structs.py).

## Grind 37: permuter wins, batch 2 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- `BattleAudience_Init` 98.58 -> 99.12 with `BattleAudience_GetExist((u32)index)` at its two call sites (`MUST_MATCH` only). The permuter changed the prototype to `unsigned long`, but the asm does not justify `u32` (`id` only passes through to `BattleAudienceGetPtr(s32)`; all callers mask the boolean with `clrlwi 24`), so the header stays `s32`.
- `battleAcDisp_PendulumCraneTiming` 98.08 -> 100%: a second pointer `p` to the dispParams is assigned before the switch and again in `case 0x63` (`MUST_MATCH`; natural form: one `p = ag`). The permuter's `new_var = ag` twice.
- `kpaEnemyHitChk` -> 100%: `mp->position.x = mp->position.x;` after the fire-hit check (`MUST_MATCH` only; a pure no-op that pins the float load order).
- Runs that found a lower score but nothing natural stay unapplied (`kpaSwimMove`, `hitDelete` 3605 -> 3175, `sort_5_3_func` 230 -> 225). `marioSetJumpPara` ends with `permuter-error` (the round trip does not compile; not looked at).

## Grind 38: six more units linked, the function-order check (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

Linked `eff_kemuri3_n64`, `eff_energy`, `eff_rothammer_fail_n64`, `ac_timing_wide`, `evt_twoddisp`, `evt_damage`.

- **Check the function order first** (`python3 tools/fn_order.py UNIT`): `ac_timing_wide` and `evt_twoddisp` had their functions in the opposite order, which reverses the literal pool (and moves the `.data` references to functions). `--write` moves the bodies; the static helpers and `USER_FUNC`/`EVT_DEF` symbols then need declarations before their first use (a prototype for a global goes in its header: `twoddisp_main` in `include/evt/evt_twoddisp.h`). `fn_order.py` cannot parse a few files (`evt_damage`, anything with an initialised table after the last function); compare `.text` symbol order by hand there.
- A zero-filled `const Vec v[12] = { 0 };` lands in `.rodata` (`ac_timing_wide`; the code read it through a cast).
- `.sbss2` pairs with `#pragma section ".sdata" ".sbss2"`, which captures every zero-initialised global after it: place the block after the file's own `.sbss` globals (`evt_twoddisp`'s `twodPtr`/`twodID`). A `gap_NN_..._sbss2` is one more object of the same type, declared first. A `gap_NN_..._data` after a script table is `#pragma explicit_zero_data on` + `__declspec(section ".data") u32 gap = 0;` at the end of the file; a GXColor constant (`dat_xxx`) is `const GXColor` defined after the code.
- **Named constant vs schedule.** Units where the original loads `0.0f` (and other constants) by name from the pool: a literal in the code gives the original's schedule but adds an anonymous pool entry; the named constant gives the right pool but the compiler hoists it into a callee-saved FPR (frame +0x10) or reorders the loads: `eff_break` Disp 93-94%, `eff_mist2` Disp 91%, `eff_magic3` Entry 99.84%, `eff_peach_dust` Disp 99.73%, `eff_snowfall` `cam_clip_chk` 99.90% (`/ 2.0f` becomes a `*0.5` literal; named `float_0p5` leaves a 3-instruction register swap). `MATCH_F32` around the named zero is 94.58 for `eff_break`. These are queued for the permuter on the tower with the named-constant variant as the base.
- `eff_toge_flush`: `0.9f` pools before 6.28/360 whatever the expression order (the target has it after); parked. `unit_mario`: reordering the functions drops one to 99.85% and the unit has 5 KB of tables; parked.

## Wave 36: near-misses (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

The next 48 dol near-miss files (closest function at 80% or better, units with one or two open functions), 15:25-16:52,
from main d6017e6. **27 of 48 reached 100%, 17 improved, 4 unchanged, 0 worse.** evt_item reaches 100% because
itemEntry's third parameter is now `u16` in both itemdrv.h and itemdrv.c (retail stores it with `sth` and masks it
with `clrlwi`). eff_batten's Disp uses main's literals for the vertex constants the unit pools, and reaches 100%.

- ac_power_gauge_lv2's timing windows are 1/3/5 seconds (`(i * 2 + 1)`), not 1/2/3 as main had them.
- For Isaac: eff_recover_heart's eight vertex arrays are reached as offsets from `lbl_803A4A60`, which is the last
  object of eff_rappa (splits end rappa .data at 0x803A4A68). The pair `{-1.0f, 1.0f}` there has the same shape as
  recover_heart's own pairs and rappa never references it, so the split is probably 8 bytes off and recover_heart's
  .data should start at 0x803A4A60. The offset form is kept under MUST_MATCH until the split moves.
- Provenance: the seq_battle agent fetched upstream doldecomp/ttyd (raw.githubusercontent.com, MIT) and browsed
  other workspaces (~/ttyd-w35base, ~/ttyd-agents, ~/ttyd-perm) despite the prompt forbidding it. The sandbox
  does not block network access yet.

## Wave 35: mri_puni in 22 groups, and the event units (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

mri_puni's 182 open functions went to 22 agents at once (tools/mripuni/groups.json, contiguous asm-order groups
of ~4 KB), each in its own copy of the file, writing only its group, on #90's first struct version; plus one agent
per event skeleton from #89. 29 agents, 14:47-16:24 (scratch/wave35).

**Collection.** A 3-way merge of 22 copies conflicts on every shared line (includes, declarations), so each
group's functions were spliced in by name (scratch/wave35/splice.py) with the static helpers, macros and typedefs
they needed; the declarations groups gave their own functions in mri_puni.h merged by name, the owning group's
signature winning. Then the agents' code was rebuilt on the corrected structs: word copies of the old int fields
became Vec copies; PuniData 0x210/0x214 are HitEntry* (hitCheckFilter results), 0x560 a Vec, PuniWork 0x364-0x370
PuniData* (overrides in tools/mripuni/structs.py); PuniMap and PuniTarget are shared types. The agents had declared
183 retail constants as externs (the stand-in pattern): each became a literal of its exact value from the asm
(scratch/wave35/literalize.py). Three pragmas left unreset changed every later function of the merged file: removed,
103 -> 117 functions at 100%. 464 raw offset accesses became struct members with unchanged code.

**Result:** mri_puni 6 -> 119 of 188 functions at 100% (.text 2% -> 88%); evt_shuryolight (aji, las) and
evt_kagemario (gra, jin) link; gor's evt_yuuranking and usu's evt_kagemario improve; yuu's evt_yuuranking is left out
(.data drops). 7,934 -> 8,095 functions.

**Review** (fresh Claude) found nine wrong behaviours, four in 100% functions, all fixed: puniAnimationInit2 copied
"D_1" for "D_2"; puniModeTrap looked up NPCs "カイド"/"カイド離" where retail has "ガイド"/"ガイド妹"; _ymode_mri_16_01
and puniOption used rounded constants (60.0f for 60.00006f, 0.045f for 0.044999994f: the dtk names round them);
puniParaAttack lacked a win condition; puniMoveLimitedNormal reflected where retail slides; unk_mri_0000980c set a
flag retail doesn't; puniModeChangeFallCheck set its hit distance once instead of per call; evt_puni_setup skipped
Punidat[109]; a MUST_MATCH `while` in puni_rnd looped forever on INT_MIN (retail uses `if`; its removal costs
puniSearchRandom 12 points). fabs/fabsf now use __fabs/__fabsf (math_ext.h declares the out-of-line ones), which took
puni_init_posy 20% -> 99% and puniTargetSearch to 100%. 687 TODO blocks the splice had duplicated are gone.

**Follow-ups:** puniMoveAtari is auto-inlined into its callers where retail calls it (puniModeWind_sub 2%,
puniModeFall/Updraft ~56%); PuniData 0x1D0-0x1DC and PuniWork 0x360 are PuniData* and PuniWork 0x374-0x43C is
PuniTarget[10], still raw in places; duplicated RNG/randf helpers; 9 mri_puni functions unwritten (puniModeAway*,
Attack, Damage, Piyori, fn_19_A820/A8DC, evt_puni_get_position); yuu evt_yuuranking and gundan_zako_group2 (wave 34)
have agent files in scratch to finish from.

## Grind 39: permuter batch 4 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- `BattleStatusChangeMsgMain` 97.43 -> 99.03: `(char)*pri != -1` in the priority loop moves the walking pointer to r4 (still `extsb` + `cmpwi -1`; MUST_MATCH only, natural `*pri != -1` in `#else`).
- `perm_run.py`: a `base.c.manual` in an input directory replaces the generated `base.c` after every prepare, for hand-written `PERM_GENERAL` searches. `eff_break` Disp was searched over named vs `MATCH_F32` at all 10 zero-constant sites (exhaustive, 174 s): no combination improves the base (the frame stays 0x10 larger), so the pool/schedule conflict there has no cheap answer. `eff_snowfall` `cam_clip_chk`: an inline `x * float_0p5` helper for the `cz` line reaches 99.92 (permuter score 15); parked. `nameEntOn`/`winBadgeMain2`/`marioSetJumpPara` end in `permuter-error` (the round trip does not compile).

## Wave 37: the closest near-misses (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

The 48 closest open functions left in dol files (all at 99.77% or better, any number of open functions in the unit),
16:45-18:03, from main 835c93a. **27 of 48 reached 100%, 1 improved, 20 unchanged, 0 worse.** The unchanged ones are
the register-allocation tail the permuter targets. Fixed at collection: eff_kemuri, eff_itemget and sac_scissor had
extern'd constants their units pool themselves; mobjdrv's mobjNearDistCheck2 had become a pool-base hack that cost
the unit's .rodata, so it is back to main's version.

Three rewrites fix bugs in main, each confirmed against the retail asm:
- imgMain tested the shadow flag inverted; the loop now exits at the first shadow with flag 4 set.
- BattleAudience_Case_TurnEnd called BattleAudienceNumToTarget unconditionally; retail skips it when flag 0x40000 is
  set.
- BattleIconInit cleared only 10 icons; retail clears 16 (0x9C0 bytes), so `BattleWork.icons` is now `[16]` and the
  padding after it starts at 0x18C70.

Two functions outside the targets also gained from mapdrv's const definitions of float_0_80419850 and
float_1_80419860: mapCheckAnimation 94.21 to 99.47 and mapCalcAnimMatrix 93.45 to 94.26.

## Wave 38: mri_puni's last 69 functions; two restarts (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

mri_puni's 69 open functions (9 unwritten, 60 partial) in 18 size-balanced groups, one agent each on the corrected
structs, plus yuu's evt_yuuranking and unit_boss_gundan_zako_group2 restarted from their earlier agents' files
(scratch/wave38). 20 agents, 17:17-18:01. Collection spliced by name as in wave 35, a group's version replacing the
tree's only where it differs from the base (scratch/wave38/splice.py); the helpers groups changed or added came
along; three named retail constants became literals (scratch/wave38/literalize2.py, which infers each type from the
asm and works for any REL unit). PuniData 0x1C8 is `OSTime modeStartTime` (structs.py override): a mode's start
time, written by puniModeWater, the fall modes and puniModeEnter.

**Result:** mri_puni 119 -> 144 of 188 functions at 100%, every function written (.text 97.7%); yuu evt_yuuranking
7 of 9 with its .data at 100% (wave 35's attempt had lowered it); gundan_zako_group2 reached .text 99.4% but its
data still drops while shake_ceil (a work-pointer materialisation) and zakogun_ext_main2 differ: left out again.
8,122 -> 8,154 functions.

**Review** (fresh Claude): puniMoveLimitedNormal only ran its wall test with flag 0x1000 set, where retail always
calls hitCheckFilter (a puni walked through walls; the line predates the wave); PuniSakanobori returns the root it
walks to (wave 35's review item, unfixed until now); puniModeRecover/Fall's jitter is `puniRandf(0, 2) - 1`, which
rounds twice as retail does instead of one fused multiply-subtract. puniModeAway and puniFukki are wrapped in
`#pragma push / dont_inline on / pop`: retail calls puniModeAway_sub and puniDeleteEffect with bl, and MWCC would
inline them (42% -> 99.9%, 97.4%; the one exception to "no pragma without a match", since it states a fact about
retail). Labels that blamed the wrong cause are rewritten.

## Grind 40: eff_sandars linked; the named-prefix rule; two permuter wins (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **`eff_sandars` linked** by keeping the first constants named (`dat` x2, deg2rad, 32, 0.5, 16, i.e. up to the last one the original loads by name) and turning every later constant, the `Z_1` string and the `0.0f`/`1.0f` `MATCH_VOLATILE` views into literals: the literals pool in first-use order after the named ones, which is the original's interleaving, and a literal zero/one load then reads its own pool slot instead of a named slot at another offset (one byte in the dol). `tools/pool_prefix.py UNIT [--apply]` tries k = 0..n named constants (after `pool_define`) and reports the worst function and whether `.sdata2` is equal; `tools/pool_floats.py --keep=` does the conversion. For `eff_break`, `eff_mist2`, `eff_magic3`, `eff_peach_dust`, `eff_kemuri9` no k gives 100% with an equal pool (kemuri9: k>=7 is 100% but leaves the `0.0f`/`1.0f` literals of the vertex writes as extra entries; k=0..6 is 99.88%, an operand-order swap of the hoisted 0.5).
- **Definitions before the code fold, after the code do not.** `mot_dokan` defines its named floats before the code (function 100%, but a second copy of every literal lands in `.sdata2`); moving the definitions after the code gives the right pool (with the `M_Z_1` string defined after `float_0`) but `mot_dokan` drops to 99.52% (hoisted loads in other registers). `countdown`: the `dat_` colours must be `extern const u32` + defined after the code, the zero word in `.sbss` is a `u32` declared before `wp`; a literal 300.0f (the pool needs it after the 0.5) drops `countDownMain` to 97.81%.
- Permuter wins: `checkOneRevolution` 100% (a pointer to the ring head taken before the loop, `MUST_MATCH`), `unk_8017599c` 99.96% (scale `pos.y` before `pos.x`).

## Wave 39: near-misses 98.5-99.98% (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

The next 48 closest dol near-misses outside the permuter's queues, 18:06-19:34, from main 157e3e2. **23 of 48 reached
100%, 10 improved, 15 unchanged, 0 worse.** Fixed at collection: eff_mahorn2's externs for constants it pools itself;
party_cloud's `pf ? -90 : 90` pooled -90 ahead of 90/270 (`pf != 0 ?` restores the order); mot_jump's new zero-Vec
literal added a duplicate pool entry, so the file is back to main's version.
Four rewrites fix behaviour bugs in main, each checked against the asm: camRoadMain's unk_1C8 store now runs every
time (main did it only inside the unk_D0 > 1.0 clamp); hitCheckSphereFilter tests entries[0].count (main read the count
through the moving entry pointer); effDamageStarDisp's else-quad texcoords are 2.0 (main had 1.0); effGetTexObj's id
compare is signed (`cmpwi`, not `cmplwi`). Two agents read the tower's main checkout, and four touched
/home/isaac/permuter (timestamp ran permuter.py). No third-party code came in.

## Wave 40: DOL whole-file pass (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

At Claude #1's suggestion, the DOL files its near-miss waves leave behind: every open function either untouched and
over 1 KB or partial below 80% (41 files, 67 functions; library units are out of scope, ported as libs). One agent
per file on all its open functions, 60-120 minutes by size, sharing the tower with wave 38 through a slot check.
data/database and data/battle/battle_item_data had no source (src/database.c and src/battle/battle_item_data.c are
other units): their skeletons come from dol_gen with global declarations in include/data/*.h.

**Collection.** Agents again declared retail constants as externs; replacing them with literals moved FP loads in
functions that were at 100%, so they are defined after the code in the original order with tools/pool_define.py
(Claude #3's), which leaves the code unchanged. Left out: mot_ship, evt_item, battle_item_data, eff_small_star,
eff_sleep, eff_opuku_wcurtain (a section score drops), eff_pokopi_thunder (a 100% function slips), ac_stick_keep_left and
eff_kameki_tornade (pragmas without a match). Kept pragmas state retail facts: dont_inline where retail calls the
helper with bl, optimize_for_size where retail saves registers with stmw.

**Result:** mot_yoshi, mario_cam and battle_ac_help link; 8,181 -> 8,196 functions. Seven more units are code-complete
but reference data their split owns through undefined stand-ins (tools/finish_unit.py is the next step for them).

**Review** (fresh Claude) found seven wrong behaviours, all fixed: fadeEntry mapped fade types 41-48 to the wrong
animations (the pipe fades were shifted by two); disp_texture loaded a 3x4 texture matrix for retail's 2x4; seq_load's
copy screens read the wrong window table; eff_stardust2's sin argument had its sign flipped; ac_gauge_timing checked
the wrong field for success, computed the gauge ratio from the unclamped value (100x too large) and drew the wrong
button; ac_signal_timing's u8 states never compared equal to -1, so the NG mark never drew. Also retail-exact floats
(0.59999996f, 0.16000001f, 0.080000006f) and eff_sstar's first rotation angle. Stubs carried over from main remain:
fadeMain (4.3 KB) and BattleBreakSlot_Main (5.6 KB).
Codex's review of #106: seq_load reached win_dt/win_dt2/win_dt3 through npc_event's lbl_80349DE8 plus fixed offsets;
that stays only under MUST_MATCH, with the named tables in #else; evt_shake and evt_continue are EVT macros (the latter
passing the file's named strings, which keeps .sdata at 100%). Every changed file compiles without MUST_MATCH. Normal
for NonMatching units and noted here: eight of this wave's units still reference data their split owns through
undefined stand-ins (they link only after tools/finish_unit.py), and seq_load's loadDraw is registered but still a
TODO.

## Grind 23 (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

- **Per-file flag sweep** (`tools/flag_sweep.py UNIT [--quick] [--json OUT]`): recompiles a dol unit with -O4,p / -O4,s /
  -O2,p / -O2,s, `-inline auto` against the unit's own `auto,deferred`, and with or without the file's
  `#pragma optimize_for_size` lines, then lists every function's score per variant and the variants where nothing drops.
  Objects go to a scratch dir. Run over all of `battle/ac/*`, `battle_ac_help`, `battle_icon`, `battle_enemy_item`,
  `sac_deka` and `sac_genki`.
- **Result**: -O2 is worse everywhere (2 to 10 functions drop per unit). -O4,s with the pragmas removed matches today's
  score for every unit that carried `optimize_for_size` pragmas, so those fourteen files now build with
  `extra_cflags=["-O4,s"]` and the pragmas are gone. Two gained: battle_icon (BtlIcon_Entry 7.6 -> 71.5, BattleIconMain
  59.4 -> 81.0, BattleIconEnd 72.6 -> 100, btlevtcmd_BtlIconSetFallAccel 77 -> 100) and ac_gauge_timing2 (battleAcMain 91.25
  -> 96.10). The units already at -O4,s (ac_lst_rc_keep, ac_stick_rotate, ...) and the four ac units with no pragma
  (ac_shot, ac_signal_timing, ac_gauge_timing, ac_repeatedly_lv) score the same under every -O4 variant, so flags do not
  gate the remaining battle/ac functions: their gap is register allocation. The wave-24 report of
  battleAcDisp_GaugeTiming2 at 100% under -O2,p does not reproduce (both functions of the unit drop under -O2).
- **Review needed**: compiler-flag changes wait for Isaac's morning review.

## Standing priority: match first, link later (2026-09-30, Isaac)

Getting every function to 100% is the goal; linking every unit is secondary and comes as a cleanup phase once units
are fully matched (data, splits and function order are mostly mechanical by then). Waves, the permuter and model
choices are judged by matched functions; linking work happens when it is cheap (autolink, a unit one fix away) or
when a unit is already 100% matched.

Wave 40 follow-ups (Sep 30, Claude #2): arcdrv links once its functions are in rule 18's reverse order (every
function had moved by 0x54 with each at 100%); eff_thunderflare_n64 links once effThunderflareN64Entry loads the
named float_0_80420088 instead of a pooled 0.0f (tools/dol_diff.py showed the one word). tools/finish_unit.py took
eff_shock's .data/.sbss2 and eff_star_power's .sbss2 to 100%; their remaining pool order keeps them unlinked.
battle_item_data's 61 item weapons are typed BattleWeapon. The two stubs are gone, written by Claude subagents from
the asm: fadeMain at 100% (and <math.h> -> math_ext.h lifted fadeEntry to 94%), BattleBreakSlot_Main at 98.46%
(its event scripts are the unit's undecompiled .data). Header types flagged along the way: FadeEntry 0xA4 is
signed, 0x10/0x14 is one s64 duration, ImageEntry 0x108 is a const char*; mariost.h's language compares unsigned.

## Wave 41: REL whole-file pass (2026-09-30, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

Every REL file with open functions (62 files, 255 functions) at Claude #1's request: one agent per file on all of
them, mri_puni's 44 split into 7 groups spliced by name; 69 agents, 19:25-20:51 (scratch/wave41).

**Result:** eight units link (dmo_00, evt_lect_dou, las_29, unit_boss_batten_satellite, evt_lect_mri, evt_lect_tik,
unit_boss_macho_gance, usu evt_kagemario); 8,222 -> 8,267 functions, code linked +0.90%, data linked +1.36%. Undefined
named-constant externs became literals (scratch/wave38/literalize2.py, now restricted to names a file declares but
never defines; it had also rewritten constants files define themselves). Left out: ten files where a section or a
function drops (gundan_zako_group2 reached 8 of 9 but its data still drops while shake_ceil differs), sys, tik_05,
yuu_02 (pragmas without a match).

**Review** (fresh Claude): evt_yuuranking_disp_func, newly written, reused its row counter for minutes (the time-mode
rank loop could hang), drew the title frame 50 units off, swapped modes 1 and 3's subtitles, and applied the icon
offsets before truncating; all fixed against the asm, with retail's exact floats (75.19998f, -75.200005f,
-37.600006f). The label script dropped the blank line before each function it touched (fixed; eight files that
changed by nothing else went back to main). Bugs on main this wave fixed: aji arm_main's 2π, barriern_satellite's
intplGetValue mode, tou's NPC names (ガンス, キノシコワ), tou_02's group pointer off gp, yuunpc's row index and pose
index. Found in passing, older than the wave: hei_10's 0.2f is 2 * randf(1.0, 1.2) (0.20000005f).

## Wave 42: the closest near-misses (2026-09-30, TTYD Claude #1; agents DeepSeek V4.1 Flash)

The 48 closest open dol functions (99.62-99.99%) outside the permuter's queues, 21:08-22:09, from main c0327be, at
10 agents at a time while the model benchmark held the rest of the key. **24 of 48 reached 100%, 2 improved,
11 unchanged, 0 worse; 11 agents produced no output** (they hit the tower's ssh switch from Tailscale SSH to OpenSSH)
and go into the next wave. unit_party_christine's `lbl_80390B70` (0x80390B70, in-file stand-in for a symbol that is
not in symbols.txt) is the same 8-byte split issue as #68's scan: the `{ -1.0f, 1.0f }` pair at the end of
`unk_evt_80390b30` needs its own symbol.

## Grind 41: the 8-byte `.data` boundary fixed for the 32 units of split_scan.py (2026-09-30, Claude Sonnet 5.5 "TTYD Claude #3")

Isaac approved the splits.txt edit for this line of work (asked in chat; the orchestrator relayed the plan). `tools/split_pair_fix.py` (new) moves, for each of the 32 units `tools/split_scan.py` lists, the end of its `.data` and the start of the next unit's `.data` 8 bytes down: the `{-1.0f, 1.0f}` pair becomes the first object of the next unit's `.data`, as in nokotarou (Grind 28).

- Linked units that defined the pair at the end of their own `.data` (battle_party, eff_bomhei_n64, eff_fire_spark_n64, eff_kumokumo_chip_n64, eff_onpu_n64, eff_rappa_n64, eff_snowman_n64, evt_memcard, unit_bomzou, unit_koura) lose that definition; unlinked sources that carried one lose it too (eff_recover_heart_n64, battle_attack_audience). The next unit's source now defines the pair as its first `.data` object, with the type its `extern`/header declaration used (`u8[8]` for the unit_party_* and eff_* anchors, `f32[2]`, `u32[2]`); the extern anchors in those files are gone. `eff_hokori_n64` (linked) got its first pair this way.
- A 16-byte and a 12-byte all-zero gap that had been folded into the preceding array (`eff_kumokumo_chip_n64` after its vertex table, `evt_memcard` after `evt_memcard_save`) are separate `explicit_zero_data` + `force_active` objects now: with the shifted split the target's symbol for the array is 8 bytes shorter and objdiff compares per symbol.
- Evidence: `ci.py` passes with the dol and every REL byte-identical (`build.sha1`), TOTAL matched_data 69.74 -> 69.81. matched_data of the 31 touched units moves by a fraction of a percent (the 8-byte pair changes each unit's data total), excused one by one with `--allow-regress`; none loses bytes.
- **Needs Isaac / follow-up:** the `weaponBase`/`lbl_ + offset` anchors in the next units' code (unit_party_yoshi/chuchurina/vivian/clauda, eff_* `lbl_` base registers) still read the pair through `lbl_XXXX + 0x...`; with the pair now in the same unit the code can name the objects directly. That is a per-function rewrite (each is a MUST_MATCH hack with a score to protect), not done here.

## Grind 42: unit_party_clauda links with its weapons named (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `unit_party_clauda` (9/9 functions at 100%) links with the nokotarou recipe: `tools/dol_gen.py UNIT --append SRC > new.c`, revert the `include/battle/*` declarations it adds (they clash with other files' `extern s32 X[]`), declare the handlers it names as local `extern s32 X[];` arrays (the build tells which), `$` in names becomes `_`, and `partyWeapon_*` are the generated `u32 X[48]` objects referenced as `(BattleWeapon*)X`. The `weaponBase` anchor (`lbl_80376320 + 0x40...`, a `MUST_MATCH` hack) is gone: with the pair now the first object of the unit's `.data` the compiler itself addresses the weapons as `lbl + offset`, byte-identical. The compiler's own int-to-float double must not be defined by name (a second copy lands in `.rodata`), and the 4-byte `gap_07_804122AC_sdata` is a zero word under `explicit_zero_data` + `force_active`.
- Not done: `unit_party_chuchurina` has the same anchor but two functions are not matched (`_get_binta_hit_position` 92.29%, `_chuchu_item_steal` 0%), `unit_party_yoshi`/`vivian` are claimed by wave 43. `unit_lecture_frankli_sac`, `gor_02`, `evt_lect_usu` are RELs (Claude #2).
- **Needs Isaac**: porting the open SDK/MSL/MusyX/THP library functions from the other MIT/CC0 decomps means cloning those repositories, which I did not do (not covered by his approval of the permuter clone).

## Grind 43: unit_mario links; strings pool in first-use order across code and data (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `unit_mario` (7/7 at 100%) links: the nokotarou recipe for the 13 scripts and tables (`dol_gen.py --append`, handlers as local `extern s32 X[];`), and the string pool needed three changes. (1) The `str_802F3780` pool-base hack is gone: the `b_mario_*` names are plain literals and the compiler addresses its own pool. (2) The two `SFX_VOICE_MARIO_RELIEF*` strings that the code reads through extern names are literals now (a name that no one defines leaves the string out of `.rodata`). (3) The GXColor read by `callback` is a compound literal, and the seven functions sit between the pose tables and the event scripts: small strings (<= 6 bytes) and the colour literal share `.sdata2` in first-use order across code and data, and the target has the colour between `M_I_O` (a table) and `M_I_S` (the first script), so `callback` must come after the tables and before the scripts. Everything else stays in address order.
- Lesson for the next unit with `str_`/`dat_` objects interleaved in `.sdata2`/`.rodata`: the original was one source file with data and functions interleaved; first uses decide the pool, so look at which data object holds the neighbouring strings and put the function there.

## Grind 44: the constant pool follows the text order and the first use inside an expression (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

Linked `eff_release_n64` and `eff_toge_flush`; found the rule behind most of the pool-order failures (micro tests in a scratch dir, one function at a time with the project's flags).

- With `-inline auto,deferred` the text order is the reverse of the source order, and the literal pool follows the **text order**: the constants of the function that comes first in `.text` are pooled first. Inside a function the order is the order of first use in the expression tree, and a call's argument constants (`sin(6.28f * x / 360.f)`) are pooled **after** the other constants of the statement that contains the call: `a = 0.04f * sinf(6.28*x/360) + 0.5f` pools 0.5, 0.04, 6.28, 360, but `s = sinf(6.28*x/360); a = 0.04f * s + 0.5f;` pools 6.28, 360, 0.5, 0.04. So when the target has a call's constants before the surrounding ones (or the other way round), the call result goes into a temp (`eff_release` `data->scales[0]`, `eff_toge_flush` both `sin` statements; `eff_lensflare` the `cos` one) without changing the code.
- A constant folded away that the target keeps (`-16.0f * 0.5f`, `eff_mist2`) stays unfolded with `f32 sixteen = 16.0f; f32 half = 0.5f; f28 = -sixteen * half;` (Disp 100%); the pool then has 0.5 before 255 where the target has it after: left to the permuter.
- `.data` tables only reached through the first object are unreferenced to the linker: `#pragma force_active on/off` around them (`eff_release_n64`: 800 bytes were dropped from the dol).
- Queued for the tower permuter (hand-prepared variants in `~/ttyd-perm/src`): `effShockMain`, `effPeachDustMain`, `effLensflareDisp` (one `fcmpu` operand order), `effBombDisp`, `effGarekiDisp`/`Main`, `effMist2Disp`.

## Grind 45: constants in locals (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

Linked `eff_nokotarou`, `eff_shock_n64`, `eff_lensflare_n64` (the near-misses that the literal pool leaves at 99.4-99.98%).

- With the literals pooled in the target's order (Grind 44) the remaining difference in a function is often only the **order of the FP loads**: the constant is loaded after the variable it multiplies, where the original loads it first. A local that holds the constant, assigned before the variable is read, moves the load: `f32 k = 0.03125f; ... scaleAnim += t * k;` (`effNokotarouMain`, the blend `(C - v) * k + v` with the constant in a local), `f32 k9 = 0.9f; f32 k8 = 0.8f;` at the top of `effShockMain`, `f32 zero = 0.0f; if (f30 == zero)` in `effLensflareDisp` (the compare's operand order follows). The pool does not change.
- Strings that the code names through extern objects (`str_803014A4`, `str_8042249C`) are literals at their uses: an undefined name leaves the string out of `.rodata`/`.sdata2`.
- Not enough: `effPeachDustMain` (`k = 0.1f` hoisted to the function top, to the loop start, or after the counter: 98.86% in all), `effBombDisp` (the natural `half`/`negHalf` form pools 1.0 and 0.5 before 30; the literal-in-loop form has the right pool and 97.93%).

## Wave 50: the overnight whole-file pass, DOL low-only files and the REL remainder (2026-10-01, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

Isaac's 12-hour run (Sep 30 22:30 to Oct 1 10:30). Claude #1 runs grouped near-miss waves (42-49) on heavy DOL files;
this session runs whole-file waves (50-59) on what is left below 80%, Flash only (Claude #1's six-model benchmark
had Flash ahead on near-misses). Files go to whichever wave claims them first in `tools/claims.py`.

- **Scope:** the 23 DOL game files whose open functions are all below 80% (`sdk/arc` left out as library code)
  and every REL file with open functions: 93 files, 308 functions, 98 agents (mri_puni in six groups spliced by
  name). Launcher `scratch/wave50/launch.py`: waves 40/41's recipe plus **continuation passes** (an agent that stops
  with listed functions still open and 15+ minutes of budget left is told to keep going, up to twice) and a slot
  cap of 24 shared by every w5x wave. The continuations fired for most agents; Flash tends to stop early.
- **Collection** (`scratch/wave5x/collect.py`, reusable): agent copies merged 3-way onto main (main had moved four
  of the files' `.data` boundaries); groups spliced by name; for a shared `rels/common` source worked by several
  modules' agents, the copy that linked is taken whole (`unit_hammer_bros`: the tik copy linked in tik but dropped
  tou2's `hammer_bros_nagemakuri_set_target` from 68% to 14%; the tou2 copy holds both). `scratch/wave5x/compare.py`
  compares every unit's functions and sections with main's report; `greedy.py` reverts changed functions one at a
  time until no section is below main; `pragma_audit.py` deletes each added pragma and keeps it only if a function
  falls from 100% without it (16 removed).
- **Left out** (a data section fell, or nothing new reached 100% once the drop was fixed): gor_10, hei_10, las_05,
  yuu_02, tou_03, end_unknown (two new 100% functions, but `.rodata` 99.73 -> 99.60: a literal value differs),
  muj_20, rsh_05, rsh_06, eki_00, unit_barriern_custom, eff_kameki_tornade, eff_pokopi_thunder; bom_01, tou2 and
  unit_boss_gundan_zako_group2 because their agents' `rels/*/include` edits were lost (56 of the 98 workspaces were
  deleted on the tower mid-run; the launcher only brings back `include/` as a patch). Also left as on main after
  review: eff_opuku_wcurtain, eff_balloon.
- **Result:** 8,336 -> 8,431 functions; unit_chorobon_gundan, unit_boss_batten_leader and yuu_01 link. party_kuribo,
  battle_item_data, eff_toiki, eff_pturn, eff_small_star and eff_thruhammer are code-complete; finish_unit.py
  didn't link them (data sections at 0-98%), left for a data pass.
- **Review** (fresh Claude, `scratch/wave50/review.md`): ac_shot's afterimage GXColors were byte-reversed;
  evt_yuuranking (yuu and gor) had moved the title box to `w - 50`; sys `display_func` wrapped its frame at 8 for
  retail's 4; mot_ship added `f30 - f30` for retail's constant 0.0; eff_opuku_wcurtain had dropped the s16 clamp;
  devices in muj_00, unit_boss_batten_leader and eff_sleep now sit under MUST_MATCH. Found outside the wave:
  usu_00 `kpaSen_ext_shadow_disp`'s shadow matrix was 20.0f on the diagonal for retail's 10.0f (100% score, wrong
  size). battle_item_data's two constants stay mutable globals: making them `const` loses `_cam_rot`.
- **Lesson:** the launcher's "wave 50 drained" gate counted workspaces on the tower, so the cleanup stalled wave 51
  for three hours; it now gates on the slot count alone.

## Grind 46: eff_break and countdown retries (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

No unit linked in this stretch.

- `eff_break` Disp: `f32 w = 48.0f; f32 h = 0.5f;` at the top (C89: declarations first) stops `-48.0f * 0.5f` folding to -24 but pools 0.5 after 48 and before 0 where the target has 0 before 0.5; adding `f32 z = 0.0f` leaves the pool unchanged and drops Disp to 94.6%. Left for the permuter.
- `countdown`: only `countDownMain` (95.61%) differs in code; `.sdata2` lacks the trailing 4 zero bytes and `.sbss` is 4 bytes short.
- The tower permuter's lowest scores so far are prototype or declaration-type changes (`iconDispGx` `char iconId`, `shadowInit` `unsigned long long`), not matches.

## Grind 48: four of the 30 newly fully-matched units link (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

Linked `eff_fall_leaf_n64`, `eff_akari_charge_n64`, `eff_small_star_n64`, `eff_thruhammer_n64`. `event/evt_lottery` linked in its own PR (#132, no source change).

- `eff_fall_leaf_n64`: the colour word `str_8041EDBC` (a 4-byte `.sdata2` object the code loads as a `GXColor`) was not defined; `const u32 str_8041EDBC = 0x7592A600;` with the other `dat_` words.
- `eff_akari_charge_n64`: the call result in a temp (`s = (f32)sin(...); ... 1024.0f * s`) fixes the pool order (rule from Grind 44); `f32 k = 0.0625f;` assigned at the top of Disp and used as `part->scale * k` restores the operand order of the `fmuls` (the local-constant idiom of Grind 45); a `Vec disp = { 0 }` in code makes the compiler pool the zero vector itself, so `vec3_...` must not also be defined (it duplicated the 12 bytes).
- `eff_small_star_n64`: the code's two `0.017453292f` literals loaded the constant a second time at the end of `.sdata2`; they use the named `float_deg2rad_8041fc80` (second use written `float_deg2rad * x`); the `.sbss2` GXColor is declared at the end of the file, after the unit's own `.sbss` static (`kaiten`), or the `.sbss` object disappears.
- `eff_thruhammer_n64`: the four `.sbss2` words are `u32` objects whose address the code takes: the sbss2 idiom with `u32` instead of `GXColor` (`finish_unit`'s step declares `GXColor` and fails the build).
- **Not needed:** the 4-byte zero words that dtk shows at the end of `.sdata2`/`.rodata` (`gap_09_..._sdata2`) are alignment between units; the linked dol is identical without defining them (they did not link `eff_akari_charge_n64` when defined, because named objects are emitted before the pooled literals).
- Not linked: `eff_cool_leaf_n64` (data and code 100% after `pool_data` and `(u8*)tbl + 0xA`; the compiler pools its own `4503601774854144.0` after the string where the target has it before), `eff_pturn_n64` (the pool needs 360 before 4.0 in `data->field_0x20 = (type == 0 || type == 1) ? 4.0f * cos(f31 / 360.f) : 0.0f`; an `if`/`else` with a temp fixes the pool but gives effPturnMain 93.8%, a local `d = 360.f` leaves the pool as it was).
- `ci.py` needs `--allow-pragma` for the new `#pragma section ".sdata" ".sbss2"` in the three units (the idiom of AGENT_BRIEF, each with its why-comment).

## Grind 49: party_kuribo links (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

`party/party_kuribo`: `tools/dol_gen.py --append` for its one event script, then `finish_unit` (sdata2 pool and string came out right); no hand edits.

## Grind 50: ac_monosiri_target links (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

`battle/ac/ac_monosiri_target` (note: its `configure.py` entry has `extra_cflags=["-O4,s"]`, so a `sed` on the plain entry text misses it).

- Pools: `pool_data` for `_frame_table`, `pool_define` for the floats/doubles/vectors, plus by hand the five `dat_` GXColors (they sit first in `.sdata2`, defined before the floats) and the string `str_80300910` between the two doubles (named, so it lands between `double_15` and `double_28`).
- The `MUST_MATCH` code reads the eleven vectors through an array view (`extern const Vec vec3_80300880[11]`), so the definitions are one 11-element array under `MUST_MATCH` and eleven `Vec`s otherwise.
- `math.h` pools two sqrt doubles (0.5, 3.0) twice that this unit does not have; `math_ext.h` plus `#define fabs(x) __fabs(x)` keeps the `fabs` intrinsic without them.
- `double_15` is only reached through the vector base under `MUST_MATCH`: `#pragma force_active` or the linker drops it (the dol came out 32 bytes short in `.rodata`).
- `tools/fn_order.py --write` moves the function bodies but leaves the `#pragma` blocks and comments where they were; the `opt_common_subs`/`dont_inline` pragmas had to be put back around their functions by hand (the lint then reports them as new: `--allow-pragma`).

## Grind 51: eff_toiki links; dol_gen `.byte` tables (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `tools/dol_gen.py` stops with `unsupported .byte` on an object made only of `.byte` lines; the patch that writes it as a `u8` array is a separate PR (the file is a protected path: needs Isaac). This unit's data was generated with that patch.
- `effect/eff_toiki`: the unit's data (28 objects, one of them `queen_toiki_dl_0_size_tbl`, a `u8[16]`) via `dol_gen --append`, then by hand: the pair `lbl_803B2FA0` was defined twice (the source's old anchor definition went, the generated one stays with an `extern` above the code), `negone_one$374` style names to `negone_one_374` (as in clauda/christine), `#pragma force_active` around the tables (all but the first are reached through one base pointer, the linker dropped 928 bytes of `.data`/`.sdata`), and the static `color_tbl` moved after `queen_toiki_dl_0_size_tbl` (a forward `static GXColor color_tbl[3];` above) because it is the last object of the original `.sdata`.

## Grind 52: snd3d's emitter layout, ac_power_gauge, eff_naniga (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `libs/musyx/include/musyx/musyx.h` `SND_EMITTER` had its fields in the wrong order: the original reads `flags` at 0xC, `pos` at 0x10, `dir` at 0x1C, `maxVol`/`minVol`/`volPush` (all `f32`) at 0x2C/0x30/0x34, `vid` at 0x38, `group` at 0x3C, `fxid` at 0x40; `paraInfo` is not among those (moved to the end). `sndCheckEmitter`, `sndEmitterVoiceID`, `sndRemoveEmitter`, `s3dKillAllEmitter` and `s3dKillEmitterByFXID` are 100% with that (the five 99.9% functions of the queue had one shared cause). The header is a protected path.
- `ac_power_gauge`: `actionCommandDisp` is 100% with `ratio = dec->gaugePercent / float_100_8042158c; if (ratio > 1.0f) ratio = 1.0f;` (a literal, where the named constant swapped f0/f2); the unit links through the all-literal pool step.
- `eff_naniga` `effNanigaDisp`: `f32 half = 0.5f;` assigned before `centerY`/`centerX` (the local-constant idiom): 100%. The unit has another function at 94.92%, so it does not link.
- Many of the 99.9% near-misses are only relocation offsets against data the unit does not define yet (`effOpukuJetwrainDisp`, `winPartyInit`: `lfd f31, 0x30(r29)` vs `0x80(r29)` is the table's place in `.rodata`), not code differences; `tools/agent_check.py` marks them `arg_mismatch`. `unit_party_sanders` needs the same `.data` boundary move as christine (the pair at 0x803848D8 is the tail of `unit_party_chuchurina`'s `.data`): a splits.txt/symbols.txt change beyond the 32 approved units, left for Isaac.
- Queued for the tower permuter (register-allocation level): `effItemUseDisp`, `effItemGetDisp`, `mot_damage` (spd/floorY in f31/f30 swapped), `BattleChoiceSamplingEnemy`, `evt_cloud_main`, `mobjHitEntry`, `effMistDisp`, `effMahorn2Disp`, `parsePush`, `effSleepDisp`, `effKemuri5N64Entry`.

## Grind 53: eff_peach_dust_n64 links, the first permuter win (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

`effPeachDustMain` (98.86% with every hand variant of the `k = 0.1f` hoist) reached score 0 on the tower permuter (`perm_run`, 227 s, `PERM_*` variant of the literal-pool source): a pointer to `data->scale` taken right after `colorIndex = data->counter` (`scalePtr = &data->scale;`) and the old scale read through it in `data->scale = k * (k - data->scale) + *scalePtr;`. It changes the order of the loads in the original's way, so it is under `MUST_MATCH` with the plain expression in `#else`. The unit then links with `finish_unit` (literal pool).
## Wave 51, batch 1: the mixed DOL files' below-80% functions (2026-10-01, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

- **Scope:** the 106 DOL files that have both below-80% functions and near-misses. Agents get the below-80% functions
  up to 2.5 KB, packed per file into groups of at most 3 KB (156 agents, 87 files, 282 functions); a file whose low
  functions fit one agent also lists its near-misses as extras. The launcher (`scratch/wave51/launch.py`) claims
  each file just before its first agent starts and skips files Claude #1's waves hold (40 skipped so far), and starts
  nothing after 08:00.
- **Collection** in a separate worktree (`~/ttyd-w51`; Claude #1 and #3 each have their own): most functions here were
  untouched, with no placeholder to splice into, so `scratch/wave5x/merge_collect.py` merges each agent copy onto
  the tree in turn (3-way from the wave's base commit; a conflicting hunk takes the agent's side, and the score
  comparison catches what that breaks). Files that stopped building after a conflicted merge (mariost, party,
  pmario_sound, seq_mapchange, unit_party_chuchurina, evt_mobj) and units whose data sections stayed below main after
  the greedy revert (mapdrv, mario, imgdrv, mario_hit, party_nokonoko, eff_kemuri1_n64, sac_muki, sac_suki) are as on
  main. `tools/ci.py`'s ratchet checks the section scores of unlinked units, so a falling section is a CI failure, not
  only a policy. `greedy.py` now also takes out functions new to the file; `nm_refresh.py` rewrites NONMATCHING
  percentages from the merged build (several agents' changes to one file shift each other's scores).
- **Result** (61 agents): 8,492 -> 8,546 functions on main 6d9598d; casedrv links.
- **Review** (`scratch/wave51/review1.md`): `effCalcMayaAnimMatrix` (66.78%) read its s16 keys unscaled where retail
  divides by 64 (`psq_l` through GQR5 = 0x06070607), swapped the interpolation operands and flipped a weight's
  sign; it is unwritten again. `zubastar_bomb` adds the offset to `field_0x30.x` as retail (an older bug).
  `#pragma optimize_for_size off` in battle_icon switched size optimisation off for everything after it (the
  file is built -O4,s); it closes with `reset`.
- **Needs Isaac:** battle_sub.c opens a file-wide `#pragma optimize_for_size on` standing in for `-O4,s` (a
  configure.py flag change); `BattleIcon.flags`/`iconId` and `ItemData.icon_id` are declared signed where retail
  loads them unsigned (`lhz`), worked around with `*(u16*)&` casts in battle_icon.
- **Follow-up:** `partyGetAppearPos2` returns BOOL in retail (declared void; the one caller ignores it); unsigned
  compares written as `(u32)` casts outside MUST_MATCH in casedrv, effdrv, evt_nannpc, battle_menu_disp; ac_shot's
  six vec3 objects stay one array (separate objects cost `actionCommandDisp` its match); gor_03's raw byte offsets
  stay (its work struct isn't declared).

## Wave 51, batch 2, and wave 52 (2026-10-01, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

- **Batch 2** (wave 51's other 47 agents, collected onto main 0a750ff): +3 functions (8,582 -> 8,585) and code
  fuzzy +0.73 from partial functions. Main had moved under these files since the agents' base (2ecf9a4), so eight
  files no longer built after the merge (battle_audience, battle_unit, animdrv, party, seq_title, win_item,
  win_log, evt_mobj) and five lost a function or section (party_hit, imgdrv, battle_seq_command,
  ac_repeatedly_lv, mario_hit, whose new code also used MUST_MATCH-only helpers unconditionally): all as on main.
  **Lesson:** a long wave should be collected in batches from the start; the later the batch, the older its base.
- Review (`scratch/wave51/review2.md`) found no behaviour defects. mapDisp transforms one bounding-box corner
  through the model matrix twice in retail (a game bug); the C had that only under MUST_MATCH and now keeps it in
  every build. partyHelloMain returns s32 (every path sets r3 = 1; callers ignore it), so include/party.h's line
  changes from void.
- Folded in from Claude #1's review of #142: battle_unit's unused PoseSound typedefs out and its two .sdata2
  floats const; effdrv's unused Maya key structs out; battle_icon's no-op optimize_for_size pairs out (the file is
  -O4,s already); battle_sub's file-wide optimize_for_size now closes at the end of the file. Noted:
  include/manager/filemgr.h:39 changed an existing field from u8 to s32 in #142 (the asm reads a word).
  Still open: raw offsets where the header names the field (battle_unit `pouch->partyData[party].currentHP`,
  filemgr `file->references`; both need s16 semantics the u16 headers lack) and btlsubResetMoveColorLvAll's
  aliases / btlMovePos's declaration order, which belong under MUST_MATCH.
- Claude #1's second review of #144: winMsgDisp had nested the name-plate block (+0x140) inside the +0x138
  block where retail has them as siblings (the plate never drew when +0x138 <= 0), and reached named objects
  through `str_802F3FE8 + offset`; battle_audience_kinopio had replaced named vec3s with base+offset; mapdrv's
  new mapSetMaterial read texwrap_tbl/lightid_tbl through vec3_802c16d8 + offset with local structs that overlap
  MapMaterialTex. win_root and battle_audience_kinopio (no new 100%) are back to main and mapSetMaterial (86%) is
  unwritten again. FontDrawMessage adds `x + (iw * scale + offset)` as retail's fmadds/fadds. Pre-existing rule 14
  issue: pmario_sound's psndBGMOff_f_d is still an empty `{}` placeholder (its 0x200-byte body is unwritten).
- **Wave 52** (launched 07:26 on a fresh base, main 0a750ff): every unclaimed game file's below-80% functions up
  to 2.5 KB, grouped as in wave 51, 61 agents capped at 90 minutes, nothing started after 08:45. The launcher
  also brings back `rels/*/include` edits as a patch (wave 50 lost three units' header edits to the tower's
  workspace pruning).

## Grind 54: the newly fully-matched units, a real `.data` gap, REL pool interleaving (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `finish_unit` over the 10 new `autolink --list` units: `ac_lst_rc_keep` (#146) and `eff_explosion_n64` (#147) link as they are; `eff_mahojin` (#148) needed its `.data` gap. Not linked: `battle_sub` (`.rodata` 67%, `.sdata2` 78%), `ac_timing_a` (`.data`/`.sdata` 0%: needs `dol_gen`), `bgdrv` (`.sdata2` 81%), `eff_updown` (`.rodata` 66%), `eff_akari_pose_n64` (`.data`/`.sdata` order and an int-to-double constant that lands in `.sdata`), `eff_magic2_n64` (`.rodata` 87%), `eff_snow_dust_n64` (`.data` 92%, `.rodata` 85%).
- **Gap objects:** the 4-byte zero gaps dtk lists at section ends are only alignment (Grind 48), but a gap of 8 bytes or more is part of the unit: `eff_mahojin`'s `.data` ends in 24 zero bytes (`gap_05_803AE248_data`) and the dol was 32 bytes short (the 24 plus its smaller siblings) until they were defined (`explicit_zero_data` + `force_active`, `u32[6]`). Gaps of 8 or more in the still-unlinked units: `ac_repeatedly` `.sdata` 0x14, `eff_akari_pose_n64` `.sdata` 0x10, `eff_magic2_n64` `.sdata` 0x8. List them with `grep` on `.obj gap_` in the unit's asm.
- `ac_repeatedly`: `dat_8041c7a8` (the `-1` word at the start of `.sdata2`) is declared `extern s32` and never defined; defining it `const` (even `volatile` under `MUST_MATCH`) reschedules the load in `battleAcMain_Repeatedly` (98.75%). `float_30_8041c7d4` is declared and unused, and the code has `-300.0f` and `30.0f` as literals next to the named `float_neg300`, so the pool gets both.
- **REL pools (`gor_02`, `jin_00`, `tik_00`: every function 100%, `.rodata` wrong):** `gor_02` has one 4-byte zero first in the target, that is the compiler's template for `(GXColor){ 0, 0, 0, *ap }` in `alpha_tev`, then the event-script strings, then the function floats; ours puts the template last. Moving the functions in front of the data puts the floats first. So the original source interleaved functions and scripts (floats of a function between the strings of the scripts around it); `jin_00` shows the same (strings, then `200.0f`/`0` floats, then more strings). The fix is a tool that reads the target's `.rodata` order, assigns every item to a function or script, and writes the unit's functions and scripts in that order; not started. `tools/fn_order.py` is dol-only.
- Tower permuter (this stretch): nothing reached zero for a unit that still needed it; best scores `effGarekiDisp` 420, `effMist2Disp` 375, `countDownMain` 350, `effWaterDamageDisp` 465, `effItemUseDisp` 440, none of them a plain source change. wave46 claims most of the register-allocation list.

## Grind 55: `tools/rel_pool_order.py` and `rels/gor/src/gor_02.c` links (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

The REL units that were 100% in code but not linked (Grind 54) have a `.rodata` pool in the wrong order because the generated source is "all scripts, then all functions"; the original interleaved them.

- How MWCC pools (derived from the target's `.rodata`, checked on `gor_02`): strings in data initialisers (scripts, tables) and aggregate templates (a Vec zero, a `(GXColor){...}` compound literal in a function body) are pooled while the source is read, in source order; the strings and float/double constants inside function bodies are pooled when the functions are generated, in text order (= the reverse of the source order), after everything that was read. So the pool starts with the templates and script strings in source order and ends with the function constants.
- Scripts keep their `.data` order and functions their text order, so the free choice is where the functions that own a parsed item (a template) sit between the scripts: `alpha_tev` (its `(GXColor){ 0, 0, 0, *ap }` template is the first 4 bytes of `.rodata`) is above every script; `enzan_cam_turn_keisan_f` (two zero `Vec`s) sits among them. `tools/rel_pool_order.py MOD/UNIT [--write]` finds those anchors from the unit's asm, keeps the other functions where they were relative to their predecessor, and moves the segments (functions with the `static inline` helper above them, EVT scripts, tables). It refuses when the pool contradicts the text/`.data` orders. It does not add forward declarations.
- `gor_02` links with it: `.rodata` bytes and everything else byte-identical, data linked +1.21%. `jin_00` is one 8-byte `GXColor` template short (ours pools three 4-byte `-1` words where the target has two) and `tik_00` fails the consistency check (its `.rodata` has parsed items the model does not explain); both left as they are.

## Grind 56: seven more units, and what a `checksum mismatch` with every section at 100% usually is (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

Linked: `jin_00` (#151, with `tools/rel_pool_order.py`), `romfont` (#153), `mario_motion` (#154), `ac_timing_a` (#155), `battle_sub` (#156), `eff_magic2_n64` (#157); `tools/rel_pool_order.py` also takes nested units and treats an `lfs`-loaded `zero_` object as a float (#152).

- **Function order first:** when `finish_unit` ends with `checksum mismatch` but every section scores 100%, run `tools/fn_order.py UNIT` (`battle_sub`, `mario_motion`): the text order of the unit differs and the `bl` targets of other units move. `fn_order --write` leaves comments and `static inline` helpers where they were: move a helper that a moved function calls back above it by hand.
- **`jin_00`:** `senteresa_setupTev` existed only to imitate `senteresa_loadTev` being inlined into `senteresa_ext_disp`; calling `senteresa_loadTev()` from `senteresa_ext_disp` (the compiler inlines it) shares its `GXColor` template (one `-1` word instead of two); `0.4f * (f32)sin(1.2f * x)` takes a temp (call-argument pool rule).
- **Templates:** each *expansion* of an inline function with a `(Vec){ 0, 0, 0 }` initialiser merges into one template; the target's two zero `Vec`s in `effMagic2Main` (two calls of `initializeVec`) came from a second copy of the helper (`initializeVec2`). The opposite: `eff_updown` defined a named `vec3_` zero and also built `(Vec){ 0, 0, 0 }` in code under `MUST_MATCH`, which emits both: the named one only in the natural build.
- **`math.h` pools two sqrt doubles per use site** that a unit without `sqrt` does not have: `math_ext.h` plus `#define fabs(x) __fabs(x)` / `fabsf` (`mario_motion`, `ac_monosiri_target`, `eff_updown`).
- **A named float/double extern forced first** (`battle_sub`'s `double_0p0001`, `eff_updown`) puts it before the compiler's own int-to-double constant; `f64 eps = 0.0001;` as a local pools it after, `atan2f_safety` stays 100%.
- **Tables reached only through a base** need `force_active`; the three map names read at `vec3_802c59f0 + 0xC...` in `mario_motion` are defined under `MUST_MATCH` (the natural build keeps the literals). `marioMotTbl` was typed from the `dol_gen` output as `MotTblEntry[41]`.
- Not done: `eff_mist2` (with the right function order Disp is 100% with `sixteen`/`k255`/`half` locals and the pool is 16, 0.5, 255 where the target has 16, 255, 0.5: the instruction order in the target is 16, 0.5, 255, so the pool order is not the load order), `eff_updown` (pool equal with locals; `effUpdownMain` 99.70% from the register order of `(f32)t * 0.25f`; the temp for `sin` pools right but costs 5%), `tik_00` (all functions 100%, `.rodata` equal after `math_ext.h`; two function-pointer relocations in the dol point at each other's function), `evt_lect_usu` (ours pools 108 bytes of floats the target `.rodata` does not have), `evt_kinopio` (the `.sdata2` constants become `__declspec(section ".sdata2")` objects and `fn_order` is applied, `.sdata2` equal, the dol still differs in `.data`/`.sdata`), `ac_repeatedly` (see Grind 54).

## Grind 57: more linked units, and the parked ones with their exact blockers (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

Linked: `driver/bgdrv` (#159), `eff_snow_dust_n64` (#160), `eff_akari_pose_n64` (#161); `battle_sub`, `eff_magic2_n64`, `ac_timing_a`, `mario_motion`, `romfont`, `jin_00` are in Grind 56.

- **Shared named constants are not per-function templates:** `bgdrv`'s two functions load the same three `GXColor` words (`dat_80419510/14/18`, defined after the code); compound literals made a second set of templates. **PSMTXScale arguments:** `608.0f / w` and `480.0f / h` in the call itself (arguments are evaluated right to left, the constants pooled left to right).
- **Mutable definitions of pool constants** (`f64 double_to_int_...`, `f32 float_...` without `const`, `eff_akari_pose_n64`, `evt_kinopio`, `battle_item_data`) land in `.sdata`: remove the definition (the compiler's own is in `.rodata`) or give it `__declspec(section ".sdata2")`.
- **`dol_gen --append` for pair objects** (`eff_snow_dust_n64`): the `lbl_XXXX` u8 anchor definition is replaced by the generated `u32[2]` one, the use casts, and the generated objects go in front of the unit's other `.data`; the vertex tables need `force_active` (all reached through the base).
- **Parked, with the exact blocker:**
  - `eff_break`: after `dol_gen` (typed as `f32 ty_data[21]`), the string `str_804210C4` defined between `float_20` and `float_2` (named constants are laid out in definition order, so 2.0 and 10.0 must be named too), every section is 100%; the dol differs by nine `lfs` offsets: the zeros in `GXPosition3f32(.., 0.0f)` are literals (a second `0.0` entry at the end of `.sdata2`) where the target loads `float_0`; with `float_0` named there, `effBreakDisp` drops to 93-96% (the target reloads it before every store).
  - `eff_mizutama`: one `lfs` offset differs (a literal `0.0f` where the target loads the named `float_0`); a local for it gets `effMizutamaEntry` to 99.87% (f1/f2 swapped). On the tower permuter queue.
  - `eff_snowfall_n64`: with `dol_gen`, the `/ 2.0f` of `cam_clip_chk` as `* float_0p5` (named: no second `0.5` in the pool) gives 99.90% (f3/f4 and the operand order of one `fmuls`); on the queue.
  - `countdown`, `battle_item_data`: the float arguments of `intplGetValue` must be loaded before the counter (a comma expression with a volatile cast on a literal gets 95%; locals 97%/86%).
  - `battle_item_data`: `dat_8041c504` (`ffffff00`) should sit before the string `mist` in `.sdata2` (ours pools it after).
  - `hw_aramdma`: the two static `ARAMTransferQueue`s are laid out `Hi, Lo` in `.bss` where the target has `Lo, Hi`; neither the declaration order nor the branch order of `aramQueueCallback` changes it; `musyx.h` includes `math.h` (its sqrt statics add 32 bytes of `.sdata2`): `#define _MATH_H_` before the includes removes it.
  - `evt_lect_usu`: the unit's `.rodata` split ends at 0x2F04 but its functions' constants (`double_to_int_usu_00006728` at 0x2F20, `zero_usu_00006730`, ...) sit after it, in the next unit's range: the split is too short (a `splits.txt` change: needs Isaac); the named `vec3_usu_*`/`dat_usu_*` definitions were wrong (they are the compound-literal templates of the code: removing them gives a `.rodata` equal for the first 716 bytes).

## Grind 58: gaps, sbss order, small-data constants; eleven more links (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

Linked since Grind 57: `eff_gareki_n64` (#162), `ac_repeatedly` (#163), `evt_kinopio` (#164), `eff_kemuri8_n64` (#165), `ac_stick_keep_left` (#166), `eff_bomb_n64` (#167), `ac_power_gauge_lv` (#168, function order only), `evt_cloud` (#169).

- **Trailing gaps (dtk `gap_NN_ADDR_section` objects):** in `.data`, `.sdata`, `.sbss` and `.bss` they must be defined when the dol shifts (the next unit's section alignment is 4, so the object's own 4-byte padding is not absorbed): `evt_kinopio` (4 bytes in each of the three), `eff_mahojin` (24 bytes in `.data`), `ac_repeatedly` (20 in `.sdata`), `eff_akari_pose_n64` (16 in `.sdata`). Zero-initialised `.data`/`.sdata` objects: `#pragma explicit_zero_data on` + `force_active on`; `.sbss`: a plain uninitialised object with `force_active`. The `.sdata2`/`.rodata` gaps cannot be defined: named objects go before the pooled literals (`eff_akari_charge_n64`'s dol broke with one). A gap after a small object keeps the object's alignment: `driver/fadedrv`'s 31 bytes after a `u8` are three objects (`u8`, `u16`, `u32[7]`), not `u8[31]` (aligned 4).
- **`.sbss` statics are laid out in the order of their first mention in the source**, declarations included: `evt_cloud` needed `cloud_once_flag` declared before `cloud_p` (the dol had `cloud_p`'s `r13` offset 4 off). For `musyx/runtime/hw_aramdma` the two queues go `Hi, Lo` where the target has `Lo, Hi`; declaring them in the other order does not change it (the first use in `aramQueueCallback` does: `Lo` assigned first gives the target order and `aramQueueCallback` 93.12%).
- **Small-data constants as objects:** a writable `__declspec(section ".sdata2")` object defined at the *top* of the file lands in front of the pooled literals (`eff_gareki_n64`'s two colour words, `ac_repeatedly`'s `dat_8041c7a8`); defined after the code it lands behind them. A `const` definition of such a word reschedules the load (`effGarekiDisp` 98.22%, `battleAcMain_Repeatedly` 98.75%): keep it writable.
- **Literals vs named constants in a pool:** a literal that the target loads by name makes a second entry at the end of `.sdata2` (`ac_repeatedly`'s `-300.0f`/`30.0f` in the tail-state interpolation, `eff_snowfall_n64`'s `/ 2.0f` as `* float_0p5`); the fix is the named constant at the use, not another definition.
- **Duplicate pool objects from `.sdata` definitions:** the two writable `f64 double_to_int_...` of `evt_cloud` (kept "to load by name") were dead duplicates in `.sdata`.
- Still parked: `eff_kemuri11_n64` (all sections 100% after `fn_order`, `.rodata` has the string before the zero `Vec` template where the target has them after: a second `kemuri11MakeVec`/`initializeVec` template pair), `driver/fadedrv` (with the gaps `.data`/`.sdata`/`.sbss`/`.sbss2` equal; `.sdata2` has `3.141592f` pooled after `0.5f`/`180.0f` where the target has it before them; and `lbl_802C1538`, a 4-byte zero at the end of its `.rodata` that `hitdrv` references, is undefined), `eff_nozle` (`dol_gen` writes its 95 objects but the typed `u8*` tables of the source conflict), `battle_event_default`, `eff_mist2`, `eff_updown`, `eff_break`, `eff_bomb` (see Grind 56/57).

## Grind 59: needs Isaac, and what is left (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

Linked since Grind 58: `eff_nozle` (#171, 95 data objects), `battle_event_default` (#173), `eff_kemuri11_n64` (#175; the unit's only zero `Vec` template is `effKemuri11Main`'s compound literal: a named `vec3_` plus an unused inline helper had made a second one; a named string is laid out before the compiler's pool, so the string is a literal), `tools/fn_order.py` for REL units (#176).

- **Needs Isaac (outside the approved scope):** PR #131 (`unit_party_christine`; the nokotarou/christine `.data` boundary and `symbols.txt`), `unit_party_sanders` (the same pair at 0x803848D8 is the tail of `unit_party_chuchurina`'s `.data`: a boundary move of a not-yet-linked pair of units), `evt_lect_usu` (the unit's `.rodata` split ends at 0x2F04, its functions' constants `double_to_int_usu_00006728` (0x2F20), `zero_usu_00006730`, ... sit in the next unit's range: the end should move to cover them; with the named `vec3_usu_*`/`dat_usu_*` definitions removed its first 716 `.rodata` bytes are already equal).
- **`gor/unit/unit_lecture_frankli_sac`** (REL): functions in text order (the reverse of what the generated source had, `fn_order.py` for RELs; the `opt_*` pragma block must stay around `_battle_set_lecture_unit_id` and `lecture_init`), `defence`/`defence_attr` defined after `unit_lecture_frankli_sac` (the target lays them out behind it), `init_event` before `phase_event`, the initialised `f32 point[4]` removed (it is `.bss`), the 4-byte `.data` gap: every section is equal, all 15 functions 100%; the link still differs in 30 `.data` relocations that point at text offsets 0x150-0x164 of the module (ours 0x10 off): not found.
- Still parked (see Grind 54-58): `driver/fadedrv` (`lbl_802C1538`, `.sdata2` order), `eff_updown`, `eff_break`, `eff_bomb`, `eff_mist2`, `eff_mizutama`, `countdown`, `eff_snowfall_n64` (`cam_clip_chk` 99.90%; 64 operand-order variants of its two sums do not reach 100%), `battle_item_data`, `hw_aramdma`, `tik_00`, `unit_boss_black_peach`, `eff_cool_leaf_n64`, `eff_magic3_n64`, `eff_pturn_n64`, `mot_dokan`.
## Wave 52, batch 1 (2026-10-01, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

- 41 of wave 52's 61 agents (see "Wave 51, batch 2, and wave 52"), collected at 09:15 onto main b0396e7 by
  sequential merges: 8,627 -> 8,659 functions. unit_boss_gundan_zako_group2 gets 8 (its file had been left out of
  wave 50 for lost header edits; the launcher now brings `rels/*/include` back as a patch). bom/bom_01's header
  signature changes broke bom.c, so both stay as on main; the greedy revert cleared seven section drops for three
  functions.
- Review (`scratch/wave52/review1.md`): an agent rewrote effCalcMayaAnimMatrix with the same defects #142 removed
  (unscaled s16 keys, a weight's sign), so it is unwritten again; **an agent prompt should list functions a review
  took out, or the next wave writes them back**. `_chuchu_item_steal` tested and cleared unit flag 0x4000 where
  retail uses 0x40000000 (`lis r4, 0x4000`). sac_genki multiplies by 60 twice as retail (one fold to 3600 differs
  by an ulp). evt_cam, hitdrv, eff_opuku_wcurtain, msgdrv, battle_ac and jin_07 had nothing new at 100% and
  carried unused helpers, externs or wrong notes: as on main.
- **Follow-up (form, behaviour equal):** match-only shapes outside MUST_MATCH in gcDvdCheckThread (`s32 width`,
  `half = 0.5f`), unit_boss_gundan_zako_group2's shake_ceil (`volatile` reads) and its ceil functions' raw-offset
  macros on `work_20_bss_6C`, chuchurina's redundant `else if (list != NULL)`.

## Grind 60: unit_party_christine links, with the nokotarou/christine `.data` boundary moved 8 bytes (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- The `{ -1.0f, 1.0f }` pair at 0x80390B70 was the tail of `unk_evt_80390b30` / `lbl_80390B74` in `unit_party_nokotarou`; `tools/split_scan.py` missed it because the pair is not a `lbl_` object. The `.data` boundary of nokotarou/christine is now 0x80390B70 (the Grind 41 edit for one more pair), `symbols.txt` has `unk_evt_80390b30` at size 0x40 and `lbl_80390B70` (8 bytes) in its place, nokotarou's table lost its last word and the `lbl_80390B74` definition, and christine's source defines the pair as its first `.data` object. The invented `lbl_80390B70` base and the `MUST_MATCH` weapon-offset block in `__makeTechMenuFunc` are gone; the four weapons are named in the unit's own `.data` as in clauda.
- The rest is `tools/dol_gen.py --append`, `tools/fn_order.py --write` (the text order was off), and `finish_unit`.
- **Needs Isaac:** the `splits.txt`/`symbols.txt` edit is outside the 32 units he approved in Grind 41; `ci.py` needs `--allow-protected` for it. With the flag everything passes (dol and RELs byte-identical).

## Grind 61: unit_party_sanders: the chuchurina/sanders `.data` boundary moves 8 bytes; data written (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Approved by Isaac (relayed by the orchestrator, 12:42). The `{ -1.0f, 1.0f }` pair at 0x803848D8 was the tail of chuchurina's `attack_audience_event`; the `.data` boundary is now 0x803848D8, `attack_audience_event` is 0x330 bytes, `lbl_803848D8` (8 bytes) replaces `lbl_803848DC`. Same shape as christine (Grind 60).
- `unit_party_sanders.c` has its whole `.data` (`dol_gen --append`); the four weapons are named in the unit's own `.data`, so the `MUST_MATCH` base-offset block of `__makeTechMenuFunc` is gone and it is 100% in the natural form.
- Not linked: `_get_bomb_hit_position` is still 92.86% (callee-saved map shifted: outX/outY take r31/r30 in the target, and the `double_to_int` constant is loaded by name where we pool it).

## Grind 62: evt_lect_usu links; the usu `.rodata` split end moves to 0x2F40 (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Approved by Isaac (relayed by the orchestrator, 12:42). `config/G8MJ01/usu/splits.txt`: `event/evt_lect_usu.c` `.rodata` ends at 0x2F40 instead of 0x2F04, so its own constants (`double_to_int_usu_00006728` ... `float_1_usu_00006744`) are no longer two `auto_03_*_rodata` units. `ci.py` needs `--allow-regress usu/auto_03_00002F04_rodata --allow-regress usu/auto_03_00002F20_rodata` because those two units are gone.
- Source: the named `vec3_usu_*`/`dat_usu_*` definitions are removed; the two remaining uses are compound literals (`(Vec){0.0f, 0.0f, 0.0f}`, `(GXColor){0xFF, 0xFF, 0xFF, 0xFF}`). They are the templates MWCC makes for the code's own compound literals; defining them by name made a second copy (48 bytes). `.rodata` and `.data` are then equal and the unit links (38/38 functions at 100%).

## Grind 63: ~/ttyd-refs, the shared read-only reference decomps (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Approved by Isaac (relayed by the orchestrator, 12:42): library ports from the MIT/CC0 reference decomps, with each repo downloaded exactly once into a shared read-only place.
- `~/ttyd-refs/<project>` on the Mac, rsynced to `~/ttyd-refs` on the tower (pop-os): ac-decomp, spm-decomp, tp, tww (all CC0-1.0) and musyx (MIT), shallow clones; `~/ttyd-refs/README.md` lists each repo's URL, commit and license (checked against its LICENSE file and the GitHub API). Anything else is deleted, never added. Wave agents have no network: prompts point at the local copy. `docs/AGENT_BRIEF.md` rule 16 says so.
- Every port PR names the project and file it took code from.


## Grind 64: library ports begin: MSL k_tan, s_tan, e_sqrt from tp (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Ports use the read-only copies in `~/ttyd-refs` (Grind 63). `__kernel_tan`, `tan` and `__ieee754_sqrt` are zeldaret/tp's `libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Double_precision/{k_tan,s_tan,e_sqrt}.c` (CC0) with the include changed to this repo's `MSL_C/MSL_Common_Embedded/Math/fdlibm.h`; all three are 100% and linked on the first build, no other change.
- The library units sit in `libs/metrowerks/src/MSL_C.PPCEABI.bare.H.a/`; `ci.py` needs `--allow-protected --allow-configure`.

## Grind 65: Runtime `__cvt_sll_flt` and `__sys_free` from tp (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `__cvt_sll_flt` (runtime.c) is the asm body of zeldaret/tp `libs/PowerPC_EABI_Support/Runtime/Src/runtime.c` (CC0) with its `lbl_` labels renamed; placed before `__cvt_ull_flt` (the target's text order). `runtime.c` is Matching, 15/15 functions. `ci.py` needs `--allow-asm` for it (a ported library's hand-written assembly).
- `__sys_free` (new `Runtime.PPCEABI.H.a/GCN_mem_alloc.c`) is tp's `GCN_mem_alloc.c`, `<os.h>` -> `<dolphin/os.h>`, Matching. The target's two strings are readonly and each padded to 4 bytes: the unit needs `extra_cflags=["-str readonly"]` (without it they land in `.data`; with `pool` they are not padded).

## Grind 66: TRK MWTrace and MWCriticalSection_gc from tp; why TRK msghndlr is not a port (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `MWTRACE` is zeldaret/tp's `libs/TRK_MINNOW_DOLPHIN/gamedev/cust_connection/utils/common/MWTrace.c` (CC0) and `MWCriticalSection_gc.cpp` its `utils/gc/MWCriticalSection_gc.c` (CC0, wrapped in `extern "C"` because the unit is C++ here); both 100% and linked.
- `msghndlr` is not in the reference projects' form: TTYD ships the release TRK, where `TRKDoVersions`/`TRKDoSupportMask` are `li r3, 0; blr` and `TRKDoReset`/`TRKDoOverride`/`TRKDoConnect`/... send one fixed 0x40-byte packet through `TRKWriteUARTN`. The existing `msghndlr.c` (ac-decomp's debug TRK) scores 0% on those 15 functions; they need rewriting from the asm, not porting. `targsupp.s` is the same release build (8-byte stubs).

## Grind 67: MSL FILE_POS, ansi_fp, mbstring from tp (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `FILE_POS.C` (`fseek`, `_fseek`, `ftell`) is zeldaret/tp `libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Src/FILE_POS.c` (CC0): TTYD's version has the text-mode newline adjustment in `_ftell` and the critical region in `ftell`, which the ac-decomp-derived file lacked. New header `libs/metrowerks/include/MSL_C/critical_regions.h` (the enum and prototypes from tp's `critical_regions.h`). 3/3 at 100%, Matching, linked.
- `ansi_fp.c`: `__equals_dec` and `__minus_dec` are tp's `MSL_Common_Embedded/Src/ansi_fp.c` (GCN branch of `#if PLATFORM_GCN`), 100% on the first build; `__two_exp` takes `long exp` and `__num2dec_internal` has `long num_bits_extract` as in tp (the `short` made an `extsh`); 5 of 7 functions 100%. Still not matching: `__two_exp` 98.38% (the target tests the cases with jump-table range checks `bgt`/`blt`, ours compares in a chain with the same cases) and `__dec2num` (4224 bytes, absent from the sources; tp only has the Wii version).
- `mbstring.c`: `wcstombs` is now 100% with tp's `unicode_to_UTF8(char*, wchar_t)` (the original's `wchar_t` argument) and an inline `wctomb`; `mbtowc` stays at 98.47% (the target tests `if (!s)` as `bne over; b end`; `if (!s) return 0`, `else { result = 0; }` and `else { }` all score the same or worse).

## Grind 68: MusyX s_data and snd3d from AxioDL/musyx (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `libs/musyx/src/musyx/runtime/s_data.c` and `snd3d.c` are AxioDL/musyx `src/musyx/runtime/{s_data,snd3d}.c` (MIT, commit dc56c921) unchanged: with the 2.0.3 version gates the reference compiles to the target. `s_data` 8/8 and `snd3d` 12/12 functions at 100%, both Matching and linked (they were 1/8 and 5/12 with the older in-tree copies).
- Header changes they need, all version-gated so nothing else moves: `synthdata.h` gets `GSTACK_INST` and `dataARAMGetInfo`/`dataARAMDefaultGetInfo` from 2.0.1; `musyx.h` gets, from the reference, `SND_LISTENER`/`SND_EMITTER` as 2.0.1+ lays them out (no `room`, `paraInfo` first, `oneMeter`, `userData`), `SND_S3D_OCCLUSION_CALLBACK` and `sndSet3DEmitterOcclusionCallback`, `SND_LISTENER_EXPARAMETER` (the `room` parameter of `sndAddListener`/`sndAddListenerEx` becomes `exPara` from 2.0.1), `SND_3DINFO::lpfFactor`, and `sndGet3DParameters` without `room`. `sndUpdateListener`/`sndUpdateEmitter` keep their `room` parameter, as in the reference; `src/sound.c` still passes `NULL`.
- Not taken from the reference: `sndVirtualSampleEndPlayback` has a third argument from 2.0.2 in the reference, while the in-tree `synth_vsamples.c` is the older two-argument file; a whole-file swap of `synth_vsamples.c` needs the reference's newer `synth.h`/`voice.h`/`stream.h` (the headers differ widely): a separate step.
- Same chapter, second commit: `synth_vsamples.c` `vsSampleStartNotify` is the reference's 2.0.3 form (`hwVoice`, `u8 voice = voiceID & 0xFF`), 5/5 at 100%, Matching and linked. `hw_dspctrl.c` takes the reference's >= 2.0.1 sample-end arithmetic (`length - 1`, the `PCM16_VIRTUAL` case 6 in the zero-address switch, `loop + loopLength + base - 1`, a separate `else if` for the virtual PCM16 loop buffer): 14/14 functions at 100% (`salBuildCommandList` was 99.48%), but the unit stays NonMatching: its source does not define the unit's own statics (`dspCompressorOn`, `dspHRTFOn`, ... are undefined at link) and its `.sdata2` (32 bytes of pooled floats), `.sbss` (60 vs 64) and `.rodata`/`.data` gaps differ: the data layout work of Grind 41-59.

## Grind 69: MSL e_acos, e_asin, e_fmod, e_log, e_pow from tp (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- All five are zeldaret/tp `libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Double_precision/e_{acos,asin,fmod,log,pow}.c` (CC0) with the include changed to `MSL_C/MSL_Common_Embedded/Math/fdlibm.h`; each is 100% and linked. `e_acos` replaces the in-tree copy (38.5%).
- `e_pow` calls `ldexp` where tp calls `scalbn` (the target has no `scalbn`; the dol links only with the change). `libs/metrowerks/include/libc/errno.h` gets `EDOM 33` (tp's value; `ERANGE` stays 32 as it was).
- `e_hypot` is in none of the reference projects' MSL trees (tp, tww, ac-decomp, spm-decomp): not ported.

## Grind 70: MSL `__strtoull` from tp; `__strtold` started (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `__strtoull` is zeldaret/tp `MSL_Common/Src/strtoul.c`'s function (CC0) with this tree's `_isspace`/`_isdigit`/`_isalpha`/`_toupper` (the originals are the `_`-prefixed inline functions of `MSL_C/ctype.h`: with the plain names it calls `isspace` etc.) and `value = *chars_scanned = 0;` in the failure branch (the separate `value = 0;` scored one `srawi` off). `strtoul.c` is 2/2 at 100%, Matching, linked.
- `strtold.c` is new: tp's `__strtold` (`MSL_Common/Src/strtold.c`, CC0) with this tree's includes, `NAN`/`HUGE_VAL` for `nan()` (the target calls no `nan`; it loads `__float_nan`), and `LDBL_*` plus `HUGE_VAL` added to `float.h`/`fdlibm.h` (MWCC 2.6 cannot read tp's hex-float `LDBL_MAX`/`LDBL_MIN`, so they are the decimal values). 75.27% (was 0%) and still NonMatching: the target's frame is 0xb0 bytes (ours 0xc0) and it copies a 0x2C-byte `zero_803025a0` const struct for an initialiser that ours puts in a `@22` literal. Not started: `qsort` (in none of the reference projects), `e_hypot` (same).

## Grind 71: TRK dolphin_trk links: tp's `dcbi` asm form (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `TRK__read_aram`/`TRK__write_aram` are zeldaret/tp `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c` (CC0): GC/2.6 has no `__dcbi` intrinsic, tp writes it as `asm { dcbi a, b }` (and `dcbf` likewise) with `register` parameters. Two details that mattered: `OSRoundUp32B` is the inline `(size + 31) & ~31` (the call is not in the target), and the interrupt status is kept in a `u32` and tested as `!(u16)r` (a `u16 r` adds a `clrlwi`).
- The unit's text order is the reverse of the source order, as everywhere: `EnableMetroTRKInterrupts`, `TRKTargetTranslate`, `static TRK_copy_vector` (inlined into `__TRK_copy_vectors`; non-static it was emitted as an extra 176-byte function), `__TRK_copy_vectors`, `TRKInitializeTarget`, `read_aram`, `write_aram`. The 4 bytes after `TRK_ISR_OFFSETS` in `.data` are a `u32` gap with `explicit_zero_data` + `force_active`. 9/9 functions at 100%, Matching, linked. `ci.py` needs `--allow-asm` (the new `asm {}` statements).

## Grind 72: TRK msghndlr is a port after all: tp's is the release TRK (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Correction to Grind 66: zeldaret/tp's `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/msghndlr.c` (CC0) is exactly TTYD's release TRK (`TRKStandardACK` writes the fixed 0x40-byte reply with `TRKWriteUARTN`, `TRKDoVersions`/`TRKDoSupportMask` are `return 0`); only the in-tree copy (ac-decomp's debug TRK) was the wrong source. The tp file with the includes changed compiles to 17/17 functions at 100% (it was 2/17), Matching and linked.
- `TRKDoFlushCache` (an unused function in tp) stays: the target has its string `"DoFlushCache unimplemented!!!\n"` in the `.rodata` pool, and the linker strips the function itself, so every later string offset is only right with it present.

## Grind 73: MusyX hw_dspctrl links; ansi_fp `__dec2num` started (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `hw_dspctrl.c`: with the reference's >= 2.0.1 arithmetic (Grind 68) all 14 functions are 100%; the unit links with `u32 dspCompressorOn = 0;` (it was missing and the link failed with an undefined symbol), `compressorTable` `__attribute__((aligned(32)))` (the target's `.data` is 32-aligned), and `#define _MATH_H_` before the includes (musyx.h pulls in `<math.h>`, whose inline `sqrt` pools two doubles in `.sdata2`; the original has none: the library units are not built with `MUST_MATCH`, so this is unconditional with a comment). The unit's own tail gaps (4 bytes in `.rodata` and `.data`) are only section-end alignment: the dol is byte-identical.
- `ansi_fp.c` `__dec2num`: TTYD's is the older GC form of tp's `__dec2num` (zeldaret/tp `MSL_Common_Embedded/Src/ansi_fp.c`, the `!PLATFORM_GCN` body) with a NaN payload parser (`sig.length == 1` sets the quiet bit, otherwise hex digits of the payload go into the mantissa bytes, `_isdigit`/`_tolower` from `MSL_C/ctype.h`) and `max = "179769313486231580793729011405303420"` (tp has `...728714053034151`), plus `static __less_dec` from the same file; 51.7% (was 5.9%). The target's frame is 0x70 bytes bigger (0x1f0 against 0x180): it keeps more `decimal` locals alive than tp's version; not found. Still NonMatching. (`include "ctype.h"` in this file resolved to the libc header without the inline `_is*` functions; it is `MSL_C/ctype.h` now.)

## Grind 74: MusyX hw_aramdma links (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- The `.bss` order blocker of Grind 54-59 (`aramQueueHi` before `aramQueueLo` where the target has `Lo, Hi`) was `static inline void aramQueueInit()`: AxioDL/musyx's `hw_aramdma.c` (MIT) has it plain `static`. The compiler then emits the function once (a stray copy the linker strips, the dol is byte-identical) and lays the statics out in the order that function mentions them (`Lo`, then `Hi`). Declaration order and branch order were irrelevant. 14/14 at 100%, Matching, linked.

## Grind 75: memory links with spm-decomp's help; filemgr `fileFree` (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Super Paper Mario's decomp (CC0, `~/ttyd-refs/spm-decomp`) shares TTYD's engine, so functions of the same name are good starting points: only six non-100% TTYD functions have an SPM namesake (`fileAsync`, `_fileAlloc`, `fileGarbageDataAdrClear`, `fileFree`-via-struct, `pouchRemoveItem`, `smartInit`, `smartGarbage`).
- `memory.c`: `smartGarbage` is SPM's `smartGarbage` (`src/memory.c`) with TTYD's field names plus the extra `OSGetCurrentThread() != &dvdmgr_thread` test of the target; the register order needs `src, space, cur, prev` declared in that order. `smartInit` and `L_smartReInit`: the free-list loop must be `for (i = 0x800; i != 0; i--)` (the `i < 0x800` form unrolls 16 times, the target 8), and `L_smartReInit` recomputes `heapEnd[5] - heapStart[5] - 32` instead of keeping a `size` local. 14/14 at 100%. The unit links with a 4-byte `.sdata` gap after `wp` and the six uninitialised globals declared in reverse (the compiler lays them out last-declared first). `dvdmgr_thread` is declared in `manager/dvdmgr.h` now (was a local extern in `filemgr.c`).
- `filemgr.h`: `FileEntry::references` is `s16` (SPM's `touchCnt`), which makes `fileFree` 100%. `_fileAlloc` and `fileAsync` remain at 92% (register numbering, on the tower permuter queue with the `mario_pouch` near misses).

## Grind 76: two mario_pouch near misses fixed by the tower permuter (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Queue 10 (filemgr, mario_pouch, seq_title near misses; 20 minutes each, 2 jobs x 4 threads, load gate 20) produced two zero-score wins and nothing else: `pouchRemoveKeepItem` (the natural fix was the parameter type: `int index`, not `s32`; header updated) and `pouchReceiveMail` (the loop counter and the mail count live in one `int[2]` on the stack; kept under `MUST_MATCH` with the natural form in the `#else`). Best scores without a win: `_fileAlloc` 2030 (from 2430), `fileAsync` 2255 (2615), `comp_aiueo` 485, `comp_aiueo_r` 515, `pouchRevisePartyParam` 305, `pouchRemoveItem` 680, `pouchRemoveItemIndex` 840; `L_titleInit` stayed at 15 with no improvement (the `.rodata` base-register issue is not something the permuter reaches).

## Grind 78: unblocker pass 1: the parameter order of `intplGetValue` (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- From Isaac via the orchestrator: for this run the role is unblocker (shared causes across near-misses before one-off matches). All 359 functions at 95-99.9% in `report.json` were diffed with `tools/agent_check.py` and grouped by what is left. Ignoring the `@NNN` symbol noise of unlinked units, the biggest shared pattern was `lwz`/`lfs` loads of one call in the wrong order around `intplGetValue` (39 diffs, 12 of them with the load pair swapped).
- Cause: MWCC evaluates the arguments in the order of the *declared parameters*, and the registers (`r3`, `r4`, `r5`, `f1`, `f2`) do not depend on that order, so the assembly only shows it as the order of the loads before the call. The prototype `(mode, currStep, steps, start, end)` loads the counters first; the original loads the two floats first: `(mode, start, end, currStep, steps)`. One header change (`include/system.h`: new declaration and a macro that keeps every caller in the old order; `src/system.c` `#undef`s it for the definition, whose own code is identical) fixed 18 near-misses at once (+21 functions at 100% in total): `BattleStageMain`, `_lvup_spot_move`, `_lvup_object_select_move`, `star_disp2D_sub`, `DrawSubMenuCommonProcess`, `winMgrSeq`, `main_base`/`main_star` (sac_suki, sac_zubastar, sac_bakugame, sac_muki) and the `barriern_zanzou_sub`/`barriern_satellite_zanzou_sub` pairs in aji and moo. No caller was edited, so no claimed unit changed. One unmatched function got 0.66 points worse (`mri/unit/unit_barriern:barriern_satellite_zanzou_sub`, 91.86 to 91.20).
- General rule (now in `docs/AGENT_BRIEF.md`, "Code shapes that fixed near-misses"): for a callee with integer and float parameters, the declared parameter order is the load order.
- Second and third causes in the same pass, both wrong header types that the source papered over with casts: `BattlePadCheckNow`/`Up`/`Repeat` return `s32` (the callers' compares are `cmpwi`; the header said `u32`, and the sources carry `(s32)` casts that are no longer needed), and `BattleActionCommandGetDifficulty` returns `s32` over a `u8` field `mAcDifficulty` (the header said `s8`: every caller got an `extsb` the original does not have; `ac_lst_rc_keep.c` and `ac_stick_keep_left.c` already worked around it with a function-pointer cast and a `(u8)` cast). The definitions follow (`battle_pad.c`, `battle_ac.c`); `BattleActionCommandGetDifficulty` is still 100% (`lbz r3; blr`). Effects: `BattleCommandDisplay_*MenuMain` 96.1-99.3 (was 95.4-98.4), `BattleBreakSlot_Main` 99.11, `_lvup_spot_off` 98.45. One wave-48 function regresses until its source drops a temp: `btlevtcmd_AcGetDifficulty` in `battle_event_cmd.c` should pass `BattleActionCommandGetDifficulty(...)` straight to `evtSetValue` (the target has no `mr r5, r3` temp); the ratchet needs `--allow-regress battle/battle_event_cmd:btlevtcmd_AcGetDifficulty` for this PR.

## Grind 77: eff_mizutama links (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Three things, none of which needed the others: the pool constants are defined after the code (`pool_define`), `str_802FE6E4` is defined after the code too (a string defined at the top lands before the zero `Vec` template of `effMizutamaMain` in `.rodata`; the target has the template first), and `effMizutamaEntry` reads the volatile `float_0` once with `zero = (data->y = float_0); data->z = zero;` (the tower permuter's variant: a plain local `zero = float_0;` swaps the `f1`/`f2` of the converted `double` and the zero; the chained assignment does not). The original's one `lfs` that differed was this load. 3/3 at 100%, `.data`/`.rodata`/`.sdata2` equal, linked.

## Grind 79: unblocker pass 2: scoring noise, negative results, the tower queue (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `tools/agent_check.py FUNC` now marks `~~` the instructions that differ only in the data symbol (named `float_0_804210b0@sda21` in the original, `@322@sda21` or `...rodata.0@l` in our unlinked object) and keeps `!!` for real differences; the last line counts both (`-- N instruction diff(s) to fix, M that differ only in the data symbol`). Of the 876 partial functions only a handful had symbol-only differences and nothing else, so the noise does not hide matched functions, it only makes real near-misses look worse and longer than they are.
- Negative results, so nobody repeats them: changing the return type of the call-site-cast suspects (`BattleAudience_GetSysCtrl/GetExist/GetWaiting/GetEscapeChangeOK` BOOL to `u8`, `BtlUnit_GetWeaponCost` to `u8`, `marioStGetSystemLevel`/`hitGetAttr` to `u32`, `effTblRandN64`/`BattleACPadCheckRecordTrigger` to `s32`, `pouchCheckItem` to `s32`) lowers or does not move the scores (6 functions at 100% lost); only `BattlePadCheck*` and `BattleActionCommandGetDifficulty` were real (Grind 78). `anotherPartyGetPtr(id != 0)` (party_move, 10 near-misses): the target recomputes the boolean in `r3` for each call where ours keeps it in a callee-saved register; `id != 0 ? 1 : 0` (worse), `!!id`, `(BOOL)` and an inline helper holding the duplicated block (85%) do not give it.
- Tower: `perm_loop` is running `queue13` (the 34 free dol near-misses at 96-99.3%, smallest first, 20 minutes each) through load stops; wins land in `~/permuter/runs.log` with score 0. Builds on the Mac are capped at `nice -n 10 ninja -j4`.

## Grind 80: eff_updown links; the all-literal pool (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `eff_updown` was code-complete for a long time (4/4 at 100%); what blocked it was the pool approach. The working shape is the plain one: no `extern`/defined named floats at all, the constants written as literals in the code (`pool_define` and a top-of-file `float_0` both made the compiler fold or duplicate them), `math_ext.h` instead of `math.h` (its sqrt doubles pool two entries), the zero `Vec` as the compound literal `(Vec){ 0.0f, 0.0f, 0.0f }` in both builds (a named `vec3_` object next to it adds a second copy), the string as a literal in the code (a named `const char[]` lands before the compiler's `double` constants; the literal lands after them, as in the target), one `sin()` result in a temp (pools `0.25` before `0.1`, same code) and the constant in a local before the converted int (`f32 k = 0.25f;`, fixes `effUpdownMain` 99.70% -> 100%). The 4-byte `.sdata` tail is a `u32` gap. Linked.
- Tried the same all-literal recipe on `eff_break`: `effBreakMain` falls to 95.5% and `effBreakDisp` to 89% (there the target does reload the named `float_0` per use); not applicable. `eff_break` stays at the data-typed version of branch `wip-eff-break`.
- `eff_expbom_n64` also links as it is (`autolink` found it): nobody had run it since the last data fix.

## Grind 81: eff_cool_leaf_n64 and eff_snowfall_n64 link; eff_kumokumo_n64 data written (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- All three were code-complete with no data. Recipe that worked (see Grind 80): `tools/dol_gen.py UNIT --append`, then reconcile the typed `extern`s the source already had with its definitions (a table the code indexes as `u8` is defined as bytes, a table of display lists as `void*[]`; the generated forward declaration of a name that already has an `extern` goes), `math_ext.h` for `math.h`, the string as a literal in the code, the zero `Vec` as a compound literal, and no named `double_to_int_*` objects (the compiler makes its own).
- `eff_cool_leaf_n64` linked after that. `eff_snowfall_n64` also needed the `.sbss2` GXColor through `#pragma section ".sdata" ".sbss2"` plus a `u32` gap for dtk's second `.sbss2` slot, one `float_0` read by name (a literal `0.0f` made a second entry) and the named string defined after the code. Linked.
- `eff_kumokumo_n64`: functions 3/3 at 100%, `.data`/`.rodata` order right, `.sbss2` right; left: `.sdata` is 28 bytes short (the `rz_tbl` moves behind the batten tables; gaps between them are missing) and the single `0.5` of `.sdata2` sits after `2.0` in the target but a named `float_0p5` read into the `half` local costs `effKumokumoDisp` 1.1% (an extra live FP register); the literal local pools it last.

## Grind 82: eff_broken_barrier_n64 links; eff_mist2, eff_magic3_n64 and eff_pturn_n64 retried (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `eff_broken_barrier_n64`: all-literal pool (no named floats; the `f32 k = 0.5f;` MUST_MATCH local is then just a literal) plus the `.sbss2` pair through `#pragma section ".sdata" ".sbss2"`, declared in reverse (`unk_804230bc` first, then `unk_804230b8`: the compiler lays these out last-declared first, like `.bss`). Linked.
- Not linked, with what is known: `eff_mist2` (named floats + `math_ext.h` + compound `(Vec)` + `.sbss2` gap + string last give every section right except two extra pool entries (`0.0`, `1.0`) from the literals in `effMist2Disp`; naming them instead drops `effMist2Disp` to 91%; all-literal gives the wrong pool order and Disp 96.9%), `eff_magic3_n64` (naming the `100.0f` argument puts the pool in the target's order but `effMagic3N64Entry` goes 100 -> 99.90%: branch `wip-magic3`, queued for the permuter), `eff_pturn_n64` (all-literal pools right except `360`/`4.0`; pulling the `cos` into a temp fixes the order but moves a callee-saved register in `effPturnMain`, 91-94%).

## Grind 83: unblocker pass 3: nested call versus local, and a permuter win (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- `battle_damage.c` `BattleSetStatusDamageFromWeapon` is 100% with `weapon->special & 0xFFFFFFFFFFFFFFFFu` as the argument (tower permuter; the 64-bit mask on the `u32` flags swaps the two call-setup loads; no cast gets it), under `MUST_MATCH`. The permuter also found `end_muki` in `sac_muki.c` (a claimed unit; given to the orchestrator).
- Rule for the brief: whether a call result sits nested in an argument list or in a local first decides whether it goes straight to its argument register or through `r0` (`mr r0, r3; mr r3, rX; mr r4, r0`). 23 functions in the near-miss diffs show exactly that `mr r0, r3` pair against its absence: `ac_power_gauge_lv2:actionCommandDisp`, `battle_audience:BattleAudienceItemCtrlProcess/SettingAudience`, `battle_event_cmd:btlevtcmd_AcGetDifficulty/AnimeChangePoseDirect/FaceDirectionAdd/GetTakeoffPosition/MoveDirectionAdd/SetPartsBaseScale/snd_se`, `imgdrv:imgCapture_Prim`, `mapdrv:_mapLoad/mapSearchDmdJointSub2/spline_maketable`, `npcdrv:npcDisp/npcDisp_offscreen/npcDisp_xlu`, `winmgr:winMgrSelectDelete/winMgrSelectEntry`, `mot_hammer:mot_hammer`, `unit_barriern:barriern_satellite_zanzou/zanzou2`, `sound:_sscallback`. `btlevtcmd_SetPartsBaseScale` goes 97.98% to 100% with a block-local temp for the nested `evtGetValue`.
- A header-wide scan (annotated struct fields against signedness mismatches in the diffs) was too noisy to name fields: all field names of the same spelling (`flags`, `x`) vote together. The near-miss pool in unclaimed units is exhausted (0 functions at 80-96% below 900 bytes); the permuter queue now holds the 154 claimed-unit functions at 96-99.9% (wins are relayed, not applied).

## Grind 84: call-temp pass on the released wave 48 units, and MUST_MATCH guards (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- Guards from the wave 48 review: `partyForceWalk` (hand-inlined `unk_800c8f24`, `#else` calls it), `envDisp_FF` (`envFF_Sqrtf`, `#else` `sqrtf`) and `hitDelete` (`hitDeleteUpdateActiveList`, `#else` `hitUpdateActiveList()`) now have `MUST_MATCH` guards with natural code. The hitdrv copy inside the unit's `MUST_MATCH` branch is gone, since the whole helper is guarded.
- `btlevtcmd_SetPartsBaseScale` reaches 100% with a block-local `partId` for the nested `evtGetValue`. `btlevtcmd_snd_se` goes 84.68% to 94.77% by passing `(s16)evtGetValue(event, args[3])` nested in the call (the target has no temp there); the three held registers still come out in a different order (target unit r28, v1 r30, v2 r29), so it stays NONMATCHING.
- `winMgrSelectDelete` takes `WinMgrSelect*` (the `(s32)` casts at both callers in `evt_window.c` are gone); score unchanged at 82.06%, the target's early `mr r0, r3; li r3, 0` is not reached by any shape I tried.
- Not cracked, recorded for the permuter: `btlevtcmd_AnimeChangePoseDirect` (the `id` from `BattleTransID` goes through `r0` in ours, direct in the target, with the nested-assignment form making no difference), `btlevtcmd_FaceDirectionAdd`/`MoveDirectionAdd` (`evtSetValue(event, r, evtGetValue(event, r) + delta)` takes the `r0` temp form where the `- delta` twin `FaceDirectionSub` matches; a local, `delta +` and `cur + delta` all keep it), `npcDisp`/`npcDisp_xlu`/`npcDisp_offscreen` (`anim = animPoseGetCurrentAnim()` goes through `r0` before `camGetPtr`; declaration order, line order and a block-local temp do not move it), `_sscallback` (prologue scheduling).
- Already 100% on main: `btlevtcmd_GetTakeoffPosition`. Held by other waves: `ac_power_gauge_lv2` (wave 49), `mot_hammer` (wave 49), `imgdrv` (wave 53), the `battle_audience` functions on the list are unwritten bodies on main (0.15% for `BattleAudienceSettingAudience`), so there is no call-temp fix to apply until they are decompiled.

## Grind 85: three more units link, and function order is a pool lever (2026-10-01, Claude Sonnet 5.5 "TTYD Claude #3")

- **Linked:** `countdown` (sdata2 colours as named objects, the other floats literals, a `static u32` sbss gap), `himo` (`Pos`/`Nrm`/`Col` aligned to 32, pooled floats defined) and `battle_item_data` (+0.59% data matched). The `volatile` comma hack around `intplGetValue` is gone from `countdown` and `battle_item_data`; the prototype fix from Grind 82 makes the plain call match.
- **`battle_item_data` was a function-order problem.** Its source listed `_nancy_mirage`/`_throw_item_icon` in the wrong order, which moved strings and floats in `.rodata`/`.sdata2`; `tools/fn_order.py --write` fixed it. A sweep of `fn_order.py` over all 63 units whose source order differs from the target text order found no other unit where reordering helps without dropping a function score (most of the units are claimed by other waves, and `mot_ship` gained 0.01% for a 730-line diff, not kept).
- **Parked:** `unit_party_yoshi` (branch `wip-yoshi-data`: `.data`/`.sdata`/`.sdata2` equal via `dol_gen --append`, the typed `BattleWeapon` tables, `negone_one_NNN` statics; blocked by `.rodata`, where the i2f double lands one string early, and three near-miss functions), `eff_mist2` (with the correct function order and named floats `.rodata`/`.sdata2` are equal, but `effMist2Disp` stays 91-95%: named floats get hoisted into registers, `volatile` reads change the schedule), `fadedrv` (`.sdata2` pi pools after 0.5/180 where the target has it first; a one-statement `pi * (x * 0.5f / 180.0f)` fixes the pool and breaks `disp_maku`), `mot_dokan` (the 180/120/-120 pool order), `msgdrv` (data offsets shift until `msgAnalize`/`msgDisp` are written), `lightdrv` (`lightMain` is a stub).
- **Shared cause not found:** `partyGetTargetDir`, `partyGetTargetDist3D`, `unk_800c9f48` all hold `id != 0` in a callee-saved register across two `anotherPartyGetPtr` calls where the target recomputes it (`neg/or/srwi` into `r3` each time). `!!id`, `(u32)id != 0`, ternaries and `> 0` changed nothing or made it worse.

## Wave 53, batch 1 (2026-10-01, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

- Wave 53 (launched 16:56 on main eb6320c; Isaac's 8-hour autonomous run): every unclaimed game file's functions
  below 95% up to 2.5 KB, grouped per file as in waves 51/52, 174 agents. Workspaces as tools/wave_launch.py builds
  them (no `tools/perm_*.py`, the base's DOL asm linked, `.git` left out); the prompt lists, per unit, the functions
  a review took out (effCalcMayaAnimMatrix, mapSetMaterial) and they are out of the agents' lists; every prompt says
  to reach a named object by name, never as another object's address plus an offset. Mac builds run
  `nice -n 10 ninja -j8` (the helper scripts too).
- Batch 1 (40 agents, onto main 80dc510): 8,780 -> 8,826 functions. **battle_camera.c's agent copy sends mwcceppc
  into an endless loop** (a build sat at 100% CPU on it for over an hour): that file is as on main. chuchurina,
  yuu_02, win_root, bom_01, battle_status_icon and muj_20 (a lower score, or nothing new once the section drops were
  cleared) as on main.
- Review (`scratch/wave53/review1.md`): no behaviour defects; two older bugs are fixed by this batch
  (`BtlUnit_snd_se_pos` tested `< 0x4D80` for retail's `== 0x4D80`; `BattleActionCommandSetup`'s
  `while (entry++->id)` skipped the first entry). Fixed: muj_20's draw placeholder (deleted without the function
  being written), notes that cited pragmas the audit had removed (evt_bero, win_log, mri_puni, battle_ac,
  zubastar), deleted notes on functions still below 100% (viPostCallback, FontDrawColor), an orphaned
  BtlUnit_ControlPoseSoundMain block and prototype (the greedy revert had taken its body out), mariost's unused
  statics, fontmgr's duplicate colour inline. **Lesson:** `pragma_audit.py` and `greedy.py` remove code but not
  the comments that describe it; check the notes around every function they touch.
- Claude #1's second review of #206: battle_attack_audience had split the 0x40-byte btlataudevtDummy into an
  invented `btlataudevtDummyBody` under MUST_MATCH (rules 13/17): the file is as on main
  (btlataudGetAttackEventPtr 93.75%). Approved header correction: include/manager/fontmgr.h `kanjiSearch` takes
  `u32`, not `u16` (retail has no clrlwi; the only caller loads with lhz). The four NONMATCHING markers it flagged
  (main_star in sac_bakugame/sac_zubastar, DrawSubMenuCommonProcess, puniModeAway) were dropped because those
  functions are at 100% in the merged build; `nm_refresh.py` now takes the lowest score when a name appears
  twice in a unit. Also open: party_vivian's `*(s8*)` switch operands (lines ~106/190) outside MUST_MATCH.
- **Follow-up (form, behaviour equal):** mot_roll reads three named vectors as `vec3_802c5d40[4..6]` (older; its
  header now declares the array non-const); match-only shapes outside MUST_MATCH in animdrv (pose byte offsets),
  battle_audience_kinopio (volatile-comma intplGetValue arguments), fontmgr (FontDrawStart's hand-inlined copy) and
  mri_puni (puniswSetindex, puniMapFindbuff); evt_bero's `*(u16*)&` on a u16 field; sac_bakugame's tables as u32
  blobs cast to structs. unit_party_vivian reaches two vec3s as base+offset only under MUST_MATCH (names in #else).

## Wave 53, batch 2, and wave 54 (2026-10-01, Claude Opus 5.5 "TTYD Claude #2"; agents DeepSeek V4.1 Flash)

- Batch 2 (80 agents, onto main c294f5f): +28 functions, with end_muki at 100% from Claude #3's permuter mask
  (`off & mask`, mask = -1, under MUST_MATCH). Files that stopped building and units with a lower score and nothing
  new are as on main; the audit's removed pragmas no longer leave comments describing them.
- Review (`scratch/wave53/review2.md`): bom_01 `ext_init` looked up pose group `"c_bom_sen"` where retail uses
  `"R_1"`, so no pose got its index or timer (87.81%, behaviour fixed). muj_20 had lost the pressStartGX
  placeholder again (as on main now); party.h/evt_shop.h/sac_muki.h declarations left from reverted work are out;
  party_motion_behind_mario_fly keeps its NONMATCHING (98.74%). battle_unit's `dat_8041c3b8` stays a non-const
  GXColor although retail has it in .sdata2: `const` takes BtlUnit_Entry from 100% to 99.31%. Open (LOW): unused
  float externs in bom_fall, invented constant names (`float_10`) in party_nokonoko.
- Claude #1's second review of #209: barriernSatelliteToOutside (v2, mri) returned DONE when satellites had
  moved; retail (`subic`/`subfe`/`and` on `updated`) returns DONE only when nothing moved. Fixed there and in
  v1's copy, which had the same inversion on main (`found == 0 ? DONE : BLOCK`). winMsgDisp (reached named objects
  past str_802F3FE8), party_motion_jump (past vec3_802f3e40) and tou's disp_proc (raw offsets) are back to main's
  form; eff_opuku_wcurtain as on main (its clampS16 stays, since retail's GQR5 store saturates). eff_mugi is
  code-complete with every section at 100% but the dol checksum still differs (a base-reached or padding issue for a
  data pass); its opt_common_subs stays open to the end of the file (the following functions need it).
- **Lesson:** don't switch the collection worktree to another commit while a review agent reads it (the build went
  stale under the reviewer); prepare the next wave's report from a separate checkout or the tower.
- **Wave 54** (launched 21:16 on main c294f5f, base `~/ttyd-w54base` from `git archive`): wave 53's recipe with
  functions and groups up to 4 KB, leaving out units whose wave 53 work wasn't merged, battle/battle_camera (its
  agent copy hangs mwcceppc) and the functions reviews took out; 80 agents.
