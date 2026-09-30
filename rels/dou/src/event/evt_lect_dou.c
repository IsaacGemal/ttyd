#include "evt_lect_dou.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/fadedrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "eff/eff_itemget.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_lecture.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_roll.h"
#include "motion/mot_slit.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"

#include <dolphin/os.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static Vec daiv;

// clang-format off
extend_data(92,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(93,
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

// NONMATCHING (85.1%): value/min->float conversions and f30/f31 register assignment for t vs the first
// evtGetFloat temp differ; logic and instruction mix are otherwise identical.
static USER_FUNC(lect_mario_ido_sum) {
    EventEntry* evt = event;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 value = evtGetValue(event, args[0]);
    s32 min;
    if (isFirstCall) {
        wwstart = evt->lifetime;
        evtSetFloat(event, LW(0), mario->position.x);
        evtSetFloat(event, LW(1), mario->position.y);
        evtSetFloat(event, LW(2), mario->position.z);
    }
    min = (s32)((s64)((s32)evt->lifetime - (s32)wwstart) / (u64)OSMillisecondsToTicks(1));
    if (min > value) {
        min = value;
    }
    {
        f32 v0 = evtGetFloat(event, LW(0));
        f32 v3 = evtGetFloat(event, LW(3));
        f32 t = (f32)value - (f32)min;
        mario->position.x = (min * v0 + t * v3) / value;
        mario->position.y = (min * evtGetFloat(event, LW(1)) + t * evtGetFloat(event, LW(4))) / value;
        mario->position.z = (min * evtGetFloat(event, LW(2)) + t * evtGetFloat(event, LW(5))) / value;
    }
    return min >= value ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

static USER_FUNC(lect_status_on) {
    statusWinForceClose();
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_status_off) {
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
    s32 value = evtGetValue(event, args[1]);
    return value == x_flag_data[index] ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

static void lecture_button_disp(CameraId camId, void* param) {
    s32 i;
    for (i = 0; i < 2; i++) {
        LectureButton* btn = &lbtn[i];
#ifdef MUST_MATCH
        // The original addresses the button Vec off &lbtn[i] + 8 with a +4 displacement; matching MWCC's
        // base register needs the pointer built the same way. The natural btn->position scores lower.
        u8* ppos = (u8*)&lbtn[i] + 8;
#endif
        if (!(btn->flags & 1)) {
            Vec pos;
            f32 yOff;

            switch (btn->iconId) {
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
                yOff = -15.0f;
                break;
            default:
                yOff = 0.0f;
                break;
            }
#ifdef MUST_MATCH
            pos = *(Vec*)(ppos + 4);
#else
            pos = btn->position;
#endif
            pos.y += yOff;
            iconDispGxAlpha(&pos, 0x50, btn->iconId, btn->alpha, 1.0f);
        }
    }
}

static USER_FUNC(lecture_button_rtn) {
    LectureButton* btn;
    s32 i;
    s32 count;
    if (isFirstCall) {
        memset(lbtn, 0, sizeof(lbtn));
        {
            Vec pos = {200.0f, -25.0f, 0.0f};
            for (i = 0; i < 2; i++) {
                lbtn[i].position = pos;
            }
        }
    }
    count = 0;
    for (i = 0; i < 2; i++) {
        btn = &lbtn[i];
        switch (btn->state) {
            case 0:
                btn->iconId = 0x68;
                btn->state = 1;
                break;
            case 2:
                btn->alpha += 7;
                if (btn->alpha >= 0xff) {
                    btn->alpha = 0xff;
                    btn->state = 1;
                }
                break;
            case 3:
                btn->alpha -= 7;
                if (btn->alpha <= 0) {
                    btn->alpha = 0;
                    btn->state = 1;
                }
                break;
            case 4:
                count++;
                btn->flags |= 1;
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
    LectureButton* btn = &lbtn[index];
    btn->iconId = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_alpha) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[index];
    btn->alpha = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    u8 mask = evtGetValue(event, args[1]);
    s32 value = evtGetValue(event, args[2]);
    LectureButton* btn = &lbtn[index];
    if (value) {
        btn->flags |= mask;
    } else {
        btn->flags &= ~mask;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[index];
    btn->state = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_get_icon_id) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[index];
    evtSetValue(event, args[1], btn->state);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_seq_wait) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    LectureButton* btn = &lbtn[index];
    if (btn->state != evtGetValue(event, args[1])) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    Vec pos = {0.0f, 0.0f, 0.0f};
    pos.x = x;
    pos.y = y;
    pos.z = z;
    *(Vec*)&lbtn[index].position = pos;
    return EVT_RETURN_DONE;
}

static void lect_map_cam_chg_sub(MapObject* obj, s32 value, s32 recurse) {
    obj->field_0x0[5] = value;
    if (obj->child != NULL) {
        lect_map_cam_chg_sub(obj->child, value, 1);
    }
    if (recurse && obj->next != NULL) {
        lect_map_cam_chg_sub(obj->next, value, 1);
    }
}

static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);
    MapObject* obj = mapGetMapObj(name);
    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, value, 0);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    effItemGetData* data = effNameToPtr(name)->userdata;
    data->field_0x38 = camId;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    effItemGetData* data = eff->userdata;
    data->field_0x38 = camId;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open) {
    s32* args = event->args;
    effItemGetData* data = effNameToPtr((const char*)evtGetValue(event, args[0]))->userdata;
    data->field_0x20 = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_open_ptr) {
    s32* args = event->args;
    effItemGetData* data = ((EffectEntry*)evtGetValue(event, args[0]))->userdata;
    data->field_0x20 = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(effitemget_busaiku) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec v;
    if (flag != 0) {
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
    cam->curveA = 0xb;
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_mobj_hit_nasi) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    MapObjectEntry* mobj;
    s32 i;
    if (strcmp(name, "me") != 0) {
        mobj = mobjNameToPtr(name);
    } else {
        mobj = event->thisMapObj;
    }
    for (i = 0; i < 2; i++) {
        if (mobj->hitObj[i].name != NULL && strcmp(mobj->hitObj[i].name, "") != 0) {
            hitGrpFlagOn(mobj->hitObj[i].name, 1);
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(dai_moveti_set) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    Vec v = {0.0f, 0.0f, 0.0f};
    v.x = x;
    v.y = y;
    v.z = z;
    daiv = v;
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
    if (marioGetPtr()->flags & (1 << 20)) {
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
            fadeEntry(10, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
            ticks = OSMillisecondsToTicks(1);
            elapsed = (u32)gp->renderTime - (u32)event->savedTime;
            if (elapsed / ticks < (u32)event->userdata[0]) {
                return EVT_RETURN_BLOCK;
            }
            event->userdata[2]++;
            return EVT_RETURN_BLOCK;
        case 3:
            fadeEntry(9, 40, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
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
            ticks = OSMillisecondsToTicks(1);
            elapsed = (u32)gp->renderTime - (u32)event->savedTime;
            return elapsed / ticks < (u32)event->userdata[0] ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
        default:
            return EVT_RETURN_DONE;
    }
}

static USER_FUNC(lect_push_r_check) {
    s32* args = event->args;
    if (keyGetButton(0) & 0x20) {
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
    if (marioRollKeyDisable() == FALSE) {
        return EVT_RETURN_BLOCK;
    }
    return ((mario->dispFlags >> 24) & 1) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static s32 bgm_start_wait(void) {
    return psndBGMStartCheck(1) != 0 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(ship_majin_lecture_init)
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
    CALL(lect_map_cam_chg, STRING("S_Ship_Change_01"), 7)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

EVT_BEGIN(ship_majin_lecture)
    CALL(evt_mario_trigflag_onoff, 1, 4)
    BEGIN_SIBLING_EVENT
        CALL(lecture_button_rtn)
    END_SIBLING_EVENT
    WAIT_FRAMES(1)
    CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_Ship_Change_01"))
        SET_VALUE(LW(1), STRING("A_rec"))
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
        CALL(dai_moveti_set, FLOAT(-50.0f), FLOAT(0.0f), FLOAT(0.0f))
        LOOP(0)
            CALL(dai_moveti_get, LW(2), LW(3), LW(4))
            CALL(evt_mapobj_trans, 1, LW(0), LW(2), LW(3), LW(4))
            CALL(evt_hit_bind_update, LW(1))
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
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_12"), 0, STRING("ペラ魔人"))
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    LOOP(4)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_LIGHT1"), LW(0), LW(1), LW(2), 0)
        CALL(evtflash)
    END_LOOP
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_13"), 0, STRING("ペラ魔人"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(80.0f), FLOAT(42.0f), FLOAT(-101.8f), 1000, 11)
    SET_FLOAT_VALUE(LW(1), FLOAT(2.66602f))
    CALL(dai_moveti_get, LW(0), LW(2), LW(3))
    LOOP(0)
        SUBTRACT_FLOAT(LW(0), LW(1))
        IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(-270.0f))
            SET_FLOAT_VALUE(LW(0), FLOAT(-270.0f))
            CALL(dai_moveti_set, LW(0), LW(2), LW(3))
            BREAK_LOOP
        ELSE
            CALL(dai_moveti_set, LW(0), LW(2), LW(3))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_14"), 0, STRING("ペラ魔人"))
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(0), -220)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(0)
            CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(98.0f), FLOAT(558.0f), FLOAT(0.0f), FLOAT(42.0f), FLOAT(-101.8f), 0, 11)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(x_flag_set, 0, 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("S_Ship_Change_01"))
        SET_VALUE(LW(1), STRING("Ship_Change_01"))
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
    CALL(evt_mario_mov_pos2, FLOAT(-200.0f), FLOAT(0.0f), FLOAT(80.0f))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_14_1"), 0, STRING("ペラ魔人"))
    LOOP(0)
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
        CALL(evt_cam_letter_box_disable, 1)
        CALL(mainwindow_opendisable)
        CALL(evt_mario_trigflag_onoff, 1, 134217728)
        CALL(evt_mario_trigflag_onoff, 1, 67108864)
        CALL(evt_mario_set_enable_key, 2048)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        LOOP(0)
            CALL(lect_status_on)
            WAIT_FRAMES(1)
            CALL(evt_mario_get_motion, LW(3))
            IF_INT_EQUAL(LW(3), 25)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(lect_status_off)
        CALL(evt_mario_trigflag_onoff, 0, 134217728)
        CALL(evt_mario_trigflag_onoff, 0, 67108864)
        CALL(evt_mario_set_enable_key, 0)
        CALL(mainwindow_openenable)
        CALL(evt_cam_letter_box_disable, 0)
        BEGIN_SIBLING_EVENT
            CALL(lecture_button_set_icon_id, 0, 107)
            WAIT_MS(300)
            CALL(lecture_button_set_seq, 0, 3)
            CALL(lecture_button_seq_wait, 0, 1)
            CALL(lecture_button_flag_on_off, 0, 2, 0)
        END_SIBLING_EVENT
        CALL(x_flag_set, 0, 2)
        CALL(lecture_button_set_seq, 0, 3)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(1), FLOAT(-614.0f))
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_set_dir, 90, 300, 0)
        WAIT_MS(500)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_flag_on_off, 0, 2, 0)
        CALL(evt_msg_print_add, 0, STRING("stg1_gon_majin4_15"))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1"), LW(0), LW(1), LW(2), LW(3))
        CALL(evt_mario_mov_pos2, FLOAT(155.0f), FLOAT(0.0f), FLOAT(80.0f))
        CALL(evt_snd_sfxoff, LW(3))
        CALL(evt_mario_set_dir, 270, 300, 0)
        WAIT_MS(500)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1"), LW(0), LW(1), LW(2), LW(3))
        CALL(evt_mario_mov_pos2, FLOAT(-145.0f), FLOAT(0.0f), FLOAT(80.0f))
        CALL(evt_snd_sfxoff, LW(3))
        WAIT_MS(500)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg1_gon_majin4_15_0"), 0, STRING("ペラ魔人"))
        CALL(lecture_button_flag_on_off, 0, 2, 1)
        CALL(lecture_button_set_pos, 0, FLOAT(-200.0f), FLOAT(-25.0f), FLOAT(0.0f))
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
        CALL(evt_cam_letter_box_disable, 1)
        CALL(mainwindow_opendisable)
        CALL(evt_mario_trigflag_onoff, 1, 134217728)
        CALL(evt_mario_trigflag_onoff, 1, 67108864)
        CALL(evt_mario_set_enable_key, 2048)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        LOOP(0)
            CALL(lect_status_on)
            CALL(evt_mario_get_motion, LW(3))
            IF_INT_NOT_EQUAL(LW(3), 25)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_key_onoff, 0)
        CALL(lect_status_off)
        CALL(evt_mario_trigflag_onoff, 0, 134217728)
        CALL(evt_mario_trigflag_onoff, 0, 67108864)
        CALL(evt_mario_set_enable_key, 0)
        CALL(mainwindow_openenable)
        CALL(x_flag_set, 0, 2)
        CALL(lecture_button_set_seq, 0, 3)
        WAIT_FRAMES(1)
        CALL(lecture_button_seq_wait, 0, 1)
        CALL(lecture_button_flag_on_off, 0, 2, 0)
        CALL(x_flag_set, 0, 1)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(0), STRING("S_Ship_Change_01"))
            SET_VALUE(LW(1), STRING("Ship_Change_01"))
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
        CALL(evt_msg_print_add, 0, STRING("stg1_gon_majin4_15_1"))
        CALL(evt_msg_select, 0, STRING("stg1_gon_majin4_16"))
        CALL(evt_cam_letter_box_disable, 0)
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_mario_set_dir, 90, 300, 0)
            CALL(evt_msg_print_add, 0, STRING("stg1_gon_majin4_17"))
        ELSE
            CALL(x_flag_set, 0, 2)
            WAIT_FRAMES(2)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(x_flag_set, 0, 1)
        SET_FLOAT_VALUE(LW(1), FLOAT(2.66602f))
        CALL(dai_moveti_get, LW(0), LW(2), LW(3))
        LOOP(0)
            ADD_FLOAT(LW(0), LW(1))
            IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(-50.0f))
                SET_FLOAT_VALUE(LW(0), FLOAT(-50.0f))
                CALL(dai_moveti_set, LW(0), LW(2), LW(3))
                CALL(x_flag_set, 0, 2)
                BREAK_LOOP
            ELSE
                CALL(dai_moveti_set, LW(0), LW(2), LW(3))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print_add, 0, STRING("stg1_gon_majin4_18"))
    CALL(x_flag_wait, 0, 2)
    CALL(lect_map_cam_chg, STRING("S_Ship_Change_01"), 4)
    CALL(evt_npc_set_anim, STRING("ペラ魔人"), STRING("P_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_camid, STRING("ペラ魔人"), 4)
    CALL(evt_npc_set_position, STRING("ペラ魔人"), 0, -1000, 0)
    CALL(evt_mario_trigflag_onoff, 0, 4)
    RETURN
EVT_END()

static OSTime wwstart;

static s32 x_flag_data[2];

static LectureButton lbtn[2];

static Vec daiv;

// clang-format on
