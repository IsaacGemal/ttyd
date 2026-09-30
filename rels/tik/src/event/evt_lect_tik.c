#include "evt_lect_tik.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_lecture.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#include <string.h>

#include <dolphin/mtx.h>

#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_itemget.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "motion/mot_roll.h"
#include "motion/mot_slit.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"

#pragma pool_data off

static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static Vec daiv;

// MapObject.field_0x5 is unnamed in mapdrv.h; byte 5 sits inside the field_0x0[8] padding array. It is the
// visibility flag the lecture sets.
#define MAPOBJ_FLAG(obj) ((obj)->field_0x0[5])

#pragma force_active on
extern s32 evt_mario_paper_plane[];
static s32 bgm_start_wait(void);
static s32 evt_nomal_cange(EventEntry* event, BOOL isFirstCall);
static s32 lect_push_r_check(EventEntry* event, BOOL isFirstCall);
static USER_FUNC(evtflash);
static s32 lect_slit_check(EventEntry* event, BOOL isFirstCall);
static USER_FUNC(dai_moveti_get);
static USER_FUNC(dai_moveti_set);
static s32 lect_mobj_hit_nasi(EventEntry* event, BOOL isFirstCall);
static s32 effitemget_busaiku(EventEntry* event, BOOL isFirstCall);
static s32 effitemget_open_ptr(EventEntry* event, BOOL isFirstCall);
static s32 effitemget_open(EventEntry* event, BOOL isFirstCall);
static s32 effitemgetchange_cam_ptr(EventEntry* event, BOOL isFirstCall);
static s32 effitemgetchange_cam(EventEntry* event, BOOL isFirstCall);
static USER_FUNC(lect_map_cam_chg);
static void lect_map_cam_chg_sub(MapObject* obj, s32 val, s32 flag);
static USER_FUNC(lecture_button_set_pos);
static USER_FUNC(lecture_button_seq_wait);
static s32 lecture_button_get_icon_id(EventEntry* event, BOOL isFirstCall);
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_flag_on_off);
static s32 lecture_button_set_alpha(EventEntry* event, BOOL isFirstCall);
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_rtn);
static void lecture_button_disp(CameraId cameraId, void* param);
static USER_FUNC(x_flag_wait);
static USER_FUNC(x_flag_get);
static s32 x_flag_add(EventEntry* event, BOOL isFirstCall);
static USER_FUNC(x_flag_set);
static s32 lect_status_on(void);
static s32 lect_status_off(void);
static USER_FUNC(lect_mario_ido_sum);
static f32 lect_mario_ido_sub(f32 a, f32 b, f32 c, f32 d);

// clang-format off
extend_data(148,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(149,
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
    s32 target = evtGetValue(event, args[0]);
    s32 elapsed;
    f32 a;
    f32 b;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, 0xFE363C80, mario->position.x);
        evtSetFloat(event, 0xFE363C81, mario->position.y);
        evtSetFloat(event, 0xFE363C82, mario->position.z);
    }
    elapsed = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (elapsed > target) {
        elapsed = target;
    }
    a = evtGetFloat(event, 0xFE363C80);
    b = evtGetFloat(event, 0xFE363C83);
    mario->position.x = lect_mario_ido_sub((f32)elapsed, (f32)target, b, a);
    a = evtGetFloat(event, 0xFE363C81);
    b = evtGetFloat(event, 0xFE363C84);
    mario->position.y = lect_mario_ido_sub((f32)elapsed, (f32)target, b, a);
    a = evtGetFloat(event, 0xFE363C82);
    b = evtGetFloat(event, 0xFE363C85);
    mario->position.z = lect_mario_ido_sub((f32)elapsed, (f32)target, b, a);
    if (elapsed >= target) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

static s32 lect_status_off(void) {
    statusWinForceClose();
    return EVT_RETURN_DONE;
}

static s32 lect_status_on(void) {
    statusWinForceCloseClear();
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_set) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[idx] = value;
    return EVT_RETURN_DONE;
}

static s32 x_flag_add(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[idx] += value;
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

    return target == x_flag_data[idx] ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// NONMATCHING (88.84%): the iconId range check's block layout differs. The original keeps the -15.0f block
// inline with an explicit branch to a shared 0.0f block; ours shares/inverts the second branch instead.
static void lecture_button_disp(CameraId cameraId, void* param) {
    s32 i;

    for (i = 0; i < 2; i++) {
        LectureButton* btn = &lbtn[i];
        Vec* ppos = &btn->position;

        if (!(btn->flags & 1)) {
            Vec pos;
            f32 offset;

            if (btn->iconId >= 0x7E) {
                goto l_zero;
            }
            if (btn->iconId >= 0x75) {
                goto l_neg;
            }
            goto l_zero;
        l_neg:
            offset = -15.0f;
            goto l_done;
        l_zero:
            offset = 0.0f;
        l_done:
            pos = *ppos;
            pos.y += offset;
            iconDispGxAlpha(&pos, 0x50, btn->iconId, btn->alpha, 1.0f);
        }
    }
}

// NONMATCHING (98.96%): the loop count lands in r6; the original uses r7 (register allocation, bytes otherwise
// equal).
static USER_FUNC(lecture_button_rtn) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        Vec v;

        memset(lbtn, 0, sizeof(lbtn));
        v = (Vec){200.0f, -25.0f, 0.0f};
        for (i = 0; i < 2; i++) {
            lbtn[i].position = v;
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
    dispEntry(8, 0.0f, 1, lecture_button_disp, NULL);
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(lecture_button_set_icon_id) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 iconId = evtGetValue(event, args[1]);
    LectureButton* btn = &lbtn[idx];

    btn->iconId = iconId;
    return EVT_RETURN_DONE;
}

static s32 lecture_button_set_alpha(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 alpha = evtGetValue(event, args[1]);
    LectureButton* btn = &lbtn[idx];

    btn->alpha = alpha;
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    u8 mask = evtGetValue(event, args[1]);
    s32 on = evtGetValue(event, args[2]);
    LectureButton* btn = &lbtn[idx];

    if (on != 0) {
        btn->flags |= mask;
    } else {
        btn->flags &= ~mask;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 seq = evtGetValue(event, args[1]);
    LectureButton* btn = &lbtn[idx];

    btn->seq = seq;
    return EVT_RETURN_DONE;
}

static s32 lecture_button_get_icon_id(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];

    evtSetValue(event, args[1], btn->seq);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_seq_wait) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 seq = evtGetValue(event, args[1]);
    LectureButton* btn = &lbtn[idx];

    return btn->seq != seq ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    LectureButton* btn = &lbtn[idx];

    btn->position = (Vec){x, y, z};
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.39%): lvl2's second value lands in r30; the original keeps it in r29 (register allocation).
static void lect_map_cam_chg_sub(MapObject* a, s32 b, s32 c) {
    MapObject* lvl3;
    MapObject* lvl2;
    MapObject* lvl1;

    MAPOBJ_FLAG(a) = b;
    lvl1 = a->child;
    if (lvl1 != NULL) {
        MAPOBJ_FLAG(lvl1) = b;
        lvl2 = lvl1->child;
        if (lvl2 != NULL) {
            MAPOBJ_FLAG(lvl2) = b;
            lvl3 = lvl2->child;
            if (lvl3 != NULL) {
                MAPOBJ_FLAG(lvl3) = b;
                if (lvl3->child != NULL) {
                    lect_map_cam_chg_sub(lvl3->child, b, 1);
                }
                if (lvl3->next != NULL) {
                    lect_map_cam_chg_sub(lvl3->next, b, 1);
                }
            }
            lvl3 = lvl2->next;
            if (lvl3 != NULL) {
                MAPOBJ_FLAG(lvl3) = b;
                if (lvl3->child != NULL) {
                    lect_map_cam_chg_sub(lvl3->child, b, 1);
                }
                if (lvl3->next != NULL) {
                    lect_map_cam_chg_sub(lvl3->next, b, 1);
                }
            }
        }
        lvl3 = lvl1->next;
        if (lvl3 != NULL) {
            MAPOBJ_FLAG(lvl3) = b;
            lvl2 = lvl3->child;
            if (lvl2 != NULL) {
                MAPOBJ_FLAG(lvl2) = b;
                if (lvl2->child != NULL) {
                    lect_map_cam_chg_sub(lvl2->child, b, 1);
                }
                if (lvl2->next != NULL) {
                    lect_map_cam_chg_sub(lvl2->next, b, 1);
                }
            }
            lvl3 = lvl3->next;
            if (lvl3 != NULL) {
                MAPOBJ_FLAG(lvl3) = b;
                if (lvl3->child != NULL) {
                    lect_map_cam_chg_sub(lvl3->child, b, 1);
                }
                if (lvl3->next != NULL) {
                    lect_map_cam_chg_sub(lvl3->next, b, 1);
                }
            }
        }
    }
    if (c != 0) {
        lvl3 = a->next;
        if (lvl3 != NULL) {
            MAPOBJ_FLAG(lvl3) = b;
            lvl2 = lvl3->child;
            if (lvl2 != NULL) {
                MAPOBJ_FLAG(lvl2) = b;
                lvl1 = lvl2->child;
                if (lvl1 != NULL) {
                    MAPOBJ_FLAG(lvl1) = b;
                    if (lvl1->child != NULL) {
                        lect_map_cam_chg_sub(lvl1->child, b, 1);
                    }
                    if (lvl1->next != NULL) {
                        lect_map_cam_chg_sub(lvl1->next, b, 1);
                    }
                }
                lvl2 = lvl2->next;
                if (lvl2 != NULL) {
                    MAPOBJ_FLAG(lvl2) = b;
                    if (lvl2->child != NULL) {
                        lect_map_cam_chg_sub(lvl2->child, b, 1);
                    }
                    if (lvl2->next != NULL) {
                        lect_map_cam_chg_sub(lvl2->next, b, 1);
                    }
                }
            }
            lvl3 = lvl3->next;
            if (lvl3 != NULL) {
                MAPOBJ_FLAG(lvl3) = b;
                lvl2 = lvl3->child;
                if (lvl2 != NULL) {
                    MAPOBJ_FLAG(lvl2) = b;
                    if (lvl2->child != NULL) {
                        lect_map_cam_chg_sub(lvl2->child, b, 1);
                    }
                    if (lvl2->next != NULL) {
                        lect_map_cam_chg_sub(lvl2->next, b, 1);
                    }
                }
                lvl3 = lvl3->next;
                if (lvl3 != NULL) {
                    MAPOBJ_FLAG(lvl3) = b;
                    if (lvl3->child != NULL) {
                        lect_map_cam_chg_sub(lvl3->child, b, 1);
                    }
                    if (lvl3->next != NULL) {
                        lect_map_cam_chg_sub(lvl3->next, b, 1);
                    }
                }
            }
        }
    }
}

static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    MapObject* obj;
    s32 name = evtGetValue(event, args[0]);
    s32 val = evtGetValue(event, args[1]);

    obj = mapGetMapObj((const char*)name);
    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, val, 0);
    }
    return EVT_RETURN_DONE;
}

static s32 effitemgetchange_cam(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 cam = evtGetValue(event, args[1]);
    EffectEntry* eff = effNameToPtr((const char*)name);
    effItemGetData* data = eff->userdata;

    data->field_0x38 = cam;
    return EVT_RETURN_DONE;
}

static s32 effitemgetchange_cam_ptr(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);
    s32 cam = evtGetValue(event, args[1]);
    effItemGetData* data = eff->userdata;

    data->field_0x38 = cam;
    return EVT_RETURN_DONE;
}

static s32 effitemget_open(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    EffectEntry* eff = effNameToPtr((const char*)evtGetValue(event, args[0]));
    effItemGetData* data = eff->userdata;

    data->field_0x20 = 1;
    return EVT_RETURN_DONE;
}

static s32 effitemget_open_ptr(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);
    effItemGetData* data = eff->userdata;

    data->field_0x20 = 1;
    return EVT_RETURN_DONE;
}

static s32 effitemget_busaiku(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 val = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec v;

    if (val != 0) {
        v = (Vec){0.0f, 2016.0f, 0.0f};
    } else {
        v = (Vec){0.0f, -2016.0f, 0.0f};
    }
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    PSVECAdd(&cam->cameraPos, &v, &cam->storedPos);
    PSVECAdd(&cam->cameraAt, &v, &cam->storedAt);
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}

static s32 lect_mobj_hit_nasi(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    MapObjectEntry* entry;
    s32 i;

    if (strcmp(name, "me") != 0) {
        entry = mobjNameToPtr(name);
    } else {
        entry = event->thisMapObj;
    }
    for (i = 0; i < 2; i++) {
        if (entry->hitObj[i].name != NULL && strcmp(entry->hitObj[i].name, "") != 0) {
            hitGrpFlagOn(entry->hitObj[i].name, 1);
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(dai_moveti_set) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    daiv = (Vec){x, y, z};
    return EVT_RETURN_DONE;
}

static USER_FUNC(dai_moveti_get) {
    s32* args = event->args;

    evtSetFloat(event, args[0], daiv.x);
    evtSetFloat(event, args[1], daiv.y);
    evtSetFloat(event, args[2], daiv.z);
    return EVT_RETURN_DONE;
}

static s32 lect_slit_check(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evtflash) {
    u32 ticks;
    u32 elapsed;

    if (isFirstCall) {
        event->userdata[2] = 0;
    }
    switch (event->userdata[2]) {
    case 0:
        fadeEntry(FADE_OUT_BLACK, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        event->userdata[2]++;
    case 1:
        if (fadeIsFinish() != TRUE) {
            return EVT_RETURN_BLOCK;
        }
        event->userdata[0] = 60;
        event->savedTime = (s32)gp->renderTime;
        event->userdata[2]++;
        if (event->userdata[0] == 0) {
            event->userdata[2]++;
        }
        return EVT_RETURN_BLOCK;
    case 2:
        ticks = OS_TIMER_CLOCK / 1000;
        elapsed = (u32)((s32)gp->renderTime - (s32)event->savedTime) / ticks;
        if (elapsed < (u32)event->userdata[0]) {
            return EVT_RETURN_BLOCK;
        }
        event->userdata[2]++;
        return EVT_RETURN_BLOCK;
    case 3:
        fadeEntry(FADE_IN_BLACK, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        event->userdata[2]++;
    case 4:
        if (fadeIsFinish() != TRUE) {
            return EVT_RETURN_BLOCK;
        }
        event->userdata[0] = 60;
        event->savedTime = (s32)gp->renderTime;
        event->userdata[2]++;
        if (event->userdata[0] == 0) {
            event->userdata[2]++;
        }
        return EVT_RETURN_BLOCK;
    case 5:
        ticks = OS_TIMER_CLOCK / 1000;
        elapsed = (u32)((s32)gp->renderTime - (s32)event->savedTime) / ticks;
        return elapsed < (u32)event->userdata[0] ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
    }
    return EVT_RETURN_DONE;
}

static s32 lect_push_r_check(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    if (keyGetButton(0) & 0x20) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static s32 evt_nomal_cange(EventEntry* event, BOOL isFirstCall) {
    MarioWork* mario = marioGetPtr();

    if (isFirstCall) {
        unk_8009f9c8();
        if (mario->flags & MARIO_FLAG_PAPER_MODE) {
            motSlitCancel3();
        }
    }
    if (!marioRollKeyDisable()) {
        return EVT_RETURN_BLOCK;
    }
    return (mario->dispFlags & 0x01000000) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static s32 bgm_start_wait(void) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(plane_majin_lecture_init)
    CALL(lect_set_systemlevel, 0)
    CALL(lect_test_systemlevel)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, -300, -600, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 0, 11)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pos, -300, -600, 0)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 0, 11)
    CALL(lect_map_cam_chg, STRING("hikou"), 7)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

EVT_BEGIN(plane_majin_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("hikou"))
        SET_VALUE(LW(1), STRING("a_asiba"))
        SET_VALUE(LW(5), STRING("a_hikou"))
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
        CALL(evt_hit_bind_mapobj, LW(5), LW(0))
        CALL(dai_moveti_set, FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
        LOOP(0)
            CALL(dai_moveti_get, LW(2), LW(3), LW(4))
            CALL(evt_mapobj_trans, 1, LW(0), LW(2), LW(3), LW(4))
            CALL(evt_hit_bind_update, LW(1))
            CALL(evt_hit_bind_update, LW(5))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("ペラ魔人"), 1073741856)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 7)
    WAIT_FRAMES(1)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_cam_get_at, 4, LW(0), LW(1), LW(2))
            SUBTRACT(LW(1), 25)
            CALL(evt_npc_set_position, STRING("ペラ魔人"), LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_1"))
    WAIT_MS(200)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_majin_13"), 0, STRING("ペラ魔人"))
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    LOOP(4)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_LIGHT1"), LW(0), LW(1), LW(2), 0)
        CALL(evtflash)
    END_LOOP
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_majin_14"), 0, STRING("ペラ魔人"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(80.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 11)
    SET_FLOAT_VALUE(LW(1), FLOAT(2.66602f))
    CALL(dai_moveti_get, LW(0), LW(2), LW(3))
    LOOP(0)
        SUBTRACT_FLOAT(LW(0), LW(1))
        IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(-220.0f))
            SET_FLOAT_VALUE(LW(0), FLOAT(-220.0f))
            CALL(dai_moveti_set, LW(0), LW(2), LW(3))
            BREAK_LOOP
        ELSE
            CALL(dai_moveti_set, LW(0), LW(2), LW(3))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_majin_15"), 0, STRING("ペラ魔人"))
    LOOP(0)
        CALL(x_flag_set, 0, 1)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(0), STRING("hikou"))
            SET_VALUE(LW(1), STRING("a_hikou_ue"))
            LOOP(0)
                SPAWN_EVENT_GET_ID(PTR(evt_ride_airport), LW(2))
                WAIT_FRAMES(2)
                IS_EVENT_RUNNING(LW(2), LW(3))
                IF_INT_EQUAL(LW(3), 1)
                    BREAK_LOOP
                END_IF
            END_LOOP
            CALL(x_flag_wait, 0, 2)
            IS_EVENT_RUNNING(LW(2), LW(3))
            IF_INT_EQUAL(LW(3), 1)
                DELETE_EVENT(LW(2))
            END_IF
            LOOP(0)
                SPAWN_EVENT_GET_ID(PTR(evt_nonride_airport), LW(2))
                WAIT_FRAMES(2)
                IS_EVENT_RUNNING(LW(2), LW(3))
                IF_INT_EQUAL(LW(3), 1)
                    BREAK_LOOP
                END_IF
            END_LOOP
            IS_EVENT_RUNNING(LW(2), LW(3))
            IF_INT_EQUAL(LW(3), 1)
                DELETE_EVENT(LW(2))
            END_IF
        END_INLINE_EVENT
        CALL(x_flag_set, 1, 1)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                IF_INT_GREATER_OR_EQUAL(LW(0), -220)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            LOOP(0)
                CALL(x_flag_get, 1, LW(0))
                IF_INT_NOT_EQUAL(LW(0), 3)
                    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 0, 11)
                END_IF
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(evt_mario_mov_pos2, FLOAT(-200.0f), FLOAT(0.0f), FLOAT(80.0f))
        CALL(evt_msg_print_add, 0, STRING("mac_majin_15_1"))
        CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_icon_id, 0, 106)
        CALL(lecture_button_set_seq, 0, 2)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(x_flag_set, 0, 1)
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(lecture_button_set_icon_id, 0, 107)
                WAIT_MS(300)
                CALL(x_flag_get, 0, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
                CALL(lecture_button_set_icon_id, 0, 106)
                WAIT_MS(300)
                CALL(x_flag_get, 0, LW(0))
                IF_INT_EQUAL(LW(0), 2)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_INLINE_EVENT
        LOOP(0)
            CALL(evt_key_get_buttontrg, 0, LW(0))
            IF_BITS_SET(LW(0), 2048)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_continue)
        CALL(evt_mario_paper_plane, 1)
        CALL(x_flag_set, 0, 2)
        CALL(lecture_button_set_seq, 0, 3)
        BEGIN_INLINE_EVENT
            CALL(x_flag_set, 0, 1)
            LOOP(0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                IF_INT_LESS_OR_EQUAL(LW(1), -750)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(x_flag_set, 1, 3)
            WAIT_FRAMES(1)
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            CALL(dai_moveti_set, FLOAT(3000.0f), FLOAT(0.0f), FLOAT(0.0f))
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            SET_VALUE(LW(0), -300)
            SET_VALUE(LW(1), -490)
            SET_VALUE(LW(2), 0)
            CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
            CALL(evt_cam3d_evt_set, FLOAT(-297.0f), FLOAT(-351.0f), FLOAT(457.0f), FLOAT(-297.0f), FLOAT(-449.0f), FLOAT(-101.8f), 1000, 0)
            CALL(evt_mario_cont_onoff, 0)
            CALL(lect_mario_ido_sum, 1000)
            CALL(evt_mario_cont_onoff, 1)
            CALL(dai_moveti_set, FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
            CALL(x_flag_set, 1, 1)
            LOOP(0)
                CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
                CALL(x_flag_get, 0, LW(3))
                IF_INT_NOT_EQUAL(LW(3), 1)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_flag_on_off, 0, 2, 0)
        WAIT_MS(300)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("mac_majin_16"), 0, STRING("ペラ魔人"))
        CALL(evt_msg_select, 0, STRING("mac_majin_17"))
        SET_VALUE(LW(3), LW(0))
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(1), -550)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(LW(3), 0)
            CALL(x_flag_set, 1, 2)
            WAIT_FRAMES(2)
            CALL(evt_cam3d_evt_set, FLOAT(-300.0f), FLOAT(-460.0f), FLOAT(457.0f), FLOAT(-300.0f), FLOAT(-558.0f), FLOAT(-101.8f), 1200, 0)
            CALL(x_flag_set, 0, 2)
            CALL(evt_mario_paper_plane, 0)
            CALL(evt_mario_wait_landon)
            CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(80.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 11)
            SET_FLOAT_VALUE(LW(1), FLOAT(2.66602f))
            CALL(dai_moveti_get, LW(0), LW(2), LW(3))
            LOOP(0)
                SUBTRACT_FLOAT(LW(0), LW(1))
                IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(-220.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(-220.0f))
                    CALL(dai_moveti_set, LW(0), LW(2), LW(3))
                    BREAK_LOOP
                ELSE
                    CALL(dai_moveti_set, LW(0), LW(2), LW(3))
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_msg_print_add, 0, STRING("mac_majin_18"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("mac_majin_18_1"))
    CALL(x_flag_set, 1, 2)
    WAIT_FRAMES(2)
    CALL(evt_cam3d_evt_set, FLOAT(-300.0f), FLOAT(-460.0f), FLOAT(457.0f), FLOAT(-300.0f), FLOAT(-558.0f), FLOAT(-101.8f), 1200, 0)
    CALL(x_flag_set, 0, 2)
    CALL(evt_mario_paper_plane, 0)
    CALL(evt_mario_wait_landon)
    CALL(evt_msg_print_add, 0, STRING("mac_majin_19"))
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 4)
    CALL(evt_npc_set_position, STRING("ペラ魔人"), 0, -1000, 0)
    RETURN
EVT_END()
