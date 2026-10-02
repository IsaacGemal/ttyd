#pragma once

#include "driver/camdrv.h"

#include <dolphin/mtx.h>
#include <dolphin/types.h>
void scissor_disp_region(CameraId cameraId, void* param);
void star_disp2D_sub(s32 arg0, s32 arg1);
s32 tess(f32* points, s32 count);
s32 scissor_cross(f32* a, f32* b, f32* c, s32 n);
s32 scissor_intersection(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy);
s32 scissor_intersection2(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy);
s32 scissor_intersection3(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy);
s32 scissor_intersection4(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy);

typedef struct ScissorWork ScissorWork;
ScissorWork* GetScissorPtr(void);
void star_disp2D(void);

// .rodata block at str_802FF570: the five vectors star_control uses (0x2C..0x5C) and
// the int->float magic double (0xC8).  The original reaches them all from this one
// base; modelling the block as a struct reproduces the base+offset addressing.
typedef struct ScissorRodata {
    u8 field_0x00[0x2C];  // 0x00
    Vec v0;               // 0x2C
    Vec v1;               // 0x38
    Vec v2;               // 0x44
    Vec v3;               // 0x50
    Vec v4;               // 0x5C
    u8 field_0x68[0x60];  // 0x68
    f64 magDouble;        // 0xC8
} ScissorRodata;

extern const ScissorRodata str_802FF570;
