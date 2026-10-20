#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effFireEntry(s32 a1, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "driver/camdrv.h"
typedef struct effFireSmokeData {
	s32 field_0x0;    // 0x0
	Vec position;     // 0x4
	Vec velocity;     // 0x10
	s32 field_0x1C;   // 0x1C
	s32 field_0x20;   // 0x20
	f32 field_0x24;   // 0x24
	f32 angle;        // 0x28
	f32 angleVel;     // 0x2C
	GXColor color;    // 0x30
	s32 lifetime;     // 0x34
	s32 counter;      // 0x38
	f32 scale;        // 0x3C
} effFireSmokeData;
typedef struct effFireData {
	s32 type;          // 0x0
	Vec position;      // 0x4
	u8 alpha;          // 0x10
	u8 pad_0x11[3];    // 0x11
	s32 lifetime;      // 0x14
	s32 counter;       // 0x18
	s32 sparkTimer;    // 0x1C
	s32 sparkInterval; // 0x20
	f32 scale;         // 0x24
	s32 colorIndex1;   // 0x28
	s32 colorIndex2;   // 0x2C
	s32 counterMax;    // 0x30
	s32 frameMax;      // 0x34
	s32 intensity;      // 0x38: TEV colour level for all four channels (0x80 set by Main, read by Disp2)
	s32 scaleIndex;    // 0x3C
	s32 scaleCounter;  // 0x40
	f32 curScale;      // 0x44
	u8 pad_0x48[4];    // 0x48
	f32 field_0x4C;    // 0x4C
	f32 field_0x50;    // 0x50
	f32 field_0x54;    // 0x54
	f32 field_0x58;    // 0x58
	f32 field_0x5C;    // 0x5C
	f32 field_0x60;    // 0x60
	s32 field_0x64;    // 0x64
	s32 screenMinX;    // 0x68: screen-space bounds of the Disp3 quads, accumulated per frame
	s32 screenMaxX;    // 0x6C
	s32 screenMaxY;    // 0x70
	s32 screenMinY;    // 0x74
	f32 scaleX;        // 0x78: X scale factor, multiplies scale in the quad matrices
	f32 scaleY;        // 0x7C: Y scale factor
	s32 field_0x80;    // 0x80
	s32 field_0x84;    // 0x84
	s32 useMapBlend;   // 0x88: nonzero draws the map blend colour in Disp3
} effFireData;
typedef struct ScaleEntry {
	s32 threshold;
	f32 scale;
} ScaleEntry;
void effFireSmokeDisp(CameraId camId, void* param);
void effFireSmokeMain(EffectEntry* effect);
void effFireDisp3(CameraId camId, void* param);
void effFireDisp2(CameraId camId, void* param);
void effFireDisp(CameraId camId, void* param);
void effFireMain(EffectEntry* effect);
