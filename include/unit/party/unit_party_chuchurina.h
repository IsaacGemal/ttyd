#pragma once

#include "driver/camdrv.h"

void mono_disp(CameraId cameraId, void* param);
void mono_capture(CameraId cameraId, void* param);

// this file's .sdata2 (defined in config/G8MJ01/symbols.txt)
extern const f32 float_910_8041ddbc;
