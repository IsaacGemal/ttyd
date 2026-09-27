#pragma once

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>
#include "party.h"
struct PartyEntry;
void partyChgMot(PartyEntry* party, s32 mot);
u32 partyGetFootmarkId(u32* flags);
void partyNextReadId(u32 index);
void partyClearFootmark2(void);
void partyMoveBesideMario(PartyEntry* party, s32 unk);
void partyMoveBehindMario(PartyEntry* party, s32 unk);
void partyClearFootmark(void);
void partyWalkInit(PartyEntry* party, s32 mode);
void partyWalkMain(PartyEntry* party);
f32 unk_800c92c4(PartyEntry* party);
void N_partyPlaySfxPartyLanding1(PartyEntry* party);
s32 unk_800c8dd0(HitEntry* hit);
void partyChkWall(PartyEntry* party);
void unk_800c9044(PartyEntry* party);
void partyMove(PartyEntry* party);
void unk_800c9398(PartyEntry* party, f32 value);
void partyMoveMain(PartyEntry* party, s32 hosei);
void unk_800c90d4(PartyEntry* party, s32 value);
void partySetDir(PartyEntry* party, f32 x, f32 z);
void partyGetMoveDirSpd(PartyEntry* party, f32* angle, f32* speed);
BOOL partyGetAppearPos3(PartyEntry* party, Vec* position);
void partyGetAppearPos4(PartyEntry* party, Vec* position);
void partyGetAppearPos5(PartyEntry* party, HitEntry* hit, Vec* position);
void partyGetAppearPos2(PartyEntry* party, Vec* pos);
