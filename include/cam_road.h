#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>

BOOL camRoadSetup(const char* filename);
void camRoadMain(f32* dst, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank);
void camRoadReset(void);
struct CamRoadState;
u32 compareCamRoad(struct CamRoadState* cur, struct CamRoadState* prev, f32* out);
// The original reads a seventh float in f7 on entry, which its only caller (camRoadMain) never sets.
void calcCamRoad(struct CamRoadState* a, struct CamRoadState* b, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az);
void calcHokanCamRoad(s32 flag, struct CamRoadState* a, struct CamRoadState* b, f32 t, struct CamRoadState* c, struct CamRoadState* out);
void calcCurrentCamState(f32* dst);
typedef struct CollisionCurveData {
    /* 0x00 */ u8 pad_00[0x40];
    /* 0x40 */ s32 unk_40;
    /* 0x44 */ s32 unk_44;
    /* 0x48 */ f32 unk_48;
    /* 0x4C */ f32 unk_4C;
    /* 0x50 */ u8 pad_50[0x10];
    /* 0x60 */ s32 unk_60;
    /* 0x64 */ s32 unk_64;
    /* 0x68 */ u8 pad_68[0x1C];
    /* 0x84 */ s32 unk_84;
    /* 0x88 */ u8 pad_88[0x18];
    /* 0xA0 */ s32 unk_A0;
    /* 0xA4 */ s32 unk_A4;
} CollisionCurveData;

typedef struct CollisionCurveHit {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ s32 unk_28;
} CollisionCurveHit;

BOOL collisionCurve(s32 mode, CollisionCurveData* data, Vec* p, CollisionCurveHit* out);
BOOL collisionTri_simple(Vec* p, Vec* v0, Vec* v1, Vec* v2, f32* out);
void camRoadCurveOn(const char* name);
void camRoadCurveOff(const char* name);
f32 camRoadGetCurrentShiftXRate(void);
struct CamRoadWork;
struct CamRoadWork* camRoadGetWorkPtr(void);
