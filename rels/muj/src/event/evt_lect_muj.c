#include "evt_lect_muj.h"

#include "rel/common/evt_lect.h"
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
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "driver/npcdrv.h"
#include "eff/eff_fukidashi.h"
#include "eff/eff_itemget.h"
#include "eff/eff_mario_balloon.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "party.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static s32 bgm_start_wait(void);
static USER_FUNC(set_dir);
static USER_FUNC(lect_party_ido_sum);
static f32 lect_party_ido_sub(f32 a, f32 b, f32 c, f32 d);
static USER_FUNC(efffukidashichange_cam);
static USER_FUNC(lect_npc_balloon_on_off);
static USER_FUNC(lect_itemget_kesan);
static USER_FUNC(lect_mobj_hit_nasi);
static USER_FUNC(effitemget_busaiku);
static USER_FUNC(effitemget_open_ptr);
static USER_FUNC(effitemget_open);
static USER_FUNC(effitemgetchange_cam_ptr);
static USER_FUNC(effitemgetchange_cam);
static USER_FUNC(lect_map_cam_chg);
static void lect_map_cam_chg_sub(MapObject* mapObj, s32 camId, s32 recurse);
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

static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static EffectEntry* eff;

// Pooled scalar constants live in the REL's generated rodata objects, not in this unit.
extern const f64 double_to_int_muj_00014350;

// clang-format off
extend_data(116,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)
extend_data(117,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

#include "rel/common/evt_lect/lect_mario_ido.h"
#include "rel/common/evt_lect/lect_status.h"
#include "rel/common/x_flag.h"
#include "rel/common/evt_lect/lecture_button_disp.h"

static USER_FUNC(lecture_button_rtn) {
    s32 i;
    LectureButton* btn;
    s32 done;

    if (isFirstCall) {
        memset(lbtn, 0, sizeof(lbtn));
        for (i = 0; i < 2; i++) {
            lbtn[i].position = (Vec){200.0f, -25.0f, 0.0f};
        }
    }
    done = 0;
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
                done++;
                break;
        }
    }
    if (done >= 2) {
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, 0.0f, 1, lecture_button_disp, NULL);
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
    s32 value = evtGetValue(event, args[2]);
    LectureButton* btn = &lbtn[idx];

    if (value != 0) {
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
    LectureButton* btn = lbtn + idx;

    return (btn->seq - seq) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}
static USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    LectureButton* btn = lbtn + index;
    Vec pos = {0.0f, 0.0f, 0.0f};

    pos.x = x;
    pos.y = y;
    pos.z = z;
    btn->position = pos;
    return EVT_RETURN_DONE;
}
static void lect_map_cam_chg_sub(MapObject* mapObj, s32 camId, s32 recurse) {
    mapObj->cameraId = camId;
    if (mapObj->child != NULL) {
        lect_map_cam_chg_sub(mapObj->child, camId, 1);
    }
    if (recurse) {
        if (mapObj->next != NULL) {
            lect_map_cam_chg_sub(mapObj->next, camId, 1);
        }
    }
}
static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 cam = evtGetValue(event, args[1]);
    MapObject* obj = mapGetMapObj((const char*)name);

    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, cam, 0);
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 cam = evtGetValue(event, args[1]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    ((effItemGetData*)eff->userdata)->camId = cam;
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, args[0]);

    ((effItemGetData*)eff->userdata)->camId = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemget_open) {
    EffectEntry* eff = effNameToPtr((const char*)evtGetValue(event, event->args[0]));

    ((effItemGetData*)eff->userdata)->scaleMode = 1;
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemget_open_ptr) {
    EffectEntry* eff = (EffectEntry*)evtGetValue(event, event->args[0]);

    ((effItemGetData*)eff->userdata)->scaleMode = 1;
    return EVT_RETURN_DONE;
}
static USER_FUNC(effitemget_busaiku) {
    s32 flag = evtGetValue(event, event->args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec pos;

    if (flag != 0) {
        pos = (Vec){0.0f, 2016.0f, 0.0f};
    } else {
        pos = (Vec){0.0f, -2016.0f, 0.0f};
    }
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    PSVECAdd(&cam->cameraPos, &pos, &cam->storedPos);
    PSVECAdd(&cam->cameraAt, &pos, &cam->storedAt);
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}
EVT_BEGIN(sanders_party_lecture_init)
    CALL(evt_mario_set_pos, 90, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 40)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(90.0f), FLOAT(510.0f), FLOAT(-20.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(lect_party_ido_sum, 0, 1000)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_set_dir, 0, 90, 300)
    RETURN
EVT_END()
EVT_BEGIN(sanders_party_lecture)
    CALL(evt_msg_print, 0, STRING("lecture_friends_15_0_1"), 0, 0)
    CALL(evt_msg_print, 0, STRING("lecture_friends_15_1"), 0, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(90.0f), FLOAT(510.0f), FLOAT(50.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 11)
    WAIT_MS(1000)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_party_thunders_use, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_party_stop, 0)
    WAIT_FRAMES(16)
    CALL(evt_party_set_dir, 0, 90, 300)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(90.0f), FLOAT(510.0f), FLOAT(-20.0f), FLOAT(30.0f), FLOAT(-100.0f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_16"))
    CALL(evt_party_set_pose, 0, STRING("O_1"))
    WAIT_MS(1000)
    CALL(evt_party_set_pose, 0, STRING("S_1"))
    RETURN
EVT_END()
static EVT_BEGIN(all_party_lecture_otosub)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GET_PARTY1"))
    WAIT_MS(2000)
    RETURN
EVT_END()
EVT_BEGIN(all_party_lecture_20_data_3C908)
    CALL(evt_mario_normalize)
    IF_INT_NOT_EQUAL(LW(9), 1)
        IF_INT_NOT_EQUAL(LW(9), 5)
            CALL(evt_mario_goodbye_party, 0)
        ELSE
            CALL(evt_mario_kill_party, 0)
        END_IF
        WAIT_MS(500)
        CALL(evt_npc_get_position, LW(8), LW(0), LW(1), LW(2))
        CALL(evt_npc_get_dir, LW(8), LW(3))
        CALL(evt_npc_set_position, LW(8), 0, -1000, 0)
        CALL(evt_pouch_party_join, LW(9))
        CALL(evt_mario_set_party_pos, 0, LW(9), LW(0), LW(1), LW(2))
        CALL(set_dir, LW(3))
    ELSE
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_party_set_camid, 0, 7)
    SPAWN_EVENT_GET_ID(PTR(all_party_lecture_otosub), LW(11))
    CALL(lect_itemget_kesan, 0, FLOAT(600.0f), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("sub_bg"), STRING("itemget"), 1, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(effitemgetchange_cam, STRING("sub_bg"), 5)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    SWITCH(LW(9))
        CASE_EQUAL(1)
        CALL(evt_msg_print, 0, STRING("lecture_friends_00"), 0, 0)
        CASE_EQUAL(2)
        CALL(evt_msg_print, 0, STRING("lecture_friends_04"), 0, 0)
        CASE_EQUAL(5)
        CALL(evt_msg_print, 0, STRING("lecture_friends_07"), 0, 0)
        CASE_EQUAL(4)
        CALL(evt_pouch_get_yoshiname, LW(0))
        CALL(evt_msg_print_insert, 0, STRING("lecture_friends_09"), 0, 0, LW(0))
        CASE_EQUAL(6)
        CALL(evt_msg_print, 0, STRING("lecture_friends_12"), 0, 0)
        CASE_EQUAL(3)
        CALL(evt_msg_print, 0, STRING("lecture_friends_15"), 0, 0)
        CALL(evt_eff_softdelete, STRING("sunlight"))
        CASE_EQUAL(7)
        CALL(evt_msg_print, 0, STRING("lecture_friends_17"), 0, 0)
    END_SWITCH
    IS_EVENT_RUNNING(LW(11), LW(12))
    IF_INT_EQUAL(LW(12), 1)
        DELETE_EVENT(LW(11))
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_IF
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mario_set_camid, 7)
    CALL(effitemget_open, STRING("sub_bg"))
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1_1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_stop_for_event)
    WAIT_MS(1000)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_LECTURE1"))
    CALL(evt_snd_envoff, 1024)
    CALL(lect_cam_save)
    CALL(effitemget_busaiku, 1)
    WAIT_FRAMES(1)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(1))
    ADD(LW(3), 2016)
    CALL(evt_party_set_pos, 0, LW(0), LW(3), LW(2))
    CALL(effitemgetchange_cam, STRING("sub_bg"), 4)
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
    CALL(evt_eff_softdelete, STRING("sub_bg"))
    WAIT_FRAMES(1)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(lect_test_systemlevel)
    SWITCH(LW(9))
        CASE_EQUAL(3)
        CALL_CHILD(sanders_party_lecture_init)
    END_SWITCH
    LOOP(6)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        WAIT_MS(100)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    SWITCH(LW(9))
        CASE_EQUAL(3)
        CALL_CHILD(sanders_party_lecture)
    END_SWITCH
    LOOP(6)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        CALL(evt_party_dispflg_onoff, 0, 1, 1)
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_LECTUREN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        CALL(evt_party_dispflg_onoff, 0, 1, 0)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 1)
    CALL(evt_mario_init_camid)
    CALL(evt_party_init_camid, 0)
    CALL(lect_cam_load, 0)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_party_dispflg_onoff, 0, 1, 0)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PARTY_LECTURE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_seal_upper_right"))
    CALL(evt_npc_start_for_event)
    SWITCH(LW(9))
        CASE_EQUAL(3)
        CALL(evt_eff64, STRING("sunlight"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    END_SWITCH
    RETURN
EVT_END()
static USER_FUNC(lect_mobj_hit_nasi) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
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
static USER_FUNC(lect_itemget_kesan) {
    s32* args = event->args;
    s32 id = 1;
    PartyEntry* party;
    f32 dist;
    CameraEntry* cam;
    Vec pos;
    Vec dir;

    if (args[0] == 0) {
        id = partyCtrlNo;
    }
    party = partyGetPtr(id);
    dist = evtGetFloat(event, args[1]);
    cam = camGetPtr(CAMERA_3D);
    if (party == NULL) {
        evtSetFloat(event, args[2], 0.0f);
        evtSetFloat(event, args[3], 0.0f);
        evtSetFloat(event, args[4], 0.0f);
        return EVT_RETURN_DONE;
    }
    pos = party->position;
    pos.y += party->height / 2.0f;
    PSVECSubtract(&pos, &cam->cameraPos, &dir);
    PSVECNormalize(&dir, &dir);
    PSVECScale(&dir, &dir, dist);
    PSVECAdd(&cam->cameraPos, &dir, &dir);
    evtSetFloat(event, args[2], dir.x);
    evtSetFloat(event, args[3], dir.y);
    evtSetFloat(event, args[4], dir.z);
    return EVT_RETURN_DONE;
}
static USER_FUNC(lect_npc_balloon_on_off) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);

    if (evtGetValue(event, args[1]) != 0) {
        EffMarioBalloonData* data;

        eff = effMarioBalloonEntry(1, npc);
        data = (EffMarioBalloonData*)eff->userdata;
        data->cameraId = 7;
    } else {
        effDelete(eff);
    }
    return EVT_RETURN_DONE;
}
static USER_FUNC(efffukidashichange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);
    EffectEntry* eff = effNameToPtr((const char*)name);

    ((effFukidashiData*)eff->userdata)->camId = value;
    return EVT_RETURN_DONE;
}
static f32 lect_party_ido_sub(f32 a, f32 b, f32 c, f32 d) {
    f32 e = b - a;

    return (e * c + a * d) / b;
}
static USER_FUNC(lect_party_ido_sum) {
    s32* args = event->args;
    s32 id = 1;
    PartyEntry* party;
    s32 time;
    s32 elapsed;

    if (args[0] == 0) {
        id = partyCtrlNo;
    }
    party = partyGetPtr(id);
    time = evtGetValue(event, args[1]);
    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(3), party->position.x);
        evtSetFloat(event, LW(4), party->position.y);
        evtSetFloat(event, LW(5), party->position.z);
        return EVT_RETURN_BLOCK;
    }
    elapsed = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (elapsed > time) {
        elapsed = time;
    }
    party->position.x = lect_party_ido_sub(elapsed, time, evtGetFloat(event, LW(3)), evtGetFloat(event, LW(0)));
    party->position.y = lect_party_ido_sub(elapsed, time, evtGetFloat(event, LW(4)), evtGetFloat(event, LW(1)));
    party->position.z = lect_party_ido_sub(elapsed, time, evtGetFloat(event, LW(5)), evtGetFloat(event, LW(2)));
    if (elapsed >= time) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
static USER_FUNC(set_dir) {
    s32* args = event->args;
    s32 dir = evtGetValue(event, args[0]);
    PartyEntry* party = partyGetPtr(0);

    if (party != NULL) {
        f32 rot = 270 - (dir > 180 ? 270 : 90);

        party->dispDir = rot;
        party->field_110 = rot;
    }
    return EVT_RETURN_DONE;
}
static s32 bgm_start_wait(void) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
// clang-format on