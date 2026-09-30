#pragma once

#include "evt/evt_cmd.h"

// Work structure for the "poof" cloud effect.
typedef struct CloudEntry {
    s32 flags;                // 0x0
    s32 mode;                 // 0x4
    char animName[0x20];      // 0x8
    char imgName[0x20];       // 0x28
    char offName[0x20];       // 0x48
    char mapGrpName[8][0x20]; // 0x68
    char hitName[0x20];       // 0x168
    s32 maxCount;             // 0x188
    s32 counter;              // 0x18C
    EventEntry* event;        // 0x190
    void* script;             // 0x194
    s32 timer;                // 0x198
    s32 left;                 // 0x19C
    s32 top;                  // 0x1A0
    s32 width;                // 0x1A4
    s32 height;               // 0x1A8
    f32 lastTime;             // 0x1AC
} CloudEntry;                 // 0x1B0

USER_FUNC(evt_las_cloud_ent);
USER_FUNC(evt_las_cloud_init);
EVT_DEF(touch_evt);
EVT_DEF(las_14_init_evt);
