// vivian_comeback, shared by unit_faker_mario (jin), unit_faker_mario_gra and unit_boss_rampell (jin). Included where
// it sits in the unit's function order (it sets the .text layout).
#include "evt/evt_cmd.h"
#include "mario/mario_pouch.h"

static USER_FUNC(vivian_comeback) {
    PouchData* pouch = pouchGetPtr();

    pouch->partyData[PARTNER_VIVIAN].flags &= ~2;
    return EVT_RETURN_YIELD;
}
