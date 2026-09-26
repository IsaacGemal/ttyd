#pragma once

#include <dolphin/types.h>

struct HitEntry;

BOOL marioChkBero(struct HitEntry* hit);
struct HitEntry* marioSearchGround(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
