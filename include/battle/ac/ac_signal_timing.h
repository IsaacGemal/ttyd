#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_SignalTiming(BattleWork* work);
s32 battleAcResult_SignalTiming(BattleWork* work);
void battleAcDisp_SignalTiming(CameraId camId, void* param);
void battleAcDelete_SignalTiming(BattleWork* work);
