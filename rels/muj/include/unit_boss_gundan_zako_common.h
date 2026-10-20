#pragma once

#include "battle/battle_unit.h"
#include "driver/extdrv.h"
#include "evt/evt_cmd.h"

// Types and helpers shared by the three X-Naut Crump formation units.  group1 defines the helpers and
// allocates the work block; group2 and group3 run their phases on the same block.

// The 0x70-byte per-Crump work entry.
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
    Vec to;                 // 0x40, interpolation end (.y doubles as the landing height)
    f32 velX;               // 0x4C, per-frame x drift
    f32 velY;               // 0x50, vertical velocity (gravity added per frame)
    f32 velZ;               // 0x54, per-frame z drift
    f32 offsetX;            // 0x58, group3: x offset from ZakogunWork.pos once stuck to the ball
    f32 offsetY;            // 0x5C, group3: y offset from ZakogunWork.pos once stuck to the ball
    u8 pad_60[4];           // 0x60
    f32 rotX;               // 0x64, degrees
    f32 rotY;               // 0x68, degrees
    f32 rotZ;               // 0x6C, degrees
} ZakogunEntry;

// The 0x74-byte control block group1 allocates; its 0x70 field points at the ZakogunEntry array.
typedef struct ZakogunWork {
    s32 phase;              // 0x0, 0 group1, 1 group2, 2 group3; picks zakogun_ext_main1/2/3
    s32 targetId;           // 0x4
    s32 state;              // 0x8
    s32 timer;              // 0xC
    Vec pos;                // 0x10, group3: the ball's centre
    u8 pad_1c[0x34 - 0x1C]; // 0x1C
    f32 radius;             // 0x34, group3: ball radius
    f32 angle;              // 0x38, group3: ball roll angle, degrees (z rotation)
    f32 speed;              // 0x3C, group3: roll speed, added to pos.x and subtracted from angle
    u8 pad_40[8];           // 0x40
    Vec ceilFrom;           // 0x48, group2 ceiling height start (.y)
    Vec ceilTo;             // 0x54, group2 ceiling height end (.y)
    s32 ceilFrame;          // 0x60, group2: counts down from ceilFrames (up_ceil/down_ceil), up in shake_ceil
    s32 ceilFrames;         // 0x64
    s32 landedCount;        // 0x68, Crumps landed this phase: group2 stacks 25, group3 sticks 500 to the ball
    s32 ceilSfxHandle;      // 0x6C
    ZakogunEntry* entries;  // 0x70
} ZakogunWork;

extern ZakogunEntry* zakogun;
extern ZakogunWork* work_20_bss_6C;

BattleUnit* zakogun_get_ptr(void);
void zakogun_set_pose(ZakogunEntry* entry, s32 poseStart, s32 poseCount, s32 poseDiv);
void zakogun_ext_init123_common(ExtPose* pose, s32 index);
s32 zakogun_get_kanbu_id(void);

USER_FUNC(zakogun_damage_pose);
USER_FUNC(zakogun_damage_pose_cancel);
USER_FUNC(zakogun_get_kanbu_id_evtcmd);
USER_FUNC(zakogun_set_target_id);
