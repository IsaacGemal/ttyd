#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>

// Koopa's per-character work, allocated at mario->koopaWork (size 0x154).
typedef struct KpaWork {
    u32 flags;              // 0x0 (KpaFlags)
    s32 level;              // 0x4
    u8 unk08[0x10 - 0x08];  // 0x8
    f32 field_0x10;         // 0x10 (kpa_walk sets +0.02; no reader in the koopa files)
    f32 field_0x14;         // 0x14 (kpa_walk sets -0.02; no reader in the koopa files)
    void* hitFlame;         // 0x18 (kpaGetHitFlame, kpaFireMain)
    void* hitobj;           // 0x1C (kpaGetHitobj, kpaHitCheck)
    void* head[10];         // 0x20
    u8 unk48[0x4C - 0x48];  // 0x48
    void* ride[10];         // 0x4C
    u8 unk74[0x78 - 0x74];  // 0x74
    f32 headDist[10];       // 0x78
    u8 unkA0[0xA4 - 0xA0];  // 0xA0
    f32 rideDist[10];       // 0xA4
    u8 unkCC[0xD0 - 0xCC];  // 0xCC
    s32 moveRate;           // 0xD0 (kpa_walk/kpa_dash, from _rateTbl/_rateTbl2)
    u8 unkD4[0xD8 - 0xD4];  // 0xD4
    s32 unkD8;              // 0xD8
    u8 unkDC[0xE0 - 0xDC];  // 0xDC
    s32 unkE0;              // 0xE0
    u8 unkE4[0x138 - 0xE4]; // 0xE4
    s32 mutekiTimer;        // 0x138
    s32 attackMode;         // 0x13C
    s32 attackTimer;        // 0x140 (counts down each attack step)
    s32 field_0x144;        // 0x144 (nonzero keeps attack mode 0x17's breath going; evt_koopa_fire sets it while its count runs)
    u8 unk148[0x14C - 0x148];
    u32 sfxId;              // 0x14C (KPA_SFX_NONE when silent)
    s32 bubbleTimer;        // 0x150 (kpa_swim frames until the next bubble)
} KpaWork;

#define KPA(m) ((m)->koopaWork)

typedef enum KpaFlags {
    KPA_FLAG_NO_HIP = 0x10,                // set on kpa_swim entry; kpaHipAttackOk returns 0 while set
    KPA_FLAG_SPLASH_PENDING = 0x100,       // set by kpa_fall over water / kpa_swim's surface jump; cleared when kpa_swim plays the landing SFX
    KPA_FLAG_MUTEKI = 0x10000,             // kpaMutekiOn/Off; kpaMutekiCheck reads it
    KPA_FLAG_ALT_POSE = 0x01000000,        // kpa_stay/kpa_walk switch to the KPA_*_2 pose when set
} KpaFlags;

#define KPA_SFX_NONE 0xFFFFFFFFu

// userdata of the fire effect (fireefp->userdata); nodes are 0x50 bytes apart
typedef struct KpaFireNode {
    u8 unk00[0x04];
    Vec pos;        // 0x04
    u8 unk10[0x1C - 0x10];
    s32 unk1C;      // 0x1C
    u8 unk20[0x40 - 0x20];
    f32 radius;     // 0x40
    s32 noHit;      // 0x44 (nonzero: skipped by the fire hit checks)
    f32 yaw;        // 0x48
    struct EffectEntry* nextEfp; // 0x4C (next node's effect entry)
} KpaFireNode;

s32 kpaCamFollow(void);
void kpaChgPose(const char* marioPose, const char* kpa2Pose);
f32 kpaSetJumpPara(void);
s32 kpaMutekiCheck(void);
void kpaSetLevel(s32 level);
void kpaPreInit(void);
s32 kpaGetLevel(void);
BOOL kpaHipAttackCheck(void);
void kpaAttackStart(void);
void kpa2DMain(void);
void kpa3DMain(void);
void kpaFireMain(void);
s32 kpaFireAttackCheck(void);
s32 kpaJumpChk(void);
s32 kpaGetBodyStatus(void);
f32 kpaGetBaseScale(s32 index);
s32 kpaGetStageViewType(void);
void kpaPauseSE2(void);
s32 kpaNoHitEnemyCheck(void);
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
void kpaMain(void);
void kpaFall(void);

extern const f32 float_0_80420e80;
