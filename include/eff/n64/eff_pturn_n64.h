#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
// Only the fields effPturnDisp reads are named so far; the real block extends to at least 0x88.
typedef struct effPturnN64Data {
    s32 field_0x0; // 0x00
    s32 field_0x4; // 0x04
} effPturnN64Data;
void effPturnDisp(CameraId camId, void* param);
void pturn_main_dispp(CameraId camId, void* param);
void pturn_num_dispp(CameraId camId, void* param);
