#pragma once

#include "driver/camdrv.h"

typedef struct BattleUnit BattleUnit;

#include <dolphin/types.h>
void main_enemy_sub(BattleUnit* unit, s32* count);
void yuka_init(void);
void yuka_main(void);
void yuka_disp(CameraId cameraId, void* param);
void disp_3D(CameraId cameraId, void* param);
void disp_3D_alpha(CameraId cameraId, void* param);
