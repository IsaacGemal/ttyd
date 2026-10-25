#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

typedef struct YuuTitleWork {
    EventEntry* entry;   // 0x00
    u16 flags;           // 0x04
    f32 x;               // 0x08, FontDrawString position, eased from fromX to toX as alpha rises
    f32 y;               // 0x0C
    f32 fromX;           // 0x10, start of the slide: toX + 200
    f32 fromY;           // 0x14, toY + 200
    f32 toX;             // 0x18, evt_yuutitle_alloc's x argument
    f32 toY;             // 0x1C, its y argument
    s32 alpha;           // 0x20
    const char* msg;     // 0x24
} YuuTitleWork;

USER_FUNC(evt_yuutitle_free);
USER_FUNC(evt_yuutitle_alloc);
USER_FUNC(evt_yuutitle_main_func);
void evt_yuutitle_disp_func(CameraId cameraId, void* param);
