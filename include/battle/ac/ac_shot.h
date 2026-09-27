#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_Shot(BattleWork* work);
s32 battleAcResult_Shot(BattleWork* work);
void battleAcDisp_Shot(CameraId camId, void* param);
void battleAcDelete_Shot(BattleWork* work);
