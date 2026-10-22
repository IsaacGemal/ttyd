#include "battle/battle_break_slot.h"

#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_message.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_eff.h"
#include "battle/battle_disp.h"
#include "eff/eff_break.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "manager/evtmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include "math_ext.h"
#include <stdlib.h>
#include <string.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803AB118[2] = { 0xBF800000, 0x3F800000 };


typedef struct SlotCards8 {
    s32 v[8];
} SlotCards8;

typedef struct SlotCards5 {
    s32 v[5];
} SlotCards5;

const char str_recovery[] = "recovery";

// The original has these constants in .sdata2. They are declared extern const in the unit's header and defined
// at the end of the file: a const object defined before its use is folded into MWCC's own literal pool, while an
// extern object defined after the code is a real object the compiler loads by name (and, being const, can hoist).

u16 tex_id_list_small[] = { '+', '-', '/', '.', ',', 0 };
u16 tex_id_list_big[] = { '0', '2', '4', '3', '1', 0 };

// 0x8 bytes
__declspec(section ".sdata") EVT_BEGIN(star_effect)
    RETURN
EVT_END()

// 0x24 bytes
__declspec(section ".sdata") EVT_BEGIN(msg_hp_max)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -205, STRING("msg_slot_HP_max"), 120)
    RETURN
EVT_END()

// 0x24 bytes
__declspec(section ".sdata") EVT_BEGIN(msg_fp_max)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -205, STRING("msg_slot_FP_max"), 120)
    RETURN
EVT_END()

// 0x24 bytes
__declspec(section ".sdata") EVT_BEGIN(msg_star_max)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -205, STRING("msg_slot_SP_max"), 120)
    RETURN
EVT_END()

// 0x24 bytes
__declspec(section ".sdata") EVT_BEGIN(msg_super_max)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -205, STRING("msg_slot_HPFPSP_max"), 120)
    RETURN
EVT_END()

// 0x24 bytes
__declspec(section ".sdata") EVT_BEGIN(msg_fault)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -205, STRING("msg_slot_HPFPSP_half"), 120)
    RETURN
EVT_END()

// trailing 4-byte gap the original has at the end of this unit's .sdata
#pragma explicit_zero_data on
__declspec(section ".sdata") u32 gap_07_80414144_sdata[1] = { 0 };
#pragma explicit_zero_data reset
const SlotCards8 R_slot_1_cards = { { 0, 1, 2, 1, 2, 0, 3, 4 } };
const SlotCards8 R_slot_2_cards = { { 3, 1, 0, 1, 4, 2, 0, 2 } };
const SlotCards5 R_slot_3_cards = { { 0, 1, 2, 3, 4 } };

// .data objects that follow the lbl_803AB118 base pair in the original unit; the original
// addresses kinoko/flower/shine/doku_kinoko through lbl_803AB118 (r27), so all of them must be
// defined here, in this order. The four non-lbl pairs are local ($) in the original, so they are
// static here (the same names are globals in ac_air_gauge.c) and force_active keeps them emitted.
#pragma force_active on
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803AB128[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_803AB138[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803AB148[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
#pragma force_active reset

__declspec(section ".data") EVT_BEGIN(kinoko_effect)
    CALL(btlevtcmd_GetUnitId, -3, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_GetMaxHp, LW(3), LW(5))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 50)
    CALL(evt_eff, PTR((void*)&zero_80420520), PTR((void*)str_recovery), 4, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), LW(5))
    CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), LW(4))
    CALL(btlevtcmd_GetUnitId, -4, LW(3))
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckStatus, LW(3), 27, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_GetMaxHp, LW(3), LW(5))
            CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
            ADD(LW(1), 50)
            CALL(evt_eff, PTR((void*)&zero_80420520), PTR((void*)str_recovery), 4, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
            CALL(btlevtcmd_RecoverHp, LW(3), LW(4), LW(5))
            CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), LW(4))
        END_IF
    END_IF
    RETURN
EVT_END()

__declspec(section ".data") EVT_BEGIN(flower_effect)
    CALL(btlevtcmd_GetUnitId, -3, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_GetMaxFp, LW(3), LW(5))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 50)
    CALL(evt_eff, PTR((void*)&zero_80420520), PTR((void*)str_recovery), 5, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_RecoverFp, LW(3), LW(4), LW(5))
    RETURN
EVT_END()

__declspec(section ".data") EVT_BEGIN(shine_effect)
    CALL(btlevtcmd_GetUnitId, -3, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_GetMaxHp, LW(3), LW(5))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 50)
    CALL(evt_eff, PTR((void*)&zero_80420520), PTR((void*)str_recovery), 4, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), LW(5))
    CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), LW(4))
    CALL(btlevtcmd_GetUnitId, -4, LW(3))
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckStatus, LW(3), 27, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_GetMaxHp, LW(3), LW(5))
            CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
            ADD(LW(1), 50)
            CALL(evt_eff, PTR((void*)&zero_80420520), PTR((void*)str_recovery), 4, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
            CALL(btlevtcmd_RecoverHp, LW(3), LW(4), LW(5))
            CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), LW(4))
        END_IF
    END_IF
    WAIT_FRAMES(60)
    CALL(btlevtcmd_GetUnitId, -3, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_GetMaxFp, LW(3), LW(5))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 50)
    CALL(evt_eff, PTR((void*)&zero_80420520), PTR((void*)str_recovery), 5, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_RecoverFp, LW(3), LW(4), LW(5))
    RETURN
EVT_END()

// TODO: type (0xc0 bytes)
__declspec(section ".data") u32 weapon_doku_kinoko[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x00000000, 0x00000000, (u32)weaponGetPowerHPHalf,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100160, 0x20000000, 0x00000302,
    0x00000000, 0x00020048, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};
// TODO: type (0xc0 bytes)
__declspec(section ".data") u32 weapon_doku_kinoko_fp[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)weaponGetFPPowerFPHalf, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100160, 0x20000000, 0x00000302,
    0x00000000, 0x00020048, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};
__declspec(section ".data") EVT_BEGIN(doku_kinoko_effect)
    CALL(btlevtcmd_GetUnitId, -3, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_CheckDamage, -3, LW(3), LW(4), PTR(weapon_doku_kinoko), 0, LW(5))
    CALL(btlevtcmd_GetUnitId, -4, LW(3))
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckStatus, LW(3), 27, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_CheckDamage, -3, LW(3), LW(4), PTR(weapon_doku_kinoko), 256, LW(5))
        END_IF
    END_IF
    WAIT_FRAMES(60)
    CALL(btlevtcmd_GetUnitId, -3, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_CheckDamage, -3, LW(3), LW(4), PTR(weapon_doku_kinoko_fp), 256, LW(5))
    RETURN
EVT_END()

BattleBreakSlot* BattleBreakSlotGetPtr(void) {
    return &_battleWorkPointer->breakSlot;
}
BattleBreakSlotReel* BattleBreakSlotReelGetPtr(s32 index) {
    return &BattleBreakSlotGetPtr()->reels[index];
}
void BattleBreakSlot_Init(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel;
    SlotCards8 tmp1 = R_slot_1_cards;
    SlotCards8 tmp2 = R_slot_2_cards;
    SlotCards5 tmp3 = R_slot_3_cards;
    s32 i;

    memset(slot, 0, 0x134);
    slot->state = 0;
    for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++, reel++) {
        reel->type = i;
        reel->state = 0;
    }
    reel = BattleBreakSlotReelGetPtr(0);
    reel->count = 8;
    reel->cards = BattleAlloc(reel->count * 4);
    memcpy(reel->cards, &tmp1, reel->count * 4);
    reel = BattleBreakSlotReelGetPtr(1);
    reel->count = 8;
    reel->cards = BattleAlloc(reel->count * 4);
    memcpy(reel->cards, &tmp2, reel->count * 4);
    reel = BattleBreakSlotReelGetPtr(2);
    reel->count = 5;
    reel->cards = BattleAlloc(reel->count * 4);
    memcpy(reel->cards, &tmp3, reel->count * 4);
}
// 100% (5612 B). Reaching it needed two things outside the function body:
// - the effect scripts (kinoko/flower/shine/doku_kinoko) and the .data pair padding are defined in this
//   file's .data, so evtEntry addresses them through the lbl_803AB118 base register (r27);
// - BattleBreakSlotDispReel is compiled before BattleBreakSlot_Main (it uses the int-to-float double
//   first), which pools that double at 0x270, before Main's SFX strings; the file's functions must be in
//   the original's reverse-asm order for that.
void BattleBreakSlot_Main(void) {
    static f32 updown;
    BattleWork* wp = _battleWorkPointer;
    BattleWorkField19068* force = &wp->field_19068;
    BattleAudience* audience = BattleAudienceBaseGetPtr();
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel;
    s32 n;
    s32 i;
    s32 j;
    f32 v;

    BattleGetMarioPtr(wp);
    BattleGetPartyPtr(wp);
    switch (slot->state) {
    case 0:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 2) {
                n++;
            }
            reel++;
        }
        if (n == 2 && BattleBreakSlotReelGetPtr(0)->value != BattleBreakSlotReelGetPtr(1)->value) {
            slot->state = 1;
            slot->timer = 0;
        }
        break;
    case 1:
        if (slot->timer == 60) {
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 2; i++) {
                reel->state = 3;
                reel->stateTimer = 0;
                reel++;
            }
        }
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 0) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 8;
            slot->counter = 0;
        }
        slot->timer++;
        break;
    case 2:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 6 || reel->state == 10) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 3;
            slot->timer = 0;
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                if (reel->state == 6) {
                    reel->state = 7;
                }
                reel++;
            }
            slot->sound = psndSFXOn("SFX_BTL_SLOT_START1");
        }
        break;
    case 3:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 10) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 4;
            slot->timer = 0;
            psndSFXOff(slot->sound);
        }
        if ((keyGetButtonTrg(0) & 0x100) && slot->timer >= 15) {
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                if (reel->state == 8) {
                    reel->state = 9;
                    reel->scrollDelta = float_neg1_80420550 * (float_0p2_80420554 + (float_0p15_80420558 * rand()) / float_32767_8042055c);
                    reel->scrollDelta = float_neg1_80420550;
                    break;
                }
                reel++;
            }
        }
        slot->timer++;
        break;
    case 4:
        if (slot->timer == 20) {
            n = 0;
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 2; i++) {
                if (reel[0].value == reel[1].value) {
                    n++;
                }
                reel++;
            }
            if (n >= 2) {
                slot->state = 5;
                psndSFXOn("SFX_BTL_SLOT_SUCCESS1");
                for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                    reel->state = 11;
                    reel->stateTimer = 0;
                    reel++;
                }
            } else {
                slot->state = 7;
                psndSFXOn("SFX_BTL_SYS_BREAK_FAILRE1");
                for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                    reel->state = 12;
                    reel->stateTimer = 0;
                    reel++;
                }
            }
        }
        slot->timer++;
        break;
    case 5:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 0) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 6;
            slot->timer = 0;
        }
        break;
    case 6:
        if (slot->timer == 0) {
            if (BattleBreakSlotReelGetPtr(0)->value != 4) {
                effBreakEntry(0, 240, float_0_8042053c, 80.0f, float_0_8042053c);
            } else {
                slot->timer = 101;
                BattleAudienceSoundBooing();
            }
        }
        if (slot->timer == 20 || slot->timer == 40 || slot->timer == 60 || slot->timer == 80 ||
            slot->timer == 100 || slot->timer == 120) {
            psndSFXOn("SFX_BTL_SYS_BREAK1");
            btl_camera_shake_h(0, 10, 0, 5.0f, float_0_8042053c);
        }
        if (slot->timer == 120) {
            BattleAudienceSoundCheer(240, 240);
        }
        if (slot->timer == 130) {
            switch (BattleBreakSlotReelGetPtr(0)->value) {
            case 0:
                evtEntry(kinoko_effect, 0, 0x20);
                slot->breakTurn = 4;
                slot->field_14 = 2.0f;
                break;
            case 1:
                evtEntry(flower_effect, 0, 0x20);
                slot->breakTurn = 4;
                slot->field_14 = 2.0f;
                break;
            case 2:
                evtEntry(star_effect, 0, 0x20);
                pouchSetAP(pouchGetMaxAP());
                psndSFXOn("SFX_HP_RECOVER_BIG1");
                slot->breakTurn = 4;
                slot->field_14 = 2.0f;
                break;
            case 3:
                evtEntry(shine_effect, 0, 0x20);
                pouchSetAP(pouchGetMaxAP());
                slot->breakTurn = 6;
                slot->field_14 = 3.0f;
                break;
            case 4:
                evtEntry(doku_kinoko_effect, 0, 0x20);
                pouchSetAP(pouchGetAP() / 2);
                break;
            }
        }
        if (slot->timer == 140) {
            switch (BattleBreakSlotReelGetPtr(0)->value) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                btl_camera_set_mode(0, 6);
                btl_camera_set_moveSpeedLv(0, 2);
                break;
            }
            if (_battleWorkPointer->flags & 0x100) {
                btl_camera_set_mode(0, 0);
            }
        }
        if (slot->timer == 180) {
            updown = float_0_8042053c;
            switch (BattleBreakSlotReelGetPtr(0)->value) {
            case 0:
            case 1:
            case 2:
                updown = BattleAudienceAddAudienceNum(audience->mMaxAudience / 2);
                break;
            case 3:
                updown = BattleAudienceAddAudienceNum(audience->mMaxAudience);
                break;
            case 4:
                updown = BattleAudienceAddAudienceNum(-audience->mMaxAudience);
                break;
            }
            if (_battleWorkPointer->flags & 0x100) {
                updown = float_0_8042053c;
            }
        }
        if (slot->timer == 209 && float_0_8042053c != updown) {
            BattleAudienceSoundRun(0);
        }
        if (slot->timer == 210) {
            if (!(_battleWorkPointer->cameraWork.flags & 2)) {
                btl_camera_shake_h(0, 1000, 0, 2.0f, 2.0f);
            }
            for (j = 0; j < 200; j++) {
                if ((u8)BattleAudience_GetExist(j) == 1 &&
                    (BattleAudienceGetPtr(j)->status == 11 || BattleAudienceGetPtr(j)->status == 12)) {
                    slot->timer--;
                    break;
                }
            }
            if (j == 200) {
                btl_camera_noshake(0);
                BattleAudienceSoundStop(13);
            }
        }
#ifdef MUST_MATCH
        // retail tests j here, the audience loop's counter, which is only assigned on a timer == 210 frame: on the
        // frame-220 call it reads an uninitialised local (rule 7: kept only under MUST_MATCH)
        if (slot->timer == 220 && j == 200) {
#else
        if (slot->timer == 220) {
#endif
            btl_camera_set_mode(0, 0);
            btl_camera_noshake(0);
        }
        if (slot->timer == 270) {
            BattleAudienceAddTargetNum(float_0_8042053c, updown);
            slot->state = 8;
            slot->flags &= ~4;
            if (gp->language == 0) {
                switch (BattleBreakSlotReelGetPtr(0)->value) {
                case 0:
                    slot->event = evtEntry(msg_hp_max, 0, 0x20);
                    break;
                case 1:
                    slot->event = evtEntry(msg_fp_max, 0, 0x20);
                    break;
                case 2:
                    slot->event = evtEntry(msg_star_max, 0, 0x20);
                    break;
                case 3:
                    slot->event = evtEntry(msg_super_max, 0, 0x20);
                    break;
                case 4:
                    slot->event = evtEntry(msg_fault, 0, 0x20);
                    break;
                }
            }
        }
        slot->timer++;
        break;
    case 7:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 0) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 8;
        }
        break;
    case 8:
        slot->state = 0;
        break;
    }

    for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++, reel++) {
        switch (reel->state) {
        case 0:
            reel->flags &= ~1;
            reel->alpha = 0xFF;
            break;
        case 1:
            if (i != 2) {
                reel->flags |= 1;
            }
            reel->stateTimer++;
            if (reel->stateTimer <= 5) {
                v = intplGetValue(0, reel->stateTimer, 5, 110.0f, 142.0f);
                reel->pos = (Vec){ float_42_80420534 * i + float_226_80420530, v, 0.0f };
                reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
                reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->alpha = 0xFF;
            } else {
                v = intplGetValue(4, reel->stateTimer - 5, 20, 142.0f, 174.0f);
                reel->pos = (Vec){ float_42_80420534 * i + float_226_80420530, v, 0.0f };
                reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
                reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->alpha = 0xFF;
            }
            if (reel->stateTimer >= 25) {
                reel->state = 2;
                reel->index++;
                if (reel->index >= reel->count) {
                    reel->index -= reel->count;
                }
                reel->value = reel->cards[reel->index];
                reel->pos = (Vec){ float_42_80420534 * i + float_226_80420530, 142.0f, 0.0f };
            }
            break;
        case 2:
            reel->pos = (Vec){ float_42_80420534 * i + float_226_80420530, 142.0f, 0.0f };
            reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->flags & 2) {
                reel->flags &= ~2;
                reel->state = 3;
                reel->stateTimer = 0;
            }
            break;
        case 3:
            v = intplGetValue(0, reel->stateTimer, 20, 142.0f, 174.0f);
            reel->pos = (Vec){ float_42_80420534 * i + float_226_80420530, v, 0.0f };
            reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->stateTimer++;
            if (reel->stateTimer > 20) {
                reel->state = 0;
                reel->stateTimer = 0;
                reel->flags &= ~1;
            }
            break;
        case 4:
            v = intplGetValue(0, reel->stateTimer, 20, 142.0f, 174.0f);
            reel->pos = (Vec){ float_42_80420534 * i + float_226_80420530, v, 0.0f };
            reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->stateTimer++;
            if (reel->stateTimer > 20) {
                reel->state = 5;
                reel->pos = (Vec){ (f32)(i * 128 - 128), -128.0f, 0.0f };
                reel->stateTimer = 0;
            }
            break;
        case 5:
            reel->flags |= 1;
            v = intplGetValue(0, reel->stateTimer, 40, float_neg128_80420548, float_0_8042053c);
            reel->pos = (Vec){ (f32)(i * 128 - 128), v, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->stateTimer++;
            if (reel->stateTimer > 40) {
                if (reel->type == 2) {
                    reel->state = 6;
                    reel->stateTimer = 0;
                } else {
                    reel->state = 10;
                    reel->stateTimer = 0;
                }
            }
            break;
        case 6:
            break;
        case 7:
            reel->scrollStep = intplGetValue(0, reel->stateTimer, 20, float_0_8042053c, 22.5f);
            reel->pos = (Vec){ 128.0f * i + float_neg128_80420548, reel->pos.y + reel->scrollStep, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->pos.y >= 128.0f) {
                reel->pos.y -= 128.0f;
                reel->index++;
                if (reel->index >= reel->count) {
                    reel->index = 0;
                }
                reel->value = reel->cards[reel->index];
            }
            reel->stateTimer++;
            if (reel->stateTimer > 20) {
                reel->state = 8;
                reel->stateTimer = 0;
            }
            break;
        case 8:
            reel->scrollStep = 22.5f;
            reel->pos = (Vec){ 128.0f * i + float_neg128_80420548, reel->pos.y + reel->scrollStep, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->pos.y >= 128.0f) {
                reel->pos.y -= 128.0f;
                reel->index++;
                if (reel->index >= reel->count) {
                    reel->index = 0;
                }
                reel->value = reel->cards[reel->index];
            }
            break;
        case 9:
            reel->scrollStep += reel->scrollDelta;
            if (reel->scrollStep < 20.0f) {
                reel->scrollStep = 20.0f;
            }
            reel->pos = (Vec){ 128.0f * i + float_neg128_80420548, reel->pos.y + reel->scrollStep, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->scrollStep >= float_0_8042053c && reel->pos.y >= 128.0f) {
                reel->pos.y -= 128.0f;
                reel->index++;
                if (reel->index == reel->count) {
                    reel->index = 0;
                }
                reel->value = reel->cards[reel->index];
                if ((audience->flags & 0x4000) && BattleBreakSlotReelGetPtr(0)->value == 4 &&
                    BattleBreakSlotReelGetPtr(1)->value == 4 && reel->value == 4) {
                    break;
                }
                if ((slot->flags & 2) && BattleBreakSlotReelGetPtr(0)->value == 4 &&
                    BattleBreakSlotReelGetPtr(1)->value == 4 && reel->value == 4) {
                    break;
                }
                if ((slot->flags & 1) && reel->value != 3) {
                    break;
                }
                if ((slot->flags & 4) && reel->value != 3) {
                    break;
                }
                if ((force->forceFlags & 0x80000) && reel->value != force->forceSlotValue) {
                    break;
                }
                reel->pos.y = float_0_8042053c;
                reel->state = 10;
                reel->stateTimer = 0;
                reel->scrollDelta = reel->scrollStep;
            }
            break;
        case 10:
            if (i == 2) {
                reel->stateTimer++;
                reel->scrollStep = intplGetValue(0, reel->stateTimer, 20, reel->scrollStep, float_0_8042053c);
                reel->pos.y = reel->scrollStep * (f32)sin(3.141592f * (2.0f * reel->stateTimer) / 8.0f);
            }
            break;
        case 11:
            reel->scrollStep = float_0_8042053c;
            reel->pos.y = float_0_8042053c;
            v = intplGetValue(0, reel->stateTimer, 60, 1.0f, 2.0f);
            reel->scale = (Vec){ v, v, 1.0f };
            reel->alpha = intplGetValue(0, reel->stateTimer, 60, 255.0f, float_0_8042053c);
            reel->stateTimer++;
            if (reel->stateTimer > 60) {
                reel->state = 13;
                reel->flags &= ~1;
            }
            break;
        case 12:
            reel->scrollStep = float_0_8042053c;
            reel->pos.y = float_0_8042053c;
            v = intplGetValue(0, reel->stateTimer, 40, float_0_8042053c, 128.0f);
            reel->pos = (Vec){ 128.0f * i + float_neg128_80420548, v, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->stateTimer++;
            if (reel->stateTimer > 40) {
                reel->state = 13;
                reel->flags &= ~1;
            }
            break;
        case 13:
            reel->state = 0;
            break;
        }
    }
}
// i->f32 pools an anonymous 2^52 double instead of the original's named double_to_int_802fd930; the value
// and instructions are identical, only the pool label differs (not scored).
void BattleBreakSlot_Disp(void) {
    dispEntry(CAMERA_2D, float_490_8042054c, 0, BattleBreakSlotDispReel, NULL);
}
__declspec(section ".sdata2") const f32 zero_80420520 = 0.0f;

void BattleBreakSlotDispReel(CameraId camId, void* param) {
    s32 i;
    BattleBreakSlotReel* reel;
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxPos;
    Mtx mtxNeg;
    Mtx mtxRot;
    Mtx mtxRotX;
    Mtx mtxRotY;
    Mtx mtxRotZ;
    Mtx mtxFinal;
    u16* texList;
    u16 tex;

    i = 0;
    {
        BattleBreakSlotReel* reel1 = BattleBreakSlotReelGetPtr(0);

        for (; i < 3; i++) {
            if ((reel1->flags & 1) && reel1->state >= 1 && reel1->state <= 4) {
                PSMTXTrans(mtxTrans, float_42_80420534 * (f32)i + float_226_80420530, float_122_80420538, float_0_8042053c);
                {
                    GXColor color = {0xFF, 0xFF, 0xFF, 0xD0};

                    iconDispGxCol(mtxTrans, 0x10, 0x1D4, color);
                }
            }
            reel1++;
        }
    }
    reel = BattleBreakSlotReelGetPtr(0);
    if (reel->state >= 1 && reel->state <= 4) {
        GXSetScissor(0, 0x52, 0x260, 0x20);
        texList = tex_id_list_small;
    } else {
        texList = tex_id_list_big;
        if (reel->state != 0xB) {
            GXSetScissor(0, 0xB0, 0x260, 0x80);
        }
    }
    i = 0;
    reel = BattleBreakSlotReelGetPtr(0);
    for (; i < 3; i++) {
        if (reel->flags & 1) {
            GXSetBlendMode(1, 4, 5, 0);
            GXSetZCompLoc(1);
            GXSetAlphaCompare(7, 0, 0, 7, 0);
            GXSetZMode(1, 3, 0);
            PSMTXTrans(mtxTrans, reel->pos.x, reel->pos.y, reel->pos.z);
            PSMTXScale(mtxScale, reel->scale.x, reel->scale.y, reel->scale.z);
            PSMTXRotRad(mtxRotX, 'x', float_deg2rad_80420540 * reel->rot.x);
            PSMTXRotRad(mtxRotY, 'y', float_deg2rad_80420540 * reel->rot.y);
            PSMTXRotRad(mtxRotZ, 'z', float_deg2rad_80420540 * reel->rot.z);
            PSMTXConcat(mtxRotX, mtxRotZ, mtxRot);
            PSMTXConcat(mtxRotY, mtxRot, mtxRot);
            PSMTXTrans(mtxPos, reel->pivot.x, reel->pivot.y, reel->pivot.z);
            PSMTXTrans(mtxNeg, -reel->pivot.x, -reel->pivot.y, -reel->pivot.z);
            PSMTXConcat(mtxRot, mtxPos, mtxFinal);
            PSMTXConcat(mtxNeg, mtxFinal, mtxFinal);
            PSMTXConcat(mtxScale, mtxFinal, mtxFinal);
            PSMTXConcat(mtxTrans, mtxFinal, mtxFinal);
            {
                u16 tex0 = texList[reel->value];
                btlDispTexPlane2(mtxFinal, tex0, (GXColor){0xFF, 0xFF, 0xFF, reel->alpha}, 0);
            }
            if (reel->state <= 0xA && reel->state != 3 && reel->state != 4) {
                s32 idx = reel->index + 1;

                if (idx >= reel->count) {
                    idx = 0;
                }
                tex = texList[reel->cards[idx]];
                if (reel->state >= 1 && reel->state <= 4) {
                    PSMTXTrans(mtxTrans, float_0_8042053c, float_neg32_80420544, float_0_8042053c);
                } else {
                    PSMTXTrans(mtxTrans, float_0_8042053c, float_neg128_80420548, float_0_8042053c);
                }
                PSMTXConcat(mtxTrans, mtxFinal, mtxFinal);
                {
                    btlDispTexPlane2(mtxFinal, tex, (GXColor){0xFF, 0xFF, 0xFF, reel->alpha}, 0);
                }
            }
        }
        reel++;
    }
    GXSetScissor(0, 0, gp->fbWidth, gp->efbHeight);
}
void BattleBreakSlot_End(void) {
	//stubbed in retail
}
void BattleBreakSlot_PointInc(void) {
    BattleWorkField19068* force = &_battleWorkPointer->field_19068;
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel;
    PouchData* pouch;

    pouch = pouchGetPtr();
    if (pouch->mStarPowersObtained == 0) {
        return;
    }
    reel = BattleBreakSlotReelGetPtr(slot->counter);
    reel->state = 1;
    reel->stateTimer = 0;
    slot->counter++;
    if (slot->counter > 3) {
        slot->counter = 3;
        return;
    }
    switch (reel->type) {
    case 0:
        reel->index = irand(reel->count) - 1;
        if (reel->index == -1) {
            reel->index = reel->count - 1;
        }
        reel->value = reel->cards[reel->index];
        if (force->forceFlags & 0x80000) {
            item_appear_force(reel, force->forceSlotValue);
        }
        if (slot->flags & 1) {
            item_appear_force(reel, 3);
        }
        if (slot->flags & 4) {
            item_appear_force(reel, 3);
        }
        break;
    case 1:
        reel->index = irand(reel->count) - 1;
        if (reel->index == -1) {
            reel->index = reel->count - 1;
        }
        reel->value = reel->cards[reel->index];
        if (force->forceFlags & 0x80000) {
            item_appear_force(reel, force->forceSlotValue);
        }
        if (slot->flags & 1) {
            item_appear_force(reel, 3);
        }
        if (slot->flags & 4) {
            item_appear_force(reel, 3);
        }
        break;
    case 2: {
        BattleBreakSlotReel* reel2;

        do {
            reel2 = BattleBreakSlotReelGetPtr(1);
            reel->index = irand(reel->count);
            reel->value = reel->cards[reel->index];
        } while (reel->value == reel2->value);
        break;
    }
    }
}

void BattleBreakSlot_Start(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();

    if (slot->counter >= 3) {
        BattleBreakSlotReel* reel;
        s32 i;

        slot->counter = 0;
        slot->state = 2;
        reel = BattleBreakSlotReelGetPtr(0);
        for (i = 0; i < 3; i++) {
            reel->state = 4;
            reel->stateTimer = 0;
            reel++;
        }
    }
}

u8 BattleBreakSlot_CheckReaction(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();

    BattleBreakSlot_Start();
    if (slot->state >= 1 && slot->state <= 8) {
        return 1;
    }
    if (slot->event != NULL && evtCheckID(slot->event->eventId) != 0) {
        return 1;
    }
    btl_camera_set_mode(0, 0);
    btl_camera_set_moveSpeedLv(0, 2);
    BattleAudienceNumToTarget();
    slot->event = NULL;
    return 0;
}
s32 BattleBreakSlot_GetBreakTurn(void) {
    return BattleBreakSlotGetPtr()->breakTurn;
}
void BattleBreakSlot_DecBreakTurn(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();

    slot->breakTurn--;
    if (slot->breakTurn < 0) {
        slot->breakTurn = 0;
    }
}
void BattleBreakSlot_HideReel(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel = BattleBreakSlotReelGetPtr(0);
    s32 i;

    for (i = 0; i < 3; i++) {
        reel->flags |= 2;
        reel++;
    }
    slot->counter = 0;
}
void item_appear_force(BattleBreakSlotReel* reel, s32 id) {
    s32 i;

    for (i = 0; i < reel->count; i++) {
        if (reel->cards[i] == id) {
            reel->index = i - 1;
        }
        if (reel->index == -1) {
            reel->index = reel->count - 1;
        }
    }
}

// .sdata2 constants, in the original's address order after zero_80420520 and BattleBreakSlotDispReel's
// colour literals. Defined after the code so the compiler emits them as real objects ahead of its own
// pooled literals instead of folding them.
#pragma explicit_zero_data on
__declspec(section ".sdata2") const f32 float_226_80420530 = 226.0f;
__declspec(section ".sdata2") const f32 float_42_80420534 = 42.0f;
__declspec(section ".sdata2") const f32 float_122_80420538 = 122.0f;
__declspec(section ".sdata2") const f32 float_0_8042053c = 0.0f;
__declspec(section ".sdata2") const f32 float_deg2rad_80420540 = 0.017453292f;
__declspec(section ".sdata2") const f32 float_neg32_80420544 = -32.0f;
__declspec(section ".sdata2") const f32 float_neg128_80420548 = -128.0f;
__declspec(section ".sdata2") const f32 float_490_8042054c = 490.0f;
__declspec(section ".sdata2") const f32 float_neg1_80420550 = -1.0f;
__declspec(section ".sdata2") const f32 float_0p2_80420554 = 0.2f;
__declspec(section ".sdata2") const f32 float_0p15_80420558 = 0.15f;
__declspec(section ".sdata2") const f32 float_32767_8042055c = 32767.0f;
#pragma explicit_zero_data reset
