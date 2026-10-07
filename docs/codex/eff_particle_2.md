# eff_particle attempts (2026-10-07)

Limit: 35 build/compile experiments. Changes remain uncommitted.

1. Baseline: configure with cached tools; build and record all function scores, pools, order, and relocation values.

1 result: ParticleEmit 93.13%, effParticleDisp 99.22%, other three 100%; .rodata IDENTICAL, .sdata2 has 10 duplicates; text contains four extra helpers and wrong order; relocation checker 0. Static pool plan reviewed (no probe builds).

2. Remove all four pool helpers, run requested fn_order rewrite, and adapt to immediate retail emission over real functions.

2 result: all scores unchanged, retail order matches; .sdata2 unchanged (10 duplicates), .rodata missing the unused signed-conversion double. No real function has a signed integer-to-float conversion.

3. Preserve retail's unreferenced signed-conversion double as ordinary const data after the real functions, with force_active; no section placement.

3 result: baseline scores retained, retail order matches, .rodata IDENTICAL; .sdata2 retains the baseline 10 duplicates. Saved no-regression helper-free candidate.

4. Test native pooling for all scalar constants with immediate real-function emission; remove named scalar definitions and use their literal values.

4 result: ParticleEmit improves to 99.98% (only reversed classify scratch slots); other three stay 100%. Disp regresses to 96.46% because -8*0.5 folds into -4; pool missing 8. Not retained.

5. Keep Disp's nonfolding named prefix (4, 8, deg2rad, half) while pooling the rest natively. Negate the height before multiplying by half.

5 result: ParticleEmit 99.98%; Disp 98.33% (FP scheduling/register rotation), rejected. Three scalar duplicates, .rodata IDENTICAL.

6. Restore Disp's starting operand order within the named prefix; retain native ParticleEmit and other scalar pooling.

6 result: no regressions; ParticleEmit 99.98%, Disp 99.22%, others 100%. Scalar duplicates reduced from 10 to 3, .rodata IDENTICAL. Saved candidate.

7. Write both Disp half-size products as division by 2; pool half natively instead of defining it.

7 result: Disp 98.37%, rejected; other scores unchanged; two scalar duplicates.

8. Test all-literal Disp with height/half locals and match-only opt_propagation off to preserve the two size products.

8 result: Disp 93.17%, rejected; pool ORDER. Restored experiment 6.

9. Pass the sqrt input by address to the existing inline classifier, testing whether the reversed scratch slots come from its by-value parameters.

9 result: ParticleEmit 98.02%, rejected; passing by address hoists the scratch stores but does not fix their offsets.

10. Test a MUST_MATCH size-constant array to control Disp's temporary creation order, analogous to Entry's existing constant array.

10 result: Disp 98.33%, rejected; array is scalarized but temporary FPRs still rotate.

11. Materialize height and half as ordinary local reads before the size products, testing named-local web allocation.

11 result: identical to experiment 10 (Disp 98.33%); rejected.

12. Keep half as the first bottom operand but negate height as the second operand.

12 result: Disp 98.35%, rejected; reversing the multiplication operands does not fix the FPR order.

13. Expand both sqrt bodies under MUST_MATCH in ParticleEmit; retain the inline function calls naturally. Test whether nested inline expansion reverses classifier scratch slots.

13 result: ParticleEmit 96.61%, rejected; expanded returns prevent FPR coalescing and scratch slots remain reversed.

14. Test match-only opt_lifetimes off for ParticleEmit's two classifier scratch slots.

14 result: ParticleEmit 99.81%, rejected; lifetime optimization is needed and does not fix scratch offsets.

15. Reverse the left product operands while negating height before the bottom product, testing the shared half web's allocation order.

15 result: Disp 98.35%, rejected; product order changes are insufficient.

16. Test a MUST_MATCH float-width round trip at the inline classifier parameter, preserving its value while potentially changing scratch creation.

16 result: unchanged 99.98%; round trip removed.

17. Give the existing sqrt/classifier inline functions explicit scratch pointers under MUST_MATCH, with two caller-declared slots and stores only in the classification branch. Natural signatures unchanged; no new functions.

17 result: code unchanged (99.98%), but scratch slots now have explicit caller declarations.

18. Reverse the two scratch declarations to match retail's stack-slot order.

18 result: ParticleEmit reaches 100%; all other scores preserved, three scalar duplicates and .rodata IDENTICAL. Saved candidate.

19. Test native half pooling with negation outside the bottom division; this keeps height runtime while changing expression-web order.

19 result: Disp 97.09%, rejected; negation outside the division remains after multiplication.

20. Cache a volatile named half read in a match-only local shared by the two products.

20 result: Disp 98.33%, rejected; volatile half does not change allocation.

21. Reuse the existing named 4/deg2rad objects at all uses to remove their remaining scalar duplicates without changing the half expression.

21 result: pool improves to one duplicate but Entry 96.44% and Disp 97.63%; rejected because native literal reads are required for their load behavior.

22. Test match-only opt_common_subs off for Disp with the correct height negation.

22 result: Disp 98.37%, rejected; CSE changes the left temporary but height/half remain swapped.

23. Test an explicit float-width boundary on height before negation, under MUST_MATCH.

23 result: Disp 98.33%, rejected; round trip does not change temporary allocation.

24. Replace writable section-placed scalar prefix with ordinary const objects and MUST_MATCH ordinary non-const pointer reads, as in the documented linked units.

24 result: all retained scores and pool layouts preserved; scalar section-placement macro removed.

25. Replace writable section-placed vectors with ordinary const Vec objects and match-only nonfolding struct reads.

25 result: Main 96.47%, Entry 92.67%, rejected; aggregate zero copies fold despite non-const reads. Original vector declarations retained.

26. Test match-only opt_lifetimes off for Disp with height negated before multiplication.

26 result: Disp 98.33%, rejected. Restored experiment 24 (ParticleEmit 100%, Disp 99.22%, others 100%).

27. Final cleanup: replace stale comments, guard match-only pragmas and Disp negation shape, and remove the unneeded explicit_zero_data pragma around nonzero scalars. Rebuild and verify scores, pools, order, and relocation values.

27 result: PASS. Four functions 100%, Disp 99.22%; .rodata IDENTICAL, scalar duplicates 10 -> 3; retail order matches, relocation wrapper 0, diff check clean.

28. Compile final source without MUST_MATCH using the actual Ninja command; directly check this unit's partial relocation values and lint against its baseline.

28 result: natural compile PASS; direct partial relocation check has 0 findings. Lint initially reads the stale full-project report and reports ParticleEmit at its old 93.1%; rerun after report refresh.

29. Run the full default Ninja build and verify all 32 build.sha1 checksums with eff_particle still NonMatching. This checks project health, not a completed eff_particle link. Refresh the report for final lint.

29 result: stopped at an unnecessary binutils download (offline/DNS unavailable), although binutils is already cached. No source candidate changed.

30. Reconfigure with explicit cached binutils as well as the other cached tools, then complete the unchanged-candidate full build/checksum.

30 result: PASS. Full default Ninja build completed using cached tools; build.sha1 reports `32 files OK`. Fresh final scores: ParticleEmit 100% (starting 93.13%), effParticleDisp 99.22% unchanged, EmitterEmit/effParticleMain/effParticleEntry 100% unchanged. .rodata IDENTICAL, .sdata2 has only three duplicates (starting ten). Retail function order matches. Wrapper and direct partial relocation checks report 0 findings. Natural compile passed for the final source in experiment 28. Lint passes after refreshing the full report; git diff --check is clean.

Final: all four pool helpers removed, retail emission restored, ParticleEmit matched, no function regressed. No new helper or section-placement trick. The unused original signed-conversion constant is retained as ordinary const data because no real function naturally emits it. Disp remains partial, so configure.py stays NonMatching; the 32-file checksum verifies project health, not a completed eff_particle link. Thirty logged build/compile experiments, below the 35 cap. All changes remain uncommitted.


# Follow-up attempts (2026-10-07)

Limit: 30 build/compile experiments for this follow-up. Prior run above retained for history. Changes remain uncommitted.

1. Configure with cached tools and rebuild the baseline; inspect Disp instruction differences and the three duplicate constants.

1 result: baseline confirmed: ParticleEmit/Main/Entry/EmitterEmit 100%, Disp 99.22%; three .sdata2 duplicates (4, half, deg2rad), .rodata IDENTICAL.

2. Move scalar definitions after real functions, declare them before use, and use plain named reads throughout. Correct the bottom expression to negate height before multiplying. This tests nonfolding forward const references without pointer casts.

2 result: Entry and ParticleEmit remain 100%, but Disp regresses to 96.86%. Forward extern references cause MWCC to emit the four definitions into .sdata, so rejected.

3. Keep the four const definitions before functions, use direct const reads at every site (no non-const cast), and negate height for bottom.

3 result: direct reads fold the size product to -4 and create the same three duplicates; rejected (Disp 96.46%).

4. Represent each size constant as an ordinary one-element const array, reading element zero throughout. Test aggregate const reads that MWCC may keep without scalar folding; no section placement.

4 result: both pools IDENTICAL, Entry/ParticleEmit/Main/EmitterEmit stay 100%; Disp 96.86% (scratch FPR rotation and scheduling), so not yet retained.

5. Keep one-element const arrays for width/height; pool half and deg2rad natively as literals. Their sizes remain runtime reads, preventing bottom folding.

5 result: four matched functions unchanged, Disp 98.37%, but half/deg2rad pool order swapped. Rejected.

6. Use all four one-element const arrays; reproduce baseline Disp non-const size reads and negated-half operand order, keeping const element reads in Entry and ParticleEmit.

6 result: both pools IDENTICAL and four functions 100%, but Disp 97.74%; named deg2rad schedules early and size mismatch remains. Rejected.

7. Keep arrays for width/height/half, but retain baseline scalar deg2rad definition and literal reads. Test removal of two duplicates without Disp scheduling regressions.

7 result: retained no-regression candidate: four functions 100%, Disp 99.22%, duplicates 3 -> 1 (deg2rad only), .rodata IDENTICAL.

8. Use explicit named negative-width, negative-height, and half locals before the products in Disp, with correct height negation. Test scratch web creation through separate expressions.

8 result: Disp 98.33%, scratch FPRs rotate (negative width f1, height f0, half f2); rejected.

9. Form bottom before left, negating height in the first size product, to change frontend temporary creation order.

9 result: Disp 98.35%, changing product order fixes height f1 but leaves half f2 and negative width f0; rejected.

10. Disable propagation only for Disp while using the explicit negative-width/height/half locals from experiment 8, to retain their declared allocation identities.

10 result: Disp 95.70%; propagation affects many GPRs but leaves the same scratch rotation. Rejected.

11. Reuse left/bottom themselves for their negative size values, then multiply both in place by half. Test register coalescing with compound assignments.

11 result: Disp 98.33%, compound assignments coalesce the negatives into saved f28/f27, unlike retail scratch FPRs; rejected.

12. Remove the final deg2rad duplicate with a one-element const array, using a volatile element read only in Disp to test its load scheduling. Size expressions retain the best candidate form.

12 result: Disp 97.74%; volatile deg2rad read schedules exactly like the const array read. Rejected.

13. Test an explicit float-double-float conversion on the deg2rad array read to change frontend temporary creation without changing its value.

13 result: same 97.74% Disp code; conversion is eliminated without affecting scheduling. Rejected.

14. Put native half as a division reciprocal while loading 4/8/deg2rad from arrays. The division frontend lowering may select the original scratch FPRs.

14 result: pools IDENTICAL, four functions unchanged, Disp 96.90%. Division keeps negative width in f2 but swaps height/half f0/f1 and named deg2rad schedules early. Rejected.

15. Form bottom division before left division to change native reciprocal temporary creation order.

15 result: Disp 96.86%; bottom-first divides rotate all three scratch registers. Rejected.

16. Use a scalarized local Vec for negative width, negative height, and half; write half then height then width to test aggregate temporary ordering. Retain baseline deg2rad literal scheduling.

16 result: Disp 98.33%, scalarized Vec produces the same three scratch webs as explicit locals. Rejected.

17. Keep width/height arrays, remove named deg2rad/half, and initialize deg2rad before size products so its native pool entry precedes half. Test the frontend literal order as well as scratch allocation.

17 result: Disp 98.37%, and pool order still half before deg2rad (backend emission order governs native pool slots). Rejected.

18. Test deferred code generation with reversed real-function source order and forward scalar constants defined before the deferred functions emit. This may preserve runtime named reads while letting MWCC recognize readonly definitions at code generation.

18 result: .sdata2 IDENTICAL, retail function order matches, but deferred inlining regresses Main to 48.36%, ParticleEmit 99.97%, Disp 96.86%, and changes .rodata order. Rejected.

19. Disable dead-assignment elimination only for Disp with the explicit size locals from experiment 8, testing whether their web identities can survive without disabling propagation.

19 result: Disp 94.95%, dead-assignment pragma changes many GPRs and conversion scratch slots without fixing size FPRs; rejected.

20. Simplify the retained candidate to ordinary const array reads for size operands, retaining negated-half bottom and literal deg2rad. Test whether non-const pointer reads are still necessary.

20 result: retained: same scores and one duplicate, with non-const size pointer casts eliminated.

21. Express negative height as zero minus height, preserving its fixed positive-value behavior while testing a different frontend negation lowering.

21 result: zero-minus-height canonicalizes to unary negation early; identical 98.33% scratch rotation. Rejected.

22. Negate height as a double and explicitly round back to float before multiplication. Test whether a redundant precision boundary survives far enough to change scratch coalescing.

22 result: Disp 95.50%; round-back remains as an extra frsp and changes load scheduling. Rejected.

23. Read positive height before width, then form the two negative products from explicit locals. Test whether the earlier height read changes its CSE web rank while preserving product order.

23 result: Disp 98.33%, earlier height assignment is propagated into the same CSE web. Rejected.

24. Test Disp-only opt_strength_reduction off with the correct height-negation form, the remaining local optimizer lever that might alter these temporary webs without losing propagation.

24 result: unchanged 98.33% Disp scratch rotation; rejected.

25. Restore the no-regression candidate and guard the singleton-array representation under MUST_MATCH, with natural scalar constants under #else. Reuse the existing constant-read macro at every width/height/half site; update comments and verify scores, pools, order, and relocation values.

25 result: retained candidate passes: ParticleEmit/Entry/Main/EmitterEmit 100%, Disp 99.22%, .sdata2 duplicates 3 -> 1 (deg2rad), .rodata IDENTICAL, retail order matches, relocation wrapper reports 0 findings, diff check clean. No new helper or section directive.

26. Compile the retained unit with MUST_MATCH removed using its actual Ninja compiler command; check direct relocation values and lint against the starting source using the fresh report.

26 result: natural compile PASS; direct partial relocation check 0 findings; fresh-report lint PASS (no output).

27. Temporarily mark eff_particle Matching, configure with cached tools, run full Ninja and the 32-file build.sha1 check. Retain Matching only if the checksum passes.

27 result: full Matching link builds but SHA-1 FAILS for MarioSt.dol; other 31 files OK. Matching rejected. Disp remains 99.22% and deg2rad remains duplicated, so no completed eff_particle link is claimed.

28. Restore eff_particle NonMatching, rebuild default project, verify all 32 checksums, refresh final scores, pools, order, relocation values, and diff checks.

28 result: PASS. Default full Ninja build completed and build.sha1 reports `32 files OK` after restoring NonMatching. Final scores: ParticleEmit 100%, EmitterEmit 100%, effParticleMain 100%, effParticleEntry 100%, effParticleDisp 99.22% (all unchanged). .sdata2 DUP_OURS reduced from 3 to 1; remaining duplicate is deg2rad (Disp and Entry). .rodata IDENTICAL; retail function order matches; wrapper and direct partial relocation checks have 0 findings; natural compile and fresh-report lint pass; git diff --check clean.

Follow-up summary: retained ordinary singleton const arrays under MUST_MATCH for 4/8/half, natural scalar definitions under #else, and shared width/half reads in Entry and ParticleEmit. Removed the previous non-const scalar pointer reads. No helper functions or section directives added. All-pool-IDENTICAL candidates were rejected because Disp regressed. Disp was not matched to 100% in this run; the attempted Matching link failed MarioSt.dol SHA-1 and was reverted. Default 32-file checksum proves project health with this unit still NonMatching. 28 logged build/compile experiments for this follow-up, within the 30 limit. Changes remain uncommitted.
