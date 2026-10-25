#pragma once

#include "evt/evt_cmd.h"
#include "driver/dispdrv.h"

typedef struct GoryuugijoWork {
    EventEntry* entry;      // 0x00
    EventEntry* waitEvent;  // 0x04, evt_goryuugijo_main_func skips main() while this event runs (nothing in gor sets it)
    u16 flags;              // 0x08
    u16 pad_0A;             // 0x0A
    s32 mode;               // 0x0C
    UserFunction init;      // 0x10
    UserFunction main;      // 0x14
    DispCallback disp;      // 0x18
    void* data;             // 0x1C
} GoryuugijoWork;

USER_FUNC(eff_call_yuugijo);
USER_FUNC(evt_goryuugijo_free);
USER_FUNC(evt_goryuugijo_alloc);
USER_FUNC(evt_goryuugijo_main_func);
void evt_goryuugijo_disp_func(CameraId cameraId, void* param);
