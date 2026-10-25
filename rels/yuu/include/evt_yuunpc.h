#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "driver/extdrv.h"
#include "driver/npcdrv.h"
#include <dolphin/mtx.h>

// YuunpcEntry: one per NPC entry in YuunpcWp.entries (stride 0x70).
typedef struct YuunpcEntry {
    u16 index;        //0x0, the entry's number; varies lanes and timing per runner (index % 101, index & 3)
    u16 flags;        //0x2
    s16 field_0x4;    //0x4
    u16 pad_06;       //0x6
    f32 errper;       //0x8, set by errper_set_rtn (+-0.25 or +-0.1); minigame 4 shifts the runner's lane bounds by 150 * errper
    Vec goal;         //0xC, finish point; goalincheck moves target to it, and stops the runner there
    Vec target;       //0x18, point the runner heads for (dir)
    Vec position;     //0x24, copied into the pose matrix and the NPC
    Vec dir;          //0x30, unit x/z direction toward target
    f32 speed;        //0x3C, step along dir per frame (0 standing, 0.5-2.25 by moveState); goalincheck's reach
    f32 velY;         //0x40, fallcheck: added to position.y, gravity 0.533 a frame
    f32 spinHeading;  //0x44, pose heading when the flags 0x8/0x10 spin started
    f32 pushZ;        //0x48, the stream's z push added to each hitcheck step
    s32 moveState;    //0x4C, minigame 4: 0 stand, 1 walk, 2 run, 3 fall, 4 jump
    s32 spinTimer;    //0x50, frames left in the 30-frame spin
    s16 poseIndex;    //0x54, current YuunpcWp.poseEntries index
    s16 standPose;    //0x56, poseEntries index for moveState 0 (an S_1 animation)
    s16 walkPose;     //0x58, for moveState 1 (W_1, or R_1 where there is none)
    s16 runPose;      //0x5A, for moveState 2 (R_1)
    s16 jumpPose;     //0x5C, held at its jumpFrame for moveStates 3 and 4 (R_1)
    u8 pad_5E[2];     //0x5E
    NpcEntry* npc;    //0x60
    f32 cycleScale;   //0x64, the pose cycle lasts cycleFrames * cycleScale frames (yuunpc_pose_sub)
    u64 animTime;     //0x68, renderTime the pose cycle started; with flags 0x4 the held offset into it
} YuunpcEntry;

typedef struct YuunpcPoseEntry {
    char name[0x10];  //0x0
    char anim[8];     //0x10
    s16 field_0x18;   //0x18
    u16 pad_1A;       //0x1A
    f32 cycleFrames;  //0x1C, frames for one pass through the group's poses
    f32 jumpFrame;    //0x20, yuunpc_set_jump_pose: the frame a jumpPose is held at
    s32 firstPose;    //0x24, texId of the group's first pose
    s32 numPoses;     //0x28
    u16 flags;        //0x2C
    u16 pad_2E;       //0x2E
    f32 walkCycleScale; //0x30, cycleScale the walk state (moveState 1) plays the group with
} YuunpcPoseEntry;

// One entry of the pose description passed to yuunpc_pose_init; stride 0xC.
typedef struct YuunpcPoseArg {
    char* name;       //0x0
    char* anim;       //0x4
    f32 cycleFrames;  //0x8, for the entries after the first list's terminator
} YuunpcPoseArg;

typedef struct YuunpcWp {
    u16 flags;                     //0x0
    u16 pad_02;                    //0x2
    s32 count;                     //0x4
    s32 spIndex[3];                //0x8, entries that become the yuusp0-yuusp2 NPCs (-1 for none)
    YuunpcEntry* entries;          //0x14
    s32 groupCount;                //0x18, poseEntries built from the first list
    s32 poseEntryCount;            //0x1C, poseEntries in use
    s32 chusanIndex;               //0x20, poseEntries index of c_tyusan CHU_S_1 (played through chusan_*_tbl)
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
