# evt_shop attempts (2026-10-06)

Limit: 35 build experiments. Changes remain uncommitted.

1. Baseline: configure with cached local tools, build evt_shop, and record function scores, pools, function order, and relocation values.

1 result: baseline 32/33 functions at 100%; list_disp 99.07%. Both pools IDENTICAL. Function order differs and contains pool_i2f_mask. Relocation checker: 0 findings. Baseline source and report saved in /tmp.

2. Remove pool_i2f_mask, run fn_order.py --write, then adapt its deferred order to immediate retail order. Put existing named pool data before real functions and preserve scalar reads through a MUST_MATCH-only ordinary f32 pointer macro. Enable defer_codegen off over real functions so list_disp supplies the native unsigned conversion before the signed one.

2 result: step 1 PASS. 32/33 functions at 100%, list_disp unchanged at 99.07%; both pools IDENTICAL; retail function order matches; helper removed with no new helper or section trick.

3. list_disp: initialize i immediately after pouchGetPtr under MUST_MATCH and group the second heading/price x arithmetic as entry x plus the relative offset.

3 result: rejected (93.39%). Early i initialization changes all callee-saved GPRs; regrouping is reassociated differently by MWCC. Restored step 1.

4. Keep i at its original source position; reverse the summand order for the second heading and price coordinates.

4 result: rejected (96.52%); reversed summands change scheduling and still fold the -90 after the entry-x addition. Restored step 1.

5. Name the scale factor after the unsigned width conversion and use factor * w to request retail's fnmsubs operand order.

5 result: rejected (97.97%). Correct arithmetic operand order but the factor/magic FPR allocation swaps. Restored step 1.

6. Initialize i before the first msgSearch, with i declared after all other locals to test its allocator priority independently of the coordinate expressions.

6 result: rejected (98.90%): i still claims r31, shifting other GPRs despite late declaration. Restored step 1.

7. Initialize i just after the first msgSearch, allowing scheduling to move the load while shortening its source-level live range.

7 result: no score gain (99.07%). GPR allocation is correct, but li remains after msgSearch. Saved for comparison, restored step 1.

8. Establish msg as the heading pointer before i initialization, then call msgSearch(msg), testing whether the message web keeps retail's allocation with initialization before the call.

8 result: rejected (98.90%), early i again takes r31. Restored step 1.

9. Express heading and price relative coordinates with named s32 statement temporaries under MUST_MATCH, preserving the ordinary expressions in the natural build.

9 result: rejected (95.55%). Statement temporaries change register/scheduling choices and still reassociate integer coordinates. Restored step 1.

10. Write the scaled width as (-w) * factor + half to test fused-operation operand canonicalization without changing first conversion use.

10 result: rejected (94.54%): emits fneg/fmadds and changes conversion scheduling. Restored step 1.

11. Multiply the width temporary in a separate statement before subtracting it from the integer half-width.

11 result: compile error, not a hang (checker mislabels compiler errors). C89 rejects a declaration after the multiplication statement.

12. Direct ninja diagnostic of experiment 11. Confirmed expression syntax error at the late f32 x declaration; no object produced.

13. Correct experiment 11 for C89 by declaring x before the multiplication.

13 result: rejected (94.57%). Separate multiplication prevents fusion and changes conversion scheduling. Restored step 1.

14. Put factor first, keeping width first in a folded multiplication by 1.0f to test expression-web creation order.

14 result: rejected (97.97%). Same FPR swap as factor-first; both pools remain IDENTICAL. Restored step 1.

15. Disable propagation only for list_disp to test whether the late integer sum webs and scale operand canonicalization come from this pass.

15 result: rejected (97.42%). Disabling propagation alters induction-variable lowering; remaining original differences persist. Restored step 1.

16. Disable common subexpression elimination only for list_disp to test its arithmetic canonicalization and invariant allocation.

16 result: rejected (96.96%). Disabling CSE changes frame/GPR allocation and leaves the arithmetic differences. Restored step 1.

17. Initialize i early and disable lifetime optimization for list_disp to test whether web splitting explains the GPR rotation.

17 result: rejected (95.72%). Lifetime optimization is needed for the original frame; disabling it enlarges the frame and changes GPR allocation. Restored step 1.

18. Express heading and price x sums as entry x minus the negated relative offset to test integer canonicalization through subtraction.

18 result: rejected (97.05%). Subtraction remains subfic/subf and changes header scheduling. Restored step 1.

19. Test a signed-width round trip on the relative coordinate expressions, which can keep expression webs distinct until lowering while folding out of the emitted code.

19 result: rejected (93.00%). Signed-width round trips alter scheduling but do not produce the needed intermediate registers. Restored step 1.

20. Initialize i early and apply the documented signed-width parameter-save shape at entry's first use, testing whether entry can retain r31 while the index initializes before msgSearch.

20 result: rejected (98.90%), same GPR rotation as experiment 6. Restored step 1.

21. Try an unsigned row index initialized early, retaining signed coordinate arithmetic with explicit casts.

21 result: rejected (97.88%). Signed coordinate casts produce an additional induction variable and GPR allocation changes. Restored step 1.

22. Try explicitly unsigned additions for the two relative-coordinate sums, preserving the final s32 argument type.

22 result: rejected (97.35%). Unsigned additions alter grouping/scheduling but leave the problematic price register. Restored step 1.

23. Materialize the unsigned width as f64 and explicitly round it to f32 at a factor-first multiply, testing conversion-web creation order.

23 result: rejected (93.45%). f64 materialization adds rounding and changes scheduling; factor/magic FPRs still swap. Restored the successful helper-free step 1 source. No list_disp candidate improved its starting score. Retained NonMatching because list_disp remains below 100%.

24. Compile the final helper-free source without MUST_MATCH using the actual Ninja compiler command into a temporary directory. Correct the stale list_disp comment to name the measured remaining differences.

24 result: natural compile PASS.

25. Rebuild the final evt_shop source and verify all function scores, both pools, retail function order, and relocation values.

25 result: PASS. 32/33 functions at 100%, list_disp 99.07% (starting score); .sdata2/.rodata IDENTICAL; function order matches; relocation-value checker reports 0 findings; git diff --check clean. No new helper or section-placement trick.

26. Run the full project build with existing cached-tool configuration and verify build.sha1. This checks project health with evt_shop still NonMatching; it does not claim evt_shop links.

Additional read-only verification: direct tools/reloc_value_check.py check_unit on evt_shop's original and built objects reports 0 findings, including partial list_disp. This confirms the unit itself independently of the report-driven wrapper. Whitespace-only cleanup follows the final source verification.

26 result: full Ninja build PASS and build.sha1 reports 32 files OK. evt_shop remains NonMatching, so this verifies project health rather than a completed evt_shop link.

27. Final rebuild after whitespace-only source cleanup; recheck function scores, pool identity, function order, and relocation values. Read-only fn_order.py assumes deferred source order regardless of pragmas, so its source suggestion remains reversed; fn_order_check verifies the actual emitted retail order.

27 result: PASS. Final scores equal baseline (32/33 at 100%; list_disp 99.07%); both pools IDENTICAL; retail order matches; evt_shop no longer appears in fn_order.py --all; relocation checker 0; diff check clean.

Final: main goal completed. pool_i2f_mask removed, native conversion pooling and retail function order preserved, no score regressions, natural compile passes, full project build/checksum passes (32 files OK). list_disp did not reach 100%, so no Matching flip or evt_shop link claim. 27 build/compile experiments, below the 35 cap. All changes uncommitted.
