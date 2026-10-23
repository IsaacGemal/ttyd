#pragma once

#include "driver/camdrv.h"
#include "battle/battle_database_common.h"

void mono_disp(CameraId cameraId, void* param);
void mono_capture(CameraId cameraId, void* param);

// this file's .sdata2 (defined in config/G8MJ01/symbols.txt)
extern u32 dat_8041ddb8;
extern BattleWeapon partyWeapon_ChuchurinaNormalAttackLeftLast;
extern BattleWeapon partyWeapon_ChuchurinaItemSteal;
extern BattleWeapon partyWeapon_ChuchurinaMadowaseAttack;
extern BattleWeapon partyWeapon_ChuchurinaKiss;
extern s32 btldefaultevt_avoid_counter_event[];
extern s32 lbl_80003500[];
extern s32 evt_audience_acrobat_notry[];
extern s32 subset_attack_ready_jump_event[];
extern s32 unk_evt_803510ec[];
