#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_database_common.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"
#include "rel/common/unit_piders.h"
#include <dolphin/gx.h>

#pragma pool_data off

#pragma force_active on
static void piders_get_next_target(s32* outUnit, s32* outPart);
static void piders_get_target(s32* outUnit, s32* outPart);
static USER_FUNC(piders_renzoku_attack_set_target);
static USER_FUNC(piders_get_first_pos_attr);
static USER_FUNC(piders_get_zyun);
static USER_FUNC(piders_get_num);
static USER_FUNC(piders_draw_yarn);
static void piders_draw_yarn_sub(CameraId cameraId, void* param);
static USER_FUNC(piders_yarn_init);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[5];
static BattleUnitKindPart parts[5];
static s32 init_event[53];
static s32 dmg_first_attack_pos[19];
static s32 yarn_event[44];
static s32 entry_event[48];
static s32 damage_event[10];
static s32 dead_event[189];
static s32 blow_dead_event[104];
static s32 battle_end_event[7];
static s32 unison_phase_event[120];
static s32 attack_event[72];
static s32 normal_attack_event[87];
static s32 renzoku_attack_event[149];
static s32 renzoku_attack_event_sub[55];
static s32 renzoku_attack_event_sub_no_last[10];
static s32 renzoku_attack_event_sub_last[79];
static s32 wait_event[7];

// clang-format off
extend_data(unit_piders_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_piders_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_PIDER,                 // mUnitType
    "btl_un_piders",            // name
    5,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    12,                         // mLevel
    0,                          // mBonusExp
    2,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    0,                          // baseCoin
    70,                         // escapeRate
    9999,                       // renzokuSoftCap
    46,                         // width
    56,                         // height
    { 0, 56 },                  // statusIconOffset
    { 0.0f, 28.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 23.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 18.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 13.0f, 32.0f, 0.0f },     // kissHitOffset
    { 0.0f, 28.0f, 0.0f },      // cutBaseOffset
    46.0f,                      // cutWidth
    56.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // shotDamageRate
    0,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_PAIDA_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00000004,                 // attributeFlags
    &regist,                    // vulnStatus
    5,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    90,  // sleepRate
    90,  // stopRate
    50,  // dizzyRate
    100, // poisonRate
    90,  // confuseRate
    100, // electricRate
    100, // burnRate
    90,  // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    90,  // allergicRate
    100, // frightRate
    70,  // galeForceRate
    100, // fastRate
    90,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "PAI_N_1" },
    { 0x02, "PAI_Y_1" },
    { 0x09, "PAI_Y_1" },
    { 0x05, "PAI_K_1" },
    { 0x04, "PAI_X_1" },
    { 0x03, "PAI_X_1" },
    { 0x1C, "PAI_S_1" },
    { 0x1D, "PAI_D_1" },
    { 0x1E, "PAI_D_1" },
    { 0x1F, "PAI_S_1" },
    { 0x27, "PAI_D_1" },
    { 0x32, "PAI_A_1" },
    { 0x28, "PAI_W_1" },
    { 0x2A, "PAI_W_1" },
    { 0x38, "PAI_X_1" },
    { 0x39, "PAI_X_1" },
    { 0x41, "PAI_S_1" },
    { 0x45, "PAI_S_1" },
};

static BattleDataEntry data_table[5] = {
    { 0x31, dead_event },
    { 0x3A, blow_dead_event },
    { 0x3F, battle_end_event },
    { 0x30, dmg_first_attack_pos },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[5] = {
    {
        1,                     // partNum
        "btl_un_piders",       // partName
        "c_paid",              // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 40.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 50.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // shadowSize
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00600009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        2,                      // partNum
        "btl_un_piders",        // partName
        "c_paid",               // modelName
        { 0.0f, 0.0f, 5.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        3,                      // partNum
        "btl_un_piders",        // partName
        "c_paid",               // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        4,                      // partNum
        "btl_un_piders",        // partName
        "c_paid",               // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        5,                      // partNum
        "btl_un_piders",        // partName
        "c_paid",               // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x13010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

static BattleWeapon weapon_piders_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002000,                               // targetWeightingFlags
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

static BattleWeapon weapon_piders_renzoku_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002000,                               // targetWeightingFlags
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

#include "rel/common/unit_piders/piders_yarn_init.h"

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventUnisonPhase, -2, PTR(unison_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 255)
    CALL(piders_yarn_init)
    SPAWN_EVENT(yarn_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_first_attack_pos)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 10)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    RETURN
EVT_END()

#include "rel/common/unit_piders/piders_draw_yarn_sub.h"

static EVT_BEGIN(yarn_event)
    LABEL(0)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_LESS(LW(0), 255)
        IF_INT_GREATER(LW(0), 2)
            SUBTRACT(LW(0), 2)
            CALL(btlevtcmd_SetUnitWork, -2, 1, LW(0))
        END_IF
    END_IF
    CALL(piders_draw_yarn, LW(0))
    WAIT_FRAMES(1)
    GOTO(0)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 180)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 400)
    DIVIDE(LW(0), 10)
    ADD(LW(0), 80)
    WAIT_FRAMES(LW(0))
    CALL(evt_sub_intpl_init, 4, 400, 0, 40)
    LOOP(40)
        CALL(evt_sub_intpl_get_value)
        CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    SET_VALUE(LW(0), 48)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LABEL(10)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AddRotate, LW(10), 0, -15, 0)
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_SetRotate, LW(10), 0, 0, 0)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(evt_eff64, STRING(""), STRING("expbom_n64"), LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
    ADD(LW(2), 10)
    ADD(LW(1), 30)
    CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    CALL(btlevtcmd_SetRotateOffset, LW(10), 0, 0, 0)
    SET_VALUE(LW(0), 30)
    LABEL(20)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AddRotate, LW(10), 3, 0, 0)
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 33554432)
    CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 65536)
    CALL(btlevtcmd_OnAttribute, LW(10), 268435456)
    CALL(btlevtcmd_SetMaxMoveCount, LW(10), 0)
    CALL(btlevtcmd_OnAttribute, LW(10), 33554432)
    CALL(btlevtcmd_SetPos, LW(10), 0, -1000, 0)
    CALL(btlevtcmd_KillUnit, LW(10), 1)
    RETURN
EVT_END()

static EVT_BEGIN(blow_dead_event)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_CLAUD_BREATH3"), 0)
    CALL(btlevtcmd_GetBodyId, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 39)
    BEGIN_SIBLING_EVENT
        LABEL(0)
        CALL(btlevtcmd_AddRotate, -2, 0, 20, 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    ADD(LW(1), 40)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -10, 2, 0, -1)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 65536)
    CALL(btlevtcmd_OnAttribute, LW(10), 268435456)
    CALL(btlevtcmd_SetMaxMoveCount, LW(10), 0)
    CALL(btlevtcmd_OnAttribute, LW(10), 33554432)
    CALL(btlevtcmd_SetPos, LW(10), 0, -1000, 0)
    CALL(btlevtcmd_KillUnit, LW(10), 1)
    RETURN
EVT_END()

static EVT_BEGIN(battle_end_event)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 254)
    RETURN
EVT_END()

static EVT_BEGIN(yure_event)
    BEGIN_INLINE_EVENT
        LOOP(10)
            CALL(btlevtcmd_AddRotate, -2, 0, 6, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(22)
            CALL(btlevtcmd_AddRotate, -2, 0, -5, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(20)
            CALL(btlevtcmd_AddRotate, -2, 0, 4, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(16)
            CALL(btlevtcmd_AddRotate, -2, 0, -3, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(14)
            CALL(btlevtcmd_AddRotate, -2, 0, 2, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(10)
            CALL(btlevtcmd_AddRotate, -2, 0, -1, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(move_kuutei_event)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
        BEGIN_SIBLING_EVENT
            LOOP(4)
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                WAIT_FRAMES(7)
            END_LOOP
        END_SIBLING_EVENT
    ELSE
        BEGIN_SIBLING_EVENT
            LOOP(4)
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                WAIT_FRAMES(7)
            END_LOOP
        END_SIBLING_EVENT
    END_IF
    CALL_CHILD(yure_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, -10, 4, 0, -1)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 6291456)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    RETURN
EVT_END()

static EVT_BEGIN(move_kuuchu_event)
    SPAWN_EVENT(yure_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 70)
    BEGIN_SIBLING_EVENT
        LOOP(4)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 10, 4, 0, -1)
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 6291456)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    RETURN
EVT_END()

#include "rel/common/unit_piders/piders_get_num.h"

static EVT_BEGIN(unison_phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(piders_get_zyun, -2, LW(14))
    IF_INT_EQUAL(LW(14), 1)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL_CHILD(move_kuuchu_event)
        ELSE
            CALL_CHILD(move_kuutei_event)
        END_IF
        GOTO(99)
    END_IF
    CALL(piders_get_first_pos_attr, LW(13))
    CALL(piders_get_num, LW(15))
    MODULO(LW(15), 2)
    IF_INT_EQUAL(LW(15), 1)
        IF_INT_EQUAL(LW(13), 0)
            CALL_CHILD(move_kuuchu_event)
            GOTO(99)
        ELSE
            CALL_CHILD(move_kuutei_event)
            GOTO(99)
        END_IF
    ELSE
        MODULO(LW(14), 2)
        IF_INT_EQUAL(LW(14), 1)
            IF_INT_EQUAL(LW(13), 0)
                CALL_CHILD(move_kuuchu_event)
                GOTO(99)
            ELSE
                CALL_CHILD(move_kuutei_event)
                GOTO(99)
            END_IF
        ELSE
            IF_INT_EQUAL(LW(13), 0)
                CALL_CHILD(move_kuutei_event)
                GOTO(99)
            ELSE
                CALL_CHILD(move_kuuchu_event)
                GOTO(99)
            END_IF
        END_IF
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 11, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 12, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 13, 0)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    LABEL(0)
    SET_VALUE(LW(0), 50)
    ADD(LW(0), 50)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    IF_INT_LESS(LW(1), 50)
        SET_VALUE(LW(9), PTR(&weapon_piders_attack))
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    SET_VALUE(LW(9), PTR(&weapon_piders_renzoku_attack))
    CALL_CHILD(renzoku_attack_event)
    GOTO(99)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_check_event_sub)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(0))
    SWITCH(LW(0))
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
        GOTO(99)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_JumpPartsContinue, -2, LW(7))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(7), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(7), 33554432)
    GOTO(99)
    LABEL(91)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(7), 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(7), 33554432)
    IF_INT_EQUAL(LW(10), 1)
        SET_VALUE(LW(0), 5)
        SET_VALUE(LW(1), 8)
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(2))
            IF_INT_EQUAL(LW(3), LW(2))
                CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(2))
                IF_INT_EQUAL(LW(4), LW(2))
                    ADD(LW(0), 6)
                    CALL(btlevtcmd_SetUnitWork, -2, LW(0), 1)
                    BREAK_LOOP
                END_IF
            END_IF
            ADD(LW(0), 1)
            ADD(LW(1), 1)
        END_LOOP
    END_IF
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    IF_INT_EQUAL(LW(8), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(0))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(0))
    END_IF
    GOTO(99)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(piders_attack_common_event1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(8))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(8))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(2), FLOAT(3.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PAI_A_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("PAI_A_2a"))
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_GetStatusMg, -2, LW(1))
    SET_FLOAT_VALUE(LW(0), FLOAT(30.0f))
    MULTIPLY_FLOAT(LW(0), LW(1))
    CALL(btlevtcmd_SetPartsRotateOffset, -2, 2, 0, LW(0), 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 4, 0, 110, 5)
    LOOP(5)
        CALL(evt_sub_intpl_get_value)
        MULTIPLY(LW(0), LW(8))
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(20)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 4, 110, 70, 5)
    LOOP(5)
        CALL(evt_sub_intpl_get_value)
        MULTIPLY(LW(0), LW(8))
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(15)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 4, 70, 100, 5)
    LOOP(5)
        CALL(evt_sub_intpl_get_value)
        MULTIPLY(LW(0), LW(8))
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(10)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(piders_attack_common_event1_2)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(8))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(8))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(2), FLOAT(3.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PAI_A_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("PAI_A_2a"))
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(0))
    CALL(btlevtcmd_GetStatusMg, -2, LW(1))
    SET_FLOAT_VALUE(LW(0), FLOAT(30.0f))
    MULTIPLY_FLOAT(LW(0), LW(1))
    CALL(btlevtcmd_SetPartsRotateOffset, -2, 2, 0, LW(0), 0)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 4, LW(0), 100, 5)
    LOOP(5)
        CALL(evt_sub_intpl_get_value)
        MULTIPLY(LW(0), LW(8))
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(5)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(piders_attack_common_event2)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(8))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(8))
    CALL(evt_sub_intpl_init, 4, 100, 0, 15)
    LOOP(15)
        CALL(evt_sub_intpl_get_value)
        MULTIPLY(LW(0), LW(8))
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PAI_S_1"))
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
    RETURN
EVT_END()

static EVT_BEGIN(piders_attack_common_event3)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAIDA_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, LW(7), STRING("PAI_A_3a"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_JumpPartsSetting, -2, LW(7), 0, FLOAT(6.0f), FLOAT(0.25f))
    CALL(btlevtcmd_SetPartsPos, -2, LW(7), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(7), 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(7), 33554432)
    CALL(btlevtcmd_SetPartsScale, -2, LW(7), FLOAT(2.0f), FLOAT(2.0f), FLOAT(2.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsPosition, -2, LW(7), LW(0), LW(1), LW(2), 0, -1)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        GOTO(99)
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
    SET_VALUE(LW(7), 3)
    CALL_CHILD(piders_attack_common_event1)
    SPAWN_EVENT(piders_attack_common_event2)
    SET_VALUE(LW(8), 1)
    SET_VALUE(LW(5), 5)
    CALL_CHILD(piders_attack_common_event3)
    SET_VALUE(LW(10), 0)
    CALL_CHILD(damage_check_event_sub)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(500)
    LABEL(99)
    RETURN
EVT_END()

#include "rel/common/unit_piders/piders_renzoku_attack_set_target.h"

static EVT_BEGIN(renzoku_attack_event)
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
    CALL(piders_renzoku_attack_set_target, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 6, LW(2))
    CALL(btlevtcmd_SetUnitWork, -2, 7, LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 8, LW(1))
    CALL(btlevtcmd_SetUnitWork, -2, 9, LW(3))
    CALL(btlevtcmd_SetUnitWork, -2, 10, LW(5))
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(3))
    CALL(btlevtcmd_GetUnitWork, -2, 8, LW(4))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    SET_VALUE(LW(5), 5)
    SET_VALUE(LW(6), 8)
    SET_VALUE(LW(7), 3)
    CALL_CHILD(piders_attack_common_event1)
    LOOP(3)
        CALL(btlevtcmd_GetUnitWork, -2, LW(5), LW(3))
        CALL(btlevtcmd_GetUnitWork, -2, LW(6), LW(4))
        CALL_CHILD(piders_attack_common_event3)
        SPAWN_EVENT(renzoku_attack_event_sub)
        ADD(LW(5), 1)
        ADD(LW(6), 1)
        ADD(LW(7), 1)
        WAIT_FRAMES(30)
    END_LOOP
    SPAWN_EVENT(piders_attack_common_event2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event_sub)
    SET_VALUE(LW(0), LW(5))
    SET_VALUE(LW(1), LW(6))
    CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(3))
    CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(4))
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(0), 7)
            CALL_CHILD(renzoku_attack_event_sub_last)
            BREAK_LOOP
        END_IF
        ADD(LW(0), 1)
        ADD(LW(1), 1)
        CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(8))
        CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(10))
        IF_INT_EQUAL(LW(3), LW(8))
            IF_INT_EQUAL(LW(4), LW(10))
                CALL_CHILD(renzoku_attack_event_sub_no_last)
                BREAK_LOOP
            END_IF
        END_IF
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event_sub_no_last)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(10), 1)
    CALL_CHILD(damage_check_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(renzoku_attack_event_sub_last)
    SET_VALUE(LW(0), 5)
    SET_VALUE(LW(1), 8)
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(2))
        IF_INT_EQUAL(LW(3), LW(2))
            CALL(btlevtcmd_GetUnitWork, -2, LW(1), LW(2))
            IF_INT_EQUAL(LW(4), LW(2))
                ADD(LW(0), 6)
                CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(2))
                BREAK_LOOP
            END_IF
        END_IF
        ADD(LW(0), 1)
        ADD(LW(1), 1)
    END_LOOP
    IF_INT_EQUAL(LW(2), 1)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(7), 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(7), 33554432)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(0))
    ELSE
        SET_VALUE(LW(8), 1)
        SET_VALUE(LW(10), 0)
        CALL_CHILD(damage_check_event_sub)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

#include "rel/common/unit_piders/piders_get_target.h"

// clang-format on
