#pragma once

#include <dolphin/types.h>
#include "battle/battle.h"

s32 battleAcMain_TimingA(BattleWork* work);
s32 battleAcResult_TimingA(BattleWork* work);
void battleAcDisp_TimingA(CameraId camId, void* param);
void battleAcDelete_TimingA(BattleWork* work);
s32 battleAcTimingA_GetSuccessFrame(BattleWork* wp);
