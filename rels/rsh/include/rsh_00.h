#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>

// moamoa_data entry: model/pose name pair with the frame index it starts at.
typedef struct MoamoaData {
    const char* name1; //0x0
    const char* name2; //0x4
    f32 frame;         //0x8
} MoamoaData;

// One pose sequence in the moamoa work table (work->poses).
typedef struct MoamoaPose {
    char name1[16]; //0x0
    char name2[8];  //0x10
    s16 field_0x18; //0x18
    s16 field_0x1A; //0x1A
    f32 duration;   //0x1C
    s32 startFrame; //0x20
    s32 frameCount; //0x24
} MoamoaPose;

// One moamoa actor, indexed the same as the ExtPose array.
typedef struct MoamoaEntry {
    u16 flags;      //0x0
    s16 state;      //0x2
    s16 counter;    //0x4
    u16 field_0x6;  //0x6
    Vec position;   //0x8
    f32 angle;      //0x14
    s16 field_0x18; //0x18
    s16 field_0x1A; //0x1A
    s32 field_0x1C; //0x1C
    OSTime time;    //0x20
} MoamoaEntry;

// The whole moamoa work area; poses are inline after the header.
typedef struct MoamoaWork {
    u32 field_0x0;        //0x0
    s32 poseNum;          //0x4
    MoamoaEntry* entries; //0x8
    s32 poseCount;        //0xC
    MoamoaPose poses[32]; //0x10
} MoamoaWork;

USER_FUNC(evt_moamoa_set_seq_all);
USER_FUNC(evt_moamoa_set_one);
void moamoa_dispent(void);
void moamoa_main(void);
void moamoa_init(void);
EVT_DEF(evt_rsh_moamoa_on_window);
EVT_DEF(evt_handle);
EVT_DEF(regu_unten);
EVT_DEF(rsh_00_init_evt);
EVT_DEF(rsh_00_a_init_evt);
EVT_DEF(rsh_00_b_init_evt);
EVT_DEF(rsh_00_c_init_evt);
EVT_DEF(rsh_00_init_evt_nidome);
EVT_DEF(rsh_00_a_init_evt_nidome);
EVT_DEF(rsh_00_b_init_evt_nidome);
EVT_DEF(rsh_00_c_init_evt_nidome);
