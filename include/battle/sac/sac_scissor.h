#pragma once

#include "driver/camdrv.h"

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
extern char str_802FF570[];
void star_disp2D(void);
void scissor_damage_sub(BattleUnit* unit, s32* out);
s32 scissor_damage_sub2(const Vec* point);
void scissor_capture(void);
void scissor_disp_control(CameraId cameraId, void* param);
