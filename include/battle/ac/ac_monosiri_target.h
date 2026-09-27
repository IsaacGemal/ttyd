#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_MonosiriTarget(BattleWork* work);
s32 battleAcResult_MonosiriTarget(BattleWork* work);
void battleAcDisp_MonosiriTarget(CameraId camId, void* param);
void battleAcDelete_MonosiriTarget(BattleWork* work);
