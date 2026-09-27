#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_CraneTiming(BattleWork* work);
s32 battleAcResult_CraneTiming(BattleWork* work);
void battleAcDisp_CraneTiming(CameraId camId, void* param);
void battleAcDelete_CraneTiming(BattleWork* work);
