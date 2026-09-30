#include "battle/battle.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "unit/unit_object_tree.h"
#include "battle/battle_disp.h"

const f64 double_to_int_802f9ec8 = 4503601774854144.0;
const f32 float_0_8041e790 = 0.0f;

static s32 _data[35];
static BattleUnitKindPart parts[1];
static s32 init_event[25];
static s32 damage_event[14];
static BattleUnitDefense defence;
static BattleUnitDefenseAttr defence_attr;
static BattleVulnerableStatus regist;
static BattleDispPose pose_table[4];
static BattleDataEntry data_table[1];
static s32 attack_event[5];
static s32 wait_event[2];

// clang-format off
USER_FUNC(_rumble_tree) {
    s32* args = event->args;
    BattleUnit* unit;
    BattleStageObject* obj;
    s32 value;

    value = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, value));
    if (unit == NULL) {
        return EVT_RETURN_DONE;
    }
    obj = BattleGetObjectPtr(*(s32*)((u8*)unit + 0xB2C));
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    obj->mPosition.y += (f32)_data[event->userdata[0]];
    event->userdata[0]++;
    if ((u32)event->userdata[0] >= 0x22) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(_turn_tree) {
    BattleUnit* unit;
    BattleStageObject* obj;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    obj = BattleGetObjectPtr(*(s32*)&unit->field_0xAE4[0x48]); // 0xB2C: stage-object id, in the header's padding
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    event->userdata[0] += 3;
    if (event->userdata[0] > 0x5A) {
        obj->mRotation.x = float_0_8041e790;
        return EVT_RETURN_DONE;
    }
    obj->mRotation.x = (f32)event->userdata[0];
    return EVT_RETURN_BLOCK;
}


BattleUnitKind unit_object_tree = {
    UNIT_UNUSED_TREE,           // mUnitType
    "btl_un_tree",              // name
    999,                        // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    2,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    0,                          // bonusCoinRate
    0,                          // baseCoin
    100,                        // field_0x13
    9,                          // field_0x14
    24,                         // width
    24,                         // height
    { 0, 0 },                   // statusIconOffset
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
    0x12003001,                 // attributeFlags
    &regist,                     // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_tree",         // partName
        "b_mario",             // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x03000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(_rumble_tree, -2)
    CALL(_turn_tree, -2)
    RETURN
EVT_END()

// TODO: type
static s32 _data[35] = {
    0xFFFFFFFB, 0x00000000, 0x0000000F, 0x00000000,
    0xFFFFFFF1, 0x00000000, 0x0000000E, 0x00000000,
    0xFFFFFFF3, 0x00000000, 0x0000000C, 0x00000000,
    0xFFFFFFF5, 0x00000000, 0x0000000A, 0x00000000,
    0xFFFFFFF7, 0x00000000, 0x00000008, 0x00000000,
    0xFFFFFFF9, 0x00000000, 0x00000006, 0x00000000,
    0xFFFFFFFA, 0x00000000, 0x00000005, 0x00000000,
    0xFFFFFFFC, 0x00000000, 0x00000002, 0x00000000,
    0xFFFFFFFE, 0x00000000, 0x00000000,
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

static BattleDispPose pose_table[4] = {
    { 0x1C, "M_S_1" },
    { 0x1F, "M_S_1" },
    { 0x27, "M_O_1" },
    { 0x45, "M_S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    RETURN
EVT_END()
// clang-format on
