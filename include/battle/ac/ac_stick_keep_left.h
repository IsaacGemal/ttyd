#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_StickKeepLeft(BattleWork* work);
s32 battleAcResult_StickKeepLeft(BattleWork* work);
void battleAcDisp_StickKeepLeft(CameraId camId, void* param);
void battleAcDelete_StickKeepLeft(BattleWork* work);
