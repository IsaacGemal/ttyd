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
void walkMain(PartyEntry* party);
f32 unk_800c92c4(PartyEntry* party);
void N_partyPlaySfxPartyLanding1(PartyEntry* party);
s32 unk_800c8dd0(HitEntry* hit);
void partyChkWall(PartyEntry* party);
void unk_800c9044(PartyEntry* party);
void partyMove(PartyEntry* party);
void unk_800c9398(PartyEntry* party, f32 value);
s32 partyMoveMain(PartyEntry* party, s32 hosei);
void unk_800c90d4(PartyEntry* party, s32 value);
void partySetDir(PartyEntry* party, f32 x, f32 z);
void partyGetMoveDirSpd(PartyEntry* party, f32* angle, f32* speed);
BOOL partyGetAppearPos3(PartyEntry* party, Vec* position);
void partyGetAppearPos4(PartyEntry* party, Vec* position);
BOOL partyGetAppearPos5(PartyEntry* party, HitEntry* hit, Vec* position);
void partyGetAppearPos2(PartyEntry* party, Vec* pos);
void partyMoveFlyInit(PartyEntry* party, s32 mode);
void partyFlyMain(PartyEntry* party);
void party_move_mode_beside_mario(PartyEntry* party);
void party_move_mode_behind_mario(PartyEntry* party);
void party_move_mode_stay(PartyEntry* party);
void party_move_mode_walk(PartyEntry* party);
void partyRecordFootmark(PartyEntry* party);
void partyMoveNoHosei(PartyEntry* party);
void partyGetAppearPos(PartyEntry* party, Vec* position);
void partyChgMoveMode(PartyEntry* party, u8 mode);
void unk_800c8f24(PartyEntry* party, f32 f1);
void partyMoveCheckWall(PartyEntry* party);
void unk_800c93d0(PartyEntry* party, f32 value);
void unk_800c8e74(PartyEntry* party);
f32 partyGetTargetDist3D(s32 id, Vec* pos);
f32 partyGetTargetDist(s32 id, f32 x, f32 z);
f32 partyGetTargetDistY(s32 id);
f32 partyGetTargetDir(s32 id, f32 x, f32 z);
s32 partyGetFootmarkWriteId(u32 index);
s32 partyGetFootmarkReadId(u32 index);
void partyGetReadId(u32 index, f32* x, f32* y, f32* z);
struct Footmark;
struct Footmark* partyGetFootmarkPtr(u32 index);
f32 unk_800c9f48(s32 id, f32 y);
f32 partyGetTargetMovespd2(PartyEntry* party);
void partyMoveBehindMario3(PartyEntry* party, s32 mode, f32 dir);
