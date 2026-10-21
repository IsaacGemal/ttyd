// x_flag_set, x_flag_add, x_flag_get and x_flag_wait, shared by the lecture units (see rel/common/evt_lect.h).
// Included where they sit in the unit's function order (it sets the .text layout); the unit defines
// `s32 x_flag_data[]` before it. unit_boss_macho_gance's copies come after its own functions, so it keeps them.
#include "evt/evt_cmd.h"

static USER_FUNC(x_flag_set) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[idx] = value;
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_add) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    x_flag_data[idx] += value;
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_get) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], x_flag_data[idx]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(x_flag_wait) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    return (x_flag_data[idx] == value) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
