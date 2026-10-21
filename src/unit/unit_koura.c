#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

static BattleDataEntry data_table[7];
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts[2];
static s32 init_event[46];
static s32 phase_event[38];
s32 koura_open[72];
s32 koura_close[79];
s32 damage_core[92];
s32 pose_tbl_reset[58];
static s32 dead_event[243];
s32 destroy_event[47];
static s32 damage_event[10];
static s32 wait_event[6];
static s32 attack_event[2];

// clang-format off
BattleUnitKind unit_koura = {
    UNIT_SHELL_SHIELD,          // mUnitType
    "btl_un_koura",             // name
    8,                          // maxHp
    0,                          // maxFp
    5,                          // mDangerHP
    1,                          // mPerilHP
    1,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    0,                          // bonusCoinRate
    0,                          // baseCoin
    100,                        // escapeRate
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
    { 25.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 25.0f, 15.6f, 0.0f },     // kissHitOffset
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
    0x10000000,                 // attributeFlags
    &regist,                    // vulnStatus
    2,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleDataEntry data_table[7] = {
    { 0x3B, koura_open },
    { 0x3C, koura_close },
    { 0x09, damage_core },
    { 0x3D, pose_tbl_reset },
    { 0x3E, destroy_event },
    { 0x31, dead_event },
    { 0x00, NULL },
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

static BattleDispPose pose_table[5] = {
    { 0x1C, "PKO_S_1" },
    { 0x1F, "PKO_S_1" },
    { 0x27, "PKO_D_1" },
    { 0x44, "PKO_Z_1" },
    { 0x45, "PKO_S_1" },
};

BattleDispPose pose_table_crack_lv1[5] = {
    { 0x1C, "PKO_S_2" },
    { 0x1F, "PKO_S_2" },
    { 0x27, "PKO_D_2" },
    { 0x44, "PKO_Z_2" },
    { 0x45, "PKO_S_2" },
};

BattleDispPose pose_table_crack_lv2[5] = {
    { 0x1C, "PKO_S_3" },
    { 0x1F, "PKO_S_3" },
    { 0x27, "PKO_D_3" },
    { 0x44, "PKO_Z_3" },
    { 0x45, "PKO_S_3" },
};

static BattleUnitKindPart parts[2] = {
    {
        1,                      // partNum
        "btl_un_koura",         // partName
        "c_koura",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 20.0f, 40.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 40.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x00080009,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
    {
        2,                      // partNum
        "btl_un_koura",         // partName
        "c_koura",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x03010000,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetBaseScale, -2, FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.5f))
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_GREATER(LW(0), 0)
            CALL_CHILD(koura_open)
        END_IF
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_GREATER(LW(0), 0)
            CALL_CHILD(koura_close)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(koura_open)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_PhaseEventStartDeclare, -2)
    CALL(btlevtcmd_SetRotateOffset, -2, -23, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    LOOP(30)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, 2)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_OffAttribute, LW(3), 64)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 65536)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 68)
    RETURN
EVT_END()

EVT_BEGIN(koura_close)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_PhaseEventStartDeclare, -2)
    CALL(btlevtcmd_SetRotateOffset, -2, -23, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 60)
    LOOP(30)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, -2)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OnAttribute, LW(3), 64)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 65536)
    CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(damage_core)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(1))
    CALL(btlevtcmd_GetHp, -2, LW(0))
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(3)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv2))
        IF_INT_NOT_EQUAL(LW(1), 2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 3, 2)
        CASE_LESS_OR_EQUAL(6)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv1))
        IF_INT_NOT_EQUAL(LW(1), 1)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    WAIT_MS(500)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(pose_tbl_reset)
    CALL(btlevtcmd_GetHp, -2, LW(0))
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(3)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv2))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetUnitWork, -2, 3, 2)
        CASE_LESS_OR_EQUAL(6)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_crack_lv1))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OffAttribute, LW(3), 96)
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_GUARD4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKO_H_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("PKO_H_2"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, 1, 23, 0, 0)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, 2, -23, 0, 0)
    BEGIN_SIBLING_EVENT
        LOOP(5)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(2)
        END_LOOP
        LOOP(6)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(2)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(2)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(10)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, -1, 0, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 1, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(5)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 1, -1, 0, 0)
            CALL(btlevtcmd_AddPartsDispOffset, -2, 2, 1, 0, 0)
            WAIT_FRAMES(2)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(10)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 1, 0, 1, 0)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 2, 0, 1, 0)
            WAIT_FRAMES(2)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(14)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 1, -1, 0, 0)
            CALL(btlevtcmd_AddPartsRotateOffset, -2, 2, 1, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    LOOP(20)
        CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 0, 2)
        CALL(btlevtcmd_AddPartsRotate, -2, 2, 0, 0, -2)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(8)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        WAIT_FRAMES(2)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        WAIT_FRAMES(2)
    END_LOOP
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

EVT_BEGIN(destroy_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_OffAttribute, LW(3), 96)
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

// TODO: type
static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()
// clang-format on
