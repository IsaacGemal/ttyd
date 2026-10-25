#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effMachinegunData {
	s32 type;       // 0x0
	Vec position;   // 0x4
	f32 scale;      // 0x10, sprite scale (PSMTXScale)
	f32 angle;      // 0x14
	f32 speed;      // 0x18, distance moved along angle each frame; decays by 0.9
	s32 lifetime;   // 0x1C
} effMachinegunData;
EffectEntry* effMachinegunEntry(s32 type, f32 x, f32 y, f32 z, f32 angle, f32 speed, f32 scale);
void effMachinegunMain(EffectEntry* effect);
void effMachinegunDisp(CameraId cameraId, void* param);
