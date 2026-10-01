#include "las_14.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "driver/imgdrv.h"
#include "driver/mapdrv.h"
#include "driver/offscreendrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "party/party_cloud.h"
#include "party/party_motion.h"
#include "pmario_sound.h"

#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(evt_cloud_main);
static void _mapclsofs_all(CloudEntry* cp);
static void _mapsetofs_all(CloudEntry* cp);
static s32 _get_boundingbox(const char* name, s32* left, s32* top, s32* width, s32* height);
static void _img_alloc_capture(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7);

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"w_bero_1", 1, 1, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_10", "se_bero_1", 5, 5, w_door_close, w_door_open},
    {NULL},
};

static EVT_BEGIN(box_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("key"), 41, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("key"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static char* capturemap[3] = {
    "S_kabe1", "S_ten", "",
};

static Vec v_list[8] = {
    {-50.0f, 100.0f, 50.0f},
    {50.0f, 100.0f, 50.0f},
    {50.0f, 0.0f, 50.0f},
    {-50.0f, 0.0f, 50.0f},
    {-50.0f, 100.0f, -50.0f},
    {50.0f, 100.0f, -50.0f},
    {50.0f, 0.0f, -50.0f},
    {-50.0f, 0.0f, -50.0f},
};

static EVT_BEGIN(evt_cloud_main_evt)
    CALL(evt_cloud_main)
    RETURN
EVT_END()

static EVT_BEGIN(cloud_evt)
    CALL(evt_mobj_flag_onoff, 0, STRING("box"), 16384)
    CALL(evt_mobj_hit_onoff, STRING("box"), 1)
    CALL(evt_hitobj_onoff, STRING("A_kura"), 1, 0)
    SET_VALUE(GSWF(4344), 1)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    RETURN
EVT_END()

EVT_BEGIN(touch_evt)
    RETURN
EVT_END()

static EVT_BEGIN(hint_plate)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg8_las_39"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(las_14_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hitobj_onoff, STRING("A_taka"), 1, 0)
    IF_INT_EQUAL(GSWF(4344), 0)
        CALL(evt_las_cloud_init)
        CALL(evt_las_cloud_ent, PTR(capturemap), STRING("A_kura"), PTR(cloud_evt))
        CALL(evt_run_case_evt, 9, 1, STRING("A_kura"), 0, PTR(touch_evt), 0)
    ELSE
        CALL(evt_hitobj_onoff, STRING("A_kura"), 1, 0)
    END_IF
    CALL(evt_mobj_itembox, STRING("box"), 0, 0, 0, 0, 0, PTR(box_evt), GSWF(4365))
    IF_INT_EQUAL(GSWF(4344), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("box"), 16384)
        CALL(evt_mobj_hit_onoff, STRING("box"), 0)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("A_pure"), 0, PTR(hint_plate), 0)
    RETURN
EVT_END()

static CloudEntry* cloud;
static s8 cloud_once_flag;

static void _img_alloc_capture(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7) {
    s32 args[10];
    EventEntry tmpEvent;

    args[0] = a0;
    args[1] = a1;
    tmpEvent.args = args;
    args[2] = a2;
    args[3] = a3;
    args[4] = a4;
    args[5] = a5;
    args[6] = a6;
    args[7] = a7;
    evt_img_alloc_capture(&tmpEvent, 1);
}

static s32 _get_boundingbox(const char* name, s32* left, s32* top, s32* width, s32* height) {
    Vec pos;
    Vec screen;
    s32 minX;
    s32 maxX;
    s32 minY;
    s32 maxY;
    s32 i;

    hitObjGetPos(name, &pos);
    PSVECAdd(&pos, v_list, &screen);
    getScreenPoint(&screen, &screen);
    minX = (s32)screen.x;
    maxX = (s32)screen.x;
    minY = (s32)screen.y;
    maxY = (s32)screen.y;
    for (i = 1; i < 8; i++) {
        PSVECAdd(&pos, &v_list[i], &screen);
        getScreenPoint(&screen, &screen);
        if (screen.x < (f32)minX) {
            minX = (s32)screen.x;
        }
        if ((f32)maxX < screen.x) {
            maxX = (s32)screen.x;
        }
        if (screen.y < (f32)minY) {
            minY = (s32)screen.y;
        }
        if ((f32)maxY < screen.y) {
            maxY = (s32)screen.y;
        }
    }
    *left = minX + 0x130;
    *top = minY + 0xF0;
    *width = maxX - minX;
    *height = maxY - minY;
    if (*left < 0) {
        *left = 0;
    }
    if (*top < 0) {
        *top = 0;
    }
    if (*left + *width > 0x260) {
        *width = 0x260 - *left;
    }
    if (*top + *height > 0x1E0) {
        *height = 0x1E0 - *top;
    }
    if (*left < 0) {
        *left = 0;
    }
    if (*top < 0) {
        *top = 0;
    }
    return 1;
}

static void _mapsetofs_all(CloudEntry* cp) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((s8)cp->mapGrpName[i][0] == 0) {
            break;
        }
        mapGrpSetOffScreen(cp->mapGrpName[i], cp->offName);
    }
}

static void _mapclsofs_all(CloudEntry* cp) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((s8)cp->mapGrpName[i][0] == 0) {
            break;
        }
        mapGrpClearOffScreen(cp->mapGrpName[i]);
    }
}

static USER_FUNC(evt_cloud_main) {
    CloudEntry* cp = (CloudEntry*)event->lwData[0];
    s32 inBattle = (gp->inBattle != 0);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(0);
    ImageEntry* img;
    HitEntry* hit;
    HitEntry* h0;
    HitEntry* h1;
    HitEntry* h2;
    HitEntry* h3;

    if (isFirstCall) {
        return EVT_RETURN_BLOCK;
    }
    if (party == NULL) {
        return EVT_RETURN_BLOCK;
    }
    if (party->currentMemberId != PARTY_MEMBER_FLURRIE) {
        return EVT_RETURN_BLOCK;
    }

    h0 = (HitEntry*)cloudGetHitBreathout(0);
    h1 = (HitEntry*)cloudGetHitBreathout(1);
    h2 = (HitEntry*)cloudGetHitBreathout(2);
    h3 = (HitEntry*)cloudGetHitBreathout(3);

    switch (cp->mode) {
    case 0:
        if (cloud_once_flag == 1) {
            break;
        }
        hit = hitNameToPtr(cp->hitName);
        if (h0 == hit || h1 == hit || h2 == hit || h3 == hit) {
            cp->mode++;
        }
        cp->counter = 0;
        break;
    case 1: {
        f32 angle = *(f32*)mario->unk1AC;

        if (angle > 90.0f && angle <= 270.0f) {
            strcpy(cp->animName, "P_oof_cloud_2");
        } else {
            strcpy(cp->animName, "P_oof_cloud_1");
        }
        if (animGroupBaseAsync(cp->animName, inBattle, NULL)) {
            cloud_once_flag = 1;
            animPaperPoseEntry(cp->animName, inBattle);
            offscreenEntry(cp->offName);
            _mapsetofs_all(cp);
            cp->mode++;
        }
        break;
    }
    case 2:
        imgEntry(cp->imgName, inBattle);
        img = imgNameToPtr(cp->imgName, inBattle);
        img->poseId = animPaperPoseGetId(cp->animName, inBattle);
        *(OSTime*)&img->field_0x10C[4] = animTimeGetTime(inBattle);
        imgClearVirtualPoint(img);
        imgSetShadow(img, 2);
        img->flags |= 2;
        cp->mode = 0xDE;
        break;
    case 0xDE:
        if (!_get_boundingbox(cp->hitName, &cp->left, &cp->top, &cp->width, &cp->height)) {
            break;
        }
        {
            EventEntry tmpEvent;
            s32 captureArgs[10];
            s32 left;
            s32 top;
            s32 width;
            s32 height;

            height = cp->height;
            width = cp->width;
            top = cp->top;
            left = cp->left;

            tmpEvent.args = captureArgs;
            captureArgs[0] = (s32)cp->imgName;
            captureArgs[1] = 0;
            captureArgs[2] = (s32)cp->offName;
            captureArgs[3] = 1;
            captureArgs[4] = left;
            captureArgs[5] = top;
            captureArgs[6] = width;
            captureArgs[7] = height;
            evt_img_alloc_capture(&tmpEvent, 1);
        }
        cp->mode = 0xDF;
        break;
    case 0xDF:
        img = imgNameToPtr(cp->imgName, inBattle);
        img->flags &= ~2;
        cp->mode = 3;
        // fallthrough
    case 3:
        img = imgNameToPtr(cp->imgName, inBattle);
        img->animName = "A_1";
        *(OSTime*)&img->field_0x10C[4] = animTimeGetTime(inBattle);
        img->flags &= ~2;
        cp->timer = 0x1E;
        cp->lastTime = 0.0f;
        cp->mode++;
        break;
    case 4:
        img = imgNameToPtr(cp->imgName, inBattle);
        if (cp->lastTime == 0.0f || 1.0f + cp->lastTime < img->field_0x118) {
            psndSFXOn_3D("SFX_OFF_CLAUD_WIND2", &mario->position);
            cp->lastTime = img->field_0x118;
        }
        if (mario->currMotionId != MARIO_MOTION_CLOUD) {
            cp->mode = 0xA;
            break;
        }
        hit = hitNameToPtr(cp->hitName);
        if (!(h0 == hit || h1 == hit || h2 == hit || h3 == hit)) {
            if (cp->timer == 0) {
                cp->mode = 0xA;
                break;
            }
            cp->timer--;
            cp->counter += 2;
            break;
        }
        cp->timer = 0x1E;
        cp->counter += 2;
        if (cp->counter > cp->maxCount) {
            cp->mode = 0x14;
        }
        break;
    case 0xA:
        cp->mode = 0x63;
        break;
    case 0x14:
        psndSFXOn_3D("SFX_OFF_CLAUD_WIND3", &mario->position);
        if (cp->script != NULL) {
            evtEntry(cp->script, 0, 0);
        }
        img = imgNameToPtr(cp->imgName, inBattle);
        img->animName = "A_2";
        *(OSTime*)&img->field_0x10C[4] = animTimeGetTime(inBattle);
        cp->timer = 0x3C;
        cp->mode++;
        break;
    case 0x15:
        if (cp->timer != 0) {
            cp->timer--;
            break;
        }
        cp->timer = 0x1E;
        cp->counter += 2;
        cp->mode = 0x64;
        break;
    case 0x63:
    case 0x64: {
        s32 i;
        ImageEntry* img;

        img = imgNameToPtr(cp->imgName, inBattle);
        for (i = 0; i < 8; i++) {
            if ((s8)cp->mapGrpName[i][0] == 0) {
                break;
            }
            mapGrpClearOffScreen(cp->mapGrpName[i]);
        }
        animPaperPoseRelease(img->poseId);
        imgFreeCapture(img, 0);
        imgSetShadow(img, 0);
        imgRelease(img, inBattle);
        offscreenDelete(cp->offName);
        cloud_once_flag = 0;
        if (cp->mode == 0x63) {
            cp->mode = 0;
        } else {
            return EVT_RETURN_DONE;
        }
        break;
    }
    default:
        break;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_las_cloud_init) {
    s32 i;

    cloud = _mapAlloc(0x1B00);
    for (i = 0; i < 16; i++) {
        cloud[i].flags = 0;
        sprintf(cloud[i].imgName, "i_name_%1d", i);
        sprintf(cloud[i].offName, "o_name_%1d", i);
    }
    cloud_once_flag = 0;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_las_cloud_ent) {
    s32* args = event->args;
    s8** a;
    s8* b;
    void* c;
    CloudEntry* cp;
    s32 i, j;

    a = (s8**)evtGetValue(event, args[0]);
    b = (s8*)evtGetValue(event, args[1]);
    c = (void*)evtGetValue(event, args[2]);
    for (j = 0; j < 16; j++) {
        if (cloud[j].flags == 0) {
            break;
        }
    }
    if (j >= 16) {
        return EVT_RETURN_DONE;
    }
    cp = &cloud[j];
    cp->flags = 1;
    strcpy(cp->animName, "P_oof_cloud_1");
    for (i = 0; i < 8; i++) {
        if (a[i][0] == 0) {
            break;
        }
        strcpy((char*)cp->mapGrpName[i], (const char*)a[i]);
    }
    strcpy(cp->hitName, (const char*)b);
    cp->script = c;
    cp->maxCount = 0xB4;
    cp->counter = 0;
    cp->mode = 0;
    cp->timer = 0;
    cp->event = evtEntry(evt_cloud_main_evt, 0, 0);
    cp->event->lwData[0] = (s32)cp;
    return EVT_RETURN_DONE;
}
// clang-format on
