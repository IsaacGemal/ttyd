#pragma once

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>
#include "party.h"
struct PartyEntry;
void partyChgMot(PartyEntry* party, s32 mot);
s32 partyGetFootmarkId(u32* flags);
void partyNextReadId(s32 index);
void partyClearFootmark2(void);
void partyMoveBesideMario(PartyEntry* party, s32 unk);
void partyMoveBehindMario(PartyEntry* party, s32 unk);
void partyClearFootmark(void);
void partyWalkInit(PartyEntry* party, s32 mode);
void partyWalkMain(PartyEntry* party);
void walkMain(PartyEntry* party);
f32 partyStepSpeed(PartyEntry* party);
void N_partyPlaySfxPartyLanding1(PartyEntry* party);
s32 partyHitIsBoxMobj(HitEntry* hit);
void partyChkWall(PartyEntry* party);
s32 partyMarioHosei(PartyEntry* party);
void partyChgIdlePose(PartyEntry* party);
void partyMove(PartyEntry* party);
f32 partySetSpeed(PartyEntry* party, f32 value);
s32 partyMoveMain(PartyEntry* party, s32 hosei);
void partyStepSfx(PartyEntry* party, s32 value);
void partySetDir(PartyEntry* party, f32 x, f32 z);
void partyGetMoveDirSpd(PartyEntry* party, f32* angle, f32* speed);
BOOL partyGetAppearPos3(PartyEntry* party, Vec* position);
BOOL partyGetAppearPos4(PartyEntry* party, Vec* position);
BOOL partyGetAppearPos5(PartyEntry* party, HitEntry* hit, Vec* position);
BOOL partyGetAppearPos2(PartyEntry* party, Vec* pos);
void partyMoveFlyInit(PartyEntry* party, s32 mode);
void partyFlyMain(PartyEntry* party);
void party_move_mode_beside_mario(PartyEntry* party);
void party_move_mode_behind_mario(PartyEntry* party);
void party_move_mode_stay(PartyEntry* party);
void party_move_mode_walk(PartyEntry* party);
void partyRecordFootmark(PartyEntry* party);
void partyMoveNoHosei(PartyEntry* party);
BOOL partyGetAppearPos(PartyEntry* party, Vec* position);
void partyChgMoveMode(PartyEntry* party, u8 mode);
void partyChgSpeedPose(PartyEntry* party, f32 f1);
void partyMoveCheckWall(PartyEntry* party);
f32 partySetTargetSpeed(PartyEntry* party, f32 value);
void partySetDispDirToMario(PartyEntry* party);
f32 partyGetTargetDist3D(s32 id, Vec* pos);
f32 partyGetTargetDist(s32 id, f32 x, f32 z);
f32 partyGetTargetDistY(s32 id);
f32 partyGetTargetDir(s32 id, f32 x, f32 z);
s32 partyGetFootmarkWriteId(s32 index);
s32 partyGetFootmarkReadId(s32 index);
void partyGetReadId(s32 index, f32* x, f32* y, f32* z);
void partyGetReadId2(s32 index, Vec* out);
void L_getTargetPos(s32 id, Vec* out);
// Layout of a footmark record (0x10 bytes) inside footmarkBuf.
typedef struct Footmark {
    u8 field_0; // 0x0, flags (party_motion tests 0x1 and 0x4)
    u8 pad[3];  // 0x1
    Vec pos;    // 0x4, copied whole from the mario/party position (lwz/stw struct copy)
} Footmark;
Footmark* partyGetFootmarkPtr(s32 index);
f32 partyGetTargetDistYFrom(s32 id, f32 y);
f32 partyGetTargetMovespd2(PartyEntry* party);
void partyMoveBehindMario3(PartyEntry* party, s32 mode, f32 dir);
void partyMoveWalk(PartyEntry* party);
void partyClearJumpPara(PartyEntry* party);
void partyForceWalk(PartyEntry* party);
void partyIdleMain(PartyEntry* party);
