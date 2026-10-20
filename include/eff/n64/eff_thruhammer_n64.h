#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct EffThruHammerData {
	s16 type;        //0x00
	Vec position;    //0x04
	f32 dirX;        //0x10: particles: 0.5 * the rotZ-rotated up vector (never read)
	f32 dirY;        //0x14
	f32 dirZ;        //0x18: always 0
	f32 scaleX;      //0x1C
	f32 scaleY;      //0x20
	f32 scaleZ;      //0x24
	s32 life;        //0x28
	s32 age;         //0x2C
	f32 rotY;        //0x30: degrees
	f32 rotZ;        //0x34: degrees
	s32 alpha;       //0x38: GX_TEVREG0 alpha
	f32 texScrollA;  //0x3C: V offset of GX_TEXMTX0, clamped at 128
	f32 texSpeedA;   //0x40
	f32 texScrollB;  //0x44: V offset of GX_TEXMTX1
	f32 texSpeedB;   //0x48
	s32 delay;       //0x4C: particles start drawing once it reaches 0
	u8 tevReg0R;     //0x50: GX_TEVREG0 colour (GXSetTevColor(1, ...))
	u8 tevReg0G;     //0x51
	u8 tevReg0B;     //0x52
	u8 tevReg1R;     //0x53: GX_TEVREG1 colour (GXSetTevColor(2, ...))
	u8 tevReg1G;     //0x54
	u8 tevReg1B;     //0x55
} EffThruHammerData; //0x58
void effThruHammerMain(EffectEntry* effect);
void effThruHammerDisp(CameraId camId, void* param);
EffectEntry* effThruHammerN64Entry(s32 type, f32 x, f32 y, f32 z, f32 angle);
