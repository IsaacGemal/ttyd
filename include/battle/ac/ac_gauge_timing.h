#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_GaugeTiming(BattleWork* work);
s32 battleAcResult_GaugeTiming(BattleWork* work);
void battleAcDisp_GaugeTiming(CameraId camId, void* param);
void battleAcDelete_GaugeTiming(BattleWork* work);
