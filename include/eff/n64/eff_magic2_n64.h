#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

typedef struct effMagic2Data {
    s32 type;        // 0x00
    f32 vx;          // 0x04
    f32 vy;          // 0x08
    f32 vz;          // 0x0C
    f32 x;           // 0x10
    f32 y;           // 0x14
    f32 z;           // 0x18
    f32 scale;       // 0x1C
    f32 scaleMul;    // 0x20
    s32 alpha;       // 0x24
    s32 life;        // 0x28
    s32 timer;       // 0x2C
    s32 uOffset;     // 0x30
    s32 uSpeed;      // 0x34
    s32 vOffset;     // 0x38
    s32 vSpeed;      // 0x3C
    s32 rot;         // 0x40
    s32 rotSpeed;    // 0x44
    s32 rot2;        // 0x48
    s32 rot2Speed;   // 0x4C
    f32 scale2;      // 0x50
    f32 scale2Speed; // 0x54
    f32 scale3;      // 0x58
    f32 scale3Speed; // 0x5C
} effMagic2Data;

EffectEntry* effMagic2N64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z, f32 a2);
void effMagic2Main(EffectEntry* effect);
void effMagic2Disp(CameraId cameraId, void* param);
void magic2_4_dl(void);
void magic2_2_line_dl(void);
