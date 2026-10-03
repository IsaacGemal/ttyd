#pragma once

#include <dolphin/mtx.h>
#include <dolphin/types.h>

typedef struct ZubaStarWork ZubaStarWork;

typedef struct ZubaStarPoints4 { Vec v[4]; } ZubaStarPoints4;

extern const Vec dat_802ff6d4[4];
extern const Vec vec3_802ff704;
extern const Vec vec3_802ff710;
extern const Vec vec3_802ff71c;
extern const Vec vec3_802ff728;
extern const Vec vec3_802ff734;
extern const Vec vec3_802ff740;
extern const Vec vec3_802ff74c;
extern const Vec vec3_802ff758;
extern const Vec vec3_802ff764;
extern const Vec vec3_802ff770;
extern const Vec vec3_802ff77c;
extern const Vec vec3_802ff788;
extern const Vec vec3_802ff794;
extern const Vec vec3_802ff7a0;
extern const char str_802FF830[];
extern const char str_802FF848[];
extern const char str_802FF668[];

ZubaStarWork* GetZubaStarPtr(void);
void zubastar_create_takaku(s32 count, Vec* points);
void zubastar_bunkatu(Vec* a, Vec* b);
s32 zubastar_get_kouten(f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8, f32* out1, f32* out2);
