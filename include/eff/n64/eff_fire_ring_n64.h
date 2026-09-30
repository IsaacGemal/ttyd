#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effFireRingN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
void effFireRingMain(EffectEntry* effect);
#include "driver/camdrv.h"
// .sdata2 constants shared with the display code, named as in the original unit.
extern const f32 float_6p2832_8041ee74;
extern const f32 float_360_8041ee78;
extern const f32 float_10_8041ee7c;
extern const f32 float_128_8041ee80;
void effFireRingDisp(CameraId camId, void* param);
