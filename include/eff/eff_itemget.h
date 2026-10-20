#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effItemGetEntry(s32 type, f32 x, f32 y, f32 z);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct effItemGetData {
	s32 type;       //0x0 glow type 0-10 (itemdrv picks it from the item kind): indexes size_tbl and color_tbl, sets the particle count; header only
	Vec position;   //0x4
	f32 animScale;  //0x10 scale factor, from scale_data over the first 12 frames and x1.1 / x0.9 per frame
	f32 baseSize;   //0x14 size_tbl[type] / 10 * 0.75
	s32 life;       //0x18 frames left (1000 = held until flagged; 16 once the effect is told to close)
	s32 frame;      //0x1C frames elapsed
	s32 scaleMode;  //0x20 header only: 0 hold, 1 grow while under 100, 2 shrink
	f32 velX;       //0x24 particle velocity
	f32 velY;       //0x28
	f32 velZ;       //0x2C
	f32 angle;      //0x30 particle spin, radians
	s32 alpha;      //0x34 0-255, faded in and out by frame
	s32 camId;      //0x38 dispEntry camera (CAMERA_3D)
} effItemGetData;
void effItemGetMain(EffectEntry* effect);
void effItemUseDisp(CameraId camId, void* param);
void effItemGetDisp(CameraId camId, void* param);
