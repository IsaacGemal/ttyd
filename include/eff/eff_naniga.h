#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/types.h>
#include <dolphin/mtx.h>
typedef struct effNanigaData {
	s32 type;       // 0x00
	f32 posX;       // 0x04
	f32 posY;       // 0x08
	f32 posZ;       // 0x0C
	Vec field_0x10; // 0x10, the position through the 3D camera's view and projection, then scaled to the screen
	f32 scale;      // 0x1C
	s32 state;      // 0x20
	s32 timer;      // 0x24
	s32* itemList;  // 0x28
	s32 count;      // 0x2C
	s32 itemId;     // 0x30
	f32 field_0x34; // 0x34
	f32 start;      // 0x38
	f32 end;        // 0x3C
	s32 poseId;     // 0x40
	s32 color;      // 0x44
	s32 mode;       // 0x48
	s32 sfxId;      // 0x4C
} effNanigaData;
EffectEntry* effNanigaEntry(s32 type, s32* itemList, s32 itemId, f32 x, f32 y, f32 z);
void effNanigaMain(EffectEntry* effect);
void effNanigaDisp(CameraId camId, void* param);
void effNanigaDisp2(CameraId camId, void* param);
void effNanigaDisp3(CameraId camId, void* param);
