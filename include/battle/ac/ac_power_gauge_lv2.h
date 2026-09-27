#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_PowerGaugeLv2(BattleWork* work);
s32 battleAcResult_PowerGaugeLv2(BattleWork* work);
void battleAcDisp_PowerGaugeLv2(CameraId camId, void* param);
void battleAcDelete_PowerGaugeLv2(BattleWork* work);
