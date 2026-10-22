#pragma once

#include <dolphin/mtx.h>

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>

struct HitEntry;

BOOL marioChkBero(struct HitEntry* hit);
struct HitEntry* marioSearchGround(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
HitEntry* marioChkLandon(f32* pos, f32 spd);
void marioCheckWall(f32 a, f32 b);
void marioCheckWallShip(f32 a, f32 b);
void marioChkGnd2(void);
f32 marioChkOverhead(void);
void marioSearchFrontWall_staying(f32 a);
HitEntry* marioSearchGroundRoll(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
void marioChkGnd(void);
HitEntry* marioLandonChk_old(f32* pos, f32 spd);
HitEntry* marioChkLandon2(f32* out, f32 a, f32 b);
HitEntry* dou10_yoko_yari(f32* p);
s32 marioSlitChkWallAround(void);
HitEntry* dou10_yoko_yari2(f32* out);
HitEntry* searchUnder2(f32* out, f32 x, f32 y, f32 z);
s32 marioChkWallAround(Vec* position, s32 flag, f32 a, f32 b, f32 c, f32 d);
HitEntry* searchFrontWall(Vec* a, f32* b, f32 c, f32 d, f32 e);
HitEntry* marioSearchHead_jabara(Vec* pos, f32* outY, s32* out, f32 a);
HitEntry* marioSearchHead(Vec* pos, f32* outY, f32 a);
HitEntry* marioHitCheck2(Vec* a, Vec* b, Vec* c, Vec* d, f32* e);
HitEntry* marioSearchHead_roll(Vec* pos, f32* outY, f32 a);
HitEntry* marioHitCheckVec(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
HitEntry* marioSearchUnder(void);
HitEntry* marioChkFrontStep(f32* outY, f32* outDist, f32* outAngle);
void clear_hitobj_ride(void);
HitEntry* marioChkFront(f32* out, Vec pos, f32 angle, f32 dist);
HitEntry* marioHitCheck(f32* outX, f32* outY, f32* outZ, f32* distance, f32* outNX, f32* outNY, f32* outNZ, f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz);
void marioCheckWall_roll(f32 a, f32 b);
void marioChkToge(void);
HitEntry* N_dou10_yoko_yari3(void* unused);
void marioHitCheckWidth(void);
#include "driver/hitdrv.h"
BOOL chkfilterVecVivian(HitCheckQuery* query, HitEntry* entry);
BOOL chkfilterVecRoll(HitCheckQuery* query, HitEntry* entry);
BOOL chkfilterVec(HitCheckQuery* query, HitEntry* entry);
f32 marioChkOverheadJabara(s32* out, f32* outF);
BOOL chkFilterAttr2(HitCheckQuery* query, HitEntry* entry);
