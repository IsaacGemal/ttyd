# mot_jabara attempts (2026-10-07, Codex)

Limit: 30 build experiments; changes remain uncommitted. Cached tools are used for configuration.

1. Build the unchanged baseline and record scores/pools.

1 result: 7/8 at 100%; mot_jabara 98.86%; .rodata IDENTICAL; .sdata2 has 51 duplicate entries. Function order matches. Static pool plan inspected without probe builds.

2. Move small-data definitions after deferred functions and replace mot_jabara literals with their named objects.

2 result: both pools IDENTICAL; swingMain regressed to 99.12, mot_jabara to 98.70. Rejected as final.

3. Define swingMain’s seven leading constants before code; prevent folding of those objects in mot_jabara with match-only candidate nonconst reads.

3 result: swingMain restored to 100%, mot_jabara 98.70%; three duplicate pool words remain. Rejected.

4. Rebuild experiment 2 to inspect swingMain register differences (no new shape).
4 result: reproduces 99.12% swingMain; clamps allocate named constants to f2 instead of the literal f0.

5. Emit real functions immediately in retail order; let swingMain pool its seven leading scalars natively, then define the remaining named objects before later functions.

5 result: rejected (swingMain 97.84%, marioChkJabara 99.90%, mot_jabara 98.77%; fabs constant became double).

6. Return to identical-pool candidate; explicitly bind swingMain clamp bounds after fetching JabaraWork and before arithmetic.

6 result: swingMain 99.10%, mot_jabara 98.70%; pools IDENTICAL. Rejected.

7. Load each swingMain clamp bound through MATCH_F32 inside its comparison and reuse that local for the clamped store.

7 result: swingMain unchanged at 99.12%; pools IDENTICAL. Rejected.

8. Return to immediate literal-prefix candidate and compare fabs as f32, preserving native float pooling.

8 result: both pools IDENTICAL. swingMain has four extra frsp instructions (99.09); marioChkJabara 99.90; main 98.78.

9. Use fabsf for native float comparisons, use the sensitivity’s truth test, and address static const Vec templates directly to share the rodata base.

9 result: swingMain and marioChkJabara restored to 100%; direct Vec reads regress main to 94.55 and post to 88.83 (compiler folds zero templates). Pools IDENTICAL.

10. Read the static Vec templates as volatile aggregate copies to retain their runtime load/store sequence.

10 result: rejected; volatile aggregate copies regress main/post to 94.41/83.40.

11. Use nonfolding nonconst Vec reads instead of volatile aggregate copies.

11 result: rejected; nonconst direct template reads still allow hoisting (94.55/88.83).

12. Restore baseline Vec-pointer shapes with the identical pools and matched swing/Chk; move var_f28 before other float locals to test saved-FPR allocation.

12 result: main 98.76% (below baseline); others 100%, pools IDENTICAL. Rejected.

13. Keep only the fifteen-word prefix (seven natively pooled by swingMain, eight named before post/main); let main and Chk pool the suffix floats and short strings natively.

13 result: compile failed, not a hang; replacing a constant left MATCH_F32(2.0f), which takes a literal address.

14. Direct ninja diagnostic confirms that compile error.

15. Remove the remaining MATCH_F32 wrappers around suffix literals and rebuild.

15 result: main 98.24%, others 100%; small-data suffix has 1/M_W_6 reversed. Rejected.

16. Restore identical-pool named-main candidate; enable common-subexpression optimization to test rodata-base copy coalescing.

16 result: main 98.71%, others 100%; pools IDENTICAL. Rejected.

17. Keep post’s original pointer shape; explicitly sequence main’s template copies as two u32 loads/two stores/last load-store under MUST_MATCH, with natural Vec assignment otherwise.

17 result: rejected; scalar copy macro adds address temporaries and changes scheduling (main 94.76%).

18. Restore pointer-based template reads; swap sp5C/sp50 declarations to match their addressed stack-slot order.

18 result: main unchanged at 98.78%; stack slot coalescing ignores declaration swap. Rejected as final.

19. Use volatile named scalar reads throughout main to test whether literal-like point-of-use loads restore the retail allocation.

19 result: main 97.19%; others 100%, pools IDENTICAL. Rejected.

20. Retain pointer indexing but give all four templates static const linkage to test whether their base coalesces with the rodata pool base.

20 result: main unchanged at 98.78%, others 100%; pools IDENTICAL.

21. Reverse the four main float-local declarations to test the remaining saved-FPR permutation.

21 result: no score change; declaration reversal has no effect.

22. Express the cyclic phase-byte update and signed clamp as one assignment/test expression to test r0/r3 ordering.

22 result: main 98.24%; rejected.

23. Pass Mario’s position directly to the by-value camera-collision call instead of copying it into a named local first.

23 result: no score change.

24. Disable lifetime optimization for main to test distinct aggregate stack slots.

24 result: main 98.75%; rejected.

25. Restore the identical-pool candidate and use the main sensitivity truth test to retain the original variable-first zero comparison.

25 result: both pools IDENTICAL; main restored to baseline 98.86%; all seven others retain 100%. Function order matches. This is the retained candidate.

26. Clean comments, guard nonfolding scalar reads and the necessary optimizer pragma under MUST_MATCH, restore the ineffective declaration permutations, and verify scores/pools.

26 result: all scores unchanged; both pools IDENTICAL; function order matches. Removed one whitespace issue found by diff --check.

27. Compile the final source without MUST_MATCH using the configured compiler command.

27 result: natural compile PASS.

28. Flip mot_jabara to Matching, configure with cached tools, run full ninja and the requested retail checksum verification.

28 result: the Matching link built its outputs, but checksum verification failed: only 1 file OK, 31 mismatches. Main is still 98.86% and its instruction/frame/register differences prevent a retail link. The Matching flag is reverted.

29. Configure NonMatching again, run full ninja, and verify all 32 retail checksums.

29 result: normal full ninja PASS with NonMatching restored.

30. Final score/pool/order/relocation/store checks, raw pool-byte comparison, retail checksums, and diff whitespace verification (unchanged source).

30 result: PASS. Normal full build and retail verification print `32 files OK`; scores are unchanged
(7/8 at 100%, mot_jabara 98.86%); both pools IDENTICAL and their raw bytes/sizes match (rodata 184,
sdata2 248). Function order matches, partial relocation-value check and direct check_unit have zero
findings, store sweep has zero findings, natural compilation passes, and git diff --check is clean.
Instruction delta is the pre-existing -5 in main (-4 stw, -4 lwz, +1 stmw, +1 addi).

Final: pools fixed without function regression. Main remains at its starting 98.86%, so function/link
completion is unfinished. Matching was reverted after failed checksums. No helpers or new section
tricks were retained. Source, log, and ledger changes remain uncommitted. All 30 experiments used.
