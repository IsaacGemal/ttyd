# eki_00 matching attempts

Budget: at most 40 build experiments. All work stays in this checkout; no tools/ source edits or pushes.

## Baseline / environment

- Initial ninja attempted to redownload existing build/tools/dtk because its download command was absent from the copied ninja log. Configured with explicit existing tool/compiler paths to avoid downloads. No tools/ sources changed.
- Baseline: ext_moamoa_main 97.04%, 244 real instruction differences; ext_moamoa_init 99.38%, moamoa_hit_check 97.25%; remaining 18 functions 100%. Relocation-value check: 0 findings.
- Saved source, object, scores and configuration in scratch/codex-eki/.

## Experiment 1

Idea: use a named one-element const array for the first 0.5 multiplication, retaining a natural literal under #else. Array access should produce the original lis/addi/lfs address pair.

Result 1: 97.05%, 243 real differences. Named array alone emits lis/lfs, not lis/addi/lfs; also moves its definition ahead of the existing anonymous pool (data layout must be restored before retaining). Other diffs reveal randf expansion/FP operand order and stack-slot differences beyond the brief's addressing diagnosis.

## Experiment 2

Idea: introduce a block-local pointer to the half array under MUST_MATCH. opt_propagation off should retain address materialization separately from its load.

Result 2: 97.07%, 241 real differences; the pointer produces the required lis/addi/lfs, but its placement and the sum's temporary still schedule differently.

## Experiment 3

Idea: compute the angle after the flag/state assignments, as one expression using the explicit half pointer. This changes the sum's evaluation and vector-copy scheduling without affecting semantics.

Result 3: 96.79%, 250 real differences; rejected, restored experiment 2.

## Experiment 4

Idea: move named Vec definitions after the code under MUST_MATCH (retain pre-code extern declarations), so their known contents cannot influence front-end lowering. Keep the natural definitions in their original position.

Result 4: unchanged 97.07%/241, with rodata reordered from +0x7c. Rejected and restored experiment 2.

## Experiment 5

Idea: put the half pointer after the angle temporary, reverse the sum operands to reproduce the target's field_0x55C-first loads. This isolates local evaluation order.

Result 5: 97.06%, 242 real differences; no benefit over experiment 2. The named half experiments have unresolved extra pool data, so restart from baseline to isolate improvements that preserve layout.

## Experiment 6

Idea: use a main-only inline randf expansion with in-place multiply/add, under MUST_MATCH, leaving the existing standalone randf and natural calls unchanged. Target randf hunks keep the random result in place and multiply it as the first operand.

Result 6: 91.84%, 547 real differences; split operations stopped fusion and the helper was parsed with propagation off. Rejected.

## Experiment 7

Idea: main-only randf helper parsed before propagation is disabled, returning min + t*(max-min) to change fused-multiply operand order while retaining fusion and constant range folding.

Result 7: identical baseline score/diffs/data. Rejected as neutral.

## Experiment 8

Idea: commute the multiply in the main-only range helper (range * random). MWCC's fused multiply uses operands differently from the C spelling; measure the alternate ordering directly.

Result 8: identical baseline score/diffs/data; MWCC normalized the commuted arithmetic. Rejected.

## Experiment 9

Idea: expand the range expression directly at main call sites under MUST_MATCH. This removes the inline callee's temporary/return copy, while retaining the same random calls and range arithmetic. All actual bounds are constants or side-effect-free field reads.

Result 9: 92.23%, 531 real differences, changed pool. Rejected. Direct expressions simplify random-range products that the original retains; keep inline range expansion.

## Experiment 10

Idea: a block-local literal half, instead of a separate named object, may retain the full address pair while sharing the existing pool. Keep the angle temporary and existing statements.

Result 10: 97.06%, 241 real differences; .rodata byte-identical. Save as first viable improvement (the local literal produces an address pair without extra data).

## Experiment 11

Idea: move the case-0x2F sound Vec declaration ahead of the main Vec declarations under MUST_MATCH, to reverse the two 12-byte stack slots without changing lifetime or data. Original uses 0x14 for this Vec, 0x20 for ekiVec3's return temp.

Result 11: score unchanged, 253 real differences; moving scope extends storage lifetime and changes additional stack slots. Rejected.

## Experiment 12

Idea: voice Vec helper takes the matrix owner pointer, rather than three precomputed float arguments. This leaves the matrix load inside the inline, after the base vector setup, matching retail's r5 matrix/r8 base order.

Result 12: 97.08%, 233 real differences; voice Vec register scheduling now matches, only its temporary stack offset differs. .rodata remains identical. Retained provisionally.

## Experiment 13

Idea: move the case-0x2F sound-vector construction/call into an inline helper under MUST_MATCH. Both sound temporaries then belong to inline expansion, which may reproduce the original stack allocation order.

Result 13: 97.09%, 217 real differences. Both voice/hit sound Vec stack slots now match; all other scores and .rodata unchanged. Retained provisionally.

## Experiment 14

Idea: delay equal-bound random-range arithmetic until moamoa_jump's argument evaluation. Keep raw random draws in locals across the next draw, as the target does; use an inline range helper so zero products remain runtime operations.

Result 14: build refused a C89 declaration after statements in case 0x1B (agent_check called it a timeout because the failure output quotes the timeout command; direct ninja reveals the syntax error). One failed build experiment.

## Experiment 15

Idea: correct experiment 14 by giving the new case-0x1B local its own block, then measure delayed range arithmetic.

Result 15: 97.07%, 224 real differences, rodata identical; delayed range evaluation removes some copies but changes saved FPRs and evaluation order. Rejected versus experiment 13.

## Experiment 16

Idea: inspect the current best's compiler register webs with mwccdbg to distinguish remaining scheduling from incorrect FPR web lifetimes. Counts as a build experiment if the debugger compiles.

Result 16: debugger unavailable at its documented local path; no compile ran. Continue using the full instruction diff (the task's zero-differing-web premise cannot be reverified here).

## Experiment 17

Idea: give the two angle inputs independent float locals before multiplying by the local half. This retains three simultaneously live inputs and may yield the target's f2/f1/f0 load order.

Result 17: same rounded 97.09%, 218 real differences; individual inputs still die before half is loaded. Not retained.

## Experiment 18

Idea: initialize half before the two angle inputs and write b+a, so half/input locals overlap and operand order matches the target.

Result 18: 97.09%, 218 real differences; literal scheduling still lazy. Restore experiment 13 (217 differences).

## Experiment 19

Idea: reset propagation only for main, retaining the trig inlines parsed with propagation off. Test whether the broad existing pragma causes range-copy/operand differences.

Result 19: 70.81%, 1702 real differences; broad propagation setting is necessary. Rejected.

## Experiment 20

Idea: reset opt_lifetimes only for main, keeping helper parse settings and propagation off. Test whether remaining saved-FPR choices come from the broad lifetime pragma.

Result 20: 94.47%, 876 differences; lifetime setting is also necessary. Rejected.

## Experiment 21

Idea: in scatter case 0x2F, reuse one float for the random distance and final z coordinate (distance is dead once z is computed). This should preserve the target's f31 for both roles instead of moving the z result into f29.

Result 21: 97.09%, 218 differences; reusing distance gives f28 rather than f31, worse than the best. Rejected.

## Experiment 22

Idea: swap the shared tx and random-fraction local declaration positions under MUST_MATCH. Many remaining hunks show target tx in f27 and random fraction in f28, where ours keeps tx in f30 and fraction in f27.

Result 22: 97.05%, 239 differences; main declaration swap does not move the shared random fraction's web as needed. Rejected.

## Experiment 23

Idea: represent the shared random fraction through a MUST_MATCH union, as already used successfully for voiceSfx. This may prevent frontend copy propagation into inline operands and preserve the target's independent random-result web.

Result 23: unchanged 97.09%/217/data; the float union does not change these webs. Rejected.

## Experiment 24

Idea: reuse scatter case's ty local for its earlier raw random fraction (the two values are never live together). This gives the random result the same saved FPR as the later jump-y parameter, which is f28 in the target.

Result 24: 97.09%, 216 real differences, matching f28 random fraction in scatter. Retained provisionally; data unchanged.

## Experiment 25

Idea: pin ext_moamoa_init's first source-vector address with the documented sign-extension round trip. This targets its seven remaining source-before-destination scheduling differences without changing data.

Experiment 25 pre-edit guard found that init already uses a volatile destination/source copy rather than plain Vec assignment; no source edit or compile occurred. Apply the address cast to that actual expression.

Result 25: init 98.82%, 16 differences; rejected (cast changes ranks but not the desired volatile read order).

## Experiment 26

Idea: read init's first volatile Vec into a block local before evaluating volatile wp, then copy that value. This explicitly sequences source evaluation first; optimizer may scalarize the temporary.

Result 26: init 97.93%, 25 differences; temporary adds copy/stack effects. Rejected. Leave init at its baseline 99.38%.

## Experiment 27

Idea: move the hit-result-3 flag/half/vector setup into a MUST_MATCH inline helper. Changing its locals to inline temporaries may give the target's load scheduling and arithmetic register order while leaving the pool untouched.

Result 27: 97.09%, 217 differences; helper introduces an extra saved-FPR difference. Rejected.

## Experiment 28

Idea: use the same guarded volatile Vec-copy shape already used by init for main's initial pose-position copy. Target requires load/load/store/store/load/store; ordinary struct assignment hoists its final load.

Result 28: 97.29%, 208 differences; initial vector word-copy order now matches and data unchanged. Retained provisionally.

## Experiment 29

Idea: with the vector copy constrained, flatten the angle sum into the half multiplication after flag assignments. Test right-operand evaluation for the target's half/input register order.

Result 29: 96.80%, 223 differences; flattening moves wp loads below flag stores. Rejected.

## Experiment 30

Idea: use a rider-vector helper with scalar parameters in z/y/x order, preserving x/y/z stores. This changes inline parameter ranks to target dx=f2, dy=f1, dz=f0 without changing operations.

Result 30: identical 97.29%/208; reversing inline parameter order alone is neutral. Rejected.

## Experiment 31

Idea: reverse rider-delta local initialization order under MUST_MATCH so z/y/x first writes allocate f0/f1/f2, retaining the existing helper's x/y/z destination writes.

Result 31: same score/count; register choices match but load/store scheduling reverses. Not retained.

## Experiment 32

Idea: keep reversed rider local declarations but assign x/y/z in the original source order, separating declaration rank from instruction order.

Result 32: 97.29%, 204 differences. Rider-vector tail now exact. Retained provisionally.

## Experiment 33

Idea: express case-0's two pairs of jump ranges through an inline helper taking the already-drawn random fraction. This may preserve random*range operand order while keeping draws and scheduling separate.

Result 33: 96.72%, 232 differences, rodata reorders from +0x55c. Rejected.

## Experiment 34

Idea: apply the declaration-versus-assignment distinction that fixed the rider Vec to the half-angle inputs. Declare half/a/b, then initialize half before inputs; preserve volatile vector-copy sequencing.

Result 34: same 97.29%/204, but more complex source; not retained.

## Experiment 35

Idea: give case-0's jump-range factors local float names under MUST_MATCH rather than literal operands in call arguments. This may prevent MWCC commuting the fused multiply's random operand to the second position.

Result 35: 97.09%, 211 differences, rodata reorders from +0x560; named factors fix operand order but disturb scheduling/pool order. Rejected.

## Experiment 36 / final source

Restore the verified best (experiment 32) and document the retained matching shapes. Stop pursuing 100% within this run: 204 differences remain and the missing register debugger prevents resolving the saved-FPR/interference differences efficiently. Keep configure.py NonMatching, because main, hit-check, and init remain unmatched.

Retained fixes: local half literal (full address pair without duplicate data), pointer-based voice Vec helper (matrix/base register order), inline hit-sound helper (temporary stack order), scatter random fraction sharing ty's f28, volatile initial pose Vec copy (word-copy order), reversed rider local declarations with original assignment order (tail exact).

## Final validation and state

- `ext_moamoa_main`: **97.044914% -> 97.29303%**; real instruction diff rows **244 -> 204**. Object function size is 0x31F4 vs original 0x31C0 (0x34 bytes too long); baseline was 0x31F0.
- `.text` section fuzzy score: **97.906075% -> 98.04849%**. Every other function/section fuzzy score is unchanged. Baseline-versus-final reports are in `scratch/codex-eki/{baseline,final}-report.json`.
- Every other function has identical code after masking relocation fields. No helper function bodies were emitted. `.rodata` (1576 B), `.data` (9384 B), and `.bss` (4 B) are unchanged from baseline.
- Natural compilations of baseline and final both pass, with identical normalized function code and data. All newly introduced matching shapes are guarded by MUST_MATCH, with the baseline natural form in #else.
- `python3 tools/reloc_value_check.py eki_00 --all`: **0 findings**.
- `tools/lint_patch.py` and `git diff --check`: clean.
- Full `ninja -j2`: exit 0, **32 files OK**. This checks the existing NonMatching configuration: eki_00 still links its original object, not our partial function.
- `configure.py:1589` remains `Object(NonMatching, "eki_00.c")`; no tools/ source modifications, commit, or push.
- Conservative build-pass accounting: 35 compiled source experiments (1–36 except debugger-only 16), 1 baseline build, 1 repeat of the failed C89 syntax build to inspect its diagnostic, 1 full ninja build, and 2 natural builds = **40**, at the limit. Checks using existing objects and the experiment-25 no-edit guard did not compile.

### Precise remaining blocker

The remaining 204 diff rows are not just anonymous-vs-named symbols. Their distribution in original switch regions (counts include objdiff insertion/deletion alignment rows) is:

| Region | Real diff rows |
| --- | ---: |
| Before switch: hit-result-3 half/Vec setup | 11 |
| case 0x00: random-range fused operands | 4 |
| case 0x34: sine/cosine argument setup, ty register and scheduling | 21 |
| case 0x01: random/trig setup and equal-bound range scheduling | 66 |
| case 0x02: random/trig and range scheduling | 34 |
| cases 0x24/0x25/0x26: random fraction/target registers and equal-bound ranges | 28 |
| case 0x2F: z FPR, fused operand order, randf result copies | 7 |
| case 0x19: half multiply operand order | 1 |
| case 0x1B: random fraction/trig/equal-bound ranges | 30 |
| case 0x3F: fraction register and fused operand order | 2 |

The 0.5 local now emits lis/addi/lfs and shares the correct pool word, but its address/load is still scheduled after the Vec address and angle-input loads; retail forms the half address first and sums in f1 with inputs in f2/f1. The named-array approach adds a word ahead of the existing pool and does not fix this schedule. Several equal-bound randf expansions also save a computed range result and add an fmr, whereas retail saves the raw random fraction until after the next draw. Attempted inline/local/union/lifetime/propagation variants either failed to change these webs, regressed code, or changed pool order.

The brief's "last unmatched function" premise also differs from this checkout: ext_moamoa_init remains **99.38%** (7 source/destination-copy scheduling differences), and moamoa_hit_check remains **97.25%**. Both are unchanged, and must be resolved before this unit can link. The documented mwcc debugger is unavailable (`~/mwccdbg/mwcc-debugger/mwcc_debugger.py` missing).

Full final diff: `scratch/codex-eki/final.diff`; checksum build log: `scratch/codex-eki/final-ninja.log`; natural verification: `scratch/codex-eki/verify_natural.py`.
