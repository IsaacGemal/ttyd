# Optimisation pragma regions kept after the second pragma pass

One line per scoped optimisation pragma region left in src/ and rels/ after n9-q-o-pragma2 (2026-10-21; chapter
"Optimisation pragmas, second pass" in docs/CONTEXT.md): what changes in the object without it, and the natural shapes
tried. Line numbers are each setter's line at the end of that pass. Condensed from the workers' reports (untracked,
build/pr2/v1..v8/REPORT.txt in that worktree).

Since then (n9-q-o-flags, 2026-10-22; chapter "Whole-file compiler settings moved to configure.py" in docs/CONTEXT.md),
40 of these regions were replaced by a per-file flag on the unit's Object line (`-pragma "opt_X off"`, the unit
compiling identical with the setting file-wide), and their entries are gone from this list, 16 of them sin/cos family
regions. The remaining entries are unchanged, line numbers included.

## The sin/cos polynomial family (studied as one problem, n9-q-o-sincos, 2026-10-21)

The regions marked "sin/cos" below (51 after that pass, 35 now) wrap an inlined sine/cosine approximation,
`sin x = x(1 - 0.16605x^2 + 0.00761x^4)` and `cos x = 1 - 0.4967x^2 + 0.03705x^4` (Abramowitz and Stegun
4.3.96/4.3.98) on [0, pi/2], reduced from [0, 2pi) by the compares against pi, pi/2 and 3pi/2, with the Horner steps
repeated in every quadrant branch. It appears in 30 retail functions across 19 files. The family study removed none of
them. Chapter "The sin/cos polynomial family" in docs/CONTEXT.md has the detail.

- **What the pair reproduces.** Retail loads the coefficients (0.00761/0.16605/1.0, then 0.03705/0.4967/1.0) into
  registers once, before the quadrant compares, and every branch uses those registers (opt_propagation off). Every
  Horner step overwrites the accumulator in place (`fmsubs f4,f4,f0,f3; fmadds f4,f4,f0,f2`), even where a lower
  volatile FPR is free, i.e. one register per source variable (opt_lifetimes off). The pool entries are literals in
  first-use order with the coefficients before pi, so the source assigned literal constants to locals before the
  compares.
- **No outside name.** No symbol in config/G8MJ01/symbols.txt, the SDK, MSL, libs/ or include/ is this polynomial.
  `sinfd`/`cosfd`/`sincosf`/`movePos` call MSL `sin`/`cos`. The four constants are absent from SPM's main.dol
  (~/spm-refs) and from the reference decomps in ~/ttyd-refs. So no shared inline was introduced, and the copies
  stay where they are.
- **Shapes tried, in isolation** (one test function, GC/2.6 with the project's flags, compared with the pair's
  object), all giving the pragma-less code: hand-written and `static inline`, plain `inline`, and auto-inlined
  `static` forms; a statement macro and a whole-function block macro; `register` locals; double literals; one-expression
  Horner with literals; an inline taking the coefficients as parameters; the polynomial inside a loop with the
  coefficients set before it; file-scope `static const`, plain `const` globals and function-local `static const`
  coefficients (MWCC folds all three); the pragma pair placed around the inline's definition instead of the caller
  (no effect: the caller's settings apply). A `static const` array keeps the loads but still differs, and it would
  change the data layout. Calling MSL `sinf` emits a call that retail doesn't have.
- **Shapes tried, in the real files.** Dropping only `opt_lifetimes` (keeping `opt_propagation off`) fails in every
  copy tried (evt_sub_get_coin 118 lines, evt_item_get_important_item 178, effFallMain 72, effSmallStarMain 60,
  effMObjBrokenMain 144, hoshi_jump 246, hei_10 npc_jump 194). The webs split, and the accumulator moves to the lowest
  free FPR. `#pragma optimization_level 2` in place of the pair matches only npcCheckHitMarioSide (it is what
  npcDisp_blur uses), not the other 17 files; an in-file `#pragma global_optimizer off` changes nothing. File-wide
  `-pragma "opt_propagation off" -pragma "opt_lifetimes off"` matches every function of pik_02, tou_02, eff_starpoint
  and evt_item (10 of the 51 regions). In the other 15 files it breaks functions that have no polynomial (bom_01 6,
  npcdrv 23, mario 16, eff_fall 4, ...), so it isn't the original's build for them. Moving those 10 regions into
  configure.py would hide the same setting rather than remove it, so it was not done (a lead for Isaac).
- **Why two copies need only the lifetimes half.** eff_mobj_broken and eff_small_star_n64 read the coefficients
  through named `extern const` .sdata2 objects defined later in the file, which MWCC can't fold, so no propagation
  happens. That is part of those units' data layout, not a shape for the others (it would change their pools).
- **A related clue.** marioDispBlur (mario.c, opt_dead_assignments) keeps only the compare skeleton of an unused copy.
  Retail ran neither dead-assignment removal nor propagation on these functions. Retail looks like these functions
  were built with the global optimiser's propagation and lifetime passes off. We can't tell how the original source
  arranged that, and no plain C form found here does it.

## rels/aji/src/aji_17.c

- `noiseDisp` (opt_propagation, line 394): base*256 and col*8 are hoisted once and shared by both inner loops; tried: plain index expressions; per-row base/col locals; i/4, i%4 inline in loop 2; the 4-term sum.

## rels/bom/src/bom_01.c

- `ext_init` (opt_propagation, line 717): sin/cos polynomial family: coefficients (0.16605, 1.0) rematerialised per branch instead of held in f23/f24, frame changes (821 lines); family study above.
- `ext_init` (opt_lifetimes, line 718): sin/cos polynomial family (309-line diff); family study above.
- `ext_main` (opt_lifetimes, line 864): sin/cos polynomial family (348-line diff); family study above.
- `ext_main` (opt_propagation, line 865): sin/cos polynomial family (1283-line diff); family study above.
- `depthDisp` (opt_lifetimes, line 2200): sin/cos polynomial family: FPR set and frame grow (~1000 lines, with u32 mtxId); family study above.
- `depthDisp` (opt_propagation, line 2202): sin/cos polynomial family, as the lifetimes pair (~1000 lines); family study above.

## rels/eki/src/eki_00.c

- `ext_moamoa_init` (opt_lifetimes, line 1307): sin/cos polynomial family (863-line diff); family study above.
- `ext_moamoa_init` (opt_propagation, line 1308): sin/cos polynomial family (902-line diff); family study above.
- `moamoa_hit_check` (opt_lifetimes, line 1524): sin/cos polynomial family (336-line diff); family study above.
- `moamoa_hit_check` (opt_propagation, line 1525): sin/cos polynomial family (1190-line diff); family study above.
- `ext_moamoa_main` (opt_propagation, line 1766): sin/cos polynomial family (1569-line diff); family study above.
- `ext_moamoa_main` (opt_lifetimes, line 1768): sin/cos polynomial family (1346-line diff); family study above.

## rels/gor/src/gor_00.c

- `gundan1000tai_init` (opt_lifetimes, line 4988): without it the second half's angle/s swap to f22/f21; tried: per-half radius/angle/sine locals in natural declaration orders (only a search-found interleaved order matched, not applied).

## rels/gor/src/gor_01.c

- `cook_chk` (opt_dead_assignments, line 5085): inlined itemNameToID tails put subfe after mr r3, r30 (retail keeps the dead id copy); tried: direct call args; if/return; ternary return; `i = 0; return i;`; id1/id2.
- `cook_chk2` (opt_propagation, line 5116): args[2] load and mr r3, r30 move ahead of the inlined naniga_okorukana_cook; tried: direct call argument; if/else result local in the inline; result declared, then assigned.

## rels/gor/src/gor_10.c

- `depthDisp` (opt_propagation, line 284): sin/cos polynomial family, same function as bom_01's depthDisp (320-line diff); family study above.

## rels/gor/src/unit/unit_lecture_frankli_sac.c

- `_battle_set_lecture_unit_id`, `lecture_init` (opt_dead_assignments, line 192): lecture_init's i/id swap r28/r29 (needs the MUST_MATCH dead `id = 0`); tried: decl orders; id in the loop; swapped arms; no id local; j counter.

## rels/hei/src/hei_10.c

- `ext_main` (opt_propagation, line 231): sin/cos polynomial family (260-line diff); family study above.
- `npc_jump` (opt_propagation, line 395): sin/cos polynomial family, as the lifetimes pair; tried: per-pair removal; one-expression Horner (worse). Family study above.

## rels/las/src/unit/unit_boss_black_peach.c

- `hand_init` (opt_propagation, line 4775): gp address and 10000000 constant swap (gp r31, const r26); tried: OSTime cast on the product; OSTime t local; full-product local; three `10000000 * t` operand orders.

## rels/mri/src/mri_puni.c

- `puniswSetindex` (opt_lifetimes, line 1655): offset base+index*7 colours r31, retail r29; tried: a `u32 addr` local (EQUIV here, breaks the three inlining callers); S1-S7 bodies without addr.
- `evt_puni_exit` (opt_lifetimes, line 7025): inlines puniswSetindex: same offset vs high-byte colouring; tried: the addr shapes; declaration permutations; `*args++`.
- `evt_puni_set_lsw_index` (opt_lifetimes, line 7544): inlines puniswSetindex: same colouring, addr in r28 not r26; tried: the addr shapes; 40 caller decl orders; rasim (best 2 wrong); `*args++` (24 lines).
- `evt_puni_all_save` (opt_lifetimes, line 7789): inlines puniswSetindex: same offset vs high-byte colouring; tried: the addr shapes; declaration permutations.

## rels/rsh/src/rsh_05.c

- `moamoa_hit_check`, `ext_moamoa_main`, `npc_jump` (opt_lifetimes, line 1242): no closer, runs to EOF; sin/cos family (ext_moamoa_main 1650/3147, moamoa_hit_check 360/750 lines for the pair); family study above.

## rels/rsh/src/rsh_06.c

- `moamoa_hit_check`, `ext_moamoa_main` (opt_lifetimes, line 1867): sin/cos polynomial family (ext_moamoa_main 1218/2408, moamoa_hit_check 360/750 lines for the pair); family study above.
- `moamoa_hit_check`, `ext_moamoa_main` (opt_propagation, line 1868): sin/cos polynomial family, as the lifetimes pair; family study above.

## rels/tik/src/tik_05.c

- `tiz_broken` (opt_propagation, line 2480): sin/cos family: frame 0x110 vs 0xd0, 12 vs 9 FPRs (~460 lines); tried: pi/halfpi/threehalves non-const; as literals; coefficients as literals; all literal. Family study above.
- `tiz_broken` (opt_lifetimes, line 2481): sin/cos polynomial family (~460 lines); same literal/non-const shapes as the propagation pair, no change. Family study above.

## rels/tou2/src/tou_03.c

- `screen_tev` (opt_propagation, line 4051): sparkle loop folds buf+sgcol into a pointer; tried: `idx2 = idx + j % 8; idx2 += sgcol` (56 lines); sgrow before sgcol (52); 5 inner-index forms; i/4, i%4 inline.

## rels/yuu/src/yuu_02.c

- `monte_single_init` (opt_propagation, line 199): constant entryCount = 1 folds into li 0x44 (retail li r28,1; mulli); tried: `= 0; ++`; initialiser at the declaration (two positions).

## rels/yuu/src/yuu_03.c

- `yuuminigame4_entry_init` (opt_propagation, line 124): count*sizeof folds to a constant, as in yuu_02; tried: s16 entryCount; `= 0; ++`.
- `evt_ryushi` (opt_lifetimes, line 720): FPR rotation: zSpeed/alphaFactor/entryScale/cameraX in f31.. not f20-f23; tried: randf ranges inline (224 lines); separate xMin/xMax locals (194); rasim (best 9 wrong).

## src/battle/ac/ac_signal_timing.c

- `battleAcMain_SignalTiming` (opt_dead_assignments, line 69): MUST_MATCH dead-initialiser pool trick: the four string addi move into the prologue; tried: initialisers alone; literals at the calls; assignment orders.

## src/battle/battle_audience.c

- `BattleAudience_ApRecoveryBuild` (opt_lifetimes, line 3486): f30/f31 swap between the loop accumulator and the hoisted i2f magic; tried: a separate weightSum (first/last/after rate); block-scoped accumulator (f26).

## src/battle/battle_audience_kinopio.c

- `BattleAudienceCtrlProcessKinopioWait` (opt_propagation, line 285): t = 0x14 folds into cmpwi and (f32)t into a pooled 20.0f; tried: running threshold `t = 0xa; t += 0xa`; t set before / right after irand.
- `BattleAudienceCtrlProcessKinopioWait` (opt_lifetimes, line 286): t/magic/product colour r0/f1/f2, not r27/f2/f0; shows only with propagation off, so not tried on its own.

## src/battle/battle_damage.c

- `BattleSetStatusDamage` (opt_common_subs, line 1018): 2170-line callee-saved reshuffle ((s32)type own web, (s8) extensions merged); tried: s8 locals without casts (122 lines); an `s32 status = type` local (2517).

## src/battle/battle_disp.c

- `btlUnitItemDisp` (opt_dead_assignments, line 922): BtlUnit_GetHeight's dead int->double conversion (two stw, frame 0x30) goes; tried: unused height local; bare (f32)/(f64) statements; `v.y = height`; `v.y +=`; `/ 2`.

## src/battle/battle_event_cmd.c

- `btlevtcmd_GetConsumeItem` (opt_dead_assignments, line 3608): &wp->commandMenu stops being materialised, folds to lwz 0x17b0; tried: tbl for both reads; tbl before/after idx; init vs assignment; a BattleCommandWeapon*.
- `btlevtcmd_PadCheckTrigger`, `btlevtcmd_PadCheckNow` (opt_propagation, line 6670): args[2] loads above evtGetValue, costing a saved register; tried: index before/after mask or chan; args[1] inline; `!= 0`; ternary set.

## src/battle/battle_seq.c

- `battleSortPhaseMoveTable` (opt_propagation, line 2534): wp and count swap r30/r31 (the vreg tie goes to count; the unpropagated work copy puts wp first); tried: using wp directly with no copy (same 30 lines).

## src/battle/battle_seq_command.c

- `BattleCommandDisplay_WeaponMultiItemCancelCheckMain` (opt_propagation, line 1766): cursor = wp + 0x1c50 folds into wp offsets, map shifts (1917 lines); tried: cursor from wp; `cursor + 13`; declared after done/cancel.

## src/battle/battle_seq_end.c

- `btlseqEnd` (opt_lifetimes, line 462): 679-line unit/party/setup register map change across the 670-line function; not attempted beyond removal.
- `btlseqEnd_DispMain` (opt_propagation, line 1156): wp and work swap r30/r31 (the unpropagated param copy puts wp first); tried: no copy (same diff).
- `_lvup_select_object_disp` (opt_common_subs, line 1799): pos2 block FPR order around PSMTXScale(float_0p8, float_0p8, 1) (26 lines); tried: a scale local passed twice; `pos2.x - (8 + 6)`; compound -=/+=; split -=.

## src/battle/battle_stage.c

- `BattleStageDispDark` (opt_common_subs, line 320): load order of the named -640/640/105 constants per DarkBySquare call (18 lines); tried: min/max locals, both orders; only a MATCH_F32 volatile read reloads them.
- `BattleStageLightRelease`, `...SetLightColor/SetWidth/SetPosition/SetTarget` (opt_propagation, line 502): stage/light pointers fold into displacements; tried: `stage = BattleStageGetPtr()` (still folds).
- `BattleStageLightSetUpVec` (opt_propagation, line 632): same stage/light pointer folding as the line-502 region; nothing further tried.
- `getSpline` (opt_loop_invariants, line 716): hoisted i2f magic and 0.0 colour f1/f2, not f2/f0; tried: t set in the loop; `float_0 <= x`; `!(x < 0)`; zero local; while(--i >= 0); 0.0f literal (matches, new pool entry).
- `tou_gamen_screen_tev` (opt_propagation, line 985): second loop folds col into a buf+col pointer (84 lines), twin of tou_03's screen_tev; only removal tried.

## src/battle/battle_stage_object.c

- `_bgset_kemuri_effect` (opt_propagation, line 1663): args and the rodata string base swap r4/r5 (8 lines); tried: *event->args; strings assigned before evtGetValue; wp first; initialised string locals.

## src/battle/battle_unit.c

- `BtlUnit_ControlPoseSoundMain` (opt_propagation, line 2153): (f64)(u32)0x7F folds to 127.0, retail converts at run time; tried: function-scope vol; u8/u16/int/s8/unsigned vol; literal (f64)0x7FU * 0.7 in both arms.

## src/battle/sac/sac_zubastar.c

- `zubastar_disp_bomb` (opt_propagation, line 1087): constants hoist after the MUST_MATCH volatile degree read, so f27..f31 load order and s/t temps change (22 lines); tried: literals in the texcoords; int counters.

## src/driver/envdrv.c

- `envDisp_DepthOfField` (opt_propagation, line 496): sin/cos family: coefficients propagated into every branch, frame +0x20, two more saved FPRs (676 lines); family study above.
- `envDisp_DepthOfField` (opt_lifetimes, line 497): sin/cos family: sv/cv seeds become their own webs hoisted into f22/f23; tried: the seed literal folded into each branch's first step (436 lines). Family study above.
- `envDisp_FF` (opt_propagation, line 941): sin/cos family: one/cosC1/sinC1 propagated into each branch (685 lines, frame 0xf0 -> 0xc0); tried: int counters. Family study above.
- `envDisp_FF` (opt_lifetimes, line 942): sin/cos family: cosv/sinv seed loads split and hoisted, two more FPRs (682 lines); family study above.
- `envSetWater` (opt_propagation, line 1444): tail stores take fresh temps instead of addi in place, inner loop 0x4330/base*32 swap r30/r31 (26 lines); tried: increments before the stores; `x + 1`; `++x`; int locals.
- `envSetWater` (opt_lifetimes, line 1445): flags load and obj split into separate webs (extra mr r4, r3); tried: `flags = obj->flags` instead of re-reading yami->unk0; int for every s32 local.
- `envSetYami` (opt_propagation, line 1634): dat_802f9338 base combines with texMap*4 differently and tail stores take fresh temps (48 lines); tried: int texMtxOff; the envSetWater tail shapes.

## src/driver/mapdrv.c

- `mapUnLoad` (opt_propagation, line 971): activeGroup = i folds to li r0 and the mapWork load is copied to both wp and entry; tried: `activeGroup = i = 0` (fixes i only); entry spellings; five decl orders; int i.
- `mapMain` (opt_common_subs, line 1791): 429 lines of FPR recolouring through the inlined anim/Hermite helpers; tried: int loop counters (no change).
- `spline_getvalue` (opt_lifetimes, line 4290): `t -= h` starts a new web, so knots[lo]/t/h recolour (42 lines); tried: `t -= knots[lo]` both orders; `t = t - knots[lo]`; a k0 local; a `u = t - h` local; int lo/hi/mid.

## src/driver/msgdrv.c

- `msgAnalize` (opt_lifetimes, line 711): the 600-line function recolours, every saved register moves (702 lines); tried: int for every s32 local (no change).

## src/driver/npcdrv.c

- `npcDisp_blur` (optimization_level 2, line 585): push/pop; sin/cos family: npcSinf/npcCosf coefficients loaded per branch, not once (126 lines); tried: rsh_05's moamoa_sin/cos shape; int alpha/i. Family study above.
- `npcCheckHitMarioSide` (opt_propagation, line 1580): sin/cos family: coefficients propagated into every branch (286 lines); family study above.
- `npcCheckHitMarioSide` (opt_lifetimes, line 1581): sin/cos family: the seed splits from its updates, accumulator/pivot FPRs move (192 lines); family study above.

## src/driver/shadowdrv.c

- `shadowCharShadowDisp_Collision` (opt_propagation, line 517): &query is formed at the call, so _filter's lis takes r3 (6 lines); tried: no q pointer; &query in the call; q initialised; query scoped to the hit block; int i.
- `cylinder` (opt_common_subs, line 921): named float_0/float_1 loads CSE'd across the fade-clamp merge (retail reloads), alpha-ramp temps recolour (45 lines); tried: int c.

## src/effect/eff_break.c

- `effBreakMain` (opt_propagation, line 81): data->x/y/z loads into disp colour f0/f1/f0, not f1/f0/f1 (18 lines); tried: no t temp; direct position writes; disp initialised; six x/y/z orders; scale first.
- `effBreakDisp` (opt_common_subs, line 170): named float_1 is CSE'd into f28 (extra saved FPR, frame +0x10, 67 lines), retail reloads it; tried: int i; the fix would extend the volatile float_0 hack.

## src/effect/eff_damage_star.c

- `effDamageStarEntry` (opt_propagation, line 251): sin/cos polynomial family (259-line diff); family study above.
- `effDamageStarEntry` (opt_lifetimes, line 252): sin/cos polynomial family (210-line diff); family study above.

## src/effect/eff_fall.c

- `effFallMain` (opt_lifetimes, line 364): sin/cos family: the inlined effFallSinf/Cosf webs split into f1/f4/f0 instead of staying in f7/f6 (72 lines); family study above.
- `effFallMain` (opt_propagation, line 365): sin/cos family: coefficients rematerialised per branch (290 lines); family study above.

## src/effect/eff_funemizu.c

- `effFunemizuDisp` (opt_lifetimes, line 244): 267-line diff, frame 0x160 -> 0x180 in a large map-lookup/goto function; only removal tried (other pointer shapes hang mwcceppc).

## src/effect/eff_gonbaba_breath.c

- `init_breath` (opt_propagation, line 101): sin/cos family: coefficients rematerialised per branch (327 lines); family study above.

## src/effect/eff_ibuki.c

- `effIbukiDisp` (opt_lifetimes, line 304): effect/cameraId swap r29/r28, cascading into the loop registers (30 lines); tried: camera->viewYaw for the second camGetPtr (189); userdata via param (30); int i/j.

## src/effect/eff_kemuri.c

- `effKemuriDisp` (opt_common_subs, line 302): MUST_MATCH-guarded; the named float_0p5 is CSE'd, so 0.5 and the i2f magic swap f29/f31 (18 lines); only removal tried.

## src/effect/eff_mobj_broken.c

- `effMObjBrokenMain` (opt_lifetimes, line 190): sin/cos family: accumulators split, then a GPR cascade (data r31 vs r30, type, counters; 140 lines); family study above.

## src/effect/n64/eff_balloon_n64.c

- `effBalloonDisp` (opt_dead_assignments, line 181): cam copy folds into the camGetPtr temp, which takes r31 (retail: life r31, cam r28); tried: cam initialised; cam declared last; camGetPtr later (131); no cam local (32).

## src/effect/n64/eff_radiation_n64.c

- `effRadiationN64Entry`, `effRadiationMain` (opt_dead_assignments, line 59): the dead last `++row` lets the r_data[row++] chain fold into offsets; tried: explicit [row]..[row + 4] (142); post-increments with [row + 1] (140).

## src/effect/n64/eff_release_n64.c

- `effReleaseMain` (opt_common_subs, line 172): case 100's element address and the i2f magic address are shared; tried: plain anime_data[...].scale/.alpha (103); an AnimeData* pointer (202); .alpha before s (221).

## src/effect/n64/eff_ripple_n64.c

- `effRippleMain` (optimization_level 2, line 80): push/pop; effect/timer/type colour r30/r29/r28, not r28/r30/r29; tried: subData init; one Vec; `timer - 1 < 0`; `timer = data->timer--`; type first; level 3; rasim (1 wrong).

## src/effect/n64/eff_small_star_n64.c

- `effSmallStarMain` (opt_lifetimes, line 258): sin/cos family: cos wave leaves f5; tried: 24 decl orders of cosC0/cosC2/wave/sq; rasim; a file-local cos inline; block-scoped t/u/s; one-expression polynomial. Family study above.

## src/effect/n64/eff_sstar_n64.c

- `effSstarN64Entry` (opt_common_subs, line 192): the CSE'd named float_0 swaps f0/f1 in the zero tests, dist products and scale; tried: 0.0f literals (141); `scale = dist` in an else; `scale = dist = ...`; decl positions.

## src/event/evt_mobj.c

- `evt_mobj_floatswitch_blue`, `evt_mobj_tornadoswitch_blue` (opt_propagation, line 606): inBattle folds into the call, gp/string-high swap r5/r4; tried: `? TRUE : FALSE`; if/else; no asyncName local; `== FALSE`; initialiser.
- `evt_mobj_floatswitch_red` (opt_propagation, line 765): same inBattle gp/string-high swap as the line-606 region; same shapes tried there.
- `evt_mobj_rideswitch_lightblue` (opt_propagation, line 1159): same inBattle gp/string-high swap as line 606, plus mr r3, r0 one slot later; same shapes tried there.
- `evt_mobj_signboard` (opt_propagation, line 2459): same inBattle gp/string-high swap as the line-606 region; same shapes tried there.
- `evt_mobj_koopa_badgeblk`, `evt_mobj_koopa_hidden_blk` (opt_propagation, line 4172): same inBattle gp/string-high swap as line 606, plus mr r3, r0 one slot later; same shapes tried there.

## src/event/evt_npc.c

- `evt_npc_set_color` (opt_propagation, line 1798): addi r3, npc, 8 (desc) moves after &color (2 lines); tried: description inline; NameToPtr()->description (353); desc set late; no tmp copy (349); tmp/color swapped (14).

## src/event/evt_sub.c

- `evt_sub_get_coin` (opt_propagation, line 653): sin/cos family: coefficients rematerialised per branch (87.63%); tried: npcdrv's npcSinf/npcCosf static-inline form (301 lines). Family study above.

## src/event/evt_telop.c

- `telop` (opt_common_subs, line 149): f1/f2 swap for width-100 vs the i2f magic, f5/f6 for 0.5 vs (f32)msgWidth (22 lines); tried: a limit local; limit plus f32 w; operand order; a/b inline; b before a.

## src/koopa_motion.c

- `kpaStickSpd` (opt_common_subs, line 107): named float_70 stays in a register across both stick clamps, conversion FPRs shift (93 lines); tried: else-if clamp (20); reversed compares; `f30 += 0.002f * stick`.
- `kpa_jump` (opt_common_subs, line 429): second `jumpVelY <= 0` compare loads field/zero into f0/f1 (8 lines); tried: no f1 local; reversed compares; f31/f1 swapped; nested-if; literal 0.0f (19, pool change).
- `kpaSwimMove` (opt_common_subs, line 800): the named float_0 is held in f1 across the function (347 lines); tried: `speed += stick * 0.01`.

## src/mario/mario.c

- `marioCheckMenuDisable` (opt_dead_assignments, line 404): every inlined !marioChkCtrl()/!marioChkKey() folds to extsb. (363 lines); tried: `if (!marioChkCtrl())`; `== FALSE`; an `== 0` local; `!= FALSE`; a cast.
- `marioDispBlur` (opt_dead_assignments, line 2535): the compare skeleton of an inlined sin/cos with unused results is removed (350 lines); nothing tried (keeping it needs a fake use). Family study above.

## src/motion/mot_hammer.c

- `mot_hammer` (opt_common_subs, line 81): the .rodata string base (r31) and mario (r30) swap for the whole function (344 lines); nothing tried (not a local shape).
- `mot_hammer` (opt_dead_assignments, line 83): the pose-time clamp's copy from t is coalesced into r27 (5-line core); tried: `(b <= 10) ? b : 10` (if-converted); reversed ternary; `a = 10`; field-arm ternaries; if/else.
- `mot_hammer2` (opt_propagation, line 405): effUltraHammerEntry's multiTimer load (r4) moves before the float args (6 lines); tried: all args inline (both y + 20.0f orders); only the py__ local; literal 1.0f.

## src/motion/mot_plane.c

- `mot_plane` (opt_propagation, line 159): MUST_MATCH-guarded; sfxId = -1 swaps r3/r4, smoothing constants sink past the ?: branch (90 lines); tried: one-expression smoothing; sfxId via marioGetPtr() (88).

## src/party.c

- `partyPreDisp` (opt_common_subs, line 2818): s16 field_2A is re-tested with extsh. instead of lha/cmpwi (6 lines); tried: merged/negated tests (336); `& 2u`; `0 < field`; `>= 1` (4); an s32 cast/local matches but steers.

## src/party/party_hit.c

- `partySearchHead` (opt_propagation, line 675): MUST_MATCH push/pop beside the guarded volatile px/py/pz; ~90 lines of FPR scheduling; tried: moving dist = *out; offSin/offCos swapped; sincosf before h; x/y/z orders.

## src/party/party_motion.c

- `party_motion_homing_walk` (opt_propagation, line 759): 429 lines: one extra instruction and an f24/f25 rotation through the whole function; not attempted in depth.
- `party_motion_homing_walk`, `party_motion_homing_fly` (opt_common_subs, line 760): ~640 lines: walk keeps `sid = id` as its own web, an extra callee-saved register; tried: `+=` lerps (fix fly's fmadds rows only).
- `party_motion_homing_fly` (opt_loop_invariants, line 1311): the hoisted s32->u32 conversion of id takes r30 ahead of mario; tried: decl orders (rasim: none); `u32 id` + `s32 sid = id` matches, rejected: `sid` is a register-steering copy.
- `party_motion_homing_fly` (opt_dead_assignments, line 1313): movePos addi r4 scheduling and an early hitCheckFilter 1000.0f load (5 lines); tried: natural `&p.z` movePos args (needed as pz temps with DA off).
- `party_motion_behind_mario`, `party_motion_behind_mario_continue` (opt_propagation, line 1722): hd = 1000.0f loads before the y + touch_wall_ct add, plus addi r7 order; shapes tried on beside_mario.
- `party_motion_beside_mario` (opt_propagation, line 2183): same early 1000.0f load and addi r7 order; tried: y + touch_wall_ct in the call; hd before ytmp; y after angleABf; hd initialised (3-24 lines).
- `party_motion_jump` (opt_lifetimes, line 2329): 1038-line diff; the function rests on the documented self-store lever; the matching-era search was not reopened.
- `party_motion_jump` (opt_dead_assignments, line 2331): f4/f5/f6 rotation of the distance/height pair (8 lines); the self-store lever is safe only with DA off; not reopened.

## src/party/party_move.c

- `partyGetTargetDir`, `...Dist`, `...Dist3D`, `...DistYFrom` (opt_common_subs, line 99): id != 0 is CSE'd into r29 (+1 instruction); tried: second halves as L_getTargetPos calls (exact code, but an extra saved register).
- `partyGetTargetDir`, `...Dist`, `...Dist3D`, `...DistYFrom` (opt_lifetimes, line 100): stmw r27 claims one more callee-saved register; tried: the L_getTargetPos call form (still an extra register).
- `partyForceWalk` (opt_propagation, line 1784): speed/up/down FPRs rotate (spd f3 or f2, not f1); tried: the #else partyChgSpeedPose call; one gravity/down local; spd read in the else; no mario local; decl orders.
- `partyGetAppearPos5` (opt_common_subs, line 3164): the named float_50 CSE temp swaps f2/f3 in the two fmadds; tried: 4 operand orders of the products (best 3 lines); `k * s + base`; z before x; moving dist = 150.

## src/pmario_sound.c

- `psndSFXChk` (opt_propagation, line 6521): high-part extrwi takes r3, sfx r4 (retail r6/r3); tried: high before sfx; high inline; both compare orders; u16 high; (u8)id index; pointer arithmetic; % 256; / 256.

## src/sequence/seq_logo.c

- `seq_logoMain` (opt_dead_assignments, line 139): the dead `decode = (u8*)0` stops pinning size/decode to r29/r30, they swap (8-12 lines); tried: dropping the store; 3 decl orders; reversed byte-swap terms.

## src/sound.c

- `_sscallback` (opt_propagation, line 1613): /7 magic folds, n homes in r0, prologue mr order and callee-saved rotation (~130 lines); tried: len2 for size2; out pointers initialised; parameter copy orders; one expression.

## src/unit/party/unit_party_chuchurina.c

- `mono_disp` (opt_common_subs, line 345): repeated reads of the named non-const .sdata2 float_*_8041ddc4..d4 are CSE'd (retail reloads each); not attempted (literals add pool entries).

## Tally

124 remaining regions: 164 after n9-q-o-pragma2 (unchanged by n9-q-o-sincos), less the 40 that became file flags in
n9-q-o-flags. The cause split is the 164-region one except for sin/cos.

By kind:
- opt_propagation: 55
- opt_lifetimes: 33
- opt_common_subs: 19
- opt_dead_assignments: 13
- opt_loop_invariants: 2
- optimization_level 2: 2

By cause family:
- sin/cos polynomial: 35
- named pool constants: 27
- register colouring: 40
- large-function reshuffle: 12
- guarded trick depends on it: 5
- other: 29
