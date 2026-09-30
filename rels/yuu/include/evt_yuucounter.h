#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

typedef struct YuuCounterWork {
    EventEntry* event; // 0x00
    u16 flags;         // 0x04
    u16 pad_06;        // 0x06
    s32 flags2;        // 0x08
    s32 mode;          // 0x0C
    f32 x;             // 0x10
    f32 y;             // 0x14
    s32 time;          // 0x18
    s32 pad_1C;        // 0x1C
    s64 accum;         // 0x20
    s64 last;          // 0x28
    s32 limit;         // 0x30
    s32 value;         // 0x34
} YuuCounterWork;

s32 yuucounter_flag(s32 mask);
void yuucounter_flag_off(u32 mask);
void yuucounter_flag_on(u32 mask);
void yuucounter_set_data(s32 data);
s32 yuucounter_get_data(void);
USER_FUNC(evt_yuucounter_free);
USER_FUNC(evt_yuucounter_alloc);
USER_FUNC(evt_yuucounter_main_func);
void evt_yuucounter_disp_func(CameraId cameraId, void* param);
USER_FUNC(unk_yuu_00009d5c);
EVT_DEF(unk_evt_yuu_00019728);
EVT_DEF(unk_evt_yuu_00019740);
