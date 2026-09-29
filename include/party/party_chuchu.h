#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>
BOOL chuchu_searchObject(void);
void chuchu_exit(PartyEntry* party);
void chuchu_init(PartyEntry* party);
void chuchu_move(PartyEntry* party);
void chuchu_use(PartyEntry* party);
