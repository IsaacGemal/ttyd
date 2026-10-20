#include "evt_lect_gon.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_lecture.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_roll.h"
#include "motion/mot_slit.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static f32 lect_mario_ido_sub(f32 f1, f32 f2, f32 f3, f32 f4);
static USER_FUNC(lect_mario_ido_sum);
static USER_FUNC(lect_status_off);
static USER_FUNC(lect_status_on);
static USER_FUNC(x_flag_set);
static USER_FUNC(x_flag_add);
static USER_FUNC(x_flag_get);
static USER_FUNC(x_flag_wait);
static void lecture_button_disp(CameraId cameraId, void* param);
static USER_FUNC(lecture_button_rtn);
static USER_FUNC(lecture_button_set_icon_id);
static USER_FUNC(lecture_button_set_alpha);
static USER_FUNC(lecture_button_flag_on_off);
static USER_FUNC(lecture_button_set_seq);
static USER_FUNC(lecture_button_get_icon_id);
static USER_FUNC(lecture_button_seq_wait);
static USER_FUNC(lecture_button_set_pos);
static void lect_map_cam_chg_sub(MapObject* obj, s32 camId, s32 arg);
static USER_FUNC(lect_map_cam_chg);
static USER_FUNC(effitemgetchange_cam);
static USER_FUNC(effitemgetchange_cam_ptr);
static USER_FUNC(effitemget_open);
static USER_FUNC(effitemget_open_ptr);
static USER_FUNC(effitemget_busaiku);
static USER_FUNC(lect_mobj_hit_nasi);
static USER_FUNC(dai_moveti_set);
static USER_FUNC(dai_moveti_get);
static USER_FUNC(lect_slit_check);
static USER_FUNC(evtflash);
static USER_FUNC(lect_push_r_check);
static USER_FUNC(evt_nomal_cange);
static USER_FUNC(bgm_start_wait);

// icondrv's header declares iconId as s16, but this unit's original passes it zero-extended (u16);
// tools/local_prototype_exceptions.json lists iconDispGxAlpha for exactly this ABI disagreement.
void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);

typedef struct LectureButton {
    u8 flags;        // 0x0
    u8 seq;          // 0x1
    u8 pad_2[0xA];   // 0x2
    Vec pos;         // 0xC
    u8 pad_18[0x28]; // 0x18
    s16 iconId;      // 0x40
    s16 alpha;       // 0x42
} LectureButton;     // 0x44

// .bss is emitted in reverse declaration order; this order reproduces the original's layout.
static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static Vec daiv;

// clang-format off
extend_data(100,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(101,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// the original keeps the three calls as real `bl`s
#pragma dont_inline on
static f32 lect_mario_ido_sub(f32 f1, f32 f2, f32 f3, f32 f4) {
    f32 f0 = f1 * f4;
    f32 f5 = f2 - f1;
    return (f5 * f3 + f0) / f2;
}
#pragma dont_inline off

static USER_FUNC(lect_mario_ido_sum) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 total = evtGetValue(event, args[0]);
    s32 elapsed;
    f32 start;
    f32 end;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(0), mario->position.x);
        evtSetFloat(event, LW(1), mario->position.y);
        evtSetFloat(event, LW(2), mario->position.z);
    }

    elapsed = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (elapsed > total) {
        elapsed = total;
    }

    mario->position.x =
        lect_mario_ido_sub((f32)elapsed, (f32)total, evtGetFloat(event, LW(3)), evtGetFloat(event, LW(0)));
    mario->position.y =
        lect_mario_ido_sub((f32)elapsed, (f32)total, evtGetFloat(event, LW(4)), evtGetFloat(event, LW(1)));
    mario->position.z =
        lect_mario_ido_sub((f32)elapsed, (f32)total, evtGetFloat(event, LW(5)), evtGetFloat(event, LW(2)));

    if (elapsed >= total) {
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
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[idx] = value;
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_add) {
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
    s32 value = evtGetValue(event, args[1]);

    return x_flag_data[idx] == value ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// The iconId 0x75..0x7D range test is a switch: MWCC lowers the grouped cases to the
// original's `cmpwi 0x7e; bge zero; cmpwi 0x75; bge neg15; b zero` chain. Writing it as
// `if (a < 0x7E && a >= 0x75)` instead makes MWCC invert the second branch (`blt zero`).
// The position base must be built as `(u8*)b + 8`, then `+ 4`, so the original's `addi r4, r9, 0x8`
// plus its 0x4/0x8/0xC field loads are reproduced (see the MUST_MATCH block).
static void lecture_button_disp(CameraId cameraId, void* param) {
    s32 i;

    for (i = 0; i < 2; i++) {
        LectureButton* b = &lbtn[i];
#ifdef MUST_MATCH
        f32* f = (f32*)((u8*)b + 8);
#endif
        f32 yoff;
        Vec pos;

        if (b->flags & 1) {
            continue;
        }
        switch (b->iconId) {
            case 0x75: case 0x76: case 0x77: case 0x78: case 0x79:
            case 0x7A: case 0x7B: case 0x7C: case 0x7D:
                yoff = -15.0f;
                break;
            default:
                yoff = 0.0f;
                break;
        }
#ifdef MUST_MATCH
        pos = *(Vec*)(f + 1);
#else
        pos = b->pos;
#endif
        pos.y += yoff;
        iconDispGxAlpha(&pos, 0x50, (u16)b->iconId, (u8)b->alpha, 1.0f);
    }
}


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
                lbtn[i].flags |= 1;
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
    LectureButton* btn = &lbtn[idx];

    btn->iconId = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_alpha) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];

    btn->alpha = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    u8 mask = evtGetValue(event, args[1]);
    s32 on = evtGetValue(event, args[2]);
    LectureButton* btn = &lbtn[idx];

    if (on) {
        btn->flags |= mask;
    } else {
        btn->flags &= ~mask;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[idx];

    btn->seq = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_get_icon_id) {
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

    btn->pos = (Vec){x, y, z};
    return EVT_RETURN_DONE;
}

// the original keeps the recursive call as a real `bl`
#pragma dont_inline on
static void lect_map_cam_chg_sub(MapObject* obj, s32 camId, s32 arg) {
    obj->field_0x5 = camId;
    if (obj->child != NULL) {
        lect_map_cam_chg_sub(obj->child, camId, 1);
    }
    if (arg != 0 && obj->next != NULL) {
        lect_map_cam_chg_sub(obj->next, camId, 1);
    }
}
#pragma dont_inline off

static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    MapObject* obj = mapGetMapObj((const char*)name);

    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, camId, 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    ((s32*)eff->userdata)[0xE] = camId;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);

    ((s32*)eff->userdata)[0xE] = camId;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open) {
    s32 name = evtGetValue(event, event->args[0]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    ((s32*)eff->userdata)[8] = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open_ptr) {
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, event->args[0]);

    ((s32*)eff->userdata)[8] = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_busaiku) {
    s32 flag = evtGetValue(event, event->args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec vec;

    if (flag) {
        vec = (Vec){0.0f, 2016.0f, 0.0f};
    } else {
        vec = (Vec){0.0f, -2016.0f, 0.0f};
    }
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    PSVECAdd(&cam->cameraPos, &vec, &cam->storedPos);
    PSVECAdd(&cam->cameraAt, &vec, &cam->storedAt);
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_mobj_hit_nasi) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
    MapObjectEntry* obj;
    s32 i;

    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }

    for (i = 0; i < 2; i++) {
        if (obj->hitObj[i].name != NULL && strcmp(obj->hitObj[i].name, "") != 0) {
            hitGrpFlagOn(obj->hitObj[i].name, 1);
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

static USER_FUNC(lect_slit_check) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (mario->flags & (1 << 20)) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evtflash) {
    if (isFirstCall) {
        event->userdata[2] = 0;
    }

    switch (event->userdata[2]) {
        case 0:
            fadeEntry(FADE_OUT_BLACK, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            event->userdata[2]++;
            // fallthrough
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
            if ((u32)OSTicksToMilliseconds((u32)gp->renderTime - (u32)event->savedTime) <
                (u32)event->userdata[0]) {
                return EVT_RETURN_BLOCK;
            }
            event->userdata[2]++;
            return EVT_RETURN_BLOCK;
        case 3:
            fadeEntry(FADE_IN_BLACK, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            event->userdata[2]++;
            // fallthrough
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
            return (u32)OSTicksToMilliseconds((u32)gp->renderTime - (u32)event->savedTime) <
                           (u32)event->userdata[0]
                       ? EVT_RETURN_BLOCK
                       : EVT_RETURN_DONE;
        default:
            return EVT_RETURN_DONE;
    }
}

static USER_FUNC(lect_push_r_check) {
    s32* args = event->args;
    u32 button = keyGetButton(0);

    if (button & 0x20) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(evt_nomal_cange) {
    MarioWork* mario = marioGetPtr();

    if (isFirstCall) {
        unk_8009f9c8();
        if (mario->flags & (1 << 20)) {
            motSlitCancel3();
        }
    }

    if (!marioRollKeyDisable()) {
        return EVT_RETURN_BLOCK;
    }
    return (mario->dispFlags & 0x01000000) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static USER_FUNC(bgm_start_wait) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(slit_majin_lecture_init)
    CALL(lect_set_systemlevel, 0)
    CALL(lect_test_systemlevel)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

EVT_BEGIN(slit_majin_lecture)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
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
    CALL(evt_msg_print, 0, STRING("stg1_gon_12_12"), 0, STRING("ペラ魔人"))
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    LOOP(4)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_LIGHT1"), LW(0), LW(1), LW(2), 0)
        CALL(evtflash)
    END_LOOP
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_gon_12_13"), 0, STRING("ペラ魔人"))
    CALL(evt_msg_print_add, 0, STRING("stg1_gon_12_14"))
    LOOP(0)
        LOOP(0)
            CALL(lecture_button_flag_on_off, 0, 2, 1)
            CALL(lecture_button_set_icon_id, 0, 128)
            CALL(lecture_button_set_seq, 0, 2)
            CALL(lecture_button_seq_wait, 0, 1)
            CALL(x_flag_set, 0, 1)
            BEGIN_INLINE_EVENT
                LOOP(0)
                    CALL(lecture_button_set_icon_id, 0, 129)
                    WAIT_MS(300)
                    CALL(x_flag_get, 0, LW(0))
                    IF_INT_EQUAL(LW(0), 2)
                        BREAK_LOOP
                    END_IF
                    CALL(lecture_button_set_icon_id, 0, 128)
                    WAIT_MS(300)
                    CALL(x_flag_get, 0, LW(0))
                    IF_INT_EQUAL(LW(0), 2)
                        BREAK_LOOP
                    END_IF
                END_LOOP
            END_INLINE_EVENT
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SET_VALUE(LW(7), 0)
            CALL(evt_cam_letter_box_disable, 1)
            CALL(mainwindow_opendisable)
            CALL(evt_mario_trigflag_onoff, 1, 134217728)
            CALL(evt_mario_trigflag_onoff, 1, 67108864)
            CALL(evt_mario_set_enable_key, 32)
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_mario_key_onoff, 1)
            LOOP(0)
                CALL(lect_status_off)
                CALL(evt_mario_get_pos, 0, LW(5), LW(1), LW(6))
                CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
                WAIT_FRAMES(1)
                CALL(lect_slit_check, LW(3))
                IF_INT_EQUAL(LW(7), 0)
                    IF_INT_EQUAL(LW(3), 1)
                        ADD(LW(7), 1)
                        SET_VALUE(LW(11), 0)
                    END_IF
                ELSE
                    IF_INT_EQUAL(LW(7), 1)
                        IF_INT_EQUAL(LW(3), 0)
                            SET_VALUE(LW(11), 0)
                            BREAK_LOOP
                        END_IF
                        ADD(LW(11), 1)
                        IF_INT_GREATER_OR_EQUAL(LW(11), 180)
                            SET_VALUE(LW(11), 1)
                            BREAK_LOOP
                        END_IF
                    END_IF
                END_IF
            END_LOOP
            CALL(evt_msg_continue)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_mario_key_onoff, 0)
            CALL(lect_status_on)
            CALL(evt_mario_trigflag_onoff, 0, 134217728)
            CALL(evt_mario_trigflag_onoff, 0, 67108864)
            CALL(evt_mario_set_enable_key, 0)
            CALL(mainwindow_openenable)
            CALL(x_flag_set, 0, 2)
            CALL(evt_cam_letter_box_disable, 0)
            CALL(lecture_button_set_seq, 0, 3)
            CALL(evt_mario_normalize)
            IF_INT_EQUAL(LW(11), 1)
                LOOP(0)
                    WAIT_FRAMES(1)
                    CALL(lect_slit_check, LW(3))
                    IF_INT_EQUAL(LW(3), 0)
                        BREAK_LOOP
                    END_IF
                END_LOOP
            END_IF
            CALL(evt_mario_adjust_dir)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(lecture_button_seq_wait, 0, 1)
            WAIT_MS(300)
            IF_INT_EQUAL(LW(11), 1)
                BREAK_LOOP
            ELSE
                CALL(evt_msg_toge, 1, 0, 0, 0)
                CALL(evt_msg_print, 0, STRING("stg1_gon_12_15_1"), 0, STRING("ペラ魔人"))
            END_IF
        END_LOOP
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg1_gon_12_15"), 0, STRING("ペラ魔人"))
        CALL(evt_msg_select, 0, STRING("stg1_gon_12_15_yn"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("stg1_gon_12_16"))
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print_add, 0, STRING("stg1_gon_12_17"))
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 4)
    CALL(evt_npc_set_position, STRING("ペラ魔人"), 0, -1000, 0)
    RETURN
EVT_END()
// clang-format on
