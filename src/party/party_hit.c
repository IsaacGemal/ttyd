#include "party.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "mario/mario.h"
#include "mario/mario_hit.h"
#include "mario/mario_sbr.h"
#include "math_ext.h"
#include "system.h"
#include "party/party_hit.h"
#include "party/party_yoshi.h"
#include "party/party_move.h"
#include <string.h>

// The original pools the sqrtf double constants into the rodata blob at vec3_802cd320.
#pragma pool_data on

// LOCAL_PROTO for out-of-line MSL math used below.
#define fabsf(x) __fabsf(x)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

extern s32 __float_nan[];
#define NAN (*(f32*)__float_nan)

// rodata blob; the pooled sqrtf doubles and block-name strings follow these.
const Vec vec3_802cd320 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd32c = {0.0f, -1.0f, 0.0f};
const Vec vec3_802cd338 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd344 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd350 = {0.0f, -1.0f, 0.0f};
const Vec vec3_802cd35c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd368 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd374 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd380 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd38c = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd398 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3a4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3b0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3bc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3c8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cd3d4 = {0.0f, 0.0f, 0.0f};

// .rodata int->float magic constant used by partyHitCheck's rounding.
const f64 double_to_int_802cd3e0 = 4503601774854144.0;
const f64 double_0p5_802cd3e8 = 0.5;
const f64 double_3_802cd3f0 = 3.0;
const f64 double_0_802cd3f8 = 0.0;
const char str_802CD400[0x18] = "MOBJ_HiddenHatenaBlock";
const char str_802CD418[0x18] = "MOBJ_HiddenBadgeBlock";
const char str_802CD430[0x18] = "MOBJ_Hidden10CountBlock";

// Float sqrt used by the original: inline expansion with NaN classification.
static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = double_0p5_802cd3e8 * guess * (double_3_802cd3f0 - guess * guess * x);
        guess = double_0p5_802cd3e8 * guess * (double_3_802cd3f0 - guess * guess * x);
        guess = double_0p5_802cd3e8 * guess * (double_3_802cd3f0 - guess * guess * x);
        return x * guess;
    } else if (x < double_0_802cd3f8) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

// .sdata2 constants
extern const f32 float_10_8041b3e8;
extern const f32 float_0_8041b3ec;
extern const f32 float_1_8041b3f0;
extern const f32 float_0p5_8041b3f4;
extern const f32 float_100_8041b3f8;
extern const f32 float_0p3_8041b3fc;
extern const f32 float_0p75_8041b400;
extern const f32 float_neg0p5_8041b404;
extern const f32 float_1000_8041b408;
extern const f32 float_11_8041b40c;
extern const f32 float_45_8041b410;
extern const f32 float_neg1_8041b414;
extern const f32 float_0p01_8041b418;
extern const f32 float_0p265_8041b41c;
extern const f32 float_3p1416_8041b420;
extern const f32 float_180_8041b424;
extern const f32 float_2_8041b428;
extern const f32 float_0p1_8041b42c;
extern const f32 float_5_8041b430;
extern const f32 float_0p375_8041b434;
extern const f32 float_neg10000_8041b438;
extern const f32 float_50_8041b43c;
extern const f32 float_30_8041b440;


// NONMATCHING (45.79%): the four corners share float temporaries in the original,
// so it needs fewer callee-saved registers and a larger frame.
HitEntry* partySearchHead(PartyEntry* party, Vec* position, f32* out, f32 angle) {
    HitEntry* hit;
    Vec pos1, nor1, pos2, nor2, pos3, nor3, pos4, nor4;
    f32 sin, cos;
    f32 h;
    f32 offSin, offCos;
    f32 px, py, pz;
    f32 x, z;
    f32 dist;
    f32 d1, d2, d3, d4;

    h = 0.3f * *(f32*)((u8*)party + 0xF4);
    sincosf(&sin, &cos, angle);
    offSin = h * sin;
    offCos = -h * cos;
    px = position->x;
    py = position->y;
    pz = position->z;
    dist = *out;

    x = px + offSin;
    z = pz + offCos;
    d1 = dist;
    hit = hitCheckFilter(NULL, &pos1.x, &pos1.y, &pos1.z, &d1, &nor1.x, &nor1.y, &nor1.z, x, py + 10.0f, z,
                         0.0f, 1.0f, 0.0f);
    if (hit != NULL) {
        if (dist >= d1) {
            dist = d1;
        }
    }
    if (hit == NULL) {
        x = px - offSin;
        z = pz - offCos;
        d2 = dist;
        hit = hitCheckFilter(NULL, &pos2.x, &pos2.y, &pos2.z, &d2, &nor2.x, &nor2.y, &nor2.z, x, py + 10.0f, z,
                             0.0f, 1.0f, 0.0f);
        if (hit != NULL) {
            if (dist >= d2) {
                dist = d2;
            }
        }
    }
    if (hit == NULL) {
        x = px + offCos;
        z = pz + offSin;
        d3 = dist;
        hit = hitCheckFilter(NULL, &pos3.x, &pos3.y, &pos3.z, &d3, &nor3.x, &nor3.y, &nor3.z, x, py + 10.0f, z,
                             0.0f, 1.0f, 0.0f);
        if (hit != NULL) {
            if (dist >= d3) {
                dist = d3;
            }
        }
    }
    if (hit == NULL) {
        x = px - offCos;
        z = pz - offSin;
        d4 = dist;
        hit = hitCheckFilter(NULL, &pos4.x, &pos4.y, &pos4.z, &d4, &nor4.x, &nor4.y, &nor4.z, x, py + 10.0f, z,
                             0.0f, 1.0f, 0.0f);
        if (hit != NULL) {
            if (dist >= d4) {
                dist = d4;
            }
        }
    }
    if (hit != NULL) {
        position->x = x;
        position->y = py;
        position->z = z;
        *out = dist;
    }
    if (hit == NULL) {
        position->x = x;
        position->y = py;
        position->z = z;
        *out = 0.0f;
    }
    return hit;
}

// NONMATCHING (68.78%): frame is 0x30 smaller; query/Vec locals differ.
HitEntry* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 dist, f32 dir) {
    HitCheckQuery query1;
    HitCheckQuery query2;
    HitEntry* hit;
    HitEntry* hit2;
    f32 sin, cos;
    f32 f31;
    f32 h;
    f32 nx, nz;
    f32 dot;

    sincosf(&sin, &cos, dir);
    f31 = 0.5f * *(f32*)((u8*)party + 0xF4) + dist;

    h = 0.75f * partyGetHeight(party);
    query1.targetPos.x = party->position.x;
    query1.targetPos.y = party->position.y + h;
    query1.targetPos.z = party->position.z;
    query1.targetDir.x = sin;
    query1.targetDir.y = 0.0f;
    query1.targetDir.z = cos;
    query1.targetDistance = f31;

    if (yoshiGetStatus() != 0) {
        hit = hitCheckVecFilter(&query1, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit = hitCheckVecFilter(&query1, chkfilterNokotaro);
    } else {
        hit = hitCheckVecFilter(&query1, chkfilter);
    }

    if (hit != NULL) {
        f31 = (f32)(s32)(1000.0f * query1.targetDistance +
                         (query1.targetDistance >= 0.0f ? 0.5f : -0.5f)) /
              1000.0f;
    }
    if (hit != NULL) {
        *(f32*)((u8*)party + 0xEC) = f31;
        nx = query1.hitNormal.x;
        nz = query1.hitNormal.z;
        dot = sin * dist * nx + cos * dist * nz;
        pos->x += 0.5f * (sin * dist - dot * nx);
        pos->z += 0.5f * (cos * dist - dot * nz);
        return hit;
    }

    h = 11.0f;
    if (party->currentMemberId == PARTY_MEMBER_YOSHI && (party->flags & 0x100)) {
        h = 1.0f;
    }
    f31 = 0.5f * *(f32*)((u8*)party + 0xF4) + dist;
    query2.targetPos.x = party->position.x;
    query2.targetPos.y = party->position.y + h;
    query2.targetPos.z = party->position.z;
    query2.targetDir.x = sin;
    query2.targetDir.y = 0.0f;
    query2.targetDir.z = cos;
    query2.targetDistance = f31;

    if (yoshiGetStatus() != 0) {
        hit2 = hitCheckVecFilter(&query2, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit2 = hitCheckVecFilter(&query2, chkfilterNokotaro);
    } else {
        hit2 = hitCheckVecFilter(&query2, chkfilter);
    }

    if (hit2 == NULL) {
        hit2 = NULL;
    } else {
        f31 = (f32)(s32)(1000.0f * query2.targetDistance +
                         (query2.targetDistance >= 0.0f ? 0.5f : -0.5f)) /
              1000.0f;
    }
    if (hit2 != NULL) {
        if (partyNrmToAngle(&query2.hitNormal) >= 45.0f) {
            *(f32*)((u8*)party + 0xEC) = f31;
            nx = query2.hitNormal.x;
            nz = query2.hitNormal.z;
            dot = sin * dist * nx + cos * dist * nz;
            pos->x += 0.5f * (sin * dist - dot * nx);
            pos->z += 0.5f * (cos * dist - dot * nz);
            return hit2;
        }
    }
    *(f32*)((u8*)party + 0xEC) = -1.0f;
    pos->x += dist * sin;
    pos->z += dist * cos;
    return NULL;
}

// NONMATCHING (54.64%): loop/register scheduling of the three marioHitCheckVec
// passes differs from the original.
HitEntry* partySearchWallFront(PartyEntry* party, Vec* pos, f32 a, f32 b) {
    Vec base;
    Vec dir;
    Vec hpos;
    Vec hnorm;
    Vec bestNorm;
    Vec delta;
    f32 sin, cos;
    f32 f31;
    f32 dist;
    f32 curDist;
    f32 sin2, cos2;
    f32 sin3, cos3;
    HitEntry* best;
    HitEntry* hit;
    s32 i;

    sincosf(&sin, &cos, b);
    base.x = 0.75f * partyGetHeight(party);
    base.y = 0.5f * partyGetHeight(party);
    if ((party->flags & 0x30) != 0 || yoshiGetStatus() == 2) {
        base.z = 1.0f;
    } else {
        base.z = 11.0f;
    }
    f31 = 0.5f * *(f32*)((u8*)party + 0xF4);
    dist = f31 + a;
    dir.x = sin;
    dir.y = 0.0f;
    dir.z = cos;
    delta.x = 0.0f;
    delta.y = 0.0f;
    delta.z = 0.0f;

    best = NULL;
    hit = NULL;
    for (i = 0; i < 3; i++) {
        Vec p = *pos;
        p.y += ((f32*)&base)[i];
        curDist = dist;
        hit = marioHitCheckVec(&p, &dir, &hpos, &hnorm, &curDist);
        if (hit != NULL) {
            if (best != NULL) {
                if (curDist < dist) {
                    best = hit;
                    dist = curDist;
                    bestNorm = hnorm;
                }
            } else {
                best = hit;
                dist = curDist;
                bestNorm = hnorm;
            }
        }
    }
    if (best != NULL) {
        f32 nx = bestNorm.x;
        f32 nz = bestNorm.z;
        f32 dz = cos * a;
        f32 dx = sin * a;
        f32 dot = dx * nx + dz * nz;
        delta.x += 0.5f * (dx - dot * nx);
        delta.z += 0.5f * (dz - dot * nz);
    }

    sincosf(&sin2, &cos2, 45.0f + b);
    dir.x = sin2;
    dir.y = 0.0f;
    dir.z = cos2;
    for (i = 0; i < 3; i++) {
        Vec p = *pos;
        p.y += ((f32*)&base)[i];
        curDist = f31;
        hit = marioHitCheckVec(&p, &dir, NULL, NULL, &curDist);
        if (hit != NULL) {
            break;
        }
    }
    if (hit != NULL) {
        sincosf(&sin3, &cos3, b - 45.0f);
        dir.x = sin3;
        dir.y = 0.0f;
        dir.z = cos3;
        for (i = 0; i < 3; i++) {
            Vec p = *pos;
            p.y += ((f32*)&base)[i];
            curDist = f31;
            hit = marioHitCheckVec(&p, &dir, NULL, NULL, &curDist);
        }
    }
    if (best != NULL) {
        if (hit == NULL) {
            pos->x += delta.x;
            pos->z += delta.z;
        }
        return best;
    }
    if (hit == NULL) {
        pos->x += a * sin;
        pos->z += a * cos;
    }
    return NULL;
}

// NONMATCHING (49.80%): builds the downward/forward query and runs the step/slope
// angle math; register and stack scheduling of the last block differ.
u32 partyChkFrontStep(PartyEntry* party, f32 a, f32* out1, f32* out2, f32* out3) {
    HitCheckQuery query;
    HitEntry* hit;
    f32 h;
    f32 f24;
    f32 dist;
    f32 px, py, pz;
    f32 angle;
    f32 s, c;
    f32 off;
    f32 rad;
    f32 hx, hz;
    f32 slope;

    *(f32*)((u8*)party + 0xC4) = 0.0f;
    *(f32*)((u8*)party + 0xC8) = 0.0f;
    *(f32*)((u8*)party + 0xCC) = 0.0f;
    *(f32*)((u8*)party + 0xE0) = 0.0f;
    h = partyGetHeight(party);
    f24 = a + float_0p01_8041b418;
    *out1 = party->position.y;
    *out2 = float_neg1_8041b414;
    if (party->speed != float_0_8041b3ec) {
        angle = party->dir;
    } else {
        angle = toMovedir(party->field_10C);
    }
    rad = (float_3p1416_8041b420 * revise360((f32)(s32)(angle + float_0p5_8041b3f4))) / float_180_8041b424;
    off = float_0p265_8041b41c * *(f32*)((u8*)party + 0xF4);
    s = (f32)sin((f64)rad);
    c = (f32)cos((f64)rad);
    px = party->position.x;
    py = party->position.y;
    pz = party->position.z;
    query.targetPos.x = px + off * s;
    query.targetPos.y = py + h;
    query.targetPos.z = pz - off * c;
    query.targetDir.x = 0.0f;
    query.targetDir.y = float_neg1_8041b414;
    query.targetDir.z = 0.0f;
    query.targetDistance = h + f24;

    if (yoshiGetStatus() != 0) {
        hit = hitCheckVecFilter(&query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit = hitCheckVecFilter(&query, chkfilterNokotaro);
    } else {
        hit = hitCheckVecFilter(&query, chkfilter);
    }
    if (hit == NULL) {
        return 0;
    }
    dist = (f32)(s32)(float_1000_8041b408 * query.targetDistance +
                      (query.targetDistance >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
           float_1000_8041b408;
    hx = (f32)(s32)(float_1000_8041b408 * query.hitPos.x +
                    (query.hitPos.x >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
         float_1000_8041b408;
    hz = (f32)(s32)(float_1000_8041b408 * query.hitPos.z +
                    (query.hitPos.z >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
         float_1000_8041b408;

    slope = angleABf(float_0_8041b3ec, float_0_8041b3ec, float_100_8041b3f8 * hx, float_100_8041b3f8 * h);
    *(f32*)((u8*)party + 0xC4) = slope - float_180_8041b424;
    *(f32*)((u8*)party + 0xC8) = float_0_8041b3ec;
    slope = angleABf(float_0_8041b3ec, float_0_8041b3ec, float_100_8041b3f8 * hx, float_100_8041b3f8 * hz);
    *(f32*)((u8*)party + 0xCC) = slope - float_180_8041b424;
    *(f32*)((u8*)party + 0xE0) = angleABf(float_0_8041b3ec, float_0_8041b3ec, float_100_8041b3f8 * h,
                                          float_100_8041b3f8 * hz);
    if (dist < h) {
        if (__fabsf(dist - h) <= float_2_8041b428) {
            *out2 = h - dist;
            *out3 = angleABf(s, f24, dist, hz);
            return (u32)hit;
        }
    }
    *out1 = dist;
    *out2 = float_0_8041b3ec;
    *out3 = angleABf(s, f24, dist, hz);
    return (u32)hit;
}

// NONMATCHING (35.44%): five downward rays fanned around the party; the post-loop
// ground-selection block is approximated.
HitEntry* partySearchGround(PartyEntry* party, f32 up, f32 down) {
    HitCheckQuery query;
    HitEntry* hit;
    HitEntry* best;
    f32 f22, f20, f24, f23, f27, f28, f26, f19, f21;
    f32 f31, f30;
    f32 dy;
    f32 angle;
    f32 s, c;
    s32 i;

    if ((party->flags & 0x02000000) != 0) {
        return NULL;
    }
    *(f32*)((u8*)party + 0xC4) = 0.0f;
    *(f32*)((u8*)party + 0xC8) = 0.0f;
    *(f32*)((u8*)party + 0xCC) = 0.0f;
    f22 = __fabsf(up) + float_0p1_8041b42c;
    *(f32*)((u8*)party + 0xE0) = float_0_8041b3ec;
    if (*(u16*)((u8*)marioGetPtr() + 0x2E) == 0x1A) {
        f20 = float_5_8041b430;
    } else {
        f20 = float_0p75_8041b400 * partyGetHeight(party);
    }
    party->field_E4 = party->position.y;
    *(f32*)((u8*)party + 0xE8) = float_neg1_8041b414;
    if (party->speed != float_0_8041b3ec) {
        angle = party->dir;
    } else {
        angle = partyToMovedir(party, party->field_10C);
    }
    angle = (float_3p1416_8041b420 * angle) / float_180_8041b424;
    s = (f32)sin((f64)angle);
    c = (f32)cos((f64)angle);
    f24 = float_0p375_8041b434 * (*(f32*)((u8*)party + 0xF4) * s);
    f23 = float_0p375_8041b434 * (*(f32*)((u8*)party + 0xF4) * -c);
    f27 = -f24;
    f28 = -f23;
    f21 = party->position.y;
    f19 = f21 + f20;
    f26 = f20 + f22;
    f22 = party->position.x;
    f20 = party->position.z;
    best = NULL;
    dy = float_neg10000_8041b438;
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            f31 = float_0_8041b3ec;
            f30 = float_0_8041b3ec;
        } else if (i == 1) {
            f31 = f24;
            f30 = f23;
        } else if (i == 2) {
            f31 = f28;
            f30 = f24;
        } else if (i == 3) {
            f31 = f23;
            f30 = f27;
        } else {
            f31 = f27;
            f30 = f28;
        }
        query.targetPos.x = f22 + f31;
        query.targetPos.y = f19;
        query.targetPos.z = f20 + f30;
        query.targetDir.x = 0.0f;
        query.targetDir.y = float_neg1_8041b414;
        query.targetDir.z = 0.0f;
        query.targetDistance = f26;

        if (yoshiGetStatus() != 0) {
            hit = hitCheckVecFilter(&query, chkfilterYoshi);
        } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
            hit = hitCheckVecFilter(&query, chkfilterNokotaro);
        } else {
            hit = hitCheckVecFilter(&query, chkfilter);
        }
        if (hit != NULL) {
            f32 d = (f32)(s32)(float_1000_8041b408 * query.targetDistance +
                               (query.targetDistance >= float_0_8041b3ec ? float_0p5_8041b3f4
                                                                         : float_neg0p5_8041b404)) /
                    float_1000_8041b408;
            f32 hy = (f32)(s32)(float_1000_8041b408 * query.hitPos.y +
                                (query.hitPos.y >= float_0_8041b3ec ? float_0p5_8041b3f4
                                                                    : float_neg0p5_8041b404)) /
                     float_1000_8041b408;
            if (d > dy) {
                best = hit;
                dy = d;
                party->field_E4 = hy;
            }
        }
    }
    (void)down;
    return best;
}

void partyChkGnd(PartyEntry* party) {
    s32 attr;
    HitEntry* hit;
    f32 a, b, c;

    if (party->flags & 0x02000000) {
        return;
    }
    attr = 0;
    hit = partySearchGround(party, float_11_8041b40c, party->jumpPara1);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
        }
    }
    party->field_138 = hit;
    party->flags = (party->flags & ~0x80);
    if (hit == NULL) {
        party->flags = (party->flags & ~0x2);
        partyChgMot(party, 3);
        party->jumpPara3 = float_neg1_8041b414;
    } else {
        party->flags = (party->flags | 0x2);
        if (attr & 0x100) {
            party->flags = (party->flags | 0x80);
        } else {
            party->flags = (party->flags & ~0x80);
        }
    }
    party->position.y = party->field_E4;
    if (party->flags & 0x30) {
        return;
    }
    partyChkFrontStep(party, float_11_8041b40c, &a, &b, &c);
    if (a - party->position.y >= float_5_8041b430 && *(f32*)((u8*)party + 0xE0) <= float_30_8041b440) {
        *(f32*)party->field_120 = a;
        if (party->speed != float_0_8041b3ec) {
            partyChgMot(party, 6);
        }
    }
}

// NONMATCHING (83.52%): the inlined sqrtf doubles are pooled in a separate
// .rodata subsection rather than following vec3_802cd320 at 0xc8/0xd0/0xd8.
f32 partyNrmToAngle(Vec* v) {
    f32 len = sqrtf(v->x * v->x + v->z * v->z);

    return angleABf(float_0_8041b3ec, float_0_8041b3ec, len, -v->y);
}

HitEntry* partyHitCheck(PartyEntry* party, Vec* pos, Vec* dir, Vec* hitPos, Vec* hitNormal, f32* dist) {
    Vec dirCopy = *dir;
    HitCheckQuery query;
    HitEntry* hit;

    query.targetPos = *pos;
    query.targetDir = dirCopy;
    query.targetDistance = *dist;

    if (yoshiGetStatus() != 0) {
        hit = hitCheckVecFilter(&query, chkfilterYoshi);
    } else if (party->currentMemberId == PARTY_MEMBER_KOOPS && (party->flags & 0x100)) {
        hit = hitCheckVecFilter(&query, chkfilterNokotaro);
    } else {
        hit = hitCheckVecFilter(&query, chkfilter);
    }

    if (hit == NULL) {
        return hit;
    }

    *dist = (f32)(s32)(float_1000_8041b408 * query.targetDistance +
                       (query.targetDistance >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
            float_1000_8041b408;
    hitPos->x = (f32)(s32)(float_1000_8041b408 * query.hitPos.x +
                           (query.hitPos.x >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
                float_1000_8041b408;
    hitPos->y = (f32)(s32)(float_1000_8041b408 * query.hitPos.y +
                           (query.hitPos.y >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
                float_1000_8041b408;
    hitPos->z = (f32)(s32)(float_1000_8041b408 * query.hitPos.z +
                           (query.hitPos.z >= float_0_8041b3ec ? float_0p5_8041b3f4 : float_neg0p5_8041b404)) /
                float_1000_8041b408;
    hitNormal->x = query.hitNormal.x;
    hitNormal->y = query.hitNormal.y;
    hitNormal->z = query.hitNormal.z;
    return hit;
}

BOOL chkfilterNokotaro(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;
    const u8* base = (const u8*)&vec3_802cd320;

    marioGetPtr();
    attr = entry->attributes;
    if ((attr & 0x800005) != 0) {
        return FALSE;
    }
    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, (char*)base + 0xE0) == 0 ||
        strcmp(obj->animName, (char*)base + 0xF8) == 0 ||
        strcmp(obj->animName, (char*)base + 0x110) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilterYoshi(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;
    const u8* base = (const u8*)&vec3_802cd320;

    marioGetPtr();
    attr = entry->attributes;
    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x800000) != 0) {
        return TRUE;
    }
    if ((attr & 0x4) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, (char*)base + 0xE0) == 0 ||
        strcmp(obj->animName, (char*)base + 0xF8) == 0 ||
        strcmp(obj->animName, (char*)base + 0x110) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}

BOOL chkfilter(HitCheckQuery* query, HitEntry* entry) {
    s32 attr;
    MapObjectEntry* obj;
    const u8* base = (const u8*)&vec3_802cd320;

    marioGetPtr();
    attr = entry->attributes;
    if ((attr & 0x80080) != 0) {
        return TRUE;
    }
    if ((attr & 0x800004) != 0) {
        return FALSE;
    }
    if ((attr & 0x80000000) == 0) {
        return TRUE;
    }
    obj = mobjHitObjPtrToPtr(entry);
    if (strcmp(obj->animName, (char*)base + 0xE0) == 0 ||
        strcmp(obj->animName, (char*)base + 0xF8) == 0 ||
        strcmp(obj->animName, (char*)base + 0x110) == 0) {
        mobjHitObjPtrToPtr(entry);
        return FALSE;
    }
    return TRUE;
}
