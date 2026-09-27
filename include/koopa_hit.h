#pragma once

#include <dolphin/mtx.h>

#include <dolphin/types.h>
void* kpaHitCheck(void);
void* kpaFlameHitCheck(f32 x, f32 y, f32 z, f32 rad);
void kpaSearchFrontUnder(void);
void kpaClearHitobjRide(void);
s32 kpaChkHitobjRide(void);
void* kpaSearchGround(f32* a, f32* b, f32* c, f32 d);
void* kpaSearchHead(Vec* pos, f32* outY, f32 a);
