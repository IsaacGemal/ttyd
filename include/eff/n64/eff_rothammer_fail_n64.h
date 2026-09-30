#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effRotHammerFailN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5);
void effRotHammerFailMain(EffectEntry* effect);
void effRotHammerFailDisp(CameraId cameraId, void* param);
// .sdata2/.rodata constants named as in the original unit.
extern const f32 float_7_8041fbe4;
extern const f32 float_0p1_8041fbe8;
extern const f32 float_0p04_8041fbec;
extern const f32 float_0p9_8041fbf0;
