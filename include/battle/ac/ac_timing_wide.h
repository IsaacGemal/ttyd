#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_TimingWide(BattleWork* work);
s32 battleAcResult_TimingWide(BattleWork* work);
void battleAcDisp_TimingWide(CameraId camId, void* param);
void battleAcDelete_TimingWide(BattleWork* work);
