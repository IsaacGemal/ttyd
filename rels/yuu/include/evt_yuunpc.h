#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "driver/extdrv.h"
#include "driver/npcdrv.h"
#include <dolphin/mtx.h>

// YuunpcEntry: one per NPC entry in YuunpcWp.entries (stride 0x70).
typedef struct YuunpcEntry {
    u16 field_0;      //0x0
    u16 flags;        //0x2
    s16 field_4;      //0x4
    u16 pad_06;       //0x6
    f32 field_8;      //0x8
    Vec field_C;      //0xC
    Vec field_18;     //0x18
    Vec field_24;     //0x24  position
    Vec field_30;     //0x30  velocity
    f32 field_3C;     //0x3C  velocity scale
    f32 field_40;     //0x40
    f32 field_44;     //0x44
    f32 field_48;     //0x48
    s32 field_4C;     //0x4C
    s32 field_50;     //0x50
    s16 field_54;     //0x54
    s16 field_56;     //0x56
    s16 field_58;     //0x58
    s16 field_5A;     //0x5A
    s16 field_5C;     //0x5C
    u8 pad_5E[2];     //0x5E
    NpcEntry* npc;    //0x60
    f32 field_64;     //0x64
    u64 field_68;     //0x68
} YuunpcEntry;

typedef struct YuunpcPoseEntry {
    char name[0x10];  //0x0
    char anim[8];     //0x10
    s16 field_18;     //0x18
    u16 pad_1A;       //0x1A
    f32 field_1C;     //0x1C
    f32 field_20;     //0x20
    s32 field_24;     //0x24
    s32 field_28;     //0x28
    u16 flags;        //0x2C
    u16 pad_2E;       //0x2E
    f32 field_30;     //0x30
} YuunpcPoseEntry;

// One entry of the pose description passed to yuunpc_pose_init; stride 0xC.
typedef struct YuunpcPoseArg {
    char* name;   //0x0
    char* anim;   //0x4
    f32 field_8;  //0x8
} YuunpcPoseArg;

typedef struct YuunpcWp {
    u16 flags;                     //0x0
    u16 pad_02;                    //0x2
    s32 count;                     //0x4
    s32 field_08[3];               //0x8
    YuunpcEntry* entries;          //0x14
    s32 field_18;                  //0x18
    s32 field_1C;                  //0x1C
    s32 field_20;                  //0x20
    s16 map[7 * 72];               //0x24
    YuunpcPoseEntry poseEntries[32];  //0x414
} YuunpcWp;

void ext_yuunpc_minigame4_main(void);
void ext_yuunpc_minigame3_main(void);
void ext_yuunpc_minigame2_main(void);
void ext_yuunpc_minigame1_main(void);
void ext_yuunpc_dispent(void);
void yuunpc_disp(CameraId cameraId, void* param);
void yuunpc_pose_minigame4_main(void);  // TODO: real signature
void ext_yuunpc_minigame4_init(void);
s32 yuunpc_juni(Vec* pos);
u16 yuunpc_wp_flag_check(u16 mask);
void yuunpc_wp_flag_off(u16 mask);
void yuunpc_wp_flag_on(u16 mask);
void errper_set_rtn(YuunpcEntry* p);
s32 goalincheck(YuunpcEntry* p);
s32 fallcheck(YuunpcEntry* p);
s32 hitcheck(YuunpcEntry* p);
void yuunpc_pose_minigame3_main(void);
void ext_yuunpc_minigame3_init(void);  // TODO: real signature
void yuunpc_pose_minigame2_main(void);
void ext_yuunpc_minigame2_init(void);  // TODO: real signature
void yuunpc_pose_minigame1_main(void);
void ext_yuunpc_minigame1_init(void);  // TODO: real signature
f32 yuunpc_pose_sub(ExtPose* pose, YuunpcEntry* entry);
void yuunpc_set_jump_pose(const char* name, const char* anim, f32 value);
void yuunpc_pose_change2(YuunpcEntry* entry, const char* name, const char* anim, f32 a);
void yuunpc_pose_change(YuunpcEntry* entry, s16 animIndex, f32 a, f32 b);
s32 yuunpc_pose_check(const char* name, const char* anim);
void yuunpc_pose_init(YuunpcPoseArg* arg);
void yuunpc_maphit_init(void);
extern u32 chusan_act_tbl[8];
EVT_DEF(evt_yuunpc_minigame1);
EVT_DEF(evt_yuunpc_minigame2);
EVT_DEF(evt_yuunpc_minigame3);
EVT_DEF(evt_yuunpc_minigame4);
void ext_yuunpc_minigame4_init_sub(void);
