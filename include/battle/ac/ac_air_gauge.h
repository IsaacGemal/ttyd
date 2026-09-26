#pragma once

#include <dolphin/types.h>
#include "battle/battle.h"

s32 battleAcMain_AirGauge(BattleWork* work);
s32 battleAcResult_AirGauge(BattleWork* work);
void battleAcDisp_AirGauge(CameraId camId, void* param);
void battleAcDelete_AirGauge(BattleWork* work);
