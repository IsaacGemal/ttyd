# win_mario pool attempts (2026-10-06, Codex GPT-6)

Limit: 35 source build experiments. Work remains uncommitted.

## Baseline

`pool_plan.py window/win_mario --no-probe` confirms BASE-REGISTER mode.
Configured with explicit paths to existing tools to avoid Ninja downloading tools; no tools or configure.py edits.
Fresh `agent_check.py` score is **96.15%** for winMarioDisp (task/comment says 93.90%); all other eight functions are 100%. This fresh score is the no-regression baseline.
.sdata2: ORDER, 0.5/38 swapped at entry 51. .rodata: DUP_OURS, one zero Vec.
reloc_value_check --partial: 0 findings.
Function order differs: retail Disp, fukidashi, unk_8016af5c, winZenkakuStr, Main2, Main, Exit, Init2, Init; ours Disp, fukidashi, Init, Init2, Exit, Main, Main2, winZenkakuStr, unk_8016af5c.

## Experiment 1

Idea: enable pool_data on under MUST_MATCH, keeping existing objects initially to isolate the conversion addressing effect. Native int-to-float conversions stay intact.
Result: unchanged 96.15%, other eight 100%; both pool defects unchanged.

## Experiment 2

Idea: full base-register port: data-owned strings as literals with their tables at the top, code strings as literals, local static Vec templates, and actual digit-table initializers in the existing inline bodies. Remove manual base-offset reads and leave native conversions.
Result: 80.93% Disp, all other eight 100%; compiler now loads both magic doubles at pool+0x820/0x828. Named local templates affect the pool base register rank; table layout and small string prefix need correction.

## Experiment 3

Idea: retain the tuned base-pointer loads but initialize vp from the first pooled string; interleave local static color/raw/tag tables in their retail slots and preserve the baseline small-string prefix. No new functions.
Initial build: undefined WMS macro (definitions were inside #else); fixed before measuring.

## Experiment 4

Idea: complete experiment 3 by exposing string literal macros in both builds.
Result: 71.69% Disp, 53.18% fukidashi; duplicate pool-base/vp registers. .rodata bytes now correct until the tail; entry classifier counts the pointer table as duplicate zero Vec. Rejected.

## Experiment 5

Idea: remove the duplicate vp register and use the corrected local pooled tables plus templates directly, retaining small-string named prefix.
Result: 80.93% Disp, others 100%. Table corrections fix pool positions but not register allocation.

## Experiment 6

Idea: replace Disp static Vec templates with compound literals at their actual assignments, as suggested by the task, avoiding constant-object load hoisting.
Result: 85.39% Disp, others 100%; compound templates change parse-time ordering and keep the table-load hoists across loops. Rejected.

## Experiment 7

Idea: start from experiment 5, use volatile views under MUST_MATCH for static Vec and row-table loads to preserve retail loads inside loops, leaving const natural access under #else.
Initial build: STR_AT macro accidentally hidden under #else; no measured result.

## Experiment 8

Idea: finish experiment 7 with STR_AT available in both modes.
Result: 81.15% Disp, all others 100%. Compiler base is now r31; four separate row-table address webs cause five extra saved GPRs. Debugger unavailable (installed script missing), so no debugger compile experiment.

## Experiment 9

Idea: disable common subexpression elimination only for the matching Disp build to prevent extra array-address webs, retaining volatile table/template views.
Result: 80.65% Disp, other eight 100%; disabling CSE does not remove the row-table bases. Rejected.

## Experiment 10

Idea: represent the pooled row templates as structs with direct fields instead of arrays; this exposes each fixed offset without a reusable array-base address. Start from experiment 8, without the CSE pragma.
Result: identical to experiment 8 (81.15%); struct fields still lower through extra table-base webs. Rejected.

## Experiment 11

Idea: narrower C representation for the conversion only: reuse one union temporary under MUST_MATCH, build the IEEE bias encoding, and subtract doubles through the existing base; natural mode keeps ordinary casts. No new helpers or .text. Define only the unsigned bias at the end of the named prefix; native Main/Init2 conversions still emit the signed bias.
Result: 92.27% Disp and 99.97% Init; manual double arithmetic emits fsub+frsp instead of fsubs, and the native signed bias pools after Init strings. Rejected; no manual conversions kept.

## Experiment 12

Idea: return to the native-conversion pool port (experiment 5), disable loop-invariant hoisting only for matching Disp so row-table loads remain inside their loops.
Result: unchanged 80.93% Disp; loop-invariant pragma does not alter the const-template addressing. Rejected.

## Experiment 13

Idea: spell row-table reads as explicit byte-offset pointer dereferences instead of array subscripts, preserving nonvolatile scheduling; keep native conversions and local static templates.
Result: unchanged 80.93%, other eight 100%; byte-offset dereferences canonicalize to the same array addressing. Rejected.

## Experiment 14

Idea: initialize actual automatic row tables from literals inside the row loop, avoiding static-array loads and their retained bases. First test code shape; template ordering will need a separate pass.
Result: 81.53% Disp, others 100%; automatic row templates still leave address webs. Rejected.

## Experiment 15

Idea: use the first compiler-pooled string directly as the matching pool-base expression (no named vp web), while retaining the existing base-relative shape and ordered local templates. Natural mode retains named/typed template access.
Result: 69.99% Disp, 48.80% fukidashi; direct pool-base expressions worsen source shape. Rejected.

## Experiment 16

Idea: resolve the baseline pool checker defect using one genuine 2x10 shared digit table (80 bytes), read by both existing public number functions, instead of separate 40-byte templates. This also supplies the matching inline copies through their existing offsets. Start from baseline.
Build fails: MWCC rejects nonconstant array initializers; switch to the equivalent memcpy.

## Experiment 17

Idea: copy the 40-byte digit-table rows with memcpy (constant size) into the existing local arrays, allowing MWCC to inline the copies.
Result: .rodata IDENTICAL; Disp remains 96.15%, public number functions regress (73.47%/74.51%) because memcpy is a call. Digit strings now pool too late in .sdata2.

## Experiment 18

Idea: emit the shared table before the code to preserve the early digit strings, and write ten explicit local assignments per public number function instead of memcpy.
Result: .rodata IDENTICAL, original .sdata2 0.5/38 swap only; public functions 89.33%/95.02%, Disp 96.15%. Explicit scalar assignments change the aggregate-copy scheduling.

## Experiment 19

Idea: model each digit row as a 40-byte struct and initialize the original local table with a struct copy, reproducing the original compiler template-copy shape while retaining the genuine shared 80-byte object.
Result: same scores as experiment 18; remaining public-function differences are early template loads interleaved with prologue saves and row-1 base+0x28 lowering.

## Experiment 20

Idea: apply the brief's signed-address round trip under MUST_MATCH to preserve the row address as a separate codegen expression and delay the aggregate loads. Natural mode uses a direct struct copy.
Result: 95.80% winZenkakuStr, 94.46% unk_8016af5c; improves prologue scheduling but does not preserve both matches. Saved for inspection; not kept.

## Experiment 21

Idea: test mutable views/storage for local templates under MUST_MATCH, explicitly placed in .rodata, to preserve retail aliasing and avoid constant load hoists. Natural mode remains const. Native conversions retained.
Build failed: this compiler does not accept a local section attribute (ignored section pragma / parse error). Rejected.

## Experiment 22

Idea: disable scheduling for the two public number functions from experiment 20 to delay shared table loads until the original prologue ends.
Result: both public functions fall to ~56%; global scheduling-off changes too much. Rejected.

## Experiment 23

Idea: isolate the .sdata2 swap on the baseline: initialize f27 to 0.5f at declaration instead of a later assignment, preserving the variable order.
Result: unchanged 96.15%, all other eight 100%; declaration initialization does not alter pool order. Rejected.

## Experiment 24

Idea: express the half factor as division by 2.0f, including the width calculation, per pool_plan's reciprocal guidance.
Result: unchanged 96.15%, others 100%; both baseline pool issues unchanged. Rejected.

## Experiment 25

Idea: disable propagation for the native pool port to preserve row-table and drawing temporaries rather than creating extra retained constant/address webs.
Result: 80.30% Disp, all others 100%; propagation-off is another regression. Rejected.

## Experiment 26

Idea: assign the shared digit-row aggregate after all local declarations, using the round-trip address form, to change initial-copy scheduling without changing semantics.
Result: same as experiment 20 (95.80%/94.46%), Disp unchanged 96.15%; assignment placement does not help. Rejected.

## Experiment 27

Idea: use a volatile aggregate view of each shared digit row under MUST_MATCH to prevent early speculative template-load scheduling, with a plain copy in natural mode.
Result: unchanged from experiment 20 (95.80%/94.46%); volatile view does not fix the public-function scheduler. Rejected.

## Experiment 28

Idea: disable propagation on baseline Disp, isolating whether constant propagation creates the 38/0.5 pool swap; keep all other source unchanged.
Result: 94.63% Disp, other eight 100%; half now emits too early (entry 49), six entries displaced. Rejected pending one placement test.

## Experiment 29

Idea: with propagation off, move the half assignment after the 50 and 2 computations, immediately before the 38 computation, to reproduce target pool order.
Result: 94.62% Disp, other eight 100%; 0.5/38 now ordered correctly, but 4/12/21 reorder. Still below baseline, so rejected.

## Experiment 30

Idea: unsigned rather than signed address round trip for shared digit rows, to preserve their original aggregate-load prologue scheduling.
Result: 89.33% winZenkakuStr / 95.02% unk_8016af5c, Disp unchanged 96.15%; unsigned round trip folds to the original shared-object code. Rejected.

## Final decision

Stopped after **30 source-build experiments** (limit 35). Restored src/window/win_mario.c byte-for-byte to its starting version. No experimental implementation met both the pool/addressing goal and the no-regression constraint, so none is retained. No helpers with extra .text were added, no tools/configure.py edits, no commit or push.

Findings:

- The full native pool port does make the compiler load the magic at pool+0x820/+0x828. Local constant-table addressing and hoisting introduce extra GPR webs and change FP allocation; tested ports scored 80.30–85.39%, well below the fresh 96.15% baseline.
- Keeping the original manually based pointer alongside the compiler-generated pool pointer introduces a second live base register; it does not coalesce into the compiler pool base in the tested forms.
- The reported zero-Vec duplicate is an **entry-boundary artifact**: retail names one 80-byte digit-table object at +0x184, while our two genuine 40-byte compiler templates begin at +0x184/+0x1AC. sdata2_diff.key_of strips all zero bytes, so these objects compare as empty keys, just like zero Vec objects. It reports the last such Vec as the extra entry. There is no extra Vec in the actual .rodata byte stream.
- Direct byte comparison of baseline .rodata: original 2152 bytes, ours 2147 bytes; every overlapping byte matches. The missing five bytes are all zero tail alignment. The checker cannot report IDENTICAL while retaining these two template symbol boundaries, even though the data bytes agree after tail alignment.
- A genuine shared 80-byte table makes the tool report .rodata IDENTICAL, but it changes public number-function prologue load scheduling (and row-1 addressing). Those previously 100% functions regressed, so that change was rejected.
- Disabling propagation and moving the half assignment can fix the 0.5/38 order, but changes 4/12/21 ordering and lowers Disp to 94.62%.
- tools/mwccdbg.py is unavailable here: ~/mwccdbg/mwcc-debugger/mwcc_debugger.py is missing. No debugger installation was attempted.

Final validation after restoration:
- agent_check: winMarioDisp **96.15%**, other eight functions **100%** (exactly baseline).
- Natural compilation without MUST_MATCH: **PASS**.
- sdata2_diff: original .sdata2 ORDER (0.5/38) and .rodata DUP_OURS remain; requested IDENTICAL goal is **not achieved**.
- reloc_value_check --partial win_mario: **0 findings**.
- fn_order_check: **ORDER DIFFERS**, exact original starting order unchanged.
- cmp against starting source: exact equality. git diff has no tracked source, tools, or configure.py changes.

No retained code fix. The task remains incomplete; these are bounded negative results, not a claim that no C solution exists.
