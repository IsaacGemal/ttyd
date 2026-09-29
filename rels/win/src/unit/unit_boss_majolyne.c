#include "unit_boss_majolyne.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "unit/party/unit_party_vivian.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(majo3_sort);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[45];
static s32 damage_event[22];
static s32 dead_event[122];
static s32 phase_event[337];
static s32 attack_event[241];
static s32 kagenuke_attack_event[222];
static s32 blizzard_attack_event[243];
static s32 powder_attack_check[16];
static s32 powder_attack_event[179];
static s32 wait_event[6];
s32 majo3_shadow_tail_event_30_data_13F70[78];

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_majolyne = {
    UNIT_BELDAM_CHAPTER2,       // mUnitType
    "btl_un_majolyne1",         // name
    9,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    26,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    9,                          // field_0x14
    26,                         // width
    34,                         // height
    { 0, 34 },                  // statusIconOffset
    { 0.0f, 17.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 6.0f, 2.0f, 0.0f },       // talkTogeBaseOffset
    { 13.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 13.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 8.0f, 22.1f, 0.0f },      // kissHitOffset
    { 0.0f, 17.0f, 0.0f },      // cutBaseOffset
    26.0f,                      // cutWidth
    34.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_MAJO_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    70,  // sleepRate
    80,  // stopRate
    70,  // dizzyRate
    100, // poisonRate
    70,  // confuseRate
    100, // electricRate
    100, // burnRate
    50,  // freezeRate
    100, // hugeRate
    80,  // tinyRate
    100, // attackUpRate
    80,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    70,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "MJR_N_1" },
    { 0x02, "MJR_Z_1" },
    { 0x09, "MJR_Z_1" },
    { 0x05, "MJR_X_1" },
    { 0x04, "MJR_X_1" },
    { 0x03, "MJR_X_1" },
    { 0x1C, "MJR_S_1" },
    { 0x1D, "MJR_Q_1" },
    { 0x1E, "MJR_Q_1" },
    { 0x1F, "MJR_D_1" },
    { 0x27, "MJR_D_1" },
    { 0x32, "MJR_S_1" },
    { 0x2A, "MJR_R_1" },
    { 0x28, "MJR_W_1" },
    { 0x38, "MJR_S_1" },
    { 0x39, "MJR_S_1" },
    { 0x41, "MJR_T_1" },
    { 0x45, "MJR_S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_majolyne1",    // partName
        "c_majyorin",          // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 5.0f, 20.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

static BattleWeapon weapon_majolyne_kagenuke_attack1 = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardsAllowed
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
    2,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000007,                               // special
    0x00000002,                               // counterResistanceFlags
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_majolyne_blizzard_attack1 = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    2,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000043,                               // special
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
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static BattleWeapon weapon_majolyne_powder_attack1 = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 2, 2, 3, 3, 4, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x0000008F,                               // special
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
    100,                                      // sizeChangeChance
    3,                                        // sizeChangeTime
    2,                                        // sizeChangeStrength
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

static BattleWeapon weapon_majolyne_powder_attack2 = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 2, 2, 3, 3, 4, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
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
    100,                                      // sizeChangeChance
    3,                                        // sizeChangeTime
    -2,                                       // sizeChangeStrength
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

static EVT_BEGIN(majolyne_down)
    BEGIN_INLINE_EVENT
        WAIT_MS(250)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MAJO_SHADOW1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    CALL(battle_evt_majo_disp_off, -2, 1, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(majolyne_up)
    BEGIN_INLINE_EVENT
        WAIT_MS(250)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MAJO_SHADOW2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_INLINE_EVENT
    CALL(battle_evt_majo_disp_on, -2, 1, LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(majo3_zoom)
    CALL(btlevtcmd_GetPos, LW(0), LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, LW(0), 65, 332, LW(0), 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

EVT_BEGIN(btl_vivian_zoom_30_data_123B4)
    SET_VALUE(LW(0), GW(10))
    CALL_CHILD(majo3_zoom)
    RETURN
EVT_END()

EVT_BEGIN(btl_marilyn_zoom_30_data_123D0)
    SET_VALUE(LW(0), GW(9))
    CALL_CHILD(majo3_zoom)
    RETURN
EVT_END()

EVT_BEGIN(btl_majolyne_zoom_30_data_123EC)
    SET_VALUE(LW(0), GW(8))
    CALL_CHILD(majo3_zoom)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 8, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 9, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    SPAWN_EVENT(majo3_shadow_tail_event_30_data_13F70)
    ALLOCATE_UW(10, GW(15))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GW(11), 1)
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(4), LW(5))
    SET_VALUE(LW(6), 1)
    IF_INT_EQUAL(LW(0), LW(3))
        IF_INT_EQUAL(LW(1), LW(4))
            IF_INT_EQUAL(LW(2), LW(5))
                SET_VALUE(LW(6), 0)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_ResetFaceDirection, -2)
    END_IF
    WAIT_FRAMES(1)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL_CHILD(btl_majolyne_zoom_30_data_123EC)
    WAIT_MS(500)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("MJR_D_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("MJR_D_1"))
    IF_INT_NOT_EQUAL(GW(12), 0)
        IF_INT_NOT_EQUAL(GW(13), 0)
            CALL(evt_msg_print, 2, STRING("btl_majo3_11"), 0, -2)
            GOTO(90)
        END_IF
    END_IF
    CALL(evt_msg_print, 2, STRING("btl_majo3_08"), 0, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_D_3"))
    LABEL(90)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    SET_VALUE(GW(11), 2)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    RETURN
EVT_END()

static s32 majo3_sort(EventEntry* event, BOOL isFirstCall) {
    struct {
        s32 a;
        s32 b;
    } list[3], tmp;
    s32 i;

    for (i = 0; i < 3; i++) {
        list[i].a = event->lwData[3 + i];
        list[i].b = event->lwData[i];
    }

    if (list[0].b < list[1].b) {
        tmp = list[0];
        list[0] = list[1];
        list[1] = tmp;
    }
    if (list[1].b < list[2].b) {
        tmp = list[1];
        list[1] = list[2];
        list[2] = tmp;
    }
    if (list[0].b < list[1].b) {
        tmp = list[0];
        list[0] = list[1];
        list[1] = tmp;
    }

    for (i = 0; i < 3; i++) {
        event->lwData[i] = list[i].a;
    }

    return EVT_RETURN_DONE;
}

EVT_BEGIN(majo_unison_phase_event_30_data_126FC)
    SET_VALUE(LW(0), -1)
    SET_VALUE(LW(1), -1)
    SET_VALUE(LW(2), -1)
    IF_INT_EQUAL(GW(13), 0)
        CALL(btlevtcmd_GetUnitWork, GW(10), 5, LW(6))
        SWITCH(LW(6))
            CASE_EQUAL(0)
            CALL(btlevtcmd_GetHp, GW(10), LW(0))
            SET_VALUE(LW(3), 1)
            CASE_EQUAL(1)
            CALL(btlevtcmd_GetHp, GW(10), LW(1))
            SET_VALUE(LW(4), 1)
            CASE_EQUAL(2)
            CALL(btlevtcmd_GetHp, GW(10), LW(2))
            SET_VALUE(LW(5), 1)
        END_SWITCH
    END_IF
    IF_INT_EQUAL(GW(12), 0)
        CALL(btlevtcmd_GetUnitWork, GW(9), 5, LW(6))
        SWITCH(LW(6))
            CASE_EQUAL(0)
            CALL(btlevtcmd_GetHp, GW(9), LW(0))
            SET_VALUE(LW(3), 2)
            CASE_EQUAL(1)
            CALL(btlevtcmd_GetHp, GW(9), LW(1))
            SET_VALUE(LW(4), 2)
            CASE_EQUAL(2)
            CALL(btlevtcmd_GetHp, GW(9), LW(2))
            SET_VALUE(LW(5), 2)
        END_SWITCH
    END_IF
    IF_INT_EQUAL(GW(11), 0)
        CALL(btlevtcmd_GetUnitWork, GW(8), 5, LW(6))
        SWITCH(LW(6))
            CASE_EQUAL(0)
            CALL(btlevtcmd_GetHp, GW(8), LW(0))
            SET_VALUE(LW(3), 3)
            CASE_EQUAL(1)
            CALL(btlevtcmd_GetHp, GW(8), LW(1))
            SET_VALUE(LW(4), 3)
            CASE_EQUAL(2)
            CALL(btlevtcmd_GetHp, GW(8), LW(2))
            SET_VALUE(LW(5), 3)
        END_SWITCH
    END_IF
    CALL(majo3_sort)
    SWITCH(LW(0))
        CASE_EQUAL(1)
        IF_INT_EQUAL(GW(13), 0)
            CALL(btlevtcmd_SetUnitWork, GW(10), 5, 0)
        END_IF
        CASE_EQUAL(2)
        IF_INT_EQUAL(GW(12), 0)
            CALL(btlevtcmd_SetUnitWork, GW(9), 5, 0)
        END_IF
        CASE_EQUAL(3)
        IF_INT_EQUAL(GW(11), 0)
            CALL(btlevtcmd_SetUnitWork, GW(8), 5, 0)
        END_IF
    END_SWITCH
    SWITCH(LW(1))
        CASE_EQUAL(1)
        IF_INT_EQUAL(GW(13), 0)
            CALL(btlevtcmd_SetUnitWork, GW(10), 5, 1)
        END_IF
        CASE_EQUAL(2)
        IF_INT_EQUAL(GW(12), 0)
            CALL(btlevtcmd_SetUnitWork, GW(9), 5, 1)
        END_IF
        CASE_EQUAL(3)
        IF_INT_EQUAL(GW(11), 0)
            CALL(btlevtcmd_SetUnitWork, GW(8), 5, 1)
        END_IF
    END_SWITCH
    SWITCH(LW(2))
        CASE_EQUAL(1)
        IF_INT_EQUAL(GW(13), 0)
            CALL(btlevtcmd_SetUnitWork, GW(10), 5, 2)
        END_IF
        CASE_EQUAL(2)
        IF_INT_EQUAL(GW(12), 0)
            CALL(btlevtcmd_SetUnitWork, GW(9), 5, 2)
        END_IF
        CASE_EQUAL(3)
        IF_INT_EQUAL(GW(11), 0)
            CALL(btlevtcmd_SetUnitWork, GW(8), 5, 2)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(unison_phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(majo_unison_phase_event_30_data_126FC)
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(3))
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(4))
        IF_INT_NOT_EQUAL(LW(3), LW(4))
            CALL(btlevtcmd_SetUnitWork, -2, 4, LW(4))
            CALL(btlevtcmd_StartWaitEvent, -2)
            WAIT_MS(400)
            CALL_CHILD(majolyne_down)
            WAIT_MS(500)
            SWITCH(LW(4))
                CASE_EQUAL(0)
                SET_VALUE(LW(0), 55)
                SET_VALUE(LW(2), 0)
                CASE_EQUAL(1)
                SET_VALUE(LW(0), 105)
                SET_VALUE(LW(2), 10)
                CASE_EQUAL(2)
                SET_VALUE(LW(0), 155)
                SET_VALUE(LW(2), 20)
            END_SWITCH
            SET_VALUE(LW(1), 0)
            CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
            CALL_CHILD(majolyne_up)
            CALL(btlevtcmd_SortPhaseMoveTable)
            CALL(btlevtcmd_StartWaitEvent, -2)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 9, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SUBTRACT(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 9, LW(0))
        END_IF
        CALL(btlevtcmd_CheckActStatus, -2, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            GOTO(90)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 6, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetMaxHp, -2, LW(0))
            DEBUG_PUT_REG(LW(0))
            MULTIPLY(LW(0), 66)
            DEBUG_PUT_REG(LW(0))
            DIVIDE(LW(0), 100)
            DEBUG_PUT_REG(LW(0))
            CALL(btlevtcmd_GetHp, -2, LW(1))
            DEBUG_PUT_REG(LW(1))
            IF_INT_LESS(LW(1), LW(0))
                CALL(btlevtcmd_SetUnitWork, -2, 6, 1)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL_CHILD(btl_majolyne_zoom_30_data_123EC)
                CALL(evt_msg_print, 2, STRING("btl_majo3_03"), 0, -2)
                WAIT_MS(200)
                CALL(btlevtcmd_GetUnitWork, -2, 7, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(btlevtcmd_SetUnitWork, -2, 7, 1)
                    CALL(btlevtcmd_SetUnitWork, -2, 8, 3)
                END_IF
                IF_INT_EQUAL(GW(12), 0)
                    CALL(btlevtcmd_SetUnitWork, GW(9), 7, 1)
                END_IF
                IF_INT_EQUAL(GW(13), 0)
                    CALL(btlevtcmd_SetUnitWork, GW(10), 7, 1)
                END_IF
                CALL(btlevtcmd_StatusWindowOnOff, 1)
                CALL(evt_btl_camera_set_mode, 0, 0)
                CALL(btlevtcmd_StartWaitEvent, -2)
            END_IF
        END_IF
        IF_INT_EQUAL(GW(12), 0)
            CALL(btlevtcmd_GetUnitWork, GW(9), 6, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_GetMaxHp, GW(9), LW(0))
                MULTIPLY(LW(0), 50)
                DIVIDE(LW(0), 100)
                CALL(btlevtcmd_GetHp, GW(9), LW(1))
                IF_INT_LESS(LW(1), LW(0))
                    CALL(btlevtcmd_SetUnitWork, GW(9), 6, 1)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL_CHILD(btl_majolyne_zoom_30_data_123EC)
                    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, GW(9), -1, 16, LW(15))
                    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
                    WAIT_MS(200)
                    CALL(evt_msg_print, 2, STRING("btl_majo3_04"), 0, -2)
                    WAIT_MS(200)
                    CALL(btlevtcmd_SetUnitWork, GW(9), 7, 1)
                    CALL(btlevtcmd_CheckActStatus, GW(9), LW(0))
                    IF_INT_NOT_EQUAL(LW(0), 0)
                        CALL_CHILD(btl_marilyn_zoom_30_data_123D0)
                        CALL(evt_msg_print, 2, STRING("btl_majo3_05"), 0, GW(9))
                        WAIT_MS(200)
                    END_IF
                    CALL(btlevtcmd_ResetFaceDirection, -2)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    CALL(btlevtcmd_StartWaitEvent, -2)
                END_IF
            END_IF
        END_IF
        IF_INT_EQUAL(GW(13), 0)
            CALL(btlevtcmd_GetUnitWork, GW(10), 6, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_GetMaxHp, GW(10), LW(0))
                MULTIPLY(LW(0), 50)
                DIVIDE(LW(0), 100)
                CALL(btlevtcmd_GetHp, GW(10), LW(1))
                IF_INT_LESS(LW(1), LW(0))
                    CALL(btlevtcmd_SetUnitWork, GW(10), 6, 1)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL_CHILD(btl_majolyne_zoom_30_data_123EC)
                    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, GW(10), -1, 16, LW(15))
                    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
                    WAIT_MS(200)
                    CALL(evt_msg_print, 2, STRING("btl_majo3_06"), 0, -2)
                    WAIT_MS(200)
                    CALL(btlevtcmd_SetUnitWork, GW(10), 7, 1)
                    CALL(btlevtcmd_CheckActStatus, GW(10), LW(0))
                    IF_INT_NOT_EQUAL(LW(0), 0)
                        CALL_CHILD(btl_vivian_zoom_30_data_123B4)
                        CALL(evt_msg_print, 2, STRING("btl_majo3_07"), 0, GW(10))
                        WAIT_MS(200)
                    END_IF
                    CALL(btlevtcmd_ResetFaceDirection, -2)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    CALL(btlevtcmd_StartWaitEvent, -2)
                END_IF
            END_IF
        END_IF
    END_IF
    LABEL(90)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 8, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_SetUnitWork, -2, 8, 1)
        SET_VALUE(LW(9), PTR(&weapon_majolyne_kagenuke_attack1))
        CALL_CHILD(kagenuke_attack_event)
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetUnitWork, -2, 8, 2)
        SET_VALUE(LW(9), PTR(&weapon_majolyne_powder_attack2))
        CALL_CHILD(powder_attack_check)
        CALL_CHILD(powder_attack_event)
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetUnitWork, -2, 8, 4)
        SET_VALUE(LW(9), PTR(&weapon_majolyne_powder_attack1))
        CALL_CHILD(powder_attack_check)
        CALL_CHILD(powder_attack_event)
        CASE_EQUAL(3)
        CALL(btlevtcmd_SetUnitWork, -2, 8, 4)
        CALL(btlevtcmd_SetUnitWork, -2, 9, 2)
        SET_VALUE(LW(9), PTR(&weapon_majolyne_blizzard_attack1))
        CALL_CHILD(blizzard_attack_event)
        CASE_EQUAL(4)
        SET_VALUE(LW(14), 40)
        SET_VALUE(LW(9), PTR(&weapon_majolyne_powder_attack1))
        CALL_CHILD(powder_attack_check)
        IF_INT_NOT_EQUAL(LW(3), -1)
            SET_VALUE(LW(10), LW(3))
            SET_VALUE(LW(11), LW(4))
            CALL(btlevtcmd_CheckStatus, LW(3), 10, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                ADD(LW(14), 20)
            END_IF
        END_IF
        SET_VALUE(LW(9), PTR(&weapon_majolyne_powder_attack2))
        CALL_CHILD(powder_attack_check)
        IF_INT_NOT_EQUAL(LW(3), -1)
            SET_VALUE(LW(12), LW(3))
            SET_VALUE(LW(13), LW(4))
            CALL(btlevtcmd_CheckStatus, LW(3), 11, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                ADD(LW(14), 20)
            END_IF
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 7, LW(2))
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_GetUnitWork, -2, 9, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                ADD(LW(14), 20)
            END_IF
        END_IF
        SUBTRACT(LW(14), 1)
        CALL(evt_sub_random, LW(14), LW(0))
        SET_VALUE(LW(1), 40)
        IF_INT_LESS(LW(0), LW(1))
            SET_VALUE(LW(9), PTR(&weapon_majolyne_kagenuke_attack1))
            CALL_CHILD(kagenuke_attack_event)
            GOTO(90)
        END_IF
        ADD(LW(1), 20)
        IF_INT_LESS(LW(0), LW(1))
            SET_VALUE(LW(9), PTR(&weapon_majolyne_powder_attack1))
            SET_VALUE(LW(3), LW(10))
            SET_VALUE(LW(4), LW(11))
            CALL_CHILD(powder_attack_event)
            GOTO(90)
        END_IF
        ADD(LW(1), 20)
        IF_INT_LESS(LW(0), LW(1))
            SET_VALUE(LW(9), PTR(&weapon_majolyne_powder_attack2))
            SET_VALUE(LW(3), LW(12))
            SET_VALUE(LW(4), LW(13))
            CALL_CHILD(powder_attack_event)
            GOTO(90)
        ELSE
            CALL(btlevtcmd_SetUnitWork, -2, 9, 2)
            SET_VALUE(LW(9), PTR(&weapon_majolyne_blizzard_attack1))
            CALL_CHILD(blizzard_attack_event)
        END_IF
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(kagenuke_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_1"))
    CALL_CHILD(majolyne_down)
    WAIT_MS(300)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 30)
    SET_VALUE(LW(1), 0)
    CALL_CHILD(majolyne_up)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MAJO_PUSH1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_2"))
    WAIT_FRAMES(25)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_3"))
    WAIT_MS(350)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_4"))
    WAIT_MS(200)
    CALL(evt_snd_sfxoff, LW(15))
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
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(98)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_1"))
    CALL_CHILD(majolyne_down)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(200)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL_CHILD(majolyne_up)
    RETURN
EVT_END()

static EVT_BEGIN(blizzard_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MAJO_HAND_OPEN1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_2"))
    WAIT_FRAMES(25)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_3"))
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A1_4"))
    WAIT_MS(200)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MAJO_ATTACK_ICE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    IF_INT_GREATER_OR_EQUAL(LW(5), 0)
        CALL(evt_eff, 0, STRING("ibuki"), 0, LW(0), LW(1), LW(2), 180, 0, 0, 0, 0, 0, 0, 0)
    ELSE
        CALL(evt_eff, 0, STRING("ibuki"), 1, LW(0), LW(1), LW(2), 180, 0, 0, 0, 0, 0, 0, 0)
    END_IF
    WAIT_FRAMES(150)
    CALL(evt_snd_sfxoff, LW(15))
    LABEL(10)
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
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    RETURN
EVT_END()

static EVT_BEGIN(powder_attack_check)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    RETURN
EVT_END()

static EVT_BEGIN(powder_attack_event)
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
    IF_INT_EQUAL(LW(9), PTR(&weapon_majolyne_powder_attack1))
        CALL(evt_btl_camera_set_mode, 0, 9)
        CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 200)
    END_IF
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MAJO_HAND_OPEN1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A2_1"))
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A2_2"))
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MJR_A2_3"))
    CALL(evt_snd_sfxoff, LW(15))
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
    GOTO(99)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(30)
    GOTO(99)
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

EVT_BEGIN(majo3_shadow_tail_event_30_data_13F70)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(1))
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(2))
    LABEL(0)
    CALL(btlevtcmd_GetUnitId, -2, LW(5))
    IF_INT_EQUAL(LW(5), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(5))
    IF_INT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 1, LW(1))
        CALL(btlevtcmd_SetUnitWork, -2, 2, LW(2))
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
