# win_root pool attempts (2026-10-07)

Limit: 35 build experiments. Changes remain uncommitted.

1. Baseline build, record every function score, pool differences, function order and relocation findings. Configure with cached local tools.

1 result: 53/65 functions at 100%; baseline partial scores recorded. Existing function order DOES NOT match retail (contrary to the task's assumption). `.sdata2` missing 73 values, `.rodata` missing 8 plus missing duplicate templates. The requested relocation command's extra `0` filters out all units; also run the meaningful command without it at completion. Static pool plan read; no probe builds used.

2. Reorder the real functions into reverse retail order using fn_order.py, keeping existing bodies and pool shapes; establish retail .text order before porting templates.

2 result: stock reorder refused macro/data regions before compiling. Manual reorder of real bodies, with shared inline definitions above the functions and the kirinuki pragma pair retained on its function, builds with every score unchanged. Retail function order now matches.

3. Native .rodata port: pool_data on; strings/doubles at uses; individually named static Vec templates for root/hako/sort at their source positions; complete the real sort descriptor tables to supply their string literals in retail parse order. Retain scalar float spellings for this experiment.

3 result: compile error: sort_data needs its forward declaration because winSortMain precedes its definition. Direct ninja diagnostic confirmed this, despite agent_check misreporting a timeout.

4. Restore sort_data's forward declaration and build the native .rodata candidate.

4 result: builds; Hako 89.27 -> 91.26, Book 90.68 -> 91.57. RootDisp and HalfBook regress, so not accepted. rodata has every previously missing value, but tbl's const pointer array incorrectly occupies .rodata and one extra zero Vec remains.

5. Make the enemy tag pointer table writable as in retail .data; read root's named templates through a match-only mutable view to stop loop invariant hoisting.

5 result: `.rodata` matches through +0x598, where 320/-16 doubles are swapped; only trailing alignment differs afterward. Waza reaches 100%; other pool-addressing functions improve. Mutable root views do not prevent hoisting; Root/Half remain below baseline. sdata2_diff's DUP_OURS zero Vec is a classifier artifact (its symbol list undercounts the original); raw template bytes and counts match.

6. Guard root templates as const volatile and preserve volatile access in root_vec; swap HalfBook's 320/-16 first uses and test opt_propagation off for its native double locals.

6 result: `.rodata` IDENTICAL (normal trailing alignment); root volatile views recover its frame but schedule copies differently (96.66 vs 97.74). HalfBook opt_propagation off worsens its score to 95.14. Reject these code shapes.

7. Use ordinary root templates with opt_loop_invariants off on RootDisp instead of volatile reads; remove HalfBook's propagation pragma and make its seven double span locals f32, matching the values passed to PSMTXScale.

7 result: RootDisp loop hoisting is unchanged; HalfBook f32 spans regress to 93.62. Reject.

8. Keep four named root tail templates, use compound literal templates for the tab loop's two vectors, and reuse the existing by-value winSortMkVec helper. Restore HalfBook's f64 spans and reorder its local declarations to match the literal load webs.

8 result: root native loop gets 97.64 (only 0.10 below baseline), Half 95.25. Both pools remain as attempt 6. Not accepted yet.

9. Test all scalar float literals and port the named GXColor prefix in retail order, with the tag pointer table between its two colour groups. Replace Mail/Book native colour templates with their corresponding named objects to avoid duplicates; preserve BgGX's necessary volatile named FIFO reads.

9 result: compile error from winZClear's existing volatile per-store reads after replacing their named lvalues with literals. Direct ninja diagnostic repeated the error.

10. Retain named lvalues at every existing volatile scalar read, as well as BgGX, and rebuild the all-literal candidate.

10 result: colour/tag prefix bytes match exactly; missing .sdata2 values drop from 73 to 10. Several float-literal functions regress and constant fabs gains four unwanted double values; reject the all-literal float forms.

11. Retain the colour/tag prefix port but restore the tuned scalar float spellings from attempt 8. Use root_vec with an addressed native compound literal for the loop. Restore HalfBook's baseline declaration order and test scalar compound-literal double loads, which keep a real value object without base offsets.

11 result: colours-only fills 21 missing sdata2 values, but replacing Mail's native colour templates causes hoisting and regresses it. Scalar double compound literals duplicate seven doubles; reject. Root still 97.64.

12. Restore Mail's native colour templates; move the final capture colour declaration after Mail and omit the named Mail-colour duplicates. Explicitly read the tab-loop x/y locals before root_vec. HalfBook: assign native span literals to locals, then fabs those locals with opt_propagation off.

12 result: Mail is restored above baseline (99.25); .rodata IDENTICAL and no duplicates. Explicit root x/y reads worsen scheduling; split Half spans worsen allocation. Reject those two shapes.

13. Restore attempt 8 Root's loop shape; move its background-colour initialization after the early return. Restore native Half double expressions/declarations and test reversed assignment order for its span preload block. Define only the 52 missing scalar objects after the code, preserving existing literal entries.

13 result: `.sdata2` improves from CONTENT to ORDER with no missing/extra/duplicate values; `.rodata` IDENTICAL. Moving root's colour init and reversing Half's assignment block worsen both functions. Keep the scalar definitions, reject those body experiments.

14. Root: ordinary six-entry const Vec array with a local mutable pointer view (real array indexing, no unrelated-symbol offsets). Half: restore original native declaration/assignment shape with 320 before -16 and test opt_lifetimes off.

14 result: named array still hoists Root's loop templates; opt_lifetimes off does not help Half. Reject.

15. Return to Root's native loop and reverse root_vec's x/y formal argument order (with its callers adjusted), to restore the target's coordinate-load order. Half: restore f64 span locals and try float absolute-value inputs for the shared spans, retaining native double loads for 320/-16.

15 result: RootDisp returns exactly to 97.74 using the adjusted argument evaluation order. Half's fabsf inputs regress; reject those. All other functions retain or improve starting scores; pools/order requirements are satisfied apart from Half's score.

16. Restore Half's native double fabs inputs and use the retail named f32 objects for its coordinate literals, preserving the original nonfolding coordinate computations.

16 result: Half's frame now matches, but naming every coordinate changes many FPR webs (94.17); four literal-only values disappear from .sdata2. Reject.

17. Restore native Half spans and coordinate shape; name only its 320.0f coordinate coefficient to reduce the extra live literal web, defining the object after the code.

17 result: naming only 320 does not resolve the extra Half frame spill (95.01); reject.

18. Restore native Half double span expressions; declare winHalfBookPage's span parameters f32, reflecting the helper's immediate casts to f32 for scaling. This should let the caller end the double live ranges before its page calls.

18 result: narrower helper parameters do not eliminate the extra Half spill (95.12); reject.

19. Keep native span loads but inline the three single-use absolute values (24, 40, 48) at their page calls rather than giving them persistent named locals. Their pool entries already exist in earlier real functions.

19 result: single-use fabs expressions worsen Half's allocation (93.80). Reject.

20. Group Half's span locals in a scalarizable local struct, assigning them in retail load order (24, 320, 240, 40, -16, 48, 248, 8, 16) so first-write ordering can reproduce the original temporary ranks.

20 result: local struct changes allocation but still spills an extra double (94.79). Reject.

21. Restore the original native span locals and narrow only the single-use 48 span to f32, testing whether one shorter live range eliminates the extra aligned stack allocation.

21 result: the frame matches after narrowing 48, but span register allocation worsens (94.51). Reject.

22. Narrow only the 24 span instead, retaining the other native double locals.

22 result: the frame matches; Half recovers to 95.25, still below 95.62. Reject as final.

23. Narrow only the 40 span, retaining the native double loads and all other original span types.

23 result: narrowing 40 gives 94.65; reject.

24. Narrow only the 320 span, which is consumed once by the first page's f32 scale calculation.

24 result: narrowing 320 gives 94.69; reject.

25. Regression-preserving fallback: restore Half's existing match-only double views except the two unique 320/-16 entries, which remain native literals in retail order. This tests how much of the natural port can be retained within the no-regression limit.

25 result: mixed native/view loads worsen Half's allocation (93.96). Reject.

26. Restore Half's complete starting body and existing match-only views. Keep the natural pool port in the other functions and all scalar/color/table work. This prioritizes the explicit no-regression constraint; the two unique Half doubles may remain unported if a natural shape cannot retain its score.

26 result: every function retains or improves its starting score; eight improve and Waza reaches 100%. `.sdata2` ORDER with all values present, `.rodata` missing only 320/-16 (versus eight missing values plus template gaps initially), no new EXTRA/DUP_OURS, retail function order matches. Half's existing views remain; both pools are still partial, not link-ready.

27. Clean stale match comments, document the retained Half limitation, guard Root's argument-order shaping with MUST_MATCH, and verify all 65 scores against baseline.

27 result: 65/65 scores are >= baseline, eight improve, 54/65 at 100%. Function order matches; requested and meaningful relocation checks both report 0 findings; diff --check clean. No new EXTRA/DUP_OURS. The requested relocation tool reports the number of units WITH findings, so '0 units' is not an empty-audit indicator; the earlier interpretation in attempt 1 was incorrect.

28. Compile final win_root without MUST_MATCH using the exact cached-tool build command into a temporary directory.

28 result: natural compile PASS.

29. Run the complete all_source build with the retained final candidate.

29 result: complete all_source build PASS (1230 build steps; existing unrelated warnings).

30. Run the default project build after the complete source build.

30 result: default project build PASS. Final natural compile, full source build, function order, relocation checks, pool checks, and whitespace checks pass. Final score audit: zero regressions, eight improvements, 54/65 functions at 100%.

Experiment accounting: 30 numbered build/validation experiments, plus two direct diagnostic repeat compilations and the pool-plan report build = 33 conservatively counted build invocations, below the 35 limit. No probe sweep was run.

Retained limitations: .sdata2 ORDER (all values present); .rodata missing only HalfBook 320/-16 doubles. Both remain partial. Leave changes uncommitted.
