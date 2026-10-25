#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

typedef struct effMagic2Data {
    s32 type;        // 0x00
    Vec worldPos;    // 0x04, Main projects it through CAMERA_3D's view and projection into position
    Vec position;    // 0x10, screen position (304 * x, 240 * y of the projection); Disp's translation
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
