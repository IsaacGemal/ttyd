#pragma once
#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct {
	u16 field_0;
	u16 field_2;
} AnimeData;
typedef struct effReleaseN64Data {
    u32 field_0x0;    // 0x00
    u32 field_0x4;    // 0x04
    f32 x;            // 0x08
    f32 y;            // 0x0C
    f32 z;            // 0x10
    s32 field_0x14;   // 0x14
    s32 field_0x18;   // 0x18
    s32 field_0x1C;   // 0x1C
    s32 field_0x20;   // 0x20
    s32 field_0x24;   // 0x24
    s32 field_0x28;   // 0x28
    s32 field_0x2C;   // 0x2C
    s32 field_0x30;   // 0x30
    s32 field_0x34;   // 0x34
    s32 field_0x38;   // 0x38
    f32 field_0x3C;   // 0x3C
    f32 scales[4];    // 0x40
    s32 alphas[4];    // 0x50
    s32 field_0x60;   // 0x60
    s32 field_0x64;   // 0x64
    s32 field_0x68;   // 0x68
    s32 field_0x6C;   // 0x6C
    s32 field_0x70;   // 0x70
    s32 field_0x74;   // 0x74
} effReleaseN64Data;
EffectEntry* effReleaseN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effReleaseMain(EffectEntry* effect);
void effReleaseDisp(CameraId camId, void* param);
