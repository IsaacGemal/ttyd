# Pendulum crane timing attempts (2026-10-07)

Limit: 35 build experiments. Changes remain uncommitted.

1. Baseline build with cached local tool paths; record all function scores, pools, order, and relocation values.

1 result: 6/7 at 100%; _get_angle_rate 99.20%. .rodata IDENTICAL, .sdata2 has two duplicate 0/1 constants. Emitted order reversed plus pool_i2f. Relocation wrapper reports 0 findings; its unit count counts only units with findings. Direct verification was also performed in experiment 29.

2. Remove pool_i2f, run requested fn_order rewrite, then emit real functions immediately in retail order. Move named constants before immediate readers and keep nonfolding named accesses under MUST_MATCH.

2 result: all scores unchanged and retail order matches; .rodata IDENTICAL but six duplicate scalar constants, so candidate needs pool correction. fn_order --write added owning-header prototypes; adapted its reverse source order to immediate retail order.

3. Preserve all plain scalar reads through NF32 under MUST_MATCH to eliminate duplicates from folding already-defined constants.

3 result: both pools IDENTICAL, but main regressed to 98.54%; not retained.

4. Restore main's native 0/1 reads to preserve FPR CSE; test nonvolatile NF32 loads for _get_angle_rate.

4 result: baseline function scores restored, .rodata IDENTICAL and .sdata2 back to baseline two duplicates. Helper-free retail order candidate saved.

5. Test native float literals for all four _get_angle_rate constants, isolating allocator behavior before pool correction.

5 result: rejected (98.80%) and five duplicates. Restored step 1.

6. _get_angle_rate: Literal zero argument; all other code unchanged.

6 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
== .sdata2: CONTENT
   DUP_OURS (2):
     1                        @185                         actionCommandDisp, battleAcMain_PendulumCraneTiming   [neg]
     0                        @170                         _get_angle_rate, battleAcMain_PendulumCraneTiming   [neg]
== .rodata: IDENTICAL
```

7. _get_angle_rate: Literal 45 addend; all other code unchanged.

7 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   98.80%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
== .sdata2: CONTENT
   DUP_OURS (3):
     0                        @292                         battleAcMain_PendulumCraneTiming   [neg]
     45                       @170                         _get_angle_rate
     1                        @185                         actionCommandDisp, battleAcMain_PendulumCraneTiming   [neg]
== .rodata: IDENTICAL
```

8. _get_angle_rate: Literal 100 factor; all other code unchanged.

8 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
== .sdata2: CONTENT
   DUP_OURS (3):
     1                        @185                         actionCommandDisp, battleAcMain_PendulumCraneTiming   [neg]
     0                        @292                         battleAcMain_PendulumCraneTiming   [neg]
     100                      @170                         _get_angle_rate
== .rodata: IDENTICAL
```

9. _get_angle_rate: Literal 90 divisor; all other code unchanged.

9 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
== .sdata2: CONTENT
   DUP_OURS (3):
     0                        @292                         battleAcMain_PendulumCraneTiming   [neg]
     1                        @185                         actionCommandDisp, battleAcMain_PendulumCraneTiming   [neg]
     90                       @170                         _get_angle_rate
== .rodata: IDENTICAL
```

10. _get_angle_rate: Widen the returned base and narrow at addition.

10 result: _get_angle_rate: 86.80% (100 B); rejected and restored.

11. _get_angle_rate: Widen bumped local and narrow at multiplication.

11 result: _get_angle_rate: 99.20% (100 B); rejected and restored.

12. _get_angle_rate: Nest compAngle and sum inside multiplication.

12 result: _get_angle_rate: 98.80% (100 B); rejected and restored.

13. _get_angle_rate: Declare product before sum and returned base.

13 result: _get_angle_rate: 99.20% (100 B); rejected and restored.

14. Remove explicitly named 0/1 objects and use native literal pooling for their reads, testing whether duplicate elimination also changes allocator webs.

14 result: all starting scores retained, both duplicates removed, but .sdata2 ORDER (19 displaced entries). Saved for analysis, not retained as final.

15. Pool the complete seven-scalar prefix natively; reverse the literal 45 addend to retain target operand order.

15 result: rate regresses (98.80%) and pool order differs; rejected.

16. Restore named pool-identical candidate and use volatile loads for main's clamp constant and final rotation zero.

16 result: rejected (main 94.76%); volatile reloads change scheduling. Restored helper-free baseline-score candidate.

17. _get_angle_rate: Load named factor before named addend.

17 result: _get_angle_rate: 99.20% (100 B); rejected and restored.

18. _get_angle_rate: Reuse incoming angle parameter for returned base.

18 result: _get_angle_rate: 99.20% (100 B); rejected and restored.

19. _get_angle_rate: Keep product in wider local then narrow before division.

19 result: _get_angle_rate: 99.20% (100 B); rejected and restored.

20. _get_angle_rate: Load divisor before sum and factor.

20 result: _get_angle_rate: 99.20% (100 B); rejected and restored.

21. _get_angle_rate: Widen base without explicit narrowing before the sum.

21 result: _get_angle_rate: 93.00% (100 B); rejected and restored.

22. _get_angle_rate: Narrow a double addition explicitly.

22 result: _get_angle_rate: 93.00% (100 B); rejected and restored.

23. _get_angle_rate: Narrow a double product explicitly.

23 result: _get_angle_rate: 93.20% (100 B); rejected and restored.

24. Keep main's constant-one in a local loaded once through MATCH_F32 after each trigger conversion; reuse it for clamp comparison and assignment. Restore second icon's NF32 one.

24 result: rejected (main 97.69%); caching one preserves a single load but rotates conversion FPRs. Restored step 1.

25. Disable opt_propagation only for _get_angle_rate with immediate emission and nonvolatile named reads.

25 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
```

26. Disable opt_lifetimes only for _get_angle_rate with immediate emission and nonvolatile named reads.

26 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
```

27. Disable opt_common_subs only for _get_angle_rate with immediate emission and nonvolatile named reads.

27 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
```

28. Disable opt_dead_assignments only for _get_angle_rate with immediate emission and nonvolatile named reads.

28 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
```

25–28: no improvement; all rejected and optional pragmas removed.

29. Clean up the retained helper-free, baseline-score source. Restore original rate loads, second-icon NF32 one, and remove unneeded fn_order-added header prototypes. Verify scores, pools, emitted order, and relocation values directly.

29 result: 6/7 functions 100%; rate 99.20% unchanged. .rodata IDENTICAL; .sdata2 retains only the same two duplicates as baseline. Retail function order matches. Wrapper and direct partial relocation checks report 0 findings.

30. Compile the final source without MUST_MATCH using its actual Ninja compiler command and a temporary output directory.

30 result: natural compile PASS.

31. Temporarily flip the object to Matching and run default configure plus full Ninja build (two jobs). Keep Matching only after the requested checksum passes.

31 result: default configure succeeded, but Ninja tried downloading dtk and failed because network/DNS is unavailable. No compile/link result from this attempt.

32. Reconfigure with the cached local tool paths, build with Matching temporarily enabled, and verify build.sha1.

32 result: compilation and linking completed; Matching checksum FAILED only for MarioSt.dol (31 REL files OK). The flip will be reverted unless the remaining bounded candidates match.

33. Final rate candidate: Only the addend uses a volatile named load; other constants use ordinary nonfolding named loads.

33 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
== .sdata2: CONTENT
   DUP_OURS (2):
     0                        @291                         battleAcMain_PendulumCraneTiming   [neg]
     1                        @290                         battleAcMain_PendulumCraneTiming   [neg]
== .rodata: IDENTICAL
```

34. Final rate candidate: Only the factor uses a volatile named load; other constants use ordinary nonfolding named loads.

34 result:

```
MarioSt/battle/ac/ac_pendulum_crane_timing: 6/7 functions at 100%
   99.20%    100 B  _get_angle_rate
  100.00%      8 B  battleAcResult_PendulumCraneTiming
  100.00%     24 B  battleAcDelete_PendulumCraneTiming
  100.00%     32 B  _get_angle_hp
  100.00%    256 B  battleAcDisp_PendulumCraneTiming
  100.00%    660 B  actionCommandDisp
  100.00%   1560 B  battleAcMain_PendulumCraneTiming
== .sdata2: CONTENT
   DUP_OURS (2):
     1                        @290                         battleAcMain_PendulumCraneTiming   [neg]
     0                        @291                         battleAcMain_PendulumCraneTiming   [neg]
== .rodata: IDENTICAL
```

33–34 result: no improvement; both rejected. Restored the no-regression helper-free candidate and reverted Matching.

35. Final bounded experiment: rebuild the retained unit and full project with NonMatching restored, then verify build.sha1, all seven scores, pools, order, and relocation values. Natural compilation already passed for this code in experiment 30; subsequent retained edits only update comments.

35 validation note: the combined Ninja command used nonexistent target `all` and stopped before compiling/linking. The subsequent unit check rebuilt the retained source successfully, with all baseline scores/pools/order/relocation checks retained. Continue this final unchanged-candidate validation using the actual default `progress` target; no further source experiment is performed.

35 result: PASS with NonMatching restored. Full default Ninja build completes and build.sha1 prints `32 files OK`. All seven function scores are unchanged from baseline (six 100%, _get_angle_rate 99.20%); retail emitted order matches; .rodata IDENTICAL; .sdata2 retains the baseline two duplicate 0/1 constants. Wrapper and direct partial relocation checks report 0 findings. Natural code compiled in experiment 30. `git diff --check` is clean.

Final: helper removal and retail function emission completed. No new helper or section trick. The bounded 35-experiment search did not bring _get_angle_rate to 100% or eliminate the two baseline scalar duplicates without a regression. Matching failed the MarioSt.dol checksum and was reverted. The project passes all 32 checksums with this unit NonMatching; this is not a completed unit link. Changes remain uncommitted.
