#include "dou_05.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "eff/eff_mahojin.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "cam_road.h"
#include "driver/hitdrv.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include <dolphin/gx.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(alpha_set_func);
static USER_FUNC(alpha_init_func);
static void alpha_tev(TevCallbackParam* param);
static u8* _ap;

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static void alpha_tev(TevCallbackParam* param) {
    s32 stage = param->field_C;
    u8* ap = _ap;

    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColor(GX_TEVREG0, (GXColor){0, 0, 0, ap[0]});
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->field_C = param->field_C + 1;
}

static USER_FUNC(alpha_init_func) {
    _ap = _mapAlloc(1);
    _ap[0] = 0xFF;
    return EVT_RETURN_DONE;
}

static USER_FUNC(alpha_set_func) {
    _ap[0] = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

static BeroEntry bero_entry_data[5] = {
    {"w_bero_1", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_03", "e_bero_1", 1, 1, 0, 0},
    {"w_bero_2", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_03", "e_bero_2", 1, 1, 0, 0},
    {"w_bero_3", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_03", "e_bero_3", 1, 1, 0, 0},
    {"e_bero", 1, 6, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_06", "w_bero", 1, 1,
     "door_01", "door_02"},
    {NULL},
};

static s32 key_tbl[] = {
    20, -1,
};

static EVT_BEGIN(evt_chk)
    CALL(evt_pouch_check_item, 20, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock)
    SET_VALUE(LW(0), STRING("e_bero"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

EVT_BEGIN(bigdoor_event_init)
    IF_INT_LESS(GSW(0), 245)
        DEBUG_PUT_MSG(STRING("dou_05：扉が開く前の処理"))
        SET_VALUE(LW(0), STRING("w_bero_3"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_mapobj_trans, 0, STRING("tetu"), 1, 0, 0)
    ELSE
        DEBUG_PUT_MSG(STRING("dou_05：扉が開く後の処理"))
        CALL(evt_hitobj_attr_onoff, 1, 1, STRING("w_bero_3"), 4)
        CALL(evt_mapobj_trans, 0, STRING("tetu"), 1, 100, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(bigdoor_event)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_wait_landon)
        CALL(evt_mario_set_mov_spd, FLOAT(0.0f))
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 150, 432, 40, 0, 337, -480, 500, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), 200)
        WAIT_MS(200)
        CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), 200)
        WAIT_MS(200)
        CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), 200)
        WAIT_MS(200)
        CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), 200)
        WAIT_MS(200)
        CALL(evt_cam_shake, 4, FLOAT(0.001f), FLOAT(0.0f), 400)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_SLIDE_DOOR1_UP"), LW(10))
    CALL(evt_sub_intpl_msec_init, 0, 0, 100, 2000)
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_mapobj_trans, 0, STRING("tetu"), 1, LW(0), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_sfxon, STRING("SFX_SLIDE_DOOR1_DOWN"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 250)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_VOICE1"), 0)
    CALL(evt_msg_print, 0, STRING("stg5_dou_10"), 0, 0)
    SET_VALUE(LW(0), STRING("w_bero_3"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_hitobj_attr_onoff, 1, 1, STRING("w_bero_3"), 4)
    SET_VALUE(GSW(0), 245)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static HitDamageReturn hitDmgRetPoints[] = {
    {"mod_01", {0.0f, -1000.0f, 0.0f}},
    {"mod_02", {0.0f, -1000.0f, 0.0f}},
    {"mod_03", {0.0f, -1000.0f, 0.0f}},
    {"mod_04", {0.0f, -1000.0f, 0.0f}},
    {"Ship_Change", {0.0f, -1000.0f, 0.0f}},
    {NULL},
};

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_item_entry, STRING("item00"), 277, 0, -1000, 0, 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(waterfall_main)
    SET_VALUE(LW(0), 255)
    LOOP(0)
        ADD(LW(0), GW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 255)
            SET_VALUE(LW(0), 255)
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            SET_VALUE(LW(0), 0)
        END_IF
        CALL(alpha_set_func, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(waterfall_init)
    CALL(alpha_init_func)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mizu_ata"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mizu_ata"), 256)
    SET_VALUE(GW(0), 0)
    SPAWN_EVENT(waterfall_main)
    RETURN
EVT_END()

EVT_BEGIN(ride_waterfall)
    SET_VALUE(GW(0), -4)
    RETURN
EVT_END()

EVT_BEGIN(nonride_waterfall)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(2), -700)
        SET_VALUE(GW(0), -4)
    ELSE
        SET_VALUE(GW(0), 4)
    END_IF
    RETURN
EVT_END()

s32 cam_displacement(EventEntry* event, BOOL isFirstCall) {
    void* work = camRoadGetWorkPtr();
    *(f32*)((u8*)work + 0x1C4) = float_1p7_dou_00004750;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(dou_05_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU5"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    WAIT_FRAMES(1)
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("tubo"), 1, 0)
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 175, 120, -135, 0, 0)
    CALL(evt_mobj_lock, STRING("mobj_lock00"), 20, 190, 10, 0, 270, PTR(evt_chk), PTR(evt_unlock), GSWF(2979))
    IF_INT_EQUAL(GSWF(2979), 1)
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_on)
    ELSE
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    CALL(evt_mobj_switch_blue, 0, STRING("mobj00"), -60, 330, -565, PTR(bigdoor_event), GSWF(2978))
    SPAWN_EVENT(bigdoor_event_init)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), 500, 275, -800, 0, 0, PTR(mobj_box00), GSWF(2986))
    CALL(evt_mobj_powerupblk, STRING("pblk1"), -65, 280, -175, 0, GSWF(5556), 0)
    CALL(evt_mobj_brick, STRING("mobj_hidden1"), -35, 255, -175, 121, 11, 0, GSWF(2993))
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change"), STRING("Ship_Change"))
    CALL_CHILD(waterfall_init)
    CALL(evt_run_case_evt, 0, 1, STRING("mizu_atari"), 0, PTR(ride_waterfall), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("mizu_atari"), 0, PTR(nonride_waterfall), 0)
    CALL(cam_displacement)
    RETURN
EVT_END()

// The original places this .rodata float last; the only user is cam_displacement.
const f32 float_1p7_dou_00004750 = 1.7f;

// clang-format on
