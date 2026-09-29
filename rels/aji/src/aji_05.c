#include "aji_05.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "eff/n64/eff_akari_charge_n64.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
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
static s32 hikaru_off[4];

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"e_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_01", "w_bero_3", 5, 5,
     e_door_close, e_door_open},
    {NULL},
};

static EVT_BEGIN(evt_box)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("dummy"), 29, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("dummy"))
    CALL(evt_hitobj_attr_onoff, 1, 0, STRING("A_no1"), 2048)
    SET_VALUE(GSWF(4182), 1)
    WAIT_FRAMES(1)
    CALL_CHILD(hikaru_off)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_mario_set_dir, 90, 300, 0)
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

static EVT_BEGIN(hikaru_on)
    CALL(_hikaru_on)
    RETURN
EVT_END()

static EVT_BEGIN(hikaru_off)
    CALL(_hikaru_off)
    RETURN
EVT_END()

static HitDamageReturn dmgReturnPt[2] = {
    {"mod_01", 0.0f, -1000.0f, 0.0f},
    {NULL, 0.0f, 0.0f, 0.0f},
};

// This is an event script, but its terminator is END_SCRIPT then END_EVENT (0x1, 0x2);
// EVT_END() only emits END_SCRIPT, so it is kept as a raw array to preserve the bytes.
static u32 hikaru_yuka_init[48] = {
    0x0001005E, (u32)&hikaru_off, 0x0005005B, (u32)evt_hitobj_attr_onoff,
    0x00000001, 0x00000001, (u32)"A_no1", 0x00000800,
    0x0009005B, (u32)evt_mobj_itembox, (u32)"box", 0xFFFFFF01,
    0x0000000A, 0xFFFFFFF6, 0x00000002, 0x00000000,
    (u32)&evt_box, 0xF8406BD2, 0x0002005B, (u32)evt_hit_damage_return_set,
    (u32)&dmgReturnPt, 0x0000006B, 0x0001005B, (u32)&ride_chk,
    0x0000006D, 0x0000006B, 0x0001005B, (u32)&thunder_main,
    0x0000006D, 0x0005005B, (u32)evt_mapobj_flag_onoff, 0x00000001,
    0x00000000, (u32)"yukayu1", 0x00000001, 0x0005005B,
    (u32)evt_mapobj_flag_onoff, 0x00000001, 0x00000001, (u32)"yukayu2",
    0x00000001, 0x0004005B, (u32)evt_map_playanim, (u32)"denyuka",
    0x00000001, 0x00000000, 0x00000001, 0x00000002,
};

EVT_BEGIN(aji_05_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("no1"), 1)
    IF_INT_EQUAL(GSWF(4182), 0)
        CALL_CHILD(hikaru_yuka_init)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ato"), 1)
    ELSE
        CALL_CHILD(hikaru_off)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("A_no1"), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yukayu1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yukayu2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ato"), 1)
    END_IF
    CALL(evt_mobj_badgeblk, STRING("mobj00"), -75, 60, -125, 138, 0, GSWF(4212), 0)
    SPAWN_EVENT(evt_emergency)
    RETURN
EVT_END()

// clang-format on

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
        if (event->userdata[1] >= 0x19) {
            event->userdata[1] = 0;
            event->userdata[2]++;
            if (event->userdata[2] > 1) {
                return EVT_RETURN_DONE;
            }
        }
    }
    for (i = 1; i < 0x19; i++) {
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

USER_FUNC(_hikaru_on) {
    char buf[0x40];
    s32 i;

    for (i = 1; i < 0x19; i++) {
        sprintf(buf, "hikaru_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "nashi_%02d", i);
        mapGrpFlagOff(buf, 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_hikaru_off) {
    char buf[0x40];
    s32 i;

    for (i = 1; i < 0x19; i++) {
        sprintf(buf, "hikaru_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "nashi_%02d", i);
        mapGrpFlagOn(buf, 1);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(thunder_main) {
    MarioWork* mario;
    EffectEntry* effect;

    mario = marioGetPtr();
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    if (event->userdata[0] == 0) {
        if (mario->currMotionId == 0x20) {
            event->userdata[0] = 1;
            psndSFXOn_3D("SFX_STG7_ELECTRIC_SHOCK1", &mario->position);
            effect = effAkariChargeN64Entry(0, 100, mario->position.x, 30.0f + mario->position.y, mario->position.z,
                                            1.0f);
            ((EffAkariChargeData*)effect->userdata)->field_0x18 = 0x1e;
            ((EffAkariChargeData*)effect->userdata)->field_0x1C = 0xe7;
            ((EffAkariChargeData*)effect->userdata)->field_0x20 = 0xdb;
            ((EffAkariChargeData*)effect->userdata)->field_0x28 = 0;
            ((EffAkariChargeData*)effect->userdata)->field_0x2C = 0;
            ((EffAkariChargeData*)effect->userdata)->field_0x30 = 0;
        }
    } else if (mario->currMotionId != 0x20) {
        event->userdata[0] = 0;
    }
    return 0;
}

static USER_FUNC(ride_chk) {
    static s32 onoff[24];
    MarioWork* mario;
    PartyEntry* party;
    HitEntry* hit;
    char name[0x40];
    char buf[0x40];
    s32 i;

    mario = marioGetPtr();
    party = partyGetPtr(0);
    if (isFirstCall) {
        memset(onoff, 0, sizeof(onoff));
    }
    if (party != NULL && party->currentMemberId == 4 && (party->flags & 0x100)) {
        hit = mario->hitobjStandOn;
        if (hit == NULL) {
            hit = mario->hitobjJumpFrom;
        }
    } else {
        hit = *(HitEntry**)((u8*)mario + 0x1F8);
    }
    for (i = 0; i < 0x18; i++) {
        if (hit != NULL) {
            sprintf(name, "A_hikaru_%02d", i + 1);
            if (strcmp(hit->joint->name, name) == 0) {
                onoff[i] = 0x1e;
            }
        }
    }
    for (i = 0; i < 0x18; i++) {
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
    if (evtGetValue(event, GSWF(4182)) != 0) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

