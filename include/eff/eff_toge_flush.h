#pragma once
typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include "driver/camdrv.h"
EffectEntry* effTogeFlushEntry(s32 type, f32 x, f32 y, f32 z, s32 life);
void effTogeFlushMain(EffectEntry* effect);
void effTogeFlushDisp(CameraId cameraId, void* param);
typedef struct effTogeFlushData {
	s32 type;            // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	f32 angle;           // 0x10, degrees
	f32 angleSpeed;      // 0x14, degrees per frame
	f32 innerScale;      // 0x18, the +/-4 quad (rotated by angle)
	f32 innerScaleSpeed; // 0x1C
	f32 outerScale;      // 0x20, the +/-8 quad
	s32 innerAlpha;      // 0x24
	s32 outerAlpha;      // 0x28
	s32 life;            // 0x2C
	s32 frame;           // 0x30
} effTogeFlushData;
