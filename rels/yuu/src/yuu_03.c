#include "yuu_03.h"
#include "cam_shift.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
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

// named .rodata constants the original object defines
const u32 zero_yuu_00014160 = 0;
const u32 zero_yuu_00014164 = 0;
const Vec vec3_yuu_00014168 = { 0.0f, 0.0f, 0.0f };

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

// .bss emission order is the reverse of declaration order; reverse the four globals so the
// emitted layout matches the original: yuuminigame4_entries@0, xwp@4, ryuwp@8, yuunpc_wp@0xC.
static void* yuunpc_wp;
s32 ryuwp;
RyushiEntry* xwp;
Yuuminigame4Work* yuuminigame4_entries;

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
USER_FUNC(minigame04_cam_reset) {
    camShiftReset();
    return EVT_RETURN_DONE;
}

EVT_BEGIN(yuuminigame4_entry_event)
    CALL(yuuminigame4_entry_init)
    RETURN
EVT_END()
void yuuminigame4_entry_disp(CameraId cameraId, void* param) {
    Yuuminigame4Work* entries = param;
    s32 i;
    if (entries != NULL) {
        // The icon bobs during the first 9 frames of each 32-frame cycle.
        for (i = 0; i < entries->count; i++) {
            if (entries[i].hidden == 0) {
                Vec iconPos = entries[i].field_0x8.position;
                if ((gp->retraceCount & 0x1F) < 9) {
                    iconPos.y += 5.0f;
                }
                iconDispGx(1.0f, &iconPos, 0, 0x1B6);
            }
        }
    }
}

#ifdef MUST_MATCH
// Match-only helper: MWCC colours an inlined helper's locals in declaration order, and assigning the
// work global inside the loop condition keeps its reload per iteration (and the base pointer in r5).
static inline void yuu4_init_work(MarioWork* mario) {
    s32 i;
    Yuuminigame4WorkField8* entryFields;
    Yuuminigame4Work* entries;
    MarioWork* marioData;
    marioData = mario;
    for (i = 0; i < (entries = yuuminigame4_entries)->count; i++) {
        entryFields = &entries[i].field_0x8;
        entryFields->position = marioData->position;
        entryFields->position.y += 40.0f;
        entryFields->field_0x10 = 1.0f;
    }
}
#endif
#pragma opt_propagation off // keep count*sizeof as the target's li/mulli sequence
USER_FUNC(yuuminigame4_entry_init) {
    s32 wasAllocated = 0;
    s32 entryCount;
    YuuminigameWork* work2 = (YuuminigameWork*)event->lwData[0];
    MarioWork* mario = marioGetPtr();
    if (isFirstCall) {
        s32 i;
        if (yuuminigame4_entries != NULL) {
            wasAllocated = 1;
        }
        entryCount = 1;
        yuuminigame4_entries = _mapAlloc(entryCount * sizeof(Yuuminigame4Work));
        memset(yuuminigame4_entries, 0, entryCount * sizeof(Yuuminigame4Work));
        yuuminigame4_entries->count = entryCount;
        yuuminigame4_entries->work = work2;
#ifdef MUST_MATCH
        yuu4_init_work(mario);
#else
        for (i = 0; i < yuuminigame4_entries->count; i++) {
            Yuuminigame4Work* entry = &yuuminigame4_entries[i];
            entry->field_0x8.position = mario->position;
            entry->field_0x8.position.y += 40.0f;
            entry->field_0x8.field_0x10 = 1.0f;
        }
#endif
    }
    {
        s32 i;
        for (i = 0; i < yuuminigame4_entries->count; i++) {
            Yuuminigame4Work* entry = &yuuminigame4_entries[i];
            switch (entry->state) {
            case 0:
                entry->field_0x8.position = mario->position;
                entry->field_0x8.position.y += 40.0f;
                break;
            case 1:
                break;
            }
        }
    }
    if (wasAllocated == 0) {
        dispEntry(4, 0.0f, 1, yuuminigame4_entry_disp, yuuminigame4_entries);
    }
    return EVT_RETURN_BLOCK;
}
#pragma opt_propagation reset













































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
    Yuuminigame4Work* entries = yuuminigame4_entries;
    if (entries != NULL) {
        YuuminigameWork* work2 = entries->work;
        Yuuminigame4Data* data = work2->data;
        if (data->subEvent != NULL) {
            evtDeleteID(data->subEvent->eventId);
            data->subEvent = NULL;
            _mapFree(yuuminigame4_entries);
            data->subEvent = evtEntryType(yuuminigame4_entry_event, 0, 0, 0);
            data->subEvent->lwData[0] = (s32)yuuminigame4_entries->work;
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
    CALL(minigame04_cam_reset)
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

// Passing the base colour by value puts the built colour and its by-value copy in the original's stack slots.
static inline GXColor ryushiColor0(GXColor c, u32 r, u32 g, u32 b) {
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}

static inline GXColor ryushiColor1(u32 r, u32 g, u32 b, u32 a) {
    GXColor c = *(GXColor*)&zero_yuu_00014164;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

// The colour helpers and the angle expression are written to reproduce the original's stack slots and
// instruction schedule; the plain forms are under #else (see the comments in the body).
void ryushi_disp(CameraId cameraId, void* param) {
    RyushiEntry* entry = xwp;
    s32 colorG = xwp->tev0G;
    s32 colorB = xwp->tev0B;
    Mtx trans;
    Mtx rot;
    Mtx scale;
    Mtx out;
    Mtx mtx;
    GXTexObj texObj;
    f32 zeroScale;
    f32 entryScale;
    f32 swayAmplitude;
    s32 i;

    PSMTXScale(mtx, entry->scale, entry->scale, 1.0f);
    PSMTXTransApply(mtx, mtx, entry->x, entry->y, entry->z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
    PSMTXConcat(mtx, rot, mtx);
    effGetTexObj(0x1E, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetTevColor(2, ryushiColor0(*(GXColor*)&zero_yuu_00014160, entry->tev1R, entry->tev1G, entry->tev1B));
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

    zeroScale = 0.0f;
    entry = &entry[1];
    for (i = 1; i < ryuwp; i++, entry++) {
        entryScale = entry->scale;
        if (entryScale == zeroScale) {
            continue;
        }
        GXSetTevColor(1, ryushiColor1((s32)swayAmplitude, colorG, colorB, entry->alpha));
        {
            // The target computes these as reciprocals of powers of two; the divisions schedule the constant
            // loads the way the original does. The result is identical to z*0.125 and 3.14*i*0.25.
            f32 t = (entry->z / 8.0f + 3.14f * (f32)i / 4.0f) / 3.14f;
            f32 d = 180.0f * t;
            s32 k = (s32)(d / 360.0f);
            if (d < 0.0f) {
                k -= 1;
            }
            {
                s32 idx;
                f32 s;
                d = d - (f32)(k * 360);
                idx = (s32)d;
                s = yuusintbl[idx];
                swayAmplitude = 3.0f;
                PSMTXTrans(trans, swayAmplitude * s + entry->x, entry->y, entry->z);
            }
        }
        PSMTXScale(scale, entryScale, entryScale, entryScale);
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
    wp->direction = 0;
    wp->age = 0;
    wp->alpha = 0;
    wp->x = 0.0f;
    wp->y = 0.0f;
    wp->z = 0.0f;
    wp->scale = 1.0f;
    wp->unk2C = 0xFF;
    wp->tev0G = 0;
    wp->tev0B = 0xFF;
    wp->tev1R = 0xFF;
    wp->tev1G = 0x80;
    wp->tev1B = 0xFF;
    wp = &wp[1];
    for (i = 0xF0; i != 0; i--) {
        wp->unk0 = 0;
        wp++;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_ryushi_clr) {
    s32* args = event->args;
    RyushiEntry* entry = xwp;
    s32 i;
    s32 entryCount = ryuwp;
    f32 half = 0.5f;

    entry->direction = evtGetValue(event, *args++);
    entry->age = 0;
    entry->alpha = 0;
    entry->x = 0.0f;
    entry->y = 0.0f;
    entry->z = 0.0f;
    xwp->spacingX = 50.0f;
    xwp->spacingY = 50.0f;
    xwp->spacingZ = 100.0f;
    entry = &entry[1];
    for (i = 1; i < entryCount; i++, entry++) {
        s32 gridX;
        s32 gridY;
        s32 gridZ;
        f32 hi;
        f32 lo;
        f32 zSpacing;

        entry->unk0 = 0;
        gridX = (i - 1) % 10;
        gridY = ((i - 1) / 10) % 6;
        gridZ = (i - 1) / 60;
        entry->homeX = xwp->spacingX * half * ((gridY + gridZ) & 1) + ((f32)gridX * xwp->spacingX - xwp->spacingX * 4.5f);
        entry->homeY = (f32)gridY * xwp->spacingY - xwp->spacingY * 2.5f;
        entry->homeZ = (f32)gridZ * xwp->spacingZ - xwp->spacingZ * 1.5f;
        hi = 0.3f * xwp->spacingX;
        lo = 0.3f * -xwp->spacingX;
        entry->x = entry->homeX + randf(lo, hi);
        hi = 0.3f * xwp->spacingY;
        lo = 0.3f * -xwp->spacingY;
        entry->y = entry->homeY + randf(lo, hi);
        // Folding the negation through `zSpacing` makes MWCC emit both
        // 0.3f products before the rand call (the natural form defers the max product).
        zSpacing = xwp->spacingZ;
        hi = 0.3f * zSpacing;
        zSpacing = -zSpacing;
        lo = 0.3f * zSpacing;
        entry->z = entry->homeZ + randf(lo, hi);
        entry->scale = 0.5f;
    }
    return EVT_RETURN_DONE;
}












// inline Vec build reproduces the target's vec3->temp->pos copy (rule 7)
static inline Vec makeVecB(f32 x, f32 y, f32 z) {
    Vec v = vec3_yuu_00014168;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

// `opt_lifetimes off` is needed: with the default pass the persistent floats land in a
// different callee-saved block (f28-f31 instead of f20-f23) and the frame grows by 16 bytes.
#pragma opt_lifetimes off
USER_FUNC(evt_ryushi) {
    YuuminigameWork* work2;
    Yuuminigame4Data* data;
    RyushiEntry* emitter = xwp;
    Vec emitterPos;
    // Allocation-steering locals. MWCC colours callee-saved FP registers by the order
    // locals are declared, and the inlined randf scratches need names to land in
    // f24/f25. The declaration order below is load-bearing: it is what puts the
    // persistent floats in f20-f23. `half` keeps the 0.5 constant in a variable so the
    // `step * 0.5f` multiply is emitted with the variable operand first.
    f32 t2;
    f32 t0;
    f32 t3;
    f32 entryScale;
    f32 half;
    f32 t1;
    f32 zSpeed;
    f32 alphaFactor;
    f32 cameraX;
    RyushiEntry* entry;
    s32 i;

    emitterPos = makeVecB(emitter->x, emitter->y, emitter->z);

    zSpeed = 0.0f;
    alphaFactor = zSpeed;
    entryScale = 1.0f;
    work2 = (YuuminigameWork*)event->lwData[0];
    data = work2->data;
    cameraX = camGetPtr(4)->cameraAt.x;
    switch (data->state) {
    case 2:
    case 3:
    case 4:
        zSpeed = 2.9166667f;
        alphaFactor = 1.0f;
        break;
    case 6:
    case 7:
    case 8:
        zSpeed = 2.9166667f;
        alphaFactor = 1.0f;
        break;
    }
    switch (data->state) {
    case 1:
    case 5:
        alphaFactor = (f32)(0x3C - data->stateTimer) / 60.0f;
        entryScale = alphaFactor;
        break;
    case 2:
    case 6:
        zSpeed = zSpeed * (f32)(0x3C - data->stateTimer) / 60.0f;
        break;
    case 4:
    case 8:
        alphaFactor = (f32)data->stateTimer / 48.0f;
        break;
    }

    emitter->age++;
    {
        s32 age = emitter->age;
        if (age <= 0x14) {
            emitter->alpha = age * 12;
        } else {
            emitter->alpha = 0xFF;
        }
        if (age & 1) {
            emitter->alpha /= 2;
        }
    }
    half = 0.5f;
    entryScale = half * entryScale;

    {
    entry = &emitter[1];
    for (i = 1; i < ryuwp; i++, entry++) {
        switch (xwp->direction) {
        case 0:
            entry->z -= zSpeed;
            if (entry->z < -200.0f) {
                t1 = 0.3f * xwp->spacingX;
                t0 = 0.3f * -xwp->spacingX;
                entry->x = entry->homeX + randf(t0, t1);
                t2 = 0.3f * xwp->spacingY;
                t1 = 0.3f * -xwp->spacingY;
                entry->y = entry->homeY + randf(t1, t2);
                t2 = 0.3f * xwp->spacingZ;
                t3 = 0.3f * -xwp->spacingZ;
                entry->z = 200.0f + randf(t3, t2);
            }
            break;
        case 1:
            entry->z += zSpeed;
            if (entry->z > 200.0f) {
                t0 = 0.3f * xwp->spacingX;
                t3 = 0.3f * -xwp->spacingX;
                entry->x = entry->homeX + randf(t3, t0);
                t0 = 0.3f * xwp->spacingY;
                t1 = 0.3f * -xwp->spacingY;
                entry->y = entry->homeY + randf(t1, t0);
                t2 = 0.3f * xwp->spacingZ;
                t1 = 0.3f * -xwp->spacingZ;
                entry->z = -200.0f + randf(t1, t2);
            }
            break;
        }
        {
            f32 step = 10.0f * xwp->spacingX;
            f32 dx = entry->x;
            f32 cellOffset;
            s32 wrapOffset;

            dx -= cameraX;
            cellOffset = dx / (step * half);
            wrapOffset = (s32)cellOffset;
            if (wrapOffset != 0) {
                if (wrapOffset > 0) {
                    wrapOffset += 1;
                } else {
                    wrapOffset -= 1;
                }
                wrapOffset = -wrapOffset;
                wrapOffset /= 2;
                entry->x += (f32)wrapOffset * step;
            }
        }
        {
            f32 fadeDistance;
            if (entry->z < 0.0f) {
                fadeDistance = -entry->z;
            } else {
                fadeDistance = entry->z;
            }
            fadeDistance = fadeDistance - 100.0f;
            if (fadeDistance < 0.0f) {
                fadeDistance = 0.0f;
            } else if (fadeDistance > 100.0f) {
                fadeDistance = 100.0f;
            }
            entry->alpha = (s32)(0.7 * (255.0f * ((100.0f - fadeDistance) / 100.0f) * alphaFactor));
        }
        entry->scale = entryScale;
    }
    }

    dispEntry(4, dispCalcZ(&emitterPos), 2, ryushi_disp, NULL);
    return EVT_RETURN_DONE;
}

#pragma opt_lifetimes reset

void yuuminigame_init_4slit(EventEntry* event, BOOL isFirstCall) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    Yuuminigame4Data* data = work->data;
    RyushiEntry* entry;
    s32 i;

    data->subEvent = evtEntryType(yuuminigame4_entry_event, 0, 0, 0);
    data->subEvent->lwData[0] = event->lwData[0];
    data->flags |= 1;

    ryuwp = 0xF1;
    entry = _mapAlloc(0x4004);
    xwp = entry;
    entry->direction = 0;
    entry->age = 0;
    entry->alpha = 0;
    entry->x = 0.0f;
    entry->y = 0.0f;
    entry->z = 0.0f;
    entry->scale = 1.0f;
    entry->unk2C = 0xFF;
    entry->tev0G = 0;
    entry->tev0B = 0xFF;
    entry->tev1R = 0xFF;
    entry->tev1G = 0x80;
    entry->tev1B = 0xFF;
    entry = &entry[1];
    for (i = 0xF0; i != 0; i--) {
        entry->unk0 = 0;
        entry++;
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
        data->state = 0;
        data->stateTimer = (irand(0xF3D) + 100) * 0x3C / 1000;
        data->mult = 0;
        data->phase = 3;
        /* fallthrough */
    case 3:
        if (yuuminigame4_gamerule(event, isFirstCall, work, data) != 0) {
            data->phase++;
        }
        break;
    case 4:
        work->childEvent = evtEntry(minigame04_end, 0, 0);
        work->childEvent->lwData[0] = data->counterData;
        switch (data->counterData) {
        case 0x0:
            data->mult = 0x105;
            break;
        case 0x1:
            data->mult = 0x103;
            break;
        case 0x2:
            data->mult = 0x101;
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
            data->mult = 0x103;
            break;
        }
        work->childEvent->lwData[1] = data->mult;
        data->phase++;
        break;
    }

    mapObjRotate("puro_1", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_2", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_3", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_4", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_5", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_6", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_7", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_8", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_9", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("puro_10", 0.0f, 0.0f, data->puroAngle);
    mapObjRotate("pera_1", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_2", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_3", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_4", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_5", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_6", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_7", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_8", 0.0f, 0.0f, data->peraAngle);
    mapObjRotate("pera_9", 0.0f, 0.0f, data->peraAngle);
}

void yuuminigame_disp_4slit(CameraId cameraId, void* param) {
    monteCountDisp();
}

s32 yuuminigame4_gamerule(EventEntry* event, BOOL isFirstCall, void* work, Yuuminigame4Data* data) {
    MarioWork* mario = marioGetPtr();
    EventEntry* childEvent;
    f32 riderDeltaZ;

    data->stateTimer--;

    // Timed state machine: states 1-4 drive puroAngle, states 5-8 drive peraAngle.
    switch (data->state) {
    case 0:
        if (data->stateTimer != 0) {
            break;
        }
        if (irand(2) != 0) {
            data->state = 1;
            childEvent = evtEntryType(ryushi_event, 0, 0, 0);
            childEvent->lwData[0] = (s32)work;
            childEvent->lwData[1] = 1;
        } else {
            data->state = 5;
            childEvent = evtEntryType(ryushi_event, 0, 0, 0);
            childEvent->lwData[0] = (s32)work;
            childEvent->lwData[1] = 0;
        }
        data->stateTimer = 0x3C;
        break;
    case 1:
        if (data->stateTimer == 0) {
            data->state = 2;
            data->stateTimer = 0x3C;
        }
        /* fallthrough */
    case 2:
        if (data->stateTimer != 0) {
            break;
        }
        data->state = 3;
        data->stateTimer = (irand(0x7D1) + 0xBB8) * 0x3C / 1000;
        break;
    case 3:
        if (data->stateTimer != 0) {
            break;
        }
        data->state = 4;
        data->stateTimer = 0x30;
        break;
    case 4:
        if (data->stateTimer != 0) {
            break;
        }
        data->state = 0;
        data->stateTimer = (irand(0xF3D) + 100) * 0x3C / 1000;
        evtSetValue(event, GW(0), 1);
        break;
    case 5:
        if (data->stateTimer != 0) {
            break;
        }
        data->state = 6;
        data->stateTimer = 0x3C;
        break;
    case 6:
        if (data->stateTimer != 0) {
            break;
        }
        data->state = 7;
        data->stateTimer = (irand(0x7D1) + 0xBB8) * 0x3C / 1000;
        break;
    case 7:
        if (data->stateTimer != 0) {
            break;
        }
        data->state = 8;
        data->stateTimer = 0x30;
        break;
    case 8:
        if (data->stateTimer != 0) {
            break;
        }
        data->state = 0;
        data->stateTimer = (irand(0xF3D) + 100) * 0x3C / 1000;
        evtSetValue(event, GW(0), 1);
        break;
    }

    // Spin the active set of map objects (puroAngle: puro_*, peraAngle: pera_*) and wrap the angle to [0, 360).
    switch (data->state) {
    case 0:
        break;
    case 1:
    case 2:
        data->puroAngle = data->puroAngle + 36.0f * (120.0f - (f32)(data->stateTimer - 1)) / 120.0f;
        data->puroAngle = (data->puroAngle + 3600.0f) - (f32)((s32)((data->puroAngle + 3600.0f) / 360.0f) * 360);
        break;
    case 3:
        data->puroAngle += 36.0f;
        data->puroAngle = (data->puroAngle + 3600.0f) - (f32)((s32)((data->puroAngle + 3600.0f) / 360.0f) * 360);
        break;
    case 4:
        data->puroAngle = data->puroAngle + 36.0f * (f32)(data->stateTimer - 1) / 48.0f;
        data->puroAngle = (data->puroAngle + 3600.0f) - (f32)((s32)((data->puroAngle + 3600.0f) / 360.0f) * 360);
        break;
    case 5:
    case 6:
        data->peraAngle = data->peraAngle + 36.0f * (120.0f - (f32)(data->stateTimer - 1)) / 120.0f;
        data->peraAngle = (data->peraAngle + 3600.0f) - (f32)((s32)((data->peraAngle + 3600.0f) / 360.0f) * 360);
        break;
    case 7:
        data->peraAngle += 36.0f;
        data->peraAngle = (data->peraAngle + 3600.0f) - (f32)((s32)((data->peraAngle + 3600.0f) / 360.0f) * 360);
        break;
    case 8:
        data->peraAngle = data->peraAngle + 36.0f * (f32)(data->stateTimer - 1) / 48.0f;
        data->peraAngle = (data->peraAngle + 3600.0f) - (f32)((s32)((data->peraAngle + 3600.0f) / 360.0f) * 360);
        break;
    }

    // States 2-4 and 6-8 push Mario along z at the particle stream speed (ramping up in states 2 and 6).
    if (mario->position.x < 1500.0f) {
        s32 state = data->state;
        riderDeltaZ = 0.0f;
        switch (state) {
        case 2:
        case 3:
        case 4:
            if (mario->dispFlags & 0x01000000) {
                if (mario->flags & 0x00100000) {
                    riderDeltaZ = 0.0f;
                } else {
                    riderDeltaZ = 0.0f;
                }
            } else {
                riderDeltaZ = 2.9166667f;
            }
            break;
        case 6:
        case 7:
        case 8:
            if (mario->dispFlags & 0x01000000) {
                if (mario->flags & 0x00100000) {
                    riderDeltaZ = -0.0f;
                } else {
                    riderDeltaZ = -0.0f;
                }
            } else {
                riderDeltaZ = -2.9166667f;
            }
            break;
        }
        switch (state) {
        case 2:
        case 6:
            riderDeltaZ = riderDeltaZ * (f32)(0x3C - data->stateTimer) / 60.0f;
            break;
        }
        mario->riderDelta.z += riderDeltaZ;
    }

    if (mario->position.y <= -100.0f || mario->position.x >= 1500.0f) {
        if (mario->position.y <= -100.0f) {
            data->counterData = 0x2BD;
            yuucounter_flag_on(1);
        } else {
            data->counterData = yuucounter_get_data();
        }
        data->state = 0;
        data->stateTimer = (irand(0xF3D) + 100) * 0x3C / 1000;
        marioKeyOff();
        partyKeyOff();
        yuucounter_flag_on(2);
        return 1;
    }
    return 0;
}

// clang-format on
