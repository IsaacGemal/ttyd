#pragma once

typedef struct SequenceWork SequenceWork;

#include <dolphin/types.h>
#include "driver/camdrv.h"
void L_titleInit(SequenceWork* work);
void titleMain(SequenceWork* work);
void disp1(CameraId cameraId, void* param);
