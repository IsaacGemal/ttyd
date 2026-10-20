#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effKumokumoN64Entry(s32 type, s32 param, f32 x, f32 y, f32 z, f32 rotation, f32 a6, f32 scale);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct effKumokumoData {
	s32 type;       //0x00
	f32 x;          //0x04
	f32 y;          //0x08
	f32 z;          //0x0C
	s32 lifetime;   //0x10
	s32 frame;      //0x14
	s32 color1R;    //0x18
	s32 color1G;    //0x1C
	s32 color1B;    //0x20
	s32 color1A;    //0x24
	s32 color2R;    //0x28
	s32 color2G;    //0x2C
	s32 color2B;    //0x30
	s32 color2A;    //0x34
	f32 texX;       //0x38
	f32 texXVel;    //0x3C
	f32 texY;       //0x40
	f32 texYVel;    //0x44
	f32 texX2;      //0x48
	f32 texX2Vel;   //0x4C
	f32 texY2;      //0x50
	f32 texY2Vel;   //0x54
	f32 rotation;   //0x58
	f32 scale;      //0x5C
	s32 camera;     //0x60
} effKumokumoData;
void effKumokumoMain(EffectEntry* effect);
void effKumokumoDisp(CameraId camId, void* param);
