#pragma once

#include "driver/camdrv.h"

typedef struct BattleUnit BattleUnit;

#include <dolphin/types.h>
void main_enemy_sub(BattleUnit* unit, s32* count);
void yuka_disp(CameraId cameraId, void* param);
