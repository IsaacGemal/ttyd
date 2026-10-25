#include "unit_boss_kanbu3.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_batstage.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "manager/evtmgr_cmd.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 btldefaultevt_Dummy[];
extern s32 down_ceil_20_text_9C8C[];
extern BattleUnitKind unit_boss_gundan_zako_group1;
extern BattleUnitKind unit_boss_gundan_zako_group2;
extern BattleUnitKind unit_boss_gundan_zako_group3;
extern BattleUnitKind unit_gundan_zako_magician_20_data_32438;
extern s32 up_ceil[];
extern s32 zakogun_entry[];
extern s32 zakogun_escape1[];
extern s32 zakogun_escape2[];
extern s32 zakogun_escape3[];
extern s32 zakogun_ext_init1[];
extern s32 zakogun_ext_init2[];
extern s32 zakogun_ext_init3[];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[1];
static s32 init_event[59];
static s32 entry_event[45];
static s32 damage_event[315];
static s32 dead_event[402];
static s32 phase_event[55];
s32 call_event[284];
static s32 attack_event[136];
static s32 guard_event[56];
static s32 guard_cancel_event[69];
s32 no_event[27];
static s32 charge_event[71];
static s32 wait_event[6];
static BattleDispPose pose_table[18];
static BattleDispPose pose_table_guard[18];
static s32 normal_attack_event[365];

// clang-format off
extend_data(76,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(77,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_kanbu3 = {
    UNIT_LORD_CRUMP_CHAPTER5,   // mUnitType
    "btl_un_kanbu3",            // name
    30,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    57,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    8,                          // field_0x14
    56,                         // width
    59,                         // height
    { 0, 56 },                  // statusIconOffset
    { 0.0f, 26.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 28.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 21.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 18.0f, 38.35f, 0.0f },    // kissHitOffset
    { 0.0f, 26.0f, 0.0f },      // cutBaseOffset
    42.0f,                      // cutWidth
    52.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_KANBU_DAMAGED1",  // damageSfxName
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

static BattleUnitDefense defence_guard = { { 2, 2, 2, 2, 2 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    80,  // sleepRate
    70,  // stopRate
    60,  // dizzyRate
    100, // poisonRate
    80,  // confuseRate
    100, // electricRate
    100, // burnRate
    60,  // freezeRate
    100, // hugeRate
    85,  // tinyRate
    100, // attackUpRate
    85,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    60,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    60,  // slowRate
    100, // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

static BattleDataEntry data_table[3] = {
    { 0x31, dead_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

BattleDataEntry data_table2[2] = {
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_kanbu3",       // partName
        "c_kanbu",             // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 45.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 50.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // shadowSize
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

BattleWeapon weapon_kanbu3_attack = {
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
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0,                                        // counterResistanceFlags
    0x00000004,                               // targetWeightingFlags
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

BattleWeapon weapon_kanbu3_charge = {
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
    3,                                        // chargeStrength
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

BattleUnitSetup entry_kenkyu[1] = {
    { &unit_gundan_zako_magician_20_data_32438, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_group1[1] = {
    { &unit_boss_gundan_zako_group1, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_group2[1] = {
    { &unit_boss_gundan_zako_group2, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_group3[1] = {
    { &unit_boss_gundan_zako_group3, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_KANBU_MOVE1L"), STRING("SFX_BOSS_KANBU_MOVE1R"), 0, 10, 10)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_KANBU_MOVE1L"), STRING("SFX_BOSS_KANBU_MOVE1R"), 0, 6, 6)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL_CHILD(zakogun_entry)
    CALL(zakogun_ext_init1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    WAIT_FRAMES(120)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 166, 88, 422, 166, 60, 0, 20, 11)
    WAIT_FRAMES(20)
    CALL(evt_msg_print, 2, STRING("stg5_muj_125_00"), 0, -2)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    WAIT_MS(500)
    RETURN
EVT_END()

USER_FUNC(check_exist_group) {
    s32 i;
    s32* args = event->args;
    BattleWork* work = _battleWorkPointer;
    s32 unitId = -1;
    BattleUnit* unit;

    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(work, i);
        if (unit != NULL) {
            if (unit->initialKind == UNIT_X_NAUTS_CRUMP_FORMATION1 ||
                unit->initialKind == UNIT_X_NAUTS_CRUMP_FORMATION2 ||
                unit->initialKind == UNIT_X_NAUTS_CRUMP_FORMATION3) {
                unitId = i;
            }
        }
    }
    evtSetValue(event, args[0], unitId);
    return EVT_RETURN_DONE;
}

USER_FUNC(check_exist_kenkyu) {
    s32 i;
    s32* args = event->args;
    BattleWork* work = _battleWorkPointer;
    s32 unitId = -1;
    BattleUnit* unit;

    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(work, i);
        if (unit != NULL) {
            if (unit->initialKind == UNIT_X_NAUT_PHD) {
                unitId = i;
            }
        }
    }
    evtSetValue(event, args[0], unitId);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_1"))
                CALL(btlevtcmd_WaitAttackEnd)
                WAIT_FRAMES(120)
                CALL(check_exist_group, LW(0))
                IF_INT_NOT_EQUAL(LW(0), -1)
                    CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_ESCAPE1"), LW(13))
                    CALL(zakogun_escape3)
                    CALL(evt_snd_sfxoff, LW(13))
                    CALL(btlevtcmd_KillUnit, LW(0), 0)
                END_IF
            END_IF
        END_IF
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 15)
            CALL(btlevtcmd_WaitAttackEnd)
            WAIT_FRAMES(90)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
            CALL(check_exist_group, LW(0))
            IF_INT_NOT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetUnitWork, LW(0), 1, LW(1))
                IF_INT_NOT_EQUAL(LW(1), 0)
                    WAIT_FRAMES(90)
                    GOTO(10)
                END_IF
            ELSE
                GOTO(10)
            END_IF
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
            WAIT_FRAMES(16)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
            WAIT_FRAMES(16)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
            WAIT_FRAMES(60)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
            CALL(check_exist_group, LW(0))
            IF_INT_NOT_EQUAL(LW(0), -1)
                CALL(zakogun_escape1)
                CALL(btlevtcmd_KillUnit, LW(0), 0)
            END_IF
            WAIT_FRAMES(20)
            LABEL(10)
            WAIT_FRAMES(20)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(evt_btl_camera_set_prilimit, 1)
            CALL(evt_btl_camera_set_mode, 1, 3)
            CALL(evt_btl_camera_set_moveto, 1, 166, 88, 422, 166, 60, 0, 20, 11)
            WAIT_FRAMES(20)
            CALL(evt_msg_print, 2, STRING("stg5_muj_125_01"), 0, -2)
            CALL(evt_btl_camera_set_prilimit, 0)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            WAIT_MS(500)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
            WAIT_FRAMES(16)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
            WAIT_FRAMES(16)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
            WAIT_FRAMES(60)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(evt_msg_print, 2, STRING("stg5_muj_125_02"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            WAIT_MS(500)
            CALL(zakogun_ext_init2)
            CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_group2), 0)
            CALL(btlevtcmd_SetPos, LW(0), 0, 80, 0)
            CALL(btlevtcmd_SetHomePos, LW(0), 0, 80, 0)
            CALL(down_ceil_20_text_9C8C)
            WAIT_FRAMES(60)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
            CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        END_IF
        GOTO(99)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence))
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    END_IF
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_WaitAttackEnd)
        CALL(check_exist_group, LW(0))
        IF_INT_NOT_EQUAL(LW(0), -1)
            CALL(btlevtcmd_GetUnitWork, LW(0), 1, LW(1))
            IF_INT_NOT_EQUAL(LW(1), 0)
                WAIT_FRAMES(180)
            END_IF
        END_IF
        CALL(check_exist_group, LW(0))
        IF_INT_NOT_EQUAL(LW(0), -1)
            CALL(zakogun_escape1)
            CALL(btlevtcmd_KillUnit, LW(0), 0)
        END_IF
        CASE_END
        CASE_EQUAL(1)
        CALL(btlevtcmd_WaitAttackEnd)
        CALL(check_exist_group, LW(0))
        IF_INT_NOT_EQUAL(LW(0), -1)
            CALL(btlevtcmd_GetUnitWork, LW(0), 2, LW(1))
            IF_INT_NOT_EQUAL(LW(1), 0)
                WAIT_FRAMES(180)
            END_IF
        END_IF
        CALL(check_exist_group, LW(0))
        IF_INT_NOT_EQUAL(LW(0), -1)
            CALL(up_ceil)
            CALL(zakogun_escape2)
            CALL(btlevtcmd_KillUnit, LW(0), 0)
        END_IF
        CASE_END
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_MovePosition, -2, 300, LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_moveto, 0, 220, 87, 419, 220, 60, 0, 60, 11)
    WAIT_FRAMES(60)
    CALL(evt_batstage_set_aud_dark, 0, 1, 0)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, 250, 30, 104, 4, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(30)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, 220, 30, 103, 5, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(25)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, 280, 30, 102, 6, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, 235, 60, 101, 7, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(10)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, 265, 60, 100, 8, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(5)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    CALL(btlevtcmd_SetHp, -2, LW(0))
    CALL(btlevtcmd_OnOffStatus, -2, 27, 0, 0, 0)
    WAIT_FRAMES(95)
    CALL(evt_batstage_return_aud_dark_base, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 80)
    SUBTRACT(LW(2), 20)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_group3), 0)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
        CALL(btlevtcmd_ChangeFaceDirection, -2, -1)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_MOVE2"), 0)
    CALL(zakogun_ext_init3)
    WAIT_FRAMES(120)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table2))
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 166, 88, 422, 166, 60, 0, 20, 11)
    WAIT_FRAMES(20)
    CALL(evt_msg_print, 2, STRING("stg5_muj_125_03"), 0, -2)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    WAIT_MS(500)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_EQUAL(LW(0), 1)
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
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        CALL_CHILD(call_event)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(call_event)
    CALL(check_exist_group, LW(15))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        IF_INT_EQUAL(LW(15), -1)
            GOTO(10)
        END_IF
        CASE_END
        CASE_EQUAL(1)
        IF_INT_EQUAL(LW(15), -1)
            GOTO(10)
        END_IF
        CASE_END
        CASE_EQUAL(2)
        IF_INT_EQUAL(LW(15), -1)
            GOTO(10)
        END_IF
        CASE_END
    END_SWITCH
    SET_VALUE(LW(0), 0)
    GOTO(99)
    LABEL(10)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_BACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_1"))
        WAIT_FRAMES(16)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_BACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_1"))
        CASE_END
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
        WAIT_FRAMES(16)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
        WAIT_FRAMES(16)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_ON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_2"))
        CASE_END
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_BACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_1"))
        WAIT_FRAMES(16)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_CALL_BACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("C_1"))
        CASE_END
    END_SWITCH
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(100)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(15), -1)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 80)
            SUBTRACT(LW(2), 20)
            CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_group1), 0)
            CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
            CALL(zakogun_ext_init1)
            WAIT_FRAMES(120)
            CASE_END
            CASE_EQUAL(1)
            CALL(zakogun_ext_init2)
            CALL(btlevtcmd_SpawnUnit, LW(0), PTR(entry_group2), 0)
            CALL(btlevtcmd_SetPos, LW(0), 0, 80, 0)
            CALL(btlevtcmd_SetHomePos, LW(0), 0, 80, 0)
            CALL(down_ceil_20_text_9C8C)
            WAIT_FRAMES(60)
            CASE_END
            CASE_EQUAL(2)
            CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 80)
            SUBTRACT(LW(2), 20)
            CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_group3), 0)
            CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_MOVE2"), 0)
            CALL(zakogun_ext_init3)
            WAIT_FRAMES(120)
            CASE_END
        END_SWITCH
    END_IF
    SET_VALUE(LW(0), 1)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            LABEL(10)
            SET_VALUE(LW(0), 75)
            ADD(LW(0), 0)
            ADD(LW(0), 30)
            SUBTRACT(LW(0), 1)
            CALL(evt_sub_random, LW(0), LW(1))
            SET_VALUE(LW(0), 75)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(&weapon_kanbu3_attack))
                CALL_CHILD(normal_attack_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 0)
            IF_INT_LESS(LW(1), LW(0))
                CALL_CHILD(guard_event)
                GOTO(99)
            END_IF
            CALL_CHILD(call_event)
            IF_INT_EQUAL(LW(0), 1)
                GOTO(99)
            ELSE
                GOTO(10)
            END_IF
        ELSE
            CALL(evt_sub_random, 99, LW(1))
            IF_INT_LESS(LW(1), 50)
                CALL_CHILD(guard_cancel_event)
            ELSE
                CALL_CHILD(no_event)
            END_IF
        END_IF
    ELSE
        SET_VALUE(LW(0), 50)
        ADD(LW(0), 0)
        SUBTRACT(LW(0), 1)
        CALL(evt_sub_random, LW(0), LW(1))
        IF_INT_LESS(LW(1), 50)
            SET_VALUE(LW(9), PTR(&weapon_kanbu3_attack))
            CALL_CHILD(normal_attack_event)
            GOTO(99)
        END_IF
        CALL_CHILD(charge_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(guard_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_FRAMES(120)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_1"))
    WAIT_FRAMES(16)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_2"))
    CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence_guard))
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_guard))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    WAIT_FRAMES(60)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

static EVT_BEGIN(guard_cancel_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_FRAMES(120)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 15, 0, FLOAT(1.0f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 15, -1)
    CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&defence))
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    WAIT_FRAMES(60)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

EVT_BEGIN(no_event)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_FRAMES(120)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

static EVT_BEGIN(charge_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_kanbu3_charge))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_kanbu3_charge))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_1"))
    WAIT_FRAMES(16)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_2"))
    WAIT_FRAMES(120)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(60)
        CALL(evt_btl_camera_set_mode, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_kanbu3_charge))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_kanbu3_charge), 256, LW(5))
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static BattleDispPose pose_table[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "X_1" },
    { 0x1C, "S_1" },
    { 0x41, "T_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x2A, "R_2" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDispPose pose_table_guard[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "X_1" },
    { 0x1C, "G_2" },
    { 0x41, "T_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "G_2" },
    { 0x27, "D_1" },
    { 0x2A, "R_2" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "G_2" },
};

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
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
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(2.0f), FLOAT(0.2f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
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
        LABEL(90)
        GOTO(98)
        LABEL(91)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(98)
        LABEL(98)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_HIP_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
        CALL(btlevtcmd_JumpSetting, -2, 0, 0, FLOAT(0.2f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
        CALL(btlevtcmd_JumpSetting, -2, 0, 0, FLOAT(0.2f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1C"))
        WAIT_FRAMES(25)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_HIP_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KANBU_HIP_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_1"))
        WAIT_FRAMES(15)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpSetting, -2, 15, 0, FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(7.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
        RETURN
EVT_END()

// clang-format on
