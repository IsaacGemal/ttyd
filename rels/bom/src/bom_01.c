#include "bom_01.h"
#include "driver/camdrv.h"
#include "driver/envdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "win/win_main.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/extdrv.h"
#include "eff/eff_kemutest.h"
#include "eff/eff_starstone.h"
#include "effect/n64/eff_ripple_n64.h"
#include "manager/filemgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Data objects defined later in this file or in other units, declared before their first use.

#pragma pool_data off

#pragma force_active on
extern s32 evt_bom_snowfall[];
static USER_FUNC(starstone_set_ry);
static USER_FUNC(starstone_set_pos);
static USER_FUNC(starstone_entry);
static USER_FUNC(anim_tevcallback);
static USER_FUNC(yami_view);
static USER_FUNC(make_name);
static USER_FUNC(depth);
static void depthDisp(CameraId cameraId, void* param);
static void ext_dispent(void);
static void ext_main(void);  // TODO: real signature
static void ext_disp(CameraId cameraId, void* param);
static void ext_init(void);  // TODO: real signature
static void pose_change(s32 index, const char* name);
static void pose_main(void);  // TODO: real signature
static void pose_init(void);


// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// MWCC's frontend hoists an integer round trip of a string address out of a loop as an invariant
// expression (battle_stage_object's BSO_SFX); a plain literal is rematerialised at each call.
#ifdef MUST_MATCH
#define BOM_SFX(name) ((char*)(u32)(name))
#else
#define BOM_SFX(name) (name)
#endif

extern u32 lp[1];

// Inline Vec build reproduces the target's vec3 -> temp -> field copy (rule 7).
static inline Vec bomVec3(f32 x, f32 y, f32 z, const Vec* base) {
    Vec v = *base;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

// Horner steps shared by the general approximations and the pose initializer.
#define BOM_SIN_POLY(v, square, angle, b, one) \
    (v) = (v) * (square) - (b); \
    (v) = (v) * (square) + (one); \
    (v) = (v) * (angle)

#define BOM_COS_POLY(v, square, b, one) \
    (v) = (v) * (square) - (b); \
    (v) = (v) * (square) + (one)

// Keep the coefficients local to each general approximation. The pose initializer uses
// the same Horner steps with its own angle-reduction scratch.
static inline f32 bomSinApprox(f32 angle) {
    f32 v = 0.00761f;
    f32 b = 0.16605f;
    f32 one = 1.0f;
    f32 u;

    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) {
            u = angle * angle;
            BOM_SIN_POLY(v, u, angle, b, one);
        } else {
            angle = 1.5707964f - (angle - 1.5707964f);
            u = angle * angle;
            BOM_SIN_POLY(v, u, angle, b, one);
        }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f;
        u = angle * angle;
        BOM_SIN_POLY(v, u, angle, b, one);
        v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f);
        u = angle * angle;
        BOM_SIN_POLY(v, u, angle, b, one);
        v = -v;
    }
    return v;
}

static inline f32 bomCosApprox(f32 angle) {
    f32 v = 0.03705f;
    f32 b = 0.4967f;
    f32 one = 1.0f;
    f32 u;

    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) {
            u = angle * angle;
            BOM_COS_POLY(v, u, b, one);
        } else {
            angle = 1.5707964f - (angle - 1.5707964f);
            u = angle * angle;
            BOM_COS_POLY(v, u, b, one);
            v = -v;
        }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f;
        u = angle * angle;
        BOM_COS_POLY(v, u, b, one);
        v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f);
        u = angle * angle;
        BOM_COS_POLY(v, u, b, one);
    }
    return v;
}

// The cannon sight state is private to this module.
static SightWork* sight_wp;

// The SoundWork fields disp_sight reads (sound.h conflicts with this unit's headers): the playing channel, the
// per-channel s16 sample rings and the write position.
typedef struct BomSoundView {
    u8 field_0x0[0x330];                // 0x0
    u16 unk330;                         // 0x330, channel
    u8 field_0x332[0x5C0 - 0x332];      // 0x332
    s16 unk5C0[2][0x400];               // 0x5C0, sample rings
    s32 unk15C0;                        // 0x15C0, write position
} BomSoundView;
extern BomSoundView sound;

// SightWork's word at 0x18 is the sight's rotation, an f32 (bom_01.h declares it as field_4[5]).
#define SIGHT_ROT(wp) (*(f32*)&(wp)->field_4[5])

// drawBG is the last function in the original asm, so it is the first body in the reverse-ordered source.
void drawBG(s32 id, f32 x, f32 y, f32 scaleX, f32 scaleY, f32 rot) {
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx m0;
    Mtx m2;
    Mtx m1;
    s16 w;
    s16 h;

    cam = camGetPtr(CAMERA_2D);
    TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)((SightWork*)sight_wp)->file + 0xA0), &texObj, id);
    GXLoadTexObj(&texObj, 0);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 3, 0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    if (id == 0) {
        GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xF);
        GXSetTevAlphaIn(0, 6, 7, 4, 7);
        GXSetTevKAlphaSel(0, 0);
    }
    PSMTXTrans(m0, x, y, 0.0f);
    PSMTXScale(m1, scaleX, scaleY, 1.0f);
    PSMTXRotRad(m2, 'z', 0.017453292f * rot);
    PSMTXConcat(m0, m1, m1);
    PSMTXConcat(m1, m2, m0);
    PSMTXConcat(cam->view, m0, m0);
    GXLoadPosMtxImm(m0, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 3, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 3, 0);
    GXBegin(0x80, 0, 4);
    w = (s16)GXGetTexObjWidth(&texObj);
    h = (s16)GXGetTexObjHeight(&texObj);
    GXPosition3s16(-w, h, 0);
    GXTexCoord2s16(0, 0);
    GXPosition3s16(0, h, 0);
    GXTexCoord2s16(1, 0);
    GXPosition3s16(0, 0, 0);
    GXTexCoord2s16(1, 1);
    GXPosition3s16(-w, 0, 0);
    GXTexCoord2s16(0, 1);
}

// drawNum(): draw the last `count` decimal digits of |num|, least significant at index count-1.
void drawNum(s32 num, s32 count, f32 a, f32 b, f32 c) {
    s32 i;
    GXTexObj texObj;
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
    s32 n = num < 0 ? -num : num;

    winTexInit(**(TEXPalette***)((u8*)((SightWork*)sight_wp)->file + 0xA0));
    for (i = 0; i < count; i++) {
        s32 texId = n % 10 + 0xd;
        u16 w;
        f32 step;

        TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)((SightWork*)sight_wp)->file + 0xA0), &texObj, texId);
        w = GXGetTexObjWidth(&texObj);
        step = (f32)w;
        step = c * step;
        step += 2.0f;
        winTexSet(texId, (Vec){step * (f32)(count - i - 1) + a, b, 0.0f}, (Vec){c, c, c}, color);
        n /= 10;
    }
}

static inline s32 sightBlink(s32 i) {
    return ((gp->retraceCount % 20) > 6 && i == sight_wp->state) ? 1 : 0;
}

enum SightArrow {
    SIGHT_ARROW_ACTIVE = 6,
    SIGHT_ARROW_INACTIVE = 7
};

void disp_sight(CameraId cameraId, void* param) {
    GXColor colorA = {0xFF, 0xFF, 0xFF, 0xFF};
    GXColor colorB = {0x00, 0x00, 0x00, 0xFF};
    s32 i;

    drawBG(0, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
    drawBG(0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    drawBG(0, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f);
    drawBG(0, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f);
    drawBG(1, 0.0f, 0.0f, 1.0f, 1.0f, SIGHT_ROT(sight_wp));
    drawBG(1, 0.0f, 0.0f, -1.0f, 1.0f, -SIGHT_ROT(sight_wp));
    drawBG(1, 0.0f, 0.0f, 1.0f, -1.0f, -SIGHT_ROT(sight_wp));
    drawBG(1, 0.0f, 0.0f, -1.0f, -1.0f, SIGHT_ROT(sight_wp));

    if (sight_wp->state == 2) {
        SIGHT_ROT(sight_wp) += 2.0f;
    } else if (sight_wp->state == 3) {
        SIGHT_ROT(sight_wp) -= 2.0f;
    } else if (sight_wp->state == 0) {
        SIGHT_ROT(sight_wp) -= 2.0f;
    } else {
        SIGHT_ROT(sight_wp) += 1.0f;
    }

    winTexInit(**(TEXPalette***)((u8*)sight_wp->file + 0xA0));
    winTexSet(5, (Vec){0.0f, 0.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, colorB);

    for (i = 0; i < 3; i++) {
        s32 sel = ((s32)(gp->retraceCount % 30)) / 10;

        if (sel != i) {
            drawBG(4 - i, 100.0f, 60.0f, 1.0f, 1.0f, 0.0f);
            drawBG(4 - i, -100.0f, 60.0f, -1.0f, 1.0f, 0.0f);
            drawBG(4 - i, 100.0f, -60.0f, 1.0f, -1.0f, 0.0f);
            drawBG(4 - i, -100.0f, -60.0f, -1.0f, -1.0f, 0.0f);
        }
    }

    winTexInit(**(TEXPalette***)((u8*)sight_wp->file + 0xA0));
    winTexSet(0x17, (Vec){-200.0f, 150.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, colorA);

    drawNum((s32)(gp->renderTime / 100 / 1000), 10, 90.0f, 180.0f, 0.6f);
    drawNum((s32)(gp->renderTime / 200 / 1000), 8, 126.0f, 150.0f, 0.6f);
    drawNum((s32)(gp->renderTime / 300 / 1000), 6, 162.0f, 120.0f, 0.6f);
    drawNum((s32)(gp->renderTime / 400 / 1000), 6, 162.0f, 90.0f, 0.6f);
    drawNum((s32)((u32)gp->retraceCount >> 2), 4, 110.0f, -160.0f, 1.0f);
    drawNum((s32)((u32)gp->retraceCount >> 3), 2, 230.0f, -170.0f, 0.8f);

    winTexSet(8, (Vec){-210.0f, -140.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, colorA);

    if ((gp->retraceCount & 3) == 0) {
        for (i = 0; i < 4; i++) {
            sight_wp->field_4[i] = sight_wp->field_4[i + 1];
        }
        // the newest sample of the playing channel's ring buffer (an s16 stream per channel)
        sight_wp->field_4[4] = abs(sound.unk5C0[sound.unk330][sound.unk15C0]);
    }

    for (i = 0; i < 5; i++) {
        s32 j;
        s32 v = sight_wp->field_4[i];

        v /= 300;

        if (v > 7) {
            v = 7;
        }
        {
            f32 x = 20.0f * (f32)i + -240.0f;

            for (j = 0; j < v; j++) {
                f32 y = 16.0f * (f32)j + -190.0f;

                if (j == 6) {
                    drawBG(10, x, y, 1.0f, 1.0f, 0.0f);
                } else {
                    drawBG(9, x, y, 1.0f, 1.0f, 0.0f);
                }
            }
            for (j = v; j < 7; j++) {
                f32 y = 16.0f * (f32)j + -190.0f;

                if (j == 6) {
                    drawBG(12, x, y, 1.0f, 1.0f, 0.0f);
                } else {
                    drawBG(11, x, y, 1.0f, 1.0f, 0.0f);
                }
            }
        }
    }

    winTexInit(**(TEXPalette***)((u8*)sight_wp->file + 0xA0));
    for (i = 0; i < 4; i++) {
        s32 flag = sightBlink(i);
        s32 id = flag ? SIGHT_ARROW_ACTIVE : SIGHT_ARROW_INACTIVE;

        if (sight_wp->state == 4) {
            flag = 0;
            id = 7;
        }

        switch (i) {
        case 0:
            drawBG(id, 16.0f, 180.0f, 1.0f, 1.0f, 90.0f);
            break;
        case 1:
            drawBG(id, -16.0f, -180.0f, 1.0f, 1.0f, 270.0f);
            break;
        case 2:
            drawBG(id, -180.0f, 16.0f, 1.0f, 1.0f, 180.0f);
            if (flag != 0 && (gp->retraceCount & 3) == 0) {
                sight_wp->field_20++;
            }
            break;
        case 3:
            drawBG(id, 180.0f, -16.0f, 1.0f, 1.0f, 0.0f);
            if (flag != 0 && (gp->retraceCount & 3) == 0) {
                sight_wp->field_24++;
            }
            break;
        }
    }

    drawNum(sight_wp->field_20, 2, -220.0f, 0.0f, 0.8f);
    drawNum(sight_wp->field_24, 2, 200.0f, 0.0f, 0.8f);
}

EVT_BEGIN(evt_door_1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.555f))
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), LW(0), FLOAT(0.0f), FLOAT(0.0f))
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    RETURN
EVT_END()

char* door_npc[4] = {
    "ホワイト", "ペントリット", "スラリー", NULL,
};

static EVT_BEGIN(door_open)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(door_close)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 door[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"s_ie_door",
    (u32)"A_ie_door_u", (u32)"A_ie_door", (u32)"s_ie_kai", 0x00000000,
    (u32)"S_ie_naka", 0x00000000, (u32)&evt_door_1, (u32)&evt_door_2,
    (u32)&evt_wall, (u32)&door_npc, 0x00000000, 0x00000000,
    (u32)&door_open, (u32)&door_close,
};

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "bom_04", "e_bero4", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "bom_02", "w_bero", 1, 1},
    {NULL},
};

static EVT_BEGIN(first_evt)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_cam_letter_box_onoff, 1, 1)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, 340, 100, -53, 340, 80, -302, 0, 11)
    CALL(evt_mario_set_pos, -580, 0, 0)
    CALL(evt_party_set_pos, 0, -580, 0, 0)
    CALL(evt_seq_wait, 2)
    BEGIN_INLINE_EVENT
        WAIT_MS(6000)
        CALL(evt_telop_entry, 13)
    END_INLINE_EVENT
    WAIT_MS(4000)
    CALL(evt_cam3d_evt_set, 200, 150, 1001, -40, 130, 348, 6000, 11)
    WAIT_MS(6000)
    CALL(evt_mario_normalize)
    CALL(evt_mario_mov_pos2, -351, 0, FLOAT(120.0f))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -380, 97, 267, -380, 38, -9, 0, 11)
    WAIT_MS(300)
    CALL(evt_mario_set_dir, 270, 300, 0)
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    IF_INT_EQUAL(GSWF(3885), 1)
        CALL(evt_msg_print_party, STRING("stg7_bom_01__2_00"))
    ELSE
        CALL(evt_msg_print_party, STRING("stg7_bom_01__2"))
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 353)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    SET_VALUE(LW(0), STRING("w_bero"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_bom_01_karakara)
    SET_VALUE(LW(9), LW(0))
    SET_VALUE(LW(8), 0)
    LOOP(0)
        CALL(evt_mapobj_get_position, LW(9), LW(0), LW(1), LW(2))
        CALL(evt_sub_intpl_msec_init, 12, 1, 10, 1000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        SUBTRACT(LW(8), LW(0))
        IF_INT_LESS(LW(8), 0)
            ADD(LW(8), 360)
        END_IF
        CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(8), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        CALL(evt_sub_random, 10, LW(5))
        ADD(LW(5), 1)
        MULTIPLY(LW(5), 30)
        LOOP(LW(5))
            SUBTRACT(LW(8), LW(0))
            IF_INT_LESS(LW(8), 0)
                ADD(LW(8), 360)
            END_IF
            CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(8), 0)
            WAIT_FRAMES(1)
        END_LOOP
        BEGIN_INLINE_EVENT
            WAIT_MS(900)
        END_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 12, 10, 1, 1000)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        SUBTRACT(LW(8), LW(0))
        IF_INT_LESS(LW(8), 0)
            ADD(LW(8), 360)
        END_IF
        CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(8), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_LOOP
    RETURN
EVT_END()

static ExtTableEntry ext_data[31];

static USER_FUNC(sight) {
    s32 i;

    if (isFirstCall) {
        sight_wp = _mapAlloc(0x28);
        memset(sight_wp, 0, 0x28);
        for (i = 0; i < 5; i++) {
            sight_wp->field_4[i] = 0;
        }
        sight_wp->field_20 = rand();
        sight_wp->field_24 = rand();
        sight_wp->state = 5;
    }
    if (sight_wp->file == NULL) {
        if ((u32)fileAsyncf(4, 0, "%s/%s", getMarioStDvdRoot(), "taihou.tpl") == 0) {
            return EVT_RETURN_BLOCK;
        }
        sight_wp->file = fileAllocf(4, "%s/%s", getMarioStDvdRoot(), "taihou.tpl");
    }
    if (sight_wp->state == 6) {
        fileFree(sight_wp->file);
        _mapFree(sight_wp);
        return EVT_RETURN_DONE;
    }
    if (sight_wp->state != 5) {
        dispEntry(CAMERA_2D, 800.0f, 0, disp_sight, NULL);
    }
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(sight_dir) {
    s32 value = evtGetValue(event, event->args[0]);

    sight_wp->state = value;
    return EVT_RETURN_DONE;
}

static USER_FUNC(bom_flag) {
    s32* args = event->args;
    s32 onoff = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    u16* p = (u16*)(u32)((u32*)(u32)lp[0])[0];

    if (onoff != 0) {
        *p |= flag;
    } else {
        *p &= ~flag;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(bom_fall) {
    PoseData* data = ((PoseRoot*)lp[0])->data;
    ExtPose* pose = extGetPosePtr();
    s32 i;
    s32 j;
    PoseEntry* e;
    PoseData* pd;

    // 0x4 of PoseData is the pose count written by ext_init; the header keeps it as padding
    for (i = 0; i < *(s32*)((u8*)data + 0x4); i++, pose++) {
        data->states[i].field_0 = 0x7f;
        pose->matrix[0][0][3] = 600.0f * rand() / 32767.0f;
        pose->matrix[0][1][3] = 300.0f + 3700.0f * rand() / 32767.0f;
        pose->matrix[0][2][3] = -650.0f + -450.0f * rand() / 32767.0f;
        pd = ((PoseRoot*)lp[0])->data;
        for (j = 0, e = pd->entries; j < pd->count; j++, e++) {
            if (strcmp(e->name, "S_2") == 0) {
                break;
            }
        }
        if (j < pd->count) {
            if (pd->states[i].field_20 != j) {
                pd->states[i].field_20 = (s16)j;
                pd->states[i].time = gp->renderTime + rand();
            }
        }
    }
    return EVT_RETURN_DONE;
}

static void pose_init(void) {
    char buf[0x20];
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    ExtTableEntry* p = ext_data;
    s32 i = 0;
    s32 start;

    strcpy(buf, "");
    pd->count = -1;
    do {
        if (strcmp(p->type, buf) != 0) {
            strcpy(buf, p->type);
            start = i;
            pd->count++;
        }
        strcpy(pd->entries[pd->count].name, buf);
        pd->entries[pd->count].field_C = start;
        pd->entries[pd->count].field_10 = i - start + 1;
        pd->entries[pd->count].field_8 = p->value;
        i++;
    } while ((++p)->name != NULL);
    pd->count++;
}

static void pose_main(void) {
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    s32 i;
    ExtPose* pose;

    pose = extGetPosePtr();
    for (i = 0; i < *(s32*)((u8*)pd + 0x4); i++) { // 0x4 of PoseData is the pose count (the header keeps it as padding)
        PoseEntry* entry = &pd->entries[pd->states[i].field_20];
        f32 t = 60.0f * (f32)OSTicksToMilliseconds(gp->renderTime - pd->states[i].time) / 1000.0f / entry->field_8;

        if (t >= 1.0f) {
            pd->states[i].time = gp->renderTime;
            t = 0.0f;
        }
        pose->field_0x10 = entry->field_C + (s32)((f32)(entry->field_10 - 1) * t);
        pose++;
    }
}

static void pose_change(s32 index, const char* name) {
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    PoseEntry* e = pd->entries;
    s32 i;

    for (i = 0; i < pd->count; i++, e++) {
        if (strcmp(e->name, name) == 0) {
            break;
        }
    }
    if (i < pd->count) {
        if (pd->states[index].field_20 != i) {
            pd->states[index].field_20 = (s16)i;
            pd->states[index].time = gp->renderTime + rand();
        }
    }
}

USER_FUNC(make_ext_table) {
    s32 i;

    memset(ext_data, 0, 0x174);
    for (i = 0; i < 10; i++) {
        ext_data[i].name = "c_bom_sen";
        ext_data[i].type = "R_1";
        ext_data[i].value = (f32)i * 2.0f;
    }
    for (i = 0; i < 20; i++) {
        ext_data[10 + i].name = "c_bom_sen";
        ext_data[10 + i].type = "S_2";
        ext_data[10 + i].value = (f32)i * 6.0f;
    }
    ext_data[30].name = NULL;
    return EVT_RETURN_DONE;
}

// Keep the approximation coefficients and accumulators in single lifetimes throughout the pose loop.
#pragma opt_propagation off
#pragma opt_lifetimes off
static void ext_init(void) {
    s32 i;
    PoseData* pd;
    ExtPose* pose;
    f32 radius;
    f32 one;
    f32 sinCoeff;
    f32 cosCoeff;
    f32 x;
    f32 angle;
    f32 sinTmp;
    f32 sinAngle;
    f32 z;
    f32 cosTmp;
    f32 cosAngle;
    f32 sine;
    f32 cosine;

    lp[0] = (u32)smartAlloc(0xBC30, 1);
    pd = ((PoseRoot*)lp[0])->data;
    memset(pd, 0, 0xBC30);
    *(s32*)((u8*)pd + 4) = extGetPoseNum();
    pose_init();
    pose = extGetPosePtr();
    for (i = 0; i < *(s32*)((u8*)pd + 4); i++, pose++) {
        pd->states[i].field_1C = 0.0f;
        pd->states[i].field_14 = 0.0f;
        pd->states[i].field_0 = 0;
        pose->flags |= 7;
        PSMTXIdentity(*pose->matrix);
        angle = (f32)rand() * 6.2831855f / 32767.0f;
        radius = (f32)rand() * 400.0f / 32767.0f + 500.0f;
        sine = 0.00761f;
        sinCoeff = 0.16605f;
        one = 1.0f;
        if (angle <= 3.1415927f) {
            sinAngle = 1.5707964f;
            if (angle <= sinAngle) {
                sinTmp = angle * angle;
                BOM_SIN_POLY(sine, sinTmp, angle, sinCoeff, one);
            } else {
                sinTmp = angle - sinAngle;
                sinAngle = sinAngle - sinTmp;
                sinTmp = sinAngle * sinAngle;
                BOM_SIN_POLY(sine, sinTmp, sinAngle, sinCoeff, one);
            }
        } else {
            sinTmp = 4.712389f;
            if (angle < sinTmp) {
                sinTmp = 3.1415927f;
                sinAngle = angle - sinTmp;
                sinTmp = sinAngle * sinAngle;
                BOM_SIN_POLY(sine, sinTmp, sinAngle, sinCoeff, one);
                sine = -sine;
            } else {
                sinTmp = angle - sinTmp;
                sinAngle = 1.5707964f;
                sinAngle = sinAngle - sinTmp;
                sinTmp = sinAngle * sinAngle;
                BOM_SIN_POLY(sine, sinTmp, sinAngle, sinCoeff, one);
                sine = -sine;
            }
        }
        x = radius * sine;
        (*pose->matrix)[0][3] = x;
        (*pose->matrix)[1][3] = 0.0f;
        cosine = 0.03705f;
        cosCoeff = 0.4967f;
        if (angle <= 3.1415927f) {
            cosAngle = 1.5707964f;
            if (angle <= cosAngle) {
                cosTmp = angle * angle;
                BOM_COS_POLY(cosine, cosTmp, cosCoeff, one);
            } else {
                cosTmp = angle - cosAngle;
                cosTmp = cosAngle - cosTmp;
                cosTmp = cosTmp * cosTmp;
                BOM_COS_POLY(cosine, cosTmp, cosCoeff, one);
                cosine = -cosine;
            }
        } else {
            cosTmp = 4.712389f;
            if (angle < cosTmp) {
                cosTmp = 3.1415927f;
                cosTmp = angle - cosTmp;
                cosTmp = cosTmp * cosTmp;
                BOM_COS_POLY(cosine, cosTmp, cosCoeff, one);
                cosine = -cosine;
            } else {
                cosTmp = angle - cosTmp;
                cosAngle = 1.5707964f;
                cosTmp = cosAngle - cosTmp;
                cosTmp = cosTmp * cosTmp;
                BOM_COS_POLY(cosine, cosTmp, cosCoeff, one);
            }
        }
        z = radius * cosine;
        (*pose->matrix)[2][3] = z;
        pose_change(i, "R_1");
    }
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset

static void ext_disp(CameraId cameraId, void* param) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

// jump(index, a, x, y, z, p, q): aim a pose at (x,z), distance/time-scaled.
void jump(s32 index, s32 a, f32 x, f32 y, f32 z, f32 p, f32 q) {
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    ExtPose* ep = &extGetPosePtr()[index];
    f32 d;
    f32 dy;

    pd->states[index].field_4 = (Vec){x, y, z};
    pd->states[index].field_10 = p;
    pd->states[index].field_14 = q;
    dy = y - ep->matrix[0][1][3];
    d = distABf(ep->matrix[0][0][3], ep->matrix[0][2][3], x, z);
    // the original calls angleABf here and discards the result (see asm 0x2EC0)
    angleABf(ep->matrix[0][0][3], ep->matrix[0][2][3], x, z);
    pd->states[index].field_18 = d / pd->states[index].field_10;
    {
        f32 m = pd->states[index].field_14 * (f32)pd->states[index].field_18;

        pd->states[index].field_1C = m / 2.0f +
                                     dy / (f32)pd->states[index].field_18;
    }
    pd->states[index].field_1 = a;
    pd->states[index].field_0 = 100;
    ep->field_0x4 = angleABf(x, z, ep->matrix[0][0][3], ep->matrix[0][2][3]);
    if (index % 20 == 0 && !(pd->flags & 1)) {
        Vec pos2 = (Vec){ep->matrix[0][0][3], ep->matrix[0][1][3], ep->matrix[0][2][3]};

        psndSFXOn_3D(BOM_SFX("SFX_STG7_BOMB_TOGETHER2"), &pos2);
    }
}

// opt_lifetimes/opt_propagation off: hoists the sin/cos coefficients as retail does.
#pragma opt_lifetimes off
#pragma opt_propagation off
static void ext_main(void) {
    s32 i;
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    ExtPose* pose = extGetPosePtr();

    for (i = 0; i < *(s32*)((u8*)pd + 0x4); i++, pose++) {  // 0x4 of PoseData is the pose count
        Vec zero1 = {0.0f, 0.0f, 0.0f};
        Vec pos = (Vec){pose->matrix[0][0][3], pose->matrix[0][1][3], pose->matrix[0][2][3]};
        Vec zero2 = {0.0f, 0.0f, 0.0f};


        switch ((s8)pd->states[i].field_0) {
        case 0:
            if (PSVECDistance(&zero1, &pos) > 280.0f) {
                Vec dir;

                PSVECSubtract(&zero1, &pos, &dir);
                PSVECNormalize(&dir, &dir);
                PSVECScale(&dir, &zero2, 3.0f);
                PSVECAdd(&pos, &zero2, &pos);
                pose->matrix[0][0][3] = pos.x;
                pose->matrix[0][1][3] = pos.y;
                pose->matrix[0][2][3] = pos.z;
                pose->field_0x4 = angleABf(0.0f, 0.0f, pos.x, pos.z);
            } else {
                pd->states[i].field_0++;
            }
            break;
        case 1: {
            f32 z;
            f32 p;
            f32 q;
            f32 radius;
            f32 angle;

            angle = (f32)rand() * 6.2831855f / 32767.0f;
            radius = (f32)rand() * 30.0f / 32767.0f + 150.0f;
            q = (f32)rand() * 0.1f / 32767.0f + 0.1f;
            p = (f32)rand() / 32767.0f + 3.0f;

            z = bomCosApprox(angle);
            z = radius * -z;

            jump(i, 2, radius * bomSinApprox(angle), 150.0f, z, p, q);
            break;
        }
        case 2: {
            f32 angle = (f32)rand() * 6.2831855f / 32767.0f;
            f32 radius = (f32)rand() * 30.0f / 32767.0f + 50.0f;
            f32 z;
            f32 p;
            f32 q;

            q = (f32)rand() * 0.1f / 32767.0f + 0.1f;
            p = (f32)rand() / 32767.0f + 3.0f;

            z = bomCosApprox(angle);
            z = radius * -z;

            jump(i, 3, radius * bomSinApprox(angle), 400.0f, z, p, q);
            break;
        }
        case 3: {
            f32 angle = (f32)rand() * 6.2831855f / 32767.0f;
            f32 x;
            f32 radius = (f32)rand() * 70.0f / 32767.0f;
            f32 z;
            f32 p;
            f32 q;
            f32 cosv;
            f32 sinv;

            rand();
            q = 0.1f;
            rand();
            p = 3.0f;
            cosv = bomCosApprox(angle);
            z = radius * -cosv;
            sinv = bomSinApprox(angle);
            x = radius * sinv + (100.0f + (f32)(i % 100));
            jump(i, 4, x, (f32)(i % 150) + 350.0f, z, p, q);
            break;
        }
        case 4:
            pd->states[i].field_0++;
            break;
        case 5:
            break;
        case 0x64: {
            f32 angle;
            f32 rad;
            f32 sinv;
            f32 cosv;

            pose->matrix[0][1][3] += pd->states[i].field_1C;
            pd->states[i].field_1C -= pd->states[i].field_14;
            angle = angleABf(pose->matrix[0][0][3], pose->matrix[0][2][3], pd->states[i].field_4.x,
                             pd->states[i].field_4.z);
            rad = 3.1415927f * angle / 180.0f;
            sinv = bomSinApprox(rad);
            pose->matrix[0][0][3] += pd->states[i].field_10 * sinv;
            cosv = bomCosApprox(rad);
            pose->matrix[0][2][3] += pd->states[i].field_10 * -cosv;
            if (--pd->states[i].field_18 < 0) {
                pose->matrix[0][0][3] = pd->states[i].field_4.x;
                pose->matrix[0][1][3] = pd->states[i].field_4.y;
                pose->matrix[0][2][3] = pd->states[i].field_4.z;
                pd->states[i].field_0 = pd->states[i].field_1;
            }
            break;
        }
        case 0x7f:
            if (pose->matrix[0][1][3] > -1000.0f) {
                Mtx a;
                Mtx b;
                Mtx c;
                f32 v;
                Vec casePos2;

                pose->matrix[0][1][3] -= 2.0f;
                pose->flags &= ~2u;
                PSMTXTrans(a, 0.0f, -20.0f, 0.0f);
                v = sin((f32)(i + gp->retraceCount) / 100.0f);
                PSMTXRotRad(b, 'z', 0.017453292f * (10.0f * v));
                PSMTXTrans(c, 0.0f, 20.0f, 0.0f);
                PSMTXConcat(a, b, b);
                PSMTXConcat(b, c, c);
                PSMTXTrans(a, pose->matrix[0][0][3], pose->matrix[0][1][3], pose->matrix[0][2][3]);
                PSMTXConcat(a, c, pose->matrix[0]);
                if (irand(100) == 0) {
                    casePos2 = (Vec){pose->matrix[0][0][3], pose->matrix[0][1][3], pose->matrix[0][2][3]};

                    psndSFXOn_3D(BOM_SFX("SFX_STG7_BOMB_FALL1"), &casePos2);
                }
            }
            break;
        default:
            break;
        }
    }

    pose_main();
}
#pragma opt_propagation reset
#pragma opt_lifetimes reset


static ExtTableEntry ext_data[31] = {0};


EVT_BEGIN(evt_bom1000)
    CALL(make_ext_table)
    CALL(evt_npc_entry, STRING("dummy"), STRING("c_bom_sen"))
    CALL(evt_ext_entry, 1000, PTR(ext_data), PTR(ext_init), PTR(ext_main), PTR(ext_dispent))
    RETURN
EVT_END()

EVT_BEGIN(gamen_shake)
    CALL(evt_cam_shake, 4, FLOAT(0.002f), FLOAT(0.0f), 0)
    RETURN
EVT_END()

EVT_BEGIN(gamen_shake2)
    CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 100)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 100)
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 100)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 100)
    RETURN
EVT_END()

EVT_BEGIN(mario_tobideru)
    CALL(evt_mario_set_pos, 400, 780, 0)
    CALL(evt_sub_intpl_msec_init, 2, 0, 400, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    SET_VALUE(LW(2), 400)
    SET_VALUE(LW(3), 780)
    ADD(LW(2), LW(0))
    ADD(LW(3), LW(0))
    CALL(evt_mario_set_pos, LW(2), LW(3), 0)
    SUBTRACT(LW(2), 30)
    SUBTRACT(LW(3), 30)
    CALL(evt_party_set_pos, 0, LW(2), LW(3), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(mario_shot)
    SPAWN_EVENT(mario_tobideru)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_DISCHARGE1"), 0)
    CALL(evt_sub_intpl_msec_init, 2, 0, -50, 150)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, -40)
    CALL(evt_mapobj_trans, 1, STRING("dekatai"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SPAWN_EVENT(gamen_shake2)
    CALL(evt_eff, STRING("kemuri"), STRING("kemuri_test"), 11, 570, 950, 0, FLOAT(3.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff64, STRING("ripple"), STRING("ripple_n64"), 2, 570, 950, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff, STRING("particle"), STRING("particle"), 0, 520, 900, 0, 1, 1, 0, 100, 0, 0, 0, 0)
    CALL(taihou_r)
    BEGIN_INLINE_EVENT
        CALL(evt_eff, 0, STRING("hit"), 1, 570, 950, 0, FLOAT(3.0f), 4, 0, 0, 0, 0, 0, 0)
        WAIT_MS(200)
        CALL(evt_eff, 0, STRING("hit"), 1, 570, 950, 0, FLOAT(3.0f), 4, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 5, -50, 0, 150)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, -40)
    CALL(evt_mapobj_trans, 1, STRING("dekatai"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(effect_delete)
    CALL(evt_eff_delete, STRING("kemuri"))
    CALL(evt_eff_delete, STRING("ripple"))
    CALL(evt_eff_delete, STRING("particle"))
    RETURN
EVT_END()

static EVT_BEGIN(mario_kemuri)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_eff, 0, STRING("kemuri_test"), 12, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(2)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_depth)
    CALL(depth)
    RETURN
EVT_END()

EVT_BEGIN(cam_outside)
    CALL(evt_map_set_blend, 0, 255, 255, 255, 255)
    CALL(evt_map_set_blend, 1, 255, 255, 255, 255)
    CALL(evt_map_blend_off, 0)
    CALL(evt_map_blend_off, 1)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), 0, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ie_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_ie_kai"), 1)
    RETURN
EVT_END()

EVT_BEGIN(cam_inside)
    CALL(evt_cam3d_evt_set, -160, 125, -100, 10, 55, -427, 0, 11)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 0)
    CALL(evt_map_set_blend, 1, 0, 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), 90, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), 0, -90, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_ie_kai"), 1)
    RETURN
EVT_END()

EVT_BEGIN(cam_inside2)
    CALL(evt_cam3d_evt_set, -93, 104, -250, 9, 62, -447, 0, 11)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 0)
    CALL(evt_map_set_blend, 1, 0, 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_door"), 90, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("s_ie_kai"), 0, -90, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ie_naka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_ie_kai"), 1)
    RETURN
EVT_END()

static EVT_BEGIN(party_damage_pose)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_D_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_D_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("D_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_D_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_D_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_D_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_D_1"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(effect_1)
    SET_VALUE(LW(0), -220)
    LOOP(22)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, LW(0), 5, -220, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, LW(0), 5, 220, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        ADD(LW(0), 20)
    END_LOOP
    SET_VALUE(LW(0), -220)
    LOOP(22)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, -220, 5, LW(0), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_eff, 0, STRING("kemuri_test"), 2, 220, 5, LW(0), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        ADD(LW(0), 20)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(open_hatch)
    CALL(evt_mapobj_get_position, LW(1), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_SHUTTER_OPEN1"), LW(10), LW(11), LW(12), 0)
    SET_VALUE(LW(8), LW(0))
    SET_VALUE(LW(9), LW(1))
    SET_VALUE(LW(7), LW(2))
    CALL(evt_sub_intpl_msec_init, 1, 0, 180, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    IF_INT_EQUAL(LW(7), 0)
        CALL(evt_mapobj_trans, 1, LW(9), 0, 0, LW(0))
        MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
        CALL(evt_mapobj_trans, 1, LW(8), 0, 0, LW(0))
    ELSE
        CALL(evt_mapobj_trans, 1, LW(9), LW(0), 0, 0)
        MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
        CALL(evt_mapobj_trans, 1, LW(8), LW(0), 0, 0)
    END_IF
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SPAWN_EVENT(gamen_shake2)
    RETURN
EVT_END()

// TODO: type
static u32 spline_dt[9] = {
    0xC3480000, 0x00000000, 0x44228000, 0x44138000,
    0x43480000, 0xC5066000, 0x44AC8000, 0x42C80000,
    0xC59AB000,
};

char* oh_data[8] = {
    "ニトロ", "stg7_bom_65_a", "ペントリット", "stg7_bom_65_b", "スラリー", "stg7_bom_65_c", "ボムヘイD",
    "stg7_bom_65_d",
};

static EVT_BEGIN(evt_taihou)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_msg_print, 0, STRING("stg7_bom_63"), 0, STRING("me"))
    WAIT_MS(300)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos, 0, 10, 0, 500)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos, 0, 10, 500)
    CALL(evt_cam3d_evt_set, 0, 152, 555, 0, 57, 13, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("chika_off"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("jime_ugo"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yuka2"), 1)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    WAIT_FRAMES(1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    CALL_CHILD(party_damage_pose)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_FLOOR_OPEN1"), LW(10), LW(11), LW(12), LW(9))
    CALL(evt_sub_intpl_msec_init, 12, 0, 20, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_b"), LW(0), -5, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_a"), LW(0), -5, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    WAIT_MS(800)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_MARIO_FALL1"), LW(3), LW(4), LW(5), LW(6))
    CALL(evt_sub_intpl_msec_init, 1, 0, -100, 400)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_mario_set_pos, LW(3), LW(0), LW(5))
    CALL(evt_snd_sfx_pos, LW(6), LW(3), LW(0), LW(5))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_party_set_pos, 0, LW(3), LW(0), LW(5))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_mario_set_pos, 0, -500, 0)
    CALL(evt_party_set_pos, 0, 0, -500, 0)
    CALL(evt_snd_sfxoff, LW(6))
    CALL(evt_mario_cont_onoff, 1)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 0)
    DEBUG_PUT_MSG(STRING("ビビアンしっぽ無効"))
    CALL(L_evt_party_vivian_tail, 0, 0)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_npc_move_position, STRING("me"), -50, -40, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("me"), -55, -30, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("me"), -45, 80, 0, FLOAT(80.0f), 1)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("me"), 0, 275, 0, FLOAT(80.0f), 1)
        CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 0, 230, 994, 0, 80, 259, 1000, 11)
    WAIT_MS(1000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(300)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 32768)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    CALL(evt_msg_print, 0, STRING("stg7_bom_64"), 0, STRING("me"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_64_01"), 0, STRING("ホワイト"))
    SET_INT_READ_ADDR(PTR(oh_data))
    LOOP(4)
        READ_INT2(LW(0), LW(1))
        BEGIN_INLINE_EVENT
            CALL(evt_set_dir_to_target, LW(0), STRING("mario"))
            CALL(evt_msg_print, 0, LW(1), 0, LW(0))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_npc_get_position, LW(0), LW(10), LW(11), LW(12))
            CALL(evt_npc_jump_position_nohit, LW(0), LW(10), LW(11), LW(12), 300, FLOAT(20.0f))
        END_INLINE_EVENT
        WAIT_MS(300)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg7_bom_66"), 0, STRING("me"))
    SET_VALUE(GF(0), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ホワイト"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("ペントリット"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("スラリー"), 512)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg7_bom_67"), 0, STRING("ホワイト"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ホワイト"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg7_bom_67"), 0, STRING("ペントリット"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ペントリット"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("ペントリット"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("スラリー"), LW(0), LW(1), LW(2))
        CALL(evt_npc_jump_position_nohit, STRING("スラリー"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg7_bom_67"), 0, STRING("スラリー"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(1000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ホワイト"), 2)
        CALL(evt_npc_move_position, STRING("ホワイト"), -50, -80, 0, FLOAT(100.0f), 1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ペントリット"), 2)
        CALL(evt_npc_move_position, STRING("ペントリット"), 0, -80, 0, FLOAT(100.0f), 1)
        SET_VALUE(GF(1), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("スラリー"), 2)
        CALL(evt_npc_move_position, STRING("スラリー"), 50, -80, 0, FLOAT(100.0f), 1)
        SET_VALUE(GF(2), 1)
    END_INLINE_EVENT
    LABEL(2)
    IF_INT_EQUAL(GF(0), 0)
        WAIT_FRAMES(1)
        GOTO(2)
    END_IF
    IF_INT_EQUAL(GF(1), 0)
        WAIT_FRAMES(1)
        GOTO(2)
    END_IF
    IF_INT_EQUAL(GF(2), 0)
        WAIT_FRAMES(1)
        GOTO(2)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("ホワイト"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("ペントリット"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("スラリー"), 512)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ホワイト"), -50, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_move_position, STRING("ホワイト"), -35, -260, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    WAIT_MS(400)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ペントリット"), -50, -80, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), -55, -225, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    WAIT_MS(400)
    CALL(evt_npc_move_position, STRING("スラリー"), -50, -80, 0, FLOAT(80.0f), 1)
    CALL(evt_cam3d_evt_set, 0, 152, 108, 0, 57, -432, 0, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("スラリー"), -70, -180, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    SET_VALUE(LW(0), PTR(door))
    SPAWN_EVENT(evt_door_open_bom_01)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ホワイト"), 0, -320, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ホワイト"), 35, -330, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ホワイト"), 80, -380, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), 70, 30, -435, 300, FLOAT(20.0f))
        CALL(evt_npc_move_position, STRING("ホワイト"), 40, -445, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("ホワイト"), 0, 40, -430, 300, FLOAT(20.0f))
        CALL(evt_npc_set_ry, STRING("ホワイト"), 90)
    END_INLINE_EVENT
    WAIT_MS(400)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ペントリット"), 0, -320, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 35, -330, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 80, -380, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 80, -415, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_move_position, STRING("ペントリット"), 45, -415, 0, FLOAT(70.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("ペントリット"), -20, 15, -390, 300, FLOAT(20.0f))
        CALL(evt_npc_set_ry, STRING("ペントリット"), 315)
    END_INLINE_EVENT
    WAIT_MS(400)
    CALL(evt_npc_move_position, STRING("スラリー"), 0, -320, 0, FLOAT(70.0f), 1)
    SET_VALUE(GF(0), 0)
    CALL(evt_npc_move_position, STRING("スラリー"), 35, -330, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_move_position, STRING("スラリー"), 80, -380, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_move_position, STRING("スラリー"), 80, -415, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_move_position, STRING("スラリー"), 45, -415, 0, FLOAT(70.0f), 1)
    CALL(evt_npc_jump_position_nohit, STRING("スラリー"), 20, 15, -390, 300, FLOAT(20.0f))
    CALL(evt_npc_set_ry, STRING("スラリー"), 90)
    CALL(evt_cam3d_evt_set, -160, 125, -100, 10, 55, -427, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_set_dir_to_target, STRING("ホワイト"), STRING("ペントリット"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_68"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("ペントリット"), STRING("ホワイト"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_69"), 0, STRING("ペントリット"))
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("ペントリット"))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE1"), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL(evt_map_playanim, STRING("kikai"), 1, 0)
    WAIT_MS(1000)
    CALL(evt_set_dir_to_target, STRING("ペントリット"), STRING("ホワイト"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_70"), 0, STRING("ペントリット"))
    CALL(evt_npc_set_ry, STRING("ペントリット"), 315)
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("ホワイト"), STRING("スラリー"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_71"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("スラリー"), STRING("ホワイト"))
    CALL(evt_msg_print, 0, STRING("stg7_bom_72"), 0, STRING("スラリー"))
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("スラリー"))
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, 200, 350, 1224, 200, 125, 250, 0, 11)
    CALL_CHILD(cam_outside)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SIREN1"), LW(10))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg7_bom_73"), 0, STRING("ホワイト"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 0, 350, 1224, 0, 125, 250, 6000, 11)
    WAIT_MS(6000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_74"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG7_HUGE_GUN1"))
    CALL(evt_cam3d_evt_set, 0, 350, 1224, 0, 125, 250, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 283, 932, 0, 125, 250, 3000, 11)
    WAIT_MS(1500)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_QUAKE1"), LW(7))
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    WAIT_MS(1500)
    SPAWN_EVENT(effect_1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_QUAKE2"), 0)
    SPAWN_EVENT(gamen_shake2)
    WAIT_MS(300)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yuka3"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("chika_off"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hikari"), 1)
    CALL(evt_mapobj_trans, 1, STRING("chika_off"), 0, FLOAT(0.0f), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_DOWN1"), LW(8))
    CALL(evt_sub_intpl_msec_init, 0, 0, -20, 1000)
    LABEL(3)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_a"), 0, LW(0), 0)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_b"), 0, LW(0), 0)
    ADD(LW(0), -5)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_a"), -20, LW(0), 0)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_b"), 20, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(3)
    END_IF
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_FLOOR_OPEN2"), LW(8))
    CALL(evt_sub_intpl_msec_init, 0, 0, 170, 3000)
    LABEL(4)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_b"), LW(0), -20, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("hiraku_l_1_a"), LW(0), -20, 0)
    ADD(LW(0), -20)
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_a"), LW(0), -25, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("hiraku_ss_1_b"), LW(0), -25, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(4)
    END_IF
    CALL(evt_snd_sfxoff, LW(8))
    SPAWN_EVENT(gamen_shake2)
    WAIT_MS(600)
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxoff, LW(7))
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 100)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(8))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_75"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, 0, 283, 932, 0, 125, 250, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -200, 734, 684, -10, 50, 63, 2800, 11)
    WAIT_MS(1800)
    SET_VALUE(LW(0), STRING("hiraku_s_1_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_1_b"))
    SET_VALUE(LW(2), 0)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    SET_VALUE(LW(0), STRING("hiraku_s_2_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_2_b"))
    SET_VALUE(LW(2), 1)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    SET_VALUE(LW(0), STRING("hiraku_s_3_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_3_b"))
    SET_VALUE(LW(2), 0)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    SET_VALUE(LW(0), STRING("hiraku_s_4_a"))
    SET_VALUE(LW(1), STRING("hiraku_s_4_b"))
    SET_VALUE(LW(2), 1)
    SPAWN_EVENT(open_hatch)
    WAIT_MS(400)
    WAIT_MS(1000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(8))
    CALL_CHILD(cam_inside2)
    WAIT_MS(300)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg7_bom_76"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, -230, 734, 684, -10, 50, 63, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    SET_VALUE(LW(0), 0)
    LOOP(36)
        ADD(LW(0), 1)
        CALL(make_name, LW(0), LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        WAIT_MS(100)
    END_LOOP
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_cam3d_evt_set, -543, 200, 1194, -16, 175, 60, 3000, 11)
        WAIT_MS(5500)
        CALL(evt_cam3d_evt_set, 0, 70, 650, 0, 60, 250, 0, 11)
    END_INLINE_EVENT
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_UP1"), LW(8))
    CALL(evt_sub_intpl_msec_init, 1, 0, 1600, 11250)
    LABEL(5)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_trans, 1, STRING("S_dekatai"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(5)
    END_IF
    SPAWN_EVENT(effect_1)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_STOP1"), 0)
    DELETE_EVENT(LW(9))
    SPAWN_EVENT(gamen_shake2)
    WAIT_MS(2000)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hikari"), 1)
    CALL(evt_cam3d_evt_set, 1500, 100, 2539, 0, 650, 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, -1500, 100, 2539, 0, 650, 0, 5000, 11)
    WAIT_MS(5000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_77"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    SET_VALUE(GW(5), 70)
    SPAWN_EVENT_GET_ID(PTR(evt_depth), LW(9))
    CALL(sight_dir, 3)
    CALL_CHILD(cam_outside)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 62, -676, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, 420, 151, -171, 420, 62, -676, 2000, 11)
    WAIT_MS(2000)
    CALL(sight_dir, 2)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 62, -676, 2000, 11)
    WAIT_MS(2000)
    CALL(sight_dir, 5)
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, -1500, 100, 2539, 0, 650, 0, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_MOVE1"), LW(8))
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 0, -500, 3000)
    LABEL(6)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(6)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_STOP2"), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 500)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(8))
    SET_VALUE(GW(10), 70)
    SPAWN_EVENT_GET_ID(PTR(evt_depth), LW(9))
    CALL(sight_dir, 4)
    CALL_CHILD(cam_outside)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 62, -676, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(300)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg7_bom_78"), 0, 0)
    WAIT_MS(300)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_moon"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_moon"), 400, -738, -676)
    CALL(evt_mapobj_scale, 1, STRING("S_moon"), FLOAT(0.8f), FLOAT(0.8f), FLOAT(0.8f))
    CALL(sight_dir, 0)
    CALL(evt_cam3d_evt_set, 400, 151, -171, 400, 262, -676, 2000, 11)
    WAIT_MS(2000)
    CALL(sight_dir, 4)
    CALL(evt_sub_intpl_msec_init, 11, 70, 0, 1000)
    LABEL(100)
    CALL(evt_sub_intpl_msec_get_value)
    SET_VALUE(GW(5), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(100)
    END_IF
    DELETE_EVENT(LW(9))
    WAIT_MS(2000)
    CALL(sight_dir, 6)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_moon"), 1)
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_79"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, -1500, 100, 2539, 0, 650, 0, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_MOVE1"), LW(8))
    SPAWN_EVENT_GET_ID(PTR(gamen_shake), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, -500, -400, 1000)
    LABEL(7)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_rotate, 1, STRING("dekatai"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(7)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_STOP2"), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.0f), 500)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg7_bom_80"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_HATCH_OPEN1"), LW(8))
    CALL(evt_sub_intpl_msec_init, 11, 0, -1200, 1000)
    LABEL(8)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_rotate, 1, STRING("futa"), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(8)
    END_IF
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, 21, 175, 781, 9, 42, 265, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg7_bom_81"), 0, STRING("村長"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_BOMB_TOGETHER1"), LW(8))
    CALL_CHILD(evt_bom1000)
    WAIT_MS(5000)
    CALL(evt_snd_sfxoff, LW(8))
    CALL(evt_cam3d_evt_set, -120, 697, 788, 39, 556, 265, 0, 11)
    WAIT_MS(5000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL(bom_flag, 1, 1)
    CALL_CHILD(cam_inside2)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(bom_flag, 0, 1)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, -120, 697, 788, 39, 556, 265, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_BOMB_TOGETHER1_2"), LW(7))
    CALL(evt_msg_toge, 2, 180, 520, 100)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_1"), 0, 0)
    CALL(evt_msg_toge, 2, 100, 400, 95)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_2"), 0, 0)
    CALL(evt_msg_toge, 2, 10, 475, 75)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_3"), 0, 0)
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG7_HATCH_SHUT1"), LW(8))
        CALL(evt_sub_intpl_msec_init, 11, -1200, 0, 3000)
        LABEL(9)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_rotate, 1, STRING("futa"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(9)
        END_IF
        CALL(evt_snd_sfxoff, LW(8))
        CALL(evt_snd_sfxoff, LW(7))
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_msg_toge, 2, 180, 520, 100)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_4"), 0, 0)
    CALL(evt_msg_toge, 2, 100, 400, 95)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_5"), 0, 0)
    CALL(evt_msg_toge, 2, 10, 475, 75)
    CALL(evt_msg_print, 0, STRING("stg7_bom_85_6"), 0, 0)
    WAIT_MS(300)
    CALL(evt_npc_reverse_ry, STRING("村長"))
    CALL(bom_flag, 1, 1)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_CANNON_SYS_ACTIVE2"), LW(10))
    CALL_CHILD(cam_inside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_86"), 0, STRING("ホワイト"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_cam3d_evt_set, 21, 175, 781, 9, 42, 265, 0, 11)
    CALL_CHILD(cam_outside)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg7_bom_87"), 0, STRING("村長"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 200, 0, 2483, 200, 550, 0, 0, 11)
    SPAWN_EVENT(mario_shot)
    WAIT_MS(800)
    CALL_CHILD(effect_delete)
    CALL(evt_cam3d_evt_set, 806, 1189, 451, 600, 982, 69, 0, 11)
    SPAWN_EVENT(mario_shot)
    WAIT_MS(800)
    CALL_CHILD(effect_delete)
    CALL(evt_cam3d_evt_set, -605, 170, 785, -25, 334, 227, 0, 11)
    SPAWN_EVENT(mario_shot)
    WAIT_MS(800)
    CALL_CHILD(effect_delete)
    SPAWN_EVENT_GET_ID(PTR(mario_kemuri), LW(9))
    CALL(evt_mario_set_pos, 200, 0, -650)
    CALL(evt_party_set_pos, 0, 200, 0, -650)
    CALL(evt_cam3d_evt_set, 370, 245, -173, 370, 62, -1205, 0, 11)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("HAIKEI"), 1)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_DAMAGED2"), 200, 0, -650, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_AC3_4"), 200, 0, -650, LW(4))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_MARIO_FLY1"), 200, 0, -650, LW(5))
    SET_FLOAT_VALUE(LW(6), FLOAT(0.0f))
    CALL(evt_sub_spline_init, 1, 11, PTR(spline_dt), 3, 0, 2000)
    LABEL(20)
    CALL(evt_sub_spline_get_value, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfx_pos, LW(4), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfx_pos, LW(5), LW(0), LW(1), LW(2))
    ADD(LW(0), -20)
    ADD(LW(1), -20)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(3), 1)
        GOTO(20)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_snd_sfxoff, LW(5))
    WAIT_FRAMES(1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    WAIT_MS(1000)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_MARIO_LIGHT1"), 0)
    CALL(evt_eff64, 0, STRING("wangflush_n64"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_toge, 2, 340, 200, -590)
        CALL(evt_msg_print, 0, STRING("stg7_bom_88"), 0, 0)
        CALL(evt_msg_toge, 2, 400, 160, -590)
        CALL(evt_msg_print, 0, STRING("stg7_bom_89"), 0, 0)
        CALL(evt_msg_toge, 2, 373, 135, -590)
        CALL(evt_msg_print, 0, STRING("stg7_bom_90"), 0, 0)
    END_INLINE_EVENT
    CALL(bom_flag, 0, 1)
    CALL(bom_fall)
    WAIT_MS(6000)
    CALL(evt_ext_reset)
    CALL(evt_npc_set_ry, STRING("村長"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("村長"), 32768)
    CALL(evt_cam3d_evt_set, -60, 26, 493, 22, 33, 257, 0, 11)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("stg7_bom_91"), 0, STRING("村長"))
    CALL(evt_snd_bgmoff_f, 512, 2000)
    WAIT_MS(300)
    WAIT_MS(2000)
    CALL(evt_pouch_remove_item, 82, LW(0))
    CALL(evt_bero_mapchange, STRING("moo_03"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(init_soncho)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(356)
        CALL(evt_npc_set_position, STRING("me"), -15, 0, -35)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_soncho)
    SWITCH(GSW(0))
        CASE_LESS(357)
        CALL_CHILD(evt_taihou)
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_04_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_04_2"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_nitoro)
    CALL(evt_npc_set_position, STRING("me"), -120, 0, -220)
    RETURN
EVT_END()

EVT_BEGIN(talk_nitoro)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_02_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_02"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_02_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_03"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_03_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_pentrit)
    CALL(evt_npc_set_position, STRING("me"), -150, 0, 130)
    RETURN
EVT_END()

EVT_BEGIN(talk_pentrit)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_04_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_04"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_04_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_05"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_05_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_surary)
    CALL(evt_npc_set_position, STRING("me"), 152, 0, -188)
    RETURN
EVT_END()

EVT_BEGIN(talk_surary)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_06_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_06"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_06_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_07"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_07_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(init_murabito_d)
    CALL(evt_npc_set_position, STRING("me"), 175, 0, 210)
    RETURN
EVT_END()

EVT_BEGIN(talk_murabito_d)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_08_00"), 0, STRING("me"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_08"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_08_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_09"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_e)
    CALL(evt_npc_set_position, STRING("me"), -200, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_e)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_10_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_10"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_11"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_f)
    CALL(evt_npc_set_position, STRING("me"), -150, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_f)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_12_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_12"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_13"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_g)
    CALL(evt_npc_set_position, STRING("me"), -100, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_g)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_14_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_14"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_15"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_h)
    CALL(evt_npc_set_position, STRING("me"), -50, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_h)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_16_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_16"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_17"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_murabito_i)
    CALL(evt_npc_set_position, STRING("me"), 0, 0, -80)
    RETURN
EVT_END()

static EVT_BEGIN(talk_murabito_i)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL(evt_msg_print, 0, STRING("stg7_bom_18_00"), 0, STRING("me"))
        CASE_LESS(357)
        CALL(evt_msg_print, 0, STRING("stg7_bom_18"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_19"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_white)
    IF_INT_EQUAL(GSW(1420), 25)
        IF_INT_EQUAL(GSWF(5411), 0)
            RETURN
        END_IF
    END_IF
END_IF
IF_INT_EQUAL(GSWF(3876), 1)
    CALL(evt_npc_set_position, STRING("me"), 50, 0, -100)
END_IF
RETURN
EVT_END()

static EVT_BEGIN(talk_white)
    SWITCH(GSW(0))
        CASE_LESS(357)
        IF_INT_EQUAL(GSWF(3874), 0)
            CALL(evt_msg_print, 0, STRING("stg7_bom_59_02"), 0, 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg7_bom_59_03"), 0, 0)
        END_IF
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_04"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_59_05"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(musume_talk)
    SWITCH(GSW(0))
        CASE_LESS(354)
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_01"), 0, STRING("３人娘Ａ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_02"), 0, STRING("３人娘Ｂ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_03"), 0, STRING("３人娘Ｃ"))
        CASE_LESS(356)
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_04"), 0, STRING("３人娘Ａ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_05"), 0, STRING("３人娘Ｂ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_06"), 0, STRING("３人娘Ｃ"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_07"), 0, STRING("３人娘Ａ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_08"), 0, STRING("３人娘Ｂ"))
        CALL(evt_msg_print, 0, STRING("stg7_bom_09_01_09"), 0, STRING("３人娘Ｃ"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(musume_a_entry)
    CALL(evt_npc_entry, STRING("３人娘Ａ"), STRING("c_dance_r"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ａ"), STRING("踊り子"))
    CALL(evt_npc_set_position, STRING("３人娘Ａ"), 370, 5, -120)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ａ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ａ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

static EVT_BEGIN(musume_b_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｂ"), STRING("c_dance_p"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｂ"), STRING("踊り子ピンク"))
    CALL(evt_npc_set_position, STRING("３人娘Ｂ"), 400, 5, -150)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ｂ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ｂ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

static EVT_BEGIN(musume_c_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｃ"), STRING("c_dance_k"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｃ"), STRING("踊り子黄緑"))
    CALL(evt_npc_set_position, STRING("３人娘Ｃ"), 430, 5, -100)
    CALL(evt_npc_flag_onoff, 1, STRING("３人娘Ｃ"), 1536)
    CALL(evt_npc_change_interrupt, STRING("３人娘Ｃ"), 6, PTR(musume_talk))
    RETURN
EVT_END()

// Inlined copy of pose_init used by ext_init_minnnanokoe (the original inlines it there with a 0x20 buffer).
static inline void pose_init_minnnanokoe_copy(void) {
    char buf[0x20];
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    ExtTableEntry* p = ext_data;
    s32 i = 0;
    s32 start;

    strcpy(buf, "");
    pd->count = -1;
    do {
        if (strcmp(p->type, buf) != 0) {
            strcpy(buf, p->type);
            start = i;
            pd->count++;
        }
        strcpy(pd->entries[pd->count].name, buf);
        pd->entries[pd->count].field_C = start;
        pd->entries[pd->count].field_10 = i - start + 1;
        pd->entries[pd->count].field_8 = p->value;
        i++;
    } while ((++p)->name != NULL);
    pd->count++;
}

// Finds the named pose entry. The loop lives in a helper so the compiler gives its locals the
// same registers as the original.
static inline s32 find_entry_minnnanokoe(PoseData* pd) {
    PoseEntry* e;
    s32 j;

    for (j = 0, e = pd->entries; j < pd->count; j++, e++) {
        if (strcmp(e->name, "S_1") == 0) {
            break;
        }
    }
    return j;
}

static void ext_dispent(void) {
    u16* p = (u16*)(u32)((u32*)(u32)lp[0])[0];

    if (!(*p & 1)) {
        dispEntry(CAMERA_3D, 0.0f, 1, ext_disp, NULL);
    }
}

USER_FUNC(taihou_r) {
    f32 c;
    f32 s;
    f32 p;
    f32 rx;
    f32 rz;

    c = (f32)cos(0.7853981852531433);
    s = (f32)sin(0.7853981852531433);
    rx = -angleABf(0.0f, 0.0f, 0.0f, p = 100.0f * s);
    rz = -angleABf(0.0f, 0.0f, 100.0f * c, p);
    effKemuTestSetRxRz(effNameToPtr("kemuri"), rx, rz);
    effRippleN64SetRxRz(effNameToPtr("ripple"), rx, rz);
    return EVT_RETURN_DONE;
}

// The sin/cos polynomials live in one `static inline` helper each
// (see pik_02.c's hoshi_sin_approx) so the angle^2 scratch / wrapped-angle pivot colour as
// f0/f1; the angle is built in two statements (angle = 6p2832 * i; angle = angle * 0p125;)
// for the target's f28/f29/f30 load order; mtxId is computed after PSMTXTrans; GXSetTevAlphaIn
// takes its operand inline while GXSetTevColorIn keeps a `colorIn` temp; and the tev boolean
// is written `!i` rather than `(i == 0)`. Both spell the same cntlzw/srwi, but `!i` gives the
// loop web {mtxId, flag} the higher vreg number, which is what lands it in r25 and the
// {texBuf, &6p2832, &zero} chain in r24 exactly as retail has them (the `==` form swaps the two).
// opt_common_subs off: with CSE on MWCC hoists the two loop-invariant seed loads
// (0.0076100002f/0.037050001f) into extra FPRs, which pushes the whole coefficient group
// into f21/f22 and grows the frame; the target rematerialises them.
#pragma opt_common_subs off // needed: CSE otherwise hoists the loop-invariant seed loads
// opt_lifetimes off: the default lifetime pass assigns the loop scratch/temp FPRs differently.
#pragma opt_lifetimes off // needed: default lifetime analysis colours the loop scratch FPRs differently
// opt_propagation off: with propagation on MWCC folds the loop-invariant seed loads again.
#pragma opt_propagation off // needed: propagation otherwise folds the loop-invariant seed loads
static void depthDisp(CameraId cameraId, void* param) {
    GXTexObj texObj;
    Mtx mtx;
    smartEntry* texBuf;
    CameraEntry* cam;
    f32 scale;
    s32 i;
    s32 mtxId;

    scale = (f32)evtGetValue(NULL, GW(5)) / 100.0f;
    cam = camGetPtr(cameraId);
    texBuf = smartAlloc(GXGetTexBufferSize(0x260, 0x1E0, GX_TF_RGB565, GX_FALSE, 0), 3);
    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x260, 0x1E0, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(texBuf->address, GX_FALSE);
    GXPixModeSync();
    GXInitTexObj(&texObj, texBuf->address, 0x260, 0x1E0, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetNumTexGens(8);
    GXSetNumTevStages(8);
    for (i = 0; i < 8; i++) {
        f32 angle = 6.2831855f * (f32)i;
        f32 s, c, tmp;
        tmp = angle / 8.0f;
        s = bomSinApprox(tmp);
        c = bomCosApprox(tmp);

        PSMTXTrans(mtx, (scale * s) / 100.0f, (scale * c) / 100.0f, 0.0f);
        mtxId = 30 + i * 3;
        GXLoadTexMtxImm(mtx, mtxId, GX_MTX2x4);
        GXSetTexCoordGen2(i, GX_TG_MTX2x4, GX_TG_TEX0, mtxId, GX_FALSE, GX_PTIDENTITY);
        GXSetTevKColorSel(i, GX_TEV_KCSEL_1_8);
        GXSetTevKAlphaSel(i, GX_TEV_KASEL_1_8);
        GXSetTevOrder(i, i, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        {
            s32 flag = !i; // `!i` over `(i == 0)`: same cntlzw/srwi, right r24/r25 colouring
            s32 colorIn = flag ? GX_CC_ZERO : GX_CC_CPREV;

            GXSetTevColorIn(i, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, colorIn);
            GXSetTevAlphaIn(i, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, flag ? GX_CA_ZERO : GX_CA_APREV);
        }
        GXSetTevSwapMode(i, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }
    GXLoadPosMtxImm(cam->view, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(2);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 3, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 3, 0);
    GXBegin(0x80, 0, 4);
    GXPosition3s16(-0x130, 0xF0, 0);
    GXTexCoord2s16(0, 0);
    GXPosition3s16(0x130, 0xF0, 0);
    GXTexCoord2s16(1, 0);
    GXPosition3s16(0x130, -0xF0, 0);
    GXTexCoord2s16(1, 1);
    GXPosition3s16(-0x130, -0xF0, 0);
    GXTexCoord2s16(0, 1);
    sysWaitDrawSync();
}
#pragma opt_propagation reset
#pragma opt_lifetimes reset
#pragma opt_common_subs reset

static USER_FUNC(depth) {
    dispEntry(CAMERA_2D, 1000.0f, 0, depthDisp, NULL);
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(make_name) {
    static char name[0x40];
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    sprintf(name, "S_hikari%d", value);
    evtSetValue(event, args[1], (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(make_ext_table_minnnanokoe) {
    s32 i;

    memset(ext_data, 0, 0x174);
    for (i = 0; i < 10; i++) {
        ext_data[i].name = "c_bom_sen";
        ext_data[i].type = "S_1";
        ext_data[i].value = (f32)i * 4.0f;
    }
    for (i = 0; i < 20; i++) {
        ext_data[10 + i].name = "c_bom_sen";
        ext_data[10 + i].type = "R_1";
        ext_data[10 + i].value = (f32)i * 2.0f;
    }
    ext_data[30].name = NULL;
    return EVT_RETURN_DONE;
}

// Sets each pose state's initial values, places a pose on a grid, and records which named
// entry it uses.
void ext_init_minnnanokoe(void) {
    PoseData* pd;
    s32 rx;
    s32 j;
    s32 i;
    PoseData* data;
    s32 rz;
    ExtPose* pose;

    lp[0] = (u32)smartAlloc(0xBC30, 1);
    data = ((PoseRoot*)lp[0])->data;
    memset(data, 0, 0xBC30);
    // 0x4 of PoseData is the pose count; the header keeps it as padding
    *(s32*)((u8*)data + 0x4) = extGetPoseNum();
    pose_init_minnnanokoe_copy();
    pose = extGetPosePtr();
    for (i = 0; i < *(s32*)((u8*)data + 0x4); i++, pose++) {
        data->states[i].field_1C = 0.0f;
        data->states[i].field_14 = 0.0f;
        data->states[i].field_0 = 5;
        pose->flags |= 3;
        PSMTXIdentity((MtxPtr)pose->matrix);
        rx = (i % 14) * 30 - 205 + rand() % 10;
        rz = 190 - (i / 14) * 20 + rand() % 20;
        pose->matrix[0][0][3] = (f32)rx;
        pose->matrix[0][1][3] = 0.0f;
        pose->matrix[0][2][3] = (f32)rz;
        pose->field_0x4 = 270.0f;
        pd = ((PoseRoot*)lp[0])->data;
        j = find_entry_minnnanokoe(pd);
        if (j < pd->count) {
            if (pd->states[i].field_20 != j) {
                pd->states[i].field_20 = (s16)j;
                pd->states[i].time = gp->renderTime + rand();
            }
        }
    }
}

// jump_minnnanokoe(index, f): kick a single pose's state (position from pose matrix, a
// short animation timer) and occasionally play a 3D SFX.
void jump_minnnanokoe(s32 index, f32 f) {
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    ExtPose* pose = &extGetPosePtr()[index];

    pd->states[index].field_4 = (Vec){pose->matrix[0][0][3], pose->matrix[0][1][3], pose->matrix[0][2][3]};
    pd->states[index].field_10 = 0.0f;
    pd->states[index].field_14 = 0.1f;
    pd->states[index].field_18 = 20;
    {
        f32 m = pd->states[index].field_14 * (f32)pd->states[index].field_18;

        pd->states[index].field_1C = m / 2.0f + f / (f32)pd->states[index].field_18;
    }
    pd->states[index].field_1 = 5;
    pd->states[index].field_0 = 100;
    if (evtGetValue(0, 0xF5DE0180) != 401 && index % 5 == 0) {
        Vec pos2 = (Vec){pose->matrix[0][0][3], pose->matrix[0][1][3], pose->matrix[0][2][3]};

        psndSFXOn_3D("SFX_STG7_BOMB_TOGETHER2", &pos2);
    }
}

void ext_disp_minnnanokoe(CameraId cameraId, void* param) {
    EnvironmentYami yami;
    EnvironmentYami* pyami = &yami;
    ExtPose* pose = extGetPosePtr();
    s32 i;

    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    for (i = 0; i < extGetPoseNum(); ) {
        pyami->tevStage = 1;
        pyami->texMap = 1;
        pyami->texCoord = 1;
        pyami->unk18 = 0;
        pyami->unk4 = (MtxPtr)pose->matrix;
        envSetYami(pyami);
        GXSetNumTexGens(pyami->texCoord);
        GXSetNumTevStages(pyami->tevStage);
        GXSetNumIndStages(pyami->unk18);
        extPoseDraw(i);
        i++;
        pose++;
    }
    extLoadTextureExit();
}

void ext_dispent_minnnanokoe(void) {
    dispEntry(CAMERA_3D, 0.0f, 1, ext_disp_minnnanokoe, NULL);
}

USER_FUNC(bom1000_msg) {
    s32* argp = event->args;
    s32* args = argp;
    s32 a = evtGetValue(event, *argp++);
    s32 b = evtGetValue(event, *argp++);
    ExtPose* pose = extGetPosePtr();
    s32 base;
    s32 flag;
    ExtPose* p;
    s32 idx;
    if (a == 0) {
        base = 3;
        flag = 1;
    } else {
        base = 10;
        flag = 0;
    }
    idx = b + base * 14;
    b += (rand() % 2 + 1) * 4;
    b %= 14;
    p = &pose[idx];
    evtSetValue(event, args[0], flag);
    evtSetValue(event, args[1], b);
    evtSetFloat(event, *argp++, p->matrix[0][0][3]);
    evtSetFloat(event, *argp++, p->matrix[0][2][3]);
    return EVT_RETURN_DONE;
}

// bom1000_jump(): kick a random pose's state and occasionally play a 3D SFX.
USER_FUNC(bom1000_jump) {
    PoseData* pd = ((PoseRoot*)lp[0])->data;
    s32 idx = rand() % *(s32*)((u8*)pd + 0x4);  // 0x4 of PoseData is the pose count

    if ((s32)pd->states[idx].field_0 != 100) {
        jump_minnnanokoe(idx, 10.0f);
    }
    return EVT_RETURN_DONE;
}

__declspec(section ".rodata") const Vec vec3_bom_00006ebc = {0.0f, 100.0f, 0.0f};
__declspec(section ".rodata") const Vec vec3_bom_00006ec8 = {0.0f, 1.0f, 0.0f};
__declspec(section ".rodata") const Vec vec3_bom_00006ed4 = {0.0f, 0.0f, 0.0f};

// TODO: type
static u32 npcEnt[161] = {
    (u32)"村長", 0x40000600, 0x00000000, (u32)&init_soncho,
    0x00000000, (u32)&talk_soncho, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ニトロ",
    0x40000600, 0x00000000, (u32)&init_nitoro, 0x00000000,
    (u32)&talk_nitoro, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ペントリット", 0x40000600,
    0x00000000, (u32)&init_pentrit, 0x00000000, (u32)&talk_pentrit,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"スラリー", 0x40000600, 0x00000000,
    (u32)&init_surary, 0x00000000, (u32)&talk_surary, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"ボムヘイD", 0x40000600, 0x00000000, (u32)&init_murabito_d,
    0x00000000, (u32)&talk_murabito_d, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ホワイト",
    0x40000600, 0x00000000, (u32)&init_white, 0x00000000,
    (u32)&talk_white, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};



EVT_BEGIN(bom1000_minnnanokoe)
    CALL(make_ext_table_minnnanokoe)
    CALL(evt_npc_entry, STRING("dummy"), STRING("c_bom_sen"))
    CALL(evt_npc_set_position, STRING("dummy"), 0, -1000, 0)
    CALL(evt_ext_entry, 250, PTR(ext_data), PTR(ext_init_minnnanokoe), PTR(ext_main), PTR(ext_dispent_minnnanokoe))
    RETURN
EVT_END()



static EVT_BEGIN(minnnanokoe_evt)
    SPAWN_EVENT(bom1000_minnnanokoe)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_yuka1"), 1)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("村長"), 520, 0, 0)
    CALL(evt_npc_set_ry, STRING("村長"), 270)
    CALL(evt_npc_status_onoff, 1, STRING("村長"), 2)
    CALL(evt_npc_set_position, STRING("ニトロ"), -30, 0, -180)
    CALL(evt_npc_set_ry, STRING("ニトロ"), 270)
    CALL(evt_npc_set_position, STRING("ペントリット"), 0, 0, -180)
    CALL(evt_npc_set_ry, STRING("ペントリット"), 270)
    CALL(evt_npc_set_position, STRING("スラリー"), 30, 0, -180)
    CALL(evt_npc_set_ry, STRING("スラリー"), 90)
    CALL(evt_npc_set_position, STRING("ボムヘイD"), 570, 0, 25)
    CALL(evt_npc_set_ry, STRING("ボムヘイD"), 270)
    CALL(evt_npc_status_onoff, 1, STRING("ボムヘイD"), 2)
    CALL(evt_npc_set_position, STRING("ホワイト"), 0, 0, -230)
    CALL(evt_npc_set_ry, STRING("ホワイト"), 270)
    CALL(evt_map_set_tevcallback, 1, PTR(envSetYami))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_bom01"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_bom01"), 256)
    CALL(anim_tevcallback, STRING("村長"), 1)
    CALL(anim_tevcallback, STRING("ニトロ"), 1)
    CALL(anim_tevcallback, STRING("ペントリット"), 1)
    CALL(anim_tevcallback, STRING("スラリー"), 1)
    CALL(anim_tevcallback, STRING("ボムヘイD"), 1)
    CALL(anim_tevcallback, STRING("ホワイト"), 1)
    CALL(yami_view, -1500)
    CALL(evt_bg_set_color, 64, 64, 64, 255)
    CALL(starstone_entry, STRING("starstone"), 0, 50, 10)
    SET_VALUE(GW(0), 50)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(starstone_set_pos, STRING("starstone"), 0, GW(0), 10)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(starstone_set_ry, STRING("starstone"), LW(0))
            SUBTRACT_FLOAT(LW(0), FLOAT(3.0f))
            IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 5000, 5500, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
            CALL(evt_sub_intpl_msec_init, 11, 5500, 5000, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(2)
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 400, 106, 331, 400, 45, -13, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(500)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ボムヘイD"), 450, 25, 0, 90, 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("村長"), 400, 0, 0, 90, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_65"), 0, STRING("村長"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_66"), 0, STRING("村長"))
    CALL(evt_cam3d_evt_set, 0, 283, 1182, 0, 75, 0, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg8_las_148_67"), 0, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(bom1000_jump)
            WAIT_MS(100)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(14), 0)
        SET_VALUE(LW(15), 3)
        LOOP(0)
            CALL(bom1000_msg, LW(14), LW(15), LW(0), LW(2))
            CALL(evt_sub_random, 2, LW(1))
            ADD(LW(1), 1)
            MULTIPLY(LW(1), 30)
            CALL(evt_sub_random, 20, LW(3))
            SUBTRACT(LW(3), 10)
            ADD(LW(1), LW(3))
            CALL(evt_msg_toge, 3, LW(0), LW(1), LW(2))
            CALL(evt_sub_random, 2, LW(3))
            SWITCH(LW(3))
                CASE_EQUAL(0)
                SET_VALUE(LW(3), STRING("stg8_las_148_68"))
                CASE_EQUAL(1)
                SET_VALUE(LW(3), STRING("stg8_las_148_69"))
                CASE_EQUAL(2)
                SET_VALUE(LW(3), STRING("stg8_las_148_70"))
            END_SWITCH
            BEGIN_INLINE_EVENT
                CALL(evt_msg_print, 0, LW(3), 0, 0)
            END_INLINE_EVENT
            WAIT_MS(300)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, 400, 106, 331, 400, 45, -13, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("村長"), 90)
    WAIT_MS(300)
    CALL(evt_npc_set_ry, STRING("ボムヘイD"), 90)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ボムヘイD"))
    CALL(evt_msg_print, 0, STRING("stg8_las_148_71"), 0, STRING("村長"))
    WAIT_MS(2000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    CALL(evt_bero_mapchange, STRING("las_29"), STRING("minnnanokoe"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_door)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg7_bom_09_01"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(bom_01_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    SWITCH(GSW(0))
        CASE_LESS(349)
        CALL_CHILD(evt_bero_info_run)
        CASE_LESS(353)
        CALL_CHILD(bero_case_entry)
        CASE_DEFAULT
        CALL_CHILD(evt_bero_info_run)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 400)
        SET_VALUE(LF(0), 0)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG8_VOICE1"))
            CALL(evt_snd_envoff, 512)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_BOM1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_BOM2"))
    END_IF
    SET_VALUE(LW(0), STRING("group10"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group11"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group12"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group13"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group14"))
    SPAWN_EVENT(evt_bom_01_karakara)
    SET_VALUE(LW(0), STRING("group15"))
    SPAWN_EVENT(evt_bom_01_karakara)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("chika_off"), 1)
    CALL(evt_hitobj_onoff, STRING("A_chika"), 1, 0)
    CALL_CHILD(evt_bom_snowfall)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_moon"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka3"), 1)
    CALL(evt_mapobj_trans, 1, STRING("chika_off"), 0, FLOAT(-1.0f), 0)
    CALL(evt_npc_setup, PTR(npcEnt))
    SWITCH(GSW(0))
        CASE_BETWEEN(352, 357)
        CALL_CHILD(musume_a_entry)
        CALL_CHILD(musume_b_entry)
        CALL_CHILD(musume_c_entry)
    END_SWITCH
    CALL(evt_run_case_evt, 9, 1, STRING("A_ie_door"), 0, PTR(evt_door), 0)
    CALL(evt_item_entry, STRING("item_00"), 125, -180, 5, 450, 16, GSWF(5659), 0)
    SWITCH(GSW(0))
        CASE_LESS(349)
        SET_VALUE(GSWF(3885), 1)
        CASE_LESS(353)
        SPAWN_EVENT(first_evt)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 400)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SPAWN_EVENT(minnnanokoe_evt)
        END_IF
    END_IF
    SPAWN_EVENT(evt_sub_mail_event)
    BEGIN_INLINE_EVENT
        CALL(sight)
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: name$915 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 name$915[16] = {
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x00000000, 0x00000000, 0x00000000,
// };






// clang-format on

// ------------------------------------------------------------------------------------------------------------
// Function bodies. The original was compiled with -inline auto,deferred, which emits functions in reverse source
// order, so the functions below are written in the reverse of the order they appear in build/G8MJ01/bom/asm/bom_01.s
// (the last asm function first). Data the functions need is declared at file scope, as the original does.
// ------------------------------------------------------------------------------------------------------------

// clang-format on


static USER_FUNC(yami_view) {
    Vec camPos;
    Vec up;
    Vec target;
    Mtx mtx;
    f32 value = evtGetFloat(event, event->args[0]);

    target = vec3_bom_00006ed4;
    target.z = value;
    up = vec3_bom_00006ec8;
    camPos = vec3_bom_00006ebc;
    camPos.z = 100.0f + value;
    C_MTXLookAt(mtx, &camPos, &up, &target);
    envSetYamiView(mtx);
    return EVT_RETURN_DONE;
}

static USER_FUNC(anim_tevcallback) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    NpcEntry* npc = npcNameToPtr_NoAssert((const char*)name);

    if (npc == NULL) {
        return EVT_RETURN_DONE;
    }
    animPoseSetMaterialFlagOn(npc->poseId, (flag & 0xF) << 24);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_entry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);

    effSetName(effStarStoneEntry(6, x, y, z, 1.0f), (const char*)name);
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_pos) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    EffectEntry* entry = effNameToPtr((const char*)name);

    ((effStarStoneData*)entry->userdata)->field_0x8 = x;
    ((effStarStoneData*)entry->userdata)->field_0xC = y;
    ((effStarStoneData*)entry->userdata)->field_0x10 = z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(starstone_set_ry) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 value = evtGetFloat(event, args[1]);
    EffectEntry* entry = effNameToPtr((const char*)name);

    ((effStarStoneData*)entry->userdata)->field_0x1C = value;
    return EVT_RETURN_DONE;
}

u32 lp[1];

