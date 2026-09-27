#pragma once

#include <dolphin/types.h>
#include "mario/mario_pouch.h"

void partyJoin(PartyId partnerId);


s32 marioGetPartyId(void);
s32 marioGetExtraPartyId(void);

s32 marioPartyEntry(s32 partyId);
void marioPartyKill(void);
s32 marioGetParty(void);
s32 partyChkJoin(s32 memberId);
void partyLeft(PartyId partnerId);
s32 partyGetHp(s32 partyId);
void unk_8013be14(void);
s32 partyGetTechLv(s32 partyId);
