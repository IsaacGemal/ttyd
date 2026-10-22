#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct effKamekiTornadeData {
	s32 type;            // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	s32 life;            // 0x10, frames left (1000 = no limit; the entry's flag sets 0x10)
	s32 timer;           // 0x14, frames since spawn
	s32 colorR;          // 0x18, GX_TEVREG0 colour (C0); its alpha is `alpha` / 2 * alphaScale (A0)
	s32 colorG;          // 0x1C
	s32 colorB;          // 0x20
	s32 alpha;           // 0x24, fades in while timer < 16 and out while life < 16
	s32 color2R;         // 0x28, GX_TEVREG1 colour (C1)
	s32 color2G;         // 0x2C
	s32 color2B;         // 0x30
	f32 scale;           // 0x34
	f32 nodeX[8];        // 0x38, chain of 8 nodes; node 0 follows the origin
	f32 nodeY[8];        // 0x58, each node sits 5 * scale above the previous one
	f32 nodeZ[8];        // 0x78
	f32 nodeScale[8];    // 0x98
	f32 nodeAngle[8];    // 0xB8, degrees
	f32 nodeSpin[8];     // 0xD8, degrees added per frame
	f32 nodeRadius[8];   // 0xF8, ring radius (x 10)
	f32 nodeTilt[8];     // 0x118, degrees, clamped to +-90
	f32 spinAngle;       // 0x138, degrees, added per frame
	f32 texScroll0;      // 0x13C, GX_TEXMTX0 S offset, wraps at 64
	f32 texScroll1;      // 0x140, GX_TEXMTX1 S offset, wraps at 64
	f32 baseRotZ;        // 0x144, degrees
	f32 alphaScale;      // 0x148, multiplies alpha in the colour
} effKamekiTornadeData;

EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 a4);
#include "driver/camdrv.h"
void effKamekiTornadeDisp(CameraId cameraId, void* param);
void effKamekiTornadeMain(EffectEntry* effect);

// 14-byte N64 vertex, matching effVtxArray's layout (without its 3-vertex colour arrays).
typedef struct KamekiTornadeVtx {
	s16 pos[3]; // 0x0, XYZ
	s16 tex[2]; // 0x6, ST
	u8 clr[4];  // 0xA, RGBA8
} KamekiTornadeVtx; // 0xE

// .sdata2 constants this unit loads.
extern const f32 float_6p2832_8041f1dc;
extern const f32 float_360_8041f1e0;
extern const f32 float_100_8041f1e4;
extern const f32 float_deg2rad_8041f1e8;
extern const f32 float_0p02_8041f1f0;
extern const f32 float_0p015625_8041f1fc;
extern const f32 float_0p03125_8041f200;
