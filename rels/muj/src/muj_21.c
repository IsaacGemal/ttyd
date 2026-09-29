#include "muj_21.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_light.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "driver/effdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_funemizu.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "forcedata.h"
#include "mario/mariost.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 anm_entotsu[];
extern s32 anm_kaji[];
extern s32 anm_neji[];
extern s32 anm_wakka[];
static USER_FUNC(fune_eff);

// clang-format off
extend_data(120,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(121,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(peach_init)
    CALL(evt_npc_set_position, STRING("me"), 62, 28, 44)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"ピーチ姫", 0x00080600, 0, peach_init},
    {NULL},
};

EVT_BEGIN(fune_yure)
    CALL(evt_hit_bind_mapobj, STRING("A_fune_m"), STRING("S_fune_m"))
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    LOOP(0)
        CALL(evt_sub_get_sincos, LW(0), LW(1), LW(2))
        MULTIPLY_FLOAT(LW(1), FLOAT(0.8f))
        CALL(evt_mapobj_rotate, 1, STRING("S_fune_m"), 0, 0, LW(1))
        CALL(evt_hit_bind_update, STRING("A_fune_m"))
        WAIT_FRAMES(1)
        ADD_FLOAT(LW(0), FLOAT(2.0f))
        IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon_f, 768, STRING("BGM_ENDING5"), 800)
    CALL(evt_snd_envon, 272, STRING("ENV_END_MUJ2"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pos, 40, 28, 64)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_flag_onoff, 1, 1024)
    CALL(evt_npc_flag_onoff, 1, STRING("ピーチ姫"), 163856)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_snd_sfx_vol, LW(3), 63)
    CALL(evt_cam3d_evt_set, 44, 173, 585, 44, 79, 50, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_npc_set_anim, STRING("ピーチ姫"), STRING("P_S_7"))
    WAIT_MS(2000)
    CALL(evt_npc_flag_onoff, 0, STRING("ピーチ姫"), 32768)
    CALL(evt_npc_set_autotalkpose, STRING("ピーチ姫"), STRING("P_S_7"), STRING("P_T_13"))
    CALL(evt_msg_print, 0, STRING("ep_27"), 0, STRING("ピーチ姫"))
    CALL(evt_npc_set_ry, STRING("ピーチ姫"), 270)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ピーチ姫"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_28"), 0, STRING("ピーチ姫"))
    CALL(evt_cam3d_evt_set, 44, 137, 380, 44, 79, 50, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("ep_29"), 0, STRING("ピーチ姫"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("ピーチ姫"), 90)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("ピーチ姫"))
    WAIT_MS(300)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ピーチ姫"), 0, 0, 0, 0, 0, 0, 0, 48)
    WAIT_MS(800)
    CALL(evt_npc_set_anim, STRING("ピーチ姫"), STRING("P_S_5"))
    CALL(evt_map_playanim, STRING("ake"), 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_eff64, STRING("sunlight"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_snd_bgmoff_f, 512, 3000)
        WAIT_MS(1500)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_ENDING3"), 2000)
    END_INLINE_EVENT
    WAIT_MS(1500)
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_color, 1, STRING("S_nami_kira"), 255, 255, 255, 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_nami_kira"), 1)
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S_nami_kira"), 255, 255, 255, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1300)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_sun_hika"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_color, 1, STRING("S_sun_hika"), 255, 255, 255, 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_sun_hika"), 1)
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_mapobj_color, 1, STRING("S_sun_hika"), 255, 255, 255, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_map_checkanim, STRING("ake"), LW(0), LW(1))
    SUBTRACT(LW(1), 8000)
    WAIT_MS(LW(1))
    CALL(evt_npc_set_autotalkpose, STRING("ピーチ姫"), STRING("P_S_5"), STRING("P_T_5"))
    CALL(evt_msg_print, 0, STRING("ep_30"), 0, STRING("ピーチ姫"))
    CALL(evt_fade_set_mapchange_type, 1, 10, 1000, 9, 1000)
    CALL(evt_bero_mapchange, STRING("gor_12"), STRING(""))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static USER_FUNC(fune_eff) {
    Vec pos;
    EffectEntry* e1;
    EffectEntry* e2;

    if (isFirstCall) {
        event->userdata[0] = (s32)effFunemizuEntry(0, 0.0f, -1000.0f, 0.0f, 0.0f, 0.0f);
        event->userdata[1] = (s32)effFunemizuEntry(2, 0.0f, -1000.0f, 0.0f, 0.0f, 0.0f);
        mapObjGetPos("S_fune_m", &pos);
        event->userdataf[2] = pos.x;
        *(f32*)&event->field_84 = pos.z;
    }
    e1 = (EffectEntry*)event->userdata[0];
    e2 = (EffectEntry*)event->userdata[1];
    mapObjGetPos("S_fune_m", &pos);
    event->userdataf[2] = pos.x;
    *(f32*)&event->field_84 = pos.z;
    ((EffFunemizuData*)e1->userdata)->field_0x44 = 3.0f;
    ((EffFunemizuData*)e1->userdata)->x = pos.x - 81.3f;
    ((EffFunemizuData*)e1->userdata)->y = pos.y - 51.0f;
    ((EffFunemizuData*)e1->userdata)->z = pos.z - 38.9f;
    ((EffFunemizuData*)e1->userdata)->field_0x10 = 1.0f;
    ((EffFunemizuData*)e1->userdata)->field_0x14 = 270.0f;
    ((EffFunemizuData*)e2->userdata)->x = pos.x - 49.5f;
    ((EffFunemizuData*)e2->userdata)->y = pos.y - 51.0f;
    ((EffFunemizuData*)e2->userdata)->z = pos.z + 70.7f;
    ((EffFunemizuData*)e2->userdata)->field_0x10 = 1.0f;
    ((EffFunemizuData*)e2->userdata)->field_0x14 = 305.0f;
    if (gp->retraceCount % 15 == 0) {
        effWaterDamageN64Entry(5, 30, pos.x + 106.1f, pos.y - 50.0f, pos.z + 106.1f, 2.0f);
    }
    return 0;
}

EVT_BEGIN(muj_21_init_evt)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("hikari"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_nami_kira"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_sun_hika"), 1)
    BEGIN_INLINE_EVENT
        CALL(fune_eff)
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(10), FLOAT(1.0f))
    SPAWN_EVENT(anm_kaji)
    SPAWN_EVENT(anm_wakka)
    SPAWN_EVENT(anm_entotsu)
    SPAWN_EVENT(anm_neji)
    SPAWN_EVENT(fune_yure)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_f_wa"), 2)
    CALL(evt_mario_kill_party, 0)
    SPAWN_EVENT(epilogue_evt)
    CALL(evt_light_charalight_all_onoff, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
