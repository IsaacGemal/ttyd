#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
struct EffectEntry;
EffectEntry* effStarStoneEntry(s32 idx, f32 x, f32 y, f32 z, f32 scale);
#include <dolphin/mtx.h>
#include "driver/dispdrv.h"
typedef struct effStarStoneData {
	s32 starType;    // 0x0 idx % 7: colour table index; 0 is the diamond_1 model, 6 diamond_7, 1-5 diamond_2_2
	u16 flags;       // 0x4 bit field, see effStarStoneMain
	u8 field_0x6[2]; // 0x6
	f32 x;           // 0x8
	f32 y;           // 0xC
	f32 z;           // 0x10
	f32 scale;       // 0x14
	f32 tipRotY;     // 0x18 y rotation of the tip pairs in degrees
	f32 rotY;        // 0x1C y rotation of the whole star in degrees
	f32 tipRotZ;     // 0x20 z rotation of the tip pairs in degrees
	f32 scrollU;     // 0x24 texture-matrix translation U
	f32 scrollV;     // 0x28 texture-matrix translation V
	s32 timer;       // 0x2C counts 0..59
	u32 matColor;    // 0x30 packed RGBA channel material colour
	struct EffectEntry* dustEffect; // 0x34 child dust effect
	s32 camId;       // 0x38
	s32 state;       // 0x3C
	f32 gravity;     // 0x40 subtracted from velY each tick
	f32 velY;        // 0x44
	f32 spreadAX;    // 0x48 x offset of tip pair A, swung by camera yaw each tick
	f32 spreadAZ;    // 0x4C z offset of tip pair A
	f32 spreadBX;    // 0x50 x offset of tip pair B (opposite swing to A)
	f32 spreadBZ;    // 0x54 z offset of tip pair B
	f32 field_0x58;  // 0x58
	u8 tevReg0R;     // 0x5C GX_TEVREG0 colour, multiplied into the vertex colours in TEV stage 1
	u8 tevReg0G;     // 0x5D
	u8 tevReg0B;     // 0x5E
	u8 tevReg0A;     // 0x5F GX_TEVREG0 alpha: the star's opacity, also passed to U_shadowSetMode as the shadow alpha
	s32 sfxHandle;   // 0x60 handle from psndSFXOn_3D for the shine loop
} effStarStoneData;
void effStarStoneDraw(CameraId camId, EffectEntry* entry);
void effStarStoneDisp_1(CameraId camId, void* param);
void effStarStoneDisp_2(CameraId camId, void* param);
void effStarStoneMain(EffectEntry* effect);
