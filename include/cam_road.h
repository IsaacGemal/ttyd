#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>

typedef struct CamRoadState {
    /* 0x00 */ s32 valid;
    /* 0x04 */ s32 onRoad; // 1 when the camera is over a road triangle
    /* 0x08 */ s32 flags; // collision clamp bits; on the blended state, 1 while the position slides
    /* 0x0C */ f32 inX;
    /* 0x10 */ f32 inY;
    /* 0x14 */ f32 inZ;
    /* 0x18 */ f32 inAx;
    /* 0x1C */ f32 inAy;
    /* 0x20 */ f32 inAz;
    /* 0x24 */ void* road;
    /* 0x28 */ f32 x;
    /* 0x2C */ f32 y;
    /* 0x30 */ f32 z;
    /* 0x34 */ f32 dirX;
    /* 0x38 */ f32 dirY;
    /* 0x3C */ f32 dirZ;
    /* 0x40 */ f32 radius;
    /* 0x44 */ f32 yaw;
    /* 0x48 */ f32 roll;
    /* 0x4C */ f32 segment;
    /* 0x50 */ s64 time;
    /* 0x58 */ f32 bank;
    /* 0x5C */ f32 shiftXRate;
} CamRoadState;

typedef struct CamRoadWork {
    /* 0x000 */ struct CamRoadMap* map;
    /* 0x004 */ u8 pad_004[0x4];
    /* 0x008 */ CamRoadState blended;
    /* 0x068 */ CamRoadState current;
    /* 0x0C8 */ s32 hokanActive;
    /* 0x0CC */ s32 hokanBlendPos;
    /* 0x0D0 */ f32 hokanT;
    /* 0x0D4 */ f32 hokanRate;
    /* 0x0D8 */ CamRoadState previous;
    /* 0x138 */ CamRoadState hokanFrom;
    /* 0x198 */ s32 changeFlags;
    /* 0x19C */ f32 distRate;
    /* 0x1A0 */ f32 velRate;
    /* 0x1A4 */ f32 sideRecoverA;
    /* 0x1A8 */ f32 sideRecoverB;
    /* 0x1AC */ f32 sideWeightA;
    /* 0x1B0 */ f32 sideWeightB;
    /* 0x1B4 */ f32 cmpHeadingW;
    /* 0x1B8 */ f32 cmpYawW;
    /* 0x1BC */ f32 cmpRollW;
    /* 0x1C0 */ f32 cmpRadiusW;
    /* 0x1C4 */ f32 cmpDistW;
    /* 0x1C8 */ OSTime hokanTime;
    /* 0x1D0 */ u32 mapKind;
    /* 0x1D4 */ f32 noRoadX;
    /* 0x1D8 */ f32 noRoadY;
    /* 0x1DC */ f32 noRoadZ;
    /* 0x1E0 */ f32 noRoadAx;
    /* 0x1E4 */ f32 noRoadAy;
    /* 0x1E8 */ f32 noRoadAz;
    /* 0x1EC */ f32 camX;
    /* 0x1F0 */ f32 camY;
    /* 0x1F4 */ f32 camZ;
    /* 0x1F8 */ f32 camAx;
    /* 0x1FC */ f32 camAy;
    /* 0x200 */ f32 camAz;
    /* 0x204 */ s32 field_0x204;
    /* 0x208 */ s32 field_0x208;
    /* 0x20C */ u8 pad_20C[0x4];
} CamRoadWork;

BOOL camRoadSetup(const char* filename);
void camRoadMain(f32* dst, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank);
void camRoadReset(void);
u32 compareCamRoad(struct CamRoadState* cur, struct CamRoadState* prev, f32* out);
void calcCamRoad(struct CamRoadState* a, struct CamRoadState* b, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank);
void calcHokanCamRoad(s32 flag, struct CamRoadState* a, struct CamRoadState* b, f32 t, struct CamRoadState* c, struct CamRoadState* out);
void calcCurrentCamState(f32* dst);
typedef struct CollisionCurveData {
    /* 0x00 */ u8 pad_00[0x40];
    /* 0x40 */ s32 firstSeg;
    /* 0x44 */ s32 lastSeg;
    /* 0x48 */ f32 limitA;
    /* 0x4C */ f32 limitB;
    /* 0x50 */ u8 pad_50[0x10];
    /* 0x60 */ s32 curvePtsStart;
    /* 0x64 */ s32 curvePtsCount;
    /* 0x68 */ u8 pad_68[0x1C];
    /* 0x84 */ s32 clampEnds;
    /* 0x88 */ u8 pad_88[0x18];
    /* 0xA0 */ s32 ptsStart;
    /* 0xA4 */ s32 ptsCount;
} CollisionCurveData;

typedef struct CollisionCurveHit {
    /* 0x00 */ f32 param; // the curve points' .y channel (second point set), interpolated; calcCamRoad clamps it to 0..1
    /* 0x04 */ u8 pad_04[0x4];
    /* 0x08 */ f32 startY;
    /* 0x0C */ f32 x;
    /* 0x10 */ f32 y; // always written 0
    /* 0x14 */ f32 z;
    /* 0x18 */ f32 normX;
    /* 0x1C */ f32 normY;
    /* 0x20 */ f32 normZ;
    /* 0x24 */ s32 segment;
    /* 0x28 */ s32 flags;
} CollisionCurveHit;

BOOL collisionCurve(s32 mode, CollisionCurveData* data, Vec p, CollisionCurveHit* out);
BOOL collisionTri_simple(Vec p, Vec v0, Vec v1, Vec v2, f32* out);
void camRoadCurveOn(const char* name);
void camRoadCurveOff(const char* name);
f32 camRoadGetCurrentShiftXRate(void);
CamRoadWork* camRoadGetWorkPtr(void);
