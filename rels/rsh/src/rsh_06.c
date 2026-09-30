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
    u16 field_0x2;   // 0x2
    u16 field_0x4;   // 0x4
    u8 pad_06[0x2];
    Vec field_0x8;   // 0x8
    f32 field_0x14;  // 0x14
    s16 field_0x18;  // 0x18
    u8 pad_1A[0x2];
    f32 field_0x1C;  // 0x1C
    f32 field_0x20;  // 0x20
    u16 field_0x24;  // 0x24
    u8 pad_26[0x3C - 0x26];
    s16 field_0x3C;  // 0x3C
    u8 pad_3E[0x2];
    OSTime field_0x40; // 0x40
    s16 field_0x48;  // 0x48
    s16 field_0x4A;  // 0x4A
    u8 field_0x4C;   // 0x4C
    u8 pad_4D[0x50 - 0x4D];
} MoamoaEntry;

typedef struct MoamoaPose {
    char name[0x10]; // 0x0
    char anim[0x8];  // 0x10
    u16 field_0x18;  // 0x18
    u8 pad_1A[0x2];
    f32 field_0x1C;  // 0x1C
    s32 field_0x20;  // 0x20
    s32 field_0x24;  // 0x24
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
    Vec vec[2];        // 0x14
    u8 pad_2C[0x3A - 0x2C];
    u16 field_0x3A[2]; // 0x3A
    u8 pad_3E[0x48 - 0x3E];
    s32 field_0x48;    // 0x48
    s32 poseCount;     // 0x4C
    MoamoaPose poses[1]; // 0x50
    u8 pad_78[0x570 - 0x78];
    s16 field_0x570[0x80];   // 0x570
    struct { s16 x; s16 y; } field_0x670[0x80]; // 0x670
} MoamoaWork;

static MoamoaWork* wp;

static void resha_tev(void* param);
static USER_FUNC(ramp_r);
static void ext_moamoa_dispent(void);  // TODO: real signature
static void ext_moamoa_main(void);  // TODO: real signature
static void moamoa_hit_check(void);  // TODO: real signature
static void moamoa_jump(ExtPose* pose, MoamoaEntry* entry, s32 field0x2, s32 flag, f32 x, f32 y, f32 z, f32 p4,
                        f32 p5);
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

// TODO: type
static MoamoaGroup group_tbl[11] = {
    {"c_n_moa_a", "S_1"}, {"c_n_moa_b", "S_1"}, {"c_kinodebu", "G_1"}, {"c_puku_c", "U_1"},
    {"c_powan", "O_1"},   {"c_smoko", "T_2"},   {"c_rose", "G_1"},     {"c_papab", "G_1"},
    {"c_mamab", "G_1"},   {"c_konari", "G_1"},  {"c_n_moa_a", "S_1"},
};

// TODO: type
// mt$806 moved into ext_shadow_disp as `static Mtx mt`


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

static USER_FUNC(ramp_r) {
    f32 value = __fabsf(evtGetFloat(event, GW(0)));
    f32 wave = (f32)sin((f32)gp->retraceCount / 15.0f);

    evtSetFloat(event, LW(10), 1.8f * value * wave);
    return EVT_RETURN_DONE;
}

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

static void resha_tev(void* param) {
    envAddTev(7, param);
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

// TODO: write ext_moamoa_main (static, 0x2fdc bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_06.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_moamoa_init (static, 0x7a0 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_06.s; this is where the original has it, just before the first script that uses it

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

USER_FUNC(npc_anim_wait) {
    return (animPoseGetLoopTimes(npcNameToPtr("base")->poseId) >= 1.0f) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

static USER_FUNC(evt_moamoa_blow) {
    s32 i;
    MoamoaEntry* entry;

    evtGetValue(event, event->args[0]);
    extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++, entry++) {
        entry->flags &= ~0x400;
        entry->field_0x2 = 0x3f;
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
                if (entry->field_0x4C == 1) {
                    entry->flags |= 0x800;
                }
            } else {
                entry->flags |= 0x800;
            }
        } else if (b == 0) {
            if (entry->field_0x4C != 1) {
                entry->flags &= ~0x800;
            }
        } else {
            entry->flags &= ~0x800;
        }
        moamoa_pose_change(entry, "c_n_moa_a", "S_1");
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
            entry->field_0x24 = 14;
            entry->field_0x2 = 14;
            entry->field_0x4 = (i < 100) ? i % 100 : i % 60 + 100;
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
            entry->field_0x2 = 0x3D;
            moamoa_pose_change(entry, "c_n_moa_a", "S_1");
        }
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
        entry->field_0x2 = 0x3D;
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
static u32 jumptable_000183EC[66] = {
    (u32)((u8*)&ext_moamoa_main + 0xd38), (u32)((u8*)&ext_moamoa_main + 0x1840), (u32)((u8*)&ext_moamoa_main + 0x1b38), (u32)((u8*)&ext_moamoa_main + 0x1d9c),
    (u32)((u8*)&ext_moamoa_main + 0x1ff8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0xfe4), (u32)((u8*)&ext_moamoa_main + 0x1010),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x201c), (u32)((u8*)&ext_moamoa_main + 0x201c), (u32)((u8*)&ext_moamoa_main + 0x201c), (u32)((u8*)&ext_moamoa_main + 0x2320),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2380),
    (u32)((u8*)&ext_moamoa_main + 0x26d0), (u32)((u8*)&ext_moamoa_main + 0x2760), (u32)((u8*)&ext_moamoa_main + 0x15a8), (u32)((u8*)&ext_moamoa_main + 0x15d4),
    (u32)((u8*)&ext_moamoa_main + 0x169c), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x2ba8),
    (u32)((u8*)&ext_moamoa_main + 0x2ba8), (u32)((u8*)&ext_moamoa_main + 0x14a4), (u32)((u8*)&ext_moamoa_main + 0x2804), (u32)((u8*)&ext_moamoa_main + 0x282c),
    (u32)((u8*)&ext_moamoa_main + 0x2ab8), (u32)((u8*)&ext_moamoa_main + 0x2adc),
};

// TODO: type
// static MoamoaWork* wp;  // moved to top (needed before the functions that use it)

// TODO: write moamoa_joint_position (global, 0x108 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_06.s; this is where the original has it, just before the first script that uses it
void moamoa_joint_position(void* entry_, Vec* out) {
    MoamoaEntry* entry = entry_;
    AnimationPose* pose = animPoseGetAnimPosePtr(npcNameToPtr("base")->poseId);
    Vec* positions = (Vec*)((smartEntry*)*(void**)((u8*)pose + 0x15C))->address;
    Vec tmp;
    Vec* py = &positions[wp->field_0x670[entry->field_0x48].y];
    Vec* px = &positions[wp->field_0x670[entry->field_0x48].x];
    f32 t = (f32)entry->field_0x4A / (f32)wp->field_0x570[entry->field_0x48];

    PSVECScale(px, out, t);
    PSVECScale(py, &tmp, 1.0f - t);
    PSVECAdd(out, &tmp, out);
}

// TODO: write moamoa_hit_check (static, 0xb28 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/rsh_06.s; this is where the original has it, just before the first script that uses it

// NONMATCHING (95.8%): the `(f32)entry->field_0x18` double-to-float conversion feeding field_0x14 gets a different
// temporary FP register (f3/f4/f5) and GPR constant order; all instructions are present otherwise.
static void moamoa_jump(ExtPose* pose, MoamoaEntry* entry, s32 field0x2, s32 flag, f32 x, f32 y, f32 z, f32 p4,
                        f32 p5) {
    if (flag) {
        f32 hx, hy, hz, dist, nx, ny, nz;

        hz = 0.0f;
        hy = 0.0f;
        hx = 0.0f;
        dist = 1000000.0f;
        if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, x, 100.0f + y, z, 0.0f, -1.0f, 0.0f)) {
            if (__fabsf(y - hy) > 30.0) {
                p4 -= 0.1f;
                p5 += 0.1f;
            }
            y = hy;
        }
    }
    {
        Vec pos = {0.0f, 0.0f, 0.0f};

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

        entry->field_0x18 = (s16)(d / entry->field_0x1C);
        entry->field_0x14 = entry->field_0x20 * entry->field_0x18 * 0.5f + dy / entry->field_0x18;
        entry->field_0x2 = 3;
        entry->field_0x24 = field0x2;
        pose->field_0x4 = 180.0f + ang;
    }
    entry->flags |= 2;
    pose->flags |= 0x10;
    pose->flags &= ~4;
}

static void ext_shadow_disp(CameraId cameraId, void* param) {
    static Mtx mt = {{8.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 8.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 8.0f, 0.0f}};
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

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return r * (max - min) + min;
}

static USER_FUNC(evt_moamoa_target_off) {
    s32 index = evtGetValue(event, event->args[0]);

    wp->vec[index] = wp->vec[1 - index];
    wp->field_0x3A[index] = 100;
    if (index == 0) {
        wp->flags |= 0x1000;
    } else {
        wp->flags |= 0x2000;
    }
    return EVT_RETURN_DONE;
}

void evalOn(void) {
    NpcEntry* npc = npcNameToPtr("base");
    AnimationPose* pose;
    Vec* positions;
    MoamoaEntry* entry;
    s32 i;
    s32 n;

    animPoseWorldPositionEvalOn(npc->poseId);
    wp->field_0x48 = npc->poseId;
    pose = animPoseGetAnimPosePtr(npc->poseId);
    positions = (Vec*)((smartEntry*)*(void**)((u8*)pose + 0x15C))->address;
    n = *(s32*)((u8*)pose + 0x160);
    for (i = 0; i < n; i++) {
        positions[i] = (Vec){0.0f, -1000.0f, 0.0f};
    }

    for (i = 0; i < dat_80419a28; i++) {
        wp->field_0x670[i].x = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].name);
        wp->field_0x670[i].y = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].anim);
    }

    {
        MoamoaEntry* e;
        s32 k;
        e = wp->entries;
        for (k = 0; k < wp->count; k++, e++) {
            e->field_0x48 = k % dat_80419a28;
            e->field_0x4A = wp->field_0x570[e->field_0x48]++;
        }
    }

    {
        MoamoaEntry* e;
        s32 k;
        e = wp->entries;
        for (k = 0; k < wp->count; k++, e++) {
            s16 v = e->field_0x48;

            if (v == 0 || v == 5 || v == 0xA || v == 0xF || v == 0x1E || v == 0x22 || v == 0x26 || v == 0x2A || v == 0x2E) {
                e->field_0x4C = 1;
            }
            if (e->field_0x4A == 0) {
                if (e->field_0x48 == 3) { e->field_0x4C = 2; }
                if (e->field_0x48 == 4) { e->field_0x4C = 3; }
                if (e->field_0x48 == 7) { e->field_0x4C = 4; }
                if (e->field_0x48 == 8) { e->field_0x4C = 5; }
                if (e->field_0x48 == 9) { e->field_0x4C = 6; }
                if (e->field_0x48 == 0xC) { e->field_0x4C = 7; }
                if (e->field_0x48 == 0xD) { e->field_0x4C = 8; }
                if (e->field_0x48 == 0xE) { e->field_0x4C = 9; }
            }
        }
    }}

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

static USER_FUNC(evt_moamoa_exit) {
    extReset();
    return EVT_RETURN_DONE;
}

static void moamoa_pose_change(MoamoaEntry* entry, char* name, char* anim) {
    MoamoaPose* pose = wp->poses;
    s32 i;

    if (entry->flags & 0x800) {
        name = group_tbl[entry->field_0x4C].name;
        if (entry->field_0x4C >= 2) {
            anim = group_tbl[entry->field_0x4C].anim;
        }
        if (entry->field_0x4C == 10) {
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
    if (entry->field_0x3C == i) {
        return;
    }
    if ((entry->flags & 4) && strncmp(anim, "D", 1) != 0) {
        return;
    }
    if ((entry->flags & 0x10) && strncmp(anim, "R", 1) != 0) {
        return;
    }
    entry->field_0x3C = i;
    entry->field_0x40 = gp->renderTime;
}

static void moamoa_pose_main(void) {
    s32 i;
    ExtPose* pose;
    MoamoaEntry* entry;

    pose = extGetPosePtr();
    entry = wp->entries;

    for (i = 0; i < wp->count; i++, entry++, pose++) {
        MoamoaPose* poseTbl = &wp->poses[entry->field_0x3C];
        u64 diff = gp->renderTime - entry->field_0x40;
        f32 f;

        f = 60.0f * (f32)OSTicksToMilliseconds(diff) / 1000.0f / poseTbl->field_0x1C;
        if (f >= 1.0f) {
            entry->field_0x40 = gp->renderTime;
            f = 0.0f;
        }
        pose->field_0x10 = poseTbl->field_0x20 + (s32)((f32)(poseTbl->field_0x24 - 1) * f);
    }
}

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
        wp->poses[wp->poseCount].field_0x20 = lastAnim;
        wp->poses[wp->poseCount].field_0x24 = i - lastAnim + 1;
        wp->poses[wp->poseCount].field_0x1C = data->value;
        wp->poses[wp->poseCount].field_0x18 = 1;
        if (strncmp(anim, "J", 1) == 0) {
            wp->poses[wp->poseCount].field_0x18 = 0;
        }
        i++;
        data++;
    } while (data->name != NULL);
    wp->poseCount++;
}
// clang-format on
