#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>

s32 vivianGetStatus(void);
void L_partyVivianTailStart(PartyEntry* party);
void L_partyVivianTailStop(PartyEntry* party);
void vivianUnhold(PartyEntry* party);
void vivianShadow(PartyEntry* party);
