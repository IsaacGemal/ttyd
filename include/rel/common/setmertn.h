// setmertn, shared by gor_03, tik_00, tik_11 and tik_12. Included where it sits in the unit's function order (it sets
// the .text layout).
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

static USER_FUNC(setmertn) {
    event->ownerNPC = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}
