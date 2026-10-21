// make_name, shared by las_02, las_06, las_27 and las_30. Included where it sits in the unit's function order (it sets
// the .text layout); the unit defines `char name[8]` before it.
#include "evt/evt_cmd.h"
#include <stdio.h>

static USER_FUNC(make_name) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    sprintf(name, "hi%d", value);
    evtSetValue(event, args[1], (s32)name);
    return EVT_RETURN_DONE;
}
