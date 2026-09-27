#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_PowerGauge(BattleWork* work);
s32 battleAcResult_PowerGauge(BattleWork* work);
void battleAcDisp_PowerGauge(CameraId camId, void* param);
void battleAcDelete_PowerGauge(BattleWork* work);
