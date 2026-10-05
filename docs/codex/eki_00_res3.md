# ext_moamoa_main scheduling attempts — 2026-10-04

Scope: eki_00 res == 3 setup, carry verified fixes to rsh_05 and rsh_06.
Maximum 25 compiler invocations, including baseline and final validation. No tools/ edits or pushes.
Existing objects are stale (eki lines refer to an older source); measure rebuilt sources.
Use exact Ninja compiler commands directly to avoid unrelated build regeneration/downloads.
System Python already provides pyelftools; the task-specified ~/ttyd/.venv/bin is absent.

## Compiler invocations

1. Baseline eki_00, unchanged source: **97.29303%**, 12,736 target bytes.

2. Baseline rsh_05, unchanged source: **94.99969%**.

3. Baseline rsh_06, unchanged source: **90.49527%**.

4. Suggested inline-scope vector copy (`moaResetPosition`), with the already-known useful addend order 0x55C + 0x550. Result recorded below.
   Result 4: **97.293655%**; only the known addend-swap gain. Inline copy does not change the hoist or multiply timing; helper removed.

5. Initialize local half before t, retaining swapped addends and direct volatile vector copy. Result recorded below.
   Result 5: **97.29523%**; half in f0, sum in f1 and multiply operands now match. Vector hoist and multiply timing persist.

6. Return the volatile-loaded Vec through an inline local, then assign to volatile destination (exp05 angle setup). Result recorded below.
   Result 6: **96.936874%**, regressed; return-Vec adds stack stores and changes GPR colouring. Reverted.

7. Name the two angle inputs before adding them, half initialized first as in exp05. Result recorded below.
   Result 7: **97.29837%**; named inputs put 0x550 in f1 and retain multiply operands. 0x55C is f0 rather than f2. Other functions unchanged.

8. Express the copy as explicit volatile u32 words in load/load/store/store/load/store order. Result recorded below.
   Result 8: **97.103645%**, regressed; explicit copy makes lwzu and different GPRs. Reverted.

9. Inline half-angle helper taking wp, with named inputs and half first; flags/state remain at block top. Result recorded below.
   Result 9: **96.80622%**, regressed; helper moves wp and angle loads after flags/copy and changes constant materialization. Reverted.

10. Half constant as a local union, retaining exp07 named input order. Result recorded below.
   Result 10: **96.95572%**, regressed; union constant becomes integer load and stack round trip. Reverted.

11. Keep volatile source, ordinary Vec destination, with exp07 half-angle locals. Result recorded below.
   Result 11: **97.06501%**, regressed; ordinary destination changes register allocation, interleaving and store order. Reverted.

12. Initialize half between the two named loads and their addition to overlap its lifetime with both inputs. Result recorded below.
   Result 12: **97.29837%**, identical scheduling to exp07. Reverted to simpler exp07.

13. Inline half-angle helper taking half as a float parameter and wp as pointer, called before flags/state. Result recorded below.
   Result 13: **97.29208%**, regressed; half/angle helper changes saved FPR rank and does not prevent hoist. Reverted.

14. Put the entire half-angle, flags/state, and volatile vector copy in one inline scope returning the half-angle. Result recorded below.
   Result 14: **97.29208%**, regressed; combined helper likewise changes f27 to f28 and retains hoist. Reverted.

15. Nested expression half * (breath + angle), no precomputed t under MUST_MATCH. Result recorded below.
   Result 15: **96.80779%**, regressed; moving input reads past flags delays wp loads and changes GPRs. Reverted.

16. Nested (breath + angle) * half evaluated before flags/state; no intermediate sum local. Result recorded below.
   Result 16: **97.293655%**, worse than exp07; removes the sum-variable lifetime and restores the old FPR pattern. Reverted.

17. Ordinary vector source with volatile destination, retaining exp07 angle setup. Result recorded below.
   Result 17: **97.09642%**, regressed; removing source volatility disrupts copy and flags register order. Reverted.

18. Dead sign-extend pointer round trip on vector address (the repo's early-save register-web technique), retaining both volatile copy sides. Result recorded below.
   Result 18: **96.776695%**, regressed; vector address moves outside loop to r26, shifting other GPRs. Reverted.

19. Make only the two input reads volatile to add load-order dependencies before the volatile vector copy. Result recorded below.
   Result 19: **97.20572%**, regressed; input volatility changes half addressing to lis/lfs and retains early vector load. Reverted.

20. Final eki_00 source: exp07 shape restricted to MUST_MATCH; original natural expression retained. Comment update only otherwise. **97.29837%**.

21. Carry the verified angle shape and volatile Vec copy to rsh_05. **95.20137%**.

22. Carry the verified angle shape and volatile Vec copy to rsh_06. Result recorded below.
   Results 20–22: eki_00 **97.29837%**, rsh_05 **95.20137%**, rsh_06 **90.7065%**. All other function scores, all data-section scores and sizes unchanged; all three .text scores improve.

23–25. Compile each final source without MUST_MATCH into scratch natural-build directories (eki_00, rsh_05, rsh_06 respectively). **All PASS**.

## Final state and what fixed what

Stopped after **25 compiler invocations** (including three baselines, three final matching builds, and three natural-build validations). No subsequent build experiments.

| Unit | Rebuilt baseline | Final | Gain (percentage points) |
| --- | ---: | ---: | ---: |
| eki/eki_00 | 97.29303% | 97.29837% | +0.00534 |
| rsh/rsh_05 | 94.99969% | 95.20137% | +0.20168 |
| rsh/rsh_06 | 90.49527% | 90.70650% | +0.21123 |

Kept only:

- The already-known breath-first addition, now restricted to MUST_MATCH. It fixes the 0x55C/0x550 memory load order (+0.000625 points in eki).
- Initializing the existing local half before the angle setup puts the sum in f1 and the half constant in f0, fixing multiply operands (+0.001575 additional points).
- Naming breath and angle independently puts the 0x550 load in f1, also improving the add's operands (+0.00314 additional points). The 0x55C load is still f0 rather than retail's f2.
- The same shape in both rsh copies, together with eki's existing volatile Vec assignment. The copy retains retail's load/load/store/store/load/store sequence; the local half retains lis/addi/lfs instead of direct lis/lfs. Combined port gains are measured above; individual rsh levers were not separately measured.

All speculative helpers, union/pointer tricks, and added input volatility were removed. Natural branches retain the original expression and ordinary vector assignment.

### Precise remaining blocker

In eki's res == 3 block (objdiff aligned rows 512–527), the scheduler still places vec3_eki_00008908's lis/addi before the half constant's address setup. Retail forms the half address first, loads 0x55C into f2 and 0x550 into f1, then forms the Vec address and loads half into f0 before fadds. Ours loads 0x55C into f0 and 0x550 into f1, forms the half address later, issues the first vector lwz before the flags store, and loads half after fadds. Retail fmuls f27,f1,f0 precedes both initial vector loads; ours issues it after those loads. No attempted inline scope prevented this hoist without losing other matches.

The task description's assertion that all other colouring matches is broader than the rebuilt assembly supports: later blocks already had independent differences. The final eki diff has 203 diff-marked aligned rows after normalizing data-symbol names, 10 in this top block and 193 elsewhere. Examples outside the target include swapped fmadds factors at aligned rows 1122, 1130, 1323 and 1330; FPR/scheduling differences beginning around row 1441; constant materialization and random-range register differences in later states. This count is aligned diff rows, including insertion/deletion gaps, not a count of unique machine instructions. These later differences did not change with the kept fix. Target ext_moamoa_main is 12,736 bytes; our final function is 12,788 bytes, the same 52-byte excess as the rebuilt baseline.

### Verification

- Exact existing Ninja MWCC commands used for every build; no build flags changed. Direct compiler commands avoided unrelated manifest regeneration/downloads in the copied checkout.
- Fresh three-unit objdiff reports compared by function and section: only ext_moamoa_main and .text improve; no other function or data-section score changes, and all section sizes are unchanged.
- `python3 tools/reloc_value_check.py eki_00 --all`: **0 findings**. The same check over eki_00, rsh_05 and rsh_06 is also clean; confirmed all three objects are covered. Its summary counts finding-bearing units, hence "0 unit(s)" does not mean zero objects checked.
- All three final files compile without MUST_MATCH (invocations 23–25).
- `git diff --check`: clean.
- No tools/ changes, commits, or pushes. Raw per-attempt reports/diffs and natural build logs are in ignored `scratch/codex-scheduling/`.
