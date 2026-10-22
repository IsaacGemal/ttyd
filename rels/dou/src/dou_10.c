#include "dou_10.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "motion/mot_ship.h"
#include "party/party_motion.h"
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(44,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(45,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[5] = {
    {"w_bero_1", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_09", "e_bero_1", 1, 1, 0, 0},
    {"w_bero_2", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_09", "e_bero_2", 1, 1, 0, 0},
    {"n_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_06", "s_bero", 1, 1, 0, 0},
    {"e_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_11", "w_bero", 1, 1, 0, 0},
    {NULL},
};

const char* namiyoke_mapobj[15] = {
    "S_nami1", "S_nami2", "S_nami3", "S_nami4",
    "S_nami5", "S_nami6", "S_nami7", "S_nami8",
    "S_nami9", "S_nami10", "S_nami11", "S_nami12",
    "S_nami13", "S_nami14", "S_nami15",
};

EVT_BEGIN(nami_running)
    CALL(evt_sub_entrymgr_entry, LW(9), LW(10), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(nami_running_init_func)
    LOOP(0)
        CALL(nami_running_func, LW(0), LW(1), LW(2))
        IF_INT_EQUAL(LW(15), 1)
            BREAK_LOOP
        END_IF
        CALL(namiyoke_set_mapobj, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_sub_entrymgr_setreg, LW(9), LW(10), LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(dou_namiatari_check, LW(0), LW(1), LW(2), LW(1))
        IF_INT_EQUAL(LW(1), 1)
            SET_VALUE(GW(0), 1)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(namiyoke_set_mapobj, LW(10), 0, -1000, 0)
    CALL(evt_sub_entrymgr_delete, LW(9), LW(10), LW(0))
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_fail_kaiten)
    CALL(evt_mario_get_dispdir, LW(0))
    SET_VALUE(LW(1), LW(0))
    ADD(LW(1), 1080)
    CALL(evt_sub_intpl_msec_init, 0, LW(0), LW(1), 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_dispdir, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_fail_shizumu)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_MINIGAME_SHIP_SINK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 0, 0, -100, 4000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_get_pos, 0, LW(7), LW(8), LW(9))
        CALL(evt_mario_set_pos, LW(7), LW(0), LW(9))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_fail_recover)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_MINIGAME_SHIP_FLOAT1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 1, -100, 0, 800)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_pos, -660, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -10, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_pos, -660, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -10, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_pos, -660, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -5, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_pos, -660, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -5, 0, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_pos, -660, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -1, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_pos, -660, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -1, 0, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mario_set_pos, -660, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_fail)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(namiyoke_fail_stop)
    CALL(evt_mario_dispflag_onoff, 1, 256)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 0, 11)
    SPAWN_EVENT_GET_ID(PTR(namiyoke_fail_kaiten), LW(10))
    SPAWN_EVENT_GET_ID(PTR(namiyoke_fail_shizumu), LW(11))
    WAIT_MS(2000)
    DELETE_EVENT(LW(10))
    DELETE_EVENT(LW(11))
    CALL(evt_mario_set_pos, -660, -100, 100)
    CALL(evt_cam3d_evt_set, -630, 152, 578, -630, 57, 37, 500, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_eff64, STRING(""), STRING("ripple_n64"), 0, LW(0), 5, LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(400)
        CALL(evt_eff64, STRING(""), STRING("ripple_n64"), 0, LW(0), 5, LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL_CHILD(namiyoke_fail_recover)
    CALL(evt_cam_road_reset)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_dispflag_onoff, 0, 256)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_atari_check)
    SET_VALUE(GW(0), 0)
    LOOP(0)
        IF_INT_EQUAL(GW(0), 1)
            DEBUG_PUT_MSG(STRING("マリオと波の接触"))
            CALL_CHILD(namiyoke_fail)
            SET_VALUE(GW(0), 0)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_main)
    SET_VALUE(LW(10), 0)
    LOOP(0)
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 15)
            SET_VALUE(LW(10), 0)
        END_IF
        SET_VALUE(LW(12), 0)
        CALL(evt_sub_random, 3, LW(14))
        SET_VALUE(LW(11), 162)
        MULTIPLY(LW(11), LW(10))
        ADD(LW(11), -500)
        SWITCH(LW(14))
            CASE_EQUAL(0)
            SET_VALUE(LW(13), -125)
            IF_INT_GREATER_OR_EQUAL(LW(10), 6)
                SPAWN_EVENT(nami_running)
            END_IF
            CASE_EQUAL(1)
            SET_VALUE(LW(13), -50)
            IF_INT_GREATER_OR_EQUAL(LW(10), 6)
                SPAWN_EVENT(nami_running)
            END_IF
            CASE_EQUAL(2)
            SET_VALUE(LW(13), 25)
            SPAWN_EVENT(nami_running)
            CASE_EQUAL(3)
            SET_VALUE(LW(13), 100)
            SPAWN_EVENT(nami_running)
        END_SWITCH
        IF_INT_EQUAL(LW(10), 9)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 15)
            SET_VALUE(LW(10), 0)
        END_IF
        SET_VALUE(LW(11), 1040)
        SET_VALUE(LW(12), -50)
        CALL(evt_sub_random, 3, LW(14))
        SWITCH(LW(14))
            CASE_EQUAL(0)
            SET_VALUE(LW(13), -125)
            CASE_EQUAL(1)
            SET_VALUE(LW(13), -50)
            CASE_EQUAL(2)
            SET_VALUE(LW(13), 25)
            CASE_EQUAL(3)
            SET_VALUE(LW(13), 100)
        END_SWITCH
        SPAWN_EVENT(nami_running)
        CALL(evt_sub_random, 500, LW(0))
        ADD(LW(0), 1500)
        WAIT_MS(LW(0))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_init)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(namiyoke_set_mapobj, LW(0), 0, -1000, 0)
        ADD(LW(0), 1)
        IF_INT_EQUAL(LW(0), 15)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_hitobj_onoff, STRING("A_nami1"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami2"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami3"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami4"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami5"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami6"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami7"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami8"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami9"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami10"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami11"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami12"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami13"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami14"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_nami15"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_se)
    SET_VALUE(LW(12), 0)
    LOOP(0)
        SET_VALUE(LW(10), 0)
        SET_VALUE(LW(11), 0)
        SET_VALUE(LW(6), 0)
        SET_VALUE(LW(7), 100000000)
        LOOP(0)
            CALL(evt_sub_entrymgr_check, LW(9), LW(10), LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                SET_VALUE(LW(11), 1)
                CALL(evt_sub_entrymgr_getreg, LW(9), LW(10), LW(0), LW(1), LW(2))
                CALL(namiyoke_se_check, LW(0), LW(1), LW(2))
            END_IF
            ADD(LW(10), 1)
            IF_INT_EQUAL(LW(10), 15)
                BREAK_LOOP
            END_IF
        END_LOOP
        IF_INT_EQUAL(LW(11), 1)
            CALL(evt_sub_entrymgr_getreg, LW(9), LW(6), LW(3), LW(4), LW(5))
            IF_INT_EQUAL(LW(12), 0)
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_MINIGAME_WAVE1"), LW(3), LW(4), LW(3), LW(13))
                SET_VALUE(LW(12), 1)
            ELSE
                CALL(evt_snd_sfx_pos, LW(13), LW(3), LW(4), LW(5))
            END_IF
        ELSE
            IF_INT_EQUAL(LW(12), 1)
                CALL(evt_snd_sfxoff, LW(13))
                SET_VALUE(LW(12), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(namiyoke_check)
    IF_INT_EQUAL(GSW(0), 248)
        SET_VALUE(GSW(0), 249)
    END_IF
    CALL(evt_sub_entrymgr_init, LW(9), 15)
    SPAWN_EVENT(namiyoke_init)
    SPAWN_EVENT(namiyoke_main)
    SPAWN_EVENT(namiyoke_se)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("w_bero_1"))
        SPAWN_EVENT(namiyoke_atari_check)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("e_bero"))
        SPAWN_EVENT(namiyoke_atari_check)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(yari_trap_sub)
    IF_INT_NOT_EQUAL(GW(15), 0)
        RETURN
    END_IF
    SET_VALUE(GW(15), 1)
    LOOP(0)
        CALL(evt_mapobj_get_position, STRING("S_yari"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_FLOOR_TRAP1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 1, 0, 150, 150)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("S_yari"), 0, 0, LW(0))
            CALL(evt_hit_bind_update, STRING("A_yari"))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(1000)
        CALL(evt_mapobj_get_position, STRING("S_yari"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_FLOOR_TRAP2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 4, 150, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_trans, 1, STRING("S_yari"), 0, 0, LW(0))
            CALL(evt_hit_bind_update, STRING("A_yari"))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(1000)
        IF_INT_EQUAL(GW(15), 2)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GW(15), 0)
    RETURN
EVT_END()

static EVT_BEGIN(yari_trap)
    CALL(evt_hit_bind_mapobj, STRING("A_yari"), STRING("S_yari"))
    SET_VALUE(GW(15), 0)
    SPAWN_EVENT(yari_trap_sub)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main0)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

static HitDamageReturn hitDmgRetPoints[4] = {
    {"mod_01", {-700.0f, 50.0f, -90.0f}},
    {"mod_03", {-130.0f, 50.0f, -90.0f}},
    {"mod_04", {0.0f, -1000.0f, 0.0f}},
    {NULL, {0.0f, 0.0f, 0.0f}},
};

EVT_BEGIN(dou_10_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU10"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    WAIT_FRAMES(1)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("moku"), 1, 0)
    CALL(evt_map_playanim, STRING("fune_02"), 1, 0)
    SPAWN_EVENT(yari_trap)
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    SPAWN_EVENT(namiyoke_check)
    SET_VALUE(LW(0), STRING("s10_fune_01"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main0), LW(10))
    CALL(evt_mobj_powerupblk, STRING("pblk00"), 130, 50, -135, 0, GSWF(5558), 0)
    RETURN
EVT_END()
// clang-format on

USER_FUNC(evt_sub_entrymgr_init) {
    s32* args = event->args;
    NamiyokeEntryMgr* mgr = _mapAlloc(8);
    evtSetValue(event, args[0], (s32)mgr);
    mgr->count = evtGetValue(event, args[1]);
    mgr->entries = _mapAlloc(mgr->count * 0x14);
    memset(mgr->entries, 0, mgr->count * 0x14);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_entrymgr_entry) {
    s32* args = event->args;
    NamiyokeEntryMgr* mgr = (NamiyokeEntryMgr*)evtGetValue(event, args[0]);
    NamiyokeEntry* entry = mgr->entries;
    s32 id = evtGetValue(event, args[1]);
    s32 found = FALSE;
    s32 i;

    for (i = 0; i < mgr->count; i++) {
        if (entry->flag == 0) {
            entry->flag = 1;
            entry->id = id;
            found = TRUE;
            break;
        }
        entry++;
    }
    evtSetValue(event, args[2], found);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_entrymgr_delete) {
    s32* args = event->args;
    NamiyokeEntryMgr* mgr = (NamiyokeEntryMgr*)evtGetValue(event, args[0]);
    NamiyokeEntry* entry = mgr->entries;
    s32 id = evtGetValue(event, args[1]);
    s32 found = FALSE;
    s32 i;

    for (i = 0; i < mgr->count; i++) {
        if (entry->id == id) {
            entry->flag = 0;
            entry->id = 0;
            found = TRUE;
            break;
        }
        entry++;
    }
    evtSetValue(event, args[2], found);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_entrymgr_check) {
    s32* args = event->args;
    NamiyokeEntryMgr* mgr = (NamiyokeEntryMgr*)evtGetValue(event, args[0]);
    NamiyokeEntry* entry = mgr->entries;
    s32 id = evtGetValue(event, args[1]);
    s32 found = FALSE;
    s32 i;

    for (i = 0; i < mgr->count; i++) {
        if (entry->flag == 0) {
            continue;
        }
        if (entry->id == id) {
            found = TRUE;
            break;
        }
        entry++;
    }
    evtSetValue(event, args[2], found);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_entrymgr_setreg) {
    s32* args = event->args;
    NamiyokeEntryMgr* mgr = (NamiyokeEntryMgr*)evtGetValue(event, args[0]);
    NamiyokeEntry* entry = mgr->entries;
    s32 id = evtGetValue(event, args[1]);
    s32 i;

    for (i = 0; i < mgr->count; i++) {
        if (entry->id == id) {
            break;
        }
        entry++;
    }
    entry->x = evtGetFloat(event, args[2]);
    entry->y = evtGetFloat(event, args[3]);
    entry->z = evtGetFloat(event, args[4]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_entrymgr_getreg) {
    s32* args = event->args;
    NamiyokeEntryMgr* mgr = (NamiyokeEntryMgr*)evtGetValue(event, args[0]);
    NamiyokeEntry* entry = mgr->entries;
    s32 id = evtGetValue(event, args[1]);
    s32 i;

    for (i = 0; i < mgr->count; i++) {
        if (entry->id == id) {
            break;
        }
        entry++;
    }
    evtSetFloat(event, args[2], entry->x);
    evtSetFloat(event, args[3], entry->y);
    evtSetFloat(event, args[4], entry->z);
    return EVT_RETURN_DONE;
}

USER_FUNC(namiyoke_set_mapobj) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);

    mapObjTranslate(namiyoke_mapobj[index], x, y, z);
    return EVT_RETURN_DONE;
}

USER_FUNC(nami_running_init_func) {
    f32* work = (f32*)&event->lwData[6];

    work[0] = (f32)event->lwData[11];
    work[1] = (f32)event->lwData[12];
    work[2] = (f32)event->lwData[13];
    return EVT_RETURN_DONE;
}

USER_FUNC(nami_running_func) {
    s32* args = event->args;
    f32 s;
    f32* work = (f32*)&event->lwData[6];

    work[0] = work[0] - 1.3499999f;
    if (work[0] > 840.0f) {
        work[1] += 0.35f;
        if (work[1] > 0.0f) {
            work[1] = 0.0f;
        }
    }
    switch (event->lwData[14]) {
    case 0:
    case 1:
        if (work[0] < 200.0f) {
            work[1] = work[1] - 0.4f;
        }
        break;
    case 2:
    case 3:
        if (work[0] < -340.0f) {
            work[1] = work[1] - 0.4f;
        }
        break;
    }
    event->lwData[15] = 0;
    switch (event->lwData[14]) {
    case 0:
    case 1:
        if (work[0] < 0.0f) {
            event->lwData[15] = 1;
        }
        break;
    case 2:
    case 3:
        if (work[0] < -540.0f) {
            event->lwData[15] = 1;
        }
        break;
    }
    s = sin((2.0f * (f32)gp->retraceCount) * 6.2831855f / 360.0f);
    evtSetFloat(event, args[0], work[0]);
    evtSetFloat(event, args[1], work[1] + (5.0f * s - 5.0f));
    evtSetFloat(event, args[2], work[2]);
    return EVT_RETURN_DONE;
}

USER_FUNC(dou_namiatari_check) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    f32 rangeX = 30.0f;
    f32 rangeZ = 30.0f;
    f32 mx = mario->position.x;
    f32 mz = mario->position.z;
    f32 padX = 20.0f;
    f32 padZ = 10.0f;
    s32 found;

    if (y <= -45.0f) {
        evtSetValue(event, args[3], 0);
        return EVT_RETURN_DONE;
    }
    if (y <= -5.0f) {
        f32 ratio = (50.0f + y) / 45.0f;
        rangeX *= ratio;
        rangeZ *= ratio;
    }
    found = TRUE;
    if (__fabsf(x - mx) > __fabsf(rangeX + padX)) {
        found = FALSE;
    }
    if (__fabsf(z - mz) > __fabsf(rangeZ + padZ)) {
        found = FALSE;
    }
    evtSetValue(event, args[3], found);
    return EVT_RETURN_DONE;
}

USER_FUNC(namiyoke_fail_stop) {
    marioShipForceStop();
    return EVT_RETURN_DONE;
}

USER_FUNC(namiyoke_se_check) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    Vec pos;
    s32 dist;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    dist = PSVECSquareDistance(&pos, &mario->position);
    if (dist < event->lwData[7]) {
        event->lwData[6] = event->lwData[10];
        event->lwData[7] = dist;
    }
    return EVT_RETURN_DONE;
}
