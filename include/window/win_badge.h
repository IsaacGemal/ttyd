#pragma once

typedef struct WindowWork WindowWork;

#include <dolphin/types.h>
void winMakeEquipList(WindowWork* work);
void winBadgeMain2(WindowWork* work);
s32 winBadgeMain(WindowWork* work);
void mario_change(WindowWork* work, s32 badgeId);
void winBadgeExit(void);
void winBadgeInit2(WindowWork* work);
void winBadgeInit(WindowWork* work);
#include "driver/camdrv.h"
void winBadgeDisp(CameraId camId, void* param, s32 idx);
void badge_disp(WindowWork* work, f32 x, f32 y);
