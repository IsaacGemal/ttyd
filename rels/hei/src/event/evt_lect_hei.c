#include "evt_lect_hei.h"
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
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#include <dolphin/os.h>
#include <string.h>

#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "eff/eff_mario_balloon.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"
#include "pmario_sound.h"
#include "statuswindow.h"

#pragma pool_data off

#pragma force_active on
static s32 bgm_start_wait(void);
static USER_FUNC(set_dir);
static USER_FUNC(lect_party_ido_sum);
static f32 lect_party_ido_sub(f32, f32, f32, f32);
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
static void lect_map_cam_chg_sub(MapObject* obj, s32 camId, BOOL recurse);
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
static f32 lect_mario_ido_sub(f32, f32, f32, f32);

void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);


typedef struct LectureButton {
    u8 flags;                  // 0x0
    u8 seq;                    // 0x1
    u8 pad_2[0x4 - 0x2];       // 0x2
    u8 pad_4[0xC - 0x4];       // 0x4
    Vec position;              // 0xC
    u8 pad_18[0x40 - 0x18];    // 0x18
    s16 iconId;                // 0x40
    s16 alpha;                 // 0x42
} LectureButton;

static OSTime wwstart;
static s32 x_flag_data[2];
static LectureButton lbtn[2];
static EffectEntry* eff;

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

f32 lect_mario_ido_sub(f32 cur, f32 total, f32 end, f32 start) {
    f32 t = total - cur;

    return (t * end + cur * start) / total;
}

USER_FUNC(lect_mario_ido_sum) {
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

USER_FUNC(lect_status_off) {
    statusWinForceClose();
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_status_on) {
    statusWinForceCloseClear();
    return EVT_RETURN_DONE;
}

USER_FUNC(x_flag_set) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[index] = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(x_flag_add) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[index] += value;
    return EVT_RETURN_DONE;
}

USER_FUNC(x_flag_get) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], x_flag_data[index]);
    return EVT_RETURN_DONE;
}

USER_FUNC(x_flag_wait) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    return (x_flag_data[index] == value) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// NONMATCHING (95.84%): the iconId/alpha test's branch layout differs by 18 bytes.
// Target: `cmpwi 0x75; bge neg15; b neg15` (branch-to-then); MWCC 2.6 emits
// `cmpwi 0x75; blt zero` plus a hoisted `addi r5, r9, 0x40` CSE of the iconId
// address. Every source form tried (flat &&, not-and, nested if, ge/lt, casts,
// struct vs raw access, all -O4 flag orders, GC/1.3 and GC/2.6) yields one of
// those two shapes; none reproduces both the size (0xd0) and the branch.
// NONMATCHING (95.84%): 4 of 53 instructions differ: 1 with different operands, 1 missing, 1 extra, 1 with a different opcode
void lecture_button_disp(CameraId cameraId, void* param) {
    s32 i;

    for (i = 0; i < 2; i++) {
        Vec* ppos = (Vec*)((u8*)&lbtn[i] + 8);
        f32 yoff;
        Vec pos;

        if (lbtn[i].flags & 1) {
            continue;
        }
        if (*(s16*)((u8*)&lbtn[i] + 0x40) < 0x7E && *(s16*)((u8*)&lbtn[i] + 0x40) >= 0x75) {
            yoff = -15.0f;
        } else {
            yoff = 0.0f;
        }
        pos = *(Vec*)((u8*)ppos + 4);
        pos.y += yoff;
        iconDispGxAlpha(&pos, 0x50, (u16)*(s16*)((u8*)&lbtn[i] + 0x40), (u8)*(s16*)((u8*)&lbtn[i] + 0x42), 1.0f);
    }
}

USER_FUNC(lecture_button_rtn) {
    s32 i;
    LectureButton* button;
    s32 done;

    if (isFirstCall) {
        memset(lbtn, 0, sizeof(lbtn));
        for (i = 0; i < 2; i++) {
            lbtn[i].position = (Vec){200.0f, -25.0f, 0.0f};
        }
    }
    done = 0;
    for (i = 0; i < 2; i++) {
        button = &lbtn[i];

        switch (button->seq) {
            case 0:
                button->iconId = 0x68;
                button->seq = 1;
                break;
            case 2:
                button->alpha += 7;
                if (button->alpha >= 0xFF) {
                    button->alpha = 0xFF;
                    button->seq = 1;
                }
                break;
            case 3:
                button->alpha -= 7;
                if (button->alpha <= 0) {
                    button->alpha = 0;
                    button->seq = 1;
                }
                break;
            case 4:
                button->flags |= 1;
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

USER_FUNC(lecture_button_set_icon_id) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    *(s16*)((u8*)lbtn + index * 0x44 + 0x40) = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(lecture_button_set_alpha) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    *(s16*)((u8*)lbtn + index * 0x44 + 0x42) = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(lecture_button_flag_on_off) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    u8 mask = evtGetValue(event, args[1]);

    {
        BOOL on = evtGetValue(event, args[2]);
        u8* base = (u8*)lbtn;
        s32 offset = index * 0x44;

        if (on) {
            base[offset] |= mask;
        } else {
            base[offset] &= ~mask;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(lecture_button_set_seq) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    *(u8*)((u8*)lbtn + index * 0x44 + 1) = value;
    return EVT_RETURN_DONE;
}

USER_FUNC(lecture_button_get_icon_id) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], *(u8*)((u8*)lbtn + index * 0x44 + 1));
    return EVT_RETURN_DONE;
}

USER_FUNC(lecture_button_seq_wait) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    unsigned char field = *(u8*)((u8*)lbtn + index * 0x44 + 1);

    return (field != value) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(lecture_button_set_pos) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);

    {
        Vec pos = (Vec){x, y, z};
        *(Vec*)((u8*)lbtn + index * 0x44 + 0xC) = pos;
    }
    return EVT_RETURN_DONE;
}

void lect_map_cam_chg_sub(MapObject* obj, s32 camId, BOOL recurse) {
    *(u8*)((u8*)obj + 0x5) = camId;
    if (obj->child != NULL) {
        lect_map_cam_chg_sub(obj->child, camId, TRUE);
    }
    if (recurse && obj->next != NULL) {
        lect_map_cam_chg_sub(obj->next, camId, TRUE);
    }
}

USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    MapObject* obj = mapGetMapObj((const char*)name);

    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, camId, FALSE);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(effitemgetchange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    EffectEntry* effect = effNameToPtr((const char*)name);

    *(s32*)((u8*)effect->userdata + 0x38) = camId;
    return EVT_RETURN_DONE;
}

USER_FUNC(effitemgetchange_cam_ptr) {
    s32* args = event->args;
    EffectEntry* effect = (EffectEntry*)evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);

    *(s32*)((u8*)effect->userdata + 0x38) = camId;
    return EVT_RETURN_DONE;
}

USER_FUNC(effitemget_open) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    EffectEntry* effect = effNameToPtr((const char*)name);

    *(s32*)((u8*)effect->userdata + 0x20) = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(effitemget_open_ptr) {
    s32* args = event->args;
    EffectEntry* effect = (EffectEntry*)evtGetValue(event, args[0]);

    *(s32*)((u8*)effect->userdata + 0x20) = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(effitemget_busaiku) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec offset;

    if (flag) {
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

USER_FUNC(lect_mobj_hit_nasi) {
    s32* args = event->args;
    MapObjectEntry* obj;
    const char* hit;
    s32 name;
    s32 i;

    name = evtGetValue(event, args[0]);
    if (strcmp((const char*)name, "me") != 0) {
        obj = mobjNameToPtr((const char*)name);
    } else {
        obj = event->thisMapObj;
    }
    for (i = 0; i < 2; i++) {
        hit = obj->hitObj[i].name;
        if (hit != NULL && strcmp(hit, "") != 0) {
            hitGrpFlagOn(hit, 1);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_itemget_kesan) {
    s32* args = event->args;
    s32 index = args[0] == 0 ? partyCtrlNo : 1;
    PartyEntry* party = partyGetPtr(index);
    f32 dist = evtGetFloat(event, args[1]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec pos;
    Vec dir;
    f32 v;
    f32 half;

    if (party == NULL) {
        evtSetFloat(event, args[2], 0.0f);
        evtSetFloat(event, args[3], 0.0f);
        evtSetFloat(event, args[4], 0.0f);
        return EVT_RETURN_DONE;
    }
    pos = party->position;
    half = 0.5f;
    v = *(f32*)((u8*)party + 0xF0);
    pos.y += v * half;
    PSVECSubtract(&pos, &cam->cameraPos, &dir);
    PSVECNormalize(&dir, &dir);
    PSVECScale(&dir, &dir, dist);
    PSVECAdd(&cam->cameraPos, &dir, &dir);
    evtSetFloat(event, args[2], dir.x);
    evtSetFloat(event, args[3], dir.y);
    evtSetFloat(event, args[4], dir.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(lect_npc_balloon_on_off) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);

    if (evtGetValue(event, args[1])) {
        eff = effMarioBalloonEntry(1, npc);
        *(s32*)((u8*)eff->userdata + 0x1C) = 7;
    } else {
        effDelete(eff);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(efffukidashichange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    EffectEntry* effect = effNameToPtr((const char*)name);

    *(s32*)((u8*)effect->userdata + 0x4C) = camId;
    return EVT_RETURN_DONE;
}

f32 lect_party_ido_sub(f32 cur, f32 total, f32 end, f32 start) {
    f32 t = total - cur;

    return (t * end + cur * start) / total;
}

USER_FUNC(lect_party_ido_sum) {
    s32* args = event->args;
    s32 index = args[0] == 0 ? partyCtrlNo : 1;
    PartyEntry* party = partyGetPtr(index);
    s32 total = evtGetValue(event, args[1]);
    s32 progress;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(3), party->position.x);
        evtSetFloat(event, LW(4), party->position.y);
        evtSetFloat(event, LW(5), party->position.z);
        return EVT_RETURN_BLOCK;
    }
    progress = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (progress > total) {
        progress = total;
    }
    {
        f32 target = evtGetFloat(event, LW(0));
        f32 start = evtGetFloat(event, LW(3));
        f32 t = (f32)total - (f32)progress;
        party->position.x = (t * start + (f32)progress * target) / (f32)total;
    }
    {
        f32 target = evtGetFloat(event, LW(1));
        f32 start = evtGetFloat(event, LW(4));
        f32 t = (f32)total - (f32)progress;
        party->position.y = (t * start + (f32)progress * target) / (f32)total;
    }
    {
        f32 target = evtGetFloat(event, LW(2));
        f32 start = evtGetFloat(event, LW(5));
        f32 t = (f32)total - (f32)progress;
        party->position.z = (t * start + (f32)progress * target) / (f32)total;
    }
    if (progress >= total) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(set_dir) {
    s32* args = event->args;
    s32 dir = evtGetValue(event, args[0]);
    PartyEntry* party = partyGetPtr(0);

    if (party != NULL) {
        s32 angle = (dir > 180) ? 270 : 90;
        f32 value = (f32)(270 - angle);

        party->field_10C = value;
        party->field_110 = value;
    }
    return EVT_RETURN_DONE;
}

s32 bgm_start_wait(void) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

EVT_BEGIN(nokotaro_party_lecture_init)
    CALL(evt_mario_set_pos, 350, 0, 0)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 40)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(90.0f), FLOAT(520.0f), FLOAT(-20.0f), FLOAT(26.0f), FLOAT(-140.0f), 1000, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(lect_party_ido_sum, 0, 1000)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_set_dir, 0, 90, 300)
    RETURN
EVT_END()

EVT_BEGIN(nokotaro_party_lecture)
    CALL(evt_msg_print, 0, STRING("lecture_friends_04_0_1"), 0, 0)
    CALL(evt_msg_print, 0, STRING("lecture_friends_04_1"), 0, 0)
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(100.0f), FLOAT(630.0f), FLOAT(45.0f), FLOAT(39.0f), FLOAT(-100.0f), 1000, 0)
    WAIT_MS(1500)
    CALL(evt_cam_letter_box_disable, 1)
    LOOP(2)
        CALL(evt_party_nokotaro_kick, 0)
        WAIT_MS(500)
    END_LOOP
    CALL(evt_npc_change_fbat_mode, 8)
    SET_VALUE(LW(8), 2)
    LOOP(LW(8))
        SWITCH(LW(8))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("lecture_friends_05"))
            CASE_EQUAL(1)
            CALL(evt_msg_print_add, 0, STRING("lecture_friends_05_2"))
        END_SWITCH
        SET_VALUE(LW(1), STRING("TESTMOBJ"))
        CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
        ADD(LW(2), -70)
        ADD(LW(3), 100)
        SET_VALUE(GSWF(1803), 0)
        CALL(evt_mobj_switch_blue, 0, LW(1), LW(2), LW(3), LW(4), 0, GSWF(1803))
        CALL(evt_mobj_set_camid, LW(1), 7)
        CALL(evt_snd_sfxon, STRING("SFX_EVT_LECTURE_BLOCK_FALL1"), LW(7))
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(evt_mobj_get_position, LW(1), LW(2), LW(3), LW(4))
                IF_INT_LESS_OR_EQUAL(LW(3), 10)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_snd_sfxon, STRING("SFX_EVT_LECTURE_BLOCK_DOWN1"), 0)
            CALL(evt_snd_sfxoff, LW(7))
        END_INLINE_EVENT
        SET_VALUE(LW(0), FLOAT(0.0f))
        LOOP(0)
            CALL(evt_mobj_get_position, LW(1), LW(2), LW(3), LW(4))
            ADD_FLOAT(LW(0), FLOAT(5.0f))
            SUBTRACT_FLOAT(LW(3), LW(0))
            IF_INT_LESS_OR_EQUAL(LW(3), FLOAT(0.0f))
                SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
                CALL(evt_mobj_set_position, LW(1), LW(2), LW(3), LW(4))
                BREAK_LOOP
            ELSE
                CALL(evt_mobj_set_position, LW(1), LW(2), LW(3), LW(4))
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(300)
        WAIT_MS(1000)
        CALL(evt_party_nokotaro_kick_hold, 0)
        SWITCH(LW(8))
            CASE_EQUAL(2)
            CALL(evt_msg_print_add, 0, STRING("lecture_friends_05_1"))
            CASE_EQUAL(1)
            CALL(evt_mario_mov_pos2, 367, -38, FLOAT(135.0f))
            CALL(evt_mario_mov_pos2, 416, -38, FLOAT(135.0f))
            CALL(evt_mario_set_dir, 270, 300, 0)
        END_SWITCH
        CALL(evt_party_nokotaro_hold_cancel, 0)
    END_LOOP
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(x_flag_set, 0, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        WAIT_FRAMES(16)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 40)
        CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 90, 300)
        CALL(x_flag_set, 0, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_05_3"))
    CALL(evt_cam3d_evt_set_rel, FLOAT(0.0f), FLOAT(90.0f), FLOAT(520.0f), FLOAT(-20.0f), FLOAT(26.0f), FLOAT(-140.0f), 1000, 0)
    WAIT_MS(1000)
    CALL(evt_msg_print_add, 0, STRING("lecture_friends_06"))
    CALL(x_flag_wait, 0, 0)
    WAIT_MS(500)
    CALL(evt_party_set_pose, 0, STRING("PNK_O_1"))
    WAIT_MS(1000)
    CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(all_party_lecture_otosub)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GET_PARTY1"))
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(all_party_lecture_12_data_1CF80)
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
        CASE_EQUAL(2)
        CALL_CHILD(nokotaro_party_lecture_init)
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
        CASE_EQUAL(2)
        CALL_CHILD(nokotaro_party_lecture)
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
    END_SWITCH
    RETURN
EVT_END()
// clang-format on
