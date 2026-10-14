#pragma once

#include <dolphin/types.h>
#include "driver/hitdrv.h"
#include "party.h"
struct HitEntry;
struct PartyEntry;
HitEntry* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 dist, f32 dir);
HitEntry* partySearchGround(PartyEntry* party, f32 up, f32 down);
HitEntry* partyHitCheck(PartyEntry* party, Vec* pos, Vec* dir, Vec* hitPos, Vec* hitNormal, f32* dist);
void partyChkGnd(PartyEntry* party);
HitEntry* partySearchHead(PartyEntry* party, Vec* position, f32* out, f32 angle);
u32 partyChkFrontStep(PartyEntry* party, f32 a, f32* out1, f32* out2, f32* out3);
HitEntry* partySearchWallFront(PartyEntry* party, Vec* pos, f32 a, f32 b);
f32 partyNrmToAngle(Vec v);
BOOL chkfilter(HitCheckQuery* query, HitEntry* entry);
BOOL chkfilterNokotaro(HitCheckQuery* query, HitEntry* entry);
BOOL chkfilterYoshi(HitCheckQuery* query, HitEntry* entry);
