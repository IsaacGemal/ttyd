#pragma once

#include "driver/camdrv.h"

#include "driver/effdrv.h"
#include <dolphin/mtx.h>

typedef struct effSleepData {
	s32 field_0x0; //0x0
	Vec position; //0x4
	f32 scale; //0x10
	s32 alpha; //0x14 GXSetChanMatColor alpha; element 0's scales every Z's
	s32 lifetime; //0x18 element 0: 1000 = no limit; counts down, fades out below 32, deleted below 0
	s32 age; //0x1C element 0: frames since entry, fades in for the first 32
	s32 delay; //0x20 elements 1..: frames before the Z is drawn and starts fading in
} effSleepData;

EffectEntry* effSleepEntry(s32 a1, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6, f32 a7);
void effSleepMain(EffectEntry* effect);
void effSleepDisp(CameraId camId, EffectEntry* effect);
