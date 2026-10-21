#include "aji_12.h"
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
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "party.h"
#include "party/party_motion.h"
#include "pmario_sound.h"

#include <stdio.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
extern s32 evt_emergency[];
static USER_FUNC(thunder_main);
static USER_FUNC(mario_chk);
static USER_FUNC(hikaru_off);

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
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
    {"e_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_01", "w_bero_4", 5, 5,
     &e_door_close, &e_door_open},
    {NULL},
};

char* hit_list[50] = {
    "A_hikaru_01", "A_hikaru_02", "A_hikaru_03", "A_hikaru_04",
    "A_hikaru_05", "A_hikaru_06", "A_hikaru_07", "A_hikaru_08",
    "A_hikaru_09", "A_hikaru_10", "A_hikaru_11", "A_hikaru_12",
    "A_hikaru_13", "A_hikaru_14", "A_hikaru_15", "A_hikaru_16",
    "A_hikaru_17", "A_hikaru_18", "A_hikaru_19", "A_hikaru_20",
    "A_hikaru_21", "A_hikaru_22", "A_hikaru_23", "A_hikaru_24",
    "A_hikaru_25", "A_hikaru_26", "A_hikaru_27", "A_hikaru_08",
    "A_hikaru_28", "A_hikaru_29", "A_hikaru_30", "A_hikaru_31",
    "A_hikaru_32", "A_hikaru_12", "A_hikaru_13", "A_hikaru_33",
    "A_hikaru_15", "A_hikaru_34", "A_hikaru_35", "A_hikaru_36",
    "A_hikaru_37", "A_hikaru_38", "A_hikaru_39", "A_hikaru_40",
    "A_hikaru_41", "A_hikaru_42", "A_hikaru_41", "A_hikaru_40",
    "A_hikaru_43", "A_hikaru_44",
};

static EVT_BEGIN(evt_box)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("dummy"), 30, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("dummy"))
    SET_VALUE(GSWF(4183), 1)
    WAIT_FRAMES(2)
    CALL(hikaru_off)
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

static HitDamageReturn dmgReturnPt[2] = {
    {"mod_01", 0.0f, -1000.0f, 0.0f},
    {NULL, 0.0f, 0.0f, 0.0f},
};

static EVT_BEGIN(hikaru_yuka_init)
    CALL(evt_mobj_itembox, STRING("box"), -350, 10, -10, 2, 0, PTR(evt_box), GSWF(4179))
    CALL(evt_hit_damage_return_set, PTR(dmgReturnPt))
    BEGIN_INLINE_EVENT
        CALL(thunder_main)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(mario_chk)
    END_INLINE_EVENT
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yukayu1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yukayu2"), 1)
    CALL(evt_map_playanim, STRING("denyuka"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(aji_12_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("no1"), 1)
    IF_INT_EQUAL(GSWF(4183), 0)
        CALL_CHILD(hikaru_yuka_init)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ato"), 1)
    ELSE
        CALL(hikaru_off)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yukayu1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yukayu2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ato"), 1)
    END_IF
    CALL(evt_mobj_badgeblk, STRING("mobj00"), 170, 60, 120, 144, 0, GSWF(4214), 0)
    SPAWN_EVENT(evt_emergency)
    RETURN
EVT_END()

static USER_FUNC(hikaru_off) {
    char buf[0x100];
    s32 i;

    for (i = 1; i <= 0x2c; i++) {
        sprintf(buf, "hikaru_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "nashi_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "A_hikaru_%02d", i);
        hitGrpAttrOff(buf, 0x800);
    }
    hitGrpAttrOff("A_no1", 0x800);
    return EVT_RETURN_DONE;
}

static USER_FUNC(mario_chk) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* party;
    HitEntry* hit;
    char buf[0x100];
    s32 parsed;
    s32 i;
    s32 j;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
    }
    if (evtGetValue(NULL, GSWF(4183)) != 0) {
        return 2;
    }
    for (i = 1; i <= 0x2c; i++) {
        sprintf(buf, "hikaru_%02d", i);
        mapGrpFlagOn(buf, 1);
        sprintf(buf, "nashi_%02d", i);
        mapGrpFlagOff(buf, 1);
    }
    for (i = 0; i < 0x32; i++) {
        hitGrpAttrOn(hit_list[i], 0x800);
    }
    party = partyGetPtr(0);
    if (party != NULL && party->currentMemberId == 4 && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        hit = mario->hitobjStandOn;
        if (hit == NULL) {
            hit = mario->hitobjJumpFrom;
        }
    } else {
        hit = mario->hitobjUnder;
    }
    if (hit == NULL) {
        mapGrpFlagOff("hikaru_01", 1);
        mapGrpFlagOn("nashi_01", 1);
        hitGrpAttrOff("A_hikaru_01", 0x800);
        event->userdata[2] = 0;
    } else if (strncmp(hitGetName(hit), "A_hikaru_", strlen("A_hikaru_")) != 0) {
        mapGrpFlagOff("hikaru_01", 1);
        mapGrpFlagOn("nashi_01", 1);
        hitGrpAttrOff("A_hikaru_01", 0x800);
        event->userdata[2] = 0;
    } else if (strcmp(hitGetName(hit), "A_hikaru_01") == 0) {
        if (event->userdata[2] == 0) {
            event->userdata[2] = 1;
            event->userdata[0] = 0;
            event->userdata[1] = 0;
        }
    }
    parsed = 0;
    j = event->userdata[0];
    if (j >= 0x32) {
        j = 0x31;
    }
    sscanf(hit_list[j], "A_hikaru_%02d", &parsed);
    sprintf(buf, "hikaru_%02d", parsed);
    mapGrpFlagOff(buf, 1);
    sprintf(buf, "nashi_%02d", parsed);
    mapGrpFlagOn(buf, 1);
    sprintf(buf, "A_hikaru_%02d", parsed);
    hitGrpAttrOff(buf, 0x800);
    j++;
    if (j >= 0x32) {
        j = 0x31;
    }
    sscanf(hit_list[j], "A_hikaru_%02d", &parsed);
    sprintf(buf, "hikaru_%02d", parsed);
    mapGrpFlagOff(buf, 1);
    sprintf(buf, "nashi_%02d", parsed);
    mapGrpFlagOn(buf, 1);
    sprintf(buf, "A_hikaru_%02d", parsed);
    hitGrpAttrOff(buf, 0x800);

    event->userdata[1]++;
    if (event->userdata[1] > (event->userdata[2] == 0 ? 0x14 : 0x3c)) {
        event->userdata[1] = 0;
        event->userdata[0]++;
        if (event->userdata[0] > 0x32) {
            event->userdata[0] = 0;
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
// clang-format on
#pragma force_active off
