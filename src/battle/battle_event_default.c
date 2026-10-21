#include "battle/battle_event_default.h"
#include "battle/battle.h"
#include "battle/battle_information.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle_ac_help.h"
#include "battle/battle_damage.h"
#include "eff/eff_fire.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_message.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"

// Event functions of other units, named here without a header.
extern s32 btlevtcmd_AnimeChangePaperAnime[];
extern s32 btlevtcmd_AnimeChangePaperGroup[];
extern s32 btlevtcmd_GetConfuseActEvent[];
extern s32 btlevtcmd_GuardMoveEnd[];
extern s32 btlevtcmd_SetGuard[];
extern s32 btlevtcmd_WaitAttackEndCheck[];

//=== added by agent: local prototypes for undeclared externals ===


extern s32 _event_MajinaiEnd[];
extern s32 badgeWeapon_DefenceCommand[];
extern s32 badgeWeapon_DefenceCommandP[];
extern s32 btldefaultevt_avoid_guard[];
extern s32 evt_batstage_light_set_player_off[];
extern s32 evt_batstage_light_set_player_on[];
extern s32 subset_ultra_hammer_acrobat_damage_event[];
extern s32 subsetevt_binta_damage_left[];
extern s32 subsetevt_binta_damage_right[];
extern s32 subsetevt_blow_dead[];
extern s32 subsetevt_terror[];
static s32 _anime_load(EventEntry* event, BOOL isFirstCall);
static s32 _anime_load_wait(EventEntry* event, BOOL isFirstCall);
s32 btldefaultevt_guard_move[85];
s32 btldefaultevt_guard_return[98];

// clang-format off
USER_FUNC(_backfire) {
    BattleUnit* unit;
    BattleWork* wp = _battleWorkPointer;
    effFireData* data;
    s32 id;
    s32 result;

    id = evtGetValue(event, event->args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    if (!unit) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        f32 x, y, z;

        BtlUnit_GetPos(unit, &x, &y, &z);
        event->userdata[0] = (s32)effFireEntry(0, 0, x, y, z, 1.0f);
        event->userdata[1] = 0;
    }
    data = ((EffectEntry*)event->userdata[0])->userdata;
    event->userdata[1] = event->userdata[1] + 1;
    if (event->userdata[1] >= 60) {
        effDelete((EffectEntry*)event->userdata[0]);
        BattleDamageDirect(-5, unit, 0, 1, 0, 0x118, 0, 1);
        result = BattleTransID(event, event->unitId);
        if (result != id) {
            return EVT_RETURN_DONE;
        }
        return 0xFF;
    }
    data->scale = 1.6f * sinfd((f32)(event->userdata[1] * 3)) + 0.4f;
    return EVT_RETURN_BLOCK;
}


static s32 _anime_load(EventEntry* event, BOOL isFirstCall) {
    BattlePartyAnimeLoad(_battleWorkPointer->commandMenu.partyChangeId);
    return EVT_RETURN_DONE;
}


static s32 _anime_load_wait(EventEntry* event, BOOL isFirstCall) {
    return BattlePartyAnimeLoad(_battleWorkPointer->commandMenu.partyChangeId) ? EVT_RETURN_DONE
                                                                            : EVT_RETURN_BLOCK;
}


USER_FUNC(_check_pose_stay) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 arg0 = event->args[0];
    s32 id;
    s32 result;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(wp, id);
    result = 0;
    if (unit->currentKind == 0xDE) {
        result = 1;
        if (wp->badgeEquippedFlags & 1) {
            if (!(unit->mTokenFlags & 0x10)) {
                result = 0;
            }
        }
    }
    evtSetValue(event, arg0, result);
    return EVT_RETURN_DONE;
}


USER_FUNC(btlevtcmd_audience_code_escape) {
    BattleAudience_Case_Escape();
    return EVT_RETURN_DONE;
}


USER_FUNC(btlevtcmd_audience_code_escape_success) {
    BattleAudience_Case_Escape_Good();
    return EVT_RETURN_DONE;
}


USER_FUNC(btlevtcmd_audience_code_escape_fail) {
    BattleAudience_Case_Escape_Bad();
    return EVT_RETURN_DONE;
}


USER_FUNC(_btl_escape_reset_move_color_lv_player_unit_all) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;

    unit = BattleGetMarioPtr(wp);
    unit->mMoveColorLv = 0xFF;
    unit = BattleGetPartyPtr(wp);
    if (unit) {
        unit->mMoveColorLv = 0xFF;
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_set_escape_ac_hlp_msg) {
    BattleAcHelpSetHelp("msg_ac_escape");
    BattleAcHelpSetDispType(2);
    return EVT_RETURN_DONE;
}


USER_FUNC(_get_escape_rate) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;

    evtSetValue(event, event->args[0],
                info->setup->groupSetup->enemyData->mUnitKindParams->escapeRate);
    return EVT_RETURN_DONE;
}


USER_FUNC(_get_heart_suitoru_point) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 id;
    s32 arg1;
    s32 value;

    id = args[0];
    arg1 = args[1];

    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    value = 0;
    if (unit) {
        value = unit->badgesEquipped.HpDrain;
        if (value > unit->field_0x280) {
            value = unit->field_0x280;
        }
    }
    evtSetValue(event, arg1, value);
    return EVT_RETURN_DONE;
}


USER_FUNC(_get_flower_suitoru_point) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 id;
    s32 arg1;
    s32 value;

    id = args[0];
    arg1 = args[1];

    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    value = 0;
    if (unit) {
        value = unit->badgesEquipped.FpDrain;
        if (value > unit->field_0x280) {
            value = unit->field_0x280;
        }
    }
    evtSetValue(event, arg1, value);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(btldefaultevt_Damage)
    CALL(btlevtcmd_SetRotate, LW(10), 0, 0, 0)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 32768, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_OnOffStatus, LW(10), 9, 0, 0, 0)
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_eff64, STRING(""), STRING("freeze_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 1, 0, 0, 0, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), 39)
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
        END_SIBLING_EVENT
        LABEL(0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), 39)
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
        END_SIBLING_EVENT
        LABEL(1)
        CALL(btlevtcmd_SetDispOffset, LW(10), 2, 0, 0)
        WAIT_FRAMES(2)
        CALL(btlevtcmd_SetDispOffset, LW(10), 0, 0, 0)
        WAIT_FRAMES(2)
        CALL(btlevtcmd_WaitAttackEndCheck, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            GOTO(1)
        END_IF
    END_IF
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 20, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), 5)
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitAttackEnd)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 7, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 54)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 54)
        END_IF
    END_IF
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 18, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(subset_ultra_hammer_acrobat_damage_event)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 262144, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 31)
        WAIT_FRAMES(30)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(31)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 37)
        GOTO(10)
        CASE_EQUAL(32)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 38)
        GOTO(10)
        CASE_EQUAL(33)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 39)
        GOTO(10)
        CASE_EQUAL(34)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 40)
        GOTO(10)
        CASE_EQUAL(35)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 41)
        GOTO(10)
        CASE_EQUAL(36)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 42)
        GOTO(10)
        CASE_EQUAL(37)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 43)
        GOTO(10)
        CASE_EQUAL(42)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 47)
        GOTO(10)
        CASE_EQUAL(38)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 45)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 45)
        ELSE
            CALL(evt_snd_sfxon, STRING("SFX_SYSTEM_LUCKY1"), 0)
        END_IF
        GOTO(10)
        CASE_EQUAL(39)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 44)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 44)
        ELSE
            CALL(btlevtcmd_CheckCommandUnit, LW(10), LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(evt_snd_sfxon, STRING("SFX_MARIO_AVOID1"), 0)
            ELSE
                CALL(evt_snd_sfxon, STRING("SFX_BTL_ATTACK_MISS1"), 0)
            END_IF
            CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 48)
            CALL(btlevtcmd_AnimeChangePaperGroup, LW(10), LW(11), STRING("p_b_st"), 1)
            CALL(btlevtcmd_CheckStatus, LW(10), 15, LW(2))
            IF_INT_NOT_EQUAL(LW(2), 0)
                CALL(btlevtcmd_AnimeChangePaperAnime, LW(10), LW(11), STRING("PM_F_3A"))
                CALL(btlevtcmd_ftomsec, 37, LW(0))
                WAIT_MS(LW(0))
                CALL(btlevtcmd_AnimeChangePaperAnime, LW(10), LW(11), STRING("PM_F_3B"))
                CALL(btlevtcmd_ftomsec, 22, LW(0))
                WAIT_MS(LW(0))
            ELSE
                CALL(btlevtcmd_CheckStatus, LW(10), 14, LW(2))
                IF_INT_NOT_EQUAL(LW(2), 0)
                    CALL(btlevtcmd_AnimeChangePaperAnime, LW(10), LW(11), STRING("PM_Y_2A"))
                    CALL(btlevtcmd_ftomsec, 40, LW(0))
                    WAIT_MS(LW(0))
                    CALL(btlevtcmd_AnimeChangePaperAnime, LW(10), LW(11), STRING("PM_Y_2B"))
                    CALL(btlevtcmd_ftomsec, 65, LW(0))
                    WAIT_MS(LW(0))
                ELSE
                    CALL(btlevtcmd_AnimeChangePaperAnime, LW(10), LW(11), STRING("PM_Y_2A"))
                    CALL(btlevtcmd_ftomsec, 40, LW(0))
                    WAIT_MS(LW(0))
                    CALL(btlevtcmd_AnimeChangePaperAnime, LW(10), LW(11), STRING("PM_Y_2B"))
                    CALL(btlevtcmd_ftomsec, 65, LW(0))
                    WAIT_MS(LW(0))
                END_IF
            END_IF
        END_IF
        GOTO(10)
        CASE_EQUAL(40)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 46)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 46)
        ELSE
        END_IF
    END_SWITCH
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 21, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), 39)
        CALL_CHILD(subsetevt_binta_damage_left)
        CALL(btlevtcmd_WaitAttackEnd)
        RETURN
    END_IF
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 22, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), 39)
        CALL_CHILD(subsetevt_binta_damage_right)
        CALL(btlevtcmd_WaitAttackEnd)
        RETURN
    END_IF
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(57)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 48)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 48)
        END_IF
        CALL(btlevtcmd_StartWaitEvent, LW(10))
        RETURN
        CASE_OR_EQUAL(30)
        CASE_OR_EQUAL(27)
        CASE_OR_EQUAL(26)
        CASE_OR_EQUAL(23)
        CALL(btlevtcmd_OnUnitFlag, LW(10), 1)
        SET_VALUE(LW(14), 39)
        SET_VALUE(LW(15), -1)
        CALL(btlevtcmd_CheckDamagePattern, LW(10), 1, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(14), 38)
        END_IF
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
        END_SIBLING_EVENT
        CALL_CHILD(subsetevt_damage)
        CASE_END
        CASE_EQUAL(22)
        CALL(btlevtcmd_CheckDefinePosetype, LW(10), LW(11), 35, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(14), 35)
            CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
            END_SIBLING_EVENT
            WAIT_FRAMES(45)
        ELSE
            WAIT_FRAMES(45)
        END_IF
        CASE_EQUAL(24)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 11)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 11)
        ELSE
            CALL(btlevtcmd_OnUnitFlag, LW(10), 1)
            SET_VALUE(LW(14), 29)
            SET_VALUE(LW(15), -1)
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
            END_SIBLING_EVENT
            CALL_CHILD(subsetevt_damage)
        END_IF
        CASE_EQUAL(25)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 12)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 12)
        ELSE
            CALL(btlevtcmd_OnUnitFlag, LW(10), 1)
            SET_VALUE(LW(14), 29)
            SET_VALUE(LW(15), -1)
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
            END_SIBLING_EVENT
            CALL_CHILD(subsetevt_damage)
        END_IF
        CASE_EQUAL(43)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 24)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 24)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(44)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 25)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 25)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(45)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 26)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 26)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(46)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 27)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 27)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(47)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 28)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 28)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(48)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 29)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 29)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(49)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 30)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 30)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(50)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 31)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 31)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(51)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 32)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 32)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(52)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 33)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 33)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(53)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 34)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 34)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(54)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 35)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 35)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(55)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 36)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 36)
        ELSE
            CALL_CHILD(subsetevt_counter_damage)
        END_IF
        CASE_EQUAL(41)
        CALL_CHILD(btldefaultevt_avoid_guard)
        RETURN
        CASE_EQUAL(56)
        CALL_CHILD(btldefaultevt_guard_move)
        CASE_EQUAL(28)
        CALL_CHILD(subsetevt_terror)
        RETURN
        CASE_EQUAL(29)
        CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(2), 58)
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 58)
        ELSE
            CALL_CHILD(subsetevt_blow_dead)
        END_IF
        RETURN
        CASE_DEFAULT
    END_SWITCH
    LABEL(10)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        WAIT_FRAMES(30)
        CALL_CHILD(subsetevt_dead)
        RETURN
    ELSE
        CALL(btlevtcmd_CheckDamageCode, LW(10), 512, LW(1))
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 15)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 16384, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(_backfire, LW(10))
        RETURN
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 4096, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL_CHILD(btldefaultevt_guard_return)
        RETURN
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(1))
    IF_INT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_WaitAttackEnd)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_CheckAttribute, LW(10), 4, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, LW(10), LW(5), LW(6), LW(7))
        CALL(btlevtcmd_GetHomePos, LW(10), LW(2), LW(3), LW(4))
        IF_INT_EQUAL(LW(2), LW(5))
            IF_INT_EQUAL(LW(3), LW(6))
                IF_INT_EQUAL(LW(4), LW(7))
                    GOTO(101)
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(14), 42)
        CALL(btlevtcmd_SetMoveSpeed, LW(10), FLOAT(4.0f))
        CALL(btlevtcmd_GetHomePos, LW(10), LW(2), LW(3), LW(4))
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
        IF_INT_GREATER(LW(7), 0)
            CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
            CALL(btlevtcmd_DivePosition, LW(10), LW(2), LW(3), LW(4), 0, 0, 0, 0, -1)
        END_IF
        LABEL(101)
    ELSE
        CALL(btlevtcmd_GetPos, LW(10), LW(5), LW(6), LW(7))
        CALL(btlevtcmd_GetHomePos, LW(10), LW(2), LW(3), LW(4))
        IF_INT_EQUAL(LW(2), LW(5))
            IF_INT_EQUAL(LW(3), LW(6))
                IF_INT_EQUAL(LW(4), LW(7))
                    GOTO(102)
                END_IF
            END_IF
        END_IF
        SET_VALUE(LW(14), 40)
        CALL(btlevtcmd_SetMoveSpeed, LW(10), FLOAT(4.0f))
        CALL(btlevtcmd_GetHomePos, LW(10), LW(2), LW(3), LW(4))
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
        IF_INT_GREATER(LW(7), 0)
            CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
            CALL(btlevtcmd_MovePosition, LW(10), LW(2), LW(3), LW(4), 0, -1, 0)
        END_IF
        LABEL(102)
    END_IF
    CALL(btlevtcmd_ResetFaceDirection, LW(10))
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_ChangeParty)
    CALL(_anime_load)
    CALL(btlevtcmd_GetPartyId, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_SetFallAccel, LW(3), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMarioId, -2, LW(8))
    CALL(btlevtcmd_GetPos, LW(8), LW(5), LW(6), LW(7))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetScale, LW(3), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        LOOP(18)
            CALL(btlevtcmd_AddScale, LW(3), FLOAT(-0.05f), FLOAT(-0.05f), FLOAT(-0.05f))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_PARTY_IN1"), 0)
    CALL(btlevtcmd_JumpPosition, LW(3), LW(5), LW(6), LW(7), 20, 0)
    CALL(_anime_load_wait)
    CALL(btlevtcmd_ChangeParty, LW(9))
    CALL(btlevtcmd_OffUnitFlag, LW(9), 2097152)
    CALL(btlevtcmd_GetBodyId, LW(9), LW(0))
    CALL(btlevtcmd_AnimeChangePoseFromTable, LW(9), LW(0))
    CALL(btlevtcmd_SetPos, LW(9), LW(5), LW(6), LW(7))
    CALL(btlevtcmd_SetFallAccel, LW(9), FLOAT(0.7f))
    CALL(btlevtcmd_CheckDataOfDataTable, LW(9), LW(0), 4)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_RunDataEventChild, LW(9), 4)
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_BTL_PARTY_CHANGE1"), 0)
    CALL(evt_audience_sound_cheer, 45, 25)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetScale, LW(9), FLOAT(0.1f), FLOAT(0.1f), FLOAT(0.1f))
        LOOP(18)
            CALL(btlevtcmd_AddScale, LW(9), FLOAT(0.05f), FLOAT(0.05f), FLOAT(0.05f))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetScale, LW(9), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHomePos, LW(9), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_PARTY_OUT1"), 0)
    CALL(btlevtcmd_JumpPosition, LW(9), LW(0), LW(1), LW(2), 20, 0)
    CALL(btlevtcmd_OnUnitFlag, LW(9), 2097152)
    CALL(btlevtcmd_KillUnit, LW(3), 0)
    CALL(btlevtcmd_SortPhaseMoveTable)
    CALL(_check_pose_stay, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, -2)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_BiribiriMove)
    CALL(evt_sub_intpl_init, 8, 0, 5, 16)
    LABEL(0)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetDispOffset, -2, LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_guard_move)
    CALL(btlevtcmd_AnimeCmpPoseType, LW(10), LW(11), 42, 40, LW(2))
    IF_INT_EQUAL(LW(2), 0)
        CALL(btlevtcmd_AnimeSetMotionSpeed, LW(10), LW(11), FLOAT(2.0f))
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 42)
    CALL(btlevtcmd_CheckAttribute, LW(10), 4, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_GetProtectId, LW(10), LW(6))
        CALL(btlevtcmd_GetHomePos, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, LW(10), LW(0), LW(1), LW(2), 20, 0, 0, 0, -1)
    ELSE
        CALL(btlevtcmd_GetProtectId, LW(10), LW(6))
        CALL(btlevtcmd_GetHomePos, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 20, -1, 0)
    END_IF
    CALL(btlevtcmd_AnimeSetMotionSpeed, LW(10), LW(11), FLOAT(1.0f))
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 46)
    CALL(btlevtcmd_GuardMoveEnd)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_guard_return)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_CheckGuardMotionEnable, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeCmpPoseType, LW(10), LW(11), 42, 40, LW(2))
        IF_INT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_AnimeSetMotionSpeed, LW(10), LW(11), FLOAT(2.0f))
        END_IF
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 42)
    END_IF
    CALL(btlevtcmd_CheckAttribute, LW(10), 4, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_GetHomePos, LW(10), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, LW(10), LW(0), LW(1), LW(2), 20, 0, 0, 0, -1)
    ELSE
        CALL(btlevtcmd_GetHomePos, LW(10), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 20, -1, 0)
    END_IF
    CALL(btlevtcmd_CheckGuardMotionEnable, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeSetMotionSpeed, LW(10), LW(11), FLOAT(1.0f))
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 45)
    END_IF
    CALL(btlevtcmd_GuardMoveEnd)
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_Appeal)
    CALL(btlevtcmd_GetBodyId, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 43)
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(3), LW(1))
        ADD(LW(1), 53)
        ADD(LW(3), 35)
        SET_VALUE(LW(4), LW(2))
        ADD(LW(2), 250)
    ELSE
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(3), LW(1))
        ADD(LW(1), 150)
        ADD(LW(3), 80)
        SET_VALUE(LW(4), LW(2))
        ADD(LW(2), 400)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, LW(0), LW(1), LW(2), LW(0), LW(3), LW(4), 30, 11)
    CALL(evt_batstage_light_set_player_on, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CheckDataOfDataTable, -2, LW(0), 5)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_RunDataEventChild, -2, 5)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_batstage_light_set_player_off)
        WAIT_FRAMES(60)
        RETURN
    END_IF
    CALL(btlevtcmd_GetBodyId, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 66)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_APPEAL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(60)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(btlevtcmd_AudienceAppeal, -2)
    CALL(evt_batstage_light_set_player_off)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    RETURN
EVT_END()

EVT_BEGIN(_Defence_core)
    CALL(btlevtcmd_GetUnitId, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, -2, LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_DEFENCE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(20)
        CALL(btlevtcmd_AddRotate, -2, 0, 36, 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_OnStatusFlag, -2, 16777216)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(4), 45)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_DEFENCE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_ftof, 60, LW(0))
    CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, STRING("btl_msg_defend_command_exec"), LW(0))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_Escape)
    CALL(evt_btl_camera_set_mode, 0, 20)
    CALL(_btl_escape_reset_move_color_lv_player_unit_all)
    CALL(btlevtcmd_AcSetDifficulty, -2, 3)
    CALL(evt_sub_random, 99, LW(0))
    CALL(evt_sub_random, 6, LW(1))
    ADD(LW(1), 30)
    CALL(_get_escape_rate, LW(2))
    CALL(btlevtcmd_AcSetParamAll, 1, 180, LW(0), LW(1), 0, 4, 6, LW(2))
    CALL(btlevtcmd_AcSetFlag, 3)
    CALL(btlevtcmd_SetupAC, -2, 5, 1, 0)
    CALL(_set_escape_ac_hlp_msg)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPartnerId, -2, LW(10))
        IF_INT_NOT_EQUAL(LW(10), -1)
            CALL(btlevtcmd_CheckActStatus, LW(10), LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_GetBodyId, LW(10), LW(11))
                CALL(btlevtcmd_CalculateFaceDirection, LW(10), LW(11), -1, -1, 260, LW(0))
                CALL(btlevtcmd_ChangeFaceDirection, LW(10), LW(0))
                CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 42)
                CALL(btlevtcmd_JumpSetting, LW(10), 20, FLOAT(0.0f), FLOAT(0.7f))
                CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 16, -1)
                CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 16, -1)
            END_IF
        END_IF
    END_INLINE_EVENT
    CALL(btlevtcmd_GetBodyId, -2, LW(11))
    CALL(btlevtcmd_CalculateFaceDirection, -2, LW(11), -1, -1, 260, LW(0))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(11), 42)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_audience_code_escape)
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_BATTLE_STAMP1"), LW(15))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 15)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 180, -1, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPartnerId, -2, LW(10))
        IF_INT_NOT_EQUAL(LW(10), -1)
            CALL(btlevtcmd_CheckActStatus, LW(10), LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_FaceDirectionSub, LW(10), LW(0), 15)
                CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 180, -1, 0)
            END_IF
        END_IF
    END_INLINE_EVENT
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(evt_snd_sfxoff, LW(15))
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_RUN_AWAY_MISS1"), 0)
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(btlevtcmd_GetPartnerId, -2, LW(10))
            IF_INT_NOT_EQUAL(LW(10), -1)
                CALL(btlevtcmd_CheckActStatus, LW(10), LW(0))
                IF_INT_NOT_EQUAL(LW(0), 0)
                    CALL(btlevtcmd_OffUnitFlag, LW(10), 2097152)
                    CALL(btlevtcmd_GetBodyId, LW(10), LW(11))
                    CALL(btlevtcmd_SetMoveSpeed, LW(10), FLOAT(10.0f))
                    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_FaceDirectionAdd, LW(10), LW(0), 30)
                    CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 0, -1, 0)
                    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 60)
                    CALL(btlevtcmd_FaceDirectionAdd, LW(10), LW(0), 5)
                    CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 5, -1, 0)
                    CALL(btlevtcmd_FaceDirectionAdd, LW(10), LW(0), 10)
                    CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 60, -1, 0)
                    WAIT_FRAMES(25)
                    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 61)
                    WAIT_FRAMES(40)
                    CALL(btlevtcmd_OnUnitFlag, LW(10), 2097152)
                END_IF
            END_IF
        END_SIBLING_EVENT
        CALL(btlevtcmd_OffUnitFlag, LW(10), 2097152)
        CALL(btlevtcmd_GetBodyId, -2, LW(11))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 30)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(11), 60)
        CALL(btlevtcmd_audience_code_escape_fail)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 5)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 5, -1, 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 60, -1, 0)
        WAIT_FRAMES(25)
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(11), 61)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_OnUnitFlag, LW(10), 2097152)
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(btlevtcmd_GetPartnerId, -2, LW(10))
            IF_INT_NOT_EQUAL(LW(10), -1)
                CALL(btlevtcmd_CheckActStatus, LW(10), LW(0))
                IF_INT_NOT_EQUAL(LW(0), 0)
                    CALL(btlevtcmd_GetBodyId, LW(10), LW(11))
                    CALL(btlevtcmd_CalculateFaceDirection, LW(10), LW(11), 0, 0, 4, LW(0))
                    CALL(btlevtcmd_ChangeFaceDirection, LW(10), LW(0))
                    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 42)
                    CALL(btlevtcmd_SetMoveSpeed, LW(10), FLOAT(3.0f))
                    CALL(btlevtcmd_GetHomePos, LW(10), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 0, -1, 0)
                    CALL(btlevtcmd_StartWaitEvent, LW(10))
                END_IF
            END_IF
        END_SIBLING_EVENT
        CALL(btlevtcmd_CalculateFaceDirection, -2, LW(11), 0, 0, 4, LW(0))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(0))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(11), 42)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_StartWaitEvent, -2)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        RETURN
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetPartnerId, -2, LW(10))
        IF_INT_NOT_EQUAL(LW(10), -1)
            CALL(btlevtcmd_CheckActStatus, LW(10), LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_SetMoveSpeed, LW(10), FLOAT(10.0f))
                CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_FaceDirectionAdd, LW(10), LW(0), 200)
                CALL(btlevtcmd_MovePosition, LW(10), LW(0), LW(1), LW(2), 0, -1, 0)
            END_IF
        END_IF
    END_SIBLING_EVENT
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_audience_code_escape_success)
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_RUN_AWAY1"), 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 200)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    SET_VALUE(LW(15), 0)
    CALL(btlevtcmd_check_battleflag, LW(0), 4194304)
    IF_INT_NOT_EQUAL(LW(0), 0)
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL_CHILD(_event_MajinaiEnd)
        END_SIBLING_EVENT
    END_IF
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
    END_IF
    CALL(btlevtcmd_InterruptStop, 0)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_Confuse)
    CALL(btlevtcmd_GetBodyId, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 5)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetConfuseActEvent, -2, LW(0))
    CALL_CHILD(LW(0))
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_CantMoveZeroGravity)
    CALL(btlevtcmd_GetBodyId, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 64)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, STRING("btl_msg_cant_move_zero_gravity"), 120)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_SuitoruBadgeEffect)
    CALL(_get_heart_suitoru_point, -2, LW(5))
    IF_INT_GREATER_OR_EQUAL(LW(5), 1)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_GetBodyId, -2, LW(4))
            CALL(btlevtcmd_GetStatusMg, -2, LW(6))
            CALL(btlevtcmd_GetHeight, -2, LW(7))
            MULTIPLY_FLOAT(LW(7), LW(6))
            CALL(btlevtcmd_GetPartsPos, -2, LW(4), LW(0), LW(1), LW(2))
            ADD(LW(1), LW(7))
            CALL(evt_eff, 0, STRING("recovery"), 0, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(120)
            CALL(btlevtcmd_RecoverHp, -2, LW(4), LW(5))
        END_SIBLING_EVENT
        WAIT_FRAMES(125)
    END_IF
    CALL(_get_flower_suitoru_point, -2, LW(5))
    IF_INT_GREATER_OR_EQUAL(LW(5), 1)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_GetStatusMg, -2, LW(6))
            CALL(btlevtcmd_GetHeight, -2, LW(7))
            MULTIPLY_FLOAT(LW(7), LW(6))
            CALL(btlevtcmd_GetBodyId, -2, LW(4))
            CALL(btlevtcmd_GetPartsPos, -2, LW(4), LW(0), LW(1), LW(2))
            ADD(LW(1), LW(7))
            CALL(evt_eff, 0, STRING("recovery"), 1, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(120)
            CALL(btlevtcmd_RecoverFp, -2, LW(4), LW(5))
        END_SIBLING_EVENT
        WAIT_FRAMES(125)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_avoid_counter_event)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 49)
    BEGIN_SIBLING_EVENT_GET_ID(LW(0))
        CALL(evt_sub_intpl_init, 5, 0, 1800, 40)
        LOOP(40)
            CALL(evt_sub_intpl_get_value_para, LW(1), LW(2))
            CALL(btlevtcmd_SetRotate, LW(10), EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(0))
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_ProtectPartner)
    CALL(btlevtcmd_GetPartnerId, -2, LW(3))
    CALL(btlevtcmd_SetGuard, -2, LW(3))
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_Dummy)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_Wait)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_LectureEnd)
    CALL(btlevtcmd_RunDataEventChild, -6, 70)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_Defend)
    SET_VALUE(LW(12), PTR(badgeWeapon_DefenceCommand))
    CALL_CHILD(_Defence_core)
    RETURN
EVT_END()

EVT_BEGIN(btldefaultevt_DefendParty)
    SET_VALUE(LW(12), PTR(badgeWeapon_DefenceCommandP))
    CALL_CHILD(_Defence_core)
    RETURN
EVT_END()
// clang-format on

// 4-byte zero words that end the unit's .data and .sdata: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8034CE8C_data = 0;
__declspec(section ".sdata") u32 gap_07_80410DF4_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
