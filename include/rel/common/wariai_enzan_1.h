// wariai_enzan_1 and wariai_enzan_1f, shared by tik_11 and unit_boss_macho_gance (tou2). Included where they sit in
// the unit's function order (it sets the .text layout).
#include "evt/evt_cmd.h"

// Interpolates between start and end; event args are (remaining, steps, start, end, out). `remaining` is the
// script's LOOP counter, which counts down, so the result moves from start to end.
static USER_FUNC(wariai_enzan_1) {
    s32* args = event->args;
    f32 remaining = evtGetFloat(event, args[0]) - 1.0f;
    f32 steps = evtGetFloat(event, args[1]);
    f32 start = evtGetFloat(event, args[2]);
    f32 end = evtGetFloat(event, args[3]);

    evtSetValue(event, args[4], (s32)((remaining * start + end * (steps - remaining)) / steps));
    return EVT_RETURN_DONE;
}

static USER_FUNC(wariai_enzan_1f) {
    s32* args = event->args;
    f32 remaining = evtGetFloat(event, args[0]) - 1.0f;
    f32 steps = evtGetFloat(event, args[1]);
    f32 start = evtGetFloat(event, args[2]);
    f32 end = evtGetFloat(event, args[3]);

    evtSetFloat(event, args[4], (remaining * start + end * (steps - remaining)) / steps);
    return EVT_RETURN_DONE;
}
