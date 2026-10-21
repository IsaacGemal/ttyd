// effect_stardust, shared by tik_11 and tik_12. Included where it sits in the unit's function order (it sets the .text
// layout).
#include "eff/eff_stardust.h"
#include "evt/evt_cmd.h"

static USER_FUNC(effect_stardust) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    f32 c = evtGetFloat(event, args[1]);
    f32 d = evtGetFloat(event, args[2]);
    f32 e = evtGetFloat(event, args[3]);
    f32 f = evtGetFloat(event, args[4]);
    f32 g = evtGetFloat(event, args[5]);
    s32 a = evtGetValue(event, args[6]);
    s32 b = evtGetValue(event, args[7]);

    effStardustEntry(type, a, b, c, d, e, f, g);
    return EVT_RETURN_DONE;
}
