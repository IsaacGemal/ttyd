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
- Baseline (local build, Mac and Linux agree, Sep 24): 12.16% of code matched, 2,065 of 9,770 functions, 100
  of 1,239 units complete. decomp.dev shows 2,027 because of the filename-case bug below. SDK 100%, MusyX 286/333 functions, game code (DOL) 942 of 5,687 functions, RELs 16 of 2,780.
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
