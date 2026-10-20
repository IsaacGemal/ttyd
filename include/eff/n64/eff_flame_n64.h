#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effFlameVtx {
	s16 pos[3]; //0x0, XYZ
	s16 tex[2]; //0x6, ST
	u8 clr[4];  //0xA, RGBA8
} effFlameVtx; //0xE
typedef struct effFlameData {
	s32 type;                 //0x0
	Vec position;             //0x4
	f32 lagX;                 //0x10, follows position at 0.1 a frame; never read
	f32 lagY;                 //0x14
	f32 lagZ;                 //0x18
	f32 scaleX;               //0x1C, type 0 starts it at 2 * baseScale and eases it toward baseScale at 0.04 a frame
	f32 scaleY;               //0x20, type 0 starts it at 0.6 * baseScale and eases it toward baseScale
	f32 baseScale;            //0x24
	s32 alpha;                //0x28
	s32 life;                 //0x2C
	s32 counter;              //0x30
	f32 scrollU;              //0x34, GX_TEXMTX1 U translation for the vertex 4-7 quad, wraps at 64
	f32 scrollUSpeed;         //0x38, eased toward scrollUSpeedTarget at 0.02 a frame
	f32 scrollUSpeedTarget;   //0x3C
	f32 scrollV;              //0x40, GX_TEXMTX1 V translation for the vertex 4-7 quad, wraps at 128
	f32 scrollVSpeed;         //0x44, eased toward scrollVSpeedTarget
	f32 scrollVSpeedTarget;   //0x48
	f32 scroll2U;             //0x4C, GX_TEXMTX1 U translation (+30) for the vertex 0-3 quad, wraps at 64
	f32 scroll2USpeed;        //0x50, eased toward scroll2USpeedTarget (types 1 and up)
	f32 scroll2USpeedTarget;  //0x54
	f32 scroll2V;             //0x58, GX_TEXMTX1 V translation (+30) for the vertex 0-3 quad, wraps at 128
	f32 scroll2VSpeed;        //0x5C, eased toward scroll2VSpeedTarget (types 1 and up)
	f32 scroll2VSpeedTarget;  //0x60
	f32 rot;                  //0x64, facing angle in degrees for type 2; this unit only ever sets it to 0
	u8 tev0Rgb[3];            //0x68, GX_TEVREG0 colour; its alpha is `alpha`
	u8 tev1Rgba[4];           //0x6B, GX_TEVREG1 colour
	u8 pad_0x6F;              //0x6F
	effFlameVtx vtx[8];       //0x70
} effFlameData;
// .data tables in this unit are addressed through the shared base symbol defined in eff_fireworks.
EffectEntry* effFlameN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale);
void effFlameMain(EffectEntry* effect);
void effFlameDisp(CameraId camId, void* param);
