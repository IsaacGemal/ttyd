#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>
BOOL christineGetStatus(void);
void kuribo_bye(PartyEntry* party);
void kuribo_init_800C23B4(PartyEntry* party);
void kuribo_use(PartyEntry* party);
void kuribo_use_post(PartyEntry* party);
