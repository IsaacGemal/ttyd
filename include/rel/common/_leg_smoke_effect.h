// _leg_smoke_effect, shared by unit_boss_zonbaba (jon) and unit_boss_bunbaba (las). Included where it sits in the
// unit's function order (it sets the .text layout).
#include "eff/n64/eff_kemuri9_n64.h"
#include "evt/evt_cmd.h"

static USER_FUNC(_leg_smoke_effect) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    effKemuri9N64Entry(1, 18, 90, x, y, z, 60.0f, 0.0f);
    return EVT_RETURN_DONE;
}
