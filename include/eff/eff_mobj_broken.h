#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effMObjBrokenEntry(s32 type, f32 x, f32 y, f32 z);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct MobjBrokenWork {
	s32 type;       // 0x00
	f32 posX;       // 0x04
	f32 posY;       // 0x08
	f32 posZ;       // 0x0C
	f32 scale;      // 0x10
	s32 field_0x14; // 0x14
	f32 rotAngle;   // 0x18 tumble angle (degrees) about the dir axis; eases toward 20/720
	f32 dir;        // 0x1C horizontal travel direction (degrees); also the tumble axis
	f32 speed;      // 0x20 horizontal speed along dir
	f32 velY;       // 0x24 added to posY each frame
	f32 gravity;    // 0x28 subtracted from velY each frame
	s32 poseId;     // 0x2C
} MobjBrokenWork;
void effMObjBrokenDisp2(CameraId camId, void* param);
void effMObjBrokenDisp(CameraId camId, void* param);
void effMObjBrokenMain(EffectEntry* effect);
