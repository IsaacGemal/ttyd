#include "battle/battle_event_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle_unit.h"

#pragma force_active on
static s32 init_event[21];
static BattleUnitDefense defence;
static BattleUnitDefenseAttr defence_attr;
static u32 regist[6];
static s32 damage_event[2];
static s32 attack_event[2];
static s32 wait_event[2];

// clang-format off
static BattleUnitKindPart parts[1] = {
    {
        1,                    // partNum
        "btl_un_system",      // partName
        NULL,                 // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // shadowSize
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x03010009,           // attributes
        0,                    // counterAttributes
        NULL,                 // poseTable
    },
};

BattleUnitKind unit_system = {
    UNIT_SYSTEM,                                                // mUnitType
    "btl_un_system",                                            // name
    999,                                                        // maxHp
    0,                                                          // maxFp
    1,                                                          // mDangerHP
    1,                                                          // mPerilHP
    0,                                                          // mLevel
    0,                                                          // mBonusExp
    0,                                                          // bonusCoinRolls
    0,                                                          // bonusCoinRate
    0,                                                          // baseCoin
    0,                                                          // escapeRate
    9,                                                          // renzokuSoftCap
    24,                                                         // width
    24,                                                         // height
    { 0, 20 },                                                  // statusIconOffset
    { 0.0f, 0.0f, 0.0f },                                       // centerOffset
    { 0, 0 },                                                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },                                       // talkTogeBaseOffset
    { 0.0f, 0.0f, 0.0f },                                       // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },                                       // burnFlameOffset
    0.0f,                                                       // kagenukeOffsetX
    0.0f,                                                       // kagenukeOffsetZ
    { 12.0f, 0.0f, 0.0f },                                      // bintaHitOffset
    { 12.0f, 15.6f, 0.0f },                                     // kissHitOffset
    { 0.0f, 0.0f, 0.0f },                                       // cutBaseOffset
    0.0f,                                                       // cutWidth
    0.0f,                                                       // cutHeight
    0,                                                          // turnOrder
    0,                                                          // turnOrderVariance
    -1,                                                         // swallowRate
    0,                                                          // swallowAttribute
    100,                                                        // shotDamageRate
    20,                                                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },                                       // starPointDispOffset
    "SFX_BTL_DAMAGE1",                                          // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",                                     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",                                      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",                                 // electricDamageSfxName
    0x52003000,                                                 // attributeFlags
    (struct BattleVulnerableStatus*)(void*)((u8*)regist + 0x8), // vulnStatus
    1,                                                          // numParts
    parts,                                                      // mParts
    init_event,                                                 // initEvtScript
    (struct BattleDataEntry*)regist,                            // dataTable
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 2, 2, 2, 2, 2 } };

// TODO: type
static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static EVT_BEGIN(damage_event)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
