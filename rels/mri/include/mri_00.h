#pragma once

#include "evt/evt_cmd.h"

typedef struct CloudEntry {
    s32 flags;                // 0x0
    s32 mode;                 // 0x4
    char animName[0x20];      // 0x8
    char imgName[0x20];       // 0x28
    char offName[0x20];       // 0x48
    s8 mapGrpName[8][0x20]; // 0x68
    char hitName[0x20];       // 0x168
    s32 breathLimit;          // 0x188
    s32 breathCount;          // 0x18C
    EventEntry* event;        // 0x190
    void* endScript;          // 0x194
    s32 waitTimer;            // 0x198
    s32 left;                 // 0x19C
    s32 top;                  // 0x1A0
    s32 width;                // 0x1A4
    s32 height;               // 0x1A8
    f32 lastLoopTimes;        // 0x1AC
} CloudEntry;                 // 0x1B0

s32 _ofs_get_boundingbox(const char* name, s32* a, s32* b, s32* c, s32* d);
USER_FUNC(evt_mri_cloud_init);
USER_FUNC(evt_mri_cloud_ent);
USER_FUNC(get_puni_pos);
EVT_DEF(mario_see_you_init);
EVT_DEF(seal_bigtree);
EVT_DEF(seal_bigtree2);
EVT_DEF(out_ne_bero);
EVT_DEF(out_dokan_1);
EVT_DEF(first_bigtree);
EVT_DEF(open_bigtree);
EVT_DEF(mario_see_you);
EVT_DEF(puni_set);
EVT_DEF(sekai_yami_puni);
EVT_DEF(mri_00_init_evt);
