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
