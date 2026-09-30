#pragma once

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>
void marioReInit_ship(void);
s32 marioShipChanging(void);
s32 marioChkShipPlace(HitEntry* hit);
s32 N_marioShipGetDir(void);
void marioShipForceStop(void);
s32 marioChkSwirlPower(void);
void marioSetSwirlPower(s32 msec, f32 a, f32 b);
