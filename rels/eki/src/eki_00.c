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
typedef struct MoamoaGroup {
    char name[0x10];  // 0x00
    char anim[8];     // 0x10
    s16 enabled;      // 0x18, 0 for the J-prefixed animations
    f32 cycleFrames;  // 0x1C, frames for one pass through the group's poses
    s32 firstPose;    // 0x20
    s32 numPoses;     // 0x24
} MoamoaGroup;

typedef struct MoamoaPose {
    u16 flags;          // 0x00
    s16 state;          // 0x02, switch key in ext_moamoa_main
    s16 stateTimer;     // 0x04, frames left in the current state
    u16 field_0x06;     // 0x06
    Vec target;         // 0x08, moamoa_jump's landing target; a per-frame velocity in the fly-off states
                        //       (0x30, 0x3F on), with 0x14/0x1C/0x20 as its per-frame change
    f32 field_0x14;     // 0x14
    s16 frames;         // 0x18, frames left in the current jump or glide
    u8 pad_0x1A[2];     // 0x1A
    f32 field_0x1C;     // 0x1C
    f32 field_0x20;     // 0x20
    s16 queuedState;    // 0x24, state to enter when the current jump lands
    u8 pad_0x26[2];     // 0x26
    f32 savedX;         // 0x28, position saved when a glide starts
    f32 savedZ;         // 0x2C
    Vec offset;         // 0x30, unused here (rsh_06 adds it to the pose)
    s16 groupIndex;     // 0x3C, index into MoamoaWork.groups
    u16 field_0x3E;     // 0x3E
    OSTime poseTime;    // 0x40
    u32 poseId;         // 0x48
    u8 kind;            // 0x4C, index into group_tbl
    u8 pad_0x4D[3];     // 0x4D
} MoamoaPose;

typedef struct MoamoaWork {
    u16 flags;         // 0x00
    u16 pad_0x02;      // 0x02
    s32 poseNum;       // 0x04
    MoamoaPose* poses; // 0x08
    u32 field_0x0C;    // 0x0C
    u32 field_0x10;    // 0x10
    Vec target0;       // 0x14, gathering points; moamoa i heads for target0 + i % 3 (MOA_TARGET).
                       //       Separate Vecs: as Vec[3] the build no longer matches
    Vec target1;       // 0x20
    Vec target2;       // 0x2C
    s16 hitCount;      // 0x38
    u16 nearHits0;     // 0x3A, hits near target0-2 (as in rsh_05); set to 100 by evt_moamoa_target_off
    u16 nearHits1;     // 0x3C
    u16 nearHits2;     // 0x3E
    s16 hammerFrames;  // 0x40, frames Mario has been in MARIO_MOTION_HAMMER
    s16 idleFrames;    // 0x42, frames Mario has stood still (capped at 1000)
    EventEntry* escapeEvt; // 0x44
    u8 pad_0x48[4];    // 0x48
    s32 groupCount;    // 0x4C
    MoamoaGroup groups[32];// 0x50
    f32 angleToMario;  // 0x550, angle from Mario to the moamoa in the current pose
    f32 distToMario;   // 0x554, distance from Mario to the current pose
    s32 bounceCount;   // 0x558, head bounces on Mario (max 2), reset while mario->flags & 0x30000 is clear
    f32 breathDir;     // 0x55C, cloud breath direction (-1 when not in use)
    f32 breathDist;    // 0x560, cloud breath distance
    Vec breathPos;     // 0x564
} MoamoaWork;

typedef struct MoamoaGroupDef {
    const char* name;  // 0x00
    const char* anim;  // 0x04
    f32 cycleFrames;   // 0x08
} MoamoaGroupDef;

static MoamoaWork* wp;

static inline f32 ekiSinf(f32 rad) {
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

static inline f32 ekiCosf(f32 rad) {
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

static inline Vec ekiVec3(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}


#pragma force_active on
static void ext_moamoa_dispent(void);
static void ext_moamoa_main(void);
static s32 evt_moamoa_escape[0xBC / 4];
static s32 moamoa_hit_check(ExtPose* ep, MoamoaPose* pose);
static void moamoa_jump(ExtPose* ep, MoamoaPose* pose, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, s32 b, s32 c);
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

static MoamoaGroupDef ext_moamoa_data[47] = {
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

static Mtx mt = {
    {8.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 8.0f, 0.0f, 0.5f},
    {0.0f, 0.0f, 8.0f, 0.0f},
};

// Non-const as in rsh_06: ext_moamoa_init copies all three into wp, and as `const` MWCC knows the values
// and reorders the copies.
__declspec(section ".rodata") Vec vec3_eki_00008884 = {0.0f, 0.0f, 10.0f};
__declspec(section ".rodata") Vec vec3_eki_00008890 = {0.0f, 0.0f, 10.0f};
__declspec(section ".rodata") Vec vec3_eki_0000889c = {0.0f, 0.0f, 10.0f};
const Vec vec3_eki_000088a8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088b4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088c0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088cc = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088d8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088e4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088f0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_000088fc = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_eki_00008908 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_00008914 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_00008920 = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_0000892c = {0.0f, 0.0f, 0.0f};

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

// dont_inline keeps moamoa_pose_init a real `bl` from ext_moamoa_init as in retail.
#pragma dont_inline on
static void moamoa_pose_init(void) {
    char name[0x20];
    char anim[0x20];
    MoamoaGroupDef* entry = (MoamoaGroupDef*)ext_moamoa_data;
    s32 i = 0;
    s32 start;

    strcpy(name, "");
    strcpy(anim, "");
    wp->groupCount = -1;
    do {
        if (strcmp(entry->name, name) != 0) {
            strcpy(name, entry->name);
            strcpy(anim, "");
        }
        if (strcmp(entry->anim, anim) != 0) {
            strcpy(anim, entry->anim);
            start = i;
            wp->groupCount++;
        }
        strcpy(wp->groups[wp->groupCount].name, name);
        strcpy(wp->groups[wp->groupCount].anim, anim);
        wp->groups[wp->groupCount].firstPose = start;
        wp->groups[wp->groupCount].numPoses = i - start + 1;
        wp->groups[wp->groupCount].cycleFrames = entry->cycleFrames;
        wp->groups[wp->groupCount].enabled = 1;
#ifdef MUST_MATCH
        // The original pools "J" as a 4-byte object at the end of .rodata; the two extra NULs pad it.
        if (strncmp(anim, "J\0\0", 1) == 0) {
#else
        if (strncmp(anim, "J", 1) == 0) {
#endif
            wp->groups[wp->groupCount].enabled = 0;
        }
        i++;
        entry++;
    } while (entry->name != NULL);
    wp->groupCount++;
}
#pragma dont_inline reset

// The original inlines every ekiSinf/ekiCosf/randf/pose-change here: raise the inliner's per-caller limits.
static void moamoa_pose_main(void) {
    s32 i;
    ExtPose* ep;
    MoamoaPose* pose;

    ep = extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        MoamoaGroup* npc = &wp->groups[pose->groupIndex];
        f32 ratio = (f32)((u64)(gp->renderTime - pose->poseTime) / (u64)((OS_BUS_CLOCK >> 2) / 1000)) * 60.0f
                    / 1000.0f / npc->cycleFrames;

        if (ratio >= 1.0f) {
            pose->poseTime = gp->renderTime;
            ratio = 0.0f;
        }
        pose++;
        ep->texId = npc->firstPose + (s32)((npc->numPoses - 1) * ratio);
        ep++;
    }
}

static void moamoa_pose_change(MoamoaPose* pose, const char* name, const char* anim) {
    MoamoaGroup* npc = wp->groups;
    s32 i;

    if (pose->flags & 0x800) {
        name = group_tbl[pose->kind][0];
        if (pose->kind >= 2) {
            anim = group_tbl[pose->kind][1];
        }
        if (pose->kind == 0xa) {
            anim = "S_2";
        }
    }
    for (i = 0; i < wp->groupCount; i++, npc++) {
        if (strcmp(npc->name, name) == 0 && strcmp(npc->anim, anim) == 0) {
            break;
        }
    }
    if (i < wp->groupCount && pose->groupIndex != i) {
        if (!(pose->flags & 0x4) || strncmp(anim, "D", 1) == 0) {
            if (!(pose->flags & 0x10) || strncmp(anim, "R", 1) == 0) {
                pose->groupIndex = i;
                pose->poseTime = gp->renderTime;
            }
        }
    }
}

static USER_FUNC(evt_moamoa_flag_onoff) {
    s32* args = event->args;
    s32 on;
    s32 mask;

    on = evtGetValue(event, args[0]);
    mask = evtGetValue(event, args[1]);
    if (on) {
        wp->flags |= mask;
    } else {
        wp->flags &= ~mask;
    }
    return EVT_RETURN_DONE;
}
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

static USER_FUNC(evt_moamoa_exit) {
    extReset();
    return EVT_RETURN_DONE;
}

static s32 evt_moamoa_ry_reset(void) {
    ExtPose* pose = extGetPosePtr();
    s32 i;

    for (i = 0; i < wp->poseNum; i++) {
        pose->yaw = 0.0f;
        pose->targetYaw = 0.0f;
        pose++;
    }
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

static inline GXColor makeColor(u8 r, u8 g, u8 b, u8 a) {
    GXColor color = {0, 0, 0, 0};

    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

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
            pose->queuedState = 0xE;
            pose->state = 0xE;
            pose->stateTimer = (i < 100) ? i % 100 : i % 60 + 100;
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
        ep->flags |= EXT_POSE_NO_SHADOW;
        pose->flags |= 0x400;
        pose->state = 0x3D;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
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
                if (pose->kind == 1) {
                    pose->flags |= 0x800;
                }
            } else {
                pose->flags |= 0x800;
            }
        } else {
            if (mode == 0) {
                if (pose->kind != 1) {
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

static USER_FUNC(evt_moamoa_blow) {
    s32 i;
    MoamoaPose* pose;

    evtGetValue(event, event->args[0]);
    extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++) {
        pose->flags &= ~0x400;
        pose->state = 0x3F;
        pose++;
    }
    return EVT_RETURN_DONE;
}

static s32 evt_moamoa_fusion_debug(void) {
    s32 i;
    ExtPose* ep = extGetPosePtr();
    MoamoaPose* pose = wp->poses;

    for (i = 0; i < wp->poseNum; i++, ep++, pose++) {
        ep->flags |= EXT_POSE_NO_SHADOW;
        pose->flags |= 0x400;
        pose->flags |= 0x100;
        pose->flags |= 0x200;
        pose->state = 0x3D;
        pose->flags |= 0x800;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_moamoa_target_off) {
    s32 index = evtGetValue(event, event->args[0]);
    Vec* vecs = &wp->target0;

    vecs[index] = vecs[1 - index];
    (&wp->nearHits0)[index] = 0x64;
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

static inline f32 randRange(f32 min, f32 max) {
    return max - min;
}

static f32 randf(f32 min, f32 max) {
    f32 t = (f32)rand() / 32767.0f;

    return t * (max - min) + min;
}



// ext_moamoa_init needs its own copy of the sin/cos polynomials: the shared ekiSinf/ekiCosf above are
// consumed by ext_moamoa_main and moamoa_hit_check, and changing them drops those scores. The `f2`
// scratch in the last sine branch plus `#pragma opt_lifetimes off` at the call site are what give
// retail's in-loop seed accumulators (sine in f2, cosine in f3).
static inline f32 initSinf(f32 rad) {
    const f32 pi = 3.1415927f;
    const f32 halfpi = 1.5707964f;
    const f32 threehalves = 4.712389f;
    f32 f0, f1, f2, sinv = 0.00761f;
    f32 c1 = 0.16605f;
    f32 c2 = 1.0f;

    if (rad <= pi) {
        if (rad <= halfpi) {
            sinv = sinv * (rad * rad) - c1;
            sinv = sinv * (rad * rad) + c2;
            sinv = sinv * rad;
        } else {
            f1 = halfpi - (rad - halfpi);
            f0 = f1 * f1;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * f1;
        }
    } else if (rad < threehalves) {
        f1 = rad - pi;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    } else {
        f1 = halfpi - (rad - threehalves);
        f2 = f1 * f1;
        sinv = sinv * f2 - c1;
        sinv = sinv * f2 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 initCosf(f32 rad) {
    const f32 pi = 3.1415927f;
    const f32 halfpi = 1.5707964f;
    const f32 threehalves = 4.712389f;
    f32 f0, cosv = 0.03705f;
    f32 c1 = 0.4967f;
    f32 c2 = 1.0f;

    if (rad <= pi) {
        if (rad <= halfpi) {
            cosv = cosv * (rad * rad) - c1;
            cosv = cosv * (rad * rad) + c2;
        } else {
            f0 = halfpi - (rad - halfpi);
            f0 = f0 * f0;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < threehalves) {
        f0 = (rad - pi) * (rad - pi);
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = halfpi - (rad - threehalves);
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

#pragma opt_lifetimes off // keeps the inlined sin/cos seeds in f2/f3 like retail instead of reusing f1
#pragma opt_propagation off // stop MWCC rematerialising the sin/cos coefficients per branch
static void ext_moamoa_init(void) {
    s32 i;
    ExtPose* ep;
    MoamoaPose* pose;
    s32 j;
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
    wp->target0 = vec3_eki_00008884;
    wp->target1 = vec3_eki_00008890;
    wp->target2 = vec3_eki_0000889c;
    wp->poseNum = extGetPoseNum();
    wp->poses = _mapAlloc(wp->poseNum * 0x50);
    memset(wp->poses, 0, wp->poseNum * 0x50);
    moamoa_pose_init();
    ep = extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->poseNum; i++, ep++, pose++) {
        pose->flags = 0;
        pose->state = 0;
        pose->stateTimer = 0;
        moamoa_pose_change(pose, "c_n_moa_a", "S_1");
        angle = randf(0.0f, 6.2831855f);
        radius = randf(0.0f, 100.0f);
        ep->flags |= 0x13;
        PSMTXIdentity(*ep->matrix);
        ep->matrix[0][0][3] = radius * initSinf(angle) + wp->target0.x;
        ep->matrix[0][1][3] = 0.0f;
        ep->matrix[0][0][3] = wp->target0.z - radius * initCosf(angle);  // retail stores z here too, over x
        hitZ = 0.0f;
        hitY = 0.0f;
        hitX = 0.0f;
        distance = 1000000.0f;
        if (hitCheckFilter(NULL, ep->matrix[0][0][3], ep->matrix[0][1][3] + 100.0f, ep->matrix[0][2][3], 0.0f, -1.0f, 0.0f, &hitX, &hitY,
                           &hitZ, &distance, &hitNX, &hitNY, &hitNZ)
            != NULL) {
            ep->matrix[0][1][3] = hitY;
        }
    }
    for (j = 0; j < 600; j++) {
        ext_moamoa_main();
    }
}

#pragma opt_lifetimes reset
#pragma opt_propagation reset

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

static void ext_shadow_disp(CameraId cameraId, void* param) {
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

// The original calls moamoa_jump (a real bl) from ext_moamoa_main; the raised inline limits would inline it.
#pragma dont_inline on
static void moamoa_jump(ExtPose* ep, MoamoaPose* pose, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, s32 b, s32 c) {
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
        if (hitCheckFilter(NULL, f1, f2 + 100.0f, f3, 0.0f, -1.0f, 0.0f, &hitX, &hitY, &hitZ, &distance,
                           &hitNX, &hitNY, &hitNZ)
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
    pose->target = pos;
    pose->field_0x1C = f4;
    pose->field_0x20 = f5;
    ty = pose->target.y - ep->matrix[0][1][3];
    dist = distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->target.x, pose->target.z);
    angle = angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->target.x, pose->target.z);
    pose->frames = (s16)(dist / pose->field_0x1C);
    pose->field_0x14 = pose->field_0x20 * (f32)pose->frames / 2.0f + ty / (f32)pose->frames;
    pose->state = 3;
    pose->queuedState = b;
    ep->heading = angle + 180.0f;
    pose->flags |= 0x2;
    ep->flags |= EXT_POSE_SHADOW_AT_GROUND;
    ep->flags &= ~EXT_POSE_NO_SHADOW;
}

#pragma dont_inline reset



// Inline sine/cosine approximations (the polynomials shared by the NPC blur path).
static inline f32 moaSin(f32 angle) {
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

static inline f32 moaCos(f32 angle) {
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
static inline Vec moaVec(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

#ifdef MUST_MATCH
// Match-only: the volatile copy stops MWCC hoisting the base vector's lane loads above the two
// `pose->matrix` stores in the pvec rebuild (retail keeps each 12-byte copy serialized); the plain
// copy is `moaVec`.
static inline Vec moaVecV(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *(volatile Vec*)base;

    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
#endif

#pragma opt_lifetimes off // keeps the sin/cos coefficients in the target's FPRs
#pragma opt_propagation off // hoist the sin/cos coefficient loads out of the inline branches
static s32 moamoa_hit_check(ExtPose* ep, MoamoaPose* pose) {
    MarioWork* mario = marioGetPtr();
    Vec v65, v55c, vnoko, vparty, mpos, pvec;
    PartyEntry* party;
    f32 f29, f30, f31;

    if (mario->position.y > 20.0f + (*ep->matrix)[1][3])
        return 0;
    if ((*ep->matrix)[1][3] > mario->position.y + mario->wPlayerCollisionBox.y)
        return 0;

    {
        f32 d = distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], mario->position.x, mario->position.z);

        // (x + 20) / 2 folds to the same * 0.5 but colours the operands like retail.
        if (d < (mario->wPlayerCollisionBox.x + 20.0f) / 2.0f) {
            if (mario->position.y > 10.0f + (*ep->matrix)[1][3] && (mario->flags & 0x20000) &&
                !(mario->flags & 0x10000) && wp->bounceCount < 2) {
                if (mario->currMotionId != 0x16 && mario->currMotionId != 0x1A)
                    marioChgMot(5);
                wp->bounceCount++;
                return 1;
            }
        }
    }

    if ((mario->flags & 0x30000) == 0)
        wp->bounceCount = 0;

    if (mario->currMotionId == 0x12 && wp->hammerFrames > 10) {
        f32 ang;
        f32 rad;
        f32 s;
        f32 c;
        f32 x;
        f32 z;

        ang = reviseAngle(mario->unk1A8 - 90.0f + camGetPtr(4)->viewYaw);
        rad = 6.2831855f * ang / 360.0f;
        s = moaSin(rad);
        x = 35.0f * s + mario->position.x;
        c = moaCos(rad);
        z = mario->position.z - 35.0f * c;
        if (distABf((*ep->matrix)[0][3], (*ep->matrix)[2][3], x, z) < 35.0f)
            return 2;
    }

    if (mario->currMotionId == 0x13 && mario->currSubMotionId == 0x14) {
        v65 = moaVec((*ep->matrix)[0][3], (*ep->matrix)[1][3], (*ep->matrix)[2][3],
                             &vec3_eki_000088b4);

        if (PSVECDistance(&mario->position, &v65) < 65.0f)
            return 2;
    }

    if (wp->breathDir >= 0.0f) {
        v55c = moaVec((*ep->matrix)[0][3], (*ep->matrix)[1][3], (*ep->matrix)[2][3],
                              &vec3_eki_000088c0);

        if (PSVECDistance(&wp->breathPos, &v55c) < 30.0f)
            return 3;
    }

    party = partyGetPtr(0);
    if (nokonokoGetStatus(party) == 2) {
        vnoko = moaVec((*ep->matrix)[0][3], (*ep->matrix)[1][3], (*ep->matrix)[2][3],
                               &vec3_eki_000088cc);

        if (PSVECDistance(&partyGetPtr(0)->position, &vnoko) < 30.0f)
            return 2;
    }

    party = partyGetPtr(0);
    if (party->currentMemberId == PARTY_MEMBER_BOBBERY && partyGetPtr(0)->useMotionId == 0x46) {
        vparty = moaVec((*ep->matrix)[0][3], (*ep->matrix)[1][3], (*ep->matrix)[2][3],
                                &vec3_eki_000088d8);

        if (PSVECDistance(&partyGetPtr(0)->position, &vparty) < 50.0f)
            return 4;
    }

    {
        mpos = moaVec(mario->position.x, (*ep->matrix)[1][3], mario->position.z,
                              &vec3_eki_000088e4);
        {
            // Matching temp: giving the pvec's x lane a name pins the mpos/pvec lane registers
            // (retail pose.x -> f3, pose.z -> f1; without it MWCC loads them the other way round).
            f32 px = (*ep->matrix)[0][3];
            pvec = moaVec(px, (*ep->matrix)[1][3], (*ep->matrix)[2][3], &vec3_eki_000088f0);
        }

        f31 = (mario->wPlayerCollisionBox.x + 20.0f) / 2.0f;
        if (PSVECDistance(&mpos, &pvec) < f31) {
#ifdef MUST_MATCH
            // Match-only volatile accesses: they keep the pvec base copy below the two matrix
            // stores, the order the target has.
            *(volatile f32*)&(*ep->matrix)[0][3] = pose->savedX;
            *(volatile f32*)&(*ep->matrix)[2][3] = pose->savedZ;
            {
                Mtx* pm = *(Mtx* volatile*)&ep->matrix;

                pvec = moaVecV((*pm)[0][3], (*pm)[1][3], (*pm)[2][3], &vec3_eki_000088fc);

                f29 = PSVECDistance(&mpos, &pvec);
            }
#else
            (*ep->matrix)[0][3] = pose->savedX;
            (*ep->matrix)[2][3] = pose->savedZ;
            {
                pvec = moaVec((*ep->matrix)[0][3], (*ep->matrix)[1][3], (*ep->matrix)[2][3],
                                  &vec3_eki_000088fc);

                f29 = PSVECDistance(&mpos, &pvec);
            }
#endif
            if (f29 < f31) {
                f30 = angleABf(mario->position.x, mario->position.z, (*ep->matrix)[0][3],
                               (*ep->matrix)[2][3]);
                if (__fabsf(compAngle(f30, mario->directionView)) < 40.0f) {
                    // Matching shapes: inlining the sine argument (no `rad` local) lets the reduced
                    // angle coalesce into the argument FPR, and naming the box delta once fixes the
                    // operand order of the two `d * s` / `d * c` fmuls.
                    f32 s, c, d;

                    s = moaSin(6.2831855f * mario->directionView / 360.0f);
                    d = f31 - f29;
                    mario->riderDelta.x = -(d * s);
                    c = moaCos(6.2831855f * mario->directionView / 360.0f);
                    mario->riderDelta.z = d * c;
                } else {
                    f32 rad, s, c;

                    rad = 6.2831855f * f30 / 360.0f;
                    s = moaSin(rad);
                    mario->riderDelta.x = ((*ep->matrix)[0][3] - f31 * s) - mario->position.x;
                    c = moaCos(rad);
                    mario->riderDelta.z = ((*ep->matrix)[2][3] + f31 * c) - mario->position.z;
                }
            }
        }
    }

    return 0;
}

#pragma opt_lifetimes reset
#pragma opt_propagation reset

// Load the matrix inside the inline to retain retail's matrix/base register order.
static inline Vec moaSoundVec(ExtPose* ep, const Vec* base) {
    Vec v = *base;
    v.x = ep->matrix[0][0][3];
    v.y = ep->matrix[0][1][3];
    v.z = ep->matrix[0][2][3];
    return v;
}

// Keeping both sound vectors in inline scopes reproduces their stack-slot order.
static inline void moaPlayHit(ExtPose* ep) {
    Vec w = vec3_eki_00008920;
    w.x = ep->matrix[0][0][3];
    w.y = ep->matrix[0][1][3];
    w.z = ep->matrix[0][2][3];
    psndSFXOn_3D((const char*)0x6C3, &w);
}

static inline f32 ekiSinfM(f32 rad) {
    f32 sinv = 0.00761f;
    f32 c1 = 0.16605f, c2 = 1.0f;
    f32 f0;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        } else {
            rad = 1.5707964f - (rad - 1.5707964f);
            f0 = rad * rad;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        }
    } else if (rad < 4.712389f) {
        rad = rad - 3.1415927f;
        f0 = rad * rad;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * rad;
        sinv = -sinv;
    } else {
        rad = 1.5707964f - (rad - 4.712389f);
        f0 = rad * rad;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * rad;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 ekiCosfM(f32 rad) {
    f32 cosv = 0.03705f;
    f32 c1 = 0.4967f, c2 = 1.0f;
    f32 f0;

    if (rad <= 3.1415927f) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
        } else {
            rad = 1.5707964f - (rad - 1.5707964f);
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        rad = rad - 3.1415927f;
        f0 = rad * rad;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        rad = 1.5707964f - (rad - 4.712389f);
        f0 = rad * rad;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

#define MOA_TARGET(i) ((&wp->target0)[(i) % 3])
// Per-frame update of every moamoa: hit checks against Mario and his partner, the jump/scatter state machine
// (MoamoaPose.state), clamping into the room, then the animation frame update and the rider-delta limit.
// voiceSfx is a union under MUST_MATCH so it is not copy-propagated (as yuu_01 fall_rtn): that puts it in r24 and `handled` in r25.
#pragma inline_max_total_size(60000)
// Keep the sin/cos coefficient loads hoisted instead of rematerialised per branch.
#pragma opt_propagation off
// Keeps the inlined sin/cos argument in a callee-saved FPR instead of rematerialising it per branch.
#pragma opt_lifetimes off
static void ext_moamoa_main(void) {
    MarioWork* mario;
    PartyEntry* party;
    ExtPose* ep;
    MoamoaPose* pose;
    s32 i;
    s32 j;
    s32 handled;
#ifdef MUST_MATCH
    union { const char* p; u32 u; } voiceSfx;
#else
    const char* voiceSfx;
#endif
    s32 voicePlayed;
    f32 rad;
    f32 tx;
    f32 ty;
    f32 tz;
    f32 dist;
    f32 f;
    f32 r1;
    s32 res;
    Vec v;
    Vec b;

    mario = marioGetPtr();
    party = partyGetPtr(0);
    camGetPtr(CAMERA_3D);
    handled = 0;
    if (mario->currMotionId == MARIO_MOTION_HAMMER) {
        wp->hammerFrames++;
    } else {
        wp->hammerFrames = 0;
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
    if (party->currentMemberId == PARTY_MEMBER_FLURRIE && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        wp->breathDir = cloudGetBreathDir();
        wp->breathDist = cloudGetBreathDist();
        wp->breathPos.x =
            wp->breathDist * ekiSinfM(6.2831855f * wp->breathDir / 360.0f) + party->position.x;
        wp->breathPos.y = party->position.y;
        wp->breathPos.z = -(wp->breathDist * ekiCosfM(6.2831855f * wp->breathDir / 360.0f) - party->position.z);
    }
    if (wp->hitCount > 0xFA) {
        MoamoaPose* p;

        extGetPosePtr();
        p = wp->poses;
        for (j = 0; j < wp->poseNum; j++, p++) {
            if (!(p->flags & 0x4)) {
                if (p->flags & 0x10) {
                    break;
                }
                p->state = 0x19;
                p->stateTimer = 0;
                p->flags |= 0x10;
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
#ifdef MUST_MATCH
        voiceSfx.p = "SFX_STG6_MOAMOA_VOICE1";
#else
        voiceSfx = "SFX_STG6_MOAMOA_VOICE1";
#endif
        wp->angleToMario = angleABf(mario->position.x, mario->position.z, ep->matrix[0][0][3], ep->matrix[0][2][3]);
        wp->distToMario = distABf(mario->position.x, mario->position.z, ep->matrix[0][0][3], ep->matrix[0][2][3]);
        if ((wp->flags & 0x1) && !(wp->flags & 0x4) && marioKeyOffChk() == 0 && marioCtrlOffChk() == 0
            && handled == 0 && !(pose->flags & 0x154)) {
            res = moamoa_hit_check(ep, pose);
            if (res == 1) {
                pose->flags |= 0x4;
                pose->state = 0x24;
                wp->hitCount++;
                moamoa_pose_change(pose, "c_n_moa_a", "D_1");
                handled = 1;
            } else if (res == 2 || res == 4) {
                pose->flags |= 0xC;
                pose->state = 0x2F;
                wp->hitCount++;
                moamoa_pose_change(pose, "c_n_moa_a", "D_1");
                if (res != 4) {
                    handled = 1;
                }
            } else if (res == 3) {
                f = (wp->breathDir + wp->angleToMario) / 2.0f;
                pose->flags |= 0x84;
                pose->state = 0x30;
                pose->target = vec3_eki_00008908;
                dist = randf(0.1f, 0.2f);
                rad = 6.2831855f * f / 360.0f;
                pose->field_0x14 = dist * ekiSinfM(rad);
                pose->field_0x1C = randf(0.03f, 0.06f);
                pose->field_0x20 = dist * -ekiCosfM(rad);
                pose->frames = 0x78;
                wp->hitCount++;
                moamoa_pose_change(pose, "c_n_moa_a", "D_1");
                handled = 1;
            }
        }
        if (wp->flags & 0x1) {
            if (!(pose->flags & 0x155) && wp->distToMario < 50.0f) {
                if (pose->flags & 0x2) {
                    pose->queuedState = 1;
                } else {
                    pose->state = 1;
                }
            }
        }
        if ((wp->flags & 0x1) && !(pose->flags & 0x444) && (u32)i == gp->retraceCount % wp->poseNum
            && irand(1000) < 200) {
            v = moaSoundVec(ep, &vec3_eki_00008914);
            #ifdef MUST_MATCH
            psndSFXOn_3D((const char*)(u32)voiceSfx.p, &v);
#else
            psndSFXOn_3D(voiceSfx, &v);
#endif

        }
        switch (pose->state) {
        case 0x0:
            if (i % 50 < 0x14) {
                f32 dist, tx, ntx, ntz, ty, tz, rad, ar, br;
                f32 ang = randf(0.0f, 6.2831855f);
                f32 len = randf(0.0f, 50.0f);

                ntx = len * ekiSinfM(ang) + MOA_TARGET(i).x;
                ntz = -(len * ekiCosfM(ang) - MOA_TARGET(i).z);
                rad = 6.2831855f * angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], ntx, ntz) / 360.0f;
                tx = 15.0f * ekiSinfM(rad) + ep->matrix[0][0][3];
                ty = ep->matrix[0][1][3];
                tz = -(15.0f * ekiCosfM(rad) - ep->matrix[0][2][3]);
                ar = (f32)rand() / 32767.0f;
                br = (f32)rand() / 32767.0f;
                moamoa_jump(ep, pose, tx, ty, tz, ar * randRange(1.5f, 3.0f) + 1.5f, br * randRange(0.8f, 2.0f) + 0.8f, pose->state, 1);
                pose->flags |= 0x1;
            } else {
                f32 dist, rad, ang, tz, ar, br;

                dist = distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], MOA_TARGET(i).x, MOA_TARGET(i).z);
                ang = angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], MOA_TARGET(i).x, MOA_TARGET(i).z);
                if (dist < 100.0f) {
                    ang = randf(0.0f, 360.0f);
                }
                rad = 6.2831855f * ang / 360.0f;
                tx = 5.0f * ekiSinfM(rad) + ep->matrix[0][0][3];
                ty = ep->matrix[0][1][3];
                tz = -(5.0f * ekiCosfM(rad) - ep->matrix[0][2][3]);
                ar = (f32)rand() / 32767.0f;
                br = (f32)rand() / 32767.0f;
                moamoa_jump(ep, pose, tx, ty, tz, ar * randRange(0.3f, 0.5f) + 0.3f, br * randRange(0.5f, 0.8f) + 0.5f, pose->state, 1);
                pose->flags &= ~0x1;
            }
            break;
        case 0x32:
            if (--pose->stateTimer < 0) {
                pose->stateTimer = 0;
                pose->state++;
            }
            break;
        case 0x33: {
            f32 rad, tx, f, dist, ty;

            pose->field_0x1C = randf(0.0f, 200.0f);
            pose->field_0x20 = randf(0.0f, 300.0f * pose->field_0x1C / 200.0f);
            pose->state++;
            pose->stateTimer = 0;
            /* fallthrough */
        case 0x34: {
            // The branch keeps its result in a temp and copies it to f at the join; writing f directly
            // coalesces the two and shifts the FP colouring for the rest of the case.
            f32 ft;

            if (pose->stateTimer < 200) {
                ft = 1.0f;
            } else {
                ft = (f32)(800 - pose->stateTimer) / 600.0f;
            }
            f = ft;
            if (ft < 0.0f) {
                f = 0.0f;
                pose->flags |= 0x200;
            }
            tx = MOA_TARGET(i).x;
            ty = MOA_TARGET(i).y;
            tz = MOA_TARGET(i).z;
            dist = pose->field_0x20 * (f32)sin((f32)(i + gp->retraceCount) / 4.0f);
            ty = pose->field_0x1C * f + ty;
            tx = f * dist + tx;
            tz = f * (-pose->field_0x20 * (f32)cos((f32)(i + gp->retraceCount) / 4.0f)) + tz;
            ep->matrix[0][0][3] += (tx - ep->matrix[0][0][3]) / 15.0f;
            ep->matrix[0][1][3] += (ty - ep->matrix[0][1][3]) / 15.0f;
            ep->matrix[0][2][3] += (tz - ep->matrix[0][2][3]) / 15.0f;
            pose->stateTimer++;
            break;
        }
        }
        case 0x1: {
            f32 dist, rad;

            {
                f32 rr, t;

                t = (f32)rand() / 32767.0f;
                rr = wp->angleToMario + (t * randRange(-45.0f, 45.0f) + -45.0f);
                rad = 6.2831855f * rr / 360.0f;
            }
            dist = wp->distToMario;
            tx = 15.0f * ekiSinfM(rad) + ep->matrix[0][0][3];
            ty = ep->matrix[0][1][3];
            tz = -(15.0f * ekiCosfM(rad) - ep->matrix[0][2][3]);
            {
                f32 a = (f32)rand() / 32767.0f * randRange(1.0f, 1.0f) + 1.0f;
                f32 b = randf(0.4f, 0.4f);

                moamoa_jump(ep, pose, tx, ty, tz, a, b, (dist < 100.0f) ? pose->state : 0, 1);
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
                tx = 30.0f * ekiSinfM(6.2831855f * rr / 360.0f) + ep->matrix[0][0][3];
            }
            ty = ep->matrix[0][1][3];
            tz = ep->matrix[0][2][3];
            {
                f32 t, b;

                // The first jump speed is kept in the float temporary f across the second rand().
                t = (f32)rand() / 32767.0f;
                f = 2.0f * (t * randRange(1.0f, 1.0f) + 1.0f);
                t = (f32)rand() / 32767.0f;
                b = 2.0f * (t * randRange(0.4f, 0.4f) + 0.4f);
                moamoa_jump(ep, pose, tx, ty, tz, f, b, (dist < 100.0f) ? pose->state : 0, 1);
            }
            break;
        }
        case 0x3: {
            f32 rad;

            ep->matrix[0][1][3] += pose->field_0x14;
            pose->field_0x14 -= pose->field_0x20;
            rad = 6.2831855f * angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], pose->target.x, pose->target.z)
                  / 360.0f;
            ep->matrix[0][0][3] = pose->field_0x1C * ekiSinfM(rad) + ep->matrix[0][0][3];
            ep->matrix[0][2][3] = pose->field_0x1C * -ekiCosfM(rad) + ep->matrix[0][2][3];
            if (--pose->frames < 0) {
                ep->matrix[0][0][3] = pose->target.x;
                ep->matrix[0][1][3] = pose->target.y;
                ep->matrix[0][2][3] = pose->target.z;
                pose->flags &= ~0x2;
                ep->flags &= ~EXT_POSE_SHADOW_AT_GROUND;
                pose->stateTimer = 0;
                pose->state++;
            }
            break;
        }
        case 0x4:
            if (++pose->stateTimer > 4) {
                pose->state = pose->queuedState;
            }
            break;
        case 0x24:
        case 0x25:
        case 0x26: {
            f32 px, pz, tz, rad;

            if (!(pose->flags & 0x1)) {
                px = mario->position.x;
                pz = mario->position.z;
            } else {
                px = MOA_TARGET(i).x;
                pz = MOA_TARGET(i).z;
            }
            r1 = (f32)rand() / 32767.0f;
            {
                f32 rr = angleABf(px, pz, ep->matrix[0][0][3], ep->matrix[0][2][3]);

                rr = rr + randScale(r1, -45.0f, 45.0f);
                rad = 6.2831855f * rr / 360.0f;
            }
            tx = 15.0f * ekiSinfM(rad) + ep->matrix[0][0][3];
            ty = ep->matrix[0][1][3];
            tz = -(15.0f * ekiCosfM(rad) - ep->matrix[0][2][3]);
            {
                f32 a, b;

                a = (f32)rand() / 32767.0f;
                b = (f32)rand() / 32767.0f;
                moamoa_jump(ep, pose, tx, ty, tz, a * randRange(1.0f, 1.0f) + 1.0f, b * randRange(1.0f, 1.0f) + 1.0f, pose->state + 1, 1);
            }
            break;
        }
        case 0x27:
            if (++pose->stateTimer < 0x1E) {
                ep->matrix[0][0][0] *= 0.9f;
                ep->matrix[0][1][1] *= 0.9f;
                ep->matrix[0][2][2] *= 0.9f;
            } else {
                pose->state = 0x31;
            }
            break;
        case 0x2F: {
            f32 rad, dist, f, grav;

            // The jump's y is the -100 literal, so the float temporary ty carries its z target (retail's register).
            r1 = (f32)rand() / 32767.0f;
            dist = randf(300.0f, 500.0f);
            rad = 6.2831855f * (r1 * randRange(135.0f, 225.0f) + 135.0f) / 360.0f;
            tx = dist * ekiSinfM(rad) + ep->matrix[0][0][3];
            ty = -(dist * ekiCosfM(rad) - ep->matrix[0][2][3]);
            f = (f32)rand() / 32767.0f * randRange(5.0f, 6.0f) + 5.0f;
            grav = (f32)rand() / 32767.0f * randRange(0.2f, 0.3f) + 0.2f;
            if (voicePlayed == 0) {
                moaPlayHit(ep);
                voicePlayed = 1;
            }
            moamoa_jump(ep, pose, tx, -100.0f, ty, f, grav, 0x31, 0);
            break;
        }
        case 0x30:
            if (--pose->frames > 0) {
                pose->target.x += pose->field_0x14;
                pose->target.y += pose->field_0x1C;
                pose->target.z += pose->field_0x20;
                ep->matrix[0][0][3] += pose->target.x;
                ep->matrix[0][1][3] += pose->target.y;
                ep->matrix[0][2][3] += pose->target.z;
            } else {
                pose->state = 0x31;
            }
            break;
        case 0x31:
            if ((wp->flags & 0x1000) && (wp->flags & 0x2000)) {
                pose->state = 0x3E;
            } else if (wp->hitCount < 0xFA) {
                PSMTXIdentity(*ep->matrix);
                ep->matrix[0][0][3] = MOA_TARGET(i).x;
                ep->matrix[0][1][3] = MOA_TARGET(i).y;
                ep->matrix[0][2][3] = MOA_TARGET(i).z;
                pose->flags = 0;
                pose->state = 0;
                pose->stateTimer = 0;
            } else {
                pose->state = 0x3E;
            }
            break;
        case 0x19:
            if (pose->stateTimer < 0x1E) {
                Mtx rot;

                PSMTXRotRad(rot, 'y', 0.20943952f);
                PSMTXConcat(*ep->matrix, rot, *ep->matrix);
                ep->flags &= ~EXT_POSE_BILLBOARD;
                pose->stateTimer++;
            } else {
                ep->flags |= EXT_POSE_BILLBOARD;
                pose->stateTimer =
                    (100.0f - distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], MOA_TARGET(i).x, MOA_TARGET(i).z)) / 2.0f;
                pose->state++;
            }
            break;
        case 0x1A:
            if (--pose->stateTimer < 0) {
                pose->state++;
            }
            break;
        case 0x1B: {
            f32 tz, rad;

            if (__fabsf(ep->matrix[0][0][3]) < 300.0 && __fabsf(ep->matrix[0][2][3]) < 300.0) {
                r1 = (f32)rand() / 32767.0f;
                {
                    f32 rr = angleABf(MOA_TARGET(i).x, MOA_TARGET(i).z, ep->matrix[0][0][3], ep->matrix[0][2][3]);

                    rr = rr + randScale(r1, -45.0f, 45.0f);
                    rad = 6.2831855f * rr / 360.0f;
                }
                tx = 15.0f * ekiSinfM(rad) + ep->matrix[0][0][3];
                tz = -(15.0f * ekiCosfM(rad) - ep->matrix[0][2][3]);
                {
                    f32 r2;

                    r1 = (f32)rand() / 32767.0f;
                    r2 = (f32)rand() / 32767.0f;
                    moamoa_jump(ep, pose, tx, 0.0f, tz, 3.0f * (r1 * randRange(1.0f, 1.0f) + 1.0f), r2 * randRange(1.0f, 1.0f) + 1.0f, pose->state, 1);
                }
            } else {
                pose->state = 0x3E;
            }
            break;
        }
        case 0x3E:
            pose->flags |= 0x20;
            ep->flags &= ~EXT_POSE_VISIBLE;
            ep->flags |= EXT_POSE_NO_SHADOW;
            break;
        case 0x3F:
            r1 = (f32)rand() / 32767.0f;
            f = randf(2.0f, 3.0f);
            pose->stateTimer = irand(i / 2);
            rad = 6.2831855f * (r1 * randRange(225.0f, 315.0f) + 225.0f) / 360.0f;
            pose->target.x = f * ekiSinfM(rad);
            pose->target.y = randf(1.0f, 2.0f);
            pose->target.z = f * -ekiCosfM(rad);
            pose->state++;
            break;
        case 0x40:
            if (--pose->stateTimer < 0) {
                pose->state++;
            }
            break;
        case 0x41: {
            Mtx rot;

            pose->target.x *= 1.1f;
            pose->target.y *= 1.02f;
            pose->target.z *= 1.1f;
            ep->matrix[0][0][3] += pose->target.x;
            ep->matrix[0][1][3] += pose->target.y;
            ep->matrix[0][2][3] += pose->target.z;
            PSMTXRotRad(rot, 'y', 0.20943952f);
            PSMTXConcat(*ep->matrix, rot, *ep->matrix);
            ep->flags &= ~EXT_POSE_BILLBOARD;
            if (ep->matrix[0][0][3] < -1000.0f) {
                pose->state = 0x3E;
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
        pose->savedX = ep->matrix[0][0][3];
        pose->savedZ = ep->matrix[0][2][3];
    }
    moamoa_pose_main();
    if (wp->flags & 0x10) {
        mario->riderDelta.x = 0.0f;
        mario->riderDelta.y = 0.0f;
        mario->riderDelta.z = 0.0f;
        return;
    }
    {
        {
            // Declaration order sets the float registers; assignment order sets the load schedule.
            f32 dz, dy, dx;
            dx = mario->riderDelta.x;
            dy = mario->riderDelta.y;
            dz = mario->riderDelta.z;

            b = ekiVec3(dx, dy, dz, &vec3_eki_0000892c);
        }
        if (PSVECMag(&b) > 2.0f) {
            PSVECNormalize(&b, &b);
            mario->riderDelta.x = 2.0f * b.x;
            mario->riderDelta.y = 2.0f * b.y;
            mario->riderDelta.z = 2.0f * b.z;
        }
    }
}

#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma inline_max_total_size reset

static void ext_moamoa_dispent(void) {
    if (wp->flags & 0x1) {
        dispEntry(CAMERA_3D, 0.0f, 1, ext_disp, NULL);
        dispEntry(CAMERA_3D, 0.0f, 2, ext_shadow_disp, NULL);
    }
    if (wp->flags & 0x2) {
        dispEntry(CAMERA_OFFSCREEN2, -5000.0f, 1, ext_disp, NULL);
        dispEntry(CAMERA_OFFSCREEN2, -5000.0f, 2, ext_shadow_disp, NULL);
    }
}

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
// clang-format on
