#pragma once

#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"

// 0x30-byte per-effect entry the chorogun horde unit walks; chorogun is a pointer to 1000 of them.
typedef struct ChorogunEntry {
    u8 flags;        // 0x0
    u8 state;        // 0x1
    u8 pad_2[2];     // 0x2
    Vec position;    // 0x4
    f32 speed;       // 0x10
    Vec target;      // 0x14
    s32 timer;       // 0x20
    f32 targetX;     // 0x24
    u8 pad_28[4];    // 0x28
    f32 targetZ;     // 0x2C
} ChorogunEntry;

extern ChorogunEntry* chorogun;
extern s32 chorogun_targetId;

// One entry of the ext-entry table handed to evt_ext_entry for the chorogun horde.
typedef struct ChorogunExtEntry {
    const char* modelName; // 0x0
    const char* poseName;  // 0x4
    f32 field_0x8;         // 0x8
} ChorogunExtEntry;

USER_FUNC(chorogun_escape_all);
USER_FUNC(chorogun_escape);
USER_FUNC(chorogun_after_attack);
USER_FUNC(chorogun_counterd);
USER_FUNC(chorogun_bound_target);
USER_FUNC(chorogun_dive_target);
USER_FUNC(chorogun_approach_target);
USER_FUNC(chorogun_appear);
USER_FUNC(chorogun_set_target_id);
USER_FUNC(chorogun_get_alive_num);
void chorogun_ext_disp(void);
void chorogun_ext_main(void);
void chorogun_ext_init(void);
BattleUnit* chorogun_get_ptr(void);
EVT_DEF(unk_evt_hei_00019e30);
EVT_DEF(chorogun_entry);
