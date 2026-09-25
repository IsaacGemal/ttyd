# TTYD decomp: the ledger

The single record of every decision on this fork and why it was made. Read it top to bottom before starting
work. New chapters go at the bottom, dated, with who made the call. The "Current state" section is the only part
that gets rewritten in place; everything below it is append-only history (correct an old chapter by adding a
new one that supersedes it, and say so).

Private notes (inference provider, accounts, keys, capacity) go in `docs/CONTEXT.private.md`, which is
gitignored, never here: this repo will be public.

Agents running in bulk never edit this file. They write their own `LOG.md`/`REPORT.md`; the orchestrator folds
anything worth keeping into a chapter here.

## Current state (rewrite this section as things change)

- Fork of [doldecomp/ttyd](https://github.com/doldecomp/ttyd) at `2df569d` (Jul 5, 2026), private at
  `IsaacGemal/ttyd`. `upstream` is fetch-only; its push URL is `DISABLED`. `gh` defaults to this repo.
- Target: `G8MJ01` (Japan, Rev 0), the only version the project supports. The disc is already extracted into
  `orig/G8MJ01/`.
- Progress (local build, Sep 25 after wave 3, PR #2): 3,114 of 9,770 functions matched. Before the fork's
  first wave: 12.16%, 2,065 (decomp.dev shows 2,027 because of the filename-case bug below). Nothing new is
  linked yet: `event` and `battle` are at 100% code, the next step for them.
- The build and objdiff run on the tower (`pop-os`) from `main`. Next step: break down the unmatched functions by unit, size and kind before designing the queue.

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

