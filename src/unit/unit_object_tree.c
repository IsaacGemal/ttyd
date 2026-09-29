#include "battle/battle.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"

const f64 double_to_int_802f9ec8 = 4503601774854144.0;
const f32 float_0_8041e790 = 0.0f;

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
