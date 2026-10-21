#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static USER_FUNC(togenoko_getnum);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[4];
static BattleUnitKindPart parts[1];
static s32 init_event[37];
static s32 damage_event[10];
static s32 dmg_avoid_counter_thorn_head_event[61];
static s32 dmg_turn_event[116];
static s32 attack_event[333];
static s32 normal_attack_event[406];
static s32 shell_attack_event[489];
static s32 charge_event[99];
static s32 guard_event[127];
static s32 guard_cancel_event[90];
static s32 friend_event[88];
static s32 wait_event[6];

// clang-format off
extend_data(unit_togenoko_ace_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_togenoko_ace_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_DARK_KOOPATROL,         // mUnitType
    "btl_un_togenoko_ace",       // name
    25,                          // maxHp
    0,                           // maxFp
    1,                           // mDangerHP
    1,                           // mPerilHP
    30,                          // mLevel
    1,                           // mBonusExp
    3,                           // bonusCoinRolls
    70,                          // bonusCoinRate
    2,                           // baseCoin
    50,                          // escapeRate
    9999,                        // field_0x14
    36,                          // width
    50,                          // height
    { -4, 50 },                  // statusIconOffset
    { 0.0f, 25.0f, 0.0f },       // centerOffset
    { 0, 0 },                    // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },        // talkTogeBaseOffset
    { 18.0f, 0.0f, -10.0f },     // field_0x3C
    { 0.0f, 0.0f, 0.0f },        // burnFlameOffset
    0.0f,                        // field_0x54
    0.0f,                        // field_0x58
    { 18.0f, 0.0f, 0.0f },       // bintaHitOffset
    { 13.0f, 32.0f, 0.0f },      // kissHitOffset
    { 0.0f, 25.0f, 0.0f },       // cutBaseOffset
    36.0f,                       // cutWidth
    50.0f,                       // cutHeight
    0,                           // turnOrder
    0,                           // turnOrderVariance
    0,                           // swallowRate
    2,                           // swallowAttribute
    100,                         // field_0x8C
    20,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },        // starPointDispOffset
    "SFX_ENM_TOGENOKO_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0,                           // attributeFlags
    &regist,                     // vulnStatus
    1,                           // numParts
    parts,                       // mParts
    init_event,                  // initEvtScript
    data_table,                  // dataTable
};

static BattleUnitDefense defence = { { 2, 2, 2, 2, 2 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    60,  // sleepRate
    65,  // stopRate
    65,  // dizzyRate
    100, // poisonRate
    65,  // confuseRate
    100, // electricRate
    100, // burnRate
    60,  // freezeRate
    100, // hugeRate
    85,  // tinyRate
    100, // attackUpRate
    85,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    70,  // allergicRate
    60,  // frightRate
    60,  // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleUnitDefense defence_turn = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_turn = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_turn = {
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

static BattleUnitDefense defence_guard = { { 99, 99, 99, 99, 99 } };

static BattleUnitDefenseAttr defence_attr_guard = { { 2, 2, 2, 2, 2 } };

static BattleVulnerableStatus regist_guard = {
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

static BattleDispPose pose_table[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "X_1" },
    { 0x1C, "S_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x32, "A_1" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_turn[15] = {
    { 0x01, "N_2" },
    { 0x02, "Z_2" },
    { 0x09, "Z_2" },
    { 0x05, "K_2" },
    { 0x04, "S_2" },
    { 0x03, "S_2" },
    { 0x1C, "S_2" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "D_1" },
    { 0x27, "D_1" },
    { 0x38, "S_2" },
    { 0x39, "S_2" },
    { 0x41, "S_2" },
    { 0x45, "S_2" },
};

static BattleDispPose pose_table_guard[15] = {
    { 0x01, "A_1" },
    { 0x02, "A_1" },
    { 0x09, "A_1" },
    { 0x05, "A_1" },
    { 0x04, "A_1" },
    { 0x03, "A_1" },
    { 0x1C, "A_1" },
    { 0x1D, "A_1" },
    { 0x1E, "A_1" },
    { 0x1F, "A_1" },
    { 0x27, "A_1" },
    { 0x38, "A_1" },
    { 0x39, "A_1" },
    { 0x41, "A_1" },
    { 0x45, "A_1" },
};

static BattleDispPose pose_table_guard_turn[15] = {
    { 0x01, "G_1" },
    { 0x02, "G_1" },
    { 0x09, "G_1" },
    { 0x05, "G_1" },
    { 0x04, "G_1" },
    { 0x03, "G_1" },
    { 0x1C, "G_1" },
    { 0x1D, "G_1" },
    { 0x1E, "G_1" },
    { 0x1F, "G_1" },
    { 0x27, "G_1" },
    { 0x38, "G_1" },
    { 0x39, "G_1" },
    { 0x41, "G_1" },
    { 0x45, "G_1" },
};

static BattleDataEntry data_table[4] = {
    { 0x0D, dmg_turn_event },
    { 0x25, dmg_avoid_counter_thorn_head_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleDataEntry data_table_turn[3] = {
    { 0x0D, dmg_turn_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleDataEntry data_table_guard[4] = {
    { 0x25, dmg_avoid_counter_thorn_head_event },
    { 0x0D, dmg_turn_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_togenoko_ace", // partName
        "c_togenoko_b",        // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 40.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 50.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00001009,            // attributes
        0x00000001,            // counterAttributes
        pose_table,            // poseTable
    },
};

static BattleWeapon weapon_togenoko_normal_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 5, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    8,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x00000002,                               // counterResistanceFlags
    0x00002004,                               // targetWeightingFlags
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_togenoko_shell_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 5, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20004004,                               // targetPropertyFlags
    0,                                        // element
    8,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000005FF,                               // counterResistanceFlags
    0x00002000,                               // targetWeightingFlags
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_togenoko_charge = {
    NULL,                                     // name
    0,                                        // icon
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
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
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
    5,                                        // chargeStrength
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_TOGENOKO_MOVE3"), STRING("SFX_ENM_TOGENOKO_MOVE4"), 0, 3, 3)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_TOGENOKO_MOVE3"), STRING("SFX_ENM_TOGENOKO_MOVE4"), 0, 6, 6)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_head_event)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 70)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 40)
        GOTO(90)
    END_IF
    ADD(LW(1), 55)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_turn_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetHitOffset, LW(10), LW(11), 0, -20, 0)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence_turn))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr_turn))
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist_turn))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
    ELSE
        CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_guard_turn))
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 69)
    END_IF
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_OffPartsCounterAttribute, LW(10), LW(11), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 2)
    WAIT_FRAMES(2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 15, -1)
    CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 10, -1)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 69)
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, LW(10))
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr))
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_OnPartsCounterAttribute, LW(10), LW(11), 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("S_1"))
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_SetHitOffset, LW(10), LW(11), 0, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    RETURN
EVT_END()

static BattleUnitSetup entry_togenoko[1] = {
    { &UNIT_DATA_SYM, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static USER_FUNC(togenoko_getnum) {
    s32* args;
    BattleUnit* unit;
    s32 count;
    s32 i;

    args = event->args;
    count = 0;
    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(_battleWorkPointer, i);
        if (unit && unit->currentKind == UNIT_DARK_KOOPATROL) {
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(9), PTR(&weapon_togenoko_normal_attack))
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetOverTurnCount, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(wakeup_event)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(0), 80)
        ADD(LW(0), 20)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 80)
            CALL_CHILD(guard_cancel_event)
            GOTO(99)
        END_IF
        GOTO(99)
    ELSE
        CALL(btlevtcmd_CheckStatus, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(0), 0)
            ADD(LW(0), 50)
            ADD(LW(0), 35)
            SUBTRACT(LW(0), 1)
            CALL(evt_sub_random, LW(0), LW(1))
            SET_VALUE(LW(0), 50)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(&weapon_togenoko_normal_attack))
                CALL_CHILD(normal_attack_event)
                GOTO(99)
            END_IF
            SET_VALUE(LW(9), PTR(&weapon_togenoko_shell_attack))
            CALL_CHILD(shell_attack_event)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        LOOP(0)
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), 0)
                SUBTRACT(LW(2), 10)
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                IF_INT_EQUAL(LW(3), -1)
                    GOTO(10)
                END_IF
            ELSE
                GOTO(9)
            END_IF
        END_LOOP
        LABEL(9)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        LOOP(0)
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
            IF_INT_LESS_OR_EQUAL(LW(0), 130)
                ADD(LW(2), 10)
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                IF_INT_EQUAL(LW(3), -1)
                    GOTO(10)
                END_IF
            ELSE
                GOTO(11)
            END_IF
        END_LOOP
        LABEL(10)
        CALL(togenoko_getnum, LW(10))
        CALL(evt_sub_random, 99, LW(11))
        SWITCH(LW(10))
            CASE_EQUAL(1)
            IF_INT_LESS(LW(11), 0)
                CALL_CHILD(friend_event)
                GOTO(99)
            END_IF
            CASE_END
            CASE_EQUAL(2)
            IF_INT_LESS(LW(11), 0)
                CALL_CHILD(friend_event)
                GOTO(99)
            END_IF
            CASE_END
            CASE_EQUAL(3)
            IF_INT_LESS(LW(11), 0)
                CALL_CHILD(friend_event)
                GOTO(99)
            END_IF
            CASE_END
        END_SWITCH
        LABEL(11)
        SET_VALUE(LW(0), 0)
        ADD(LW(0), 50)
        ADD(LW(0), 35)
        ADD(LW(0), 15)
        ADD(LW(0), 0)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        SET_VALUE(LW(0), 50)
        IF_INT_LESS(LW(1), LW(0))
            SET_VALUE(LW(9), PTR(&weapon_togenoko_normal_attack))
            CALL_CHILD(normal_attack_event)
            GOTO(99)
        END_IF
        ADD(LW(0), 35)
        IF_INT_LESS(LW(1), LW(0))
            SET_VALUE(LW(9), PTR(&weapon_togenoko_shell_attack))
            CALL_CHILD(shell_attack_event)
            GOTO(99)
        END_IF
        ADD(LW(0), 15)
        IF_INT_LESS(LW(1), LW(0))
            SET_VALUE(LW(9), PTR(&weapon_togenoko_charge))
            CALL_CHILD(charge_event)
            GOTO(99)
        END_IF
        CALL_CHILD(guard_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    BEGIN_SIBLING_EVENT
        LOOP(10)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(3)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_3"))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 10)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 500)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    CALL(btlevtcmd_ChangeFaceDirection, -2, -1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, 300, LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(10), LW(11), LW(12))
    SUBTRACT_FLOAT(LW(10), LW(0))
    SUBTRACT_FLOAT(LW(11), LW(1))
    SUBTRACT_FLOAT(LW(12), LW(2))
    MULTIPLY_FLOAT(LW(10), FLOAT(0.75f))
    MULTIPLY_FLOAT(LW(11), FLOAT(0.75f))
    MULTIPLY_FLOAT(LW(12), FLOAT(0.75f))
    ADD_FLOAT(LW(0), LW(10))
    ADD_FLOAT(LW(1), LW(11))
    ADD_FLOAT(LW(2), LW(12))
    CALL(btlevtcmd_JumpSetting, -2, 20, 0, FLOAT(0.2f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 20, 0, FLOAT(0.2f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    WAIT_MS(500)
    RETURN
EVT_END()

static EVT_BEGIN(shell_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 10)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(200)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 20)
    WAIT_MS(200)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 40)
    WAIT_MS(200)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            ADD(LW(1), 10)
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(12.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 0)
    SET_VALUE(LW(6), 0)
    LABEL(10)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(12.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 100)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 0)
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        SET_VALUE(LW(6), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 500)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(12.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    WAIT_MS(500)
    CALL(btlevtcmd_SetPos, -2, 250, LW(1), LW(2))
    LABEL(98)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(12.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0)
    DELETE_EVENT(LW(13))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_SHELL2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(10.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    WAIT_FRAMES(3)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(charge_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_togenoko_charge))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_togenoko_charge))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        LOOP(8)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(3)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_3"))
    WAIT_FRAMES(48)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(60)
        CALL(evt_btl_camera_set_mode, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_togenoko_charge))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_togenoko_charge), 256, LW(5))
    RETURN
EVT_END()

static EVT_BEGIN(guard_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(5)
        CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("A_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, LW(10), 20, 0, FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 20, -1)
    SET_FLOAT_VALUE(LW(12), FLOAT(30.0f))
    SET_FLOAT_VALUE(LW(13), FLOAT(0.0f))
    LOOP(15)
        CALL(evt_sub_get_sincos, LW(13), LW(14), LW(15))
        MULTIPLY_FLOAT(LW(14), LW(12))
        CALL(btlevtcmd_SetRotate, LW(10), 0, 0, LW(14))
        SUBTRACT_FLOAT(LW(12), FLOAT(2.0f))
        ADD_FLOAT(LW(13), FLOAT(30.0f))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, LW(10), 0, 0, 0)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence_guard))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr_guard))
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist_guard))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_guard))
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table_guard))
    CALL(btlevtcmd_SetUnitWork, LW(10), 0, 1)
    CALL(btlevtcmd_SetHitOffset, LW(10), LW(11), 0, -20, 0)
    WAIT_MS(300)
    RETURN
EVT_END()

static EVT_BEGIN(guard_cancel_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_SHELL2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(5)
        CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, LW(10), 20, 0, FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_MOVE6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_SetPartsDefenceAttrTable, LW(10), LW(11), PTR(&defence_attr))
    CALL(btlevtcmd_SetRegistStatus, LW(10), PTR(&regist))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table))
    CALL(btlevtcmd_SetUnitWork, LW(10), 0, 0)
    CALL(btlevtcmd_SetHitOffset, LW(10), LW(11), 0, 0, 0)
    WAIT_MS(300)
    RETURN
EVT_END()

static EVT_BEGIN(friend_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGENOKO_CALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_1"))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_togenoko), 0)
    CALL(btlevtcmd_SetPos, LW(3), 300, LW(1), LW(2))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("R_1"))
    CALL(btlevtcmd_SetMoveSpeed, LW(3), 8)
    CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("S_1"))
    WAIT_MS(500)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(300)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
