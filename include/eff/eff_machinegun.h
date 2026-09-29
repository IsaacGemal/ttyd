#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effMachinegunData {
	s32 type;       // 0x0
	Vec position;   // 0x4
	f32 field_0x10; // 0x10
	f32 angle;      // 0x14
	f32 scale;      // 0x18
	s32 lifetime;   // 0x1C
} effMachinegunData;
EffectEntry* effMachinegunEntry(s32 type, f32 x, f32 y, f32 z, f32 angle, f32 scale, f32 field_0x10);
void effMachinegunMain(EffectEntry* effect);
void effMachinegunDisp(CameraId cameraId, void* param);
