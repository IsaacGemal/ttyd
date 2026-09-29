#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
typedef struct effMagic3Data {
	s32 type;  // 0x00
	f32 x;     // 0x04
	f32 y;     // 0x08
	f32 z;     // 0x0C
	f32 vx;    // 0x10
	f32 vy;    // 0x14
	f32 vz;    // 0x18
	f32 rot;   // 0x1C
	f32 scale; // 0x20
	s32 alpha; // 0x24
	s32 life;  // 0x28
	s32 timer; // 0x2C
} effMagic3Data;
void effMagic3Disp(CameraId camId, void* param);
