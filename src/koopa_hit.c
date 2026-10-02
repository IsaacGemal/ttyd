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
s32 head_sphere_data_lv0[] = { 1, 10, 15, 6, 0 };
s32 head_sphere_data_lv1[] = { 3, 32, 55, 24, 0, 26, 30, 16, 2, -10, 44, 25, 1 };
s32 head_sphere_data_lv2[] = { 3, 45, 75, 34, 0, 36, 30, 16, 2, -15, 59, 36, 1 };
s32 head_sphere_data_lv3[] = { 4, 100, 162, 80, 0, 90, 75, 30, 2, 50, 40, 30, 3, -43, 142, 74, 1 };
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
extern const f32 float_0p5_80420e70;
extern const f32 float_1000_80420e74;
extern const f32 float_3p1416_80420e78;
extern const f32 float_180_80420e7c;
extern const f32 float_neg1_80420e84;
extern const f32 float_2_80420e88;
extern const f32 float_100_80420e98;
extern const f32 float_0p01_80420e9c;
extern const f32 float_0p25_80420ea0;
extern const f32 float_0p6_80420ea4;
extern const f32 float_3_80420ea8;
extern const f32 float_0p05_80420eac;

// Reusing the work pointer as the node cursor (`root++` here and in the for-increment) makes mwcc schedule the
// preheader `li r29,1` before `addi r30,r30,0x50`; a separate `node` pointer emits them in the opposite order.
s32 kpaEnemyFireHitChk(f32 x, f32 y, f32 z, f32 a, f32 b) {
    EffectEntry* efp = getKoopaFireEfp();
    KpaFireNode* root;
    Mtx m2, m0, m1, m3;
    Vec out;
    Vec pos;
    Vec diff;
    Vec v0;
    Vec v;
    f32 threshold;
    f32 scale;
    f32 radius;
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
    PSMTXRotRad(m1, 0x79, (root->unk48 - camGetPtr(4)->viewYaw) * float_deg2rad_80420eb4);
    if (root->unk48 > float_90_80420eb8 && root->unk48 < float_270_80420ebc) {
        PSMTXScale(m2, root->unk40, root->unk40, -root->unk40);
    } else {
        PSMTXScale(m2, root->unk40, root->unk40, root->unk40);
    }
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m3);
    root++;
    i = 1;
    threshold = 0.5f * b;
    for (; i < efp->count; i++, root++) {
        f32 mag;
        if (root->unk44 != 0) {
            continue;
        }
        scale = float_7_80420ec0 * root->unk40;
        PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
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

s32 kpaEnemyHitChk(f32 x, f32 y, f32 z, f32 a, f32 b) {
    MarioWork* mp = marioGetPtr();
    f32 halfH;
    f32 halfW;
    f32 dx;
    f32 dy;

    if (kpaEnemyFireHitChk(x, y, z, a, b) != 0) {
        return 2;
    }
#ifdef MUST_MATCH
    mp->position.x = mp->position.x; // retail reloads/stores position.x here (pins the float load order)
#endif
    halfH = 0.5f * (mp->wPlayerCollisionBox.y + a);
    halfW = 0.5f * (mp->wPlayerCollisionBox.x + b);
    dx = __fabsf(mp->position.x - x);
    dy = __fabsf(mp->position.y + 0.5f * mp->wPlayerCollisionBox.y - y - 0.5f * a);
    if (dy <= halfH && dx <= halfW) {
        if (mp->flags & 0x20000) {
            if (mp->position.y > y) {
                if (kpaGetLevel() == 0 && kpaGetStageType() != 1 && mp->unk4C == 0 && z < float_50_80420eb0) {
                    marioChgMot(4);
                }
                return 1;
            }
        }
        if (kpaMutekiCheck() == 1) {
            return 3;
        }
        if (mp->unk4C != 0) {
            return 3;
        }
        if (MW_S16(mp, 0x78) == 0) {
            return 4;
        }
    }
    return 0;
}

// float sqrt/NaN helpers for kpaSearchGround's inline expansion (same shapes as party_hit.c).
extern s32 __float_nan[];
#define KPA_NAN (*(f32*)__float_nan)

static inline s32 kpa_fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return 1;
            } else {
                return 2;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return 5;
            } else {
                return 3;
            }
            break;
        default:
            return 4;
    }
}
#define fpclassify(x) kpa_fpclassifyf(x)
#define isnan(x) (fpclassify(x) == 1)

static inline f32 sqrtf(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return KPA_NAN;
    } else if (isnan(x)) {
        return KPA_NAN;
    }
    return x;
}

static inline f32 kpaNrmToAngle(Vec* v) {
    f32 len = sqrtf(v->x * v->x + v->z * v->z);

    return angleABf(0.0f, 0.0f, len, -v->y);
}

// NONMATCHING (99.96%): byte-for-byte except 4 register-name diffs — the two Vec-n reload blocks load the
// hitN slots (0x18,0x14,0x10) in the opposite order to the target (0x10,0x14,0x18); dest registers and the
// whole rest of the function match. Pure scheduler order; every source shape tried kept it.
void* kpaSearchGround(f32* a, f32* b, f32* c, f32 d) {
    MarioWork* mp;
    s32 i;
    s32 count;
    void* hit;
    f32 piAngle;
    f32 off;
    f32 zero = 0.0f;
    f32 rad2;
    f32 rad3;
    f32 dist;
    f32 y;
    f32 base;
    f32 px, py, pz;
    f32 height;
    f32 angle;
    f32 dx, dz;
    f32 x, z;
    f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;
    void* result;
    mp = marioGetPtr();
    result = NULL;
    MW_F32(mp, 0x218) = 0.0f;
    MW_F32(mp, 0x21c) = 0.0f;
    MW_F32(mp, 0x220) = 0.0f;
    MW_F32(mp, 0x214) = 0.0f;
    if (mp->flags & MARIO_FLAG_FORCE_WALL) {
        *a = mp->position.y;
        return NULL;
    }
    d += float_0p01_80420e9c;
    if (kpaGetLevel() == 0) {
        height = mp->wPlayerCollisionBox.y;
    } else {
        height = float_0p25_80420ea0 * mp->wPlayerCollisionBox.y;
    }
    *a = mp->position.y;
    *b = float_neg1_80420e84;
    if (mp->baseSpeed != float_0_80420e80) {
        angle = mp->directionView;
    } else {
        angle = toMovedir(mp->unk1A8);
    }
    px = mp->position.x;
    py = mp->position.y;
    pz = mp->position.z;
    for (i = 0; i < 10; i++) {
        KPA(marioGetPtr())->ride[i] = NULL;
        KPA(marioGetPtr())->rideDist[i] = float_1000_80420e74;
    }
    switch (kpaGetLevel()) {
    case 0:
    case 1:
    case 2:
        count = 3;
        base = 0.6f * (0.5 * mp->wPlayerCollisionBox.x);
        break;
    case 3:
        count = 7;
        base = float_0p6_80420ea4 * (float_0p5_80420e70 * mp->wPlayerCollisionBox.x / float_3_80420ea8);
        break;
    }
    dist = height + d;
    piAngle = float_3p1416_80420e78 * angle;
    y = py + height;
    off = 0.05f;
    rad2 = float_2_80420e88 * base - off;
    rad3 = float_3_80420ea8 * base - off;
    for (i = 0; i < count; i++) {
        switch (i) {
        case 0:
            dx = 0.0f;
            dz = 0.0f;
            break;
        case 1: {
            f32 r = piAngle / float_180_80420e7c;
            f32 c;
            f32 b;
            dx = (base - float_0p05_80420eac) * (f32)sin((f64)r);
            c = (f32)cos((f64)r);
            b = base - float_0p05_80420eac;
            dz = b * -c;
            break;
        }
        case 2: {
            f32 r = piAngle / float_180_80420e7c;
            f32 s;
            f32 c;
            f32 b;
            s = (f32)sin((f64)r);
            b = base - float_0p05_80420eac;
            dx = b * -s;
            c = (f32)cos((f64)r);
            b = base - float_0p05_80420eac;
            dz = b * -c;
            break;
        }
        case 3: {
            f32 r = piAngle / float_180_80420e7c;
            f32 c;
            dx = rad2 * (f32)sin((f64)r);
            c = (f32)cos((f64)r);
            dz = rad2 * -c;
            break;
        }
        case 4: {
            f32 r = piAngle / float_180_80420e7c;
            f32 s;
            f32 c;
            s = (f32)sin((f64)r);
            dx = rad2 * -s;
            c = (f32)cos((f64)r);
            dz = rad2 * -c;
            break;
        }
        case 5: {
            f32 r = piAngle / float_180_80420e7c;
            f32 c;
            dx = rad3 * (f32)sin((f64)r);
            c = (f32)cos((f64)r);
            dz = rad3 * -c;
            break;
        }
        case 6: {
            f32 r = piAngle / float_180_80420e7c;
            f32 s;
            f32 c;
            s = (f32)sin((f64)r);
            dx = rad3 * -s;
            c = (f32)cos((f64)r);
            dz = rad3 * -c;
            break;
        }
        }
        x = px + dx;
        z = pz + dz;
        distance = dist;
        hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, 0.0f,
                             float_neg1_80420e84, 0.0f);
        if (hit != NULL) {
            Vec n;
            MarioWork* m2;
            f32 ny100;
            n.x = hitNX;
            n.y = hitNY;
            n.z = hitNZ;
            m2 = marioGetPtr();
            ny100 = float_100_80420e98 * n.y;
            MW_F32(m2, 0x218) =
                (f32)(s32)(angleABf(0.0f, 0.0f, float_100_80420e98 * n.z, ny100) - float_180_80420e7c);
            MW_F32(m2, 0x21c) = 0.0f;
            MW_F32(m2, 0x220) =
                (f32)(s32)(angleABf(0.0f, 0.0f, float_100_80420e98 * n.x, ny100) - float_180_80420e7c);
            if (kpaNrmToAngle(&n) >= float_50_80420eb0) {
                hit = NULL;
            }
        }
        hitY = (f32)(s32)(float_100_80420e98 * hitY + float_0p5_80420e70) / float_100_80420e98;
        if (hit != NULL) {
            Vec n;
            MarioWork* m2;
            f32 ny100;
            n.x = hitNX;
            n.y = hitNY;
            n.z = hitNZ;
            m2 = marioGetPtr();
            ny100 = float_100_80420e98 * n.y;
            MW_F32(m2, 0x218) =
                (f32)(s32)(angleABf(0.0f, 0.0f, float_100_80420e98 * n.z, ny100) - float_180_80420e7c);
            MW_F32(m2, 0x21c) = 0.0f;
            MW_F32(m2, 0x220) =
                (f32)(s32)(angleABf(0.0f, 0.0f, float_100_80420e98 * n.x, ny100) - float_180_80420e7c);
            MW_F32(mp, 0x214) = kpaNrmToAngle(&n);
            if (hitY < py && __fabsf(hitY - py) >= float_2_80420e88) {
                *b = py - hitY;
                *c = angleABf(px, pz, hitX, hitZ);
                hit = NULL;
            } else {
                if (hitNX == zero && hitNZ == zero) {
                    hitY = (f32)(s32)(float_100_80420e98 * hitY + float_0p5_80420e70) / float_100_80420e98;
                }
                *a = hitY;
                *b = 0.0f;
                *c = angleABf(px, pz, hitX, hitZ);
                result = hit;
            }
        }
        KPA(marioGetPtr())->ride[i] = hit;
        KPA(marioGetPtr())->rideDist[i] = dx;
    }
    return result;
}


// Only 13 instructions differ, all register naming or block layout: the clear loop's counter is r28
// where the original uses r27 (offset r27 vs r28), the `kpaGetLevel()==0` test emits bne+b where the
// original emits beq+b, and in the x/y/z block the sz/entry[1] temporaries land in f3/f4 vs f5/f3.
// Computing the head offsets first and only adding the position after the entry[2]/flags guard (with
// `entry += 3` as a body statement) is what took this from 46 to 13 differing instructions.
// NONMATCHING (95.67%): no instruction selection, constant or stack slot differs.
void* kpaSearchHead(Vec* pos, f32* outY, f32 a) {
    // per-head offset tables (forward, height, facing condition)
    static s32 _ofs0[] = { 3, 12, 16, 1, 0, 16, 0, -9, 16, 0 };
    static s32 _ofs1[] = { 4, 25, 74, 0, 50, 65, 1, 0, 67, 0, -25, 62, 0 };
    static s32 _ofs2[] = { 6, 50, 102, 0, 76, 90, 1, 25, 100, 0, 0, 94, 0, -25, 90, 0, -45, 68, 0 };
    static s32 _ofs3[] = { 13, 170, 194, 1, 145, 204, 0, 120, 232, 0, 95, 227, 0, 70, 232, 0, 55, 232,
                           0,  30,  194, 0, 5,   187, 0, -20, 210, 0, -45, 201, 0, -70, 181, 0, -95, 153,
                           0,  -120, 56, 0 };
    static s32* headchkofs[] = { _ofs0, _ofs1, _ofs2, _ofs3 };
    f32 angle;
    f32 dist;
    s32 i;
    MarioWork* mp;
    s32 count;
    s32* entry;
    void* result;
    s32 ofs;

    mp = marioGetPtr();
    if (mp->flags & 0x20000) {
        return NULL;
    }
    result = NULL;
    if (mp->baseSpeed != float_0_80420e80) {
        angle = mp->directionView;
    } else {
        angle = toMovedir(mp->unk1A8);
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
    for (i = 0; i < count; i++, ofs += 4) {
        f32 sx = (f32)entry[0] * (f32)sin(angle);
        f32 c = (f32)cos(angle);
        f32 sz = (f32)entry[0] * -c;
        f32 yo = (f32)entry[1];
        f32 hitX, hitY, hitZ, hitNX, hitNY, hitNZ, distance;
        f32 x, y, z;
        s32 t = entry[2];
        void* hit;

        entry += 3;
        if (t == 0 && !(mp->flags & 0x10000)) continue;
        x = mp->position.x + sx;
        y = mp->position.y + yo;
        z = mp->position.z + sz;
        distance = dist;
        hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, x, y, z, 0.0f, 1.0f,
                             0.0f);
        if (hit == NULL) continue;
        hitY = (f32)(s32)(100.0f * hitY + 0.5f) / 100.0f;
        if (y + dist >= hitY) {
            pos->y = hitY;
            result = hit;
            *outY = 0.0f;
            KPA(marioGetPtr())->head[i] = hit;
        } else {
            *outY = hitY - pos->y - dist;
        }
    }
    return result;
}

// The original is a stub that returns NULL (`li r3, 0; blr`); its callers ignore the result.
void* kpaSearchFrontUnder(void) {
    return NULL;
}

// Written as a scaled byte offset (the field reached through offsetof) because the plain `kpa->head[index]`
// makes MWCC fold 0x20 into the load displacement; the original computes the offset first (`addi r0,r4,0x20`).
void* kpaGetHitobjHead(s32 index) {
    KpaWork* kpa = KPA(marioGetPtr());
    return *(void**)((u8*)kpa + index * 4 + offsetof(KpaWork, head));
}

void* kpaGetHitobjHead2(void) {
    return KPA(marioGetPtr())->head[0];
}

// Written as a scaled byte offset (the field reached through offsetof) because the plain `kpa->ride[index]`
// makes MWCC fold 0x4C into the load displacement; the original computes the offset first (`addi r0,r4,0x4c`).
void* kpaGetHitobjRide(s32 index) {
    KpaWork* kpa = KPA(marioGetPtr());
    return *(void**)((u8*)kpa + index * 4 + offsetof(KpaWork, ride));
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

void kpaClearHitobjRide(void) {
    MarioWork* mp = marioGetPtr();
    unsigned i;
    for (i = 0; i < 10; i++) {
        KPA(mp)->ride[i] = NULL;
    }
}

void* kpaGetHitobj(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return NULL;
    }
    return KPA(mp)->unk1C;
}


void* kpaGetHitFlame(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return NULL;
    }
    return KPA(mp)->unk18;
}


// Register allocation only: the clear loop's `i`/`ofs` zero-inits are emitted as `mr r28,r26` + `li r27,0`
// instead of the original's `mr r27,r26` + `mr r28,r26`, and the pi constant is in f30 vs the original's f31
// (double_to_int stays in f31 vs f30). `result` is correctly in r25. Declaring `result` last (after
// entry/i/count/mp) is what fixes the colouring; the `pi`/`deg` locals are deliberate and removing them
// costs 3 more instructions.
// NONMATCHING (97.11%): every other instruction, constant and stack slot matches.
void* kpaHitCheck(void) {
    s32* entry;
    s32 i;
    s32 count;
    MarioWork* mp;
    void* result = NULL;

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
    entry = head_sphere_data[kpaGetLevel()];
    count = entry[0];
    entry = entry + 1;
    {
    f32 pi = 3.1415927f;
    f32 deg = 180.0f;
    for (i = 0; i < count; i++) {
        f32 x = (f32)entry[0] * (f32)sin(pi * mp->directionView / deg) + mp->position.x;
        f32 z = mp->position.z - (f32)entry[0] * (f32)cos(pi * mp->directionView / deg);
        f32 y = mp->position.y + (f32)entry[1];
        f32 c = (f32)entry[2];
        s32 type = entry[3];
        void* hit;

        entry += 4;
        switch (type) {
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
    }
    KPA(mp)->unk1C = result;
    return result;
}

BOOL kpaFlameHitFilter(HitCheckQuery* query, HitEntry* entry) {
    MapObjectEntry* obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, "MOBJ_HiddenHatenaBlock") == 0) {
        return FALSE;
    }
    return strcmp(obj->animName, "MOBJ_Block") != 0;
}

void* kpaFlameHitCheck(f32 x, f32 y, f32 z, f32 rad) {
    marioGetPtr();
    return hitCheckSphereFilter(kpaFlameHitFilter, x, y, z, rad);
}
