#pragma once

#include "driver/camdrv.h"

void mono_disp(CameraId cameraId, void* param);
void mono_capture(CameraId cameraId, void* param);

// this file's .sdata2 (defined in config/G8MJ01/symbols.txt)
extern const f32 float_910_8041ddbc;
extern f32 float_0_8041ddc0;
extern f32 float_neg304_8041ddc4;
extern f32 float_240_8041ddc8;
extern f32 float_304_8041ddcc;
extern f32 float_1_8041ddd0;
extern f32 float_neg240_8041ddd4;
extern u32 dat_8041ddb8;
