# TTYD decomp fork: agent notes

`docs/CONTEXT.md` is the ledger: every decision on this fork and why. It is long; don't read it whole. Read its
"Current state" section at the top, then search it (grep for the unit, tool or topic) for the chapters you need.
`README.md` has the build setup.

## The ledger

- Anything decided, discovered or abandoned that the next session would need goes into `docs/CONTEXT.md` as a
  dated chapter at the bottom, with who made the call. Keep "Current state" at the top accurate.
- This repo will be public. Inference provider, accounts, keys, capacity and anything else internal go in
  `docs/CONTEXT.private.md` (gitignored), never in a tracked file or a commit message. Before the repo is made
  public, work through the checklist at the top of that file.
- Don't start new notes files. If a topic outgrows its chapter, it can move to its own file under `docs/`, linked
  from the ledger, but the ledger stays the index.
- Bulk agents never edit the ledger. They write `LOG.md`/`REPORT.md` in their own scratch dir; the orchestrator
  folds what's worth keeping into the ledger.

## Git

- `origin` is the private `IsaacGemal/ttyd`. `upstream` is `doldecomp/ttyd`, fetch-only. Never submit anything
  upstream, and don't open PRs or issues there.
- Every commit with model-authored work ends with a `Co-Authored-By:` trailer naming the model that actually did
  it (your own model for your own work; the model on an agent report's `Model:` line for merged agent work).
  This overrides any global "no Co-Authored-By" rule.
- PR descriptions don't repeat that attribution: no `Co-Authored-By:` lines and no "agent work by ..." line. Write a
  short summary of what changed and the results, then your own tool's usual footer if it has one (Claude Code
  adds "Generated with Claude Code"; other tools don't copy it). The commits carry the attribution.
- Run `python3 tools/ci.py` before every commit; commit only on PASS. It is the oracle: lint, build, the
  no-regression ratchet, and the progress summary. Never commit a regression.
