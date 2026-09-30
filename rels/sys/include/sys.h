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
    s32 field_0x10; // 0x10
    s32 field_0x14; // 0x14
    s32 field_0x18; // 0x18
} SysWorkEntry;

typedef struct SysWork {
    s32 state;              // 0x00
    FileEntry* handle;      // 0x04
    f32 field_0x8;          // 0x08
    s32 field_0xC;          // 0x0C
    s32 field_0x10;         // 0x10
    s32 field_0x14;         // 0x14
    s32 field_0x18;         // 0x18
    f32 field_0x1C;         // 0x1C
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
