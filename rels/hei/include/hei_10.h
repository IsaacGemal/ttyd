#pragma once

#include "driver/extdrv.h"
#include "evt/evt_cmd.h"

// Per-pose state used by hei_10's extended-pose hop/escape updaters.
typedef struct HeiExtEntry {
    s16 field_0x00; // 0x0
    s16 state;      // 0x2
    s16 field_0x04; // 0x4
    u8 pad_06[2];
    Vec position;   // 0x8, the hop's landing point
    f32 velY;       // 0x14, added to the pose's y each hop frame, less gravity each frame
    s16 framesLeft; // 0x18, hop frames left; the pose lands on position when it drops below 0
    u8 pad_1A[2];
    f32 speed;      // 0x1C, horizontal distance per hop frame
    f32 gravity;    // 0x20, taken off velY each hop frame
    s16 nextState;  // 0x24, the state set when the hop lands
    u8 pad_26[0xA];
} HeiExtEntry;

// Per-pose state table allocated by hei_10's ext_init.
typedef struct HeiExtWork {
    s32 field_0x0;        // 0x0
    s32 count;            // 0x4
    HeiExtEntry* entries; // 0x8
} HeiExtWork;

extern s32 hei_test_water_tev[];

USER_FUNC(wait_anim_end);
USER_FUNC(wait_anim_frame);
USER_FUNC(evt_chorobon_wait);
USER_FUNC(evt_chorobon_escape);
void chb_jump(ExtPose* pose, HeiExtEntry* entry, f32 x, f32 y, f32 z, f32 speed, f32 gravity, s16 nextState);
EVT_DEF(golden_se);
EVT_DEF(evt_golden);
EVT_DEF(hei_10_init_evt);
