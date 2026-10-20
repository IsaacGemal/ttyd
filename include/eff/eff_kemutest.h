#pragma once
typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
EffectEntry* effKemuTestEntry(s32 type, f32 x, f32 y, f32 z, f32 scale);
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effKemuTestData {
	s32 type;       //0x0
	f32 posX;       //0x4
	f32 posY;       //0x8
	f32 posZ;       //0xC
	f32 velX;       //0x10
	f32 velY;       //0x14
	f32 velZ;       //0x18
	s32 camId;      //0x1C: camera to draw into (4 = CAMERA_3D unless effKemuTestDrawCam changes it)
	s32 frame;      //0x20: flipbook column of the current frame, 0..15 (16 * age / life)
	s32 nextFrame;  //0x24: flipbook column of the next frame, capped at 15
	s32 age;        //0x28: frames elapsed
	s32 field_0x2C; //0x2C
	s32 life;       //0x30: frames until the particle is done
	f32 speed;      //0x34: per-frame motion (added to posY, or along the particle's fan direction in types 10/12/14)
	f32 accel;      //0x38: added to speed each frame
	f32 damp;       //0x3C: speed and accel are multiplied by this each frame
	s32 field_0x40; //0x40
	s32 delay;      //0x44: frames to wait before the particle is drawn or moved
	f32 scale;      //0x48
	u8 colorR;      //0x4C
	u8 colorG;      //0x4D
	u8 colorB;      //0x4E
	u8 pad_0x4F;    //0x4F
	f32 rotX;       //0x50: tilt around X (degrees), set by effKemuTestSetRxRz
	f32 rotZ;       //0x54: tilt around Z (degrees), set by effKemuTestSetRxRz
} effKemuTestData;
void effKemuTestMain(EffectEntry* effect);
void effKemuTestDisp(CameraId camId, void* param);
void effKemuTestSetRxRz(EffectEntry* effect, f32 rx, f32 rz);
void effKemuTestDrawCam(EffectEntry* effect, s32 camId);
