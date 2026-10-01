#pragma once

#include <dolphin/types.h>
#include "battle/battle_unit.h"

// Tattle ("monosiri") viewer window (win_log in the original; no source file yet).
void* monosiriInit(BattleUnitType kind, f32 x, f32 y);
void monosiriMain(void* work);
void monosiriClose(void* work);
BOOL monosiriCloseWait(void* work);
void monosiriExit(void* work);
#include "driver/dispdrv.h"
void monosiri_disp(CameraId camId, void* param);
void capture(CameraId camId, void* param);
typedef struct WindowWork WindowWork;
void monoshiriGX(WindowWork* work, f32 x, f32 y);
void winLogDisp(CameraId camId, WindowWork* work, s32 idx);
void mapGX(WindowWork* work, f32 x, f32 y);
