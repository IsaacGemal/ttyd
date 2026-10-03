#pragma once

typedef struct WindowWork WindowWork;

#include <dolphin/types.h>
void winMakeSkipList(WindowWork* work);
typedef struct WinMgrEntry WinMgrEntry;
void itemUseDisp(WinMgrEntry* entry);
void itemUseDisp2(WinMgrEntry* entry);
#include "driver/camdrv.h"
void winItemDisp(CameraId camId, WindowWork* work, s32 idx);
void item_disp(WindowWork* work, f32 x, f32 y);
void winItemInit(WindowWork* work);
void winItemInit2(WindowWork* work);
s32 winItemMain(WindowWork* work);
void winItemMain2(WindowWork* work);
void winItemExit(WindowWork* work);
