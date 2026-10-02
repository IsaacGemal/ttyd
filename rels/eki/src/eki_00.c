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
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party.h"
#include "party/party_motion.h"
#include "party/party_cloud.h"
#include "party/party_nokonoko.h"
#include "pmario_sound.h"
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
    s16 field_0x02;     // 0x02
    s16 field_0x04;     // 0x04
    u16 field_0x06;     // 0x06
    Vec position;       // 0x08
    f32 field_0x14;     // 0x14
    s16 field_0x18;     // 0x18
    u8 pad_0x1A[2];     // 0x1A
    f32 field_0x1C;     // 0x1C
    f32 field_0x20;     // 0x20
    s16 field_0x24;     // 0x24
    u8 pad_0x26[2];     // 0x26
    Vec field_0x28;     // 0x28
    u8 pad_0x34[8];     // 0x34
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
    EventEntry* escapeEvt; // 0x44
    u8 pad_0x48[4];    // 0x48
    s32 field_0x4C;    // 0x4C
    MoamoaNpc npcs[32];// 0x50
    f32 field_0x550;   // 0x550
    f32 field_0x554;   // 0x554
    s32 field_0x558;   // 0x558
    f32 field_0x55C;   // 0x55C
    f32 field_0x560;   // 0x560
    Vec field_0x564;   // 0x564
} MoamoaWork;

typedef struct MoamoaData {
    const char* name;  // 0x00
    const char* anim;  // 0x04
    f32 field_0x8;     // 0x08
} MoamoaData;

static MoamoaWork* wp;

const Vec vec3_eki_000088a8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088b4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088c0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088cc = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088d8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088e4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088f0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088fc = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_00008908 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_00008914 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_00008920 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_0000892c = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_00008884 = {0.0f, 0.0f, 10.0f};
const Vec vec3_eki_00008890 = {0.0f, 0.0f, 10.0f};
const Vec vec3_eki_0000889c = {0.0f, 0.0f, 10.0f};

static inline f32 ekiSinf(f32 rad) {
    f32 f0, f1, sinv = 0.00761f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            sinv = sinv * f0 - 0.16605f;
            sinv = sinv * f0 + 1.0f;
            sinv = sinv * rad;
        } else {
            f0 = rad - 1.5707964f;
            f1 = 1.5707964f - f0;
            f0 = f1 * f1;
            sinv = sinv * f0 - 0.16605f;
            sinv = sinv * f0 + 1.0f;
            sinv = sinv * f1;
        }
    } else if (rad < 4.712389f) {
        f1 = rad - 3.1415927f;
        f0 = f1 * f1;
        sinv = sinv * f0 - 0.16605f;
        sinv = sinv * f0 + 1.0f;
        sinv = sinv * f1;
        sinv = -sinv;
    } else {
        f0 = rad - 4.712389f;
        f1 = 1.5707964f - f0;
        f0 = f1 * f1;
        sinv = sinv * f0 - 0.16605f;
        sinv = sinv * f0 + 1.0f;
        sinv = sinv * f1;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 ekiCosf(f32 rad) {
    f32 f0, cosv = 0.03705f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            cosv = cosv * f0 - 0.4967f;
            cosv = cosv * f0 + 1.0f;
        } else {
            f0 = rad - 1.5707964f;
            f0 = 1.5707964f - f0;
            f0 = f0 * f0;
            cosv = cosv * f0 - 0.4967f;
            cosv = cosv * f0 + 1.0f;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        f0 = rad - 3.1415927f;
        f0 = f0 * f0;
        cosv = cosv * f0 - 0.4967f;
        cosv = cosv * f0 + 1.0f;
        cosv = -cosv;
    } else {
        f0 = rad - 4.712389f;
        f0 = 1.5707964f - f0;
        f0 = f0 * f0;
        cosv = cosv * f0 - 0.4967f;
        cosv = cosv * f0 + 1.0f;
    }
    return cosv;
}

static inline Vec ekiVec3(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

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
static s32 evt_moamoa_escape[0xBC / 4];
static s32 moamoa_hit_check(ExtPose* ep, MoamoaPose* pose);
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

static MoamoaData ext_moamoa_data[47] = {
    {"c_n_moa_a", "S_1", 0.0f},
    {"c_n_moa_a", "S_1", 2.0f},
    {"c_n_moa_a", "S_1", 4.0f},
    {"c_n_moa_a", "S_1", 6.0f},
    {"c_n_moa_a", "S_1", 8.0f},
    {"c_n_moa_a", "S_1", 10.0f},
    {"c_n_moa_a", "S_1", 12.0f},
    {"c_n_moa_a", "S_1", 14.0f},
    {"c_n_moa_a", "S_1", 16.0f},
    {"c_n_moa_a", "S_1", 18.0f},
    {"c_n_moa_a", "S_1", 20.0f},
    {"c_n_moa_a", "S_1", 22.0f},
    {"c_n_moa_a", "S_1", 24.0f},
    {"c_n_moa_a", "S_1", 26.0f},
    {"c_n_moa_a", "S_1", 28.0f},
    {"c_n_moa_a", "S_1", 30.0f},
    {"c_n_moa_a", "S_1", 32.0f},
    {"c_n_moa_a", "S_1", 34.0f},
    {"c_n_moa_a", "S_1", 36.0f},
    {"c_n_moa_a", "S_1", 38.0f},
    {"c_n_moa_a", "D_1", 0.0f},
    {"c_n_moa_a", "D_1", 4.0f},
    {"c_n_moa_a", "D_1", 8.0f},
    {"c_n_moa_b", "S_1", 0.0f},
    {"c_n_moa_b", "S_1", 2.0f},
    {"c_n_moa_b", "S_1", 4.0f},
    {"c_n_moa_b", "S_1", 6.0f},
    {"c_n_moa_b", "S_1", 8.0f},
    {"c_n_moa_b", "S_1", 10.0f},
    {"c_n_moa_b", "S_1", 12.0f},
    {"c_n_moa_b", "S_1", 14.0f},
    {"c_n_moa_b", "S_1", 16.0f},
    {"c_n_moa_b", "S_1", 18.0f},
    {"c_n_moa_b", "S_1", 20.0f},
    {"c_n_moa_b", "S_1", 22.0f},
    {"c_n_moa_b", "S_1", 24.0f},
    {"c_n_moa_b", "S_1", 26.0f},
    {"c_n_moa_b", "S_1", 28.0f},
    {"c_n_moa_b", "S_1", 30.0f},
    {"c_n_moa_b", "S_1", 32.0f},
    {"c_n_moa_b", "S_1", 34.0f},
    {"c_n_moa_b", "S_1", 36.0f},
    {"c_n_moa_b", "S_1", 38.0f},
    {"c_n_moa_b", "D_1", 0.0f},
    {"c_n_moa_b", "D_1", 4.0f},
    {"c_n_moa_b", "D_1", 8.0f},
    {NULL, NULL, 0.0f},
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

#pragma inline_max_total_size(60000)
#pragma inline_max_size(1000)
#pragma opt_propagation off
static inline f32 ekiSinf2(f32 rad) {
    f32 f0, f1, sinv = 0.00761f;
    f32 c1 = 0.16605f, c2 = 1.0f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        } else {
            f0 = rad - 1.5707964f;
            f1 = 1.5707964f - f0;
            f0 = f1 * f1;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * f1;
        }
    } else if (rad < 4.712389f) {
        f1 = rad - 3.1415927f;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    } else {
        f0 = rad - 4.712389f;
        f1 = 1.5707964f - f0;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 ekiCosf2(f32 rad) {
    f32 f0, cosv = 0.03705f;
    f32 c1 = 0.4967f, c2 = 1.0f;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
        } else {
            f0 = rad - 1.5707964f;
            f0 = 1.5707964f - f0;
            f0 = f0 * f0;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        f0 = rad - 3.1415927f;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = rad - 4.712389f;
        f0 = 1.5707964f - f0;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

// Per-frame update of every moamoa: hit checks against Mario and his partner, the jump/scatter state machine
// (MoamoaPose.field_0x02), clamping into the room, then the animation frame update and the rider-delta limit.
#define MOA_TARGET(i) ((&wp->field_0x14)[(i) % 3])
static void ext_moamoa_main(void) {
    MarioWork* mario;
    PartyEntry* party;
    ExtPose* ep;
    MoamoaPose* pose;
    s32 i;
    s32 handled;
    s32 voicePlayed;
    f32 rad;
    f32 tx;
    f32 ty;
    f32 tz;
    f32 dist;
    f32 f;
    s32 res;
    Vec v;

    mario = marioGetPtr();
    party = partyGetPtr(0);
    camGetPtr(CAMERA_3D);
    handled = 0;
    if (mario->currMotionId == MARIO_MOTION_HAMMER) {
        wp->field_0x40++;
    } else {
        wp->field_0x40 = 0;
    }
    if (mario->currMotionId == MARIO_MOTION_STAY) {
        wp->field_0x42++;
        if (wp->field_0x42 > 1000) {
            wp->field_0x42 = 1000;
        }
    } else {
        wp->field_0x42 = 0;
    }
    if (party->flags & 0x100) {
        wp->field_0x00 |= 0x10;
    } else {
        wp->field_0x00 &= ~0x10;
    }
    wp->field_0x55C = -1.0f;
    wp->field_0x560 = -1.0f;
    if (party->currentMemberId == 5 && (party->flags & 0x100)) {
        wp->field_0x55C = cloudGetBreathDir();
        wp->field_0x560 = cloudGetBreathDist();
        rad = 6.2831855f * wp->field_0x55C / 360.0f;
        wp->field_0x564.x = wp->field_0x560 * ekiSinf2(rad) + party->position.x;
        wp->field_0x564.y = party->position.y;
        rad = 6.2831855f * wp->field_0x55C / 360.0f;
        wp->field_0x564.z = -(wp->field_0x560 * ekiCosf2(rad) - party->position.z);
    }
    if (wp->field_0x38 > 0xFA) {
        extGetPosePtr();
        pose = wp->poses;
        for (i = 0; i < wp->poseNum; i++, pose++) {
            if (!(pose->flags & 0x4) && !(pose->flags & 0x10)) {
                pose->field_0x02 = 0x19;
                pose->field_0x04 = 0;
                pose->flags |= 0x10;
            }
        }
        if (wp->escapeEvt == NULL) {
            wp->escapeEvt = evtEntry(evt_moamoa_escape, 0, 0);
        }
    }
    ep = extGetPosePtr();
    voicePlayed = 0;
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++, ep++, pose++) {
        wp->field_0x550 = angleABf(mario->position.x, mario->position.z, ep->matrix[0][0][3], ep->matrix[0][2][3]);
        wp->field_0x554 = distABf(mario->position.x, mario->position.z, ep->matrix[0][0][3], ep->matrix[0][2][3]);
        if ((wp->field_0x00 & 0x1) && !(wp->field_0x00 & 0x4) && marioKeyOffChk() == 0 && marioCtrlOffChk() == 0
            && handled == 0 && !(pose->flags & 0x154)) {
            res = moamoa_hit_check(ep, pose);
            if (res == 1) {
                pose->flags |= 0x4;
                pose->field_0x02 = 0x24;
                wp->field_0x38++;
                moamoa_pose_change(pose, "c_n_moa_a", "D_1");
                handled = 1;
            } else if (res == 2 || res == 4) {
                pose->flags |= 0xC;
                pose->field_0x02 = 0x2F;
                wp->field_0x38++;
                moamoa_pose_change(pose, "c_n_moa_a", "D_1");
                if (res != 4) {
                    handled = 1;
                }
            } else if (res == 3) {
                pose->flags |= 0x84;
                pose->field_0x02 = 0x30;
                f = (wp->field_0x55C + wp->field_0x550) * 0.5f;
                pose->position = vec3_eki_00008908;
                dist = randf(0.1f, 0.2f);
                rad = 6.2831855f * f / 360.0f;
                pose->field_0x14 = dist * ekiSinf2(rad);
                pose->field_0x1C = randf(0.03f, 0.06f);
                pose->field_0x20 = dist * -ekiCosf2(rad);
                pose->field_0x18 = 0x78;
                wp->field_0x38++;
                moamoa_pose_change(pose, "c_n_moa_a", "D_1");
                handled = 1;
            }
        }
        if (wp->field_0x00 & 0x1) {
            if (!(pose->flags & 0x155) && wp->field_0x554 < 50.0f) {
                if (pose->flags & 0x2) {
                    pose->field_0x24 = 1;
                } else {
                    pose->field_0x02 = 1;
                }
            }
        }
        if ((wp->field_0x00 & 0x1) && !(pose->flags & 0x444) && (u32)i == gp->retraceCount % wp->poseNum
            && irand(1000) < 200) {
            v = ekiVec3(ep->matrix[0][0][3], ep->matrix[0][1][3], ep->matrix[0][2][3], &vec3_eki_00008914);
            psndSFXOn_3D("SFX_STG6_MOAMOA_VOICE1", &v);
        }
        switch (pose->field_0x02) {
        case 0x0:
            if (i % 50 < 0x14) {
                f32 ang = randf(0.0f, 6.2831855f);
                f32 len = randf(0.0f, 50.0f);

                tx = len * ekiSinf2(ang) + MOA_TARGET(i).x;
                tz = -(len * ekiCosf2(ang) - MOA_TARGET(i).z);
                rad = 6.2831855f * angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], tx, tz) / 360.0f;
                tx = 15.0f * ekiSinf2(rad) + ep->matrix[0][0][3];
                ty = ep->matrix[0][1][3];
                tz = -(15.0f * ekiCosf2(rad) - ep->matrix[0][2][3]);
                moamoa_jump(ep, pose, pose->field_0x02, 1, tx, ty, tz, randf(1.5f, 3.0f), randf(0.8f, 2.0f));
                pose->flags |= 0x1;
            } else {
                f32 ang;

                dist = distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], MOA_TARGET(i).x, MOA_TARGET(i).z);
                ang = angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], MOA_TARGET(i).x, MOA_TARGET(i).z);
                if (dist < 100.0f) {
                    ang = randf(0.0f, 360.0f);
                }
                rad = 6.2831855f * ang / 360.0f;
                tx = 5.0f * ekiSinf2(rad) + ep->matrix[0][0][3];
                ty = ep->matrix[0][1][3];
                tz = -(5.0f * ekiCosf2(rad) - ep->matrix[0][2][3]);
                moamoa_jump(ep, pose, pose->field_0x02, 1, tx, ty, tz, randf(0.3f, 0.5f), randf(0.5f, 0.8f));
                pose->flags &= ~0x1;
            }
            break;
        case 0x32:
            if (--pose->field_0x04 < 0) {
                pose->field_0x04 = 0;
                pose->field_0x02++;
            }
            break;
        case 0x33:
            pose->field_0x1C = randf(0.0f, 200.0f);
            f = 300.0f * pose->field_0x1C / 200.0f;
            pose->field_0x20 = randf(0.0f, f);
            pose->field_0x02++;
            pose->field_0x04 = 0;
            /* fallthrough */
        case 0x34:
            if (pose->field_0x04 < 200) {
                f = 1.0f;
            } else {
                f = (f32)(800 - pose->field_0x04) / 600.0f;
            }
            if (f < 0.0f) {
                f = 0.0f;
                pose->flags |= 0x200;
            }
            tx = MOA_TARGET(i).x;
            ty = MOA_TARGET(i).y;
            tz = MOA_TARGET(i).z;
            dist = pose->field_0x20 * (f32)sin((f32)(i + gp->retraceCount) * 0.25f);
            ty = pose->field_0x1C * f + ty;
            tx = f * dist + tx;
            tz = f * (-pose->field_0x20 * (f32)cos((f32)(i + gp->retraceCount) * 0.25f)) + tz;
            ep->matrix[0][0][3] += (tx - ep->matrix[0][0][3]) / 15.0f;
            ep->matrix[0][1][3] += (ty - ep->matrix[0][1][3]) / 15.0f;
            ep->matrix[0][2][3] += (tz - ep->matrix[0][2][3]) / 15.0f;
            pose->field_0x04++;
            break;
        case 0x1:
            dist = wp->field_0x554;
            rad = 6.2831855f * (wp->field_0x550 + randf(-45.0f, 45.0f)) / 360.0f;
            tx = 15.0f * ekiSinf2(rad) + ep->matrix[0][0][3];
            ty = ep->matrix[0][1][3];
            tz = -(15.0f * ekiCosf2(rad) - ep->matrix[0][2][3]);
            moamoa_jump(ep, pose, (dist < 100.0f) ? pose->field_0x02 : 0, 1, tx, ty, tz, randf(1.0f, 1.0f),
                        randf(0.4f, 0.4f));
            break;
        case 0x2:
            dist = wp->field_0x554;
            rad = 6.2831855f * (wp->field_0x550 + randf(-45.0f, 45.0f) + 180.0f) / 360.0f;
            ty = ep->matrix[0][1][3];
            tx = 30.0f * ekiSinf2(rad) + ep->matrix[0][0][3];
            tz = ep->matrix[0][2][3];
            moamoa_jump(ep, pose, (dist < 100.0f) ? pose->field_0x02 : 0, 1, tx, ty, tz,
                        2.0f * randf(1.0f, 1.0f), 2.0f * randf(0.4f, 0.4f));
            break;
        case 0x3:
            ep->matrix[0][1][3] += pose->field_0x14;
            pose->field_0x14 -= pose->field_0x20;
            rad = 6.2831855f * angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->position.x, pose->position.z)
                  / 360.0f;
            ep->matrix[0][0][3] = pose->field_0x1C * ekiSinf2(rad) + ep->matrix[0][0][3];
            ep->matrix[0][2][3] = pose->field_0x1C * -ekiCosf2(rad) + ep->matrix[0][2][3];
            if (--pose->field_0x18 < 0) {
                ep->matrix[0][0][3] = pose->position.x;
                ep->matrix[0][1][3] = pose->position.y;
                ep->matrix[0][2][3] = pose->position.z;
                pose->flags &= ~0x2;
                ep->flags &= ~0x10;
                pose->field_0x04 = 0;
                pose->field_0x02++;
            }
            break;
        case 0x4:
            if (++pose->field_0x04 > 4) {
                pose->field_0x02 = pose->field_0x24;
            }
            break;
        case 0x24:
        case 0x25:
        case 0x26:
            if (!(pose->flags & 0x1)) {
                tx = mario->position.x;
                tz = mario->position.z;
            } else {
                tx = MOA_TARGET(i).x;
                tz = MOA_TARGET(i).z;
            }
            rad = 6.2831855f * (angleABf(tx, tz, ep->matrix[0][0][3], ep->matrix[0][2][3]) + randf(-45.0f, 45.0f))
                  / 360.0f;
            tx = 15.0f * ekiSinf2(rad) + ep->matrix[0][0][3];
            ty = ep->matrix[0][1][3];
            tz = -(15.0f * ekiCosf2(rad) - ep->matrix[0][2][3]);
            moamoa_jump(ep, pose, pose->field_0x02 + 1, 1, tx, ty, tz, randf(1.0f, 1.0f), randf(1.0f, 1.0f));
            break;
        case 0x27:
            if (++pose->field_0x04 < 0x1E) {
                ep->matrix[0][0][0] *= 0.9f;
                ep->matrix[0][1][1] = ep->matrix[0][1][1] * 0.9f;
                ep->matrix[0][2][2] = ep->matrix[0][2][2] * 0.9f;
            } else {
                pose->field_0x02 = 0x31;
            }
            break;
        case 0x2F:
            rad = 6.2831855f * randf(135.0f, 225.0f) / 360.0f;
            dist = randf(300.0f, 500.0f);
            tx = dist * ekiSinf2(rad) + ep->matrix[0][0][3];
            tz = -(dist * ekiCosf2(rad) - ep->matrix[0][2][3]);
            f = randf(5.0f, 6.0f);
            ty = randf(0.2f, 0.3f);
            if (voicePlayed == 0) {
                v = ekiVec3(ep->matrix[0][0][3], ep->matrix[0][1][3], ep->matrix[0][2][3], &vec3_eki_00008920);
                psndSFXOn_3D((const char*)0x6C3, &v);
                voicePlayed = 1;
            }
            moamoa_jump(ep, pose, 0x31, 0, tx, -100.0f, tz, f, ty);
            break;
        case 0x30:
            if (--pose->field_0x18 > 0) {
                pose->position.x += pose->field_0x14;
                pose->position.y += pose->field_0x1C;
                pose->position.z += pose->field_0x20;
                ep->matrix[0][0][3] += pose->position.x;
                ep->matrix[0][1][3] += pose->position.y;
                ep->matrix[0][2][3] += pose->position.z;
            } else {
                pose->field_0x02 = 0x31;
            }
            break;
        case 0x31:
            if ((wp->field_0x00 & 0x1000) && (wp->field_0x00 & 0x2000)) {
                pose->field_0x02 = 0x3E;
            } else if (wp->field_0x38 < 0xFA) {
                PSMTXIdentity(*ep->matrix);
                ep->matrix[0][0][3] = MOA_TARGET(i).x;
                ep->matrix[0][1][3] = MOA_TARGET(i).y;
                ep->matrix[0][2][3] = MOA_TARGET(i).z;
                pose->flags = 0;
                pose->field_0x02 = 0;
                pose->field_0x04 = 0;
            } else {
                pose->field_0x02 = 0x3E;
            }
            break;
        case 0x19:
            if (pose->field_0x04 < 0x1E) {
                Mtx rot;

                PSMTXRotRad(rot, 'y', 0.20944f);
                PSMTXConcat(*ep->matrix, rot, *ep->matrix);
                ep->flags &= ~0x2;
                pose->field_0x04++;
            } else {
                ep->flags |= 0x2;
                pose->field_0x04 =
                    (100.0f - distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], MOA_TARGET(i).x, MOA_TARGET(i).z)) * 0.5f;
                pose->field_0x02++;
            }
            break;
        case 0x1A:
            if (--pose->field_0x04 < 0) {
                pose->field_0x02++;
            }
            break;
        case 0x1B:
            if (fabs(ep->matrix[0][0][3]) < 300.0 && fabs(ep->matrix[0][2][3]) < 300.0) {
                rad = 6.2831855f * (angleABf(MOA_TARGET(i).x, MOA_TARGET(i).z, ep->matrix[0][0][3], ep->matrix[0][2][3])
                                    + randf(-45.0f, 45.0f))
                      / 360.0f;
                tx = 15.0f * ekiSinf2(rad) + ep->matrix[0][0][3];
                tz = -(15.0f * ekiCosf2(rad) - ep->matrix[0][2][3]);
                moamoa_jump(ep, pose, pose->field_0x02, 1, tx, 0.0f, tz, 3.0f * randf(1.0f, 1.0f), randf(1.0f, 1.0f));
            } else {
                pose->field_0x02 = 0x3E;
            }
            break;
        case 0x3E:
            pose->flags |= 0x20;
            ep->flags &= ~0x1;
            ep->flags |= 0x4;
            break;
        case 0x3F:
            rad = 6.2831855f * randf(225.0f, 315.0f) / 360.0f;
            f = randf(2.0f, 3.0f);
            pose->field_0x04 = irand(i / 2);
            pose->position.x = f * ekiSinf2(rad);
            pose->position.y = randf(1.0f, 2.0f);
            pose->position.z = f * -ekiCosf2(rad);
            pose->field_0x02++;
            break;
        case 0x40:
            if (--pose->field_0x04 < 0) {
                pose->field_0x02++;
            }
            break;
        case 0x41: {
            Mtx rot;

            pose->position.x *= 1.1f;
            pose->position.y *= 1.02f;
            pose->position.z *= 1.1f;
            ep->matrix[0][0][3] += pose->position.x;
            ep->matrix[0][1][3] += pose->position.y;
            ep->matrix[0][2][3] += pose->position.z;
            PSMTXRotRad(rot, 'y', 0.20944f);
            PSMTXConcat(*ep->matrix, rot, *ep->matrix);
            ep->flags &= ~0x2;
            if (ep->matrix[0][0][3] < -1000.0f) {
                pose->field_0x02 = 0x3E;
            }
            break;
        }
        }
        if (!(pose->flags & 0x98)) {
            if (ep->matrix[0][0][3] > 280.0f) {
                ep->matrix[0][0][3] = 280.0f;
            }
            if (ep->matrix[0][0][3] < -120.0f) {
                ep->matrix[0][0][3] = -120.0f;
            }
            if (ep->matrix[0][2][3] > 70.0f) {
                ep->matrix[0][2][3] = 70.0f;
            }
            if (ep->matrix[0][2][3] < -70.0f) {
                ep->matrix[0][2][3] = -70.0f;
            }
        }
        pose->field_0x28.x = ep->matrix[0][0][3];
        pose->field_0x28.y = ep->matrix[0][2][3];
    }
    moamoa_pose_main();
    if (wp->field_0x00 & 0x10) {
        mario->riderDelta.x = 0.0f;
        mario->riderDelta.y = 0.0f;
        mario->riderDelta.z = 0.0f;
        return;
    }
    {
        Vec a;
        Vec b;

        a = ekiVec3(mario->riderDelta.x, mario->riderDelta.y, mario->riderDelta.z, &vec3_eki_0000892c);
        b = a;
        if (PSVECMag(&b) > 2.0f) {
            PSVECNormalize(&b, &b);
            mario->riderDelta.x = 2.0f * b.x;
            mario->riderDelta.y = 2.0f * b.y;
            mario->riderDelta.z = 2.0f * b.z;
        }
    }
}

#pragma opt_propagation reset
#pragma inline_max_size reset
#pragma inline_max_total_size reset

// dont_inline keeps moamoa_pose_init a real `bl` from ext_moamoa_init as in retail.
#pragma dont_inline on
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
}
#pragma dont_inline reset

#pragma opt_propagation off // stop MWCC rematerialising the ekiSinf/ekiCosf coefficients per branch
// NONMATCHING (79.81%): `#pragma opt_propagation off` above stops MWCC rematerialising the ekiSinf/ekiCosf
// coefficients per branch, which recovered most of the gap (72.30 -> 79.81). What is left is register
// pressure: retail saves f20-f31 and r17-r31 (frame 0x140), ours f21-f31 and r19-r31 (frame 0x120).
static void ext_moamoa_init(void) {
    ExtPose* ep;
    MoamoaPose* pose;
    s32 i;
    f32 angle;
    f32 radius;
    f32 hitX;
    f32 hitY;
    f32 hitZ;
    f32 distance;
    f32 hitNX;
    f32 hitNY;
    f32 hitNZ;

    wp = _mapAlloc(0x870);
    memset(wp, 0, 0x870);
    wp->field_0x14 = vec3_eki_00008884;
    wp->field_0x20 = vec3_eki_00008890;
    wp->field_0x2C = vec3_eki_0000889c;
    wp->poseNum = extGetPoseNum();
    wp->poses = _mapAlloc(wp->poseNum * 0x50);
    memset(wp->poses, 0, wp->poseNum * 0x50);
    moamoa_pose_init();
    ep = extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++, ep++, pose++) {
        pose->flags = 0;
        pose->field_0x02 = 0;
        pose->field_0x04 = 0;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        angle = randf(0.0f, 6.2831855f);
        radius = randf(0.0f, 100.0f);
        ep->flags |= 0x13;
        PSMTXIdentity(*ep->matrix);
        ep->matrix[0][0][3] = radius * ekiSinf(angle) + wp->field_0x14.x;
        ep->matrix[0][1][3] = 0.0f;
        ep->matrix[0][0][3] = wp->field_0x14.z - radius * ekiCosf(angle);  // retail stores z here too, over x
        hitX = 0.0f;
        hitY = 0.0f;
        hitZ = 0.0f;
        distance = 1000000.0f;
        if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, ep->matrix[0][0][3],
                           ep->matrix[0][1][3] + 100.0f, ep->matrix[0][2][3], 0.0f, -1.0f, 0.0f)
            != NULL) {
            ep->matrix[0][1][3] = hitY;
        }
    }
    for (i = 0; i < 600; i++) {
        ext_moamoa_main();
    }
}

#pragma opt_propagation reset
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

static s32 item_tbl[2] = {
    26, -1,
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

#pragma opt_propagation off // stop MWCC rematerialising the ekiSinf/ekiCosf coefficients per branch
// NONMATCHING (68.14%): dropping the cached `Mtx* mtx` local fixed the r29/r30 argument-register
// swap and `#pragma opt_propagation off` recovered the sin/cos coefficient gap (58.50 -> 68.14).
// Left: Vec temporaries make the frame 0xc0 vs retail 0xf0, plus a few FPR-colouring diffs.
static s32 moamoa_hit_check(ExtPose* ep, MoamoaPose* pose) {
    MarioWork* mario = marioGetPtr();
    Vec va;
    Vec vc;
    f32 dist;
    f32 thresh;
    f32 angle;
    f32 diff;
    f32 rad;
    f32 t;

    if (mario->position.y > ep->matrix[0][1][3] + 20.0f) {
        return 0;
    }
    if (ep->matrix[0][1][3] > mario->position.y + mario->wPlayerCollisionBox.y) {
        return 0;
    }
    dist = distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], mario->position.x, mario->position.z);
    if (dist < (20.0f + mario->wPlayerCollisionBox.x) * 0.5f) {
        if (mario->position.y > ep->matrix[0][1][3] + 10.0f && (mario->flags & MARIO_FLAG_IS_FALLING)
            && !(mario->flags & MARIO_FLAG_IS_JUMPING) && wp->field_0x558 < 2) {
            if (mario->currMotionId != MARIO_MOTION_ROLL && mario->currMotionId != MARIO_MOTION_YOSHI) {
                marioChgMot(MARIO_MOTION_JUMP_SW);
            }
            wp->field_0x558++;
            return 1;
        }
    }
    if (!(mario->flags & (MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING))) {
        wp->field_0x558 = 0;
    }
    if (mario->currMotionId == MARIO_MOTION_HAMMER && wp->field_0x40 > 10) {
        CameraEntry* cam = camGetPtr(CAMERA_3D);
        angle = reviseAngle((mario->unk1A8 - 90.0f) + cam->viewYaw);
        rad = angle * 6.2831855f / 360.0f;
        t = 35.0f * ekiSinf(rad) + mario->position.x;
        diff = mario->position.z - 35.0f * ekiCosf(rad);
        if (distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], t, diff) < 35.0f) {
            return 2;
        }
    }
    if (mario->currMotionId == MARIO_MOTION_HAMMER_2 && (s32)mario->currSubMotionId == 0x14) {
        va = ekiVec3(ep->matrix[0][0][3], ep->matrix[0][1][3], ep->matrix[0][2][3], &vec3_eki_000088b4);
        if (PSVECDistance(&mario->position, &va) < 65.0f) {
            return 2;
        }
    }
    if (wp->field_0x55C >= 0.0f) {
        va = ekiVec3(mario->position.x, ep->matrix[0][1][3], mario->position.z, &vec3_eki_000088c0);
        if (PSVECDistance(&wp->field_0x564, &va) < 30.0f) {
            return 3;
        }
    }
    if (nokonokoGetStatus(partyGetPtr(0)) == 2) {
        va = ekiVec3(mario->position.x, ep->matrix[0][1][3], mario->position.z, &vec3_eki_000088cc);
        if (PSVECDistance(&partyGetPtr(0)->position, &va) < 30.0f) {
            return 2;
        }
    }
    if (partyGetPtr(0)->currentMemberId == 3 && partyGetPtr(0)->useMotionId == 0x46) {
        va = ekiVec3(mario->position.x, ep->matrix[0][1][3], mario->position.z, &vec3_eki_000088d8);
        if (PSVECDistance(&partyGetPtr(0)->position, &va) < 50.0f) {
            return 4;
        }
    }
    va = ekiVec3(mario->position.x, ep->matrix[0][1][3], mario->position.z, &vec3_eki_000088e4);
    vc = ekiVec3(ep->matrix[0][0][3], ep->matrix[0][1][3], ep->matrix[0][2][3], &vec3_eki_000088f0);
    thresh = (20.0f + mario->wPlayerCollisionBox.x) * 0.5f;
    if (PSVECDistance(&va, &vc) >= thresh) {
        return 0;
    }
    ep->matrix[0][0][3] = pose->field_0x28.x;
    ep->matrix[0][2][3] = pose->field_0x28.y;
    vc = ekiVec3(ep->matrix[0][0][3], ep->matrix[0][1][3], ep->matrix[0][2][3], &vec3_eki_000088f0);
    dist = PSVECDistance(&va, &vc);
    if (dist >= thresh) {
        return 0;
    }
    angle = angleABf(mario->position.x, mario->position.z, ep->matrix[0][0][3], ep->matrix[0][2][3]);
    diff = compAngle(angle, mario->directionView);
    if (__fabsf(diff) < 40.0f) {
        rad = mario->directionView * 6.2831855f / 360.0f;
        t = thresh - dist;
        mario->riderDelta.x = -(t * ekiSinf(rad));
        mario->riderDelta.z = t * ekiCosf(rad);
    } else {
        rad = angle * 6.2831855f / 360.0f;
        mario->riderDelta.x = (ep->matrix[0][0][3] - thresh * ekiSinf(rad)) - mario->position.x;
        mario->riderDelta.z = (thresh * ekiCosf(rad) + ep->matrix[0][2][3]) - mario->position.z;
    }
    return 0;
}

#pragma opt_propagation reset

#pragma dont_inline on
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

    if (c != 0) {
        hitZ = 0.0f;
        hitY = 0.0f;
        hitX = 0.0f;
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
    pose->position = pos;
    pose->field_0x1C = f4;
    pose->field_0x20 = f5;
    ty = pose->position.y - ep->matrix[0][1][3];
    dist = distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->position.x, pose->position.z);
    angle = angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->position.x, pose->position.z);
    pose->field_0x18 = (s16)(dist / pose->field_0x1C);
    pose->field_0x14 = pose->field_0x20 * (f32)pose->field_0x18 / 2.0f + ty / (f32)pose->field_0x18;
    pose->field_0x02 = 3;
    pose->field_0x24 = b;
    ep->field_0x4 = angle + 180.0f;
    pose->flags |= 0x2;
    ep->flags |= 0x10;
    ep->flags &= ~0x4;
}

#pragma dont_inline reset

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

static s32 evt_moamoa_fusion_debug(void) {
    s32 i;
    ExtPose* ep = extGetPosePtr();
    MoamoaPose* pose = wp->poses;

    for (i = 0; i < wp->poseNum; i++, ep++, pose++) {
        ep->flags |= 4;
        pose->flags |= 0x400;
        pose->flags |= 0x100;
        pose->flags |= 0x200;
        pose->field_0x02 = 0x3D;
        pose->flags |= 0x800;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
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

static USER_FUNC(evt_moamoa_kind_onoff) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 mode = evtGetValue(event, args[1]);
    s32 i;
    MoamoaPose* pose;

    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++, pose++) {
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
// clang-format on
