#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
static u32 regist[6];
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];
static s32 init_event[9];
static s32 wait_event[6];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_act_kinopio[49] = {
    0x000000B9, (u32)"btl_un_act_kinopio", 0x000A0000, 0x01010100,
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

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

static BattleDispPose pose_table[7] = {
    { 0x0000001C, "KNP_S_1" },
    { 0x0000001F, "KNP_S_1" },
    { 0x00000027, "KNP_D_1" },
    { 0x0000002A, "KNP_R_1" },
    { 0x00000028, "KNP_W_1" },
    { 0x00000041, "KNP_T_1" },
    { 0x00000045, "KNP_S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0, 0 },
};

static BattleUnitKindPart parts[1] = {
    {
        0x00000001,
        "btl_un_teresa",
        "c_kinopio",
        { 0.0f, 0.0f, 0.0f },
        { 15.0f, 25.0f, 0.0f },
        { 0x00, 0x00, 0x00, 0x00, 0x41, 0xF0, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x1E,
          0x00, 0xFF, 0x00, 0x00 },
        (BattleUnitDefense*)&defence,
        (BattleUnitDefenseAttr*)&defence_attr,
        0x00000009,
        0x00000000,
        pose_table,
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
