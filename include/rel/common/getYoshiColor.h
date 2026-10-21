// getYoshiColor, shared by jin_04 and usu_00. Included where it sits in the unit's function order (it sets the .text
// layout).
#include "evt/evt_cmd.h"
#include "mario/mario_pouch.h"

static USER_FUNC(getYoshiColor) {
    s32* args = event->args;

    evtSetValue(event, args[0], pouchGetPartyColor(PARTNER_YOSHI));
    return EVT_RETURN_DONE;
}
