#include "koopa.h"
#include "koopa_hit.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mario_motion.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "system.h"
#include "math_ext.h"
#include <dolphin/mtx.h>
#include <string.h>

#define MW_F32(m, off) (*(f32*)((u8*)(m) + (off)))
#define MW_U32(m, off) (*(u32*)((u8*)(m) + (off)))
#define MW_S16(m, off) (*(s16*)((u8*)(m) + (off)))
#define MW_U16(m, off) (*(u16*)((u8*)(m) + (off)))

// head_sphere_data: sphere offsets (forward, height, radius) and a facing-condition type per level.
static s32 head_sphere_data_lv0[] = { 1, 10, 15, 6, 0 };
static s32 head_sphere_data_lv1[] = { 3, 32, 55, 24, 0, 26, 30, 16, 2, -10, 44, 25, 1 };
static s32 head_sphere_data_lv2[] = { 3, 45, 75, 34, 0, 36, 30, 16, 2, -15, 59, 36, 1 };
static s32 head_sphere_data_lv3[] = { 4, 100, 162, 80, 0, 90, 75, 30, 2, 50, 40, 30, 3, -43, 142, 74, 1 };
s32* head_sphere_data[] = { head_sphere_data_lv0, head_sphere_data_lv1, head_sphere_data_lv2,
                            head_sphere_data_lv3 };

const Vec vec3_802fe320 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe32c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe338 = { 0.0f, 0.0f, 0.0f };

extern const f32 float_50_80420eb0;
extern const f32 float_deg2rad_80420eb4;
extern const f32 float_90_80420eb8;
extern const f32 float_270_80420ebc;
extern const f32 float_7_80420ec0;

void* kpaFlameHitCheck(f32 x, f32 y, f32 z, f32 rad) {
    marioGetPtr();
    return hitCheckSphereFilter(kpaFlameHitFilter, x, y, z, rad);
}

BOOL kpaFlameHitFilter(HitCheckQuery* query, HitEntry* entry) {
    MapObjectEntry* obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, "MOBJ_HiddenHatenaBlock") == 0) {
        return FALSE;
    }
    return strcmp(obj->animName, "MOBJ_Block") != 0;
}

// NONMATCHING (91.6%): register allocation only -- `result` lands in r27 vs the original's r25, the 0.0f
// constant in r25 vs r26, plus an extra `mr r29,r28`; instruction selection and constants all match.
void* kpaHitCheck(void) {
    void* result = NULL;
    MarioWork* mp;
    s32* data;
    s32 count;
    s32* entry;
    s32 i;

    mp = marioGetPtr();
    if (mp->characterId != 2) {
        return NULL;
    }
    KPA(mp)->unk1C = NULL;
    for (i = 0; i < 10; i++) {
        KPA(marioGetPtr())->head[i] = NULL;
        KPA(marioGetPtr())->headDist[i] = 1000.0f;
    }
    if (kpaFireAttackCheck() == 1) {
        return NULL;
    }
    data = head_sphere_data[kpaGetLevel()];
    count = data[0];
    entry = data + 1;
    for (i = 0; i < count; i++, entry += 4) {
        f32 x = (f32)entry[0] * (f32)sin(3.1415927f * mp->directionView / 180.0f) + mp->position.x;
        f32 z = mp->position.z - (f32)entry[0] * (f32)cos(3.1415927f * mp->directionView / 180.0f);
        f32 y = mp->position.y + (f32)entry[1];
        f32 c = (f32)entry[2];
        void* hit;

        switch (entry[3]) {
        case 1:
            if (!(mp->flags & 0x10000)) continue;
            break;
        case 2:
            if (mp->flags & 0x10000) continue;
            break;
        case 3:
            if (mp->flags & 0x30000) continue;
            break;
        }
        hit = hitCheckSphereFilter(NULL, x, y, z, c);
        if (mp->flags & 0x10000) {
            if (hitGetAttr(hit) & 0x80000000) {
                if (mobjHitObjPtrToPtr(hit)->position.y > y) {
                    KPA(marioGetPtr())->head[i] = hit;
                }
            }
        }
        if (hit != NULL) {
            result = hit;
        }
    }
    KPA(mp)->unk1C = result;
    return result;
}

void* kpaGetHitFlame(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return NULL;
    }
    return KPA(mp)->unk18;
}

void* kpaGetHitobj(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return NULL;
    }
    return KPA(mp)->unk1C;
}

// NONMATCHING (99.7%): the final 1000.0f store reuses r3 where the original materialises the offset in r4.
void kpaClearHitobjRide(void) {
    MarioWork* mp = marioGetPtr();
    KPA(mp)->ride[0] = NULL;
    KPA(mp)->ride[1] = NULL;
    KPA(mp)->ride[2] = NULL;
    KPA(mp)->ride[3] = NULL;
    KPA(mp)->ride[4] = NULL;
    KPA(mp)->ride[5] = NULL;
    KPA(mp)->ride[6] = NULL;
    KPA(mp)->ride[7] = NULL;
    KPA(mp)->ride[8] = NULL;
    KPA(mp)->ride[9] = NULL;
}

s32 kpaChkHitobjRide(void) {
    KpaWork* kpa = KPA(marioGetPtr());
    void* result = kpa->ride[0];
    if (result == NULL) {
        result = kpa->ride[1];
        if (result == NULL) {
            result = kpa->ride[2];
            if (result == NULL) {
                result = kpa->ride[3];
                if (result == NULL) {
                    result = kpa->ride[4];
                    if (result == NULL) {
                        result = kpa->ride[5];
                        if (result == NULL) {
                            result = kpa->ride[6];
                            if (result == NULL) {
                                result = kpa->ride[7];
                                if (result == NULL) {
                                    result = kpa->ride[8];
                                    if (result == NULL) {
                                        result = kpa->ride[9];
                                        if (result == NULL) {
                                            return (s32)result;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return (s32)result;
}

// NONMATCHING (72.9%): MWCC folds the 0x4C field into `lwz 0x4c(r3)` after adding the base, where the
// original emits `addi r0,r4,0x4c; lwzx`.
void* kpaGetHitobjRide(s32 index) {
    KpaWork* kpa = KPA(marioGetPtr());
    return kpa->ride[index];
}

void* kpaGetHitobjHead2(void) {
    return KPA(marioGetPtr())->head[0];
}

// NONMATCHING (72.9%): MWCC folds the 0x20 field into `lwz 0x20(r3)` after adding the base, where the
// original emits `addi r0,r4,0x20; lwzx`.
void* kpaGetHitobjHead(s32 index) {
    KpaWork* kpa = KPA(marioGetPtr());
    return kpa->head[index];
}

// The original is a no-op stub (`li r3, 0; blr`).  The return type is void* -- the header previously said
// `void`, which cannot produce the original's `li r3, 0` (see REPORT.md).
void* kpaSearchFrontUnder(void) {
    return (void*)(s32)0;
}

// kpaSearchHead's per-head offset tables (forward, height, facing condition).
static s32 _ofs0[] = { 3, 12, 16, 1, 0, 16, 0, -9, 16, 0 };
static s32 _ofs1[] = { 4, 25, 74, 0, 50, 65, 1, 0, 67, 0, -25, 62, 0 };
static s32 _ofs2[] = { 6, 50, 102, 0, 76, 90, 1, 25, 100, 0, 0, 94, 0, -25, 90, 0, -45, 68, 0 };
static s32 _ofs3[] = { 13, 170, 194, 1, 145, 204, 0, 120, 232, 0, 95, 227, 0, 70, 232, 0, 55, 232,
                       0,  30,  194, 0, 5,   187, 0, -20, 210, 0, -45, 201, 0, -70, 181, 0, -95, 153,
                       0,  -120, 56, 0 };
static s32* headchkofs[] = { _ofs0, _ofs1, _ofs2, _ofs3 };

// NONMATCHING (81.7%): the spilled float register set differs (extra f27 save) and the `kpaGetLevel()==0`
// test emits bne where the original emits beq.
void* kpaSearchHead(Vec* pos, f32* outY, f32 a) {
    MarioWork* mp = marioGetPtr();
    void* result = NULL;
    f32 angle;
    f32 dist;
    s32* entry;
    s32 count;
    s32 i;
    s32 ofs;

    if (mp->flags & 0x20000) {
        return NULL;
    }
    if (mp->baseSpeed != 0.0f) {
        angle = mp->directionView;
    } else {
        angle = toMovedir(MW_F32(mp, 0x1A8));
    }
    dist = *outY;
    *outY = -1.0f;
    for (i = 0; i < 10; i++) {
        KPA(marioGetPtr())->head[i] = NULL;
        KPA(marioGetPtr())->headDist[i] = 1000.0f;
    }
    entry = headchkofs[kpaGetLevel()];
    count = entry[0];
    entry = entry + 1;
    if (!(mp->flags & 0x10000)) {
        switch (kpaGetLevel()) {
        case 0:
            dist = 2.0f;
            break;
        case 1:
            dist = 0.0f;
            break;
        case 2:
            dist = 0.0f;
            break;
        case 3:
            dist = 35.0f;
            break;
        }
    } else {
        if (kpaGetLevel() == 0) {
            if (mp->currMotionId == 0x25) {
                dist = 10.0f;
            } else {
                dist = a;
            }
        } else {
            dist = a;
        }
    }
    angle = 3.1415927f * angle / 180.0f;
    ofs = 0;
    for (i = 0; i < count; i++, entry += 3, ofs += 4) {
        f32 sx = (f32)entry[0] * (f32)sin(angle);
        f32 z = mp->position.z - (f32)entry[0] * (f32)cos(angle);
        f32 x = mp->position.x + sx;
        f32 y = mp->position.y + (f32)entry[1];
        f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;
        void* hit;

        if (entry[2] == 0) {
            if (!(mp->flags & 0x10000)) continue;
        }
        distance = dist;
        hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, 0.0f, 1.0f,
                             0.0f);
        if (hit == NULL) continue;
        {
            f32 q = (f32)(s32)(100.0f * hitY + 0.5f) / 100.0f;
            if (y + dist >= q) {
                pos->y = q;
                result = hit;
                *outY = 0.0f;
                KPA(marioGetPtr())->head[i] = hit;
            } else {
                *outY = q - pos->y - dist;
            }
        }
    }
    return result;
}

// NONMATCHING (98.1%): one scheduled instruction -- mine hoists `lfs f1,0x90(r31)` (mario y) above the
// preceding `fadds`; every other instruction matches.
s32 kpaEnemyHitChk(f32 x, f32 y, f32 z, f32 a, f32 b) {
    MarioWork* mp = marioGetPtr();
    f32 halfH;
    f32 halfW;
    f32 dx;
    f32 dy;

    if (kpaEnemyFireHitChk(x, y, z, a, b) != 0) {
        return 2;
    }
    halfH = 0.5f * (mp->wPlayerCollisionBox.y + a);
    halfW = 0.5f * (mp->wPlayerCollisionBox.x + b);
    dx = __fabsf(mp->position.x - x);
    dy = __fabsf(mp->position.y + 0.5f * mp->wPlayerCollisionBox.y - y - 0.5f * a);
    if (dy <= halfH && dx <= halfW) {
        if (mp->flags & 0x20000) {
            if (mp->position.y > y) {
                if (kpaGetLevel() == 0 && kpaGetStageType() != 1 && MW_S16(mp, 0x4C) == 0 && z < float_50_80420eb0) {
                    marioChgMot(4);
                }
                return 1;
            }
        }
        if (kpaMutekiCheck() == 1) {
            return 3;
        }
        if (MW_S16(mp, 0x4C) != 0) {
            return 3;
        }
        if (MW_S16(mp, 0x78) == 0) {
            return 4;
        }
    }
    return 0;
}

// NONMATCHING (94.9%): frame is 0x10 bytes smaller; the argument registers for a/b are swapped and the `pos`
// vector is kept in registers instead of being copied on the stack, shifting every matrix offset.
s32 kpaEnemyFireHitChk(f32 x, f32 y, f32 z, f32 a, f32 b) {
    EffectEntry* efp = getKoopaFireEfp();
    KpaFireNode* root;
    KpaFireNode* node;
    CameraEntry* cam;
    Mtx m0, m1, m2, m3;
    Vec v;
    Vec v0;
    Vec out;
    Vec pos;
    Vec diff;
    f32 radius;
    f32 scale;
    f32 threshold;
    s32 i;

    camGetPtr(4);
    v0 = vec3_802fe320;
    v0.x = x;
    v0.y = 0.5f * a + y;
    v0.z = z;
    pos = v0;
    if (efp == NULL) {
        return 0;
    }
    root = efp->userdata;
    radius = root->unk40;
    PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
    cam = camGetPtr(4);
    PSMTXRotRad(m1, 0x79, (root->unk48 - MW_F32(cam, 0x114)) * float_deg2rad_80420eb4);
    if (root->unk48 > float_90_80420eb8 && root->unk48 < float_270_80420ebc) {
        PSMTXScale(m2, root->unk40, root->unk40, -root->unk40);
    } else {
        PSMTXScale(m2, root->unk40, root->unk40, root->unk40);
    }
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m3);
    i = 1;
    node = (KpaFireNode*)((u8*)root + 0x50);
    threshold = 0.5f * b;
    for (; i < efp->count; i++, node = (KpaFireNode*)((u8*)node + 0x50)) {
        f32 mag;
        if (node->unk44 != 0) {
            continue;
        }
        scale = float_7_80420ec0 * node->unk40;
        PSMTXTrans(m0, node->pos.x, node->pos.y, node->pos.z);
        PSMTXScale(m2, scale, scale, scale);
        PSMTXConcat(m0, m2, m0);
        PSMTXConcat(m3, m0, m0);
        v = vec3_802fe32c;
        PSMTXMultVec(m0, &v, &out);
        PSVECSubtract(&pos, &out, &diff);
        mag = PSVECMag(&diff);
        if (mag < scale * radius + threshold) {
            return 1;
        }
    }
    return 0;
}
