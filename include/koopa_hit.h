#pragma once

#include <dolphin/mtx.h>

#include <dolphin/types.h>
void* kpaHitCheck(void);
void* kpaFlameHitCheck(f32 x, f32 y, f32 z, f32 rad);
void* kpaSearchFrontUnder(void);
void kpaClearHitobjRide(void);
void* kpaChkHitobjRide(void);
void* kpaSearchGround(f32* a, f32* b, f32* c, f32 d);
void* kpaSearchHead(Vec* pos, f32* outY, f32 a);
#include "driver/hitdrv.h"
BOOL kpaFlameHitFilter(HitCheckQuery* query, HitEntry* entry);
void* kpaGetHitFlame(void);
void* kpaGetHitobj(void);
void* kpaGetHitobjRide(s32 index);
void* kpaGetHitobjHead2(void);
void* kpaGetHitobjHead(s32 index);
s32 kpaEnemyHitChk(f32 x, f32 y, f32 z, f32 a, f32 b);
s32 kpaEnemyFireHitChk(f32 x, f32 y, f32 z, f32 a, f32 b);
