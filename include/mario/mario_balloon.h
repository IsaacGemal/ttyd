#pragma once

#include <dolphin/types.h>
void marioBaloonInit(void);
void marioBalloonOn(s32 type);
void marioBalloonOff(void);
void marioBalloonMain(void);
struct HitEntry;
BOOL chkBalloon(struct HitEntry* hit);
