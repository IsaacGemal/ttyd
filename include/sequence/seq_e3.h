#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>
void e3Disp(CameraId cameraId, void* param);
void e3DispOffscreen(CameraId cameraId, void* param);
void pressStartGX(s32 alpha, f32 x, f32 y);
