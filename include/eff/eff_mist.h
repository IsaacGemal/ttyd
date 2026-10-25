#pragma once
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>
typedef struct MistWork {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 scaleX;     // 0x10
	f32 scaleY;     // 0x14
	f32 scaleZ;     // 0x18
	f32 velX;       // 0x1C
	f32 velY;       // 0x20
	f32 velZ;       // 0x24
	f32 riseSpeed;  // 0x28 type 4: added to y each frame (0.95 at start)
	f32 riseAccel;  // 0x2C type 4: added to riseSpeed each frame
	f32 riseAccelDecay; // 0x30 type 4: riseAccel is multiplied by this each frame
	f32 alpha;      // 0x34
	s32 counter;    // 0x38
	s32 life;       // 0x3C
	f32 yaw;        // 0x40 degrees (Entry arg minus the 3D camera yaw); type 4 aims velX/velZ along it
	u8 r;           // 0x44
	u8 g;           // 0x45
	u8 b;           // 0x46
	u8 pad_47;      // 0x47
} MistWork;
EffectEntry* effMistEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 yaw);
void effMistMain(EffectEntry* effect);
void effMistDisp(CameraId cameraId, void* param);
