#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
struct EffectEntry;
EffectEntry* effRecoveryEntry(s32 type, s32 count, f32 x, f32 y, f32 z);
#include "driver/camdrv.h"
void effRecoveryDisp4(CameraId camId, void* param);
void effRecoveryDisp3(CameraId camId, void* param);
void effRecoveryDisp2(CameraId camId, void* param);
void effRecoveryDisp(CameraId camId, void* param);
void effRecoveryMain2(EffectEntry* effect);
void effRecoveryMain(EffectEntry* effect);
