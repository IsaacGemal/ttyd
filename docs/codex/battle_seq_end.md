# battle/battle_seq_end pool attempts

1. Baseline: `agent_check.py` builds, 31/35 functions at 100%; partials `_MarioExpDisp` 99.27, `btlseqEnd_DispMain` 98.87, `_LvupParamConfirmDisp` 85.46, `_lvup_select_object_disp` 98.09. `.sdata2` CONTENT, DUP_OURS 57; `.rodata` IDENTICAL. Function-order check finds existing stray empty `_ac_help_disp` (belongs to another unit).
2. Replace scalar literals and colour copies with their retail named objects; retain constant Vec templates; remove the stray empty body. Pending validation.
   Result: both pools IDENTICAL, but six former matches regress and ConfirmDisp drops to 77.67; reject as final. Named const loads hoist relative to literal templates, zero gains a saved FPR, and ConfirmDisp loses common arithmetic.
3. Use MUST_MATCH-style volatile reads for six colour copies and point-of-use zero reads in Main/Help/Confirm. Pending validation.
   Result: spot_off restored to 100; other regressions remain. Both pools remain IDENTICAL. Reject as final.
4. Restore plain named objects, use typed colours for Disp/Move, scalar volatile colour stores for Init, localise Main/Help zero loads, and share Confirm's half-width explicitly. Pending validation.
   Result: Help restored to 100, Main 99.94 (zero in f28 instead of f31), Confirm 85.04. Both pools IDENTICAL; reject as final.
5. Order Init flag/colour volatile stores, preload Disp/Move float locals, block-scope Main zero, keep successful spot_off volatile copy, preload Confirm coefficients. Pending validation.
   Result: Init and spot_off at 100; Disp/Move/Main unchanged; Confirm preloads regress to 74.54. Both pools IDENTICAL.
6. Restore Confirm attempt 4, pass Disp colour directly, sequence Move volatile float/colour loads, use scalarised Vec for Main zero. Pending validation.
   Result: Disp 87.35, Move 98.62, Main 99.94, Confirm 85.04. Both pools IDENTICAL. Reject as final.
7. Restore native colour templates and interleave the remaining named prefix objects between the real functions, in the templates' parse order. Retain attempt 4 float arithmetic. Pending validation.
   Result: Disp/Move at 100, both pools IDENTICAL. Init restoration accidentally retained a volatile scalar view, causing stack copies; Main/Confirm remain below baseline.
8. Restore Init's exact baseline body; try Main opt_lifetimes off; share Confirm's converted length in a local. Pending validation.
   Result: Init restored to 100; Main pragma regresses to 98.36, converted-length local regresses Confirm to 82.35. Both pools IDENTICAL.
9. Restore best shapes, use Main zero directly at the final-loop call, scalarise Confirm's work pointer in a local struct. Pending validation.
   Result: compile error from a statement preceding declarations (agent_check incorrectly calls it a timeout).
10. Diagnostic direct ninja confirms expression syntax error, not a compiler hang; no new source experiment.
11. Move Confirm struct assignment below all declarations. Pending validation.
   Result: Main restored to 100 with direct named zero at use; all remaining functions at starting scores except Confirm (85.04 vs 85.46). Both pools IDENTICAL. Struct pointer did not change allocation.
12. Assign Confirm's shared half-width within the first call's x argument, preserving its expression-tree placement; restore plain work pointer. Pending validation.
   Result: both `.sdata2` and `.rodata` IDENTICAL; all 35 functions retain starting scores (31 at 100%). Function order matches and partial relocation value check has 0 findings.
13. Clean declaration grouping/comments and guard point-of-use volatile zero reads, the expression-tree half-width assignment, and force_active pragmas under MUST_MATCH. Pending final validation.
   Result: all 35 scores unchanged, both pools IDENTICAL, function order matches, 0 relocation findings. Natural compile without MUST_MATCH passes; all_source and default ninja builds pass. Raw bytes/offsets match with each original section's four-byte tail supplied by its existing eight-byte linker alignment. Store sweep: 0; instruction delta: pre-existing +1 mr in Confirm only.
14. Remove force_active scopes from referenced intermediate objects; only unreferenced suffix words need retention. Pending validation.
   Result: both pools IDENTICAL, all 35 scores exactly unchanged, order matches, 0 relocation findings. Final natural compile passes. Intermediate force_active scopes were unnecessary and are removed. Final build/report verification below uses the same source, not another source experiment.
