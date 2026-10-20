#pragma once

#include <dolphin/types.h>
void* effSmallStarN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 dirX, f32 dirY, f32 dirZ);
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effSmallStarN64Data {
    s32 type;       // 0x0
    f32 x;          // 0x4
    f32 y;          // 0x8
    f32 z;          // 0xC
    f32 velX;       // 0x10
    f32 velY;       // 0x14
    f32 velZ;       // 0x18
    f32 spinAngle;  // 0x1C
    f32 spinSpeed;  // 0x20
    s32 alpha;      // 0x24
    s32 life;       // 0x28
    s32 frameCount; // 0x2C
    u8 camera;      // 0x30
    u8 pad_0x31[3]; // 0x31
} effSmallStarN64Data;
void effSmallStarN64CamChg(EffectEntry* effect, s32 camId);
void effSmallStarMain(EffectEntry* effect);
void effSmallStarDisp(CameraId camId, void* param);
