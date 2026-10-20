#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>

typedef struct MotionPlaneData {
	s32 flags; //0x0
	f32 pitch; //0x4
	f32 heading; //0x8
	f32 bank; //0xC
	f32 field_0x10; //0x10
	f32 field_0x14; //0x14
	f32 field_0x18; //0x18
	f32 field_0x1C; //0x1C
	s32 stallTimer; //0x20
	f32 field_0x24; //0x24
	s32 sfxId; //0x28
	u8 field_0x2C[0x30 - 0x2C]; //0x2C
	Vec field_0x30; //0x30
	Vec field_0x3C; //0x3C
} MotionPlaneData;

// Shared plane trig/geometry table read by chkCollision/flyMain (planeData[39]=6.0f, [40]=28.0f).
extern f32 planeData[];
