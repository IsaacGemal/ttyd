#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>

typedef struct EffWhirlwindData {
	s32 type;             //0x0
	struct MarioWork* mario; //0x4 followed each frame (NULL keeps the spawn point)
	f32 x;                //0x8
	f32 y;                //0xC
	f32 z;                //0x10
	f32 scale;            //0x14 current scale (header and particles)
	f32 baseScale;        //0x18 1.2 * the size argument; the header's scale eases toward 2x this
	s32 tevReg0A;         //0x1C TEVREG0 alpha (header only; every particle is drawn with it)
	s32 lifetime;         //0x20 total frames (divides the header's scale ramp)
	s32 life;             //0x24 frames left
	s32 timer;            //0x28 frames elapsed
	f32 rotX;             //0x2C degrees
	f32 rotY;             //0x30 degrees
	f32 rotZ;             //0x34 degrees
	f32 rotXVel;          //0x38 degrees per frame added to rotX
	f32 rotYVel;          //0x3C degrees per frame added to rotY
	f32 rotZVel;          //0x40 degrees per frame added to rotZ
	u8 tevReg0R;          //0x44 particle colour, TEVREG0 red
	u8 tevReg0G;          //0x45 TEVREG0 green
	u8 tevReg0B;          //0x46 TEVREG0 blue
	u8 field_0x47;        //0x47
} EffWhirlwindData; //0x48

EffectEntry* effWhirlwindN64Entry(s32 type, s32 mario, s32 life, f32 size);
#include "driver/camdrv.h"
void effWhirlwindMain(EffectEntry* effect);
void effWhirlwindDisp(CameraId camId, void* param);
extern GXColor unk_80423088;
extern const char str_802FA168[];
