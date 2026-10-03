#pragma once

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>
#include <dolphin/mtx.h>
s32 checkHarbor(Vec* out);
void shipMove(void);
void marioReInit_ship(void);
void marioForceShipAnime(void);
s32 marioShipChanging(void);
s32 marioChkShipPlace(HitEntry* hit);
s32 N_marioShipGetDir(void);
void marioShipForceStop(void);
s32 marioChkSwirlPower(void);
void marioSetSwirlPower(s32 msec, f32 a, f32 b);
void marioShipSetDispDir(void);
s32 marioChkShipMoveMode(void);
void marioShipChgFall(void);
