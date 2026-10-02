#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_GaugeTiming2(BattleWork* work);
s32 battleAcResult_GaugeTiming2(BattleWork* work);
void battleAcDisp_GaugeTiming2(CameraId camId, void* param);
void battleAcDelete_GaugeTiming2(BattleWork* work);
extern const f64 double_6_80300058;
extern const f64 double_to_int_80300060;
extern const f64 double_to_int_mask_80300068;
