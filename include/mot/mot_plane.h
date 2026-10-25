#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>

typedef struct MotionPlaneData {
	s32 flags; //0x0
	f32 pitch; //0x4
	f32 heading; //0x8
	f32 bank; //0xC
	f32 speedAccel; //0x10, added to baseSpeed each frame while pitch is in [-80, 0) (flyMain)
	f32 pitchSpeed; //0x14, added to pitch each frame (flyMain); capped at 1 while the stick is held
	f32 velY; //0x18, added to position.y each frame (flyMain); the held stick drives it down to planeData[19]
	f32 field_0x1C; //0x1C, also added to pitch (flyMain): set at most 0 from the speed below planeData[0], eased back to 0 while the stick is held
	s32 stallTimer; //0x20
	f32 pitchMin; //0x24, lower bound flyMain clamps pitch to (-22 over attr 0x38 ground, else eased toward planeData[4])
	u32 sfxId; //0x28
	u8 field_0x2C[0x30 - 0x2C]; //0x2C
	Vec hitRearPos; //0x30, chkCollision: planeData[40] (28) back along the flight axis from the display centre; origin of its
	                 //      forward, down and up hit rays and of the ray to hitFrontPos
	Vec hitFrontPos; //0x3C, chkCollision: 4 off the display centre, ahead of hitRearPos; origin of its other forward and down rays
} MotionPlaneData;

// Shared plane trig/geometry table read by chkCollision/flyMain (planeData[39]=6.0f, [40]=28.0f).
extern f32 planeData[];
