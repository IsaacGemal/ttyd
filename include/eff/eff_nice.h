#pragma once

#include "driver/effdrv.h"
#include <dolphin/mtx.h>

typedef struct NiceEffectWork {
	s32 type; //0x0
	Vec position; //0x4
	f32 scaleX; //0x10, acrobat sub-entry scale vector
	f32 scaleY; //0x14
	f32 scaleZ; //0x18
	f32 scale; //0x1C, uniform scale of the whole effect (callers pass 0.75)
	s32 stateTimer; //0x20, frames in the current acrobat state
	u8 field_0x24[0x28 - 0x24]; //0x24
	s32 state; //0x28, acrobat state machine (0-9)
	u8 field_0x2C[0x30 - 0x2C]; //0x2C
	s32 poseId; //0x30
	u8 field_0x34[0x64 - 0x34]; //0x34
} NiceEffectWork;

BOOL effNiceAsync(s32 group);
EffectEntry* effNiceEntry(s32 type, f32 x, f32 y, f32 z);
void effNiceMain(EffectEntry* effect);
void acrobatMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effNiceDisp(CameraId camId, void* param);
void effAcrobatDisp(CameraId camId, void* param);
