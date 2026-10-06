# winmgr link attempts (2026-10-06)

Limit: 35 build experiments. Changes remain uncommitted.

1. Baseline build and function/pool checks. Configure uses the cached local tool paths to avoid download rules.

1 result: 37/37 functions score 100% with current objdiff; .rodata IDENTICAL; .sdata2 lacks 25 entries; function order differs and includes two pool helpers plus get10. The .data descriptor arrays and table are incomplete. Starting scores are recorded in /tmp/winmgr baseline.

2. Remove the pool helpers, emit real functions immediately in retail order, make get10 inline, complete descriptor data, and order const small-data objects. Preserve named scalar reads through a match-only non-const cast; native conversions supply the two .rodata doubles between message groups.

2 result: compile failed because the label tables still used char* for the newly const strings (the checker incorrectly labeled this a timeout).

3. Direct Ninja diagnostic repeated the same compile failure; no hang occurred.

4. Make string pointer tables const-correct and rebuild the immediate-function candidate.

4 result: function order matches, all missing small-data values are present, 33/37 score 100%. Fourteen floats duplicate their literal equivalents. The unsigned double first pools in select_disp_party, so the last message group was defined too early. winMgrSelectEntry loses inlining of later winMgrEntry/winMgrSetParam bodies.

5. Use native float literals except the named 0.5/-144 loads, interleave their definitions, move the final message group after select_disp_party, and express the two original inlined calls directly under MUST_MATCH.

5 result: 36/37 functions; both .rodata entries and offsets IDENTICAL. 0.5 was classified as writable because its address was cast before the definition. winMgrSelectEntry differs only in the manually expressed free-entry loop.

6. Read the extern 0.5 normally before its definition, retain cast reads afterward; use the exact winMgrEntry for-loop shape in the inlined body, and initialize the zero const color normally.

6 result: 36/37; winMgrSelectEntry now differs in eight registers, with identical loop instructions. The compiler still puts the forward 0.5 in writable small data. The zero color needs explicit_zero_data off to enter .sbss2.

7. Pool 0.5 natively, use ordinary zero-data suppression for the color, and move the direct inline's descriptor temporary after the callee locals.

7 result: rejected: literal 0.5 commutes FPR operands in four display functions, and swapping direct-inline declarations worsens registers. The named initialized zero is still .sdata2 despite explicit_zero_data off.

8. Use static const forward declaration for 0.5, a native zero color aggregate in the existing display inline (so the compiler supplies its zero template), and direct-inline declaration order descriptor/index/entry.

8 result: native zero aggregate removed the duplicate and emits the intended .sbss2 template. Static forward declaration still makes 0.5 writable. Direct-inline declaration permutation rejected.

9. Define the complete six-entry float prefix before its readers, load it through nonfolding named reads, and try register declarations in the direct inline.

9 result: rejected: named prefix reads change the literal CSE/FPR behavior (32/37); pools are ORDER with only 170/%d swapped.

10. Compile one small standalone diagnostic with the same flags to inspect native deferred-function flush order and forward const scalar/array/volatile placement; no repository source changes.

10 result: toggling deferred generation produces saved functions between immediate groups. A deferred forward const scalar stays readonly. This suggests preserving the later definitions for the allocator's original inlining.

11. Probe immediate forward const scalar/array/volatile reads to distinguish code-generation storage inference from declaration type.

11 result: all immediate forward constants become writable small data, regardless of scalar/array/volatile type. A definition must precede immediate generation.

12. Queue the real middle functions in reverse retail order before immediate display functions, then flush at their retail position, preserving cross-function inlining. Try a native half literal held in a nonconst local and read through its address.

12 result: rewrite script stopped before changing source or compiling (incorrect return-type search); score command reused experiment 9's object.

13. Correct rewrite script and build the queued-real-function candidate.

13 result: compile failed before emission: queued functions reference the later message definitions without forward declarations. The checker again falsely labels any compiler error as a timeout.

14. Add ordinary forward declarations for later message objects and compile the queued-real-function candidate.

14 result: allocator returns to 100%, but saved functions flush immediately AFTER the first immediate function. Local half cast requires a stack slot and is rejected. Three forward strings and -144 were also inferred writable.

15. Queue the real middle after the immediate display prefix, then generate select_main immediately so the queue follows it in retail order. Use the named six-entry scalar prefix as a tuning candidate; place %d after 170 and define 24/6/-144 before the queue.

15 result: both pools IDENTICAL, retail function order matches, allocator is 100%. Four display functions differ from named-prefix expression canonicalization: three differ only in the order of the +32 operand and saved FPR assignments; Luigi also shares distinct zero loads.

16. Put +32 first explicitly in bound comparisons, and preserve Luigi's separate zero reads with const volatile loads from the same constant object.

16 result: still 33/37, but only seven FPR-register differences in each of three displays; Luigi has those plus one moved zero load. Both pools remain unchanged.

17. Cache the named 32 margin in an explicit local before the loops, testing its saved-FPR allocation.

17 result: rejected: explicit margin local does not change saved-FPR assignment and moves its load.

18. Keep plain known const 32 reads with opt_propagation off in the four display functions, testing whether named storage can retain literal register assignment.

18 result: rejected: const reads fold to anonymous 32 despite opt_propagation off; several integer expressions also change.

19. Restore native scalar prefix, pool %3d inline, and express the half-width as division by 2.0f so the compiler's reciprocal multiplication may keep the target's operand/register order.

19 result: 37/37 functions at 100%; native division by 2 yields the original half multiply, including FPR order. Both pools IDENTICAL; function order matches; direct relocation-value check has 0 findings. Before linking, the full data check found two descriptor-table addends (+3 entries) and swapped BSS objects.

20. Correct descriptor table rows 13/16 to select_desc[3], and use ordinary zero initializers under explicit_zero_data off for the BSS objects in retail declaration order. Remove redundant zero-data pragmas.

20 result: 37/37 retained, but ordinary zero initializers still go into .sdata; rejected.

21. Restore tentative BSS declarations. Forward-declare initialized small data and define it after select_disp_mario first uses mario_status_point_table, before wp's initializer takes work's address. Definitions remain in retail .sdata order.

21 result: all functions/pools stay matched, but BSS still emits work before point_table. Initialized small data remains in the correct order.

22. Swap the two tentative BSS declaration lines, as in the successful tou BSS correction.

22 result: 37/37 functions, all .text/.data/.sdata/.sdata2/.rodata bytes equal through normal trailing alignment, every relocation kind/target section/offset equal, BSS point_table +0/work +0xc. No extra .text functions remain.

23. Flip winmgr.c to Matching and run the exact requested python3 configure.py && ninja command, then verify all build.sha1 checksums if the build completes.

23 result: the default tool-download rule fails in the restricted-network environment before compilation. The same cached local tool paths used for baseline avoid this unrelated download.

24. Configure with explicit cached compiler/binutils/wrapper/dtk/objdiff/sjiswrap paths and run the full Matching build.

24 result: full build found two legacy callers declaring winmgr event callbacks as s32 arrays, conflicting with the added owning-header function prototypes. gor_02 stopped compilation; tik_11 has the same declaration pattern.

25. Use the owning winmgr header in gor_02/tik_11 and remove their incorrect array declarations. Clean winmgr comments/pragmas, restore moved explanatory comments, and put small-data initializers back beside their declarations while retaining the corrected tentative BSS order. Build the affected units.

25 result: winmgr 37/37, gor_02 44/44, tik_11 14/14 at 100%. Both pools IDENTICAL; function order matches; BSS offsets match after simplifying small-data declarations.

26. Rerun full ninja with Matching retained and verify build.sha1.

27. Compile winmgr without MUST_MATCH using the same cached-tool compiler command into a temporary directory.

26 result: full Matching build passed; `build/tools/dtk shasum -c config/G8MJ01/build.sha1 -q` printed `32 files OK`. Matching flag retained. Cached tool paths are needed because the default download rule cannot reach the network in this environment.

27 result: natural compile without MUST_MATCH PASS.

28. Remove the extra blank line at EOF and rerun ninja/checksum plus the requested pool, function-order, and relocation checks on the final source.

28 result: PASS. Full build and all 32 checksums pass again; 37/37 functions at 100%; .sdata2/.rodata IDENTICAL; function order matches; relocation-value checker reports 0 findings; git diff --check is clean.

Final: winmgr links, Matching retained, natural compile passes. No pool helpers or new section-placement tricks were added; get10 is now truly inline. The descriptor data and callback declarations are complete, including two legacy caller declaration fixes. All changes are uncommitted. 28 logged attempts, including a rewrite that stopped before compilation, below the 35-experiment cap.
