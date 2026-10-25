#include "unit_atmic_teresa.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "math_ext.h"
#include "party/party_motion.h"
#include "system.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

// One animated Teresian used by teresa_ext_main's state machine.
typedef struct TeresaEntry {
    u8 state;        // 0x0
    s8 animMode;     // 0x1, 0 loops frames 0-7, 1 plays frames 8-15 once (pose->texId)
    u8 pad_0x2[2];   // 0x2
    s32 timer;       // 0x4, animation frame clock; state 0x14 also waits on it
    s32 delay;       // 0x8, frames to wait before the state machine runs
    s32 counter;     // 0xC, per-state frame countdown, or the interpolation step in state 0x35
    Vec velocity;    // 0x10, added to the matrix translation each frame
    Vec startPos;    // 0x1C, interpolation start for state 0x35
    Vec targetPos;   // 0x28
    u8 easeMode;     // 0x34, intplGetValue mode
    u8 pad_0x35[3];  // 0x35
} TeresaEntry;       // 0x38

// Defined later in the original's data block; forward-declared so the functions above can use them.
s32 atmic_teresa_id;
s32 target_unit_id;
s32 target_parts_id;
f32 atmic_teresa_scale;
TeresaEntry* teresa;
s32 teresa_count;

#pragma pool_data off

#pragma force_active on
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];
static BattleVulnerableStatus regist;
static s32 init_event[26];
static s32 damage_event[10];
static s32 attack_event[101];
s32 kaokakusi_event[110];
s32 press_event[264];
s32 surprise_event[303];
s32 bikkuri_event[285];
static s32 breath_event[254];
s32 breath_event_sub[102];
static s32 wait_event[6];
s32 teresa_entry[19];

// clang-format off
extend_data(80,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(81,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_atmic_teresa = {
    UNIT_ATOMIC_BOO,             // mUnitType
    "btl_un_atmic_teresa",       // name
    40,                          // maxHp
    0,                           // maxFp
    1,                           // mDangerHP
    1,                           // mPerilHP
    52,                          // mLevel
    0,                           // mBonusExp
    1,                           // bonusCoinRolls
    100,                         // bonusCoinRate
    0,                           // baseCoin
    50,                          // escapeRate
    9999,                        // renzokuSoftCap
    120,                         // width
    110,                         // height
    { 30, 105 },                 // statusIconOffset
    { 0.0f, 55.0f, 0.0f },       // centerOffset
    { 0, 0 },                    // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },        // talkTogeBaseOffset
    { 60.0f, 0.0f, -10.0f },     // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },        // burnFlameOffset
    0.0f,                        // field_0x54
    0.0f,                        // field_0x58
    { 60.0f, 0.0f, 0.0f },       // bintaHitOffset
    { 46.0f, 71.5f, 10.0f },     // kissHitOffset
    { 0.0f, 55.0f, 0.0f },       // cutBaseOffset
    120.0f,                      // cutWidth
    110.0f,                      // cutHeight
    0,                           // turnOrder
    0,                           // turnOrderVariance
    0,                           // swallowRate
    2,                           // swallowAttribute
    100,                         // field_0x8C
    20,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },        // starPointDispOffset
    "SFX_BOSS_ATMTLSA_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0x00000004,                  // attributeFlags
    &regist,                     // vulnStatus
    1,                           // numParts
    parts,                       // mParts
    init_event,                  // initEvtScript
    data_table,                  // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleDispPose pose_table[17] = {
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
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

BattleDispPose pose_table_kaokakusi[17] = {
    { 0x01, "A_1" },
    { 0x02, "A_1" },
    { 0x09, "A_1" },
    { 0x05, "A_1" },
    { 0x04, "A_1" },
    { 0x03, "A_1" },
    { 0x1C, "A_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "A_1" },
    { 0x27, "D_1" },
    { 0x2A, "A_1" },
    { 0x28, "A_1" },
    { 0x38, "A_1" },
    { 0x39, "A_1" },
    { 0x41, "T_1" },
    { 0x45, "A_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                      // partNum
        "btl_un_atmic_teresa",  // partName
        "c_atmic_trs",          // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 10.0f, 80.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 110.0f, 0.0f }, // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x00600009,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

static BattleVulnerableStatus regist = {
    40,  // sleepRate
    40,  // stopRate
    40,  // dizzyRate
    100, // poisonRate
    40,  // confuseRate
    100, // electricRate
    100, // burnRate
    30,  // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    80,  // defenseDownRate
    40,  // allergicRate
    0,   // frightRate
    10,  // galeForceRate
    100, // fastRate
    40,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    5,   // instaKillRate
};

BattleWeapon weapon_atmic_teresa_press = {
    NULL,                                       // name
    0,                                          // icon
    ITEM_NULL,                                  // item
    NULL,                                       // description
    100,                                        // baseAccuracy
    0,                                          // baseFpCost
    0,                                          // baseSpCost
    1,                                          // superguardType
    1.0f,                                       // field_0x14
    1,                                          // mStylishCommandMultiplier
    1,                                          // field_0x19
    1,                                          // mBingoSlotChance
    1,                                          // field_0x1B
    weaponGetPowerDefault,                      // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x02101260,                                 // targetClassFlags
    0x20000000,                                 // targetPropertyFlags
    0,                                          // element
    0,                                          // damagePattern
    3,                                          // actionLevel
    2,                                          // field_0x6F
    NULL,                                       // acHelpMsg
    0x00001087,                                 // special
    0x0000006C,                                 // counterResistanceFlags
    0,                                          // targetWeightingFlags
    0,                                          // sleepChance
    0,                                          // sleepTime
    0,                                          // stopChance
    0,                                          // stopTime
    0,                                          // dizzyChance
    0,                                          // dizzyTime
    0,                                          // poisonChance
    0,                                          // poisonTime
    0,                                          // poisonStrength
    0,                                          // confuseChance
    0,                                          // confuseTime
    0,                                          // electricChance
    0,                                          // electricTime
    0,                                          // dodgyChance
    0,                                          // dodgyTime
    0,                                          // burnChance
    0,                                          // burnTime
    0,                                          // freezeChance
    0,                                          // freezeTime
    0,                                          // sizeChangeChance
    0,                                          // sizeChangeTime
    0,                                          // sizeChangeStrength
    0,                                          // atkChangeChance
    0,                                          // atkChangeTime
    0,                                          // atkChangeStrength
    0,                                          // defChangeChance
    0,                                          // defChangeTime
    0,                                          // defChangeStrength
    0,                                          // allergicChance
    0,                                          // allergicTime
    0,                                          // ohkoChance
    0,                                          // chargeStrength
    0,                                          // fastChance
    0,                                          // fastTime
    0,                                          // slowChance
    0,                                          // slowTime
    0,                                          // frightChance
    0,                                          // galeForceChance
    0,                                          // paybackTime
    0,                                          // holdFastTime
    0,                                          // invisibleChance
    0,                                          // invisibleTime
    0,                                          // hpRegenTime
    0,                                          // hpRegenStrength
    0,                                          // fpRegenTime
    0,                                          // fpRegenStrength
    NULL,                                       // mAttackEvtScript
    { 0, 50, 50, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon weapon_atmic_teresa_bikkuri = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000047,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    50,                                       // stopChance
    3,                                        // stopTime
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

BattleWeapon weapon_atmic_teresa_surprise = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000067,                               // special
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
    50,                                       // confuseChance
    3,                                        // confuseTime
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

BattleWeapon weapon_atmic_teresa_breath = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000097,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000004,                               // targetWeightingFlags
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
    CALL(atmic_teresa_id_set, -2)
    CALL_CHILD(teresa_entry)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
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
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(LW(1), 30)
        ADD(LW(1), 20)
        ADD(LW(1), 20)
        SUBTRACT(LW(1), 1)
        CALL(evt_sub_random, LW(1), LW(0))
        SET_VALUE(LW(1), 30)
        IF_INT_LESS(LW(0), LW(1))
            CALL_CHILD(kaokakusi_event)
            GOTO(99)
        END_IF
        ADD(LW(1), 20)
        IF_INT_LESS(LW(0), LW(1))
            CALL_CHILD(press_event)
            GOTO(99)
        END_IF
        CALL_CHILD(bikkuri_event)
        GOTO(99)
    ELSE
        SET_VALUE(LW(1), 50)
        ADD(LW(1), 50)
        SUBTRACT(LW(1), 1)
        CALL(evt_sub_random, LW(1), LW(0))
        SET_VALUE(LW(1), 50)
        IF_INT_LESS(LW(0), LW(1))
            CALL_CHILD(surprise_event)
            GOTO(99)
        END_IF
        CALL_CHILD(breath_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(kaokakusi_event)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 60)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 10, 4, 0, -1)
    WAIT_FRAMES(20)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_FACE_HIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1"))
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 20, 0, 5, 0, -1)
    WAIT_FRAMES(10)
    LOOP(15)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 12, 0)
    END_LOOP
    WAIT_FRAMES(20)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_kaokakusi))
    RETURN
EVT_END()

EVT_BEGIN(press_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_press))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_press), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_press))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_press))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 10)
    CALL(btlevtcmd_DivePosition, -2, -125, 100, LW(2), 120, 10, 4, 0, -1)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("Z_1"))
    CALL(btlevtcmd_DivePosition, -2, -125, 0, LW(2), 30, 0, 1, 0, -1)
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_press), 0, LW(5))
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
        CASE_END
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
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_press))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_press), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(0)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(98)
    WAIT_FRAMES(60)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("W_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 50, 6, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    WAIT_MS(250)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(surprise_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_surprise))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_surprise), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_surprise))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_surprise))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_FACE_HIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 10, 4, 0, -1)
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_DivePosition, -2, 0, LW(1), LW(2), 10, 0, 5, 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_SURPRISED1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_w, 0, 10, 0, 60, 0)
    CALL(evt_btl_camera_shake_h, 0, 10, 0, 60, 4)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_surprise), 0, LW(5))
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
        CASE_END
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
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_surprise))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_surprise), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(0)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(98)
    WAIT_FRAMES(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("W_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, -20, 6, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    WAIT_MS(250)
    LABEL(99)
    RETURN
EVT_END()

EVT_BEGIN(bikkuri_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_bikkuri))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_bikkuri), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_bikkuri))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_press))
    CALL(teresa_appear_bikkuri)
    WAIT_FRAMES(75)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 5)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_S_ROUND1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(teresa_wait_bikkuri)
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_SURPRISED2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_bikkuri), 0, LW(5))
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
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_bikkuri))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_bikkuri), 256, LW(5))
    GOTO(97)
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(0)
    END_IF
    LABEL(98)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_WHISTLE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(teresa_wait_bikkuri2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_UNION1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        ADD(LW(2), 1)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 8, LW(0), LW(1), LW(2), FLOAT(1.25f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(30)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(250)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(breath_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_atmic_teresa_breath), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(target_id_set, LW(3), LW(4))
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_FACE_HIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 180, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, 10, 4, 0, -1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3"))
    CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 10, 0, 5, 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_BLOW2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(teresa_appear_breath, 0)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(10)
    IF_INT_EQUAL(LW(3), -1)
        GOTO(20)
    END_IF
    SPAWN_EVENT(breath_event_sub)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(10)
    LABEL(20)
    WAIT_FRAMES(120)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(teresa_wait_breath)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_WHISTLE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    CALL(teresa_wait_breath2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ATMTLSA_UNION1"), EVTDAT_POINTER_MAX, 0, LW(15))
    LOOP(3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 40)
        ADD(LW(2), 1)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 8, LW(0), LW(1), LW(2), FLOAT(1.25f), 0, 0, 0, 0, 0, 0, 0)
        WAIT_FRAMES(30)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(1500)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    RETURN
EVT_END()

EVT_BEGIN(breath_event_sub)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 0, LW(5))
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
    WAIT_FRAMES(15)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 0, LW(5))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 0, LW(5))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_atmic_teresa_breath))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_atmic_teresa_breath), 256, LW(5))
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

s32 atmic_teresa_id = -1;

s32 target_unit_id = -1;

s32 target_parts_id = -1;

f32 atmic_teresa_scale = 1.0f;

ExtMakeTextureEntry teresa_ext_entry_data[17] = {
    { "c_teresa", "S_1", 0.0f },
    { "c_teresa", "S_1", 2.0f },
    { "c_teresa", "S_1", 4.0f },
    { "c_teresa", "S_1", 6.0f },
    { "c_teresa", "S_1", 8.0f },
    { "c_teresa", "S_1", 10.0f },
    { "c_teresa", "S_1", 6.0f },
    { "c_teresa", "S_1", 7.0f },
    { "c_teresa", "A_1", 0.0f },
    { "c_teresa", "A_2", 6.0f },
    { "c_teresa", "A_2", 7.0f },
    { "c_teresa", "A_2", 8.0f },
    { "c_teresa", "A_2", 9.0f },
    { "c_teresa", "A_2", 10.0f },
    { "c_teresa", "A_2", 11.0f },
    { "c_teresa", "A_2", 12.0f },
    { NULL, NULL, 0.0f },
};

// This is where the original has it, just before the first script that uses it.

EVT_BEGIN(teresa_entry)
    CALL(evt_npc_entry, STRING("ext_teresa"), STRING("c_teresa"))
    CALL(evt_npc_set_position, STRING("ext_teresa"), 0, -1000, 0)
    CALL(evt_ext_entry, 30, PTR(teresa_ext_entry_data), PTR(teresa_ext_init), PTR(teresa_ext_main), 0)
    RETURN
EVT_END()

// This is where the original has it, just before the first script that uses it.
void teresa_ext_init(void) {
    s32 i;
    ExtPose* pose = extGetPosePtr();

    teresa = BattleAlloc(0x690);
    for (i = 0; i < extGetPoseNum(); i++, pose++) {
        pose->flags = EXT_POSE_VISIBLE;
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = 0.0f;
        (*pose->matrix)[1][3] = -1000.0f;
        (*pose->matrix)[2][3] = 0.0f;
        pose->heading = 90.0f;
        teresa[i].state = 0;
        teresa[i].animMode = 0;
        teresa[i].timer = 0;
        teresa[i].velocity = (Vec){0.0f, 0.0f, 0.0f};
    }
}

static inline f32 teresa_intpl(s32 mode, f32 start, f32 end, s32 step, s32 steps) {
    return intplGetValue(mode, step, steps, start, end);
}

void teresa_ext_main(void) {
    s32 i;
    ExtPose* pose = extGetPosePtr();
    TeresaEntry* entry;
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, atmic_teresa_id);
    BattleUnit* target = BattleGetUnitPtr(_battleWorkPointer, target_unit_id);
    s32 sp60[8] = {0, 2, 5, 7, 10, 12, 15, 17};
    f32 sp80[60] = {
        -30.0f, 8.0f,   -28.0f, 10.0f, -26.0f, 6.0f,   -24.0f, 30.0f, -22.0f, 16.0f,
        -20.0f, 50.0f,  -18.0f, 14.0f, -16.0f, 18.0f,  -14.0f, 60.0f, -12.0f, 34.0f,
        -10.0f, 66.0f,  -8.0f,  8.0f,  -6.0f,  28.0f,  -4.0f,  40.0f, -2.0f,  46.0f,
        0.0f,   20.0f,  2.0f,   26.0f, 4.0f,   52.0f,  6.0f,   74.0f, 8.0f,   68.0f,
        10.0f,  0.0f,   12.0f,  28.0f, 14.0f,  44.0f,  16.0f,  72.0f, 18.0f,  42.0f,
        20.0f,  76.0f,  22.0f,  56.0f, 24.0f,  32.0f,  26.0f,  4.0f,  28.0f,  2.0f,
    };
    u8 sp2C[7] = {0, 1, 2, 3, 4, 5, 6};
    s32 j;
    s32 count;
    BattleUnitPart* part;
    f32 posX, posY, posZ;
    f32 hitX, hitY, hitZ;
    f32 scaleX, scaleY, scaleZ;
    f32 angle;

    teresa_count++;
    if (teresa_count >= 360) {
        teresa_count -= 360;
    }
    if (unit != NULL) {
        BtlUnit_GetPos(unit, &posX, &posY, &posZ);
        posY += 40.0f;
        BtlUnit_GetScale(unit, &scaleX, &scaleY, &scaleZ);
        if (scaleX > atmic_teresa_scale) {
            scaleX -= 0.005f;
            if (scaleX <= atmic_teresa_scale) {
                scaleX = atmic_teresa_scale;
            }
            BtlUnit_SetScale(unit, scaleX, scaleX, scaleZ);
        }
        if (scaleX < atmic_teresa_scale) {
            scaleX += 0.005f;
            if (scaleX >= atmic_teresa_scale) {
                scaleX = atmic_teresa_scale;
            }
            BtlUnit_SetScale(unit, scaleX, scaleX, scaleZ);
        }
    }
    if (target != NULL) {
        part = BtlUnit_GetPartsPtr(target, target_parts_id);
        BtlUnit_GetHitPos(target, part, &hitX, &hitY, &hitZ);
    }
    for (i = 0; i < extGetPoseNum(); i++, pose++) {
        entry = &teresa[i];

        switch (entry->animMode) {
        case 1:
            entry->timer++;
            if (entry->timer <= 0x1E) {
                pose->texId = 8;
            } else if (entry->timer < 0x2C) {
                pose->texId = (entry->timer - 0x1E) / 2 + 9;
            } else {
                pose->texId = 0xF;
            }
            break;
        default:
            entry->timer++;
            if (entry->timer >= 0x18) {
                entry->timer -= 0x18;
            }
            pose->texId = entry->timer / 3;
            break;
        }
        if (entry->delay >= 1) {
            entry->delay--;
            continue;
        }
        (*pose->matrix)[0][3] += entry->velocity.x;
        (*pose->matrix)[1][3] += entry->velocity.y;
        (*pose->matrix)[2][3] += entry->velocity.z;
        if ((*pose->matrix)[1][3] < 0.0f) {
            (*pose->matrix)[1][3] = 0.0f;
        }
        switch (entry->state) {
        case 0:
            (*pose->matrix)[0][3] = 0.0f;
            (*pose->matrix)[1][3] = -1000.0f;
            (*pose->matrix)[2][3] = 0.0f;
            pose->heading = 90.0f;
            (*pose->matrix)[1][1] = 1.0f;
            (*pose->matrix)[0][0] = 1.0f;
            break;
        case 1:
            pose->flags &= ~EXT_POSE_BILLBOARD;
            (*pose->matrix)[0][3] = posX;
            (*pose->matrix)[1][3] = posY;
            (*pose->matrix)[2][3] = posZ - 5.0f;
            (*pose->matrix)[0][0] = 0.0f;
            (*pose->matrix)[1][1] = 0.0f;
            angle = 0.785398f * (f32)i;
            entry->velocity.x = 2.0f * (f32)cos(angle);
            entry->velocity.y = 2.0f * (f32)sin(angle);
            entry->velocity.z = 0.0f;
            entry->delay = sp60[i];
            entry->counter = 0x32;
            entry->state = 5;
            break;
        case 5:
            entry->counter--;
            if (entry->counter == 0x19) {
                atmic_teresa_scale -= 0.05f;
                if (i % 2 == 0) {
                    Vec pos = {0.0f, 0.0f, 0.0f};
                    pos.x = (*pose->matrix)[0][3];
                    pos.y = (*pose->matrix)[1][3];
                    pos.z = (*pose->matrix)[2][3];
                    BtlUnit_snd_se_pos(unit, "SFX_BOSS_ATMTLSA_DIVIDE1", 0xF1194D80, 0, &pos);
                }
            }
            (*pose->matrix)[0][0] += 0.02f;
            (*pose->matrix)[1][1] += 0.02f;
            if (entry->counter <= 0) {
                pose->flags |= EXT_POSE_BILLBOARD;
                entry->state = 0xA;
                entry->counter = 0x96;
                entry->velocity.x = 0.0f;
                entry->velocity.y = 0.0f;
                entry->velocity.z = 0.0f;
                if (i % 2 == 0) {
                    Vec pos = {0.0f, 0.0f, 0.0f};
                    pos.x = (*pose->matrix)[0][3];
                    pos.y = (*pose->matrix)[1][3];
                    pos.z = (*pose->matrix)[2][3];
                    BtlUnit_snd_se_pos(unit, "SFX_BOSS_ATMTLSA_S_MOVE1", 0xF1194D80, 0, &pos);
                }
            }
            break;
        case 0xA:
            angle = 6.283184f * (0.125f * (f32)i + 2.0f * (f32)teresa_count / 360.0f);
            entry->targetPos.x = 90.0f * (f32)cos(angle) - 100.0f;
            entry->targetPos.y = 10.0f;
            entry->targetPos.z = 90.0f * (f32)sin(angle);
            if ((*pose->matrix)[0][3] >= -100.0f) {
                pose->heading = 90.0f;
            } else {
                pose->heading = 270.0f;
            }
            entry->velocity.x = (entry->targetPos.x - (*pose->matrix)[0][3]) / 20.0f;
            entry->velocity.y = (entry->targetPos.y - (*pose->matrix)[1][3]) / 20.0f;
            entry->velocity.z = (entry->targetPos.z - (*pose->matrix)[2][3]) / 20.0f;
            if (entry->counter <= 0) {
                entry->state = 0xF;
                entry->velocity.x = 0.0f;
                entry->velocity.y = 0.0f;
                entry->velocity.z = 0.0f;
            }
            entry->counter--;
            break;
        case 0xF:
            angle = 6.283184f * (0.125f * (f32)i + 2.0f * (f32)teresa_count / 360.0f);
            entry->targetPos.x = 90.0f * (f32)cos(angle) - 100.0f;
            entry->targetPos.y = 10.0f;
            entry->targetPos.z = 90.0f * (f32)sin(angle);
            if ((*pose->matrix)[0][3] >= -100.0f) {
                pose->heading = 90.0f;
            } else {
                pose->heading = 270.0f;
            }
            entry->velocity.x = (entry->targetPos.x - (*pose->matrix)[0][3]) / 20.0f;
            entry->velocity.y = (entry->targetPos.y - (*pose->matrix)[1][3]) / 20.0f;
            entry->velocity.z = (entry->targetPos.z - (*pose->matrix)[2][3]) / 20.0f;
            count = 0;
            for (j = 0; j < 8; j++) {
                if (teresa[j].state == 0xF) {
                    count++;
                } else {
                    break;
                }
            }
            if (count == 8) {
                for (j = 0; j < 8; j++) {
                    TeresaEntry* e = &teresa[j];
                    e->state = 0x14;
                    e->animMode = 1;
                    e->timer = 0;
                    e->velocity.x = 0.0f;
                    e->velocity.y = 0.0f;
                    e->velocity.z = 0.0f;
                }
            }
            break;
        case 0x14:
            if (entry->timer >= 0xB4) {
                entry->state = 0x17;
                entry->animMode = 0;
                entry->timer = 0;
                entry->counter = 0x3C;
                entry->delay = sp60[i];
                pose->heading = 90.0f;
            }
            break;
        case 0x17:
            entry->targetPos.x = posX - 35.0f + (f32)irand(90);
            entry->targetPos.y = posY - 35.0f + (f32)irand(90);
            entry->targetPos.z = posZ - 30.0f - 0.05f * (f32)i;
            entry->counter = 0x3C;
            entry->state = 0x19;
            if (i % 2 == 0) {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = (*pose->matrix)[0][3];
                pos.y = (*pose->matrix)[1][3];
                pos.z = (*pose->matrix)[2][3];
                BtlUnit_snd_se_pos(unit, "SFX_BOSS_ATMTLSA_S_MOVE1", 0xF1194D80, 0, &pos);
            }
            break;
        case 0x19:
            entry->velocity.x = (entry->targetPos.x - (*pose->matrix)[0][3]) / 20.0f;
            entry->velocity.y = (entry->targetPos.y - (*pose->matrix)[1][3]) / 20.0f;
            entry->velocity.z = (entry->targetPos.z - (*pose->matrix)[2][3]) / 20.0f;
            if (entry->counter <= 0) {
                entry->state = 0x1E;
                entry->velocity.x = 0.0f;
                entry->velocity.y = 0.0f;
                entry->velocity.z = 0.0f;
                pose->flags &= ~EXT_POSE_BILLBOARD;
            }
            entry->counter--;
            break;
        case 0x1F:
            (*pose->matrix)[0][0] -= 0.0125f;
            (*pose->matrix)[1][1] = (*pose->matrix)[0][0];
            if ((*pose->matrix)[0][0] <= 0.0f) {
                entry->state = 0;
            }
            break;
        case 0x33:
        case 0x34:
            pose->flags &= ~EXT_POSE_BILLBOARD;
            (*pose->matrix)[0][3] = posX - 15.0f;
            (*pose->matrix)[1][3] = posY - 5.0f;
            (*pose->matrix)[2][3] = posZ + 5.0f;
            (*pose->matrix)[1][1] = 0.0f;
            (*pose->matrix)[0][0] = 0.0f;
            entry->velocity.x = 0.0f;
            entry->velocity.y = 0.0f;
            entry->velocity.z = 0.0f;
            entry->startPos.x = (*pose->matrix)[0][3];
            entry->startPos.y = (*pose->matrix)[1][3];
            entry->startPos.z = (*pose->matrix)[2][3];
            if (entry->state == 0x33) {
                entry->targetPos.x = hitX + sp80[i * 2] - 200.0f;
            } else {
                entry->targetPos.x = hitX + sp80[i * 2] - 200.0f;
            }
            if (entry->state == 0x33) {
                entry->targetPos.y = hitY + sp80[i * 2 + 1] - 50.0f;
            } else {
                entry->targetPos.y = hitY + sp80[i * 2 + 1] - 50.0f;
            }
            entry->targetPos.z = hitZ - 10.0f - 0.05f * (f32)i;
            entry->delay = i * 3;
            entry->counter = 0;
            entry->state = 0x35;
            entry->easeMode = sp2C[irand(7)];
            break;
        case 0x35:
            atmic_teresa_scale -= 0.015f;
            entry->state = 0x37;
            break;
        case 0x37:
            (*pose->matrix)[0][0] += 0.04f;
            if ((*pose->matrix)[0][0] >= 1.0f) {
                (*pose->matrix)[0][0] = 1.0f;
            }
            (*pose->matrix)[1][1] = (*pose->matrix)[0][0];
            (*pose->matrix)[0][3] = teresa_intpl(entry->easeMode, entry->startPos.x, entry->targetPos.x, entry->counter, 0x1E);
            (*pose->matrix)[1][3] = teresa_intpl(entry->easeMode, entry->startPos.y, entry->targetPos.y, entry->counter, 0x1E);
            (*pose->matrix)[2][3] = teresa_intpl(entry->easeMode, entry->startPos.z, entry->targetPos.z, entry->counter, 0x1E);
            if (entry->counter >= 0x1E) {
                entry->state = 0x3C;
            }
            entry->counter++;
            break;
        case 0x3F:
            entry->targetPos.x = posX - 35.0f + (f32)irand(90);
            entry->targetPos.y = posY - 35.0f + (f32)irand(90);
            entry->targetPos.z = posZ - 30.0f - 0.05f * (f32)i;
            entry->counter = 0x3C;
            entry->state = 0x41;
            if (i % 2 == 0) {
                Vec pos = {0.0f, 0.0f, 0.0f};
                pos.x = (*pose->matrix)[0][3];
                pos.y = (*pose->matrix)[1][3];
                pos.z = (*pose->matrix)[2][3];
                BtlUnit_snd_se_pos(unit, "SFX_BOSS_ATMTLSA_S_MOVE1", 0xF1194D80, 0, &pos);
            }
            break;
        case 0x41:
            entry->velocity.x = (entry->targetPos.x - (*pose->matrix)[0][3]) / 15.0f;
            entry->velocity.y = (entry->targetPos.y - (*pose->matrix)[1][3]) / 15.0f;
            entry->velocity.z = (entry->targetPos.z - (*pose->matrix)[2][3]) / 15.0f;
            if (entry->counter <= 0) {
                entry->state = 0x46;
                entry->velocity.x = 0.0f;
                entry->velocity.y = 0.0f;
                entry->velocity.z = 0.0f;
            }
            entry->counter--;
            break;
        case 0x47:
            (*pose->matrix)[0][0] -= 0.0125f;
            (*pose->matrix)[1][1] = (*pose->matrix)[0][0];
            if ((*pose->matrix)[0][0] <= 0.0f) {
                entry->state = 0;
            }
            break;
        }
    }
    dispEntry(4, 0.0f, 1, (DispCallback)teresa_ext_disp, NULL);
}

void teresa_ext_disp(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

s32 atmic_teresa_id_set(EventEntry* event, BOOL isFirstCall) {
    s32 id = evtGetValue(event, event->args[0]);
    atmic_teresa_id = BattleTransID(event, id);
    return EVT_RETURN_DONE;
}

s32 target_id_set(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 a = evtGetValue(event, args[0]);
    s32 b = evtGetValue(event, args[1]);
    target_unit_id = BattleTransID(event, a);
    target_parts_id = b;
    return EVT_RETURN_DONE;
}

s32 teresa_appear_bikkuri(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    for (i = 0; i < 8; i++) {
        teresa[i].state = 1;
    }
    return EVT_RETURN_DONE;
}

s32 teresa_wait_bikkuri(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    for (i = 0; i < 8; i++) {
        if (teresa[i].state != 0x0F) {
            return EVT_RETURN_BLOCK;
        }
    }
    return EVT_RETURN_DONE;
}

s32 teresa_wait_bikkuri2(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    for (i = 0; i < 8; i++) {
        if (teresa[i].state != 0x1E) {
            return EVT_RETURN_BLOCK;
        }
    }
    for (i = 0; i < 8; i++) {
        teresa[i].state = 0x1F;
    }
    atmic_teresa_scale = 1.0f;
    return EVT_RETURN_DONE;
}

s32 teresa_appear_breath(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 state;
    s32 t = evtGetValue(event, event->args[0]) - 1;

    if ((u32)t <= 0) {
        state = 0x33;
    } else {
        state = 0x34;
    }
    for (i = 0; i < extGetPoseNum(); i++) {
        teresa[i].state = state;
    }
    return EVT_RETURN_DONE;
}

s32 teresa_wait_breath(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    for (i = 0; i < 30; i++) {
        if (teresa[i].state != 0x3C) {
            return EVT_RETURN_BLOCK;
        }
    }
    for (i = 0; i < 30; i++) {
        TeresaEntry* entry = &teresa[i];
        entry->state = 0x3F;
        entry->delay = 30;
    }
    return EVT_RETURN_DONE;
}

s32 teresa_wait_breath2(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    for (i = 0; i < 30; i++) {
        if (teresa[i].state != 0x46) {
            return EVT_RETURN_BLOCK;
        }
    }
    for (i = 0; i < 30; i++) {
        teresa[i].state = 0x47;
    }
    atmic_teresa_scale = 1.0f;
    return EVT_RETURN_DONE;
}



// These live in .bss in the original, so they must not be initialized.
s32 teresa_count;
TeresaEntry* teresa;
// clang-format on
