#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

// .sdata2 float constants owned by this unit.
extern const f32 float_0p5_8042008c;
extern const f32 float_6p2832_80420090;
extern const f32 float_360_80420094;
extern const f32 float_2_80420098;
extern const f32 float_255_8042009c;
extern const f32 float_100_804200a0;
extern const f32 float_0p2_804200a4;
extern const f32 float_80_804200a8;

EffectEntry* effThunderflareN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
void effThunderflareMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effThunderflareDisp(CameraId camId, void* param);
