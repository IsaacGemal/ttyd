#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>
#include <dolphin/mtx.h>

// This unit's .rodata zero vectors (defined in party_chuchu.c).
extern const Vec vec3_802f8288;
extern const Vec vec3_802f839c;

BOOL chuchu_searchObject(void);
void chuchu_exit(PartyEntry* party);
void chuchu_init(PartyEntry* party);
void chuchu_move(PartyEntry* party);
void chuchu_use(PartyEntry* party);
void chuchu_reinit(PartyEntry* party);
