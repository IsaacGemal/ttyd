#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cloud.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_14_text_4DB8[];
extern s32 evt_usu_kagemario_party_kill_14_data_AF00[];

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door07"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door07"), 0, LW(0), 0)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[30] = {
    (u32)"w_bero", 0x00010002, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"jin_07", (u32)"e_bero",
    0x00050005, (u32)&w_close, (u32)&w_open, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(jin_06_wall_open_evt)
    SET_VALUE(GSWF(2229), 1)
    RETURN
EVT_END()

EVT_BEGIN(jin_06_jumpstand)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            LOOP(0)
                CALL(evt_party_get_name_hitobj_ride, 0, LW(0))
                IF_STR_EQUAL(LW(0), STRING("MOBJ_mobj_jump"))
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_party_jump_pos, 0, 30, 0, 50, 600, 0, -1000)
        END_IF
    END_INLINE_EVENT
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_jump_jumpstand, 0, 0, 0, 0, 400, 0, 0, 0, 50, 1000, 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 281, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(jin_06_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_JIN1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_kagemario_init_14_text_4DB8)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_14_data_AF00)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), 650, 0, -150, 0, 0, PTR(mobj_box00), GSWF(2238))
    CALL(evt_mobj_jumpstand_red, 0, STRING("mobj_jump"), 0, 0, 0, PTR(jin_06_jumpstand), 0)
    IF_INT_NOT_EQUAL(GSWF(2079), 0)
        SET_VALUE(GSWF(2079), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_cam_letter_box_disable, 1)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_mario_set_pos, 5, 365, 0)
            CALL(evt_party_set_pos, 0, 5, 645, 0)
            CALL(evt_cam_road_reset)
            CALL(evt_seq_wait, 2)
            CALL(evt_mario_wait_landon)
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_cam_letter_box_disable, 0)
        END_INLINE_EVENT
        SET_VALUE(LW(0), 1)
    ELSE
        SET_VALUE(LW(0), 0)
    END_IF
    BEGIN_INLINE_EVENT
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_seq_wait, 2)
            LOOP(0)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                IF_INT_LESS(LW(1), 365)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_IF
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(1), 365)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_bero_mapchange, STRING("jin_00"), STRING("ana"))
    END_INLINE_EVENT
    IF_INT_EQUAL(GSWF(2229), 0)
        CALL(evt_cloud_init)
        CALL(evt_cloud_ent, 0, STRING("mekure"), STRING("a_mekure"), PTR(jin_06_wall_open_evt))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("mekure"), 1)
        CALL(evt_hitobj_onoff, STRING("a_mekure"), 0, 0)
    END_IF
    IF_INT_LESS(GSW(0), 195)
        SET_VALUE(GSW(0), 195)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
