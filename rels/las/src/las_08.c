#include "las_08.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_torch.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l1_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l1_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l1_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l1_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(w2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_l2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r1_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r1_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e1_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r1_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r1_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e2_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e2_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_01"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("S_doa_r2_02"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door_matigai_evt)
    IF_INT_EQUAL(GSW(0), 385)
        SET_VALUE(GSW(1121), 0)
        SET_VALUE(GSWF(4325), 1)
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door_seikai_evt)
    IF_INT_EQUAL(GSW(0), 385)
        ADD(GSW(1121), 1)
        IF_INT_GREATER_OR_EQUAL(GSW(1121), 7)
            SET_VALUE(GSW(0), 386)
        END_IF
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[5] = {
    {"w_bero_1", 1, 6, 0x00004E20, 0x000186A0,
     0x00000000, 0x00000000, 0xFFFFFFFF, NULL,
     0xFFFFFFFF, door_matigai_evt, "las_07", "e_bero_1",
     5, 5, &w1_door_close, &w1_door_open},
    {"e_bero_1", 1, 6, 0x00004E20, 0x000186A0,
     0x00000000, 0x00000000, 0xFFFFFFFF, NULL,
     0xFFFFFFFF, door_seikai_evt, "las_09", "w_bero",
     5, 5, &e1_door_close, &e1_door_open},
    {"w_bero_2", 1, 6, 0x00004E20, 0x000186A0,
     0x00000000, 0x00000000, 0xFFFFFFFF, NULL,
     0xFFFFFFFF, door_matigai_evt, "las_07", "e_bero_2",
     5, 5, &w2_door_close, &w2_door_open},
    {"e_bero_2", 1, 6, 0x00004E20, 0x000186A0,
     0x00000000, 0x00000000, 0xFFFFFFFF, NULL,
     0xFFFFFFFF, door_matigai_evt, "las_07", "w_bero_2",
     5, 5, &e2_door_close, &e2_door_open},
    {NULL},
};

BeroEntry e_bero_1_matigai[] = {
    {"e_bero_1", 1, 6, 0x00004E20, 0x000186A0,
     0x00000000, 0x00000000, 0xFFFFFFFF, NULL,
     0xFFFFFFFF, door_matigai_evt, "las_07", "w_bero_1",
     5, 5, &e1_door_close, &e1_door_open},
};

BeroEntry e_bero_1_seikai[] = {
    {"e_bero_1", 1, 6, 0x00004E20, 0x000186A0,
     0x00000000, 0x00000000, 0xFFFFFFFF, NULL,
     0xFFFFFFFF, door_seikai_evt, "las_08", "w_bero_1",
     5, 5, &e1_door_close, &e1_door_open},
};

BeroEntry e_bero_2_seikai[] = {
    {"e_bero_2", 1, 6, 0x00004E20, 0x000186A0,
     0x00000000, 0x00000000, 0xFFFFFFFF, NULL,
     0xFFFFFFFF, door_seikai_evt, "las_08", "w_bero_2",
     5, 5, &e2_door_close, &e2_door_open},
};

char* taimatu_data[16] = {
    "S_hikari_01", "A_hi_01", "S_hikari_02", "A_hi_02",
    "S_hikari_03", "A_hi_03", "S_hikari_04", "A_hi_04",
    "S_hikari_05", "A_hi_05", "S_hikari_06", "A_hi_06",
    "S_hikari_07", "A_hi_07", "S_hikari_08", "A_hi_08",
};

EVT_BEGIN(hikari_anim)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 65536)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_sub_get_sincos, LW(0), LW(1), LW(2))
        MULTIPLY_FLOAT(LW(1), FLOAT(0.2f))
        ADD_FLOAT(LW(1), FLOAT(0.8f))
        SET_INT_READ_ADDR(PTR(taimatu_data))
        LOOP(8)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mapobj_scale, 0, LW(3), LW(1), LW(1), LW(1))
        END_LOOP
        ADD(LW(0), 2)
        IF_INT_GREATER_OR_EQUAL(LW(0), 360)
            SUBTRACT(LW(0), 360)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(taimatu_evt)
    SET_INT_READ_ADDR(PTR(taimatu_data))
    LOOP(8)
        READ_INT2(LW(0), LW(1))
        CALL(evt_mapobj_flag_onoff, 0, 1, LW(0), 1)
    END_LOOP
    SPAWN_EVENT(hikari_anim)
    RETURN
EVT_END()

s32 mugen_kairou_list[7] = {
    7, 7, 3, 3, 7, 3, 7,
};

USER_FUNC(hi_onoff) {
    s32* args = event->args;
    s32 index;
    s32 onoff;
    MapObject* mapObj;
    EffectEntry* effect;
    Vec pos;

    index = evtGetValue(event, args[0]);
    onoff = evtGetValue(event, args[1]);
    mapObj = mapGetMapObj(taimatu_data[index * 2]);
    if (onoff != 0) {
        *(u32*)mapObj->field_0x0 &= ~1;
        hitObjGetPos(taimatu_data[index * 2 + 1], &pos);
        effect = effTorchEntry(0, pos.x, pos.y, pos.z, float_0p7_las_00012688);
        effSetName(effect, taimatu_data[index * 2 + 1]);
    } else {
        *(u32*)mapObj->field_0x0 |= 1;
        effSoftDelete(effNameToPtr(taimatu_data[index * 2 + 1]));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(mugen_kairou_get_list) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], mugen_kairou_list[index]);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(mugen_kairou_init)
    CALL_CHILD(taimatu_evt)
    IF_INT_EQUAL(GSW(0), 385)
        CALL(mugen_kairou_get_list, GSW(1121), LW(0))
        CALL(hi_onoff, LW(0), 1)
        CALL(evt_seq_wait, 2)
        CALL(evt_bero_exec_wait, 65536)
        CALL(mugen_kairou_get_list, GSW(1121), LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(7)
            IF_INT_LESS(GSW(1121), 6)
                SET_VALUE(LW(0), STRING("e_bero_1"))
                CALL_CHILD(bero_case_switch_off)
                CALL(evt_bero_overwrite, STRING("e_bero_1"), PTR(e_bero_1_seikai))
                SET_VALUE(LW(0), STRING("e_bero_1"))
                CALL_CHILD(bero_case_switch_on)
            END_IF
            CASE_EQUAL(3)
            SET_VALUE(LW(0), STRING("e_bero_2"))
            CALL_CHILD(bero_case_switch_off)
            CALL(evt_bero_overwrite, STRING("e_bero_2"), PTR(e_bero_2_seikai))
            SET_VALUE(LW(0), STRING("e_bero_2"))
            CALL_CHILD(bero_case_switch_on)
            SET_VALUE(LW(0), STRING("e_bero_1"))
            CALL_CHILD(bero_case_switch_off)
            CALL(evt_bero_overwrite, STRING("e_bero_1"), PTR(e_bero_1_matigai))
            SET_VALUE(LW(0), STRING("e_bero_1"))
            CALL_CHILD(bero_case_switch_on)
        END_SWITCH
    ELSE
        CALL(hi_onoff, 4, 1)
        CALL(hi_onoff, 5, 1)
        CALL(hi_onoff, 6, 1)
        CALL(hi_onoff, 7, 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(las_08_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG8_LST1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG8_LAS1"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT(mugen_kairou_init)
    RETURN
EVT_END()

const f32 float_0p7_las_00012688 = 0.7f;
// clang-format on
#pragma force_active off
