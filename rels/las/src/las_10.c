#include "las_10.h"
#include "driver/camdrv.h"
#include "driver/mapdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "party.h"
#include "party/party_motion.h"

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

static EVT_BEGIN(s_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_2"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_1"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door1_2"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(nw_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(nw_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door2"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(ne_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door3"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(ne_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door3"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(sw_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door4"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(sw_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("S_door4"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(se_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door5"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(se_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door5"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(nw2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door6"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(nw2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door6"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(ne2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door7"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(ne2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door7"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(sw2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door8"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(sw2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door8"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(se2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door9"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(se2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_door9"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[10] = {
    { "s_bero", 0x0001, 0x0006, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_09", "n_bero", 0x0005, 0x0005,
      s_door_close, s_door_open },
    { "nw_bero_1", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_11", "e_bero_1", 0x0005, 0x0005,
      nw_door_close, nw_door_open },
    { "ne_bero_1", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_12", "w_bero_1", 0x0005, 0x0005,
      ne_door_close, ne_door_open },
    { "sw_bero_1", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_13", "e_bero_1", 0x0005, 0x0005,
      sw_door_close, sw_door_open },
    { "se_bero_1", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_14", "w_bero_1", 0x0005, 0x0005,
      se_door_close, se_door_open },
    { "nw_bero_2", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_15", "e_bero_1", 0x0005, 0x0005,
      nw2_door_close, nw2_door_open },
    { "ne_bero_2", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_16", "w_bero_1", 0x0005, 0x0005,
      ne2_door_close, ne2_door_open },
    { "sw_bero_2", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_17", "e_bero_1", 0x0005, 0x0005,
      sw2_door_close, sw2_door_open },
    { "se_bero_2", 0x0001, 0x0001, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
      0xFFFFFFFF, NULL, 0xFFFFFFFF, NULL, "las_18", "w_bero_1", 0x0005, 0x0005,
      se2_door_close, se2_door_open },
    { NULL, 0x0000, 0x0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, NULL, 0x00000000, NULL, NULL, NULL, 0x0000, 0x0000,
      0x00000000, 0x00000000 },
};

char* kabe_list_1[8] = {
    "S11_kabe4", "S11_kabe5", "S14_kabe4", "S14_kabe3",
    "S14_kabe2", "S11_kabe1", "S11_kabe2", "S11_kabe3",
};

char* kabe_list_2[8] = {
    "S112_kabe4", "S112_kabe5", "S115_kabe4", "S115_kabe3",
    "S115_kabe2", "S12_kabe1", "S112_kabe2", "S112_kabe3",
};

char* kabe_list_3[8] = {
    "S113_kabe4", "S113_kabe5", "S116_kabe4", "S116_kabe3",
    "S116_kabe2", "S113_kabe1", "S113_kabe2", "S113_kabe3",
};

USER_FUNC(temae_kesu) {
    CameraEntry* cam = camGetPtr(4);
    s32 i;
    s32 prev;
    s32 next;

    mapGrpFlagOff("S_kesu", 1);
    mapGrpFlagOff("S11_kabe", 1);
    mapGrpFlagOff("S112_kabe", 1);
    mapGrpFlagOff("S113_kabe", 1);

    i = (s32)((157.5f + cam->viewYaw) / 45.0f);
    i %= 8;
    prev = (i == 0) ? 7 : i - 1;
    next = (i == 7) ? 0 : i + 1;

    mapGrpFlagOn(kabe_list_1[i], 1);
    mapGrpFlagOn(kabe_list_1[prev], 1);
    mapGrpFlagOn(kabe_list_1[next], 1);
    mapGrpFlagOn(kabe_list_2[i], 1);
    mapGrpFlagOn(kabe_list_2[prev], 1);
    mapGrpFlagOn(kabe_list_2[next], 1);
    mapGrpFlagOn(kabe_list_3[i], 1);
    mapGrpFlagOn(kabe_list_3[prev], 1);
    mapGrpFlagOn(kabe_list_3[next], 1);

    if (i >= 2 && i <= 4) {
        mapGrpFlagOn("S14_hashira", 1);
        mapGrpFlagOn("S115_hashira", 1);
        mapGrpFlagOn("S116_hashira", 1);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(las_10_temae_kabe)
    CALL(evt_seq_wait, 2)
    LOOP(0)
        CALL(temae_kesu)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

USER_FUNC(check_party_pos) {
    s32* args = event->args;
    PartyEntry* party = partyGetPtr(partyCtrlNo);

    if (party == NULL) {
        evtSetValue(event, args[0], 1);
        return EVT_RETURN_DONE;
    }
    if (party->position.y >= 360.0f) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(first_evt)
    CALL(evt_seq_wait, 2)
    LOOP(0)
        CALL(evt_mario_get_name_hitobj_ride, LW(0))
        IF_STR_EQUAL(LW(0), STRING("A_3_3"))
            BREAK_LOOP
        END_IF
        IF_STR_EQUAL(LW(0), STRING("A_3_2"))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_hello_party, 0, 1)
    END_IF
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_party_use_check, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_party_stop, 0)
        CALL(check_party_pos, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_party_set_pos, 0, 97, 360, 43)
        END_IF
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -40, 79, 60)
    CALL(evt_mario_adjust_dir)
    WAIT_WHILE(GF(0))
    CALL(evt_party_move_pos2, 0, -55, 107, 80)
    WAIT_MS(300)
    CALL(evt_party_set_dir, 0, 90, 300)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg8_las_31"))
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 530, 644, 0, 560, 95, 3000, 11)
    WAIT_MS(3000)
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, -460, 865, 594, 0, 455, -128, 0, 11)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_mario_set_reardir)
    CALL(evt_party_set_dir, 0, 270, 0)
    WAIT_FRAMES(1)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("stg8_las_32"))
    SET_VALUE(GSWF(4330), 1)
    WAIT_MS(2000)
    CALL(evt_party_run, 0)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    WAIT_FRAMES(1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tenkyugi_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff_f, 512, 3000)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_CELESTIAL_SHAKE1"), LW(10))
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1200)
    CALL(evt_snd_sfxoff, LW(10))
    WAIT_MS(1200)
    CALL(evt_cam3d_evt_set, 0, 425, 787, 0, 530, 95, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(1000)
    CALL(evt_mapobj_color, 1, STRING("S_hoshihoshi"), 255, 255, 255, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hoshihoshi"), 1)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_CELESTIAL_MOVE1"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_color, 1, STRING("S_hoshihoshi"), 255, 255, 255, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_map_playanim, STRING("en"), 1, 0)
    CALL(evt_sub_intpl_msec_init, 1, 0, 1000, 3000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
        CALL(evt_map_set_playrate, STRING("en"), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_map_set_playrate, STRING("en"), FLOAT(1.0f))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -125, 575, 295, -125, 575, 95, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, 125, 575, 295, 125, 575, 95, 5000, 0)
    WAIT_MS(5000)
    CALL(evt_mario_set_pos, -55, 400, 60)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_set_party, 0, 1)
    END_IF
    CALL(evt_party_stop, 0)
    CALL(evt_party_set_pos, 0, -25, 400, 80)
    CALL(evt_mario_dispflag_onoff, 1, 512)
    CALL(evt_party_dispflg_onoff, 0, 32, 1)
    CALL(evt_mario_set_dispdir, 180)
    CALL(evt_party_set_dispdir, 0, 180)
    CALL(evt_mario_adjust_dir)
    WAIT_FRAMES(1)
    CALL(evt_mario_dispflag_onoff, 0, 512)
    CALL(evt_party_dispflg_onoff, 0, 32, 0)
    CALL(evt_cam3d_evt_set, 0, 500, 390, 0, 450, 95, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg8_las_33"))
    WAIT_MS(2000)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG8_CELESTIAL_SHAKE1"), LW(10))
        LOOP(20)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 500)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 100, 0)
        WAIT_MS(300)
        CALL(evt_mario_set_dir, 90, 100, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg8_las_34"))
    SET_VALUE(GF(0), 1)
    CALL(evt_bero_mapchange, STRING("las_19"), STRING("chgmap_evt"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tenkyugi_evt2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_CELESTIAL_MOVE2"), 0, 400, -150, LW(15))
    CALL(evt_snd_sfx_dist, LW(15), 10)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hoshihoshi"), 1)
    CALL(evt_map_playanim, STRING("en"), 1, 0)
    CALL(evt_mario_dispflag_onoff, 1, 512)
    CALL(evt_party_dispflg_onoff, 0, 32, 1)
    CALL(evt_mario_set_dispdir, 180)
    CALL(evt_party_set_dispdir, 0, 180)
    CALL(evt_party_stop, 0)
    CALL(evt_cam_road_reset)
    WAIT_FRAMES(1)
    CALL(evt_mario_dispflag_onoff, 0, 512)
    CALL(evt_party_dispflg_onoff, 0, 32, 0)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, 0, 500, 390, 0, 450, 95, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_mario_set_pos, -55, 400, 60)
    CALL(evt_party_set_pos, 0, -25, 400, 80)
    CALL(evt_snd_sfxon, STRING("SFX_STG8_CELESTIAL_SHAKE1"), LW(10))
    SET_FLOAT_VALUE(LW(0), FLOAT(0.01f))
    LOOP(4)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.75f))
        CALL(evt_cam_shake, 4, LW(0), FLOAT(0.0f), 1000)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg8_las_35"))
    SET_VALUE(GSW(0), 390)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG8_LST2"), 2000)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_off, 1000, 11)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kagi_rotate)
    SET_VALUE(LW(10), LW(0))
    SET_VALUE(LW(9), LW(1))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG8_KEYSTAND_SHAKE1"), LW(0))
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1000)
        CALL(evt_snd_sfxoff, LW(0))
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, LW(10), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEYSTAND_ROUND1"), LW(0), LW(1), LW(2), LW(8))
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, LW(10), 0, LW(0), 0)
    CALL(evt_hit_bind_update, LW(9))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_snd_sfxoff, LW(8))
    RETURN
EVT_END()

s32 kagi_tbl[9] = {
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1,
};

USER_FUNC(make_kagi_tbl) {
    s32* args = event->args;
    s32 count = 0;
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((s32)pouchCheckItem(i + 38) > 0) {
            kagi_tbl[count] = i + 38;
            count++;
        }
    }
    kagi_tbl[count] = -1;
    evtSetValue(event, args[0], (s32)kagi_tbl);
    evtSetValue(event, args[1], count);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(kagi1_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi1"))
    SET_VALUE(LW(1), STRING("A113_daiza1"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4331), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(kagi2_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi2"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi2"))
    SET_VALUE(LW(1), STRING("A113_daiza2"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4332), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(kagi3_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi3"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi3"))
    SET_VALUE(LW(1), STRING("A113_daiza3"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4333), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(kagi4_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi4"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi4"))
    SET_VALUE(LW(1), STRING("A113_daiza4"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4334), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(kagi5_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi5"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi5"))
    SET_VALUE(LW(1), STRING("A113_daiza5"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4335), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(kagi6_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi6"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi6"))
    SET_VALUE(LW(1), STRING("A113_daiza6"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4336), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(kagi7_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi7"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi7"))
    SET_VALUE(LW(1), STRING("A113_daiza7"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4337), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(kagi8_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(make_kagi_tbl, LW(0), LW(1))
    IF_INT_GREATER(LW(1), 0)
        CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
        IF_INT_EQUAL(LW(1), -1)
            CALL(evt_mario_key_onoff, 1)
            RETURN
        END_IF
    ELSE
        CALL(evt_msg_print, 0, STRING("stg8_las_32_01"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, LW(1), LW(2))
    CALL(evt_mapobj_get_position, STRING("S_kagi8"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEY_SET1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(0), STRING("S_kagi8"))
    SET_VALUE(LW(1), STRING("A113_daiza8"))
    CALL_CHILD(kagi_rotate)
    SET_VALUE(GSWF(4338), 1)
    ADD(GSW(1122), 1)
    IF_INT_EQUAL(GSW(1122), 8)
        CALL_CHILD(tenkyugi_evt)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(daiza_up)
    SET_VALUE(LW(8), LW(0))
    SET_VALUE(LW(9), LW(1))
    SET_VALUE(LW(7), LW(2))
    CALL(evt_mapobj_get_position, LW(8), LW(0), LW(1), LW(2))
    CALL(evt_eff, LW(7), STRING("mist2"), 0, LW(0), 410, LW(2), 40, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT_GET_ID(LW(6))
        CALL(evt_mapobj_get_position, LW(8), LW(10), LW(11), LW(12))
        LOOP(0)
            CALL(evt_mapobj_get_position, LW(8), LW(0), LW(1), LW(2))
            SUBTRACT_FLOAT(LW(1), LW(11))
            SUBTRACT_FLOAT(LW(1), FLOAT(38.0f))
            CALL(evt_eff, 0, STRING("mist2"), 1, LW(0), 400, LW(2), 15, LW(1), 100, 0, 0, 0, 0, 0)
            WAIT_MS(200)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mapobj_color, 1, LW(9), 0, 0, 0, 255)
    BEGIN_INLINE_EVENT
        WAIT_MS(1700)
        CALL(evt_mapobj_get_position, LW(8), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEYSTAND_APPEAR2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 11, -10000, 0, 3000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
    CALL(evt_mapobj_trans, 1, LW(8), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(3.6f))
    CALL(evt_mapobj_rotate, 1, LW(8), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    DELETE_EVENT(LW(6))
    CALL(evt_eff_softdelete, LW(7))
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, LW(9), LW(0), LW(1), LW(2))
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_color, 1, LW(9), LW(0), LW(0), LW(0), 255)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_mapobj_trans, 1, LW(8), 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, LW(8), 0, 0, 0)
    CALL(evt_mapobj_color, 1, LW(9), 255, 255, 255, 255)
    RETURN
EVT_END()

EVT_BEGIN(kagi_8_off)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kagi"), 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza1"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A113_daiza2"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A113_daiza3"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A113_daiza4"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A113_daiza5"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A113_daiza6"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A113_daiza7"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A113_daiza8"), 1, 0)
    RETURN
EVT_END()

char* daiza_name[25] = {
    "S_kagi1", "S113_daiza1", "eff_1", "S_kagi2",
    "S113_daiza2", "eff_2", "S_kagi3", "S113_daiza3",
    "eff_3", "S_kagi4", "S113_daiza4", "eff_4",
    "S_kagi5", "S113_daiza5", "eff_5", "S_kagi6",
    "S113_daiza6", "eff_6", "S_kagi7", "S113_daiza7",
    "eff_7", "S_kagi8", "S113_daiza8", "eff_8",
    NULL,
};

EVT_BEGIN(kagi_8_on)
    IF_INT_LESS(GSW(0), 389)
        WAIT_MS(500)
        SET_VALUE(GF(0), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_snd_sfxon, STRING("SFX_STG8_KEYSTAND_SHAKE1"), LW(10))
            LOOP(0)
                IF_INT_NOT_EQUAL(GF(0), 0)
                    BREAK_LOOP
                END_IF
                CALL(evt_cam_shake, 4, FLOAT(0.003f), FLOAT(0.0f), 300)
            END_LOOP
            CALL(evt_snd_sfxoff, LW(10))
        END_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi"), 1)
        SET_INT_READ_ADDR(PTR(daiza_name))
        LOOP(0)
            READ_INT3(LW(0), LW(1), LW(2))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_mapobj_trans, 1, LW(0), 0, -100, 0)
        END_LOOP
        CALL(evt_mapobj_get_position, STRING("S_daizadai"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_KEYSTAND_APPEAR1"), LW(0), LW(1), LW(2), LW(5))
        SET_INT_READ_ADDR(PTR(daiza_name))
        LOOP(0)
            READ_INT3(LW(0), LW(1), LW(2))
            IF_INT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            SPAWN_EVENT(daiza_up)
            WAIT_MS(1000)
        END_LOOP
        WAIT_MS(4000)
        CALL(evt_snd_sfxoff, LW(5))
        SET_VALUE(GF(0), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kagi"), 1)
    END_IF
    CALL(evt_hitobj_onoff, STRING("A113_daiza1"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza2"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza3"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza4"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza5"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza6"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza7"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A113_daiza8"), 1, 1)
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza1"), STRING("S_kagi1"))
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza2"), STRING("S_kagi2"))
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza3"), STRING("S_kagi3"))
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza4"), STRING("S_kagi4"))
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza5"), STRING("S_kagi5"))
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza6"), STRING("S_kagi6"))
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza7"), STRING("S_kagi7"))
    CALL(evt_hit_bind_mapobj, STRING("A113_daiza8"), STRING("S_kagi8"))
    IF_INT_EQUAL(GSWF(4331), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi1"), 0, PTR(kagi1_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi1"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza1"))
    END_IF
    IF_INT_EQUAL(GSWF(4332), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi2"), 0, PTR(kagi2_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi2"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza2"))
    END_IF
    IF_INT_EQUAL(GSWF(4333), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi3"), 0, PTR(kagi3_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi3"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza3"))
    END_IF
    IF_INT_EQUAL(GSWF(4334), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi4"), 0, PTR(kagi4_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi4"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza4"))
    END_IF
    IF_INT_EQUAL(GSWF(4335), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi5"), 0, PTR(kagi5_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi5"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza5"))
    END_IF
    IF_INT_EQUAL(GSWF(4336), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi6"), 0, PTR(kagi6_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi6"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza6"))
    END_IF
    IF_INT_EQUAL(GSWF(4337), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi7"), 0, PTR(kagi7_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi7"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza7"))
    END_IF
    IF_INT_EQUAL(GSWF(4338), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("A_kagi8"), 0, PTR(kagi8_evt), 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_kagi8"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A113_daiza8"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kagi_8_init)
    IF_INT_LESS(GSW(0), 389)
        CALL_CHILD(kagi_8_off)
    ELSE
        CALL_CHILD(kagi_8_on)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(kagi_8_syutugen)
    CALL(evt_mapobj_get_position, STRING("S_daizadai"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_STAR_SET1"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(500)
    CALL_CHILD(kagi_8_on)
    RETURN
EVT_END()

s32 daiza_key_tbl[2] = {
    0x00000025, -1,
};

EVT_BEGIN(daiza_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_pouch_check_item, 37, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_win_item_select, 0, PTR(daiza_key_tbl), LW(0), 0)
    IF_INT_EQUAL(LW(0), -1)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    CALL(evt_pouch_remove_item, 37, LW(0))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hosi_off"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("hosi_on"), 1)
    CALL_CHILD(kagi_8_syutugen)
    SET_VALUE(GSW(0), 389)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(las_10_init_evt)
    IF_INT_EQUAL(GSW(0), 389)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("ret_chgmap_evt"))
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS4"))
            CALL(evt_snd_set_rev_mode, 2)
        ELSE
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS4"))
            CALL(evt_snd_set_rev_mode, 2)
        END_IF
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS4"))
        CALL(evt_snd_set_rev_mode, 2)
    END_IF
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT(las_10_temae_kabe)
    IF_INT_EQUAL(GSWF(4330), 0)
        SPAWN_EVENT(first_evt)
    END_IF
    IF_INT_LESS(GSW(0), 389)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hosi_on"), 1)
        CALL(evt_run_case_evt, 9, 1, STRING("A_daiza1"), 0, PTR(daiza_evt), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("hosi_off"), 1)
    END_IF
    SPAWN_EVENT(kagi_8_init)
    IF_INT_EQUAL(GSW(0), 389)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("tenkyugi_evt"))
            BEGIN_INLINE_EVENT
                CALL(evt_mario_set_pos, -84, 400, 240)
                CALL(evt_party_set_pos, 0, -123, 400, 255)
                CALL(evt_cam_road_reset)
                CALL(evt_seq_wait, 2)
                CALL(evt_bero_exec_wait, 65536)
                SPAWN_EVENT(tenkyugi_evt)
            END_INLINE_EVENT
        END_IF
        IF_STR_EQUAL(LW(0), STRING("ret_chgmap_evt"))
            SPAWN_EVENT(tenkyugi_evt2)
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 390)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_CELESTIAL_MOVE2"), 0, 400, -150, LW(10))
        CALL(evt_snd_sfx_dist, LW(10), 10)
        CALL(evt_map_playanim, STRING("en"), 1, 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hoshihoshi"), 1)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group19"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group18"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group10"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group11"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group12"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group13"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group14"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S11_kabe5_ushi"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S11_kabe1_ushi"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S11_dan"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S112_hashira9"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S112_hashira10"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S112_hashira1"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S112_hashira2"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S112_hashira3"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S112_hashira4"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S112_kabe5_ushi"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S12_kabe1"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_112_dan"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S113_hashira6"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S113_hashira5"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S113_kabe5"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S113_kabe1"), 2)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
