# shadowdrv matching attempts (2026-10-07, Codex)

Limit: 35 build experiments. Changes stay uncommitted. Baseline source/configuration are saved outside the repository in /tmp. Use cached local tools for configuration because this checkout has no generated build files.

1. Baseline build: 37/38 functions at 100%; Projection 99.52% (1656 B), Texture 100% (916 B). `.rodata` IDENTICAL; `.sdata2` has duplicate pooled 0/1 from Texture. Function order matches; partial relocation-value check has 0 findings. Initial agent_check before configuration stopped before building (missing objdiff.json).

2. Replace only Texture 0/1 literals with their named constant objects; Projection untouched.
   Result: Build exit 0; shadowCharShadowDisp_Texture 92.66 (baseline 100.00); Projection 99.52; Texture 92.66.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

3. Retain named zero local/fog; read Texture vertex zero/one through const volatile f32 to preserve retail per-use reloads.
   Result: Build exit 0; shadowCharShadowDisp_Texture 94.39 (baseline 100.00); Projection 99.52; Texture 94.39.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

4. Write Texture FIFO coordinates at point of use, with named volatile zero/one reads; initialize loop zero immediately before loop.
   Result: Build exit 0; shadowCharShadowDisp_Texture 99.78 (baseline 100.00); Projection 99.52; Texture 99.78.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

5. Continue 1/20: explicitly name positive then negative quad coordinates before FIFO writes, testing FPR allocation.
   Result: Build exit 0; no function regression; Projection 99.52; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

6. Continue 2/20: compute named right inner edge between vertex 11 y/z FIFO writes and reuse in vertices 15/16.
   Result: Build exit 0; shadowCharShadowDisp_Projection 98.27 (baseline 99.52); Projection 98.27; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

7. Continue 3/20: use point-of-use FIFO position writes throughout Projection quad block, preserving arithmetic.
   Result: Build exit 0; shadowCharShadowDisp_Projection 98.30 (baseline 99.52); Projection 98.30; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

8. Continue 4/20: point-of-use FIFO positions only for Projection vertices 15/16.
   Result: Build exit 0; shadowCharShadowDisp_Projection 98.96 (baseline 99.52); Projection 98.96; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

9. Continue 5/20: point-of-use FIFO position only for Projection vertex 11.
   Result: Build exit 0; shadowCharShadowDisp_Projection 98.76 (baseline 99.52); Projection 98.76; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

10. Continue 6/20: express right-inner subtraction as addition of negated we; check backend canonicalization.
   Result: Build exit 0; shadowCharShadowDisp_Projection 98.72 (baseline 99.52); Projection 98.72; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

11. Continue 7/20: direct byte FIFO writes for vertex 11 color, testing inline boundary scheduling.
   Result: Build exit 0; no function regression; Projection 99.52; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

12. Continue 8/20: explicitly reuse left outer edge temporary for right inner edge after vertex 12; preserve natural form.
   Result: Build exit 0; shadowCharShadowDisp_Projection 99.21 (baseline 99.52); Projection 99.21; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

13. Continue 9/20: point-of-use FIFO position only for Projection vertex 12.
   Result: Build exit 0; shadowCharShadowDisp_Projection 98.76 (baseline 99.52); Projection 98.76; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

14. Continue 10/20: disable lifetime optimization for Projection to test late subtraction placement.
   Result: Build exit 0; no function regression; Projection 99.52; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

15. Continue 11/20: restore baseline Projection; clean and guard Texture point-of-use loads/coordinate locals, with natural GX calls under else.
   Result: Build exit 0; no function regression; Projection 99.52; Texture 100.00.
   == .sdata2: IDENTICAL
   == .rodata: IDENTICAL

16. Continue 12/20: compile cleaned Texture source without MUST_MATCH using the configured compiler command.
   Result: natural compile exit 0.

17. Continue 13/20: flip shadowdrv to Matching, configure with cached tools, build full project, and check all retail SHA-1s.
   Result: full Matching link fails before checksum: pre-existing global data names negone_one_374, one_negone_373, negone_one_360, one_negone_359 collide with ac_air_gauge.o. Matching flag reverted. All shadowdrv data-section bytes and sizes equal retail; Projection still has one moved subtraction.

18. Continue 14/20: restore NonMatching, regenerate with cached tools, run normal full ninja and retail checksums.
   Result: full ninja passes; `build/tools/dtk shasum -c config/G8MJ01/build.sha1 -q` prints `32 files OK`. This uses the retail shadowdrv object because the Matching trial did not link.

Final: Texture restored from the Continue candidate's 99.78% to 100%; Projection remains at its original 99.52%. All 37 other functions are 100%, with no function below its original starting score. The unit's matched_code is restored to 85.64% (9876/11532 B), and matched_data is 100%. Both `.sdata2` and `.rodata` are IDENTICAL. Function order matches; partial relocation-value check (including a direct check_unit invocation) has 0 findings; store sweep and instruction delta have 0 findings; natural compilation passes; git diff --check is clean. Projection experiments 6–14 were rejected and its source is unchanged from the original baseline. The Matching flag is restored to NonMatching after the link failure. Source/report/log changes remain uncommitted. Fourteen additional build experiments were used (18 total logged), within the Continue cap of 20.

Compiler observation: naming Texture's positive quad coordinate before its negative half-size computation restores retail f1/f2 allocation while separate volatile reads of the existing zero/one objects preserve retail reloads without duplicate literals. Direct FIFO writes and these temporaries are guarded by MUST_MATCH, with ordinary GX calls in the natural branch. No helpers or section-placement tricks were added.
