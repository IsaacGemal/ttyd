#pragma once

#include "evt/evt_cmd.h"

// Work structure for the area-local "poof" cloud effect (the REL copy of
// evt_cloud.c's CloudEntry, with a map-group name list instead of one name).
typedef struct CloudEntry {
    s32 flags;                // 0x0
    s32 mode;                 // 0x4
    char animName[0x20];      // 0x8
    char imgName[0x20];       // 0x28
    char offName[0x20];       // 0x48
    char mapGrpName[8][0x20]; // 0x68
    char hitName[0x20];       // 0x168
    s32 breathLimit;          // 0x188
    s32 breathCount;          // 0x18C
    EventEntry* event;        // 0x190
    void* endScript;        // 0x194
    s32 waitTimer;          // 0x198
    s32 field_0x19C;          // 0x19C
    s32 field_0x1A0;          // 0x1A0
    s32 field_0x1A4;          // 0x1A4
    s32 field_0x1A8;          // 0x1A8
    f32 lastLoopTimes;          // 0x1AC
} CloudEntry;                 // 0x1B0

USER_FUNC(evt_iri_16_cloud_init);
USER_FUNC(evt_iri_16_cloud_ent);
EVT_DEF(evt_sound);
EVT_DEF(gonbaba_event);
EVT_DEF(iri_16_box_evt);
EVT_DEF(iri_16_cloud_evt);
EVT_DEF(iri_16_touch_evt);
EVT_DEF(gon_11_init_evt);
