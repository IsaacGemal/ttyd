#pragma once

#include "battle/battle.h"
#include "manager/evtmgr.h"

void btlsubResetMoveColorLvAll(BattleWork* work);
s32 BattleTransID(EventEntry* event, s32 type);
f32 sinfd(f32 deg);
f32 cosfd(f32 angle);
f32 atan2f_safety(f32 y, f32 x);
f32 intpl_sub(s32 mode, s32 currStep, f32 start, f32 end, s32 steps);
// Callers write (mode, currStep, steps, start, end). The original evaluates the two f32 arguments before the step
// counters, which is what the argument order of the declaration above gives; the registers are the same either way.
#define intpl_sub(mode, currStep, steps, start, end) intpl_sub(mode, currStep, start, end, steps)
void btlMovePos(f32* x, f32* z, f32 speed, f32 angle);
s32 BtlCompForwardLv(s32 direction, f32 dist);
