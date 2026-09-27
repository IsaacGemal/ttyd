#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_Repeatedly(BattleWork* work);
s32 battleAcResult_Repeatedly(BattleWork* work);
void battleAcDisp_Repeatedly(CameraId camId, void* param);
void battleAcDelete_Repeatedly(BattleWork* work);
