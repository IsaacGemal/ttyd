#pragma once
#include <dolphin/types.h>
typedef struct EffectEntry EffectEntry;
EffectEntry* effBombN64Entry(s32 type, f32 x, f32 y, f32 z);
void effBombN64SetCamId(EffectEntry* entry, s32 camId);
