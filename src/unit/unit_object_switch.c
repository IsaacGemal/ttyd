#include "battle/battle.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "battle/ac/ac_table.h"
#include "battle/battle_ac.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "driver/mobjdrv.h"
#include "evt/evt_map.h"
#include "unit/unit_object_switch.h"

// Rumble strength table, indexed by event->userdata[0] (original's local .data:0x1CC _data).

const f64 double_to_int_802f9f50 = 4503601774854144.0;

static s32 _data[35];
static u32 parts[19];
static s32 init_event[25];
static s32 damage_event[22];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static u32 pose_table[8];
static u32 data_table[2];
static s32 attack_event[5];
static s32 wait_event[2];

// clang-format off
USER_FUNC(_rumble_switch) {
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

// TODO: type
u32 unit_object_switch[49] = {
    0x000000D9, (u32)"btl_un_switch", 0x03E70000, 0x01010200,
    0x00000064, 0x00090018, 0x00180000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41400000,
    0x00000000, 0x00000000, 0x41400000, 0x4179999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BTL_DAMAGE1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x12003001,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_switch", (u32)"b_mario", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x03000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
static s32 init_event[25] = {
    0x0003005B, (s32)btlevtcmd_SetEventWait, 0xFFFFFFFE, (s32)&wait_event,
    0x0003005B, (s32)btlevtcmd_SetEventAttack, 0xFFFFFFFE, (s32)&attack_event,
    0x0003005B, (s32)btlevtcmd_SetEventDamage, 0xFFFFFFFE, (s32)&damage_event,
    0x0003005B, (s32)btlevtcmd_SetEventConfusion, 0xFFFFFFFE, (s32)btldefaultevt_Confuse,
    0x0003005B, (s32)btlevtcmd_SetMaxMoveCount, 0xFFFFFFFE, 0x00000000,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002,
    0x00000001,
};

// TODO: type
static s32 damage_event[22] = {
    0x0004005B, (s32)btlevtcmd_CheckDamageCode, 0xFFFFFFFE, 0x00000100,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000000,
    0x0002005B, (s32)_rumble_switch, 0xFFFFFFFE, 0x00000002,
    0x00000021, 0x0004005B, (s32)evt_map_playanim, (s32)"switch_a",
    0x00000000, 0x00000000, 0x0001000A, 0x000001F4,
    0x00000002, 0x00000001,
};

// TODO: type
static s32 _data[35] = {
    -5, 0, 15, 0, -15, 0, 14, 0, -13, 0, 12, 0, -11, 0, 10, 0, -9, 0,
    8,  0, -7, 0, 6,   0, -6, 0, 5,   0, -4, 0, 2,   0, -2, 0, 0,
};

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

// TODO: type
static u32 pose_table[8] = {
    0x0000001C, (u32)"M_S_1", 0x0000001F, (u32)"M_S_1",
    0x00000027, (u32)"M_O_1", 0x00000045, (u32)"M_S_1",
};

// TODO: type
static u32 data_table[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static s32 attack_event[5] = {
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002,
    0x00000001,
};

// TODO: type
static s32 wait_event[2] = {
    0x00000002, 0x00000001,
};
// clang-format on
