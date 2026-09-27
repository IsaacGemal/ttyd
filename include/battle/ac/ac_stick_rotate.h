#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_StickRotate(BattleWork* work);
s32 battleAcResult_StickRotate(BattleWork* work);
void battleAcDisp_StickRotate(CameraId camId, void* param);
void battleAcDelete_StickRotate(BattleWork* work);
