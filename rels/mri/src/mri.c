#include "mri_puni.h"
#include "mri.h"
#include <dolphin/mtx.h>
#include <stdlib.h>
#include "data/mapdata.h"
#include "driver/mapdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "rel/mri/mri.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(2,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(3,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(6,
     f64, _three, 3,
     f64, _half, 0.5)

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("mri_00", mri_00_init_evt);
    relSetEvtAddr("mri_01", mri_01_init_evt);
    relSetEvtAddr("mri_02", mri_02_init_evt);
    relSetEvtAddr("mri_03", mri_03_init_evt);
    relSetEvtAddr("mri_04", mri_04_init_evt);
    relSetEvtAddr("mri_05", mri_05_init_evt);
    relSetEvtAddr("mri_06", mri_06_init_evt);
    relSetEvtAddr("mri_07", mri_07_init_evt);
    relSetEvtAddr("mri_08", mri_08_init_evt);
    relSetEvtAddr("mri_09", mri_09_init_evt);
    relSetEvtAddr("mri_10", mri_10_init_evt);
    relSetEvtAddr("mri_11", mri_11_init_evt);
    relSetEvtAddr("mri_12", mri_12_init_evt);
    relSetEvtAddr("mri_13", mri_13_init_evt);
    relSetEvtAddr("mri_14", mri_14_init_evt);
    relSetEvtAddr("mri_15", mri_15_init_evt);
    relSetEvtAddr("mri_16", mri_16_init_evt);
    relSetEvtAddr("mri_17", mri_17_init_evt);
    relSetEvtAddr("mri_18", mri_18_init_evt);
    relSetEvtAddr("mri_19", mri_19_init_evt);
    relSetEvtAddr("mri_20", mri_20_init_evt);
    relSetBtlAddr("mri", btlsetup_mri_tbl, btl_mri_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

EVT_BEGIN(evt_mri_door_open_pure)
    IF_INT_EQUAL(LW(9), 0)
        SET_VALUE(LW(2), -5)
    ELSE
        SET_VALUE(LW(2), 5)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 0, LW(2), 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, 0, -60, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(2), 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_open)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_bero_mario_go)
        CALL(evt_bero_exec_onoff, 0, 268435456)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_open_pure)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_close_pure)
    IF_INT_EQUAL(LW(9), 0)
        SET_VALUE(LW(2), -5)
    ELSE
        SET_VALUE(LW(2), 5)
    END_IF
    CALL(evt_mapobj_trans, 1, LW(10), LW(2), 0, -60)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_ELEC2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, -60, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(2), 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, LW(2), 0, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, LW(10), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_close)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_close_pure)
    RETURN
EVT_END()

#pragma opt_propagation off // propagation coalesces curStep into elapsed's register; the target keeps them in separate registers
USER_FUNC(evt_mri_door_naname_func) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 startX = evtGetFloat(event, args[1]);
    f32 startZ = evtGetFloat(event, args[2]);
    f32 endX = evtGetFloat(event, args[3]);
    f32 endZ = evtGetFloat(event, args[4]);
    s32 steps = evtGetValue(event, args[5]);
    s32 mode = evtGetValue(event, args[6]);
    s32 elapsed;
    s32 curStep;
    f32 x;
    f32 z;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
    }
    elapsed = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->savedTime);
    if (elapsed < steps) {
#ifdef MUST_MATCH
        // MUST_MATCH: assigning curStep inside the argument makes mwcc materialise it in r4 and copy it to r28 at
        // the call (the target); a plain assignment on its own line schedules the copy before the branch instead.
        x = intplGetValue(mode, curStep = elapsed, steps, startX, endX);
#else
        curStep = elapsed;
        x = intplGetValue(mode, curStep, steps, startX, endX);
#endif
        z = intplGetValue(mode, curStep, steps, startZ, endZ);
        mapObjTranslate(name, x, 0.0f, z);
        return EVT_RETURN_BLOCK;
    }
    mapObjTranslate(name, endX, 0.0f, endZ);
    return EVT_RETURN_DONE;
}

#pragma opt_propagation reset

EVT_BEGIN(evt_mri_door_naname_open_pure)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(0.0f), FLOAT(0.0f), FLOAT(6.5f), FLOAT(-6.5f), 200, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(6.5f), FLOAT(-6.5f), FLOAT(-34.5f), FLOAT(-46.5f), 500, 11)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_naname_open)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_naname_open_pure)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_naname_close_pure)
    CALL(evt_mapobj_trans, 1, LW(10), FLOAT(-34.5f), FLOAT(0.0f), FLOAT(-46.5f))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_ELEC2"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(-34.5f), FLOAT(-46.5f), FLOAT(6.5f), FLOAT(-6.5f), 500, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC2_1"), LW(11), LW(12), LW(13), 0)
    END_INLINE_EVENT
    CALL(evt_mri_door_naname_func, LW(10), FLOAT(6.5f), FLOAT(-6.5f), FLOAT(0.0f), FLOAT(0.0f), 200, 11)
    RETURN
EVT_END()

EVT_BEGIN(evt_mri_door_naname_close)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_bero_mario_go)
    END_INLINE_EVENT
    CALL_CHILD(evt_mri_door_naname_close_pure)
    RETURN
EVT_END()

EVT_BEGIN(_ueki_anime)
    CALL(evt_mapobj_get_position, LW(2), LW(4), LW(5), LW(6))
    CALL(evt_snd_sfxon_3d, STRING("SFX_GRASS_SHAKE1"), LW(4), LW(5), LW(6), 0)
    CALL(evt_mapobj_trans, 1, LW(2), 2, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), -2, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), 1, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), -1, 0, 0)
    WAIT_MS(60)
    CALL(evt_mapobj_trans, 1, LW(2), 0, 0, 0)
    IF_INT_NOT_EQUAL(LW(10), 0)
        SPAWN_EVENT(LW(10))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(mri_ueki_entry)
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT3(LW(1), LW(2), LW(10))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 9, 1, LW(1), 0, PTR(_ueki_anime), 0)
        CALL(evt_run_case_evt, 13, 1, LW(1), 0, PTR(_ueki_anime), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_save)
    DEBUG_PUT_MSG(STRING("プニの状態をセーブします"))
    CALL(evt_puni_all_save)
    RETURN
EVT_END()

ItemType mri_itemsel_table[2] = {
    ITEM_PUNI_ORB, -1,
};

EVT_BEGIN(mri_punipuni_setup)
    CALL(evt_item_entry, LW(10), 64, 0, -20000, 0, 16, -1, 0)
    CALL(evt_item_flag_on, LW(10), 2)
    RETURN
EVT_END()

EVT_BEGIN(mri_punipuni_itemselect)
    SET_VALUE(LW(1), 0)
    CALL(evt_pouch_check_item, 64, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("sys_no_item_to_put_on"), 0, 0)
        RETURN
    END_IF
    CALL(evt_win_item_select, 0, PTR(mri_itemsel_table), LW(0), 0)
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    SET_VALUE(LW(1), 1)
    RETURN
EVT_END()

EVT_BEGIN(mri_punipuni_itemset)
    CALL(evt_target_flag_onoff, LW(11), 0, 3)
    CALL(evt_pouch_remove_item, 64, LW(0))
    CALL(evt_item_get_position, LW(10), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNIDAMA_SET1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("sys_stg2_punitama_on"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(mri_punipuni_itemremove)
    CALL(evt_target_flag_onoff, LW(11), 1, 1)
    CALL(evt_pouch_add_item, 64, LW(0))
    CALL(evt_item_set_position, LW(10), 0, -20000, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("sys_stg2_punitama_off"), 0, 0)
    RETURN
EVT_END()

USER_FUNC(hakari_color) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 a = evtGetValue(event, args[1]);
    s32 b = evtGetValue(event, args[2]);
    if (a <= b) {
        GXColor color;
        GXColor tmp;
        GXColor hsv = {0, 0xFF, 0xFF, 0xFF};

        hsv.r = (u8)gp->retraceCount;
        tmp = hsv;
        *(u32*)&color = HSV2RGB(&tmp);
        mapGrpSetColor(name, &color);
    } else {
        GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};

        mapGrpSetColor(name, &color);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(mri_hikari_color)
    CALL(evt_mapobj_flag_onoff, 0, 0, LW(0), 256)
    GET_INT_INDEXED(LW(1), LW(11))
    LOOP(0)
        GET_INT_INDEXED(LW(1), LW(10))
        IF_INT_LESS_OR_EQUAL(LW(10), LW(2))
            IF_INT_GREATER(LW(11), LW(2))
                CALL(evt_mapobj_get_position, LW(0), LW(4), LW(5), LW(6))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNISWITCH_RIDE1"), LW(4), LW(5), LW(6), 0)
            END_IF
        ELSE
            IF_INT_LESS_OR_EQUAL(LW(11), LW(2))
                CALL(evt_mapobj_get_position, LW(0), LW(4), LW(5), LW(6))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNISWITCH_RIDE1"), LW(4), LW(5), LW(6), 0)
            END_IF
        END_IF
        SET_VALUE(LW(11), LW(10))
        CALL(hakari_color, LW(0), LW(10), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

USER_FUNC(evt_mri_get16bitdata) {
    s32* args = event->args;
    s32 base = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);

    evtSetValue(event, args[2], *(s16*)(base + index * 2));
    return EVT_RETURN_DONE;
}

USER_FUNC(mri_npc_buruburu) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, name);
    s32 mode = evtGetValue(event, args[1]);
    f32 amplitude = evtGetFloat(event, args[2]);
    s32 period = evtGetValue(event, args[3]);
    s32 endTime = evtGetValue(event, args[4]);
    s32 elapsed;
    f32 cur;
    f32 delta;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        event->userdataf[0] = 0.0f;
        event->userdataf[1] = npc->position.x;
        event->userdataf[2] = npc->position.y;
        *(f32*)&event->field_84 = npc->position.z;
    }
    elapsed = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->savedTime);
    if ((elapsed % (period * 2)) >= period) {
        cur = amplitude;
    } else {
        cur = -amplitude;
    }
    delta = cur - event->userdataf[0];
    switch (mode) {
        case 0:
            npc->position.x += delta;
            npc->prevPosition.x = npc->position.x;
            break;
        case 1:
            npc->position.y += delta;
            npc->prevPosition.y = npc->position.y;
            break;
        case 2:
            npc->position.z += delta;
            npc->prevPosition.z = npc->position.z;
            break;
    }
    event->userdataf[0] = cur;
    if (endTime == 0) {
        return EVT_RETURN_BLOCK;
    }
    if (elapsed < endTime) {
        return EVT_RETURN_BLOCK;
    }
    {
        Vec pos = {0.0f, 0.0f, 0.0f};

        pos.x = event->userdataf[1];
        pos.y = event->userdataf[2];
        pos.z = *(f32*)&event->field_84;
        npc->prevPosition = pos;
        npc->position = npc->prevPosition;
    }
    return EVT_RETURN_YIELD;
}

EVT_BEGIN(mri_countdown)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 118)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2880), 1)
        CALL(evt_sub_countdown_start, 300000, 300000)
        SET_VALUE(GSWF(2880), 0)
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("mri_13"))
        CALL(evt_sub_countdown_set_restart_time, 300000)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("mri_06"))
        CALL(evt_sub_countdown_set_restart_time, 250000)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("mri_01"))
        CALL(evt_sub_countdown_set_restart_time, 100000)
    END_IF
    LOOP(0)
        CALL(evt_sub_countdown_get_status, LW(0))
        CALL(evt_bero_exec_get, LW(1))
        IF_INT_EQUAL(LW(0), 1)
            IF_BITS_CLEAR(LW(1), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SUSPEND_TYPES_OTHER(4)
    CALL(evt_mario_key_onoff, 0)
    WAIT_FRAMES(5)
    IF_BITS_SET(LW(1), 1)
        CALL(evt_mario_key_onoff, 1)
        RESUME_TYPES_OTHER(4)
        RETURN
    END_IF
    SET_VALUE(GSW(0), 127)
    CALL(evt_snd_bgmoff_f, 512, 500)
    CALL(evt_snd_envoff_f, 512, 500)
    CALL(evt_snd_sfxon, STRING("SFX_STG2_QUAKE1"), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 500)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 500)
    CALL(evt_cam_shake, 4, FLOAT(0.015f), FLOAT(0.0f), 500)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 10000)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(1), 150)
        CALL(evt_cam_get_pos, 4, LW(10), LW(11), LW(12))
        LOOP(0)
            CALL(evt_eff64, STRING(""), STRING("hokori_n64"), 0, LW(10), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_fade_entry, 17, 4000, 0, 0, 0)
    WAIT_MS(4000)
    CALL(evt_seq_set_seq, 5, 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(mri_waterfall_effect)
    ADD(LW(1), -10)
    LOOP(0)
        CALL(evt_eff64, STRING(""), STRING("water_damage_n64"), 15, LW(0), LW(1), LW(2), LW(3), 18, 0, 0, 0, 0, 0, 0)
        WAIT_MS(150)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_guide_hagure_check)
    CALL(evt_puni_get_lsw_index, LSW(0), 0, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_puni_get_trap_index, 1, LW(4))
    SET_VALUE(LW(1), 0)
    IF_INT_NOT_EQUAL(LW(4), 0)
        SET_VALUE(LW(1), 3)
        DEBUG_PUT_MSG(STRING("ガイド・トラップにはまっている"))
    END_IF
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(1), 1)
        DEBUG_PUT_MSG(STRING("ガイド・別のエリアにはぐれている"))
    END_IF
    CALL(evt_puni_get_mode, 1, LW(0))
    IF_INT_EQUAL(LW(0), 16)
        SET_VALUE(LW(1), 2)
        DEBUG_PUT_MSG(STRING("ガイド・同じエリアではぐれている"))
    END_IF
    DEBUG_PUT_MSG(STRING("ガイド・普通の状態"))
    RETURN
EVT_END()

EVT_BEGIN(mri_guide_goryu)
    CALL_CHILD(mri_guide_hagure_check)
    IF_INT_EQUAL(LW(1), 0)
        RETURN
    END_IF
    CALL(evt_puni_set_lsw_index, LSW(0), 0, 0, 0, 0, 0)
    CALL(evt_puni_set_trap_index, 1, 0)
    CALL(evt_puni_init_puni, 1, 30, 0, -1000, 0, 0)
    CALL(evt_puni_flag_onoff, 1, 1, 65536)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_get_dir, LW(3))
    ADD(LW(2), -10)
    IF_INT_LESS_OR_EQUAL(LW(0), 180)
        ADD(LW(0), -10)
    ELSE
        ADD(LW(0), 10)
    END_IF
    CALL(evt_puni_set_position, 1, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_walk_se)
    LOOP(0)
        CALL(evt_npc_get_position, LW(0), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_MOVE2_1"), LW(10), LW(11), LW(12), 0)
        WAIT_FRAMES(5)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_jump_se)
    CALL(evt_npc_get_position, LW(0), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    RETURN
EVT_END()

s32 toge_searchParent(PuniData* puni, s16 id) {
    s32 i;
    s32 best = 10000;
    PuniData* parent;
    u8* p;

    for (i = 110; i <= 209; i++) {
        p = (u8*)&puniGetPunidat()[i];
        if (!(*(u32*)(p + 0x4) & 0x400) || !(*(u32*)(p + 0x4) & 0x80000) || *(s16*)(p + 0x118) != id ||
            *(void**)(p + 0x1D4) != NULL || p == (u8*)puni || (s16)best <= *(s16*)(p + 0x11A)) {
            continue;
        }
        parent = (PuniData*)p;
        best = *(s16*)(p + 0x11A) + 1;
    }
    if ((s16)best == 10000) {
        return 0;
    }
    *(void**)((u8*)puni + 0x1D0) = parent;
    *(s16*)((u8*)puni + 0x11A) = best;
    *(s16*)((u8*)puni + 0x118) = *(s16*)((u8*)parent + 0x118);
    *(void**)((u8*)parent + 0x1D4) = puni;
    return 1;
}

void togeModeParent(PuniData* puni) {
    Vec diff;

    PSVECSubtract((Vec*)((u8*)*(u8**)((u8*)puni + 0x1D0) + 0x12C), (Vec*)((u8*)puni + 0x12C), &diff);
    *(f32*)((u8*)puni + 0x1A8) = PSVECMag(&diff);
    if (*(f32*)((u8*)puni + 0x1A8) >
        2.0f + (*(f32*)((u8*)puni + 0x190) + *(f32*)((u8*)*(u8**)((u8*)puni + 0x1D0) + 0x190)) + 3.0f) {
        PSVECNormalize(&diff, &diff);
        diff.y = 0.0f;
        PSVECScale(&diff, &diff, 1.0f);
        PSVECAdd((Vec*)((u8*)puni + 0x15C), &diff, (Vec*)((u8*)puni + 0x15C));
        *(f32*)((u8*)puni + 0x174) = 3.0f;
    }
}

void togeMoveParent(PuniData* puni) {
    puniMoveDecel(puni);
}

void puniModeWinjump(PuniData* puni) {
    u8* p = (u8*)puni;
    s32 elapsed;
    f32 t;
    f32 accel;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            *(OSTime*)(p + 0x1C8) = gp->renderTime;
            *(s32*)(p + 0x1E4) = 0;
            *(f32*)(p + 0x1EC) = *(f32*)(p + 0x130);
            *(f32*)(p + 0x1F0) = -72.0f;
            *(f32*)(p + 0x1F4) = -*(f32*)(p + 0x1F0);
            *(u16*)(p + 0x114) = 1;
            psndSFXOn_3D("SFX_STG2_PUNI_JUMP1", (Vec*)(p + 0x12C));
            // fallthrough
        case 1:
            elapsed = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)*(OSTime*)(p + 0x1C8));
            if (elapsed > 100 && *(s32*)(p + 0x1E4) == 0) {
                *(s32*)(p + 0x1E4) = 1;
                *(s32*)(p + 0x124) = 0;
            }
            if (elapsed < 200) {
                t = (f32)elapsed / 200.0f;
                accel = *(f32*)(p + 0x1F0) * t + *(f32*)(p + 0x1F4);
                *(f32*)(p + 0x130) = t * accel + *(f32*)(p + 0x1EC);
            } else {
                *(f32*)(p + 0x130) = *(f32*)(p + 0x1EC);
                *(u16*)(p + 0xC) = 27;
                *(u16*)(p + 0x11C) = 1;
                *(u16*)(p + 0x114) = 0;
            }
            break;
    }
}

void togeMakeRetsu(Vec* pos, s16 a, s16 b, s16 c) {
    u8* parent;
    s32 best;
    s32 j;
    u8* p;
    s32 i;
    u8* q;

    p = (u8*)&puniGetPunidat()[a];
    *(f32*)(p + 0x12C) = pos->x;
    *(f32*)(p + 0x130) = pos->y;
    *(f32*)(p + 0x134) = pos->z;
    *(void**)(p + 0x56C) = NULL;
    *(void**)(p + 0x570) = NULL;
    *(s16*)(p + 0x118) = *(s16*)(p + 0x116);
    *(s16*)(p + 0x11C) = 1;
    *(u32*)(p + 0x4) |= 0x80000;

    for (i = b; i <= c; i++) {
        p = (u8*)&puniGetPunidat()[i];
        *(void**)(p + 0x56C) = togeModeParent;
        *(void**)(p + 0x570) = togeMoveParent;
        *(f32*)(p + 0x12C) = pos->x;
        *(f32*)(p + 0x130) = pos->y;
        *(f32*)(p + 0x134) = pos->z;
        *(s16*)(p + 0x11C) = 1;
        *(u32*)(p + 0x4) |= 0x80000;
        toge_searchParent((PuniData*)p, a);
    }
}

USER_FUNC(evt_mri_toge_make_retsu) {
    s32* args = event->args;
    Vec pos;
    s16 a;
    s16 b;
    s16 c;

    pos.x = (f32)evtGetValue(event, args[0]);
    pos.y = (f32)evtGetValue(event, args[1]);
    pos.z = (f32)evtGetValue(event, args[2]);
    a = (s16)evtGetValue(event, args[3]);
    b = (s16)evtGetValue(event, args[4]);
    c = (s16)evtGetValue(event, args[5]);
    togeMakeRetsu(&pos, a, b, c);

    return EVT_RETURN_DONE;
}
USER_FUNC(evt_mri_toge_retsu_stop) {
    s32* args = event->args;
    s16 a = (s16)evtGetValue(event, args[0]);
    s16 start = (s16)evtGetValue(event, args[1]);
    s16 end = (s16)evtGetValue(event, args[2]);
    u8* p = (u8*)&puniGetPunidat()[a];
    s32 i;

    *(void**)(p + 0x56C) = NULL;
    *(void**)(p + 0x570) = NULL;
    *(s16*)(p + 0x11C) = 0;
    *(u32*)(p + 0x4) &= ~0x80000;
    for (i = start; i <= end; i++) {
        p = (u8*)&puniGetPunidat()[i];
        *(void**)(p + 0x56C) = NULL;
        *(void**)(p + 0x570) = NULL;
        *(s16*)(p + 0x11C) = 0;
        *(u32*)(p + 0x4) &= ~0x80000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mri_toge_enter_init) {
    s32 i;
    u8* p;

    for (i = 110; i <= 209; i++) {
        p = (u8*)&puniGetPunidat()[i];
        if (*(u32*)(p + 0x4) & 0x400) {
            *(f32*)(p + 0x12C) = 0.0f;
            *(f32*)(p + 0x130) = -1000.0f;
            *(f32*)(p + 0x134) = 0.0f;
            *(s16*)(p + 0xC) = 29;
            *(s16*)(p + 0x11C) = 0;
            *(u32*)(p + 0x4) &= ~0x100000;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mri_puni_win_init) {
    s32 i;
    u8* p;

    for (i = 1; i <= 102; i++) {
        p = (u8*)&puniGetPunidat()[i];
        if (*(u32*)(p + 0x4) & 0x400) {
            *(s16*)(p + 0x574) = *(s16*)(p + 0x576);
            *(f32*)(p + 0x188) = 0.0f;
            *(f32*)(p + 0x184) = 0.0f;
            *(f32*)(p + 0x148) = 0.0f;
            *(u32*)(p + 0x8) &= ~1;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mri_puni_win_attack) {
    s32* args = event->args;
    f32 a0 = (f32)evtGetValue(event, args[0]);
    f32 a1 = (f32)evtGetValue(event, args[1]);
    f32 a2 = (f32)evtGetValue(event, args[2]);
    f32 a3 = (f32)evtGetValue(event, args[3]);
    f32 a4 = (f32)evtGetValue(event, args[4]);
    f32 a5 = (f32)evtGetValue(event, args[5]);
    u8* w = (u8*)puniGetPunigp();
    s32 i;
    u8* p;

    *(s32*)(w + 0x47C) = 2;
    *(f32*)(w + 0x480) = a0;
    *(f32*)(w + 0x484) = a1;
    *(f32*)(w + 0x488) = a2;
    *(f32*)(w + 0x4B0) = 100.0f;
    *(f32*)(w + 0x48C) = a3;
    *(f32*)(w + 0x490) = a4;
    *(f32*)(w + 0x494) = a5;
    *(f32*)(w + 0x4B4) = 100.0f;

    for (i = 1; i <= 102; i++) {
        p = (u8*)&puniGetPunidat()[i];
        if (*(u32*)(p + 0x4) & 0x400) {
            PuniDokuritsu((PuniData*)p);
            *(s16*)(p + 0x11C) = 1;
            *(s16*)(p + 0x114) = 0;
            if (*(u32*)(p + 0x4) & 0x4000) {
                *(u32*)(p + 0x4) &= ~0x4000;
                *(s16*)(p + 0xC) = 29;
                *(void**)(p + 0x56C) = puniModeWinjump;
                *(void**)(p + 0x570) = puniMoveDecel;
            } else {
                *(s16*)(p + 0xC) = 27;
            }
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mri_puni_win_end) {
    s32* args = event->args;
    f32 a0 = (f32)evtGetValue(event, args[0]);
    f32 a1 = (f32)evtGetValue(event, args[1]);
    f32 a2 = (f32)evtGetValue(event, args[2]);
    f32 a3 = (f32)evtGetValue(event, args[3]);
    f32 a4 = (f32)evtGetValue(event, args[4]);
    f32 a5 = (f32)evtGetValue(event, args[5]);
    u8* w = (u8*)puniGetPunigp();
    s32 i;
    u8* p;
    f32 r1;
    f32 r2;

    *(s32*)(w + 0x43C) = 0;
    for (i = 1; i <= 102; i++) {
        p = (u8*)&puniGetPunidat()[i];
        if (*(u32*)(p + 0x4) & 0x400) {
            PuniDokuritsu((PuniData*)p);
            r1 = (f32)(rand() % 21);
            r2 = (f32)(rand() % 21);
            *(f32*)(p + 0x12C) = a0 + r1 - 10.0f;
            *(f32*)(p + 0x130) = a1;
            *(f32*)(p + 0x134) = a2 + r2 - 10.0f;
            *(f32*)(p + 0x1F8) = a3 + r1 - 10.0f;
            *(f32*)(p + 0x1FC) = a4;
            *(f32*)(p + 0x200) = a5 + r2 - 10.0f;
            *(s16*)(p + 0xC) = 25;
            *(s16*)(p + 0x114) = 0;
        }
    }
    return EVT_RETURN_DONE;
}
// clang-format on
