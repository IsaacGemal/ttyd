// lect_mobj_hit_nasi. A lecture helper: see rel/common/evt_lect.h.
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_cmd.h"

#include <string.h>

static USER_FUNC(lect_mobj_hit_nasi) {
    s32* args = event->args;
    MapObjectEntry* obj;
    const char* hit;
    const char* name;
    s32 i;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    for (i = 0; i < 2; i++) {
        hit = obj->hitObj[i].name;
        if (hit != NULL && strcmp(hit, "") != 0) {
            hitGrpFlagOn(hit, 1);
        }
    }
    return EVT_RETURN_DONE;
}
