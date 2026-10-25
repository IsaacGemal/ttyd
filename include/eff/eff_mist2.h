#pragma once
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
// Entry 0 is the effect itself (position, overall scale/alpha/lifetime, colour); entries 1.. are the puffs.
typedef struct EffMist2Data {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 field_0x10;  // 0x10
	f32 targetScale; // 0x14, puffs: scale eases toward it
	f32 scale;       // 0x18, effMist2Disp's PSMTXScale
	f32 range;       // 0x1C, puffs: effMist2Entry's range, the radius they respawn within
	f32 yOffset;     // 0x20, puffs: effMist2Entry's yOffset, type 1's start height
	f32 rotZ;        // 0x24, puffs: degrees, effMist2Disp's z rotation
	f32 rotZVel;     // 0x28, puffs: added to rotZ each frame
	f32 alpha;       // 0x2C, 0-255; fades out over the last 0x20 frames of lifetime
	s32 field_0x30;  // 0x30
	s32 lifetime;    // 0x34, frames left; 1000 on entry 0 never counts down
	u8 colorR;       // 0x38, tik_05 kemuri_set_color's r, g, b; the GXSetChanMatColor colour
	u8 colorG;       // 0x39
	u8 colorB;       // 0x3A
	u8 pad_0x3B;     // 0x3B
	s32 delay;       // 0x3C, puffs: frames before the puff starts
	f32 gravity;     // 0x40, puffs (type 1): subtracted from velY each frame
	f32 velY;        // 0x44
} EffMist2Data;
void effMist2Disp(CameraId cameraId, void* param);
void effMist2Main(EffectEntry* effect);
EffectEntry* effMist2Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 range, f32 yOffset);
