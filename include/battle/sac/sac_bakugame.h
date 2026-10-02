#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "battle/battle_ac.h"

// Same-layout blobs used to copy the two button tables out of their named .rodata objects.
typedef struct BakuGamePositions { Vec v[3]; } BakuGamePositions;
typedef struct BakuGamePerms { s32 v[6][3]; } BakuGamePerms;

//BattleACExtraParams* GetBakuGamePtr(void);
typedef struct BakuGameWork BakuGameWork;
BakuGameWork* GetBakuGamePtr(void);
void bakuGameDisp3D(CameraId camId, void* param);
void bakuGameDispStar(CameraId camId, void* param);
s32 bakuGameAudienceCanThrowPos(s32 id);
BOOL bakuGamePartyExist(BattleUnit* unit);
