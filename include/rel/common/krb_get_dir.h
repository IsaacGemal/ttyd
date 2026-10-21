// krb_get_dir, shared by the Goomba units: unit_kuriboo and unit_patakuri (both versions), gon's unit_togekuri, and
// the hyper and yami Goomba and Paragoomba and unit_kurikuri. Included where it sits in the unit's function order (it
// sets the .text layout). The other spiky Goomba copies keep their own: they define krb_comp_dir and get_attacker
// before it, which can't come from here (an included function links after the unit's own).
#include "battle/battle_sub.h"
#include "evt/evt_cmd.h"

static USER_FUNC(krb_get_dir) {
    s32* args = event->args;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
    s32 out;
    f32 dx;
    f32 dy;

    x0 = evtGetFloat(event, args[0]);
    y0 = evtGetFloat(event, args[1]);
    x1 = evtGetFloat(event, args[2]);
    y1 = evtGetFloat(event, args[3]);
    out = evtGetValue(event, args[4]);
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx == 0.0f && dy == 0.0f) {
        evtSetValue(event, args[4], out);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, args[4], (s32)angleABf(dx, dy, 0.0f, 0.0f));
    return EVT_RETURN_DONE;
}
