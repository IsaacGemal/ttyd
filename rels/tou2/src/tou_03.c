// This unit defines its own GXSetTexCoordGen (the SDK header declares it static inline); rename the header's
// copy so the unit's global definition below does not clash.
#define GXSetTexCoordGen GXSetTexCoordGen_sdk_inline
#include "tou_03.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/extdrv.h"
#include "driver/itemdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_mahojin.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include "mario/mariost.h"
#include "memory.h"
#include "driver/dispdrv.h"
#include "pmario_sound.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin/os.h>
#include <dolphin/gx/GXTev.h>
#include <dolphin/gx/GXFrameBuffer.h>
#include <dolphin/gx/GXManage.h>
#undef GXSetTexCoordGen

#pragma pool_data off

typedef struct ScreenWork {
    void* field_0x0; // 0x0 tex buffer
    s32 field_0x4;   // 0x4
    f32 field_0x8;   // 0x8
    f32 field_0xC;   // 0xC
    f32 field_0x10;  // 0x10
    f32 field_0x14;  // 0x14
    f32 field_0x18;  // 0x18
    f32 field_0x1C;  // 0x1C
    s32 field_0x20;  // 0x20
    s32 field_0x24;  // 0x24
    f32 field_0x28;  // 0x28
    void* field_0x2C; // 0x2C
} ScreenWork;

typedef struct KyakuEntry {
    char name1[0x10]; // 0x0
    char name2[0x8];  // 0x10
    s16 field_0x18;   // 0x18
    u8 pad_0x1A[2];   // 0x1A
    f32 field_0x1C;   // 0x1C
    s32 field_0x20;   // 0x20
    s32 field_0x24;   // 0x24
} KyakuEntry;

typedef struct KyakuState {
    u8 field_0x0[0x10]; // 0x0
    s16 poseId;         // 0x10
    u8 pad_0x12[6];     // 0x12
    u64 field_0x18;     // 0x18
} KyakuState;

typedef struct KyakuWork {
    u8 field_0x0[0x4];    // 0x0
    s32 count1;           // 0x4
    KyakuState* array1;   // 0x8
    s32 count2;           // 0xC
    KyakuEntry entries[]; // 0x10
} KyakuWork;

#pragma force_active on
extern s32 evt_tou_get_fighter_battle_msg_28_text_3CB4[];
extern s32 evt_tou_get_fighter_info_28_text_3E58[];
extern s32 evt_tou_get_fighter_name2_28_text_3DD4[];
extern s32 evt_tou_get_fighter_num[];
extern s32 evt_tou_get_fighter_tribe[];
extern s32 evt_tou_get_ranking_28_text_4048[];
extern s32 evt_tou_set_ranking_win_lose_28_text_35C0[];
extern s32 evt_tou_set_rule_result_28_text_355C[];
extern s32 tou_champion_belt_28_text_2A10[];
extern s32 tou_champion_belt_onoff_28_text_29DC[];
static void callback(TevCallbackParam* param);
// cosf: this unit has its own copy; a header already declares it.
// sinf: this unit has its own copy; a header already declares it.
static USER_FUNC(screen_evt);
static void screen_capture(CameraId camera, void* param);
static f32 randf(f32 min, f32 max);
static void pose_change(KyakuState* state, const char* name1, const char* name2);
static void pose_main(void);  // TODO: real signature
static void pose_init(void);
s32 gans_alpha;
static ScreenWork* screen_wp;
static KyakuWork* kyaku_wp;
s32 burst_chain_entry[289];
s32 burst_wangwang[69];
s32 evt_kurikuri_opening[442];
s32 evt_kurikuri_win[231];
s32 evt_kurikuri_lose[146];
s32 evt_shinemon_opening[647];
s32 evt_shinemon_win[199];
s32 evt_shinemon_lose[84];
s32 evt_champ_opening[556];
s32 evt_champ_opening_re[767];
s32 evt_champ_win[335];
s32 evt_champ_lose[124];
s32 evt_default_opening[748];
s32 evt_default_win[201];
s32 evt_default_lose[137];

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

// static kyaku pose table: {model name, group name, animation frame value}
typedef struct ExtKyakuData {
    const char* name1; // 0x0
    const char* name2; // 0x4
    f32 value;         // 0x8
} ExtKyakuData;

ExtKyakuData ext_kyaku_data[51] = {
    { "c_kinopio", "KNP_S_1", 0.0f },
    { "c_kinopio", "KNP_S_1", 10.0f },
    { "c_kinopio", "KNP_S_1", 20.0f },
    { "c_kinopio", "KNP_S_1", 30.0f },
    { "c_kinopio", "KNP_S_1", 40.0f },
    { "c_kinopio", "KNP_S_1", 50.0f },
    { "c_kinopio", "KNP_S_1", 60.0f },
    { "c_kinopio", "KNP_S_1", 70.0f },
    { "c_kinopio", "KNP_S_1", 80.0f },
    { "c_kinopio", "KNP_S_1", 90.0f },
    { "c_kinopio", "KNP_S_1", 100.0f },
    { "c_kinopio", "KNP_S_1", 110.0f },
    { "c_kinopio", "KNP_S_1", 120.0f },
    { "c_kinopio", "KNP_S_1", 130.0f },
    { "c_kinopio", "KNP_S_1", 140.0f },
    { "c_kinopio", "KNP_S_1", 150.0f },
    { "c_kinopio", "KNP_S_1", 160.0f },
    { "c_kinopio", "KNP_S_1", 170.0f },
    { "c_kinopio", "KNP_S_1", 180.0f },
    { "c_kinopio", "KNP_S_1", 190.0f },
    { "c_kinopiko", "KNF_S_1", 0.0f },
    { "c_kinopiko", "KNF_S_1", 10.0f },
    { "c_kinopiko", "KNF_S_1", 20.0f },
    { "c_kinopiko", "KNF_S_1", 30.0f },
    { "c_kinopiko", "KNF_S_1", 40.0f },
    { "c_kinopiko", "KNF_S_1", 50.0f },
    { "c_kinopiko", "KNF_S_1", 60.0f },
    { "c_kinopiko", "KNF_S_1", 70.0f },
    { "c_kinopiko", "KNF_S_1", 80.0f },
    { "c_kinopiko", "KNF_S_1", 90.0f },
    { "c_kinopiko", "KNF_S_1", 100.0f },
    { "c_kinopiko", "KNF_S_1", 110.0f },
    { "c_kinopiko", "KNF_S_1", 120.0f },
    { "c_kinopiko", "KNF_S_1", 130.0f },
    { "c_kinopiko", "KNF_S_1", 140.0f },
    { "c_kinopiko", "KNF_S_1", 150.0f },
    { "c_kinopiko", "KNF_S_1", 160.0f },
    { "c_roten", "S_1", 0.0f },
    { "c_roten", "S_1", 10.0f },
    { "c_roten", "S_1", 20.0f },
    { "c_roten", "S_1", 30.0f },
    { "c_roten", "S_1", 40.0f },
    { "c_roten", "S_1", 50.0f },
    { "c_jugemu", "JGM_S_1", 0.0f },
    { "c_jugemu", "JGM_S_1", 10.0f },
    { "c_jugemu", "JGM_S_1", 20.0f },
    { "c_jugemu", "JGM_S_1", 30.0f },
    { "c_jugemu", "JGM_S_1", 40.0f },
    { "c_jugemu", "JGM_S_1", 50.0f },
    { "c_jugemu", "JGM_S_1", 60.0f },
    { NULL, NULL, 0.0f },
};

EVT_BEGIN(x)
EVT_END()

// TODO: write ext_kyaku_dispent (global, 0x40 bytes) here:
void ext_kyaku_dispent(void) {
    dispEntry(4, 0.0f, 1, kyaku_disp, NULL);
}

// TODO: write ext_kyaku_main (global, 0x20 bytes) here:
void ext_kyaku_main(void) {
    pose_main();
}

// NONMATCHING (partial): only the allocation/init preamble is written; the per-pose hit-check placement
// search (hitCheckFilter / angleABf / irand switch) is still TODO.
void ext_kyaku_init(void) {
    kyaku_wp = _mapAlloc(0x510);
    memset(kyaku_wp, 0, 0x510);
    kyaku_wp->count1 = extGetPoseNum();
    kyaku_wp->array1 = _mapAlloc(kyaku_wp->count1 * 0x20);
    memset(kyaku_wp->array1, 0, kyaku_wp->count1 * 0x20);
    pose_init();
    __tou_srand(0x25B0E2B);
}

EVT_BEGIN(evt_kyaku)
    CALL(evt_npc_entry, STRING("kyaku1"), STRING("c_kinopio"))
    CALL(evt_npc_entry, STRING("kyaku2"), STRING("c_kinopiko"))
    CALL(evt_npc_entry, STRING("kyaku3"), STRING("c_roten"))
    CALL(evt_npc_entry, STRING("kyaku4"), STRING("c_jugemu"))
    CALL(evt_npc_set_tribe, STRING("kyaku1"), STRING("キノピオ"))
    CALL(evt_npc_set_tribe, STRING("kyaku2"), STRING("キノピオ女"))
    CALL(evt_npc_set_tribe, STRING("kyaku3"), STRING("ロテン"))
    CALL(evt_npc_set_tribe, STRING("kyaku4"), STRING("ジュゲム"))
    CALL(evt_npc_set_ry, STRING("kyaku1"), 90)
    CALL(evt_npc_set_ry, STRING("kyaku2"), 90)
    CALL(evt_npc_set_ry, STRING("kyaku3"), 270)
    CALL(evt_npc_set_ry, STRING("kyaku4"), 270)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("s_bero_2"))
        CALL(evt_npc_set_position, STRING("kyaku1"), -40, 120, 680)
        CALL(evt_npc_set_position, STRING("kyaku2"), -15, 120, 670)
        CALL(evt_npc_set_position, STRING("kyaku3"), 15, 120, 680)
        CALL(evt_npc_set_position, STRING("kyaku4"), 40, 120, 670)
    ELSE
        CALL(evt_npc_set_position, STRING("kyaku1"), -50, 0, 370)
        CALL(evt_npc_set_position, STRING("kyaku2"), -20, 0, 360)
        CALL(evt_npc_set_position, STRING("kyaku3"), 20, 0, 370)
        CALL(evt_npc_set_position, STRING("kyaku4"), 50, 0, 360)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("kyaku1"), 1073743360)
    CALL(evt_npc_flag_onoff, 1, STRING("kyaku2"), 1073743360)
    CALL(evt_npc_flag_onoff, 1, STRING("kyaku3"), 1073743360)
    CALL(evt_npc_flag_onoff, 1, STRING("kyaku4"), 1073743360)
    CALL(evt_ext_entry, 1000, PTR(ext_kyaku_data), PTR(ext_kyaku_init), PTR(ext_kyaku_main), PTR(ext_kyaku_dispent))
    CALL(evt_hitobj_onoff, STRING("group246"), 1, 0)
    RETURN
EVT_END()

// TODO: type
static u32 inta_tex[8] = {
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
};

EVT_BEGIN(open_main)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, LW(8), LW(0), 0, 0)
    CALL(evt_mapobj_rotate, 1, LW(9), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_FALL1"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(close_main)
    CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, LW(8), LW(0), 0, 0)
    CALL(evt_mapobj_rotate, 1, LW(9), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_door_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("aka_door_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_open)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w_close)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_ddor_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_door_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("ao_ddor_b"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_close)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(s1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_b"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(s1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_b"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(s1_door_open2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("door_2_a"), LW(0), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("door_2_b"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(s1_open)
    BEGIN_INLINE_EVENT
        CALL(evt_bero_exec_wait, 65536)
        SET_VALUE(LW(8), STRING("door_2_a"))
        SET_VALUE(LW(9), STRING("door_2_b"))
        CALL_CHILD(open_main)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s1_close)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(8), STRING("door_2_a"))
        SET_VALUE(LW(9), STRING("door_2_b"))
        CALL_CHILD(close_main)
    END_INLINE_EVENT
    WAIT_MS(500)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(s2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_b"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(s2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_a"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_b"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(s2_door_open2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("door_3_a"), LW(0), 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("door_3_b"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(s2_open)
    BEGIN_INLINE_EVENT
        CALL(evt_bero_exec_wait, 65536)
        SET_VALUE(LW(8), STRING("door_3_a"))
        SET_VALUE(LW(9), STRING("door_3_b"))
        CALL_CHILD(open_main)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(s2_close)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(8), STRING("door_3_a"))
        SET_VALUE(LW(9), STRING("door_3_b"))
        CALL_CHILD(close_main)
    END_INLINE_EVENT
    WAIT_MS(500)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[5] = {
    {"w_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, w_open, -1, w_close, "tou_04", "n_bero_2", 5, 5, w_door_close, w_door_open},
    {"e_bero", 1, 6, 0x4E20, 100000, 0, 0, -1, e_open, -1, e_close, "tou_04", "n_bero_1", 5, 5, e_door_close, e_door_open},
    {"s_bero_1", 1, 6, 0x4E20, 100000, 0, 0, -1, s1_open, -1, s1_close, "tou_02", "n_bero_1", 5, 5, s1_door_close, s1_door_open},
    {"s_bero_2", 1, 6, 0x4E20, 100000, 0, 0, -1, s2_open, -1, s2_close, "tou_02", "n_bero_2", 5, 5, s2_door_close, s2_door_open},
    {NULL},
};

EVT_BEGIN(evt_tougi)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    SET_VALUE(LF(0), 0)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("s_bero_2"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_mario_set_pos, -25, 0, 635)
        CALL(evt_party_set_pos, 0, 25, 0, 635)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_party_set_dir, 0, 270, 0)
    ELSE
        CALL(evt_mario_set_pos, -25, 120, 980)
        CALL(evt_party_set_pos, 0, 25, 120, 980)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_party_set_dir, 0, 270, 0)
    END_IF
    CALL(evt_mario_set_dir_npc, STRING("チャンピョン"))
    CALL(evt_party_set_dir_npc, 0, STRING("相手"))
    CALL(evt_set_dir_to_target, STRING("チャンピョン"), STRING("相手"))
    CALL(evt_cam3d_evt_set, 0, 563, 962, 0, 554, 412, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_npc_sound_data_set, STRING("相手"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, STRING("SFX_STG3_TOGENOKO_DOWN1"))
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    IF_INT_EQUAL(LF(0), 0)
        CALL_CHILD(s1_door_open)
    ELSE
        CALL_CHILD(s2_door_open)
    END_IF
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, 0, 258, 962, 0, 215, 412, 5000, 0)
    WAIT_MS(5000)
    WAIT_MS(2000)
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_cam3d_evt_set, 0, 130, 840, 0, 50, 397, 1500, 11)
    ELSE
        CALL(evt_cam3d_evt_set, 0, 240, 1187, 0, 170, 742, 1500, 11)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        SUBTRACT(LW(2), 230)
        CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(80.0f))
    END_INLINE_EVENT
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 230)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(80.0f))
    SET_VALUE(LW(1), 2)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    IF_INT_EQUAL(LF(0), 0)
        CALL_CHILD(s1_door_close)
    ELSE
        CALL_CHILD(s2_door_close)
    END_IF
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    LABEL(2)
    CALL(evt_sub_intpl_msec_get_value)
    IF_INT_EQUAL(LF(0), 0)
        CALL_CHILD(s1_door_open2)
    ELSE
        CALL_CHILD(s2_door_open2)
    END_IF
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 300, 0)
    END_INLINE_EVENT
    CALL(evt_party_set_dir, 0, 270, -1)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_25"))
    WAIT_MS(300)
    CALL(evt_mario_set_dir_npc, STRING("チャンピョン"))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 85, 172, 326, 85, 96, -104, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_26"), 0, STRING("チャンピョン"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -85, 172, 326, -85, 96, -104, 1000, 11)
    CALL(evt_npc_get_position, STRING("チャンピョン"), LW(10), LW(11), LW(12))
    CALL(evt_npc_status_onoff, 1, STRING("チャンピョン"), 2)
    CALL(evt_npc_get_position, STRING("チャンピョン"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 50)
    CALL(evt_npc_move_position, STRING("チャンピョン"), LW(0), LW(2), 0, FLOAT(100.0f), 1)
    CALL(evt_npc_set_rotate, STRING("チャンピョン"), 0, 0, -30)
    CALL(evt_npc_get_position, STRING("相手"), LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(evt_npc_jump_position_nohit, STRING("チャンピョン"), LW(0), LW(1), LW(2), 700, FLOAT(40.0f))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_BLED_ATTACK1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(3.0f), 10, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 300)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), -30)
        LOOP(10)
            CALL(evt_npc_set_rotate, STRING("チャンピョン"), 0, 0, LW(0))
            ADD(LW(0), 3)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_set_rotate, STRING("チャンピョン"), 0, 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_sound_data_set, STRING("チャンピョン"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, 0, 0)
        ADD(LW(0), 30)
        SUBTRACT(LW(1), 20)
        CALL(evt_npc_jump_position_nohit, STRING("チャンピョン"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
        ADD(LW(0), 30)
        CALL(evt_npc_jump_position_nohit, STRING("チャンピョン"), LW(0), LW(1), LW(2), 300, FLOAT(10.0f))
        CALL(evt_npc_jump_position_nohit, STRING("チャンピョン"), LW(10), LW(11), LW(12), 300, FLOAT(10.0f))
        CALL(evt_npc_sound_data_reset, STRING("チャンピョン"))
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("相手"), STRING("K_2"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("相手"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 30)
        CALL(evt_npc_jump_position_nohit, STRING("相手"), LW(0), LW(1), LW(2), 300, FLOAT(20.0f))
        SUBTRACT(LW(0), 30)
        CALL(evt_npc_jump_position_nohit, STRING("相手"), LW(0), LW(1), LW(2), 300, FLOAT(10.0f))
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg3_tou_27"), 0, STRING("相手"))
    CALL(evt_cam3d_evt_set, 65, 172, 326, 65, 96, -104, 1000, 11)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 1, STRING("ガンス"), 2)
        CALL(evt_npc_set_position, STRING("ガンス"), 160, 60, -65)
        CALL(evt_npc_move_position, STRING("ガンス"), 45, -65, 0, FLOAT(50.0f), 1)
        CALL(evt_npc_move_position, STRING("ガンス"), 20, 0, 0, FLOAT(50.0f), 1)
        CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("チャンピョン"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(1200)
    CALL(evt_npc_set_anim, STRING("チャンピョン"), STRING("S_4"))
    CALL(evt_npc_set_autotalkpose, STRING("チャンピョン"), STRING("S_4"), STRING("T_4"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_28"), 0, STRING("チャンピョン"))
    WAIT_MS(300)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_29"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_5"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_6"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_30"), 0, STRING("チャンピョン"))
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_7"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("S_2"))
    CALL(evt_npc_set_anim, STRING("チャンピョン"), STRING("H_1"))
    CALL(evt_npc_set_autotalkpose, STRING("チャンピョン"), STRING("S_2"), STRING("T_2"))
    CALL(evt_msg_continue)
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("チャンピョン"), STRING("H_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("チャンピョン"), STRING("S_3"))
    CALL(evt_npc_entry, STRING("ベルト"), STRING("c_bred_h"))
    CALL(evt_npc_get_position, STRING("チャンピョン"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 5)
    ADD(LW(1), 65)
    CALL(evt_npc_set_position, STRING("ベルト"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_anim, STRING("ベルト"), STRING("C_1"))
    ADD(LW(2), 10)
    CALL(evt_eff, 0, STRING("starstone"), 9, LW(0), LW(1), LW(2), FLOAT(0.75f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_WIN_AUDIENCE2"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(2000)
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_cam3d_evt_set, 0, 95, 692, 0, 43, 397, 0, 11)
    ELSE
        CALL(evt_cam3d_evt_set, 0, 218, 1037, 0, 165, 742, 0, 11)
    END_IF
    WAIT_FRAMES(1)
    CALL(evt_mario_set_dir_npc, STRING("チャンピョン"))
    CALL(evt_party_set_dir_npc, 0, STRING("チャンピョン"))
    CALL(evt_eff_fukidashi, 3, STRING("eff"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("stg3_tou_31"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 80, 67, 225, 80, 172, -161, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print_party, STRING("stg3_tou_32"))
    WAIT_MS(300)
    CALL(evt_fade_set_mapchange_type, 0, 2, 500, 1, 500)
    SET_VALUE(GSWF(2374), 1)
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_bero_mapchange, STRING("tou_02"), STRING("n_bero_1"))
    ELSE
        CALL(evt_bero_mapchange, STRING("tou_02"), STRING("n_bero_2"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(init_kinosikowa)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 172)
        CALL(evt_npc_set_tribe, STRING("キノシコワ"), STRING("キノシコワ（マイク）"))
        CALL(evt_npc_set_stay_anim, STRING("キノシコワ"))
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[115] = {
    (u32)"ガンス", 0x40000600, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"キノシコワ",
    0x40000600, 0x00000000, (u32)&init_kinosikowa, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"マッチョガンス", 0x40000600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"キノシチョフ", 0x40000600, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(evt_damage_pose)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_D_2"))
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_D_3"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_D_3"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("D_3"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_D_3"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_D_3"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_D_3"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_D_3"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(evt_kime_pose)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_Y_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_Y_1"))
        CASE_EQUAL(3)
        CALL(evt_party_set_pose, 0, STRING("Y_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_Y_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_Y_1"))
        CASE_EQUAL(6)
        CALL(evt_party_set_pose, 0, STRING("PTR_Y_1"))
        CASE_EQUAL(7)
        CALL(evt_party_set_pose, 0, STRING("PCH_Y_1"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(npc_entry)
    IF_INT_LESS(GSW(0), 128)
        CALL(evt_npc_entry, STRING("チャンピョン"), STRING("c_bred_h"))
        CALL(evt_npc_set_position, STRING("チャンピョン"), 80, 60, 0)
        CALL(evt_npc_entry, STRING("相手"), STRING("c_togenoko_b"))
        CALL(evt_npc_set_position, STRING("相手"), -80, 60, 0)
        CALL(evt_npc_set_tribe, STRING("チャンピョン"), STRING("ブレッドハート"))
        CALL(evt_npc_set_tribe, STRING("相手"), STRING("トゲノコエース"))
        CALL(evt_npc_flag_onoff, 1, STRING("チャンピョン"), 1073743360)
        CALL(evt_npc_flag_onoff, 1, STRING("相手"), 1073741824)
        CALL(evt_npc_set_ry, STRING("チャンピョン"), 270)
        CALL(evt_npc_set_ry, STRING("相手"), 90)
        RETURN
    END_IF
    SET_VALUE(LF(0), 0)
    SWITCH(GSW(0))
        CASE_BETWEEN(128, 162)
        SET_VALUE(LF(0), 1)
        CASE_LESS(164)
        CASE_DEFAULT
        SET_VALUE(LF(0), 1)
    END_SWITCH
    IF_INT_EQUAL(LF(0), 1)
        IF_INT_EQUAL(GSWF(2388), 0)
            CALL(evt_sub_random, 10000, LW(10))
            IF_INT_LESS(LW(10), 5000)
                LOOP(0)
                    CALL(evt_sub_random, 3000, LW(10))
                    SWITCH(LW(10))
                        CASE_LESS(1000)
                        SET_VALUE(LW(0), STRING("c_bomhey"))
                        SET_VALUE(LW(1), STRING("ボム兵"))
                        CASE_LESS(2000)
                        SET_VALUE(LW(0), STRING("c_kuribo"))
                        SET_VALUE(LW(1), STRING("クリボー"))
                        CASE_DEFAULT
                        SET_VALUE(LW(0), STRING("c_kmoon_wb"))
                        SET_VALUE(LW(1), STRING("モノクロクロクモーン"))
                    END_SWITCH
                    CALL(evt_sub_random, 3000, LW(10))
                    SWITCH(LW(10))
                        CASE_LESS(1000)
                        SET_VALUE(LW(2), STRING("c_bomhey"))
                        SET_VALUE(LW(3), STRING("ボム兵"))
                        CASE_LESS(2000)
                        SET_VALUE(LW(2), STRING("c_kuribo"))
                        SET_VALUE(LW(3), STRING("クリボー"))
                        CASE_DEFAULT
                        SET_VALUE(LW(2), STRING("c_kmoon_wb"))
                        SET_VALUE(LW(3), STRING("モノクロクロクモーン"))
                    END_SWITCH
                    IF_STR_NOT_EQUAL(LW(0), LW(2))
                        BREAK_LOOP
                    END_IF
                END_LOOP
            ELSE
                LOOP(0)
                    CALL(evt_sub_random, 4000, LW(10))
                    SWITCH(LW(10))
                        CASE_LESS(1000)
                        SET_VALUE(LW(0), STRING("c_chorobon"))
                        SET_VALUE(LW(1), STRING("チョロボン"))
                        CASE_LESS(2000)
                        SET_VALUE(LW(0), STRING("c_monban_t"))
                        SET_VALUE(LW(1), STRING("ダークキーパー"))
                        CASE_LESS(3000)
                        SET_VALUE(LW(0), STRING("c_togedaruma"))
                        SET_VALUE(LW(1), STRING("トゲダルマー"))
                        CASE_DEFAULT
                        SET_VALUE(LW(0), STRING("c_wanwan_a"))
                        SET_VALUE(LW(1), STRING("バーストワンワン"))
                    END_SWITCH
                    CALL(evt_sub_random, 4000, LW(10))
                    SWITCH(LW(10))
                        CASE_LESS(1000)
                        SET_VALUE(LW(2), STRING("c_chorobon"))
                        SET_VALUE(LW(3), STRING("チョロボン"))
                        CASE_LESS(2000)
                        SET_VALUE(LW(2), STRING("c_monban_t"))
                        SET_VALUE(LW(3), STRING("ダークキーパー"))
                        CASE_LESS(3000)
                        SET_VALUE(LW(2), STRING("c_togedaruma"))
                        SET_VALUE(LW(3), STRING("トゲダルマー"))
                        CASE_DEFAULT
                        SET_VALUE(LW(2), STRING("c_wanwan_a"))
                        SET_VALUE(LW(3), STRING("バーストワンワン"))
                    END_SWITCH
                    IF_STR_NOT_EQUAL(LW(0), LW(2))
                        BREAK_LOOP
                    END_IF
                END_LOOP
            END_IF
            CALL(evt_npc_entry, STRING("選手1"), LW(0))
            CALL(evt_npc_set_position, STRING("選手1"), -80, 60, 0)
            CALL(evt_npc_entry, STRING("選手2"), LW(2))
            CALL(evt_npc_set_position, STRING("選手2"), 80, 60, 0)
            CALL(evt_npc_set_tribe, STRING("選手1"), LW(1))
            CALL(evt_npc_set_tribe, STRING("選手2"), LW(3))
            CALL(evt_npc_flag_onoff, 1, STRING("選手1"), 1073743392)
            CALL(evt_npc_flag_onoff, 1, STRING("選手2"), 1073743392)
            CALL(evt_npc_set_ry, STRING("選手1"), 90)
            CALL(evt_npc_set_ry, STRING("選手2"), 270)
            IF_STR_EQUAL(LW(1), STRING("バーストワンワン"))
                CALL(evt_npc_set_anim, STRING("選手1"), STRING("WAN_A_1"))
                SET_VALUE(LW(0), 6)
                SPAWN_EVENT(burst_chain_entry)
            END_IF
            IF_STR_EQUAL(LW(3), STRING("バーストワンワン"))
                CALL(evt_npc_set_anim, STRING("選手1"), STRING("WAN_A_1"))
                SET_VALUE(LW(0), 7)
                SPAWN_EVENT(burst_chain_entry)
            END_IF
            RETURN
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(2388), 1)
        CALL(evt_tou_get_fighter_num, GSW(501), LW(0))
        SET_VALUE(LW(1), 0)
        LOOP(0)
            IF_INT_EQUAL(LW(0), LW(1))
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(GSW(501), 11)
                SWITCH(LW(1))
                    CASE_EQUAL(0)
                    SET_VALUE(LW(2), STRING("敵３"))
                    CASE_EQUAL(1)
                    SET_VALUE(LW(2), STRING("敵４"))
                    CASE_EQUAL(2)
                    SET_VALUE(LW(2), STRING("敵１"))
                    CASE_EQUAL(3)
                    SET_VALUE(LW(2), STRING("敵２"))
                    CASE_EQUAL(4)
                    SET_VALUE(LW(2), STRING("敵５"))
                    CASE_DEFAULT
                    DEBUG_PUT_MSG(STRING("おかしい"))
                    BREAK_LOOP
                END_SWITCH
            ELSE
                SWITCH(LW(1))
                    CASE_EQUAL(0)
                    SET_VALUE(LW(2), STRING("敵１"))
                    CASE_EQUAL(1)
                    SET_VALUE(LW(2), STRING("敵２"))
                    CASE_EQUAL(2)
                    SET_VALUE(LW(2), STRING("敵３"))
                    CASE_EQUAL(3)
                    SET_VALUE(LW(2), STRING("敵４"))
                    CASE_EQUAL(4)
                    SET_VALUE(LW(2), STRING("敵５"))
                    CASE_DEFAULT
                    DEBUG_PUT_MSG(STRING("おかしい"))
                    BREAK_LOOP
                END_SWITCH
            END_IF
            CALL(evt_tou_get_fighter_tribe, GSW(501), LW(1), LW(3), LW(4))
            CALL(evt_npc_entry, LW(2), LW(3))
            CALL(evt_npc_set_tribe, LW(2), LW(4))
            CALL(evt_npc_set_position, LW(2), 0, -1000, 0)
            CALL(evt_npc_flag_onoff, 1, LW(2), 1073743360)
            CALL(evt_npc_set_ry, LW(2), 270)
            BEGIN_INLINE_EVENT
                CALL(evt_npc_get_height, LW(2), LW(0))
                IF_INT_GREATER(LW(0), 50)
                    CALL(evt_npc_set_height, LW(2), 50)
                END_IF
            END_INLINE_EVENT
            BEGIN_INLINE_EVENT
                IF_INT_EQUAL(GSW(501), 2)
                    SWITCH(LW(1))
                        CASE_EQUAL(0)
                        SET_VALUE(LW(0), 0)
                        SPAWN_EVENT(burst_chain_entry)
                        CASE_EQUAL(1)
                        SET_VALUE(LW(0), 1)
                        SPAWN_EVENT(burst_chain_entry)
                    END_SWITCH
                END_IF
            END_INLINE_EVENT
            ADD(LW(1), 1)
        END_LOOP
        CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
        IF_INT_EQUAL(LW(1), 10)
            CALL(evt_npc_set_scale, STRING("敵１"), FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.0f))
            CALL(evt_npc_set_scale, STRING("敵２"), FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.0f))
            CALL(evt_npc_sound_data_set, STRING("敵１"), STRING("SFX_STG3_ISIN_MOVE1L"), STRING("SFX_STG3_ISIN_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CALL(evt_npc_sound_data_set, STRING("敵２"), STRING("SFX_STG3_ISIN_MOVE1L"), STRING("SFX_STG3_ISIN_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        END_IF
        SET_VALUE(LF(0), 0)
        CALL(evt_tou_get_ranking_28_text_4048, LW(0))
        IF_INT_EQUAL(LW(0), 6)
            CALL(evt_npc_entry, STRING("乱"), STRING("c_koopa"))
            CALL(evt_npc_set_position, STRING("乱"), 0, -1000, 0)
            CALL(evt_npc_set_tribe, STRING("乱"), STRING("クッパ"))
            CALL(evt_npc_flag_onoff, 1, STRING("乱"), 1073743360)
            CALL(evt_npc_set_ry, STRING("乱"), 270)
            CALL(evt_npc_sound_data_set, STRING("乱"), STRING("SFX_STG3_KOOPA_MOVE1L"), STRING("SFX_STG3_KOOPA_MOVE1R"), 12000, STRING("SFX_STG3_KOOPA_JUMP1"), EVTDAT_POINTER_MAX)
        ELSE
            CALL(evt_npc_entry, STRING("乱"), STRING("c_sinemon_a"))
            CALL(evt_npc_set_position, STRING("乱"), 0, -1000, 0)
            CALL(evt_npc_set_tribe, STRING("乱"), STRING("アイアンシンエモン"))
            CALL(evt_npc_flag_onoff, 1, STRING("乱"), 1073743360)
            CALL(evt_npc_set_ry, STRING("乱"), 270)
            CALL(evt_npc_set_scale, STRING("乱"), FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.0f))
            CALL(evt_npc_sound_data_set, STRING("乱"), STRING("SFX_STG3_ISIN_MOVE1L"), STRING("SFX_STG3_ISIN_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CALL(evt_npc_entry, STRING("乱２"), STRING("c_sinemon_m"))
            CALL(evt_npc_set_position, STRING("乱２"), 0, -1000, 0)
            CALL(evt_npc_set_tribe, STRING("乱２"), STRING("アイアンシンエモン2"))
            CALL(evt_npc_flag_onoff, 1, STRING("乱２"), 1073743360)
            CALL(evt_npc_set_ry, STRING("乱２"), 270)
            CALL(evt_npc_set_scale, STRING("乱２"), FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.0f))
            CALL(evt_npc_sound_data_set, STRING("乱２"), STRING("SFX_STG3_ISIN_MOVE1L"), STRING("SFX_STG3_ISIN_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_random_battle)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("m"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("乱"), 370, 0, 210)
    CALL(evt_npc_set_position, STRING("乱２"), 390, 0, 235)
    CALL(evt_msg_print, 0, STRING("stg3_tou_403"), 0, STRING("乱"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 300, 134, 732, 300, 38, 190, 1000, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("乱２"), 290, 235, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("乱２"), 200, 60, 192, 500, 30)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("乱"), 270, 210, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_jump_position_nohit, STRING("乱"), 180, 60, 167, 500, 30)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("乱２"), 75, 75, 0, FLOAT(80.0f), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("乱"), 55, 50, 0, FLOAT(80.0f), 1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 180, 517, 0, 95, -145, 0, 11)
    WAIT_FRAMES(1)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_cam3d_evt_set, 0, 130, 62, 0, 115, -22, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_403_01"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 40, 155, 320, 40, 96, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_404"), 0, STRING("乱"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_GONG1"), 0)
    WAIT_MS(500)
    CALL(evt_npc_set_battle_info, STRING("乱"), 10)
    CALL(evt_npc_battle_start, STRING("乱"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_npc_check, STRING("敵１"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("敵１"), 0, -1000, 0)
    END_IF
    CALL(evt_npc_check, STRING("敵２"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("敵２"), 0, -1000, 0)
    END_IF
    CALL(evt_npc_check, STRING("敵３"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("敵３"), 0, -1000, 0)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -50)
    ADD(LW(2), -20)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_battle_result, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_anim, STRING("乱"), STRING("SIN_K_1"))
        CALL(evt_npc_set_anim, STRING("乱２"), STRING("SIN_K_1"))
        CALL(evt_npc_flag_onoff, 0, STRING("乱"), 1536)
        CALL(evt_npc_flag_onoff, 0, STRING("乱２"), 1536)
        CALL(evt_msg_print, 0, STRING("stg3_tou_406"), 0, STRING("乱"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg3_tou_403_02"), 0, STRING("ガンス"))
        WAIT_MS(300)
    ELSE
        CALL_CHILD(evt_damage_pose)
        CALL(evt_msg_print, 0, STRING("stg3_tou_405"), 0, STRING("乱"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg3_tou_406_01"), 0, STRING("ガンス"))
        WAIT_MS(300)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_random_battle2)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("m"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("乱"), 370, 0, 210)
    CALL(evt_msg_print, 0, STRING("stg3_tou_403_04"), 0, STRING("乱"))
    WAIT_MS(300)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_KUPPA2"))
    CALL(evt_cam3d_evt_set, 300, 134, 732, 300, 38, 190, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_move_position, STRING("乱"), 270, 210, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_jump_position_nohit, STRING("乱"), 180, 60, 167, 500, 30)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.03f), 500)
    END_INLINE_EVENT
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("乱"), 55, 50, 0, FLOAT(80.0f), 1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 180, 517, 0, 95, -145, 0, 11)
    WAIT_FRAMES(1)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_cam3d_evt_set, 0, 130, 62, 0, 115, -22, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_403_05"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 15, 189, 396, 15, 116, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_403_06"), 0, STRING("乱"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_GONG1"), 0)
    WAIT_MS(500)
    CALL(evt_npc_set_battle_info, STRING("乱"), 24)
    CALL(evt_npc_battle_start, STRING("乱"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_npc_check, STRING("敵１"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("敵１"), 0, -1000, 0)
    END_IF
    CALL(evt_npc_check, STRING("敵２"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("敵２"), 0, -1000, 0)
    END_IF
    CALL(evt_npc_check, STRING("敵３"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_position, STRING("敵３"), 0, -1000, 0)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -50)
    ADD(LW(2), -20)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_battle_result, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_set_anim, STRING("乱"), STRING("KPA_Z_3"))
        CALL(evt_npc_set_autotalkpose, STRING("乱"), STRING("KPA_Z_3"), STRING("KPA_T_8"))
        CALL(evt_msg_print, 0, STRING("stg3_tou_403_08"), 0, STRING("乱"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg3_tou_403_10"), 0, STRING("ガンス"))
        WAIT_MS(300)
    ELSE
        CALL_CHILD(evt_damage_pose)
        CALL(evt_msg_print, 0, STRING("stg3_tou_403_07"), 0, STRING("乱"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg3_tou_403_09"), 0, STRING("ガンス"))
        WAIT_MS(300)
    END_IF
    SET_VALUE(LW(0), GSW(0))
    SET_VALUE(GSW(0), 178)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    SET_VALUE(GSW(0), LW(0))
    RETURN
EVT_END()

EVT_BEGIN(evt_teki_nyuujyou)
    SWITCH(LW(1))
        CASE_EQUAL(0)
        SET_VALUE(LW(3), 70)
        SET_VALUE(LW(4), 0)
        CASE_EQUAL(1)
        SET_VALUE(LW(3), 40)
        SET_VALUE(LW(4), 25)
        CASE_EQUAL(2)
        SET_VALUE(LW(3), 20)
        SET_VALUE(LW(4), -25)
        CASE_EQUAL(3)
        SET_VALUE(LW(3), 60)
        SET_VALUE(LW(4), 0)
        CASE_EQUAL(4)
        SET_VALUE(LW(3), 80)
        SET_VALUE(LW(4), -25)
    END_SWITCH
    SET_VALUE(LW(5), 450)
    ADD(LW(5), LW(3))
    CALL(evt_npc_move_position, LW(0), LW(5), LW(4), 0, FLOAT(60.0f), 1)
    WAIT_MS(1000)
    CALL(evt_npc_move_position, LW(0), 280, LW(4), 0, FLOAT(60.0f), 1)
    CALL(evt_npc_jump_position_nohit, LW(0), 250, 20, LW(4), 500, 30)
    CALL(evt_npc_jump_position_nohit, LW(0), 215, 40, LW(4), 500, 30)
    CALL(evt_npc_jump_position_nohit, LW(0), 185, 60, LW(4), 500, 30)
    SET_VALUE(LW(5), 50)
    ADD(LW(5), LW(3))
    CALL(evt_npc_move_position, LW(0), LW(5), LW(4), 0, FLOAT(60.0f), 1)
    RETURN
EVT_END()

s32 tribe_chk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 name1 = evtGetValue(event, args[0]);
    s32 name2 = evtGetValue(event, args[1]);
    NpcEntry* npc = npcNameToPtr((char*)name1);
    if (strcmp(npc->tribe->nameJp, (char*)name2) == 0) {
        evtSetValue(event, args[2], 1);
    } else {
        evtSetValue(event, args[2], 0);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_teki_nyuujyou_default)
    SWITCH(LW(1))
        CASE_EQUAL(0)
        IF_INT_EQUAL(LW(2), 1)
            SET_VALUE(LW(3), 70)
            SET_VALUE(LW(4), 0)
        ELSE
            SET_VALUE(LW(3), 50)
            SET_VALUE(LW(4), 0)
        END_IF
        CASE_EQUAL(1)
        SET_VALUE(LW(3), 90)
        SET_VALUE(LW(4), 25)
        CASE_EQUAL(2)
        SET_VALUE(LW(3), 70)
        SET_VALUE(LW(4), -25)
        CASE_EQUAL(3)
        SET_VALUE(LW(3), 110)
        SET_VALUE(LW(4), 0)
        CASE_EQUAL(4)
        SET_VALUE(LW(3), 130)
        SET_VALUE(LW(4), -25)
    END_SWITCH
    CALL(tribe_chk, LW(0), STRING("カメック緑空"), LW(5))
    IF_INT_EQUAL(LW(5), 1)
        CALL(evt_npc_get_position, LW(0), LW(6), LW(7), LW(8))
        ADD(LW(7), 25)
        CALL(evt_npc_set_position, LW(0), LW(6), LW(7), LW(8))
        CALL(evt_npc_move_position, LW(0), 280, LW(4), 0, FLOAT(60.0f), 1)
        CALL(evt_npc_jump_position_nohit, LW(0), 250, 45, LW(4), 500, 30)
        CALL(evt_npc_jump_position_nohit, LW(0), 215, 65, LW(4), 500, 30)
        CALL(evt_npc_jump_position_nohit, LW(0), 185, 85, LW(4), 500, 30)
        CALL(evt_npc_move_position, LW(0), LW(3), LW(4), 0, FLOAT(60.0f), 1)
    ELSE
        CALL(evt_npc_move_position, LW(0), 280, LW(4), 0, FLOAT(60.0f), 1)
        CALL(evt_npc_jump_position_nohit, LW(0), 250, 20, LW(4), 500, 30)
        CALL(evt_npc_jump_position_nohit, LW(0), 215, 40, LW(4), 500, 30)
        CALL(evt_npc_jump_position_nohit, LW(0), 185, 60, LW(4), 500, 30)
        CALL(evt_npc_sound_data_set, LW(0), 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        CALL(evt_npc_move_position, LW(0), LW(3), LW(4), 0, FLOAT(60.0f), 1)
        CALL(evt_npc_sound_data_reset, LW(0))
        CALL(tribe_chk, LW(0), STRING("バーストワンワン"), LW(5))
        IF_INT_EQUAL(LW(5), 1)
            SPAWN_EVENT(burst_wangwang)
        END_IF
    END_IF
    RETURN
EVT_END()

// NONMATCHING (99.22%): the four float constants land in different callee-saved registers (retail f29=3,
// f30=1, f31=10, f28=210); every other instruction matches.
s32 burst_chain(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 flag = evtGetValue(event, args[1]);
    NpcEntry* npc = npcNameToPtr((char*)name);
    NpcEntry* base;
    NpcEntry* cur;
    NpcEntry* next;
    f32 f3;
    f32 f1;
    f32 f10;
    f32 f210;
    f32 d;
    f32 t;
    char buf[32];
    s32 i;

    sprintf(buf, "鎖%d_%d", flag, 0);
    base = npcNameToPtr(buf);
    base->position = npc->position;
    f3 = 3.0f;
    f1 = 1.0f;
    f10 = 10.0f;
    f210 = 210.0f;
    for (i = 0; i < 4; i++) {
        sprintf(buf, "鎖%d_%d", flag, i);
        cur = npcNameToPtr(buf);
        sprintf(buf, "鎖%d_%d", flag, i + 1);
        next = npcNameToPtr(buf);
        next->position.y -= f3;
        d = PSVECDistance(&cur->position, &next->position);
        t = f1 - f10 / d;
        next->position.x = t * (cur->position.x - next->position.x) + next->position.x;
        next->position.y = t * (cur->position.y - next->position.y) + next->position.y;
        next->position.z = t * (cur->position.z - next->position.z) + next->position.z;
        if (next->position.x > f210 || next->position.x < -210.0f) {
            if (next->position.y < 0.0f) {
                next->position.y = 0.0f;
            }
        } else if (next->position.y < 60.0f) {
            next->position.y = 60.0f;
        }
    }
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(burst_chain_entry)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(evt_npc_entry, STRING("鎖0_0"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_1"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_2"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_3"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_4"), STRING("c_wanwan_a"))
        CALL(evt_npc_set_anim, STRING("鎖0_0"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_1"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_2"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_3"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_4"), STRING("WAN_S_1B"))
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_0"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_1"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_2"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_3"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_4"), 1074003968)
        CALL(burst_chain, STRING("敵１"), 0)
        CASE_EQUAL(1)
        CALL(evt_npc_entry, STRING("鎖1_0"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_1"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_2"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_3"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_4"), STRING("c_wanwan_a"))
        CALL(evt_npc_set_anim, STRING("鎖1_0"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_1"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_2"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_3"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_4"), STRING("WAN_S_1B"))
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_0"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_1"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_2"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_3"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_4"), 1074003968)
        CALL(burst_chain, STRING("敵２"), 1)
        CASE_EQUAL(6)
        CALL(evt_npc_entry, STRING("鎖0_0"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_1"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_2"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_3"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖0_4"), STRING("c_wanwan_a"))
        CALL(evt_npc_set_anim, STRING("鎖0_0"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_1"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_2"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_3"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖0_4"), STRING("WAN_S_1B"))
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_0"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_1"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_2"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_3"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖0_4"), 1074003968)
        CALL(burst_chain, STRING("選手1"), 0)
        CASE_EQUAL(7)
        CALL(evt_npc_entry, STRING("鎖1_0"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_1"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_2"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_3"), STRING("c_wanwan_a"))
        CALL(evt_npc_entry, STRING("鎖1_4"), STRING("c_wanwan_a"))
        CALL(evt_npc_set_anim, STRING("鎖1_0"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_1"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_2"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_3"), STRING("WAN_S_1B"))
        CALL(evt_npc_set_anim, STRING("鎖1_4"), STRING("WAN_S_1B"))
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_0"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_1"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_2"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_3"), 1074003968)
        CALL(evt_npc_flag_onoff, 1, STRING("鎖1_4"), 1074003968)
        CALL(burst_chain, STRING("選手2"), 1)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(burst_wangwang)
    SET_VALUE(LW(10), LW(0))
    CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(evt_sub_random, 360, LW(3))
        CALL(evt_sub_get_sincos, LW(3), LW(4), LW(5))
        MULTIPLY_FLOAT(LW(4), FLOAT(10.0f))
        MULTIPLY_FLOAT(LW(5), FLOAT(10.0f))
        ADD_FLOAT(LW(4), LW(0))
        ADD_FLOAT(LW(5), LW(2))
        CALL(evt_sub_random, 200, LW(6))
        ADD(LW(6), 300)
        CALL(evt_sub_random, 20, LW(7))
        ADD(LW(7), 20)
        CALL(evt_npc_flag_onoff, 0, LW(10), 1024)
        CALL(evt_npc_jump_position_nohit, LW(10), LW(4), LW(1), LW(5), LW(6), LW(7))
        CALL(evt_snd_sfxon_3d, STRING("SFX_FLD_ENM_WAN_LANDING1"), LW(4), LW(1), LW(5), 0)
    END_LOOP
    RETURN
EVT_END()

s32 snd_sfx_stop(EventEntry* event, BOOL isFirstCall) {
    psndMapChange();
    return EVT_RETURN_DONE;
}

s32 _firstattack_setting(EventEntry* event, BOOL isFirstCall) {
    FieldBattleData* fbat = fbatGetPointer();
    *(u32*)((u8*)fbat + 0x10) = 0x10000000;
    *(u32*)((u8*)fbat + 0x14) = 0x10;
    *(u32*)((u8*)fbat + 0x1C) &= ~1;
    fbatSetAttackAnnounce(0x10000000);
    fbatSetAttackAnnounceEnable();
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_match)
    SWITCH(GSW(501))
        CASE_EQUAL(19)
        IF_INT_EQUAL(GSWF(2395), 0)
            SET_VALUE(GSWF(2395), 1)
            CALL_CHILD(evt_kurikuri_opening)
        ELSE
            CALL_CHILD(evt_default_opening)
        END_IF
        CASE_EQUAL(10)
        IF_INT_EQUAL(GSWF(2397), 0)
            SET_VALUE(GSWF(2397), 1)
            CALL_CHILD(evt_shinemon_opening)
        ELSE
            CALL_CHILD(evt_default_opening)
        END_IF
        CASE_EQUAL(0)
        IF_INT_EQUAL(GSWF(2399), 0)
            SET_VALUE(GSWF(2399), 1)
            IF_INT_LESS(GSW(0), 172)
                CALL_CHILD(evt_champ_opening)
            ELSE
                CALL_CHILD(evt_champ_opening_re)
            END_IF
        ELSE
            CALL_CHILD(evt_default_opening)
        END_IF
        CASE_DEFAULT
        CALL_CHILD(evt_default_opening)
    END_SWITCH
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    IF_INT_EQUAL(GSWF(2509), 0)
        CALL(evt_npc_set_attack_mode, STRING("敵１"), 16)
        CALL(_firstattack_setting)
    END_IF
    CALL(evt_npc_set_battle_info, STRING("敵１"), LW(1))
    CALL(evt_npc_set_battle_rule, STRING("敵１"), GSW(502), GSW(503), GSW(504))
    CALL(evt_npc_battle_start, STRING("敵１"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_npc_get_battle_rule_keep_result, LW(0))
    CALL(evt_tou_set_rule_result_28_text_355C, LW(0))
    SET_VALUE(GSWF(2388), 0)
    SET_VALUE(GSWF(2389), 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -50)
    ADD(LW(2), -20)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_battle_result, LW(0))
    BEGIN_INLINE_EVENT
        IF_INT_LESS(GSW(0), 172)
            CALL(evt_npc_set_stay_anim, STRING("ガンス"))
            CALL(evt_npc_set_autotalkpose, STRING("ガンス"), STRING("S_2"), STRING("T_2"))
        ELSE
            CALL(evt_npc_set_stay_anim, STRING("キノシコワ"))
            CALL(evt_npc_set_autotalkpose, STRING("キノシコワ"), STRING("S_2"), STRING("T_2"))
        END_IF
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(0), 1)
        SWITCH(GSW(501))
            CASE_EQUAL(19)
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU2"))
            IF_INT_EQUAL(GSWF(2396), 0)
                SET_VALUE(GSWF(2396), 1)
                CALL_CHILD(evt_kurikuri_win)
            ELSE
                CALL_CHILD(evt_default_win)
            END_IF
            CASE_EQUAL(10)
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU2"))
            IF_INT_EQUAL(GSWF(2398), 0)
                SET_VALUE(GSWF(2398), 1)
                CALL_CHILD(evt_shinemon_win)
            ELSE
                CALL_CHILD(evt_default_win)
            END_IF
            CASE_EQUAL(0)
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU3"))
            IF_INT_EQUAL(GSWF(2400), 0)
                SET_VALUE(GSWF(2400), 1)
                CALL_CHILD(evt_champ_win)
            ELSE
                CALL_CHILD(evt_default_win)
            END_IF
            CASE_DEFAULT
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU2"))
            CALL_CHILD(evt_default_win)
        END_SWITCH
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        CALL(evt_tou_set_ranking_win_lose_28_text_35C0, 0, GSW(501))
        IF_INT_LESS(GSW(0), 172)
            IF_INT_GREATER_OR_EQUAL(GSW(0), 158)
                CALL(evt_tou_get_ranking_28_text_4048, LW(10))
            ELSE
                IF_INT_GREATER_OR_EQUAL(GSW(0), 138)
                    IF_INT_GREATER(LW(10), 1)
                        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
                    END_IF
                END_IF
            END_IF
            CALL(evt_tou_get_ranking_28_text_4048, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(7)
                IF_INT_EQUAL(GSWF(2417), 0)
                    CALL_CHILD(evt_random_battle)
                END_IF
                CASE_EQUAL(5)
                IF_INT_EQUAL(GSWF(2419), 0)
                    CALL_CHILD(evt_random_battle2)
                END_IF
            END_SWITCH
        ELSE
            IF_INT_EQUAL(LW(10), 1)
                IF_INT_EQUAL(GSW(501), 0)
                    SET_VALUE(LW(10), 0)
                END_IF
            END_IF
        END_IF
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU2"))
        SWITCH(GSW(501))
            CASE_EQUAL(19)
            CALL_CHILD(evt_kurikuri_lose)
            CASE_EQUAL(10)
            CALL_CHILD(evt_shinemon_lose)
            CASE_EQUAL(0)
            CALL_CHILD(evt_champ_lose)
            CASE_DEFAULT
            CALL_CHILD(evt_default_lose)
        END_SWITCH
        IF_INT_EQUAL(GSW(501), 10)
            IF_INT_LESS(GSW(505), 100)
                ADD(GSW(505), 1)
            END_IF
        END_IF
        CALL(evt_tou_set_ranking_win_lose_28_text_35C0, 1, GSW(501))
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
    END_IF
    IF_INT_LESS(GSW(0), 133)
        SET_VALUE(GSW(0), 133)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 172)
        SET_VALUE(GSWF(2383), 0)
    END_IF
    CALL(evt_fade_set_mapchange_type, 0, 2, 300, 1, 300)
    CALL(evt_snd_envoff, 512)
    CALL(snd_sfx_stop)
    SWITCH(LW(10))
        CASE_LESS(1)
        CALL(evt_bero_mapchange, STRING("tou_07"), 0)
        CASE_LESS(11)
        CALL(evt_bero_mapchange, STRING("tou_08"), 0)
        CASE_DEFAULT
        CALL(evt_bero_mapchange, STRING("tou_10"), 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(evt_kurikuri_opening)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_status_onoff, 1, STRING("ガンス"), 2)
    CALL(evt_npc_set_position, STRING("ガンス"), 0, 60, -130)
    CALL(evt_npc_set_position, STRING("敵１"), 50, 60, 0)
    CALL(evt_npc_set_position, STRING("敵２"), 90, 60, 25)
    CALL(evt_npc_set_position, STRING("敵３"), 70, 60, -25)
    CALL(evt_npc_set_position, STRING("敵４"), 110, 60, 0)
    CALL(evt_npc_set_position, STRING("敵５"), 130, 60, -25)
    CALL(evt_cam3d_evt_set, 1075, 547, 998, 0, 287, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU1"))
    CALL(evt_cam3d_evt_set, -1024, 547, 1050, 0, 287, -8, 6000, 0)
    WAIT_MS(6000)
    CALL(evt_cam3d_evt_set, 0, 127, 105, 0, 95, -145, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg3_tou_77"), 0, STRING("ガンス"))
    CALL(evt_cam3d_evt_set, 70, 137, 260, 70, 96, -70, 2000, 0)
    CALL(evt_npc_move_position, STRING("ガンス"), 10, -20, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_78"), 0, STRING("ガンス"))
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_5"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_6"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_79"), 0, STRING("敵２"))
    CALL(evt_eff_fukidashi, 1, STRING("eff"), STRING("ガンス"), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_7"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("S_2"))
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("mario"))
    CALL(evt_mario_set_pos, -550, 0, 0)
    CALL(evt_party_set_pos, 0, -560, 0, -10)
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    CALL_CHILD(w_door_open)
    CALL(evt_cam3d_evt_off, 1000, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), -565, 0, 0, 0)
        CALL(evt_sub_intpl_init, 11, -180, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL_CHILD(w_door_open)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("stg3_tou_80"), 0, 0)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -450, 0, FLOAT(120.0f))
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_party_move_pos2, 0, -280, 25, FLOAT(120.0f))
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -250, 20, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -215, 40, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -185, 60, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_move_pos2, 0, -90, -25, FLOAT(120.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -280, 0, FLOAT(120.0f))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, -250, 20, 0, 400, 1, -1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, -215, 40, 0, 400, 1, -1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP5"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, -185, 60, 0, 400, 1, -1000)
    CALL(evt_mario_mov_pos2, -55, 0, FLOAT(120.0f))
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_move_position, STRING("ガンス"), -20, -20, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_81"), 0, STRING("ガンス"))
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg3_tou_82"), 0, STRING("敵１"))
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("敵１"), 0, 0, 0, FLOAT(100.0f), 1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(10)
            ADD_FLOAT(LW(0), FLOAT(13.5f))
            CALL(evt_npc_set_rotate, STRING("敵１"), 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_sound_data_set, STRING("敵１"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, STRING("SFX_STG3_KURI_ATTACK1"), EVTDAT_POINTER_MAX)
    CALL(evt_npc_jump_position_nohit, STRING("敵１"), -30, 100, 0, 800, 100)
    RETURN
EVT_END()

EVT_BEGIN(evt_kurikuri_win)
    CALL(evt_cam3d_evt_set, 70, 138, 263, 70, 90, -8, 0, 11)
    CALL(evt_npc_set_position, STRING("敵１"), 50, 60, 0)
    CALL(evt_npc_set_position, STRING("敵２"), 90, 80, 25)
    CALL(evt_npc_set_position, STRING("敵３"), 70, 60, -25)
    CALL(evt_npc_set_position, STRING("敵４"), 110, 60, 0)
    CALL(evt_npc_set_position, STRING("敵５"), 130, 60, -25)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_rotate, STRING("敵１"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("敵２"), 0, 0, 180)
    CALL(evt_npc_set_rotate, STRING("敵３"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("敵４"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("敵５"), 0, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_anim, STRING("敵１"), STRING("KUR_K_1"))
    CALL(evt_npc_set_anim, STRING("敵２"), STRING("KUR_D_1"))
    CALL(evt_npc_set_anim, STRING("敵３"), STRING("KUR_K_1"))
    CALL(evt_npc_set_anim, STRING("敵４"), STRING("KUR_K_1"))
    CALL(evt_npc_set_anim, STRING("敵５"), STRING("KUR_K_1"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL_CHILD(evt_kime_pose)
    CALL(evt_npc_get_position, STRING("敵３"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_KURI_FURAFURA1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("敵１"), 512)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_86"), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("mario"))
    CALL(evt_cam3d_evt_set, -70, 138, 263, -70, 90, -8, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_87"), 0, STRING("ガンス"))
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_5"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_6"))
    CALL(evt_msg_select, 0, STRING("stg3_tou_87_yn"))
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_7"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("S_2"))
    CALL(evt_msg_print_add, 0, STRING("stg3_tou_88"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, STRING(""), STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_WIN_AUDIENCE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_cam3d_evt_set, 0, 850, 2160, 0, 287, -8, 10000, 0)
    WAIT_MS(7000)
    RETURN
EVT_END()

EVT_BEGIN(evt_kurikuri_lose)
    CALL_CHILD(evt_damage_pose)
    CALL(evt_npc_set_rotate, STRING("敵１"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("敵２"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("敵３"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("敵４"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("敵５"), 0, 0, 0)
    CALL(evt_npc_set_position, STRING("敵１"), 50, 60, 0)
    CALL(evt_npc_set_position, STRING("敵２"), 90, 60, 25)
    CALL(evt_npc_set_position, STRING("敵３"), 70, 60, -25)
    CALL(evt_npc_set_position, STRING("敵４"), 110, 60, 0)
    CALL(evt_npc_set_position, STRING("敵５"), 130, 60, -25)
    CALL(evt_npc_get_position, STRING("敵２"), LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, STRING(""), STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_cam3d_evt_set, 70, 138, 263, 70, 90, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_WIN_AUDIENCE1"), 0)
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(2), STRING("ガンス"))
    ELSE
        SET_VALUE(LW(2), STRING("キノシコワ"))
    END_IF
    CALL(evt_npc_set_position, LW(2), 33, 60, -83)
    CALL(evt_set_dir_to_target, LW(2), STRING("敵１"))
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    CALL(evt_msg_print_insert, 0, STRING("stg3_tou_118_61"), 0, LW(2), LW(0))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_85"), 0, STRING("敵１"))
    RETURN
EVT_END()

EVT_BEGIN(evt_shinemon_opening)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_status_onoff, 1, STRING("ガンス"), 2)
    CALL(evt_npc_set_position, STRING("ガンス"), 0, 60, -130)
    CALL(evt_cam3d_evt_set, 1075, 547, 998, 0, 287, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU1"))
    CALL(evt_cam3d_evt_set, -1024, 547, 1050, 0, 287, -8, 5000, 0)
    WAIT_MS(5000)
    CALL(evt_cam3d_evt_set, 0, 127, 105, 0, 95, -145, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_257"), 0, STRING("ガンス"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_258_00"), 0, STRING("ガンス"))
    CALL(evt_mario_set_pos, -550, 0, 0)
    CALL(evt_party_set_pos, 0, -560, 0, -10)
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    CALL_CHILD(w_door_open)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), -565, 0, 0, 0)
        CALL(evt_sub_intpl_init, 11, -180, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL_CHILD(w_door_open)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -450, 0, FLOAT(120.0f))
        WAIT_MS(1000)
        SET_VALUE(GF(1), 1)
        CALL(evt_mario_mov_pos2, -280, 0, FLOAT(120.0f))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -250, 20, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -215, 40, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -185, 60, 0, 400, 1, -1000)
        CALL(evt_mario_mov_pos2, -80, 0, FLOAT(120.0f))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_EQUAL(GF(1), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(300)
        CALL(evt_party_move_pos2, 0, -280, 25, FLOAT(120.0f))
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -250, 20, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -215, 40, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -185, 60, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_move_pos2, 0, -130, -25, FLOAT(120.0f))
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -426, 30, -195)
        CALL(evt_msg_print, 0, STRING("stg3_tou_258_01"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -379, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_258_02"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -293, 60, -180)
        CALL(evt_msg_print, 0, STRING("stg3_tou_258_03"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -272, 90, 200)
        CALL(evt_msg_print, 0, STRING("stg3_tou_258_04"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_move_position, STRING("ガンス"), -20, -20, 0, FLOAT(70.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_259"), 0, STRING("ガンス"))
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_5"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_6"))
    CALL(evt_msg_select, 0, STRING("stg3_tou_259_yn"))
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_7"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("S_2"))
    CALL(evt_msg_print_add, 0, STRING("stg3_tou_260"))
    CALL(evt_eff_fukidashi, 1, STRING("e"), STRING("ガンス"), 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("敵１"))
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    CALL_CHILD(e_door_open)
    CALL(evt_cam3d_evt_set, 475, 134, 533, 475, 38, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_position, STRING("敵１"), 550, 0, 0)
    CALL(evt_npc_set_position, STRING("敵２"), 550, 0, 0)
    SET_VALUE(LW(0), STRING("敵２"))
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 2)
    SPAWN_EVENT(evt_teki_nyuujyou)
    WAIT_MS(500)
    SET_VALUE(LW(0), STRING("敵１"))
    SET_VALUE(LW(1), 1)
    SET_VALUE(LW(2), 2)
    SPAWN_EVENT(evt_teki_nyuujyou)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), 565, 0, 0, 0)
        CALL(evt_sub_intpl_init, 11, -180, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL_CHILD(e_door_open)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_toge, 2, 426, 30, -195)
        CALL(evt_msg_print, 0, STRING("stg3_tou_261_01"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_msg_toge, 2, 379, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_261_02"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_toge, 2, 293, 30, -180)
        CALL(evt_msg_print, 0, STRING("stg3_tou_261_03"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, 50, 146, 273, 50, 96, -8, 6000, 0)
    WAIT_MS(6000)
    CALL(evt_npc_move_position, STRING("ガンス"), 0, -30, 0, FLOAT(70.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_262"), 0, STRING("ガンス"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_263"), 0, STRING("敵１"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 80, 146, 273, 80, 96, -8, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_264"), 0, STRING("敵２"))
    CALL(evt_npc_set_position, STRING("ガンス"), 0, 60, -130)
    CALL(evt_cam3d_evt_set, 0, 150, 141, 0, 100, -154, 0, 11)
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_1"))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_2"))
    CALL(evt_npc_set_autotalkpose, STRING("ガンス"), STRING("M_2"), STRING("M_3"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_116_20_01"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_GONG1"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 0, 250, 730, 0, 100, -154, 2000, 11)
    END_INLINE_EVENT
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(evt_shinemon_win)
    SET_VALUE(GSWF(2512), 1)
    CALL(evt_cam3d_evt_set, 60, 143, 263, 60, 95, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_position, STRING("敵１"), 55, 60, 0)
    CALL(evt_npc_set_position, STRING("敵２"), 95, 60, 0)
    CALL(evt_npc_set_anim, STRING("敵１"), STRING("SIN_K_1"))
    CALL(evt_npc_set_anim, STRING("敵２"), STRING("SIN_K_1"))
    CALL(evt_npc_flag_onoff, 0, STRING("敵１"), 1536)
    CALL(evt_npc_flag_onoff, 0, STRING("敵２"), 1536)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 30)
    ADD(LW(2), -30)
    CALL(evt_npc_set_position, STRING("ガンス"), LW(0), LW(1), LW(2))
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("mario"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_275"), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_276"), 0, STRING("敵２"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -65, 143, 263, -65, 95, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_277"), 0, STRING("ガンス"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, STRING(""), STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mario_set_pose, STRING("M_I_U"))
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(400)
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_MS(400)
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(400)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_PAUSE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_V_2"))
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_WIN_AUDIENCE2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 0, 850, 2160, 0, 287, -8, 10000, 0)
    WAIT_MS(7000)
    RETURN
EVT_END()

EVT_BEGIN(evt_shinemon_lose)
    CALL_CHILD(evt_damage_pose)
    CALL(evt_npc_get_position, STRING("敵１"), LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, STRING(""), STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_WIN_AUDIENCE1"), 0)
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(2), STRING("ガンス"))
    ELSE
        SET_VALUE(LW(2), STRING("キノシコワ"))
    END_IF
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    CALL(evt_msg_print_insert, 0, STRING("stg3_tou_118_61"), 0, LW(2), LW(0))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 60, 143, 263, 60, 95, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_274"), 0, STRING("敵１"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_274_01"), 0, STRING("敵２"))
    WAIT_MS(300)
    RETURN
EVT_END()

EVT_BEGIN(evt_champ_opening)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_status_onoff, 1, STRING("ガンス"), 2)
    CALL(evt_npc_set_position, STRING("ガンス"), 0, 60, 0)
    CALL(evt_npc_set_position, STRING("敵１"), 80, 60, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ガンス"), 32768)
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("敵１"))
    CALL(evt_set_dir_to_target, STRING("敵１"), STRING("ガンス"))
    CALL(evt_cam3d_evt_set, 0, 192, 620, 0, 95, -145, 0, 11)
    CALL(evt_npc_set_tribe, STRING("敵１"), STRING("ブレッドハートベルト"))
    BEGIN_INLINE_EVENT
        CALL(tou_champion_belt_28_text_2A10, STRING("敵１"))
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
    CALL(evt_snd_bgm_scope, 0, 1)
    WAIT_MS(2000)
    CALL(evt_npc_flag_onoff, 0, STRING("ガンス"), 32768)
    CALL(evt_eff_fukidashi, 1, STRING("a"), STRING("ガンス"), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_npc_set_ry, STRING("ガンス"), 270)
    WAIT_MS(500)
    CALL(evt_mario_set_pos, -550, 0, 0)
    CALL(evt_party_set_pos, 0, -560, 0, -10)
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    CALL_CHILD(w_door_open)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), -565, 0, 0, 0)
        CALL(evt_sub_intpl_init, 11, -180, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL_CHILD(w_door_open)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -450, 0, FLOAT(120.0f))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 155, 330, 0, 95, -145, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_424"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 328, 1219, 0, 130, 145, 3000, 11)
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_msg_toge, 2, -200, 30, -200)
        CALL(evt_msg_print, 0, STRING("stg3_tou_425"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_msg_toge, 2, 200, 30, 200)
        CALL(evt_msg_print, 0, STRING("stg3_tou_426"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_toge, 2, -300, 200, -400)
        CALL(evt_msg_print, 0, STRING("stg3_tou_427"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_toge, 2, 200, 200, -300)
        CALL(evt_msg_print, 0, STRING("stg3_tou_428"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(5000)
        CALL(evt_msg_toge, 2, 200, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_429"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(7000)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_430"))
    WAIT_MS(300)
    CALL(evt_party_run, 0)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -280, 0, FLOAT(60.0f))
        CALL(evt_cam3d_evt_set, 0, 192, 620, 0, 95, -145, 4000, 11)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP5"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -250, 20, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP4"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -215, 40, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -185, 60, 0, 400, 1, -1000)
        CALL(evt_mario_mov_pos2, -80, 0, FLOAT(60.0f))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        WAIT_MS(300)
        CALL(evt_party_move_pos2, 0, -280, 25, FLOAT(60.0f))
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -250, 20, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -215, 40, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -185, 60, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_move_pos2, 0, -130, -25, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -426, 30, -195)
        CALL(evt_msg_print, 0, STRING("stg3_tou_430_01"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -379, 30, 233)
        CALL(evt_msg_print, 0, STRING("stg3_tou_430_02"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -293, 30, -180)
        CALL(evt_msg_print, 0, STRING("stg3_tou_430_03"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -272, 30, 100)
        CALL(evt_msg_print, 0, STRING("stg3_tou_430_04"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -150, 100, -200)
        CALL(evt_msg_print, 0, STRING("stg3_tou_430_04"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_1"))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_2"))
    CALL(evt_npc_set_autotalkpose, STRING("ガンス"), STRING("M_2"), STRING("M_3"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_431"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_GONG1"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 0, 250, 730, 0, 100, -154, 2000, 11)
    END_INLINE_EVENT
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(evt_champ_opening_re)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_set_position, STRING("キノシコワ"), 0, 60, -130)
    CALL(evt_cam3d_evt_set, 0, 481, 1310, 0, 295, -145, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_cam3d_evt_set, 0, 281, 1310, 0, 95, -145, 3000, 0)
    WAIT_MS(3000)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 127, 105, 0, 95, -145, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_086"), 0, STRING("キノシコワ"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_087"), 0, STRING("キノシコワ"))
    CALL(evt_npc_set_position, STRING("敵１"), 550, 0, 0)
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    CALL_CHILD(e_door_open)
    CALL(evt_cam3d_evt_set, 475, 134, 533, 475, 38, -8, 0, 0)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), 565, 0, 0, 0)
        CALL(evt_sub_intpl_init, 11, -180, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL_CHILD(e_door_open)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("敵１"), 450, 0, 0, FLOAT(60.0f), 1)
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("敵１"), 310, 0, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("敵１"), 280, 20, 0, 500, 30)
        CALL(evt_npc_jump_position_nohit, STRING("敵１"), 245, 40, 0, 500, 30)
        CALL(evt_npc_jump_position_nohit, STRING("敵１"), 215, 60, 0, 500, 30)
        CALL(evt_npc_move_position, STRING("敵１"), 80, 0, 0, FLOAT(60.0f), 1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_toge, 2, 426, 30, -195)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_089"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_msg_toge, 2, 379, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_090"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_toge, 2, 293, 30, -180)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_091"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(5000)
        CALL(evt_msg_toge, 2, 200, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_092"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, 50, 146, 273, 50, 96, -8, 6000, 0)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 20, -20, 0, FLOAT(70.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_093"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_094"), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_095"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_096"), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_097"), 0, STRING("キノシコワ"))
    CALL(evt_npc_set_ry, STRING("キノシコワ"), 270)
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -130, 0, FLOAT(70.0f), 1)
    END_INLINE_EVENT
    CALL(evt_mario_set_pos, -550, 0, 0)
    CALL(evt_party_set_pos, 0, -560, 0, -10)
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    CALL_CHILD(w_door_open)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), -565, 0, 0, 0)
        CALL(evt_sub_intpl_init, 11, -180, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL_CHILD(w_door_open)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -450, 0, FLOAT(120.0f))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 328, 1219, 0, 130, 145, 3000, 11)
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_msg_toge, 2, -200, 30, -200)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_098"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_msg_toge, 2, 200, 30, 200)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_099"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_toge, 2, -300, 200, -400)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_100"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_toge, 2, 200, 200, -300)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_101"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(5000)
        CALL(evt_msg_toge, 2, 200, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_102"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(7000)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_re_103"))
    WAIT_MS(300)
    CALL(evt_party_run, 0)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -280, 0, FLOAT(60.0f))
        CALL(evt_cam3d_evt_set, 0, 192, 620, 0, 95, -145, 5000, 11)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP5"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -250, 20, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -215, 40, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP4"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -185, 60, 0, 400, 1, -1000)
        CALL(evt_mario_mov_pos2, -80, 0, FLOAT(60.0f))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_party_move_pos2, 0, -280, 25, FLOAT(60.0f))
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -250, 20, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -215, 40, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -185, 60, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_move_pos2, 0, -130, -25, FLOAT(60.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -426, 30, -195)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_110"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -379, 30, 233)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_111"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -293, 60, -180)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_112"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -272, 90, 300)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_113"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -150, 100, -200)
        CALL(evt_msg_print, 0, STRING("stg3_tou_re_114"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_2"))
    WAIT_MS(200)
    CALL(evt_npc_set_autotalkpose, STRING("キノシコワ"), STRING("S_3"), STRING("T_3"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_115"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_GONG1"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 0, 250, 730, 0, 100, -154, 2000, 11)
    END_INLINE_EVENT
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(evt_champ_win)
    CALL(tou_champion_belt_onoff_28_text_29DC, 1)
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(15), STRING("ガンス"))
        SET_VALUE(LW(14), STRING("stg3_tou_438"))
        SET_VALUE(LW(13), STRING("stg3_tou_439"))
    ELSE
        SET_VALUE(LW(15), STRING("キノシコワ"))
        SET_VALUE(LW(14), STRING("stg3_tou_re_133"))
        SET_VALUE(LW(13), STRING("stg3_tou_re_134"))
    END_IF
    CALL(evt_npc_set_position, LW(15), -20, 60, -50)
    CALL(evt_npc_set_position, STRING("敵１"), 80, 60, 0)
    CALL(evt_npc_get_position, STRING("敵１"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_BLED_DIE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("敵１"), STRING("D_3"))
    CALL(evt_npc_set_autotalkpose, STRING("敵１"), STRING("S_10"), STRING("T_10"))
    CALL(evt_cam3d_evt_set, 80, 146, 273, 80, 96, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, LW(14), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -80, 146, 273, -80, 96, -8, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, LW(13), 0, LW(15))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, STRING(""), STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_WIN_AUDIENCE1"), 0)
    CALL(evt_mario_set_pose, STRING("M_I_U"))
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(400)
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_MS(400)
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(400)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_PAUSE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_V_2"))
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_WIN_AUDIENCE2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_set, 1075, 547, 998, 0, 287, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, -1024, 547, 1050, 0, 287, -8, 5000, 0)
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(15), STRING("stg3_tou_440"))
        SET_VALUE(LW(14), STRING("stg3_tou_441"))
        SET_VALUE(LW(13), STRING("stg3_tou_442"))
        SET_VALUE(LW(12), STRING("stg3_tou_443"))
        SET_VALUE(LW(11), STRING("stg3_tou_444"))
    ELSE
        SET_VALUE(LW(15), STRING("stg3_tou_re_135"))
        SET_VALUE(LW(14), STRING("stg3_tou_re_136"))
        SET_VALUE(LW(13), STRING("stg3_tou_re_137"))
        SET_VALUE(LW(12), STRING("stg3_tou_re_138"))
        SET_VALUE(LW(11), STRING("stg3_tou_re_139"))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -300, 300, -195)
        CALL(evt_msg_print, 0, LW(15), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, -150, 100, -133)
        CALL(evt_msg_print, 0, LW(14), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, 0, 300, -180)
        CALL(evt_msg_print, 0, LW(13), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, 150, 30, -200)
        CALL(evt_msg_print, 0, LW(12), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_toge, 2, 300, 200, -200)
        CALL(evt_msg_print, 0, LW(11), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    RETURN
EVT_END()

EVT_BEGIN(evt_champ_lose)
    CALL_CHILD(evt_damage_pose)
    CALL(evt_npc_get_position, STRING("敵１"), LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, STRING(""), STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_WIN_AUDIENCE1"), 0)
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(2), STRING("ガンス"))
    ELSE
        SET_VALUE(LW(2), STRING("キノシコワ"))
    END_IF
    CALL(evt_msg_print_insert, 0, STRING("stg3_tou_118_61"), 0, LW(2), LW(0))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 40, 142, 250, 40, 96, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_tou_get_ranking_28_text_4048, LW(1))
    IF_INT_LESS(GSW(0), 172)
        IF_INT_EQUAL(GSWF(2383), 1)
            SET_VALUE(LW(0), STRING("stg3_tou_437"))
        ELSE
            IF_INT_EQUAL(LW(1), 0)
                SET_VALUE(LW(0), STRING("stg3_tou_400_03"))
            ELSE
                SET_VALUE(LW(0), STRING("stg3_tou_400_01"))
            END_IF
        END_IF
    ELSE
        IF_INT_EQUAL(GSWF(2383), 1)
            SET_VALUE(LW(0), STRING("stg3_tou_re_132"))
        ELSE
            IF_INT_EQUAL(LW(1), 0)
                SET_VALUE(LW(0), STRING("stg3_tou_re_290"))
            ELSE
                SET_VALUE(LW(0), STRING("stg3_tou_re_288"))
            END_IF
        END_IF
    END_IF
    CALL(evt_msg_print, 0, LW(0), 0, STRING("敵１"))
    WAIT_MS(300)
    RETURN
EVT_END()

EVT_BEGIN(evt_default_opening)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(0), STRING("ガンス"))
    ELSE
        SET_VALUE(LW(0), STRING("キノシコワ"))
    END_IF
    CALL(evt_npc_status_onoff, 1, LW(0), 2)
    CALL(evt_npc_set_position, LW(0), 0, 60, -130)
    CALL(evt_mario_set_pos, -350, 0, 0)
    CALL(evt_party_set_pos, 0, -390, 0, 25)
    CALL(evt_tou_get_fighter_num, GSW(501), LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_npc_set_position, STRING("敵１"), 350, 0, 0)
        CASE_EQUAL(2)
        CALL(evt_npc_set_position, STRING("敵１"), 350, 0, 0)
        CALL(evt_npc_set_position, STRING("敵２"), 390, 0, 25)
        CASE_EQUAL(3)
        CALL(evt_npc_set_position, STRING("敵１"), 350, 0, 0)
        CALL(evt_npc_set_position, STRING("敵２"), 390, 0, 25)
        CALL(evt_npc_set_position, STRING("敵３"), 370, 0, -25)
        CASE_EQUAL(4)
        CALL(evt_npc_set_position, STRING("敵１"), 350, 0, 0)
        CALL(evt_npc_set_position, STRING("敵２"), 390, 0, 25)
        CALL(evt_npc_set_position, STRING("敵３"), 370, 0, -25)
        CALL(evt_npc_set_position, STRING("敵４"), 410, 0, 0)
        CASE_EQUAL(5)
        CALL(evt_npc_set_position, STRING("敵１"), 350, 0, 0)
        CALL(evt_npc_set_position, STRING("敵２"), 390, 0, 25)
        CALL(evt_npc_set_position, STRING("敵３"), 370, 0, -25)
        CALL(evt_npc_set_position, STRING("敵４"), 410, 0, 0)
        CALL(evt_npc_set_position, STRING("敵５"), 430, 0, -25)
    END_SWITCH
    CALL(evt_cam3d_evt_set, 0, 585, 1310, 0, 400, -145, 0, 11)
    IF_INT_LESS(GSW(0), 158)
        IF_INT_EQUAL(GSW(501), 0)
            CALL(evt_npc_set_tribe, STRING("敵１"), STRING("ブレッドハートベルト"))
            BEGIN_INLINE_EVENT
                CALL(tou_champion_belt_28_text_2A10, STRING("敵１"))
            END_INLINE_EVENT
        END_IF
    END_IF
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_TOU1"))
    CALL(evt_cam3d_evt_set, 0, 331, 1310, 0, 145, -145, 3000, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -280, 0, FLOAT(120.0f))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -250, 20, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP4"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -215, 40, 0, 400, 1, -1000)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_jump_pos, -185, 60, 0, 400, 1, -1000)
        CALL(evt_mario_mov_pos2, -55, 0, FLOAT(120.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(evt_party_move_pos2, 0, -280, 25, FLOAT(120.0f))
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -250, 20, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -215, 40, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_jump_pos, 0, -185, 60, 25, 400, 1, -1000)
        WAIT_FRAMES(1)
        CALL(evt_party_move_pos2, 0, -90, -25, FLOAT(120.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_tou_get_fighter_num, GSW(501), LW(0))
        SET_VALUE(LW(3), LW(0))
        SET_VALUE(LW(4), 0)
        LOOP(0)
            IF_INT_EQUAL(LW(3), LW(4))
                BREAK_LOOP
            END_IF
            SWITCH(LW(4))
                CASE_EQUAL(0)
                SET_VALUE(LW(0), STRING("敵１"))
                CASE_EQUAL(1)
                SET_VALUE(LW(0), STRING("敵２"))
                CASE_EQUAL(2)
                SET_VALUE(LW(0), STRING("敵３"))
                CASE_EQUAL(3)
                SET_VALUE(LW(0), STRING("敵４"))
                CASE_EQUAL(4)
                SET_VALUE(LW(0), STRING("敵５"))
            END_SWITCH
            SET_VALUE(LW(1), LW(4))
            SET_VALUE(LW(2), LW(3))
            SPAWN_EVENT(evt_teki_nyuujyou_default)
            ADD(LW(4), 1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(3000)
    LOOP(0)
        IS_EVENT_RUNNING(LW(10), LW(11))
        IF_INT_EQUAL(LW(11), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_set_pos, 0, -95, 60, -25)
    CALL(evt_cam3d_evt_set, 0, 127, 105, 0, 95, -145, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    CALL(evt_tou_get_fighter_name2_28_text_3DD4, GSW(501), LW(2))
    IF_INT_EQUAL(GSW(501), 0)
        IF_INT_LESS(GSW(0), 172)
            SET_VALUE(LW(0), STRING("ガンス"))
        ELSE
            SET_VALUE(LW(0), STRING("キノシコワ"))
        END_IF
        CALL(evt_tou_get_ranking_28_text_4048, LW(1))
        SWITCH(LW(1))
            CASE_EQUAL(0)
            IF_INT_LESS(GSW(0), 172)
                SET_VALUE(LW(1), STRING("stg3_tou_378"))
            ELSE
                SET_VALUE(LW(1), STRING("stg3_tou_re_266"))
            END_IF
            CASE_EQUAL(1)
            IF_INT_LESS(GSW(0), 172)
                SET_VALUE(LW(1), STRING("stg3_tou_377"))
            ELSE
                SET_VALUE(LW(1), STRING("stg3_tou_re_265"))
            END_IF
        END_SWITCH
        CALL(evt_msg_print, 0, LW(1), 0, LW(0))
    ELSE
        IF_INT_LESS(GSW(0), 172)
            SET_VALUE(LW(3), STRING("ガンス"))
            SET_VALUE(LW(4), STRING("stg3_tou_115"))
            SET_VALUE(LW(5), STRING("stg3_tou_116_20"))
        ELSE
            SET_VALUE(LW(3), STRING("キノシコワ"))
            SET_VALUE(LW(4), STRING("stg3_tou_re_147"))
            SET_VALUE(LW(5), STRING("stg3_tou_re_149"))
        END_IF
        CALL(evt_msg_print, 0, LW(4), 0, LW(3))
        CALL(evt_msg_print_add_insert, 0, LW(5), LW(2), LW(0))
    END_IF
    CALL(evt_cam3d_evt_set, -65, 143, 263, -65, 95, -8, 0, 0)
    WAIT_FRAMES(1)
    IF_INT_LESS(GSW(0), 172)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_party_stop, 0)
            WAIT_MS(300)
            CALL(evt_sub_random, 1000, LW(0))
            IF_INT_LESS(LW(0), 500)
                CALL(evt_msg_print_party, STRING("stg3_tou_117_00"))
            ELSE
                CALL(evt_msg_print_party, STRING("stg3_tou_117_01"))
            END_IF
            WAIT_MS(300)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg3_tou_305"), 0, STRING("ガンス"))
        END_IF
    ELSE
        CALL(evt_party_stop, 0)
        WAIT_MS(300)
        CALL(evt_msg_print_party, STRING("stg3_tou_re_151"))
        WAIT_MS(300)
    END_IF
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    SWITCH(LW(1))
        CASE_EQUAL(3)
        CALL(evt_cam3d_evt_set, 65, 183, 263, 65, 135, -8, 0, 0)
        CASE_DEFAULT
        CALL(evt_cam3d_evt_set, 60, 143, 263, 60, 95, -8, 0, 0)
    END_SWITCH
    WAIT_FRAMES(1)
    CALL(evt_tou_get_ranking_28_text_4048, LW(2))
    CALL(evt_tou_get_fighter_battle_msg_28_text_3CB4, GSW(501), LW(0), LW(1))
    SWITCH(GSW(501))
        CASE_EQUAL(10)
        SWITCH(GSW(0))
            CASE_BETWEEN(138, 172)
            SET_VALUE(LW(0), STRING("stg3_tou_118_37"))
            CASE_GREATER_OR_EQUAL(172)
            IF_INT_LESS_OR_EQUAL(LW(2), 10)
                SET_VALUE(LW(0), STRING("stg3_tou_118_37"))
            END_IF
        END_SWITCH
        CASE_EQUAL(0)
        SWITCH(GSW(0))
            CASE_LESS(172)
            IF_INT_EQUAL(LW(2), 1)
                SET_VALUE(LW(0), STRING("stg3_tou_400_00"))
            END_IF
            CASE_DEFAULT
            IF_INT_EQUAL(LW(2), 1)
                SET_VALUE(LW(0), STRING("stg3_tou_re_287"))
            END_IF
        END_SWITCH
    END_SWITCH
    WAIT_MS(300)
    CALL(evt_msg_print, 0, LW(0), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 150, 141, 0, 100, -154, 0, 11)
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(0), STRING("ガンス"))
        SET_VALUE(LW(1), STRING("stg3_tou_116_20_01"))
        CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_1"))
        WAIT_MS(200)
        CALL(evt_npc_set_anim, STRING("ガンス"), STRING("M_2"))
        CALL(evt_npc_set_autotalkpose, STRING("ガンス"), STRING("M_2"), STRING("M_3"))
    ELSE
        SET_VALUE(LW(0), STRING("キノシコワ"))
        SET_VALUE(LW(1), STRING("stg3_tou_re_150"))
        CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_2"))
        WAIT_MS(200)
        CALL(evt_npc_set_autotalkpose, STRING("キノシコワ"), STRING("S_3"), STRING("T_3"))
    END_IF
    CALL(evt_msg_print, 0, LW(1), 0, LW(0))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_GONG1"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, 0, 250, 730, 0, 100, -154, 2000, 11)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

s32 confupose(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    NpcEntry* npc = npcNameToPtr((char*)evtGetValue(event, args[0]));
    evtSetValue(event, args[1], (s32)npc->tribe->confuseAnimation);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_default_win)
    CALL(evt_cam3d_evt_set, 0, 220, 533, 0, 125, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(tou_champion_belt_onoff_28_text_29DC, 1)
    CALL(evt_tou_get_fighter_num, GSW(501), LW(0))
    SET_VALUE(LW(1), 0)
    LOOP(0)
        IF_INT_EQUAL(LW(0), LW(1))
            BREAK_LOOP
        END_IF
        SWITCH(LW(1))
            CASE_EQUAL(0)
            SET_VALUE(LW(2), STRING("敵１"))
            CASE_EQUAL(1)
            SET_VALUE(LW(2), STRING("敵２"))
            CASE_EQUAL(2)
            SET_VALUE(LW(2), STRING("敵３"))
            CASE_EQUAL(3)
            SET_VALUE(LW(2), STRING("敵４"))
            CASE_EQUAL(4)
            SET_VALUE(LW(2), STRING("敵５"))
        END_SWITCH
        CALL(confupose, LW(2), LW(3))
        CALL(evt_npc_set_anim, LW(2), LW(3))
        ADD(LW(1), 1)
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, 0, STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_PAUSE2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_V_2"))
    CALL_CHILD(evt_kime_pose)
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_WIN_AUDIENCE2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(0), STRING("ガンス"))
    ELSE
        SET_VALUE(LW(0), STRING("キノシコワ"))
    END_IF
    CALL(evt_msg_print, 0, STRING("stg3_tou_118_39"), 0, LW(0))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_cam3d_evt_set, -65, 143, 263, -65, 95, -8, 0, 0)
    WAIT_FRAMES(1)
    IF_INT_LESS(GSW(0), 172)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_party_stop, 0)
            WAIT_MS(300)
            CALL(evt_sub_random, 1000, LW(0))
            IF_INT_LESS(LW(0), 500)
                CALL(evt_msg_print_party, STRING("stg3_tou_117_02"))
            ELSE
                CALL(evt_msg_print_party, STRING("stg3_tou_117_03"))
            END_IF
            WAIT_MS(300)
        END_IF
    ELSE
        CALL(evt_party_stop, 0)
        WAIT_MS(300)
        CALL(evt_msg_print_party, STRING("stg3_tou_re_153"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_default_lose)
    CALL(evt_cam3d_evt_set, 0, 220, 533, 0, 125, -8, 0, 0)
    CALL_CHILD(evt_damage_pose)
    WAIT_FRAMES(1)
    CALL(evt_npc_get_position, STRING("敵１"), LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, STRING(""), STRING("confetti"), 3, LW(0), LW(1), LW(2), 300, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_WIN_AUDIENCE1"), 0)
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    IF_INT_LESS(GSW(0), 172)
        SET_VALUE(LW(1), STRING("ガンス"))
    ELSE
        SET_VALUE(LW(1), STRING("キノシコワ"))
    END_IF
    CALL(evt_msg_print_insert, 0, STRING("stg3_tou_118_61"), 0, LW(1), LW(0))
    CALL(evt_tou_get_fighter_info_28_text_3E58, GSW(501), LW(0), LW(1))
    SWITCH(LW(1))
        CASE_EQUAL(3)
        CALL(evt_cam3d_evt_set, 65, 183, 263, 65, 135, -8, 0, 0)
        CASE_DEFAULT
        CALL(evt_cam3d_evt_set, 60, 143, 263, 60, 95, -8, 0, 0)
    END_SWITCH
    WAIT_FRAMES(1)
    CALL(evt_tou_get_fighter_battle_msg_28_text_3CB4, GSW(501), LW(0), LW(1))
    SWITCH(GSW(501))
        CASE_EQUAL(10)
        SWITCH(GSW(0))
            CASE_BETWEEN(138, 172)
            SET_VALUE(LW(0), STRING("stg3_tou_118_38"))
            CASE_GREATER_OR_EQUAL(172)
            IF_INT_LESS_OR_EQUAL(LW(2), 10)
                SET_VALUE(LW(0), STRING("stg3_tou_118_38"))
            END_IF
        END_SWITCH
    END_SWITCH
    CALL(evt_msg_print, 0, LW(1), 0, STRING("敵１"))
    WAIT_MS(300)
    RETURN
EVT_END()

static EVT_BEGIN(evt_champion)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_2_a"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_2_b"), 1)
    CALL(evt_npc_status_onoff, 1, STRING("ガンス"), 2)
    CALL(evt_npc_set_position, STRING("ガンス"), 0, 60, 0)
    CALL(evt_npc_set_ry, STRING("ガンス"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("ガンス"), 32768)
    CALL(evt_cam3d_evt_set, 0, 281, 1310, 0, 95, -145, 0, 11)
    CALL(evt_npc_set_tribe, STRING("敵１"), STRING("ブレッドハートベルト"))
    BEGIN_INLINE_EVENT
        CALL(tou_champion_belt_28_text_2A10, STRING("敵１"))
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    CALL(evt_npc_flag_onoff, 0, STRING("ガンス"), 32768)
    CALL(evt_cam3d_evt_set, 0, 155, 327, 0, 96, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_411"), 0, STRING("ガンス"))
    CALL(evt_npc_set_ry, STRING("ガンス"), 90)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_412"), 0, STRING("ガンス"))
    CALL(evt_npc_set_position, STRING("敵１"), 550, 0, 0)
    SET_FLOAT_VALUE(LW(0), FLOAT(-180.0f))
    CALL_CHILD(e_door_open)
    CALL(evt_cam3d_evt_set, 475, 134, 533, 475, 38, -8, 0, 0)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), 565, 0, 0, 0)
        CALL(evt_sub_intpl_init, 11, -180, 0, 20)
        LOOP(21)
            CALL(evt_sub_intpl_get_value)
            CALL_CHILD(e_door_open)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("敵１"), 450, 0, 0, FLOAT(60.0f), 1)
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("敵１"), 310, 0, 0, FLOAT(60.0f), 1)
        CALL(evt_npc_jump_position_nohit, STRING("敵１"), 280, 20, 0, 500, 30)
        CALL(evt_npc_jump_position_nohit, STRING("敵１"), 245, 40, 0, 500, 30)
        CALL(evt_npc_jump_position_nohit, STRING("敵１"), 215, 60, 0, 500, 30)
        CALL(evt_npc_move_position, STRING("敵１"), 80, 0, 0, FLOAT(60.0f), 1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_msg_toge, 2, 426, 30, -195)
        CALL(evt_msg_print, 0, STRING("stg3_tou_413_01"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_msg_toge, 2, 379, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_413_02"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_toge, 2, 293, 30, -180)
        CALL(evt_msg_print, 0, STRING("stg3_tou_413_03"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(5000)
        CALL(evt_msg_toge, 2, 200, 30, 133)
        CALL(evt_msg_print, 0, STRING("stg3_tou_413_04"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, 50, 155, 273, 50, 106, -8, 6000, 0)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_npc_move_position, STRING("ガンス"), 20, -20, 0, FLOAT(70.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_414"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_415"), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_416"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_417"), 0, STRING("敵１"))
    WAIT_MS(300)
    WAIT_MS(1000)
    SET_VALUE(GSWF(2455), 1)
    CALL(evt_fade_set_mapchange_type, 0, 2, 300, 1, 300)
    CALL(evt_bero_mapchange, STRING("tou_11"), STRING("nidome"))
    RETURN
EVT_END()

EVT_BEGIN(evt_champion2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_cam3d_evt_set, 50, 155, 273, 50, 106, -8, 0, 0)
    CALL(evt_npc_set_position, STRING("敵１"), 80, 60, 0)
    CALL(evt_npc_set_ry, STRING("敵１"), 270)
    CALL(evt_npc_set_position, STRING("ガンス"), 20, 60, -20)
    CALL(evt_npc_set_ry, STRING("ガンス"), 90)
    CALL(evt_npc_set_tribe, STRING("敵１"), STRING("ブレッドハートベルト"))
    BEGIN_INLINE_EVENT
        CALL(tou_champion_belt_28_text_2A10, STRING("敵１"))
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("ガンス"), 270)
    CALL(evt_msg_print, 0, STRING("stg3_tou_418"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -475, 134, 533, -475, 38, -8, 0, 0)
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_set, 20, 155, 273, 20, 106, -8, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_419"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 80, 155, 273, 80, 106, -8, 1000, 0)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_420"), 0, STRING("敵１"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 20, 155, 273, 20, 106, -8, 1000, 0)
    WAIT_MS(1000)
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("敵１"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_421"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 850, 2160, 0, 287, -8, 10000, 0)
    WAIT_MS(7000)
    CALL(evt_fade_set_mapchange_type, 0, 2, 300, 1, 300)
    CALL(evt_bero_mapchange, STRING("tou_11"), STRING("sandome"))
    RETURN
EVT_END()

char* block_data[9] = {
    "block_1", "block_2", "block_3", "block_4", "block_5", "block_6", "block_7", "block_8",
    "block_9",
};

static EVT_BEGIN(evt_shake)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("ガンス"), LW(0), LW(1), LW(2))
        ADD(LW(0), 2)
        CALL(evt_npc_set_position, STRING("ガンス"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(2)
        CALL(evt_npc_get_position, STRING("ガンス"), LW(0), LW(1), LW(2))
        ADD(LW(0), -2)
        CALL(evt_npc_set_position, STRING("ガンス"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(2)
    END_LOOP
    RETURN
EVT_END()

s32 starstone_sound_onoff(EventEntry* event, BOOL isFirstCall) {
    s32 val = evtGetValue(event, event->args[0]);
    EffectEntry* effect = itemNameToPtr("stone")->effect;
    if (effect != NULL) {
        if (val == 0) {
            *(u16*)((u8*)effect->userdata + 0x4) &= ~4;
        }
        if (val == 1) {
            *(u16*)((u8*)effect->userdata + 0x4) |= 4;
        }
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_boss_stone)
    SET_VALUE(LW(10), 0)
    CALL(evt_item_entry, STRING("stone"), 116, 0, 0, 0, 17, -1, 0)
    LOOP(0)
        CALL(evt_mapobj_get_position, STRING("kaiten_souti"), LW(0), LW(1), LW(2))
        ADD_FLOAT(LW(1), FLOAT(20.0f))
        CALL(evt_sub_get_sincos, LW(10), LW(11), LW(12))
        MULTIPLY_FLOAT(LW(12), FLOAT(10.0f))
        ADD(LW(1), LW(12))
        ADD(LW(10), FLOAT(2.5f))
        MODULO(LW(10), 360)
        IF_INT_LESS(LW(1), 50)
            CALL(starstone_sound_onoff, 0)
        ELSE
            CALL(starstone_sound_onoff, 1)
        END_IF
        CALL(evt_item_set_position, STRING("stone"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

s32 gans_white_off(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("ガンス");
    mapSetTevCallback(0xC, NULL);
    animPoseSetMaterialFlagOff(npc->poseId, 0x0C000000);
    return EVT_RETURN_DONE;
}

s32 gans_white_on(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("ガンス");
    if (isFirstCall) {
        gans_alpha = 0;
        mapSetTevCallback(0xC, callback);
        animPoseSetMaterialFlagOn(npc->poseId, 0x0C000000);
    }
    gans_alpha += 5;
    if (gans_alpha > 0xFF) {
        gans_alpha = 0xFF;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

s32 eff_pos(EventEntry* event, BOOL isFirstCall) {
    ItemEntry* item = itemNameToPtr("stone");
    EffectEntry* eff1 = effNameToPtr("eff_1");
    EffectEntry* eff2 = effNameToPtr("eff_2");
    if (item != NULL) {
        if (eff1 != NULL) {
            *(f32*)((u8*)eff1->userdata + 0x4) = item->position.x;
            *(f32*)((u8*)eff1->userdata + 0x8) = 15.0f + item->position.y;
            *(f32*)((u8*)eff1->userdata + 0xC) = item->position.z;
        }
        if (eff2 != NULL) {
            *(f32*)((u8*)eff2->userdata + 0x4) = item->position.x;
            *(f32*)((u8*)eff2->userdata + 0x8) = 15.0f + item->position.y;
            *(f32*)((u8*)eff2->userdata + 0xC) = item->position.z;
        }
    }
    return EVT_RETURN_BLOCK;
}

s32 evt_boss_stone_r(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    EffectEntry* effect = itemNameToPtr("stone")->effect;
    f32 value;
    value = evtGetFloat(event, args[0]);
    *(f32*)((u8*)effect->userdata + 0x1C) = value;
    return EVT_RETURN_DONE;
}

s32 getY(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("ガンス");
    MapObject* obj = mapGetMapObj("block_5");
    Vec pos;
    f32 y;
    mapObjGetPos("group151", &pos);
    y = obj->modelWorldMtx[1][3] + 100.0f;
    if (y > pos.y) {
        npc->position.y = y;
    } else {
        npc->position.z = 0.0f;
        npc->position.x = 0.0f;
        npc->position.y = pos.y;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (49.03%): logic is right but MWCC keeps x/y/z/d/dist/v in six callee-saved float regs
// (f26-f31) instead of retail's four (f28-f31), growing the frame from 0x80 to 0x90.
s32 item_jump(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    ItemEntry* item = itemNameToPtr("stone");
    f32 d;
    f32 dist;
    f32 v;
    f32 angle;
    Vec pos;

    if (isFirstCall) {
        d = y - item->position.y;
        dist = distABf(x, z, item->position.x, item->position.z) / 1.0f;
        v = 0.5f * dist * 0.5f + d / dist;
        angle = angleABf(item->position.x, item->position.z, x, z);
        event->userdataf[0] = dist;
        event->userdataf[1] = v;
        event->userdataf[2] = angle;
    }
    dist = event->userdataf[0];
    v = event->userdataf[1];
    angle = event->userdataf[2];
    item->position.x += sinf(6.2832f * angle / 360.0f);
    item->position.z -= cosf(6.2832f * angle / 360.0f);
    item->position.y += v;
    dist = dist - 1.0f;
    v = v - 0.5f;
    if (dist < 0.0f) {
        item->position.x = x;
        item->position.y = y;
        item->position.z = z;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        psndSFXOn_3D("stone", &pos);
        return EVT_RETURN_YIELD;
    }
    event->userdataf[0] = dist;
    event->userdataf[1] = v;
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(evt_boss)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("group34"), 1)
    CALL(evt_hitobj_onoff, STRING("a_kai_right"), 0, 0)
    SET_VALUE(LW(0), STRING("e_bero"))
    CALL_CHILD(bero_case_switch_off)
    CALL(evt_npc_set_position, STRING("ガンス"), 0, 60, 0)
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("mario"))
    CALL(evt_npc_set_tribe, STRING("ガンス"), STRING("ガンス"))
    SPAWN_EVENT_GET_ID(PTR(evt_boss_stone), GW(10))
    WAIT_FRAMES(2)
    SET_VALUE(GW(12), 1000)
    BEGIN_INLINE_EVENT_GET_ID(GW(11))
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 0, 0, 360, GW(12))
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_boss_stone_r, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    LOOP(0)
        CALL(evt_mario_get_name_hitobj_ride, LW(9))
        IF_STR_EQUAL(LW(9), STRING("a_butai"))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set, 0, 197, 484, 0, 110, -8, 2000, 11)
    CALL(evt_mario_mov_pos2, -75, 0, FLOAT(60.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_set_dir, 0, 90, 200)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG3_GANCE2"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_463"), 0, STRING("ガンス"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG3_GROUND_SHAKE1"), LW(10))
        CALL(evt_cam_shake, 4, FLOAT(0.003f), FLOAT(0.003f), 13000)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("m"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_GREATER(LW(1), 50)
        CALL(evt_eff_fukidashi, 3, STRING("p"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    CALL(evt_cam3d_evt_set, 0, 242, 821, 0, 96, -8, 3000, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -180, 0, FLOAT(60.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
        CALL(evt_party_stop, 0)
    END_INLINE_EVENT
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_jump_pos, -150, 60, 0, 500, 0, -1000)
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("butai_2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("butai_1"), 1)
    CALL(evt_map_playanim, STRING("tyuubu"), 1, 0)
    CALL(evt_map_playanim, STRING("guruguru"), 1, 0)
    CALL(evt_map_playanim, STRING("hakkou"), 1, 0)
    CALL(evt_map_playanim, STRING("souti"), 0, 0)
    CALL(evt_map_playanim, STRING("comp"), 1, 0)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_map_pauseanim, 0, STRING("souti"))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_ABSORB_DEVICE_UP1"), 0, 60, 0, LW(10))
        WAIT_MS(2000)
        CALL(evt_map_replayanim, 0, STRING("souti"))
        CALL(evt_map_checkanim, STRING("souti"), LW(0), LW(1))
        SUBTRACT(LW(1), 100)
        WAIT_MS(LW(1))
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_ABSORB_DEVICE_UP_STOP1"), 0, 60, 0, 0)
    END_INLINE_EVENT
    SET_INT_READ_ADDR(PTR(block_data))
    LOOP(9)
        READ_INT(LW(5))
        BEGIN_INLINE_EVENT
            CALL(evt_mapobj_get_position, LW(5), LW(0), LW(1), LW(2))
            LOOP(0)
                CALL(evt_mapobj_get_position, LW(5), LW(10), LW(11), LW(12))
                IF_INT_NOT_EQUAL(LW(1), LW(11))
                    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_RING_DOWN1"), LW(10), LW(11), LW(12), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_INLINE_EVENT
    END_LOOP
    CALL(evt_npc_flag_onoff, 0, STRING("ガンス"), 1024)
    LOOP(0)
        CALL(evt_map_checkanim, STRING("souti"), LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        CALL(getY)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_flag_onoff, 1, STRING("ガンス"), 1024)
    CALL(evt_hitobj_onoff, STRING("a_souti"), 1, 1)
    WAIT_MS(1000)
    CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_GREATER(LW(1), 50)
        CALL(evt_eff_fukidashi, 3, STRING("p"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 305, 384, 0, 325, -15, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print_party, STRING("stg3_tou_464"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 155, 288, 0, 105, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_465"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_I_Y"))
    CALL(evt_party_set_pos, 0, -180, 60, 0)
    CALL(evt_party_stop, 0)
    WAIT_FRAMES(1)
    CALL(evt_cam3d_evt_set, -150, 155, 288, -150, 105, -8, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_465_01"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 155, 288, 0, 105, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_466"), 0, STRING("ガンス"))
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("ガンス"), STRING("P_1"))
        WAIT_MS(333)
        CALL(evt_npc_set_anim, STRING("ガンス"), STRING("P_2"))
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(GF(10), 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_ABSORB_DEVICE1"), 0, 60, 0, LW(9))
        LOOP(0)
            CALL(evt_sub_intpl_msec_init, 11, 0, 2000, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_snd_sfx_pit, LW(9), LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
            CALL(evt_sub_intpl_msec_init, 11, 2000, 0, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_snd_sfx_pit, LW(9), LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
            IF_INT_EQUAL(GF(10), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, 90, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_snd_sfx_vol, LW(9), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_snd_sfxoff, LW(9))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 100, 800, 5000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
        CALL(evt_map_set_playrate, STRING("tyuubu"), LW(0))
        CALL(evt_map_set_playrate, STRING("hakkou"), LW(0))
        CALL(evt_map_set_playrate, STRING("guruguru"), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
    END_INLINE_EVENT
    SPAWN_EVENT_GET_ID(PTR(evt_shake), LW(9))
    CALL(evt_cam3d_evt_set, 0, 142, 215, 0, 105, -8, 5000, 11)
    WAIT_MS(5000)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_get_position, STRING("ガンス"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_GANSU_BIG1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("H_1"))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 100, 200, 300)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_npc_set_scale, STRING("ガンス"), LW(0), LW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 0, 170, 385, 0, 135, -11, 300, 11)
    WAIT_MS(300)
    CALL(gans_white_on)
    CALL(evt_paper_entry, STRING("OFF_d_bom"))
    CALL(evt_npc_get_position, STRING("ガンス"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("マッチョガンス"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("マッチョガンス"), 270)
    CALL(evt_npc_set_width, STRING("マッチョガンス"), 60)
    CALL(evt_npc_set_height, STRING("マッチョガンス"), 80)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_GANSU_BIG2"), 0)
    CALL(evt_npc_set_paper, STRING("ガンス"), STRING("OFF_d_bom"))
    CALL(evt_npc_set_paper_anim, STRING("ガンス"), STRING("A_1"))
    WAIT_MS(2500)
    CALL(evt_npc_set_position, STRING("ガンス"), 0, -1000, 0)
    CALL(evt_npc_clear_paper, STRING("ガンス"))
    CALL(gans_white_off)
    DELETE_EVENT(LW(9))
    CALL(evt_msg_print, 0, STRING("stg3_tou_466_01"), 0, STRING("マッチョガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -150, 155, 288, -150, 105, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pose, STRING("M_I_N"))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 300, 0, -1000)
    CALL(evt_msg_print_party, STRING("stg3_tou_467"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 170, 385, 0, 135, -11, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_468"), 0, STRING("マッチョガンス"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 210, 852, 0, 135, -12, 1000, 11)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_npc_set_battle_info, STRING("マッチョガンス"), 23)
    CALL(evt_npc_battle_start, STRING("マッチョガンス"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_ABSORB_DEVICE2"), LW(15))
    CALL(evt_snd_sfx_vol, LW(15), 38)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_party_stop, 0)
    CALL(evt_npc_get_position, STRING("マッチョガンス"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガンス"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("マッチョガンス"), 0, -1000, 0)
    CALL(evt_npc_set_scale, STRING("ガンス"), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(evt_npc_set_anim, STRING("ガンス"), STRING("D_1"))
    CALL(evt_npc_set_autotalkpose, STRING("ガンス"), STRING("D_1"), STRING("D_2"))
    CALL(evt_cam3d_evt_set, 0, 140, 385, 0, 105, -11, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473"), 0, STRING("ガンス"))
    WAIT_MS(300)
    SET_VALUE(GF(10), 1)
    CALL(evt_cam3d_evt_set, -130, 140, 385, -130, 105, -11, 700, 11)
    WAIT_MS(700)
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_EVT_ORGEL1"), 3000)
    END_INLINE_EVENT
    CALL(evt_npc_set_position, STRING("キノシコワ"), -325, 0, 0)
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_01"), 0, STRING("キノシコワ"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("m"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("p"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_cam3d_evt_set, -320, 95, 287, -320, 40, -8, 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_02"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -160, 150, 288, -160, 100, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg3_tou_473_03"))
    CALL(evt_cam3d_evt_set, -200, 222, 682, -200, 100, -8, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_04"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -320, 95, 287, -320, 40, -8, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_05"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -160, 150, 288, -160, 100, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg3_tou_473_06"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -320, 95, 287, -320, 40, -8, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -25, 197, 574, -25, 105, -8, 3000, 11)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -280, 20, 0, FLOAT(60.0f), 1)
    CALL(evt_npc_jump_position_nohit, STRING("キノシコワ"), -250, 20, 20, 500, 30)
    CALL(evt_npc_jump_position_nohit, STRING("キノシコワ"), -215, 40, 20, 500, 30)
    CALL(evt_npc_jump_position_nohit, STRING("キノシコワ"), -185, 60, 20, 500, 30)
    CALL(evt_npc_move_position, STRING("キノシコワ"), -135, 80, 0, FLOAT(60.0f), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 300, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 300)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("キノシコワ"), -50, 75, 0, FLOAT(60.0f), 1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07_01"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07_02"), 0, STRING("ガンス"))
    WAIT_MS(300)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07_03"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -160, 150, 288, -160, 100, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg3_tou_473_07_04"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(eff_pos)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 30)
    ADD(LW(1), 30)
    CALL(evt_eff64, STRING("sl"), STRING("sunlight_n64"), 1, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(300)
    CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1500)
    CALL(evt_msg_print_party, STRING("stg3_tou_473_07_04_01"))
    CALL(evt_mapobj_get_position, STRING("kaiten_souti"), LW(0), LW(1), LW(2))
    ADD_FLOAT(LW(1), FLOAT(40.0f))
    CALL(evt_eff64, STRING("eff_2"), STRING("pokopi_pcharge_n64"), 11, LW(0), LW(1), LW(2), FLOAT(1.0f), 270, 0, 0, 0, 0, 0, 0)
    WAIT_MS(300)
    CALL(evt_snd_sfxon, STRING("SFX_STG3_STARSTONE_ROUND1"), 0)
    CALL(evt_snd_sfx_vol, LW(15), 161)
    CALL(evt_cam3d_evt_set, 0, 400, 357, 0, 325, -8, 0, 11)
    CALL(evt_eff_delete, STRING("sl"))
    WAIT_FRAMES(1)
    SET_VALUE(GW(12), 650)
    WAIT_MS(600)
    SET_VALUE(GW(12), 400)
    WAIT_MS(350)
    SET_VALUE(GW(12), 250)
    WAIT_MS(250)
    CALL(evt_mapobj_get_position, STRING("kaiten_souti"), LW(0), LW(1), LW(2))
    ADD_FLOAT(LW(1), FLOAT(40.0f))
    CALL(evt_eff64, STRING("eff_1"), STRING("pokopi_pcharge_n64"), 4, LW(0), LW(1), LW(2), FLOAT(1.0f), 180, 0, 0, 0, 0, 0, 0)
    WAIT_MS(2000)
    CALL(evt_npc_set_scale, STRING("キノシチョフ"), FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
    CALL(evt_mapobj_get_position, STRING("kaiten_souti"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("キノシチョフ"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 0, STRING("キノシチョフ"), 1024)
        CALL(evt_sub_intpl_msec_init, 11, 0, 1800, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_npc_set_rotate, STRING("キノシチョフ"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_npc_flag_onoff, 1, STRING("キノシチョフ"), 1024)
        CALL(evt_npc_set_rotate, STRING("キノシチョフ"), FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 100, 300)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_npc_set_scale, STRING("キノシチョフ"), LW(0), LW(0), LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("キノシチョフ"), 100, 60, 75, 800, 170)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 127)
        LOOP(10)
            ADD(LW(0), -8)
            CALL(evt_snd_sfx_vol, LW(15), LW(0))
            WAIT_MS(100)
        END_LOOP
        CALL(evt_snd_sfx_vol, LW(15), 38)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 100, 166, 536, 100, 90, -8, 1000, 11)
    WAIT_MS(1000)
    WAIT_MS(2000)
    WAIT_MS(400)
    CALL(evt_cam3d_evt_set, 100, 133, 339, 100, 85, -8, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07_05"), 0, STRING("キノシチョフ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -75, 133, 339, -75, 85, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07_06"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("キノシコワ"), 75, 75, 0, FLOAT(100.0f), 1)
    END_INLINE_EVENT
    WAIT_MS(200)
    CALL(evt_cam3d_evt_set, 88, 133, 339, 88, 85, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_set_dir_to_target, STRING("キノシチョフ"), STRING("キノシコワ"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07_07"), 0, STRING("キノシチョフ"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_07_08"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -160, 150, 288, -160, 100, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg3_tou_473_07_09"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 0, 356, 1016, 25, 180, 32, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 75, 75, 0, FLOAT(60.0f), 1)
    WAIT_MS(333)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_08"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(evt_snd_sfxon, STRING("SFX_STG3_STARSTONE_ROUND2"), 0)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("A_1"))
    WAIT_MS(500)
    CALL(evt_map_pauseanim, 0, STRING("comp"))
    CALL(evt_sub_intpl_msec_init, 0, 800, 0, 5000)
    LABEL(10)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
    CALL(evt_map_set_playrate, STRING("tyuubu"), LW(0))
    CALL(evt_map_set_playrate, STRING("hakkou"), LW(0))
    CALL(evt_map_set_playrate, STRING("guruguru"), LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(10)
    END_IF
    CALL(evt_map_pauseanim, 0, STRING("tyuubu"))
    CALL(evt_map_pauseanim, 0, STRING("guruguru"))
    CALL(evt_map_pauseanim, 0, STRING("hakkou"))
    DELETE_EVENT(GW(11))
    DELETE_EVENT(GW(10))
    CALL(evt_boss_stone_r, FLOAT(0.0f))
    CALL(item_jump, FLOAT(0.0f), FLOAT(200.0f), FLOAT(60.0f))
    CALL(item_jump, FLOAT(0.0f), FLOAT(60.0f), FLOAT(100.0f))
    CALL(item_jump, FLOAT(0.0f), FLOAT(60.0f), FLOAT(100.0f))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -160, 150, 288, -160, 100, -8, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_473_09"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 88, 133, 339, 88, 85, -8, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_473_10"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -160, 150, 288, -160, 100, -8, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg3_tou_473_11"))
    WAIT_MS(300)
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_party_run, 0)
    CALL(evt_cam3d_evt_set, 0, 200, 584, 0, 100, -8, 2000, 11)
    CALL(evt_mario_mov_pos2, 0, 75, FLOAT(60.0f))
    SET_VALUE(GSW(0), 164)
    SET_VALUE(LW(0), 3)
    SET_VALUE(LW(1), STRING("stone"))
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_bgmoff, 513)
    CALL_CHILD(evt_sub_starstone)
    RETURN
EVT_END()

EVT_BEGIN(evt_watch_match)
    IF_INT_LESS(GSW(0), 172)
        CALL(evt_npc_set_position, STRING("ガンス"), 0, 60, -130)
    ELSE
        CALL(evt_npc_set_position, STRING("キノシコワ"), 0, 60, -130)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("選手1"), 32)
    LOOP(0)
        CALL(evt_sub_random, 3, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_npc_move_position, STRING("選手1"), -30, 0, 0, FLOAT(200.0f), 0)
            CALL(evt_npc_jump_position_nohit, STRING("選手1"), 20, 60, 0, 500, 30)
            CALL(evt_npc_jump_position_nohit, STRING("選手1"), 80, 70, 0, 500, 60)
            CALL(evt_eff, 0, STRING("hit"), 0, 80, 80, 0, FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FIGHTING3"), 80, 80, 0, 0)
            CALL(evt_npc_jump_position_nohit, STRING("選手1"), 60, 60, 0, 500, 30)
            CALL(evt_npc_jump_position_nohit, STRING("選手1"), 30, 60, 0, 300, 20)
            CALL(evt_npc_move_position, STRING("選手1"), -80, 0, 0, FLOAT(200.0f), 0)
            CALL(evt_npc_move_position, STRING("選手2"), 30, 0, 0, FLOAT(200.0f), 0)
            CALL(evt_npc_jump_position_nohit, STRING("選手2"), -20, 60, 0, 500, 30)
            CALL(evt_npc_jump_position_nohit, STRING("選手2"), -80, 70, 0, 500, 60)
            CALL(evt_eff, 0, STRING("hit"), 0, -80, 80, 0, FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FIGHTING3"), -80, 80, 0, 0)
            CALL(evt_npc_jump_position_nohit, STRING("選手2"), -60, 60, 0, 500, 30)
            CALL(evt_npc_jump_position_nohit, STRING("選手2"), -30, 60, 0, 300, 20)
            CALL(evt_npc_move_position, STRING("選手2"), 80, 0, 0, FLOAT(200.0f), 0)
            CASE_EQUAL(1)
            BEGIN_INLINE_EVENT_GET_ID(LW(12))
                CALL(evt_npc_move_position, STRING("選手1"), -10, 0, 300, 0, 0)
            END_INLINE_EVENT
            CALL(evt_npc_move_position, STRING("選手2"), 10, 0, 300, 0, 0)
            CALL(evt_eff, 0, STRING("hit"), 1, 0, 80, 0, FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FIGHTING3"), 0, 80, 0, 0)
            WAIT_MS(100)
            LOOP(0)
                IS_EVENT_RUNNING(LW(12), LW(13))
                IF_INT_EQUAL(LW(13), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_random, 4, LW(10))
            ADD(LW(10), 1)
            LOOP(LW(10))
                CALL(evt_sub_random, 100, LW(0))
                IF_INT_LESS(LW(0), 50)
                    BEGIN_INLINE_EVENT_GET_ID(LW(12))
                        CALL(evt_npc_move_position, STRING("選手1"), -140, 0, 800, 0, 0)
                    END_INLINE_EVENT
                    CALL(evt_npc_move_position, STRING("選手2"), -120, 0, 800, 0, 0)
                    CALL(evt_eff, 0, STRING("hit"), 0, -130, 80, 0, FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FIGHTING3"), -130, 80, 0, 0)
                ELSE
                    BEGIN_INLINE_EVENT_GET_ID(LW(12))
                        CALL(evt_npc_move_position, STRING("選手1"), 120, 0, 800, 0, 0)
                    END_INLINE_EVENT
                    CALL(evt_npc_move_position, STRING("選手2"), 140, 0, 800, 0, 0)
                    CALL(evt_eff, 0, STRING("hit"), 0, 130, 80, 0, FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FIGHTING3"), 130, 80, 0, 0)
                END_IF
                WAIT_MS(100)
                LOOP(0)
                    IS_EVENT_RUNNING(LW(12), LW(13))
                    IF_INT_EQUAL(LW(13), 0)
                        BREAK_LOOP
                    END_IF
                    WAIT_FRAMES(1)
                END_LOOP
            END_LOOP
            BEGIN_INLINE_EVENT_GET_ID(LW(12))
                CALL(evt_npc_jump_position_nohit, STRING("選手1"), -80, 60, 0, 500, 30)
            END_INLINE_EVENT
            CALL(evt_npc_jump_position_nohit, STRING("選手2"), 80, 60, 0, 500, 30)
            WAIT_MS(100)
            LOOP(0)
                IS_EVENT_RUNNING(LW(12), LW(13))
                IF_INT_EQUAL(LW(13), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            CASE_DEFAULT
            BEGIN_INLINE_EVENT_GET_ID(LW(12))
                CALL(evt_npc_move_position, STRING("選手1"), -40, 0, 500, 0, 0)
            END_INLINE_EVENT
            CALL(evt_npc_move_position, STRING("選手2"), 40, 0, 500, 0, 0)
            WAIT_MS(100)
            LOOP(0)
                IS_EVENT_RUNNING(LW(12), LW(13))
                IF_INT_EQUAL(LW(13), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            BEGIN_INLINE_EVENT_GET_ID(LW(12))
                CALL(evt_npc_jump_position_nohit, STRING("選手1"), -10, 100, 0, 400, 30)
            END_INLINE_EVENT
            CALL(evt_npc_jump_position_nohit, STRING("選手2"), 10, 100, 0, 400, 30)
            CALL(evt_eff, 0, STRING("hit"), 2, 0, 120, 0, FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_FIGHTING3"), 0, 120, 0, 0)
            WAIT_MS(100)
            LOOP(0)
                IS_EVENT_RUNNING(LW(12), LW(13))
                IF_INT_EQUAL(LW(13), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            BEGIN_INLINE_EVENT_GET_ID(LW(12))
                CALL(evt_npc_jump_position_nohit, STRING("選手1"), -80, 60, 0, 500, 30)
            END_INLINE_EVENT
            CALL(evt_npc_jump_position_nohit, STRING("選手2"), 80, 60, 0, 500, 30)
            WAIT_MS(100)
            LOOP(0)
                IS_EVENT_RUNNING(LW(12), LW(13))
                IF_INT_EQUAL(LW(13), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_SWITCH
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku1"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku1"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_119"), 0, STRING("kyaku1"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_119"), 0, STRING("kyaku1"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_120"), 0, STRING("kyaku1"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_121"), 0, STRING("kyaku1"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku2"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku2"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_122"), 0, STRING("kyaku2"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_122"), 0, STRING("kyaku2"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_123"), 0, STRING("kyaku2"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_124"), 0, STRING("kyaku2"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku3"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku3"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_125"), 0, STRING("kyaku3"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_125"), 0, STRING("kyaku3"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_126"), 0, STRING("kyaku3"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_127"), 0, STRING("kyaku3"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_4)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku4"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku4"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_128"), 0, STRING("kyaku4"))
        CALL(evt_msg_select, 0, STRING("stg3_tou_128_01"))
        CALL(evt_msg_print_add, 0, STRING("stg3_tou_128_02"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_128"), 0, STRING("kyaku4"))
            CALL(evt_msg_select, 0, STRING("stg3_tou_128_01"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_128_02"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_129"), 0, STRING("kyaku4"))
            CALL(evt_msg_select, 0, STRING("stg3_tou_129_01"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_129_02"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_130"), 0, STRING("kyaku4"))
            CALL(evt_msg_select, 0, STRING("stg3_tou_130_01"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_130_02"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_5)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku1"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku1"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_131"), 0, STRING("kyaku1"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_131"), 0, STRING("kyaku1"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_132"), 0, STRING("kyaku1"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_133"), 0, STRING("kyaku1"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_6)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku2"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku2"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_134"), 0, STRING("kyaku2"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_134"), 0, STRING("kyaku2"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_135"), 0, STRING("kyaku2"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_136"), 0, STRING("kyaku2"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_7)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku3"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku3"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_137"), 0, STRING("kyaku3"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_137"), 0, STRING("kyaku3"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_138"), 0, STRING("kyaku3"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_139"), 0, STRING("kyaku3"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_kyaku_8)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_set_dir_to_target, STRING("kyaku4"), STRING("mario"))
    CALL(evt_mario_set_dir_npc, STRING("kyaku4"))
    SWITCH(GSW(0))
        CASE_LESS(131)
        CALL(evt_msg_print, 0, STRING("stg3_tou_140"), 0, STRING("kyaku4"))
        CALL(evt_msg_select, 0, STRING("stg3_tou_140_01"))
        CALL(evt_msg_print_add, 0, STRING("stg3_tou_140_02"))
        CASE_DEFAULT
        CALL(evt_tou_get_ranking_28_text_4048, LW(10))
        SWITCH(LW(10))
            CASE_GREATER(10)
            CALL(evt_msg_print, 0, STRING("stg3_tou_140"), 0, STRING("kyaku4"))
            CALL(evt_msg_select, 0, STRING("stg3_tou_140_01"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_140_02"))
            CASE_GREATER(0)
            CALL(evt_msg_print, 0, STRING("stg3_tou_141"), 0, STRING("kyaku4"))
            CALL(evt_msg_select, 0, STRING("stg3_tou_141_01"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_141_02"))
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("stg3_tou_142"), 0, STRING("kyaku4"))
            CALL(evt_msg_select, 0, STRING("stg3_tou_142_01"))
            CALL(evt_msg_print_add, 0, STRING("stg3_tou_142_02"))
        END_SWITCH
    END_SWITCH
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// NONMATCHING (2.38%): not decompiled yet. The `screen_wp->field_0x4 == 0` early-out is the only part
// recovered; the GX tev/tex copy setup is still TODO.
void screen_tev(void) {
    if (screen_wp->field_0x4 == 0) {
        return;
    }
    if (screen_wp->field_0x24 != 0 && screen_wp->field_0x2C != NULL) {
        return;
    }
}

static s32 screen_evt(EventEntry* event, BOOL isFirstCall) {
    dispEntry(8, 1000.0f, 0, screen_capture, NULL);
    return EVT_RETURN_BLOCK;
}

s32 screen_init(EventEntry* event, BOOL isFirstCall) {
    u32 size;
    f32 negOne;
    f32 zero;
    f32 one;

    size = GXGetTexBufferSize(0x260, 0x1E0, 0, 0, 0);
    screen_wp = _mapAlloc(0x30);
    screen_wp->field_0x0 = _mapAlloc(size);
    one = 1.0f;
    zero = 0.0f;
    negOne = -1.0f;
    screen_wp->field_0x4 = 0;
    screen_wp->field_0xC = one;
    screen_wp->field_0x8 = one;
    screen_wp->field_0x14 = zero;
    screen_wp->field_0x10 = zero;
    screen_wp->field_0x1C = zero;
    screen_wp->field_0x18 = zero;
    screen_wp->field_0x20 = 0;
    screen_wp->field_0x28 = negOne;
    screen_wp->field_0x2C = _mapAlloc(GXGetTexBufferSize(0x40, 0x40, 1, 0, 0));
    if (evtGetValue(event, GSW(0)) == 0xA3) {
        screen_wp->field_0x24 = 1;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(tou_03_init_evt)
    SWITCH(GSW(0))
        CASE_EQUAL(163)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
        CASE_DEFAULT
        BEGIN_INLINE_EVENT
            CALL(evt_snd_envoff, 512)
            WAIT_MS(1000)
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU3"))
        END_INLINE_EVENT
    END_SWITCH
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_NOT_EQUAL(LW(0), STRING("s_bero_2"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("ue"), 1)
    END_IF
    CALL(evt_map_playanim, STRING("light_coloer"), 1, 0)
    CALL(evt_map_playanim, STRING("light_l"), 1, 0)
    CALL(evt_map_playanim, STRING("light_r"), 1, 0)
    IF_INT_NOT_EQUAL(GSW(0), 163)
        CALL_CHILD(evt_kyaku)
        WAIT_FRAMES(1)
    END_IF
    CALL(screen_init)
    BEGIN_INLINE_EVENT
        CALL(screen_evt)
    END_INLINE_EVENT
    WAIT_FRAMES(1)
    CALL(evt_map_set_tevcallback, 1, PTR(screen_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("monittor"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("monittor"), 256)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("butai_2"), 1)
    CALL(evt_hitobj_onoff, STRING("a_souti"), 1, 0)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        RETURN
    END_IF
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL_CHILD(npc_entry)
    SWITCH(GSW(0))
        CASE_LESS(128)
        SET_VALUE(LW(0), PTR(bero_data))
        CALL(evt_bero_get_info)
        CALL_CHILD(bero_case_entry)
        CASE_DEFAULT
        SET_VALUE(LW(0), PTR(bero_data))
        CALL(evt_bero_get_info)
        CALL_CHILD(evt_bero_info_run)
    END_SWITCH
    SWITCH(GSW(0))
        CASE_LESS(128)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
        CALL(evt_snd_bgmon, 160, 0)
        SPAWN_EVENT(evt_tougi)
        CASE_EQUAL(163)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_2_a"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_2_b"), 1)
        SET_VALUE(LW(0), STRING("s_bero_1"))
        CALL_CHILD(bero_case_switch_off)
        SPAWN_EVENT(evt_boss)
    END_SWITCH
    IF_INT_EQUAL(GSWF(2450), 1)
        IF_INT_EQUAL(GSWF(2451), 0)
            CALL(evt_mario_kill_party, 0)
            CALL(evt_mario_kill_party, 1)
        END_IF
    END_IF
    IF_INT_LESS(GSW(0), 172)
        IF_INT_EQUAL(GSW(501), 0)
            IF_INT_EQUAL(GSWF(2383), 1)
                IF_INT_EQUAL(GSWF(2455), 0)
                    CALL(evt_snd_bgmoff, 512)
                    SPAWN_EVENT(evt_champion)
                ELSE
                    SPAWN_EVENT(evt_champion2)
                END_IF
                RETURN
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(2388), 1)
        CALL(evt_mario_set_pos, 0, 0, 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_party_set_pos, 1, LW(0), LW(1), LW(2))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_2_a"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("door_2_b"), 1)
        CALL(evt_tou_get_ranking_28_text_4048, LW(0))
        SET_VALUE(GSW(506), LW(0))
        SPAWN_EVENT(evt_match)
    ELSE
        SWITCH(GSW(0))
            CASE_BETWEEN(128, 162)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
            CALL(evt_snd_bgmon, 160, 0)
            SPAWN_EVENT(evt_watch_match)
            CASE_LESS(164)
            CASE_DEFAULT
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
            CALL(evt_snd_bgmon, 160, 0)
            SPAWN_EVENT(evt_watch_match)
        END_SWITCH
    END_IF
    IF_INT_NOT_EQUAL(GSW(0), 163)
        CALL(evt_hitobj_onoff, STRING("a_kyaku_A"), 1, 1)
        CALL(evt_hitobj_onoff, STRING("a_kyaku_B"), 1, 1)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku1"), 0, PTR(evt_kyaku_1), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku2"), 0, PTR(evt_kyaku_2), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku3"), 0, PTR(evt_kyaku_3), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku4"), 0, PTR(evt_kyaku_4), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku5"), 0, PTR(evt_kyaku_5), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku6"), 0, PTR(evt_kyaku_6), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku7"), 0, PTR(evt_kyaku_7), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("kyaku8"), 0, PTR(evt_kyaku_8), 0)
    ELSE
        CALL(evt_hitobj_onoff, STRING("a_kyaku_A"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("a_kyaku_B"), 1, 0)
    END_IF
    RETURN
EVT_END()

static ScreenWork* screen_wp;

// NONMATCHING (99.92%): the GXColor temporary is 4 bytes lower on the stack than retail (0xC vs 0x8);
// every other instruction matches.
static void callback(TevCallbackParam* param) {
    GXColor color = {0, 0xFF, 0xFF, 0};
    s32 stage = param->field_C;
    s32 field_10 = param->field_10;
    s32 field_14 = param->field_14;
    s32 field_18 = param->field_18;

    color.a = gans_alpha;
    GXSetTevColor(GX_TEVREG0, color);
    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_C0, GX_CC_A0, GX_CC_ZERO);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);

    param->field_C = stage + 1;
    param->field_14 = field_14;
    param->field_18 = field_18;
    param->field_10 = field_10;
}

static f32 cosf(f32 x) {
    return cos(x);
}

static f32 sinf(f32 x) {
    return sin(x);
}

void GXSetTexCoordGen(GXTexCoordID dst_coord, GXTexGenType func, GXTexGenSrc src_param, u32 mtx) {
    GXSetTexCoordGen2(dst_coord, func, src_param, mtx, GX_FALSE, GX_PTIDENTITY);
}

static void screen_capture(CameraId camId, void* param) {
    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x130, 0xF0, 4, 1);
    GXCopyTex(screen_wp->field_0x0, 0);
    GXPixModeSync();
    screen_wp->field_0x4 = 1;
}

void kyaku_disp(CameraId camera, void* param) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

static f32 randf(f32 min, f32 max) {
    f32 t = (f32)__tou_rand() / 32767.0f;
    return t * (max - min) + min;
}

// dont_inline keeps __tou_rand/__tou_srand real `bl` calls from randf and ext_kyaku_init, as in retail.
#pragma dont_inline on
u32 __tou_rand(void) {
    x[0] = x[0] * 1103515245 + 12345;
    return (x[0] >> 16) & 0x7FFF;
}

void __tou_srand(s32 seed) {
    x[0] = seed;
}
#pragma dont_inline reset

static void pose_change(KyakuState* state, const char* name1, const char* name2) {
    KyakuEntry* entry;
    s32 i;

    for (i = 0, entry = kyaku_wp->entries; i < kyaku_wp->count2; i++, entry++) {
        if (strcmp(entry->name1, name1) == 0 &&
            strcmp(entry->name2, name2) == 0) {
            break;
        }
    }
    if (i < kyaku_wp->count2 && state->poseId != i) {
        state->poseId = i;
        state->field_0x18 = gp->renderTime + (s64)rand();
    }
}

// dont_inline keeps the real `bl pose_main` that ext_kyaku_main has in retail.
#pragma dont_inline on
static void pose_main(void) {
    s32 i;
    ExtPose* pose = extGetPosePtr();
    KyakuState* state = kyaku_wp->array1;
    f32 ratio;

    for (i = 0; i < kyaku_wp->count1; i++, state++, pose++) {
        KyakuEntry* entry = &kyaku_wp->entries[state->poseId];
        u64 delta = gp->renderTime - state->field_0x18;
        u32 ticks = OS_TIMER_CLOCK / 1000;

        ratio = 60.0f * (f32)(delta / ticks) / 1000.0f / entry->field_0x1C;
        if (ratio >= 1.0f) {
            state->field_0x18 = gp->renderTime;
            ratio = 0.0f;
        }
        pose->field_0x10 = entry->field_0x20 + (s32)((f32)(entry->field_0x24 - 1) * ratio);
    }
}
#pragma dont_inline reset

static void pose_init(void) {
    char buf1[0x28];
    char buf2[0x20];
    ExtKyakuData* data;
    s32 i;
    s32 start;

    data = ext_kyaku_data;
    i = 0;
    strcpy(buf1, "");
    strcpy(buf2, "");
    kyaku_wp->count2 = -1;
    do {
        if (strcmp(data->name1, buf1) != 0) {
            strcpy(buf1, data->name1);
            strcpy(buf2, "");
        }
        if (strcmp(data->name2, buf2) != 0) {
            strcpy(buf2, data->name2);
            start = i;
            kyaku_wp->count2++;
        }
        strcpy(kyaku_wp->entries[kyaku_wp->count2].name1, buf1);
        strcpy(kyaku_wp->entries[kyaku_wp->count2].name2, buf2);
        kyaku_wp->entries[kyaku_wp->count2].field_0x20 = start;
        kyaku_wp->entries[kyaku_wp->count2].field_0x24 = i - start + 1;
        kyaku_wp->entries[kyaku_wp->count2].field_0x1C = data->value;
        kyaku_wp->entries[kyaku_wp->count2].field_0x18 = 1;
        i++;
        data++;
    } while (data->name1 != NULL);
    kyaku_wp->count2++;
}
// clang-format on
