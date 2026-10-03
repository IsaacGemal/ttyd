#pragma once

#include <dolphin/types.h>
s32 marioGetHammerLevel(void);
s32 marioChkHammer2(void);
f32 marioGetAngleKaitenHammer(void);
struct HitEntry;
struct HitEntry* hitEstimate(s32* type);
void mot_hammer(void);
void mot_hammer2(void);
void mot_ham2_post(void);
