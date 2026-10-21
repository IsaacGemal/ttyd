// _space_rotate, shared by the battle databases of aji and moo. Included where it sits in the unit's function order (it
// sets the .text layout).
#include "battle/battle_stage_object.h"
#include "evt/evt_cmd.h"

static USER_FUNC(_space_rotate) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 index = args[1];
    s32 start = evtGetValue(event, index);
    s32 end = evtGetValue(event, args[2]);
    BattleStageObject* obj = BattleSearchObjectPtr(name);
    s32 next;

    obj->mRotation.z = 360.0f * (f32)start / (f32)end;
    next = start + 1;
    if (next >= end) {
        next = 0;
    }
    evtSetValue(event, index, next);
    return EVT_RETURN_DONE;
}
