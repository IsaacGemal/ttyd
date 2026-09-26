#pragma once

#include <dolphin/types.h>
#include "battle/battle.h"

s32 battleAcMain_ButtonDown(BattleWork* work);
s32 battleAcResult_ButtonDown(BattleWork* work);
void battleAcDisp_ButtonDown(CameraId camId, void* param);
void battleAcDelete_ButtonDown(BattleWork* work);
