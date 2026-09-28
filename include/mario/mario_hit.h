#pragma once

#include <dolphin/mtx.h>

typedef struct HitEntry HitEntry;

#include <dolphin/types.h>

struct HitEntry;

BOOL marioChkBero(struct HitEntry* hit);
struct HitEntry* marioSearchGround(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
void* marioChkLandon(f32* pos, f32 spd);
void marioCheckWall(f32 a, f32 b);
void marioChkGnd2(void);
f32 marioChkOverhead(void);
void marioSearchFrontWall_staying(f32 a);
s32 marioSearchGroundRoll(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
void marioChkGnd(void);
void* marioLandonChk_old(f32* pos, f32 spd);
HitEntry* dou10_yoko_yari(f32* p);
s32 marioSlitChkWallAround(void);
HitEntry* dou10_yoko_yari2(f32* out);
s32 searchUnder2(Vec* out, f32 x, f32 y, f32 z);
void marioChkWallAround(Vec* position, s32 flag, f32 a, f32 b, f32 c, f32 d);
HitEntry* searchFrontWall(Vec* a, f32* b, f32 c, f32 d, f32 e);
void* marioSearchHead_jabara(Vec* pos, f32* outY, s32* out, f32 a);
void* marioSearchHead(Vec* pos, f32* outY, f32 a);
void* marioHitCheck2(Vec* a, Vec* b, Vec* c, Vec* d, f32* e);
void* marioSearchHead_roll(Vec* pos, f32* outY, f32 a);
HitEntry* marioHitCheckVec(Vec* a, Vec* b, Vec* c, Vec* d, f32* distance);
HitEntry* marioSearchUnder(void);
void clear_hitobj_ride(void);
void* marioChkFront(f32* out, Vec* pos, f32 angle, f32 dist);
HitEntry* marioHitCheck(f32* outX, f32* outY, f32* outZ, f32* distance, f32* outNX, f32* outNY, f32* outNZ, f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz);
