#pragma once

typedef struct SequenceWork SequenceWork;

#include <dolphin/types.h>
#include "driver/camdrv.h"
extern Vec vec3_802c11a0[]; // this unit's .rodata vector pool (6 Vecs, following the pointers)
void L_titleInit(SequenceWork* work);
void titleMain(SequenceWork* work);
void disp1(CameraId cameraId, void* param);
void disp2(CameraId cameraId, void* param);
