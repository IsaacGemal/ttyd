# BtlUnit_snd_se_pos matching attempts

Result: SUCCESS. Target 99.86% -> 100.00%; all 120 functions are 100.00%. The unit is Matching and `python3 configure.py && ninja` passes with `32 files OK`. No commit.

Winning shape: `s16 sum = c; sum += val; val = sum;` before the existing random increment. It reproduces `add r30,r28,r30` without narrowing between the two adds. Both this temporary and equivalent inline helper matched; the local temporary is retained under `MUST_MATCH`, with `val += c` in the natural branch.

## Function variants

- baseline: 99.86%; only `add r30,r30,r28` differs from retail `add r30,r28,r30`.
- wide_sum: BtlUnit_snd_se_pos: 92.39% (276 B); !! mr r27, r4                                 | mr r30, r4                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch | !! mr r29, r7                                 | mr r28, r7                                 arg_mismatch
- wide_sum_reverse: BtlUnit_snd_se_pos: 92.83% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- wide_val_cfirst: BtlUnit_snd_se_pos: 93.19% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- c_compound: BtlUnit_snd_se_pos: 95.36% (276 B); !! add r30, r28, r30                          | add r28, r28, r30                          arg_mismatch | !!                                            | extsh r0, r28                              insert | !!                                            | mr r4, r29                                 insert
- c_compound_wide: BtlUnit_snd_se_pos: 95.58% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- single_cfirst: BtlUnit_snd_se_pos: 92.39% (276 B); !! mr r27, r4                                 | mr r30, r4                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch | !! mr r29, r7                                 | mr r28, r7                                 arg_mismatch
- minus_neg: BtlUnit_snd_se_pos: 98.33% (276 B); !! add r30, r28, r30                          | add r0, r30, r28                           arg_mismatch | !!                                            | extsh r30, r0                              insert
- zero_minus: BtlUnit_snd_se_pos: 98.33% (276 B); !! add r30, r28, r30                          | add r0, r30, r28                           arg_mismatch | !!                                            | extsh r30, r0                              insert
- minus_neg_val: BtlUnit_snd_se_pos: 97.03% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- cast_c32: BtlUnit_snd_se_pos: 98.33% (276 B); !! add r30, r28, r30                          | add r0, r30, r28                           arg_mismatch | !!                                            | extsh r30, r0                              insert
- cast_cu32: BtlUnit_snd_se_pos: 98.33% (276 B); !! add r30, r28, r30                          | add r0, r30, r28                           arg_mismatch | !!                                            | extsh r30, r0                              insert
- cast_cu16: BtlUnit_snd_se_pos: 94.78% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- cast_cu16_assign: BtlUnit_snd_se_pos: 94.13% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- assign_u16: BtlUnit_snd_se_pos: 95.58% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- assign_s16: BtlUnit_snd_se_pos: 97.03% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- assign_nested: BtlUnit_snd_se_pos: 94.13% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- add_zero: BtlUnit_snd_se_pos: 98.33% (276 B); !! add r30, r28, r30                          | add r0, r30, r28                           arg_mismatch | !!                                            | extsh r30, r0                              insert
- comma_c: BtlUnit_snd_se_pos: 99.86% (276 B); !! add r30, r28, r30                          | add r30, r30, r28                          arg_mismatch
- c_nested_assign: BtlUnit_snd_se_pos: 97.03% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- c_u16_alias: BtlUnit_snd_se_pos: 93.48% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- c_s16_alias: BtlUnit_snd_se_pos: 100.00% (276 B); no instruction differences
- same_pointer: BtlUnit_snd_se_pos: 97.03% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- pointer_compound: BtlUnit_snd_se_pos: 99.86% (276 B); !! add r30, r28, r30                          | add r30, r30, r28                          arg_mismatch
- cast_lvalue_unsigned: BtlUnit_snd_se_pos: 83.01% (276 B); !!                                            | li r0, 0x0                                 insert | !! stmw r26, 0x18(r1)                         | stmw r27, 0x1c(r1)                         arg_mismatch | !! mr r27, r4                                 | mr r30, r4                                 arg_mismatch
- neg_self_compound: BtlUnit_snd_se_pos: 92.03% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- inline_s16_plain_c_val: BtlUnit_snd_se_pos: 97.03% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- inline_s16_plain_val_c: BtlUnit_snd_se_pos: 97.10% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- inline_s16_compound_c_val: BtlUnit_snd_se_pos: 100.00% (276 B); no instruction differences
- inline_s16_compound_val_c: BtlUnit_snd_se_pos: 99.86% (276 B); !! add r30, r28, r30                          | add r30, r30, r28                          arg_mismatch
- inline_s16_wide_c_val: BtlUnit_snd_se_pos: 94.13% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- inline_s16_wide_val_c: BtlUnit_snd_se_pos: 94.13% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- inline_u16_plain_c_val: BtlUnit_snd_se_pos: 95.58% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- inline_u16_plain_val_c: BtlUnit_snd_se_pos: 95.65% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- inline_u16_compound_c_val: BtlUnit_snd_se_pos: 95.58% (276 B); !! mr r27, r4                                 | mr r29, r4                                 arg_mismatch | !! mr r31, r5                                 | mr r30, r5                                 arg_mismatch | !! mr r28, r6                                 | mr r27, r6                                 arg_mismatch
- local_static_const: MarioSt/battle/battle_unit: 119/120 functions at 100%; .rodata: identical (32 bytes); .data: identical (456 bytes); .sdata: identical (0 bytes); .sdata2: original 48 bytes, built 44 bytes;   +0x2c  original 00000000  built --------; dol: 0 bytes differ; 
- local_static_volatile: MarioSt/battle/battle_unit: 119/120 functions at 100%; .rodata: identical (32 bytes); .data: identical (456 bytes); .sdata: identical (0 bytes); .sdata2: original 48 bytes, built 44 bytes;   +0x2c  original 00000000  built --------; dol: 0 bytes differ; 
- local_factor: MarioSt/battle/battle_unit: 119/120 functions at 100%; .rodata: identical (32 bytes); .data: identical (456 bytes); .sdata: identical (0 bytes); .sdata2: original 48 bytes, built 44 bytes;   +0x2c  original 00000000  built --------; dol: 0 bytes differ; 
- factor_block: BtlUnit_ControlPoseSoundMain: 95.90% (700 B); 13 instruction differences
- factor_block_first: BtlUnit_ControlPoseSoundMain: 95.90% (700 B); 13 instruction differences
- factor_assign: BtlUnit_ControlPoseSoundMain: 95.90% (700 B); 13 instruction differences
- double_vol: BtlUnit_ControlPoseSoundMain: 99.94% (700 B); 1 instruction differences
- factor_before_convert: BtlUnit_ControlPoseSoundMain: 95.90% (700 B); 13 instruction differences
- convert_before_factor: BtlUnit_ControlPoseSoundMain: 99.77% (700 B); 5 instruction differences
- double_vol_mult: compile failed (declaration placed after a statement); no score. check.py reports a misleading timeout because the echoed command contains its timeout marker.
- factor_reverse: BtlUnit_ControlPoseSoundMain: 95.93% (700 B); 13 instruction differences
- block_static_vol: BtlUnit_ControlPoseSoundMain: 95.93% (700 B); 13 instruction differences
- converted_literal_reverse: BtlUnit_ControlPoseSoundMain: 99.94% (700 B)
- converted_mul_assign: BtlUnit_ControlPoseSoundMain: 99.94% (700 B)
- double_compound: BtlUnit_ControlPoseSoundMain: 99.94% (700 B); 2 instruction differences
- factor_compound: BtlUnit_ControlPoseSoundMain: 99.74% (700 B); 6 instruction differences
- factor_alias_assign: BtlUnit_ControlPoseSoundMain: 99.77% (700 B); 5 instruction differences
- converted_compound_local: BtlUnit_ControlPoseSoundMain: 99.77% (700 B); 5 instruction differences
- factor_temp_expr: BtlUnit_ControlPoseSoundMain: 99.80% (700 B); 5 instruction differences
- converted_assign_literal: BtlUnit_ControlPoseSoundMain: 99.94% (700 B); 1 instruction differences
- local_static_ptr: BtlUnit_ControlPoseSoundMain: 100.00% (700 B); 0 instruction differences; .rodata: identical (32 bytes); .data: identical (456 bytes); .sdata: identical (0 bytes); .sdata2: original 48 bytes, built 44 bytes;   +0x2c  original 00000000  built --------; dol: 0 bytes differ; 

## Additional unit link/layout variants

- annotated winning temporary: 120/120 functions at 100%; first Matching build fails on undefined `_enemy_can_use_item_table`; Matching flag reverted immediately.
- split existing paired `dont_inline` regions and reorder functions: compile rejected because existing field typedefs now follow their users; no score (check.py misleadingly reports timeout).
- hoist existing typedefs, define the exact 33-word retail item table, fix every existing sentinel to -250000000.0f, remove explicit unused conversion/pow doubles: 120/120 functions at 100%; .data identical, remaining pool/section order differs.
- all named const floats, literal 0.7, const colour template: 118/120 at 100%; Entry 99.31%, ControlPoseSoundMain 95.93%; constant hoisting and duplicate pooled zero/one remain.
- named zero/one at literal sites, volatile const colour, global 0.7 definition: first compile rejected by an accidental substring replacement inside 360.0f; corrected immediately, then 117/120 at 100% (Entry 96.87%, GetHitPos 83.23%, ControlPoseSoundMain 95.93%); named immutable floats change hoisting.
- all literal floats with local `half = 0.5f`, mutable colour in retail .sdata2, static local 0.7 addressed through `*(const f64*)&...`: 120/120 at 100%; .rodata, .data, .sdata identical; remaining four-byte .sdata2 zero is supplied by linker alignment.
- inline_u16_compound_val_c: interrupted before the checker returned; no result, discarded.
- final MUST_MATCH annotations and natural branches: 120/120 at 100%; function order verified, rebuilt DOL has zero differing bytes, full build checksum passes (32 files OK); natural #else branches also compile with MWCC without MUST_MATCH.

The first `uv` pool-plan invocation could not use its default cache under the sandbox. The installed Python already provides pyelftools, so `python3 tools/pool_plan.py battle/battle_unit` was used successfully. No tools or headers were modified. The baseline's explicit constants, incorrect sentinel, missing table and source order prevented linking independently of the target add.

Changed files: src/battle/battle_unit.c, configure.py (Matching flip only), ATTEMPTS.md.

Original and rebuilt DOL SHA-1: `cf559d97fef1b3efb8788126250aee88f0491410`.

## Final function scores

```text
MarioSt/battle/battle_unit: 120/120 functions at 100%
  100.00%      8 B  BtlUnit_GetUnitId
  100.00%      8 B  BtlUnit_GetWidth
  100.00%      8 B  BtlUnit_GetHeight
  100.00%      8 B  BtlUnit_SetHeight
  100.00%      8 B  BtlUnit_SetFallAccel
  100.00%      8 B  BtlUnit_SetPartsFallAccel
  100.00%      8 B  BtlUnit_SetMoveSpeed
  100.00%      8 B  BtlUnit_SetPartsMoveSpeed
  100.00%      8 B  BtlUnit_SetJumpSpeed
  100.00%      8 B  BtlUnit_GetBelong
  100.00%      8 B  BtlUnit_GetTotalHitDamage
  100.00%     12 B  BtlUnit_CheckUnitFlag
  100.00%     12 B  BtlUnit_GetHitDamage
  100.00%     12 B  BtlUnit_SetTotalHitDamage
  100.00%     16 B  BtlUnit_SetPos
  100.00%     16 B  BtlUnit_SetPartsPos
  100.00%     16 B  BtlUnit_SetHitOffset
  100.00%     16 B  BtlUnit_SetHitCursorOffset
  100.00%     16 B  BtlUnit_SetHomePos
  100.00%     16 B  BtlUnit_SetPartsHomePos
  100.00%     16 B  BtlUnit_SetPartsRotate
  100.00%     16 B  BtlUnit_SetPartsBaseRotate
  100.00%     16 B  BtlUnit_OnStatusFlag
  100.00%     16 B  BtlUnit_OffStatusFlag
  100.00%     16 B  BtlUnit_OnUnitFlag
  100.00%     16 B  BtlUnit_OffUnitFlag
  100.00%     24 B  BtlUnit_PoseSoundInit
  100.00%     28 B  BtlUnit_GetPos
  100.00%     28 B  BtlUnit_GetPartsPos
  100.00%     28 B  BtlUnit_GetHomePos
  100.00%     28 B  BtlUnit_GetRotate
  100.00%     28 B  BtlUnit_GetPartsRotate
  100.00%     28 B  BtlUnit_GetBaseRotate
  100.00%     28 B  BtlUnit_GetPartsBaseRotate
  100.00%     28 B  BtlUnit_GetScale
  100.00%     28 B  BtlUnit_GetPartsOffsetPos
  100.00%     28 B  BtlUnit_CheckStatusFlag
  100.00%     32 B  BtlUnit_CanGuardStatus
  100.00%     36 B  BtlUnit_SetAnim
  100.00%     36 B  BtlUnit_RecoverHp
  100.00%     40 B  BtlUnit_AddPos
  100.00%     40 B  BtlUnit_AddPartsPos
  100.00%     40 B  BtlUnit_AddHomePos
  100.00%     40 B  BtlUnit_AddPartsRotateOffset
  100.00%     40 B  BtlUnit_AddPartsOffsetPos
  100.00%     44 B  BtlUnit_GetEnemyBelong
  100.00%     48 B  BtlUnit_SetSeMode
  100.00%     52 B  BtlUnit_SetRotateOffset
  100.00%     52 B  BtlUnit_SetPartsRotateOffset
  100.00%     52 B  BtlUnit_SetBaseScale
  100.00%     52 B  BtlUnit_SetScale
  100.00%     52 B  BtlUnit_SetPartsBaseScale
  100.00%     52 B  BtlUnit_SetPartsScale
  100.00%     52 B  BtlUnit_SetOffsetPos
  100.00%     52 B  BtlUnit_SetPartsOffsetPos
  100.00%     52 B  BtlUnit_SetDispOffset
  100.00%     52 B  BtlUnit_SetPartsDispOffset
  100.00%     52 B  BtlUnit_SetMoveStartPos
  100.00%     52 B  BtlUnit_SetPartsMoveStartPos
  100.00%     52 B  BtlUnit_SetMoveCurrentPos
  100.00%     52 B  BtlUnit_SetPartsMoveCurrentPos
  100.00%     52 B  BtlUnit_SetMoveTargetPos
  100.00%     52 B  BtlUnit_SetPartsMoveTargetPos
  100.00%     60 B  BtlUnit_CheckShadowGuard
  100.00%     64 B  BtlUnit_GetPartsPtr
  100.00%     64 B  BtlUnit_GetBodyPartsId
  100.00%     64 B  BtlUnit_GetData
  100.00%     64 B  BtlUnit_CheckData
  100.00%     68 B  BtlUnit_GetFp
  100.00%     68 B  BtlUnit_GetMaxFp
  100.00%     68 B  BtlUnit_EnemyItemCanUseCheck
  100.00%     72 B  BtlUnit_SetBodyAnimType
  100.00%     72 B  BtlUnit_ChangeTalkAnim
  100.00%     72 B  BtlUnit_ChangeStayAnim
  100.00%     76 B  BtlUnit_Init
  100.00%     76 B  BtlUnit_AddScale
  100.00%     76 B  BtlUnit_AddPartsScale
  100.00%     76 B  BtlUnit_AddPartsDispOffset
  100.00%     76 B  _CheckMoveCount
  100.00%     76 B  BtlUnit_SetBodyAnim
  100.00%     84 B  BtlUnit_ClearStatus
  100.00%     84 B  BtlUnit_SetFp
  100.00%     84 B  BtlUnit_SetMaxFp
  100.00%     92 B  BtlUnit_snd_se
  100.00%     96 B  BtlUnit_SetBaseRotate
  100.00%    108 B  BtlUnit_CheckWeaponCost
  100.00%    116 B  BtlUnit_GetGuardKouraPtr
  100.00%    120 B  BtlUnit_GetCoin
  100.00%    124 B  BtlUnit_CheckStatus
  100.00%    128 B  BtlUnit_GetPoseNameFromType
  100.00%    128 B  BtlUnit_SetAnimType
  100.00%    140 B  BtlUnit_SetCommandAnimPose
  100.00%    144 B  BtlUnit_CanActStatus
  100.00%    148 B  BtlUnit_RecoverFp
  100.00%    176 B  BtlUnit_HpGaugeInit
  100.00%    180 B  BtlUnit_SetRotate
  100.00%    184 B  BtlUnit_AddPartsRotate
  100.00%    184 B  BtlUnit_GetACPossibility
  100.00%    188 B  BtlUnit_PayWeaponCost
  100.00%    212 B  BtlUnit_AddRotate
  100.00%    212 B  BtlUnit_SetParamToPouch
  100.00%    224 B  BtlUnit_GetPartsWorldPos
  100.00%    224 B  BtlUnit_GetExp
  100.00%    228 B  BtlUnit_GetTalkTogePos
  100.00%    232 B  BtlUnit_Spawn
  100.00%    248 B  BtlUnit_ResetMoveStatus
  100.00%    256 B  BtlUnit_CheckPinchStatus
  100.00%    264 B  BtlUnit_SetParamFromPouch
  100.00%    276 B  BtlUnit_snd_se_pos
  100.00%    292 B  BtlUnit_GetHitPos
  100.00%    332 B  BtlUnit_ReviseHpFp
  100.00%    332 B  BtlUnit_LoadSeMode
  100.00%    344 B  BtlUnit_GetWeaponCost
  100.00%    452 B  BtlUnit_GetStatus
  100.00%    472 B  BtlUnit_Delete
  100.00%    592 B  BtlUnit_HpGaugeMain
  100.00%    700 B  BtlUnit_ControlPoseSoundMain
  100.00%    712 B  BtlUnit_CheckRecoveryStatus
  100.00%    904 B  BtlUnit_SetStatus
  100.00%   1716 B  BtlUnit_Entry
```
