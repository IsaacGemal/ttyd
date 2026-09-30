#pragma once

#include "driver/extdrv.h"
#include "evt/evt_cmd.h"

// Per-pose state used by hei_10's extended-pose hop/escape updaters.
typedef struct HeiExtEntry {
    s16 field_0x00; // 0x0
    s16 state;      // 0x2
    s16 field_0x04; // 0x4
    u8 pad_06[2];
    Vec position;   // 0x8
    f32 field_0x14; // 0x14
    s16 field_0x18; // 0x18
    u8 pad_1A[2];
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    s16 field_0x24; // 0x24
    u8 pad_26[0xA];
} HeiExtEntry;

// Per-pose state table allocated by hei_10's ext_init.
typedef struct HeiExtWork {
    s32 field_0x0;        // 0x0
    s32 count;            // 0x4
    HeiExtEntry* entries; // 0x8
} HeiExtWork;

extern s32 hei_test_water_tev[];

// Overlay for the unnamed 0x20..0x3C gap in AnimationPose; 0x24 holds the
// pose's current frame, which wait_anim_frame compares against its argument.
typedef struct AnimPoseField24 {
    u8 pad_00[0x24];
    f32 field_0x24;
} AnimPoseField24;

// One texture/pose pair in the extended-pose table passed to evt_ext_entry.
typedef struct ExtEntry {
    const char* name;    // 0x0
    const char* texName; // 0x4
    f32 value;           // 0x8
} ExtEntry;

USER_FUNC(wait_anim_end);
USER_FUNC(wait_anim_frame);
USER_FUNC(evt_chorobon_wait);
USER_FUNC(evt_chorobon_escape);
void chb_jump(ExtPose* pose, HeiExtEntry* entry, s16 field24, f32 x, f32 y, f32 z, f32 v1, f32 v2);
EVT_DEF(golden_se);
EVT_DEF(evt_golden);
EVT_DEF(hei_10_init_evt);
