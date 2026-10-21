#include "jin_07.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_ext.h"
#include "evt/evt_item.h"
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
#include "driver/dispdrv.h"
#include "jin_10.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include <stdlib.h>

#pragma pool_data off

#pragma force_active on
static f32 randf(f32 min, f32 max);
static void senteresa_ext_init(void);
static f32 computePos(f32 a, f32 b, f32 t);
static void senteresa_ext_disp(void);
static void senteresa_ext_shadow_disp(void);
static void senteresa_ext_dispent(void);
static void senteresa_ext_main(void);
static TeresaEntry* pTeresa;
static s32 g_nNumEndProc[1];
u32 g_nNumOutBox[1];

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door08"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door08"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door06"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door06"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(jin_07_juyo_oumu_vivian)
    IF_INT_NOT_EQUAL(LSWF(1), 0)
        SET_VALUE(LSWF(1), 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_seq_wait, 2)
        CALL(evt_bero_exec_wait, 65536)
        CALL(evt_party_stop, 0)
        CALL(evt_mario_set_dir, 270, 300, 0)
        CALL(evt_cam3d_evt_set, -214, 129, 454, -214, 57, 48, 500, 11)
        WAIT_MS(500)
        CALL(evt_msg_print_party, STRING("stg4_jin_46"))
        CALL(evt_mario_set_pose, STRING("M_N_2"))
        WAIT_MS(500)
        CALL(evt_msg_print_party, STRING("stg4_jin_47"))
        CALL(evt_cam3d_evt_off, 1000, 11)
        CALL(evt_party_run, 0)
        CALL(evt_mario_key_onoff, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(check_roll)
    CALL(evt_mario_get_motion, LW(0))
    IF_INT_EQUAL(LW(0), 22)
        SET_VALUE(LW(0), 0)
    ELSE
        SET_VALUE(LW(0), 1)
    END_IF
    RETURN
EVT_END()

static BeroEntry bero_entry_data[4] = {
    {"w_bero_2", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, check_roll, "jin_08", "e_bero_2", 1, 1},
    {"w_bero_1", 1, 2, 0x4E20, 100000, 0, 0, 70, NULL, -1, NULL, "jin_08", "e_bero_1", 5, 5, w_close, w_open},
    {"e_bero", 1, 2, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "jin_06", "w_bero", 5, 5, e_close, e_open},
    {NULL},
};

EVT_BEGIN(evt_locked)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg4_jin_48"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock)
    SET_VALUE(LW(0), STRING("w_bero_1"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[2] = {
    {"テレサ", 0x40000620},
    {NULL},
};

// TODO: type
static u32 senteresa_ext_entry_data[15] = {
    (u32)"c_teresa", (u32)"W_1", 0x3F800000, (u32)"c_teresa",
    (u32)"W_1", 0x40000000, (u32)"c_teresa", (u32)"W_1",
    0x40400000, (u32)"c_teresa", (u32)"W_1", 0x40800000,
    0x00000000, 0x00000000, 0x00000000,
};

// Declared non-const so the Hermite copy becomes an opaque block load (lwzu),
// but forced into .rodata to keep the original object's section layout.
Vec vec3_jin_00007f9c __attribute__((section(".rodata"))) = {0.0f, 0.0f, 0.0f};
Vec vec3_jin_00007fa8 __attribute__((section(".rodata"))) = {1.0f, 0.0f, 0.0f};
const Vec vec3_jin_00007fb4 = {0.0f, 250.0f, 0.0f};
const Vec vec3_jin_00007fc0 = {1.0f, 0.0f, 0.0f};

// Functions are written in the reverse of their order in the asm (deferred inlining).

#include "rel/common/randf.h"

// Inlined arithmetic helper: keeps the `+ min` (even when min is 0) as a
// separate fmadds, which the original's inlined randf produces.
static inline f32 scale(f32 r, f32 min, f32 max) {
    return r * (max - min) + min;
}

static void senteresa_ext_init(void) {
    ExtPose* pose = extGetPosePtr();
    s32 count = extGetPoseNum();
    s32 i;

    pTeresa = _mapAlloc(0x2580);
    for (i = 0; i < count; i++, pose++) {
        f32 a;
        f32 b;
        f32 angle;
        f32 c;
        f32 bscale;

        pose->flags = EXT_POSE_BILLBOARD;
        pose->heading = 270.0f;
        PSMTXIdentity(*pose->matrix);
        pTeresa[i].field_0x00 = 0.0f;
        (*pose->matrix)[0][3] = 0.0f;
        pTeresa[i].field_0x04 = 6.0f;
        (*pose->matrix)[1][3] = 6.0f;
        pTeresa[i].field_0x08 = 80.0f;
        (*pose->matrix)[2][3] = 80.0f;
        a = (f32)rand() / 32767.0f;
        b = (f32)rand() / 32767.0f;
        pTeresa[i].field_0x0C = 200.0f;
        angle = scale(a, 0.0f, 360.0f);
        pTeresa[i].field_0x10 = pTeresa[i].field_0x04 + randf(27.0f, 170.0f);
        c = (f32)cos(angle);
        bscale = scale(b, 10.0f, 30.0f);
        pTeresa[i].field_0x14 = pTeresa[i].field_0x08 + bscale * c;
        pTeresa[i].field_0x1C = 0.0f;
        pTeresa[i].field_0x20 = randf(-0.1f, 0.1f);
        pTeresa[i].field_0x18 = 0.0f;
        pTeresa[i].field_0x28 = (s32)(1.7f * (f32)(i + 1));
        pTeresa[i].field_0x2C = 0;
        pTeresa[i].field_0x24 = randf(0.0f, 350.0f);
    }
    g_nNumOutBox[0] = 0;
    g_nNumEndProc[0] = 0;
}

static f32 computePos(f32 a, f32 b, f32 t) {
    return (1.0f - t) * a + t * b;
}

// va/vb start as vec3_jin_00007f9c/vec3_jin_00007fa8 and va.x/va.y are then
// overwritten with p[9] and 3.4*p[4]. The parenthesization of h00/h10 keeps the
// t*omt intermediate shared and puts the 1.0/2.0/3.0 constants in the registers
// the target uses.
void computePosHermite(ExtPose* pose, f32* p) {
    f32 t = p[6];
    f32 omt = 1.0f - t;
    f32 two_t = 2.0f * t;
    f32 t2 = t * t;
    f32 h00 = ((1.0f + two_t) * omt) * omt;
    f32 h10 = t * omt * omt;
    f32 h11 = (t - 1.0f) * t2;
    f32 h01 = (3.0f - two_t) * t2;
    Vec va = vec3_jin_00007f9c;
    Vec vb = vec3_jin_00007fa8;
    va.x = p[9];
    va.y = 3.4f * p[4];

    (*pose->matrix)[0][3] = p[0] * h00 + va.x * h10 + vb.x * h11 + p[3] * h01;
    (*pose->matrix)[1][3] = p[1] * h00 + va.y * h10 + vb.y * h11 + p[4] * h01;
    (*pose->matrix)[2][3] = p[2] * h00 + va.z * h10 + vb.z * h11 + p[5] * h01;
}



// Inlined copy of computePosHermite. The inlined instance sees different
// register pressure than the out-of-line body, so it inlines the `2.0f * t`
// product and keeps `t * t` inline instead of using a shared t2 local.
static inline void senteresa_ext_main_computePosHermite(ExtPose* pose, f32* p) {
    f32 t = p[6];
    f32 omt = 1.0f - t;
    f32 h00 = ((1.0f + 2.0f * t) * omt) * omt;
    f32 h10 = t * omt * omt;
    f32 h11 = (t - 1.0f) * (t * t);
    f32 h01 = (3.0f - 2.0f * t) * (t * t);
    Vec va = vec3_jin_00007f9c;
    Vec vb = vec3_jin_00007fa8;
    va.x = p[9];
    va.y = 3.4f * p[4];

    (*pose->matrix)[0][3] = p[0] * h00 + va.x * h10 + vb.x * h11 + p[3] * h01;
    (*pose->matrix)[1][3] = p[1] * h00 + va.y * h10 + vb.y * h11 + p[4] * h01;
    (*pose->matrix)[2][3] = p[2] * h00 + va.z * h10 + vb.z * h11 + p[5] * h01;
}


// The inlined Hermite body's FP schedule is reproduced by the helper above; the
// rest of this function (loop, switch, state updates, tail) matches directly.
static void senteresa_ext_main(void) {
    ExtPose* pose = extGetPosePtr();
    s32 count;
    s32 i;

    marioGetPtr();
    count = extGetPoseNum();
    for (i = 0; i < count; i++, pose++) {
        switch (pTeresa[i].field_0x2C) {
            case 0:
                pose->texId = 0;
                pTeresa[i].field_0x28--;
                if (pTeresa[i].field_0x28 == 0) {
                    pose->flags |= EXT_POSE_VISIBLE;
                    g_nNumOutBox[0]++;
                    pTeresa[i].field_0x2C++;
                }
                break;
            case 1: {
                pose->texId = (gp->retraceCount + i * i) % 3 + 1;
                senteresa_ext_main_computePosHermite(pose, (f32*)&pTeresa[i]);
                pTeresa[i].field_0x18 += 0.015f;
                if (pTeresa[i].field_0x18 >= 1.0f) {
                    pTeresa[i].field_0x00 = (*pose->matrix)[0][3];
                    pTeresa[i].field_0x04 = (*pose->matrix)[1][3];
                    pTeresa[i].field_0x08 = (*pose->matrix)[2][3];
                    pTeresa[i].field_0x0C = 200.0f;
                    pTeresa[i].field_0x10 = (*pose->matrix)[1][3];
                    pTeresa[i].field_0x14 = 50.0f;
                    pTeresa[i].field_0x18 = 0.01f;
                    pTeresa[i].field_0x2C = 99;
                    g_nNumEndProc[0]++;
                }
                break;
            }
            case 99:
                break;
            default:
                break;
        }
    }
    if (g_nNumEndProc[0] == 200) {
        extReset();
    }
}

static void senteresa_ext_dispent(void) {
    dispEntry(CAMERA_3D, 0.0f, 1, (DispCallback)senteresa_ext_disp, NULL);
}

static void senteresa_ext_disp(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

static void senteresa_ext_shadow_disp(void) {
    static Mtx mt_600 = {
        {10.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 10.0f, 0.0f, 0.1f},
        {0.0f, 0.0f, 10.0f, 0.0f},
    };

    extLoadShadowMtx(mt_600);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
}

USER_FUNC(senteresa_get_position) {
    s32* args = event->args;
    ExtPose* poses = extGetPosePtr();
    ExtPose* pose = &poses[evtGetValue(event, args[0])];

    evtSetFloat(event, args[1], (*pose->matrix)[0][3]);
    evtSetFloat(event, args[2], (*pose->matrix)[1][3]);
    evtSetFloat(event, args[3], (*pose->matrix)[2][3]);
    return EVT_RETURN_DONE;
}

USER_FUNC(senteresa_okure_move_init) {
    s32* args = event->args;

    event->lwData[10] = evtGetValue(event, args[0]);
    event->lwData[11] = evtGetValue(event, args[1]);
    event->lwData[12] = evtGetValue(event, args[2]);
    event->lwData[13] = evtGetValue(event, args[3]);
    event->lwData[14] = evtGetValue(event, args[4]);
    event->lwData[15] = evtGetValue(event, args[5]);
    return EVT_RETURN_DONE;
}

// The `pos; va; vb; h01` declaration order is what puts vb at 0x8, va at 0x14 and
// pos at 0x20 and keeps the args pointer in r31 across npcNameToPtr.
USER_FUNC(senteresa_okure_move) {
    s32* args = event->args;
    NpcEntry* npc = npcNameToPtr("テレサ");
    f32 t = evtGetFloat(event, args[0]);
    f32 p0 = (f32)event->lwData[10];
    f32 p1 = (f32)event->lwData[11];
    f32 p2 = (f32)event->lwData[12];
    f32 p3 = (f32)event->lwData[13];
    f32 p4 = (f32)event->lwData[14];
    f32 p5 = (f32)event->lwData[15];
    f32 omt = 1.0f - t;
    f32 two_t = 2.0f * t;
    f32 h00 = (1.0f + two_t) * omt * omt;
    f32 h11 = (t - 1.0f) * (t * t);
    f32 h10 = t * omt * omt;
    Vec pos;
    Vec va = vec3_jin_00007fb4;
    Vec vb = vec3_jin_00007fc0;
    f32 h01 = (3.0f - two_t) * (t * t);

    pos.x = p0 * h00 + va.x * h10 + vb.x * h11 + p3 * h01;
    pos.y = p1 * h00 + va.y * h10 + vb.y * h11 + p4 * h01;
    pos.z = p2 * h00 + va.z * h10 + vb.z * h11 + p5 * h01;
    npc->position = pos;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_teresa_box_open)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envoff, 1024)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 1, 100, 400, 0, 38, 48, 500, 11)
    CALL(evt_mapobj_get_position, STRING("hako"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_BOX_SWING1"), LW(10), LW(11), LW(12), LW(13))
    LOOP(30)
        CALL(evt_sub_random, 3, LW(0))
        CALL(evt_mapobj_trans, 1, STRING("hako"), LW(0), 0, 0)
        WAIT_FRAMES(2)
        CALL(evt_sub_random, 3, LW(0))
        MULTIPLY(LW(0), -1)
        CALL(evt_mapobj_trans, 1, STRING("hako"), LW(0), 0, 0)
        WAIT_FRAMES(2)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(13))
    CALL(evt_mapobj_trans, 1, STRING("hako"), 0, 0, 0)
    WAIT_MS(300)
    CALL(evt_mapobj_get_position, STRING("hako"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_TBOX_OPEN1"), LW(10), LW(11), LW(12), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 950, 50, 800)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa"), LW(0), 0, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    WAIT_MS(100)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 0, 175, 703, 0, 57, 26, 300, 4)
        WAIT_MS(350)
        CALL(evt_cam3d_evt_set, 0, 152, 567, 0, 57, 26, 250, 1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_ext_entry, 200, PTR(senteresa_ext_entry_data), PTR(senteresa_ext_init), PTR(senteresa_ext_main), PTR(senteresa_ext_dispent))
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED2"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_TELESA_APPEAR1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(500)
        CALL(evt_mario_set_pose, STRING("M_I_O"))
        WAIT_MS(5500)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_EQUAL(GF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(0)
            PEEK_INT(LW(0), PTR(g_nNumEndProc))
            IF_INT_GREATER_OR_EQUAL(LW(0), 175)
                BREAK_LOOP
            END_IF
            PEEK_INT(LW(1), PTR(g_nNumOutBox))
            IF_INT_GREATER(LW(1), LW(0))
                CALL(evt_sub_random, 50, LW(1))
                ADD(LW(1), 30)
                CALL(evt_sub_random, LW(1), LW(0))
                CALL(evt_msg_toge, 3, LW(0), LW(1), 100)
                CALL(evt_sub_random, 1000, LW(0))
                MODULO(LW(0), 3)
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    CALL(evt_msg_print, 0, STRING("stg4_jin_66"), 0, 0)
                    CASE_EQUAL(1)
                    CALL(evt_msg_print, 0, STRING("stg4_jin_67"), 0, 0)
                    CASE_EQUAL(2)
                    CALL(evt_msg_print, 0, STRING("stg4_jin_68"), 0, 0)
                END_SWITCH
            END_IF
            CALL(evt_sub_random, 200, LW(0))
            ADD(LW(0), 100)
            WAIT_MS(LW(0))
        END_LOOP
    END_INLINE_EVENT
    LOOP(0)
        PEEK_INT(LW(0), PTR(g_nNumEndProc))
        IF_INT_EQUAL(LW(0), 200)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1200)
    CALL(evt_npc_set_position, STRING("テレサ"), 0, 6, 80)
    CALL(evt_npc_set_ry, STRING("テレサ"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("テレサ"), 128)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(14), 0)
        SET_VALUE(LW(15), 0)
        LOOP(100)
            CALL(evt_npc_get_position, STRING("テレサ"), LW(0), LW(1), LW(2))
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            CALL(evt_party_get_pos, 0, LW(6), LW(7), LW(8))
            IF_INT_GREATER(LW(0), LW(3))
                IF_INT_EQUAL(LW(14), 0)
                    CALL(evt_mario_set_dir, 90, 300, 0)
                    SET_VALUE(LW(14), 1)
                END_IF
            END_IF
            IF_INT_GREATER(LW(0), LW(6))
                IF_INT_EQUAL(LW(15), 0)
                    CALL(evt_party_set_dir, 0, 90, -1)
                    SET_VALUE(LW(15), 1)
                END_IF
            END_IF
            IF_INT_EQUAL(LW(14), 1)
                IF_INT_EQUAL(LW(15), 1)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(senteresa_okure_move_init, 0, 6, 80, 200, 30, 80)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("テレサ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_TELESA_APPEAR1_2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    SET_VALUE(LF(0), 0)
    SET_FLOAT_VALUE(LW(9), FLOAT(0.0f))
    LOOP(70)
        CALL(senteresa_okure_move, LW(9))
        ADD_FLOAT(LW(9), FLOAT(0.015f))
        CALL(evt_npc_get_position, STRING("テレサ"), LW(1), LW(2), LW(3))
        IF_INT_GREATER(LW(1), 75)
            IF_INT_EQUAL(LF(0), 0)
                SET_VALUE(LF(0), 1)
                WAIT_MS(300)
                CALL(evt_set_dir_to_target, STRING("テレサ"), STRING("mario"))
                CALL(evt_msg_print, 0, STRING("stg4_jin_69"), 0, STRING("テレサ"))
                CALL(evt_msg_select, 0, STRING("stg4_jin_69_yn"))
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    CALL(evt_msg_print_add, 0, STRING("stg4_jin_71"))
                    SET_VALUE(LW(0), 121)
                    CASE_EQUAL(1)
                    CALL(evt_msg_print_add, 0, STRING("stg4_jin_71"))
                    SET_VALUE(LW(0), 121)
                    CASE_EQUAL(2)
                    CALL(evt_msg_print_add, 0, STRING("stg4_jin_70"))
                    SET_VALUE(LW(0), 150)
                END_SWITCH
                CALL(evt_item_entry, STRING("teresa"), LW(0), LW(1), LW(2), LW(3), 4, -1, 0)
                ADD(LW(2), 5)
                CALL(evt_item_set_move_dir_speed, STRING("teresa"), 0, FLOAT(0.0f))
                CALL(evt_item_set_gravity, STRING("teresa"), FLOAT(1.0f))
                CALL(evt_item_set_bound_rate, STRING("teresa"), FLOAT(1.0f))
                CALL(evt_item_set_jump_power, STRING("teresa"), FLOAT(100.0f))
                CALL(evt_item_status_on, STRING("teresa"), 16)
                CALL(evt_msg_print, 0, STRING("stg4_jin_72"), 0, STRING("テレサ"))
                CALL(evt_npc_reverse_ry, STRING("テレサ"))
                CALL(evt_npc_get_position, STRING("テレサ"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_TELESA_APPEAR1_2"), LW(0), LW(1), LW(2), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_position, STRING("テレサ"), 0, -1000, 0)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 196)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(jin_07_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN3"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_npc_setup, PTR(npcEnt))
    SPAWN_EVENT(jin_07_juyo_oumu_vivian)
    CALL(evt_item_entry, STRING("item_00"), 125, 220, 75, -20, 16, GSWF(5636), 0)
    IF_INT_NOT_EQUAL(GSWF(2079), 0)
        SET_VALUE(GSWF(2079), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_mario_set_pos, 0, 365, -50)
            CALL(evt_mario_get_party, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(evt_party_set_pos, 0, 0, 465, -50)
            END_IF
            CALL(evt_cam_road_reset)
            CALL(evt_seq_wait, 2)
            CALL(evt_mario_wait_landon)
            CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
            CALL(evt_mario_key_onoff, 1)
        END_INLINE_EVENT
    END_IF
    IF_INT_EQUAL(GSWF(2231), 1)
        SET_VALUE(LW(0), STRING("w_bero_1"))
        CALL_CHILD(bero_case_switch_on)
    ELSE
        SET_VALUE(LW(0), STRING("w_bero_1"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    IF_INT_EQUAL(GSWF(2231), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("w_bero_1"), 0, PTR(evt_locked), 0)
    END_IF
    IF_INT_LESS(GSW(0), 196)
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa"), 95, 0, 0)
        CALL(evt_run_case_evt, 9, 1, STRING("a_hako"), 0, PTR(evt_teresa_box_open), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("hako_futa"), 5, 0, 0)
    END_IF
    CALL(evt_map_replace_mapobj, STRING("honou01"), 0)
    CALL(evt_map_replace_mapobj, STRING("honou02"), 0)
    CALL(evt_map_replace_mapobj, STRING("honou03"), 0)
    CALL(evt_map_replace_mapobj, STRING("honou04"), 0)
    RETURN
EVT_END()

// clang-format on
