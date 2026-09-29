#pragma once

#include <dolphin/mtx.h>
#include "driver/camdrv.h"
void vivihimo(Mtx mtxA, Mtx mtxB, f32 phase, f32 spread, f32 offA, f32 offB);
void vivihimoDisp(CameraId cameraId, void* data);
