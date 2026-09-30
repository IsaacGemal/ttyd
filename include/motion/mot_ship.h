#pragma once

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>

// Event scripts in this unit's .data (not yet decompiled).
extern u32 sound_evt[];
extern u32 sound_evt2[];

void marioReInit_ship(void);
s32 marioShipChanging(void);
s32 marioChkShipPlace(HitEntry* hit);
s32 N_marioShipGetDir(void);
void marioShipForceStop(void);
s32 marioChkSwirlPower(void);
void marioSetSwirlPower(s32 msec, f32 a, f32 b);
