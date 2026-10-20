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
	f32 tiltAngle;  // 0x18
	f32 dirAngle;   // 0x1C
	f32 horizSpeed; // 0x20
	f32 velY;       // 0x24
	f32 gravity;    // 0x28
	s32 poseId;     // 0x2C
} MobjBrokenWork;
void effMObjBrokenDisp2(CameraId camId, void* param);
void effMObjBrokenDisp(CameraId camId, void* param);
void effMObjBrokenMain(EffectEntry* effect);
