#include "unit/party/unit_party_nokotarou.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "eff/eff_nokotarou.h"
#include "driver/msgdrv.h"
#include "mario/mario_party.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_attack_audience.h"
#include "battle/battle_weapon_power.h"


#define TECH_MENU_ENTRY(table)                                                        \
    cmd->weaponTable[*count].mIndex = -1;                                            \
    cmd->weaponTable[*count].mItem = 0;                                              \
    cmd->weaponTable[*count].mWeaponParams = (table);                                \
    cmd->weaponTable[*count].mEnabled = 0;                                           \
    cmd->weaponTable[*count].mIcon =                                                 \
        cmd->weaponTable[*count].mWeaponParams->icon;                                \
    cmd->weaponTable[*count].mName =                                                 \
        msgSearch(cmd->weaponTable[*count].mWeaponParams->name);                      \
    (*count)++;

#define TECH_MENU_ENTRY_ICON(table, icon)                                             \
    cmd->weaponTable[*count].mIndex = -1;                                            \
    cmd->weaponTable[*count].mItem = 0;                                              \
    cmd->weaponTable[*count].mWeaponParams = (table);                                \
    cmd->weaponTable[*count].mEnabled = 0;                                           \
    cmd->weaponTable[*count].mIcon = (icon);                                         \
    cmd->weaponTable[*count].mName =                                                 \
        msgSearch(cmd->weaponTable[*count].mWeaponParams->name);                      \
    (*count)++;

extern s32 btldefaultevt_avoid_counter_event[];
extern s32 evt_audience_acrobat_notry[];
extern s32 subset_attack_ready_jump_event[];
extern BattleUnitKind unit_koura;
static void __makeTechMenuFunc(BattleCommand* cmd, s32* count);
static USER_FUNC(_check_guard_koura);
static s32 init_event[78];
s32 change_party_spawn_init_event[28];
static s32 dmg_turn_event[65];
static s32 wakeup_event[67];
BattleWeapon partyWeapon_NokotarouKouraAttack;
BattleWeapon partyWeapon_NokotarouSyubibinKoura;
BattleWeapon partyWeapon_NokotarouKouraGuard;
BattleWeapon partyWeapon_NokotarouTsuranukiKoura;
s32 partyNokotarouAttack_FirstAttack[236];
s32 partyNokotarouAttack_NormalAttack[793];
s32 partyNokotarouAttack_SyubibinKoura[691];
s32 partyNokotarouAttack_KouraGuard[838];
s32 partyNokotarouAttack_TsuranukiKoura[931];
s32 nokotarou_appeal_event[34];
static s32 party_win_reaction[16];
static s32 attack_audience_event[263];
s32 unk_evt_80390b30[16];
static BattleUnitDefense defence;
static BattleUnitDefenseAttr defence_attr;
static BattleUnitDefense defence_turn;
static BattleVulnerableStatus regist;
static s32 damage_event[10];
static s32 attack_event[2];
static s32 wait_event[6];
BattleUnitSetup entry_koura[1];

// The tables are reached only from other units, so keep the data linked.
#pragma force_active on
// clang-format off
static void __makeTechMenuFunc(BattleCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

    TECH_MENU_ENTRY_ICON(&partyWeapon_NokotarouKouraAttack, 0x150);
    if (techLv >= 0) {
        TECH_MENU_ENTRY_ICON(&partyWeapon_NokotarouSyubibinKoura, 0x151);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_NokotarouKouraGuard);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_NokotarouTsuranukiKoura);
    }
}

USER_FUNC(_check_mario_move_count) {
    s32 index = event->args[0];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);

    evtSetValue(event, index, mario->movesRemaining);
    return EVT_RETURN_DONE;
}


USER_FUNC(_color_lv_set) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 mode = evtGetValue(event, args[1]);
    s32 target = args[2];
    s32 colorLv = evtGetValue(event, target);
    BattleUnit* unit = BattleGetUnitPtr(work, BattleTransID(event, id));

    if (mode != 0) {
        unit->mMoveColorLv = colorLv;
    } else {
        evtSetValue(event, target, unit->mMoveColorLv);
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_tsuranuki_effect_control) {
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart* part;
    EffectEntry* effect;
    Vec pos;
    s32 target;
    s32 id;
    f32 t;

    id = evtGetValue(event, event->args[0]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    part = BtlUnit_GetPartsPtr(unit, 1);

    if (isFirstCall) {
        effect = effNokotarouEntry(0, 1000, pos.x, pos.y, pos.z, unit->mSizeMultiplier);
        event->userdata[0] = (s32)effect;
        part->work[2] = 0;
        part->work[1] = 0;
        event->userdata[1] = 0;
    }

    target = part->work[1] * 100;
    if (event->userdata[1] != target) {
        if (event->userdata[1] < target) {
            event->userdata[1] += 10;
            if (event->userdata[1] > target) {
                event->userdata[1] = target;
            }
        } else {
            event->userdata[1] -= 10;
            if (event->userdata[1] < target) {
                event->userdata[1] = target;
            }
        }
    }

    t = 0.006f * (f32)event->userdata[1] + 2.0f;
    t *= unit->mSizeMultiplier;
    effect = (EffectEntry*)event->userdata[0];
    ((NokotarouWork*)effect->userdata)->x2 = pos.x;
    ((NokotarouWork*)effect->userdata)->y2 = pos.y;
    ((NokotarouWork*)effect->userdata)->z2 = pos.z;
    ((NokotarouWork*)effect->userdata)->scale = t;

    if (part->work[2] != 0) {
        part->work[2] = 0;
        effSoftDelete(effect);
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}


static USER_FUNC(_check_guard_koura) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 index = args[1];
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    BattleUnit* koura;

    if (!(unit->mAttributeFlags & kShellShielded)) {
        evtSetValue(event, index, -1);
        return EVT_RETURN_DONE;
    }

    koura = BtlUnit_GetGuardKouraPtr(unit);
    if (koura == NULL) {
        evtSetValue(event, index, -1);
        return EVT_RETURN_DONE;
    }

    evtSetValue(event, index, koura->unitId);
    return EVT_RETURN_DONE;
}

// TODO: type
__declspec(section ".data") u32 lbl_8038BEB8[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_8038BEC8[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_8038BED8[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_8038BEE8[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

BattleDispPose pose_table_nokotarou_stay[26] = {
    { 0x09, "PNK_D_5" },
    { 0x02, "PNK_D_5" },
    { 0x03, "PNK_X_1" },
    { 0x04, "PNK_D_2" },
    { 0x05, "PNK_X_1" },
    { 0x01, "PNK_N_1" },
    { 0x1B, "PNK_D_3" },
    { 0x1C, "PNK_S_1" },
    { 0x25, "PNK_G_2" },
    { 0x1F, "PNK_G_1" },
    { 0x27, "PNK_D_1" },
    { 0x28, "PNK_W_1" },
    { 0x2A, "PNK_R_1" },
    { 0x32, "PNK_A_1" },
    { 0x39, "PNK_D_2" },
    { 0x38, "PNK_D_2" },
    { 0x3C, "PNK_D_4" },
    { 0x3D, "PNK_X_1" },
    { 0x41, "PNK_T_1" },
    { 0x34, "PNK_I_1" },
    { 0x43, "PNK_C_1" },
    { 0x42, "PNK_O_1" },
    { 0x40, "PNK_X_1" },
    { 0x31, "PNK_A_1" },
    { 0x2D, "PNK_G_1" },
    { 0x45, "PNK_S_1" },
};

BattleDispPose pose_table_nokotarou_turn[12] = {
    { 0x09, "PNK_D_5" },
    { 0x02, "PNK_D_5" },
    { 0x01, "PNK_N_2" },
    { 0x1B, "PNK_D_3" },
    { 0x1C, "PNK_H_1" },
    { 0x1F, "PNK_D_1" },
    { 0x27, "PNK_D_1" },
    { 0x39, "PNK_H_1" },
    { 0x38, "PNK_H_1" },
    { 0x40, "PNK_H_1" },
    { 0x2D, "PNK_H_1" },
    { 0x45, "PNK_H_1" },
};

static BattleDataEntry data_table[12] = {
    { 0x05, nokotarou_appeal_event },
    { 0x06, attack_audience_event },
    { 0x04, change_party_spawn_init_event },
    { 0x0D, dmg_turn_event },
    { 0x01, __makeTechMenuFunc },
    { 0x02, wakeup_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x40, party_win_reaction },
    { 0x07, subset_attack_ready_jump_event },
    { 0x43, unk_evt_80390b30 },
    { 0x2F, btldefaultevt_avoid_counter_event },
    { 0x00, NULL },
};

BattleUnitKindPart unitpartsdata_Party_Nokotarou[1] = {
    {
        1,                         // partNum
        "btl_un_nokotarou",        // partName
        "c_pnk",                   // modelName
        { 0.0f, 0.0f, 0.0f },      // positionOffset
        { 12.0f, 30.0f, 5.0f },    // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },     // hitCursorBaseOffset
        20,                        // shadowSize
        30,                        // field_0x32
        255,                       // baseAlpha
        &defence,                  // defense
        &defence_attr,             // defenseAttr
        0x00001009,                // attributes
        0,                         // counterAttributes
        pose_table_nokotarou_stay, // poseTable
    },
};

BattleUnitKind unitdata_Party_Nokotarou = {
    UNIT_KOOPS,                    // mUnitType
    "btl_un_nokotarou",            // name
    10,                            // maxHp
    5,                             // maxFp
    5,                             // mDangerHP
    1,                             // mPerilHP
    1,                             // mLevel
    0,                             // mBonusExp
    0,                             // bonusCoinRolls
    0,                             // bonusCoinRate
    0,                             // baseCoin
    100,                           // escapeRate
    9,                             // renzokuSoftCap
    20,                            // width
    33,                            // height
    { 14, 36 },                    // statusIconOffset
    { 0.0f, 12.0f, 0.0f },         // centerOffset
    { 0, 0 },                      // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },         // talkTogeBaseOffset
    { 0.0f, 40.0f, 0.0f },         // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // kagenukeOffsetX
    0.0f,                          // kagenukeOffsetZ
    { 10.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 10.0f, 21.45f, 0.0f },       // kissHitOffset
    { 0.0f, 0.0f, 0.0f },          // cutBaseOffset
    0.0f,                          // cutWidth
    0.0f,                          // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    -1,                            // swallowRate
    0,                             // swallowAttribute
    100,                           // shotDamageRate
    20,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },          // starPointDispOffset
    "SFX_PARTY_DAMAGE1",           // damageSfxName
    "SFX_MARIO_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_MARIO_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_MARIO_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0x02000000,                    // attributeFlags
    &regist,                       // vulnStatus
    1,                             // numParts
    unitpartsdata_Party_Nokotarou, // mParts
    init_event,                    // initEvtScript
    data_table,                    // dataTable
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_PARTY_BATTLE_SUBMOVE1L"), STRING("SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 9, 9, 0, 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 3410)
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 0, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 2, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 6, 12, 16)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 9, 3)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(change_party_spawn_init_event)
    CALL(btlevtcmd_GetOverTurnCount, -4, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetPartsDefenceTable, -4, 1, PTR(&defence_turn))
        CALL(btlevtcmd_AnimeSetPoseTable, -4, 1, PTR(pose_table_nokotarou_turn))
        CALL(btlevtcmd_OnStatusFlag, -4, 1)
    END_IF
    CALL(btlevtcmd_AnimeChangePoseFromTable, -4, 1)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_turn_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_nokotarou_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 2)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("PNK_D_1"))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 16777216)
    CALL(evt_snd_sfxon, STRING("SFX_CONDITION_INSIDE"), 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_nokotarou_stay))
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("PNK_S_1"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleWeapon partyWeapon_NokotarouFirstAttack = {
    NULL,                                     // name
    336,                                      // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerFromPartyAttackLv,          // hpDamageFn
    { 2, 2, 3, 3, 5, 5, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21004004,                               // targetPropertyFlags
    0,                                        // element
    8,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x0002008F,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyNokotarouAttack_FirstAttack,         // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_NokotarouKouraAttack = {
    "btl_wn_pnk_normal",                      // name
    336,                                      // icon
    ITEM_NULL,                                // item
    "msg_pnk_normal_attack",                  // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromPartyAttackLv,          // hpDamageFn
    { 1, 2, 2, 3, 3, 5, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21004004,                               // targetPropertyFlags
    0,                                        // element
    8,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_hammer",                          // acHelpMsg
    0x00030087,                               // special
    0x000005FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyNokotarouAttack_NormalAttack,        // mAttackEvtScript
    { 0, 0, 0, 100, 0, 2, 2, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_NokotarouSyubibinKoura = {
    "btl_wn_pnk_lv1",                         // name
    337,                                      // icon
    ITEM_NULL,                                // item
    "msg_pnk_syubibin_koura",                 // description
    100,                                      // baseAccuracy
    3,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    2,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromPartyAttackLv,          // hpDamageFn
    { 1, 2, 2, 3, 3, 5, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20004004,                               // targetPropertyFlags
    0,                                        // element
    8,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_koura_shubibin",                  // acHelpMsg
    0x00020087,                               // special
    0x000005FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyNokotarouAttack_SyubibinKoura,       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_NokotarouKouraGuard = {
    "btl_wn_pnk_lv2",                         // name
    338,                                      // icon
    ITEM_NULL,                                // item
    "msg_pnk_koura_no_mamori",                // description
    100,                                      // baseAccuracy
    4,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 2, 2, 3, 3, 4, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01110020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_koura_no_mamori",                 // acHelpMsg
    0x00020000,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyNokotarouAttack_KouraGuard,          // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon partyWeapon_NokotarouTsuranukiKoura = {
    "btl_wn_pnk_lv3_tsuranuki",               // name
    339,                                      // icon
    ITEM_NULL,                                // item
    "msg_pnk_koura_tsuranuki",                // description
    100,                                      // baseAccuracy
    6,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    3,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetACOutputParam,                   // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20004004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_tsuranuki_koura",                 // acHelpMsg
    0x000200C7,                               // special
    0x000005FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyNokotarouAttack_TsuranukiKoura,      // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 0, 0, 0, 0, 0 }, // field_B4
};

static EVT_BEGIN(_change_koura_pose)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_2"))
    LOOP(18)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_1"))
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(_change_koura_pose_fast)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("kemuri_test"), 5, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_2"))
    LOOP(10)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 27, 0)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_1"))
    LOOP(3)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 30, 0)
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, 0, STRING("kemuri_test"), 5, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(_restore_koura_pose)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_1"))
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_2"))
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(_koura_rotate_start)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    BEGIN_INLINE_EVENT
        LABEL(0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            GOTO(10)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(11))
        IF_INT_NOT_EQUAL(LW(11), 0)
            CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(10))
            MULTIPLY(LW(10), -1)
            MULTIPLY(LW(0), -1)
            CALL(btlevtcmd_SetUnitWork, -2, 1, LW(10))
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(10))
        IF_INT_GREATER_OR_EQUAL(LW(10), 0)
            ADD(LW(0), 1)
            IF_INT_GREATER(LW(0), LW(10))
                SET_VALUE(LW(0), LW(10))
            END_IF
            CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
            CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
            IF_INT_LESS(LW(1), LW(0))
                CALL(btlevtcmd_GetPartsWork, -2, 1, 0, LW(1))
                IF_INT_NOT_EQUAL(LW(1), 0)
                    BEGIN_SIBLING_EVENT
                        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                        CALL(evt_eff, 0, STRING("kemuri_test"), 5, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
                    END_SIBLING_EVENT
                END_IF
            END_IF
        ELSE
            SUBTRACT(LW(0), 1)
            IF_INT_LESS(LW(0), LW(10))
                SET_VALUE(LW(0), LW(10))
            END_IF
            CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
            CALL(btlevtcmd_GetPartsWork, -2, 1, 0, LW(1))
            IF_INT_NOT_EQUAL(LW(1), 0)
                BEGIN_SIBLING_EVENT
                    MULTIPLY(LW(0), -1)
                    CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
                    ADD(LW(1), 360)
                    IF_INT_LESS(LW(1), LW(0))
                        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                        CALL(evt_eff, 0, STRING("kemuri_test"), 5, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
                    END_IF
                END_SIBLING_EVENT
            END_IF
        END_IF
        WAIT_FRAMES(1)
        GOTO(0)
        LABEL(10)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(_koura_rotate_stop)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(partyNokotarouAttack_FirstAttack)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&partyWeapon_NokotarouFirstAttack))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    SET_VALUE(LW(0), 45)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 45)
    CALL_CHILD(_koura_rotate_start)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(50)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(5), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_ATTACK2"), 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    CALL_CHILD(_koura_rotate_stop)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&partyWeapon_NokotarouFirstAttack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_NokotarouFirstAttack), 131328, LW(5))
    SET_VALUE(LW(0), 45)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 45)
    CALL_CHILD(_koura_rotate_start)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    CALL_CHILD(_koura_rotate_stop)
    CALL_CHILD(_restore_koura_pose)
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

EVT_BEGIN(partyNokotarouAttack_NormalAttack)
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
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_btl_camera_set_posoffset, 0, 20, 0, 0)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL_CHILD(_change_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_ATTACK1"), LW(13))
    SET_VALUE(LW(0), 15)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 45)
    CALL_CHILD(_koura_rotate_start)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 360)
        SET_VALUE(LW(1), 100)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 280)
        SET_VALUE(LW(1), 150)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 240)
        SET_VALUE(LW(1), 200)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 200)
        SET_VALUE(LW(1), 250)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 200)
        SET_VALUE(LW(1), 360)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 200)
        SET_VALUE(LW(1), 600)
        CASE_DEFAULT
        SET_VALUE(LW(0), 200)
        SET_VALUE(LW(1), 960)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 2, LW(0), 0, 1, LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 3)
    CALL(btlevtcmd_SetupAC, -2, 19, 1, 60)
    WAIT_FRAMES(22)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        LOOP(10000)
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(6), 0)
        LOOP(10000)
            ADD(LW(6), 1)
            IF_INT_GREATER(LW(6), 100)
                SET_VALUE(LW(6), 100)
            END_IF
            CALL(btlevtcmd_SetUnitWork, -2, 5, LW(6))
            CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
            CALL(btlevtcmd_GetUnitWork, -2, 5, LW(1))
            MULTIPLY(LW(0), LW(1))
            DIVIDE(LW(0), 100)
            CALL(evt_snd_sfx_pit, LW(13), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(evt_snd_sfxoff, LW(13))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_ATTACK2"), 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_posoffset, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(13))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(13), 0, 0)
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL_CHILD(_koura_rotate_stop)
        CALL_CHILD(_restore_koura_pose)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        WAIT_FRAMES(30)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_CommandPreCheckCounter, -2, LW(3), LW(4), 256, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL_CHILD(_koura_rotate_stop)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ACRStart, -2, 0, 17, 17, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 10, 0)
        LOOP(9)
            CALL(btlevtcmd_AddRotate, -2, 0, 0, 20)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_2"))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_SetUnitWork, -2, 1, 45)
            LABEL(91)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            IF_INT_LESS_OR_EQUAL(LW(0), 0)
                GOTO(92)
            END_IF
            SUBTRACT(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 1, LW(0))
            WAIT_FRAMES(2)
            GOTO(91)
            LABEL(92)
        END_SIBLING_EVENT
        CALL(btlevtcmd_ACRStart, -2, 60, 90, 90, 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        IF_INT_EQUAL(LW(6), 1)
            GOTO(94)
        END_IF
        IF_INT_NOT_EQUAL(LW(6), 2)
            GOTO(94)
        END_IF
        CALL_CHILD(_koura_rotate_stop)
        LOOP(6)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_3"))
        LOOP(6)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
        END_LOOP
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        WAIT_FRAMES(60)
        GOTO(95)
        LABEL(94)
        CALL_CHILD(_koura_rotate_stop)
        CALL(evt_audience_acrobat_notry)
        CALL_CHILD(_restore_koura_pose)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        WAIT_FRAMES(30)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    CALL_CHILD(_restore_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(30)
    LABEL(95)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyNokotarouAttack_SyubibinKoura)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_btl_camera_set_posoffset, 0, 20, 0, 0)
    WAIT_FRAMES(20)
    CALL_CHILD(_change_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_SHUBIBIN1"), LW(13))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    SET_VALUE(LW(0), 15)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 45)
    CALL_CHILD(_koura_rotate_start)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 480)
        SET_VALUE(LW(1), 240)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 300)
        SET_VALUE(LW(1), 180)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 240)
        SET_VALUE(LW(1), 120)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 180)
        SET_VALUE(LW(1), 105)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 150)
        SET_VALUE(LW(1), 90)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 60)
        CASE_DEFAULT
        SET_VALUE(LW(0), 100)
        SET_VALUE(LW(1), 30)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 2, LW(0), 1, 95, LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 3)
    CALL(btlevtcmd_SetupAC, -2, 19, 1, 60)
    WAIT_FRAMES(22)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        LOOP(10000)
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(6), 0)
        LOOP(10000)
            ADD(LW(6), 1)
            IF_INT_GREATER(LW(6), 200)
                SET_VALUE(LW(6), 200)
            END_IF
            SET_VALUE(LW(7), LW(6))
            DIVIDE(LW(7), 2)
            CALL(btlevtcmd_SetUnitWork, -2, 5, LW(7))
            CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
            CALL(btlevtcmd_GetUnitWork, -2, 5, LW(1))
            MULTIPLY(LW(0), LW(1))
            DIVIDE(LW(0), 100)
            CALL(evt_snd_sfx_pit, LW(13), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_StopAC)
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(evt_snd_sfxoff, LW(13))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_SHUBIBIN2"), 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_posoffset, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(5), LW(5))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(50)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
    CALL(btlevtcmd_CommandPreCheckCounter, -2, LW(3), LW(4), 256, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL_CHILD(_koura_rotate_stop)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(80)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, 400, LW(1), LW(2), 0, 0, 0)
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    MULTIPLY(LW(0), -1)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    CALL(btlevtcmd_ACRStart, -2, 0, 9, 9, 0)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 12)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_1"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_ACRStart, -2, 49, 79, 79, 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        IF_INT_NOT_EQUAL(LW(6), 2)
            CALL_CHILD(_koura_rotate_stop)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_2"))
            LOOP(6)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
            END_LOOP
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_S_1"))
            LOOP(6)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
            END_LOOP
            CALL(evt_audience_acrobat_notry)
            GOTO(99)
        END_IF
        CALL_CHILD(_koura_rotate_stop)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_2"))
        LOOP(6)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_3"))
        LOOP(6)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
        END_LOOP
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        WAIT_FRAMES(60)
        GOTO(99)
    END_IF
    CALL_CHILD(_restore_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyNokotarouAttack_KouraGuard)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_btl_camera_set_posoffset, 0, 20, 0, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(_check_guard_koura, -3, LW(10))
    IF_INT_NOT_EQUAL(LW(10), -1)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 62)
    END_IF
    CALL(_color_lv_set, -3, 0, LW(14))
    CALL(_color_lv_set, -3, 1, 255)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_A_2"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_SpawnUnit, LW(10), PTR(entry_koura), 0)
    CALL(btlevtcmd_OnAttribute, LW(10), 16777216)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    ADD(LW(1), 240)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffAttribute, LW(10), 16777216)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_GetBodyId, LW(10), LW(11))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 360)
        SET_VALUE(LW(1), 480)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 180)
        SET_VALUE(LW(1), 360)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 300)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 90)
        SET_VALUE(LW(1), 240)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 70)
        SET_VALUE(LW(1), 200)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 40)
        SET_VALUE(LW(1), 120)
        CASE_DEFAULT
        SET_VALUE(LW(0), 20)
        SET_VALUE(LW(1), 80)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 1, LW(10), LW(11), 30, LW(0), LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 12, 1, 0)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 125)
    CALL(btlevtcmd_DivePosition, LW(10), LW(0), LW(1), LW(2), 60, 0, 13, 0, -1)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_INT_EQUAL(0, 0)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(10)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_N_7"))
            WAIT_FRAMES(35)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_B_1"))
            WAIT_FRAMES(15)
            CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 43)
        END_SIBLING_EVENT
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 400)
        CALL(evt_btl_camera_set_posoffset, 0, 0, 0, 0)
        CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(0.4f))
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_GetPos, LW(10), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_NOKO_GUARD2"), EVTDAT_POINTER_MAX, 0, LW(5))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(2)
            CALL(btlevtcmd_ACRStart, -2, 48, 67, 67, 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            IF_INT_EQUAL(LW(6), 2)
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_O_1"))
                WAIT_MS(1000)
            ELSE
                CALL(evt_audience_acrobat_notry)
            END_IF
        END_SIBLING_EVENT
        CALL(btlevtcmd_FallPosition, LW(10), LW(0), LW(1), LW(2), 60)
        CALL(evt_snd_sfxoff, LW(5))
        CALL(btlevtcmd_SetHomePos, LW(10), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("PKO_F_1"))
        CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_NOKO_GUARD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        BEGIN_SIBLING_EVENT
            SET_VALUE(LW(9), LW(10))
            CALL(btlevtcmd_GetRotate, LW(9), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(8))
            IF_INT_LESS(LW(8), 180)
                CALL(evt_sub_intpl_init, 11, LW(8), 0, 50)
            ELSE
                CALL(evt_sub_intpl_init, 11, LW(8), 360, 50)
            END_IF
            LOOP(50)
                CALL(evt_sub_intpl_get_value_para, LW(1), LW(2))
                CALL(btlevtcmd_SetRotate, LW(9), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(1))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        IF_BITS_SET(LW(6), 2)
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
            SWITCH(LW(0))
                CASE_GREATER_OR_EQUAL(8)
                SET_VALUE(LW(0), 2)
                CASE_GREATER_OR_EQUAL(6)
                SET_VALUE(LW(0), 1)
                CASE_GREATER_OR_EQUAL(4)
                SET_VALUE(LW(0), 0)
                CASE_DEFAULT
                SET_VALUE(LW(0), -1)
            END_SWITCH
            IF_INT_GREATER_OR_EQUAL(LW(0), 0)
                CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(7))
                CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_ACSuccessEffect, LW(7), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(7))
            ELSE
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
            END_IF
        ELSE
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        END_IF
        WAIT_FRAMES(50)
        CALL(btlevtcmd_OnAttribute, LW(3), 32)
        CALL(btlevtcmd_GetUnitId, LW(3), LW(0))
        CALL(btlevtcmd_SetUnitWork, LW(10), 0, LW(0))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        CALL(btlevtcmd_SetUnitWork, LW(10), 2, 3)
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            SET_VALUE(LW(5), 5)
            LOOP(LW(5))
                CALL(btlevtcmd_GetUnitWork, LW(10), 2, LW(10))
                IF_INT_NOT_EQUAL(LW(0), 3)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
                SET_VALUE(LW(5), 5)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(btlevtcmd_SetUnitWork, LW(10), 2, 0)
        IF_BITS_SET(LW(6), 2)
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
        ELSE
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
        END_IF
        CALL(btlevtcmd_SetHp, LW(10), LW(0))
        CALL(btlevtcmd_RunDataEventChild, LW(10), 61)
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), 1, 43)
        CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 16777216)
        CALL(btlevtcmd_OnAttribute, LW(3), 96)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
        CALL(_check_mario_move_count, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
            CALL(evt_btl_camera_off_posoffset_manual, 0)
            WAIT_MS(500)
            CALL(btlevtcmd_RunDataEventChild, LW(10), 59)
            WAIT_FRAMES(60)
        END_IF
        CALL(btlevtcmd_ResetFaceDirection, -2)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_StartWaitEvent, -2)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_off_posoffset_manual, 0)
        RETURN
    END_IF
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_N_7"))
        WAIT_FRAMES(35)
        CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_B_1"))
        WAIT_FRAMES(150)
        CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 43)
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_NOKO_GUARD2"), EVTDAT_POINTER_MAX, 0, LW(5))
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_FallPosition, LW(10), LW(0), LW(1), LW(2), 60)
    CALL(btlevtcmd_SetHomePos, LW(10), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxoff, LW(5))
    CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_SetRotate, LW(10), 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("PKO_F_1"))
    WAIT_FRAMES(50)
    CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("PKO_H_1"))
    CALL(btlevtcmd_AnimeChangePose, LW(10), 2, STRING("PKO_H_2"))
    CALL(btlevtcmd_OffPartsAttribute, LW(10), 2, 16777216)
    CALL(btlevtcmd_SetPartsRotateOffset, LW(10), 1, 20, 0, 0)
    CALL(btlevtcmd_SetPartsRotateOffset, LW(10), 2, -20, 0, 0)
    LOOP(20)
        CALL(btlevtcmd_AddPartsRotate, LW(10), 1, 0, 0, 2)
        CALL(btlevtcmd_AddPartsRotate, LW(10), 2, 0, 0, -2)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(10)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), 2, 16777216)
        WAIT_FRAMES(2)
        CALL(btlevtcmd_OffPartsAttribute, LW(10), 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, LW(10), 2, 16777216)
        WAIT_FRAMES(2)
    END_LOOP
    CALL(btlevtcmd_KillUnit, LW(10), 0)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(_color_lv_set, -3, 1, LW(14))
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyNokotarouAttack_TsuranukiKoura)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_btl_camera_set_posoffset, 0, 20, 0, 0)
    WAIT_FRAMES(20)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 25)
        SET_VALUE(LW(1), 15)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 24)
        SET_VALUE(LW(1), 21)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 22)
        SET_VALUE(LW(1), 24)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 20)
        SET_VALUE(LW(1), 25)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 17)
        SET_VALUE(LW(1), 27)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 15)
        SET_VALUE(LW(1), 30)
        CASE_DEFAULT
        SET_VALUE(LW(0), 12)
        SET_VALUE(LW(1), 30)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 7, 200, 178, LW(0), LW(1), 20, 61, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetGaugeParam, 60, 80, 100, 100)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_SetUnitWork, -2, 3, -1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        LABEL(5)
        CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            WAIT_FRAMES(1)
            GOTO(5)
        END_IF
        CALL_CHILD(_change_koura_pose_fast)
        CALL(btlevtcmd_SetPartsWork, -2, 1, 0, 1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_IPPATSU1"), LW(13))
        CALL(btlevtcmd_SetUnitWork, -2, 3, LW(13))
        CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
        SET_VALUE(LW(0), 15)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 15)
        CALL_CHILD(_koura_rotate_start)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(_tsuranuki_effect_control, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_StartAC, 1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SWITCH(LW(0))
                CASE_LESS_OR_EQUAL(1)
                CALL(btlevtcmd_SetPartsWork, -2, 1, 1, 0)
                CASE_EQUAL(2)
                CALL(btlevtcmd_SetPartsWork, -2, 1, 1, 1)
                CALL(btlevtcmd_SetUnitWork, -2, 1, 24)
                CASE_EQUAL(3)
                CALL(btlevtcmd_SetPartsWork, -2, 1, 1, 2)
                CALL(btlevtcmd_SetUnitWork, -2, 1, 36)
                CASE_EQUAL(4)
                CALL(btlevtcmd_SetPartsWork, -2, 1, 1, 3)
                CALL(btlevtcmd_SetUnitWork, -2, 1, 45)
            END_SWITCH
            CALL(btlevtcmd_GetUnitWork, -2, 3, LW(13))
            IF_INT_NOT_EQUAL(LW(13), -1)
                CALL(btlevtcmd_AcGetOutputParam, 1, LW(1))
                CALL(btlevtcmd_SetUnitWork, -2, 5, LW(1))
                CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
                CALL(btlevtcmd_GetUnitWork, -2, 5, LW(1))
                MULTIPLY(LW(0), LW(1))
                DIVIDE(LW(0), 100)
                CALL(evt_snd_sfx_pit, LW(13), LW(0))
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    DELETE_EVENT(LW(14))
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        DELETE_EVENT(LW(15))
        CALL(btlevtcmd_SetPartsWork, -2, 1, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_2"))
        LOOP(10)
            CALL(btlevtcmd_AddRotate, -2, 0, 12, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_1"))
        LOOP(16)
            CALL(btlevtcmd_AddRotate, -2, 0, 20, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_H_1"))
        LOOP(14)
            CALL(btlevtcmd_AddRotate, -2, 0, 20, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_ACROBAT_SHAKE1"), 0)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_SetPartsWork, -2, 1, 2, 1)
        CALL_CHILD(_restore_koura_pose)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        WAIT_FRAMES(30)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_SetPartsWork, -2, 1, 0, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(13))
    CALL(evt_snd_sfxoff, LW(13))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_IPPATSU2"), 0)
    CALL(btlevtcmd_StopAC)
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(evt_snd_sfxoff, LW(13))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_SHUBIBIN2"), 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_posoffset, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(50)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
    CALL(btlevtcmd_CommandPreCheckCounter, -2, LW(3), LW(4), 256, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetPartsWork, -2, 1, 2, 1)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL_CHILD(_koura_rotate_stop)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_AcGetOutputParam, 2, LW(9))
        IF_INT_GREATER_OR_EQUAL(LW(9), 4)
            SUBTRACT(LW(9), 4)
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(9), LW(6))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        ELSE
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        END_IF
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(80)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, 400, LW(1), LW(2), 0, 0, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 1, 2, 1)
    WAIT_MS(100)
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    MULTIPLY(LW(0), -1)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    CALL(btlevtcmd_ACRStart, -2, 0, 9, 9, 0)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 12)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_1"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_ACRStart, -2, 49, 79, 79, 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        IF_INT_NOT_EQUAL(LW(6), 2)
            CALL(evt_audience_acrobat_notry)
            CALL_CHILD(_koura_rotate_stop)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_2"))
            LOOP(6)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
            END_LOOP
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_S_1"))
            LOOP(6)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
            END_LOOP
            GOTO(99)
        END_IF
        CALL_CHILD(_koura_rotate_stop)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_2"))
        LOOP(6)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_E_3"))
        LOOP(6)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
        END_LOOP
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        WAIT_FRAMES(60)
        GOTO(99)
    ELSE
        CALL(evt_audience_acrobat_notry)
    END_IF
    CALL_CHILD(_restore_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(nokotarou_appeal_event)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 66)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_APPEAL1"), LW(0), LW(1), LW(2), 0)
    WAIT_FRAMES(58)
    CALL(btlevtcmd_AudienceAppeal, -2)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(party_win_reaction)
    CALL(btlevtcmd_CheckStatus, -2, 17, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_Y_1"))
    END_IF
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
    CALL(evt_btl_camera_set_mode, 0, 12)
    CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, 1, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(0), 15)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 45)
        CALL_CHILD(_koura_rotate_start)
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    WAIT_MS(1000)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_NOKO_ATTACK2"), 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 15)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(1000)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    CALL_CHILD(_restore_koura_pose)
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

EVT_BEGIN(unk_evt_80390b30)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_nokotarou_stay))
    RETURN
EVT_END()

static BattleUnitDefense defence = { { 1, 1, 1, 1, 1 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefense defence_turn = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    100, // sleepRate
    100, // stopRate
    100, // dizzyRate
    100, // poisonRate
    100, // confuseRate
    100, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    100, // tinyRate
    100, // attackUpRate
    100, // attackDownRate
    100, // defenseUpRate
    100, // defenseDownRate
    100, // allergicRate
    100, // frightRate
    100, // galeForceRate
    100, // fastRate
    100, // slowRate
    100, // dodgyRate
    100, // invisibleRate
    100, // instaKillRate
};

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

BattleUnitSetup entry_koura[1] = {
    { &unit_koura, 0, 0x04000001, { 1000.0f, 0.0f, 1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};
// clang-format on
#pragma force_active off
