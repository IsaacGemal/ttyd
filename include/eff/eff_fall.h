#pragma once

#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include <dolphin/types.h>

typedef struct FallWork {
	s32 type;                // 0x00 effect type 0-8
	f32 x;                   // 0x04
	f32 y;                   // 0x08
	f32 z;                   // 0x0C
	f32 radius;              // 0x10 spawn radius (entry's radius, the caller's unit width)
	f32 height;              // 0x14 target hit position above the floor y; elements within radius/2 land at y + height
	f32 angle;               // 0x18 heading in degrees (entry's angle)
	s32 num;                 // 0x1C number of particle elements (count - 1)
	s32 timerMax;            // 0x20 frame count the per-element timer runs up to (entry's timerMax)
	s32 timer;               // 0x24 per-element frame counter/timer
	s32 texFrame;            // 0x28 bug texture row 0-2, stepped every other 4-frame window
	f32 scale;               // 0x2C model scale
	f32 gravity;             // 0x30 subtracted from velY each frame
	f32 velY;                // 0x34 vertical velocity
	s32 state;               // 0x38 per-element state machine index
	s32 alpha;               // 0x3C fade alpha
	smartEntry* vertexBuf;   // 0x40 smartAlloc'd strip vertices (type 2)
	f32 wave;                // 0x44 type-2 impact wobble amplitude
	f32 spin;                // 0x48 rotation angle in degrees
	f32 roll;                // 0x4C type-6 rotation about the pivot at x = -200, used while spin is clamped to 0
	f32 field_0x50;          // 0x50 per-type vector: type 3 texture pan, type 6 velocity, type 7 waypoint
	f32 field_0x54;          // 0x54
	f32 field_0x58;          // 0x58
	s32 poseId;              // 0x5C animPoseEntry handle, -1 = none
	s32 poseId2;             // 0x60 second animPoseEntry handle (type 7)
	s32 sfxId;               // 0x64 psndSFXOn_3D handle
} FallWork;

s32 effFallCheckHit(EffectEntry* effect);
void effFallDispWater(CameraId cameraId, void* param);
void effFallDispInseki(CameraId cameraId, void* param);
void effFallDisp(CameraId cameraId, void* param);
void effFallMain(EffectEntry* effect);
EffectEntry* effFallEntry(s32 type, f32 x, f32 y, s32 num, f32 z, s32 timerMax, f32 radius, f32 height, f32 angle);
