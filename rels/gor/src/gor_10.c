#include "gor_10.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "system.h"
#include "win/win_main.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(depth);
static void depthDisp(CameraId cameraId, void* param);
static void draw(CameraId cameraId, void* param);

typedef struct TegamiWork {
    s32 unk0;
    FileEntry* file;
} TegamiWork;

// File-scope constants used by mabutaDisp/draw. They are emitted at the start of .rodata, before the scripts' strings.
static const Vec vec3_gor_0001da38 = { 0.0f, -74.0f, 0.0f };
static const Vec vec3_gor_0001da44 = { 1.0f, 1.0f, 1.0f };
static const GXColor dat_gor_0001da50 = { 0xFF, 0xFF, 0xFF, 0x00 };

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)


EVT_BEGIN(tegami_init_evt)
    CALL(evt_snd_envoff, 1024)
    CALL(evt_snd_envon, 257, STRING("ENV_STG0_GOR6"))
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    BEGIN_INLINE_EVENT
        CALL(tegami)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_seq_wait, 2)
        CALL(evt_fade_end_wait)
        CALL(evt_cam_letter_box_camid, 8)
        WAIT_MS(200)
        CALL(evt_msg_print, 1, STRING("<diary><nowinse><once_stop>"), 0, 0)
        CALL(evt_msg_print_add, 0, STRING("pro_04"))
        CALL(evt_cam_letter_box_camid, 10)
        CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
        CALL(evt_bero_mapchange, STRING("aaa_00"), STRING("prologue2"))
    END_INLINE_EVENT
    RETURN
EVT_END()

// GXColor used by mabutaDisp; emitted here in .rodata, between the tegami_init_evt and evt_mezame strings.
static const GXColor dat_gor_0001da9c = { 0x00, 0x00, 0x00, 0xFF };

EVT_BEGIN(evt_mezame)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_fade_end_wait)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        WAIT_MS(2000)
        LOOP(0)
            CALL(evt_snd_sfxon, STRING("SFX_VOICE_MARIO_SLEEP1"), GW(2))
            WAIT_MS(3000)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_msg_print, 0, STRING("mac_0_012"), 0, 0)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("mac_0_013"), 0, 0)
    CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR0"))
    DELETE_EVENT(LW(10))
    WAIT_MS(300)
    CALL(evt_snd_sfxchk, GW(2), LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(evt_snd_sfxoff, GW(2))
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_VOICE_MARIO_FIND1_2"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 2400, 1200, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 1, 1200, 2400, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 1, 2400, 1200, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 1, 1200, 2400, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, 500, 0, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            SET_FLOAT_VALUE(GW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 1, 2400, 0, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_msg_print, 0, STRING("mac_0_013_01"), 0, 0)
    WAIT_MS(300)
    WAIT_MS(2000)
    CALL(evt_bero_mapchange, STRING("gor_00"), 0)
    RETURN
EVT_END()

EVT_BEGIN(mezame_init_evt)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envoff, 512)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("marukongou"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("densha"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("pukupuku"), 1)
    SET_FLOAT_VALUE(GW(0), FLOAT(50.0f))
    BEGIN_INLINE_EVENT
        CALL(depth)
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(1), FLOAT(240.0f))
    BEGIN_INLINE_EVENT
        CALL(mabuta)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_yure)
    END_INLINE_EVENT
    SPAWN_EVENT(evt_mezame)
    RETURN
EVT_END()

EVT_BEGIN(gor_10_init_evt)
    CALL(evt_map_playanim, STRING("anm_umi"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_nami"), 1, 0)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        RETURN
    END_IF
    IF_INT_LESS(GSW(0), 2)
        CALL_CHILD(tegami_init_evt)
    ELSE
        CALL_CHILD(mezame_init_evt)
    END_IF
    RETURN
EVT_END()

static f32 view_dy;
static s32 view_trg;
static TegamiWork* wp;

// Builds draw's GXColor in a returning inline: this makes the local colour take the lower
// stack slot (0x8) and winTexSet's outgoing copy the higher one (0xc), as the original does.
// Written inline the two copies swap slots.
#ifdef MUST_MATCH
static inline GXColor drawColor(u8 a) {
    GXColor c = dat_gor_0001da50;

    c.a = a;
    return c;
}
#endif
static void draw(CameraId cameraId, void* param) {
    winTexInit((TEXPalette*)wp->file->data[0]);
    {
#ifdef MUST_MATCH
        GXColor color = drawColor(wp->unk0);
#else
        GXColor color = dat_gor_0001da50;

        color.a = wp->unk0;
#endif
        winTexSet(4, vec3_gor_0001da38, vec3_gor_0001da44, color);
    }
}
USER_FUNC(tegami) {
    if (isFirstCall) {
        wp = _mapAlloc(8);
        memset(wp, 0, 8);
        wp->unk0 = 0;
        wp->file = fileAllocf(4, "%s/mariost.tpl", getMarioStDvdRoot());
    }
    wp->unk0 += 2;
    if (wp->unk0 > 0x80) {
        wp->unk0 = 0x80;
    }
    dispEntry(8, 0.0f, 0, draw, wp);
    return EVT_RETURN_BLOCK;
}
void tegami_delete(void) {
    if (wp != NULL && wp->file != NULL) {
        fileFree(wp->file);
    }
}

#pragma global_optimizer off
// NONMATCHING (72.23%): 118 of 392 instructions differ: 98 with different operands, 4 replaced, 8 missing, 7 extra, 1 with a different opcode
static void depthDisp(CameraId cameraId, void* param) {
    GXTexObj texObj;
    Mtx mtx;
    smartEntry* texBuf;
    CameraEntry* cam;
    f32 twoPi, quarter, c16605, one, pi, c4967, hundred;
    f32 scale;
    s32 i;
    s32 mtxId;
    f32 f3, f2, f4, f0, f1;

    scale = evtGetFloat(NULL, GW(0)) / 100.0f;
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
    hundred = 100.0f;
    for (i = 0; i < 8; i++) {
        twoPi = 6.2831855f;
        quarter = 0.125f;
        f2 = 0.00761f;
        f3 = (twoPi * (f32)i) * quarter;
        c16605 = 0.16605f;
        one = 1.0f;
        pi = 3.1415927f;
        if (f3 <= pi) {
            if (f3 <= 1.5707964f) {
                f0 = f3 * f3;
                f2 = (f2 * f0) - c16605;
                f2 = (f2 * f0) + one;
                f2 = f2 * f3;
            } else {
                f0 = f3 - 1.5707964f;
                f1 = 1.5707964f - f0;
                f0 = f1 * f1;
                f2 = (f2 * f0) - c16605;
                f2 = (f2 * f0) + one;
                f2 = f2 * f1;
            }
        } else {
            if (f3 < 4.712389f) {
                f1 = f3 - 3.1415927f;
                f0 = f1 * f1;
                f2 = (f2 * f0) - c16605;
                f2 = (f2 * f0) + one;
                f2 = f2 * f1;
                f2 = -f2;
            } else {
                f0 = f3 - 4.712389f;
                f1 = 1.5707964f - f0;
                f0 = f1 * f1;
                f2 = (f2 * f0) - c16605;
                f2 = (f2 * f0) + one;
                f2 = f2 * f1;
                f2 = -f2;
            }
        }
        f4 = 0.03705f;
        c4967 = 0.4967f;
        if (f3 <= pi) {
            if (f3 <= 1.5707964f) {
                f0 = f3 * f3;
                f4 = (f4 * f0) - c4967;
                f4 = (f4 * f0) + one;
            } else {
                f0 = f3 - 1.5707964f;
                f0 = 1.5707964f - f0;
                f0 = f0 * f0;
                f4 = (f4 * f0) - c4967;
                f4 = (f4 * f0) + one;
                f4 = -f4;
            }
        } else {
            if (f3 < 4.712389f) {
                f0 = f3 - 3.1415927f;
                f0 = f0 * f0;
                f4 = (f4 * f0) - c4967;
                f4 = (f4 * f0) + one;
                f4 = -f4;
            } else {
                f0 = f3 - 4.712389f;
                f0 = 1.5707964f - f0;
                f0 = f0 * f0;
                f4 = (f4 * f0) - c4967;
                f4 = (f4 * f0) + one;
            }
        }
        mtxId = 30 + i * 3;
        PSMTXTrans(mtx, (scale * f2) / hundred, (scale * f4) / hundred, 0.0f);
        GXLoadTexMtxImm(mtx, mtxId, GX_MTX2x4);
        GXSetTexCoordGen2(i, GX_TG_MTX2x4, GX_TG_TEX0, mtxId, GX_FALSE, GX_PTIDENTITY);
        GXSetTevKColorSel(i, GX_TEV_KCSEL_1_8);
        GXSetTevKAlphaSel(i, GX_TEV_KASEL_1_8);
        GXSetTevOrder(i, i, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorOp(i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        {
            s32 colorIn;
            s32 alphaIn;

            if (i == 0) {
                colorIn = GX_CC_ZERO;
                alphaIn = GX_CA_ZERO;
            } else {
                colorIn = GX_CC_CPREV;
                alphaIn = GX_CA_APREV;
            }
            GXSetTevColorIn(i, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, colorIn);
            GXSetTevAlphaIn(i, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, alphaIn);
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
#pragma global_optimizer reset
USER_FUNC(depth) {
    dispEntry(8, 1200.0f, 0, depthDisp, NULL);
    return EVT_RETURN_BLOCK;
}
void mabutaDisp(CameraId cameraId, void* param) {
    f32 v;
    CameraEntry* cam;

    v = evtGetFloat(NULL, GW(1));
    cam = camGetPtr(cameraId);
    GXSetZMode(0, 3, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetChanMatColor(4, dat_gor_0001da9c);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevOp(0, GX_PASSCLR);
    GXSetTevSwapMode(0, 0, 0);
    GXLoadPosMtxImm(cam->view, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(2);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxAttrFmt(0, 9, 1, 3, 0);
    GXBegin(0x80, 0, 8);
    GXPosition3s16(-0x130, 0xF0, 0);
    GXPosition3s16(0x130, 0xF0, 0);
    GXPosition3s16(0x130, 240.0f - v, 0);
    GXPosition3s16(-0x130, 240.0f - v, 0);
    GXPosition3s16(-0x130, -240.0f + v, 0);
    GXPosition3s16(0x130, -240.0f + v, 0);
    GXPosition3s16(0x130, -0xF0, 0);
    GXPosition3s16(-0x130, -0xF0, 0);
}
USER_FUNC(mabuta) {
    dispEntry(8, 1100.0f, 0, mabutaDisp, NULL);
    return EVT_RETURN_BLOCK;
}
USER_FUNC(cam_yure) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    if (isFirstCall) {
        view_dy = cam->viewOffset.y;
        view_trg = 0;
    }
    cam->viewOffset.y += 0.3f * (f32)sin((f32)gp->retraceCount / 50.0f);
    if (view_trg == 0) {
        if (cam->viewOffset.y > view_dy) {
            view_trg = 1;
        }
    } else {
        if (cam->viewOffset.y < view_dy) {
            psndSFXOn("SFX_EVT_OPN_SHIP_CREAK1");
            view_trg = 0;
        }
    }
    return EVT_RETURN_BLOCK;
}
// clang-format on
