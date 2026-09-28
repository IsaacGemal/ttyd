#pragma once

#include "win/win_main.h"
#include "driver/camdrv.h"

#include <dolphin/types.h>

void winPartyDisp(CameraId camId, WindowWork* work, s32 partyIdx);
void winPartyMain2(WindowWork* work);
s32 winPartyMain(WindowWork* work);
void winPartyExit(WindowWork* work);
void winPartyInit2(WindowWork* work);
void winPartyInit(WindowWork* work);
