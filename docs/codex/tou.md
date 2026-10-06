# tou link attempts (2026-10-06, Codex GPT-6)

Limit: 35 build experiments. Source changes remain uncommitted.

1. Baseline: 36/36 functions at 100%; .sdata2 IDENTICAL; .rodata has duplicate conversion double and make_monitor zero. Local tool paths were supplied to configure because default Ninja tooling rules attempted an unavailable network download. The function-order checker assumes nested REL paths, so build-only symlinks are used for its requested tou/tou invocation.

2. Removed named conversion double and reused named zero in make_monitor: .rodata ORDER (28 displaced); make_monitor 99.84%, rejected.

3. Pool_data on with named conversion double removed: 28/36 at 100%, one zero duplicate left; rejected.

4–5. All tail scalars/strings to literals, conversion double removed: build failed because the rewrite used stale source offsets and damaged declarations; repeated direct Ninja build confirmed the syntax error. Rejected and rewrite corrected.

6. Correct all-literal tail, pool_data off: 33/36, fight_money 94.73%, evt_tou_print_ranking 99.51%, disp_proc 83.84%; four constants folded away. Rejected.

7. Named prefix before magic; literal suffix: 34/36; ranking 99.51%, disp_proc 83.84%; four missing constants and duplicate zero. Rejected.

8. Immediate real functions with named definitions before first users: 32/36; compiler folds defined constants and introduces 34 duplicates. Rejected.

9. Literal suffix loaded through function locals with opt_propagation off: 34/36, ranking 79.27%, disp_proc 53.42%; rejected.

10. Immediate functions with volatile scalar reads to avoid folding: 31/36; .rodata retains one zero duplicate; rejected.

11. Immediate functions with scalar reads cast through f32 pointers: 36/36 at 100%; only zero duplicate remains, conversion double is at retail offset. Kept as candidate.

12. Reuse named zero in make_monitor through non-const read: both pools IDENTICAL; 35/36, make_monitor 99.84% (three integer argument loads reordered). Kept as tuning candidate, not final.

13. Single local z passed three times to GXInitTexObjLOD: unchanged 35/36, both pools IDENTICAL. Rejected.

14. Disable common subexpressions in make_monitor: 35/36, make_monitor 99.52%; rejected.

15. Literal prefix with immediate functions: failed to compile due to a redundant nested zero cast in the generated candidate. Corrected for next build.

16. Correct native prefix with existing real functions emitted immediately: both pools IDENTICAL; 35/36, only gans_tex 99.83% (four viewport FPR moves). make_monitor restored to 100%.

17. Swap viewport local declarations: unchanged 35/36; rejected.

18. Native literal prefix plus native viewport constants, remaining named scalars read through f32 pointers: 36/36 at 100%, .sdata2/.rodata IDENTICAL, function order matches, relocation-value checker reports 0. No helper functions added.

19. Cleaned match-only reads into TOU_F32 with a natural #else, documented/guarded immediate function emission, and formatted code: 36/36, both pools IDENTICAL.

20. Compile without MUST_MATCH: PASS.

21. Flip tou.c to Matching; run python3 configure.py && ninja, then the full build.sha1 checksum. Failed before compilation because Ninja tried to re-download the cached dtk tool and DNS/network access was unavailable. Refreshing target timestamps did not prevent command-change detection.

22. Configure with explicit paths to installed tools (compilers, binutils, wibo, dtk, objdiff-cli, sjiswrap) and run full ninja. This uses the same compiler/link settings and avoids download rules. See result below.

22 result: full compilation passed; checksum 31 files OK, tou.rel failed. Only two bytes differ in the REL: .data+0x828/+0x82c point to swapped BSS work buffers. Text and both pools match. Matching flag reverted while fixing.

23. Explicit ordinary zero initializers for champion_belt_onoff, gans_wp, disp_work, rank_work in original BSS order; no section placement tricks. See result below.

23 result: explicit zero initializers put the objects in .data, rejected despite all functions still scoring 100%.

24. Swap tentative rank_work/disp_work declaration order (both forward and final), retaining ordinary BSS declarations. See result below.

24 result: 36/36 functions; all bytes and relocation targets compare equal. Only normal trailing .rodata/.bss alignment padding differs in the object; BSS offsets now match retail.

25. Flip to Matching again, configure with cached local tools, run ninja and full build.sha1 verification. PASS: ninja succeeded and `build/tools/dtk shasum -c config/G8MJ01/build.sha1 -q` printed `32 files OK`. Matching flag retained.

26. Final natural compile without MUST_MATCH after the BSS declaration-order fix: PASS.

Final: 36/36 functions at 100%; .sdata2 and .rodata IDENTICAL; function order matches; relocation-value check 0; all 32 checksums pass. 26 experiments including baseline, failed builds, natural compiles and full link attempts (below cap 35). Changes uncommitted.
