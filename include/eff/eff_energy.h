#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effEnergyEntry(s32 a, s32 b, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6);
#include "driver/camdrv.h"
void effEnergyDisp(CameraId camId, void* param);
void effEnergyMain(EffectEntry* effect);
extern GXColor unk_80423358;
