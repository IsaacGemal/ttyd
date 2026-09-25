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
