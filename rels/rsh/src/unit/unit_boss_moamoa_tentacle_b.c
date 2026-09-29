#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma force_active on
extern s32 boss_moamoa_check_zenei_kouei[];
extern s32 btldefaultevt_Confuse[];
extern s32 moamoa1_weakness_pose_table[];
extern s32 moamoa_defence_no_parts[];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts;
static s32 init_event[26];
static s32 damage_event[10];
static s32 dead_event[115];
static s32 attack_event[21];
static s32 evt_attack[387];
static s32 wait_event[32];

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_moamoa_tentacle_b = {
    UNIT_SMORG_MIASMA_TENTACLE_B, // mUnitType
    "btl_un_moamoa_tentacl_b",    // name
    4,                            // maxHp
    0,                            // maxFp
    1,                            // mDangerHP
    1,                            // mPerilHP
    0,                            // mLevel
    0,                            // mBonusExp
    0,                            // bonusCoinRolls
    100,                          // bonusCoinRate
    0,                            // baseCoin
    50,                           // field_0x13
    10,                           // field_0x14
    90,                           // width
    100,                          // height
    { 0, 85 },                    // statusIconOffset
    { 0.0f, 0.0f, 0.0f },         // centerOffset
    { 0, 0 },                     // hpGaugeOffset
    { 280.0f, 40.0f, 0.0f },      // talkTogeBaseOffset
    { 45.0f, 0.0f, -10.0f },      // field_0x3C
    { 0.0f, 0.0f, 0.0f },         // burnFlameOffset
    0.0f,                         // field_0x54
    0.0f,                         // field_0x58
    { 45.0f, 0.0f, 0.0f },        // bintaHitOffset
    { 35.0f, 75.0f, 20.0f },      // kissHitOffset
    { -30.0f, 130.0f, 0.0f },     // cutBaseOffset
    90.0f,                        // cutWidth
    100.0f,                       // cutHeight
    0,                            // turnOrder
    0,                            // turnOrderVariance
    -1,                           // swallowRate
    0,                            // swallowAttribute
    0,                            // field_0x8C
    0,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },         // starPointDispOffset
    "SFX_BOSS_MOA_G_DAMAGED1",    // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",       // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",        // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",   // electricDamageSfxName
    0x00200000,                   // attributeFlags
    &regist,                       // vulnStatus
    1,                            // numParts
    &parts,                       // mParts
    init_event,                   // initEvtScript
    data_table,                   // dataTable
};

static BattleUnitDefense defence = { { 1, 1, 1, 1, 1 } };

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

static BattleDispPose pose_table[4] = {
    { 0x1C, "S_1B_1" },
    { 0x1F, "D_1B_1" },
    { 0x27, "D_1B_1" },
    { 0x45, "S_1B_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts = {
    1,                          // partNum
    "btl_un_moamoa_tentacle_b", // partName
    "c_g_moamoa_b",             // modelName
    { 0.0f, 0.0f, 0.0f },       // positionOffset
    { 0.0f, 80.0f, 20.0f },     // hitBaseOffset
    { 0.0f, 100.0f, 20.0f },    // hitCursorBaseOffset
    20,                         // field_0x30
    30,                         // field_0x32
    255,                        // baseAlpha
    &defence,                    // defense
    &defence_attr,               // defenseAttr
    0x00600001,                 // attributes
    0,                          // counterAttributes
    pose_table,                 // poseTable
};

static BattleWeapon weapon_moamoa_attack = {
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
    { 5, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
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
    0x80000000,                               // targetWeightingFlags
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

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetHpGaugeOffset, -2, 0, 80)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1B"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
    CALL(btlevtcmd_SetUnitWork, LW(1), 4, -1)
    CALL(btlevtcmd_GetUnitWork, LW(1), 5, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_GetUnitWork, LW(1), 4, LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(btlevtcmd_GetUnitWork, LW(1), 3, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_SetPartsDefenceTable, LW(1), 1, PTR(moamoa_defence_no_parts))
                CALL(btlevtcmd_OffPartsAttribute, LW(1), 1, 536870912)
                CALL(btlevtcmd_AnimeSetPoseTable, LW(1), 1, PTR(moamoa1_weakness_pose_table))
                CALL(btlevtcmd_SetHitOffset, LW(1), 1, 0, 32, 0)
                CALL(btlevtcmd_SetHitCursorOffset, LW(1), 1, 0, 41, 0)
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_BIG1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_BIG2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_AnimeChangePose, LW(1), 1, STRING("V_1"))
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, LW(1), 1)
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL_CHILD(evt_attack)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(evt_attack_other_arm)
    SET_INT_READ_ADDR(LW(1))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, LW(0), LW(5), LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(0), LW(5))
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, LW(6), 0, LW(10))
        IF_INT_NOT_EQUAL(LW(10), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, LW(0), LW(5), LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(0), LW(5))
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    RETURN
EVT_END()

char* A_of_B_Attack[] = {
    "S_1A_1", NULL, NULL,
};

char* B_of_B_Attack_front[] = {
    "M_1B_3", "A_1B_1", "A_1B_2", NULL,
    "A_1B_3", NULL, "A_1B_4", "M_1B_4",
    "S_1B_1", NULL,
};

char* B_of_B_Attack_back[] = {
    "M_1B_3", "A_1B_5", "A_1B_6", NULL,
    "A_1B_7", NULL, "A_1B_8", "M_1B_4",
    "S_1B_1", NULL,
};

char* C_of_B_Attack[] = {
    "M_1C_1", "S_1C_2", NULL, "M_1C_2",
    "S_1C_1", NULL,
};

static EVT_BEGIN(evt_attack)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_moamoa_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_moamoa_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        RETURN
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_moamoa_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_moamoa_attack))
    CALL(boss_moamoa_check_zenei_kouei, LW(3), LW(1))
    IF_INT_EQUAL(LW(1), 0)
        SET_VALUE(LF(0), 0)
    ELSE
        SET_VALUE(LF(0), 1)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(6))
    CALL(btlevtcmd_SetUnitWork, LW(6), 0, 0)
    CALL(btlevtcmd_AnimeChangePose, LW(6), 1, STRING("S_1_1"))
    SET_VALUE(LW(15), 0)
    CALL(btlevtcmd_GetUnitWork, LW(6), 5, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        SET_VALUE(LW(1), PTR(A_of_B_Attack))
        SET_VALUE(LW(5), 1)
        SPAWN_EVENT(evt_attack_other_arm)
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(6), 3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        SET_VALUE(LW(1), PTR(C_of_B_Attack))
        SET_VALUE(LW(5), 1)
        SPAWN_EVENT(evt_attack_other_arm)
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        SET_INT_READ_ADDR(PTR(B_of_B_Attack_front))
        SET_VALUE(LW(15), 1)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(14))
            WAIT_MS(2000)
            CALL(evt_snd_sfxoff, LW(14))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
    ELSE
        SET_INT_READ_ADDR(PTR(B_of_B_Attack_back))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, LW(14))
            WAIT_MS(2300)
            CALL(evt_snd_sfxoff, LW(14))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE2_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
    END_IF
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    END_LOOP
    WAIT_MS(1000)
    IF_INT_EQUAL(LF(0), 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_MOVE3_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    IF_INT_EQUAL(LW(15), 0)
        LOOP(0)
            READ_INT(LW(2))
            IF_INT_EQUAL(LW(2), 0)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        END_LOOP
    ELSE
        BEGIN_SIBLING_EVENT
            LOOP(0)
                READ_INT(LW(2))
                IF_INT_EQUAL(LW(2), 0)
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            END_LOOP
            LOOP(0)
                READ_INT(LW(2))
                IF_INT_EQUAL(LW(2), 0)
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            END_LOOP
            SET_VALUE(GF(9), 0)
        END_SIBLING_EVENT
        WAIT_MS(70)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_moamoa_attack), 256, LW(5))
    SWITCH(LW(5))
        CASE_EQUAL(1)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_moamoa_attack))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_moamoa_attack), 256, LW(5))
        CASE_EQUAL(4)
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
    END_SWITCH
    CALL(btlevtcmd_SetUnitWork, LW(6), 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    IF_INT_EQUAL(LW(15), 0)
        LOOP(0)
            READ_INT(LW(2))
            IF_INT_EQUAL(LW(2), 0)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        END_LOOP
    ELSE
        SET_VALUE(GF(9), 1)
        WAIT_WHILE(GF(9))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(btlevtcmd_GetUnitWork, LW(6), 5, LW(10))
    IF_INT_NOT_EQUAL(LW(10), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(10), 1)
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(6), 3, LW(10))
    IF_INT_NOT_EQUAL(LW(10), -1)
        CALL(btlevtcmd_AnimeChangePoseFromTable, LW(10), 1)
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
