#pragma once

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>
s32 battleAcMain_Shot(BattleWork* work);
s32 battleAcResult_Shot(BattleWork* work);
void battleAcDisp_Shot(CameraId camId, void* param);
void battleAcDelete_Shot(BattleWork* work);
void _draw_target_mark(GXColor* color, f32 x, f32 y, f32 z, f32 angle);
void _disp_target_mark(CameraId cameraId, void* param);
void _disp_target_mark_afterimage(CameraId cameraId, void* param);
