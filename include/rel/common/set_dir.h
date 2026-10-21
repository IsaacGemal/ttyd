// set_dir, as the partner-lecture units (rel/common/evt_lect.h) and hei_01 have it. Included where it sits in the
// unit's function order (it sets the .text layout).
#include "evt/evt_cmd.h"
#include "party.h"

static USER_FUNC(set_dir) {
    s32* args = event->args;
    s32 dir = evtGetValue(event, args[0]);
    PartyEntry* party = partyGetPtr(0);

    if (party != NULL) {
        s32 angle = (dir > 180) ? 270 : 90;
        f32 value = (f32)(270 - angle);

        party->dispDir = value;
        party->field_110 = value;
    }
    return EVT_RETURN_DONE;
}
