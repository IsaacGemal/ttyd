#pragma once

#include <dolphin/types.h>
#include "driver/hitdrv.h"
#include "party.h"
struct HitEntry;
struct PartyEntry;
HitEntry* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 dist, f32 dir);
HitEntry* partySearchGround(PartyEntry* party, f32 up, f32 down);
