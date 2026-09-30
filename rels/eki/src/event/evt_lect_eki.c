#include "evt_lect_eki.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_itemget.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/itemdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_img.h"
#include "evt/evt_lecture.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

// Forward declarations for the unit's .bss objects (defined near the end, at their original .bss position).
static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];

// Pooled vectors referenced by lecture_button_* / effitemget_busaiku. The original REL defines these here.
const Vec vec3_eki_0000a4e0 = {200.0f, -25.0f, 0.0f};
const Vec vec3_eki_0000a4ec = {0.0f, 0.0f, 0.0f};
const Vec vec3_eki_0000a4f8 = {0.0f, 2016.0f, 0.0f};
const Vec vec3_eki_0000a504 = {0.0f, -2016.0f, 0.0f};

#pragma force_active on
static USER_FUNC(bgm_start_wait);
static USER_FUNC(lect_push_b_check);
static USER_FUNC(lect_push_a_check);
static USER_FUNC(lect_mobj_set_nosysblend);
static USER_FUNC(juyoitem_goato);
static USER_FUNC(juyoitem_gogo);
static USER_FUNC(juyoitem_end);
static USER_FUNC(juyoitem_get_no);
static USER_FUNC(lect_mobj_hit_nasi);
static USER_FUNC(effitemget_busaiku);
static USER_FUNC(effitemget_open_ptr);
static USER_FUNC(effitemget_open);
static USER_FUNC(effitemgetchange_cam_ptr);
static USER_FUNC(effitemgetchange_cam);
static USER_FUNC(lect_map_cam_chg);
static void lect_map_cam_chg_sub(MapObject* obj, s32 val, s32 recurse);
static USER_FUNC(lecture_button_set_pos);
static USER_FUNC(lecture_button_seq_wait);
static USER_FUNC(lecture_button_get_icon_id);
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_flag_on_off);
static USER_FUNC(lecture_button_set_alpha);
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_rtn);
static void lecture_button_disp(CameraId cameraId, void* param);
static USER_FUNC(x_flag_wait);
static USER_FUNC(x_flag_get);
static USER_FUNC(x_flag_add);
static USER_FUNC(x_flag_set);
static USER_FUNC(lect_status_on);
static USER_FUNC(lect_status_off);
static USER_FUNC(lect_mario_ido_sum);
static f32 lect_mario_ido_sub(f32 a, f32 b, f32 c, f32 d);

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)


static f32 lect_mario_ido_sub(f32 a, f32 b, f32 c, f32 d) {
    f32 diff = b - a;

    return (diff * c + a * d) / b;
}

static USER_FUNC(lect_mario_ido_sum) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 total = evtGetValue(event, args[0]);
    s32 progress;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(0), mario->position.x);
        evtSetFloat(event, LW(1), mario->position.y);
        evtSetFloat(event, LW(2), mario->position.z);
    }
    progress = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (progress > total) {
        progress = total;
    }
    {
        f32 start = evtGetFloat(event, LW(0));
        f32 target = evtGetFloat(event, LW(3));
        f32 t = (f32)total - (f32)progress;
        mario->position.x = (t * target + (f32)progress * start) / (f32)total;
    }
    {
        f32 start = evtGetFloat(event, LW(1));
        f32 target = evtGetFloat(event, LW(4));
        f32 t = (f32)total - (f32)progress;
        mario->position.y = (t * target + (f32)progress * start) / (f32)total;
    }
    {
        f32 start = evtGetFloat(event, LW(2));
        f32 target = evtGetFloat(event, LW(5));
        f32 t = (f32)total - (f32)progress;
        mario->position.z = (t * target + (f32)progress * start) / (f32)total;
    }
    if (progress >= total) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(lect_status_off) {
    statusWinForceClose();
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_status_on) {
    statusWinForceCloseClear();
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_set) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    x_flag_data[index] = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_add) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    x_flag_data[index] += evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_get) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], x_flag_data[index]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_wait) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 val = evtGetValue(event, args[1]);

    return (val == x_flag_data[index]) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// The iconId 0x75..0x7D range test must be a switch: MWCC lowers the grouped cases to the original's
// `cmpwi 0x7e; bge zero; cmpwi 0x75; bge neg15; b zero` chain. An `if (a < 0x7E && a >= 0x75)` makes
// MWCC invert the second branch (`blt zero`).
static void lecture_button_disp(CameraId cameraId, void* param) {
    s32 i;

    for (i = 0; i < 2; i++) {
        LectureButtonVec* p = (LectureButtonVec*)((u8*)&lbtn[i] + 8);
        LectureButton* btn = &lbtn[i];
        if (!(btn->flags & 1)) {
            Vec pos;
            f32 yOff;

            switch (btn->iconId) {
            case 0x75: case 0x76: case 0x77: case 0x78: case 0x79:
            case 0x7A: case 0x7B: case 0x7C: case 0x7D:
                yOff = -15.0f;
                break;
            default:
                yOff = 0.0f;
                break;
            }
            pos = p->position;
            pos.y += yOff;
#ifdef MUST_MATCH
            // iconDispGxAlpha's header says s16 but the original passes a zero-extended u16 (clrlwi, no extsh).
            ((void (*)(Vec*, s16, u16, u8, f32))iconDispGxAlpha)(&pos, 0x50, (u16)btn->iconId, (u8)btn->alpha,
                                                                 1.0f);
#else
            iconDispGxAlpha(&pos, 0x50, btn->iconId, btn->alpha, 1.0f);
#endif
        }
    }
}

static USER_FUNC(lecture_button_rtn) {
    s32 i;
    LectureButton* btn;
    s32 count;

    if (isFirstCall) {
        memset(lbtn, 0, sizeof(lbtn));
        for (i = 0; i < 2; i++) {
            lbtn[i].position = vec3_eki_0000a4e0;
        }
    }
    count = 0;
    for (i = 0; i < 2; i++) {
        btn = &lbtn[i];

        switch (btn->seq) {
        case 0:
            btn->iconId = 0x68;
            btn->seq = 1;
            break;
        case 2:
            btn->alpha += 7;
            if (btn->alpha >= 0xFF) {
                btn->alpha = 0xFF;
                btn->seq = 1;
            }
            break;
        case 3:
            btn->alpha -= 7;
            if (btn->alpha <= 0) {
                btn->alpha = 0;
                btn->seq = 1;
            }
            break;
        case 4:
            btn->flags |= 1;
            count++;
            break;
        }
    }
    if (count >= 2) {
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, 0.0f, 1, lecture_button_disp, NULL);
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(lecture_button_set_icon_id) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = lbtn + index;

    btn->iconId = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_alpha) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = lbtn + index;

    btn->alpha = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    u8 mask = evtGetValue(event, args[1]);
    s32 flag = evtGetValue(event, args[2]);
    u8* f = &lbtn[index].flags;

    if (flag != 0) {
        *f |= mask;
    } else {
        *f &= ~mask;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = lbtn + index;
    s32 seq = evtGetValue(event, args[1]);

    btn->seq = seq;
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_get_icon_id) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = lbtn + index;

    evtSetValue(event, args[1], btn->seq);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_seq_wait) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 val = evtGetValue(event, args[1]);
    LectureButton* btn = lbtn + index;

    return (btn->seq - val) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    LectureButton* btn = lbtn + index;
    Vec pos = vec3_eki_0000a4ec;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    btn->position = pos;
    return EVT_RETURN_DONE;
}

static void lect_map_cam_chg_sub(MapObject* obj, s32 val, s32 recurse) {
    MapObject* c;
    MapObject* b;
    MapObject* a;
    MapObject* d;
    MapObject* e;

    obj->field_0x0[5] = val;
    a = obj->child;
    if (a != NULL) {
        a->field_0x0[5] = val;
        b = a->child;
        if (b != NULL) {
            b->field_0x0[5] = val;
            c = b->child;
            if (c != NULL) {
                c->field_0x0[5] = val;
                if (c->child != NULL)
                    lect_map_cam_chg_sub(c->child, val, 1);
                if (c->next != NULL)
                    lect_map_cam_chg_sub(c->next, val, 1);
            }
            c = b->next;
            if (c != NULL) {
                c->field_0x0[5] = val;
                if (c->child != NULL)
                    lect_map_cam_chg_sub(c->child, val, 1);
                if (c->next != NULL)
                    lect_map_cam_chg_sub(c->next, val, 1);
            }
        }
        d = a->next;
        if (d != NULL) {
            d->field_0x0[5] = val;
            e = d->child;
            if (e != NULL) {
                e->field_0x0[5] = val;
                if (e->child != NULL)
                    lect_map_cam_chg_sub(e->child, val, 1);
                if (e->next != NULL)
                    lect_map_cam_chg_sub(e->next, val, 1);
            }
            d = d->next;
            if (d != NULL) {
                d->field_0x0[5] = val;
                if (d->child != NULL)
                    lect_map_cam_chg_sub(d->child, val, 1);
                if (d->next != NULL)
                    lect_map_cam_chg_sub(d->next, val, 1);
            }
        }
    }
    if (recurse) {
        c = obj->next;
        if (c != NULL) {
            c->field_0x0[5] = val;
            b = c->child;
            if (b != NULL) {
                b->field_0x0[5] = val;
                a = b->child;
                if (a != NULL) {
                    a->field_0x0[5] = val;
                    if (a->child != NULL)
                        lect_map_cam_chg_sub(a->child, val, 1);
                    if (a->next != NULL)
                        lect_map_cam_chg_sub(a->next, val, 1);
                }
                e = b->next;
                if (e != NULL) {
                    e->field_0x0[5] = val;
                    if (e->child != NULL)
                        lect_map_cam_chg_sub(e->child, val, 1);
                    if (e->next != NULL)
                        lect_map_cam_chg_sub(e->next, val, 1);
                }
            }
            d = c->next;
            if (d != NULL) {
                d->field_0x0[5] = val;
                e = d->child;
                if (e != NULL) {
                    e->field_0x0[5] = val;
                    if (e->child != NULL)
                        lect_map_cam_chg_sub(e->child, val, 1);
                    if (e->next != NULL)
                        lect_map_cam_chg_sub(e->next, val, 1);
                }
                d = d->next;
                if (d != NULL) {
                    d->field_0x0[5] = val;
                    if (d->child != NULL)
                        lect_map_cam_chg_sub(d->child, val, 1);
                    if (d->next != NULL)
                        lect_map_cam_chg_sub(d->next, val, 1);
                }
            }
        }
    }
}

static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 val = evtGetValue(event, args[1]);
    MapObject* obj = mapGetMapObj((const char*)name);

    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, val, 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);
    s32 val = evtGetValue(event, args[1]);

    eff = effNameToPtr((const char*)eff);
    ((effItemGetData*)eff->userdata)->field_0x38 = val;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);
    s32 val = evtGetValue(event, args[1]);

    ((effItemGetData*)eff->userdata)->field_0x38 = val;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open) {
    s32* args = event->args;
    EffectEntry* eff = effNameToPtr((const char*)evtGetValue(event, args[0]));

    ((effItemGetData*)eff->userdata)->field_0x20 = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open_ptr) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);

    ((effItemGetData*)eff->userdata)->field_0x20 = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_busaiku) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec offset;

    if (flag != 0) {
        offset = vec3_eki_0000a4f8;
    } else {
        offset = vec3_eki_0000a504;
    }
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    PSVECAdd(&cam->cameraPos, &offset, &cam->storedPos);
    PSVECAdd(&cam->cameraAt, &offset, &cam->storedAt);
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_mobj_hit_nasi) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    MapObjectEntry* obj;
    s32 i;

    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    for (i = 0; i < 2; i++) {
        if (obj->hitObj[i].name != NULL &&
            strcmp(obj->hitObj[i].name, "") != 0) {
            hitGrpFlagOn(obj->hitObj[i].name, 1);
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_get_no) {
    s32* args = event->args;

    evtSetValue(event, args[0], ((ItemEntry*)event->lwData[8])->itemId);
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_end) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];
    EffectEntry* eff = (EffectEntry*)event->lwData[9];

    effSoftDelete(eff);
    marioSetCamId(CAMERA_3D);
    iconFlagOff(item->name, 0x200);
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_gogo) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    iconFlagOff(item->name, 0x200);
    itemSetPosition(item, 0.0f, -1000.0f, 0.0f);
    marioChgStayMotion();
    marioStSystemLevel(0);
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_goato) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    iconFlagOn(item->name, 2);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_mobj_set_nosysblend) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    MapObjectEntry* obj;

    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    animPoseSetMaterialFlagOn(obj->poseId, 0x1800);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_push_a_check) {
    s32* args = event->args;

    if (keyGetButton(0) & 0x100) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_push_b_check) {
    s32* args = event->args;

    if (keyGetButton(0) & 0x200) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(bgm_start_wait) {
    return psndBGMStartCheck(1) != 0 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(boots_lv3_juyoitem_lecture_init)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_mario_set_pos, 0, -600, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 150)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1073741856)
    CALL(evt_npc_set_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
    CALL(evt_mario_set_pos, LW(3), LW(4), LW(5))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(lect_mario_ido_sum, 1000)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, 0, -600, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 0, 11)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pos, 0, -600, 0)
    CALL(evt_mario_set_dir, 90, 300, 0)
    RETURN
EVT_END()

EVT_BEGIN(boots_lv3_juyoitem_lecture_sub01)
    CALL(lecture_button_set_alpha, 0, 255)
    CALL(lecture_button_set_alpha, 1, 128)
    CALL(lecture_button_set_icon_id, 1, 121)
    LOOP(0)
        CALL(lecture_button_set_icon_id, 0, 101)
        WAIT_MS(300)
        CALL(lecture_button_set_icon_id, 0, 100)
        WAIT_MS(300)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(boots_lv3_juyoitem_lecture_sub02)
    CALL(lecture_button_set_icon_id, 0, 101)
    LOOP(0)
        CALL(evt_mario_get_motion, LW(3))
        IF_INT_EQUAL(LW(3), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(lecture_button_set_alpha, 0, 128)
    CALL(lecture_button_set_alpha, 1, 255)
    LOOP(0)
        CALL(lecture_button_set_icon_id, 1, 120)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 123)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 124)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 125)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 122)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 119)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 118)
        WAIT_MS(15)
        CALL(lecture_button_set_icon_id, 1, 117)
        WAIT_MS(15)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(boots_lv3_juyoitem_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    CALL(lect_map_cam_chg, STRING("S_rec"), 7)
    CALL(evt_npc_set_camid, STRING("キノピコ"), 7)
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 85)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_msg_print, 0, STRING("lecture_u_boots_04"), 0, STRING("キノピコ"))
    CALL(evt_msg_print_add, 0, STRING("lecture_u_boots_05"))
    LOOP(0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("lecture_u_boots_06"), 0, 0)
        CALL(x_flag_set, 1, 1)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_3"), STRING("T_3"))
            CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_3"))
            LOOP(0)
                CALL(evt_sub_random, 2, LW(1))
                ADD(LW(1), 4)
                MULTIPLY(LW(1), 2)
                LOOP(LW(1))
                    WAIT_MS(500)
                    CALL(x_flag_get, 1, LW(0))
                    IF_INT_EQUAL(LW(0), 2)
                        BREAK_LOOP
                    END_IF
                END_LOOP
                CALL(x_flag_get, 1, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("T_3"))
                CALL(evt_msg_print, 0, STRING("lecture_u_boots_07"), 0, STRING("キノピコ"))
                CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_3"))
            END_LOOP
            CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
            CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_1"))
        END_INLINE_EVENT
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(85.0f), FLOAT(520.0f), FLOAT(0.0f), FLOAT(40.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
        CALL(lecture_button_set_pos, 1, FLOAT(-140.0f), FLOAT(-25.0f), FLOAT(0.0f))
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_icon_id, 0, 100)
        CALL(lecture_button_set_seq, 0, 2)
        CALL(lecture_button_flag_on_off, 1, 2, 1)
        CALL(lecture_button_set_icon_id, 1, 121)
        CALL(lecture_button_set_seq, 1, 2)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        SPAWN_EVENT_GET_ID(PTR(boots_lv3_juyoitem_lecture_sub01), LW(11))
        SET_VALUE(LW(7), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SET_VALUE(LW(7), 0)
        CALL(mainwindow_opendisable)
        CALL(evt_mario_trigflag_onoff, 1, 67108864)
        CALL(evt_mario_set_enable_key, 256)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        LOOP(0)
            CALL(lect_status_off)
            CALL(evt_mario_get_pos, 0, LW(5), LW(1), LW(6))
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(1), FLOAT(-470.0f))
                CALL(x_flag_set, 1, 2)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(7), 0)
                CALL(lect_push_a_check, LW(7))
                IF_INT_NOT_EQUAL(LW(7), 0)
                    DELETE_EVENT(LW(11))
                    SPAWN_EVENT_GET_ID(PTR(boots_lv3_juyoitem_lecture_sub02), LW(11))
                END_IF
            ELSE
                IF_INT_EQUAL(LW(7), 1)
                    CALL(lect_push_a_check, LW(7))
                    IF_INT_EQUAL(LW(7), 0)
                        DELETE_EVENT(LW(11))
                        SPAWN_EVENT_GET_ID(PTR(boots_lv3_juyoitem_lecture_sub01), LW(11))
                    END_IF
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(lect_status_on)
        CALL(evt_mario_trigflag_onoff, 0, 67108864)
        CALL(evt_mario_set_enable_key, 0)
        CALL(mainwindow_openenable)
        DELETE_EVENT(LW(11))
        CALL(lecture_button_set_seq, 0, 3)
        CALL(lecture_button_set_seq, 1, 3)
        CALL(evt_msg_continue)
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(85.0f), FLOAT(520.0f), FLOAT(0.0f), FLOAT(40.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_seq_wait, 1, 1)
        CALL(evt_msg_print, 0, STRING("lecture_u_boots_08"), 0, STRING("キノピコ"))
        CALL(evt_msg_continue)
        WAIT_MS(500)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(1), 1)
        ADD(LW(2), 1)
        CALL(evt_mario_normalize)
        CALL(evt_mario_wait_landon)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mario_set_pos, LW(0), LW(1), 0)
        CALL(evt_mario_set_dir, 90, 300, 0)
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 65)
        CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
        CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_2"), STRING("T_2"))
        CALL(evt_msg_print, 0, STRING("lecture_u_boots_10"), 0, STRING("キノピコ"))
        CALL(evt_msg_select, 0, STRING("lecture_u_boots_11"))
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("lecture_u_boots_12"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("lecture_u_boots_13"))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_position, STRING("キノピコ"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(all_juyoitem_lecture_rel_7_data_150F8)
    CALL(evt_mario_normalize)
    CALL(juyoitem_get_no, LW(10))
    CALL(evt_mario_set_camid, 7)
    CALL(effitemget_open_ptr, LW(9))
    CALL(juyoitem_gogo)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_stop_for_event)
    CALL(evt_npc_start_for_one_event, STRING("キノピコ"))
    CALL(evt_party_stop, 0)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_LECTURE1"))
    CALL(evt_snd_envoff, 1024)
    CALL(bgm_start_wait)
    CALL(lect_cam_save)
    CALL(effitemget_busaiku, 1)
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(1))
    ADD_FLOAT(LW(3), FLOAT(2016.0f))
    CALL(evt_mario_set_pos, LW(0), LW(3), LW(2))
    CALL(effitemgetchange_cam_ptr, LW(9), 4)
    CALL(evt_paper_entry, STRING("OFF_d_seal_upper_right"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_seal_upper_right"))
    CALL(evt_img_free_capture, STRING("img"), 0)
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_clear_virtual_point, STRING("img"))
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("Z_1"))
    CALL(evt_img_onoff, STRING("img"), 1)
    WAIT_FRAMES(1)
    CALL(effitemget_busaiku, 0)
    CALL(evt_eff_softdelete_ptr, LW(9))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_mario_cont_onoff, 1)
    CALL(lect_test_systemlevel)
    CALL(evt_npc_sound_data_set, STRING("キノピコ"), STRING("SFX_FLD_NPC_KINOPIO_MOVE1L"), STRING("SFX_FLD_NPC_KINOPIO_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SWITCH(LW(10))
        CASE_EQUAL(8)
        CALL_CHILD(boots_lv3_juyoitem_lecture_init)
    END_SWITCH
    SWITCH(LW(10))
        CASE_EQUAL(8)
        CALL_CHILD(boots_lv3_juyoitem_lecture)
    END_SWITCH
    LOOP(6)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mario_init_camid)
    CALL(lect_cam_load, 0)
    CALL(evt_cam3d_evt_off, 0, 11)
    CALL(evt_cam_road_reset)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(juyoitem_goato)
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_seal_upper_right"))
    CALL(evt_npc_start_for_event)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

// Start time of the lecture move-in animation (captured on the first call).
static OSTime wwstart;

// TODO: type
static s32 x_flag_data[2];

// TODO: type
static LectureButton lbtn[2];

// clang-format on
