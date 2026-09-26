#pragma once

#include <dolphin/types.h>
#include "mario/mario_pouch.h"

void partyJoin(PartyId partnerId);


s32 marioGetPartyId(void);
s32 marioGetExtraPartyId(void);

s32 marioPartyEntry(s32 partyId);
void marioPartyKill(void);
