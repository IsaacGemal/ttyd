#include "aji_19.h"
#include "driver/hitdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "party.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(mario_chk);

// clang-format off
extend_data(88,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(89,
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

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door2_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "moo_02", "n_bero", 5, 5,
     w_door_close, w_door_open},
    {"e_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_00", "w_bero", 5, 5,
     e_door_close, e_door_open},
    {NULL},
};

EVT_BEGIN(evt_hajimete)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_MS(1000)
    CALL(evt_party_stop, 0)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("stg7_aji_00_00_01"))
    CALL(evt_party_run, 0)
    SET_VALUE(GSW(0), 360)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_telop_entry, 15)
    RETURN
EVT_END()

EVT_BEGIN(aji_19_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_map_playanim, STRING("hosi_01"), 1, 0)
    CALL(evt_map_playanim, STRING("hosi_02"), 1, 0)
    CALL(evt_map_playanim, STRING("hosi_03"), 1, 0)
    CALL(evt_map_playanim, STRING("yuka"), 1, 0)
    BEGIN_INLINE_EVENT
        CALL(mario_chk)
    END_INLINE_EVENT
    IF_INT_LESS(GSW(0), 360)
        SPAWN_EVENT(evt_hajimete)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("kanaami2"), 2)
    RETURN
EVT_END()

static s32 _hitIsGroupSub(HitEntry* group, HitEntry* hit, s32 recurse) {
    if (strcmp(group->joint->name, hit->joint->name) == 0) {
        return TRUE;
    }
    if (hit->child != NULL) {
        return _hitIsGroupSub(group, hit->child, TRUE);
    }
    if (recurse != 0 && hit->sibling != NULL) {
        return _hitIsGroupSub(group, hit->sibling, TRUE);
    }
    return FALSE;
}

static s32 hitIsGroup(HitEntry* group, HitEntry* hit) {
    return _hitIsGroupSub(group, hit, FALSE);
}

static USER_FUNC(mario_chk) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* party;

    if (mario != NULL && mario->hitobjStandOn != NULL) {
        if (hitIsGroup(mario->hitobjStandOn, hitNameToPtr("A_b_ue"))) {
            mario->riderDelta.x = 1.0f;
        } else if (hitIsGroup(mario->hitobjStandOn, hitNameToPtr("A_b_shita"))) {
            mario->riderDelta.x = -1.0f;
        }
    }

    party = partyGetPtr(marioGetPartyId());
    if (party != NULL && party->groundHit != NULL) {
        if (hitIsGroup(party->groundHit, hitNameToPtr("A_b_ue"))) {
            party->moveDelta.x = 1.0f;
        }
        if (hitIsGroup(party->groundHit, hitNameToPtr("A_b_shita"))) {
            party->moveDelta.x = -1.0f;
        }
    }

    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL && party->groundHit != NULL) {
        if (hitIsGroup(party->groundHit, hitNameToPtr("A_b_ue"))) {
            party->moveDelta.x = 1.0f;
        }
        if (hitIsGroup(party->groundHit, hitNameToPtr("A_b_shita"))) {
            party->moveDelta.x = -1.0f;
        }
    }

    return 0;
}
// clang-format on
