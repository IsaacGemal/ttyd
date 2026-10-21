// killer_get_angle, shared by unit_killer_cannon (dou) and unit_super_killer_cannon (las). Included where it sits in the
// unit's function order (it sets the .text layout).
#include "evt/evt_cmd.h"
#include "math_ext.h"

static USER_FUNC(killer_get_angle) {
    s32* args = event->args;
    f32 x1 = evtGetFloat(event, args[0]);
    f32 y1 = evtGetFloat(event, args[1]);
    f32 x2 = evtGetFloat(event, args[2]);
    f32 y2 = evtGetFloat(event, args[3]);
    f32 angle = atan((y2 - y1) / (x2 - x1));
    evtSetFloat(event, args[4], 360.0f * angle / 6.283184f);
    return EVT_RETURN_DONE;
}
