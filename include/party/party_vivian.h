#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>

s32 vivianGetStatus(void);
void L_partyVivianTailStart(PartyEntry* party);
void L_partyVivianTailStop(PartyEntry* party);
void vivianUnhold(PartyEntry* party);
void vivianShadow(PartyEntry* party);
void vivian_move(PartyEntry* party);
void vivian_use(PartyEntry* party);
s32 N_vivianDisplayTailCheck(PartyEntry* party);
void vivian_init_801809C0(PartyEntry* party);
void mot_vivian_post(void);
void mot_vivian(void);
void N_marioForceVivianAnime(void);
