#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
#include "driver/camdrv.h"
void effMagic1Disp(CameraId camId, EffectEntry* effect);
extern const f32 float_0p83_8041f69c;
extern const f32 float_0p9_8041f6a0;
EffectEntry* effMagic1N64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
void effMagic1Main(EffectEntry* effect);
