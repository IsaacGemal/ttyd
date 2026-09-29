#pragma once

#include <dolphin/types.h>

typedef struct ZubaStarWork ZubaStarWork;

ZubaStarWork* GetZubaStarPtr(void);
#include <dolphin/mtx.h>
void zubastar_bunkatu(Vec* a, Vec* b);
s32 zubastar_get_kouten(f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8, f32* out1, f32* out2);
