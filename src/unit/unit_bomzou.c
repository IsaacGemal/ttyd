#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

static BattleUnitKindPart parts[1];
static BattleUnitDefense defence;
static BattleUnitDefenseAttr defence_attr;
static BattleVulnerableStatus regist;
static BattleDispPose pose_table_3[4];
static s32 init_event[70];
static s32 phase_event[67];
static s32 wait_event[57];
static s32 explosion_event[173];
static BattleDataEntry data_table[4];
static s32 damage_event[10];
static s32 attack_event[5];
static s32 dead_event[6];

// clang-format off
BattleUnitKind unit_bomzou = {
    UNIT_BOMB_SQUAD_BOMB,       // mUnitType
    "btl_un_bomzou",            // name
    1,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    1,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    0,                          // bonusCoinRate
    0,                          // baseCoin
    100,                        // field_0x13
    9,                          // field_0x14
    24,                         // width
    24,                         // height
    { 0, 20 },                  // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 0.0f, 0.0f, 0.0f },       // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 12.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 12.0f, 15.6f, 0.0f },     // kissHitOffset
    { 0.0f, 0.0f, 0.0f },       // cutBaseOffset
    0.0f,                       // cutWidth
    0.0f,                       // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BTL_DAMAGE1",          // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x12400000,                 // attributeFlags
    &regist,                     // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

BattleWeapon weapon_bomzou_explosion = {
    NULL,                                        // name
    0,                                           // icon
    ITEM_NULL,                                   // item
    NULL,                                        // description
    100,                                         // baseAccuracy
    0,                                           // baseFpCost
    0,                                           // baseSpCost
    0,                                           // superguardsAllowed
    0.0f,                                        // field_0x14
    0,                                           // mStylishCommandMultiplier
    0,                                           // field_0x19
    0,                                           // mBingoSlotChance
    0,                                           // field_0x1B
    weaponGetPowerDefault,                       // hpDamageFn
    { 3, 0, 0, 0, 0, 0, 0, 0 },                  // hpDamageWork
    NULL,                                        // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // fpDamageWork
    0x02200020,                                  // targetClassFlags
    0x20000000,                                  // targetPropertyFlags
    3,                                           // element
    0,                                           // field_0x6D
    3,                                           // actionLevel
    2,                                           // field_0x6F
    NULL,                                        // acHelpMsg
    0x00020080,                                  // special
    0x000007FF,                                  // counterResistanceFlags
    0x00000004,                                  // targetWeightingFlags
    0,                                           // sleepChance
    0,                                           // sleepTime
    0,                                           // stopChance
    0,                                           // stopTime
    0,                                           // dizzyChance
    0,                                           // dizzyTime
    0,                                           // poisonChance
    0,                                           // poisonTime
    0,                                           // poisonStrength
    0,                                           // confuseChance
    0,                                           // confuseTime
    0,                                           // electricChance
    0,                                           // electricTime
    0,                                           // dodgyChance
    0,                                           // dodgyTime
    0,                                           // burnChance
    0,                                           // burnTime
    0,                                           // freezeChance
    0,                                           // freezeTime
    0,                                           // sizeChangeChance
    0,                                           // sizeChangeTime
    0,                                           // sizeChangeStrength
    0,                                           // atkChangeChance
    0,                                           // atkChangeTime
    0,                                           // atkChangeStrength
    0,                                           // defChangeChance
    0,                                           // defChangeTime
    0,                                           // defChangeStrength
    0,                                           // allergicChance
    0,                                           // allergicTime
    0,                                           // ohkoChance
    0,                                           // chargeStrength
    0,                                           // fastChance
    0,                                           // fastTime
    0,                                           // slowChance
    0,                                           // slowTime
    0,                                           // frightChance
    0,                                           // galeForceChance
    0,                                           // paybackTime
    0,                                           // holdFastTime
    0,                                           // invisibleChance
    0,                                           // invisibleTime
    0,                                           // hpRegenTime
    0,                                           // hpRegenStrength
    0,                                           // fpRegenTime
    0,                                           // fpRegenStrength
    NULL,                                        // mAttackEvtScript
    { 0, 10, 10, 100, 30, 5, 5, 5, 5, 0, 0, 0 }, // field_B4
};

static BattleUnitKindPart parts[1] = {
    {
        1,                      // partNum
        "btl_un_bomzou",        // partName
        "c_bakudan",            // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,                // defense
        &defence_attr,           // defenseAttr
        0x00080009,             // attributes
        0,                      // counterAttributes
        pose_table_3,           // poseTable
    },
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0, // sleepRate
    0, // stopRate
    0, // dizzyRate
    0, // poisonRate
    0, // confuseRate
    0, // electricRate
    0, // burnRate
    0, // freezeRate
    0, // hugeRate
    0, // tinyRate
    0, // attackUpRate
    0, // attackDownRate
    0, // defenseUpRate
    0, // defenseDownRate
    0, // allergicRate
    0, // frightRate
    0, // galeForceRate
    0, // fastRate
    0, // slowRate
    0, // dodgyRate
    0, // invisibleRate
    0, // instaKillRate
};

static BattleDispPose pose_table_3[4] = {
    { 0x1C, "B_2" },
    { 0x1F, "B_2" },
    { 0x27, "B_2" },
    { 0x45, "B_2" },
};

static BattleDispPose pose_table_2[4] = {
    { 0x1C, "B_3" },
    { 0x1F, "B_3" },
    { 0x27, "B_3" },
    { 0x45, "B_3" },
};

static BattleDispPose pose_table_1[4] = {
    { 0x1C, "B_4" },
    { 0x1F, "B_4" },
    { 0x27, "B_4" },
    { 0x45, "B_4" },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_GREATER_OR_EQUAL(3)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_3))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2))
        CASE_GREATER_OR_EQUAL(1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_1"))
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
        SWITCH(LW(0))
            CASE_GREATER_OR_EQUAL(3)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_3))
            CASE_GREATER_OR_EQUAL(2)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2))
            CASE_GREATER_OR_EQUAL(1)
            CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1))
        END_SWITCH
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            CALL(btlevtcmd_PhaseEventStartDeclare, -2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_2"))
            WAIT_FRAMES(36)
            CALL_CHILD(explosion_event)
        ELSE
            CALL(btlevtcmd_StartWaitEvent, -2)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    LABEL(0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_GREATER(LW(0), 1)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(evt_eff64, 0, STRING("sweat_n64"), 1, LW(0), LW(1), LW(2), 15, -45, 15, 0, 0, 0, 0, 0)
    CALL(evt_sub_random, 45, LW(0))
    ADD(LW(0), 45)
    WAIT_FRAMES(LW(0))
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(end_battle_evt)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_snd_sfxoff, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(explosion_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_snd_sfxoff, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("hibashira"), 2, LW(0), LW(1), LW(2), LW(0), LW(1), LW(2), 1, FLOAT(1.0f), 60, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_THUNDERS_BOMB2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_StageDispellFog)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 50331648)
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 4)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("B_2"))
    CALL(btlevtcmd_SamplingEnemy, -2, 1, PTR(&weapon_bomzou_explosion))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_bomzou_explosion), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), LW(3))
        GOTO(80)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    SUBTRACT(LW(0), LW(1))
    IF_INT_LESS_OR_EQUAL(LW(0), -1)
        MULTIPLY(LW(0), -1)
    END_IF
    IF_INT_GREATER(LW(0), 50)
        GOTO(80)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_bomzou_explosion), 256, LW(5))
    IF_INT_EQUAL(LW(5), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_bomzou_explosion), 256, LW(5))
    END_IF
    LABEL(80)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

// TODO: type
__declspec(section ".data") u32 lbl_80398960[2] = {
    0xBF800000, 0x3F800000,
};

static BattleDataEntry data_table[4] = {
    { 0x31, dead_event },
    { 0x35, explosion_event },
    { 0x3F, end_battle_evt },
    { 0x00, NULL },
};

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_AfterReactionEntry, -2, 53)
    RETURN
EVT_END()
// clang-format on
