#pragma once

#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>

EffectEntry* effIrekaeruEntry(s32 type, f32 x, f32 y, f32 z);

void effIrekaeruMain(EffectEntry* effect);

void effIrekaeruDisp(CameraId camId, void* param);
