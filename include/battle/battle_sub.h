#pragma once

#include "battle/battle.h"
#include "manager/evtmgr.h"

void btlsubResetMoveColorLvAll(BattleWork* work);
s32 BattleTransID(EventEntry* event, s32 type);
f32 sinfd(f32 deg);
f32 cosfd(f32 angle);
f32 intpl_sub(s32 mode, s32 currStep, s32 steps, f32 start, f32 end);
void btlMovePos(f32* x, f32* z, f32 speed, f32 angle);
