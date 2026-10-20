#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effAkariSflushN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale);
void effAkariSflushMain(EffectEntry* effect);
typedef struct EffAkariSflushData {
	s32 type;       //0x0
	f32 x;          //0x4
	f32 y;          //0x8
	f32 z;          //0xC
	s32 life;       //0x10
	s32 age;        //0x14
	s32 color1R;    //0x18: TEVREG0 colour
	s32 color1G;    //0x1C
	s32 color1B;    //0x20
	s32 alpha;      //0x24
	f32 scale;      //0x28
	f32 growScale;  //0x2C: ring size multiplier, starts at 1 and grows in the last 8 frames
	s32 lastRing;   //0x30: index of the last ring (rings run 0..lastRing)
} EffAkariSflushData;
#include "driver/camdrv.h"
void effAkariSflushDisp(CameraId camId, void* param);
/* .sdata2 constant shared with the original unit. */
extern const f32 float_0p3_8041e920;
