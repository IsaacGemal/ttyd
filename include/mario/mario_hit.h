#pragma once

#include <dolphin/mtx.h>

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>

extern const f32 float_18p5_8041a7ec;
extern const f32 float_0p265_8041a7f4;
extern const f32 float_0p01_8041a854;
extern const f32 float_neg10000_8041a858;
extern const f32 float_0p425_8041a864;
extern const f32 float_0p375_8041a868;
extern const f32 float_2_8041a7e8;
extern const f32 float_10_8041a7fc;
extern const f64 double_0p5_802c5928;
extern const f64 double_3_802c5930;
extern const f64 double_0_802c5938;
extern const Vec vec3_802c57a8;
extern const Vec vec3_802c57b4;

struct HitEntry;

BOOL marioChkBero(struct HitEntry* hit);
struct HitEntry* marioSearchGround(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
void* marioChkLandon(f32* pos, f32 spd);
void marioCheckWall(f32 a, f32 b);
void marioCheckWallShip(f32 a, f32 b);
void marioChkGnd2(void);
f32 marioChkOverhead(void);
void marioSearchFrontWall_staying(f32 a);
s32 marioSearchGroundRoll(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
void marioChkGnd(void);
void* marioLandonChk_old(f32* pos, f32 spd);
void* marioChkLandon2(f32* out, f32 a, f32 b);
HitEntry* dou10_yoko_yari(f32* p);
s32 marioSlitChkWallAround(void);
HitEntry* dou10_yoko_yari2(f32* out);
s32 searchUnder2(Vec* out, f32 x, f32 y, f32 z);
s32 marioChkWallAround(Vec* position, s32 flag, f32 a, f32 b, f32 c, f32 d);
HitEntry* searchFrontWall(Vec* a, f32* b, f32 c, f32 d, f32 e);
void* marioSearchHead_jabara(Vec* pos, f32* outY, s32* out, f32 a);
void* marioSearchHead(Vec* pos, f32* outY, f32 a);
void* marioHitCheck2(Vec* a, Vec* b, Vec* c, Vec* d, f32* e);
void* marioSearchHead_roll(Vec* pos, f32* outY, f32 a);
HitEntry* marioHitCheckVec(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
HitEntry* marioSearchUnder(void);
HitEntry* marioChkFrontStep(f32* outY, f32* outDist, f32* outAngle);
void clear_hitobj_ride(void);
void* marioChkFront(f32* out, Vec* pos, f32 angle, f32 dist);
HitEntry* marioHitCheck(f32* outX, f32* outY, f32* outZ, f32* distance, f32* outNX, f32* outNY, f32* outNZ, f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz);
void marioCheckWall_roll(f32 a, f32 b);
void marioChkToge(void);
HitEntry* N_dou10_yoko_yari3(void);
void marioHitCheckWidth(void);
extern const f32 float_0p25_8041a7d4;
#include "driver/hitdrv.h"
extern const f32 float_0p75_8041a7d8;
extern const f32 float_3p1416_8041a7e0;
extern const f32 float_180_8041a7cc;
extern const f32 float_neg1_8041a7dc;
extern const f32 float_100_8041a7f8;
extern const Vec vec3_802c5730;
extern const Vec vec3_802c5748;
extern const Vec vec3_802c573c;
extern const f32 float_8p64_8041a85c;
extern const f32 float_105_8041a860;
BOOL chkfilterVecVivian(HitCheckQuery* query, HitEntry* entry);
BOOL chkfilterVecRoll(HitCheckQuery* query, HitEntry* entry);
BOOL chkfilterVec(HitCheckQuery* query, HitEntry* entry);
f32 marioChkOverheadJabara(s32* out, f32* outF);
BOOL chkFilterAttr2(HitCheckQuery* query, HitEntry* entry);
