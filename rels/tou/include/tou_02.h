#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>

typedef struct FanPose FanPose;
typedef struct PoseGroup PoseGroup;
typedef struct ExtFanWork ExtFanWork;
typedef struct ExtFanData ExtFanData;

struct PoseGroup {
    char name[0x10];  // 0x0
    char motion[0x8]; // 0x10
    u16 field_0x18;   // 0x18
    f32 scale;        // 0x1C
    s32 start;        // 0x20
    s32 count;        // 0x24
};

struct FanPose {
    u16 flags;      // 0x0
    s16 state;      // 0x2, 0 = wandering, 10 = hop to target, 20 = drift to target, 100 = settled
    s16 groupIndex; // 0x4
    s16 field_0x6;  // 0x6
    OSTime time;    // 0x8
    Vec position;   // 0x10
    Vec velocity;   // 0x1C
    f32 field_0x28; // 0x28
    Vec target;     // 0x2C, the hop's landing point; the per-frame velocity while drifting (state 20)
    union {
        f32 hopVelY;     // 0x38, state 10: rise per frame, reduced by hopGravity each frame
        f32 driftAccelX; // 0x38, state 20: added to target.x, the velocity, each frame
    };
    union {
        f32 hopSpeed;    // 0x3C, state 10: x/z step toward target per frame (timer = distance / hopSpeed)
        f32 driftAccelY; // 0x3C, state 20: added to target.y
    };
    union {
        f32 hopGravity;  // 0x40, state 10: taken off hopVelY each frame
        f32 driftAccelZ; // 0x40, state 20: added to target.z
    };
    s16 timer;      // 0x44, frames left in states 10 and 20
    s16 field_0x46; // 0x46
};

struct ExtFanData {
    char* name;   // 0x0
    char* motion; // 0x4
    f32 scale;    // 0x8
};

struct ExtFanWork {
    s32 poseNum;                  // 0x0
    FanPose* poses;               // 0x4
    s32 groupCount;               // 0x8
    PoseGroup groups[3];          // 0xC
    u8 field_0x84[0x50C - 0x84];  // 0x84
    s32 hammerFrames;             // 0x50C, frames Mario has been in MARIO_MOTION_HAMMER
    s32 idleCount;                // 0x510, fans in state 0 not pinned to Mario or the champion (flags bit 0)
    s32 sfxId;                    // 0x514
    u8 volume;                    // 0x518
    u8 field_0x519[3];            // 0x519
};

USER_FUNC(evt_end_count);
s32 check_hammer(FanPose* pose);
void ext_fan_dispent(void);
void ext_fan_main(void);
void fan_disp(void);
void ext_fan_init(void);
EVT_DEF(n_1_door_open);
EVT_DEF(n_1_door_close);
EVT_DEF(n_3_door_open);
EVT_DEF(n_3_door_close);
EVT_DEF(n_4_door_open);
EVT_DEF(n_4_door_close);
EVT_DEF(evt_move_g);
EVT_DEF(init_gard1);
EVT_DEF(talk_gard1);
EVT_DEF(init_gard2);
EVT_DEF(talk_gard2);
EVT_DEF(init_repo);
EVT_DEF(talk_repo);
EVT_DEF(init_cam1);
EVT_DEF(talk_cam1);
EVT_DEF(init_cam2);
EVT_DEF(talk_cam2);
EVT_DEF(init_kino4);
EVT_DEF(reg_kino4);
EVT_DEF(talk_kino4);
EVT_DEF(init_kino5);
EVT_DEF(talk_kino5);
EVT_DEF(init_kinopiko);
EVT_DEF(talk_kinopiko);
EVT_DEF(init_kinopio_f);
EVT_DEF(talk_kinopio_f);
EVT_DEF(init_chu);
EVT_DEF(talk_chu);
EVT_DEF(move_champ);
EVT_DEF(talk_champ);
EVT_DEF(init_shikowa);
EVT_DEF(talk_shikowa);
EVT_DEF(init_ironA);
EVT_DEF(talk_ironA);
EVT_DEF(init_ironB);
EVT_DEF(talk_ironB);
EVT_DEF(init_borodo);
EVT_DEF(talk_borodo);
EVT_DEF(evt_tougi2);
EVT_DEF(mario_momikucha_msg);
EVT_DEF(evt_momikucha);
EVT_DEF(tou_02_init_evt);
