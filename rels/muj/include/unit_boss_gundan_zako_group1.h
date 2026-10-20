#pragma once

#include "battle/battle_unit.h"
#include "driver/extdrv.h"
#include "evt/evt_cmd.h"

// The 0x70-byte per-effect work entry the zakogun group units share.  Same layout as group3's copy: group1 owns the
// definitions of these helpers, but group3's header predates group1 having one, so the types are repeated here
// rather than restructured (no translation unit includes both headers).
typedef struct ZakogunEntry {
    u8 field_0x0;           // 0x0
    s8 state;               // 0x1
    u8 pad_2[2];            // 0x2
    s32 timer;              // 0x4
    s32 timerMax;           // 0x8
    Vec pos;                // 0xC
    Vec target;             // 0x18
    s32 poseStart;          // 0x24
    s32 poseCount;          // 0x28
    s32 poseDiv;            // 0x2C
    s32 poseIndex;          // 0x30
    Vec from;               // 0x34, interpolation start
    Vec to;                 // 0x40, interpolation end
    f32 velX;               // 0x4C, per-frame x drift
    f32 velY;               // 0x50, vertical velocity (gravity added per frame)
    f32 velZ;               // 0x54, per-frame z drift
    f32 offsetX;            // 0x58, x offset from the controller position
    f32 offsetY;            // 0x5C, y offset from the controller position
    u8 pad_60[4];           // 0x60
    f32 rotX;               // 0x64
    f32 rotY;               // 0x68
    f32 rotZ;               // 0x6C
} ZakogunEntry;

// The 0x74-byte control block group1 allocates; its 0x70 field points at the ZakogunEntry array.
typedef struct ZakogunWork {
    s32 phase;              // 0x0, 0 group1, 1 group2, 2 group3
    s32 targetId;           // 0x4
    s32 state;              // 0x8
    s32 timer;              // 0xC
    Vec pos;                // 0x10
    u8 pad_1c[0x34 - 0x1C]; // 0x1C
    f32 radius;             // 0x34
    f32 angle;              // 0x38
    f32 speed;              // 0x3C
    u8 pad_40[0x68 - 0x40]; // 0x40
    s32 landedCount;        // 0x68
    u8 pad_6c[4];           // 0x6C
    ZakogunEntry* entries;  // 0x70
} ZakogunWork;

extern ZakogunEntry* zakogun;
extern ZakogunWork* work_20_bss_6C;

// Six f32 offsets indexed by case in zakogun_ext_main1's state-0x14 handling.
typedef struct ZakoOffsetTable {
    f32 v[6];
} ZakoOffsetTable;

extern const ZakoOffsetTable dat_muj_00012fdc;

BattleUnit* zakogun_get_ptr(void);
void zakogun_set_pose(ZakogunEntry* entry, s32 poseStart, s32 poseCount, s32 poseDiv);
void zakogun_ext_init123_common(ExtPose* pose, s32 index);
s32 zakogun_get_kanbu_id(void);

USER_FUNC(zakogun_end_sukima);
USER_FUNC(zakogun_start_sukima);
USER_FUNC(zakogun_escape1);
USER_FUNC(zakogun_dead1);
USER_FUNC(zakogun_attack1);
void zakogun_ext_main1(void);
USER_FUNC(zakogun_ext_init1);
USER_FUNC(zakogun_get_kanbu_id_evtcmd);
void zakogun_ext_disp(CameraId cameraId, void* param);
void zakogun_ext_main(void);
void zakogun_ext_init(void);
USER_FUNC(zakogun_damage_pose_cancel);
USER_FUNC(zakogun_damage_pose);
USER_FUNC(zakogun_set_target_id);
void zakogun_ext_main3(void);
EVT_DEF(damage_event_kaiten_hammer);
EVT_DEF(zakogun_entry);
