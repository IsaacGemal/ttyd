#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

typedef struct EffThunderData {
	s32 type;           //0x0
	f32 x;              //0x4
	f32 y;              //0x8
	f32 z;              //0xC
	s32 alpha;          //0x10 TEVREG0 alpha (drops by 25 or 13 a frame)
	s32 life;           //0x14 frames left (100 = still growing in; the effect is deleted below 0)
	s32 counter;        //0x18 frames elapsed
	f32 scrollU0;       //0x1C texture 0 U offset, wraps at 256
	f32 scrollV0;       //0x20 texture 0 V offset, wraps at 64
	f32 scrollU1;       //0x24 texture 1 U offset, wraps at 256
	f32 scrollV1;       //0x28 texture 1 V offset, wraps at 64
	f32 scrollSpeedU0;  //0x2C added to scrollU0 every frame
	f32 scrollSpeedV0;  //0x30 added to scrollV0 every intervalV0 frames
	f32 scrollSpeedU1;  //0x34 added to scrollU1 every intervalU1 frames
	f32 scrollSpeedV1;  //0x38 added to scrollV1 every frame
	s32 intervalV0;     //0x3C
	s32 intervalU1;     //0x40
	EffectEntry* flare; //0x44 thunderflare child (type 1)
	EffectEntry* charge; //0x48 akari charge child (type 2)
} EffThunderData;

EffectEntry* effThunderN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6);
void effThunderMain(EffectEntry* effect);
void effThunderDisp(CameraId camId, void* param);
