#include "las_18.h"
#include "driver/animdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 box_syutugen[];

// clang-format off
extend_data(76,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(77,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero_1", 1, 1, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "las_10", "se_bero_2", 5, 5,
     (s32)&w_door_close, (s32)&w_door_open},
    {NULL},
};

static EVT_BEGIN(box_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("key"), 45, LW(0), LW(1), LW(2), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("key"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

s32 switch_set_alpha(EventEntry* event, BOOL isFirstCall) {
    MapObjectEntry* obj;
    s32 alpha;

    alpha = evtGetValue(event, event->args[0]);
    obj = mobjNameToPtrNoAssert("switch");
    if (obj == NULL) {
        return 2;
    }
    if (alpha > 0x24) {
        obj->flags &= ~2;
    } else {
        obj->flags |= 2;
    }
    animPoseSetMaterialEvtColor(obj->poseId, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });
    animPoseSetMaterialFlagOn(obj->poseId, 0x40);
    return 2;
}

s32 switch_alpha_off(EventEntry* event, BOOL isFirstCall) {
    MapObjectEntry* obj;

    obj = mobjNameToPtrNoAssert("switch");
    if (obj == NULL) {
        return 2;
    }
    obj->flags &= ~2;
    animPoseSetMaterialEvtColor(obj->poseId, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    animPoseSetMaterialFlagOff(obj->poseId, 0x40);
    return 2;
}

s32 get_mobj_seq(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    MapObjectEntry* obj;

    obj = mobjNameToPtrNoAssert((const char*)evtGetValue(event, args[0]));
    if (obj == NULL) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], obj->unk1DC);
    }
    return 2;
}

EVT_BEGIN(vivian_check)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(15), 0)
    LOOP(0)
        CALL(get_mobj_seq, STRING("switch"), LW(0))
        IF_INT_GREATER(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_mario_get_party, LW(0))
        IF_INT_EQUAL(LW(0), 6)
            CALL(evt_party_get_status, 6, LW(0))
            IF_INT_EQUAL(LW(0), 2)
                ADD(LW(15), 4)
            ELSE
                SUBTRACT(LW(15), 4)
            END_IF
        ELSE
            SUBTRACT(LW(15), 4)
        END_IF
        IF_INT_GREATER(LW(15), 160)
            SET_VALUE(LW(15), 160)
        END_IF
        IF_INT_LESS(LW(15), 0)
            SET_VALUE(LW(15), 0)
        END_IF
        CALL(switch_set_alpha, LW(15))
        IF_INT_EQUAL(LW(15), 160)
            IF_INT_EQUAL(GSWF(4382), 0)
                SET_VALUE(GSWF(4382), 1)
                CALL(evt_snd_bgmoff, 1024)
                CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
                WAIT_MS(2000)
                CALL(evt_snd_bgmoff, 513)
                CALL(evt_snd_bgmon, 288, 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(switch_evt)
    CALL(switch_alpha_off)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(1250)
    CALL(evt_cam3d_evt_set, 0, 100, 363, 0, 30, -30, 500, 11)
    WAIT_MS(1000)
    CALL(evt_mobj_set_position, STRING("box"), 0, -30, 0)
    CALL(evt_mobj_flag_onoff, 0, STRING("box"), 16448)
    CALL(evt_mobj_hit_onoff, STRING("box"), 1)
    SET_VALUE(LW(0), STRING("box"))
    CALL_CHILD(box_syutugen)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(hint_plate)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("stg8_las_43"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(las_18_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_floatswitch_blue, 0, STRING("switch"), 75, 60, 0, PTR(switch_evt), GSWF(4352))
    CALL(evt_mobj_flag_onoff, 1, STRING("switch"), 2)
    IF_INT_EQUAL(GSWF(4352), 0)
        SPAWN_EVENT(vivian_check)
    END_IF
    CALL(evt_mobj_itembox, STRING("box"), 0, 0, 0, 0, 0, PTR(box_evt), GSWF(4369))
    IF_INT_EQUAL(GSWF(4352), 0)
        CALL(evt_mobj_flag_onoff, 1, STRING("box"), 16384)
        CALL(evt_mobj_hit_onoff, STRING("box"), 0)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("A_pure"), 0, PTR(hint_plate), 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
