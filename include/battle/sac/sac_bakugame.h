#pragma once

#include <dolphin/types.h>
#include "battle/battle_ac.h"

//BattleACExtraParams* GetBakuGamePtr(void);
typedef struct BakuGameWork BakuGameWork;
BakuGameWork* GetBakuGamePtr(void);
void bakuGameDisp3D(CameraId camId, void* param);
void bakuGameDispStar(CameraId camId, void* param);
s32 bakuGameAudienceCanThrowPos(s32 id);
BOOL bakuGamePartyExist(BattleUnit* unit);
