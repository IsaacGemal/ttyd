#include "aji_02.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "eff/n64/eff_akari_charge_n64.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "party.h"
#include "party/party_motion.h"
#include "pmario_sound.h"

#include <stdio.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(ride_chk);
static USER_FUNC(thunder_main);
static USER_FUNC(loop_onoff);

// clang-format off
extend_data(8,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(9,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"w_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_01", "e_bero_1", 5, 5,
     w_door_close, w_door_open},
    {NULL},
};

// byte list of hikaru group ids; no matching struct in include/
u32 root_0[3] = {
    0x0A090807, 0x06050403, 0x02010000,
};

// byte list of hikaru group ids; no matching struct in include/
u32 root_1[4] = {
    0x15080706, 0x14131211, 0x100F0E0D, 0x0C0B0000,
};

// byte list of hikaru group ids; no matching struct in include/
u32 root_2[4] = {
    0x1E1D1C1B, 0x1A111213, 0x14060519, 0x18171600,
};

// {u8* list, s32 count} pairs; no matching struct in include/
u32 root_data[6] = {
    (u32)&root_0, 0x0000000A, (u32)&root_1, 0x0000000E,
    (u32)&root_2, 0x0000000F,
};


static EVT_BEGIN(hikaru_on)
    CALL(hikaru_on_func)
    RETURN
EVT_END()


static EVT_BEGIN(hikaru_off)
    CALL(hikaru_off_func)
    RETURN
EVT_END()

EVT_BEGIN(hikaru_yuka)
    CALL_CHILD(hikaru_on)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 80, 172, 562, 80, 50, -10, 1000, 11)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_off, 800, 11)
    WAIT_MS(800)
    CALL_CHILD(hikaru_off)
    WAIT_MS(500)
    RETURN
EVT_END()


static EVT_BEGIN(evt_box)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("dummy"), 27, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("dummy"))
    SET_VALUE(GSW(0), 362)
    WAIT_FRAMES(1)
    CALL(evt_hitobj_attr_onoff, 1, 0, STRING("A_no01"), 2048)
    CALL(all_flag_off)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_set_dir, 270, 300, 0)
    END_INLINE_EVENT
    LOOP(5)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yukayu1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yukayu2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ato"), 1)
        WAIT_MS(150)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yukayu1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yukayu2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ato"), 1)
        WAIT_MS(150)
    END_LOOP
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static HitDamageReturn dmgReturnPt[2] = {
    {"mod_01", 0.0f, -1000.0f, 0.0f},
    {NULL, 0.0f, 0.0f, 0.0f},
};



// event script, left in the generated raw form (matches the original byte-for-byte)
static u32 hikaru_yuka_init[83] = {
    0x0001005E, (u32)&hikaru_off, 0x0005005B, (u32)evt_hitobj_attr_onoff,
    0x00000001, 0x00000001, (u32)"A_no01", 0x00000800,
    0x0009005B, (u32)evt_mobj_itembox, (u32)"box", 0x000000A0,
    0x0000000A, 0xFFFFFFF6, 0x00000002, 0x00000000,
    (u32)&evt_box, 0xF8406BD0, 0x0002005B, (u32)evt_hit_damage_return_set,
    (u32)&dmgReturnPt, 0x0005005B, (u32)evt_mapobj_flag_onoff, 0x00000001,
    0x00000000, (u32)"yukayu1", 0x00000001, 0x0005005B,
    (u32)evt_mapobj_flag_onoff, 0x00000001, 0x00000001, (u32)"yukayu2",
    0x00000001, 0x0004005B, (u32)evt_map_playanim, (u32)"denyuka",
    0x00000001, 0x00000000, 0x0003005B, (u32)evt_sub_random,
    0x0000012C, 0xFE363C80, 0x00010022, 0xFE363C80,
    0x00010026, 0x00000064, 0x00020032, 0xFD050F8A,
    0x00000000, 0x00010026, 0x000000C8, 0x00020032,
    0xFD050F8A, 0x00000001, 0x0000002A, 0x00020032,
    0xFD050F8A, 0x00000002, 0x00000031, 0x0002005B,
    (u32)evt_mario_key_onoff, 0x00000000, 0x0002005B, (u32)evt_seq_wait,
    0x00000002, 0x0002005B, (u32)evt_bero_exec_wait, 0x00010000,
    0x0001005E, (u32)&hikaru_yuka, 0x0002005B, (u32)evt_mario_key_onoff,
    0x00000001, 0x0000006B, 0x0001005B, (u32)&ride_chk,
    0x0000006D, 0x0000006B, 0x0001005B, (u32)&thunder_main,
    0x0000006D, 0x00000001, 0x00000002,
};

static EVT_BEGIN(peach_evt_stg1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mario_set_pos, -140, 10, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("no1"), 1)
    CALL_CHILD(hikaru_off)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yukayu1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yukayu2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ato"), 1)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1500)
    CALL(evt_fade_set_mapchange_type, 0, 12, 100, 11, 100)
    CALL(evt_bero_mapchange, STRING("aji_16"), STRING("peach_evt_stg1"))
    RETURN
EVT_END()

EVT_BEGIN(aji_02_init_evt)
    CALL(evt_bero_get_entername, LW(0))
    CALL(evt_snd_set_rev_mode, 1)
    IF_INT_LESS(GSW(0), 352)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
    END_IF
    IF_STR_EQUAL(LW(0), STRING("peach_evt_stg1"))
        CALL(evt_mario_kill_party, 0)
        CALL(evt_mario_set_mode, 1)
        CALL(evt_mario_set_pos, -140, 10, 0)
        CALL(evt_cam_road_reset)
        WAIT_FRAMES(1)
        CALL(evt_cam3d_evt_set, -78, 172, 560, -78, 50, -9, 0, 11)
        WAIT_FRAMES(1)
        SPAWN_EVENT(peach_evt_stg1)
        RETURN
    END_IF
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_cam_road_reset)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("no1"), 1)
    IF_INT_LESS(GSW(0), 362)
        SPAWN_EVENT(hikaru_yuka_init)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ato"), 1)
        WAIT_FRAMES(1)
    ELSE
        CALL_CHILD(hikaru_off)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yukayu1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yukayu2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ato"), 1)
    END_IF
    CALL(evt_mobj_badgeblk, STRING("mobj00"), -125, 60, -125, 149, 0, GSWF(4211), 0)
    SPAWN_EVENT(evt_emergency)
    RETURN
EVT_END()

USER_FUNC(hikaru_off_func) {
    char buf[0x40];
    s32 i;

    for (i = 1; i <= 0x1e; i++) {
        sprintf(buf, "hikaru_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "nashi_%02d", i);
        mapGrpFlagOn(buf, 1);
    }
    return 2;
}

USER_FUNC(hikaru_on_func) {
    char buf[0x40];
    s32 idx = evtGetValue(event, GW(10));
    u8* list = (u8*)root_data[idx * 2];
    s32 count = root_data[idx * 2 + 1];
    s32 i;
    s32 j;

    for (i = 1; i <= 0x1e; i++) {
        for (j = 0; j < count; j++) {
            if (i == list[j]) {
                sprintf(buf, "hikaru_%02d", i);
                mapGrpFlagOff(buf, 1);
                sprintf(buf, "nashi_%02d", i);
                mapGrpFlagOn(buf, 1);
                sprintf(buf, "A_hikaru_%02d", i);
                hitGrpAttrOff(buf, 0x800);
                break;
            }
        }
        if (j >= count) {
            sprintf(buf, "hikaru_%02d", i);
            mapGrpFlagOn(buf, 1);
            sprintf(buf, "nashi_%02d", i);
            mapGrpFlagOff(buf, 1);
            sprintf(buf, "A_hikaru_%02d", i);
            hitGrpAttrOn(buf, 0x800);
        }
    }
    return 2;
}

USER_FUNC(all_flag_off) {
    char buf[0x40];
    s32 i;

    for (i = 1; i <= 0x1e; i++) {
        sprintf(buf, "hikaru_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "nashi_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "A_hikaru_%02d", i);
        hitGrpAttrOff(buf, 0x800);
    }
    return 2;
}

static USER_FUNC(loop_onoff) {
    char buf[0x40];
    s32 i;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        event->userdata[2] = 0;
    }
    event->userdata[0]++;
    if (event->userdata[0] > 2) {
        event->userdata[0] = 0;
        event->userdata[1]++;
        if (event->userdata[1] > 0x1e) {
            event->userdata[1] = 0;
            event->userdata[2]++;
            if (event->userdata[2] > 1) {
                return 2;
            }
        }
    }
    for (i = 1; i <= 0x1e; i++) {
        if (i == event->userdata[1]) {
            sprintf(buf, "hikaru_%02d", i);
            mapGrpFlagOff(buf, 1);
            sprintf(buf, "nashi_%02d", i);
            mapGrpFlagOn(buf, 1);
        } else {
            sprintf(buf, "hikaru_%02d", i);
            mapGrpFlagOn(buf, 1);
            sprintf(buf, "nashi_%02d", i);
            mapGrpFlagOff(buf, 1);
        }
    }
    return 0;
}

static USER_FUNC(thunder_main) {
    MarioWork* mario = marioGetPtr();
    EffectEntry* effect;

    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    if (event->userdata[0] == 0) {
        if (mario->currMotionId == 0x20) {
            event->userdata[0] = 1;
            psndSFXOn_3D("SFX_STG7_ELECTRIC_SHOCK1", &mario->position);
            effect = effAkariChargeN64Entry(0, 100, mario->position.x, 30.0f + mario->position.y, mario->position.z,
                                            1.0f);
            ((EffAkariChargeWork*)effect->userdata)->reg0R = 0x1e;
            ((EffAkariChargeWork*)effect->userdata)->reg0G = 0xe7;
            ((EffAkariChargeWork*)effect->userdata)->reg0B = 0xdb;
            ((EffAkariChargeWork*)effect->userdata)->reg1R = 0;
            ((EffAkariChargeWork*)effect->userdata)->reg1G = 0;
            ((EffAkariChargeWork*)effect->userdata)->reg1B = 0;
        }
    } else if (mario->currMotionId != 0x20) {
        event->userdata[0] = 0;
    }
    return 0;
}

static USER_FUNC(ride_chk) {
    static s32 onoff[0x1e];
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(0);
    char name[0x40];
    char buf[0x40];
    s32 i;
    HitEntry* hit;

    if (isFirstCall) {
        memset(onoff, 0, sizeof(onoff));
    }
    if (party != NULL && party->currentMemberId == 4 && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        hit = mario->hitobjStandOn;
        if (hit == NULL) {
            hit = mario->hitobjJumpFrom;
        }
    } else {
        hit = mario->hitobjUnder;
    }
    for (i = 0; i < 0x1e; i++) {
        if (hit != NULL) {
            sprintf(name, "A_hikaru_%02d", i + 1);
            if (strcmp(hit->joint->name, name) == 0) {
                onoff[i] = 0x1e;
            }
        }
    }
    for (i = 0; i < 0x1e; i++) {
        if (onoff[i] > 0) {
            onoff[i]--;
        }
        if (onoff[i] != 0) {
            sprintf(buf, "hikaru_%02d", i + 1);
            mapGrpFlagOff(buf, 1);
            sprintf(buf, "nashi_%02d", i + 1);
            mapGrpFlagOn(buf, 1);
        } else {
            sprintf(buf, "hikaru_%02d", i + 1);
            mapGrpFlagOn(buf, 1);
            sprintf(buf, "nashi_%02d", i + 1);
            mapGrpFlagOff(buf, 1);
        }
    }
    if (evtGetValue(event, GSW(0)) >= 0x16a) {
        return 2;
    }
    return 0;
}

// clang-format on
