#include "eki_00.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_ext.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "driver/hitdrv.h"
#include "driver/imgdrv.h"
#include "driver/offscreendrv.h"
#include "forcedata.h"
#include "math_ext.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party.h"
#include "party/party_motion.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

// Layout of the work area allocated by ext_moamoa_init (offset comments are the original's).
typedef struct MoamoaNpc {
    char name[0x10];  // 0x00
    char anim[8];     // 0x10
    s16 field_0x18;   // 0x18
    f32 field_0x1C;   // 0x1C
    s32 field_0x20;   // 0x20
    s32 field_0x24;   // 0x24
} MoamoaNpc;

typedef struct MoamoaPose {
    u16 flags;          // 0x00
    u16 field_0x02;     // 0x02
    u16 field_0x04;     // 0x04
    u16 field_0x06;     // 0x06
    f32 x;              // 0x08
    f32 y;              // 0x0C
    f32 z;              // 0x10
    f32 field_0x14;     // 0x14
    s16 field_0x18;     // 0x18
    u8 pad_0x1A[2];     // 0x1A
    f32 field_0x1C;     // 0x1C
    f32 field_0x20;     // 0x20
    s16 field_0x24;     // 0x24
    u8 pad_0x26[0x16];  // 0x26
    s16 animId;         // 0x3C
    u16 field_0x3E;     // 0x3E
    OSTime field_0x40;  // 0x40
    u32 field_0x48;     // 0x48
    u8 groupIndex;      // 0x4C
    u8 pad_0x4D[3];     // 0x4D
} MoamoaPose;

typedef struct MoamoaWork {
    u16 field_0x00;    // 0x00
    u16 pad_0x02;      // 0x02
    s32 poseNum;       // 0x04
    MoamoaPose* poses; // 0x08
    u32 field_0x0C;    // 0x0C
    u32 field_0x10;    // 0x10
    Vec field_0x14;    // 0x14
    Vec field_0x20;    // 0x20
    Vec field_0x2C;    // 0x2C
    s16 field_0x38;    // 0x38
    u16 field_0x3A;    // 0x3A
    u16 field_0x3C;    // 0x3C
    u16 field_0x3E;    // 0x3E
    s16 field_0x40;    // 0x40
    s16 field_0x42;    // 0x42
    u8 pad_0x44[8];    // 0x44
    s32 field_0x4C;    // 0x4C
    MoamoaNpc npcs[52];// 0x50
} MoamoaWork;

typedef struct MoamoaData {
    const char* name;  // 0x00
    const char* anim;  // 0x04
    f32 field_0x8;     // 0x08
} MoamoaData;

static MoamoaWork* wp;

const Vec vec3_eki_000088a8 = {0.0f, 0.0f, 0.0f};

static inline GXColor makeColor(u8 r, u8 g, u8 b, u8 a) {
    GXColor color = {0, 0, 0, 0};

    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

#pragma force_active on
static void ext_moamoa_dispent(void);
static void ext_moamoa_main(void);
static s32 moamoa_hit_check(MoamoaPose* pose, s32 kind);
static void moamoa_jump(ExtPose* ep, MoamoaPose* pose, s32 b, s32 c, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5);
static void ext_shadow_disp(CameraId cameraId, void* param);
static void ext_disp(CameraId cameraId, void* param);
static void ext_moamoa_init(void);
static f32 randf(f32 min, f32 max);
static USER_FUNC(evt_moamoa_target_off);
static s32 evt_moamoa_fusion_debug(void);
static USER_FUNC(evt_moamoa_blow);
static USER_FUNC(evt_moamoa_kind_onoff);
static USER_FUNC(evt_moamoa_fusion);
static USER_FUNC(evt_moamoa_end_wait);
static s32 evt_moamoa_ry_reset(void);
static USER_FUNC(evt_moamoa_exit);
static USER_FUNC(evt_moamoa_flag_onoff);
static void moamoa_pose_change(MoamoaPose* pose, const char* name, const char* anim);
static void moamoa_pose_main(void);
static void moamoa_pose_init(void);

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 ext_moamoa_data[141] = {
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
    (u32)"D_1", 0x41000000, 0x00000000, 0x00000000,
    0x00000000,
};

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("e_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("e_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("s_door_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[3] = {
    {"e_bero", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_01", "w_dero", 5, 5, e_door_close, e_door_open},
    {"s_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hom_00", "n_bero_2", 5, 5, s_door_close, s_door_open},
    {NULL},
};

EVT_BEGIN(evt_bridge)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    SET_VALUE(GSWF(3576), 1)
    CALL(evt_bero_mapchange, STRING("hom_00"), STRING("n_bero_2"))
    RETURN
EVT_END()

EVT_BEGIN(evt_bridge2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_00"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_00"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_ori"), 2, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidann"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ori"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kaidan"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_ori"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kabe"), 1)
    CALL(evt_map_playanim, STRING("anm_ere_1"), 2, 0)
    CALL(evt_mario_set_pos, -210, 60, 0)
    CALL(evt_party_set_pos, 0, -230, 60, 0)
    CALL(evt_item_delete_check, STRING("item"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(1), 2000)
        CALL(evt_item_set_position, STRING("item"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_mapobj_rotate, 1, STRING("s_lever"), 0, 0, -80)
    SET_VALUE(GF(0), 1)
    SET_VALUE(GSW(0), 325)
    SET_VALUE(GSWF(3576), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_origami)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mapobj_get_position, STRING("s_lever"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_SWITCH_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, -800, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, STRING("s_lever"), 0, 0, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam3d_evt_set, 340, 80, 280, -130, 90, -15, 1000, 11)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 1)
    SUBTRACT(LW(2), 1)
    CALL(evt_mario_mov_pos, LW(0), LW(2), 30)
    WAIT_MS(500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ORIGAMI_APPEAR1"), -220, 60, 0, 0)
    CALL(evt_map_playanim, STRING("anm_ori"), 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidann"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ori"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kaidan"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_ori"), 1)
    CALL(evt_hitobj_onoff, STRING("kaidann_A"), 1, 1)
    CALL(evt_map_checkanim, STRING("anm_ori"), LW(0), LW(1))
    WAIT_MS(1600)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kabe"), 1)
    WAIT_MS(1000)
    CALL(evt_mobj_switch_blue, 0, STRING("bridge_sw"), -220, 60, 0, PTR(evt_bridge), GSWF(3725))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_APPEAR1"), -220, 60, 0, 0)
    CALL(evt_mobj_set_scale, STRING("bridge_sw"), FLOAT(0.1f), FLOAT(0.1f), FLOAT(0.1f))
    CALL(evt_sub_intpl_msec_init, 8, 100, 1000, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
    CALL(evt_mobj_set_scale, STRING("bridge_sw"), LW(0), LW(0), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GSW(0), 324)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static void ext_moamoa_dispent(void) {
    if (wp->field_0x00 & 0x1) {
        dispEntry(CAMERA_3D, 0.0f, 1, ext_disp, NULL);
        dispEntry(CAMERA_3D, 0.0f, 2, ext_shadow_disp, NULL);
    }
    if (wp->field_0x00 & 0x2) {
        dispEntry(CAMERA_OFFSCREEN2, -5000.0f, 1, ext_disp, NULL);
        dispEntry(CAMERA_OFFSCREEN2, -5000.0f, 2, ext_shadow_disp, NULL);
    }
}

// TODO: write ext_moamoa_main (static, 0x31c0 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_moamoa_init (static, 0x624 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(evt_moamoa_init)
    CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
    CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
    CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
    CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
    CALL(evt_ext_entry, 150, PTR(ext_moamoa_data), PTR(ext_moamoa_init), PTR(ext_moamoa_main), PTR(ext_moamoa_dispent))
    RETURN
EVT_END()

static EVT_BEGIN(evt_meet_moamoa)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_eki_02_6"), 0, STRING("party"))
    END_SWITCH
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 32, 151, 528, 32, 56, -12, 500, 11)
    WAIT_MS(2000)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_dispflag_onoff, 1, 2)
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_N_2"))
    END_INLINE_EVENT
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_0"), 0, STRING("party"))
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_1"), 0, STRING("party"))
        CASE_EQUAL(4)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_2"), 0, STRING("party"))
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_3"), 0, STRING("party"))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_4"), 0, STRING("party"))
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_5"), 0, STRING("party"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("stg6_eki_03_6"), 0, STRING("party"))
    END_SWITCH
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 322)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

static USER_FUNC(evt_moamoa_end_wait) {
    MoamoaPose* pose;
    s32 i;

    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        if (!(pose->flags & 0x20)) {
            return EVT_RETURN_BLOCK;
        }
        pose++;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_exit) {
    extReset();
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_moamoa_escape)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 0, 151, 528, 0, 56, -12, 500, 11)
    CALL(evt_snd_bgmoff_f, 512, 180)
    BEGIN_INLINE_EVENT
        CALL(evt_moamoa_end_wait)
        CALL(evt_moamoa_exit)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_run_case_evt, 9, 1, STRING("a_lever"), 0, PTR(evt_origami), GW(10))
    SET_VALUE(GSW(0), 323)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_elv_rumble)
    IF_INT_EQUAL(GF(0), 0)
        CALL(evt_map_checkanim, STRING("anm_ere_1"), LW(0), LW(1))
    ELSE
        CALL(evt_map_checkanim, STRING("anm_ere_2"), LW(0), LW(1))
    END_IF
    SUBTRACT(LW(1), 500)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_SHAKE1"), LW(3), LW(4), LW(5), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 250)
    WAIT_MS(LW(1))
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_SHAKE1"), LW(3), LW(4), LW(5), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 250)
    RETURN
EVT_END()

static EVT_BEGIN(evt_elv_init)
    CALL(evt_paper_entry, STRING("OFF_hei_07"))
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_hei_07"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 2, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_clear_virtual_point, STRING("capture_img"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    SET_VALUE(GF(0), 0)
    RETURN
EVT_END()

USER_FUNC(img_set_color) {
    s32* args = event->args;
    const char* name;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    ImageEntry* entry;

    name = (const char*)evtGetValue(event, args[0]);
    r = evtGetValue(event, args[1]);
    g = evtGetValue(event, args[2]);
    b = evtGetValue(event, args[3]);
    a = evtGetValue(event, args[4]);
    entry = imgNameToPtr(name, gp->inBattle != 0);
    animPoseSetMaterialFlagOn(entry->poseId, 0x40);
    animPoseSetMaterialEvtColor(entry->poseId, makeColor(r, g, b, a));
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_flag_onoff) {
    s32* args = event->args;
    s32 on;
    s32 mask;

    on = evtGetValue(event, args[0]);
    mask = evtGetValue(event, args[1]);
    if (on) {
        wp->field_0x00 |= mask;
    } else {
        wp->field_0x00 &= ~mask;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_elv_down)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    WAIT_FRAMES(20)
    CALL(evt_mario_mov_pos2, 190, -65, FLOAT(60.0f))
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(1))
    ADD(LW(1), 1)
    WAIT_MS(LW(1))
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 190, -65, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
        CALL(evt_party_jump_pos, 0, 160, 0, -160, 500, 0, -1000)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 190, -160, FLOAT(60.0f))
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_00"), STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_07"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_00"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_MOVE1"), LW(0), LW(1), LW(2), LW(10))
        CALL(evt_map_checkanim, STRING("anm_ere_1"), LW(0), LW(1))
        WAIT_MS(LW(1))
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_1"), 0, 0)
    SPAWN_EVENT(evt_elv_rumble)
    CALL(evt_map_checkanim, STRING("anm_ere_1"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(400)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_00"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_07"), STRING("offscreen"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 0, 1)
        CALL(evt_moamoa_flag_onoff, 1, 2)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 1, 1)
        CALL(evt_moamoa_flag_onoff, 0, 2)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_00"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_00"), 1, 0)
    SWITCH(GSW(0))
        CASE_LESS(323)
        CASE_EQUAL(323)
        CALL(evt_run_case_evt, 9, 1, STRING("a_lever"), 0, PTR(evt_origami), GW(10))
        CASE_DEFAULT
        CALL(evt_mapobj_rotate, 1, STRING("s_lever"), 0, 0, -80)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 324)
        CALL(evt_mobj_switch_blue, 0, STRING("bridge_sw"), -220, 60, 0, PTR(evt_bridge), GSWF(3725))
    END_IF
    CALL(evt_item_delete_check, STRING("item"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(1), 2000)
        CALL(evt_item_set_position, STRING("item"), LW(0), LW(1), LW(2))
    END_IF
    IF_INT_LESS(GSW(0), 324)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kaidann"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ori"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_kaidan"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("n_ori"), 1)
        CALL(evt_hitobj_onoff, STRING("kaidann_A"), 1, 0)
    ELSE
        CALL(evt_map_playanim, STRING("anm_ori"), 2, 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kaidann"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ori"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kaidan"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_ori"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_kabe"), 1)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        WAIT_MS(100)
        CALL(evt_party_move_pos2, 0, 190, -65, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
        WAIT_MS(100)
        CALL(evt_party_move_pos2, 0, 150, -65, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_mario_mov_pos2, 190, -65, FLOAT(60.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    WAIT_MS(500)
    IF_INT_LESS(GSW(0), 322)
        CALL_CHILD(evt_meet_moamoa)
    END_IF
    IF_INT_EQUAL(GSW(0), 322)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
    END_IF
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(1))
    WAIT_MS(LW(1))
    RETURN
EVT_END()

EVT_BEGIN(evt_elv_up)
    CALL(evt_snd_bgmoff, 512)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    WAIT_FRAMES(20)
    CALL(evt_mario_mov_pos2, 190, -65, FLOAT(60.0f))
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(1))
    ADD(LW(1), 1)
    WAIT_MS(LW(1))
    BEGIN_INLINE_EVENT
        CALL(evt_hitobj_onoff, STRING("ere__A"), 0, 0)
        CALL(evt_party_move_pos, 0, 190, -65, 500)
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos, 0, 160, -160, 500)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir, 0, 90, 0)
        CALL(evt_hitobj_onoff, STRING("ere__A"), 0, 1)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos, 190, -160, 500)
    CALL(evt_mario_set_dir, 270, 0, 0)
    WAIT_MS(500)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_00"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_07"), STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_07"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 0, 1)
        CALL(evt_moamoa_flag_onoff, 1, 2)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    IF_INT_LESS(GSW(0), 323)
        CALL(evt_moamoa_flag_onoff, 0, 2)
    END_IF
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_MOVE1"), LW(0), LW(1), LW(2), LW(10))
        CALL(evt_map_checkanim, STRING("anm_ere_2"), LW(0), LW(1))
        WAIT_MS(LW(1))
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_2"), 0, 0)
    SPAWN_EVENT(evt_elv_rumble)
    CALL(evt_map_checkanim, STRING("anm_ere_2"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_BELL1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(400)
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_eki_00"), STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_eki_07"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_1"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_1"), LW(0), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(img_set_color, STRING("capture_img"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    IF_INT_LESS(GSW(0), 324)
    END_IF
    IF_INT_EQUAL(GSW(0), 324)
        CALL(evt_mobj_delete, STRING("bridge_sw"))
    END_IF
    CALL(evt_item_delete_check, STRING("item"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item"), LW(0), LW(1), LW(2))
        ADD(LW(1), 2000)
        CALL(evt_item_set_position, STRING("item"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_eki_00"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_00"), 1, 1)
    IF_INT_EQUAL(GSW(0), 323)
        CALL(evt_del_case_evt, 0, GW(10))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_party_jump_pos, 0, 190, 0, -65, 500, 0, -1000)
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos, 0, 150, -65, 1000)
        CALL(evt_party_stop, 0)
    END_INLINE_EVENT
    WAIT_FRAMES(30)
    CALL(evt_mario_mov_pos, 190, -65, 500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("d_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_ELEVATOR_CLOSE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_ere_door_2"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_ere_door_2"), LW(0), LW(1))
    WAIT_MS(LW(1))
    RETURN
EVT_END()

// TODO: type
static u32 item_tbl[2] = {
    0x0000001A, 0xFFFFFFFF,
};

EVT_BEGIN(evt_elv)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_LESS(GSW(0), 321)
        CALL(evt_win_item_select, 0, PTR(item_tbl), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
        CALL(evt_pouch_remove_item, 26, LW(0))
        SET_VALUE(GSW(0), 321)
        CALL(evt_snd_sfxon, STRING("SFX_STG6_ELEVATOR_KEY1"), 0)
        CALL(evt_msg_print, 0, STRING("stg6_eki_07"), 0, 0)
    END_IF
    IF_INT_EQUAL(GF(0), 0)
        CALL_CHILD(evt_elv_down)
        SET_VALUE(GF(0), 1)
    ELSE
        CALL_CHILD(evt_elv_up)
        SET_VALUE(GF(0), 0)
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_elv_nomsg)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg6_eki_06"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_head)
    CALL(evt_item_delete, STRING("item"))
    CALL(evt_item_entry, STRING("item"), 300, -260, 160, -60, 4, GSWF(3745), 0)
    CALL(evt_item_set_move_dir_speed, STRING("item"), FLOAT(135.0f), FLOAT(90.0f))
    CALL(evt_item_set_gravity, STRING("item"), FLOAT(1.0f))
    CALL(evt_item_set_bound_rate, STRING("item"), FLOAT(0.6f))
    CALL(evt_item_set_jump_power, STRING("item"), FLOAT(500.0f))
    CALL(evt_item_status_on, STRING("item"), 16)
    CALL(evt_item_flag_on, STRING("item"), 2048)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(evt_kanban)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg6_eki_00_01"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(eki_00_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    IF_INT_EQUAL(GSWF(3576), 0)
        CALL_CHILD(evt_bero_info_run)
    ELSE
        CALL_CHILD(bero_case_entry)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_eki_07"), 1)
    CALL(evt_hitobj_onoff, STRING("a_eki_07"), 1, 0)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 320)
        CALL_CHILD(evt_elv_init)
        CALL(evt_run_case_evt, 9, 1, STRING("a_suitti"), 0, PTR(evt_elv), 0)
    ELSE
        CALL(evt_run_case_evt, 9, 1, STRING("a_suitti"), 0, PTR(evt_elv_nomsg), 0)
    END_IF
    IF_INT_LESS(GSW(0), 323)
        CALL_CHILD(evt_moamoa_init)
    END_IF
    CALL(evt_item_entry, STRING("item"), 300, -260, 130, -60, 17, GSWF(3745), 0)
    IF_INT_EQUAL(GSWF(3745), 0)
        CALL(evt_run_case_evt, 16, 1, STRING("a_dai_sita"), 0, PTR(evt_head), 0)
    END_IF
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI2"))
    IF_INT_EQUAL(GSWF(3576), 1)
        SPAWN_EVENT(evt_bridge2)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("a_kan"), 0, PTR(evt_kanban), 0)
    RETURN
EVT_END()

// TODO: type
static u32 jumptable_00002400[66] = {
    (u32)((u8*)&ext_moamoa_main + 0xcf0), (u32)((u8*)&ext_moamoa_main + 0x1770), (u32)((u8*)&ext_moamoa_main + 0x1a68), (u32)((u8*)&ext_moamoa_main + 0x1ccc),
    (u32)((u8*)&ext_moamoa_main + 0x1f28), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2744), (u32)((u8*)&ext_moamoa_main + 0x2810), (u32)((u8*)&ext_moamoa_main + 0x2834),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x1f4c), (u32)((u8*)&ext_moamoa_main + 0x1f4c), (u32)((u8*)&ext_moamoa_main + 0x1f4c), (u32)((u8*)&ext_moamoa_main + 0x2250),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x22b0),
    (u32)((u8*)&ext_moamoa_main + 0x2600), (u32)((u8*)&ext_moamoa_main + 0x2690), (u32)((u8*)&ext_moamoa_main + 0x14d8), (u32)((u8*)&ext_moamoa_main + 0x1504),
    (u32)((u8*)&ext_moamoa_main + 0x15cc), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8),
    (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2ef8), (u32)((u8*)&ext_moamoa_main + 0x2b54), (u32)((u8*)&ext_moamoa_main + 0x2b7c),
    (u32)((u8*)&ext_moamoa_main + 0x2e08), (u32)((u8*)&ext_moamoa_main + 0x2e2c),
};

// TODO: write moamoa_hit_check (static, 0xb28 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_00.s; this is where the original has it, just before the first script that uses it

// NONMATCHING (78.79%): float arg register allocation (f27/f28/f29 vs f31/f27/f28), extra `frsp` before the
// pose Vec stores and the fabs/angle result scheduling; logic is complete.
static void moamoa_jump(ExtPose* ep, MoamoaPose* pose, s32 b, s32 c, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5) {
    f32 hitX;
    f32 hitY;
    f32 hitZ;
    f32 distance;
    f32 hitNX;
    f32 hitNY;
    f32 hitNZ;
    Vec pos;
    f32 ty;
    f32 dist;
    f32 angle;
    f32 dy;
    s32 distInt;

    if (c != 0) {
        hitX = 0.0f;
        hitY = 0.0f;
        hitZ = 0.0f;
        distance = 1000000.0f;
        if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, f1, f2 + 100.0f, f3,
                           0.0f, -1.0f, 0.0f)
            != NULL) {
            dy = f2 - hitY;
            if (__fabs(dy) > 30.0) {
                f4 -= 0.1f;
                f5 += 0.1f;
            }
            f2 = hitY;
        }
    }
    pos = vec3_eki_000088a8;
    pos.x = f1;
    pos.y = f2;
    pos.z = f3;
    pose->x = pos.x;
    pose->y = pos.y;
    pose->z = pos.z;
    pose->field_0x1C = f4;
    pose->field_0x20 = f5;
    ty = pose->y - ep->matrix[0][1][3];
    dist = distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->x, pose->z);
    angle = angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->x, pose->z);
    distInt = (s32)(dist / pose->field_0x1C);
    pose->field_0x18 = (s16)distInt;
    pose->field_0x14 = pose->field_0x20 * (f32)distInt * 0.5f + ty / (f32)pose->field_0x18;
    pose->field_0x02 = 3;
    pose->field_0x24 = b;
    ep->field_0x4 = 180.0f + angle;
    pose->flags |= 0x2;
    ep->flags |= 0x10;
    ep->flags &= ~0x4;
}

static void ext_shadow_disp(CameraId cameraId, void* param) {
    static Mtx mt = {
        {8.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 8.0f, 0.0f, 0.5f},
        {0.0f, 0.0f, 8.0f, 0.0f},
    };
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;

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
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;

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
    GXSetTevDirect(0);
    if (cameraId == CAMERA_OFFSCREEN2) {
        sysWaitDrawSync();
        GXReadBoundingBox(&left, &top, &right, &bottom);
        offscreenAddBoundingBox(offscreenNameToId("offscreen"), left, top, right, bottom);
    }
}

static f32 randf(f32 min, f32 max) {
    f32 t = (f32)rand() / 32767.0f;

    return t * (max - min) + min;
}

static USER_FUNC(evt_moamoa_target_off) {
    s32 index = evtGetValue(event, event->args[0]);
    Vec* vecs = &wp->field_0x14;

    vecs[index] = vecs[1 - index];
    (&wp->field_0x3A)[index] = 0x64;
    if (index == 0) {
        wp->field_0x00 |= 0x1000;
    } else {
        wp->field_0x00 |= 0x2000;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (99%): 46 of 119 instructions differ in operands only (registers or offsets)
static s32 evt_moamoa_fusion_debug(void) {
    ExtPose* ep = extGetPosePtr();
    MoamoaPose* pose = wp->poses;
    s32 i;

    for (i = 0; i < wp->poseNum; i++) {
        ep->flags |= 4;
        pose->flags |= 0x400;
        pose->flags |= 0x100;
        pose->flags |= 0x200;
        pose->field_0x02 = 0x3D;
        pose->flags |= 0x800;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        ep++;
        pose++;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_blow) {
    s32 i;
    MoamoaPose* pose;

    evtGetValue(event, event->args[0]);
    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        pose->flags &= ~0x400;
        pose->field_0x02 = 0x3F;
        pose++;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.21%): 43 of 136 instructions differ in operands only (registers or offsets)
static USER_FUNC(evt_moamoa_kind_onoff) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 mode = evtGetValue(event, args[1]);
    MoamoaPose* pose;
    s32 i;

    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        if (on != 0) {
            if (mode == 0) {
                if (pose->groupIndex == 1) {
                    pose->flags |= 0x800;
                }
            } else {
                pose->flags |= 0x800;
            }
        } else {
            if (mode == 0) {
                if (pose->groupIndex != 1) {
                    pose->flags &= ~0x800;
                }
            } else {
                pose->flags &= ~0x800;
            }
        }
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        pose++;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_fusion) {
    s32 i;
    ExtPose* ep;
    MoamoaPose* pose;

    if (isFirstCall) {
        ep = extGetPosePtr();
        pose = wp->poses;
        for (i = 0; i < wp->poseNum; i++, ep++, pose++) {
            ep->matrix[0][2][2] = 1.0f;
            ep->matrix[0][1][1] = 1.0f;
            ep->matrix[0][0][0] = 1.0f;
            pose->field_0x24 = 0xE;
            pose->field_0x02 = 0xE;
            pose->field_0x04 = (i < 100) ? i % 100 : i % 60 + 100;
            pose->flags |= 0x100;
        }
    }
    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        if (!(pose->flags & 0x200)) {
            return EVT_RETURN_BLOCK;
        }
        pose++;
    }
    ep = extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++, ep++, pose++) {
        ep->flags |= 4;
        pose->flags |= 0x400;
        pose->field_0x02 = 0x3D;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
    }
    return EVT_RETURN_DONE;
}

static s32 evt_moamoa_ry_reset(void) {
    ExtPose* pose = extGetPosePtr();
    s32 i;

    for (i = 0; i < wp->poseNum; i++) {
        pose->field_0x14 = 0.0f;
        pose->field_0x1C = 0.0f;
        pose++;
    }
    return EVT_RETURN_DONE;
}

static void moamoa_pose_change(MoamoaPose* pose, const char* name, const char* anim) {
    static const char* group_tbl[11][2] = {
        {"c_n_moa_a", "S_1"},
        {"c_n_moa_b", "S_1"},
        {"c_kinodebu", "G_1"},
        {"c_puku_c", "U_1"},
        {"c_powan", "O_1"},
        {"c_smoko", "T_2"},
        {"c_rose", "G_1"},
        {"c_papab", "G_1"},
        {"c_mamab", "G_1"},
        {"c_konari", "G_1"},
        {"c_n_moa_a", "S_1"},
    };
    MoamoaNpc* npc = wp->npcs;
    s32 i;

    if (pose->flags & 0x800) {
        name = group_tbl[pose->groupIndex][0];
        if (pose->groupIndex >= 2) {
            anim = group_tbl[pose->groupIndex][1];
        }
        if (pose->groupIndex == 0xa) {
            anim = "S_2";
        }
    }
    for (i = 0; i < wp->field_0x4C; i++, npc++) {
        if (strcmp(npc->name, name) == 0 && strcmp(npc->anim, anim) == 0) {
            break;
        }
    }
    if (i < wp->field_0x4C && pose->animId != i) {
        if (!(pose->flags & 0x4) || strncmp(anim, "D", 1) == 0) {
            if (!(pose->flags & 0x10) || strncmp(anim, "R", 1) == 0) {
                pose->animId = i;
                pose->field_0x40 = gp->renderTime;
            }
        }
    }
}

static void moamoa_pose_main(void) {
    s32 i;
    ExtPose* ep;
    MoamoaPose* pose;

    ep = extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        MoamoaNpc* npc = &wp->npcs[pose->animId];
        f32 ratio = (f32)((u64)(gp->renderTime - pose->field_0x40) / (u64)((OS_BUS_CLOCK >> 2) / 1000)) * 60.0f
                    / 1000.0f / npc->field_0x1C;

        if (ratio >= 1.0f) {
            pose->field_0x40 = gp->renderTime;
            ratio = 0.0f;
        }
        pose++;
        ep->field_0x10 = npc->field_0x20 + (s32)((npc->field_0x24 - 1) * ratio);
        ep++;
    }
}

static void moamoa_pose_init(void) {
    char name[0x20];
    char anim[0x20];
    MoamoaData* entry = (MoamoaData*)ext_moamoa_data;
    s32 i = 0;
    s32 start;

    strcpy(name, "");
    strcpy(anim, "");
    wp->field_0x4C = -1;
    do {
        if (strcmp(entry->name, name) != 0) {
            strcpy(name, entry->name);
            strcpy(anim, "");
        }
        if (strcmp(entry->anim, anim) != 0) {
            strcpy(anim, entry->anim);
            start = i;
            wp->field_0x4C++;
        }
        strcpy(wp->npcs[wp->field_0x4C].name, name);
        strcpy(wp->npcs[wp->field_0x4C].anim, anim);
        wp->npcs[wp->field_0x4C].field_0x20 = start;
        wp->npcs[wp->field_0x4C].field_0x24 = i - start + 1;
        wp->npcs[wp->field_0x4C].field_0x1C = entry->field_0x8;
        wp->npcs[wp->field_0x4C].field_0x18 = 1;
        if (strncmp(anim, "J", 1) == 0) {
            wp->npcs[wp->field_0x4C].field_0x18 = 0;
        }
        i++;
        entry++;
    } while (entry->name != NULL);
    wp->field_0x4C++;
}// clang-format on
