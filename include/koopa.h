#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>

// Koopa's per-character work, allocated at mario->field_0x28C (size 0x154).
typedef struct KpaWork {
    u32 flags;              // 0x0
    s32 level;              // 0x4
    u8 unk08[0x18 - 0x08];  // 0x8
    void* unk18;            // 0x18
    void* unk1C;            // 0x1C
    void* head[10];         // 0x20
    u8 unk48[0x4C - 0x48];  // 0x48
    void* ride[10];         // 0x4C
    u8 unk74[0x78 - 0x74];  // 0x74
    f32 headDist[10];       // 0x78
    u8 unkA0[0xA4 - 0xA0];  // 0xA0
    f32 rideDist[10];       // 0xA4
    u8 unkCC[0xD0 - 0xCC];  // 0xCC
    s32 unkD0;              // 0xD0
    u8 unkD4[0xD8 - 0xD4];  // 0xD4
    s32 unkD8;              // 0xD8
    u8 unkDC[0xE0 - 0xDC];  // 0xDC
    s32 unkE0;              // 0xE0
    u8 unkE4[0x138 - 0xE4]; // 0xE4
    s32 mutekiTimer;        // 0x138
    s32 attackMode;         // 0x13C
    u8 unk140[0x14C - 0x140];
    u32 sfxId;              // 0x14C
    u8 unk150[0x154 - 0x150];
} KpaWork;

#define KPA(m) (*(KpaWork**)((u8*)(m) + 0x28C))

// userdata of the fire effect (fireefp->userdata); nodes are 0x50 bytes apart
typedef struct KpaFireNode {
    u8 unk00[0x04];
    Vec pos;        // 0x04
    u8 unk10[0x1C - 0x10];
    s32 unk1C;      // 0x1C
    u8 unk20[0x40 - 0x20];
    f32 unk40;      // 0x40
    s32 unk44;      // 0x44
    f32 unk48;      // 0x48
    void* unk4C;    // 0x4C
} KpaFireNode;

s32 kpaCamFollow(void);
void kpaChgPose(const char* marioPose, const char* kpa2Pose);
f32 kpaSetJumpPara(void);
s32 kpaMutekiCheck(void);
void kpaSetLevel(s32 level);
s32 kpaGetLevel(void);
void kpaAttackStart(void);
void kpa2DMain(void);
void kpa3DMain(void);
void kpaFireMain(void);
s32 kpaFireAttackCheck(void);
s32 kpaGetBodyStatus(void);
f32 kpaGetBaseScale(s32 index);
s32 kpaGetStageViewType(void);
void kpaFireAttack(void);
void kpa_jumpNpc(void);
s32 kpaGetStageType(void);
void* getKoopaFireEfp(void);
void kpaAttackStart2(void);
s32 kpaGetAttackMode(void);
void kpaCancelAttackMode(void);
f32 kpaGetWidth(s32 index);
f32 kpaSetFallPara(void);
void kpaMSetMutekiTimer(void);
void unk_801f8cd0(void);
void kpaMutekiOn(void);
void kpaMutekiOff(void);
