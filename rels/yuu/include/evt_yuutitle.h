#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

typedef struct YuuTitleWork {
    EventEntry* entry;   // 0x00
    u16 flags;           // 0x04
    f32 field_08;        // 0x08
    f32 field_0c;        // 0x0C
    f32 field_10;        // 0x10
    f32 field_14;        // 0x14
    f32 field_18;        // 0x18
    f32 field_1c;        // 0x1C
    s32 alpha;           // 0x20
    const char* msg;     // 0x24
} YuuTitleWork;

USER_FUNC(evt_yuutitle_free);
USER_FUNC(evt_yuutitle_alloc);
USER_FUNC(evt_yuutitle_main_func);
void evt_yuutitle_disp_func(CameraId cameraId, void* param);
