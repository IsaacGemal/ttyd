#pragma once
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>
typedef struct effIbukiData {
	s32 type;         // 0x00
	f32 positionX;    // 0x04
	f32 positionY;    // 0x08
	f32 positionZ;    // 0x0C
	f32 velocityX;    // 0x10
	f32 velocityY;    // 0x14
	f32 velocityZ;    // 0x18
	f32 rotX;         // 0x1C
	f32 rotY;         // 0x20
	f32 rotVelX;      // 0x24
	f32 rotVelY;      // 0x28
	f32 scale;        // 0x2C
	s32 lifetime;     // 0x30
	s32 counter;      // 0x34
	s32 alpha;        // 0x38
	s32 alpha2;       // 0x3C
	f32 texU0;        // 0x40
	f32 texV0;        // 0x44
	f32 texU1;        // 0x48
	f32 field_0x4C;   // 0x4C
	s32 wait;         // 0x50
} effIbukiData;
void effIbukiDisp(CameraId cameraId, void* param);
void effIbukiMain(EffectEntry* effect);
EffectEntry* effIbukiEntry(s32 type, s32 lifetime, f32 x, f32 y, f32 z);
