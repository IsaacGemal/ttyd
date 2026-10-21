#include "tik_00.h"
#include "evt/evt_badgeshop.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_johoya.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_movefloor.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_shop.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_uranaisi.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "eff/eff_mahojin.h"
#include "memory.h"
#include "pmario_sound.h"
#include "manager/evtmgr_cmd.h"
#include "driver/npcdrv.h"
#include "mario/mario.h"
#include "party.h"
#include <stdio.h>
#include <string.h>
#include "math_ext.h"
#include "driver/mapdrv.h"

// Work area for the fortune-teller (uranai) card rendering. Only the fields
// reached by this file are named; the rest is the copy of the event's LW/GW
// area that uranai_init_func stores here (0x0..0xBF).
typedef struct UranaiWork {
    u8 pad_0x0[0xC0];
    smartEntry* buffer;     // 0xC0, render target from smartAlloc
    GXTexObj texObj;        // 0xC4
    s32 field_0xE4;         // 0xE4, texture valid flag
    EventEntry* field_0xE8; // 0xE8, the evt_uranai_cap event
    s32 field_0xEC;         // 0xEC
    s32 field_0xF0;         // 0xF0
    s32 field_0xF4;         // 0xF4, viewport width (low 16 bits)
    s32 field_0xF8;         // 0xF8, viewport height (low 16 bits)
} UranaiWork;

#pragma pool_data off

#pragma force_active on
extern s32 tik_villager_urouro[];
static USER_FUNC(scale_func);
static USER_FUNC(evt_osreport);
static USER_FUNC(ss_set);
static USER_FUNC(ss_check);
static USER_FUNC(setmertn);
s32 kurihakase_event[221];

// Declared in the reverse of the original's .bss address order (MWCC lays
// uninitialised globals out last-declared-first): ssf at +0x8, uranai_wp at
// +0x4, uranaisi_msgp at +0x0.
static s32 ssf[1];
UranaiWork* uranai_wp;
char* uranaisi_msgp;

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

static USER_FUNC(setmertn) {
    s32 name = evtGetValue(event, event->args[0]);

    event->ownerNPC = npcNameToPtr((const char*)name);
    return EVT_RETURN_DONE;
}

#ifdef MUST_MATCH
// Match-only: reading ssf through a volatile pointer inside an inline makes MWCC emit the target's
// int->float register allocation (value in r5, conversion double address in r6, event->args in r4).
static inline f32 ss_check_to_f(volatile s32* p) { return (f32)*p; }
#endif
static USER_FUNC(ss_check) {
#ifdef MUST_MATCH
    evtSetFloat(event, event->args[0], ss_check_to_f(ssf));
#else
    evtSetFloat(event, event->args[0], ssf[0]);
#endif
    return EVT_RETURN_DONE;
}

static USER_FUNC(ss_set) {
    ssf[0] = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_uranaisi_msg_free) {
    if (uranaisi_msgp == NULL) {
        return EVT_RETURN_DONE;
    }
    _mapFree(uranaisi_msgp);
    uranaisi_msgp = NULL;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_uranaisi_msg_alloc) {
    uranaisi_msgp = _mapAlloc(0x100);
    memset(uranaisi_msgp, 0, 0x100);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_uranaisi_msg_set) {
    s32* args = event->args;
    const char* msg = msgSearch((const char*)evtGetValue(event, args[0]));

    sprintf(uranaisi_msgp, msg, uranaisi_ryokin_rtn(0), uranaisi_ryokin_rtn(1), uranaisi_ryokin_rtn(2));
    evtSetValue(event, args[1], (s32)uranaisi_msgp);
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_osreport) {
    s32* args = event->args;

    switch (evtGetValue(event, args[0])) {
    case 0:
        evtSetValue(event, args[1], (s32)search_evt_moji(evtGetValue(event, 0xF5DE0180)));
        break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(uranai_init_func) {
    char name[64];
    s32 i;

    uranai_wp = _mapAlloc(0xFC);
    memset(uranai_wp, 0, 0xFC);
    memcpy(uranai_wp, event->lwData, 0x40);
    memcpy((u8*)uranai_wp + 0x40, evtGetWork()->gwData, 0x80);
    for (i = 0; i < 9; i++) {
        sprintf(name, "card_omote_%d", i + 1);
        if (mapGetMapObj(name) != NULL) {
            mapGetMapObj(name)->cameraId = 5;
        }
        sprintf(name, "card_ura_%d", i + 1);
        if (mapGetMapObj(name) != NULL) {
            mapGetMapObj(name)->cameraId = 5;
        }
    }
    uranai_wp->field_0xE8 = evtEntry(evt_uranai_cap, 0, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(uranai_exit_func) {
    memcpy(event->lwData, uranai_wp, 0x40);
    memcpy(evtGetWork()->gwData, (u8*)uranai_wp + 0x40, 0x80);
    evtDelete(uranai_wp->field_0xE8);
    _mapFree(uranai_wp);
    return EVT_RETURN_DONE;
}

static BeroEntry bero_entry_data[4] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_07", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_01", "w_bero_1", 1, 1},
    {"tensou", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tik_00", "tensou", 1, 1},
    {NULL},
};

static char* obj_list[12] = {
    "sina_1", "a_sina1", "sina_2", "a_sina_2", "sina_3", "a_sina_3", "sina_4", "a_sina_4", "sina_5",
    "a_sina_5", "sina_6", "a_sina_6",
};

// TODO: type
static u32 goods_list[12] = {
    0x0000009D, 0x0000000F, 0x0000009E, 0x0000000F,
    0x00000096, 0x000000C8, 0x0000009C, 0x000000C8,
    0x0000007E, 0x0000006E, 0x0000007F, 0x0000015E,
};

// TODO: type
static u32 trade_list[24] = {
    0x0000009D, 0x00070000, 0x0000009E, 0x00070000,
    0x00000096, 0x00320000, 0x0000009C, 0x00320000,
    0x0000007E, 0x00640000, 0x0000007F, 0x012C0000,
    0x000000D5, 0x00190000, 0x000000CB, 0x00140000,
    0x000000CA, 0x00080000, 0x000000D1, 0x000F0000,
    0x000000C4, 0x00280000, 0x00000000, 0x00000000,
};

static char* shopper_data[38] = {
    "店員", NULL, NULL, "tik_shop_00", "tik_shop_01", "tik_shop_02", "tik_shop_03", "tik_shop_04",
    "tik_shop_05", "tik_shop_06", "tik_shop_07", "tik_shop_08", "tik_shop_09", "tik_shop_10",
    "tik_shop_11", "tik_shop_12", "tik_shop_13", "tik_shop_14", "tik_shop_15", "tik_shop_16",
    "tik_shop_17", "tik_shop_18", "tik_shop_19", "tik_shop_20", "tik_shop_21", "tik_shop_22",
    "tik_shop_23", "tik_shop_24", "tik_shop_25", "tik_shop_26", "tik_shop_27", "tik_shop_28",
    "tik_shop_29", "shop_30", "shop_31", "shop_32", "shop_33", "shop_34",
};

EVT_BEGIN(uranaisi_next_evt)
    IF_INT_LESS(GSW(0), 405)
        CALL(uranaisi_data_alloc, 0, STRING("un_next"))
        CALL(uranaisi_next_check, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(uranaisi_next_makelabel, 0, LW(1))
            CALL(evt_msg_print, 0, LW(1), 0, STRING("me"))
        ELSE
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_win_coin_on, 0, LW(12))
                CALL(evt_osreport, 0, LW(1))
                CALL(evt_msg_print, 1, STRING("ゆうこうな　データが\n見つかりませんでした\nメッセージを　とうろく　してください\n<key>\n<scrl_auto>\n<once_stop>\n"), 0, STRING("me"))
                CALL(evt_msg_print_add_insert, 1, STRING("げんざいの　イベントばんごうは\n『%s』\nです\n<key>"), LW(1))
                CALL(uranaisi_ryokin, 0, LW(2))
                CALL(evt_pouch_add_coin, LW(2))
                CALL(evt_win_coin_wait, LW(12))
                WAIT_MS(200)
                CALL(evt_win_coin_off, LW(12))
            ELSE
                CALL(evt_win_coin_on, 0, LW(12))
                CALL(evt_osreport, 0, LW(1))
                CALL(evt_msg_print, 1, STRING("ゆうこうな　データが\nじゅうふくして　みつかりました\nゆうこうな　メッセージは　１つにしてください\n<key>\n<scrl_auto>\n<once_stop>"), 0, STRING("me"))
                SET_VALUE(LW(2), 0)
                SET_VALUE(LW(3), LW(0))
                LOOP(LW(0))
                    CALL(uranaisi_next_makelabel, LW(2), LW(1))
                    CALL(evt_msg_print_add_insert, 1, STRING("『%s』\n<key>\n<scrl_auto>\n<once_stop>\n"), LW(1))
                    ADD(LW(2), 1)
                END_LOOP
                CALL(evt_msg_print_add_insert, 1, STRING("いじょう　%dこが\nじゅうふくしている　メッセージです\n<key>"), LW(3))
                CALL(uranaisi_ryokin, 0, LW(2))
                CALL(evt_pouch_add_coin, LW(2))
                CALL(evt_win_coin_wait, LW(12))
                WAIT_MS(200)
                CALL(evt_win_coin_off, LW(12))
            END_IF
        END_IF
        CALL(uranaisi_data_free)
    ELSE
        CALL(evt_msg_print, 0, STRING("uranaisi_17"), 0, STRING("me"))
        CALL(evt_win_coin_on, 0, LW(12))
        WAIT_MS(500)
        CALL(uranaisi_ryokin, 0, LW(2))
        CALL(evt_pouch_add_coin, LW(2))
        CALL(evt_win_coin_wait, LW(12))
        WAIT_MS(500)
        CALL(evt_win_coin_off, LW(12))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(uranaisi_starpiece_evt)
    CALL(uranaisi_data_alloc, 1, STRING("un_starpiece"))
    CALL(uranaisi_starpiece_check, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_win_coin_on, 0, LW(12))
        CALL(evt_msg_print, 0, STRING("uranaisi_14"), 0, STRING("me"))
        CALL(uranaisi_ryokin, 1, LW(2))
        CALL(evt_pouch_add_coin, LW(2))
        CALL(evt_win_coin_wait, LW(12))
        WAIT_MS(200)
        CALL(evt_win_coin_off, LW(12))
    ELSE
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(0))
        CALL(uranaisi_starpiece_makelabel, LW(0), LW(0))
        CALL(evt_msg_print, 0, LW(0), 0, STRING("me"))
        CALL(uranaisi_starpiece_setreadflag)
    END_IF
    CALL(uranaisi_data_free)
    RETURN
EVT_END()

EVT_BEGIN(uranaisi_supercoin_evt)
    CALL(uranaisi_data_alloc, 2, STRING("un_supercoin"))
    CALL(uranaisi_supercoin_check, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_win_coin_on, 0, LW(12))
        CALL(evt_msg_print, 0, STRING("uranaisi_16"), 0, STRING("me"))
        CALL(uranaisi_ryokin, 2, LW(2))
        CALL(evt_pouch_add_coin, LW(2))
        CALL(evt_win_coin_wait, LW(12))
        WAIT_MS(200)
        CALL(evt_win_coin_off, LW(12))
    ELSE
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(0))
        CALL(uranaisi_supercoin_makelabel, LW(0), LW(0))
        CALL(evt_msg_print, 0, LW(0), 0, STRING("me"))
        CALL(uranaisi_supercoin_setreadflag)
    END_IF
    CALL(uranaisi_data_free)
    RETURN
EVT_END()

// TODO: type
static u32 name_tbl[18] = {
    (u32)"card_1", 0x00000000, (u32)"card_2", 0x00000001,
    (u32)"card_3", 0x00000002, (u32)"card_4", 0x00000003,
    (u32)"card_5", 0x00000001, (u32)"card_6", 0x00000000,
    (u32)"card_7", 0x00000003, (u32)"card_8", 0x00000002,
    (u32)"card_9", 0x00000001,
};

// Named zero/unit vectors used by uranai_cap. Defined as real .rodata objects
// (in the target's order) so the code reaches them by name instead of the
// compiler pooling anonymous compound-literal templates in the wrong slots.
static const Vec vec3_tik_00008358 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_tik_00008364 = { 0.0f, 1.0f, 0.0f };
static const Vec vec3_tik_00008370 = { 0.0f, 0.0f, 0.0f };

EVT_BEGIN(evt_uranai_cap)
    CALL(uranai_cap_evt)
    RETURN
EVT_END()

void uranai_cap(CameraId cameraId, void* param) {
    CameraEntry cam;
    f32 viewport[6];
    f32 proj[7];
    Mtx44 persp;
    Mtx lookMtx;
    Mtx mtx;
    Vec min;
    Vec max;
    Vec camPos;
    Vec camUp;
    Vec camTarget;
    AnimationPoseData* anim;
    NpcEntry* npc;
    f32 ratio;
#ifdef MUST_MATCH
    // Match-only: a named 0.5f local and an f32 tan result reproduce the original's float
    // register allocation in the tan/camPos.z block (tan in f4, 0.5f in f0).
    f32 tanValue;
    f32 half;
#else
    f64 tanValue;
#endif
    u16 width;
    u16 height;
    u32 left;
    u32 top;
    u32 wd;
    u32 ht;

    camGetPtr(CAMERA_3D);
    width = (u16)uranai_wp->field_0xF4;
    height = (u16)uranai_wp->field_0xF8;
    GXGetViewportv(viewport);
    GXGetProjectionv(proj);
    GXGetScissor(&left, &top, &wd, &ht);
    GXSetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
    GXSetScissor(1, 1, width - 2, height - 2);
    C_MTXPerspective(persp, 25.0f, (f32)(width / height), 1.0f, 1000.0f);
    GXSetProjection(persp, GX_PERSPECTIVE);
    camTarget = vec3_tik_00008370;
    camTarget.y = (f32)height / 3.0f;
    camUp = vec3_tik_00008364;
    camPos = vec3_tik_00008358;
    camPos.y = camTarget.y;
#ifdef MUST_MATCH
    half = 0.5f;
    tanValue = (f32)tan(0.21816615760326385);
    camPos.z = 0.7f * ((f32)height * half / tanValue);
#else
    tanValue = tan(0.21816615760326385);
    camPos.z = 0.7f * ((f32)height * 0.5f / (f32)tanValue);
#endif
    C_MTXLookAt(lookMtx, &camPos, &camUp, &camTarget);
    cam = *camGetPtr(cameraId);
    npc = npcNameToPtr("うらない師");
    anim = animPoseGetAnimBaseDataPtr(npc->poseId);
    min = (Vec){ anim->bboxMin.x, anim->bboxMin.y, anim->bboxMin.z };
    max = (Vec){ anim->bboxMax.x, anim->bboxMax.y, anim->bboxMax.z };
    ratio = (f32)width / (2.0f * (max.x - min.x));
    PSMTXCopy(lookMtx, camGetCurPtr()->view);
    animPosePeraOff(npc->poseId);
    PSMTXIdentity(mtx);
    animPoseDrawMtx(npc->poseId, mtx, 1, 0.0f, ratio);
    animPoseDrawMtx(npc->poseId, mtx, 2, 0.0f, ratio);
    animPoseDrawMtx(npc->poseId, mtx, 3, 0.0f, ratio);
    animPosePeraOn(npc->poseId);
    PSMTXCopy(cam.view, camGetCurPtr()->view);
    uranai_wp->buffer = smartAlloc(GXGetTexBufferSize(width, height, 5, 0, 0), HEAP_EFFECT);
    GXSetTexCopySrc(0, 0, width, height);
    GXSetTexCopyDst(width, height, 5, 0);
    GXCopyTex(uranai_wp->buffer->address, 1);
    GXPixModeSync();
    GXInitTexObj(&uranai_wp->texObj, uranai_wp->buffer->address, width, height, 5, 0, 0, 0);
    GXInitTexObjLOD(&uranai_wp->texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
    GXSetProjectionv(proj);
    GXSetScissor(left, top, wd, ht);
    uranai_wp->field_0xE4 = 1;
}

USER_FUNC(uranai_cap_evt) {
    dispEntry(CAMERA_SHADOW, 0.0f, 3, uranai_cap, NULL);
    return EVT_RETURN_BLOCK;
}

void card_tev(MapTevCallback* param) {
    if (uranai_wp->field_0xE4 != 0) {
        GXLoadTexObj(smartTexObj(&uranai_wp->texObj, (void**)uranai_wp->buffer), GX_TEXMAP0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_ZERO, GX_CA_APREV);
        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
        param->tevStage = 1;
        param->texCoord = 1;
        param->texMap = 1;
    }
}

USER_FUNC(uranai_bbox) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    evtGetValue(event, args[3]);
    uranai_wp->field_0xEC = value;
    uranai_wp->field_0xF0 = value;
    uranai_wp->field_0xF4 = value;
    uranai_wp->field_0xF8 = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(card_xlu) {
    const char* name = (const char*)evtGetValue(event, LW(2));

    if (isFirstCall) {
        event->userdata[0] = 0xFF;
    }
    mapGrpSetColor(name, (GXColor){0xFF, 0xFF, 0xFF, event->userdata[0]});
    event->userdata[0] -= 8;
    if (event->userdata[0] < 0) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(card_camid) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    if (mapGetMapObj(name) != NULL) {
        mapGetMapObj(name)->cameraId = value;
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(uranai_supotto_mode) {
    MapObject* obj = mapGetMapObj("supotto");

    obj->renderMode = 10;
    return EVT_RETURN_DONE;
}

USER_FUNC(starmaniac_snd_off) {
    psndSetFlag(0x80);
    return EVT_RETURN_DONE;
}

USER_FUNC(starmaniac_snd_on) {
    psndClearFlag(0x80);
    return EVT_RETURN_DONE;
}

USER_FUNC(mario_key_onoff_wait) {
    marioGetPtr();
    switch (!marioChkKey()) {
    case 0:
        break;
    default:
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}


EVT_BEGIN(uranai_init)
    CALL(uranai_init_func)
    SET_VALUE(GW(15), LW(0))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("supotto"), 1)
    CALL(uranai_supotto_mode)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("uranai_mae"), 6144)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 1000)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_color, 1, STRING("uranai_mae"), LW(0), LW(0), LW(0), 255)
        CALL(evt_mapobj_color, 1, STRING("ie_1_door"), LW(0), LW(0), LW(0), 255)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("uranai_mae"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ie_1_door"), 1)
    CALL(evt_mapobj_trans, 1, STRING("card"), 0, 0, 0)
    SET_INT_READ_ADDR(PTR(name_tbl))
    LOOP(9)
        READ_INT2(LW(0), LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 6144)
        CALL(evt_mapobj_trans, 1, LW(0), 0, -1000, 0)
    END_LOOP
    CALL(evt_map_set_tevcallback, 1, PTR(card_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("card_omote_5"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("card_omote_5"), 256)
    CALL(evt_offscreen_entry, STRING("ofs"))
    CALL(evt_npc_set_offscreen, STRING("うらない師"), STRING("ofs"))
    CALL(evt_offscreen_get_boundingbox, STRING("ofs"), LW(0), LW(1), LW(2), LW(3))
    CALL(uranai_bbox, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_offscreen_delete, STRING("ofs"))
    CALL(evt_npc_set_offscreen, STRING("うらない師"), 0)
    RETURN
EVT_END()





EVT_BEGIN(uranai_main)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_snd_sfxon, STRING("SFX_EVT_FORTUNE_CARD_MOVE1"), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, -1300, 1300, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_mapobj_trans, 1, STRING("card_1"), 0, -200, 0)
        CALL(evt_mapobj_rotate, 1, STRING("card_1"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("card_1"), 0, 180, 0)
        IF_FLOAT_GREATER(LW(0), FLOAT(-20.0f))
            CALL(evt_npc_flag_onoff, 1, STRING("うらない師"), 128)
        END_IF
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
    SET_INT_READ_ADDR(PTR(name_tbl))
    LOOP(9)
        READ_INT2(LW(0), LW(1))
        SWITCH(LW(1))
            CASE_EQUAL(0)
            SET_VALUE(LW(2), 500)
            SET_VALUE(LF(0), 0)
            CASE_EQUAL(1)
            SET_VALUE(LW(2), -500)
            SET_VALUE(LF(0), 0)
            CASE_EQUAL(2)
            SET_VALUE(LW(2), 500)
            SET_VALUE(LF(0), 1)
            CASE_EQUAL(3)
            SET_VALUE(LW(2), -500)
            SET_VALUE(LF(0), 1)
        END_SWITCH
        BEGIN_INLINE_EVENT
            WAIT_MS(100)
            CALL(evt_mapobj_get_position, LW(0), LW(10), LW(11), LW(12))
            CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_FORTUNE_CARD_MOVE2"), LW(10), LW(11), LW(12), 0)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(9), LW(0))
            MULTIPLY_FLOAT(LW(2), FLOAT(10.0f))
            CALL(evt_sub_intpl_msec_init, 11, LW(2), 0, 360)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
                IF_INT_EQUAL(LF(0), 0)
                    CALL(evt_mapobj_trans, 1, LW(9), LW(8), 0, LW(0))
                ELSE
                    CALL(evt_mapobj_trans, 1, LW(9), LW(8), LW(0), 0)
                END_IF
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_INLINE_EVENT
        ADD_FLOAT(LW(8), FLOAT(0.2f))
        WAIT_MS(300)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_FORTUNE_CARD_MOVE3"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 600, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
        SET_INT_READ_ADDR(PTR(name_tbl))
        LOOP(9)
            READ_INT2(LW(2), LW(3))
            CALL(evt_mapobj_trans, 1, LW(2), LW(8), -100, 0)
            CALL(evt_mapobj_rotate, 1, LW(2), LW(0), 0, 0)
            CALL(evt_mapobj_trans, 1, LW(2), LW(8), 100, 0)
            ADD_FLOAT(LW(8), FLOAT(0.2f))
        END_LOOP
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 600, -600, 1500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(9), FLOAT(0.0f))
        SET_INT_READ_ADDR(PTR(name_tbl))
        LOOP(9)
            READ_INT2(LW(2), LW(3))
            SET_FLOAT_VALUE(LW(6), FLOAT(60.0f))
            SET_FLOAT_VALUE(LW(7), LW(9))
            MULTIPLY_FLOAT(LW(7), FLOAT(15.0f))
            SUBTRACT_FLOAT(LW(6), LW(7))
            CALL(evt_mapobj_trans, 1, LW(2), LW(8), -100, 0)
            IF_FLOAT_LESS(LW(0), LW(6))
                CALL(evt_mapobj_rotate, 1, LW(2), LW(6), 0, 0)
            ELSE
                CALL(evt_mapobj_rotate, 1, LW(2), LW(0), 0, 0)
            END_IF
            CALL(evt_mapobj_trans, 1, LW(2), LW(8), 100, 0)
            ADD_FLOAT(LW(8), FLOAT(0.2f))
            ADD_FLOAT(LW(9), FLOAT(1.0f))
        END_LOOP
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_FLOAT_VALUE(LW(8), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(9), FLOAT(0.0f))
    SET_INT_READ_ADDR(PTR(name_tbl))
    LOOP(9)
        READ_INT2(LW(2), LW(3))
        SET_FLOAT_VALUE(LW(6), FLOAT(60.0f))
        SET_FLOAT_VALUE(LW(7), LW(9))
        MULTIPLY_FLOAT(LW(7), FLOAT(15.0f))
        SUBTRACT_FLOAT(LW(6), LW(7))
        BEGIN_INLINE_EVENT
            IF_INT_EQUAL(LW(9), 4)
                CALL(evt_snd_sfxon, STRING("SFX_EVT_FORTUNE_CARD_MOVE4"), 0)
                CALL(evt_sub_intpl_msec_init, 0, 0, 500, 1000)
            ELSE
                CALL(evt_sub_intpl_msec_init, 1, 0, -3000, 1000)
            END_IF
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
                CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
                CALL(evt_mapobj_trans, 1, LW(2), LW(8), -100, 0)
                CALL(evt_mapobj_rotate, 1, LW(2), LW(6), 0, 0)
                CALL(evt_mapobj_trans, 1, LW(2), LW(8), 100, 0)
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_INLINE_EVENT
        ADD_FLOAT(LW(8), FLOAT(0.2f))
        ADD_FLOAT(LW(9), FLOAT(1.0f))
    END_LOOP
    WAIT_MS(1200)
    CALL(card_camid, STRING("card_omote_5"), 4)
    CALL(card_camid, STRING("card_ura_5"), 4)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(50.0f))
    SET_FLOAT_VALUE(GW(2), FLOAT(0.0f))
    SET_FLOAT_VALUE(GW(3), FLOAT(0.0f))
    CALL(evt_snd_sfxon, STRING("SFX_EVT_FORTUNE_CARD_MOVE5"), 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(9))
        LOOP(0)
            CALL(evt_mapobj_trans, 1, STRING("card_5"), GW(0), GW(1), GW(2))
            CALL(evt_mapobj_rotate, 1, STRING("card_5"), 0, GW(3), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 13, 500, 700, 300)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            SET_FLOAT_VALUE(GW(1), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 12, 700, -200, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            SET_FLOAT_VALUE(GW(1), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, -450, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            SET_FLOAT_VALUE(GW(2), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 37800, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            SET_FLOAT_VALUE(GW(3), LW(0))
            IF_FLOAT_GREATER(GW(3), FLOAT(3600.0f))
                CALL(evt_mapobj_flag_onoff, 1, 1, STRING("card_omote_5"), 1)
                CALL(evt_npc_flag_onoff, 0, STRING("うらない師"), 128)
            END_IF
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(2500)
    DELETE_EVENT(LW(9))
    SET_VALUE(LW(0), GW(15))
    RETURN
EVT_END()


EVT_BEGIN(uranai_exit)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("uranai_mae"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_1_door"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_color, 1, STRING("uranai_mae"), LW(0), LW(0), LW(0), 255)
        CALL(evt_mapobj_color, 1, STRING("ie_1_door"), LW(0), LW(0), LW(0), 255)
        SET_VALUE(LW(2), 255)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_color, 1, STRING("supotto"), 255, 255, 255, LW(2))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("supotto"), 1)
    CALL(evt_mapobj_color, 1, STRING("supotto"), 255, 255, 255, 255)
    SET_INT_READ_ADDR(PTR(name_tbl))
    LOOP(9)
        READ_INT2(LW(0), LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 6144)
    END_LOOP
    CALL(uranai_exit_func)
    RETURN
EVT_END()


EVT_BEGIN(uranaisi_table_talk)
    CALL(setmertn, STRING("うらない師"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
    CALL(evt_npc_set_ry, STRING("me"), 180)
    LOOP(0)
        IF_INT_EQUAL(GSWF(6), 0)
            IF_INT_EQUAL(GSWF(5), 0)
                CALL(evt_msg_print, 0, STRING("uranaisi_00"), 0, STRING("me"))
                SET_VALUE(GSWF(5), 1)
                SET_VALUE(GSWF(237), 1)
            ELSE
                IF_INT_EQUAL(GSWF(237), 0)
                    CALL(evt_msg_print, 0, STRING("uranaisi_01"), 0, STRING("me"))
                    SET_VALUE(GSWF(237), 1)
                ELSE
                    CALL(evt_msg_print, 0, STRING("uranaisi_19"), 0, STRING("me"))
                END_IF
            END_IF
            CALL(evt_msg_select, 0, STRING("uranaisi_03"))
            IF_INT_EQUAL(LW(0), 1)
                CALL(evt_msg_print_add, 0, STRING("uranaisi_06"))
                GOTO(0)
            END_IF
            WAIT_MS(200)
        ELSE
            CALL(evt_msg_print, 0, STRING("uranaisi_02"), 0, STRING("me"))
        END_IF
        SET_VALUE(GSWF(6), 1)
        CALL(evt_win_coin_on, 0, LW(12))
        CALL(evt_uranaisi_msg_alloc)
        CALL(evt_uranaisi_msg_set, STRING("uranaisi_04"), LW(1))
        CALL(evt_msg_select, 1, LW(1))
        CALL(evt_uranaisi_msg_free)
        IF_INT_EQUAL(LW(0), 4)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_msg_print_add, 0, STRING("uranaisi_06"))
            GOTO(0)
        END_IF
        CALL(uranaisi_ryokin, LW(0), LW(2))
        CALL(evt_pouch_get_coin, LW(1))
        IF_INT_LESS(LW(1), LW(2))
            CALL(evt_msg_print_add, 0, STRING("uranaisi_07"))
            CALL(evt_win_coin_off, LW(12))
            GOTO(0)
        END_IF
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_msg_print_add, 0, STRING("uranaisi_08"))
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("uranaisi_09"))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("uranaisi_11"))
        END_SWITCH
        MULTIPLY(LW(2), -1)
        CALL(evt_pouch_add_coin, LW(2))
        CALL(evt_win_coin_wait, LW(12))
        WAIT_MS(200)
        CALL(evt_win_coin_off, LW(12))
        CALL_CHILD(uranai_init)
        CALL(evt_msg_print, 0, STRING("uranaisi_12"), 0, STRING("me"))
        BEGIN_INLINE_EVENT
            CALL(evt_npc_set_anim, STRING("me"), STRING("I_1"))
            WAIT_MS(240)
            CALL(evt_npc_set_anim, STRING("me"), STRING("S_2"))
        END_INLINE_EVENT
        CALL_CHILD(uranai_main)
        CALL(evt_npc_set_anim, STRING("me"), STRING("D_1"))
        WAIT_MS(170)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL_CHILD(uranaisi_next_evt)
            CASE_EQUAL(1)
            CALL_CHILD(uranaisi_starpiece_evt)
            CASE_EQUAL(2)
            CALL_CHILD(uranaisi_supercoin_evt)
        END_SWITCH
        CALL_CHILD(uranai_exit)
    END_LOOP
    LABEL(0)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()





EVT_BEGIN(uranaisi_direct_talk)
    CALL(evt_msg_print, 0, STRING("uranaisi_18"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(uranaisi_init)
    CALL(evt_run_case_evt, 9, 1, STRING("uranai"), 0, PTR(uranaisi_table_talk), 0)
    RETURN
EVT_END()

EVT_BEGIN(starmaniac_init)
    CALL_CHILD(npc_init_evt)
    IF_INT_LESS(GSW(0), 20)
        CALL(evt_npc_set_position, STRING("me"), 400, -20, 175)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(starmaniac_get_event)
    CALL(ss_set, 1)
    RETURN
EVT_END()


EVT_BEGIN(starmaniac_talk)
    CALL(evt_badgeShop_starmaniac_get_kind_cnt, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("starmaniac_02"), 0, STRING("me"))
        RETURN
    END_IF
    CALL(starmaniac_snd_off)
    IF_INT_EQUAL(GSWF(1325), 0)
        CALL(evt_msg_print, 0, STRING("starmaniac_00"), 0, STRING("me"))
        SET_VALUE(GSWF(1325), 1)
    ELSE
        CALL(evt_msg_print, 0, STRING("starmaniac_01"), 0, STRING("me"))
    END_IF
    CALL(evt_msg_select, 0, STRING("starmaniac_03"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("starmaniac_05"))
        CALL(starmaniac_snd_on)
        RETURN
    END_IF
    CALL(evt_pouch_get_starpiece, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("starmaniac_06"))
        CALL(starmaniac_snd_on)
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("starmaniac_07"))
    LABEL(0)
    CALL(evt_win_coin_on, 2, LW(12))
    CALL(evt_win_other_select, 13)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print, 0, STRING("starmaniac_08"), 0, STRING("me"))
        CALL(starmaniac_snd_on)
        RETURN
    END_IF
    CALL(evt_sub_get_language, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_insert, 0, STRING("starmaniac_09"), 0, STRING("me"), LW(3), LW(2), LW(4))
    ELSE
        CALL(evt_msg_print_insert, 0, STRING("starmaniac_09"), 0, STRING("me"), LW(2), LW(3), LW(4))
    END_IF
    CALL(evt_msg_select, 0, STRING("starmaniac_10"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print_add, 0, STRING("starmaniac_11"))
        GOTO(0)
    END_IF
    CALL(evt_pouch_add_item, LW(1), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print_add, 0, STRING("starmaniac_15"))
        CALL(starmaniac_snd_on)
        RETURN
    END_IF
    CALL(evt_badgeShop_starmaniac_dec, LW(1))
    MULTIPLY(LW(3), -1)
    CALL(evt_pouch_add_starpiece, LW(3))
    CALL(evt_win_coin_wait, LW(12))
    WAIT_MS(500)
    CALL(evt_win_coin_off, LW(12))
    CALL(evt_badgeShop_starmaniac_get_kind_cnt, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("starmaniac_12"))
        CALL(starmaniac_snd_on)
        RETURN
    END_IF
    CALL(evt_pouch_get_starpiece, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("starmaniac_12"))
        CALL(starmaniac_snd_on)
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("starmaniac_13"))
    CALL(evt_msg_select, 0, STRING("starmaniac_10"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("starmaniac_14"))
        CALL(starmaniac_snd_on)
        RETURN
    END_IF
    CALL(evt_msg_continue)
    GOTO(0)
    CALL(starmaniac_snd_on)
    RETURN
EVT_END()



EVT_BEGIN(johoyaketi_talk)
    IF_INT_EQUAL(GSWF(3), 0)
        CALL(evt_msg_print, 0, STRING("johoyaketi_00"), 0, STRING("me"))
        SET_VALUE(GSWF(3), 1)
        RETURN
    END_IF
    CALL(johoya_keti_newjoho_check, LW(1))
    IF_INT_EQUAL(LW(1), 0)
        CALL(johoya_keti_oldspeak_check, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print, 0, STRING("johoyaketi_08"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("johoyaketi_06"), 0, STRING("me"))
            SUBTRACT(LW(0), 1)
            CALL(evt_sub_random, LW(0), LW(0))
            CALL(johoya_keti_oldspeak_makelabel, LW(0), LW(0), LW(3), LW(4), LW(5))
            CALL(evt_msg_print_add, 0, LW(0))
            CALL(johoya_keti_oldspeak_setreadflag)
            CALL(evt_msg_print_add, 0, STRING("johoyaketi_07"))
        END_IF
    ELSE
        SUBTRACT(LW(1), 1)
        CALL(evt_sub_random, LW(1), LW(0))
        CALL(johoya_keti_newjoho_makelabel, LW(0), LW(2), LW(3), LW(4), LW(5))
        CALL(evt_msg_print_insert, 0, STRING("johoyaketi_01"), 0, STRING("me"), LW(5), LW(4), LW(3))
        CALL(evt_win_coin_on, 0, LW(12))
        CALL(evt_msg_select, 0, STRING("johoyaketi_02"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_pouch_get_coin, LW(0))
            IF_INT_LESS(LW(0), LW(5))
                CALL(evt_msg_print_add, 0, STRING("johoyaketi_09"))
                CALL(evt_win_coin_off, LW(12))
                RETURN
            END_IF
            MULTIPLY(LW(5), -1)
            CALL(evt_pouch_add_coin, LW(5))
            CALL(evt_win_coin_wait, LW(12))
            WAIT_MS(500)
            CALL(evt_win_coin_off, LW(12))
            CALL(evt_msg_print_add, 0, LW(2))
            CALL(johoya_keti_newjoho_setreadflag)
            CALL(evt_msg_print_add, 0, STRING("johoyaketi_07"))
        ELSE
            CALL(evt_win_coin_off, LW(12))
            CALL(johoya_keti_oldspeak_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print_add, 0, STRING("johoyaketi_04"))
            ELSE
                CALL(evt_msg_print_add, 0, STRING("johoyaketi_05"))
                SUBTRACT(LW(0), 1)
                CALL(evt_sub_random, LW(0), LW(0))
                CALL(johoya_keti_oldspeak_makelabel, LW(0), LW(0), LW(3), LW(4), LW(5))
                CALL(evt_msg_print_add, 0, LW(0))
                CALL(johoya_keti_oldspeak_setreadflag)
                CALL(evt_msg_print_add, 0, STRING("johoyaketi_07"))
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kuribo5_init)
    RETURN
EVT_END()

EVT_BEGIN(kuribo5_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("tik_001_01"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("tik_001_02"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("tik_001_03"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("tik_001_04"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("tik_001_05"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("tik_001_06"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("tik_001_07"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("tik_001_08"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("tik_001_09"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(chusan4_init)
    CALL(evt_npc_set_position, STRING("me"), -175, -19, 235)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

EVT_BEGIN(chusan4_regl)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), 175, 235, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("me"), -175, 235, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(chusan4_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("tik_002_01"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("tik_002_02"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("tik_002_03"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("tik_002_04"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("tik_002_05"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("tik_002_06"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("tik_002_07"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("tik_002_08"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("tik_002_09"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borodo3_init)
    RETURN
EVT_END()

EVT_BEGIN(borodo3_regl)
    CALL_CHILD(tik_villager_urouro)
    RETURN
EVT_END()

EVT_BEGIN(borodo3_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("tik_003_01"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("tik_003_02"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("tik_003_03"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("tik_003_04"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("tik_003_05"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("tik_003_06"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("tik_003_07"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("tik_003_08"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("tik_003_09"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(borodo4_init)
    RETURN
EVT_END()

EVT_BEGIN(borodo4_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("tik_004_01"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("tik_004_02"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("tik_004_03"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("tik_004_04"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("tik_004_05"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("tik_004_06"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("tik_004_07"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("tik_004_08"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("tik_004_09"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(pinkbom_init)
    RETURN
EVT_END()

EVT_BEGIN(pinkbom_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("tik_005_01"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("tik_005_02"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("tik_005_03"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("tik_005_04"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("tik_005_05"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("tik_005_06"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("tik_005_07"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("tik_005_08"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("tik_005_09"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(master_2_init)
    CALL(evt_npc_set_position, STRING("me"), -85, 95, -535)
    RETURN
EVT_END()

EVT_BEGIN(master_2_talk)
    SWITCH(GSW(0))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("tik_005_10"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("tik_005_11"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("tik_005_12"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("tik_005_13"), 0, STRING("me"))
        CASE_LESS(261)
        CALL(evt_msg_print, 0, STRING("tik_005_14"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("tik_005_15"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("tik_005_16"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("tik_005_17"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("tik_005_18"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[253] = {
    (u32)"うらない師", 0x00080600, 0x00000000, (u32)&uranaisi_init,
    0x00000000, (u32)&uranaisi_direct_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"星マニア",
    0x00080600, 0x00000000, (u32)&starmaniac_init, 0x00000000,
    (u32)&starmaniac_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"情報屋ケチ", 0x00080600,
    0x00000000, (u32)npc_init_evt, 0x00000000, (u32)&johoyaketi_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"クリボー５", 0x00080600, 0x00000000,
    (u32)&kuribo5_init, 0x00000000, (u32)&kuribo5_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"チューさん４", 0x00080600, 0x00000000, (u32)&chusan4_init,
    (u32)&chusan4_regl, (u32)&chusan4_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"ボロドー３",
    0x00080600, 0x00000000, (u32)&borodo3_init, (u32)&borodo3_regl,
    (u32)&borodo3_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"ボロドー４", 0x00080600,
    0x00000000, (u32)&borodo4_init, 0x00000000, (u32)&borodo4_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"ピンクボム兵", 0x00080600, 0x00000000,
    (u32)&pinkbom_init, 0x00000000, (u32)&pinkbom_talk, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"マスター２", 0x00080600, 0x00000000, (u32)&master_2_init,
    0x00000000, (u32)&master_2_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"店員",
    0x00080600, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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

static EVT_BEGIN(evt_door_01_00)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("supotto"), 1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("ie_1_door"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_01_00)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
        ELSE
            WAIT_MS(300)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_1_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(300)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_1_1"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_01_01)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(100)
        ELSE
            WAIT_MS(200)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_1_2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_1_2"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_01_02)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(200)
        ELSE
            WAIT_MS(100)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_1_3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_1_3"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static char* door_data_01_npc[2] = {
    "うらない師", NULL,
};

static EVT_BEGIN(evt_wall_01)
    IF_INT_EQUAL(LW(6), 0)
        SPAWN_EVENT(evt_wall_01_00)
        SPAWN_EVENT(evt_wall_01_01)
        SPAWN_EVENT(evt_wall_01_02)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_open_01)
    CALL(evt_mapobj_trans, 1, STRING("card"), 0, -1000, 0)
    CALL(evt_mapobj_trans, 1, STRING("ie_1_naka"), 0, FLOAT(0.2f), 0)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_mobj_check, STRING("pblk1"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("pblk1"), 2)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_close_01)
    CALL(evt_mapobj_trans, 1, STRING("card"), 0, -1000, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("mon"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kuro_maku"), 1)
    CALL(evt_door_end_wait)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("mon"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kuro_maku"), 1)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    CALL(evt_mobj_check, STRING("pblk1"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_mobj_flag_onoff, 0, STRING("pblk1"), 2)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 door_data_01[18] = {
    0x00000002, 0x00000003, 0x00000001, (u32)"ie_1_door",
    (u32)"a_door_1_N", (u32)"a_door_1", (u32)"ie_1_soto", 0x00000000,
    (u32)"ie_1_naka", 0x00000000, (u32)&evt_door_01_00, 0x00000000,
    (u32)&evt_wall_01, (u32)&door_data_01_npc, 0x00000000, 0x00000000,
    (u32)&evt_open_01, (u32)&evt_close_01,
};

static EVT_BEGIN(evt_door_02_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("ie_2_door"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_00)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
        ELSE
            WAIT_MS(300)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_2_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(300)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_2_1"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_01)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(100)
        ELSE
            WAIT_MS(200)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_2_2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_2_2"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_02)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(200)
        ELSE
            WAIT_MS(100)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_2_3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_2_3"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_02_03)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(300)
        ELSE
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_2_4"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(300)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_2_4"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_02)
    IF_INT_EQUAL(LW(6), 0)
        SPAWN_EVENT(evt_wall_02_00)
        SPAWN_EVENT(evt_wall_02_01)
        SPAWN_EVENT(evt_wall_02_02)
        SPAWN_EVENT(evt_wall_02_03)
    END_IF
    RETURN
EVT_END()

static char* mise_npc[2] = {
    "店員", NULL,
};

EVT_BEGIN(evt_open_02)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_close_02)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

// TODO: type
static u32 door_data_02[18] = {
    0x00000002, 0x00000003, 0x00000001, (u32)"ie_2_door",
    (u32)"a_door_2_N", (u32)"a_door_2", (u32)"ie_2_soto", 0x00000000,
    (u32)"ie_2_naka", 0x00000000, (u32)&evt_door_02_00, 0x00000000,
    (u32)&evt_wall_02, (u32)&mise_npc, 0x00000000, 0x00000000,
    (u32)&evt_open_02, (u32)&evt_close_02,
};

static EVT_BEGIN(evt_door_03_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("ie_3_door"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_03_00)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
        ELSE
            WAIT_MS(300)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_3_1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(300)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_3_1"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_03_01)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(100)
        ELSE
            WAIT_MS(200)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_3_2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_3_2"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_03_02)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(200)
        ELSE
            WAIT_MS(100)
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_3_3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(200)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        WAIT_MS(100)
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_3_3"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_03_03)
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 0)
            WAIT_MS(300)
        ELSE
        END_IF
        CALL(evt_mapobj_get_position, STRING("ie_3_4"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(1), 0)
        WAIT_MS(300)
        CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    ELSE
        CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    END_IF
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("ie_3_4"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_03)
    IF_INT_EQUAL(LW(6), 0)
        SPAWN_EVENT(evt_wall_03_00)
        SPAWN_EVENT(evt_wall_03_01)
        SPAWN_EVENT(evt_wall_03_02)
        SPAWN_EVENT(evt_wall_03_03)
    END_IF
    RETURN
EVT_END()

static char* door_data_03_npc[5] = {
    "情報屋ケチ", "ボロドー４", "ピンクボム兵", "マスター２", NULL,
};

EVT_BEGIN(evt_open_03)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

EVT_BEGIN(evt_close_03)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

static char* door_data_03_mobj[2] = {
    "kururin1", NULL,
};

// TODO: type
static u32 door_data_03[18] = {
    0x00000002, 0x00000003, 0x00000001, (u32)"ie_3_door",
    (u32)"a_door_3_N", (u32)"a_door_3", (u32)"ie_3_soto", 0x00000000,
    (u32)"ie_3_naka", 0x00000000, (u32)&evt_door_03_00, 0x00000000,
    (u32)&evt_wall_03, (u32)&door_data_03_npc, (u32)&door_data_03_mobj, 0x00000000,
    (u32)&evt_open_03, (u32)&evt_close_03,
};

static EVT_BEGIN(evt_door_04_00)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.6f))
    CALL(evt_mapobj_rotate, 1, STRING("ie_4door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_01)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("ie_4door"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_04)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("ie_4_1_"), 0, LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_open_04_open)
    SWITCH(GSW(0))
        CASE_LESS(377)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_4_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ie_4_2"), 1)
        IF_INT_EQUAL(GW(10), 0)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("wa_pu"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("wa_pu"), 1)
        END_IF
        CASE_LESS(405)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ie_4_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_4_2"), 1)
        IF_INT_EQUAL(GW(10), 0)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("wa_pu"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("wa_pu"), 1)
        END_IF
        CASE_DEFAULT
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_4_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ie_4_2"), 1)
        IF_INT_EQUAL(GW(10), 0)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("wa_pu"), 1)
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("wa_pu"), 1)
        END_IF
    END_SWITCH
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    IF_INT_LESS(GSW(0), 377)
        IF_INT_EQUAL(GSWF(1329), 0)
            CALL(evt_door_end_wait)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_party_stop, 0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 40)
            CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(120.0f))
            BEGIN_INLINE_EVENT
                CALL(evt_mario_set_dir, 270, 200, 0)
            END_INLINE_EVENT
            CALL(evt_party_set_dir, 0, 90, 200)
            CALL(evt_msg_print_party, STRING("tik_00_tensou_1"))
            SET_VALUE(GSWF(1329), 1)
            CALL(evt_party_run, 0)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    END_IF
    IF_INT_EQUAL(GSW(0), 377)
        CALL(evt_door_end_wait)
        CALL(evt_mario_key_onoff, 0)
        CALL_CHILD(kurihakase_event)
        CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 door_data_04[18] = {
    0x00000000, 0x00000003, 0x00000001, (u32)"ie_4door",
    (u32)"a_door_4_N", (u32)"a_door_4", (u32)"ie_4_soto", 0x00000000,
    (u32)"ie_4_naka", 0x00000000, (u32)&evt_door_04_00, (u32)&evt_door_04_01,
    (u32)&evt_wall_04, 0x00000000, 0x00000000, 0x00000000,
    (u32)&evt_open_04_open, (u32)&evt_door_04_close,
};

EVT_BEGIN(evt_door_stg7_open)
    IF_INT_EQUAL(GSWF(1327), 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("tik_000"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    SPAWN_EVENT(evt_door_open)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_stg7_close)
    SET_VALUE(GSWF(1327), 1)
    SPAWN_EVENT(evt_door_close)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_04_entry)
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 0)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_door_stg7_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(3), 0, PTR(evt_door_stg7_open), 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(tensou_return)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_map_playanim, STRING("anm_wa-p"), 1, 0)
    BEGIN_INLINE_EVENT
        LOOP(10)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("wa_pu"), 1)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_hit_get_position, STRING("A_tensou"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WARP_LIGHT1"), LW(0), LW(1), LW(2), LW(9))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 5, 0, 5760, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            MODULO(LW(0), 360)
            CALL(evt_party_set_dispdir, 0, LW(0))
            ADD(LW(0), 180)
            MODULO(LW(0), 360)
            CALL(evt_mario_set_dispdir, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 12, 0, 100, 800)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(scale_func, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WARP1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
    CALL(evt_party_get_pos, 0, LW(5), LW(6), LW(7))
    CALL(evt_sub_intpl_msec_init, 12, 70, 0, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(8), LW(0))
        ADD(LW(0), LW(3))
        CALL(evt_mario_set_pos, LW(2), LW(0), LW(4))
        ADD(LW(8), LW(6))
        CALL(evt_party_set_pos, 0, LW(5), LW(8), LW(7))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_snd_sfxoff, LW(9))
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_color, 1, STRING("wa_pu"), 255, 255, 255, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("wa_pu"), 1)
    CALL(evt_mapobj_color, 1, STRING("wa_pu"), 255, 255, 255, 255)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_cont_onoff, 1, 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    RETURN
EVT_END()


EVT_BEGIN(tensou_init_event)
    SET_VALUE(GW(10), 0)
    SWITCH(GSW(0))
        CASE_LESS(377)
        CALL(evt_map_playanim, STRING("anm_PC"), 1, 0)
        CALL(evt_map_playanim, STRING("anm_Terop"), 1, 0)
        CALL(evt_map_playanim, STRING("anm_tiktik"), 1, 0)
        CASE_LESS(405)
        CASE_DEFAULT
        CALL(evt_map_playanim, STRING("anm_PC"), 1, 0)
        CALL(evt_map_playanim, STRING("anm_Terop"), 1, 0)
        CALL(evt_map_playanim, STRING("anm_tiktik"), 1, 0)
    END_SWITCH
    IF_INT_EQUAL(GSWF(4175), 0)
        RETURN
    END_IF
    SET_VALUE(GSWF(4175), 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, -305, 38, -250)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_pos, 0, -275, 38, -250)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_party_set_pos, 1, -290, 38, -265)
    CALL(evt_party_set_dir, 1, 270, 0)
    CALL(evt_cam_road_reset)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(0), PTR(door_data_04))
    SPAWN_EVENT(evt_door_open_quick)
    CALL_CHILD(tensou_return)
    SWITCH(GSW(0))
        CASE_LESS(377)
        IF_INT_EQUAL(GSWF(1328), 0)
            WAIT_MS(1500)
            CALL(evt_msg_print_party, STRING("tik_00_tensou_0"))
            SET_VALUE(GSWF(1328), 1)
        END_IF
        CASE_LESS(378)
        WAIT_MS(1000)
        CALL(evt_cam3d_evt_set, -300, 109, 111, -300, 70, -184, 500, 11)
        WAIT_MS(500)
        CALL(evt_party_set_dir, 0, 270, 300)
        WAIT_MS(300)
        CALL(evt_party_set_dir, 0, 90, 300)
        WAIT_MS(300)
        CALL(evt_party_set_dir, 0, 270, 300)
        WAIT_MS(300)
        CALL(evt_mario_set_dir, 90, 300, 0)
        CALL(evt_msg_print_party, STRING("stg7_tik_01"))
        CALL(evt_mario_set_pose, STRING("M_N_1"))
        WAIT_MS(700)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_cam3d_evt_off, 500, 11)
        CASE_DEFAULT
    END_SWITCH
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static USER_FUNC(scale_func) {
    f32 scale = evtGetFloat(event, event->args[0]);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(0);
    Vec v = {0.0f, 0.0f, 0.0f};

    v.x = scale;
    v.y = scale;
    v.z = scale;
    mario->scale = v;
    if (party != NULL) {
        Vec v2 = {0.0f, 0.0f, 0.0f};

        v2.x = scale;
        v2.y = scale;
        v2.z = scale;
        party->field_70 = v2;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(tensou_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_mov_pos2, -305, -250, FLOAT(100.0f))
    CALL(evt_party_move_pos2, 0, -275, -250, FLOAT(100.0f))
    CALL(evt_hit_get_position, STRING("tensou"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WARP_LIGHT1"), LW(0), LW(1), LW(2), LW(9))
    SET_VALUE(GW(0), 0)
    CALL(evt_mario_get_dispdir, LW(0))
    CALL(evt_mario_get_dispdir, LW(1))
    BEGIN_INLINE_EVENT
        LOOP(0)
            ADD(LW(0), GW(0))
            ADD(LW(1), GW(0))
            CALL(evt_mario_set_dispdir, LW(0))
            CALL(evt_party_set_dispdir, 0, LW(1))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WARP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 30, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_cam_off)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
    CALL(evt_party_get_pos, 0, LW(5), LW(6), LW(7))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 12, 100, 0, 800)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(scale_func, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_WARP2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 12, 0, 100, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(8), LW(0))
        ADD(LW(0), LW(3))
        CALL(evt_mario_set_pos, LW(2), LW(0), LW(4))
        ADD(LW(8), LW(6))
        CALL(evt_party_set_pos, 0, LW(5), LW(8), LW(7))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(2000)
    CALL(evt_snd_sfxoff, LW(9))
    SET_VALUE(GSWF(1326), 1)
    CALL(evt_bero_mapchange, STRING("aji_07"), 0)
    RETURN
EVT_END()

EVT_BEGIN(tensou_anim_event)
    SET_VALUE(GW(10), 1)
    CALL(evt_map_playanim, STRING("anm_saisho"), 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("hikari"), 1)
    CALL(evt_map_checkanim, STRING("anm_saisho"), LW(1), LW(2))
    WAIT_MS(LW(2))
    CALL(evt_map_playanim, STRING("anm_wa-p"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(tensou_sw_event)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_SWITCH_WARP1"), 0)
    IF_INT_LESS(GSW(0), 377)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("wa_pu"), 1)
        SPAWN_EVENT(tensou_anim_event)
        CALL(evt_run_case_evt, 0, 1, STRING("tensou"), 0, PTR(tensou_event), 0)
        CALL(evt_exit_case_evt)
    ELSE
        IF_INT_GREATER_OR_EQUAL(GSW(0), 403)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("sui__1"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("sui__2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("wa_pu"), 1)
            SPAWN_EVENT(tensou_anim_event)
            CALL(evt_run_case_evt, 0, 1, STRING("tensou"), 0, PTR(tensou_event), 0)
            CALL(evt_exit_case_evt)
        ELSE
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_msg_toge, 1, 0, 0, 0)
            CALL(evt_msg_print, 0, STRING("stg7_tik_01_01"), 0, 0)
            CALL(evt_mario_key_onoff, 1)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kurihakase_event)
    CALL(evt_party_stop, 0)
    CALL(evt_snd_envon_f, 272, STRING("ENV_STG0_TIK2"), 2000)
    CALL(evt_npc_set_position, STRING("クリハカセ"), -30, 0, 170)
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 1536)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 40)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, -1)
    END_INLINE_EVENT
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_7_000"), 0, STRING("クリハカセ"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 30)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(800)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_FRANKURI1"))
    CALL(evt_cam3d_evt_set, -175, 55, 409, 83, 30, -123, 400, 11)
    WAIT_MS(900)
    CALL(evt_msg_print, 0, STRING("mac_7_001"), 0, STRING("クリハカセ"))
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -105, 125, FLOAT(120.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -71, 139, FLOAT(120.0f))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_7_002"), 0, STRING("クリハカセ"))
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("mac_7_003"), 0, STRING("クリハカセ"))
    CALL(evt_party_move_pos2, 0, -100, 145, FLOAT(120.0f))
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("mac_7_004"))
    CALL(evt_msg_print, 0, STRING("mac_7_005"), 0, STRING("クリハカセ"))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("mac_7_006"))
    WAIT_MS(250)
    CALL(evt_msg_print, 0, STRING("mac_7_007"), 0, STRING("クリハカセ"))
    WAIT_MS(200)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_npc_set_ry_lr, STRING("クリハカセ"), 1)
    CALL(evt_npc_move_position, STRING("クリハカセ"), 160, 64, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_set_position, STRING("クリハカセ"), 0, -1000, 0)
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 378)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_TIK1"), 2000)
    RETURN
EVT_END()

EVT_BEGIN(hashira_bomb_sub)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, STRING("S_ana_before"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_hitobj_onoff, STRING("a_bom_mae"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("a_bom_go"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("ro_ru"), 1, 0)
    SET_VALUE(GSWF(1330), 1)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("bom_mae"), STRING("offscreen"))
    CALL(evt_offscreen_get_boundingbox2, STRING("offscreen"), LW(10), LW(11), LW(12), LW(13))
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(12), 0)
        IF_INT_EQUAL(LW(13), 0)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 1, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_img_clear_virtual_point, STRING("capture_img"))
        CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_mae"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bom_go"), 1)
        CALL(evt_img_wait_animend, STRING("capture_img"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_mae"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("bom_go"), 1)
    END_IF
    CALL(evt_mapobj_clear_offscreen, 1, STRING("bom_mae"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    RETURN
EVT_END()

EVT_BEGIN(hashira_bomb)
    IF_INT_EQUAL(GSWF(1330), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_go"), 1)
        CALL(evt_hitobj_onoff, STRING("a_bom_go"), 1, 0)
        CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
        CALL(evt_run_case_evt, 15, 1, STRING("a_bom_mae"), 0, PTR(hashira_bomb_sub), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("bom_mae"), 1)
        CALL(evt_hitobj_onoff, STRING("a_bom_mae"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("ro_ru"), 1, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_npc_entry)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), -540, 20, -25)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073742880)
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_cam3d_evt_set, -350, 160, 582, -350, 65, 40, 0, 11)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_4_naka"), 1)
    CALL(evt_seq_wait, 2)
    CALL(evt_fade_end_wait)
    WAIT_MS(2000)
    CALL(evt_npc_move_position, STRING("クリスチーヌ"), -345, -25, 0, 60, 0)
    CALL(evt_npc_move_position, STRING("クリスチーヌ"), -345, -5, 0, 60, 0)
    SET_VALUE(LW(0), PTR(door_data_04))
    SPAWN_EVENT(door_open_se)
    CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SPAWN_EVENT(evt_door_04_00)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_move_position, STRING("クリスチーヌ"), -345, -70, 0, 60, 0)
    SET_VALUE(LW(0), PTR(door_data_04))
    SPAWN_EVENT(door_close_se)
    CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SPAWN_EVENT(evt_door_04_00)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_bero_mapchange, STRING("moo_04"), STRING(""))
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box01)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box01"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item01"), 246, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item01"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box01"))
    CALL(evt_item_get_item, STRING("item01"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tik_00_init_evt)
    IF_INT_NOT_EQUAL(GSW(0), 402)
        IF_INT_NOT_EQUAL(GSW(0), 377)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_TIK1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG0_TIK2"))
        ELSE
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envoff, 512)
        END_IF
    END_IF
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(johoya_data_alloc, STRING("jketi_code"))
    SET_VALUE(GSWF(6), 0)
    SET_VALUE(GSWF(237), 0)
    CALL(uranaisi_table_clear)
    SET_VALUE(LW(0), PTR(door_data_01))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_02))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_03))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_data_04))
    CALL_CHILD(evt_door_04_entry)
    SPAWN_EVENT(tensou_init_event)
    CALL(evt_run_case_evt, 9, 1, STRING("a_sui"), 0, PTR(tensou_sw_event), 0)
    CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list), PTR(shopper_data), PTR(trade_list))
    SET_VALUE(LW(0), STRING("ugo_yuka"))
    SET_VALUE(LW(1), STRING("a_ugo_yuka"))
    SET_VALUE(LW(2), 75)
    SET_VALUE(LW(3), 0)
    SET_VALUE(LW(4), 2000)
    SET_VALUE(LW(5), 500)
    SET_VALUE(LW(6), 1)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT(moving_floor)
    SPAWN_EVENT(hashira_bomb)
    CALL(evt_mobj_powerupblk, STRING("pblk1"), 525, 209, -175, 0, GSWF(5530), 0)
    CALL(evt_mobj_itembox, STRING("mobj_box01"), -375, -60, 320, 0, 0, PTR(mobj_box01), GSWF(1354))
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -220, 95, -360, STRING("ie_3_naka"), 125, GSWF(5588))
    CALL(evt_item_entry, STRING("item_00"), 125, 460, -40, 215, 16, GSWF(5585), 0)
    CALL(evt_item_entry, STRING("item_01"), 125, 530, 20, -220, 16, GSWF(5586), 0)
    CALL(evt_item_entry, STRING("item_02"), 125, -530, 20, -220, 16, GSWF(5587), 0)
    IF_INT_EQUAL(GSW(0), 402)
        CALL(evt_mario_kill_party, 0)
        CALL_CHILD(epilogue_npc_entry)
        SPAWN_EVENT(epilogue_evt)
    END_IF
    SPAWN_EVENT(evt_sub_mail_event)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_1"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_2"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_3"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ie_4"), 2)
    RETURN
EVT_END()

// clang-format on
