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
extern const f32 float_0p5_80420e70;
extern const f32 float_1000_80420e74;
extern const f32 float_3p1416_80420e78;
extern const f32 float_180_80420e7c;
extern const f32 float_0_80420e80;
extern const f32 float_neg1_80420e84;
extern const f32 float_2_80420e88;
extern const f32 float_35_80420e8c;
extern const f32 float_10_80420e90;
extern const f32 float_1_80420e94;
extern const f32 float_100_80420e98;
extern const f32 float_0p01_80420e9c;
extern const f32 float_0p25_80420ea0;
extern const f32 float_0p6_80420ea4;
extern const f32 float_3_80420ea8;
extern const f32 float_0p05_80420eac;
extern const f32 float_50_80420eb0;
extern const f32 float_deg2rad_80420eb4;
extern const f32 float_90_80420eb8;
extern const f32 float_270_80420ebc;
extern const f32 float_7_80420ec0;

#define MW_F32(m, off) (*(f32*)((u8*)(m) + (off)))
#define MW_U32(m, off) (*(u32*)((u8*)(m) + (off)))
#define MW_S16(m, off) (*(s16*)((u8*)(m) + (off)))
#define MW_U16(m, off) (*(u16*)((u8*)(m) + (off)))

const Vec vec3_802fe320 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe32c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe338 = { 0.0f, 0.0f, 0.0f };
const char str_802FE344[] = "MOBJ_HiddenHatenaBlock";

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
    v0.y = float_0p5_80420e70 * a + y;
    v0.z = z;
    pos = v0;
    if (efp == NULL) {
        return 0;
    }
    root = efp->userdata;
    radius = root->radius;
    PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
    PSMTXRotRad(m1, 0x79, (root->yaw - camGetPtr(4)->viewYaw) * float_deg2rad_80420eb4);
    if (root->yaw > float_90_80420eb8 && root->yaw < float_270_80420ebc) {
        PSMTXScale(m2, root->radius, root->radius, -root->radius);
    } else {
        PSMTXScale(m2, root->radius, root->radius, root->radius);
    }
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m3);
    root++;
    i = 1;
    threshold = float_0p5_80420e70 * b;
    for (; i < efp->count; i++, root++) {
        f32 mag;
        if (root->noHit != 0) {
            continue;
        }
        scale = float_7_80420ec0 * root->radius;
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
    halfH = float_0p5_80420e70 * (mp->wPlayerCollisionBox.y + a);
    halfW = float_0p5_80420e70 * (mp->wPlayerCollisionBox.x + b);
    dx = __fabsf(mp->position.x - x);
    dy = __fabsf(mp->position.y + float_0p5_80420e70 * mp->wPlayerCollisionBox.y - y - float_0p5_80420e70 * a);
    if (dy <= halfH && dx <= halfW) {
        if (mp->flags & 0x20000) {
            if (mp->position.y > y) {
                if (kpaGetLevel() == 0 && kpaGetStageType() != 1 && mp->unk4C == 0 && z < float_50_80420eb0) {
                    marioChgMot(MARIO_MOTION_JUMP_NPC);
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
    if (x > float_0_80420e80) {
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

    return angleABf(float_0_80420e80, float_0_80420e80, len, -v->y);
}

// Writing the three components through a Vec* (rather than `n.x = x` etc.) makes MWCC emit the component
// reloads in ascending stack-address order and colour the destination FPRs the way the target does; the
// direct field assignments emit them in the opposite order. This function is byte-for-byte identical.
static inline void kpaSetVec(Vec* n, f32 x, f32 y, f32 z) {
    n->x = x;
    n->y = y;
    n->z = z;
}

// Common-subexpression elimination merges the loop's `base - off` terms and colours the FPRs differently from
// the target (f14/f15 for the long-lived locals), so it is disabled for this function only.
#pragma opt_common_subs off
void* kpaSearchGround(f32* a, f32* b, f32* c, f32 d) {
    MarioWork* mp;
    s32 i;
    s32 count;
    void* hit;
    f32 piAngle;
    f32 off;
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
    mp->slopeTiltX = float_0_80420e80;
    mp->slopeTiltY = float_0_80420e80;
    mp->slopeTiltZ = float_0_80420e80;
    mp->slopeAngle = float_0_80420e80;
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
    off = float_0p05_80420eac;
    rad2 = float_2_80420e88 * base - float_0p05_80420eac;
    rad3 = float_3_80420ea8 * base - off;
    for (i = 0; i < count; i++) {
        switch (i) {
        case 0:
            dx = float_0_80420e80;
            dz = float_0_80420e80;
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
        hit = hitCheckFilter(NULL, x, y, z, float_0_80420e80, float_neg1_80420e84, float_0_80420e80, &hitX, &hitY, &hitZ, &distance, &hitNX,
                             &hitNY, &hitNZ);
        if (hit != NULL) {
            Vec n;
            MarioWork* m2;
            f32 ny100;
            kpaSetVec(&n, hitNX, hitNY, hitNZ);
            m2 = marioGetPtr();
            ny100 = float_100_80420e98 * n.y;
            m2->slopeTiltX =
                (f32)(s32)(angleABf(float_0_80420e80, float_0_80420e80, float_100_80420e98 * n.z, ny100) - float_180_80420e7c);
            m2->slopeTiltY = float_0_80420e80;
            m2->slopeTiltZ =
                (f32)(s32)(angleABf(float_0_80420e80, float_0_80420e80, float_100_80420e98 * n.x, ny100) - float_180_80420e7c);
            if (kpaNrmToAngle(&n) >= float_50_80420eb0) {
                hit = NULL;
            }
        }
        hitY = (f32)(s32)(float_100_80420e98 * hitY + float_0p5_80420e70) / float_100_80420e98;
        if (hit != NULL) {
            Vec n;
            MarioWork* m2;
            f32 ny100;
            kpaSetVec(&n, hitNX, hitNY, hitNZ);
            m2 = marioGetPtr();
            ny100 = float_100_80420e98 * n.y;
            m2->slopeTiltX =
                (f32)(s32)(angleABf(float_0_80420e80, float_0_80420e80, float_100_80420e98 * n.z, ny100) - float_180_80420e7c);
            m2->slopeTiltY = float_0_80420e80;
            m2->slopeTiltZ =
                (f32)(s32)(angleABf(float_0_80420e80, float_0_80420e80, float_100_80420e98 * n.x, ny100) - float_180_80420e7c);
            mp->slopeAngle = kpaNrmToAngle(&n);
            if (hitY < py && __fabsf(hitY - py) >= float_2_80420e88) {
                *b = py - hitY;
                *c = angleABf(px, pz, hitX, hitZ);
                hit = NULL;
            } else {
                if (hitNX == float_0_80420e80 && hitNZ == float_0_80420e80) {
                    hitY = (f32)(s32)(float_100_80420e98 * hitY + float_0p5_80420e70) / float_100_80420e98;
                }
                *a = hitY;
                *b = float_0_80420e80;
                *c = angleABf(px, pz, hitX, hitZ);
                result = hit;
            }
        }
        KPA(marioGetPtr())->ride[i] = hit;
        KPA(marioGetPtr())->rideDist[i] = dx;
    }
    return result;
}


#pragma opt_common_subs reset
// The original clears the head/headDist arrays from an inlined loop: its counter lands in r27 and
// the byte-offset induction variable in r28, which only comes out when the loop is expanded as an
// inline helper's body (its locals rank above the function's own names in the allocator).
static inline void kpaClearSearchHeads(void) {
    s32 n;
    for (n = 0; n < 10; n++) {
        KPA(marioGetPtr())->head[n] = NULL;
        KPA(marioGetPtr())->headDist[n] = float_1000_80420e74;
    }
}

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
    *outY = float_neg1_80420e84;
    kpaClearSearchHeads();
    entry = headchkofs[kpaGetLevel()];
    count = entry[0];
    entry = entry + 1;
    if (!(mp->flags & 0x10000)) {
        switch (kpaGetLevel()) {
        case 0:
            dist = float_2_80420e88;
            break;
        case 1:
            dist = float_0_80420e80;
            break;
        case 2:
            dist = float_0_80420e80;
            break;
        case 3:
            dist = float_35_80420e8c;
            break;
        }
    } else {
        switch (kpaGetLevel()) {
        case 0:
            if (mp->currMotionId == MARIO_MOTION_KPA_SWIM) {
                dist = float_10_80420e90;
            } else {
                dist = a;
            }
            break;
        default:
            dist = a;
            break;
        }
    }
    angle = float_3p1416_80420e78 * angle / float_180_80420e7c;
    ofs = 0;
    for (i = 0; i < count; i++, ofs += 4) {
        f32 yo;
        f32 sx = (f32)entry[0] * (f32)sin(angle);
        f32 c = (f32)cos(angle);
        f32 sz = (f32)entry[0] * -c;
        f32 hitX, hitY, hitZ, hitNX, hitNY, hitNZ, distance;
        f32 x, y, z;
        s32 t = entry[2];
        void* hit;

        yo = (f32)entry[1];
        entry += 3;
        if (t == 0 && !(mp->flags & 0x10000)) continue;
        x = mp->position.x + sx;
        y = mp->position.y + yo;
        z = mp->position.z + sz;
        distance = dist;
        hit = hitCheckFilter(NULL, x, y, z, float_0_80420e80, float_1_80420e94, float_0_80420e80, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY,
                             &hitNZ);
        if (hit == NULL) continue;
        hitY = (f32)(s32)(float_100_80420e98 * hitY + float_0p5_80420e70) / float_100_80420e98;
        if (y + dist >= hitY) {
            pos->y = hitY;
            result = hit;
            *outY = float_0_80420e80;
            KPA(marioGetPtr())->head[i] = hit;
        } else {
            *outY = hitY - pos->y - dist;
        }
    }
    return result;
}

// head_sphere_data: sphere offsets (forward, height, radius) and a facing-condition type per level.
s32 head_sphere_data_lv0[] = { 1, 10, 15, 6, 0 };
s32 head_sphere_data_lv1[] = { 3, 32, 55, 24, 0, 26, 30, 16, 2, -10, 44, 25, 1 };
s32 head_sphere_data_lv2[] = { 3, 45, 75, 34, 0, 36, 30, 16, 2, -15, 59, 36, 1 };
s32 head_sphere_data_lv3[] = { 4, 100, 162, 80, 0, 90, 75, 30, 2, 50, 40, 30, 3, -43, 142, 74, 1 };
s32* head_sphere_data[] = { head_sphere_data_lv0, head_sphere_data_lv1, head_sphere_data_lv2,
                            head_sphere_data_lv3 };

#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") static u32 gap_07_80415B84_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

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
    return KPA(mp)->hitobj;
}


void* kpaGetHitFlame(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return NULL;
    }
    return KPA(mp)->hitFlame;
}


// The clear loop must live in its own inlined helper: written directly in kpaHitCheck its counter/offset
// zero-inits come out as `mr r28,r26` + `li r27,0` instead of the original's `mr r27,r26` + `mr r28,r26`.
static inline void kpaClearHeadAll(void) {
    s32 j;
    for (j = 0; j < 10; j++) {
        KPA(marioGetPtr())->head[j] = NULL;
        KPA(marioGetPtr())->headDist[j] = float_1000_80420e74;
    }
}

// Holding each sin/cos result in a named local and dividing by the `float_180_80420e7c` literal directly
// (no `deg` local) gives the original's FPR colouring: `pi` in f31 and the int->float magic double in f30.
// The clear loop stays in the kpaClearHeadAll helper above for its counter/offset zero-init.
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
    KPA(mp)->hitobj = NULL;
    kpaClearHeadAll();
    if (kpaFireAttackCheck() == 1) {
        return NULL;
    }
    entry = head_sphere_data[kpaGetLevel()];
    count = entry[0];
    entry = entry + 1;
    {
    f32 pi = float_3p1416_80420e78;
    for (i = 0; i < count; i++) {
        f32 sn = (f32)sin(pi * mp->directionView / float_180_80420e7c);
        f32 x = (f32)entry[0] * sn + mp->position.x;
        f32 cs = (f32)cos(pi * mp->directionView / float_180_80420e7c);
        f32 z = mp->position.z - (f32)entry[0] * cs;
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
    KPA(mp)->hitobj = result;
    return result;
}

BOOL kpaFlameHitFilter(HitCheckQuery* query, HitEntry* entry) {
    MapObjectEntry* obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, str_802FE344) == 0) {
        return FALSE;
    }
    return strcmp(obj->animName, "MOBJ_Block") != 0;
}

void* kpaFlameHitCheck(f32 x, f32 y, f32 z, f32 rad) {
    marioGetPtr();
    return hitCheckSphereFilter(kpaFlameHitFilter, x, y, z, rad);
}

// .sdata2 constants, in the original address order. Defined after the code so the compiler emits
// them before it pools anything, matching the original object layout.
const f32 float_0p5_80420e70 = 0.5f;
const f32 float_1000_80420e74 = 1000.f;
const f32 float_3p1416_80420e78 = 3.1415927f;
const f32 float_180_80420e7c = 180.f;
const f32 float_0_80420e80 = 0.0f;
const f32 float_neg1_80420e84 = -1.0f;
const f32 float_2_80420e88 = 2.0f;
const f32 float_35_80420e8c = 35.0f;
const f32 float_10_80420e90 = 10.0f;
const f32 float_1_80420e94 = 1.0f;
const f32 float_100_80420e98 = 100.f;
const f32 float_0p01_80420e9c = 0.01f;
const f32 float_0p25_80420ea0 = 0.25f;
const f32 float_0p6_80420ea4 = 0.6f;
const f32 float_3_80420ea8 = 3.0f;
const f32 float_0p05_80420eac = 0.05f;
const f32 float_50_80420eb0 = 50.f;
const f32 float_deg2rad_80420eb4 = 0.017453292f;
const f32 float_90_80420eb8 = 90.f;
const f32 float_270_80420ebc = 270.f;
const f32 float_7_80420ec0 = 7.0f;

// 4-byte zero word that ends the unit .sdata2; nothing reads it, but naming it keeps the section
// length identical to the original.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") const u32 gap_09_80420EC4_sdata2 = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
