// _wave_offset, shared by the battle databases of dou, muj and tik. Included where it sits in the unit's function order
// (it sets the .text layout).
#include "battle/battle_stage_object.h"
#include "evt/evt_cmd.h"
#include "math_ext.h"

static USER_FUNC(_wave_offset) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 index1 = args[1];
    s32 arg1 = evtGetValue(event, index1);
    s32 arg2 = evtGetValue(event, args[2]);
    s32 arg3 = evtGetValue(event, args[3]);
    s32 index4 = args[4];
    s32 arg4 = evtGetValue(event, index4);
    s32 arg5 = evtGetValue(event, args[5]);
    s32 arg6 = evtGetValue(event, args[6]);
    BattleStageObject* obj = BattleSearchObjectPtr((char*)name);
    s32 next1;
    s32 next4;

    obj->mPositionOffset1.x = (f32)arg3 * sinfd((f32)(arg1 * 360 / arg2));
    obj->mPositionOffset1.y = (f32)arg6 + sinfd((f32)(arg4 * 360 / arg5 + 270)) * (f32)arg6;

    next1 = arg1 + 1;
    if (next1 >= arg2) {
        next1 = 0;
    }
    evtSetValue(event, index1, next1);

    next4 = arg4 + 1;
    if (next4 >= arg5) {
        next4 = 0;
    }
    evtSetValue(event, index4, next4);

    return EVT_RETURN_DONE;
}
