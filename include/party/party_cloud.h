#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/mtx.h>

typedef struct PartyCloud {
    f32 unk0; //0x0
    f32 unk4; //0x4
    f32 unk8; //0x8
    u8 field_0xC[0x20 - 0xC]; // 0xC
    s32 unk20; //0x20
    s32 unk24; //0x24
    s32 unk28; //0x28
    s32 unk2C; //0x2C
    u8 field_0x30[0x48 - 0xC];
} PartyCloud;

void cloudGetAt(Vec* position);
void cloudResetAt();
f32 cloudGetBreathDir(void);
f32 cloudGetBreathDist(void);
s32 cloudGetHitBreathout(s32 param_1);
u8 cloudGetStatus();
void unk_801820d0(PartyEntry* party);
void unk_8018206c(PartyEntry* party);
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
