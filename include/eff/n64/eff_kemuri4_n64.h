#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effKemuri4Data {
	s32 active;          // 0x00: 0 once the puff has expired
	u16 type;            // 0x04
	s16 lifetime;        // 0x06: frames left; also extended while Mario idles
	u8 field_0x08;       // 0x08: 255, reduced by 12 per frame (never read)
	u8 textureIndex;     // 0x09: picks texture 0x4C or 0x4D
	u8 pad_0A[2];        // 0x0A
	Vec position;        // 0x0C
	f32 scaleX;          // 0x18
	f32 scaleY;          // 0x1C
	f32 scaleZ;          // 0x20
	f32 rotX;            // 0x24: degrees
	f32 rotY;            // 0x28: degrees
	f32 rotZ;            // 0x2C: degrees, driven by swirl
	Mtx matrix;          // 0x30: world matrix built each frame
	f32 spin;            // 0x60: rotZ delta per swirl unit, sign from direction
	f32 driftScale;      // 0x64: horizontal drift per swirl unit
	f32 floating;        // 0x68: cleared once swirl drops below zero
	f32 field_0x6C;      // 0x6C: set to zero, never read
	f32 swirl;           // 0x70: accumulated drift/rotation amount
	f32 swirlVelocity;   // 0x74
	f32 swirlAccel;      // 0x78
	f32 swirlJerk;       // 0x7C
	f32 sinAngle;        // 0x80
	f32 cosAngle;        // 0x84
} effKemuri4Data;       // 0x88
EffectEntry* effKemuri4N64Entry(s32 type, f32 x, f32 y, f32 z, f32 angleDeg, f32 direction);
void effKemuri4Main(EffectEntry* effect);
void effKemuri4Disp(CameraId camId, void* param);
