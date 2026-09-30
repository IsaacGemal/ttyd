#include "kpa.h"
#include "data/mapdata.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_twoddisp.h"
#include "forcedata.h"
#include "koopa.h"
#include "kpa_act.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party/party_motion.h"
#include <stdio.h>

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

#pragma pool_data off

#pragma force_active on
s32 kpa_stage_clear[35];

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(kpa_init)
    CALL(evt_mario_set_mode, 2)
    CALL(evt_mario_kill_party, 0)
    CALL(kpa_mobj_on)
    CALL(kpa_score_disp_init)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("kpa_replay"))
        SET_VALUE(GSW(1222), 180)
    ELSE
        IF_STR_NOT_EQUAL(LW(0), STRING("kpa_next_map"))
            SET_VALUE(GSW(1221), 1)
            SET_VALUE(GSW(1222), 180)
        END_IF
    END_IF
    CALL(evt_cam_type_change, 1, 4)
    SET_VALUE(LSWF(2), 0)
    SET_VALUE(LSWF(3), 0)
    SET_VALUE(LSWF(4), 0)
    SET_VALUE(LSWF(5), 0)
    RETURN
EVT_END()

EVT_BEGIN(kpa_read_mobj)
    SET_INT_READ_ADDR(LW(0))
    LOOP(100)
        READ_INT(LW(1))
        IF_INT_EQUAL(LW(1), -1)
            BREAK_LOOP
        END_IF
        CALL(kpa_make_mobj_name, LW(2))
        SWITCH(LW(1))
            CASE_EQUAL(0)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_ojama_blk, LW(2), LW(3), LW(4), 0, 0, 0, STRING("MOBJ_Lv1BigBlock"))
            CASE_EQUAL(1)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_stone_blk, LW(2), LW(3), LW(4), 0, 0, 0, STRING("MOBJ_Lv2BigBlock"))
            CASE_EQUAL(2)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 0, 0, 0)
            CASE_EQUAL(4)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 2, 0, 0)
            CASE_EQUAL(3)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 1, 0, 0)
            CASE_EQUAL(5)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_brick, LW(2), LW(3), LW(4), 0, 0, 3, 0, 0)
            CASE_EQUAL(6)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_koopa_badgeblk, LW(2), LW(3), LW(4), 0, LW(5), 0, 0, 0)
            CASE_EQUAL(7)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_koopa_hidden_blk, LW(2), LW(3), LW(4), 0, LW(5), 1, 0, 0)
            CASE_EQUAL(8)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_blk, LW(2), LW(3), LW(4), 0, 0, 0, 0)
            CASE_EQUAL(9)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_dokan, 0, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(10)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_dokan, 1, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(11)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_dokan, 2, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(12)
            READ_INT2(LW(3), LW(4))
            CALL(evt_mobj_koopa_sango, LW(2), LW(3), LW(4), 0, 0, 0)
            CASE_EQUAL(13)
            READ_INT4(LW(3), LW(4), LW(5), LW(6))
            CALL(evt_mobj_koopa_fireber_dodai, 0, LW(2), LW(3), LW(4), 0, LW(5), LW(6), 0, 0)
            CASE_EQUAL(14)
            READ_INT4(LW(3), LW(4), LW(5), LW(6))
            CALL(evt_mobj_koopa_fireber_dodai, 1, LW(2), LW(3), LW(4), 0, LW(5), LW(6), 0, 0)
            CASE_EQUAL(15)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_koopa_pole, STRING("pole"), LW(3), LW(4), 0, LW(5), PTR(kpa_stage_clear), LSWF(3))
            CASE_EQUAL(16)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_jumpstand_red, 0, LW(2), LW(3), LW(4), 0, LW(5), 0)
            CASE_EQUAL(17)
            READ_INT3(LW(3), LW(4), LW(5))
            CALL(evt_mobj_jumpstand_blue, 0, LW(2), LW(5), LW(3), LW(4), 0, 0, 0)
        END_SWITCH
    END_LOOP
    RETURN
EVT_END()

// helper placed here so its (Vec){0,0,0} literal pools at the same .rodata offset
// as the original's vec3_kpa_000014a4 (the scheduler then keeps the event->args load first)
static inline void kpaZeroVec(Vec* v) {
    *v = (Vec){ 0.0f, 0.0f, 0.0f };
}

EVT_BEGIN(kpa_coin_evt)
    CALL(kpa_add_coin, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

EVT_BEGIN(kpa_read_item)
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT(LW(1))
        IF_INT_EQUAL(LW(1), -1)
            BREAK_LOOP
        END_IF
        CALL(kpa_make_item_name, LW(2))
        READ_INT2(LW(3), LW(4))
        CALL(evt_item_entry, LW(2), 121, LW(3), LW(4), 0, 16, -1, PTR(kpa_coin_evt))
        CALL(evt_item_flag_on, LW(2), 4)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kpa_dead)
    IF_INT_EQUAL(LSWF(3), 1)
        RETURN
    END_IF
    SET_VALUE(LSWF(3), 1)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(unk_kpa_00000a08)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_adjust_dir)
    CALL(kpa_nohitenemy_ct_clear)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_set_motion, 0)
    CALL(kpa_chg_pose, STRING("KPA2_D_4"))
    SET_VALUE(LSWF(4), 1)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
    SET_FLOAT_VALUE(LW(3), LW(5))
    SET_FLOAT_VALUE(LW(0), LW(3))
    SET_FLOAT_VALUE(LW(1), FLOAT(8.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(-0.14f))
    CALL(evt_koopa_chk_dead, LW(7))
    IF_INT_EQUAL(LW(7), 1)
        MULTIPLY_FLOAT(LW(1), FLOAT(0.7f))
    END_IF
    SUBTRACT_FLOAT(LW(3), FLOAT(200.0f))
    CALL(evt_sub_get_mapname, LW(15))
    IF_STR_EQUAL(LW(15), STRING("kpa_03"))
        SET_VALUE(LW(5), 150)
    ELSE
        SET_VALUE(LW(5), 155)
    END_IF
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(1), 0)
            SET_FLOAT_VALUE(LF(0), 0)
        ELSE
            SET_FLOAT_VALUE(LF(0), 1)
        END_IF
        ADD_FLOAT(LW(0), LW(1))
        ADD_FLOAT(LW(1), LW(2))
        IF_INT_LESS(LW(1), 0)
            IF_INT_EQUAL(LF(0), 0)
            END_IF
        END_IF
        IF_INT_LESS(LW(0), LW(3))
            BREAK_LOOP
        END_IF
        CALL(evt_mario_set_pos, LW(4), LW(0), LW(5))
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_LESS(GSW(1221), 99)
        ADD(GSW(1221), 1)
    END_IF
    CALL(kpa_status_get_stage, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(1), STRING("kpa_02"))
        CASE_EQUAL(2)
        SET_VALUE(LW(1), STRING("kpa_03"))
        CASE_EQUAL(3)
        SET_VALUE(LW(1), STRING("kpa_05"))
    END_SWITCH
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_cam_letter_box_disable, 0)
    END_INLINE_EVENT
    CALL(evt_bero_mapchange, LW(1), STRING("kpa_replay"))
    RETURN
EVT_END()

EVT_BEGIN(kpa_fall_sub)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_ctrl_onoff, 4, 0)
    CALL(evt_mario_get_pos, 0, LW(13), LW(14), LW(15))
    LOOP(0)
        CALL(evt_mario_get_name_hitobj_ride, LW(0))
        IF_STR_NOT_EQUAL(LW(0), STRING(""))
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL_CHILD(kpa_dead)
    RETURN
EVT_END()

const Vec vec3_kpa_000014d8 = {0.0f, 0.0f, 0.0f};

EVT_BEGIN(kpa_bgm_change)
    IF_INT_GREATER(GSW(1222), 30)
        SET_VALUE(LW(2), 0)
    ELSE
        SET_VALUE(LW(2), 1)
    END_IF
    SET_VALUE(LW(3), -1)
    SET_VALUE(LW(4), -1)
    LABEL(10)
    IF_INT_GREATER(GSW(1222), 30)
        SET_VALUE(LW(3), 0)
        ADD(LW(3), LSW(1))
    ELSE
        IF_INT_EQUAL(LW(2), 0)
            SET_VALUE(LW(3), 3)
            ADD(LW(3), LSW(1))
        ELSE
            SET_VALUE(LW(3), 6)
        END_IF
    END_IF
    CALL(kpa_muteki_check)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(3), 7)
        SET_VALUE(LW(2), 0)
    END_IF
    IF_INT_EQUAL(LSWF(4), 1)
        SET_VALUE(LW(3), 8)
    END_IF
    IF_INT_EQUAL(LSWF(5), 1)
        SET_VALUE(LW(3), 9)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), LW(4))
        SET_VALUE(LW(4), LW(3))
        SWITCH(LW(3))
            CASE_EQUAL(0)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD3"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(2)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD4"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(3)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD1_UP1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(4)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD3_UP1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(5)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD4_UP1"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(6)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STGK_FIELD1_UP2"))
            CALL(evt_snd_bgmoff, 513)
            CASE_EQUAL(7)
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_bgmon, 513, STRING("BGM_STGK_FIELD2"))
            CASE_EQUAL(8)
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_KUPPA_DOWN1"))
            CALL(evt_snd_bgmoff, 513)
            RETURN
            CASE_EQUAL(9)
            CALL(evt_snd_bgmon, 512, STRING("BGM_FF_KUPPA_GOAL1"))
            CALL(evt_snd_bgmoff, 513)
            RETURN
        END_SWITCH
        WAIT_MS(300)
    END_IF
    WAIT_FRAMES(1)
    GOTO(10)
    RETURN
EVT_END()

EVT_BEGIN(kpa_stage_start)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_snd_envoff, 512)
    CALL(evt_map_blend_set_flag, 1, STRING("mario"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("mario"), 4096)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 255)
    CALL(evt_map_set_blend, 1, 0, 0, 0, 255)
    CALL(kpa_set_scale, FLOAT(0.4f))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    ADD(LW(1), 25)
    ADD(LW(2), 400)
    CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(0), LW(1), 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_dispdir, 180)
    CALL(evt_seq_wait, 2)
    CALL(evt_twoddisp_init)
    CALL(evt_twoddisp_entry, STRING("batu"), 469)
    CALL(evt_twoddisp_set_base_position, STRING("batu"), 20, -50)
    CALL(evt_twoddisp_set_position, STRING("batu"), 10, 0)
    CALL(evt_twoddisp_entry, STRING("mugen"), 526)
    CALL(evt_twoddisp_set_base_position, STRING("mugen"), 20, -50)
    CALL(evt_twoddisp_set_position, STRING("mugen"), 54, 3)
    CALL(kpa_get_msg, LW(0))
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA2_S_1"), STRING("KPA2_T_1"))
    CALL(evt_msg_print, 0, LW(0), 0, STRING("mario"))
    CALL(evt_fade_out, 1000)
    SPAWN_EVENT(kpa_bgm_change)
    CALL(evt_snd_envoff, 512)
    CALL(evt_map_blend_off, 0)
    CALL(evt_map_blend_off, 1)
    CALL(evt_twoddisp_flag_onoff, 0, STRING("batu"), 1)
    CALL(evt_twoddisp_flag_onoff, 0, STRING("mugen"), 1)
    CALL(kpa_set_scale, FLOAT(1.0f))
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_fade_in, 1000)
    SET_VALUE(LSWF(2), 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

const GXColor dat_kpa_000015cc = {0xFF, 0xFF, 0xFF, 0xFF};

u16 numeric_id[10] = {
    0x01D9, 0x01DB, 0x01DD, 0x01DF, 0x01E1,
    0x01E3, 0x01E5, 0x01E7, 0x01E9, 0x01EB,
};

EVT_BEGIN(kpa_status_main_evt)
    LOOP(0)
        CALL(kpa_status_main)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kpa_time)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 180)
    SUBTRACT(LW(1), GSW(1222))
    SET_VALUE(LW(3), LW(1))
    DIVIDE(LW(1), 10)
    MODULO(LW(3), 10)
    MULTIPLY(LW(3), 1000)
    SET_VALUE(LF(0), 0)
    LOOP(0)
        IF_INT_NOT_EQUAL(LSWF(2), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(0)
        SPAWN_EVENT_GET_ID(PTR(evt_sub_stopwatch), LW(0))
        LOOP(0)
            IF_INT_NOT_EQUAL(LSWF(3), 0)
                RETURN
            END_IF
            CALL(evt_sub_get_stopwatch, LW(0), LW(2))
            IF_INT_EQUAL(LF(0), 0)
                ADD(LW(2), LW(3))
            END_IF
            CALL(kpa_set_time, LW(2), LW(1), GSW(1222))
            IF_INT_GREATER_OR_EQUAL(LW(2), 10000)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LF(0), 1)
        ADD(LW(1), 1)
        IF_INT_GREATER_OR_EQUAL(LW(1), 18)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(kpa_pole_hitoff)
    CALL_CHILD(kpa_dead)
    RETURN
EVT_END()

EVT_BEGIN(kpa_stage_clear)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_sfxon, STRING("SFX_KUPPA_COUNTER1"), LW(0))
    CALL(kpa_time_countdown)
    CALL(evt_snd_sfxoff, LW(0))
    CALL(evt_eff, 0, STRING("stageclear"), 1, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(3500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

u8* kwp;

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("kpa_00", kpa_00_init_evt);
    relSetEvtAddr("kpa_01", kpa_01_init_evt);
    relSetEvtAddr("kpa_02", kpa_02_init_evt);
    relSetEvtAddr("kpa_03", kpa_03_init_evt);
    relSetEvtAddr("kpa_04", kpa_04_init_evt);
    relSetEvtAddr("kpa_05", kpa_05_init_evt);
    relSetEvtAddr("kpa_06", kpa_06_init_evt);
    relSetEvtAddr("kpa_07", kpa_07_init_evt);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

USER_FUNC(kpa_status_get_stage) {
    evtSetValue(event, event->args[0], kwp[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_mobj_on) {
    mobjKoopaOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_make_mobj_name) {
    static s32 no;
    static char name[16];
    s32* args = event->args;

    sprintf(name, "mobj%x", no);
    no++;
    evtSetValue(event, args[0], (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_make_item_name) {
    static s32 no;
    static char name[16];
    s32* args = event->args;

    sprintf(name, "item%2x", no);
    no++;
    evtSetValue(event, args[0], (s32)name);
    return EVT_RETURN_DONE;
}

// The zero Vec comes from kpaZeroVec's anonymous (Vec){0,0,0} literal rather than a named
// vec3_... object: the literal's pool entry is what keeps the event->args load ahead of the copy.
USER_FUNC(kpa_add_coin) {
    s32* args = event->args;
    Vec pos;

    kpaZeroVec(&pos);
    pos.x = evtGetFloat(event, args[0]);
    pos.y = evtGetFloat(event, args[1]);
    pos.z = evtGetFloat(event, args[2]);
    kpaAddCoinPos(1, &pos);
    pouchAddCoin(-1);
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_nohitenemy_ct_clear) {
    MarioWork* mario = marioGetPtr();
    *(u16*)&mario->unk78[0] = 0;
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_kpa_00000a08) {
    unk_801f8cd0();
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_set_scale) {
    f32 scale = evtGetFloat(event, event->args[0]);
    MarioWork* mario = marioGetPtr();
    Vec v = vec3_kpa_000014d8;

    v.x = scale;
    v.y = scale;
    v.z = scale;
    mario->scale = v;
    mario->wPlayerCollisionBox.x = kpaGetWidth(1) * scale;
    mario->wPlayerCollisionBox.y = 75.0f * scale;
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_get_msg) {
    static char msgBuf[16];
    s32* args = event->args;
    s32 n = kwp[1];
    s32 stage = kwp[0];
    s32 x;

    if (n > 0x1E) {
        n = 0x1E;
    }
    x = (stage - 1) * 0x1E + n;
    x -= 1;
    sprintf(msgBuf, "kpa_start_%03d", x);
    evtSetValue(event, args[0], (s32)msgBuf);
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_muteki_check) {
    event->lwData[0] = kpaMutekiCheck();
    return EVT_RETURN_DONE;
}

// NONMATCHING (79.41%): numeric_id is u16 and the original passes it to iconDispGx without
// sign-extension (lhzx -> bl), i.e. iconDispGx's iconId parameter is u16, but include/driver/
// icondrv.h declares it s16, so the compiler inserts an extsh. With the header changed to u16
// the score rises to 83.53%, but the callee-saved register rotation (r27-r31) still differs.
void kpa_status_disp(CameraId cameraId, void* param) {
    Vec pos;
    s32 digits[6];
    s32 i;
    s32 value;

    pos.x = -210.0f;
    pos.y = 160.0f;
    iconDispGx(&pos, 0x10, gp->language == 0 ? 0x206 : 0x210, 1.0f);
    pos.x = 50.0f;
    iconDispGx(&pos, 0x10, 0x208, 1.0f);
    pos.x = 220.0f;
    iconDispGx(&pos, 0x10, 0x207, 1.0f);

    value = pouchGetKpaScore();
    pos.x = -250.0f;
    pos.y = 130.0f;
    digits[0] = value / 100000;
    value %= 100000;
    digits[1] = value / 10000;
    value %= 10000;
    digits[2] = value / 1000;
    value %= 1000;
    digits[3] = value / 100;
    value %= 100;
    digits[4] = value / 10;
    value %= 10;
    digits[5] = value;
    for (i = 0; i < 6; i++) {
        iconDispGx(&pos, 0x10, numeric_id[digits[i]], 1.0f);
        pos.x += 20.0f;
    }
    pos.x += 20.0f;
    iconDispGx(&pos, 0x10, 0x18B, 0.7f);
    pos.x += 30.0f;
    iconDispGx(&pos, 0x10, 0x1D5, 1.0f);

    value = pouchGetKpaCoin();
    pos.x += 20.0f;
    digits[0] = value / 10;
    digits[1] = value % 10;
    for (i = 0; i < 2; i++) {
        iconDispGx(&pos, 0x10, numeric_id[digits[i]], 1.0f);
        pos.x += 20.0f;
    }

    pos.x = 50.0f;
    iconDispGx(&pos, 0x10, numeric_id[kwp[0]], 1.0f);
    pos.x += 20.0f;
    iconDispGx(&pos, 0x10, 0x1EE, 1.0f);

    value = kwp[1];
    pos.x += 20.0f;
    digits[0] = value / 10;
    digits[1] = value % 10;
    for (i = (digits[0] == 0); i < 2; i++) {
        iconDispGx(&pos, 0x10, numeric_id[digits[i]], 1.0f);
        pos.x += 20.0f;
    }

    value = kwp[2];
    pos.x = 212.0f;
    digits[0] = value / 100;
    value %= 100;
    digits[1] = value / 10;
    value %= 10;
    digits[2] = value;
    for (i = 0; i < 3; i++) {
        iconDispGx(&pos, 0x10, numeric_id[digits[i]], 1.0f);
        pos.x += 20.0f;
    }
}

USER_FUNC(kpa_status_main) {
    dispEntry(8, 1000.0f, 0, kpa_status_disp, NULL);
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_status_init) {
    EventEntry* newEvent;
    s32* args = event->args;
    s32 a0, a1, a2;
    s32 i;

    a0 = evtGetValue(event, args[0]);
    a1 = evtGetValue(event, args[1]);
    a2 = evtGetValue(event, args[2]);
    newEvent = evtEntryType(kpa_status_main_evt, event->priority, 0, event->type);
    newEvent->unitId = event->unitId;
    newEvent->ownerNPC = event->ownerNPC;
    for (i = 0; i < 16; i++) {
        newEvent->lwData[i] = event->lwData[i];
    }
    for (i = 0; i < 3; i++) {
        newEvent->lfData[i] = event->lfData[i];
    }
    newEvent->uwBase = event->uwBase;
    newEvent->ufBase = event->ufBase;
    kwp = (u8*)_mapAlloc(3);
    kwp[0] = a0;
    kwp[1] = a1;
    kwp[2] = a2;
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_set_time) {
    s32* args = event->args;
    s32 remain;

    remain = evtGetValue(event, args[0]) / 1000;
    remain = 180 - (remain + evtGetValue(event, args[1]) * 10);
    if (remain < 0) {
        remain = 0;
    }
    kwp[2] = remain;
    evtSetValue(event, args[2], remain);
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_pole_hitoff) {
    MapObjectEntry* entry = mobjNameToPtrNoAssert("pole");

    if (entry != NULL) {
        entry->flags |= 0x100;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (87.11%): only the two float literals' load order/registers differ
// (ours loads -140 into r4 base and 40 via addi+0; the original loads 40 into r4 and
// -140 via addi+0(r5)). Pure scheduling/addressing.
void kpa_stageclear_disp(CameraId cameraId, void* param) {
    GXColor color;

    FontDrawStart();
    FontDrawScale(2.0f);
    FontDrawEdge();
    color = dat_kpa_000015cc;
    FontDrawColor(&color);
    FontDrawString("ゴールイン！！", -140.0f, 40.0f);
    FontDrawEdgeOff();
}

USER_FUNC(kpa_stageclear_disp_entry) {
    dispEntry(8, 1000.0f, 0, kpa_stageclear_disp, NULL);
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_time_countdown) {
    if (kwp[2] != 0) {
        kwp[2]--;
        kpaAddScore(100);
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(kpa_get_cam_pt) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(4);

    evtSetFloat(event, args[0], cam->cameraPos.x);
    evtSetFloat(event, args[1], cam->cameraPos.y);
    evtSetFloat(event, args[2], cam->cameraPos.z);
    return EVT_RETURN_DONE;
}
// clang-format on
