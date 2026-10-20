#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "manager/filemgr.h"

// Work data for the sys_01 stage-select icons (size 0x104).
typedef struct SysWorkEntry {
    f32 x;          // 0x00
    f32 y;          // 0x04
    f32 z;          // 0x08
    f32 angle;      // 0x0C
    s32 timer;      // 0x10, frames to wait before the spin, then frames into it
    s32 phase;      // 0x14, 0 waiting, 1 spinning, 2 spun
    s32 palIndex;   // 0x18, texture index in the effect_sys palette
} SysWorkEntry;

typedef struct SysWork {
    s32 state;              // 0x00
    FileEntry* handle;      // 0x04
    f32 spinAngle;          // 0x08
    s32 palIndex;           // 0x0C, texture index of the rotating icon
    s32 frameCount;         // 0x10
    s32 stage;              // 0x14, index into msg_tbl
    s32 reveal;             // 0x18, revealed chars in the high byte, next char alpha in the low
    f32 textAngle;          // 0x1C
    s32 count;              // 0x20
    SysWorkEntry entries[]; // 0x24
} SysWork;

USER_FUNC(init);
USER_FUNC(bg);
USER_FUNC(start);
USER_FUNC(display);
void display_func(CameraId cameraId, void* param);
s32 getLightColor(f32 angle);
void sys_01_release(void);
void _prolog(void); // TODO: real signature
EVT_DEF(dummy_evt);
EVT_DEF(sys_01_init_evt);
EVT_DEF(evt_start);
