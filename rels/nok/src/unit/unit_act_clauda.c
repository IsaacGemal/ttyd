#include "battle/battle_event_cmd.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 pose_table_clauda_stay[];
static u32 regist[6];
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];
static s32 init_event[9];
static s32 wait_event[6];

// clang-format off
extend_data(16,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(17,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
// BattleUnitKind: mUnitType (0xB6 = UNIT_EPILOGUE_FLURRIE), the four SFX names and the
// mParts/init_event/dataTable pointers line up, but the header leaves 0x4, 0x10..0xB8 and 0xBC
// as unnamed byte arrays, and the original data there is non-zero (name string, floats, &regist), so
// it cannot be written as a struct initializer without naming those fields. Left raw.
u32 unit_act_clauda[49] = {
    0x000000B6, (u32)"btl_un_act_clauda", 0x000A0000, 0x01010100,
    0x00000064, 0x0009001C, 0x00180005, 0x00190000,
    0x00000000, 0x41600000, 0x00000000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x41600000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0x00000000, 0x41600000, 0x4179999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BTL_DAMAGE1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

static BattleUnitDefense defence = { 0 };

static BattleUnitDefenseAttr defence_attr = { 0 };

// Pointer stored at BattleUnitKind +0xB0. 24 bytes, no matching struct in include/.
static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static BattleDataEntry data_table[1] = {
    { 0, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                    // partNum
        "btl_un_clauda",      // partName
        "c_windy",            // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
        // field_0x24: unnamed in the header; bytes of 0, 30.0f, 0, 0x0014001E, 0x00FF0000
        { 0x00, 0x00, 0x00, 0x00,
          0x41, 0xF0, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x14, 0x00, 0x1E,
          0x00, 0xFF, 0x00, 0x00 },
        &defence,                                    // defense
        &defence_attr,                               // defenseAttr
        kMainPart | kUnknown_0x8,                    // attributes
        0,                                           // counterAttributes
        pose_table_clauda_stay,                      // poseTable
    },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
