// yoshi_exist, shared by the two Iron Cleft units of tou2 (unit_iron_sinemon and unit_iron_sinemon2). Included where it
// sits in the unit's function order (it sets the .text layout).
#include "evt/evt_cmd.h"
#include "mario/mario_pouch.h"

static USER_FUNC(yoshi_exist) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    s32 exists = 0;

    if (pouch->partyData[PARTNER_YOSHI].flags & 1) {
        exists = 1;
    }
    evtSetValue(event, args[0], exists);
    return EVT_RETURN_DONE;
}
