#pragma once

#include "evt/evt_cmd.h"
#include "memory.h"

// The 0xC-byte animation callback work block allocated by pposter_init and
// pointed to by the global `poster`. Driven by draw_poster.
typedef struct PosterWork {
    u32 flags;    // 0x0, bit 0 = visible
    s32 groupIdx; // 0x4
    s32 shapeIdx; // 0x8
} PosterWork;

extern PosterWork* poster;

void nok_stg2_013_cb(s32 type, smartEntry* smart);
USER_FUNC(pposter_nok_on);
USER_FUNC(pposter_init);
void draw_poster(void* userdata, s32 group, void* mtx);
EVT_DEF(nokonoko_A_init);
EVT_DEF(nokonoko_A_regl);
EVT_DEF(nokonoko_A_talk);
EVT_DEF(nokonoko_B_init);
EVT_DEF(nokonoko_B_regl);
EVT_DEF(nokonoko_B_talk);
EVT_DEF(nokonoko_B_damage);
EVT_DEF(nokonoko_C_regl);
EVT_DEF(nokonoko_C_talk);
EVT_DEF(nokonoko_D_talk);
EVT_DEF(nokonoko_F_talk);
EVT_DEF(first_time_camera);
EVT_DEF(nok_00_koopa_npc_entry);
EVT_DEF(nok_00_koopa_evt);
EVT_DEF(nok_00_init_evt);
