#include "unit_wanawana.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "forcedata.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
// This unit has its own copy of tanf (see its asm); math.h declares it as f32 tanf(f32).
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];
static s32 init_event[29];
static s32 damage_event[10];
static s32 attack_event[19];
static s32 wait_event[6];

// clang-format off
extend_data(148,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(149,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_wanawana = {
    UNIT_UNUSED_WANAWANA,       // mUnitType
    "btl_un_wanawana",          // name
    2,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    1,                          // mLevel
    0,                          // mBonusExp
    1,                          // bonusCoinRolls
    0,                          // bonusCoinRate
    0,                          // baseCoin
    100,                        // field_0x13
    9,                          // field_0x14
    24,                         // width
    24,                         // height
    { 24, 24 },                 // statusIconOffset
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
    0,                          // swallowRate
    0,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BTL_DAMAGE1",          // damageSfxName
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

static BattleDispPose pose_table[4] = {
    { 0x1C, "S_1" },
    { 0x1F, "S_1" },
    { 0x27, "S_1" },
    { 0x45, "S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_wanawana",     // partName
        "nakayama_test4",      // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 40.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 40.0f, 0.0f }, // hitCursorBaseOffset
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

// TODO: write replace_evt (global, 0x26c bytes) here:
// its asm is in build/G8MJ01/tou2/asm/unit/unit_wanawana.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_yuka"), 4194304)
    CALL(replace_evt)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
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
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: write replace_callback (global, 0xb8 bytes) here:
// its asm is in build/G8MJ01/tou2/asm/unit/unit_wanawana.s; this is where the original has it, just before the first script that uses it

// TODO: write replace_disp (global, 0x35c bytes) here:
// its asm is in build/G8MJ01/tou2/asm/unit/unit_wanawana.s; this is where the original has it, just before the first script that uses it

// TODO: write tanf (static, 0x24 bytes) here:
// its asm is in build/G8MJ01/tou2/asm/unit/unit_wanawana.s; this is where the original has it, just before the first script that uses it
// clang-format on
