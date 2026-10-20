#pragma once

#include "evt/evt_cmd.h"

/*
 * Per-instance state allocated by senkaron_ext_init for the ext model effect.
 */
typedef struct SenkaronEntry {
    s8 state;        // 0x0
    u8 pad_1[3];     // 0x1
    s32 timer;       // 0x4, frames left in the current state
    f32 velX;        // 0x8, per-frame step in X (also the wander/fly direction)
    f32 velY;        // 0xC, per-frame step in Y (starts at -5, -0.3 per frame of fall)
    f32 velZ;        // 0x10, per-frame step in Z
    u32 flags;       // 0x14, bit 0 = facing flag, picked at random when a wander starts
} SenkaronEntry;

void nokotaro_callback(s32 index);
USER_FUNC(unk_gon_00000374);
EVT_DEF(tegami_event);
EVT_DEF(gon_03_init_evt);
EVT_DEF(richard_init_event);
EVT_DEF(richard_regl_event);
EVT_DEF(richard_talk_event);
EVT_DEF(leaderkaron_init_event);
EVT_DEF(leaderkaron_talk_event);
EVT_DEF(senkarondummy_init_event);
EVT_DEF(senkarondummy_regl_event);
EVT_DEF(senkarondummy_talk_event);
EVT_DEF(senkaron_entry_event);
EVT_DEF(senkaron_escape_event);
