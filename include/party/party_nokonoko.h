#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>
s32 nokonoko_holdItem(void);
s32 nokonokoGetStatus(PartyEntry* party);
void nokotaro_hold_cancel(PartyEntry* party);
void unk_800cd9a0(void);
void nokonoko_finish(PartyEntry* party);
void nokonoko_bye(PartyEntry* party);
void nokonoko_init(PartyEntry* party);
void nokonoko_move(PartyEntry* party);
void nokonoko_use(PartyEntry* party);
void nokonoko_use_post(PartyEntry* party);
