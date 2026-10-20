#pragma once

#include <dolphin/types.h>

typedef struct MotionPlaneData {
	s32 flags; //0x0
	f32 pitch; //0x4
	f32 heading; //0x8
	f32 bank; //0xC
	u8 field_0x10[0x20 - 0x10]; //0x10
	s32 stallTimer; //0x20
	u8 field_0x24[0x28 - 0x24]; //0x24
	s32 sfxId; //0x28
	u8 field_0x2C[0x48 - 0x2C]; //0x2C
} MotionPlaneData;

// Shared plane trig/geometry table read by chkCollision/flyMain (planeData[39]=6.0f, [40]=28.0f).
extern f32 planeData[];
