#include "eki_06.h"
#include "driver/animdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on

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

static EVT_BEGIN(e_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[3] = {
    {"e_bero_1", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_05", "w_bero_2", 5, 5, e_close, e_open},
    {"e_bero_2", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_05", "w_bero_1", 1, 1},
    {NULL},
};

EVT_BEGIN(evt_box_open)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 317)
        RETURN
    END_IF
    LABEL(0)
    CALL(evt_npc_get_ReactionOfLivingBody, 0, LW(0))
    IF_INT_GREATER(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set, -140, 152, 528, -140, 57, -12, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GW(0), 0)
    SET_VALUE(GW(1), 0)
    SET_VALUE(GW(2), 0)
    CALL(sw_entry, STRING("MOBJ_eki_06_su3"), -240, 0, 40)
    CALL(sw_entry, STRING("MOBJ_eki_06_su2"), -140, 0, 40)
    CALL(sw_entry, STRING("MOBJ_eki_06_su1"), -40, 0, 40)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_BOX_OPEN1"), -240, 0, 40, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_BOX_OPEN1"), -140, 0, 40, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_BOX_OPEN1"), -40, 0, 40, 0)
    CALL(evt_map_playanim, STRING("anm_hako"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hako"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_hitobj_onoff, STRING("a_hako"), 1, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 317)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_sw_complete)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -140, 152, 528, -140, 57, -12, 500, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mobj_set_anim, STRING("MOBJ_eki_06_su1"), STRING("ANIM_6"))
        CALL(evt_mobj_get_position, STRING("MOBJ_eki_06_su1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_SMALL1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(600)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_JUMP1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_mobj_set_anim, STRING("MOBJ_eki_06_su2"), STRING("ANIM_8"))
        CALL(evt_mobj_get_position, STRING("MOBJ_eki_06_su2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_SMALL1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(600)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_JUMP1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_mobj_set_anim, STRING("MOBJ_eki_06_su3"), STRING("ANIM_8"))
        CALL(evt_mobj_get_position, STRING("MOBJ_eki_06_su3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_SMALL1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(600)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_JUMP1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_mobj_wait_animation_end, STRING("MOBJ_eki_06_su1"))
    CALL(evt_mobj_wait_animation_end, STRING("MOBJ_eki_06_su2"))
    CALL(evt_mobj_wait_animation_end, STRING("MOBJ_eki_06_su3"))
    CALL(evt_mobj_delete, STRING("MOBJ_eki_06_su1"))
    CALL(evt_mobj_delete, STRING("MOBJ_eki_06_su2"))
    CALL(evt_mobj_delete, STRING("MOBJ_eki_06_su3"))
    CALL(evt_cam3d_evt_set, -80, 202, 448, -80, 107, -92, 500, 11)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("origami"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_origami"), 1)
    CALL(evt_hitobj_onoff, STRING("a_ori"), 1, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("origami"), LW(0), LW(1), LW(2))
        LOOP(3)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_PERA_KAI_OPEN1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(300)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_PERA_KAI_OPEN1_2"), LW(0), LW(1), LW(2), 0)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(2), -60)
            CALL(evt_mario_mov_pos2, LW(0), -25, FLOAT(80.0f))
        END_IF
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_kaidan167"), 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(900)
        CALL(evt_mapobj_get_position, STRING("origami"), LW(0), LW(1), LW(2))
        LOOP(6)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_PERA_KAI_OPEN2"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    WAIT_FRAMES(100)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_1"), 1)
    WAIT_FRAMES(32)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_2"), 1)
    WAIT_FRAMES(34)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_3"), 1)
    CALL(evt_map_checkanim, STRING("anm_kaidan167"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_SUCCESS1"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 318)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

const Vec vec3_eki_00009504 = { 0.0f, 0.0f, 0.0f };

EVT_BEGIN(eki_06_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI2"))
    CALL(evt_npc_change_fbat_mode, 1)
    SPAWN_EVENT(evt_box_open)
    SWITCH(GSW(0))
        CASE_LESS(317)
        CALL(evt_hitobj_onoff, STRING("a_ori"), 1, 0)
        CASE_EQUAL(317)
        SET_VALUE(GW(0), 0)
        SET_VALUE(GW(1), 0)
        SET_VALUE(GW(2), 0)
        CALL(sw_entry, STRING("MOBJ_eki_06_su3"), -240, 0, 40)
        CALL(sw_entry, STRING("MOBJ_eki_06_su2"), -140, 0, 40)
        CALL(sw_entry, STRING("MOBJ_eki_06_su1"), -40, 0, 40)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("origami"), 1)
        CALL(evt_hitobj_onoff, STRING("a_ori"), 1, 0)
        CASE_DEFAULT
        CALL(evt_map_playanim, STRING("anm_kaidan167"), 2, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_origami"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_3"), 1)
    END_SWITCH
    IF_INT_LESS(GSW(0), 317)
    ELSE
        CALL(evt_map_playanim, STRING("anm_hako"), 2, 0)
        CALL(evt_hitobj_onoff, STRING("a_hako"), 1, 0)
    END_IF
    CALL(evt_mapobj_trans, 1, STRING("HAKO"), 0, FLOAT(0.1f), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk01"), 95, 295, -80, 0, GSWF(5562), 0)
    RETURN
EVT_END()

s32 lock;

void sw_cntup(MapObjectEntry* obj) {
    if (strcmp(obj->name, "MOBJ_eki_06_su1") == 0) {
        evtSetValue(NULL, GW(0), evtGetValue(NULL, GW(0)) + 1);
    } else if (strcmp(obj->name, "MOBJ_eki_06_su2") == 0) {
        evtSetValue(NULL, GW(1), evtGetValue(NULL, GW(1)) + 1);
    } else if (strcmp(obj->name, "MOBJ_eki_06_su3") == 0) {
        evtSetValue(NULL, GW(2), evtGetValue(NULL, GW(2)) + 1);
    }
}

// The original emits sw_cnt with its last guard negated (`if (name != su3) return 0; return GW(2)`),
// but the copies inlined into sw_reset/sw_main keep the plain if/else-if chain. Calling sw_cnt from them
// would make the inliner use the negated form, so they expand SW_CNT_NATURAL instead.
s32 sw_cnt(MapObjectEntry* obj) {
    if (strcmp(obj->name, "MOBJ_eki_06_su1") == 0)
        return evtGetValue(NULL, GW(0));
    if (strcmp(obj->name, "MOBJ_eki_06_su2") == 0)
        return evtGetValue(NULL, GW(1));
    if (strcmp(obj->name, "MOBJ_eki_06_su3") != 0)
        return 0;
    return evtGetValue(NULL, GW(2));
}

#define SW_CNT_NATURAL(obj)                                                                     \
    (strcmp((obj)->name, "MOBJ_eki_06_su1") == 0   ? evtGetValue(NULL, GW(0))                   \
     : strcmp((obj)->name, "MOBJ_eki_06_su2") == 0 ? evtGetValue(NULL, GW(1))                   \
     : strcmp((obj)->name, "MOBJ_eki_06_su3") == 0 ? evtGetValue(NULL, GW(2))                   \
                                                   : 0)

void sw_reset(void) {
    MapObjectEntry* obj;

    obj = mobjNameToPtr("MOBJ_eki_06_su1");
    obj->flags &= ~2;
    switch (SW_CNT_NATURAL(obj)) {
        case 2:
            animPoseSetAnim(obj->poseId, "ANIM_5", 1);
            evtSetValue(NULL, GW(0), 0);
            break;
    }
    obj = mobjNameToPtr("MOBJ_eki_06_su2");
    obj->flags &= ~2;
    switch (SW_CNT_NATURAL(obj)) {
        case 11:
            animPoseSetAnim(obj->poseId, "ANIM_7", 1);
            evtSetValue(NULL, GW(1), 0);
            break;
    }
    obj = mobjNameToPtr("MOBJ_eki_06_su3");
    obj->flags &= ~2;
    switch (SW_CNT_NATURAL(obj)) {
        case 4:
            animPoseSetAnim(obj->poseId, "ANIM_7", 1);
            evtSetValue(NULL, GW(2), 0);
            break;
    }
}

s32 sw_main(MapObjectEntry* obj) {
    MapObjectEntry* su;

    switch (obj->unk1DC) {
        case 0:
            if (!(obj->flags & 8)) {
                break;
            }
            if (lock != 0) {
                break;
            }
            lock = 1;
            if (obj->unk1E0 == 0x200000) {
                marioChgMot(5);
            }
            psndSFXOn_3D("SFX_STG6_OBJ_SWITCH_ON1", &obj->position);
            sw_cntup(obj);
            if (strcmp(obj->name, "MOBJ_eki_06_su1") == 0) {
                if (SW_CNT_NATURAL(obj) == 1) {
                    animPoseSetAnim(obj->poseId, "ANIM_2", 1);
                } else {
                    sw_reset();
                }
            }
            if (strcmp(obj->name, "MOBJ_eki_06_su2") == 0) {
                if (SW_CNT_NATURAL(obj) < 10) {
                    animPoseSetAnim(obj->poseId, "ANIM_2", 1);
                } else if (SW_CNT_NATURAL(obj) == 10) {
                    animPoseSetAnim(obj->poseId, "ANIM_4", 1);
                } else {
                    sw_reset();
                }
            }
            if (strcmp(obj->name, "MOBJ_eki_06_su3") == 0) {
                if (SW_CNT_NATURAL(obj) < 3) {
                    animPoseSetAnim(obj->poseId, "ANIM_2", 1);
                } else if (SW_CNT_NATURAL(obj) == 3) {
                    animPoseSetAnim(obj->poseId, "ANIM_4", 1);
                } else {
                    sw_reset();
                }
            }
            obj->unk1DC++;
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            if (evtGetValue(NULL, GW(0)) == 1 && evtGetValue(NULL, GW(1)) == 10 &&
                evtGetValue(NULL, GW(2)) == 3) {
                evtEntry(evt_sw_complete, 0, 0);
                obj->unk1DC++;
                break;
            }
            su = mobjNameToPtr("MOBJ_eki_06_su1");
            su->flags |= 2;
            su = mobjNameToPtr("MOBJ_eki_06_su2");
            su->flags |= 2;
            su = mobjNameToPtr("MOBJ_eki_06_su3");
            su->flags |= 2;
            if (strcmp(obj->name, "MOBJ_eki_06_su1") == 0) {
                if (SW_CNT_NATURAL(obj) == 1) {
                    animPoseSetAnim(obj->poseId, "ANIM_3", 1);
                } else {
                    animPoseSetAnim(obj->poseId, "ANIM_1", 1);
                }
            }
            if (strcmp(obj->name, "MOBJ_eki_06_su2") == 0) {
                if (SW_CNT_NATURAL(obj) == 10) {
                    animPoseSetAnim(obj->poseId, "ANIM_5", 1);
                } else {
                    animPoseSetAnim(obj->poseId, "ANIM_1", 1);
                }
            }
            if (strcmp(obj->name, "MOBJ_eki_06_su3") == 0) {
                if (SW_CNT_NATURAL(obj) == 3) {
                    animPoseSetAnim(obj->poseId, "ANIM_5", 1);
                } else {
                    animPoseSetAnim(obj->poseId, "ANIM_1", 1);
                }
            }
            obj->flags &= ~8;
            obj->unk1DC = 0;
            lock = 0;
            break;
        case 2:
            break;
        default:
            break;
    }
    return 0;
}

USER_FUNC(sw_entry) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(name, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, name);
    obj = mobjNameToPtr(name);
    /* MUST_MATCH: volatile makes the flags store stay ahead of the vec3 copy, as in the original;
       the objects themselves are not volatile, but this reproduces MWCC's exact schedule. */
    *(volatile s32*)&obj->flags |= 6;
    pos = *(volatile Vec*)&vec3_eki_00009504;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->unk1D0 = sw_main;
    obj->flags |= 0xa10000;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "ANIM_1", 1);
    lock = 0;
    return EVT_RETURN_DONE;
}
// clang-format on
