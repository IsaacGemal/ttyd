#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_PendulumCraneTiming(BattleWork* work);
s32 battleAcResult_PendulumCraneTiming(BattleWork* work);
void battleAcDisp_PendulumCraneTiming(CameraId camId, void* param);
void battleAcDelete_PendulumCraneTiming(BattleWork* work);
// .sdata2 constants this unit defines after its code.
