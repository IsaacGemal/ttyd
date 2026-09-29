#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effMugiKemuriEntry(f32 float1, f32 float2, f32 float3, f32 float4);
EffectEntry* effMugiKemuri2Entry(s32 int1, f32 float2, f32 float3, f32 float4, f32 float5, f32 float6);
#include "driver/camdrv.h"
// One particle. EffectEntry->userdata points at an array of these (count entries).
typedef struct MugiKemuriWork {
	s32 active;      // 0x00
	s32 type;        // 0x04
	f32 x;           // 0x08
	f32 y;           // 0x0C
	f32 z;           // 0x10
	f32 rotX;        // 0x14
	f32 rotY;        // 0x18
	f32 rotZ;        // 0x1C
	f32 scaleX;      // 0x20
	f32 scaleY;      // 0x24
	f32 scaleZ;      // 0x28
	f32 scaleSpeed;  // 0x2C
	f32 scale;       // 0x30
	f32 speed;       // 0x34
	f32 unk38;       // 0x38
	f32 time;        // 0x3C
	f32 timeAdd;     // 0x40
	f32 timeVel;     // 0x44
	f32 timeVelAdd;  // 0x48
	f32 sinA;        // 0x4C
	f32 cosA;        // 0x50
	s32 lifetime;    // 0x54
	s32 alpha;       // 0x58
	f32 randOff;     // 0x5C
	s32 animPose;    // 0x60
} MugiKemuriWork;
void effMugiKemuriDisp(CameraId cameraId, void* param);
void effMugiKemuri2Disp(CameraId cameraId, void* param);
void effMugiKemuriMain(EffectEntry* effect);
void effMugiKemuri2Main(EffectEntry* effect);
