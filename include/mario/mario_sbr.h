#pragma once

#include <dolphin/types.h>

void marioSetBottomlessResetPosition(f32 x, f32 y, f32 z);
f32 revise360(f32 angle);
f32 toMovedir(f32 angle);
void marioSetForceMove(s32 timer, f32 direction, f32 speed);
void marioAdjustMoveDir(void);
