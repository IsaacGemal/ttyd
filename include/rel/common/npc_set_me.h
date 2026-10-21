// npc_set_me and npc_dead, shared by hei_08 and hei_12. Included where they sit in the unit's function order (it sets
// the .text layout).
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"

static USER_FUNC(npc_set_me) {
    event->ownerNPC = npcNameToPtr("チョロボン");
    return EVT_RETURN_DONE;
}

static USER_FUNC(npc_dead) {
    npcGroupDead(npcNameToPtr("チョロボン"), 0);
    return EVT_RETURN_DONE;
}
