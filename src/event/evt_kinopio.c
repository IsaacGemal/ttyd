#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/npcdrv.h"
#include "driver/animdrv.h"
#include "driver/itemdrv.h"
#include "driver/camdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include <string.h>
#include "evt/evt_kinopio.h"
#include "evt/evt_cam.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "party/party_motion.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "math_ext.h"
static USER_FUNC(paper_off);  // local to this file in the original
static USER_FUNC(paper_on);  // local to this file in the original


// itemEntry is defined in driver/itemdrv.c but not declared in any header yet.

// shared .sdata2 constants (named by their original addresses)
__declspec(section ".sdata2") char str_mario[8] = "mario";
__declspec(section ".sdata2") f32 float_6p2832_8041bc70 = 6.2831855f;
__declspec(section ".sdata2") f32 float_90_8041bc74 = 90.0f;
__declspec(section ".sdata2") f32 float_360_8041bc78 = 360.0f;
__declspec(section ".sdata2") char str_M_S_1[8] = "M_S_1";

// force_active: these strings are referenced only through the unit's other data (and by other units), the linker would drop the rest.
#pragma force_active on
// shared .rodata string pool (named by their original addresses)
const char str_BGM_FF_GO_SLEEPING1[0x14] = "BGM_FF_GO_SLEEPING1";
const char str_802ECE64[0x14] = "\217h\211\256\202\314\203J\203\201\203\211\203}\203\223";
const char str_c_jugem_c[0x0C] = "c_jugem_c";
const char str_802ECE84[0x0C] = "\203J\203\201\203\211\203}\203\223";
const char str_bom_02[0x08] = "bom_02";
const char str_gor_01[0x08] = "gor_01";
const char str_muj_01[0x08] = "muj_01";
const char str_nok_00[0x08] = "nok_00";
const char str_pik_04[0x08] = "pik_04";
const char str_usu_00[0x08] = "usu_00";
const char str_confetti[0x0C] = "confetti";
const char str_SFX_VOICE_MARIO_FIND1[0x18] = "SFX_VOICE_MARIO_FIND1";
const char str_SFX_EVT_50_HOTEL_SHUTTER1[0x1C] = "SFX_EVT_50_HOTEL_SHUTTER1";
const char str_breakfast[0x0C] = "breakfast";
const char str_p_slit[0x08] = "p_slit";
const char str_PM_S_3A[0x08] = "PM_S_3A";
#pragma force_active reset

// event function prototypes referenced by the script tables

// script tables (defined before the functions so the string pool order matches)
extern s32 evt_50[];
EVT_BEGIN(kinopio_house_talk_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(get_ryoukin, LW(10))
    CALL(get_npcname, LW(11))
    CALL(evt_win_coin_on, 0, LW(12))
    CALL(msg_no, 0, LW(0))
    CALL(evt_msg_print_insert, 0, LW(0), 0, LW(11), LW(10))
    CALL(msg_no, 6, LW(0))
    CALL(evt_msg_select, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(msg_no, 2, LW(0))
        CALL(evt_msg_print_add, 0, LW(0))
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_win_coin_off, LW(12))
        RETURN
    END_IF
    CALL(power_chk, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(msg_no, 1, LW(0))
        CALL(evt_msg_print_add, 0, LW(0))
        CALL(msg_no, 6, LW(0))
        CALL(evt_msg_select, 0, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(msg_no, 2, LW(0))
            CALL(evt_msg_print_add, 0, LW(0))
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_win_coin_off, LW(12))
            RETURN
        END_IF
    END_IF
    SET_VALUE(LF(0), 0)
    CALL(evt_pouch_check_item, 164, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(msg_no, 7, LW(0))
        CALL(evt_msg_print_add, 0, LW(0))
        CALL(msg_no, 8, LW(0))
        CALL(evt_msg_select, 0, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LF(0), 1)
            CALL(evt_pouch_remove_item, 164, LW(0))
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(coin_chk, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(msg_no, 4, LW(0))
            CALL(evt_msg_print_add, 0, LW(0))
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_win_coin_off, LW(12))
            RETURN
        END_IF
        CALL(get_ryoukin, LW(10))
        MULTIPLY(LW(10), -1)
        CALL(evt_pouch_add_coin, LW(10))
        WAIT_MS(1000)
    END_IF
    CALL(evt_win_coin_off, LW(12))
    CALL(msg_no, 3, LW(0))
    CALL(evt_msg_print_add, 0, LW(0))
    CALL(get_npcname, LW(0))
    CALL(evt_npc_flag_onoff, 1, LW(0), 32)
    CALL(evt_mario_get_party, LW(11))
    CALL(evt_mario_get_exparty, LW(12))
    CALL(evt_mario_get_pos, 0, LW(13), LW(14), LW(15))
    CALL(evt_mario_goodbye_party, 1)
    CALL(evt_mario_goodbye_party, 0)
    CALL(get_to_bed_evt, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(1000)
            CALL(get_cam, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
            CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 0, 11)
        END_INLINE_EVENT
        CALL(get_bedside, LW(0), LW(1), LW(2))
        CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(90.0f))
    ELSE
        BEGIN_INLINE_EVENT_GET_ID(LW(0))
            CALL_CHILD(LW(0))
        END_INLINE_EVENT
        LOOP(0)
            IS_EVENT_RUNNING(LW(0), LW(1))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(get_cam, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
        CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 0, 11)
        WAIT_MS(500)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, 131230, LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_flag_onoff, 1, 1792)
    CALL(get_bed, LW(0), LW(1), LW(2))
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), FLOAT(400.0f), 0, -1000)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_mario_adjust_dir)
    WAIT_MS(200)
    CALL(evt_mario_dispflag_onoff, 1, 4096)
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        CALL(get_beddeg, LW(0))
        CALL(paper_on, LW(0))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, 132026, LW(0), LW(1), LW(2), 0)
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, 132027, LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(snd_off)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, PTR((char*)str_BGM_FF_GO_SLEEPING1))
    CALL(evt_fade_entry, 4, 3000, 0, 0, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_fade_set_spot_pos, LW(0), LW(1), LW(2))
    CALL(evt_fade_end_wait)
    CALL(evt_fade_entry, 12, 0, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_pouch_mario_recovery)
    CALL(evt_pouch_all_party_recovery)
    CALL(breakfast)
    WAIT_MS(3000)
    DELETE_EVENT(LW(10))
    CALL(paper_off)
    CALL(evt_mario_dispflag_onoff, 0, 4096)
    IF_INT_NOT_EQUAL(LW(11), 0)
        CALL(get_bed, LW(0), LW(1), LW(2))
        CALL(evt_mario_set_party_pos, 0, LW(11), LW(0), LW(1), LW(2))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), 0)
        CALL(get_bed, LW(0), LW(1), LW(2))
        CALL(offset, FLOAT(30.0f), LW(3), LW(4))
        ADD_FLOAT(LW(0), LW(3))
        ADD_FLOAT(LW(2), LW(4))
        CALL(evt_mario_set_party_pos, 1, LW(12), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_mario_flag_onoff, 0, 1792)
    CALL(get_bedside, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(get_npcname, LW(0))
    CALL(evt_set_dir_to_target, LW(0), PTR(str_mario))
    CALL(evt_mario_set_dir_npc, LW(0))
    CALL(evt_party_set_dir_npc, 0, LW(0))
    CALL(evt_party_set_dir_npc, 1, LW(0))
    CALL(evt_npc_flag_onoff, 1, LW(0), 32768)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_npc_flag_onoff, 0, LW(0), 32768)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(snd_on)
    ADD(GSW(1), 1)
    IF_INT_GREATER(GSW(1), 100)
        SET_VALUE(GSW(1), 100)
    END_IF
    IF_INT_EQUAL(GSW(1), 50)
        CALL_CHILD(evt_50)
        CALL(get_npcname, LW(0))
        CALL(evt_npc_flag_onoff, 0, LW(0), 32)
        RETURN
    END_IF
    CALL(evt_mario_adjust_dir)
    CALL(evt_fade_entry, 11, 1000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(get_from_bed_evt, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        BEGIN_INLINE_EVENT_GET_ID(LW(10))
            CALL_CHILD(LW(0))
        END_INLINE_EVENT
        LOOP(0)
            IS_EVENT_RUNNING(LW(10), LW(11))
            IF_INT_EQUAL(LW(11), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(get_npctalk, LW(0), LW(1), LW(2))
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(90.0f))
    CALL(evt_fade_end_wait)
    CALL(get_npcname, LW(11))
    CALL(evt_mario_set_dir_npc, LW(11))
    CALL(evt_party_set_dir_npc, 0, LW(11))
    CALL(get_npcname, LW(11))
    CALL(msg_no, 5, LW(0))
    CALL(evt_msg_print, 0, LW(0), 0, LW(11))
    CALL(get_npcname, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 32)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_50)
    CALL(get_npcname, LW(10))
    CALL(evt_npc_get_position, LW(10), LW(13), LW(14), LW(15))
    CALL(evt_npc_entry, PTR((char*)str_802ECE64), PTR((char*)str_c_jugem_c))
    CALL(evt_npc_set_tribe, PTR((char*)str_802ECE64), PTR((char*)str_802ECE84))
    CALL(N_cameraman_on)
    CALL(evt_npc_flag_onoff, 1, PTR((char*)str_802ECE64), 32768)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), PTR((char*)str_bom_02))
        CALL(evt_npc_set_position, PTR((char*)str_802ECE64), -365, 30, -170)
    END_IF
    IF_STR_EQUAL(LW(0), PTR((char*)str_gor_01))
        CALL(evt_npc_set_position, PTR((char*)str_802ECE64), 168, 154, -229)
    END_IF
    IF_STR_EQUAL(LW(0), PTR((char*)str_muj_01))
        CALL(evt_npc_set_position, PTR((char*)str_802ECE64), 270, 20, -145)
    END_IF
    IF_STR_EQUAL(LW(0), PTR((char*)str_nok_00))
        CALL(evt_npc_set_position, PTR((char*)str_802ECE64), -385, 25, -145)
    END_IF
    IF_STR_EQUAL(LW(0), PTR((char*)str_pik_04))
        CALL(evt_npc_set_position, PTR((char*)str_802ECE64), 310, 1329, -100)
        CALL(evt_npc_set_position, LW(10), 310, 1329, -65)
    END_IF
    IF_STR_EQUAL(LW(0), PTR((char*)str_usu_00))
        CALL(evt_npc_set_position, PTR((char*)str_802ECE64), 268, 0, -210)
    END_IF
    CALL(evt_set_dir_to_target, PTR((char*)str_802ECE64), PTR(str_mario))
    CALL(evt_set_dir_to_target, LW(10), PTR(str_mario))
    CALL(evt_mario_set_dir_npc, PTR((char*)str_802ECE64))
    CALL(evt_party_set_dir_npc, 0, PTR((char*)str_802ECE64))
    CALL(msg_no, 9, LW(0))
    CALL(evt_msg_print, 0, LW(0), 0, LW(10))
    CALL(get_bed, LW(0), LW(1), LW(2))
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(evt_party_set_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_fade_entry, 9, 1000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 100)
    CALL(evt_eff, 0, PTR((char*)str_confetti), 3, LW(0), LW(1), LW(2), 600, 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, PTR((char*)str_SFX_VOICE_MARIO_FIND1), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(msg_no, 10, LW(0))
    CALL(evt_msg_print, 0, LW(0), 0, LW(10))
    CALL(get_bedside, LW(0), LW(1), LW(2))
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(90.0f))
    CALL(get_npctalk, LW(0), LW(1), LW(2))
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(90.0f))
    CALL(evt_mario_set_dir_npc, LW(10))
    CALL(msg_no, 11, LW(0))
    CALL(evt_msg_print, 0, LW(0), 0, LW(10))
    CALL(evt_mario_set_dir_npc, PTR((char*)str_802ECE64))
    CALL(evt_party_set_dir_npc, 0, PTR((char*)str_802ECE64))
    WAIT_MS(300)
    CALL(evt_snd_sfxon, PTR((char*)str_SFX_EVT_50_HOTEL_SHUTTER1), 0)
    CALL(evt_fade_entry, 8, 150, 255, 255, 255)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    CALL(evt_fade_set_spot_pos, LW(0), LW(1), LW(2))
    CALL(evt_fade_end_wait)
    CALL(evt_fade_entry, 7, 150, 255, 255, 255)
    CALL(evt_fade_end_wait)
    CALL(msg_no, 12, LW(0))
    CALL(evt_msg_print, 0, LW(0), 0, LW(10))
    CALL(evt_mario_set_dir_npc, LW(10))
    CALL(evt_party_set_dir_npc, 0, LW(10))
    WAIT_MS(300)
    CALL(evt_sub_get_coin, 200)
    CALL(msg_no, 13, LW(0))
    CALL(evt_msg_print, 0, LW(0), 0, LW(10))
    CALL(evt_fade_entry, 10, 1000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_npc_check_delete, PTR((char*)str_802ECE64))
    CALL(evt_npc_set_position, LW(10), LW(13), LW(14), LW(15))
    CALL(evt_set_dir_to_target, LW(10), PTR(str_mario))
    CALL(evt_fade_entry, 9, 1000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()




static KinopioData* work;
static KinopioData** wp = &work;
USER_FUNC(evt_kinopio_setup) {
    KinopioData* data = (KinopioData*)evtGetValue(event, event->args[0]);
    NpcEntry* npc = npcNameToPtr(data->npcname);

    if (animGroupBaseAsync(str_p_slit, 0, NULL) == 0) {
        return 0;
    }
    npc->talkEvt = kinopio_house_talk_evt;
    *wp = data;
    return EVT_RETURN_DONE;
}

USER_FUNC(msg_no) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], (s32)(*wp)->msgs[id]);
    return EVT_RETURN_DONE;
}

USER_FUNC(power_chk) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    BOOL full = TRUE;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (pouch->partyData[i].flags & 1) {
            if (pouch->partyData[i].currentHP != pouch->partyData[i].maximumHP) {
                full = FALSE;
                break;
            }
        }
    }
    if (pouchGetHP() == pouchGetMaxHP() && pouchGetFP() == pouchGetMaxFP() && pouchGetAP() == pouchGetMaxAP() && full) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(coin_chk) {
    s32* args = event->args;

    if (pouchGetCoin() < (*wp)->ryoukin) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(get_ryoukin) {
    evtSetValue(event, event->args[0], (*wp)->ryoukin);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_npcname) {
    evtSetValue(event, event->args[0], (s32)(*wp)->npcname);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_bed) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->bedX);
    evtSetFloat(event, args[1], (*wp)->bedY);
    evtSetFloat(event, args[2], (*wp)->bedZ);
    return EVT_RETURN_DONE;
}
USER_FUNC(get_bedside) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->bedsideX);
    evtSetFloat(event, args[1], (*wp)->bedsideY);
    evtSetFloat(event, args[2], (*wp)->bedsideZ);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_beddeg) {
    evtSetFloat(event, event->args[0], (*wp)->beddeg);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_npctalk) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->npctalkX);
    evtSetFloat(event, args[1], (*wp)->npctalkY);
    evtSetFloat(event, args[2], (*wp)->npctalkZ);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_cam) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->camX);
    evtSetFloat(event, args[1], (*wp)->camY);
    evtSetFloat(event, args[2], (*wp)->camZ);
    evtSetFloat(event, args[3], (*wp)->camX2);
    evtSetFloat(event, args[4], (*wp)->camY2);
    evtSetFloat(event, args[5], (*wp)->camZ2);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_to_bed_evt) {
    evtSetValue(event, event->args[0], (s32)(*wp)->to_bed_evt);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_from_bed_evt) {
    evtSetValue(event, event->args[0], (s32)(*wp)->from_bed_evt);
    return EVT_RETURN_DONE;
}

static USER_FUNC(paper_on) {
    s32* args = event->args;
    const char* str = str_BGM_FF_GO_SLEEPING1;
    f32 val = evtGetFloat(event, *args++);
    MarioWork* mario = marioGetPtr();

    if (isFirstCall) {
        marioPaperOn(str + 0xBC);
        marioChgPose(str_M_S_1);
        marioChgPaper(str + 0xC4);
        event->userdata[0] = 0;
    }
    if (event->userdata[0] < 0x1E) {
        if (strcmp(gp->currentMapName, str + 0x60) == 0) {
            f32 deg = float_6p2832_8041bc70 * val / float_360_8041bc78;
            f32 c;
            f32 s;

            c = cos(deg);
            mario->position.x -= 0.2f * c;
            s = sin(deg);
            mario->position.z -= 0.2f * s;
        } else {
            f32 deg = float_6p2832_8041bc70 * val / float_360_8041bc78;
            f32 c;
            f32 s;

            c = cos(deg);
            mario->position.x += 0.2f * c;
            s = sin(deg);
            mario->position.z += 0.2f * s;
        }
        event->userdata[0]++;
    }
    mario->dispReqDir = val;
    mario->dispDir = val;
    return 0;
}


static USER_FUNC(paper_off) {
    marioPaperOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(offset) {
    s32* args = event->args;
    f32 val = evtGetFloat(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 deg;
    f32 s;
    f32 c;

    deg = cam->viewYaw;
    deg = float_90_8041bc74 + deg;
    deg = float_6p2832_8041bc70 * deg / float_360_8041bc78;
    s = sin(deg);
    evtSetFloat(event, args[1], val * -s);
    c = cos(deg);
    evtSetFloat(event, args[2], val * c);
    return EVT_RETURN_DONE;
}

USER_FUNC(breakfast) {
    ItemEntry* item = itemNameToPtr(str_breakfast);
    if (item == NULL || !(item->flags & 1)) {
        if ((*wp)->breakfastItem != 0) {
            itemEntry(str_breakfast, (*wp)->breakfastItem, 0x10, (*wp)->breakfastX, (*wp)->breakfastY, (*wp)->breakfastZ, -1, 0);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(snd_off) {
    psndSetFlag(0x80);
    return EVT_RETURN_DONE;
}

USER_FUNC(snd_on) {
    psndClearFlag(0x80);
    return EVT_RETURN_DONE;
}

USER_FUNC(N_cameraman_on) {
    animPoseSetMaterialFlagOn(npcNameToPtr(str_802ECE64)->poseId, 0x1800);
    return EVT_RETURN_DONE;
}

// 4-byte zero words that end the unit's .data, .sdata and .sbss: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8033CA64_data = 0;
__declspec(section ".sdata") u32 gap_07_8041083C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
#pragma force_active on
u32 gap_08_8041894C_sbss;
#pragma force_active reset
