#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "manager/evtmgr_cmd.h"
#include "math_ext.h"
#include "system.h"
#include "battle/battle.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static USER_FUNC(krb_get_dir);
static s32 krb_comp_dir(f32 x, f32 y);
static USER_FUNC(get_attacker);
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts[1];
static BattleDispPose pose_table[19];
static BattleDataEntry data_table[3];
static s32 init_event[42];
static s32 dmg_avoid_counter_thorn_head_event[127];
static s32 damage_event[10];
static s32 attack_event[738];
static s32 wait_event[6];

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_hyper_togekuri = {
    UNIT_HYPER_SPIKY_GOOMBA,     // mUnitType
    "btl_un_hyper_togekuri",     // name
    8,                           // maxHp
    0,                           // maxFp
    1,                           // mDangerHP
    1,                           // mPerilHP
    16,                          // mLevel
    0,                           // mBonusExp
    2,                           // bonusCoinRolls
    70,                          // bonusCoinRate
    1,                           // baseCoin
    70,                          // escapeRate
    9999,                        // renzokuSoftCap
    23,                          // width
    30,                          // height
    { 5, 30 },                   // statusIconOffset
    { 0.0f, 15.0f, 0.0f },       // centerOffset
    { 0, 0 },                    // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },        // talkTogeBaseOffset
    { 12.0f, 0.0f, -10.0f },     // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },        // burnFlameOffset
    0.0f,                        // kagenukeOffsetX
    0.0f,                        // kagenukeOffsetZ
    { 11.5f, 0.0f, 0.0f },       // bintaHitOffset
    { 12.0f, 18.0f, 0.0f },      // kissHitOffset
    { 0.0f, 15.0f, 0.0f },       // cutBaseOffset
    23.0f,                       // cutWidth
    30.0f,                       // cutHeight
    0,                           // turnOrder
    0,                           // turnOrderVariance
    0,                           // swallowRate
    2,                           // swallowAttribute
    100,                         // shotDamageRate
    20,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },        // starPointDispOffset
    "SFX_ENM_TOGEKURI_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0x00040000,                  // attributeFlags
    &regist,                     // vulnStatus
    1,                           // numParts
    parts,                       // mParts
    init_event,                  // initEvtScript
    data_table,                  // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    80,  // sleepRate
    90,  // stopRate
    80,  // dizzyRate
    100, // poisonRate
    80,  // confuseRate
    100, // electricRate
    100, // burnRate
    80,  // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    80,  // allergicRate
    90,  // frightRate
    90,  // galeForceRate
    100, // fastRate
    80,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleUnitKindPart parts[1] = {
    {
        1,                       // partNum
        "btl_un_hyper_togekuri", // partName
        "c_kuribo_h",            // modelName
        { 0.0f, 0.0f, 0.0f },    // positionOffset
        { 0.0f, 25.0f, 0.0f },   // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },   // hitCursorBaseOffset
        20,                      // shadowSize
        30,                      // field_0x32
        255,                     // baseAlpha
        &defence,                // defense
        &defence_attr,           // defenseAttr
        0x00000009,              // attributes
        0x00000001,              // counterAttributes
        pose_table,              // poseTable
    },
};

static BattleWeapon weapon_togekuri_attack = {
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
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00005087,                               // special
    0,                                        // counterResistanceFlags
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon weapon_togekuri_charge = {
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
    0,                                        // damagePattern
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
    6,                                        // chargeStrength
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

static BattleDispPose pose_table[19] = {
    { 0x01, "TGK_N_1" },
    { 0x02, "TGK_Y_1" },
    { 0x09, "TGK_Y_1" },
    { 0x05, "TGK_K_1" },
    { 0x04, "TGK_I_1" },
    { 0x03, "TGK_I_1" },
    { 0x1C, "TGK_S_1" },
    { 0x1D, "TGK_Q_1" },
    { 0x1E, "TGK_Q_1" },
    { 0x1F, "TGK_S_1" },
    { 0x27, "TGK_D_1" },
    { 0x32, "TGK_A_1" },
    { 0x29, "TGK_R_2" },
    { 0x2A, "TGK_R_1" },
    { 0x28, "TGK_W_1" },
    { 0x38, "TGK_I_1" },
    { 0x39, "TGK_I_1" },
    { 0x41, "TGK_T_1" },
    { 0x45, "TGK_S_1" },
};

static BattleDataEntry data_table[3] = {
    { 0x25, dmg_avoid_counter_thorn_head_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_TOGEKURI_MOVE1"), STRING("SFX_ENM_TOGEKURI_MOVE2"), 0, 6, 6)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_TOGEKURI_MOVE1"), STRING("SFX_ENM_TOGEKURI_MOVE2"), 0, 6, 6)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_ENM_KURI_JUMP1"), STRING("SFX_ENM_KURI_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_head_event)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 50)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 30)
        GOTO(90)
    END_IF
    ADD(LW(1), 40)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetUnitId, -6, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_GetUnitWork, -6, 0, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(get_attacker, LW(0))
            IF_INT_EQUAL(LW(0), 2)
                RETURN
            END_IF
            WAIT_MS(2500)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(btlevtcmd_ChangeFaceDirection, -6, -1)
            CALL(get_attacker, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print, 2, STRING("mac_0_092_06"), 0, -6)
            ELSE
                CALL(evt_msg_print, 2, STRING("mac_0_092_07"), 0, -6)
            END_IF
            CALL(btlevtcmd_SetUnitWork, -6, 0, 1)
            CALL(btlevtcmd_ChangeFaceDirection, -6, 1)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

// TODO: type
static u32 SinTable[91] = {
    0x00000000, 0x3C8EF77F, 0x3D0EF241, 0x3D565E46,
    0x3D8EDC3C, 0x3DB27ED8, 0x3DD612C7, 0x3DF99674,
    0x3E0E835E, 0x3E20303C, 0x3E31D0C8, 0x3E43636F,
    0x3E54E6E2, 0x3E66598E, 0x3E77BA67, 0x3E8483ED,
    0x3E8D204B, 0x3E95B1C8, 0x3E9E377A, 0x3EA6B0D9,
    0x3EAF1D3F, 0x3EB77C03, 0x3EBFCC7D, 0x3EC80DE5,
    0x3ED03FD5, 0x3ED86163, 0x3EE0722A, 0x3EE87161,
    0x3EF05EA2, 0x3EF83904, 0x3F000000, 0x3F03D988,
    0x3F07A8C6, 0x3F0B6D76, 0x3F0F2746, 0x3F12D5E0,
    0x3F167914, 0x3F1A108C, 0x3F1D9BF6, 0x3F211B1E,
    0x3F248DC1, 0x3F27F37C, 0x3F2B4C2B, 0x3F2E976C,
    0x3F31D51B, 0x3F3504F7, 0x3F3826AB, 0x3F3B3A04,
    0x3F3E3EC0, 0x3F4134AD, 0x3F441B76, 0x3F46F30A,
    0x3F49BB17, 0x3F4C7369, 0x3F4F1BBD, 0x3F51B3F2,
    0x3F543BD6, 0x3F56B325, 0x3F5919AC, 0x3F5B6F4C,
    0x3F5DB3D0, 0x3F5FE719, 0x3F6208E1, 0x3F641909,
    0x3F66175D, 0x3F6803CD, 0x3F69DE16, 0x3F6BA637,
    0x3F6D5BEE, 0x3F6EFF19, 0x3F708FB8, 0x3F720D88,
    0x3F737879, 0x3F74D068, 0x3F761544, 0x3F7746ED,
    0x3F786552, 0x3F797050, 0x3F7A67E8, 0x3F7B4BE8,
    0x3F7C1C61, 0x3F7CD91F, 0x3F7D8234, 0x3F7E177F,
    0x3F7E98FE, 0x3F7F06A3, 0x3F7F605B, 0x3F7FA637,
    0x3F7FD817, 0x3F7FF60A, 0x3F800000,
};

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_CheckStatus, -2, 16, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_DrawLots, LW(0), 2, 25, 75)
    ELSE
        CALL(btlevtcmd_DrawLots, LW(0), 2, 25, 25)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(9), PTR(&weapon_togekuri_charge))
        CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
        CALL(evt_btl_camera_set_mode, 0, 7)
        CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 300)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 15)
        ADD(LW(2), 10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_eff64, STRING(""), STRING("crystal_n64"), 7, LW(0), LW(1), LW(2), FLOAT(1.5f), 60, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1000)
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpSetting, -2, 20, 0, FLOAT(1.0f))
            CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
            WAIT_MS(500)
            CALL(evt_btl_camera_set_mode, 0, 0)
        END_INLINE_EVENT
        CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
        RETURN
    END_IF
    LABEL(10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_togekuri_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_togekuri_attack), LW(3), LW(4))
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
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_togekuri_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_togekuri_attack))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 80)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 15, 0)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
        SET_VALUE(LW(8), 0)
        LOOP(28)
            CALL(btlevtcmd_GetPosFloat, -2, LW(9), LW(10), LW(11))
            CALL(krb_get_dir, LW(5), LW(6), LW(9), LW(10), LW(8))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(8))
            SET_FLOAT_VALUE(LW(5), LW(9))
            SET_FLOAT_VALUE(LW(6), LW(10))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(12)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 51)
    END_INLINE_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 2)
    SUBTRACT(LW(1), 4)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 28, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_togekuri_attack), 256, LW(5))
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
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KURI_LANDING_MISS1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetDispOffset, -2, 0, FLOAT(-12.0f), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGK_D_1"))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGK_A_2"))
    CALL(btlevtcmd_SetDispOffset, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(6))
        DIVIDE(LW(6), 10)
        MULTIPLY(LW(6), -1)
        SET_VALUE(LW(7), 10)
        LABEL(10)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(6))
        SUBTRACT(LW(7), 1)
        WAIT_FRAMES(1)
        IF_INT_GREATER_OR_EQUAL(LW(7), 1)
            GOTO(10)
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGK_K_1"))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), -30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    WAIT_FRAMES(40)
    GOTO(95)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    LOOP(4)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.08f), FLOAT(-0.12f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_togekuri_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_togekuri_attack), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(6))
        DIVIDE(LW(6), 10)
        MULTIPLY(LW(6), -1)
        SET_VALUE(LW(7), 10)
        LABEL(20)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(6))
        SUBTRACT(LW(7), 1)
        WAIT_FRAMES(1)
        IF_INT_GREATER_OR_EQUAL(LW(7), 1)
            GOTO(20)
        END_IF
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 28, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 10)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
    GOTO(95)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_ENM_KURI_MOVE1"), STRING("SFX_ENM_KURI_MOVE2"), 0, 3, 3, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 41)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static USER_FUNC(get_attacker) {
    s32* args = event->args;
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, _battleWorkPointer->activeUnitId);

    if (unit->initialKind == UNIT_MARIO) {
        evtSetValue(event, args[0], 0);
    } else if (unit->initialKind == UNIT_GOOMBELLA) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 2);
    }
    return EVT_RETURN_DONE;
}
static s32 krb_comp_dir(f32 x, f32 y) {
    u16 i;
    u16 low;
    u16 high;
    u16 mid;
    f32 ratio;
    f32 len;

    len = (f32)hypot(x, y);
    ratio = __fabs(y) / len;
    low = 0;
    high = 90;
    for (i = 0; i < 7; i++) {
        mid = low + (high - low) / 2;
        if (((f32*)SinTable)[mid] < ratio) {
            low = mid;
        } else {
            high = mid;
        }
    }
    if (__fabsf(((f32*)SinTable)[low] - ratio) < __fabsf(((f32*)SinTable)[high] - ratio)) {
        mid = low;
    } else {
        mid = high;
    }
    if (x < 0.0f && y >= 0.0f) {
        mid = 180 - mid;
    }
    if (x < 0.0f && y < 0.0f) {
        mid += 180;
    }
    if (x >= 0.0f && y < 0.0f) {
        mid = 360 - mid;
    }
    return mid;
}
static USER_FUNC(krb_get_dir) {
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


// clang-format on
