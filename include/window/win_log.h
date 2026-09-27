#pragma once

#include <dolphin/types.h>
#include "battle/battle_unit.h"

// Tattle ("monosiri") viewer window (win_log in the original; no source file yet).
void* monosiriInit(BattleUnitType kind, f32 x, f32 y);
void monosiriMain(void* work);
void monosiriClose(void* work);
BOOL monosiriCloseWait(void* work);
void monosiriExit(void* work);
