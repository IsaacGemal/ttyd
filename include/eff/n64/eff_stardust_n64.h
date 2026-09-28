#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effStardustN64Entry(s32 a, f32 b, f32 c, f32 d, f32 e);
void effStardustN64SetScale(EffectEntry* entry, f32 scale);
void effStardustN64SetDrawCam(EffectEntry* entry, s32 cam);
void effStardustN64SetColor(EffectEntry* entry, f32 r, f32 g, f32 b);
