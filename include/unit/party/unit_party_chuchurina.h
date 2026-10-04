#pragma once

#include "driver/camdrv.h"

void mono_disp(CameraId cameraId, void* param);
void mono_capture(CameraId cameraId, void* param);

// this file's .sdata2 (defined in config/G8MJ01/symbols.txt)
extern const f32 float_910_8041ddbc;
extern f32 float_0_8041ddc0;
extern f32 float_neg304_8041ddc4;
extern f32 float_240_8041ddc8;
extern f32 float_304_8041ddcc;
extern f32 float_1_8041ddd0;
extern f32 float_neg240_8041ddd4;
extern u32 dat_8041ddb8;
extern u32 partyWeapon_ChuchurinaNormalAttackLeftLast[48];
extern u32 partyWeapon_ChuchurinaItemSteal[48];
extern u32 partyWeapon_ChuchurinaMadowaseAttack[48];
extern u32 partyWeapon_ChuchurinaKiss[48];
extern s32 btldefaultevt_avoid_counter_event[];
extern s32 evt_audience_acrobat_notry[];
extern s32 lbl_80003500[];
extern s32 negone_one[];
extern s32 one_negone[];
extern s32 subset_attack_ready_jump_event[];
extern s32 unk_evt_803510ec[];
extern s32 weaponGetACOutputParam[];
