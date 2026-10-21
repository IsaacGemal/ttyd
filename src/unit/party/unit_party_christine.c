#include "unit/party/unit_party_christine.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "battle/battle_monosiri.h"
#include "driver/msgdrv.h"
#include "mario/mario_party.h"
#include "window/win_log.h"
#include "system.h"
#include <dolphin/types.h>
#include "battle/battle_attack_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_mario.h"
#include "battle/battle_message.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "battle/battle_weapon_power.h"


extern s32 btldefaultevt_avoid_counter_event[];
extern s32 evt_audience_acrobat_notry[];
extern s32 negone_one[];
extern s32 one_negone[];
extern s32 subset_attack_ready_jump_event[];
extern s32 unk_evt_803510ec[];
static s32 krb_get_dir(EventEntry* event, BOOL isFirstCall);
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count);
static s32 init_event[51];
static s32 damage_event[16];
u32 partyWeapon_ChristineNormalAttack[48];
u32 partyWeapon_ChristineMonosiri[48];
u32 partyWeapon_ChristineRenzokuAttack[48];
u32 partyWeapon_ChristineKiss[48];
s32 partyChristineAttack_NormalAttack[824];
s32 partyChristineAttack_Monosiri[397];
s32 partyChristineAttack_RenzokuAttack[1071];
s32 partyChristineAttack_Kiss[546];
s32 christine_appeal_event[71];
static s32 party_win_reaction[14];
static s32 attack_audience_event[392];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static s32 attack_event[2];
static s32 wait_event[6];

// clang-format off

#define TECH_MENU_ENTRY(table)                                                             \
    cmd->mWeaponTable[*count].mIndex = -1;                                                 \
    cmd->mWeaponTable[*count].mItem = 0;                                                   \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                      \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                               \
    cmd->mWeaponTable[*count].mIcon =                                                      \
        cmd->mWeaponTable[*count].mWeaponParams->icon;                                     \
    cmd->mWeaponTable[*count].mName =                                                      \
        (char*)msgSearch(cmd->mWeaponTable[*count].mWeaponParams->name);                   \
    (*count)++;

static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count);
static s32 init_event[51];
static s32 damage_event[16];
u32 partyWeapon_ChristineNormalAttack[48];
u32 partyWeapon_ChristineMonosiri[48];
u32 partyWeapon_ChristineRenzokuAttack[48];
u32 partyWeapon_ChristineKiss[48];
s32 partyChristineAttack_NormalAttack[824];
s32 partyChristineAttack_Monosiri[397];
s32 partyChristineAttack_RenzokuAttack[1071];
s32 partyChristineAttack_Kiss[546];
s32 christine_appeal_event[71];
static s32 party_win_reaction[14];
static s32 attack_audience_event[392];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static s32 attack_event[2];
static s32 wait_event[6];

// clang-format off

#define TECH_MENU_ENTRY(table)                                                             \
    cmd->mWeaponTable[*count].mIndex = -1;                                                 \
    cmd->mWeaponTable[*count].mItem = 0;                                                   \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                      \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                               \
    cmd->mWeaponTable[*count].mIcon =                                                      \
        cmd->mWeaponTable[*count].mWeaponParams->icon;                                     \
    cmd->mWeaponTable[*count].mName =                                                      \
        (char*)msgSearch(cmd->mWeaponTable[*count].mWeaponParams->name);                   \
    (*count)++;

static s32 krb_get_dir(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
    s32 out;
    f32 dx;
    f32 dy;

    x0 = evtGetFloat(event, args[0]);
    y0 = evtGetFloat(event, args[1]);
    x1 = evtGetFloat(event, args[2]);
    y1 = evtGetFloat(event, args[3]);
    out = evtGetValue(event, args[4]);
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx == 0.0f && dy == 0.0f) {
        evtSetValue(event, args[4], out);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, args[4], (s32)angleABf(dx, dy, 0.0f, 0.0f));
    return EVT_RETURN_DONE;
}


static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

    TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ChristineNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ChristineMonosiri);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ChristineRenzokuAttack);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_ChristineKiss);
    }
}


USER_FUNC(btlevtcmd_get_monosiri_msg_no) {
    s32* args = event->args;
    s32 value;
    s32 out;

    value = evtGetValue(event, args[0]);
    evtGetValue(event, args[1]);
    out = args[2];
    evtSetValue(event, out,
                (s32)battleGetUnitMonosiriPtr(
                          BattleGetUnitPtr(_battleWorkPointer, value)->currentKind)
                    ->tattleBtlTag);
    return EVT_RETURN_DONE;
}


USER_FUNC(_monosiri_flag_on) {
    BattleUnit* unit;
    s32 value;

    value = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, value);
    battleSetUnitMonosiriFlag(unit->currentKind);
    return EVT_RETURN_DONE;
}


// 100%: the instruction sequence was already identical; only MWCC's callee-saved register split
// differed. The declaration order below is what makes the allocator pick the original's split
// (id1=r31, args=r30, id2 reusing r30, state=r29, work=r28, ptr reusing event's r27); declaring them
// in read order (work, args, ...) gives work=r30/args=r31/id1=r29/id2=r28. Declaration order does not
// change the emitted statement order here.
USER_FUNC(_dictionary) {
    s32 id1;
    s32* args = event->args;
    s32 id2;
    s32 state;
    s32 value;
    BattleUnit* unit;
    BattleWork* work = _battleWorkPointer;
    s32 ptr;

    id1 = BattleTransID(event, -2);
    value = evtGetValue(event, args[0]);
    id2 = BattleTransID(event, value);
    evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(work, id1);
    state = unit->work[0];
    ptr = unit->work[1];

    switch (state) {
    case 0:
        break;
    case 1:
        ptr = (s32)monosiriInit(BattleGetUnitPtr(work, id2)->currentKind, -50.f,
                                50.f);
        state = 2;
        break;
    case 2:
        monosiriMain((void*)ptr);
        state = 3;
        break;
    case 3:
        monosiriMain((void*)ptr);
        break;
    case 4:
        monosiriClose((void*)ptr);
        monosiriMain((void*)ptr);
        state = 5;
        break;
    case 5:
        if (monosiriCloseWait((void*)ptr)) {
            monosiriExit((void*)ptr);
            state = 6;
        } else {
            monosiriMain((void*)ptr);
        }
        break;
    case 6:
        break;
    }

    unit->work[0] = state;
    unit->work[1] = ptr;
    return EVT_RETURN_DONE;
}

USER_FUNC(_set_hustle) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 value;

    value = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(work, BattleTransID(event, value));
    if (unit != NULL) {
        if (unit->movesRemaining <= 0) {
            unit->movesRemaining++;
            unit->moveState = 0;
            unit->mMoveColorLv = 0xFF;
            if (unit->maxMovesThisTurn <= 0) {
                unit->maxMovesThisTurn = 1;
                if (BtlUnit_CheckStatus(unit, STATUS_SLOW)) {
                    unit->mTokenFlags |= 0x2000;
                }
            }
        } else {
            BtlUnit_OnStatusFlag(unit, 0x2000000);
        }
    }
    return EVT_RETURN_DONE;
}

__declspec(section ".data") u8 lbl_80390B70[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80390B80[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80390B90[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80390BA0[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 pose_table_christine_stay[50] = {
    0x00000009, (u32)"PKR_D_5", 0x00000002, (u32)"PKR_D_5",
    0x00000003, (u32)"PKR_X_1", 0x00000004, (u32)"PKR_D_2",
    0x00000005, (u32)"PKR_X_1", 0x00000001, (u32)"PKR_N_1",
    0x0000001B, (u32)"PKR_D_3", 0x0000001C, (u32)"PKR_S_1",
    0x00000025, (u32)"PKR_G_2", 0x0000001F, (u32)"PKR_G_1",
    0x00000027, (u32)"PKR_D_1", 0x00000028, (u32)"PKR_W_1",
    0x0000002A, (u32)"PKR_R_1", 0x00000039, (u32)"PKR_D_2",
    0x00000038, (u32)"PKR_D_2", 0x0000003C, (u32)"PKR_D_4",
    0x0000003D, (u32)"PKR_D_2", 0x00000041, (u32)"PKR_T_1",
    0x00000034, (u32)"PKR_I_1", 0x00000043, (u32)"PKR_C_1",
    0x00000042, (u32)"PKR_O_1", 0x00000040, (u32)"PKR_X_1",
    0x00000031, (u32)"PKR_A_1", 0x0000002D, (u32)"PKR_G_1",
    0x00000045, (u32)"PKR_S_1",
};

// TODO: type
static u32 data_table[16] = {
    0x00000005, (u32)&christine_appeal_event, 0x00000006, (u32)&attack_audience_event,
    0x00000001, (u32)__makeTechMenuFunc, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000040, (u32)&party_win_reaction, 0x00000007, (u32)subset_attack_ready_jump_event,
    0x0000002F, (u32)btldefaultevt_avoid_counter_event, 0x00000000, 0x00000000,
};

// TODO: type
u32 unitpartsdata_Party_Christine[19] = {
    0x00000001, (u32)"btl_un_christine", (u32)"c_pkr", 0x00000000,
    0x00000000, 0x00000000, 0x41800000, 0x41C00000,
    0x40A00000, 0x00000000, 0x41C00000, 0x00000000,
    0x00140018, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table_christine_stay,
};

// TODO: type
u32 unitdata_Party_Christine[49] = {
    0x000000E0, (u32)"btl_un_christine", 0x000A0005, 0x05010100,
    0x00000064, 0x00090014, 0x0014000A, 0x00180000,
    0x00000000, 0x41400000, 0x00000000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x00000000,
    0x41F00000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41200000,
    0x00000000, 0x00000000, 0x41200000, 0x41500000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_PARTY_DAMAGE1",
    (u32)"SFX_MARIO_DAMAGE_FIRE1", (u32)"SFX_MARIO_DAMAGE_ICE1", (u32)"SFX_MARIO_DAMAGE_BIRIBIRI1", 0x02000000,
    (u32)&regist, 0x01000000, (u32)&unitpartsdata_Party_Christine, (u32)&init_event,
    (u32)&data_table,
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_PARTY_BATTLE_SUBMOVE1L"), STRING("SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 9, 3, 0, 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 6, 12, 16)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 9, 3)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

// TODO: type
u32 partyWeapon_ChristineNormalAttack[48] = {
    (u32)"btl_wn_pkr_normal", 0x01500000, 0x00000000, (u32)"msg_pkr_normal_jump",
    0x64000000, 0x3F800000, 0x01016432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000001, 0x00000001, 0x00000002, 0x00000002,
    0x00000003, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01001260, 0x20001004, 0x00000302,
    (u32)"msg_ac_zutsuki", 0x00035007, 0x0000000C, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyChristineAttack_NormalAttack, 0x00000064, 0x00020200, 0x00000000,
};

// TODO: type
u32 partyWeapon_ChristineMonosiri[48] = {
    (u32)"btl_wn_pkr_lv1", 0x01510000, 0x00000000, (u32)"msg_pkr_monosiri",
    0x64000000, 0x3F800000, 0x01056432, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000001, 0x00000302,
    (u32)"msg_ac_monoshiri", 0x00020008, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyChristineAttack_Monosiri, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 partyWeapon_ChristineRenzokuAttack[48] = {
    (u32)"btl_wn_pkr_lv2", 0x01520000, 0x00000000, (u32)"msg_pkr_renzoku_zutsuki",
    0x64030000, 0x3F800000, 0x02056432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000001, 0x00000001, 0x00000002, 0x00000002,
    0x00000003, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01001260, 0x20001004, 0x00000302,
    (u32)"msg_ac_zutsuki", 0x00025017, 0x0000000C, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyChristineAttack_RenzokuAttack, 0x00000064, 0x00050500, 0x00050000,
};

// TODO: type
u32 partyWeapon_ChristineKiss[48] = {
    (u32)"btl_wn_pkr_lv3", 0x01530000, 0x00000000, (u32)"msg_pkr_nage_kiss",
    0x64040000, 0x3F800000, 0x01056432, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101160, 0x10000000, 0x00000302,
    (u32)"msg_ac_kiss", 0x00020108, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64020000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyChristineAttack_Kiss, 0x00000064, 0x00050500, 0x00000000,
};

EVT_BEGIN(partyChristineAttack_NormalAttack)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(14))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(14), -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    IF_INT_LESS_OR_EQUAL(LW(10), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_A_2B"))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(5)
            CALL(btlevtcmd_ACRStart, -2, 0, 4, 4, 0)
        END_SIBLING_EVENT
        WAIT_FRAMES(10)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_J_1A"))
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(15)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(15)
            LOOP(6)
                CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
                MULTIPLY(LW(0), -30)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 3, 0)
    ELSE
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        IF_INT_EQUAL(LW(6), 2)
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
            END_SIBLING_EVENT
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
                IF_INT_EQUAL(LW(15), 1)
                    SET_VALUE(LW(9), -15)
                ELSE
                    SET_VALUE(LW(9), 15)
                END_IF
                CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
                SET_VALUE(LW(8), 0)
                LOOP(36)
                    CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(9))
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
        ELSE
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
                SET_VALUE(LW(8), 0)
                LOOP(36)
                    CALL(btlevtcmd_GetPosFloat, -2, LW(12), LW(13), LW(14))
                    CALL(krb_get_dir, LW(5), LW(6), LW(12), LW(13), LW(8))
                    CALL(btlevtcmd_GetRotate, -2, LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_SetRotate, -2, LW(0), LW(1), LW(8))
                    SET_FLOAT_VALUE(LW(5), LW(12))
                    SET_FLOAT_VALUE(LW(6), LW(13))
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
        END_IF
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        SET_VALUE(LW(8), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(8))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 0, 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_S_1"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 30)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
        WAIT_MS(500)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        IF_INT_EQUAL(LW(10), 0)
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        END_IF
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetResultCountAC, LW(6))
    IF_INT_LESS_OR_EQUAL(LW(6), 1)
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
            CALL(evt_btl_camera_set_zoom, 0, 200)
            WAIT_FRAMES(10)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            CALL(evt_btl_camera_set_zoom, 0, 250)
        END_INLINE_EVENT
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131072, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        ADD(LW(10), 1)
        GOTO(10)
    ELSE
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_GetResultAC, LW(0))
    END_IF
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(6))
        IF_INT_LESS(LW(6), 0)
            MULTIPLY(LW(6), -1)
        END_IF
        CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
        MULTIPLY(LW(6), LW(0))
        DIVIDE(LW(6), 15)
        SET_VALUE(LW(7), 15)
        LABEL(20)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(6))
        SUBTRACT(LW(7), 1)
        WAIT_FRAMES(1)
        IF_INT_GREATER_OR_EQUAL(LW(7), 1)
            GOTO(20)
        END_IF
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(christine_dictionary_event)
    LABEL(0)
    CALL(_dictionary, LW(3), LW(4))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 6)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(partyChristineAttack_Monosiri)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(evt_btl_camera_set_posoffset, 0, 40, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_2A_1"))
    WAIT_MS(500)
    LOOP(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_2A_2"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_LEARNED1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(400)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_2A_3"))
        WAIT_MS(350)
    END_LOOP
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, LW(3), LW(4), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 21, 1, 0)
    BEGIN_SIBLING_EVENT
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_posoffset, 0, 0, 0, 0)
    END_SIBLING_EVENT
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_SET(LW(6), 2)
        CALL(btlevtcmd_GetResultPrizeLv, -5, 0, LW(6))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    ELSE
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 2, 0, LW(0), LW(1), LW(2), 30, 0, 56)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_FAIL_LEARNED1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_2A_4"))
        WAIT_FRAMES(56)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_LEARNED2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_2A_4"))
    WAIT_FRAMES(3)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(53)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ftof, 0, LW(0))
    CALL(btlevtcmd_ftof, 53, LW(1))
    CALL(btlevtcmd_ACRStart, -2, LW(0), LW(1), LW(1), 0)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_GREATER_OR_EQUAL(LW(6), 2)
        DELETE_EVENT(LW(15))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_Y_1"))
        CALL(btlevtcmd_ftomsec, 28, LW(0))
        WAIT_MS(LW(0))
    ELSE
        CALL(evt_audience_acrobat_notry)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
    END_IF
    CALL(btlevtcmd_SetTalkPose, -2, STRING("PKR_2A_5"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("PKR_2A_5"))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    SPAWN_EVENT(christine_dictionary_event)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    LABEL(10)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 3)
        WAIT_FRAMES(1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_get_monosiri_msg_no, LW(3), LW(4), LW(0))
    CALL(evt_msg_print, 2, LW(0), 0, -2)
    CALL(_monosiri_flag_on, LW(3))
    CALL(btlevtcmd_SetTalkPose, -2, STRING("PKR_T_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("PKR_S_1"))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 4)
    LABEL(20)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(20)
    END_IF
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyChristineAttack_RenzokuAttack)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 2)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_A_2B"))
    CALL(btlevtcmd_ACRStart, -2, 0, 10, 10, 0)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_J_1A"))
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(15)
            LOOP(6)
                CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
                MULTIPLY(LW(0), -30)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_INLINE_EVENT
            WAIT_FRAMES(15)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_INLINE_EVENT
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 3, 0)
    ELSE
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        SET_VALUE(LW(8), 40)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(8))
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
        BEGIN_SIBLING_EVENT
            IF_INT_GREATER_OR_EQUAL(LW(6), 2)
                WAIT_FRAMES(20)
            END_IF
            CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
            SET_VALUE(LW(15), 0)
            LOOP(LW(8))
                CALL(btlevtcmd_GetPosFloat, -2, LW(12), LW(13), LW(14))
                CALL(krb_get_dir, LW(5), LW(6), LW(12), LW(13), LW(15))
                CALL(btlevtcmd_GetRotate, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(9))
                ADD(LW(15), LW(9))
                CALL(btlevtcmd_SetRotate, -2, LW(0), LW(1), LW(15))
                SET_FLOAT_VALUE(LW(5), LW(12))
                SET_FLOAT_VALUE(LW(6), LW(13))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_INLINE_EVENT
            IF_INT_GREATER_OR_EQUAL(LW(6), 2)
                WAIT_FRAMES(20)
            END_IF
            WAIT_FRAMES(15)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_INLINE_EVENT
        IF_INT_GREATER_OR_EQUAL(LW(6), 2)
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
                MULTIPLY(LW(5), -18)
                LOOP(20)
                    CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(5))
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, 0)
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                CALL(evt_eff64, STRING(""), STRING("kemuri2_n64"), 2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
            END_SIBLING_EVENT
            SET_VALUE(LW(9), LW(8))
            SUBTRACT(LW(9), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 2, LW(9))
            BEGIN_SIBLING_EVENT
                LOOP(LW(9))
                    WAIT_FRAMES(1)
                    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
                    SUBTRACT(LW(0), 1)
                    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
                END_LOOP
            END_SIBLING_EVENT
            BEGIN_SIBLING_EVENT_GET_ID(LW(15))
                LOOP(1)
                    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
                    IF_INT_GREATER(LW(0), 15)
                        SUBTRACT(LW(0), 15)
                        CALL(btlevtcmd_ACRStart, -2, 0, LW(0), LW(0), 0)
                        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
                        SWITCH(LW(6))
                            CASE_GREATER_OR_EQUAL(2)
                            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                            CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
                            LOOP(15)
                                CALL(btlevtcmd_AddRotate, -2, 0, 24, 0)
                                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
                                CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(1))
                                IF_INT_GREATER_OR_EQUAL(LW(1), 0)
                                    ADD(LW(0), -24)
                                ELSE
                                    ADD(LW(0), 24)
                                END_IF
                                CALL(btlevtcmd_SetUnitWork, -2, 3, LW(0))
                                WAIT_FRAMES(1)
                            END_LOOP
                            CASE_DEFAULT
                        END_SWITCH
                    END_IF
                END_LOOP
            END_SIBLING_EVENT
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            ADD(LW(2), 5)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 0, 0)
            DELETE_EVENT(15)
        ELSE
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                CALL(evt_eff64, STRING(""), STRING("kemuri2_n64"), 2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
            END_SIBLING_EVENT
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            ADD(LW(2), 5)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 0, 0)
        END_IF
    END_IF
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_S_1"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 30)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
        WAIT_MS(500)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        IF_INT_LESS_OR_EQUAL(LW(10), 0)
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        ELSE
            SET_VALUE(LW(6), LW(10))
            SUBTRACT(LW(6), 1)
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(6), LW(6))
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        END_IF
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetResultCountAC, LW(6))
    CALL(mario_get_renzoku_count_max, LW(3), LW(6), LW(7))
    IF_INT_LESS(LW(6), LW(7))
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
            CALL(evt_btl_camera_set_zoom, 0, 200)
            WAIT_FRAMES(10)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            CALL(evt_btl_camera_set_zoom, 0, 250)
        END_INLINE_EVENT
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131072, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(10), LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        ADD(LW(10), 1)
        CALL(btlevtcmd_StartAC, 0)
        GOTO(10)
    ELSE
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(10), LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    END_IF
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(evt_btl_camera_set_mode, 0, 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(6))
        IF_INT_LESS(LW(6), 0)
            MULTIPLY(LW(6), -1)
        END_IF
        CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
        MULTIPLY(LW(6), LW(0))
        DIVIDE(LW(6), 15)
        SET_VALUE(LW(7), 15)
        LABEL(20)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(6))
        SUBTRACT(LW(7), 1)
        WAIT_FRAMES(1)
        IF_INT_GREATER_OR_EQUAL(LW(7), 1)
            GOTO(20)
        END_IF
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyChristineAttack_Kiss)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL_CHILD(unk_evt_803510ec)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CalculateFaceDirection, LW(3), LW(4), -2, -1, 16, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, LW(3), LW(15))
    END_IF
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 27)
        SET_VALUE(LW(1), 20)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 25)
        SET_VALUE(LW(1), 28)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 23)
        SET_VALUE(LW(1), 30)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 23)
        SET_VALUE(LW(1), 30)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 23)
        SET_VALUE(LW(1), 30)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 23)
        SET_VALUE(LW(1), 30)
        CASE_DEFAULT
        SET_VALUE(LW(0), 23)
        SET_VALUE(LW(1), 32)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 16, 220, 70, 0, 0, LW(0), LW(1), 0)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 5, 3, 60)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 12)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
    MULTIPLY_FLOAT(LW(6), 12)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 200, -1, 0)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, -2, LW(6))
        ADD(LW(1), LW(6))
        CALL(btlevtcmd_GetResultPrizeLv, -5, 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(6))
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_KISS1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetBodyId, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, LW(9), STRING("PKR_O_1"))
    WAIT_MS(200)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 30)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(14), 45)
    CALL(evt_eff, STRING(""), STRING("kiss"), 1, LW(0), LW(1), LW(2), LW(14), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(15))
        IF_BITS_SET(LW(6), 2)
            CALL_CHILD(unk_evt_803510ec)
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 58)
            END_IF
            CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
            ADD(LW(2), 15)
            CALL(btlevtcmd_GetHeight, LW(3), LW(15))
            CALL(btlevtcmd_GetStatusMg, LW(3), LW(14))
            MULTIPLY_FLOAT(LW(15), LW(14))
            DIVIDE(LW(15), 2)
            ADD(LW(1), LW(15))
            CALL(evt_eff, 0, STRING("recovery"), 6, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
            CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_KURI_CHEER_KISS1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(40)
            CALL(btlevtcmd_GetResultPrizeLv, -5, 0, LW(6))
            CALL(btlevtcmd_AudienceDeclareACResult, PTR(partyWeapon_ChristineKiss), LW(6))
            CALL(_set_hustle, LW(3))
        ELSE
            WAIT_FRAMES(40)
            CALL(btlevtcmd_AudienceDeclareACResult, PTR(partyWeapon_ChristineKiss), -1)
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(partyWeapon_ChristineKiss), 536871168, LW(5))
        END_IF
    END_INLINE_EVENT
    CALL(btlevtcmd_ftof, 24, LW(0))
    CALL(btlevtcmd_ftof, 48, LW(1))
    CALL(btlevtcmd_ACRStart, -2, LW(0), LW(1), LW(1), 0)
    CALL(btlevtcmd_ACRGetResult, LW(7), LW(8))
    IF_INT_GREATER_OR_EQUAL(LW(7), 2)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_J_1B"))
        WAIT_MS(166)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_Y_1"))
        WAIT_MS(167)
    ELSE
        CALL(evt_audience_acrobat_notry)
        WAIT_FRAMES(20)
    END_IF
    IF_BITS_SET(LW(6), 2)
        CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, STRING("msg_st_chg_mario_quick"), 60)
    ELSE
        WAIT_MS(500)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL_CHILD(unk_evt_803510ec)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_ResetFaceDirection, LW(3))
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_IF
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(christine_appeal_event)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 66)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_APPEAL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 30)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(14), 45)
    CALL(evt_eff, STRING(""), STRING("kiss"), 1, LW(0), LW(1), LW(2), LW(14), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(667)
    CALL(btlevtcmd_AudienceAppeal, -2)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(party_win_reaction)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_Y_1"))
    WAIT_FRAMES(64)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(3), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(4))
    SET_VALUE(LW(8), LW(3))
    MULTIPLY(LW(8), LW(4))
    MULTIPLY(LW(8), -1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_MovePosition, -2, LW(8), LW(1), LW(2), 0, -1, 0)
    WAIT_FRAMES(20)
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageAudienceAreaSize, LW(4), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    SET_VALUE(LW(6), LW(4))
    MULTIPLY(LW(6), LW(5))
    MULTIPLY(LW(6), -1)
    CALL(btlevtcmd_SetPos, -2, LW(6), LW(1), LW(2))
    CALL(evt_btl_camera_set_mode, 0, 13)
    CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, 1, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_A_2B"))
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_J_1A"))
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
        SET_VALUE(LW(8), 0)
        LOOP(36)
            CALL(btlevtcmd_GetPosFloat, -2, LW(12), LW(13), LW(14))
            CALL(krb_get_dir, LW(5), LW(6), LW(12), LW(13), LW(8))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(8))
            SET_FLOAT_VALUE(LW(5), LW(12))
            SET_FLOAT_VALUE(LW(6), LW(13))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(15)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_KURI_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 28)
    ADD(LW(2), 5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, 0)
    WAIT_FRAMES(2)
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(1000)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(6))
        IF_INT_LESS(LW(6), 0)
            MULTIPLY(LW(6), -1)
        END_IF
        DIVIDE(LW(6), 15)
        SET_VALUE(LW(7), 15)
        LABEL(20)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(6))
        SUBTRACT(LW(7), 1)
        WAIT_FRAMES(1)
        IF_INT_GREATER_OR_EQUAL(LW(7), 1)
            GOTO(20)
        END_IF
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(6), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(8), LW(1), LW(2))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x64646464, 0x64646464, 0x64646464, 0x64646464,
    0x64646464, 0x64640000,
};

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
