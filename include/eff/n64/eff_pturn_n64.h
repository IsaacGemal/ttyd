#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
// effPturnN64Entry allocates two of these (0x48 bytes each). Element 0 is the head: never drawn, it holds
// the type, the frame counter and the number being counted up or down. Element 1 is the one drawn quad
// (badge plus digits). The head-only and particle-only fields sit at disjoint offsets; only x/y/z and life
// are used by both, with different meanings (noted per field).
typedef struct PturnN64Elem {
    s32 type;        // 0x00, head only
    s32 active;      // 0x04, head only; Disp draws nothing while 0
    f32 x;           // 0x08, head: world position; particle: offset inside the head's transform
    f32 y;           // 0x0C
    f32 z;           // 0x10
    f32 velX;        // 0x14, particle only (velX..alpha, scale, angle)
    f32 velY;        // 0x18
    f32 velZ;        // 0x1C
    f32 bobY;        // 0x20, cos wobble added to the Y offset of the quad
    s32 alpha;       // 0x24, TEVREG0 alpha
    s32 life;        // 0x28, head: countdown (held at 100), the effect dies below 0; particle: age, counts up (scale_dat index)
    s32 counter;     // 0x2C, head only; frame counter
    f32 scale;       // 0x30
    f32 scaleFactor; // 0x34, head only; multiplies every particle scale
    s32 displayNum;  // 0x38, head only; number shown
    s32 targetNum;   // 0x3C, head only; number the head counts towards
    f32 angle;       // 0x40, degrees, Z rotation of the quad
    s32 stepDelay;   // 0x44, head only; frames to wait between steps
} PturnN64Elem;
// .sdata2 constants defined in this unit (see config/G8MJ01/symbols.txt).
extern const f32 float_deg2rad_8041fa60;
extern const f32 float_0p03125_8041fa6c;
extern const f32 float_0p0026042_8041fa70;
void effPturnDisp(CameraId camId, void* param);
void effPturnMain(EffectEntry* effect);
void pturn_main_dispp(CameraId camId, void* param);
void pturn_num_dispp(CameraId camId, void* param);
EffectEntry* effPturnN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z);
