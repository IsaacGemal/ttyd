# party_cloud link attempts (2026-10-07)

Limit: 35 build experiments. Changes remain uncommitted.

1. Baseline: configure with cached local compiler/binutils/wibo/dtk/objdiff/sjiswrap paths, rebuild party_cloud, and record function/pool/order scores.

1 result: 15/16 at 100%; N__cloud_use 99.90%; function order matches; .sdata2 ORDER (26 displaced), .rodata DUP_OURS conversion double.

2. Remove the named conversion double and obsolete partial comment; emit real functions immediately in retail order, retaining existing inline logic and data for this first candidate.

2 result: 15/16 retained, N__cloud_use 99.89% (temporary regression, rejected as final); native double is now unique but still follows strings, and small-data order is unchanged.

3. Define the ordinary three-scalar prefix before immediate functions, use guarded nonfolding reads, and test ordinary forward const pose arrays defined after N__cloud_use so the native conversion can pool first.

3 result: compile failed from an overly broad literal substitution (e.g. 20.0f became 2CLOUD_ZERO). The checker mislabeled the compile error as a timeout; no compiler hang occurred.

4. Correct literal token substitutions and retain the native zero Vec template; rebuild the same named-prefix/forward-string candidate.

4 result: compile still failed; request raw Ninja diagnostics because the checker hides the actual compiler message.

5. Run the same failed candidate directly through Ninja to identify its compiler error.

5 result: MWCC rejects a tentative static const pooled array followed by an initialized definition once immediate code uses it. No timeout occurred.

6. Use ordinary extern const forward declarations for pose arrays, with initialized definitions after N__cloud_use.

6 result: 15/16 retained, but forward arrays inferred writable and N__cloud_use dropped to 96.63%; the native conversion double is correctly at +0x10. Static scalar storage emitted at first use rather than declaration (70/180 late). Rejected as final.

7. Give the shared prefix ordinary external const definitions, and test unsized extern const pose arrays to avoid early small-data inference.

7 result: external prefix is in retail order; only pi/3 remain swapped in .sdata2. Unsized extern arrays still become writable. Rejected as final.

8. Queue the real N__cloud_use before cloud_exit, define its ordinary const poses before generation, then emit cloud_exit immediately so the queued use follows it in retail text order.

8 result: function order matches and readonly pose storage is restored, but its strings still precede the native double; N__cloud_use 99.89%. Rejected as final.

9. Restore literal poses and immediate real functions, retain the correct external scalar prefix, reorder the sine expression to pool pi before 3, and test explicit native int->double->float divisor casts.

9 result: explicit double casts emit fsub+frsp (99.25%) and do not move the magic; reordered sine syntax alone still pools 3 before pi. Rejected.

10. Test moving the real case-one interpolation into the existing move inline, specialized by a constant flag; no helper function is added. This tests whether its native conversions pool before the caller strings. Natural code retains direct interpolation.

10 result: moving conversions through the existing inline changes allocation (99.66%) without moving the double. Rejected and restored the direct native conversions.

11. Place an ordinary named pi constant before its first function and use guarded nonfolding reads; use the named seventy prefix directly instead of the existing trivial inline.

11 result: all other functions remain 100%, but pi must follow 4.25, and the direct seventy read worsens N__cloud_use (99.61%). Rejected as final.

12. Put ordinary 4.25/pi definitions in retail order before cloud_move, use guarded nonfolding reads, and restore the existing seventy inline.

12 result: .sdata2 IDENTICAL; all other functions remain 100%; N__cloud_use 99.89% with native double still following strings. This remains a tuning candidate, not an accepted regression.

Named-greedy sweep: run both directions. The to-literal direction has no recognized named scalar sites inside N__cloud_use (the prefix uses guarded macros). Each literal-to-named candidate below is one experiment.

13. named_greedy literal-to-named candidate: +extern const f32 float_100_8041dfb4;; -        cloud->unk0 = 100.0f;; +        cloud->unk0 = float_100_8041dfb4;

13 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

14. named_greedy literal-to-named candidate: +extern const f32 float_120_8041dfb8;; -        *(f32*)((u8*)cloud + 0xC) = 120.0f;; +        *(f32*)((u8*)cloud + 0xC) = float_120_8041dfb8;

14 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

15. named_greedy literal-to-named candidate: +extern const f32 float_2_8041dfd0;; -            *(void**)((u8*)party + 0x178) = effKumokumoN64Entry(0, 0, CLOUD_ZERO, CLOUD_ZERO, CLOUD_ZERO, CLOUD_ZERO, -2.0f, CLOUD_ZERO);; +            *(void**)((u8*)party + 0x178) = effKumokumoN64Entry(0, 0, CLOUD_ZERO, CLOUD_ZERO, CLOUD_ZERO, CLOUD_ZERO, -float_2_8041dfd0, CLOUD_ZERO);

15 result: N__cloud_use 99.75%. The tool retains only score improvements with no other function or pool regression.

16. named_greedy literal-to-named candidate: +extern const f32 float_270_8041df98;; -            angle = revise360(270.0f - *(f32*)((u8*)mario + 0x1AC));; +            angle = revise360(float_270_8041df98 - *(f32*)((u8*)mario + 0x1AC));

16 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

17. named_greedy literal-to-named candidate: +extern const f32 float_1_8041dfa4;; -            mario->unk138 = SEVENTY() * ((angle <= CLOUD_HALF_TURN) ? -1.0f : 1.0f);; +            mario->unk138 = SEVENTY() * ((angle <= CLOUD_HALF_TURN) ? -float_1_8041dfa4 : 1.0f);

17 result: N__cloud_use 99.46%. The tool retains only score improvements with no other function or pool regression.

18. named_greedy literal-to-named candidate: +extern const f32 float_1_8041dfa4;; -            mario->unk138 = SEVENTY() * ((angle <= CLOUD_HALF_TURN) ? -1.0f : 1.0f);; +            mario->unk138 = SEVENTY() * ((angle <= CLOUD_HALF_TURN) ? -1.0f : float_1_8041dfa4);

18 result: N__cloud_use 99.61%. The tool retains only score improvements with no other function or pool regression.

19. named_greedy literal-to-named candidate: +extern const f32 float_90_8041df94;; -        if (f31 < 90.0f || f31 >= 270.0f) { flag = 1; }; +        if (f31 < float_90_8041df94 || f31 >= 270.0f) { flag = 1; }

19 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

20. named_greedy literal-to-named candidate: +extern const f32 float_270_8041df98;; -        if (f31 < 90.0f || f31 >= 270.0f) { flag = 1; }; +        if (f31 < 90.0f || f31 >= float_270_8041df98) { flag = 1; }

20 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

21. named_greedy literal-to-named candidate: +extern const f32 float_90_8041df94;; -        f31 = toMovedir(f31 + (flag ? -90.0f : 90.0f));; +        f31 = toMovedir(f31 + (flag ? -float_90_8041df94 : 90.0f));

21 result: N__cloud_use 99.75%. The tool retains only score improvements with no other function or pool regression.

22. named_greedy literal-to-named candidate: +extern const f32 float_90_8041df94;; -        f31 = toMovedir(f31 + (flag ? -90.0f : 90.0f));; +        f31 = toMovedir(f31 + (flag ? -90.0f : float_90_8041df94));

22 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

23. named_greedy literal-to-named candidate: +extern const f32 float_10_8041dfc4;; -        party->position.y = mario->position.y + 10.0f;; +        party->position.y = mario->position.y + float_10_8041dfc4;

23 result: N__cloud_use 99.86%. The tool retains only score improvements with no other function or pool regression.

24. named_greedy literal-to-named candidate: -        *(Vec*)party->field_7C = (Vec){0.0f, 0.0f, 0.0f};; +        *(Vec*)party->field_7C = (Vec){float_0_8041df70, 0.0f, 0.0f};

24 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

25. named_greedy literal-to-named candidate: -        *(Vec*)party->field_7C = (Vec){0.0f, 0.0f, 0.0f};; +        *(Vec*)party->field_7C = (Vec){0.0f, float_0_8041df70, 0.0f};

25 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

26. named_greedy literal-to-named candidate: -        *(Vec*)party->field_7C = (Vec){0.0f, 0.0f, 0.0f};; +        *(Vec*)party->field_7C = (Vec){0.0f, 0.0f, float_0_8041df70};

26 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

27. named_greedy literal-to-named candidate: +extern const f32 float_2_8041dfd0;; -        *(f32*)((u8*)cloud + 0x8) += 2.0f;; +        *(f32*)((u8*)cloud + 0x8) += float_2_8041dfd0;

27 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

28. named_greedy literal-to-named candidate: +extern const f32 float_18_8041df8c;; -                             party->position.x, 18.0f + mario->position.y, party->position.z,; +                             party->position.x, float_18_8041df8c + mario->position.y, party->position.z,

28 result: N__cloud_use 99.89%. The tool retains only score improvements with no other function or pool regression.

Sweep result: both named_greedy directions completed; none of the 16 literal-to-named candidates passed its improvement gate. Source restored to experiment 12.

29. Test compound-assignment interpolation to change the old-position web rank, and put the selected sign before the named seventy operand.

29 result: N__cloud_use improves to 99.98%, all other functions 100%, .sdata2 IDENTICAL. Compound assignment gives the original f2/f3 conversion allocation; six differences remain (magic/string offsets and multiply operand order).

30. Preserve the selected sign in a match-only local before multiplying by seventy, testing operand allocation without changing the pool.

30 result: unchanged six differences and 99.98%; rejected as unnecessary extra source structure.

31. Test moving the existing fully inlined seventy literal function before all real functions, removing the named seventy definition, to see whether native early pooling can preserve the original multiply order without a new helper or extra text.

31 result: original function order and no extra text, but native seventy pools late (23 displaced .sdata2 entries); rejected. Restored experiment 29.

32. Clean the retained 99.98% candidate: guard matching pragmas, remove redundant emission directives, restore original sine/sign expression spelling, document the precise remaining differences, and trim whitespace. Rebuild and check pools/order/relocation values.

32 result: N__cloud_use 99.98%, remaining 15 functions 100%, .sdata2 IDENTICAL, .rodata ORDER with one unique native double, function order matches, relocation-value checker 0 findings, store/constant sweep 0, instruction-count check 0, diff whitespace clean.

33. Compile the final party_cloud source without MUST_MATCH using the same cached-tool compiler command into a temporary directory.

33 result: natural compile PASS.

34. Temporarily flip party_cloud to Matching, configure with cached local tools, run the full Ninja build, and verify every build.sha1 checksum. Retain Matching only if all 32 pass.

34 result: full compilation/linking succeeded, but MarioSt.dol checksum failed; 31 other files OK. Matching is rejected and reverted. Remaining native-double/string order and one multiply operand difference prevent linking.

35. Restore NonMatching, reconfigure with cached tools, run full Ninja and all 32 checksum checks, then verify the retained function/pool/order/value results without further source changes.

35 result: full Ninja PASS after reverting Matching; `build/tools/dtk shasum -c config/G8MJ01/build.sha1 -q` prints `32 files OK`. Final .sdata2 IDENTICAL, .rodata ORDER, function order matches, relocation-value check 0 findings (also checked directly against the two objects), natural compile PASS, and `git diff --check` clean.

Final: 35/35 build experiments used. N__cloud_use improves from 99.90% to 99.98%; the other 15 functions remain 100%. The named conversion-double pin is removed, leaving one native copy and no extra helpers or new section-placement tricks. Ordinary guarded scalar reads give the original .sdata2 pool order. Compound-assignment interpolation fixes the original f2/f3 allocation. The .rodata double still follows the four pose strings, and the seventy multiply swaps operands: six instruction differences remain. The Matching trial differs from retail by 7 .text bytes and 37 .rodata bytes; .data is identical. NonMatching is retained. All changes are uncommitted.
