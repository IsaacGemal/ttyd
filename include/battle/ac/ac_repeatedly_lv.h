#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_RepeatedlyLv(BattleWork* work);
s32 battleAcResult_RepeatedlyLv(BattleWork* work);
void battleAcDisp_RepeatedlyLv(CameraId camId, void* param);
void battleAcDelete_RepeatedlyLv(BattleWork* work);
