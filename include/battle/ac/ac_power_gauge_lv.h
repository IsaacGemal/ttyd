#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_PowerGaugeLv(BattleWork* work);
s32 battleAcResult_PowerGaugeLv(BattleWork* work);
void battleAcDisp_PowerGaugeLv(CameraId camId, void* param);
void battleAcDelete_PowerGaugeLv(BattleWork* work);
