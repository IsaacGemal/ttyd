#pragma once

#include "battle/battle_unit.h"
#include "driver/extdrv.h"
#include "evt/evt_cmd.h"

// The 0x70-byte per-effect work entry that the zakogun group units share (group1 defines zakogun_set_pose,
// zakogun_get_ptr and friends).  Kept here because group1 has no header of its own yet.
typedef struct ZakogunEntry {
    u8 field_0x0;         // 0x0
    s8 state;             // 0x1
    u8 pad_2[2];          // 0x2
    s32 timer;            // 0x4
    s32 timerMax;         // 0x8
    Vec pos;              // 0xC
    Vec target;           // 0x18
    s32 poseStart;        // 0x24
    s32 poseCount;        // 0x28
    s32 poseDiv;          // 0x2C
    s32 poseIndex;        // 0x30
    f32 field_0x34;       // 0x34
    f32 field_0x38;       // 0x38
    f32 field_0x3c;       // 0x3C
    f32 field_0x40;       // 0x40
    f32 field_0x44;       // 0x44
    f32 field_0x48;       // 0x48
    u8 pad_4c[4];         // 0x4C
    f32 field_0x50;       // 0x50
    u8 pad_54[4];         // 0x54
    f32 field_0x58;       // 0x58
    f32 field_0x5c;       // 0x5C
    u8 pad_60[4];         // 0x60
    f32 rotX;             // 0x64
    f32 rotY;             // 0x68
    f32 rotZ;             // 0x6C
} ZakogunEntry;

// The 0x74-byte control block group1 allocates; its 0x70 field points at the ZakogunEntry array.
typedef struct ZakogunWork {
    s32 field_0x0;        // 0x0
    s32 targetId;         // 0x4
    s32 state;            // 0x8
    s32 timer;            // 0xC
    Vec pos;              // 0x10
    u8 pad_1c[0x34 - 0x1C]; // 0x1C
    f32 field_0x34;       // 0x34
    f32 field_0x38;       // 0x38
    f32 field_0x3c;       // 0x3C
    u8 pad_40[0x68 - 0x40]; // 0x40
    s32 field_0x68;       // 0x68
    u8 pad_6c[4];         // 0x6C
    ZakogunEntry* entries; // 0x70
} ZakogunWork;

extern ZakogunEntry* zakogun;
extern ZakogunWork* work_20_bss_6C;
USER_FUNC(zakogun_damage_pose);
extern s32 zakogun_damage_pose_cancel[];
extern s32 zakogun_get_kanbu_id_evtcmd[];
USER_FUNC(zakogun_set_target_id);

BattleUnit* zakogun_get_ptr(void);
void zakogun_set_pose(ZakogunEntry* entry, s32 poseStart, s32 poseCount, s32 poseDiv);
void zakogun_ext_init123_common(ExtPose* pose, s32 index);
s32 zakogun_get_kanbu_id(void);

USER_FUNC(zakogun_escape3);
USER_FUNC(zakogun_dead3);
USER_FUNC(zakogun_attack3_3);
USER_FUNC(zakogun_attack3_2);
USER_FUNC(zakogun_attack3_1);
void zakogun_ext_main3(void);
USER_FUNC(zakogun_ext_init3);
USER_FUNC(zakogun_kanbu_on_ball);
