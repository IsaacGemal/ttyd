#include "evt_lect_mri.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_img.h"
#include "evt/evt_lecture.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/itemdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <dolphin/pad.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static f32 lect_mario_ido_sub(f32, f32, f32, f32);
static USER_FUNC(lect_mario_ido_sum);
static USER_FUNC(lect_status_off);
static USER_FUNC(lect_status_on);
static USER_FUNC(x_flag_set);
static USER_FUNC(x_flag_add);
static USER_FUNC(x_flag_get);
static USER_FUNC(x_flag_wait);
static void lecture_button_disp(CameraId, void*);
static USER_FUNC(lecture_button_rtn);
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_set_alpha);
static USER_FUNC(lecture_button_flag_on_off);
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_get_icon_id);
static USER_FUNC(lecture_button_seq_wait);
static USER_FUNC(lecture_button_set_pos);
static void lect_map_cam_chg_sub(MapObject*, s32, s32);
static USER_FUNC(lect_map_cam_chg);
static USER_FUNC(effitemgetchange_cam);
static USER_FUNC(effitemgetchange_cam_ptr);
static USER_FUNC(effitemget_open);
static USER_FUNC(effitemget_open_ptr);
static USER_FUNC(effitemget_busaiku);
static USER_FUNC(lect_mobj_hit_nasi);
static USER_FUNC(juyoitem_get_no);
static USER_FUNC(juyoitem_end);
static USER_FUNC(juyoitem_gogo);
static USER_FUNC(juyoitem_goato);
static USER_FUNC(lect_mobj_set_nosysblend);
static USER_FUNC(lect_push_a_check);
static USER_FUNC(lect_push_b_check);
static USER_FUNC(bgm_start_wait);

typedef struct LectureButton {
    u8 flag;       // 0x0
    u8 seq;        // 0x1
    u8 pad_2[0xA]; // 0x2
    Vec pos;       // 0xC
    u8 pad_18[0x28]; // 0x18
    s16 iconId;    // 0x40
    s16 alpha;     // 0x42
} LectureButton;

static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];

// clang-format off
extend_data(144,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(145,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static f32 lect_mario_ido_sub(f32 a, f32 b, f32 c, f32 d) {
    f32 t = b - a;

    return (t * c + a * d) / b;
}

// NONMATCHING (98.67%): wwstart's low word is loaded into r7 instead of r5; the divisor-high
// zero is scheduled before the subtraction rather than after.
static USER_FUNC(lect_mario_ido_sum) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 total = evtGetValue(event, args[0]);
    s32 count;
    s32 diff;
    u32 ticks;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(0), mario->position.x);
        evtSetFloat(event, LW(1), mario->position.y);
        evtSetFloat(event, LW(2), mario->position.z);
    }
    diff = (s32)(((s32*)&event->lifetime)[1] - ((s32*)&wwstart)[1]);
    ticks = OS_TIMER_CLOCK / 1000;
    count = (s32)((u64)(s32)diff / (u64)ticks);
    if (count > total) {
        count = total;
    }
    mario->position.x = lect_mario_ido_sub(count, total, evtGetFloat(event, LW(3)), evtGetFloat(event, LW(0)));
    mario->position.y = lect_mario_ido_sub(count, total, evtGetFloat(event, LW(4)), evtGetFloat(event, LW(1)));
    mario->position.z = lect_mario_ido_sub(count, total, evtGetFloat(event, LW(5)), evtGetFloat(event, LW(2)));
    if (count >= total) {
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
    s32 idx = evtGetValue(event, args[0]);

    x_flag_data[idx] = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_add) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    x_flag_data[idx] += evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_get) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], x_flag_data[idx]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_wait) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 target = evtGetValue(event, args[1]);

    return (target == x_flag_data[idx]) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// NONMATCHING (85.78%): MWCC copies lbtn[i].pos through a base four bytes early (addi r4,r9,8)
// and lays the dy=0/dy=-15 constant blocks out differently.
static void lecture_button_disp(CameraId camId, void* param) {
    s32 i;
    for (i = 0; i < 2; i++) {
        if (!(lbtn[i].flag & 1)) {
            Vec pos;
            f32 dy;
            if (lbtn[i].iconId < 0x7e && lbtn[i].iconId >= 0x75) dy = -15.0f;
            else dy = 0.0f;
            pos = lbtn[i].pos;
            pos.y += dy;
            iconDispGxAlpha(&pos, 0x50, lbtn[i].iconId, lbtn[i].alpha, 1.0f);
        }
    }
}

// NONMATCHING (98.96%): count and btn occupy r6/r7 swapped.
static USER_FUNC(lecture_button_rtn) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        memset(lbtn, 0, sizeof(lbtn));
        for (i = 0; i < 2; i++) {
            lbtn[i].pos = (Vec){200.0f, -25.0f, 0.0f};
        }
    }
    count = 0;
    for (i = 0; i < 2; i++) {
        LectureButton* btn = &lbtn[i];
        switch (btn->seq) {
        case 0:
            btn->iconId = 0x68;
            btn->seq = 1;
            break;
        case 2:
            btn->alpha += 7;
            if (btn->alpha >= 0xff) {
                btn->alpha = 0xff;
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
            btn->flag |= 1;
            count++;
            break;
        }
    }
    if (count >= 2) {
        return EVT_RETURN_DONE;
    }
    dispEntry(8, 0.0f, 1, lecture_button_disp, NULL);
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(lecture_button_set_icon_id) {
    s32* args = event->args;
    LectureButton* btn;
    s32 idx = evtGetValue(event, args[0]);

    btn = &lbtn[idx];
    btn->iconId = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_alpha) {
    s32* args = event->args;
    LectureButton* btn;
    s32 idx = evtGetValue(event, args[0]);

    btn = &lbtn[idx];
    btn->alpha = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    u8 flag = evtGetValue(event, args[1]);
    s32 on = evtGetValue(event, args[2]);
    u8* base = (u8*)lbtn;
    s32 off = idx * 0x44;

    if (on != 0) {
        base[off] |= flag;
    } else {
        base[off] &= ~flag;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;
    LectureButton* btn;
    s32 idx = evtGetValue(event, args[0]);

    btn = &lbtn[idx];
    btn->seq = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_get_icon_id) {
    s32* args = event->args;
    LectureButton* btn;
    s32 idx = evtGetValue(event, args[0]);

    btn = &lbtn[idx];
    evtSetValue(event, args[1], btn->seq);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_seq_wait) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 target = evtGetValue(event, args[1]);
    LectureButton* btn = &lbtn[idx];

    return (btn->seq != target) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    LectureButton* btn;
    s32 idx = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    Vec pos = (Vec){0.0f, 0.0f, 0.0f};

    pos.x = x;
    pos.y = y;
    pos.z = z;
    btn = &lbtn[idx];
    btn->pos = pos;
    return EVT_RETURN_DONE;
}

static void lect_map_cam_chg_sub(MapObject* obj, s32 flag, s32 recurse) {
    obj->field_0x0[5] = flag;
    if (obj->child) {
        lect_map_cam_chg_sub(obj->child, flag, 1);
    }
    if (recurse && obj->next) {
        lect_map_cam_chg_sub(obj->next, flag, 1);
    }
}

static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    MapObject* obj;

    obj = mapGetMapObj(name);
    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, flag, 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 cam = evtGetValue(event, args[1]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    *(s32*)((u8*)eff->userdata + 0x38) = cam;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);
    s32 cam = evtGetValue(event, args[1]);

    *(s32*)((u8*)eff->userdata + 0x38) = cam;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open) {
    EffectEntry* eff = effNameToPtr((const char*)evtGetValue(event, event->args[0]));

    *(s32*)((u8*)eff->userdata + 0x20) = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open_ptr) {
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, event->args[0]);

    *(s32*)((u8*)eff->userdata + 0x20) = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_busaiku) {
    s32 arg = evtGetValue(event, event->args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec offset;

    if (arg != 0) {
        offset = (Vec){0.0f, 2016.0f, 0.0f};
    } else {
        offset = (Vec){0.0f, -2016.0f, 0.0f};
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

// NONMATCHING (99.77%): name is kept in r29 (reusing obj's register) instead of r28.
static USER_FUNC(lect_mobj_hit_nasi) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
    MapObjectEntry* obj;
    const char* hitName;
    s32 i;

    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    for (i = 0; i < 2; i++) {
        hitName = obj->hitObj[i].name;
        if (hitName != NULL && strcmp(hitName, "") != 0) {
            hitGrpFlagOn(hitName, 1);
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_get_no) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    evtSetValue(event, event->args[0], item->itemId);
    return EVT_RETURN_DONE;
}

static USER_FUNC(juyoitem_end) {
    ItemEntry* item = (ItemEntry*)event->lwData[8];

    effSoftDelete((EffectEntry*)event->lwData[9]);
    marioSetCamId(4);
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
    const char* name = (const char*)evtGetValue(event, event->args[0]);
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

    if (keyGetButton(0) & PAD_BUTTON_A) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_push_b_check) {
    s32* args = event->args;

    if (keyGetButton(0) & PAD_BUTTON_B) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(bgm_start_wait) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(boots_lv2_juyoitem_lecture_init)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    CALL(evt_mario_set_pos, 0, 0, 70)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 150)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("キノピコ"), 1073741856)
    CALL(evt_npc_set_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(lect_mario_ido_sum, 1000)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_set_dir, 90, 300, 0)
    RETURN
EVT_END()

EVT_BEGIN(boots_lv2_juyoitem_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    CALL(evt_npc_set_camid, STRING("キノピコ"), 7)
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 85)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_msg_print, 0, STRING("lecture_s_boots_04"), 0, STRING("キノピコ"))
    CALL(evt_msg_print_add, 0, STRING("lecture_s_boots_05"))
    LOOP(0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("lecture_s_boots_06"), 0, 0)
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
                CALL(evt_msg_print, 0, STRING("lecture_s_boots_07"), 0, STRING("キノピコ"))
                CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_3"))
            END_LOOP
            CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
            CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("S_1"))
        END_INLINE_EVENT
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(85.0f), FLOAT(520.0f), FLOAT(0.0f), FLOAT(40.0f), FLOAT(-100.0f), 1000, 11)
        WAIT_MS(1000)
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_icon_id, 0, 100)
        CALL(lecture_button_set_seq, 0, 2)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(x_flag_set, 0, 1)
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(lecture_button_set_icon_id, 0, 101)
                WAIT_MS(300)
                CALL(x_flag_get, 0, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                CALL(lecture_button_set_icon_id, 0, 100)
                WAIT_MS(300)
                CALL(x_flag_get, 0, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SET_VALUE(LW(7), 0)
        CALL(mainwindow_opendisable)
        CALL(evt_mario_trigflag_onoff, 1, 134217728)
        CALL(evt_mario_trigflag_onoff, 1, 67108864)
        CALL(evt_mario_set_enable_key, 256)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        LOOP(0)
            CALL(lect_status_off)
            WAIT_FRAMES(1)
            CALL(evt_mario_get_motion, LW(3))
            IF_INT_EQUAL(LW(7), 0)
                IF_INT_EQUAL(LW(3), 17)
                    CALL(lecture_button_set_seq, 0, 3)
                    ADD(LW(7), 1)
                    CALL(x_flag_set, 1, 2)
                END_IF
            ELSE
                IF_INT_EQUAL(LW(7), 1)
                    IF_INT_NOT_EQUAL(LW(3), 17)
                        BREAK_LOOP
                    END_IF
                END_IF
            END_IF
        END_LOOP
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(lect_status_on)
        CALL(evt_mario_trigflag_onoff, 0, 134217728)
        CALL(evt_mario_trigflag_onoff, 0, 67108864)
        CALL(evt_mario_set_enable_key, 0)
        CALL(mainwindow_openenable)
        CALL(x_flag_set, 0, 2)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_adjust_dir)
        CALL(evt_msg_continue)
        CALL(evt_mario_set_pose, STRING("M_V_2"))
        WAIT_MS(500)
        CALL(lecture_button_seq_wait, 0, 1)
        WAIT_MS(300)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_adjust_dir)
        CALL(evt_mario_set_dir, 90, 150, 0)
        CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(26.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
        WAIT_MS(1000)
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_2"), STRING("T_2"))
        CALL(evt_msg_print, 0, STRING("lecture_s_boots_08"), 0, STRING("キノピコ"))
        CALL(evt_msg_select, 0, STRING("lecture_s_boots_09"))
        CALL(evt_npc_set_autotalkpose, STRING("キノピコ"), STRING("S_1"), STRING("T_1"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("lecture_s_boots_10"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("lecture_s_boots_11"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(80.0f), FLOAT(455.0f), FLOAT(52.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), 35)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 270)
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("A_1"))
    WAIT_MS(200)
    SET_VALUE(LW(1), STRING("TESTMOBJ"))
    CALL(evt_mobj_entry, LW(1), STRING("MOBJ_BlackBreakingFloor"))
    CALL(lect_mobj_hit_nasi, LW(1))
    CALL(evt_mobj_set_camid, LW(1), 7)
    CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
    ADD(LW(2), 50)
    ADD(LW(3), 150)
    SET_VALUE(LW(4), 0)
    CALL(evt_mobj_flag_onoff, 1, LW(1), 64)
    CALL(evt_mobj_set_position, LW(1), LW(2), LW(3), LW(4))
    CALL(evt_snd_sfxon, STRING("SFX_EVT_LECTURE_BLOCK_FALL1"), LW(7))
    CALL(evt_mobj_set_gravity_bound, LW(1), FLOAT(0.16602f), FLOAT(1.0f))
    WAIT_MS(600)
    CALL(evt_snd_sfxoff, LW(7))
    CALL(evt_snd_sfxon, STRING("SFX_EVT_LECTURE_BLOCK_DOWN1"), 0)
    WAIT_MS(400)
    CALL(evt_npc_set_anim, STRING("キノピコ"), STRING("A_2"))
    WAIT_MS(170)
    CALL(evt_msg_print, 0, STRING("lecture_s_boots_12"), 0, STRING("キノピコ"))
    WAIT_MS(300)
    LOOP(6)
        CALL(evt_mobj_flag_onoff, 1, LW(1), 16384)
        WAIT_MS(100)
        CALL(evt_mobj_flag_onoff, 0, LW(1), 16384)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mobj_delete, LW(1))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("lecture_s_boots_13"), 0, STRING("キノピコ"))
    CALL(evt_npc_get_position, STRING("キノピコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), 100)
    CALL(evt_npc_set_ry, STRING("キノピコ"), 90)
    WAIT_MS(300)
    CALL(evt_npc_move_position, STRING("キノピコ"), LW(0), LW(2), 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_position, STRING("キノピコ"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(all_juyoitem_lecture_rel_19_data_3A540)
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
        CASE_EQUAL(7)
        CALL_CHILD(boots_lv2_juyoitem_lecture_init)
    END_SWITCH
    SWITCH(LW(10))
        CASE_EQUAL(7)
        CALL_CHILD(boots_lv2_juyoitem_lecture)
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

// clang-format on
