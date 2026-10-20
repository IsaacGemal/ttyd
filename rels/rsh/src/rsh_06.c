#include "rsh_06.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "party.h"
#include "party/party_nokonoko.h"
#include "party/party_cloud.h"
#include "pmario_sound.h"
#include "driver/camdrv.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/envdrv.h"
#include "driver/extdrv.h"
#include "driver/hitdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

typedef struct MoamoaEntry {
    u16 flags;       // 0x0
    s16 state;       // 0x2, switch key in ext_moamoa_main
    s16 stateTimer;  // 0x4, frames left in the current state
    u8 pad_06[0x2];
    Vec field_0x8;   // 0x8
    f32 field_0x14;  // 0x14
    s16 frames;      // 0x18, frames left in the current jump or glide
    u8 pad_1A[0x2];
    f32 field_0x1C;  // 0x1C
    f32 field_0x20;  // 0x20
    s16 queuedState; // 0x24, state to enter when the current jump lands
    u8 pad_26[0x28 - 0x26];
    f32 field_0x28;  // 0x28
    f32 field_0x2C;  // 0x2C
    Vec field_0x30;  // 0x30
    s16 groupIndex;  // 0x3C, index into MoamoaWork.poses
    u8 pad_3E[0x2];
    OSTime poseTime; // 0x40
    s16 chain;       // 0x48, index into MoamoaWork.chainGroup
    s16 chainSlot;   // 0x4A, this entry's slot among those with the same chain
    u8 kind;         // 0x4C, index into group_tbl
    u8 pad_4D[0x50 - 0x4D];
} MoamoaEntry;

typedef struct MoamoaPose {
    char name[0x10]; // 0x0
    char anim[0x8];  // 0x10
    u16 enabled;     // 0x18, 0 for the J-prefixed animations
    u8 pad_1A[0x2];
    f32 cycleFrames; // 0x1C, frames for one pass through the group's poses
    s32 firstPose;   // 0x20
    s32 numPoses;    // 0x24
} MoamoaPose; // 0x28

typedef struct MoamoaData {
    char* name; // 0x0
    char* anim; // 0x4
    f32 value;  // 0x8
} MoamoaData; // 0xC

typedef struct MoamoaGroup {
    char* name; // 0x0
    char* anim; // 0x4
} MoamoaGroup;

typedef struct MoamoaWork {
    u16 flags;            // 0x0
    u8 pad_02[0x2];
    s32 count;            // 0x4
    MoamoaEntry* entries; // 0x8
    u8 pad_0C[0x8];
    Vec vec[3];        // 0x14
    s16 hitCount;      // 0x38, incremented on each hit
    u16 nearHits[2];   // 0x3A, per station; set to 100 by evt_moamoa_target_off
    u8 pad_3E[0x40 - 0x3E];
    s16 field_0x40;    // 0x40
    s16 idleFrames;    // 0x42, frames Mario has stood still (capped at 1000)
    u8 pad_44[0x48 - 0x44];
    s32 poseId;        // 0x48, pose id of the "base" NPC
    s32 poseCount;     // 0x4C
    MoamoaPose poses[1]; // 0x50
    u8 pad_78[0x550 - 0x78];
    f32 angleToMario;  // 0x550, angle from Mario to the moamoa in the current pose
    f32 distToMario;   // 0x554, distance from Mario to the current pose
    s32 field_0x558;   // 0x558
    f32 breathDir;     // 0x55C, cloud breath direction (-1 when not in use)
    f32 breathDist;    // 0x560, cloud breath distance
    Vec breathPos;     // 0x564
    s16 chainSlots[0x80];   // 0x570, per chain: how many entries have taken a slot
    struct { s16 x; s16 y; } chainGroup[0x80]; // 0x670, per chain: name group (x) and anim group (y)
} MoamoaWork;

static MoamoaWork* wp;

static void resha_tev(void* param);
static USER_FUNC(ramp_r);
static void ext_moamoa_dispent(void);  // TODO: real signature
static void ext_moamoa_main(void);  // TODO: real signature
static s32 moamoa_hit_check(ExtPose* pose, MoamoaEntry* entry);
static void moamoa_jump(ExtPose* pose, MoamoaEntry* entry, f32 x, f32 y, f32 z, f32 p4, f32 p5, s32 field0x2, s32 flag);
static void ext_shadow_disp(CameraId cameraId, void* param);
static void ext_disp(CameraId cameraId, void* param);
static void ext_moamoa_init(void);  // TODO: real signature
static f32 randf(f32 min, f32 max);
static USER_FUNC(evt_moamoa_target_off);
static USER_FUNC(evt_moamoa_fusion_debug);
static USER_FUNC(evt_moamoa_blow);
static USER_FUNC(evt_moamoa_kind_onoff);
static USER_FUNC(evt_moamoa_fusion);
static USER_FUNC(evt_moamoa_end_wait);
static USER_FUNC(evt_moamoa_ry_reset);
static USER_FUNC(evt_moamoa_exit);
static USER_FUNC(evt_moamoa_flag_onoff);
static void moamoa_pose_change(MoamoaEntry* entry, char* name, char* anim);
static void moamoa_pose_main(void);
static void moamoa_pose_init(void);
s32 evt_great_moamoa[737];

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 ext_moamoa_data[213] = {
    (u32)"c_n_moa_a", (u32)"S_1", 0x00000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x40000000, (u32)"c_n_moa_a", (u32)"S_1",
    0x40800000, (u32)"c_n_moa_a", (u32)"S_1", 0x40C00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41200000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41400000, (u32)"c_n_moa_a", (u32)"S_1", 0x41600000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41800000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41900000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41A00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41B00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41C00000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41D00000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41E00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41F00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x42000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x42080000, (u32)"c_n_moa_a", (u32)"S_1",
    0x42100000, (u32)"c_n_moa_a", (u32)"S_1", 0x42180000,
    (u32)"c_n_moa_a", (u32)"D_1", 0x00000000, (u32)"c_n_moa_a",
    (u32)"D_1", 0x40800000, (u32)"c_n_moa_a", (u32)"D_1",
    0x41000000, (u32)"c_n_moa_b", (u32)"S_1", 0x00000000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x40000000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x40800000, (u32)"c_n_moa_b", (u32)"S_1",
    0x40C00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41000000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41200000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41400000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41600000, (u32)"c_n_moa_b", (u32)"S_1", 0x41800000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41900000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41A00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41B00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41C00000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41D00000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41E00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41F00000, (u32)"c_n_moa_b", (u32)"S_1", 0x42000000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x42080000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x42100000, (u32)"c_n_moa_b", (u32)"S_1",
    0x42180000, (u32)"c_n_moa_b", (u32)"D_1", 0x00000000,
    (u32)"c_n_moa_b", (u32)"D_1", 0x40800000, (u32)"c_n_moa_b",
    (u32)"D_1", 0x41000000, (u32)"c_kinodebu", (u32)"G_1",
    0x00000000, (u32)"c_kinodebu", (u32)"G_1", 0x40800000,
    (u32)"c_kinodebu", (u32)"G_1", 0x41000000, (u32)"c_puku_c",
    (u32)"U_1", 0x00000000, (u32)"c_puku_c", (u32)"U_1",
    0x40800000, (u32)"c_puku_c", (u32)"U_1", 0x41000000,
    (u32)"c_powan", (u32)"O_1", 0x00000000, (u32)"c_powan",
    (u32)"O_1", 0x40800000, (u32)"c_powan", (u32)"O_1",
    0x41000000, (u32)"c_smoko", (u32)"T_2", 0x00000000,
    (u32)"c_smoko", (u32)"T_2", 0x40800000, (u32)"c_smoko",
    (u32)"T_2", 0x41000000, (u32)"c_rose", (u32)"G_1",
    0x00000000, (u32)"c_rose", (u32)"G_1", 0x40800000,
    (u32)"c_rose", (u32)"G_1", 0x41000000, (u32)"c_papab",
    (u32)"G_1", 0x00000000, (u32)"c_papab", (u32)"G_1",
    0x40800000, (u32)"c_papab", (u32)"G_1", 0x41000000,
    (u32)"c_mamab", (u32)"G_1", 0x00000000, (u32)"c_mamab",
    (u32)"G_1", 0x40800000, (u32)"c_mamab", (u32)"G_1",
    0x41000000, (u32)"c_konari", (u32)"G_1", 0x00000000,
    (u32)"c_konari", (u32)"G_1", 0x40800000, (u32)"c_konari",
    (u32)"G_1", 0x41000000, 0x00000000, 0x00000000,
    0x00000000,
};

#ifdef MUST_MATCH
// As a const object MWCC folds the known contents and hoists the three loads out of
// the loop; the original reloads them inside the loop.
__declspec(section ".rodata") Vec vec3_rsh_00013cec = {0.0f, -1000.0f, 0.0f};
#else
const Vec vec3_rsh_00013cec = {0.0f, -1000.0f, 0.0f};
#endif
// Non-const on purpose: ext_moamoa_init copies all three into wp->vec; as `const` MWCC knows they
// are equal and reuses the loads, which changes the first copy's schedule (see that function).
__declspec(section ".rodata") Vec vec3_rsh_00013cf8 = {10.0f, 215.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_rsh_00013d04 = {10.0f, 215.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_rsh_00013d10 = {10.0f, 215.0f, 0.0f};
const Vec vec3_rsh_00013d1c = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d28 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d34 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d40 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d4c = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d58 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d64 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d70 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d7c = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_rsh_00013d88 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013d94 = {0.0f, 0.0f, 0.0f};
const Vec vec3_rsh_00013da0 = {0.0f, 0.0f, 0.0f};

static MoamoaGroup group_tbl[11] = {
    {"c_n_moa_a", "S_1"}, {"c_n_moa_b", "S_1"}, {"c_kinodebu", "G_1"}, {"c_puku_c", "U_1"},
    {"c_powan", "O_1"},   {"c_smoko", "T_2"},   {"c_rose", "G_1"},     {"c_papab", "G_1"},
    {"c_mamab", "G_1"},   {"c_konari", "G_1"},  {"c_n_moa_a", "S_1"},
};

// a function-local static in the original (mt$806); file scope keeps it local and in this .data slot
static Mtx mt = {{8.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 8.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 8.0f, 0.0f}};

// TODO: type
static u32 sharinDt[116] = {
    (u32)"sha_1_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_3", 0x00000002, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_4", 0x00000001, 0x00000001, 0xF24A7E80,
    (u32)"sha_1_5", 0x00000001, 0x00000001, 0xF24A7E80,
    (u32)"sha_2_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_2_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_2_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_3_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_3_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_3_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_4_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_4_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_4_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_5_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_5_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_5_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_1", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_2", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_3", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_1_1_c", 0x00000000, 0x00000000, 0xF24A7680,
    (u32)"sha_1_2_c", 0x00000000, 0x00000000, 0xF24A7680,
    (u32)"sha_1_3_c", 0x00000002, 0x00000000, 0xF24A7680,
    (u32)"sha_1_4_c", 0x00000001, 0x00000001, 0xF24A7680,
    (u32)"sha_1_5_c", 0x00000001, 0x00000001, 0xF24A7680,
    (u32)"sha_6_1_c", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_2_c", 0x00000000, 0x00000000, 0xF24A7E80,
    (u32)"sha_6_3_c", 0x00000000, 0x00000000, 0xF24A7E80,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(evt_sharin_main)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    SWITCH(LW(1))
        CASE_EQUAL(0)
        SET_FLOAT_VALUE(LW(11), FLOAT(23.89f))
        CASE_EQUAL(1)
        SET_FLOAT_VALUE(LW(11), FLOAT(31.14f))
        CASE_EQUAL(2)
        SET_FLOAT_VALUE(LW(11), FLOAT(55.02f))
    END_SWITCH
    LOOP(0)
        SWITCH(LW(2))
            CASE_EQUAL(0)
            SET_FLOAT_VALUE(LW(9), GW(0))
            CASE_EQUAL(1)
            SET_FLOAT_VALUE(LW(9), GW(1))
        END_SWITCH
        DIVIDE_FLOAT(LW(9), LW(11))
        MULTIPLY_FLOAT(LW(9), FLOAT(57.295f))
        ADD_FLOAT(LW(10), LW(9))
        IF_INT_GREATER(LW(10), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(0.0f))
            ADD_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        SET_FLOAT_VALUE(LW(12), LW(10))
        MULTIPLY_FLOAT(LW(12), LW(3))
        CALL(evt_mapobj_rotate, 1, LW(0), 0, 0, LW(12))
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_sharin)
    SET_INT_READ_ADDR(PTR(sharinDt))
    LOOP(0)
        READ_INT4(LW(0), LW(1), LW(2), LW(3))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        SPAWN_EVENT(evt_sharin_main)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 clankDt[45] = {
    (u32)"bou_1", 0x00000000, 0x00000000, 0xF24ACA80,
    0xF24A7E80, (u32)"bou_2", 0x00000001, 0x00000001,
    0xF24ABC80, 0xF24A7E80, (u32)"bou_3", 0x00000000,
    0x00000000, 0xF24ACA80, 0xF24A7E80, (u32)"bou_4",
    0x00000001, 0x00000001, 0xF24ABC80, 0xF24A7E80,
    (u32)"bou_1_c", 0x00000000, 0x00000000, 0xF24ACA80,
    0xF24A7680, (u32)"bou_2_c", 0x00000001, 0x00000001,
    0xF24ABC80, 0xF24A7680, (u32)"bou_3_c", 0x00000000,
    0x00000000, 0xF24ACA80, 0xF24A7680, (u32)"bou_4_c",
    0x00000001, 0x00000001, 0xF24ABC80, 0xF24A7680,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(evt_clank_main)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    SWITCH(LW(1))
        CASE_EQUAL(0)
        SET_FLOAT_VALUE(LW(11), FLOAT(23.89f))
        CASE_EQUAL(1)
        SET_FLOAT_VALUE(LW(11), FLOAT(31.14f))
        CASE_EQUAL(2)
        SET_FLOAT_VALUE(LW(11), FLOAT(55.02f))
    END_SWITCH
    LOOP(0)
        SWITCH(LW(2))
            CASE_EQUAL(0)
            SET_FLOAT_VALUE(LW(9), GW(0))
            CASE_EQUAL(1)
            SET_FLOAT_VALUE(LW(9), GW(1))
        END_SWITCH
        DIVIDE_FLOAT(LW(9), LW(11))
        MULTIPLY_FLOAT(LW(9), FLOAT(57.295f))
        ADD_FLOAT(LW(10), LW(9))
        IF_INT_GREATER(LW(10), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(0.0f))
            ADD_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        SET_FLOAT_VALUE(LW(12), LW(10))
        MULTIPLY_FLOAT(LW(12), LW(4))
        CALL(evt_sub_get_sincos, LW(12), LW(13), LW(14))
        MULTIPLY_FLOAT(LW(13), LW(3))
        MULTIPLY_FLOAT(LW(14), LW(3))
        SUBTRACT_FLOAT(LW(14), LW(3))
        MULTIPLY_FLOAT(LW(14), FLOAT(-1.0f))
        CALL(evt_mapobj_trans, 1, LW(0), LW(14), LW(13), FLOAT(0.0f))
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_clank)
    SET_INT_READ_ADDR(PTR(clankDt))
    LOOP(0)
        READ_INT4(LW(0), LW(1), LW(2), LW(3))
        READ_INT(LW(4))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        SPAWN_EVENT(evt_clank_main)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_bg)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(11), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), GW(0))
        ADD_FLOAT(LW(10), LW(0))
        IF_INT_GREATER(LW(10), FLOAT(1200.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(2400.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(-1200.0f))
            ADD_FLOAT(LW(10), FLOAT(2400.0f))
        END_IF
        ADD_FLOAT(LW(11), LW(0))
        IF_INT_GREATER(LW(11), FLOAT(3600.0f))
            SUBTRACT_FLOAT(LW(11), FLOAT(7200.0f))
        END_IF
        IF_INT_LESS(LW(11), FLOAT(-3600.0f))
            ADD_FLOAT(LW(11), FLOAT(7200.0f))
        END_IF
        CALL(evt_mapobj_trans, 1, STRING("jimen"), LW(10), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("yama"), LW(10), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("iwayama"), LW(11), 0, 0)
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_hoshi)
    SET_FLOAT_VALUE(LW(10), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(9), GW(0))
        DIVIDE_FLOAT(LW(9), FLOAT(30.0f))
        MULTIPLY_FLOAT(LW(9), FLOAT(57.295f))
        ADD_FLOAT(LW(10), LW(9))
        IF_INT_GREATER(LW(10), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        IF_INT_LESS(LW(10), FLOAT(0.0f))
            ADD_FLOAT(LW(10), FLOAT(360.0f))
        END_IF
        CALL(evt_mapobj_rotate, 0, STRING("hosi_right"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_left"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_center"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_right_c"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_left_c"), LW(10), 0, 0)
        CALL(evt_mapobj_rotate, 0, STRING("hosi_center_c"), LW(10), 0, 0)
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start)
    SET_VALUE(GSWF(3583), 1)
    SET_VALUE(GSWF(3579), 0)
    SET_VALUE(GSWF(3580), 0)
    SET_VALUE(GSWF(3581), 0)
    CALL(evt_mapobj_get_position, STRING("ranpu"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        IF_INT_LESS(GSW(0), 339)
            CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_2"), 0)
        ELSE
            CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_2_2"), 0)
        END_IF
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_3"), 0)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
        CALL(evt_sub_intpl_msec_init, 11, 0, -1500, 20000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(0), FLOAT(-15.0f))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            CALL(evt_sub_get_stopwatch, LW(0), LW(1))
            IF_INT_GREATER(LW(1), 1500)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, 0, -1000, 1000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        CALL(evt_sub_intpl_msec_init, 11, -1000, 0, 1000)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start2)
    SET_VALUE(GSWF(3583), 1)
    SET_VALUE(GSWF(3579), 0)
    SET_VALUE(GSWF(3580), 0)
    SET_VALUE(GSWF(3581), 0)
    CALL(evt_mapobj_get_position, STRING("ranpu"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(200)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_TRAIN_BELL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_2_3"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE1_3"), 0)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        BEGIN_INLINE_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
            LOOP(0)
                ADD_FLOAT(LW(0), GW(0))
                CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
        CALL(evt_sub_intpl_msec_init, 11, 0, 1500, 20000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(0), FLOAT(15.0f))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            CALL(evt_sub_get_stopwatch, LW(0), LW(1))
            IF_INT_GREATER(LW(1), 1500)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, 0, 1000, 1000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
        CALL(evt_sub_intpl_msec_init, 11, 1000, 0, 1000)
        LABEL(2)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        SET_FLOAT_VALUE(GW(1), GW(0))
        ADD_FLOAT(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(2)
        END_IF
        LOOP(0)
            SET_FLOAT_VALUE(GW(1), GW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start3)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(-15.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(-15.0f))
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_start4)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(15.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(15.0f))
    RETURN
EVT_END()

static EVT_BEGIN(stop_1)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(3000.0f))
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_3"), 0)
    END_IF
    CALL(evt_sub_intpl_msec_init, 11, 3000, 0, 10000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    MULTIPLY_FLOAT(LW(0), FLOAT(1.0f))
    CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
    SUBTRACT_FLOAT(LW(2), LW(0))
    SET_FLOAT_VALUE(GW(0), LW(2))
    SET_FLOAT_VALUE(GW(1), LW(2))
    MULTIPLY_FLOAT(GW(0), FLOAT(-1.0f))
    MULTIPLY_FLOAT(GW(1), FLOAT(-1.0f))
    SET_FLOAT_VALUE(LW(2), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GSWF(3583), 0)
    RETURN
EVT_END()

static EVT_BEGIN(stop_2)
    SET_VALUE(GSWF(3583), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(-3000.0f))
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_2_2"), 0)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL(evt_snd_sfxon, STRING("SFX_STG6_TRAIN_MOVE2_3"), 0)
    END_IF
    CALL(evt_sub_intpl_msec_init, 11, -3000, 0, 10000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    MULTIPLY_FLOAT(LW(0), FLOAT(1.0f))
    CALL(evt_mapobj_trans, 1, STRING("train"), LW(0), 0, 0)
    SUBTRACT_FLOAT(LW(2), LW(0))
    SET_FLOAT_VALUE(GW(0), LW(2))
    SET_FLOAT_VALUE(GW(1), LW(2))
    MULTIPLY_FLOAT(GW(0), FLOAT(-1.0f))
    MULTIPLY_FLOAT(GW(1), FLOAT(-1.0f))
    SET_FLOAT_VALUE(LW(2), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GSWF(3583), 0)
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_stop)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        SPAWN_EVENT(stop_1)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        SPAWN_EVENT(stop_2)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        IF_INT_EQUAL(GSWF(3578), 0)
            SPAWN_EVENT(stop_1)
        ELSE
            SPAWN_EVENT(stop_2)
        END_IF
    END_IF
    RETURN
EVT_END()

// dont_inline keeps moamoa_pose_init a real `bl` from ext_moamoa_init as in retail; without it
// MWCC inlines it and ext_moamoa_init's stack frame no longer matches.
#pragma dont_inline on
static void moamoa_pose_init(void) {
    char name[0x20];
    char anim[0x20];
    MoamoaData* data = (MoamoaData*)ext_moamoa_data;
    s32 i = 0;
    s32 lastAnim;

    strcpy(name, "");
    strcpy(anim, "");
    wp->poseCount = -1;
    do {
        if (strcmp(data->name, name) != 0) {
            strcpy(name, data->name);
            strcpy(anim, "");
        }
        if (strcmp(data->anim, anim) != 0) {
            strcpy(anim, data->anim);
            lastAnim = i;
            wp->poseCount++;
        }
        strcpy(wp->poses[wp->poseCount].name, name);
        strcpy(wp->poses[wp->poseCount].anim, anim);
        wp->poses[wp->poseCount].firstPose = lastAnim;
        wp->poses[wp->poseCount].numPoses = i - lastAnim + 1;
        wp->poses[wp->poseCount].cycleFrames = data->value;
        wp->poses[wp->poseCount].enabled = 1;
#ifdef MUST_MATCH
        // the original's "J" string object is word padded; the extra NULs spell the pad out
        if (strncmp(anim, "J\0\0", 1) == 0) {
#else
        if (strncmp(anim, "J", 1) == 0) {
#endif
            wp->poses[wp->poseCount].enabled = 0;
        }
        i++;
        data++;
    } while (data->name != NULL);
    wp->poseCount++;
}
#pragma dont_inline reset

static EVT_BEGIN(evt_ramp)
    LOOP(0)
        CALL(ramp_r)
        CALL(evt_mapobj_rotate, 1, STRING("ranpu"), 0, 0, LW(10))
        CALL(evt_mapobj_rotate, 1, STRING("ranpu_c"), 0, 0, LW(10))
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GSWF(3583), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static char* pika_tbl[15] = {
    "pika_1_1", "pika_1_2", "pika_1_3", "pika_1_4", "pika_2", "pika_3", "pika_4", "pika_5",
    "pika_6", "pika_1_1_c", "pika_1_2_c", "pika_1_3_c", "pika_1_4_c", "pika_6_c", NULL,
};

static char* pika_tbl_rsh_06[10] = {
    "pika_1_1", "pika_1_2", "pika_1_3", "pika_1_4", "pika_2", "pika_3", "pika_4", "pika_5",
    "pika_6", NULL,
};
static void moamoa_pose_main(void) {
    s32 i;
    ExtPose* pose;
    MoamoaEntry* entry;

    pose = extGetPosePtr();
    entry = wp->entries;

    for (i = 0; i < wp->count; i++, entry++, pose++) {
        MoamoaPose* poseTbl = &wp->poses[entry->groupIndex];
        u64 diff = gp->renderTime - entry->poseTime;
        f32 f;

        f = 60.0f * (f32)OSTicksToMilliseconds(diff) / 1000.0f / poseTbl->cycleFrames;
        if (f >= 1.0f) {
            entry->poseTime = gp->renderTime;
            f = 0.0f;
        }
        pose->field_0x10 = poseTbl->firstPose + (s32)((f32)(poseTbl->numPoses - 1) * f);
    }
}

static EVT_BEGIN(hom_10_evt_resha_sub)
    SET_VALUE(GSWF(3583), 0)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SPAWN_EVENT(evt_sharin)
    SPAWN_EVENT(evt_clank)
    SET_VALUE(LF(0), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("hom_10"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_11"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_12"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("rsh_06_a"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        SPAWN_EVENT(evt_bg)
    END_IF
    SPAWN_EVENT(evt_hoshi)
    SPAWN_EVENT(evt_ramp)
    CALL(evt_map_set_tevcallback, 1, PTR(resha_tev))
    SET_INT_READ_ADDR(PTR(pika_tbl))
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("rsh_06_a"))
        SET_INT_READ_ADDR(PTR(pika_tbl_rsh_06))
    END_IF
    LOOP(0)
        READ_INT(LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 16777216)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 256)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 268435456)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_right)
    CALL_CHILD(hom_10_evt_resha_sub)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_NOT_EQUAL(LW(0), STRING("rsh_06_a"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_7"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_8"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_6"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_7"), 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_evt_resha_left)
    CALL_CHILD(hom_10_evt_resha_sub)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("train_6"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu_5"), 1)
    RETURN
EVT_END()

static EVT_BEGIN(hom_10_init_evt)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH2"))
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    IF_INT_EQUAL(GSWF(3578), 0)
        CALL_CHILD(hom_10_evt_resha_right)
        CALL_CHILD(hom_10_evt_resha_start3)
        SET_VALUE(LW(0), 500)
        SET_VALUE(LW(1), -500)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("renketu"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_3"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_4"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("renketu_5"), 1)
    ELSE
        CALL_CHILD(hom_10_evt_resha_left)
        CALL_CHILD(hom_10_evt_resha_start4)
        SET_VALUE(LW(0), 0)
        SET_VALUE(LW(1), 1000)
        CALL(evt_mapobj_trans, 1, STRING("train"), -1500, 0, 0)
    END_IF
    CALL(evt_cam_letter_box_onoff, 1, 1)
    CALL(evt_cam3d_evt_set, LW(0), 300, 1400, LW(0), 125, 0, 0, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_seq_wait, 2)
        CALL(evt_cam3d_evt_set, LW(1), 300, 1400, LW(1), 125, 0, 8000, 0)
        WAIT_MS(4000)
        CALL(evt_bero_mapchange, STRING("rsh_02_a"), STRING("nidome"))
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(evt_resha_init)
    CALL_CHILD(hom_10_evt_resha_right)
    SET_VALUE(GSWF(3583), 1)
    SET_VALUE(GW(0), FLOAT(-15.0f))
    SET_VALUE(GW(1), FLOAT(-15.0f))
    RETURN
EVT_END()

static BeroEntry bero_data_a[2] = {
    {"w_bero", 0, 8, 0x02, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_05_a", "w_bero", 1, 1},
    {NULL},
};
static void moamoa_pose_change(MoamoaEntry* entry, char* name, char* anim) {
    MoamoaPose* pose = wp->poses;
    s32 i;

    if (entry->flags & 0x800) {
        name = group_tbl[entry->kind].name;
        if (entry->kind >= 2) {
            anim = group_tbl[entry->kind].anim;
        }
        if (entry->kind == 10) {
            anim = "S_2";
        }
    }
    for (i = 0; i < wp->poseCount; i++, pose++) {
        if (strcmp(pose->name, name) == 0 && strcmp(pose->anim, anim) == 0) {
            break;
        }
    }
    if (i >= wp->poseCount) {
        return;
    }
    if (entry->groupIndex == i) {
        return;
    }
    if ((entry->flags & 4) && strncmp(anim, "D", 1) != 0) {
        return;
    }
    if ((entry->flags & 0x10) && strncmp(anim, "R", 1) != 0) {
        return;
    }
    entry->groupIndex = i;
    entry->poseTime = gp->renderTime;
}

static USER_FUNC(evt_moamoa_flag_onoff) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 flags = evtGetValue(event, args[1]);

    if (on != 0) {
        wp->flags |= flags;
    } else {
        wp->flags &= ~flags;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_exit) {
    extReset();
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_ry_reset) {
    ExtPose* pose = extGetPosePtr();
    s32 i;

    for (i = 0; i < wp->count; i++) {
        pose->field_0x14 = 0.0f;
        pose->field_0x1C = 0.0f;
        pose++;
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_moamoa_init)
    CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
    CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
    CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
    CALL(evt_npc_entry, STRING("kinopio"), STRING("c_kinodebu"))
    CALL(evt_npc_entry, STRING("cook"), STRING("c_puku_c"))
    CALL(evt_npc_entry, STRING("powan"), STRING("c_powan"))
    CALL(evt_npc_entry, STRING("sarary"), STRING("c_smoko"))
    CALL(evt_npc_entry, STRING("rose"), STRING("c_rose"))
    CALL(evt_npc_entry, STRING("papa"), STRING("c_papab"))
    CALL(evt_npc_entry, STRING("mama"), STRING("c_mamab"))
    CALL(evt_npc_entry, STRING("konari"), STRING("c_konari"))
    CALL(evt_npc_set_position, STRING("kinopio"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("cook"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("powan"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("sarary"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("rose"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("papa"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("mama"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("konari"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("kinopio"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("cook"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("powan"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("sarary"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("rose"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("papa"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("mama"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("konari"), 2048)
    CALL(evt_snd_sfxon, STRING("SFX_STG6_MOAMOA_VOICE2"), GW(2))
    CALL(evt_npc_entry, STRING("base"), STRING("c_g_moamoa"))
    CALL(evt_npc_set_position, STRING("base"), 0, -1000, 0)
    CALL(evt_ext_entry, 700, PTR(ext_moamoa_data), PTR(ext_moamoa_init), PTR(ext_moamoa_main), PTR(ext_moamoa_dispent))
    CALL(evt_moamoa_flag_onoff, 1, 1)
    BEGIN_INLINE_EVENT
        LABEL(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(0), -300)
            WAIT_FRAMES(1)
            GOTO(0)
        END_IF
        CALL_CHILD(evt_great_moamoa)
    END_INLINE_EVENT
    RETURN
EVT_END()
static USER_FUNC(evt_moamoa_end_wait) {
    MoamoaEntry* entry;
    s32 i;

    extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++, entry++) {
        if (!(entry->flags & 0x20)) {
            return EVT_RETURN_BLOCK;
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_fusion) {
    if (isFirstCall) {
        s32 i;
        ExtPose* pose;
        MoamoaEntry* entry;

        pose = extGetPosePtr();
        entry = wp->entries;
        for (i = 0; i < wp->count; i++, pose++, entry++) {
            (*pose->matrix)[2][2] = 1.0f;
            (*pose->matrix)[1][1] = 1.0f;
            (*pose->matrix)[0][0] = 1.0f;
            entry->queuedState = 14;
            entry->state = 14;
            entry->stateTimer = (i < 100) ? i % 100 : i % 60 + 100;
            entry->flags |= 0x100;
        }
    }
    {
        MoamoaEntry* entry;
        s32 i;

        extGetPosePtr();
        entry = wp->entries;
        for (i = 0; i < wp->count; i++, entry++) {
            if (!(entry->flags & 0x200)) {
                return EVT_RETURN_BLOCK;
            }
        }
    }
    {
        ExtPose* pose = extGetPosePtr();
        MoamoaEntry* entry = wp->entries;
        s32 i;

        for (i = 0; i < wp->count; i++, pose++, entry++) {
            pose->flags |= 4;
            entry->flags |= 0x400;
            entry->state = 0x3D;
            moamoa_pose_change(entry, "c_n_moa_a", "S_1");
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_kind_onoff) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 b = evtGetValue(event, args[1]);
    s32 i;
    MoamoaEntry* entry;

    extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++, entry++) {
        if (a != 0) {
            if (b == 0) {
                if (entry->kind == 1) {
                    entry->flags |= 0x800;
                }
            } else {
                entry->flags |= 0x800;
            }
        } else if (b == 0) {
            if (entry->kind != 1) {
                entry->flags &= ~0x800;
            }
        } else {
            entry->flags &= ~0x800;
        }
        moamoa_pose_change(entry, "c_n_moa_a", "S_1");
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_blow) {
    s32 i;
    MoamoaEntry* entry;

    evtGetValue(event, event->args[0]);
    extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++, entry++) {
        entry->flags &= ~0x400;
        entry->state = 0x3f;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_great_moamoa)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_moamoa_flag_onoff, 1, 8)
    CALL(evt_moamoa_flag_onoff, 1, 4)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_244"), 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("group339"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("group340"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("group341"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("senro"), 1)
    CALL(evt_npc_set_position, STRING("base"), 10, 190, 0)
    CALL(evt_npc_pera_onoff, STRING("base"), 0)
    CALL(evt_npc_set_anim, STRING("base"), STRING("Z_1"))
    CALL(evt_snd_sfxoff, GW(2))
    CALL(evt_snd_sfxon, STRING("SFX_STG6_MOA_TOGETHER_MOVE1"), GW(2))
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, STRING("eff0"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff_fukidashi, 3, STRING("eff1"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1000)
        CALL(evt_mario_set_dir, 90, 200, 0)
        CALL(evt_party_set_dir, 0, 90, 200)
        WAIT_MS(300)
        CALL(evt_cam3d_evt_set, 10, 349, 516, 10, 253, -25, 3000, 11)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(15)
            CALL(evt_snd_sfxon, STRING("SFX_STG6_MOA_TOGETHER_MOVE2"), 0)
            WAIT_MS(500)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_moamoa_fusion)
    CALL(evt_snd_sfxoff, GW(2))
    CALL(evt_snd_sfxon, STRING("SFX_STG6_MOAMOA_VOICE2"), GW(2))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group166"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group339"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group340"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group341"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("senro"), 1)
    WAIT_MS(2000)
    CALL(evt_moamoa_kind_onoff, 1, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("base"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_LEG1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(800)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_LEG1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(800)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_LEG1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(800)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_LEG1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("base"), STRING("H_1"))
    CALL(npc_anim_wait)
    CALL(evt_cam3d_evt_set, 10, 335, 369, 10, 265, -25, 1000, 11)
    CALL(evt_moamoa_kind_onoff, 1, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("base"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_LEG2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("base"), STRING("U_1"))
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        WAIT_MS(1700)
        CALL(evt_npc_get_position, STRING("base"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_LEG3"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_cam3d_evt_set, 10, 300, 450, 10, 300, -25, 1000, 11)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("base"), STRING("H_2"))
    CALL(npc_anim_wait)
    CALL(evt_npc_set_anim, STRING("base"), STRING("S_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pos, -160, 215, 0)
    CALL(evt_party_set_pos, 0, -200, 215, -10)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    CALL(evt_cam3d_evt_set, -180, 308, 270, -180, 255, -25, 0, 11)
    WAIT_MS(500)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg6_rsh_245"))
    WAIT_MS(300)
    CALL(evt_party_run, 0)
    CALL(evt_cam3d_evt_set, 10, 498, 467, 10, 410, -25, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_246_02"), 0, 0)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_246_03"), 0, 0)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 1000, 20, 841, -50, 200, -66, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, -440, 230, 180, 5, 300, -36, 4000, 11)
    WAIT_MS(4000)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("base"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_LEG4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("base"), STRING("P_1"))
    CALL(npc_anim_wait)
    CALL(evt_npc_set_anim, STRING("base"), STRING("S_3"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_246_01"), 0, 0)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -100, 488, 1370, -100, 245, -8, 4000, 11)
    WAIT_MS(2000)
    CALL(evt_snd_sfxoff, GW(2))
    CALL(evt_npc_set_battle_info, STRING("base"), 1)
    CALL(evt_npc_battle_start, STRING("base"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_cam3d_evt_set, -50, 355, 534, -50, 260, -8, 0, 11)
    CALL(evt_npc_set_anim, STRING("base"), STRING("Z_1"))
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -25)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 0, STRING("kinopio"), 2048)
    CALL(evt_npc_flag_onoff, 0, STRING("cook"), 2048)
    CALL(evt_npc_flag_onoff, 0, STRING("powan"), 2048)
    CALL(evt_npc_flag_onoff, 0, STRING("sarary"), 2048)
    CALL(evt_npc_flag_onoff, 0, STRING("rose"), 2048)
    CALL(evt_npc_flag_onoff, 0, STRING("papa"), 2048)
    CALL(evt_npc_flag_onoff, 0, STRING("mama"), 2048)
    CALL(evt_npc_flag_onoff, 0, STRING("konari"), 2048)
    CALL(evt_npc_set_position, STRING("kinopio"), 0, 215, 0)
    CALL(evt_npc_set_position, STRING("cook"), 0, 215, 1)
    CALL(evt_npc_set_position, STRING("sarary"), 0, 215, 2)
    CALL(evt_npc_set_position, STRING("rose"), 0, 215, 3)
    CALL(evt_npc_set_position, STRING("papa"), 0, 215, 4)
    CALL(evt_npc_set_position, STRING("mama"), 0, 215, 5)
    CALL(evt_npc_set_position, STRING("konari"), 0, 215, 6)
    CALL(evt_npc_set_position, STRING("powan"), 20, 215, 7)
    CALL(evt_npc_set_ry, STRING("kinopio"), 270)
    CALL(evt_npc_set_ry, STRING("cook"), 270)
    CALL(evt_npc_set_ry, STRING("sarary"), 270)
    CALL(evt_npc_set_ry, STRING("rose"), 270)
    CALL(evt_npc_set_ry, STRING("papa"), 270)
    CALL(evt_npc_set_ry, STRING("mama"), 270)
    CALL(evt_npc_set_ry, STRING("konari"), 270)
    CALL(evt_npc_set_ry, STRING("powan"), 270)
    CALL(evt_npc_set_anim, STRING("kinopio"), STRING("Z_1"))
    CALL(evt_npc_set_anim, STRING("cook"), STRING("Z_1"))
    CALL(evt_npc_set_anim, STRING("sarary"), STRING("Z_1"))
    CALL(evt_npc_set_anim, STRING("rose"), STRING("Z_1"))
    CALL(evt_npc_set_anim, STRING("papa"), STRING("Z_1"))
    CALL(evt_npc_set_anim, STRING("mama"), STRING("Z_1"))
    CALL(evt_npc_set_anim, STRING("konari"), STRING("Z_1"))
    CALL(evt_npc_set_anim, STRING("powan"), STRING("Z_1"))
    CALL(evt_moamoa_kind_onoff, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_247"), 0, STRING("base"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("base"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_MOAMOA_G_ESCAPE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_moamoa_blow)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, -150, 485, 1469, -150, 225, -8, 4000, 11)
    WAIT_MS(4000)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_reverse_ry, STRING("powan"))
            WAIT_MS(1500)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_reverse_ry, STRING("konari"))
            WAIT_MS(1000)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(2000)
    SET_VALUE(GSW(0), 332)
    CALL(evt_bero_mapchange, STRING("rsh_01_a"), STRING("w_bero"))
    RETURN
EVT_END()

char* pose_name[8] = {
    "Z_1", "H_1", "S_1", "U_1", "H_2", "S_2", "P_1", "S_3",
};
static USER_FUNC(evt_moamoa_fusion_debug) {
    s32 i;
    ExtPose* pose;
    MoamoaEntry* entry;

    pose = extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++, pose++, entry++) {
        pose->flags |= 4;
        entry->flags |= 0x400;
        entry->flags |= 0x100;
        entry->flags |= 0x200;
        entry->state = 0x3D;
        entry->flags |= 0x800;
        moamoa_pose_change(entry, "c_n_moa_a", "S_1");
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(moamoa_debug)
    CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
    CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
    CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
    CALL(evt_npc_entry, STRING("kinopio"), STRING("c_kinodebu"))
    CALL(evt_npc_entry, STRING("cook"), STRING("c_puku_c"))
    CALL(evt_npc_entry, STRING("powan"), STRING("c_powan"))
    CALL(evt_npc_entry, STRING("sarary"), STRING("c_smoko"))
    CALL(evt_npc_entry, STRING("rose"), STRING("c_rose"))
    CALL(evt_npc_entry, STRING("papa"), STRING("c_papab"))
    CALL(evt_npc_entry, STRING("mama"), STRING("c_mamab"))
    CALL(evt_npc_entry, STRING("konari"), STRING("c_konari"))
    CALL(evt_npc_set_position, STRING("kinopio"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("cook"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("powan"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("sarary"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("rose"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("papa"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("mama"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("konari"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("kinopio"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("cook"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("powan"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("sarary"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("rose"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("papa"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("mama"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("konari"), 2048)
    CALL(evt_npc_entry, STRING("base"), STRING("c_g_moamoa"))
    CALL(evt_npc_set_position, STRING("base"), 0, -1000, 0)
    CALL(evt_ext_entry, 700, PTR(ext_moamoa_data), PTR(ext_moamoa_init), PTR(ext_moamoa_main), PTR(ext_moamoa_dispent))
    CALL(evt_moamoa_flag_onoff, 1, 1)
    CALL(evt_moamoa_flag_onoff, 1, 4)
    CALL(evt_moamoa_fusion_debug)
    CALL(evt_npc_set_position, STRING("base"), 10, 190, 0)
    CALL(evt_npc_pera_onoff, STRING("base"), 0)
    CALL(evt_npc_set_anim, STRING("base"), STRING("S_2"))
    CALL(evt_npc_set_scale, STRING("base"), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(0)
            CALL(evt_key_get_buttontrg, 0, LW(10))
            IF_BITS_SET(LW(10), 256)
                ADD(LW(0), 1)
                MODULO(LW(0), 8)
                SET_VALUE(LW(1), LW(0))
                ADD(LW(1), 1)
                SET_INT_READ_ADDR(PTR(pose_name))
                LOOP(LW(1))
                    READ_INT(LW(2))
                END_LOOP
                CALL(evt_npc_set_anim, STRING("base"), LW(2))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(rsh_06_init_evt)
    CALL_CHILD(evt_resha_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(284, 286)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(304, 309)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(310, 325)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_HOM1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_BETWEEN(327, 328)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(329, 331)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(334, 344)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_DEFAULT
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH2"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(rsh_06_a_init_evt)
    CALL(evt_mario_set_pos, -500, 250, 0)
    SET_VALUE(LW(0), PTR(bero_data_a))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_06_init_evt)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        SPAWN_EVENT(moamoa_debug)
        RETURN
    END_IF
    IF_INT_LESS(GSW(0), 332)
        CALL_CHILD(evt_moamoa_init)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_06_init_evt_nidome)
    CALL_CHILD(evt_resha_init)
    IF_INT_EQUAL(GSWF(3460), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
    ELSE
        IF_INT_EQUAL(GSWF(3461), 1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
            CALL(evt_snd_envoff, 512)
            CALL(evt_snd_bgmoff, 26624)
        ELSE
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH2"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_06_a_init_evt_nidome)
    SET_VALUE(LW(0), PTR(bero_data_a))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_06_init_evt_nidome)
    RETURN
EVT_END()

// TODO: type
// static MoamoaWork* wp;  // moved to top (needed before the functions that use it)
void evalOn(void) {
    NpcEntry* npc = npcNameToPtr("base");
    AnimationPose* pose;
    Vec* positions;
    MoamoaEntry* entry;
    s32 i;
    s32 n;

    animPoseWorldPositionEvalOn(npc->poseId);
    wp->poseId = npc->poseId;
    pose = animPoseGetAnimPosePtr(npc->poseId);
    positions = (Vec*)((smartEntry*)*(void**)((u8*)pose + 0x15C))->address;
    n = *(s32*)((u8*)pose + 0x160);
    for (i = 0; i < n; i++) {
        positions[i] = vec3_rsh_00013cec;
    }

    for (i = 0; i < dat_80419a28; i++) {
        wp->chainGroup[i].x = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].name);
        wp->chainGroup[i].y = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].anim);
    }

    {
        MoamoaEntry* e;
        s32 k;
        e = wp->entries;
        for (k = 0; k < wp->count; k++, e++) {
            e->chain = k % dat_80419a28;
            e->chainSlot = wp->chainSlots[e->chain]++;
        }
    }

    {
        MoamoaEntry* e;
        s32 k;
        e = wp->entries;
        for (k = 0; k < wp->count; k++, e++) {
            s16 v = e->chain;

            if (v == 0 || v == 5 || v == 0xA || v == 0xF || v == 0x1E || v == 0x22 || v == 0x26 || v == 0x2A || v == 0x2E) {
                e->kind = 1;
            }
            if (e->chainSlot == 0) {
                if (e->chain == 3) { e->kind = 2; }
                if (e->chain == 4) { e->kind = 3; }
                if (e->chain == 7) { e->kind = 4; }
                if (e->chain == 8) { e->kind = 5; }
                if (e->chain == 9) { e->kind = 6; }
                if (e->chain == 0xC) { e->kind = 7; }
                if (e->chain == 0xD) { e->kind = 8; }
                if (e->chain == 0xE) { e->kind = 9; }
            }
        }
    }}
static USER_FUNC(evt_moamoa_target_off) {
    s32 index = evtGetValue(event, event->args[0]);

    wp->vec[index] = wp->vec[1 - index];
    wp->nearHits[index] = 100;
    if (index == 0) {
        wp->flags |= 0x1000;
    } else {
        wp->flags |= 0x2000;
    }
    return EVT_RETURN_DONE;
}

// randf's scaling step, for the sites in ext_moamoa_main that draw the rand() fraction ahead of its use.
static inline f32 randScale(f32 t, f32 min, f32 max) {
    return t * (max - min) + min;
}

// randf's first step, the rand() fraction in [0, 1].
static inline f32 frand(void) {
    f32 r = (f32)rand() / 32767.0f;

    return r;
}

// base + randf(min, max), with randf's body spelled out.
static inline f32 randOffset(f32 base, f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return base + (r * (max - min) + min);
}

// A random angle in [min, max) degrees, converted to radians.
static inline f32 randRad(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return 6.2831855f * (r * (max - min) + min) / 360.0f;
}

static inline f32 randRange(f32 min, f32 max) {
    return max - min;
}

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return r * (max - min) + min;
}


// The local-declaration order is load-bearing: `s32 i; ExtPose* pose; MoamoaEntry* entry;` gives the
// target's register allocation (every other permutation scores lower). The three source vectors must
// stay non-const: as `const` MWCC knows their equal values, CSEs the later loads and schedules the
// first `wp->vec[0]` copy differently (`wp` loaded before lane 0).
static void ext_moamoa_init(void) {
    s32 i;
    ExtPose* pose;
    MoamoaEntry* entry;
    f32 hx, hy, hz, dist, nx, ny, nz;

    wp = _mapAlloc(0x870);
    memset(wp, 0, 0x870);

    wp->vec[0] = vec3_rsh_00013cf8;
    wp->vec[1] = vec3_rsh_00013d04;
    wp->vec[2] = vec3_rsh_00013d10;

    wp->count = extGetPoseNum();
    wp->entries = _mapAlloc(wp->count * 0x50);
    memset(wp->entries, 0, wp->count * 0x50);
    moamoa_pose_init();

    pose = extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++, pose++, entry++) {
        entry->flags = 0;
        entry->state = 0;
        entry->stateTimer = 0;
        moamoa_pose_change(entry, "c_n_moa_a", "S_1");
        pose->flags |= 7;
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = randf(-500.0f, 500.0f);
        (*pose->matrix)[1][3] = 215.0f;
        (*pose->matrix)[2][3] = randf(-45.0f, 45.0f);
        hz = 0.0f;
        hy = 0.0f;
        hx = 0.0f;
        dist = 1000000.0f;
        if (hitCheckFilter(NULL, (*pose->matrix)[0][3], 100.0f + (*pose->matrix)[1][3], (*pose->matrix)[2][3], 0.0f, -1.0f, 0.0f, &hx, &hy,
                           &hz, &dist, &nx, &ny, &nz)) {
            (*pose->matrix)[1][3] = hy;
        } else {
            pose->flags |= 4;
            (*pose->matrix)[0][3] = randf(-500.0f, 500.0f);
            (*pose->matrix)[1][3] = randf(100.0f, 215.0f);
            (*pose->matrix)[2][3] = randf(70.0f, 75.0f);
            entry->flags |= 0x40;
            moamoa_pose_change(entry, "c_n_moa_a", "S_1");
        }
        {
#ifdef MUST_MATCH
            // The original's zero vector is read-only but MWCC must reload it every iteration, so the
            // volatile read stands in for the missing non-const qualifier.
            Vec offset = *(volatile Vec*)&vec3_rsh_00013d1c;
#else
            Vec offset = vec3_rsh_00013d1c;
#endif

            offset.x = randf(-10.0f, 10.0f);
            offset.y = randf(-10.0f, 10.0f);
            offset.z = randf(-10.0f, 10.0f);
            entry->field_0x30 = offset;
        }
    }
    evalOn();
    for (i = 0; i < 600; i++) {
        ext_moamoa_main();
    }
}

static void ext_disp(CameraId cameraId, void* param) {
    u16 left, top, right, bottom;

    if (cameraId == CAMERA_OFFSCREEN2) {
        GXClearBoundingBox();
    }
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
    GXSetNumIndStages(0);
    GXSetTevDirect(GX_TEVSTAGE0);
    if (cameraId == CAMERA_OFFSCREEN2) {
        sysWaitDrawSync();
        GXReadBoundingBox(&left, &top, &right, &bottom);
        offscreenAddBoundingBox(offscreenNameToId("offscreen"), left, top, right, bottom);
    }
}

static void ext_shadow_disp(CameraId cameraId, void* param) {
    u16 left, top, right, bottom;

    if (cameraId == CAMERA_OFFSCREEN2) {
        GXClearBoundingBox();
    }
    extLoadShadowMtx(mt);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
    if (cameraId == CAMERA_OFFSCREEN2) {
        sysWaitDrawSync();
        GXReadBoundingBox(&left, &top, &right, &bottom);
        offscreenAddBoundingBox(offscreenNameToId("offscreen"), left, top, right, bottom);
    }
}
// The original calls moamoa_jump (a real bl) from ext_moamoa_main; the raised inline limits would inline it.
#pragma dont_inline on
static void moamoa_jump(ExtPose* pose, MoamoaEntry* entry, f32 x, f32 y, f32 z, f32 p4, f32 p5, s32 field0x2, s32 flag) {
    if (flag) {
        f32 hx, hy, hz, dist, nx, ny, nz;

        hz = 0.0f;
        hy = 0.0f;
        hx = 0.0f;
        dist = 1000000.0f;
        if (hitCheckFilter(NULL, x, 100.0f + y, z, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &dist, &nx, &ny, &nz)) {
            if (__fabsf(y - hy) > 30.0) {
                p4 -= 0.1f;
                p5 += 0.1f;
            }
            y = hy;
        }
    }
    {
        Vec pos = vec3_rsh_00013d28;

        pos.x = x;
        pos.y = y;
        pos.z = z;
        entry->field_0x8 = pos;
    }
    entry->field_0x1C = p4;
    entry->field_0x20 = p5;
    {
        f32 dy = entry->field_0x8.y - (*pose->matrix)[1][3];
        f32 d = distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->field_0x8.x, entry->field_0x8.z);
        f32 ang = angleABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->field_0x8.x, entry->field_0x8.z);
        s32 frames;

        frames = (s32)(d / entry->field_0x1C);
        entry->frames = frames;
        entry->field_0x14 = entry->field_0x20 * entry->frames / 2.0f + dy / entry->frames;
        entry->state = 3;
        entry->queuedState = field0x2;
        pose->field_0x4 = 180.0f + ang;
    }
    entry->flags |= 2;
    pose->flags |= 0x10;
    pose->flags &= ~4;
}
#pragma dont_inline reset


// Inline sine/cosine approximations (the polynomials shared by the NPC blur path).
static inline f32 moamoa_sin(f32 angle) {
    f32 v = 0.00761f;
    f32 b = 0.16605f;
    f32 one = 1.0f;
    f32 u;
    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) { u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; }
        else { angle = 1.5707964f - (angle - 1.5707964f); u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f; u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f); u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; v = -v;
    }
    return v;
}

static inline f32 moamoa_cos(f32 angle) {
    f32 v = 0.03705f;
    f32 b = 0.4967f;
    f32 one = 1.0f;
    f32 u;
    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) { u = angle * angle; v = v * u - b; v = v * u + one; }
        else { angle = 1.5707964f - (angle - 1.5707964f); u = angle * angle; v = v * u - b; v = v * u + one; v = -v; }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f; u = angle * angle; v = v * u - b; v = v * u + one; v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f); u = angle * angle; v = v * u - b; v = v * u + one;
    }
    return v;
}

// The original builds each temporary Vec from a named zero vector and passes it by
// value through an inline; MWCC keeps the inline's return temp alongside the local.
static inline Vec moamoa_vec(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

#ifdef MUST_MATCH
// Match-only: the volatile copy stops MWCC hoisting the base vector's lane loads above the two
// `pose->matrix` stores in the pvec rebuild (retail keeps each 12-byte copy serialized); the plain
// copy is `moamoa_vec`.
static inline Vec moamoa_vec_v(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *(volatile Vec*)base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
#endif

// Matches retail. The sine/cosine coefficients hoist and colour like retail (the pik/evt_sub helper
// shape and the two pragmas). Declaring the six Vec temporaries at function scope instead of
// per-block is load-bearing: with them block-local the pragma pushes the frame to 0x130; at function
// scope it is the target's 0xf0 and every stack slot lines up.
// Reusing the parameter as the sine/cosine half-angle (`angle = halfpi - (angle - halfpi)`) colours
// the reduced angle into the target's register. In the block whose cosine recomputes its argument,
// the sine's argument must NOT be named (the `rad` local is inlined into the call): keeping a `rad`
// local there leaves f5 live and pushes the reduced angle to f1, whereas retail coalesces it into f5.
// Naming the box-delta `d = f31 - f29` once (assigned after the sine, used by both riderDelta lanes)
// fixes the operand order of the two `d * s`/`d * c` fmuls. Giving the pvec's x lane a local `px`
// pins the mpos/pvec lane registers (retail pose x -> f3, z -> f1; without it they swap).
// The first ang/rad block matches by splitting `ang = reviseAngle(...)` from `rad = TWO_PI * ang / 360`
// and inlining `camGetPtr(4)` (check_hammer's shape), and the two `(20+box.x)*0.5` sites match by
// writing `(box.x + 20) / 2` (folded to the same fmuls, under MUST_MATCH).
#pragma opt_lifetimes off // keeps the sin/cos coefficients in the target's FPRs
#pragma opt_propagation off // hoist the sin/cos coefficient loads out of the inline branches
static s32 moamoa_hit_check(ExtPose* pose, MoamoaEntry* entry) {
    MarioWork* mario = marioGetPtr();
    Vec v65, v55c, vnoko, vparty, mpos, pvec;
    PartyEntry* party;
    f32 f29, f30, f31;

    if (mario->position.y > 20.0f + (*pose->matrix)[1][3])
        return 0;
    if ((*pose->matrix)[1][3] > mario->position.y + mario->wPlayerCollisionBox.y)
        return 0;

    {
        f32 d = distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], mario->position.x, mario->position.z);

#ifdef MUST_MATCH
        // (x + 20) / 2 folds to the same * 0.5 but colours the operands like retail.
        if (d < (mario->wPlayerCollisionBox.x + 20.0f) / 2.0f) {
#else
        if (d < (20.0f + mario->wPlayerCollisionBox.x) * 0.5f) {
#endif
            if (mario->position.y > 10.0f + (*pose->matrix)[1][3] && (mario->flags & 0x20000) &&
                !(mario->flags & 0x10000) && wp->field_0x558 < 2) {
                if (mario->currMotionId != 0x16 && mario->currMotionId != 0x1A)
                    marioChgMot(5);
                wp->field_0x558++;
                return 1;
            }
        }
    }

    if ((mario->flags & 0x30000) == 0)
        wp->field_0x558 = 0;

    if (mario->currMotionId == 0x12 && wp->field_0x40 > 10) {
        f32 ang;
        f32 rad;
        f32 s;
        f32 c;
        f32 x;
        f32 z;

        ang = reviseAngle(mario->unk1A8 - 90.0f + camGetPtr(4)->viewYaw);
        rad = 6.2831855f * ang / 360.0f;
        s = moamoa_sin(rad);
        x = 35.0f * s + mario->position.x;
        c = moamoa_cos(rad);
        z = mario->position.z - 35.0f * c;
        if (distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], x, z) < 35.0f)
            return 2;
    }

    if (mario->currMotionId == 0x13 && (s32)mario->currSubMotionId == 0x14) {
        v65 = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                             &vec3_rsh_00013d34);

        if (PSVECDistance(&mario->position, &v65) < 65.0f)
            return 2;
    }

    if (wp->breathDir >= 0.0f) {
        v55c = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                              &vec3_rsh_00013d40);

        if (PSVECDistance(&wp->breathPos, &v55c) < 30.0f)
            return 3;
    }

    party = partyGetPtr(0);
    if (nokonokoGetStatus(party) == 2) {
        vnoko = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                               &vec3_rsh_00013d4c);

        if (PSVECDistance(&partyGetPtr(0)->position, &vnoko) < 30.0f)
            return 2;
    }

    party = partyGetPtr(0);
    if (party->currentMemberId == 3 && partyGetPtr(0)->useMotionId == 0x46) {
        vparty = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                                &vec3_rsh_00013d58);

        if (PSVECDistance(&partyGetPtr(0)->position, &vparty) < 50.0f)
            return 4;
    }

    {
        mpos = moamoa_vec(mario->position.x, (*pose->matrix)[1][3], mario->position.z,
                              &vec3_rsh_00013d64);
#ifdef MUST_MATCH
        {
            // Matching temp: giving the pvec's x lane a name pins the mpos/pvec lane registers
            // (retail pose.x -> f3, pose.z -> f1; without it MWCC loads them the other way round).
            f32 px = (*pose->matrix)[0][3];
            pvec = moamoa_vec(px, (*pose->matrix)[1][3], (*pose->matrix)[2][3], &vec3_rsh_00013d70);
        }
#else
        pvec = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                              &vec3_rsh_00013d70);
#endif

#ifdef MUST_MATCH
        f31 = (mario->wPlayerCollisionBox.x + 20.0f) / 2.0f;
#else
        f31 = (20.0f + mario->wPlayerCollisionBox.x) * 0.5f;
#endif
        if (PSVECDistance(&mpos, &pvec) < f31) {
#ifdef MUST_MATCH
            // Match-only volatile accesses: they keep the pvec base copy below the two matrix
            // stores, the order the target has.
            *(volatile f32*)&(*pose->matrix)[0][3] = entry->field_0x28;
            *(volatile f32*)&(*pose->matrix)[2][3] = entry->field_0x2C;
            {
                Mtx* pm = *(Mtx* volatile*)&pose->matrix;

                pvec = moamoa_vec_v((*pm)[0][3], (*pm)[1][3], (*pm)[2][3], &vec3_rsh_00013d7c);

                f29 = PSVECDistance(&mpos, &pvec);
            }
#else
            (*pose->matrix)[0][3] = entry->field_0x28;
            (*pose->matrix)[2][3] = entry->field_0x2C;
            {
                pvec = moamoa_vec((*pose->matrix)[0][3], (*pose->matrix)[1][3], (*pose->matrix)[2][3],
                                  &vec3_rsh_00013d7c);

                f29 = PSVECDistance(&mpos, &pvec);
            }
#endif
            if (f29 < f31) {
                f30 = angleABf(mario->position.x, mario->position.z, (*pose->matrix)[0][3],
                               (*pose->matrix)[2][3]);
                if (__fabsf(compAngle(f30, mario->directionView)) < 40.0f) {
#ifdef MUST_MATCH
                    // Matching shapes: inlining the sine argument (no `rad` local) lets the reduced
                    // angle coalesce into the argument FPR, and naming the box delta once fixes the
                    // operand order of the two `d * s` / `d * c` fmuls.
                    f32 s, c, d;

                    s = moamoa_sin(6.2831855f * mario->directionView / 360.0f);
                    d = f31 - f29;
                    mario->riderDelta.x = -(d * s);
                    c = moamoa_cos(6.2831855f * mario->directionView / 360.0f);
                    mario->riderDelta.z = d * c;
#else
                    f32 rad, s, c;

                    rad = 6.2831855f * mario->directionView / 360.0f;
                    s = moamoa_sin(rad);
                    mario->riderDelta.x = -((f31 - f29) * s);
                    c = moamoa_cos(6.2831855f * mario->directionView / 360.0f);
                    mario->riderDelta.z = (f31 - f29) * c;
#endif
                } else {
                    f32 rad, s, c;

                    rad = 6.2831855f * f30 / 360.0f;
                    s = moamoa_sin(rad);
                    mario->riderDelta.x = ((*pose->matrix)[0][3] - f31 * s) - mario->position.x;
                    c = moamoa_cos(rad);
                    mario->riderDelta.z = ((*pose->matrix)[2][3] + f31 * c) - mario->position.z;
                }
            }
        }
    }

    return 0;
}

#pragma opt_propagation reset
#pragma opt_lifetimes reset

// The original inlines every sin/cos/randf/pose-change here: raise the inliner's per-caller limits.
#pragma inline_max_total_size(60000)
#pragma inline_max_size(1000)
// Keep the sin/cos coefficient loads hoisted instead of rematerialised per branch.
#pragma opt_propagation off
// Keeps the inlined sin/cos argument in a callee-saved FPR instead of rematerialising it per branch.
#pragma opt_lifetimes off // needed: without it ext_moamoa_main 97.80%
#define MOA_TARGET(i) (wp->vec[(i) % 3])
// Per-frame update of every moamoa (the Great Moamoa variant: a follower pulls the swarm to a joint of "base").
// The float temporaries follow eki_00/rsh_05 (function-scope tx/ty/tz/rad/dist/r1/f reused across the cases);
// their declaration order is what rasim.py's allocator replay needs for retail's f26..f31.
static void ext_moamoa_main(void) {
    MarioWork* mario;
    PartyEntry* party;
    CameraEntry* cam;
    ExtPose* ep;
    MoamoaEntry* entry;
    s32 handled;
    s32 voicePlayed;
    s32 i;
    f32 push;
    f32 len;
    f32 rad;
    f32 tx;
    f32 ty;
    f32 tz;
    f32 dist;
    f32 r1;
    f32 f;
    s32 res;
    Vec v3, v, v2;
#ifdef MUST_MATCH
    union { const char* p; u32 u; } sv;
#else
    const char* sv;
#endif

    mario = marioGetPtr();
    party = partyGetPtr(0);
    cam = camGetPtr(CAMERA_3D);
    push = 0.0f;
    handled = 0;
    if (mario->currMotionId == MARIO_MOTION_HAMMER) {
        wp->field_0x40++;
    } else {
        wp->field_0x40 = 0;
    }
    if (mario->currMotionId == MARIO_MOTION_STAY) {
        wp->idleFrames++;
        if (wp->idleFrames > 1000) {
            wp->idleFrames = 1000;
        }
    } else {
        wp->idleFrames = 0;
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        wp->flags |= 0x10;
    } else {
        wp->flags &= ~0x10;
    }
    wp->breathDir = -1.0f;
    wp->breathDist = -1.0f;
    if (party->currentMemberId == 5 && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        wp->breathDir = cloudGetBreathDir();
        wp->breathDist = cloudGetBreathDist();
        wp->breathPos.x =
            wp->breathDist * moamoa_sin(6.2831855f * wp->breathDir / 360.0f) + party->position.x;
        wp->breathPos.y = party->position.y;
        wp->breathPos.z = -(wp->breathDist * moamoa_cos(6.2831855f * wp->breathDir / 360.0f) - party->position.z);
    }
    ep = extGetPosePtr();
    voicePlayed = 0;
    entry = wp->entries;
    for (i = 0; i < wp->count; i++, ep++, entry++) {
#ifdef MUST_MATCH
        sv.p = "SFX_STG6_MOAMOA_VOICE1";
#else
        sv = "SFX_STG6_MOAMOA_VOICE1";
#endif
        wp->angleToMario = angleABf(mario->position.x, mario->position.z, (*ep->matrix)[0][3], (*ep->matrix)[2][3]);
        wp->distToMario = distABf(mario->position.x, mario->position.z, (*ep->matrix)[0][3], (*ep->matrix)[2][3]);
        if (!(entry->flags & 0x154) && mario->position.y > 200.0f && wp->distToMario < 30.0f) {
            push += 1.0f;
        }
        if ((wp->flags & 0x1) && !(wp->flags & 0x4) && marioKeyOffChk() == 0 && marioCtrlOffChk() == 0
            && handled == 0 && !(entry->flags & 0x154)) {
            res = moamoa_hit_check(ep, entry);
            if (res == 1) {
                entry->flags |= 0x4;
                entry->state = 0x24;
                wp->hitCount++;
                moamoa_pose_change(entry, "c_n_moa_a", "D_1");
                handled = 1;
            } else if (res == 2 || res == 4) {
                entry->flags |= 0xC;
                entry->state = 0x2F;
                wp->hitCount++;
                moamoa_pose_change(entry, "c_n_moa_a", "D_1");
                if (res != 4) {
                    handled = 1;
                }
            } else if (res == 3) {
                f32 f;

                f = (wp->breathDir + wp->angleToMario) / 2.0f;
                entry->flags |= 0x84;
                entry->state = 0x30;
                entry->field_0x8 = vec3_rsh_00013d88;
                len = frand() * randRange(0.1f, 0.2f) + 0.1f;
                rad = 6.2831855f * f / 360.0f;
                entry->field_0x14 = len * moamoa_sin(rad);
                entry->field_0x1C = randf(0.03f, 0.06f);
                entry->field_0x20 = len * -moamoa_cos(rad);
                entry->frames = 0x78;
                wp->hitCount++;
                moamoa_pose_change(entry, "c_n_moa_a", "D_1");
                handled = 1;
            }
        }
        if (wp->flags & 0x1) {
            if (!(entry->flags & 0x155)) {
                if (!mario->currMotionId || (u16)(mario->currMotionId - 1) <= 1 || (u16)(mario->currMotionId - 0x15) <= 1 ||
                    mario->currMotionId == 0x1A) {
                    if (mario->position.y > 200.0f) {
                        f32 fv = wp->distToMario;

                        if (fv > 30.0f && fv < 100.0f && (i & 1)) {
                            if (entry->flags & 0x2) {
                                entry->queuedState = 2;
                            } else {
                                entry->state = 2;
                            }
                        }
                    }
                } else if (wp->distToMario < 50.0f) {
                    if (entry->flags & 0x2) {
                        entry->queuedState = 1;
                    } else {
                        entry->state = 1;
                    }
                }
            }
        }
        if ((wp->flags & 0x1) && !(entry->flags & 0x444) && (u32)i == gp->retraceCount % wp->count
            && irand(1000) < 200) {
#ifdef MUST_MATCH
            {
                Vec t = vec3_rsh_00013d94;

                t.x = (*ep->matrix)[0][3];
                t.y = (*ep->matrix)[1][3];
                t.z = (*ep->matrix)[2][3];
                v3 = t;
            }
#else
            v3 = moamoa_vec((*ep->matrix)[0][3], (*ep->matrix)[1][3], (*ep->matrix)[2][3], &vec3_rsh_00013d94);
#endif
#ifdef MUST_MATCH
            psndSFXOn_3D((const char*)(u32)sv.p, &v3);
#else
            psndSFXOn_3D(sv, &v3);
#endif
        }
        switch (entry->state) {
        case 0x0: {
            f32 z0, r0, x0;

            if (!(entry->flags & 0x40)) {
                r0 = randRad(0.0f, 360.0f);
                x0 = 5.0f * moamoa_sin(r0) + (*ep->matrix)[0][3];
                z0 = -(5.0f * moamoa_cos(r0) - (*ep->matrix)[2][3]);
                {
                    f32 rb, ra;

                    ra = (f32)rand() / 32767.0f;
                    rb = (f32)rand() / 32767.0f;
                    moamoa_jump(ep, entry, x0, 215.0f, z0, ra * randRange(0.3f, 0.5f) + 0.3f,
                                rb * randRange(0.5f, 0.8f) + 0.5f, entry->state, 0);
                }
            }
            break;
        }
        case 0xE:
            if (--entry->stateTimer < 0) {
                entry->stateTimer = 0;
                entry->state++;
            }
            break;
        case 0xF: {
            f32 f, rad;

            dist = distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], MOA_TARGET(i).x, MOA_TARGET(i).z);
            f = angleABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], MOA_TARGET(i).x, MOA_TARGET(i).z);
            if (dist > 31.0f) {
                rad = 6.2831855f * f / 360.0f;
                tx = 30.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
                tz = -(30.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
                {
                    f32 b;

                    r1 = (f32)rand() / 32767.0f;
                    b = (f32)rand() / 32767.0f;
                    moamoa_jump(ep, entry, tx, 215.0f, tz, 2.0f * (r1 * randRange(1.0f, 1.0f) + 1.0f),
                                2.0f * (b * randRange(0.4f, 0.4f) + 0.4f), entry->state, 0);
                }
            } else {
                f32 b;

                moamoa_joint_position(entry, &v);
                PSVECAdd(&v, &entry->field_0x30, &v);
                r1 = (f32)rand() / 32767.0f;
                b = (f32)rand() / 32767.0f;
                moamoa_jump(ep, entry, v.x, v.y, v.z, 2.0f * (r1 * randRange(1.5f, 1.5f) + 1.5f),
                            2.0f * (b * randRange(0.8f, 0.8f) + 0.8f), 0x3D, 0);
                entry->flags |= 0x200;
                ep->flags |= 4;
            }
            break;
        }
        case 0x3D:
            moamoa_joint_position(entry, &v2);
            PSVECAdd(&v2, &entry->field_0x30, &v2);
            (*ep->matrix)[0][3] = v2.x;
            (*ep->matrix)[1][3] = v2.y;
            (*ep->matrix)[2][3] = v2.z;
            break;
        case 0x32:
            if (--entry->stateTimer < 0) {
                entry->stateTimer = 0;
                entry->state++;
            }
            break;
        case 0x33: {
            f32 ty, f, tx, dist;

            entry->field_0x1C = randf(0.0f, 200.0f);
            entry->field_0x20 = randf(0.0f, 300.0f * entry->field_0x1C / 200.0f);
            entry->state++;
            entry->stateTimer = 0;
            /* fallthrough */
        case 0x34: {
#ifdef MUST_MATCH
            // The branch keeps its result in a temp and copies it to f at the join (as in eki_00).
            f32 ft;

            if (entry->stateTimer < 200) {
                ft = 1.0f;
            } else {
                ft = (f32)(800 - entry->stateTimer) / 600.0f;
            }
            f = ft;
            if (ft < 0.0f) {
                f = 0.0f;
                entry->flags |= 0x200;
            }
#else
            if (entry->stateTimer < 200) {
                f = 1.0f;
            } else {
                f = (f32)(800 - entry->stateTimer) / 600.0f;
            }
            if (f < 0.0f) {
                f = 0.0f;
                entry->flags |= 0x200;
            }
#endif
            tx = MOA_TARGET(i).x;
            ty = MOA_TARGET(i).y;
            tz = MOA_TARGET(i).z;
            dist = entry->field_0x20 * (f32)sin((f32)(i + gp->retraceCount) / 4.0f);
            ty = entry->field_0x1C * f + ty;
            tx = f * dist + tx;
            tz = f * (-entry->field_0x20 * (f32)cos((f32)(i + gp->retraceCount) / 4.0f)) + tz;
            (*ep->matrix)[0][3] += (tx - (*ep->matrix)[0][3]) / 15.0f;
            (*ep->matrix)[1][3] += (ty - (*ep->matrix)[1][3]) / 15.0f;
            (*ep->matrix)[2][3] += (tz - (*ep->matrix)[2][3]) / 15.0f;
            entry->stateTimer++;
            break;
        }
        }
        case 0x1: {
            f32 rad, dist;

            {
                f32 rr, t;

                t = (f32)rand() / 32767.0f;
                rr = wp->angleToMario + (t * randRange(-45.0f, 45.0f) + -45.0f);
                rad = 6.2831855f * rr / 360.0f;
            }
            dist = wp->distToMario;
            tx = 15.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
            ty = (*ep->matrix)[1][3];
            tz = -(15.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
            {
                f32 b, a;

                a = (f32)rand() / 32767.0f * randRange(1.0f, 1.0f) + 1.0f;
                b = randf(0.4f, 0.4f);
                moamoa_jump(ep, entry, tx, ty, tz, a, b, (dist < 100.0f) ? entry->state : 0, 0);
            }
            break;
        }
        case 0x2: {
            f32 r1;

            {
                f32 rr;

                r1 = (f32)rand() / 32767.0f;
                rr = wp->angleToMario + (r1 * randRange(-45.0f, 45.0f) + -45.0f);
                rr += 180.0f;
                dist = wp->distToMario;
                tx = 30.0f * moamoa_sin(6.2831855f * rr / 360.0f) + (*ep->matrix)[0][3];
            }
            ty = (*ep->matrix)[1][3];
            tz = (*ep->matrix)[2][3];
            {
                f32 t, b;

                t = (f32)rand() / 32767.0f;
                f = 2.0f * (t * randRange(1.0f, 1.0f) + 1.0f);
                t = (f32)rand() / 32767.0f;
                b = 2.0f * (t * randRange(0.4f, 0.4f) + 0.4f);
                moamoa_jump(ep, entry, tx, ty, tz, f, b, (dist < 100.0f) ? entry->state : 0, 0);
            }
            break;
        }
        case 0x3: {
            f32 rad3;

            (*ep->matrix)[1][3] += entry->field_0x14;
            entry->field_0x14 -= entry->field_0x20;
            rad3 = 6.2831855f * angleABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], entry->field_0x8.x, entry->field_0x8.z)
                   / 360.0f;
            (*ep->matrix)[0][3] = entry->field_0x1C * moamoa_sin(rad3) + (*ep->matrix)[0][3];
            (*ep->matrix)[2][3] = entry->field_0x1C * -moamoa_cos(rad3) + (*ep->matrix)[2][3];
            if (--entry->frames < 0) {
                (*ep->matrix)[0][3] = entry->field_0x8.x;
                (*ep->matrix)[1][3] = entry->field_0x8.y;
                (*ep->matrix)[2][3] = entry->field_0x8.z;
                entry->flags &= ~0x2;
                ep->flags &= ~0x10;
                entry->stateTimer = 0;
                entry->state++;
            }
            break;
        }
        case 0x4:
            if (++entry->stateTimer > 4) {
                entry->state = entry->queuedState;
            }
            break;
        case 0x24:
        case 0x25:
        case 0x26: {
            f32 rad, px, pz, tz;

            if (!(entry->flags & 0x1)) {
                px = mario->position.x;
                pz = mario->position.z;
            } else {
                px = MOA_TARGET(i).x;
                pz = MOA_TARGET(i).z;
            }
            r1 = (f32)rand() / 32767.0f;
            {
                f32 rr = angleABf(px, pz, (*ep->matrix)[0][3], (*ep->matrix)[2][3]);

                rr = rr + randScale(r1, -45.0f, 45.0f);
                rad = 6.2831855f * rr / 360.0f;
            }
            tx = 15.0f * moamoa_sin(rad) + (*ep->matrix)[0][3];
            ty = (*ep->matrix)[1][3];
            tz = -(15.0f * moamoa_cos(rad) - (*ep->matrix)[2][3]);
            {
                f32 a, b;

                a = (f32)rand() / 32767.0f;
                b = (f32)rand() / 32767.0f;
                moamoa_jump(ep, entry, tx, ty, tz, a * randRange(1.0f, 1.0f) + 1.0f,
                            b * randRange(1.0f, 1.0f) + 1.0f, entry->state + 1, 1);
            }
            break;
        }
        case 0x27:
            if (++entry->stateTimer < 0x1E) {
                (*ep->matrix)[0][0] *= 0.9f;
                (*ep->matrix)[1][1] *= 0.9f;
                (*ep->matrix)[2][2] *= 0.9f;
            } else {
                entry->state = 0x31;
            }
            break;
        case 0x2F: {
            f32 dist, rad, f, grav;

            // The jump's y is the -100 literal, so the float temporary ty carries its z target (retail's register).
            r1 = (f32)rand() / 32767.0f;
            dist = randf(300.0f, 500.0f);
            rad = 6.2831855f * (r1 * randRange(135.0f, 225.0f) + 135.0f) / 360.0f;
            tx = dist * moamoa_sin(rad) + (*ep->matrix)[0][3];
            ty = -(dist * moamoa_cos(rad) - (*ep->matrix)[2][3]);
            f = (f32)rand() / 32767.0f * randRange(5.0f, 6.0f) + 5.0f;
            grav = (f32)rand() / 32767.0f * randRange(0.2f, 0.3f) + 0.2f;
            if (voicePlayed == 0) {
                Vec w;

                w = vec3_rsh_00013da0;
                w.x = (*ep->matrix)[0][3];
                w.y = (*ep->matrix)[1][3];
                w.z = (*ep->matrix)[2][3];
                psndSFXOn_3D((const char*)0x6C3, &w);
                voicePlayed = 1;
            }
            moamoa_jump(ep, entry, tx, -100.0f, ty, f, grav, 0x31, 0);
            break;
        }
        case 0x30:
            if (--entry->frames > 0) {
                entry->field_0x8.x += entry->field_0x14;
                entry->field_0x8.y += entry->field_0x1C;
                entry->field_0x8.z += entry->field_0x20;
                (*ep->matrix)[0][3] += entry->field_0x8.x;
                (*ep->matrix)[1][3] += entry->field_0x8.y;
                (*ep->matrix)[2][3] += entry->field_0x8.z;
            } else {
                entry->state = 0x31;
            }
            break;
        case 0x31: {
            MoamoaEntry* other;
            s32 idx;

            PSMTXIdentity(*ep->matrix);
            do {
                idx = rand() % 700;
                other = &wp->entries[idx];
            } while ((other->state != 0 && other->state != 3) || (other->flags & 0x40));
            {
                ExtPose* src = &extGetPosePtr()[idx];

                (*ep->matrix)[0][3] = (*src->matrix)[0][3];
                (*ep->matrix)[1][3] = 215.0f;
                (*ep->matrix)[2][3] = (*src->matrix)[2][3];
            }
            entry->flags = 0;
            entry->state = 0;
            entry->stateTimer = 0;
            break;
        }
        case 0x3E:
            entry->flags |= 0x20;
            ep->flags &= ~0x1;
            ep->flags |= 0x4;
            break;
        case 0x3F: {
            r1 = (f32)rand() / 32767.0f;
            len = (f32)rand() / 32767.0f * randRange(2.0f, 3.0f) + 2.0f;
            entry->stateTimer = irand(i / 2);
            rad = 6.2831855f * (r1 * randRange(225.0f, 315.0f) + 225.0f) / 360.0f;
            entry->field_0x8.x = len * moamoa_sin(rad);
            entry->field_0x8.y = randf(1.0f, 2.0f);
            entry->field_0x8.z = len * -moamoa_cos(rad);
            entry->state++;
            break;
        }
        case 0x40:
            if (--entry->stateTimer < 0) {
                entry->state++;
            }
            break;
        case 0x41: {
            Mtx rot;

            entry->field_0x8.x *= 1.1f;
            entry->field_0x8.y *= 1.02f;
            entry->field_0x8.z *= 1.1f;
            (*ep->matrix)[0][3] += entry->field_0x8.x;
            (*ep->matrix)[1][3] += entry->field_0x8.y;
            (*ep->matrix)[2][3] += entry->field_0x8.z;
            PSMTXRotRad(rot, 'y', 0.20943952f);
            PSMTXConcat(*ep->matrix, rot, *ep->matrix);
            ep->flags &= ~0x2;
            if ((*ep->matrix)[0][3] < -1000.0f) {
                entry->state = 0x3E;
            }
            break;
        }
        }
        if (!(entry->flags & 0x588)) {
            f32 fx;

            if (cam->cameraAt.x - (*ep->matrix)[0][3] > 250.0f) {
                fx = 450.0f + (*ep->matrix)[0][3];
                if (fx < -50.0f) {
                    (*ep->matrix)[0][3] = fx;
                }
            } else if ((*ep->matrix)[0][3] - cam->cameraAt.x > 250.0f) {
                fx = (*ep->matrix)[0][3] - 450.0f;
                if (fx > -1870.0f) {
                    (*ep->matrix)[0][3] = fx;
                }
            }
        }
        if (!(entry->flags & 0x588)) {
            if ((*ep->matrix)[0][3] > 100.0f) {
                (*ep->matrix)[0][3] = randOffset(100.0f, 0.0f, 5.0f);
            }
            if ((*ep->matrix)[0][3] < -1870.0f) {
                (*ep->matrix)[0][3] = randOffset(-1870.0f, 0.0f, 5.0f);
            }
            if (!(entry->flags & 0x40)) {
                if ((*ep->matrix)[2][3] > 45.0f) {
                    (*ep->matrix)[2][3] = randOffset(45.0f, 0.0f, 5.0f);
                }
                if ((*ep->matrix)[2][3] < -45.0f) {
                    (*ep->matrix)[2][3] = randOffset(-45.0f, 0.0f, 5.0f);
                }
            }
        }
        entry->field_0x28 = (*ep->matrix)[0][3];
        entry->field_0x2C = (*ep->matrix)[2][3];
    }
    moamoa_pose_main();
    mario->riderDelta.x = 0.0f;
    mario->riderDelta.y = 0.0f;
    mario->riderDelta.z = 0.0f;
    if (!(wp->flags & 0x18)) {
        mario->riderDelta.x = -push / 20.0f;
    }
}
#undef MOA_TARGET
#pragma opt_propagation reset
#pragma opt_lifetimes reset
#pragma inline_max_size reset
#pragma inline_max_total_size reset
static void ext_moamoa_dispent(void) {
    if (wp->flags & 1) {
        dispEntry(CAMERA_3D, 0.0f, 1, ext_disp, NULL);
        dispEntry(CAMERA_3D, 0.0f, 2, ext_shadow_disp, NULL);
    }
    if (wp->flags & 2) {
        dispEntry(CAMERA_OFFSCREEN2, -5000.0f, 1, ext_disp, NULL);
        dispEntry(CAMERA_OFFSCREEN2, -5000.0f, 2, ext_shadow_disp, NULL);
    }
}

void moamoa_joint_position(void* entry_, Vec* out) {
    MoamoaEntry* entry = entry_;
    AnimationPose* pose = animPoseGetAnimPosePtr(npcNameToPtr("base")->poseId);
    Vec* positions = (Vec*)((smartEntry*)*(void**)((u8*)pose + 0x15C))->address;
    Vec tmp;
    Vec* py = &positions[wp->chainGroup[entry->chain].y];
    f32 t = (f32)entry->chainSlot / (f32)wp->chainSlots[entry->chain];

    PSVECScale(&positions[wp->chainGroup[entry->chain].x], out, t);
    PSVECScale(py, &tmp, 1.0f - t);
    PSVECAdd(out, &tmp, out);
}

static USER_FUNC(ramp_r) {
    f32 value = __fabsf(evtGetFloat(event, GW(0)));
    f32 wave = (f32)sin((f32)gp->retraceCount / 15.0f);

    evtSetFloat(event, LW(10), 1.8f * value * wave);
    return EVT_RETURN_DONE;
}

static void resha_tev(void* param) {
    envAddTev(7, param);
}

USER_FUNC(npc_anim_wait) {
    return (animPoseGetLoopTimes(npcNameToPtr("base")->poseId) >= 1.0f) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
// clang-format on
