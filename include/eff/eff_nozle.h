#pragma once
typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
EffectEntry* effNozleEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 rotX, f32 rotY);
void effNozleDisp(CameraId camId, void* param);
void effNozleMain(EffectEntry* effect);
