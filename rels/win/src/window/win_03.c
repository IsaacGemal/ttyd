#include "win_03.h"
#include "driver/camdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_env.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "mario/mariost.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(12,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(13,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(clouda_house_in)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_mapchange, STRING("win_04"), STRING("w_bero"))
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(clouda_house_out)
    CALL(evt_mario_bgmode_on)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_cam_y_off)
    CALL(evt_mario_set_pos, 277, 54, -450)
    CALL_CHILD(bero_case_entry)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(cloud_out)
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), 306, 23, -448, 0)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(cloud_in)
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD3"), 306, 23, -448, 0)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "win_02", "e_bero", 1, 1},
    {"dokan_1", 2, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_2", 1, 1},
    {"dokan_2", 0x1002, 0, 9, 100000, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_1", 1, 1},
    {"n_bero", 0x1003, 0, 2, 100000, 0, 0, -1, cloud_out, 5, cloud_in, "win_04", "w_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(lookat_house)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), 200)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    CALL(evt_mario_normalize)
    CALL(evt_msg_print, 0, STRING("stg2_win_e04_02"), 0, STRING("extparty"))
    SET_VALUE(GSW(0), 80)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_start_for_event)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()


USER_FUNC(camera_action) {
    CameraEntry* camera = camGetPtr(CAMERA_3D);
    f32* work = (f32*)event->lwData;
    Vec pos = { 290.0f, 150.0f, 491.0f };
    Vec at = { 290.0f, 90.0f, -6.0f };
    f32 t;
    f32 sum;

    if (isFirstCall) {
        work[0] = 0.0f;
        work[1] = 2.5f;
        event->lwData[2] = 0;
        t = 0.0f;
        sum = 0.0f;
        for (;;) {
            if (t >= 2.5f) {
                break;
            }
            t += 0.01f;
            sum += t;
        }
        work[2] = sum - 2.5f;
    }

    if (work[0] < 300.0f - work[2]) {
        work[0] += work[1];
        pos.x = work[0] + -10.0f;
        at.x = work[0] + -10.0f;
        event->lwData[10] = 0;
    } else if (work[0] < 300.0f) {
        work[0] += work[1];
        work[1] -= 0.01f;
        pos.x = work[0] + -10.0f;
        at.x = work[0] + -10.0f;
        event->lwData[10] = 0;
    } else {
        event->lwData[10] = 1;
    }

    camera->presetPos = camera->cameraPos;
    camera->presetAt = camera->cameraAt;
    camera->storedPos = pos;
    camera->storedAt = at;
    camera->startTimeA = gp->renderTime;
    camera->durationA = 0;
    camera->stateA = 3;
    camera->curveA = 11;
    return event->lwData[10] == 0 ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

static EVT_BEGIN(cloud_toujou)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    CALL(evt_cam3d_evt_set, -10, 150, 491, -10, 90, -6, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(camera_action)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg2_win_e04_00_01"), 0, 0)
    CALL(evt_fade_set_mapchange_type, 0, 2, 500, 1, 500)
    CALL(evt_bero_mapchange, STRING("win_04"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(cloud_lostitem)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    CALL(evt_npc_check, STRING("パックン1"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("パックン1"), 0, -100, 0)
    END_IF
    CALL(evt_npc_check, STRING("パックン2"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("パックン2"), 0, -100, 0)
    END_IF
    CALL(evt_cam3d_evt_set, 290, 150, 491, 290, 90, -6, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_env_blur_on, 1, 600)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 290, 137, 391, 290, 90, -6, 100, 11)
        WAIT_MS(100)
        CALL(evt_cam3d_evt_set, 290, 150, 491, 290, 90, -6, 100, 11)
        WAIT_MS(100)
        CALL(evt_cam3d_evt_set, 290, 137, 391, 290, 90, -6, 100, 11)
        WAIT_MS(100)
        CALL(evt_cam3d_evt_set, 290, 150, 491, 290, 90, -6, 100, 11)
        WAIT_MS(100)
        CALL(evt_cam3d_evt_set, 290, 137, 391, 290, 90, -6, 100, 11)
        WAIT_MS(100)
        CALL(evt_cam3d_evt_set, 290, 150, 491, 290, 90, -6, 100, 11)
        WAIT_MS(100)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED3"))
        WAIT_MS(3000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg2_win_e04_00_05"), 0, 0)
    WAIT_WHILE(GF(0))
    WAIT_MS(2000)
    SET_VALUE(GSW(0), 78)
    CALL(evt_fade_set_mapchange_type, 0, 2, 500, 1, 500)
    CALL(evt_bero_mapchange, STRING("win_01"), STRING("n_bero"))
    RETURN
EVT_END()

EVT_BEGIN(tree_evt_01)
    CALL(evt_item_delete_check, STRING("item_tree01"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_tree01"), 125, -333, 72, -24, 0, GSWF(5609), 0)
    END_IF
    RETURN
EVT_END()

static TreeAccessEntry tree_access_data[] = {
    {4, "A_tree_01", "S_tree_mki_01", "S_tree_ha01", NULL, NULL},
    {4, "A_tree_02", "S_tree_mki_02", "S_tree_ha03", NULL, tree_evt_01},
    {4, "A_tree_03", "S_tree_mki_03", "S_tree_ha02", NULL, NULL},
    {4, "A_tree_04", "S_tree_mki_04", "S_tree_ha04", NULL, NULL},
    {7},
};

EVT_BEGIN(dummy_event)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(win_03_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_run_case_evt, 5, 1, STRING("n_bero"), 0, PTR(dummy_event), 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_WIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG2_WIN1"))
    CALL(evt_snd_bgmon, 160, 0)
    SET_VALUE(LW(0), PTR(tree_access_data))
    CALL_CHILD(evt_sub_tree_access_entry)
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 7, 425, 60, 75, 0, 0)
    CALL(evt_item_entry, STRING("i"), 135, 475, 0, -60, 16, GSWF(2679), 0)
    CALL(evt_item_entry, STRING("item_00"), 125, -150, 0, -30, 16, GSWF(5610), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_siro_asu"), 1)
    CALL(evt_hitobj_onoff, STRING("A_siro_asu"), 1, 0)
    IF_INT_EQUAL(GSW(0), 79)
        SPAWN_EVENT(lookat_house)
    END_IF
    IF_INT_EQUAL(GSW(0), 76)
        CALL(evt_npc_check_delete, STRING("パックン1"))
        SPAWN_EVENT(cloud_toujou)
    END_IF
    IF_INT_EQUAL(GSW(0), 77)
        CALL(evt_npc_check_delete, STRING("パックン1"))
        SPAWN_EVENT(cloud_lostitem)
    END_IF
    RETURN
    OPCODE_END_SCRIPT,
    0, 0, 0, 0,
};
// clang-format on
#pragma force_active off
