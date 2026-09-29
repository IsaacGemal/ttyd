#include "battle/battle.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

// Rumble strength table, indexed by event->userdata[0] (original's local .data:0x1CC _data).
static s32 _data[35] = {
    -5, 0, 15, 0, -15, 0, 14, 0, -13, 0, 12, 0, -11, 0, 10, 0, -9, 0,
    8,  0, -7, 0, 6,   0, -6, 0, 5,   0, -4, 0, 2,   0, -2, 0, 0,
};

const f64 double_to_int_802f9f50 = 4503601774854144.0;

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
