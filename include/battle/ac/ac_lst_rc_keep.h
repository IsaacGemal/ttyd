#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_LstRcKeep(BattleWork* work);
s32 battleAcResult_LstRcKeep(BattleWork* work);
void battleAcDisp_LstRcKeep(CameraId camId, void* param);
void battleAcDelete_LstRcKeep(BattleWork* work);
