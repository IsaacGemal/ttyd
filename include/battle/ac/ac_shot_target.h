#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_ShotTarget(BattleWork* work);
s32 battleAcResult_ShotTarget(BattleWork* work);
void battleAcDisp_ShotTarget(CameraId camId, void* param);
void battleAcDelete_ShotTarget(BattleWork* work);
void _ac_rumble_param_set(u32 type, s16* period, s16* timer, f32* amplitude);
