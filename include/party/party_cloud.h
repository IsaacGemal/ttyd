#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/mtx.h>

typedef struct PartyCloud {
    f32 unk0; //0x0
    f32 unk4; //0x4
    f32 breathDist; //0x8, cloudGetBreathDist
    f32 maxDist; //0xC, cap for breathDist in getHitBreatheout2, set to 120 at init
    f32 sweepOffset1; //0x10, angle offset from unk4 for the first hit ray in getHitBreatheout2 (-20..20, 1 per frame)
    s32 sweepDown1; //0x14, 1 while sweepOffset1 is decreasing
    f32 sweepOffset2; //0x18, angle offset from unk4 for the second hit ray in getHitBreatheout2, same motion as sweepOffset1
    s32 sweepDown2; //0x1C, 1 while sweepOffset2 is decreasing
    struct HitEntry* hitBreathout[4]; //0x20, cloudGetHitBreathout(n) returns [n]
    s32 useTimer; //0x30, frames left in an event-driven use (cloud_use)
    s32 sfxId; //0x34, sound handle, 0xFFFFFFFF when none
    s16 mode; //0x38, cycles 0/1/2 per frame: hit-ray height 8/20/40 above Mario (getHitBreatheout2)
    u8 field_0x3A[0x48 - 0x3A]; // 0x3A
} PartyCloud;

void cloudGetAt(Vec* position);
void cloudResetAt();
f32 cloudGetBreathDir(void);
f32 cloudGetBreathDist(void);
struct HitEntry* cloudGetHitBreathout(s32 param_1);
u8 cloudGetStatus();
void cloudBobInit(PartyEntry* party);
void cloudBobUpdate(PartyEntry* party);
f32 cloudGetBreathPower(f32 param_1, Vec* param_2);
void cloud_init(PartyEntry* party);
void cloud_move(PartyEntry* party);
void cloud_exit(PartyEntry* party);
void cloud_use(PartyEntry* party);
void N__cloud_use(PartyEntry* party);
void getHitBreatheout2(PartyEntry* party, f32 angle);
#ifdef MUST_MATCH
void cloudSetMarioSpeed(s32 dir);
#endif

void mot_cloud(void);
