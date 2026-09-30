#include "yuu_03.h"
#include "cam_shift.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_yuugijou.h"
#include "evt_yuuminigame.h"
#include "forcedata.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_roll.h"
#include "motion/mot_slit.h"
#include "party.h"
#include "party/party_motion.h"
#include "rel/yuu/yuu.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
extern s32 evt_yuucounter_alloc[];
extern s32 evt_yuucounter_free[];
extern s32 evt_yuunpc_minigame4[];
extern s32 evt_yuuranking_alloc_31_text_52F8[];
extern s32 evt_yuuranking_free_31_text_5274[];
extern s32 evt_yuutitle_alloc[];
extern s32 evt_yuutitle_free[];
extern s32 yuuranking_get_monte_31_text_51C0[];
extern s32 yuuranking_wait_button_31_text_51F8[];
static f32 randf(f32 min, f32 max);
static void polygon(f32 x, f32 y, f32 w, f32 h, f32 u, f32 v);
static s32 evt_nomal_cange(EventEntry* event, BOOL isFirstCall);

Yuuminigame4Work* unk_yuu_bss00000010;
RyushiEntry* xwp;
s32 ryuwp;
static void* yuunpc_wp;

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

USER_FUNC(unk_yuu_0000371c) {
    camShiftReset();
    return EVT_RETURN_DONE;
}

EVT_BEGIN(unk_evt_yuu_00015fe0)
    CALL(unk_yuu_000034bc)
    RETURN
EVT_END()

void unk_yuu_0000365c(CameraId cameraId, void* param) {
    Yuuminigame4Work* work = param;
    s32 i;
    if (work != NULL) {
        for (i = 0; i < work->count; i++) {
            if (work[i].unk0 == 0) {
                Vec pos = work[i].position;
                if ((gp->retraceCount & 0x1F) < 9) {
                    pos.y += 5.0f;
                }
                iconDispGx(&pos, 0, 0x1B6, 1.0f);
            }
        }
    }
}
// NONMATCHING (88.26%): unk_yuu_000034bc; needs register/allocation scheduling work
USER_FUNC(unk_yuu_000034bc) {
    s32 prev = 0;
    s32 num = 1;
    YuuminigameWork* work2 = (YuuminigameWork*)event->lwData[0];
    MarioWork* mario = marioGetPtr();
    if (isFirstCall) {
        s32 i;
        if (unk_yuu_bss00000010 != NULL) {
            prev = 1;
        }
        unk_yuu_bss00000010 = _mapAlloc(num * sizeof(Yuuminigame4Work));
        memset(unk_yuu_bss00000010, 0, num * sizeof(Yuuminigame4Work));
        unk_yuu_bss00000010->count = num;
        unk_yuu_bss00000010->work = work2;
        for (i = 0; i < unk_yuu_bss00000010->count; i++) {
            Yuuminigame4Work* e = &unk_yuu_bss00000010[i];
            e->position = mario->position;
            e->position.y += 40.0f;
            e->unk18 = 1.0f;
        }
    }
    {
        s32 i;
        for (i = 0; i < unk_yuu_bss00000010->count; i++) {
            Yuuminigame4Work* e = &unk_yuu_bss00000010[i];
            switch (e->unk4) {
            case 0:
                e->position = mario->position;
                e->position.y += 40.0f;
                break;
            case 1:
                break;
            }
        }
    }
    if (prev == 0) {
        dispEntry(4, 0.0f, 1, unk_yuu_0000365c, unk_yuu_bss00000010);
    }
    return EVT_RETURN_BLOCK;
}













































EVT_BEGIN(ryushi_event)
    SET_VALUE(GW(0), 0)
    CALL(evt_ryushi_clr, LW(1))
    LOOP(0)
        CALL(evt_ryushi)
        IF_INT_EQUAL(GW(0), 1)
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(GW(0), 2)
            RETURN
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yuu_03_kaijo_init)
    CALL(evt_mario_set_pos, -1650, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(yuu_03_kaijo_init2)
    CALL(evt_mario_set_pos, -1520, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

USER_FUNC(unk_yuu_0000341c) {
    Yuuminigame4Work* work = unk_yuu_bss00000010;
    if (work != NULL) {
        YuuminigameWork* work2 = work->work;
        Yuuminigame4Data* data = work2->data;
        if (data->subEvent != NULL) {
            evtDeleteID(data->subEvent->eventId);
            data->subEvent = NULL;
            _mapFree(unk_yuu_bss00000010);
            data->subEvent = evtEntryType(unk_evt_yuu_00015fe0, 0, 0, 0);
            data->subEvent->lwData[0] = (s32)unk_yuu_bss00000010->work;
        }
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(minigame04_main)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(yuu_03_kaijo_init)
    CALL(evt_yuuminigame_alloc, LW(0), 3)
    CALL(yuuminigame_set_dataadr, 0, LW(0))
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(yuuminigame_get_dataadr, 0, LW(0))
    CALL(yuuminigame4_set_phase, LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(minigame04_start_cam)
    CALL(evt_cam3d_evt_set, FLOAT(-1520.0f), FLOAT(335.0f), FLOAT(850.0f), FLOAT(-1520.0f), FLOAT(65.0f), FLOAT(-25.0f), 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, FLOAT(1285.0f), FLOAT(334.0f), FLOAT(850.0f), FLOAT(1285.0f), FLOAT(65.0f), FLOAT(-25.0f), 10000, 0)
    WAIT_MS(1000)
    CALL(evt_yuutitle_alloc, 0, 0, STRING("menu_title_minigame4"))
    WAIT_MS(7000)
    CALL(evt_yuutitle_free)
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(minigame04_start_cam_cancel)
    CALL(evt_seq_wait, 2)
    CALL(evt_yuutitle_free)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_wait_move_end)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(yuu_03_kaijo_init2)
    RETURN
EVT_END()

USER_FUNC(yuuminigame4_set_phase) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    Yuuminigame4Data* data = work->data;
    data->phase = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_slit_cancel) {
    MarioWork* mario = marioGetPtr();
    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        marioSlitForceCancel();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(re_init_yuunpc) {
    ext_yuunpc_minigame4_init_sub();
    return EVT_RETURN_DONE;
}

EVT_BEGIN(minigame04_start)
    CALL(yuuminigame4_opening_chk, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT_GET_ID(PTR(minigame04_start_cam), LW(8))
        LOOP(0)
            IS_EVENT_RUNNING(LW(8), LW(9))
            IF_INT_EQUAL(LW(9), 0)
                CALL(evt_seq_wait, 2)
                BREAK_LOOP
            ELSE
                CALL(evt_key_get_buttontrg, 0, LW(9))
                IF_BITS_SET(LW(9), 768)
                    DELETE_EVENT(LW(8))
                    CALL(evt_seq_wait, 2)
                    CALL_CHILD(minigame04_start_cam_cancel)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(yuuminigame4_opening_on_off, LW(0), 0)
        CALL_CHILD(yuu_03_kaijo_init2)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    END_IF
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_MINIGAME1"))
    CALL(eff_call_minigame, 0, 0, 0, 500)
    WAIT_MS(3000)
    CALL(evt_mario_set_enable_key, 352)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(yuunpc_go)
    CALL(evt_yuucounter_alloc, 0, 70, 2, 701)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

USER_FUNC(yuunpc_go) {
    yuunpc_wp_flag_off(1);
    return EVT_RETURN_DONE;
}

static s32 evt_nomal_cange(EventEntry* event, BOOL isFirstCall) {
    MarioWork* mario = marioGetPtr();
    if (isFirstCall) {
        unk_8009f9c8();
        if (mario->flags & 0x100000) {
            motSlitCancel3();
        }
    }
    if (!marioRollKeyDisable()) {
        return EVT_RETURN_BLOCK;
    }
    return (mario->dispFlags & 0x01000000) ? 0 : 2;
}

EVT_BEGIN(minigame04_end)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_cam3d_evt_set_now)
    CALL(evt_mario_set_enable_key, 0)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE2"), 0)
    SET_VALUE(GW(0), 1)
    SET_VALUE(GF(0), 1)
    SET_VALUE(LW(3), 1)
    BEGIN_INLINE_EVENT
        IF_INT_LESS(LW(0), 701)
            LOOP(0)
                CALL(evt_mario_get_name_hitobj_ride, LW(0))
                IF_STR_NOT_EQUAL(LW(0), 0)
                    BREAK_LOOP
                END_IF
                IF_INT_EQUAL(GF(0), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER(LW(1), -100)
                IF_INT_NOT_EQUAL(GF(0), 0)
                    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                    IF_INT_LESS(LW(0), 1650)
                        CALL(evt_mario_normalize)
                        CALL(evt_mario_mov_pos2, 1650, LW(2), 120)
                    END_IF
                END_IF
            ELSE
                CALL(evt_mario_wait_rideon)
                CALL(evt_mario_normalize)
            END_IF
        ELSE
            CALL(evt_mario_wait_rideon)
            CALL(evt_mario_normalize)
        END_IF
    END_INLINE_EVENT
    IF_INT_GREATER_OR_EQUAL(LW(0), 701)
        CALL(eff_call_minigame, 2, 0, 0, 500)
        WAIT_MS(4000)
    ELSE
        CALL(eff_call_minigame, 1, 0, 0, 500)
        WAIT_MS(4000)
    END_IF
    CALL(evt_yuucounter_free)
    CALL(yuugijou_montemedal_disp_onoff, 1)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 1, 0, LW(0), 0, LW(1), 701)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(yuuranking_get_monte_31_text_51C0, LW(3))
    CALL(yuugijou_add_montemedal, 0, LW(3))
    CALL(yuugijou_montemedal_wait)
    CALL(evt_yuuranking_free_31_text_5274)
    WAIT_MS(200)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 1, 1, LW(0), 0, LW(1), 701)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(evt_yuuranking_free_31_text_5274)
    SET_VALUE(LW(1), 10)
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(0), 1)
    ELSE
        CALL(evt_msg_select, 0, STRING("sysyuu_03_00"))
    END_IF
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GSWF(1229), 1)
        CALL(evt_mario_set_prev_party_pos, 0, 0, -1000, 0)
        CALL(evt_mario_set_prev_party_pos, 1, 0, -1000, 0)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_bero_mapchange, STRING("gor_03"), STRING("A_dokan_02"))
        RETURN
    END_IF
    MULTIPLY(LW(1), -1)
    CALL(yuugijou_add_montemedal, 0, LW(1))
    CALL(yuugijou_montemedal_wait)
    WAIT_MS(200)
    CALL(yuugijou_montemedal_disp_onoff, 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_sfxoff, GW(10))
    CALL(evt_fade_out, 1000)
    CALL(evt_cam3d_evt_off, 0, 11)
    SET_VALUE(GF(0), 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(yuu_03_kaijo_init2)
    CALL(evt_cam_road_reset)
    CALL(unk_yuu_0000371c)
    WAIT_MS(500)
    CALL(re_init_yuunpc)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(evt_fade_in, 1000)
    RETURN
EVT_END()

EVT_BEGIN(yuu_03_init_evt)
    CALL(evt_map_playanim, STRING("yajirushi"), 1, 0)
    CALL(evt_map_playanim, STRING("tama"), 1, 0)
    CALL(evt_map_playanim, STRING("gurade"), 1, 0)
    CALL(evt_map_playanim, STRING("kyaku"), 1, 0)
    SPAWN_EVENT(minigame04_main)
    CALL_CHILD(evt_yuunpc_minigame4)
    RETURN
EVT_END()

USER_FUNC(yuuminigame4_opening_on_off) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    Yuuminigame4Data* data = work->data;
    if (evtGetValue(event, args[1]) != 0) {
        data->flags |= 1;
    } else {
        data->flags &= ~1;
    }
    return EVT_RETURN_DONE;
}

// TODO: type
// (globals declared above)

USER_FUNC(yuuminigame4_opening_chk) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    Yuuminigame4Data* data = work->data;
    if (data->flags & 1) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

static void polygon(f32 x, f32 y, f32 w, f32 h, f32 u, f32 v) {
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(u, 0.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(u, v);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, v);
}

// NONMATCHING (88%): ryushi_disp; needs register/allocation scheduling work
void ryushi_disp(CameraId cameraId, void* param) {
    RyushiEntry* wp = xwp;
    Mtx mtx;
    Mtx rot;
    Mtx scale;
    Mtx trans;
    Mtx out;
    GXTexObj texObj;
    GXColor color;
    f32 f29 = 0.0f;
    f32 f30;
    f32 f31;
    s32 i;

    PSMTXScale(mtx, wp->scale, wp->scale, 1.0f);
    PSMTXTransApply(mtx, mtx, wp->x, wp->y, wp->z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
    PSMTXConcat(mtx, rot, mtx);
    effGetTexObj(0x1E, &texObj);
    GXLoadTexObj(&texObj, 0);
    color.r = wp->unk38;
    color.g = wp->unk3C;
    color.b = wp->unk40;
    color.a = 0;
    GXSetTevColor(2, color);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(2);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 8, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetTevOrder(1, 0xFF, 0xFF, 0xFF);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 2, 4, 0, 0xF);
    GXSetTevAlphaIn(1, 7, 1, 0, 7);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    f31 = 0.0f;
    wp = &xwp[1];
    for (i = 1; i < ryuwp; i++, wp++) {
        f30 = wp->scale;
        if (f30 == f31) {
            continue;
        }
        color.r = (s32)f29;
        color.g = xwp->unk30;
        color.b = xwp->unk34;
        color.a = wp->unk20;
        GXSetTevColor(1, color);
        {
            f32 t = (3.1415927f * (f32)i * 0.25f + wp->z * 0.125f) / 3.1415927f;
            f32 d = 180.0f * t;
            s32 k = (s32)(d / 360.0f);
            if (d < 0.0f) {
                k -= 1;
            }
            {
                f32 a = d - (f32)(k * 360);
                s32 idx = (s32)a;
                f32 s = *(f32*)&yuusintbl[idx];
                f29 = 3.0f;
                PSMTXTrans(trans, f29 * s + wp->x, wp->y, wp->z);
            }
        }
        PSMTXScale(scale, f30, f30, f30);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(mtx, trans, trans);
        PSMTXConcat(camGetPtr(cameraId)->view, trans, out);
        GXLoadPosMtxImm(out, 0);
        GXSetCurrentMtx(0);
        polygon(-8.0f, -8.0f, 16.0f, 16.0f, 1.0f, 1.0f);
    }
}

static f32 randf(f32 min, f32 max) {
    f32 d = max - min;
    f32 r = (f32)rand() / 32767.0f;
    return r * d + min;
}

USER_FUNC(evt_ryushi_init) {
    RyushiEntry* wp;
    s32 i;
    ryuwp = 0xF1;
    wp = _mapAlloc(0x4004);
    xwp = wp;
    wp->unk04 = 0;
    wp->unk24 = 0;
    wp->unk20 = 0;
    wp->x = 0.0f;
    wp->y = 0.0f;
    wp->z = 0.0f;
    wp->scale = 1.0f;
    wp->unk2C = 0xFF;
    wp->unk30 = 0;
    wp->unk34 = 0xFF;
    wp->unk38 = 0xFF;
    wp->unk3C = 0x80;
    wp->unk40 = 0xFF;
    wp = &wp[1];
    for (i = 0xF0; i != 0; i--) {
        wp->unk0 = 0;
        wp++;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (89.63%): evt_ryushi_clr; reload xwp global for unk08/0C/10 matched; randf fneg order + r27/r28 still differ
USER_FUNC(evt_ryushi_clr) {
    RyushiEntry* wp = xwp;
    s32 i;
    s32 n = ryuwp;

    wp->unk04 = evtGetValue(event, event->args[0]);
    wp->unk24 = 0;
    wp->unk20 = 0;
    wp->x = 0.0f;
    wp->y = 0.0f;
    wp->z = 0.0f;
    xwp->unk08 = 50.0f;
    xwp->unk0C = 50.0f;
    xwp->unk10 = 100.0f;
    wp = &wp[1];
    for (i = 1; i < n; i++, wp++) {
        s32 a = (i - 1) % 10;
        s32 b = ((i - 1) / 10) % 6;
        s32 c = (i - 1) / 60;

        wp->unk08 = xwp->unk08 * 0.5f * ((b + c) & 1) + ((f32)a * xwp->unk08 - xwp->unk08 * 4.5f);
        wp->unk0C = (f32)b * xwp->unk0C - xwp->unk0C * 2.5f;
        wp->unk10 = (f32)c * xwp->unk10 - xwp->unk10 * 1.5f;
        wp->x = wp->unk08 + randf(-(0.3f * xwp->unk08), 0.3f * xwp->unk08);
        wp->y = wp->unk0C + randf(-(0.3f * xwp->unk08), 0.3f * xwp->unk08);
        wp->z = wp->unk10 + randf(-(0.3f * xwp->unk08), 0.3f * xwp->unk08);
        wp->scale = 0.5f;
    }
    return EVT_RETURN_DONE;
}












// NONMATCHING (77.7%): evt_ryushi; needs register/allocation scheduling work
USER_FUNC(evt_ryushi) {
    YuuminigameWork* work2 = (YuuminigameWork*)event->lwData[0];
    Yuuminigame4Data* data = work2->data;
    RyushiEntry* wp = xwp;
    Vec pos = { 0.0f, 0.0f, 0.0f };
    f32 f20;
    f32 f21;
    f32 f22;
    f32 f23 = 1.0f;
    s32 i;

    pos.x = wp->x;
    pos.y = wp->y;
    pos.z = wp->z;

    f22 = 0.0f;
    f21 = f22;
    f20 = camGetPtr(4)->cameraAt.x;
    switch (data->unk10) {
    case 2:
    case 3:
    case 4:
        f22 = 2.9166667f;
        f21 = 1.0f;
        break;
    case 6:
    case 7:
    case 8:
        f22 = 2.9166667f;
        f21 = 1.0f;
        break;
    }
    switch (data->unk10) {
    case 1:
    case 5:
        f21 = (f32)(0x3C - data->unk14) / 60.0f;
        f23 = f21;
        break;
    case 2:
    case 6:
        f22 = f22 * (f32)(0x3C - data->unk14) / 60.0f;
        break;
    case 4:
    case 8:
        f21 = (f32)data->unk14 / 48.0f;
        break;
    }

    wp->unk24++;
    if (wp->unk24 <= 0x14) {
        wp->unk20 = wp->unk24 * 12;
    } else {
        wp->unk20 = 0xFF;
    }
    if (wp->unk24 & 1) {
        wp->unk20 /= 2;
    }
    f23 *= 0.5f;

    for (i = 1; i < ryuwp; i++) {
        RyushiEntry* e = &wp[i];

        switch (e->unk04) {
        case 0:
            e->z -= f22;
            if (e->z < -200.0f) {
                e->x = e->unk08 + randf(-0.3f * wp->unk08, 0.3f * wp->unk08);
                e->y = e->unk0C + randf(-0.3f * wp->unk0C, 0.3f * wp->unk0C);
                e->z = 200.0f + randf(-0.3f * wp->unk10, 0.3f * wp->unk10);
            }
            break;
        case 1:
            e->z += f22;
            if (e->z > 200.0f) {
                e->x = e->unk08 + randf(-0.3f * wp->unk08, 0.3f * wp->unk08);
                e->y = e->unk0C + randf(-0.3f * wp->unk0C, 0.3f * wp->unk0C);
                e->z = -200.0f + randf(-0.3f * wp->unk10, 0.3f * wp->unk10);
            }
            break;
        }
        {
            f32 step = 10.0f * wp->unk08;
            f32 t = (e->x - f20) / (step * 0.5f);
            s32 n = (s32)t;
            if (n != 0) {
                if (n > 0) {
                    n += 1;
                } else {
                    n -= 1;
                }
                n = -n;
                n /= 2;
                e->x = e->x + (f32)n * step;
            }
        }
        {
            f32 a = e->z;
            if (a < 0.0f) {
                a = -a;
            }
            a = a - 100.0f;
            if (a < 0.0f) {
                a = 0.0f;
            } else if (a > 100.0f) {
                a = 100.0f;
            }
            e->unk20 = (s32)(0.7 * (255.0f * ((100.0f - a) / 100.0f) * f21));
        }
        e->scale = f23;
    }

    dispEntry(4, dispCalcZ(&pos), 2, ryushi_disp, NULL);
    return EVT_RETURN_DONE;
}

void yuuminigame_init_4slit(EventEntry* event, BOOL isFirstCall) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    Yuuminigame4Data* data = work->data;
    RyushiEntry* wp;
    s32 i;

    data->subEvent = evtEntryType(unk_evt_yuu_00015fe0, 0, 0, 0);
    data->subEvent->lwData[0] = event->lwData[0];
    data->flags |= 1;

    ryuwp = 0xF1;
    wp = _mapAlloc(0x4004);
    xwp = wp;
    wp->unk04 = 0;
    wp->unk24 = 0;
    wp->unk20 = 0;
    wp->x = 0.0f;
    wp->y = 0.0f;
    wp->z = 0.0f;
    wp->scale = 1.0f;
    wp->unk2C = 0xFF;
    wp->unk30 = 0;
    wp->unk34 = 0xFF;
    wp->unk38 = 0xFF;
    wp->unk3C = 0x80;
    wp->unk40 = 0xFF;
    wp = &wp[1];
    for (i = 0xF0; i != 0; i--) {
        wp->unk0 = 0;
        wp++;
    }
}

void yuuminigame_main_4slit(EventEntry* event, BOOL isFirstCall) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    Yuuminigame4Data* data = work->data;

    switch (data->phase) {
    case 0:
        break;
    case 5:
        data->phase = 1;
        /* fallthrough */
    case 1:
        work->childEvent = evtEntry(minigame04_start, 0, 0);
        work->childEvent->lwData[0] = event->lwData[0];
        data->phase++;
        break;
    case 2:
        data->unk10 = 0;
        data->unk14 = (irand(0xF3D) + 100) * 0x3C / 1000;
        data->unk0C = 0;
        data->phase = 3;
        /* fallthrough */
    case 3:
        if (yuuminigame4_gamerule(event, isFirstCall, work, data) != 0) {
            data->phase++;
        }
        break;
    case 4:
        work->childEvent = evtEntry(minigame04_end, 0, 0);
        work->childEvent->lwData[0] = data->unk08;
        switch (data->unk08) {
        case 0x0:
            data->unk0C = 0x105;
            break;
        case 0x1:
            data->unk0C = 0x103;
            break;
        case 0x2:
            data->unk0C = 0x101;
            break;
        case 0x15:
        case 0x41:
        case 0xA:
        case 0x20:
        case 0x2B:
        case 0x36:
        case 0x4C:
        case 0x57:
        case 0x62:
        case 0x6E:
        case 0xDD:
        case 0x14C:
        case 0x1BB:
        case 0x22A:
        case 0x299:
        case 0x308:
        case 0x377:
        case 0x3E6:
        case 0x456:
        case 0x8AD:
        case 0xD04:
        case 0x115B:
        case 0x15B2:
        case 0x1A09:
        case 0x1E60:
        case 0x22B7:
        case 0x270E:
            data->unk0C = 0x103;
            break;
        }
        work->childEvent->lwData[1] = data->unk0C;
        data->phase++;
        break;
    }

    mapObjRotate("puro_1", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_2", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_3", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_4", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_5", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_6", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_7", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_8", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_9", 0.0f, 0.0f, data->unk18);
    mapObjRotate("puro_10", 0.0f, 0.0f, data->unk18);
    mapObjRotate("pera_1", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_2", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_3", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_4", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_5", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_6", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_7", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_8", 0.0f, 0.0f, data->unk1C);
    mapObjRotate("pera_9", 0.0f, 0.0f, data->unk1C);
}

void yuuminigame_disp_4slit(CameraId cameraId, void* param) {
    monteCountDisp();
}

// NONMATCHING (96.12%): yuuminigame4_gamerule; needs register/allocation scheduling work
s32 yuuminigame4_gamerule(EventEntry* event, BOOL isFirstCall, void* work, Yuuminigame4Data* data) {
    MarioWork* mario = marioGetPtr();
    EventEntry* e;
    f32 f;

    data->unk14--;

    switch (data->unk10) {
    case 0:
        if (data->unk14 != 0) {
            break;
        }
        if (irand(2) != 0) {
            data->unk10 = 1;
            e = evtEntryType(ryushi_event, 0, 0, 0);
            e->lwData[0] = (s32)work;
            e->lwData[1] = 1;
        } else {
            data->unk10 = 5;
            e = evtEntryType(ryushi_event, 0, 0, 0);
            e->lwData[0] = (s32)work;
            e->lwData[1] = 0;
        }
        data->unk14 = 0x3C;
        break;
    case 1:
        if (data->unk14 == 0) {
            data->unk10 = 2;
            data->unk14 = 0x3C;
        }
        /* fallthrough */
    case 2:
        if (data->unk14 != 0) {
            break;
        }
        data->unk10 = 3;
        data->unk14 = (irand(0x7D1) + 0xBB8) * 0x3C / 1000;
        break;
    case 3:
        if (data->unk14 != 0) {
            break;
        }
        data->unk10 = 4;
        data->unk14 = 0x30;
        break;
    case 4:
        if (data->unk14 != 0) {
            break;
        }
        data->unk10 = 0;
        data->unk14 = (irand(0xF3D) + 100) * 0x3C / 1000;
        evtSetValue(event, 0xFD050F80, 1);
        break;
    case 5:
        if (data->unk14 != 0) {
            break;
        }
        data->unk10 = 6;
        data->unk14 = 0x3C;
        break;
    case 6:
        if (data->unk14 != 0) {
            break;
        }
        data->unk10 = 7;
        data->unk14 = (irand(0x7D1) + 0xBB8) * 0x3C / 1000;
        break;
    case 7:
        if (data->unk14 != 0) {
            break;
        }
        data->unk10 = 8;
        data->unk14 = 0x30;
        break;
    case 8:
        if (data->unk14 != 0) {
            break;
        }
        data->unk10 = 0;
        data->unk14 = (irand(0xF3D) + 100) * 0x3C / 1000;
        evtSetValue(event, 0xFD050F80, 1);
        break;
    }

    switch (data->unk10) {
    case 0:
        break;
    case 1:
    case 2:
        data->unk18 = data->unk18 + 36.0f * (120.0f - (f32)(data->unk14 - 1)) / 120.0f;
        data->unk18 = (data->unk18 + 3600.0f) - (f32)((s32)((data->unk18 + 3600.0f) / 360.0f) * 360);
        break;
    case 3:
        data->unk18 = data->unk18 + 36.0f;
        data->unk18 = (data->unk18 + 3600.0f) - (f32)((s32)((data->unk18 + 3600.0f) / 360.0f) * 360);
        break;
    case 4:
        data->unk18 = data->unk18 + 36.0f * (f32)(data->unk14 - 1) / 48.0f;
        data->unk18 = (data->unk18 + 3600.0f) - (f32)((s32)((data->unk18 + 3600.0f) / 360.0f) * 360);
        break;
    case 5:
    case 6:
        data->unk1C = data->unk1C + 36.0f * (120.0f - (f32)(data->unk14 - 1)) / 120.0f;
        data->unk1C = (data->unk1C + 3600.0f) - (f32)((s32)((data->unk1C + 3600.0f) / 360.0f) * 360);
        break;
    case 7:
        data->unk1C = data->unk1C + 36.0f;
        data->unk1C = (data->unk1C + 3600.0f) - (f32)((s32)((data->unk1C + 3600.0f) / 360.0f) * 360);
        break;
    case 8:
        data->unk1C = data->unk1C + 36.0f * (f32)(data->unk14 - 1) / 48.0f;
        data->unk1C = (data->unk1C + 3600.0f) - (f32)((s32)((data->unk1C + 3600.0f) / 360.0f) * 360);
        break;
    }

    if (mario->position.x < 1500.0f) {
        f = 0.0f;
        switch (data->unk10) {
        case 2:
        case 3:
        case 4:
            if (mario->dispFlags & 0x01000000) {
                if (mario->flags & 0x00100000) {
                    f = 0.0f;
                }
            } else {
                f = 2.9166667f;
            }
            break;
        case 6:
        case 7:
        case 8:
            if (mario->dispFlags & 0x01000000) {
                if (mario->flags & 0x00100000) {
                    f = -0.0f;
                } else {
                    f = -0.0f;
                }
            } else {
                f = -2.9166667f;
            }
            break;
        }
        if (data->unk10 == 2 || data->unk10 == 6) {
            f = f * (f32)(0x3C - data->unk14) / 60.0f;
        }
        mario->riderDelta.z += f;
    }

    if (mario->position.y <= -100.0f || mario->position.x >= 1500.0f) {
        if (mario->position.y <= -100.0f) {
            data->unk08 = 0x2BD;
            yuucounter_flag_on(1);
        } else {
            data->unk08 = yuucounter_get_data();
        }
        data->unk10 = 0;
        data->unk14 = (irand(0xF3D) + 100) * 0x3C / 1000;
        marioKeyOff();
        partyKeyOff();
        yuucounter_flag_on(2);
        return 1;
    }
    return 0;
}

// clang-format on
