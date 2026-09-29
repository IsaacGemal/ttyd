#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effDogabonAttackN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale);
void effDogabonAttackMain(EffectEntry* effect);
