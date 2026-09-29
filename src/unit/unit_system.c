#include "battle/battle_event_cmd.h"
#include "evt/evt_cmd.h"

#pragma force_active on
static s32 init_event[21];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static s32 damage_event[2];
static s32 attack_event[2];
static s32 wait_event[2];

// clang-format off
// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_system", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x03010009, 0x00000000, 0x00000000,
};

// TODO: type
u32 unit_system[49] = {
    0x000000DC, (u32)"btl_un_system", 0x03E70000, 0x01010000,
    0x00000000, 0x00090018, 0x00180000, 0x00140000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41400000,
    0x00000000, 0x00000000, 0x41400000, 0x4179999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BTL_DAMAGE1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x52003000,
    (u32)((u8*)&regist + 0x8), 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&regist,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x02020202, 0x02000000,
};

// TODO: type
static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 data_table[2] = {
    0x00000000, 0x00000000,
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
